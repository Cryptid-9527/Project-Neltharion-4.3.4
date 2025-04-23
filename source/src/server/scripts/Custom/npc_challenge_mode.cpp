/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ChallengeModeMgr.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Spell.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Group.h"

#define GOSSIP_MENU_NO_KEYSTONE                     1010001
#define GOSSIP_MENU_HAS_KEYSTONE_OUTSIDE            1010002
#define GOSSIP_MENU_HAS_KEYSTONE_CORRECT_DUNGEON    1010003
#define GOSSIP_MENU_HAS_KEYSTONE_WRONG_DUNGEON      1010004
#define GOSSIP_MENU_CHALLENGE_STARTED               1010005
#define GOSSIP_MENU_NO_LFG                          1010006
#define GOSSIP_MENU_AFFIX_INFO                      1010007


#define GOSSIP_TEXT_GET_KEYSTONE "I will find a group and handle this. I will need a keystone to seek out the threat.\n"
#define GOSSIP_TEXT_MYTHIC_DETAILS "What information can you provide about the Mythic challenge?"

#define GOSSIP_TEXT_MYTHIC_DETAILS_BACK_BUTTON "Back"



struct npc_challenge_mode_handler : public ScriptedAI
{
    npc_challenge_mode_handler(Creature* creature) : ScriptedAI(creature) { }

    void sGossipHello(Player* player)
    {
        InstanceScript* script = player->GetInstanceScript();



        auto keystone_of_player{ player };
        bool talker_is_leader{ true };
        if (auto g = player->GetGroup())
            if (!g->IsLeader(player->GetGUID()))
                if (auto lead = Unit::GetUnit(*player, g->GetLeaderGUID()))
                    if (auto p_lead = lead->ToPlayer())
                    {
                        keystone_of_player = p_lead->ToPlayer();
                        talker_is_leader = false;
                    }

        if (player->isUsingLfg())
        {
            player->SEND_GOSSIP_MENU(GOSSIP_MENU_NO_LFG, me->GetGUID());
            return;
        }

        if (!instance)
        {
            player->GetSession()->SendNotification("|cffff0000Instance script not found!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }


        if (Group* g = player->GetGroup())
        {

            if (g->isRaidGroup())
            {
                player->GetSession()->SendNotification("|cffff0000Challenge mode cannot be started while group is in raid format!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }
        }
        else
        {
            player->GetSession()->SendNotification("|cffff0000You must be in a group to start a mythic dungeon!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        if (player->isGameMaster())
        {
            player->GetSession()->SendNotification("|cffff0000Gamemasters can not start a mythic dungeon!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        auto players = me->GetPlayersInRange(100.f, true);
        if (players.size() < 5)
        {
            player->GetSession()->SendNotification("|cffff0000You must have a full group nearby to start!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }
        
        if (!instance->MeetsChallengeRequirements())
        {
            player->GetSession()->SendNotification("|cffff0000A player in your group does not meet the item level requirement!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        if (instance->IsChallengeModeFinished())
        {
            player->GetSession()->SendNotification("|cffff0000Mythic challenge already finished!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }
        if (instance->IsChallengeModeStarted())
        {
            player->GetSession()->SendNotification("|cffff0000Mythic dungeon already in progress!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        if (instance->GetCompletedEncounterMask() != 0)
        {
            player->GetSession()->SendNotification("|cffff0000Normal dungeon is already in progress. Reset the instance to begin.|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        if (instance->GetMythicTamperState())
        {
            player->GetSession()->SendNotification("|cffff0000Can't start challenge with NPCs having entered combat!|r");
            player->CLOSE_GOSSIP_MENU();
            return;
        }

        Keystone const& keystone = keystone_of_player->GetKeystone();
        MapEntry const* mapEntry = sMapStore.LookupEntry(keystone.mapId);

        if (!script)
        {
            if (keystone.mapId != 0)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Dungeon: " + ChallengeModeMgr::GetKeystoneName(keystone_of_player, keystone_of_player->GetKeystone().mapId), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Close", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_MENU_HAS_KEYSTONE_OUTSIDE, me->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(GOSSIP_MENU_NO_KEYSTONE, me->GetGUID());

            return;
        }

        if (script->IsChallengeModeStarted())
        {
            player->SEND_GOSSIP_MENU(GOSSIP_MENU_CHALLENGE_STARTED, me->GetGUID());
            return;
        }

        
        if (keystone.mapId == 0)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ChallengeModeMgr::GetKeystoneNameCurrent(keystone_of_player, player->GetMapId()), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            if (talker_is_leader)
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_TALK, "Yes, start the challenge mode of this dungeon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to start this dungeon?", 0, false);
            else
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Only group leader may start the challenge.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(GOSSIP_MENU_HAS_KEYSTONE_CORRECT_DUNGEON, me->GetGUID());
        }
        else
        {
            if (keystone.mapId != player->GetMapId())
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Your current keystone:\nDungeon: " + ChallengeModeMgr::GetKeystoneName(keystone_of_player, keystone.mapId), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                if (talker_is_leader)
                    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_TALK, "I want to reset my keystone and start at level 1 in this dungeon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to delete your keystone and start this dungeon?", 0, false);
                else
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Only group leader may start the challenge.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(GOSSIP_MENU_HAS_KEYSTONE_WRONG_DUNGEON, me->GetGUID());
            }
            else
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, ChallengeModeMgr::GetKeystoneName(keystone_of_player, keystone.mapId), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

                if (keystone.affixes.front() != 0)
                    for (uint8 i = 0; i < MAX_KEYSTONE_AFFIXES; ++i)
                        if (keystone.affixes[i] != 0)
                        {
                            auto affix_data = sWorld->GetAffixDetails(ChallengeModeAffix(keystone.affixes[i]));
                            std::string menu_option {ChallengeModeMgr::GenerateAfficon(25, affix_data->affix_icon) + affix_data->affix_name};

                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, menu_option, GOSSIP_SENDER_MAIN, (AFFIX_INFO + keystone.affixes[i]));
                        }

                if (talker_is_leader)
                    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_BATTLE, "Yes, start the challenge mode of this dungeon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to start this dungeon?", 0, false);
                else
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Only group leader may start the challenge.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                player->SEND_GOSSIP_MENU(GOSSIP_MENU_HAS_KEYSTONE_CORRECT_DUNGEON, me->GetGUID());
            }
        }


    }

    enum VoiceInfo
    {
        AGGRA_SAY_LINE_ONE = 0,
        AGGRA_SAY_LINE_TWO = 1,
        EVENT_AGGRA_SAY_LINE_TWO = 2,
    };

    void sGossipSelect(Player* player, uint32 /*sender*/, uint32 action)
    {
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:
        {
            player->PlayerTalkClass->ClearMenus();
            sGossipHello(player);
        }
        break;
        case GOSSIP_ACTION_INFO_DEF + 1:
        {
            InstanceScript* Instance = player->GetInstanceScript();

            if (!Instance)
                {
                    player->GetSession()->SendNotification("|cffff0000Instance script not found!|r");
                    player->CLOSE_GOSSIP_MENU();
                    return;
                }


            if (auto g = player->GetGroup())
            if (!g->IsLeader(player->GetGUID()))
            {
                player->GetSession()->SendNotification("|cffff0000Only party leaders may start a mythic challenge!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }

            if (Instance->IsChallengeModeStarted())
            {
                player->GetSession()->SendNotification("|cffff0000Challenge already started!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }

            if (Instance->GetCompletedEncounterMask() != 0)
            {
                player->GetSession()->SendNotification("|cffff0000Can't start challenge with 1 or more bosses dead!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }

            if (Instance->GetMythicTamperState())
            {
                player->GetSession()->SendNotification("|cffff0000Can't start challenge with NPCs having entered combat!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }

            if (Group* g = player->GetGroup())
            {
                if (!g->IsLeader(player->GetGUID()))
                {
                    player->GetSession()->SendNotification("|cffff0000Only party leaders may start a mythic challenge!|r");
                    player->CLOSE_GOSSIP_MENU();
                    return;
                }

                if (g->isRaidGroup())
                {
                    player->GetSession()->SendNotification("|cffff0000Challenge mode cannot be started while group is in raid format!|r");
                    player->CLOSE_GOSSIP_MENU();
                    return;
                }
            }
            else
            {
                player->GetSession()->SendNotification("|cffff0000You must be in a group to start a mythic dungeon!|r");
                player->CLOSE_GOSSIP_MENU();
                return;
            }

            Keystone const& player_stone = player->GetKeystone();
            if (player_stone.mapId != 0 && player_stone.mapId != player->GetMapId())
            {
                player->RemoveKeystone();
                player->SetKeystone(player->GetMapId(), 1);
            }

            player->GetInstanceScript()->StartChallengeMode(player);
            Talk(AGGRA_SAY_LINE_ONE, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
            _events.ScheduleEvent(EVENT_AGGRA_SAY_LINE_TWO, 9000);

        }
        break;
        case GOSSIP_ACTION_INFO_DEF + 3:
        {
            player->CLOSE_GOSSIP_MENU();
        }
        case AFFIX_INFO + 1:
        case AFFIX_INFO + 2:
        case AFFIX_INFO + 3:
        case AFFIX_INFO + 4:
        case AFFIX_INFO + 5:
        case AFFIX_INFO + 6:
        case AFFIX_INFO + 7:
        case AFFIX_INFO + 8:
        case AFFIX_INFO + 9:
        case AFFIX_INFO + 10:
        case AFFIX_INFO + 11:
        case AFFIX_INFO + 12:
        case AFFIX_INFO + 13:
        case AFFIX_INFO + 14:
        case AFFIX_INFO + 15:
        case AFFIX_INFO + 16:
        case AFFIX_INFO + 17:
        case AFFIX_INFO + 18:
        case AFFIX_INFO + 19:
        case AFFIX_INFO + 20:
        case AFFIX_INFO + 21:
        case AFFIX_INFO + 22:
        case AFFIX_INFO + 23:
        {

            auto affix_data = sWorld->GetAffixDetails((action - AFFIX_INFO));
            std::string menu_option{ ChallengeModeMgr::GenerateAfficon(25, affix_data->affix_icon) + affix_data->affix_name };

            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, menu_option, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, affix_data->affix_description, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_TEXT_MYTHIC_DETAILS_BACK_BUTTON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(GOSSIP_MENU_AFFIX_INFO, me->GetGUID());
        }
        break;
        }
    }
    void UpdateAI(const uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_AGGRA_SAY_LINE_TWO:
                Talk(AGGRA_SAY_LINE_TWO, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                break;
            default:
                break;
            }
        }
    }
private:
    EventMap _events;
    enum AffixInfo { AFFIX_INFO = 2000 };
};


class npc_challenge_mode_city : public CreatureScript
{
public:
    npc_challenge_mode_city() : CreatureScript("npc_challenge_mode_city") { }

    struct npc_challenge_mode_cityAI : public ScriptedAI
    {
        npc_challenge_mode_cityAI(Creature* creature) : ScriptedAI(creature)
        {

        }
        void InitializeAI() override
        {
            me->SetSheath(SHEATH_STATE_UNARMED);
        }
    };
    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_TEXT_GET_KEYSTONE, GOSSIP_SENDER_MAIN, 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_TEXT_MYTHIC_DETAILS, GOSSIP_SENDER_MAIN, 2);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {

        case 1:
            if (!player->HasItemCount(100050/*Mythic Keystone*/, 1, true))
            {
                player->AddItem(100050/*Mythic Keystone*/, 1);
                std::string whisp = "This is the mythic keystone. It can reveal the details of the threat you must deal with.";
                creature->MonsterWhisper(whisp.c_str(), player->GetGUID());
            }
            else
            {
                std::string whisp = "You already have a mythic Keystone in your bags or bank.";
                creature->MonsterWhisper(whisp.c_str(), player->GetGUID());
            }
            player->CLOSE_GOSSIP_MENU();
            break;
        case 2:
            player->PlayerTalkClass->ClearMenus();
            for (auto i = 1; i < AFFIX_MAX; ++i)
            {
                auto affix_data = sWorld->GetAffixDetails((i));
                std::string menu_option{ ChallengeModeMgr::GenerateAfficon(25, affix_data->affix_icon) + affix_data->affix_name };
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, menu_option, GOSSIP_SENDER_MAIN, i + 2);
            }
            player->SEND_GOSSIP_MENU(54003, creature->GetGUID());
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        {
            player->PlayerTalkClass->ClearMenus();

            auto affix_data = sWorld->GetAffixDetails((action - 2));
            std::string menu_option{ ChallengeModeMgr::GenerateAfficon(25, affix_data->affix_icon) + affix_data->affix_name };

            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, menu_option, GOSSIP_SENDER_MAIN, 100);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, affix_data->affix_description, GOSSIP_SENDER_MAIN, 100);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_TEXT_MYTHIC_DETAILS_BACK_BUTTON, GOSSIP_SENDER_MAIN, 100);

            player->SEND_GOSSIP_MENU(54003, creature->GetGUID());
            break;
        }
        case 100:
            for (auto i = 1; i < AFFIX_MAX; ++i)
            {
                auto affix_data = sWorld->GetAffixDetails((i));
                std::string menu_option{ ChallengeModeMgr::GenerateAfficon(25, affix_data->affix_icon) + affix_data->affix_name };
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, menu_option, GOSSIP_SENDER_MAIN, i + 2);
            }
            player->SEND_GOSSIP_MENU(54003, creature->GetGUID());
            break;
        default:
            break;
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_challenge_mode_cityAI(creature);
    }
};
struct mob_explosive_affix_orb : public ScriptedAI
{
    mob_explosive_affix_orb(Creature* c) : ScriptedAI(c)
    {
        me->ApplyAllSpellImmune(0, true);
        me->AddUnitMovementFlag(MOVEMENTFLAG_HOVER);
    }

    void Reset()
    {
        InstanceScript* script = me->GetInstanceScript();
        if (!script)
            return;

        me->SetReactState(REACT_PASSIVE);
        me->CastSpell(me, AFFIX_SPELL_EXPLOSIVE_ORB_VISUAL, true);
        me->DespawnOrUnsummon(7000);

        me->GetScheduler().Schedule(500ms, [this](TaskContext /*context*/)
            {
                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_CAST_TIME, 6000);
                me->CastCustomSpell(AFFIX_SPELL_EXPLOSIVE_CAST, values, me);
            });

        me->GetScheduler().Schedule(6500ms, [this, script](TaskContext /*context*/)
            {
                me->CastSpell(me, AFFIX_SPELL_EXPLOSIVE_EXPLOSION_VISUAL, true);

                script->DoOnPlayers([this](Player* player)
                    {
                        if (me->GetDistance(player) < 60.f)
                            me->SpellNonMeleeDamageLog(player, AFFIX_SPELL_EXPLOSIVE_NAME, player->CountPctFromMaxHealth(40));
                    });
            });
    }
};

struct mob_sanguine_affix : public ScriptedAI
{
    mob_sanguine_affix(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC);

        me->CastSpell(me, AFFIX_SPELL_SANGUINE_VISUAL, true);

        me->DespawnOrUnsummon(60 * IN_MILLISECONDS);

        me->GetScheduler().Schedule(1s, [this](TaskContext context)
            {
                for (auto player : me->GetPlayersInRange(2.f, true))
                    me->SpellNonMeleeDamageLog(player, AFFIX_SPELL_EXPLOSIVE_NAME, player->CountPctFromMaxHealth(15));

                for (auto unit : me->SelectNearbyUnits(0, 2.f, true))
                {
                    if (unit->IsPlayer() || unit->isPet() || unit->isTotem())
                        continue;

                    uint32 addHealth = unit->CountPctFromMaxHealth(5);
                    int32 gain = me->DealHeal(unit, addHealth);
                    me->SendHealSpellLog(unit, AFFIX_SPELL_EXPLOSIVE_NAME, addHealth, uint32(addHealth - gain), 0);
                }

                context.Repeat();
            });
    }
};

struct mob_volcanic_affix : public ScriptedAI
{
    mob_volcanic_affix(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->DespawnOrUnsummon(3500);

        me->GetScheduler().Schedule(2500ms, [this](TaskContext context)
            {
                for (auto player : me->GetPlayersInRange(2.f, true))
                    me->SpellNonMeleeDamageLog(player, AFFIX_SPELL_EXPLOSIVE_NAME, player->CountPctFromMaxHealth(40));
            });
    }
};

class spell_affix_buff : public AuraScript
{
    PrepareAuraScript(spell_affix_buff);

public:
    spell_affix_buff() : AuraScript() { }

    void BeforeDispel(DispelInfo* dispelData)
    {
        if (auto target = GetTarget())
            if (auto script = target->GetInstanceScript())
                if (script->IsChallengeModeStarted())
                    dispelData->SetRemovedCharges(0);
    }

    void Register()
    {
        OnDispel += AuraDispelFn(spell_affix_buff::BeforeDispel);
    }
};

class spell_affix_check_stack : public SpellScript
{
    PrepareSpellScript(spell_affix_check_stack);

public:
    spell_affix_check_stack() : SpellScript() { }

    void ModAuraStack()
    {
        if (GetCaster()->IsPlayer())
            if (Aura* aur = GetHitAura())
                aur->SetStackAmount(1);
    }

    void Register()
    {
        AfterHit += SpellHitFn(spell_affix_check_stack::ModAuraStack);
    }
};

class spell_challenge_mode_bleeding_wound : public AuraScript
{
    PrepareAuraScript(spell_challenge_mode_bleeding_wound);

    enum {
        NPC_UMBRIS = 39625,
        NPC_UMBRIS_H = 48337,
    };

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        // This spell was originaly from Grim Batol boss Umbris
        // Don't change the duration if he is the caster
        if (auto caster = GetCaster())
        {
            if (caster->GetEntry() != NPC_UMBRIS && caster->GetEntry() != NPC_UMBRIS_H)
            {
                _isChallengeMode = true;
                SetMaxDuration(-1);
                SetDuration(-1);
            }
        }
    }

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        
            if (_isChallengeMode)
            if (auto o = GetOwner()->ToUnit())
            if (auto i = o->GetInstanceScript())
            if (auto level = i->GetKeystone().level)
            {
                float maxhealth     {   float(o->GetMaxHealth())                                                        };
                float max_periodic  {   .08f                                                                            };//max pct possible
                float cap           {   (7.f)                                                                           };//ticks to complete ramp-up
                float pct_per_stack {   (max_periodic * (1.f/cap))                                                      };
                float ticks         {   float(aurEff->GetTickNumber()) <= cap ? float(aurEff->GetTickNumber()) : cap    };
                int32 amount        {   int32(maxhealth * pct_per_stack * ticks)                                        };

                //TC_LOG_ERROR("sql.sql", "max health: %f, tick scale: %f, amount: %u", maxhealth, ticks, amount);
                aurEff->SetAmount(amount);
            }
        
        /*
        if (_isChallengeMode)
        {
            int32 amount = int32(CalculatePct(aurEff->GetBaseAmount(), 100.f * float(aurEff->GetTickNumber()) / 3.f));
            aurEff->SetAmount(amount);
        }
        */
    }

    void Register()
    {
        AfterEffectApply += AuraEffectApplyFn(spell_challenge_mode_bleeding_wound::AfterApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_challenge_mode_bleeding_wound::HandleEffectPeriodicUpdate, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }

private:
    bool _isChallengeMode = false;
};

struct mob_spiteful_affix : public ScriptedAI
{
    mob_spiteful_affix(Creature* c) : ScriptedAI(c) {}

    void InitializeAI()
    {
        me->AddAura(39662, me);
        me->setRegeneratingHealth(false);

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
        me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
        events.ScheduleEvent(1, 1500);
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);

        if (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case 1:
                {
                    me->AddAura(91791, me);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->ClearUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
                    me->SetObjectScale(1.f);

                    auto list_of_players = me->GetPlayersInRange(140.f, true);
                    if (list_of_players.size())
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                        {
                            me->SetAttackTime(BASE_ATTACK, 2500);
                            AttackStart(random_target);
                            me->TauntApply(random_target);
                            me->AddThreat(random_target, 10000.f);
                            if (auto mark = me->AddAura(81355, random_target))
                            {
                                mark->SetDuration(3000);
                                mark->SetMaxDuration(3000);
                            }
                        }

                    break;
                }
                default:
                    break;
            }

        if (!me->HasUnitState(UNIT_STATE_CANNOT_AUTOATTACK))
            if (UpdateVictim())
                DoMeleeAttackIfReady();
    }

    void JustDied(Unit* who)
    {
        auto list_of_players = me->GetPlayersInRange(140.f, true);
        if (list_of_players.size())
            for (auto current_target_within_list : list_of_players)
                if (auto aura = current_target_within_list->GetAura(81355, me->GetGUID()))
                    aura->Remove();
    }

private:
    EventMap events;
};

class mob_storming_affix : public CreatureScript
{
public:
    mob_storming_affix() : CreatureScript("mob_storming_affix") { }

    struct mob_storming_affixAI : public VehicleAI
    {
        mob_storming_affixAI(Creature* creature) : VehicleAI(creature)
        {
            //TC_LOG_ERROR("sql.sql", "Summoned storming npc");
        }

        void Reset()
        {
            events.Reset();
            me->AddAura(88922, me);
            events.ScheduleEvent(1, 3500);
        }
        void IsSummonedBy(Unit* summoner) 
        {
            revolve = (*summoner);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
                if (id == AFFIX_STORMING)
                    events.ScheduleEvent(2, 0);
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                {
                    me->AddAura(96665, me);
                    Position pos(*me);
                    auto p = me->FindNearestPlayer(100.f);
                    me->MoveBlink(pos, 4.f, p ? me->GetAngle(p) :frand(0.f, M_PI * 2.f));
                    me->GetMotionMaster()->MovePoint(AFFIX_STORMING, pos);
                    break;
                }
                case 2:
                    me->GetMotionMaster()->MoveAroundPoint(me->GetHomePosition(), me->GetDistance(me->GetHomePosition()), bool(urand(0, 1)), 32, me->GetPositionZ(), true);
                    events.ScheduleEvent(3, 1);
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);

                    me->RemoveAurasDueToSpell(88922);
                    me->DespawnOrUnsummon(10000);
                    break;
                case 3:
                {
                    auto list_of_players = me->GetPlayersInRange(2.f, true);
                    if (list_of_players.size())
                        for (auto current_target_within_list : list_of_players)
                        {
                            CustomSpellValues values;
                            int32 damage{ int32(float(current_target_within_list->GetMaxHealth()) *.15f) };
                            values.AddSpellMod(SPELLVALUE_BASE_POINT1, damage);
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 180);
                            me->CastCustomSpell(61663, values, current_target_within_list, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
                        }
                    events.ScheduleEvent(3, 300);
                    break;
                }
                default:
                    break;
                }
        }

    private:
        Position revolve{0.f, 0.f, 0.f, 0.f};
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_storming_affixAI(creature);
    }
};

class mob_prideful_affix : public CreatureScript
{
public:
    mob_prideful_affix() : CreatureScript("mob_prideful_affix") { }


    struct mob_prideful_affixAI : public ScriptedAI
    {

        mob_prideful_affixAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            me->AddAura(44773, me);
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);


            events.ScheduleEvent(EVENT_START_RP, 0);

        }
        void Reset()
        {
            pride_ticks = 0;
            me->AddAura(44773, me);
            events.Reset();
            summons.DespawnAll();
            events.ScheduleEvent(EVENT_START_RP, 0);
        }

        bool CanSeeAlways(WorldObject const* obj) 
        { 
            if (auto p = obj->ToPlayer())
                if (p->isGameMaster() || !p->isGMVisible())
                    return false;

            return true;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->isInCombat())
                if (who->ToPlayer())
                    AttackStart(who);
        }


        void JustSummoned(Creature* summon) 
        {
            summons.Summon(summon);
        }
        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id) 
        {
            if (summon->GetEntry() == me->GetEntry() + 1)
                if (type == POINT_MOTION_TYPE)
                    if (id == 500)
                        summon->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* /*who*/)
        {
            pride_ticks = 0;
            if (!intro[0])
            {
                DoAction(501);
                return;
            }
            events.ScheduleEvent(EVENT_END_RP, 0);

        }

        void JustDied(Unit* /*killer*/)
        {

            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();

            auto list_of_players = me->GetPlayersInRange(150.f, true);
            if (list_of_players.size())
                for (auto current_target_within_list : list_of_players)
                {
                    if (auto aura = me->AddAura(70873, current_target_within_list))
                    {
                        aura->SetStackAmount(3);
                        if (auto eff1 = aura->GetEffect(0))
                        {
                            eff1->SetAmount(int32(float(current_target_within_list->GetMaxPower(POWER_MANA)) * .15f));
                        }
                        aura->SetMaxDuration(60000);
                        aura->SetDuration(60000);
                    }
                    /*
                        speed : 76748
                    */
                    CustomSpellValues values2;
                    values2.AddSpellMod(SPELLVALUE_BASE_POINT0, 60);
                    values2.AddSpellMod(SPELLVALUE_DURATION, 60000);
                    values2.AddSpellMod(SPELLVALUE_MAXDURATION, 60000);
                    current_target_within_list->CastCustomSpell(76748, values2, current_target_within_list, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
                }
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            
            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_START_RP,
            EVENT_END_RP,
            EVENT_WIPE_CHECK,

            EVENT_BURSTING_WITH_PRIDE,
            EVENT_BELLIGERENT_BOAST,

            EVENT_BELLIGERENT_BOAST_2,

            SPELL_TSUNAMI = 76560,
            SPELL_WATERBOLT = 76535,
        };

        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH
        };

        void BurstWithPride()
        {
            me->SetHomePosition(*me);
            int32 base{ 800 };
            int32 bonus{ int32(float(base) * float(pride_ticks) * 0.4f) };
            CustomSpellValues values2;
            values2.AddSpellMod(SPELLVALUE_BASE_POINT0, base+bonus);
            values2.AddSpellMod(SPELLVALUE_RADIUS_MOD, 60000);
            if (auto v = me->getVictim())
            me->CastCustomSpell(92025, values2, v, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
            pride_ticks++;
        }

        void BelligerentBoast(bool start)
        {
            auto list_of_players = me->GetPlayersInRange(100.f, true);
            if (start)
            {
                if (list_of_players.size())
                    if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            for (auto i = 0; i <= 3; ++i)
                                if (auto trigger = me->SummonCreature(173730, *random_target, TEMPSUMMON_TIMED_DESPAWN, 20000))
                                {
                                    trigger->SetReactState(REACT_PASSIVE);
                                    trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                                    trigger->DisableSpline();
                                    trigger->m_movementInfo.t_guid = random_target->GetGUID();
                                    switch (i)
                                    {
                                        case 0:
                                            trigger->m_movementInfo.t_pos.Relocate(0.f, 2.f, 4.f, M_PI*0.5f);//left
                                            break;
                                        case 1:
                                            trigger->m_movementInfo.t_pos.Relocate(-2.f, 0.f, 4.f, M_PI);//back
                                            break;
                                        case 2:
                                            trigger->m_movementInfo.t_pos.Relocate(2.f, 0.f, 4.f, 0.f);//fwd
                                            break;
                                        case 3:
                                            trigger->m_movementInfo.t_pos.Relocate(0.f, -2.f, 4.f, M_PI * 1.5f);//right
                                            break;
                                    }

                                    trigger->DisableSpline();
                                    trigger->SendTeleportPacket(trigger->m_movementInfo.t_pos);
                                    trigger->SetHover(true);
                                    trigger->AddAura(82047, trigger);//black
                                    trigger->AddAura(82978, trigger);//alpha to 25%
                                    trigger->AddAura(101511, trigger);//trail
                                }
            }
            else
            {
                std::list<Unit*>casters;

                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 173730, 250.0f);
                if (list_of_npcs.size())
                    for (auto c : list_of_npcs)
                    {
                        c->DisableMovementFlagUpdate(true);
                        c->DeleteThreatList();
                        c->DespawnOrUnsummon(8000);

                        c->SetCanFly(true);
                        c->SetDisableGravity(true);
                        c->SetHover(false);
                        float o{ c->m_movementInfo.t_pos.GetOrientation() };
                        if (auto anchor = Unit::GetUnit(*c, c->m_movementInfo.t_guid))
                        {
                            bool anchor_in_casters{ false };

                            if (casters.size())
                                for (auto caster : casters)
                                    if (caster->GetGUID() == anchor->GetGUID())
                                        anchor_in_casters = true;

                            if (!anchor_in_casters)
                            casters.push_back(anchor);

                            Position move_to{ *anchor };
                            anchor->MovePosition(move_to, 2.f, o, false);
                            anchor->MovePosition(move_to, 2.f, o, true);
                            anchor->MoveBlink(move_to, 80.f, o);

                            if (auto c2 = me->SummonCreature(173730, *anchor, TEMPSUMMON_TIMED_DESPAWN, 15000))
                            {
                                c2->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                                c2->SetReactState(REACT_PASSIVE);
                                c2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                                c2->DisableMovementFlagUpdate(true);
                                c2->SetHover(false);
                                c2->GetMotionMaster()->MovePoint(500, move_to, false, 25.f);
                            }

                        }
                        c->DespawnOrUnsummon();
                    }


                if (casters.size())
                for (auto caster : casters)
                    if (list_of_players.size())
                        for (auto player : list_of_players)
                        {
                            if (caster->GetGUID() != player->GetGUID())
                            {
                                float actual_angle{ fabs(caster->GetRelativeAngleOffset(player)) };
                                float limit    { .08f * M_PI    };

                                if (actual_angle < (NORTH + limit)
                                    || (actual_angle > (SOUTH - limit))
                                    || (actual_angle > (WEST - limit)
                                        && actual_angle < (WEST + limit))
                                    )
                                {
                                    CustomSpellValues values2;
                                    values2.AddSpellMod(SPELLVALUE_BASE_POINT0, 60000);
                                        me->CastCustomSpell(96777, values2, player, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
                                }
                            }
                        }

            }
        }

        void DoAction(int32 const param) 
        {
            if (param == 501)
            {
                me->AddAura(47172, me);
                me->DisableMovementFlagUpdate(true);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
                intro[0] = true;
                events.ScheduleEvent(EVENT_END_RP, 5000);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_PASSIVE);
                auto players = me->GetPlayersInRange(150.f, true);
                for (auto player : players)
                    me->CastSpell(player, 72303, true);

                me->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
                if (instance)
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (auto p = me->FindNearestPlayer(150.f))
                    me->SetFacingTo(me->GetAngle(p));
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_START_RP:
                    break;
                case EVENT_END_RP:
                {
                    auto players = me->GetPlayersInRange(150.f, true);
                    for (auto player : players)
                       player->RemoveAurasDueToSpell(72303);
                    me->RemoveAurasDueToSpell(47172);
                    me->ClearUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->CastStop();
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.Reset();
                    intro[1] = true;
                    events.ScheduleEvent(EVENT_BURSTING_WITH_PRIDE, 2000);
                    events.ScheduleEvent(EVENT_BELLIGERENT_BOAST, 7000);
                    events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);

                    if (auto p = me->FindNearestPlayer(150.f))
                        AttackStart(p);
                }
                    break;
                case EVENT_BURSTING_WITH_PRIDE:
                    BurstWithPride();
                    events.ScheduleEvent(EVENT_BURSTING_WITH_PRIDE, 2000);
                    break;
                case EVENT_BELLIGERENT_BOAST:
                    BelligerentBoast(true);
                    events.ScheduleEvent(EVENT_BELLIGERENT_BOAST_2, 4000);
                    break;
                case EVENT_BELLIGERENT_BOAST_2:
                    BelligerentBoast(false);
                    events.ScheduleEvent(EVENT_BELLIGERENT_BOAST, 15000);
                    break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        me->RemoveAllAuras();
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool intro[2]{ false, false };

        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
        int8 pride_ticks{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_prideful_affixAI(creature);
    }
};

class mob_thundering_affix_mobile_spawner : public CreatureScript
{
public:
    mob_thundering_affix_mobile_spawner() : CreatureScript("mob_thundering_affix_mobile_spawner") { }

    struct mob_thundering_affix_mobile_spawnerAI : public VehicleAI
    {
        mob_thundering_affix_mobile_spawnerAI(Creature* creature) : VehicleAI(creature), summons(creature)
        {
        }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
            me->SetFacingTo(WEST);
            Position start(*me);
            me->SetCanFly(false);
            me->SetDisableGravity(false);
            me->SetHover(false);
            me->MoveBlink(start, 120.f, 0.f);
            me->GetMotionMaster()->MovePoint(500, start, true, 35.f);
            events.ScheduleEvent(1, 1);
            me->DespawnOrUnsummon(15000);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;
            if (point == 500)
            {
                me->DespawnOrUnsummon(500);
            }
        }
        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1://spawn
                {
                    bool spawn{ true };
                    if (auto spot = me->FindNearestCreature(AFFIX_NPC_THUNDERING_TRIGGER, 15.f))
                        if (me->GetExactDist(spot) < 4.f)
                            spawn = false;

                    if (spawn)
                    {
                        Position spawn_at(*me);
                        float angle{ urand(0,1) ? WEST : EAST };

                        me->MoveBlink(spawn_at, 1.5f, angle);
                        if (auto new_spot = me->SummonCreature(AFFIX_NPC_THUNDERING_TRIGGER, spawn_at))
                            new_spot->DespawnOrUnsummon(15000);
                    }
                    events.ScheduleEvent(1, 100);
                    break;
                }
                default:
                    break;
                }
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
        uint32 spawned_at{ getMSTime() };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_thundering_affix_mobile_spawnerAI(creature);
    }
};

class mob_thundering_affix : public CreatureScript
{
public:
    mob_thundering_affix() : CreatureScript("mob_thundering_affix") { }

    struct mob_thundering_affixAI : public VehicleAI
    {
        mob_thundering_affixAI(Creature* creature) : VehicleAI(creature)
        {
        }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
            me->AddAura(77350, me);
            me->SetHover(false);
            me->GetMotionMaster()->MoveFall();
            events.ScheduleEvent(50, 1000);
            Position pos(*me);
            me->MoveBlink(pos, 0.f, 0.f);
            me->NearTeleportTo(pos);
        }


        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 50://activate
                {
                    events.ScheduleEvent(1, urand(1000, 4000));
                    break;
                }
                case 1:
                {
                    Position strike_at(*me);
                    strike_at.m_positionZ -= 6.f;
                    if (auto c = me->SummonCreature(144951, strike_at))
                    {
                        c->SetObjectScale(4.f);
                        c->SetDisableGravity(true);
                        c->SetCanFly(true);
                        c->CastWithDelay(100, c, 52880, true);
                        c->DespawnOrUnsummon(1000);
                    }
                    me->DespawnOrUnsummon(1000);
                    events.ScheduleEvent(2, 100);
                    break;
                }
                case 2:
                {
                    auto list_of_players = me->GetPlayersInRange(5.f, true);
                    if (list_of_players.size())
                        for (auto current_target_within_list : list_of_players)
                        if (me->GetExactDist(current_target_within_list) < 5.f)
                        {
                            /*
                                Stun effect
                            */
                            if (auto stun = me->AddAura(7803, current_target_within_list))
                            {
                                stun->SetMaxDuration(1500);
                                stun->SetDuration(1500);
                            }
                            /*
                                Damage Effect
                            */
                            CustomSpellValues values;
                            float fdamage{ float(current_target_within_list->GetMaxHealth()) * 0.50f };
                            int32 damage{ int32(fdamage) };
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
                            me->CastCustomSpell(90852, values, current_target_within_list, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());
                            /*
                                Orbs Effect
                            */
                            for (auto i = 0; i <= 5; ++i)
                                if (auto trigger = instance->instance->SummonCreature(AFFIX_NPC_THUNDERING_ORB, *current_target_within_list))
                                {
                                    trigger->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                                    trigger->SetReactState(REACT_PASSIVE);
                                    trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                                    trigger->DisableSpline();

                                    trigger->SetHover(true);
                                    trigger->m_movementInfo.t_guid = current_target_within_list->GetGUID();
                                    switch (i)
                                    {
                                    case 0:
                                        trigger->m_movementInfo.t_pos.Relocate(2.f, 0.f, 4.f, M_PI);//left
                                        trigger->AddAura(76650, trigger);//black
                                        break;
                                    case 1:
                                        trigger->m_movementInfo.t_pos.Relocate(0.618036007720638f, 1.90211237658576f, 1.f, M_PI + 5.026544f);//back
                                        trigger->AddAura(76650, trigger);//black
                                        break;
                                    case 2:
                                        trigger->m_movementInfo.t_pos.Relocate(-1.61803149316073f, 1.17557393946108f, 1.f, M_PI + 3.769908f);//fwd
                                        trigger->AddAura(76650, trigger);//black
                                        break;
                                    case 3:
                                        trigger->m_movementInfo.t_pos.Relocate(-1.61803773211996f, -1.17556535226081f, 1.f, M_PI + 2.513272f);//right
                                        trigger->AddAura(76650, trigger);//black
                                        break;
                                    case 4:
                                        trigger->m_movementInfo.t_pos.Relocate(0.618025912859957f, -1.90211565658706f, 1.f, M_PI + 1.256636f);//right
                                        trigger->AddAura(76650, trigger);//black
                                        break;
                                    case 5:
                                        trigger->m_movementInfo.t_pos.Relocate(0.0f, 0.0f, 0.f, 0.0f);//right
                                        trigger->AddAura(98481, trigger);//center
                                        break;
                                    }

                                    trigger->DisableSpline();
                                    trigger->SendTeleportPacket(trigger->m_movementInfo.t_pos);
                                    trigger->DespawnOrUnsummon(25000);
                                }
                        }

                    break;
                }
                default:
                    break;
                }
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_thundering_affixAI(creature);
    }
};

class mob_thundering_orb : public CreatureScript
{
public:
    mob_thundering_orb() : CreatureScript("mob_thundering_orb") { }


    struct mob_thundering_orbAI : public ScriptedAI
    {

        mob_thundering_orbAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
        }
        void Reset()
        {
        }

        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                if (p->isGameMaster() || !p->isGMVisible())
                    return false;

            return true;
        }

        void JustSummoned(Creature* summon)
        {
        }
        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DeleteThreatList();
            events.Reset();
        }
        void MovementInform(uint32 type, uint32 point)
        {
            if (type == POINT_MOTION_TYPE)
                if (point == 500)
                {
                    me->DespawnOrUnsummon();
                }
        }
        void SetGUID(uint64 guid, int32 id) 
        {
            if (id == 500)
                ignore = guid;
        }

        void DoAction(int32 const param)
        {
            if (param == 67)
            {
                me->AddAura(76650, me);//black
                dangerous = true;
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->DisableMovementFlagUpdate(true);
                me->SetHover(false);
                me->DespawnOrUnsummon(25000);
            }

            if (param == 66)
            {
                if (me->HasAura(98481))
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                std::list<Unit*>casters;
                me->DisableMovementFlagUpdate(true);
                me->DeleteThreatList();

                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetHover(false);
                float o{ me->m_movementInfo.t_pos.GetOrientation() };
                if (auto anchor = Unit::GetUnit(*me, me->m_movementInfo.t_guid))
                {
                    bool anchor_in_casters{ false };

                    if (casters.size())
                        for (auto caster : casters)
                            if (caster->GetGUID() == anchor->GetGUID())
                                anchor_in_casters = true;

                    if (!anchor_in_casters)
                        casters.push_back(anchor);

                    Position move_to{ *anchor };
                    anchor->MovePosition(move_to, 2.f, o, false);
                    anchor->MovePosition(move_to, 2.f, o, true);
                    anchor->MoveBlink(move_to, 80.f, o);

                    if (auto c2 = instance->instance->SummonCreature(AFFIX_NPC_THUNDERING_ORB, *anchor))
                    {
                        c2->AI()->SetGUID(anchor->GetGUID(), 500);
                        c2->AI()->DoAction(67);
                        c2->GetMotionMaster()->MovePoint(500, move_to, false, 15.f);
                    }

                }
                me->DespawnOrUnsummon(100);
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            if (dangerous)
                if (auto p = me->FindNearestPlayer(5.f))
                    if (me->GetExactDist(p) < 2.f)
                        if (p->GetGUID() != ignore)
                        {
                        /*
                            Stun effect
                        */
                        if (auto stun = me->AddAura(7803, p))
                        {
                            stun->SetMaxDuration(1500);
                            stun->SetDuration(1500);
                        }
                        /*
                            Damage Effect
                        */
                        CustomSpellValues values; 
                        float fdamage{ float(p->GetMaxHealth()) * 0.50f };
                        int32 damage{ int32(fdamage) };
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, damage);
                        me->CastCustomSpell(90852, values, p, TRIGGERED_FULL_MASK, NULL, NULL, me->GetGUID());

                        dangerous = false;
                        me->DespawnOrUnsummon(500);
                        }
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
        bool dangerous{ false };
        uint64 ignore{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_thundering_orbAI(creature);
    }
};

class mob_thundering_affix_despawner : public CreatureScript
{
public:
    mob_thundering_affix_despawner() : CreatureScript("mob_thundering_affix_despawner") { }

    struct mob_thundering_affix_despawnerAI : public VehicleAI
    {
        mob_thundering_affix_despawnerAI(Creature* creature) : VehicleAI(creature), summons(creature)
        {
        }

        void Reset()
        {
            events.ScheduleEvent(1, 10000);
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1://spawn
                {
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, AFFIX_NPC_THUNDERING_ORB, 500.0f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, AFFIX_NPC_THUNDERING_SPAWNER, 500.0f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, AFFIX_NPC_THUNDERING_TRIGGER, 500.0f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, AFFIX_NPC_THUNDERING_DESPAWNER, 500.0f);

                        if (list_of_npcs.size())
                            for (auto c : list_of_npcs)
                            {
                                switch (c->GetEntry())
                                {
                                case AFFIX_NPC_THUNDERING_SPAWNER:
                                case AFFIX_NPC_THUNDERING_TRIGGER:
                                    c->DeleteThreatList();
                                    c->DespawnOrUnsummon();
                                    break;
                                case AFFIX_NPC_THUNDERING_ORB:
                                    c->AI()->DoAction(66);
                                    break;
                                case AFFIX_NPC_THUNDERING_DESPAWNER:
                                    c->DeleteThreatList();
                                    c->DespawnOrUnsummon(5000);
                                    break;
                                }
                            }
                    break;
                }
                default:
                    break;
                }
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_thundering_affix_despawnerAI(creature);
    }
};


struct mob_entangling_affix : public ScriptedAI
{
    mob_entangling_affix(Creature* c) : ScriptedAI(c) {}


    void InitializeAI() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->DeleteThreatList();
        me->AddUnitState(UNIT_STATE_ROOT);
        me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
        events.ScheduleEvent(1, 8000);
        events.ScheduleEvent(2, 10000);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        me->CastSpell(me, 68766, true);

    }

    void DamageTaken(Unit* damageDealer, uint32& damage)
    {
        damage = 0;
    }

    void IsSummonedBy(Unit* summoner)
    {
    }

    void UpdateAI(uint32 diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case 1://explode
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);

                    auto targets = me->GetPlayersInRange(5.f, true);
                    for (auto p : targets)
                    {
                        if (auto root_stun = me->AddAura(100658, p))
                        {
                            root_stun->SetMaxDuration(6000);
                            root_stun->SetDuration(6000);
                        }
                    }
                    
                    break;
                }
                case 2:
                    me->DespawnOrUnsummon();
                    break;
                case 3:
                {
                }
                break;
                default:
                    break;
            }
    }

private:
    EventMap events;
};

struct mob_incorporeal_affix : public ScriptedAI
{
    mob_incorporeal_affix(Creature* c) : ScriptedAI(c) 
    {
        me->SetReactState(REACT_PASSIVE);
        me->DeleteThreatList();
        me->AddAura(51589, me);
    }

    void Reset()
    {
    }

    bool hasCrowdControlPreventingActions()
    {

        if (me->HasAuraType(SPELL_AURA_MOD_STUN))
            return true;

        if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
            return true;

        if (me->HasAuraType(SPELL_AURA_MOD_FEAR))
            return true;

        if (me->HasAuraType(SPELL_AURA_MOD_CHARM))
            return true;

        if (me->HasAuraWithMechanic(MECHANIC_BANISH))
            return true;

        if (me->HasBreakableByDamageCrowdControlAura())
            return true;

        return false;
    }

    void UpdateAI(uint32 diff)
    {
        if (!hasCrowdControlPreventingActions())
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (!me->HasAura(AFFIX_SPELL_INCORPOREAL_CHANNEL))
                    if (!me->HasSpellCooldown(AFFIX_SPELL_INCORPOREAL_CHANNEL))
                        if (!me->HasCategoryCooldown(AFFIX_SPELL_INCORPOREAL_CHANNEL))
                        {
                            me->StopMoving();
                            CustomSpellValues values;
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                            me->CastCustomSpell(AFFIX_SPELL_INCORPOREAL_CHANNEL, values, me);
                        }
                        else me->RemoveAurasDueToSpell(72391);
                    else me->RemoveAurasDueToSpell(72391);
                else me->RemoveAurasDueToSpell(72391);
            else me->RemoveAurasDueToSpell(72391);
        else me->RemoveAurasDueToSpell(72391);
    }
};

struct mob_afflicted_affix : public ScriptedAI
{
    mob_afflicted_affix(Creature* c) : ScriptedAI(c) 
    {
        me->SetReactState(REACT_PASSIVE);
        me->DeleteThreatList();
        me->SetHealth(me->GetMaxHealth() * 0.5);
        me->setRegeneratingHealth(false);

        me->AddAura(23153, me);//magic dispel
        me->AddAura(23154, me);//curse dispel
        me->AddAura(23169, me);//poison dispel
        me->AddAura(23155, me);//disease dispel
        me->AddAura(19502, me);//cosmetic
        
        me->TalkWithDelay(500, 0, NULL, CHAT_MSG_MONSTER_SAY);
        events.ScheduleEvent(1, 10000);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) 
    {
        if (!faded)
        if (!
            (me->HasAura(23153)
                && me->HasAura(23154)
                && me->HasAura(23169)
                && me->HasAura(23155)
                && (me->GetHealthPct() < 99.f)
                ))
        {

            me->RemoveAurasDueToSpell(19502);//cosmetic
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            events.Reset();
            faded = true;
            me->TalkWithDelay(500, 1, NULL, CHAT_MSG_MONSTER_SAY);
            me->DespawnOrUnsummon(2500);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
            case 1:
            {
                auto players = me->GetPlayersInRange(80.f, true);

                    if (auto selection = RAND(23153, 23154, 23169, 23155))
                    {

                        CustomSpellValues values;

                        switch (selection)
                        {
                        case 23153://blue: 0 = mana burned every second, 1 = %castspeed reduction, 2 = %movement speed cut
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 200);
                            values.AddSpellMod(SPELLVALUE_BASE_POINT1, 10);
                            values.AddSpellMod(SPELLVALUE_BASE_POINT2, 15);
                            break;
                        case 23154:// black: increased fire damage taken %
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 1000);
                            break;
                        case 23155://red: 3s periodic fire damage
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 5000);
                            break;
                        case 23169://green: 0 = 5s periodic nature damage, 1 = healing taken %reduction
                            values.AddSpellMod(SPELLVALUE_BASE_POINT0, 8000);
                            values.AddSpellMod(SPELLVALUE_BASE_POINT1, 25);
                            break;
                        default:
                            break;
                        }

                        values.AddSpellMod(SPELLVALUE_DURATION, 300000);
                        values.AddSpellMod(SPELLVALUE_MAXDURATION, 300000);

                        for (auto p : players)
                            p->CastCustomSpell(selection, values, p, TRIGGERED_FULL_MASK);
                    }

                    events.Reset();
                    faded = true;
                    me->TalkWithDelay(500, 2, NULL, CHAT_MSG_MONSTER_SAY);
                    me->DespawnOrUnsummon(1500);

                
                break;
            }
            default:
                break;
            }
    }

private:
    EventMap events;
    bool faded{ false };

};


class spell_challenge_mode_incorporeal : public AuraScript
{
    PrepareAuraScript(spell_challenge_mode_incorporeal);

public:

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (auto o = GetOwner())
        if (auto c = o->ToUnit())
        {
            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_DURATION, 2000);
            c->CastCustomSpell(AFFIX_SPELL_INCORPOREAL_DEBUFF_AURA, values, c);
            
            if (c->GetUInt32Value(UNIT_NPC_EMOTESTATE) != EMOTE_STATE_SPELL_CHANNEL_OMNI)
                c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_SPELL_CHANNEL_OMNI);
            
        }
    }

    void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
    {
        if (auto o = GetOwner())
            if (auto c = o->ToUnit())
            {
                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_DURATION, 2000);
                c->CastCustomSpell(AFFIX_SPELL_INCORPOREAL_DEBUFF_AURA, values, c);
                
                if (c->GetUInt32Value(UNIT_NPC_EMOTESTATE) != EMOTE_STATE_SPELL_CHANNEL_OMNI)
                    c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_SPELL_CHANNEL_OMNI);
                
            }
    }

    void Register()
    {
        OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_challenge_mode_incorporeal::HandleEffectPeriodicUpdate, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        AfterEffectApply += AuraEffectRemoveFn(spell_challenge_mode_incorporeal::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_affix_incorporeal_filter : public AuraScript
{
    PrepareAuraScript(spell_affix_incorporeal_filter);

public:
    spell_affix_incorporeal_filter() : AuraScript() { }

    bool CheckAreaTarget(Unit* target)
    {
        if (target->ToPlayer())
            return true;

        if (auto o = target->GetOwner())
            if (o->ToPlayer())
                return true;

        return false;
    }
    void Register()
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_affix_incorporeal_filter::CheckAreaTarget);
    }
};


struct mob_shielding_affix : public ScriptedAI
{
    mob_shielding_affix(Creature* c) : ScriptedAI(c)
    {
    }

    void InitializeAI() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->DeleteThreatList();
        me->AddAura(AFFIX_SPELL_EXPLOSIVE_ORB_VISUAL, me);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND_SIGHT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE_PCT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST_2, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INSTAKILL, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INEBRIATE, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_BURN, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_DRAIN, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT_ALL, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_TAMECREATURE, true);

        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);

        me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
        me->AddAura(101511, me);


    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {

    }
};


class spell_affix_shielding_filter : public AuraScript
{
    PrepareAuraScript(spell_affix_shielding_filter);

public:
    spell_affix_shielding_filter() : AuraScript() { }

    bool CheckAreaTarget(Unit* target)
    {
        if (auto c = target->ToCreature())
            if (!c->CanBeSubjectOfAffix(AFFIX_SHIELDING))
                return false;

        if (auto c = GetCaster())
            if (target->GetEntry() == c->GetEntry())
                return false;


        if (target->HasAura(42480))
        return false;

        if (target->GetEntry() == AFFIX_NPC_EXPLOSIVE)
            return false;

        return true;
    }
    void Register()
    {
        DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_affix_shielding_filter::CheckAreaTarget);
    }
};

void AddSC_npc_challenge_mode()
{
    new npc_challenge_mode_city();
    new mob_storming_affix();
    new mob_prideful_affix();
    new mob_thundering_affix();
    new mob_thundering_affix_mobile_spawner();
    new mob_thundering_orb();
    new mob_thundering_affix_despawner();
    RegisterCreatureAI(npc_challenge_mode_handler);
    RegisterCreatureAI(mob_explosive_affix_orb);
    RegisterCreatureAI(mob_sanguine_affix);
    RegisterCreatureAI(mob_volcanic_affix);
    RegisterCreatureAI(mob_spiteful_affix);
    RegisterCreatureAI(mob_entangling_affix);
    RegisterCreatureAI(mob_incorporeal_affix);
    RegisterCreatureAI(mob_afflicted_affix);
    RegisterCreatureAI(mob_shielding_affix);
    RegisterAuraScript(spell_affix_buff);
    RegisterSpellScript(spell_affix_check_stack);
    RegisterAuraScript(spell_challenge_mode_bleeding_wound);
    RegisterAuraScript(spell_challenge_mode_incorporeal);
    RegisterAuraScript(spell_affix_incorporeal_filter);
    RegisterAuraScript(spell_affix_shielding_filter);
};