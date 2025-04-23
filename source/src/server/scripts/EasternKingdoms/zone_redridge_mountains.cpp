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

/* Script Data Start
SDName: Redridge Mountains
SD%Complete: 100 (with SAI)
SDComment:
Script Data End */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Player.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum RedridgeEvents
{
    EVENT_BOARDED = 1,
    EVENT_TALK_1,
    EVENT_START_TRIP,
    EVENT_LAST_WAYPOINT,
    EVENT_RIDE_VEH,
    EVENT_START_INTRO,
    EVENT_REACH_INTRO_WP,
    EVENT_TRANSFORMED_TALK_1,
    EVENT_TRANSFORMED_TALK_2,
    EVENT_TRANSFORMED_TALK_3,
    EVENT_BACK_TO_FORM,
    EVENT_START_FIGHT,
    EVENT_LOW_HP,
    EVENT_BEFORE_CAST_BREATH,
    EVENT_CAST_BREATH,
    EVENT_KEESHAN_FIND,
    EVENT_KEESHAN_RIDE,
    EVENT_FLIGHT_WITH_KEESHAN,
    EVENT_FALL_WITH_KEESHAN,
    EVENT_DELAY_1,
    EVENT_DELAY_2,
    EVENT_SAY_TIP_1,
    EVENT_SAY_TIP_2,
    EVENT_SAY_TIP_3,
    EVENT_SAY_TIP_4,
    EVENT_SAY_TIP_5,
    EVENT_UPDATE_2ND_SPELL_BAR,
    EVENT_START,
    EVENT_EJECT,
    EVENT_TO_FADE,
    EVENT_DELAY_BETWEEN_SUMMONS,
    EVENT_DELAY_ENTER_VEH,
    EVENT_DELAY,
    EVENT_COLONEL_SAY,
    EVENT_HAXY_AREA_UPDATE,
    EVENT_MESSNER_BLAST_WAVE,
    EVENT_MESSNER_FIREBALL,
    EVENT_MESSNER_FLAMESTRIKE,
    EVENT_MESSNER_MOLTEN_ARMOR,
    EVENT_JORGENSEN_HOLY_LIGHT,
    EVENT_JORGENSEN_EXORCISM,
    EVENT_JORGENSEN_HOLY_SHOCK,
    EVENT_JORGENSEN_SEAL_OF_RIGHTEOUSNESS,
    EVENT_KRAKAUER_BLOODTHIRST,
    EVENT_KRAKAUER_REND,
    EVENT_KRAKAUER_SHATTERING_THROW,
    EVENT_KRAKAUER_SLAM,
    EVENT_KRAKAUER_BATTLE_SHOUT,
    EVENT_DANFORTH_SUNDER_ARMOR,
    EVENT_DANFORTH_CHARGE,
    EVENT_DANFORTH_SHOCKWAVE,
    EVENT_DANFORTH_THUNDERCLAP,

    ACTION_AAAAAA_CREDIT = 1,
    ACTION_DESPAWN_MACHINE,
};

// quest=26616/its-never-over by Wondi
enum RedridgeRiverboatEscortData
{   
    SAY_GROUP_0         = 0,
    SAY_GROUP_1         = 1,

    WAYPOINT_LAST_POINT = 5,

    NPC_KEESHAN         = 43458,
    NPC_MESSNER         = 43432,

    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_MESSNER_BOAT_ENGINE   = 81260,
    SPELL_QUEST_COMPLETE        = 81265,
    SPELL_EJECT_PASSENGER_6     = 64634,
    SPELL_PING                  = 98914,
    SPELL_FADE_TO_BLACK_1       = 88629,
    SPELL_FADE_TO_BLACK_3       = 89315,
    SPELL_FADE_TO_BLACK_2       = 89120,
    SPELL_FLIGHT_SPEED_100      = 49777,
    SPELL_UPDATE_ZONE_AURAS     = 93425,
};

uint32 const RiverboatVehiclePathSize = 6;
const G3D::Vector3 RiverboatVehiclePath[RiverboatVehiclePathSize] =
{
    { -9334.30f, -2390.35f, 55.7f }, // 0
    { -9381.73f, -2435.41f, 55.7f }, // 1
    { -9389.94f, -2583.57f, 55.7f }, // 2
    { -9426.63f, -2732.40f, 55.7f }, // 3
    { -9383.77f, -2816.56f, 55.7f }, // 4
    { -9420.81f, -2838.46f, 55.7f }, // 5
};

static Position PlayerJumpPos = { -9429.35f, -2837.39f, 57.68f };
static Position BoatPos = { -9301.05f, -2362.02f, 55.8f, 3.97413f };

class npc_keeshans_riverboat_vehicle : public CreatureScript
{
public:
    npc_keeshans_riverboat_vehicle() : CreatureScript("npc_keeshans_riverboat_vehicle") { }

    struct npc_keeshans_riverboat_vehicleAI : public VehicleAI
    {
        npc_keeshans_riverboat_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {          
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = passenger->ToPlayer())
                {
                    playerGUID = player->GetGUID();
                    me->GetMotionMaster()->MovePoint(1, BoatPos);
                    events.ScheduleEvent(EVENT_BOARDED, 3000);

                    if (auto messner = me->FindNearestCreature(10.f, NPC_MESSNER))
                        messner->UpdateOrientation(2.0f * M_PI);
                }
            }
            else
                return;
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto messner = me->GetVehicleKit()->GetPassenger(1)) {
                if (auto keeshan = me->GetVehicleKit()->GetPassenger(0)) {
                    switch (point)
                    {
                    case WAYPOINT_LAST_POINT:
                        keeshan->ToCreature()->AI()->Talk(SAY_GROUP_1);
                        messner->RemoveAura(SPELL_MESSNER_BOAT_ENGINE);
                        events.ScheduleEvent(EVENT_LAST_WAYPOINT, 3000);
                        if (auto player = Unit::GetPlayer(*me, playerGUID))
                        {
                            player->KilledMonsterCredit(43443);
                            me->CastSpell(me, SPELL_QUEST_COMPLETE);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BOARDED:
                    if (auto keeshan = me->GetVehicleKit()->GetPassenger(0)) // passenger seat_0 = kesshan in vehicle_template_accessory
                    {
                        keeshan->ToCreature()->AI()->Talk(SAY_GROUP_0);
                        events.ScheduleEvent(EVENT_TALK_1, 4000);
                        me->SetSpeed(MOVE_SWIM, 1.7f);
                    }
                    break;
                case EVENT_TALK_1:
                    if (auto messner = me->GetVehicleKit()->GetPassenger(1)) // passenger seat_1 = messner in vehicle_template_accessory
                    {
                        messner->ToCreature()->AI()->Talk(SAY_GROUP_0);
                        messner->CastSpell(messner, SPELL_MESSNER_BOAT_ENGINE);
                        events.ScheduleEvent(EVENT_START_TRIP, 3000);
                    }
                    break;
                case EVENT_START_TRIP:
                    me->GetMotionMaster()->MoveSmoothPath(RiverboatVehiclePath, RiverboatVehiclePathSize);
                    break;
                case EVENT_LAST_WAYPOINT:
                    me->CastSpell(me, SPELL_EJECT_PASSENGER_6);
                    me->DespawnOrUnsummon(1000);
                    if (auto player = Unit::GetPlayer(*me, playerGUID))
                        player->GetMotionMaster()->MoveJump(PlayerJumpPos, 12.0f, 14.0f);
                    if (auto pkeeshan = me->FindNearestCreature(NPC_KEESHAN, 60.0f, true))
                        pkeeshan->CastSpell(pkeeshan, SPELL_PING); // ping to start dialogue
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
        uint64 playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_keeshans_riverboat_vehicleAI(creature);
    }
};

enum RedridgeRiverboatData
{
    SPELL_DUMMY = 56685,
    SPELL_INVISIBILITY_DETECTION_3_MISC_9 = 60922,
    NPC_KEESHANS_RIVERBOAT_VEHICLE = 43450,
    SPELL_ROOT = 42716,
};

class npc_keeshans_riverboat_spellclick : public CreatureScript
{
public:
    npc_keeshans_riverboat_spellclick() : CreatureScript("npc_keeshans_riverboat_spellclick") { }

    struct npc_keeshans_riverboat_spellclickAI : public ScriptedAI
    {
        npc_keeshans_riverboat_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_FADE_TO_BLACK_3);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                    events.ScheduleEvent(EVENT_DELAY_1, 1600);
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DELAY_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto boat = player->SummonCreature(NPC_KEESHANS_RIVERBOAT_VEHICLE, -9295.115f, -2355.662f, 56.647f, 3.9f, TEMPSUMMON_DEAD_DESPAWN, 5000))
                        {
                            player->RemoveAura(SPELL_INVISIBILITY_DETECTION_3_MISC_9);
                            player->CastWithDelay(700, boat, 46598);
                        }
                    events.ScheduleEvent(EVENT_DELAY_2, 10000);
                    break;
                case EVENT_DELAY_2:
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_keeshans_riverboat_spellclickAI(creature);
    }
};


enum RedridgeATJorgensen
{
    QUEST_PRISONERS_OF_WAR = 26646,
    QUEST_TO_WIN_A_WAR = 26651,

    NPC_JORGENSEN = 43546,

    // quest=26646/prisoners-of-war
    SPELL_PING_JORGENSEN_1 = 151287,
    SPELL_PING_JORGENSEN_2 = 151288,
    SPELL_PING_JORGENSEN_3 = 151289,
    SPELL_PING_JORGENSEN_4 = 151290,
    // for quest=26651/to-win-a-war-you-gotta-become-war
    SPELL_PING_JORGENSEN_5 = 151292,
    SPELL_PING_JORGENSEN_6 = 151293,
    SPELL_PING_JORGENSEN_7 = 151295,
    SPELL_PING_JORGENSEN_8 = 151294,

    SPELL_BRAVO_COMPANY_FIELD_KIT_1 = 82580,
    SPELL_BRAVO_COMPANY_FIELD_KIT_2 = 82587,

    SPELL_SUMMON_AURA               = 81447,
};

class npc_jorgensen_43546 : public CreatureScript
{
public:
    npc_jorgensen_43546() : CreatureScript("npc_jorgensen_43546") { }

    struct npc_jorgensen_43546AI : public npc_escortAI
    {
        npc_jorgensen_43546AI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                _PhaseTalk_2 = true;
                if (player->GetQuestStatus(QUEST_PRISONERS_OF_WAR) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_PRISONERS_OF_WAR) == QUEST_STATUS_REWARDED)
                {
                    _IsPrisonerQuestComleted = true;
                }
            }
        }
        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_JORGENSEN_1 && !_RunSpellItem)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (who->GetGUID() == _playerGUID)
                    {
                        me->AI()->Talk(0, _playerGUID);
                        _RunSpellItem = true;
                    }
            }
            if (_ReachedATFirst == false && _IsPrisonerQuestComleted == false)
            {
                if (spellInfo->Id == SPELL_PING_JORGENSEN_2)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (who->GetGUID() == _playerGUID)
                        {
                            me->AI()->Talk(1, _playerGUID);
                            _ReachedATFirst = true;
                        }
            }
            if (_ReachedATSecond == false && _IsPrisonerQuestComleted == false)
            {
                if (spellInfo->Id == SPELL_PING_JORGENSEN_3)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (who->GetGUID() == _playerGUID)
                        {
                            _PhaseTalk_2 = false;
                            _ReachedATSecond = true;
                            me->AI()->Talk(2, _playerGUID);
                            events.ScheduleEvent(EVENT_SAY_TIP_1, 8000);
                        }
            }
            if (_ReachedATThird == false && _IsPrisonerQuestComleted == false)
            {
                if (spellInfo->Id == SPELL_PING_JORGENSEN_4)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (who->GetGUID() == _playerGUID)
                        {
                            me->AI()->Talk(5, _playerGUID);
                            _ReachedATThird = true;
                        }
            }            
            if (_IsPrisonerQuestComleted)
            {
                if (spellInfo->Id == SPELL_PING_JORGENSEN_5)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (who->GetGUID() == _playerGUID)
                        {
                            me->AI()->Talk(11, _playerGUID);
                        }
            }
            if (_IsPrisonerQuestComleted)
            {
                if (spellInfo->Id == SPELL_PING_JORGENSEN_6)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (who->GetGUID() == _playerGUID)
                        {
                            me->AI()->Talk(12, _playerGUID);
                        }
            }
            if (spellInfo->Id == SPELL_PING_JORGENSEN_8)
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (who->GetGUID() == _playerGUID)
                    {
                        player->RemoveAura(SPELL_SUMMON_AURA);
                        me->DespawnOrUnsummon();
                    }
        }
        void UpdateAI(uint32 const diff)
        {
            if (_PhaseTalk_2 && (_PhaseTalk_2_TIMER <= diff))
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    _PhaseTalk_2_TIMER = 100000;
                    me->AI()->Talk(10, _playerGUID);
                }
            }
            else _PhaseTalk_2_TIMER -= diff;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SAY_TIP_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(3, _playerGUID);
                        events.ScheduleEvent(EVENT_SAY_TIP_2, 9000);
                        _PhaseTalk_1 = true;

                    }
                    break;
                case EVENT_SAY_TIP_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(6, _playerGUID);
                        events.ScheduleEvent(EVENT_SAY_TIP_3, 20000);
                    }
                    break;
                case EVENT_SAY_TIP_3:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(7, _playerGUID);
                        events.ScheduleEvent(EVENT_SAY_TIP_4, 25000);
                    }
                    break;
                case EVENT_SAY_TIP_4:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(8, _playerGUID);
                        events.ScheduleEvent(EVENT_SAY_TIP_5, 25000);
                    }
                    break;
                case EVENT_SAY_TIP_5:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(9, _playerGUID);
                        _PhaseTalk_1 = false;
                        _PhaseTalk_2 = true;
                    }
                    break;
                case EVENT_UPDATE_2ND_SPELL_BAR:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->RemoveAura(SPELL_BRAVO_COMPANY_FIELD_KIT_2);
                        player->CastSpell(player, SPELL_UPDATE_ZONE_AURAS);
                        player->CastWithDelay(500, player, SPELL_BRAVO_COMPANY_FIELD_KIT_2);
                    }
                    break;
                default:
                    break;
                }
            }

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (player->GetQuestStatus(QUEST_PRISONERS_OF_WAR) == QUEST_STATUS_REWARDED && player->HasAura(SPELL_BRAVO_COMPANY_FIELD_KIT_1) && !player->HasAura(SPELL_BRAVO_COMPANY_FIELD_KIT_2))
                {
                    player->RemoveAura(SPELL_BRAVO_COMPANY_FIELD_KIT_1);
                    player->CastWithDelay(100, player, SPELL_BRAVO_COMPANY_FIELD_KIT_2);
                    events.ScheduleEvent(EVENT_UPDATE_2ND_SPELL_BAR, 500);
                    _IsPrisonerQuestComleted = true;
                }
                
                if (player->GetQuestStatus(QUEST_TO_WIN_A_WAR) == QUEST_STATUS_REWARDED)
                {
                    player->RemoveAura(SPELL_BRAVO_COMPANY_FIELD_KIT_2);
                    me->DespawnOrUnsummon();
                }
            }

        };
    private:
        EventMap events;
        uint64 _playerGUID = 0;

        bool _RunSpellItem = false;
        bool _IsPrisonerQuestComleted = false;
        bool _PhaseTalk_1 = false;
        bool _PhaseTalk_2 = false;
        uint32 _PhaseTalk_2_TIMER = 5000;

        bool _ReachedATFirst = false;
        bool _ReachedATSecond = false;
        bool _ReachedATThird = false;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jorgensen_43546AI(creature);
    }
};

enum
{
    SPELL_PING_ENEMIES = 151291,
};

class npc_wild_rat_43518 : public CreatureScript
{
public:
    npc_wild_rat_43518() : CreatureScript("npc_wild_rat_43518") { }

    struct npc_wild_rat_43518AI : public ScriptedAI
    {
        npc_wild_rat_43518AI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_PING_ENEMIES);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new  npc_wild_rat_43518AI(creature);
    }
};

// 82580
class spell_bravo_company_field_kit : public SpellScriptLoader
{
public:
    spell_bravo_company_field_kit() : SpellScriptLoader("spell_bravo_company_field_kit") { }

    class spell_bravo_company_field_kit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bravo_company_field_kit_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto caster = GetCaster())
                caster->CastSpell(caster, SPELL_PING_JORGENSEN_1);
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_bravo_company_field_kit_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_bravo_company_field_kit_SpellScript();
    }
};

// 82577
class spell_prison_camouflaged : public SpellScriptLoader
{
public:
    spell_prison_camouflaged() : SpellScriptLoader("spell_prison_camouflaged") { }

    class spell_prison_camouflaged_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_prison_camouflaged_AuraScript);

        bool CheckAreaTarget(Unit* target)
        {
            if (target->GetEntry() == NPC_JORGENSEN || target->GetTypeId() == TYPEID_PLAYER)
                return true;
            return false;
        }
        void Register()
        {
            DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_prison_camouflaged_AuraScript::CheckAreaTarget);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_prison_camouflaged_AuraScript();
    }
};

// quest 26668 detonation
uint32 const DetonationCameraPathSize = 6;
const G3D::Vector3 DetonationCameraPath[DetonationCameraPathSize] =
{
    { -9639.500f, -3197.64f, 117.17f, }, // 0
    { -9679.093f, -3121.72f, 155.63f, }, // 1
    { -9784.115f, -3095.63f, 156.91f, }, // 2
    { -9836.242f, -3199.68f, 150.32f, }, // 3
    { -9753.767f, -3277.31f, 146.93f, }, // 4
    { -9655.07f, -3193.71f,  135.69f },  // 5
};

enum DetonationData
{
    QUEST_DETONATION = 26668,

    REDRIDGE_MOUNTAIN_ZONEID = 44,
    RENDERS_VALLEY_AREAID = 997,

    NPC_NUKE_TRIGGER = 74975,

    SPELL_SUMMON_CAMERA = 81607,
    SPELL_EJECT_PASSENGER = 60603,
    SPELL_PING_EXPLODE_TRIGGER = 151296,
    SPELL_TELEPORT_TO_CAMP = 81621,

    SPELL_FLIGHT_SPEED_300 = 54422,
    SPELL_FLIGHT_SPEED_280 = 52211,
    SPELL_FLIGHT_SPEED_180 = 48602,

    NPC_KILL_CREDIT = 74975,
};

static Position TeleportToSummon = { -9661.5f, -3250.5f, 129.2f, 2.5f };
static Position TeleportToCamp = { -9629.74f, -3477.67f, 121.95f, 2.69f };

class npc_detonation_camera : public CreatureScript
{
public:
    npc_detonation_camera() : CreatureScript("npc_detonation_camera") { }

    struct npc_detonation_cameraAI : public npc_escortAI
    {
        npc_detonation_cameraAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            me->SetCanFly(true);
            me->SetDisableGravity(true);
            me->SetSpeed(MOVE_FLIGHT, 1.0f);
            me->AI()->DoCast(SPELL_FLIGHT_SPEED_280);
            me->NearTeleportTo(TeleportToSummon, false);
        }
        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER && !_IsCinematicInProgress)
            {
                me->CastSpell(me, SPELL_PING_EXPLODE_TRIGGER);
                events.ScheduleEvent(EVENT_START, 1000);
            }
            else if (!apply && passenger->GetTypeId() == TYPEID_PLAYER && _IsCinematicInProgress)
            {
                events.ScheduleEvent(EVENT_TO_FADE, 0);
            }
        }
        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto passenger = me->GetVehicleKit()->GetPassenger(0)) {
                switch (point)
                {
                case 5:
                    _IsCinematicInProgress = false;
                    me->SummonCreature(NPC_NUKE_TRIGGER, -9738.18f, -3184.05f, 59.59f, 3.35f, TEMPSUMMON_DEAD_DESPAWN, 0);
                    events.ScheduleEvent(EVENT_TO_FADE, 2500);
                    break;
                default:
                    break;
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_START:

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->RemoveAura(SPELL_FADE_TO_BLACK_1);
                    me->GetMotionMaster()->MoveSmoothPath(DetonationCameraPath, DetonationCameraPathSize);
                    _IsCinematicInProgress = true;
                    break;
                case EVENT_TO_FADE:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->CastSpell(player, SPELL_FADE_TO_BLACK_2);
                        events.ScheduleEvent(EVENT_EJECT, 500);
                    }
                case EVENT_EJECT:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastWithDelay(200, player, SPELL_TELEPORT_TO_CAMP);
                        me->CastSpell(me, SPELL_EJECT_PASSENGER);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
        uint64 _playerGUID = 0;
        bool _IsCinematicInProgress = false;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_detonation_cameraAI(creature);
    }
};

enum FallingTankData
{
    NPC_BRAVO_COMPANY_SIEGE_TANK = 43714,
    NPC_COLONEL = 43728,

    SPELL_PARACHUTE         = 81793,
    SPELL_INVIS_DETECTION_3 = 60922,
    SPELL_RIDE_VEHICLE      = 46598,

    SPELL_DUMMY_SPELLCLICK  = 56685,

    SPELL_SUMMON_BRAVO_COMPANY_SIEGE_TANK = 81808,
};

static Position TankMovePoint = { -9647.009f, -3461.22f, 118.81f };
static Position ColonelJumpPosition = { -9640.82f, -3469.34f, 121.11f };

class npc_bravo_company_siege_tank : public CreatureScript
{
public:
    npc_bravo_company_siege_tank() : CreatureScript("npc_bravo_company_siege_tank") { }

    struct npc_bravo_company_siege_tankAI : public ScriptedAI
    {
        npc_bravo_company_siege_tankAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetPhaseMask(128, true);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            if (auto colonel = me->SummonCreature(NPC_COLONEL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000))
            {
                colonel->CastSpell(me, SPELL_RIDE_VEHICLE);
                _colonelGUID = colonel->GetGUID();
            }

            me->CastSpell(me, SPELL_PARACHUTE);
            me->GetMotionMaster()->MovePoint(1, TankMovePoint);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                me->RemoveAura(SPELL_PARACHUTE);
                events.ScheduleEvent(EVENT_DELAY, 4000);
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {

            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
                if (!who->HasAura(SPELL_SUMMON_BRAVO_COMPANY_SIEGE_TANK))
                {
                    who->CastSpell(who, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    who->CastSpell(who, SPELL_SUMMON_BRAVO_COMPANY_SIEGE_TANK);
                }
        }
        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DELAY:
                    if (auto colonel = me->GetVehicleKit()->GetPassenger(0))
                    {
                        me->CastSpell(me, SPELL_EJECT_PASSENGER);
                        colonel->GetMotionMaster()->MoveJump(ColonelJumpPosition, 12.0f, 15.0f);
                        events.ScheduleEvent(EVENT_COLONEL_SAY, 2000);
                    }
                    break;
                case EVENT_COLONEL_SAY:
                    if (auto colonel = Unit::GetCreature(*me, _colonelGUID))
                    {
                        colonel->AI()->Talk(0);
                        colonel->DespawnOrUnsummon(5000);
                        me->DespawnOrUnsummon(5000);

                        events.ScheduleEvent(EVENT_HAXY_AREA_UPDATE, 4500);
                    }
                    break;
                case EVENT_HAXY_AREA_UPDATE:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->GetPosition(_x, _y, _z, _o);
                        // haxy area update
                        player->NearTeleportTo(-9625.8203f, -3397.94140f, 88.9748f, 4.456795f);
                        player->NearTeleportTo(_x, _y, _z, _o);
                    }
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
        uint64 _colonelGUID = 0;
        uint64 _playerGUID = 0;
        float _x, _y, _z, _o;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bravo_company_siege_tankAI(creature);
    }
};

static Position SceneTankPos = { -9644.66f, -3462.24f, 197.91f, 5.29f };

// custom summon spell for scene
class spell_summon_scene_tank : public SpellScriptLoader
{
public:
    spell_summon_scene_tank() : SpellScriptLoader("spell_summon_scene_tank") { }

    class spell_summon_scene_tank_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_scene_tank_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto caster = GetCaster())
            {
                caster->SummonCreature(NPC_BRAVO_COMPANY_SIEGE_TANK, SceneTankPos, TEMPSUMMON_TIMED_DESPAWN, 30000);
            }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_summon_scene_tank_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_summon_scene_tank_SpellScript();
    }
};

class detonation_redridge_playerscript : public PlayerScript
{
public:
    detonation_redridge_playerscript() : PlayerScript("detonation_redridge_playerscript") { }

    Position const TeleportPC = { -9698.73f, -3293.98f, 117.81f, 1.367f };

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newZone == REDRIDGE_MOUNTAIN_ZONEID && newArea == RENDERS_VALLEY_AREAID && player->GetQuestStatus(QUEST_DETONATION) == QUEST_STATUS_INCOMPLETE)
        {
            player->KilledMonsterCredit(NPC_KILL_CREDIT);
            player->AreaExploredOrEventHappens(QUEST_DETONATION);
            player->NearTeleportTo(TeleportPC, false);
            player->CastWithDelay(500, player, SPELL_SUMMON_CAMERA);
        }
        else
            return;
    }
};

enum VehicleTankData
{
    NPC_BRAVO_COMPANY_SIEGE_TANK_VEH = 43734,
    NPC_MACHINE_GUN = 43745,
    NPC_KEESHAN_ON_VEH = 43744,
    NPC_BLACKROCK_INVADER = 43775,

    SPELL_DUMMY_PING = 98918,
    SPELL_DUMMY_PING_2 = 98917,
    SPELL_DUMMY_PING_ORIENTATION = 98916,
    SPELL_KILL_CREDIT = 81878,
};

class npc_bravo_company_siege_tank_vehicle : public CreatureScript
{
public:
    npc_bravo_company_siege_tank_vehicle() : CreatureScript("npc_bravo_company_siege_tank_vehicle") { }

    struct npc_bravo_company_siege_tank_vehicleAI : public VehicleAI
    {
        npc_bravo_company_siege_tank_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetPhaseMask(512, true);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->RemoveAura(60922);
            }

            if (auto gun = me->SummonCreature(NPC_MACHINE_GUN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                _machineGUID = gun->GetGUID();
                gun->EnterVehicle(me, 1);
            }
        }

        void DoAction(int32 const actionId) override
        {
            if (actionId == ACTION_AAAAAA_CREDIT)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->CastSpell(player, SPELL_KILL_CREDIT);
            }

        }
        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (!apply)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->RemoveAura(SPELL_SUMMON_BRAVO_COMPANY_SIEGE_TANK);
                    player->RemoveAura(SPELL_RIDE_VEHICLE);
                }

                if (auto gun = Unit::GetCreature(*me, _machineGUID))
                {
                    gun->AI()->DoAction(ACTION_DESPAWN_MACHINE);
                    me->DespawnOrUnsummon(100);
                }

                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                me->DespawnOrUnsummon(1000);
            }
        }
        void JustDied(Unit* killer)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                player->RemoveAura(SPELL_SUMMON_BRAVO_COMPANY_SIEGE_TANK);
                player->RemoveAura(SPELL_RIDE_VEHICLE);
            }

            if (auto gun = Unit::GetCreature(*me, _machineGUID))
            {
                gun->AI()->DoAction(ACTION_DESPAWN_MACHINE);
            }

            me->DespawnOrUnsummon(1000);
        }
    private:
        EventMap events;
        uint64 _keeshanGUID = 0;
        uint64 _machineGUID = 0;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bravo_company_siege_tank_vehicleAI(creature);
    }
};

enum MachineGun
{
    SPELL_MACHINE_GUN = 81874,
};

class npc_bravo_company_machine_gun : public CreatureScript
{
public:
    npc_bravo_company_machine_gun() : CreatureScript("npc_bravo_company_machine_gun") { }

    struct npc_bravo_company_machine_gunAI : public VehicleAI
    {
        npc_bravo_company_machine_gunAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetPhaseMask(512, true);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            events.ScheduleEvent(EVENT_DELAY_ENTER_VEH, 1000);
            _tankGUID = summoner->GetGUID();
        }

        void DoAction(int32 const actionId)
        {
            if (actionId == ACTION_DESPAWN_MACHINE)
            {
                if (auto keeshan = Unit::GetCreature(*me, _keeshanGUID))
                {
                    keeshan->DespawnOrUnsummon();
                    me->DespawnOrUnsummon(100);
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto tank = Unit::GetCreature(*me, _tankGUID))
                {
                    tank->AI()->DoAction(ACTION_AAAAAA_CREDIT);
                    _randomYell = urand(1, 10);
                    switch (_randomYell)
                    {
                    case 5:
                        if (auto keeshan = Unit::GetCreature(*me, _keeshanGUID))
                            keeshan->AI()->Talk(0);
                        break;
                    default:
                        break;
                    }
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_ORIENTATION)
                me->SetFacingToObject(who);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DELAY_ENTER_VEH:
                    if (auto keeshan = me->SummonCreature(NPC_KEESHAN_ON_VEH, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        keeshan->CastSpell(me, SPELL_RIDE_VEHICLE);
                        _keeshanGUID = keeshan->GetGUID();
                    }
                    break;
                default:
                    break;
                }
            }
            if (me->FindNearestCreature(NPC_BLACKROCK_INVADER, 50.0f) && !me->HasAura(SPELL_MACHINE_GUN))
                me->CastSpell(me, SPELL_MACHINE_GUN);
            else if (!me->FindNearestCreature(NPC_BLACKROCK_INVADER, 50.0f) && me->HasAura(SPELL_MACHINE_GUN))
                me->RemoveAura(SPELL_MACHINE_GUN);
            else
                return;
        };
    private:
        EventMap events;
        uint64 _keeshanGUID = 0;
        uint64 _tankGUID = 0;
        bool _questCompleted = false;
        uint8 _randomYell = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bravo_company_machine_gunAI(creature);
    }
};


class npc_spawn_more_blackrock_invaders : public CreatureScript
{
public:
    npc_spawn_more_blackrock_invaders() : CreatureScript("npc_spawn_more_blackrock_invaders") { }

    struct npc_spawn_more_blackrock_invadersAI : public ScriptedAI
    {
        npc_spawn_more_blackrock_invadersAI(Creature* creature) : ScriptedAI(creature) {}

        void UpdateAI(const uint32 diff)
        {
            if (me->FindNearestCreature(NPC_BRAVO_COMPANY_SIEGE_TANK_VEH, 60.0f) && !me->FindNearestCreature(NPC_BLACKROCK_INVADER, 60.0f) && !_spawned)
            {
                _spawned = true;
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 4, me->GetPositionY() + 2, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 8, me->GetPositionY() + 4, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + -4, me->GetPositionY() - 2, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + -8, me->GetPositionY() - 4, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 8, me->GetPositionY() - 4, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 12, me->GetPositionY() + 4, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 4, me->GetPositionY() + 12, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 12, me->GetPositionY() + 14, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + -6, me->GetPositionY() - 12, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + -21, me->GetPositionY() - 14, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 14, me->GetPositionY() - 14, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                me->SummonCreature(NPC_BLACKROCK_INVADER, me->GetPositionX() + 9, me->GetPositionY() + 12, me->GetPositionZ() + 0.2f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 60000);
                events.ScheduleEvent(EVENT_DELAY_BETWEEN_SUMMONS, 65000);
            }
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DELAY_BETWEEN_SUMMONS:
                    _spawned = false;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
        bool _spawned = false;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spawn_more_blackrock_invadersAI(creature);
    }
};

uint32 const DarkblazePathSize1 = 2;
const G3D::Vector3 DarkblazePath1[DarkblazePathSize1] =
{
    { -9483.115f, -2944.332f, 147.563f, }, // 0
    { -9465.605f, -2978.729f, 128.456f, }, // 1
};

uint32 const DarkblazePathSize2 = 3;
const G3D::Vector3 DarkblazePath2[DarkblazePathSize2] =
{
    { -9453.202f, -2975.67f, 140.841f, }, // 0
    { -9408.54f, -2920.922f, 145.85f, },  // 1
    { -9390.761f, -2871.87f, 120.373f, }, // 2
};

enum DarkblazeData
{
    NPC_KEESHAN_DARKBLADE = 43812,

    SPELL_THREATEN              = 53799,
    SPELL_DARKBLAZE_BREATH      = 82069, // duration 10s
    SPELL_DARKBLAZE_TRANSFORM   = 82067,
    SPELL_MOUNT_DARKBLAZE       = 82098,
    SPELL_SANCTUARY             = 88467,
    SPELL_FORCE_FLYING          = 83412,
    SPELL_FLYING_SPEED_180      = 48602,

    QUEST_SHOWDOWN_AT_STONEWATCH = 26713,
    QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER = 26714,
};
static Position DarkblazeHomePos = { -9466.980469f, -2976.833984f, 127.975487f, 5.069747f };

class npc_darkblaze : public CreatureScript
{
public:
    npc_darkblaze() : CreatureScript("npc_darkblaze") { }

    struct npc_darkblazeAI : public ScriptedAI
    {
        npc_darkblazeAI(Creature* creature) : ScriptedAI(creature) {}

        void EnterEvadeMode()
        {
            if (_fightInProgress)
                me->DespawnOrUnsummon();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;
            if ((me->GetHealthPct() <= 5) && _fightInProgress)
            {
                _fightInProgress = false;
                events.ScheduleEvent(EVENT_LOW_HP, 0);
            }
        }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetCanFly(true);
            me->SetDisableGravity(true);
            _introFlight = true;
            me->HandleEmoteCommand(EMOTE_STATE_FLYFALL);
            me->SetPhaseMask(4096, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            events.ScheduleEvent(EVENT_START_INTRO, 0);
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_introFlight == true && point == 1)
                events.ScheduleEvent(EVENT_REACH_INTRO_WP, 0);

            if (_introFlight == false && point == 2)
                events.ScheduleEvent(EVENT_FALL_WITH_KEESHAN, 0);
        }
        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_START_INTRO:
                    me->GetMotionMaster()->MoveSmoothPath(DarkblazePath1, DarkblazePathSize1);
                    break;
                case EVENT_REACH_INTRO_WP:
                    _introFlight = false;
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                    events.ScheduleEvent(EVENT_TRANSFORMED_TALK_1, 4000);
                    break;
                case EVENT_TRANSFORMED_TALK_1:
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    me->CastSpell(me, SPELL_DARKBLAZE_TRANSFORM);
                    me->AI()->Talk(4);
                    events.ScheduleEvent(EVENT_TRANSFORMED_TALK_2, 6000);
                    break;
                case EVENT_TRANSFORMED_TALK_2:
                    me->AI()->Talk(5);
                    events.ScheduleEvent(EVENT_TRANSFORMED_TALK_3, 7500);
                    break;
                case EVENT_TRANSFORMED_TALK_3:
                    me->AI()->Talk(6);
                    events.ScheduleEvent(EVENT_BACK_TO_FORM, 8500);
                    break;
                case EVENT_BACK_TO_FORM:
                    me->RemoveAura(SPELL_DARKBLAZE_TRANSFORM);
                    me->SetHomePosition(DarkblazeHomePos);
                    events.ScheduleEvent(EVENT_START_FIGHT, 3000);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    break;
                case EVENT_START_FIGHT:
                    _fightInProgress = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->AI()->Talk(0);
                    me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 0);
                    me->canWalk();

                    if (auto player = me->FindNearestPlayer(35))
                        player->CastSpell(me, 98442);

                    break;
                case EVENT_LOW_HP:
                    me->SetHomePosition(DarkblazeHomePos);
                    me->AI()->Talk(1);
                    events.ScheduleEvent(EVENT_BEFORE_CAST_BREATH, 2000);
                    break;
                case EVENT_BEFORE_CAST_BREATH:
                    me->SetReactState(REACT_PASSIVE);
                    events.ScheduleEvent(EVENT_CAST_BREATH, 0);
                    break;
                case EVENT_CAST_BREATH:
                    me->AI()->Talk(2);
                    me->CastSpell(me, SPELL_DARKBLAZE_BREATH);
                    events.ScheduleEvent(EVENT_KEESHAN_FIND, 12000);
                    break;
                case EVENT_KEESHAN_FIND:
                    if (!me->FindNearestCreature(NPC_KEESHAN_DARKBLADE, 35.0f))
                    {
                        if (auto keeshan = me->SummonCreature(NPC_KEESHAN_DARKBLADE, me->GetPositionX() + 3, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 40000))
                        {
                            keeshan->CastSpell(me, SPELL_MOUNT_DARKBLAZE);
                            keeshan->AI()->Talk(0);
                            events.ScheduleEvent(EVENT_KEESHAN_RIDE, 2000);
                        }
                    }
                    else
                    {
                        if (auto keeshan = me->FindNearestCreature(NPC_KEESHAN_DARKBLADE, 35.0f))
                        {
                            keeshan->CastSpell(me, SPELL_MOUNT_DARKBLAZE);
                            keeshan->AI()->Talk(0);
                            events.ScheduleEvent(EVENT_KEESHAN_RIDE, 2000);
                        }
                    }
                    me->CastSpell(me, SPELL_FLYING_SPEED_180);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                    me->CombatStop();
                    me->SetRooted(true);
                    me->CastWithDelay(2000, me, SPELL_FORCE_FLYING);
                    break;
                case EVENT_KEESHAN_RIDE:
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                    std::list<Player*> player = me->GetPlayersInRange(50, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_INCOMPLETE)
                        {
                            (*it)->CastSpell(*it, SPELL_SANCTUARY);
                            (*it)->KilledMonsterCredit(43496);
                        }

                    me->AI()->Talk(3);
                    me->CastSpell(me, SPELL_FORCE_FLYING);
                    me->HandleEmoteCommand(EMOTE_STATE_FLY_FALL);
                    me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10);
                    events.ScheduleEvent(EVENT_FLIGHT_WITH_KEESHAN, 4000);
                    break;
                }
                case EVENT_FLIGHT_WITH_KEESHAN:
                    me->GetMotionMaster()->MoveSmoothPath(DarkblazePath2, DarkblazePathSize2);
                    break;
                case EVENT_FALL_WITH_KEESHAN:
                    if (auto keeshan = me->FindNearestCreature(NPC_KEESHAN_DARKBLADE, 15))
                        keeshan->DespawnOrUnsummon();
                    me->Kill(me);
                    break;
                default:
                    break;
                }
            }
            if (_fightInProgress)
                DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        bool _introFlight = false;
        bool _fightInProgress = false;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkblazeAI(creature);
    }
};

enum BravoCompanyTeam
{
    NPC_MESSNER_UNGEARED = 43300,
    NPC_JORGENSEN_UNGEARED = 43305,
    NPC_KRAKAUER_UNGEARED = 43303,
    NPC_DANFORTH_UNGEARED = 43302,

    NPC_KEESHAN_GEARED = 43812,
    NPC_MESSNER_GEARED = 43826,
    NPC_JORGENSEN_GEARED = 43827,
    NPC_DANFORTH_GEARED = 43828,
    NPC_KRAKAUER_GEARED = 43829,
    NPC_DARKBLAZE = 43496,

    SPELL_MESSNER_BLAST_WAVE = 81996,
    SPELL_MESSNER_FIREBALL = 79854,
    SPELL_MESSNER_FLAMESTRIKE = 81995,
    SPELL_MESSNER_MOLTEN_ARMOR = 79849,
    SPELL_JORGENSEN_HOLY_LIGHT = 81452,
    SPELL_JORGENSEN_EXORCISM = 81456,
    SPELL_JORGENSEN_HOLY_SHOCK = 81453,
    SPELL_JORGENSEN_SEAL_OF_RIGHTEOUSNESS = 81454,
    SPELL_KRAKAUER_BLOODTHIRST = 79878,
    SPELL_KRAKAUER_REND = 11977,
    SPELL_KRAKAUER_SHATTERING_THROW = 79883,
    SPELL_KRAKAUER_SLAM = 79881,
    SPELL_KRAKAUER_BATTLE_SHOUT = 32064,
    SPELL_DANFORTH_SUNDER_ARMOR = 11971,
    SPELL_DANFORTH_CHARGE = 22120,
    SPELL_DANFORTH_SHOCKWAVE = 79872,
    SPELL_DANFORTH_THUNDERCLAP = 8078,
    SPELL_KESSHAN_SHOUT = 82061,

    // Breaking Out is Hard to Do / Return of the Bravo Company - ends
    SPELL_SUMMON_MESSNER_UNGEARED = 80893,
    SPELL_SUMMON_JORGENSEN_UNGEARED = 80940,
    SPELL_SUMMON_KRAKAUER_UNGEARED = 80941,
    SPELL_SUMMON_DANFORTH_UNGEARED = 80943,

    // Showdown at Stonewatch
    SPELL_SUMMON_KEESHAN_GEARED = 82002,
    SPELL_SUMMON_MESSNER_GEARED = 82004,
    SPELL_SUMMON_JORGENSEN_GEARED = 82005,
    SPELL_SUMMON_DANFORTH_GEARED = 82007,
    SPELL_SUMMON_KRAKAUER_GEARED = 82008,

    QUEST_RETURN_OF_THE_BRAVO_COMPANY = 26563,
    QUEST_BREAKING_OUT_IS_HARD_TO_DO = 26587,
    QUEST_JORGENSEN = 26560,
    QUEST_KRAKAUER = 26561,
    QUEST_AND_LAST_BUT_NOT_LEAST_DANFORTH = 26562,

};

// ######################### BravoCompanyTeamUngeared #########################
class npc_bravo_company_team_redridge : public CreatureScript
{
public:
    npc_bravo_company_team_redridge() : CreatureScript("npc_bravo_company_team_redridge") { }

    struct npc_bravo_company_team_redridgeAI : public ScriptedAI
    {
        npc_bravo_company_team_redridgeAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);

            if (me->GetEntry() == NPC_KEESHAN_GEARED)
            {
                if (damage >= me->GetHealth())
                    damage = 0;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                switch (me->GetEntry())
                {
                case NPC_MESSNER_GEARED:
                case NPC_MESSNER_UNGEARED:
                    _events.ScheduleEvent(EVENT_MESSNER_BLAST_WAVE, urand(4000, 15000));
                    _events.ScheduleEvent(EVENT_MESSNER_FIREBALL, urand(2000, 5000));
                    _events.ScheduleEvent(EVENT_MESSNER_FLAMESTRIKE, urand(8000, 18000));

                    if (!me->GetAura(SPELL_MESSNER_MOLTEN_ARMOR))
                        me->CastSpell(me, SPELL_MESSNER_MOLTEN_ARMOR);
                    break;
                case NPC_JORGENSEN_GEARED:
                case NPC_JORGENSEN_UNGEARED:
                    _events.ScheduleEvent(EVENT_JORGENSEN_EXORCISM, urand(1000, 15000));
                    _events.ScheduleEvent(EVENT_JORGENSEN_HOLY_SHOCK, urand(7000, 15000));

                    if (!me->GetAura(SPELL_JORGENSEN_SEAL_OF_RIGHTEOUSNESS))
                        me->CastSpell(me, SPELL_JORGENSEN_SEAL_OF_RIGHTEOUSNESS);
                    break;
                case NPC_KRAKAUER_GEARED:
                case NPC_KRAKAUER_UNGEARED:
                    _events.ScheduleEvent(EVENT_KRAKAUER_BLOODTHIRST, urand(2000, 8000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_REND, urand(2000, 15000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_SHATTERING_THROW, urand(1000, 12000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_SLAM, urand(5000, 15000));

                    if (me->getVictim()->GetEntry() == NPC_DARKBLAZE)
                    {
                        if (auto v = me->getVictim()) me->CastSpell(v, SPELL_THREATEN);
                    }
                    break;
                case NPC_DANFORTH_GEARED:
                case NPC_DANFORTH_UNGEARED:
                    _events.ScheduleEvent(EVENT_DANFORTH_CHARGE, urand(200, 1000));
                    _events.ScheduleEvent(EVENT_DANFORTH_SUNDER_ARMOR, urand(2000, 15000));
                    _events.ScheduleEvent(EVENT_DANFORTH_SHOCKWAVE, urand(7000, 12000));
                    _events.ScheduleEvent(EVENT_DANFORTH_THUNDERCLAP, urand(9000, 15000));
                    break;
                case NPC_KEESHAN_GEARED: // hmm he is using krakauer spells
                    _events.ScheduleEvent(EVENT_KRAKAUER_BLOODTHIRST, urand(2000, 8000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_REND, urand(2000, 11000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_SHATTERING_THROW, urand(1000, 12000));
                    _events.ScheduleEvent(EVENT_KRAKAUER_SLAM, urand(5000, 15000));

                    if (!me->GetAura(SPELL_KESSHAN_SHOUT))
                        me->CastSpell(me, SPELL_KESSHAN_SHOUT);
                    break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetEntry() == NPC_MESSNER_UNGEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_MESSNER_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_RETURN_OF_THE_BRAVO_COMPANY) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_MESSNER_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.5f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.5f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_MESSNER_GEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_MESSNER_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_MESSNER_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.7f, 0.4f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.7f, 0.4f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_JORGENSEN_UNGEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_JORGENSEN_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_RETURN_OF_THE_BRAVO_COMPANY) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_JORGENSEN_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.8f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.8f * M_PI);
                            _isFollowing = true;
                        }


                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            if (player->GetHealthPct() < 40)
                            {
                                me->CastSpell(player, SPELL_JORGENSEN_HOLY_LIGHT);
                            }
                            else
                                return;
                        }
                    }
                    else if (me->GetEntry() == NPC_JORGENSEN_GEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_JORGENSEN_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_JORGENSEN_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.6f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.6f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_KRAKAUER_UNGEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_KRAKAUER_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_RETURN_OF_THE_BRAVO_COMPANY) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_KRAKAUER_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.1f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.1f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_KRAKAUER_GEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_KRAKAUER_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_KRAKAUER_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.9f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.9f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_DANFORTH_UNGEARED)
                    {

                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_DANFORTH_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_RETURN_OF_THE_BRAVO_COMPANY) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_DANFORTH_UNGEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.4f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.4f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_DANFORTH_GEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_DANFORTH_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_DANFORTH_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.2f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.2f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_KEESHAN_GEARED)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 44)
                        {
                            player->RemoveAura(SPELL_SUMMON_KEESHAN_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) == QUEST_STATUS_REWARDED)
                        {
                            player->RemoveAura(SPELL_SUMMON_KEESHAN_GEARED);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY);
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.5f * M_PI);
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.5f * M_PI);
                            _isFollowing = true;
                        }

                        if (auto darkblaze = me->getVictim())
                        {
                            if (darkblaze->GetEntry() == NPC_DARKBLAZE)
                            {
                                me->CastSpell(darkblaze, SPELL_THREATEN);

                                if (!_changeDarkblazePos)
                                {
                                    _changeDarkblazePos = true;
                                    me->NearTeleportTo(-9451.59f, -2953.8f, 126.92f, 4.3f);
                                }
                            }
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MESSNER_BLAST_WAVE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_MESSNER_BLAST_WAVE);
                    _events.ScheduleEvent(EVENT_MESSNER_BLAST_WAVE, urand(8000, 20000));
                    break;
                case EVENT_MESSNER_FIREBALL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_MESSNER_FIREBALL);
                    _events.ScheduleEvent(EVENT_MESSNER_FIREBALL, urand(3000, 6000));
                    break;
                case EVENT_MESSNER_FLAMESTRIKE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_MESSNER_FLAMESTRIKE);
                    _events.ScheduleEvent(EVENT_MESSNER_FLAMESTRIKE, urand(12000, 25000));
                    break;
                case EVENT_JORGENSEN_EXORCISM:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_JORGENSEN_EXORCISM);
                    _events.ScheduleEvent(EVENT_JORGENSEN_EXORCISM, urand(8000, 25000));
                    break;
                case EVENT_JORGENSEN_HOLY_SHOCK:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_JORGENSEN_HOLY_SHOCK);
                    _events.ScheduleEvent(EVENT_JORGENSEN_HOLY_SHOCK, urand(8000, 25000));
                    break;
                case EVENT_KRAKAUER_BLOODTHIRST:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_BLOODTHIRST);
                    _events.ScheduleEvent(EVENT_KRAKAUER_BLOODTHIRST, urand(8000, 15000));
                    break;
                case EVENT_KRAKAUER_REND:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_REND);
                    _events.ScheduleEvent(EVENT_KRAKAUER_REND, urand(5000, 12000));
                    break;
                case EVENT_KRAKAUER_SHATTERING_THROW:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_SHATTERING_THROW);
                    _events.ScheduleEvent(EVENT_KRAKAUER_SHATTERING_THROW, urand(25000, 45000));
                    break;
                case EVENT_KRAKAUER_SLAM:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_SLAM);
                    _events.ScheduleEvent(EVENT_KRAKAUER_SLAM, urand(8000, 25000));
                    break;
                case EVENT_DANFORTH_CHARGE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_SLAM);
                    break;
                case EVENT_DANFORTH_SUNDER_ARMOR:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_DANFORTH_SUNDER_ARMOR);
                    _events.ScheduleEvent(EVENT_DANFORTH_SUNDER_ARMOR, urand(8000, 25000));
                    break;
                case EVENT_DANFORTH_SHOCKWAVE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_DANFORTH_SHOCKWAVE);
                    _events.ScheduleEvent(EVENT_DANFORTH_SHOCKWAVE, urand(8000, 25000));
                    break;
                case EVENT_DANFORTH_THUNDERCLAP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_KRAKAUER_SLAM);
                    _events.ScheduleEvent(SPELL_DANFORTH_THUNDERCLAP, urand(8000, 25000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _changeDarkblazePos = false;
        bool _isFollowing = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bravo_company_team_redridgeAI(creature);
    }
};

class playerscript_redridge_bravo_company_summons : public PlayerScript
{
public:
    playerscript_redridge_bravo_company_summons() : PlayerScript("playerscript_redridge_bravo_company_summons") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newZone == 44)
        {
            if (player->GetQuestStatus(QUEST_RETURN_OF_THE_BRAVO_COMPANY) != QUEST_STATUS_REWARDED)
            {
                if (!player->GetAura(SPELL_SUMMON_MESSNER_UNGEARED) && player->GetQuestStatus(QUEST_BREAKING_OUT_IS_HARD_TO_DO) == QUEST_STATUS_REWARDED)
                {
                    player->CastWithDelay(500, player, SPELL_SUMMON_MESSNER_UNGEARED);
                }

                if (!player->GetAura(SPELL_SUMMON_JORGENSEN_UNGEARED) && player->GetQuestStatus(QUEST_JORGENSEN) == QUEST_STATUS_REWARDED)
                {
                    player->CastWithDelay(500, player, SPELL_SUMMON_JORGENSEN_UNGEARED);
                }

                if (!player->GetAura(SPELL_SUMMON_KRAKAUER_UNGEARED) && player->GetQuestStatus(QUEST_KRAKAUER) == QUEST_STATUS_REWARDED)
                {
                    player->CastWithDelay(500, player, SPELL_SUMMON_KRAKAUER_UNGEARED);
                }

                if (!player->GetAura(SPELL_SUMMON_DANFORTH_UNGEARED) && player->GetQuestStatus(QUEST_AND_LAST_BUT_NOT_LEAST_DANFORTH) == QUEST_STATUS_REWARDED)
                {
                    player->CastWithDelay(500, player, SPELL_SUMMON_DANFORTH_UNGEARED);
                }
            }

            if (player->GetQuestStatus(QUEST_DARKBLAZE_BROOD_OF_THE_WORLDBREAKER) != QUEST_STATUS_REWARDED)
            {
                if (player->GetQuestStatus(QUEST_SHOWDOWN_AT_STONEWATCH) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_SHOWDOWN_AT_STONEWATCH) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_SHOWDOWN_AT_STONEWATCH) == QUEST_STATUS_REWARDED)
                {
                    if (!player->GetAura(SPELL_SUMMON_KEESHAN_GEARED) && !player->GetAura(SPELL_SUMMON_MESSNER_GEARED) && !player->GetAura(SPELL_SUMMON_JORGENSEN_GEARED)
                        && !player->GetAura(SPELL_SUMMON_DANFORTH_GEARED) && !player->GetAura(SPELL_SUMMON_KRAKAUER_GEARED))
                    {
                        player->CastWithDelay(500, player, SPELL_SUMMON_KEESHAN_GEARED);
                        player->CastWithDelay(500, player, SPELL_SUMMON_MESSNER_GEARED);
                        player->CastWithDelay(500, player, SPELL_SUMMON_JORGENSEN_GEARED);
                        player->CastWithDelay(500, player, SPELL_SUMMON_DANFORTH_GEARED);
                        player->CastWithDelay(500, player, SPELL_SUMMON_KRAKAUER_GEARED);
                    }
                }
            }
        }
        else
            return;
    }
};

enum HangingDenforth
{
    NPC_DANFORTH_QUEST_GIVER = 43275,
    NPC_DANFORTH_TARGET_1 = 43366,
    NPC_DANFORTH_TARGET_2 = 74977,

    SPELL_CHAINS_RIGHT = 81081,
    SPELL_CHAINS_LEFT = 81085,
    SPELL_CREEPER_ANIM = 98858,
};

class npc_damn_hanging_danforth : public CreatureScript
{
public:
    npc_damn_hanging_danforth() : CreatureScript("npc_damn_hanging_danforth") { }

    struct npc_damn_hanging_danforthAI : public ScriptedAI
    {
        npc_damn_hanging_danforthAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->setActive(true);
            JustRespawned();
        }

        void JustRespawned()
        {
            me->setActive(true);
            me->CastSpell(me, SPELL_CREEPER_ANIM);
            me->CastSpell(me, SPELL_ROOT);

            if (auto target1 = me->SummonCreature(NPC_DANFORTH_TARGET_1, -8802.50f, -2195.45f, 149.984f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                if (auto target2 = me->SummonCreature(NPC_DANFORTH_TARGET_2, -8810.36f, -2216.82f, 149.685f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    _target1GUID = target1->GetGUID();
                    _target2GUID = target2->GetGUID();
                    me->CastWithDelay(1000, me, SPELL_CHAINS_RIGHT);
                    me->CastWithDelay(500, me, SPELL_CHAINS_LEFT);
                }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                who->SummonCreature(NPC_DANFORTH_QUEST_GIVER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
                who->RemoveAura(49417); // remove spell invis
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                me->RemoveAura(SPELL_CHAINS_RIGHT);
                me->RemoveAura(SPELL_CHAINS_LEFT);
                me->CastSpell(me, SPELL_CHAINS_RIGHT);
                me->CastSpell(me, SPELL_CHAINS_LEFT);
                _checkTimer = 5000;
            }
            else _checkTimer -= diff;
        }

    private:
        uint64 _target1GUID = 0;
        uint64 _target2GUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_damn_hanging_danforthAI(creature);
    }
};

void AddSC_redridge_mountains()
{
    new npc_keeshans_riverboat_spellclick();
    new npc_keeshans_riverboat_vehicle();
    new npc_jorgensen_43546();
    new npc_wild_rat_43518();
    new spell_bravo_company_field_kit();
    new spell_prison_camouflaged();
    new spell_summon_scene_tank();
    new detonation_redridge_playerscript();
    new npc_detonation_camera();
    new npc_bravo_company_siege_tank();
    new npc_bravo_company_siege_tank_vehicle();
    new npc_bravo_company_machine_gun();
    new npc_spawn_more_blackrock_invaders();
    new npc_darkblaze();
    new npc_bravo_company_team_redridge();
    new playerscript_redridge_bravo_company_summons();
    new npc_damn_hanging_danforth();
}
