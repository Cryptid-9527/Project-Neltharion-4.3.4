/*
 * Copyright (C) 2010 - 2012 Project SkyFire <http://www.projectskyfire.org/>
 *
 * Copyright (C) 2011 - 2012 ArkCORE <http://www.arkania.net/>
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

#include "AchievementMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "Player.h"
#include "Vehicle.h"
#include "GameObject.h"
#include "CreatureAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SmartAI.h"
#include "SmartScript.h"

enum KezanDefaultSpells
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_PING_2 = 98915,
    SPELL_DUMMY_PING_3 = 98916,
    SPELL_DUMMY_PING_4 = 98917,
    SPELL_DUMMY_PING_5 = 98918,
    SPELL_SANCTUARY_NO_COMBAT = 88467,
    SPELL_ROOT = 42716,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_FLIGHT_SPEED_280 = 50345,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum KezanEvents
{
    EVENT_PARTYGOER_1 = 1,
    EVENT_PARTYGOER_2,
    EVENT_DEATHWING_1,
    EVENT_DEATHWING_2,
    EVENT_LIFE_SAVING_1,
    EVENT_LIFE_SAVING_2,
    EVENT_LIFE_SAVING_3,
};

enum NPCPartygoer
{
    // from override spell bar
    SPELL_HIT_DRINK = 66909,
    SPELL_HIT_WATER = 66910,
    SPELL_HIT_DANCE = 66911,
    SPELL_HIT_FIRE = 66912,
    SPELL_HIT_FOOD = 66913,

    // npcs aura
    SPELL_BUBBLY = 75042,
    SPELL_BUCKET = 75044,
    SPELL_DANCE = 75046,
    SPELL_FIREWORKS = 75048,
    SPELL_HORS_DOEUVRES = 75050,

    SPELL_HAPPY_PARTYGOER = 66916,

    SPELL_LIFE_OF_THE_PARTY_FIREWORKS_BLUE = 66917,
    SPELL_LIFE_OF_THE_PARTY_FIREWORKS_GREEN = 66918,
    SPELL_LIFE_OF_THE_PARTY_FIREWORKS_RED = 66919,
    SPELL_LIFE_OF_THE_PARTY_PAPARAZZI = 66929,
    SPELL_LIFE_OF_THE_PARTY_SUMMON_DISCO_BALL = 66930,
    SPELL_LIFE_OF_THE_PARTY_SUMMON_BUCKET = 66931,

    SPELL_LIFE_OF_THE_PARTY_PC_HORS_DOEUVRES = 75124,
    SPELL_LIFE_OF_THE_PARTY_PC_DANCE = 75123,
    SPELL_LIFE_OF_THE_PARTY_PC_BUBBLY = 75122,

    SPELL_COSMETIC_STUN = 46957,
    SPELL_COSMETIC_DRUNKEN_INTOXICATION = 55664,

    NPC_PARTYGOER_ENTERTAINED = 35175,
    NPC_PARTYGOER_1 = 35186,

    QUEST_LIFE_OF_THE_PARTY_FEMALE = 14153,
    QUEST_LIFE_OF_THE_PARTY_MALE = 14113,
};

class npc_kezan_partygoer : public CreatureScript
{
public:
    npc_kezan_partygoer() : CreatureScript("npc_kezan_partygoer") { }

    struct npc_kezan_partygoerAI : public ScriptedAI
    {
        npc_kezan_partygoerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, const SpellInfo* spell)
        {
            if (auto pl = who->ToPlayer())
            {
                switch (spell->Id)
                {
                case SPELL_HIT_DRINK:
                    if (!me->GetAura(SPELL_BUBBLY))
                        return;
                    else
                    {
                        me->RemoveAura(SPELL_BUBBLY);
                        me->CastSpell(me, SPELL_HAPPY_PARTYGOER);
                        me->AI()->Talk(6, pl->GetGUID());
                        me->CastSpell(pl, SPELL_LIFE_OF_THE_PARTY_PC_BUBBLY);
                        me->SetFacingToObject(pl);
                        pl->KilledMonsterCredit(NPC_PARTYGOER_ENTERTAINED);
                        _events.ScheduleEvent(EVENT_PARTYGOER_1, 90000);
                        _events.CancelEvent(EVENT_PARTYGOER_2);
                    }
                    break;
                case SPELL_HIT_WATER:
                    if (!me->GetAura(SPELL_BUCKET))
                        return;
                    else
                    {
                        me->RemoveAura(SPELL_BUCKET);
                        me->RemoveAura(SPELL_COSMETIC_STUN);
                        me->RemoveAura(SPELL_COSMETIC_DRUNKEN_INTOXICATION);
                        me->CastSpell(me, SPELL_HAPPY_PARTYGOER);
                        me->CastSpell(me, SPELL_LIFE_OF_THE_PARTY_SUMMON_BUCKET);
                        me->AI()->Talk(8, pl->GetGUID());
                        me->SetFacingToObject(pl);
                        pl->KilledMonsterCredit(NPC_PARTYGOER_ENTERTAINED);
                        _events.ScheduleEvent(EVENT_PARTYGOER_1, 90000);
                        _events.CancelEvent(EVENT_PARTYGOER_2);
                    }
                    break;
                case SPELL_HIT_DANCE:
                    if (!me->GetAura(SPELL_DANCE))
                        return;
                    else
                    {
                        me->SetFacingToObject(pl);
                        me->RemoveAura(SPELL_DANCE);
                        me->CastSpell(me, SPELL_HAPPY_PARTYGOER);
                        me->CastSpell(me, SPELL_LIFE_OF_THE_PARTY_SUMMON_DISCO_BALL);
                        me->AI()->Talk(5, pl->GetGUID());
                        me->CastSpell(pl, SPELL_LIFE_OF_THE_PARTY_PC_DANCE);
                        pl->KilledMonsterCredit(NPC_PARTYGOER_ENTERTAINED);
                        _events.ScheduleEvent(EVENT_PARTYGOER_1, 90000);
                        _events.CancelEvent(EVENT_PARTYGOER_2);
                    }
                    break;
                case SPELL_HIT_FIRE:
                    if (!me->GetAura(SPELL_FIREWORKS))
                        return;
                    else
                    {
                        me->SetFacingToObject(pl);
                        me->RemoveAura(SPELL_FIREWORKS);
                        me->CastSpell(me, SPELL_HAPPY_PARTYGOER);
                        me->AI()->Talk(9, pl->GetGUID());
                        pl->KilledMonsterCredit(NPC_PARTYGOER_ENTERTAINED);
                        _events.ScheduleEvent(EVENT_PARTYGOER_1, 90000);
                        _events.CancelEvent(EVENT_PARTYGOER_2);

                        switch (urand(0, 2))
                        {
                        case 0:
                            me->CastSpell(me, SPELL_LIFE_OF_THE_PARTY_FIREWORKS_BLUE);
                            break;
                        case 1:
                            me->CastSpell(me, SPELL_LIFE_OF_THE_PARTY_FIREWORKS_GREEN);
                            break;
                        case 2:
                            me->CastSpell(me, SPELL_LIFE_OF_THE_PARTY_FIREWORKS_RED);
                            break;
                        }
                    }
                    break;
                case SPELL_HIT_FOOD:
                    if (!me->GetAura(SPELL_HORS_DOEUVRES))
                        return;
                    else
                    {
                        me->SetFacingToObject(pl);
                        me->RemoveAura(SPELL_HORS_DOEUVRES);
                        me->CastSpell(me, SPELL_HAPPY_PARTYGOER);
                        me->CastSpell(pl, SPELL_LIFE_OF_THE_PARTY_PC_HORS_DOEUVRES);
                        me->AI()->Talk(7, pl->GetGUID());
                        pl->KilledMonsterCredit(NPC_PARTYGOER_ENTERTAINED);
                        _events.ScheduleEvent(EVENT_PARTYGOER_1, 90000);
                        _events.CancelEvent(EVENT_PARTYGOER_2);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void DoBuff()
        {
            switch (urand(1, 5))
            {
            case 1:
                me->AddAura(SPELL_BUBBLY, me);
                break;
            case 2:
                me->AddAura(SPELL_BUCKET, me);
                break;
            case 3:
                me->AddAura(SPELL_HORS_DOEUVRES, me);
                break;
            case 4:
                me->AddAura(SPELL_DANCE, me);
                break;
            case 5:
                me->AddAura(SPELL_FIREWORKS, me);
                break;
            default:
                break;
            }
            me->RemoveAura(SPELL_HAPPY_PARTYGOER);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!_hasAura)
            {
                DoBuff();
                _hasAura = true;
            }

            if (!_startingEmote)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (me->GetAura(SPELL_BUBBLY))
                    {
                        me->HandleEmoteCommand(EMOTE_STATE_EAT);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(5000, 300000));
                        _startingEmote = true;
                    }
                    else if (me->GetAura(SPELL_BUCKET))
                    {
                        me->CastSpell(me, SPELL_COSMETIC_STUN);
                        me->CastSpell(me, SPELL_COSMETIC_DRUNKEN_INTOXICATION);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(4000, 300000));
                        _startingEmote = true;
                    }
                    else if (me->GetAura(SPELL_HORS_DOEUVRES))
                    {
                        me->HandleEmoteCommand(EMOTE_STATE_EAT);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(7000, 300000));
                        _startingEmote = true;
                    }
                    else if (me->GetAura(SPELL_DANCE))
                    {
                        me->HandleEmoteCommand(EMOTE_STATE_DANCE);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(4000, 300000));
                        _startingEmote = true;
                    }
                    else if (me->GetAura(SPELL_FIREWORKS))
                    {
                        me->SetCurrentEquipmentId(me->GetEntry());
                        me->HandleEmoteCommand(EMOTE_STATE_READY_THROWN);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(8000, 300000));
                        _startingEmote = true;
                    }
                }
                else _checkTimer -= diff;
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PARTYGOER_1:
                    _startingEmote = false;
                    me->RemoveAllAuras();
                    DoBuff();
                    break;

                case EVENT_PARTYGOER_2:
                    if (me->GetAura(SPELL_BUBBLY))
                    {
                        me->AI()->Talk(1);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(25000, 300000));
                    }
                    else if (me->GetAura(SPELL_BUCKET))
                    {
                        me->AI()->Talk(3);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(25000, 300000));
                    }
                    else if (me->GetAura(SPELL_HORS_DOEUVRES))
                    {
                        me->AI()->Talk(2);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(25000, 300000));
                    }
                    else if (me->GetAura(SPELL_DANCE))
                    {
                        me->AI()->Talk(0);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(25000, 300000));
                    }
                    else if (me->GetAura(SPELL_FIREWORKS))
                    {
                        me->AI()->Talk(4);
                        _events.ScheduleEvent(EVENT_PARTYGOER_2, urand(25000, 300000));
                    }
                default:
                    break;
                }
            }
        }

    private:
        bool _hasAura = false;
        bool _startingEmote = false;
        uint64 _aura = 0;
        uint32 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kezan_partygoerAI(creature);
    }
};

enum HotRod
{
    QUEST_ROLLING_WITH_MY_HOMIES = 14071,
    QUEST_WALTZ_RIGHT_IN = 14123,

    SPELL_PING_ACE_IZZY_GOBBER = 151124,
    SPELL_RIDE_HOT_ROD_AURA = 66392,

    SPELL_ROLLING_WITH_MY_HOMIES_SUMMON_ACE = 66597,
    SPELL_ROLLING_WITH_MY_HOMIES_SUMMON_GOBBER = 66599,
    SPELL_ROLLING_WITH_MY_HOMIES_SUMMON_IZZY = 66600,

    SPELL_COSMETIC_MOOK_DISGUISE = 75098,
    SPELL_MOOK_DISGUISE_PC_AURA = 67435,

    NPC_MOOK_DISGUISE = 48925,

    NPC_SUMMONED_IZZY = 34959,
    NPC_SUMMONED_GOBBER = 34958,
    NPC_SUMMONED_ACE = 34957,

    NPC_CREDIT_IZZY = 34890,
    NPC_CREDIT_GOBBER = 34954,
    NPC_CREDIT_ACE = 34892,

    AREA_GALLYWIX_VILLA = 4768,
};

const Position SummonHomies[] =
{
    { -8507.459f, 1342.829f, 101.69f, 6.2571f },
    { -8179.379f, 1321.510f, 27.617f, 5.0608f },
    { -8065.600f, 1475.520f, 8.9208f, 3.0085f },
};

class npc_hot_rod_controllable_vehicle : public CreatureScript
{
public:
    npc_hot_rod_controllable_vehicle() : CreatureScript("npc_hot_rod_controllable_vehicle") { }

    struct npc_hot_rod_controllable_vehicleAI : public VehicleAI
    {
        npc_hot_rod_controllable_vehicleAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                    player->RemoveAura(SPELL_RIDE_HOT_ROD_AURA);
                    player->SendRemoveControlBar();
                    _summons.DespawnAll();
                    me->DespawnOrUnsummon();
                }
            }

            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (spellInfo->Id == SPELL_DUMMY_PING && !_izzyBoarded)
                {
                    player->RemoveAura(49417);
                    player->KilledMonsterCredit(NPC_CREDIT_IZZY);

                    if (auto izzy = me->SummonCreature(NPC_SUMMONED_IZZY, SummonHomies[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        izzy->EnterVehicle(me, 1);
                        _summons.Summon(izzy);
                        _izzyBoarded = true;
                    }
                }

                if (spellInfo->Id == SPELL_DUMMY_PING_2 && !_gobberBoarded)
                {
                    player->RemoveAura(60922);
                    player->KilledMonsterCredit(NPC_CREDIT_GOBBER);

                    if (auto gobber = me->SummonCreature(NPC_SUMMONED_GOBBER, SummonHomies[1], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        gobber->EnterVehicle(me, 2);
                        _summons.Summon(gobber);
                        _gobberBoarded = true;
                    }
                }

                if (spellInfo->Id == SPELL_DUMMY_PING_3 && !_aceBoarded)
                {
                    player->RemoveAura(49416);
                    player->KilledMonsterCredit(NPC_CREDIT_ACE);

                    if (auto ace = me->SummonCreature(NPC_SUMMONED_ACE, SummonHomies[2], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        ace->EnterVehicle(me, 3);
                        _summons.Summon(ace);
                        _aceBoarded = true;
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetDistance(player) > 20.f || !me->GetMap()->IsOutdoors(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()))
                    {
                        player->RemoveAura(SPELL_RIDE_HOT_ROD_AURA);
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        bool _gobberBoarded = false;
        bool _izzyBoarded = false;
        bool _aceBoarded = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;

        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hot_rod_controllable_vehicleAI(creature);
    }
};

class npc_mook_disguise_waltz : public CreatureScript
{
public:
    npc_mook_disguise_waltz() : CreatureScript("npc_mook_disguise_waltz") { }

    struct npc_mook_disguise_waltzAI : public ScriptedAI
    {
        npc_mook_disguise_waltzAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->SetReactState(REACT_PASSIVE);
                me->EnterVehicle(player, 0);
                me->AddAura(SPELL_COSMETIC_MOOK_DISGUISE, me);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 500;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_WALTZ_RIGHT_IN) == QUEST_STATUS_NONE)
                    {
                        player->RemoveAura(SPELL_COSMETIC_MOOK_DISGUISE);
                        player->RemoveAura(SPELL_MOOK_DISGUISE_PC_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetAreaId() != AREA_GALLYWIX_VILLA || player->isInCombat())
                    {
                        player->RemoveAura(SPELL_COSMETIC_MOOK_DISGUISE);
                        player->RemoveAura(SPELL_MOOK_DISGUISE_PC_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->GetAura(SPELL_RIDE_VEHICLE) || player->GetAura(SPELL_RIDE_VEHICLE))
                    {
                        me->EnterVehicle(player, 0);
                    }
                }
            }
            else _checkTimer -= diff;

        }
    private:
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mook_disguise_waltzAI(creature);
    }
};

class spell_apply_waltz_mook_disguise : public SpellScriptLoader
{
public:
    spell_apply_waltz_mook_disguise() : SpellScriptLoader("spell_apply_waltz_mook_disguise") { }

    class spell_apply_waltz_mook_disguise_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_apply_waltz_mook_disguise_AuraScript);

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (auto caster = GetCaster()->ToPlayer())
            {
                if (caster->GetAreaId() != AREA_GALLYWIX_VILLA)
                    return;

                if (caster->isAlive())
                {
                    if (caster->isInCombat())
                    {
                        if (caster->GetAura(SPELL_MOOK_DISGUISE_PC_AURA))
                        {
                            caster->RemoveAura(SPELL_COSMETIC_MOOK_DISGUISE);
                            caster->RemoveAura(SPELL_MOOK_DISGUISE_PC_AURA);

                            std::list<Unit*> mook = caster->SelectNearbyUnits(NPC_MOOK_DISGUISE, 10.0f);
                            for (std::list<Unit*>::iterator itr = mook.begin(); itr != mook.end(); ++itr)
                                if (auto mook = (*itr))
                                {
                                    if (mook->ToCreature())
                                        if (mook->GetOwnerGUID() == caster->GetGUID())
                                            mook->ToCreature()->DespawnOrUnsummon();
                                }
                        }
                    }
                    else
                    {
                        if (!caster->GetAura(SPELL_MOOK_DISGUISE_PC_AURA))
                        {
                            caster->CastSpell(caster, SPELL_COSMETIC_MOOK_DISGUISE);
                            caster->CastSpell(caster, SPELL_MOOK_DISGUISE_PC_AURA);
                            caster->SummonCreature(NPC_MOOK_DISGUISE, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                        }
                    }
                }
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_apply_waltz_mook_disguise_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_apply_waltz_mook_disguise_AuraScript();
    }
};

enum QuestNecessaryRoughness
{
    QUEST_NECESSARY_ROUGHNESS = 24502,

    SPELL_RIDE_MECH = 70016,

    NPC_SUMMONED_STEAMWHEEDLE_SHARK = 37114,
    NPC_BOARD_CREDIT = 37179,
};

const Position SummonMechs[] =
{
    { -8295.049f, 1477.75f, 44.167f, 0.011f },
    { -8295.049f, 1484.85f, 44.218f, 0.000f },
    { -8294.099f, 1491.54f, 44.169f, 6.267f },
};

const Position MovePointMechs[] =
{
    { -8258.45f, 1477.040f, 41.89f },
    { -8258.21f, 1484.519f, 41.98f },
    { -8257.05f, 1490.789f, 41.93f },
};

class npc_bilgewater_buccaneer_roughness : public CreatureScript
{
public:
    npc_bilgewater_buccaneer_roughness() : CreatureScript("npc_bilgewater_buccaneer_roughness") { }

    struct npc_bilgewater_buccaneer_roughnessAI : public VehicleAI
    {
        npc_bilgewater_buccaneer_roughnessAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                me->SetRooted(true);
                _playerGUID = player->GetGUID();
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetGUID() == _playerGUID)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->SetRooted(true);
                    player->RemoveAura(49416);
                    player->KilledMonsterCredit(NPC_BOARD_CREDIT);
                    _startEvent = true;
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                }
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _startEvent = false;
                passenger->RemoveAura(SPELL_RIDE_MECH);
                _summons.DespawnAll();
                me->DespawnOrUnsummon();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (spellInfo->Id == SPELL_DUMMY_PING)
                    player->KilledMonsterCredit(NPC_SUMMONED_STEAMWHEEDLE_SHARK);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_INCOMPLETE)
                        player->RemoveAura(49416);

                    if (player->GetQuestStatus(QUEST_NECESSARY_ROUGHNESS) == QUEST_STATUS_COMPLETE)
                    {
                        _startEvent = false;
                        player->ExitVehicle();
                        player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                    }
                }
            }
            else _checkTimer -= diff;

            if (_startEvent)
            {
                if (_summonTimer <= diff)
                {
                    _summonTimer = 4500;

                    if (auto enemy1 = me->SummonCreature(NPC_SUMMONED_STEAMWHEEDLE_SHARK, SummonMechs[0], TEMPSUMMON_TIMED_DESPAWN, 21000))
                        if (auto enemy2 = me->SummonCreature(NPC_SUMMONED_STEAMWHEEDLE_SHARK, SummonMechs[1], TEMPSUMMON_TIMED_DESPAWN, 21000))
                            if (auto enemy3 = me->SummonCreature(NPC_SUMMONED_STEAMWHEEDLE_SHARK, SummonMechs[2], TEMPSUMMON_TIMED_DESPAWN, 21000))
                            {
                                _summons.Summon(enemy1);
                                _summons.Summon(enemy2);
                                _summons.Summon(enemy3);
                                enemy1->GetMotionMaster()->MovePoint(1, MovePointMechs[0]);
                                enemy2->GetMotionMaster()->MovePoint(1, MovePointMechs[1]);
                                enemy3->GetMotionMaster()->MovePoint(1, MovePointMechs[2]);
                            }
                }
                else _summonTimer -= diff;
            }
        }
    private:
        bool _startEvent = false;
        uint32 _checkTimer = 2000;
        uint32 _summonTimer = 3000;
        uint64 _playerGUID = 0;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bilgewater_buccaneer_roughnessAI(creature);
    }
};

uint32 const DeathwingPathSize = 2;
const G3D::Vector3 DeathwingPath[DeathwingPathSize] =
{
    { -8269.45f, 1483.31f, 105.86f },
    { -8330.67f, 1482.86f, 140.12f },
};

uint32 const DeathwingPathSize2 = 4;
const G3D::Vector3 DeathwingPath2[DeathwingPathSize2] =
{
    { -8400.024f, 1483.00f, 184.54f },
    { -8441.695f, 1493.09f, 193.23f },
    { -8438.126f, 1554.57f, 172.97f },
    { -8278.64f, 1483.46f,  62.621f }, // 3
};

uint32 const DeathwingPathSize3 = 2;
const G3D::Vector3 DeathwingPath3[DeathwingPathSize3] =
{
    { -8261.20f, 1470.82f, 71.794f, },
    { -8240.08f, 1452.61f, 67.155f, }, // 1
};

class npc_deathwing_scene_fourth_and_goal : public CreatureScript
{
public:
    npc_deathwing_scene_fourth_and_goal() : CreatureScript("npc_deathwing_scene_fourth_and_goal") { }

    struct npc_deathwing_scene_fourth_and_goalAI : public VehicleAI
    {
        npc_deathwing_scene_fourth_and_goalAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->GetMotionMaster()->MoveSmoothPath(DeathwingPath, DeathwingPathSize);
            me->AI()->TalkWithDelay(1000, 0);

            if (summoner->ToCreature())
                _ownerGUID = summoner->GetGUID();
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
                ;
            case 1:
                if (_flightMode == 0)
                {
                    me->CastSpell(me, 66858);
                    _events.ScheduleEvent(EVENT_DEATHWING_1, 2500);
                }
                break;
            case 3:
                if (_flightMode == 1)
                {
                    me->AI()->Talk(1);
                    _events.ScheduleEvent(EVENT_DEATHWING_2, 3000);
                }
                break;
            case 2:
                if (_flightMode == 2)
                    me->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DEATHWING_1:
                    _flightMode++;
                    me->GetMotionMaster()->MoveSmoothPath(DeathwingPath2, DeathwingPathSize2);
                    break;
                case EVENT_DEATHWING_2:
                    _flightMode++;

                    if (auto owner = Unit::GetCreature(*me, _ownerGUID))
                    {
                        owner->AI()->SetData(2, 2);
                        me->GetMotionMaster()->MoveSmoothPath(DeathwingPath3, DeathwingPathSize3);
                        me->DespawnOrUnsummon(3000);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        uint64 _ownerGUID = 0;
        uint8 _flightMode = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathwing_scene_fourth_and_goalAI(creature);
    }
};

#define RADIO 23406

class spell_radio : public SpellScriptLoader
{
public:
    spell_radio() : SpellScriptLoader("spell_radio") { }

    class  spell_radioSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_radioSpellScript);

        bool Validate(SpellInfo const* spellEntry)
        {
            st = false;
            return true;
        }

        bool Load()
        {
            return true;
        }

        void SendPacketToPlayers(WorldPacket const* data, Player* player) const
        {
            player->GetSession()->SendPacket(data);
        }

        void HandleOnHit()
        {
            if (st)
                return;

            if (auto caster = GetCaster()->GetOwner())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                    GetCaster()->PlayDistanceSound(RADIO, caster->ToPlayer());
                st = true;
            }
        }

    private:
        bool st;

        void Register()
        {
            OnHit += SpellHitFn(spell_radioSpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_radioSpellScript();
    }
};

uint32 const HotRodSavingPathSize1 = 25;
const G3D::Vector3 HotRodSavingPath1[HotRodSavingPathSize1] =
{
    { -8419.166f, 1335.844f, 102.48f },
    { -8400.743f, 1335.867f, 102.00f },
    { -8386.796f, 1346.295f, 102.04f },
    { -8375.958f, 1349.280f, 101.92f },
    { -8366.845f, 1347.834f, 100.73f },
    { -8356.833f, 1344.556f, 97.269f },
    { -8347.336f, 1340.407f, 91.547f },
    { -8336.646f, 1333.636f, 87.198f },
    { -8323.668f, 1324.263f, 87.844f },
    { -8312.716f, 1316.000f, 86.308f },
    { -8297.570f, 1307.516f, 78.293f },
    { -8285.078f, 1301.720f, 66.797f },
    { -8268.652f, 1296.309f, 54.460f },
    { -8244.688f, 1292.379f, 41.914f },
    { -8217.560f, 1290.675f, 32.779f },
    { -8188.618f, 1292.371f, 28.257f },
    { -8163.327f, 1296.255f, 24.468f },
    { -8140.116f, 1296.728f, 21.007f },
    { -8128.656f, 1302.450f, 19.691f },
    { -8125.476f, 1331.897f, 17.645f },
    { -8123.439f, 1361.889f, 20.013f },
    { -8120.380f, 1385.929f, 23.721f },
    { -8117.381f, 1407.449f, 20.311f },
    { -8112.809f, 1433.652f, 12.075f },
    { -8106.359f, 1461.755f, 10.059f }, // 24
};

uint32 const HotRodSavingPathSize2 = 27;
const G3D::Vector3 HotRodSavingPath2[HotRodSavingPathSize2] =
{
    { -8113.119f, 1471.103f, 10.112f },
    { -8122.227f, 1490.323f, 10.944f },
    { -8122.208f, 1503.192f, 10.396f },
    { -8117.462f, 1510.558f, 10.308f },
    { -8108.864f, 1516.237f, 9.7056f },
    { -8101.130f, 1533.864f, 9.3755f },
    { -8098.356f, 1578.926f, 8.8980f },
    { -8096.260f, 1630.549f, 8.9184f },
    { -8100.937f, 1689.483f, 15.027f },
    { -8110.045f, 1695.833f, 16.542f },
    { -8124.124f, 1708.939f, 26.371f },
    { -8136.201f, 1730.589f, 33.885f },
    { -8137.760f, 1749.137f, 35.077f },
    { -8132.282f, 1763.100f, 33.241f },
    { -8119.330f, 1779.162f, 28.901f },
    { -8108.613f, 1787.707f, 29.318f },
    { -8098.528f, 1800.007f, 32.382f },
    { -8084.839f, 1822.489f, 39.515f },
    { -8072.442f, 1823.315f, 42.396f },
    { -8050.656f, 1821.790f, 42.564f },
    { -8034.430f, 1822.313f, 39.825f },
    { -8009.152f, 1820.231f, 27.477f },
    { -7984.329f, 1824.981f, 17.194f },
    { -7963.231f, 1833.948f, 10.702f },
    { -7948.875f, 1834.749f, 6.6716f },
    { -7910.810f, 1833.957f, 5.2614f },
    { -7883.048f, 1833.121f, 4.2327f }, // 26
};

const Position HotRodPositions[] =
{
    {  -7849.27f, 1840.16f, 7.59f }, // jump
};

enum LifeSaving
{
    QUEST_LIFE_SAVING = 14126,

    SPELL_ROD_TURN = 70325,
    SPELL_PERIODIC_AURA_KNOCKBACK_1S = 70329,

    NPC_SASSY_HARDWRENCH_IN_VEH = 37680,
    NPC_WONDI_BUNNY_TARGET = 75042,
};

class npc_hot_rod__life_saving_vehicle : public CreatureScript
{
public:
    npc_hot_rod__life_saving_vehicle() : CreatureScript("npc_hot_rod__life_saving_vehicle") { }

    struct npc_hot_rod__life_saving_vehicleAI : public VehicleAI
    {
        npc_hot_rod__life_saving_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto sassy = me->SummonCreature(NPC_SASSY_HARDWRENCH_IN_VEH, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                sassy->EnterVehicle(me, 0);
                _sassyGUID = sassy->GetGUID();
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_LIFE_SAVING_1, 1500);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto sassy = Unit::GetCreature(*me, _sassyGUID))
                {
                    sassy->DespawnOrUnsummon(100);
                    me->DespawnOrUnsummon(100);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto sassy = Unit::GetCreature(*me, _sassyGUID))
            {
                if (_pathMode == 1)
                {
                    if (point == 24)
                    {
                        sassy->AI()->Talk(0);
                        _events.ScheduleEvent(EVENT_LIFE_SAVING_2, 500);
                    }
                }

                if (_pathMode == 2)
                {
                    if (point == 26)
                    {
                        me->RemoveAura(SPELL_PERIODIC_AURA_KNOCKBACK_1S);
                        me->GetMotionMaster()->MoveJump(HotRodPositions[0], 20.f, 20.f);
                        _events.ScheduleEvent(EVENT_LIFE_SAVING_3, 1800);
                        _pathMode = 0;
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LIFE_SAVING_1:
                    _pathMode = 1;
                    me->CastSpell(me, SPELL_PERIODIC_AURA_KNOCKBACK_1S);
                    me->GetMotionMaster()->MoveSmoothPath(HotRodSavingPath1, HotRodSavingPathSize1);
                    break;
                case EVENT_LIFE_SAVING_2:
                    if (auto sassy = Unit::GetCreature(*me, _sassyGUID))
                    {
                        me->GetMotionMaster()->Clear();
                        _pathMode++;
                        sassy->AI()->TalkWithDelay(1000, 1);
                        me->GetMotionMaster()->MoveSmoothPath(HotRodSavingPath2, HotRodSavingPathSize2);
                    }
                    break;
                case EVENT_LIFE_SAVING_3:
                    if (auto sassy = Unit::GetCreature(*me, _sassyGUID))
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            me->GetMotionMaster()->Clear();

                            if (auto facetarget = me->FindNearestCreature(NPC_WONDI_BUNNY_TARGET, 15.f))
                            {
                                me->CastSpell(facetarget, SPELL_ROD_TURN);
                                sassy->AI()->TalkWithDelay(1000, 2);
                                sassy->DespawnOrUnsummon(4600);
                                me->CastWithDelay(4500, player, SPELL_EJECT_ALL_PASSENGERS);
                                me->DespawnOrUnsummon(4650);
                            }
                        }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _pathMode = 0;
        uint64 _sassyGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hot_rod__life_saving_vehicleAI(creature);
    }
};

void AddSC_kezan()
{
    new npc_kezan_partygoer();
    new npc_hot_rod_controllable_vehicle();
    new npc_bilgewater_buccaneer_roughness();
    new npc_deathwing_scene_fourth_and_goal();
    new npc_mook_disguise_waltz();
    new spell_apply_waltz_mook_disguise();
    new spell_radio();
    new npc_hot_rod__life_saving_vehicle();
}
