/*
 * Copyright (C) 2013-2014 trinity core og
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "blackrock_caverns.h"

TeleportationData const teleportationData[] =
{
    {DATA_TELEPORT_CORLA,             BOSS_CORLA             },
    {DATA_TELEPORT_KARSH,             BOSS_KARSH_STEELBENDER },
    {DATA_TELEPORT_BEAUTY,            BOSS_BEAUTY            },
    {0,                               0                      }, // END
};

Position startingPos =          { 223.788986f,  1130.640137f,   205.568848f,    4.201883f };
Position midDungeonPos =        { 279.030121f,  843.465210f,    95.899254f,     4.201888f };
Position startingPos_spawn =    { 232.763f,     1127.83f,       205.57f,        4.201883f };
Position midDungeonPos_spawn =  { 289.391f,     844.403f,       96.007f,        4.201888f };
Position Raz_summon_portals =   { 207.001999f,  602.231995f,    75.477501f,     5.888130f };

class instance_blackrock_caverns : public InstanceMapScript
{
public:
    instance_blackrock_caverns() : InstanceMapScript("instance_blackrock_caverns", 645) { }

    struct instance_blackrock_cavernsInstanceMapScript : public InstanceScript
    {
        instance_blackrock_cavernsInstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadTeleportationData(teleportationData);
            RomoggBonecrusherGUID      = 0;
            CorlaGUID                  = 0;
            KarshSteelbenderGUID       = 0;
            BeautyGUID                 = 0;
            AscendantLordObsidiusGUID  = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            InstanceScript::OnCreatureCreate(creature);

           switch (creature->GetEntry())
            {
                case NPC_ROMOGG_BONECRUSHER:
                    RomoggBonecrusherGUID = creature->GetGUID();
                    break;
                case NPC_CORLA:
                    CorlaGUID = creature->GetGUID();
                    break;
                case NPC_KARSH_STEELBENDER:
                    KarshSteelbenderGUID = creature->GetGUID();
                    break;
                case NPC_BEAUTY:
                    BeautyGUID = creature->GetGUID();
                    break;
                case NPC_ASCENDANT_LORD_OBSIDIUS:
                    AscendantLordObsidiusGUID = creature->GetGUID();
                    break;
                case NPC_RAZ_THE_CRAZED:
                    RazGUID = creature->GetGUID();
                    break;
                case NPC_TWILIGHT_PORTAL:
                    AddTeleporter(creature->GetGUID());
                    break;
            }
        }

        void OnUnitDeath(Unit* unit)
        {
            InstanceScript::OnUnitDeath(unit);
            switch (unit->GetEntry())
            {
            case NPC_BRIDGE_ZEALOT:
                break;
            case NPC_RAZ_THE_CRAZED:
                if (unit->GetDistance(Raz_summon_portals) < 20.f)
                {
                    unit->SummonCreature(51340, startingPos_spawn, TEMPSUMMON_MANUAL_DESPAWN);
                    unit->SummonCreature(51340, midDungeonPos_spawn, TEMPSUMMON_MANUAL_DESPAWN);
                }

                if (unit->FindNearestCreature(NPC_ASCENDANT_LORD_OBSIDIUS, 30.f))
                    if (auto c = unit->ToCreature())
                        c->DespawnOrUnsummon(3000);
                break;
            default:
                break;
            }
        }

        uint64 GetData64(uint32 data) const
        {
            switch (data)
            {
                case DATA_ROMOGG_BONECRUSHER:
                    return RomoggBonecrusherGUID;
                case DATA_CORLA:
                    return CorlaGUID;
                case DATA_KARSH_STEELBENDER:
                    return KarshSteelbenderGUID;
                case DATA_BEAUTY:
                    return BeautyGUID;
                case DATA_ASCENDANT_LORD_OBSIDIUS:
                    return AscendantLordObsidiusGUID;
            }
            return 0;
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (type == BOSS_ROMOGG_BONECRUSHER && (state == DONE || state == DONE_HM))
            {
                if (Creature *raz = instance->GetCreature(RazGUID))
                    raz->AI()->DoAction(ACTION_RAZ_START_EVENT);
            }
            return true;
        }

        private:
            uint64 RomoggBonecrusherGUID;
            uint64 CorlaGUID;
            uint64 KarshSteelbenderGUID;
            uint64 BeautyGUID;
            uint64 AscendantLordObsidiusGUID;
            uint64 RazGUID;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_blackrock_cavernsInstanceMapScript(map);
    }
};

class npc_finkle_einhornq : public CreatureScript
{
    public:
        npc_finkle_einhornq() : CreatureScript("npc_finkle_einhornq") { }

		struct npc_finkle_einhornqAI : public ScriptedAI
		{
			npc_finkle_einhornqAI(Creature* creature) : ScriptedAI(creature) {}

			void UpdateAI(uint32 const diff)
			{
				Map::PlayerList const& player = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = player.begin(); itr != player.end(); ++itr)
					if (auto player = itr->getSource())
					{
						if (me->FindNearestPlayer(150.0f) && player->GetQuestStatus(28732) == QUEST_STATUS_INCOMPLETE)
						{

							player->CompleteQuest(28732);

						}

					}
			}
		};
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_finkle_einhornqAI(creature);
        }
};

enum FinkeleQuest {
    Quest0 = 28735,
    Quest1 = 28737,
    Quest2 = 28738,
    Quest3 = 28740,
    Quest4 = 28741,
};

class npc_finkle_einhorna : public CreatureScript
{
public:
    npc_finkle_einhorna() : CreatureScript("npc_finkle_einhorna") { }

    struct npc_finkle_einhornaAI : public ScriptedAI
    {
        npc_finkle_einhornaAI(Creature* creature) : ScriptedAI(creature) {}

        void UpdateAI(uint32 const diff)
        {
            if (auto player = me->FindNearestPlayer(100.0f))
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            if (auto player = me->FindNearestPlayer(5.0f))
            {
                if (player->GetQuestStatus(Quest0) == QUEST_STATUS_COMPLETE && !player->IsActiveQuest(Quest1))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(Quest1))
                    {
                        if (player->CanAddQuest(quest, false))
                            player->AddQuest(quest, NULL);
                    }
                }
                else if (player->GetQuestStatus(Quest1) == QUEST_STATUS_COMPLETE && !player->IsActiveQuest(Quest2))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(Quest2))
                    {
                        if (player->CanAddQuest(quest, false))
                            player->AddQuest(quest, NULL);
                    }
                }
                else if (player->GetQuestStatus(Quest2) == QUEST_STATUS_COMPLETE && !player->IsActiveQuest(Quest3))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(Quest3))
                    {
                        if (player->CanAddQuest(quest, false))
                            player->AddQuest(quest, NULL);
                    }
                }
                else if (player->GetQuestStatus(Quest3) == QUEST_STATUS_COMPLETE && !player->IsActiveQuest(Quest4))
                {
                    if (Quest const* quest = sObjectMgr->GetQuestTemplate(Quest4))
                    {
                        if (player->CanAddQuest(quest, false))
                            player->AddQuest(quest, NULL);
                    }
                }
            }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_finkle_einhornaAI(creature);
    }
};

class npc_brc_twilight_portal : public CreatureScript
{
public:
    npc_brc_twilight_portal() : CreatureScript("npc_brc_twilight_portal") { }

    struct npc_brc_twilight_portalAI : public PassiveAI
    {
        npc_brc_twilight_portalAI(Creature* creature) : PassiveAI(creature)
        {
            me->SetSpeed(MOVE_FLIGHT, 0.35f);
            instance = me->GetInstanceScript();
        }

        void OnSpellClick(Unit* passenger, bool& result)
        {
            if (passenger->isInCombat())
            {
                if (auto passPlayer = passenger->ToPlayer())
                    if (passPlayer->GetSession())
                        passPlayer->GetSession()->SendNotification(passPlayer->GetSession()->GetTrinityString(LANG_YOU_IN_COMBAT));
                return;
            }

            if (instance)
                if (me->GetMapId() == 645/*Blackrock Caverns*/)
                    if (instance->GetDeadBossTotal(true) >= 2)
                    {
                        if (passenger->GetDistance(startingPos) < passenger->GetDistance(midDungeonPos))//player is clicking the starting portal
                            passenger->NearTeleportTo(midDungeonPos);
                        else
                            passenger->NearTeleportTo(startingPos);
                    }
        };
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brc_twilight_portalAI(creature);
    }
};

class spell_defiled_rager_meteor : public SpellScriptLoader // 76717
{
public:
    spell_defiled_rager_meteor() : SpellScriptLoader("spell_defiled_rager_meteor") {}

    class spell_defiled_rager_meteor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_defiled_rager_meteor_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                        if (auto t = (*itr))
                            if (!t->ToPlayer())
                            {
                                targets.erase(itr++);
                            }
                            else
                            {
                                ++itr;
                            }
        }
        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_defiled_rager_meteor_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_defiled_rager_meteor_SpellScript();
    }
};

class AreaTrigger_at_corla_entrance : public AreaTriggerScript
{
public:
    AreaTrigger_at_corla_entrance() : AreaTriggerScript("at_corla_entrance") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
    {
        if (player->isGameMaster() || !player->isGMVisible())
            return false;

        if (auto instance = player->GetInstanceScript())
            if (auto corla = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_CORLA)))
                corla->AI()->DoAction(99);//respawn zealot if they haven't been yet. this fixes a client issue where one of the zealots doesn't show kneeling or the aura.

        return false;
    }
};
/*
    Evolved Twilight zealot
*/
enum bridge_zealot
{
    EVENT_GRAVITY_STRIKE = 1,
    EVENT_SHADOW_STRIKE,
    EVENT_GRIEVOUS_WHIRL,
    EVENT_FORCE_BLAST,

    SPELL_GRAVITY_STRIKE = 76561,
    SPELL_SHADOW_STRIKE = 82362,
    SPELL_GRIEVOUS_WHIRL = 76524,
    SPELL_FORCE_BLAST    = 76522,
};

class npc_evolved_twilight_zealot_bridge : public CreatureScript
{
public:
    npc_evolved_twilight_zealot_bridge() : CreatureScript("npc_evolved_twilight_zealot_bridge") { }


    struct npc_evolved_twilight_zealot_bridgeAI : public ScriptedAI
    {

        npc_evolved_twilight_zealot_bridgeAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            events.ScheduleEvent(EVENT_GRAVITY_STRIKE, urand(5000, 8000));
            events.ScheduleEvent(EVENT_SHADOW_STRIKE, urand(9800, 25000));
            events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, urand(9800, 25000));
            events.ScheduleEvent(EVENT_FORCE_BLAST, urand(9800, 25000));
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void InitializeAI()
        {
            events.Reset();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* /*spell*/) 
        {
            if (target->GetEntry() == NPC_RAZ_THE_CRAZED)
            {
                if (target->GetHealthPct() <= 90.f)
                {
                    target->SetHealth(target->GetMaxHealth() * 0.90f);
                    //TC_LOG_ERROR("sql.sql", "Raz was under 90% health so I forced him to heal up.");
                }

                if (!target->HasAura(80218))
                {
                    target->CastSpell(target, 80218, true);
                    //TC_LOG_ERROR("sql.sql", "Raz was missing his aura so I forced him to reapply it.");
                }
            }
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
            Position home = me->GetHomePosition();

            if (auto raz_clear = me->FindNearestCreature(NPC_RAZ_THE_CRAZED, 50.f))
                raz_clear->SetHealth(raz_clear->GetMaxHealth());
            /*
            X: 293.865997 Y: 814.999023 Z: 103.523422 Orientation: 0.015312
            X: 413.495361 Y: 817.437988 Z: 103.498428 Orientation: 0.031019
            X: 376.032257 Y: 847.860168 Z: 104.891716 Orientation: 3.078361
            X: 379.758118 Y: 783.534241 Z: 107.126411 Orientation: 2.850569

            */
                if (
                    (home.GetPositionZ() > 120.37482f)
                    ||
                    (home.GetPositionX() > 413.495361f || home.GetPositionX() < 293.865997f)
                    ||
                    (home.GetPositionY() > 847.860168f || home.GetPositionY() < 783.534241f)
                    )
                {
                    //springvale is outside his box.
                    //TC_LOG_ERROR("sql.sql", "Zealot died outside of the specified box.");
                }
                else
                {
                    //TC_LOG_ERROR("sql.sql", "Zealot died inside of the specified box.");
                    //springvale is inside his box

                    if (auto raz = me->FindNearestCreature(NPC_RAZ_THE_CRAZED, 50.f))
                        if (!raz->FindNearestCreature(NPC_BRIDGE_ZEALOT, 100.f, true))
                        {
                            //TC_LOG_ERROR("sql.sql", "There are no more bridge zealots nearby to attack, raz is done fighting.");
                            raz->setFaction(35);
                            raz->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                        }
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
                UpdateVictim();
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

        void UpdateAI(const uint32 diff) override
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_GRAVITY_STRIKE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_GRAVITY_STRIKE;             //Spell ID
                        min_repeat = 5000;                  //Minimum Delay
                        max_repeat = 8000;                 //Maximum Delay
                        wait_castnext = true;               //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        DoCastVictim(spell);
                        UpdateVictim();
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_SHADOW_STRIKE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_SHADOW_STRIKE;       //Spell ID
                        min_repeat = 10000;                  //Minimum delay
                        max_repeat = 25000;                 //Maximum delay
                        wait_castnext = true;               //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        DoCastRandom(spell, 20.f);
                        UpdateVictim();
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_GRIEVOUS_WHIRL:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_GRIEVOUS_WHIRL;              //Spell ID
                        min_repeat = 10000;                      //Minimum delay
                        max_repeat = 25000;                      //Maximum delay
                        wait_castnext = false;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    }
                    break;
                case EVENT_FORCE_BLAST:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "EVENT_FORCE_BLAST");
                        spell = SPELL_FORCE_BLAST;              //Spell ID
                        min_repeat = 10000;                      //Minimum delay
                        max_repeat = 25000;                      //Maximum delay
                        wait_castnext = false;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    }
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
            {
                if (me->HasReactState(REACT_AGGRESSIVE))
                {
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        if (UpdateVictim())
                        {
                            DoMeleeAttackIfReady();
                        }
                    }
                }
            }
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_evolved_twilight_zealot_bridgeAI(creature);
    }
};

void AddSC_instance_blackrock_caverns()
{
    new spell_defiled_rager_meteor();
    new instance_blackrock_caverns();
	new npc_finkle_einhornq();
    new npc_finkle_einhorna();
    new npc_brc_twilight_portal();
    new AreaTrigger_at_corla_entrance();
    new npc_evolved_twilight_zealot_bridge();
}
