/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
 *
 * ScriptData
 * SDName: Azshara
 * SD%Complete: 100 as hell
 * SDComment: Placeholder
 * SDCategory: Azshara
 * EndScriptData
 *
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

enum AzsharaQuests
{
    QUEST_THE_CAPTAINS_LOGS = 14134,
    QUEST_UP_A_TREE = 14135,
    QUEST_DEFEND_THE_GATES = 14146,
    QUEST_ARBORCIDE = 14155,
    QUEST_SURVEY_THE_LAKESHORE = 14202,
    QUEST_RAPTOR_RAPTOR_ROCKET = 14422,
    QUEST_TROUBLE_UNDER_FOOT = 14226,
};

enum AzsharaEvents
{
    EVENT_SUMMON_SNIPER_1                   = 1,
    EVENT_SUMMON_SNIPER_2                   = 2,
    EVENT_SUMMON_SNIPER_3                   = 3,
    EVENT_SUMMON_SNIPER_4                   = 4,
    //
    EVENT_GOBLIN_SURV_NORTH_1               = 5,
    EVENT_GOBLIN_SURV_NORTH_2               = 6,
    EVENT_GOBLIN_SURV_WEST_1                = 7,
    EVENT_GOBLIN_SURV_WEST_2                = 8,
    EVENT_GOBLIN_SURV_EAST_1                = 9,
    EVENT_GOBLIN_SURV_EAST_2                = 10,
    EVENT_GOBLIN_SURV_SUMMON_ENEMY          = 11,
    EVENT_GOBLIN_SURV_END                   = 12,
    EVENT_PRIEST_DOZERCISM_1                = 13,
    EVENT_PRIEST_DOZERCISM_2                = 14,
    EVENT_PRIEST_DOZERCISM_3                = 15,
    EVENT_PRIEST_DOZERCISM_4                = 16,
    EVENT_DOZER_1                           = 17,
    EVENT_DOZER_2                           = 18,
    EVENT_DOZER_3                           = 19,
    EVENT_DOZER_4                           = 20,
    EVENT_DOZER_5                           = 21,
    EVENT_AZSHARITE_TRANSPORT_1             = 22,
    EVENT_MUTANT_GOBLIN_1                   = 23,
    EVENT_EXPERIMENTAL_RAPTOR_1             = 24,
    EVENT_GYRO_FLIGHT_1                     = 25,
    EVENT_GYRO_FLIGHT_2                     = 26,
    EVENT_XYLEM_1                           = 27,
    EVENT_XYLEM_BOTTOM_1                    = 28,
    EVENT_XYLEM_BOTTOM_2                    = 29,
    EVENT_XYLEM_BOTTOM_3                    = 30,
    EVENT_ARCANE_CONSTRUCT_1                = 31,
    EVENT_ARCANE_CONSTRUCT_2                = 32,
    EVENT_ARCANE_CONSTRUCT_3                = 33,
    EVENT_SURFACE_TRANS_1                   = 34,
    EVENT_SURFACE_TRANS_SUM_1               = 35,
    EVENT_SQUAD_TICKER_1                    = 36,
    EVENT_SQUAD_TICKER_2                    = 37,
    EVENT_SQUAD_TICKER_3                    = 38,
    EVENT_SQUAD_TICKER_4                    = 39,
    EVENT_SQUAD_TICKER_5                    = 40,
    EVENT_RUNESTONE_EXPLOSTION              = 41,
    EVENT_AIRBORNE_SPELLCLICK               = 42,
    EVENT_SHARPSHIV_1                       = 43,
    EVENT_SHARPSHIV_2                       = 44,
    EVENT_SHARPSHIV_3                       = 45,
    EVENT_SHARPSHIV_4                       = 52,
    EVENT_SHARPSHIV_5                       = 53,
    EVENT_SHARPSHIV_6                       = 54,
    EVENT_SHARPSHIV_7                       = 55,
    EVENT_CLIMBING_1                        = 46,
    EVENT_CLIMBING_2                        = 47,
    EVENT_CLIMBING_3                        = 48,
    EVENT_CLIMBING_4                        = 49,
    EVENT_CLIMBING_5                        = 50,
    EVENT_CLIMBING_6                        = 51,
    EVENT_PUSHTHEBUTTON_1                   = 52,
    EVENT_PUSHTHEBUTTON_2                   = 53,
    EVENT_VALORMOK_WIND_RIDER_1             = 54,
    EVENT_FIRE_PILLAR_1                     = 55,
    EVENT_FIRE_PILLAR_2                     = 56,
    EVENT_FIRE_PILLAR_3                     = 57,
    EVENT_FIRE_PILLAR_4                     = 58,
    EVENT_WEEPING_SOUL_1                    = 59,
    EVENT_ACTION_TRIAL_FIRE_CONTROLLER_1    = 59,
    EVENT_ACTION_TRIAL_FIRE_CONTROLLER_2    = 60,
    EVENT_ACTION_TRIAL_FIRE_CONTROLLER_3    = 61,
    EVENT_XYLEM_TELEPORT                    = 62,
    EVENT_MALICON_1                         = 63,
    EVENT_ROCKETWAY_TRANSIT_1               = 64,
    EVENT_SHREDDER_DESPAWN                  = 65,
    EVENT_POWER_STONE_TIMER                 = 66,
    EVENT_AIRPLANE_1                        = 67,
    EVENT_AIRPLANE_2                        = 68,
    EVENT_AIRPLANE_3                        = 69,
    EVENT_AIRPLANE_4                        = 70,

    ACTION_PRIEST_END                       = 1,
    ACTION_AZSHARITE_CORE_1                 = 2,
    ACTION_AZSHARITE_CORE_2                 = 3,
    ACTION_AZSHARITE_CORE_COSMETIC_FIRE_OFF = 4,
    ACTION_ARCANE_CONSTRUCT_1               = 5,
    ACTION_RUNESTONE_TRIGGER_DESPAWN        = 6,
    ACTION_RUNESTONE_TRIGGER_END            = 7,
    ACTION_REMOVE_HOOK_EFFECT               = 8,
    ACTION_FIRE_PILLAR_1                    = 9,
    ACTION_FIRE_PILLAR_2                    = 10,
    ACTION_TRIAL_FIRE_CONTROLLER_1          = 11,
    ACTION_XYLEM_TELEPORT                   = 12,
    ACTION_MALICON_1                        = 13,
    ACTION_MALICON_2                        = 14,
};

enum AzsharaNpcs
{
    NPC_RUNAWAY_SHREDDER_VEH        = 35129,
    NPC_SUMMON_SNIPER_CONTROLLER    = 74971,
    NPC_TALRENDIS_SNIPER            = 35149,
    NPC_BILGEWATER_MORTAR           = 36768, // cannon
    NPC_SURVEY_EQUIPMENT            = 35485,
    NPC_SHADE_OF_HATE               = 35503,
    NPC_AIRBORNE_GOBLIN_PRIEST      = 36552,
    NPC_DOZER_CLONE                 = 74969,
    NPC_AUTOMATED_LAB_TRANSPORT     = 36098,
    NPC_AZSHARITE_CORE              = 36105,
    NPC_HOBGOBLIN_PUSHER            = 36136,
    NPC_RED_LIGHT                   = 36125,
    NPC_COSMETIC_FIRE               = 74967,
    NPC_GORMUNGAN                   = 36297,
    NPC_MUTANT_GOBLIN               = 36156,
    NPC_MUTANT_TENTACLE             = 36157,
};

enum AzsharaSpellUsed
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_QUEST_REWARD_PING = 151301,
    SPELL_BUZZSAW = 66811,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_FLIGHT_SPEED_280 = 50345,
    SPELL_FLIGHT_SPEED_100 = 54421,

    // quest=14202/survey-the-lakeshore
    SPELL_NORTH_SURVEY = 67582,
    SPELL_WEST_SURVEY = 67583,
    SPELL_EAST_SURVEY = 67584,
    SPELL_NORTH_SURVEY_COMPLETE = 67585,
    SPELL_WEST_SURVEY_COMPLETE = 67586,
    SPELL_EAST_SURVEY_COMPLETE = 67587,
    SPELL_SURVEYING = 67588,
    SPELL_SURVEY_PULSE = 67589,
    SPELL_SURVEYOR_DIES = 83025,
    SPELL_ESCORTING_A_SURVEYOR = 67570,

    //dozen
    SPELL_FORCECAST_PLAYER_TO_TAUNT_BULLDOZER = 68918,
    SPELL_ANGER_THE_DOZER = 68923,
    SPELL_BULLDOZER_EXORCISM_KILL_CREDIT = 68944,
    SPELL_AIRDROP_A_PRIEST = 68007,
    SPELL_SUMMON_SPOTLIGHT = 68041,
    SPELL_PARACHUTE = 110660,

    //core transport
    SPELL_SUMMON_AUTOMATED_LAB_TRANSPORT = 68348,
    SPELL_SUMMON_AZSHARITE_CORE = 68370,
    SPELL_SUMMON_HOBGOBLIN_PUSHER = 68374,
    SPELL_RIDE_VEHICLE_SEAT = 94066, // 68372
    SPELL_PUSH_ME = 68375,
    SPELL_CORE_DELIVERY_QUEST_CREDIT = 68379,
    SPELL_PERIODIC_OVERHEAT = 68389,
    SPELL_OVERHEATING = 68380,
    SPELL_COSMETIC_STUN = 68369,
    SPELL_STEAM = 95503,
    SPELL_FIRE_BURNING = 52855,
    SPELL_EXTINGUISH = 68283,
    SPELL_DUMMY = 98914,
    SPELL_SHRUNKIFIED = 68710,
    SPELL_RIDE_RAT = 68726,
    SPELL_RAPTOR_CAGE_CUE_FROM_PLAYER = 68801,
    SPELL_UNLEASH_THE_RAPTOR = 69707,
    SPELL_BABYSITTING_RAPTORS = 69704,
    SPELL_PING_BABYSITTING_RAPTORS = 151305,

    SPELL_AURA_PREVENT_EVENT_1 = 151312,
    SPELL_AURA_PREVENT_EVENT_2 = 151313,
    SPELL_AURA_PREVENT_EVENT_3 = 151314,
};

class npc_summon_sniper_controller : public CreatureScript
{
public:
    npc_summon_sniper_controller() : CreatureScript("npc_summon_sniper_controller") { }

    struct npc_summon_sniper_controllerAI : public ScriptedAI
    {
        npc_summon_sniper_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            events.ScheduleEvent(EVENT_SUMMON_SNIPER_1, 3500);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SUMMON_SNIPER_1:
                    me->SummonCreature(NPC_TALRENDIS_SNIPER, me->GetPositionX() + 3, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000);
                    events.ScheduleEvent(EVENT_SUMMON_SNIPER_2, 1500);
                    break;
                case EVENT_SUMMON_SNIPER_2:
                    me->SummonCreature(NPC_TALRENDIS_SNIPER, me->GetPositionX() - 3, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000);
                    events.ScheduleEvent(EVENT_SUMMON_SNIPER_3, 1500);
                    break;
                case EVENT_SUMMON_SNIPER_3:
                    me->SummonCreature(NPC_TALRENDIS_SNIPER, me->GetPositionX(), me->GetPositionY() + 3, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000);
                    events.ScheduleEvent(EVENT_SUMMON_SNIPER_4, 1000);
                    break;
                case EVENT_SUMMON_SNIPER_4:
                    me->SummonCreature(NPC_TALRENDIS_SNIPER, me->GetPositionX(), me->GetPositionY() - 3, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000);
                    me->DespawnOrUnsummon();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summon_sniper_controllerAI(creature);
    }
};

class npc_azshara_sapling : public CreatureScript
{
public:
    npc_azshara_sapling() : CreatureScript("npc_azshara_sapling") { }

    struct npc_azshara_saplingAI : public ScriptedAI
    {
        npc_azshara_saplingAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_BUZZSAW)
            {
                me->SummonCreature(NPC_SUMMON_SNIPER_CONTROLLER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 9, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 15000);
                me->Kill(me);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azshara_saplingAI(creature);
    }
};

// BILGEWATER_MORTAR 36768
class npc_bilgewater_mortar_cannon : public CreatureScript
{
public:
    npc_bilgewater_mortar_cannon() : CreatureScript("npc_bilgewater_mortar_cannon") { }

    struct npc_bilgewater_mortar_cannonAI : public ScriptedAI
    {
        npc_bilgewater_mortar_cannonAI(Creature* creature) : ScriptedAI(creature) {}

        void OnSpellClick(Unit* /*clicker*/, bool& /*result*/)
        {
            me->SetRooted(true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bilgewater_mortar_cannonAI(creature);
    }
};

// GOBLIN_SURVEYOR_JR_GRADE
class npc_goblin_surveyor_jr_grade : public CreatureScript
{
public:
    npc_goblin_surveyor_jr_grade() : CreatureScript("npc_goblin_surveyor_jr_grade") { }

    struct npc_goblin_surveyor_jr_gradeAI : public ScriptedAI
    {
        npc_goblin_surveyor_jr_gradeAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(summoner, SPELL_ESCORTING_A_SURVEYOR);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_NORTH_SURVEY && !_surveyNorthMarkerCompleted)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (auto survey = who->ToCreature())
                    {
                        _surveyEquipmentGUID = survey->GetGUID();
                        me->CastSpell(who, SPELL_DUMMY_PING);
                        me->AI()->Talk(0, _playerGUID);
                        me->GetMotionMaster()->MovePoint(1, who->GetPositionX() + 2, who->GetPositionY(), who->GetPositionZ());
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_NORTH_1, 2000);
                        _surveyNorthMarkerCompleted = true;
                    }
            }

            if (spellInfo->Id == SPELL_WEST_SURVEY && !_surveyWestMarkerCompleted)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (auto survey = who->ToCreature())
                    {
                        _surveyEquipmentGUID = survey->GetGUID();
                        me->CastSpell(who, SPELL_DUMMY_PING);
                        me->AI()->Talk(0, _playerGUID);
                        me->GetMotionMaster()->MovePoint(1, who->GetPositionX() + 2, who->GetPositionY(), who->GetPositionZ());
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_WEST_1, 2000);
                        _surveyWestMarkerCompleted = true;
                    }
            }

            if (spellInfo->Id == SPELL_EAST_SURVEY && !_surveyEastMarkerCompleted)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (auto survey = who->ToCreature())
                    {
                        _surveyEquipmentGUID = survey->GetGUID();
                        me->CastSpell(who, SPELL_DUMMY_PING);
                        me->AI()->Talk(0, _playerGUID);
                        me->GetMotionMaster()->MovePoint(1, who->GetPositionX() + 2, who->GetPositionY(), who->GetPositionZ());
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_EAST_1, 2000);
                        _surveyEastMarkerCompleted = true;
                    }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                if (!me->GetOwner()->isAlive())
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->RemoveAura(SPELL_ESCORTING_A_SURVEYOR);
                        me->DespawnOrUnsummon();
                    }
                }

                if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_SURVEY_THE_LAKESHORE) == QUEST_STATUS_COMPLETE && !_questCompleted)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_END, 2000);
                        _questCompleted = true;
                    }
                }

                if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_SURVEY_THE_LAKESHORE) == QUEST_STATUS_NONE)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->RemoveAura(SPELL_ESCORTING_A_SURVEYOR);
                        me->DespawnOrUnsummon(1000);
                    }
                }
            }
            else _checkTimer -= diff;

            if (_checkSpeedTimer <= diff)
            {
                if (me->GetOwner()->ToPlayer()->GetSpeedRate(MOVE_RUN) != me->GetSpeedRate(MOVE_RUN))
                {
                    me->SetSpeed(MOVE_RUN, me->GetOwner()->ToPlayer()->GetSpeedRate(MOVE_RUN));
                    _checkSpeedTimer = 3000;
                }
            }
            else _checkSpeedTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GOBLIN_SURV_NORTH_1:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                    {
                        me->SetRooted(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->CastSpell(survey, SPELL_SURVEYING);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_NORTH_2, 23000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 3000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 11000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 19000);
                    }
                    break;
                case EVENT_GOBLIN_SURV_NORTH_2:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            me->SetRooted(false);
                            me->CombatStop();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->AI()->Talk(2, _playerGUID);
                            survey->CastSpell(survey, SPELL_NORTH_SURVEY_COMPLETE);
                            _surveyEquipmentGUID = 0;
                            me->GetMotionMaster()->MoveFollow(player, 1, 2);
                        }
                    break;
                case EVENT_GOBLIN_SURV_WEST_1:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                    {
                        me->SetRooted(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->CastSpell(survey, SPELL_SURVEYING);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_WEST_2, 23000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 3000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 11000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 19000);
                    }
                    break;
                case EVENT_GOBLIN_SURV_WEST_2:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            me->SetRooted(false);
                            me->CombatStop();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->AI()->Talk(2, _playerGUID);
                            survey->CastSpell(survey, SPELL_WEST_SURVEY_COMPLETE);
                            _surveyEquipmentGUID = 0;
                            me->GetMotionMaster()->MoveFollow(player, 1, 2);
                        }
                    break;
                case EVENT_GOBLIN_SURV_EAST_1:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                    {
                        me->SetRooted(true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->CastSpell(survey, SPELL_SURVEYING);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_EAST_2, 23000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 3000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 11000);
                        _events.ScheduleEvent(EVENT_GOBLIN_SURV_SUMMON_ENEMY, 19000);
                    }
                    break;
                case EVENT_GOBLIN_SURV_EAST_2:
                    if (auto survey = Unit::GetCreature(*me, _surveyEquipmentGUID))
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            me->SetRooted(false);
                            me->CombatStop();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->AI()->Talk(2, _playerGUID);
                            survey->CastSpell(survey, SPELL_EAST_SURVEY_COMPLETE);
                            _surveyEquipmentGUID = 0;
                            me->GetMotionMaster()->MoveFollow(player, 1, 2);
                        }
                    break;

                case EVENT_GOBLIN_SURV_END:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->RemoveAura(SPELL_ESCORTING_A_SURVEYOR);
                        me->AI()->Talk(3, _playerGUID);
                        me->SetFacingToObject(player);
                        me->CastSpell(me, 42716);
                        me->DespawnOrUnsummon(4000);
                    }
                    break;
                case EVENT_GOBLIN_SURV_SUMMON_ENEMY:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto enemy = me->SummonCreature(NPC_SHADE_OF_HATE, me->GetPositionX() + (urand(5, 10)), me->GetPositionY() + (urand(5, 8)), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                            enemy->AI()->AttackStart(player);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _surveyWestMarkerCompleted = false;
        bool _surveyNorthMarkerCompleted = false;
        bool _surveyEastMarkerCompleted = false;
        bool _questCompleted = false;

        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _surveyEquipmentGUID = 0;
        uint16 _checkTimer = 2000;
        uint16 _checkSpeedTimer = 3000;

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_goblin_surveyor_jr_gradeAI(creature);
    }
};

static Position PriestSummonPos = { 2899.08f, -5027.65f, 168.00f, 6.207721f };

class npc_clubniks_dozer : public CreatureScript
{
public:
    npc_clubniks_dozer() : CreatureScript("npc_clubniks_dozer") { }

    struct npc_clubniks_dozerAI : public ScriptedAI
    {
        npc_clubniks_dozerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_AIRDROP_A_PRIEST && !_eventStarted)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_SUMMON_SPOTLIGHT);
                }

                if (auto priest = who->SummonCreature(NPC_AIRBORNE_GOBLIN_PRIEST, PriestSummonPos, TEMPSUMMON_TIMED_DESPAWN, 600000))
                {
                    _eventStarted = true;
                }
            }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if ((me->GetHealthPct() <= 76 && me->GetHealthPct() >= 51) && !_reached_75_pct)
            {
                if (auto priest = Unit::GetCreature(*me, _priestGUID))
                {
                    priest->AI()->Talk(5);
                    _reached_75_pct = true;
                }
            }
            else if ((me->GetHealthPct() <= 50 && me->GetHealthPct() >= 26) && !_reached_50_pct)
            {
                if (auto priest = Unit::GetCreature(*me, _priestGUID))
                {
                    priest->AI()->Talk(6);
                    _reached_50_pct = true;
                }
            }
            else if ((me->GetHealthPct() <= 25 && me->GetHealthPct() >= 1) && !_reached_25_pct)
            {
                if (auto priest = Unit::GetCreature(*me, _priestGUID))
                {
                    priest->AI()->Talk(7);
                    _reached_25_pct = true;
                }
            }
            else
                return;
        }

        void EnterEvadeMode()
        {
            if (auto priest = me->FindNearestCreature(NPC_AIRBORNE_GOBLIN_PRIEST, 10))
                priest->DespawnOrUnsummon();
            _playerGUID = 0;
            _priestGUID = 0;
            _eventStarted = false;
            me->DespawnOrUnsummon(200);
        }

        void JustDied(Unit* /* killer */)
        {
            if (auto priest = Unit::GetCreature(*me, _priestGUID))
            {
                priest->GetMotionMaster()->ForceMoveJump(me->GetPositionX() + 8, me->GetPositionY(), me->GetPositionZ(), 12, 15);
                priest->AI()->Talk(8);
                priest->AI()->DoAction(ACTION_PRIEST_END);
                me->CastSpell(me, SPELL_BULLDOZER_EXORCISM_KILL_CREDIT);
                me->setFaction(1933);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply)
            {
                if (auto priest = passenger->ToCreature())
                    _priestGUID = priest->GetGUID();

                me->AI()->Talk(0);
                _events.ScheduleEvent(EVENT_DOZER_1, 2000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DOZER_1:
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->setFaction(14);
                    me->JumpWithDelay(1000, 2890.14f, -5043.42f, 125.870f, 30, 15, 0);
                    _events.ScheduleEvent(EVENT_DOZER_2, 1500);
                    break;
                case EVENT_DOZER_2:
                    me->JumpWithDelay(500, 2911.0195f, -5040.9072f, 126.157f, 30, 15, 1);
                    _events.ScheduleEvent(EVENT_DOZER_3, 1000);
                    break;
                case EVENT_DOZER_3:
                    me->JumpWithDelay(500, 2896.3332f, -5021.5234f, 126.571f, 30, 15, 2);
                    _events.ScheduleEvent(EVENT_DOZER_4, 2000);
                    break;
                case EVENT_DOZER_4:
                    if (auto priest = Unit::GetCreature(*me, _priestGUID))
                    {
                        priest->AI()->Talk(3);
                        priest->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        _events.ScheduleEvent(EVENT_DOZER_5, 1000);
                    }
                    break;
                case EVENT_DOZER_5:
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->isAlive())
                            me->AI()->AttackStart(player);
                        else
                            me->AI()->AttackStart(me->FindNearestPlayer(40));
                    }

                    if (auto priest = Unit::GetCreature(*me, _priestGUID))
                    {
                        priest->AI()->TalkWithDelay(3000, 4);
                    }
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        bool _eventStarted = false;
        bool _reached_75_pct = false;
        bool _reached_50_pct = false;
        bool _reached_25_pct = false;

        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _priestGUID = 0;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_clubniks_dozerAI(creature);
    }
};

class npc_priest_dozercism : public CreatureScript
{
public:
    npc_priest_dozercism() : CreatureScript("npc_priest_dozercism") { }

    struct npc_priest_dozercismAI : public ScriptedAI
    {
        npc_priest_dozercismAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->CastSpell(me, SPELL_PARACHUTE);
            _onGround = false;
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_PRIEST_END)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                _events.ScheduleEvent(EVENT_PRIEST_DOZERCISM_4, 3000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (!_onGround && !me->GetAura(SPELL_PARACHUTE))
            {
                _onGround = true;
                _events.ScheduleEvent(EVENT_PRIEST_DOZERCISM_1, 2000);
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PRIEST_DOZERCISM_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->AI()->Talk(0, _playerGUID);
                        _events.ScheduleEvent(EVENT_PRIEST_DOZERCISM_2, 4000);
                    }
                    break;
                case EVENT_PRIEST_DOZERCISM_2:
                    me->GetMotionMaster()->MovePoint(1, 2904.92f, -5030.56f, 127.136f);
                    break;
                case EVENT_PRIEST_DOZERCISM_3:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetFacingToObject(player);
                        me->TalkWithDelay(1000, 1, _playerGUID);
                        me->TalkWithDelay(6000, 2, _playerGUID);

                        if (auto dozer = me->FindNearestCreature(NPC_DOZER_CLONE, 15.0f))
                            me->CastWithDelay(12000, dozer, SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_PRIEST_DOZERCISM_4:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetFacingToObject(player);
                        me->TalkWithDelay(1000, 9, _playerGUID);
                        me->DespawnOrUnsummon(7000);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (point == 1 && !_reachedPoint)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
                _events.ScheduleEvent(EVENT_PRIEST_DOZERCISM_3, 2000);
                _reachedPoint = true;
            }
        }

    private:
        bool _onGround = true;
        bool _reachedPoint = false;

        EventMap _events;
        uint64 _playerGUID = 0;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_priest_dozercismAI(creature);
    }
};

uint32 const AzshariteCoreTransportPathSize = 22;
const G3D::Vector3 AzshariteCoreTransportPath[AzshariteCoreTransportPathSize] =
{
    { 2590.109f, -5796.0302f, 77.180f },  // 0
    { 2593.136f, -5795.93f, 77.17f },    // 1
    { 2600.189f, -5806.2133f, 77.055f},  // 2
    { 2600.381f, -5831.9462f, 78.104f },  // 3
    { 2599.214f, -5860.1821f, 81.905f },  // 4
    { 2597.905f, -5882.8139f, 86.718f },  // 5
    { 2597.612f, -5910.4960f, 95.372f },  // 6
    { 2597.640f, -5931.8906f, 99.000f },  // 7
    { 2597.218f, -5950.3413f, 101.06f },  // 8
    { 2597.530f, -5959.8369f, 101.04f },  // 9
    { 2599.987f, -5962.4648f, 101.11f },  // 10
    { 2613.386f, -5971.2210f, 101.12f },  // 11
    { 2618.123f, -5983.3139f, 101.08f },  // 12
    { 2617.831f, -6021.9331f, 101.09f },  // 13
    { 2617.876f, -6059.1997f, 101.11f },  // 14
    { 2618.797f, -6084.4760f, 101.05f },  // 15
    { 2619.305f, -6088.0302f, 101.05f },  // 16
    { 2622.828f, -6091.3061f, 101.14f },  // 17
    { 2633.472f, -6100.3183f, 101.09f },  // 18
    { 2637.287f, -6101.3872f, 101.12f },  // 19
    { 2646.194f, -6101.5092f, 100.97f },  // 20
    { 2655.230f, -6102.5375f, 100.97f },  // 21
};

class npc_lab_azsharite_core_transport : public CreatureScript
{
public:
    npc_lab_azsharite_core_transport() : CreatureScript("npc_lab_azsharite_core_transport") { }

    struct npc_lab_azsharite_core_transportAI : public ScriptedAI
    {
        npc_lab_azsharite_core_transportAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_STEAM);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto labgoblin = me->SummonCreature(NPC_HOBGOBLIN_PUSHER, 2543.67993f, -5801.1499f, 77.13369f, 0.279253f, TEMPSUMMON_MANUAL_DESPAWN))
            {
                _labgoblinGUID = labgoblin->GetGUID();
                labgoblin->AI()->TalkWithDelay(2000, 0);
                labgoblin->CastWithDelay(4000, me, SPELL_RIDE_VEHICLE_SEAT);
            }

            if (auto core = me->SummonCreature(NPC_AZSHARITE_CORE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 2, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                _coreGUID = core->GetGUID();
                core->EnterVehicle(me, 0);
            }

        }
        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetEntry() == NPC_HOBGOBLIN_PUSHER)
            {
                _events.ScheduleEvent(EVENT_AZSHARITE_TRANSPORT_1, 500);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_EXTINGUISH)
            {
                if (auto core = Unit::GetCreature(*me, _coreGUID))
                {
                    core->AI()->DoAction(ACTION_AZSHARITE_CORE_2);
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
                case EVENT_AZSHARITE_TRANSPORT_1:
                    if (auto core = Unit::GetCreature(*me, _coreGUID))
                    {
                        core->AI()->DoAction(ACTION_AZSHARITE_CORE_1);
                        me->GetMotionMaster()->MoveSmoothPath(AzshariteCoreTransportPath, AzshariteCoreTransportPathSize);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 12:
                if (auto gormungan = me->FindNearestCreature(NPC_GORMUNGAN, 40.0f))
                    gormungan->AI()->TalkWithDelay(1000, 0);
                break;
            case 19:
                if (auto labgoblin = Unit::GetCreature(*me, _labgoblinGUID))
                {
                    me->CastSpell(me, 64614);
                    labgoblin->DespawnOrUnsummon(3000);
                    me->CastSpell(me, SPELL_CORE_DELIVERY_QUEST_CREDIT);
                }

                if (auto intern = me->FindNearestCreature(36061, 20.0f))
                    intern->AI()->TalkWithDelay(1000, 2);

                if (auto core = Unit::GetCreature(*me, _coreGUID))
                {
                    core->RemoveAura(SPELL_PERIODIC_OVERHEAT);
                    core->RemoveAura(SPELL_OVERHEATING);
                }

                if (auto cosmetic = me->FindNearestCreature(NPC_COSMETIC_FIRE, 20.0f))
                {
                    cosmetic->RemoveAura(SPELL_FIRE_BURNING);
                }
                break;

            case 21:
                if (auto core = Unit::GetCreature(*me, _coreGUID))
                    if (auto cosmetic = me->FindNearestCreature(NPC_COSMETIC_FIRE, 20.0f))
                    {
                        core->DespawnOrUnsummon();
                        cosmetic->DespawnOrUnsummon();
                        me->DespawnOrUnsummon();
                    }
                break;
            default:
                break;
            }
        }
    private:
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _labgoblinGUID = 0;
        uint64 _coreGUID = 0;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lab_azsharite_core_transportAI(creature);
    }
};

class npc_azsharite_core_passenger : public CreatureScript
{
public:
    npc_azsharite_core_passenger() : CreatureScript("npc_azsharite_core_passenger") { }

    struct npc_azsharite_core_passengerAI : public ScriptedAI
    {
        npc_azsharite_core_passengerAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->AI()->Talk(0);
            me->setRegeneratingHealth(false);

            if (auto transport = summoner->ToCreature())
            {
                _transportGUID = transport->GetGUID();
                _labgoblinGUID = me->FindNearestCreature(NPC_HOBGOBLIN_PUSHER, 40.0f)->GetGUID();
            }
        }

        void JustDied(Unit* /* killer */)
        {
            if (auto cosmetic = Unit::GetCreature(*me, _cosmeticGUID))
                if (auto labgoblin = Unit::GetCreature(*me, _labgoblinGUID))
                    if (auto transport = Unit::GetCreature(*me, _transportGUID))
                    {
                        cosmetic->CastSpell(cosmetic, 95695);
                        cosmetic->DespawnOrUnsummon(2000);
                        labgoblin->DespawnOrUnsummon();
                        transport->DespawnOrUnsummon();
                        me->DespawnOrUnsummon();
                    }
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
            if (me->GetHealthPct() > 86 && me->GetHealthPct() < 93 && !_reminderUseSpell)
            {
                me->AI()->Talk(1);
                _reminderUseSpell = true;
            }
            else if (me->GetHealthPct() > 55 && me->GetHealthPct() < 62 && !_reminder60pct)
            {
                me->AI()->Talk(2);
                _reminder60pct = true;
            }
            else if (me->GetHealthPct() > 21 && me->GetHealthPct() < 31 && !_reminder30pct)
            {
                me->AI()->Talk(3);
                _reminder30pct = true;
            }
            else if (me->GetHealthPct() > 1 && me->GetHealthPct() < 12 && !_reminder10pct)
            {
                me->AI()->Talk(4);
                me->AI()->Talk(5);
                _reminder10pct = true;
            }
            else
                return;
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_AZSHARITE_CORE_1)
            {
                me->CastSpell(me, SPELL_PERIODIC_OVERHEAT);
                if (auto cosmetic = me->SummonCreature(NPC_COSMETIC_FIRE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    _cosmeticGUID = cosmetic->GetGUID();
                    cosmetic->EnterVehicle(me, 0);
                }
            }

            if (action == ACTION_AZSHARITE_CORE_2)
                if (me->GetAuraCount(SPELL_OVERHEATING) >= 1)
                {
                    if (auto cosmetic = Unit::GetCreature(*me, _cosmeticGUID))
                    {
                        cosmetic->RemoveAura(SPELL_FIRE_BURNING);
                        me->AI()->Talk(6);
                        me->RemoveAura(SPELL_OVERHEATING);

                        if (auto labgoblin = Unit::GetCreature(*me, _labgoblinGUID))
                        {
                            uint16 chance = urand(1, 6);
                            if (chance == 1)
                                labgoblin->AI()->Talk(1);
                        }
                    }
                }
        }

    private:
        bool _reminderUseSpell = false;
        bool _reminder60pct = false;
        bool _reminder30pct = false;
        bool _reminder10pct = false;

        uint64 _labgoblinGUID = 0;
        uint64 _transportGUID = 0;
        uint64 _cosmeticGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azsharite_core_passengerAI(creature);
    }
};


class npc_azsharite_core_cosmetic_fire : public CreatureScript
{
public:
    npc_azsharite_core_cosmetic_fire() : CreatureScript("npc_azsharite_core_cosmetic_fire") { }

    struct npc_azsharite_core_cosmetic_fireAI : public ScriptedAI
    {
        npc_azsharite_core_cosmetic_fireAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_OVERHEATING)
            {
                me->CastSpell(me, SPELL_FIRE_BURNING);
            }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azsharite_core_cosmetic_fireAI(creature);
    }
};

class npc_rocketway_rat_azshara : public CreatureScript
{
public:
    npc_rocketway_rat_azshara() : CreatureScript("npc_rocketway_rat_azshara") { }

    struct npc_rocketway_rat_azsharaAI : public ScriptedAI
    {
        npc_rocketway_rat_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
                if (who->GetAura(SPELL_SHRUNKIFIED))
                {
                    who->CastSpell(who, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    if (auto clonerat = who->SummonCreature(74966, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        who->CastSpell(clonerat, SPELL_RIDE_RAT);
                        me->DespawnOrUnsummon();
                    }
                }
                else
                    me->AI()->Talk(0, who->ToPlayer()->GetGUID());
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rocketway_rat_azsharaAI(creature);
    }
};

class npc_mutant_goblin_azshara : public CreatureScript
{
public:
    npc_mutant_goblin_azshara() : CreatureScript("npc_mutant_goblin_azshara") { }

    struct npc_mutant_goblin_azsharaAI : public ScriptedAI
    {
        npc_mutant_goblin_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto tentancle1 = me->SummonCreature(NPC_MUTANT_TENTACLE, me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() + 2, me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
            {
                _firstTentancleGUID = tentancle1->GetGUID();
                tentancle1->EnterVehicle(me, 0);
            }
            if (auto tentancle2 = me->SummonCreature(NPC_MUTANT_TENTACLE, me->GetPositionX() - 2, me->GetPositionY(), me->GetPositionZ() + 2, me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
            {
                _secondTentancleGUID = tentancle2->GetGUID();
                tentancle2->EnterVehicle(me, 1);
            }

            me->AI()->AttackStart(me->FindNearestPlayer(40));
            me->SetHomePosition(2579.3425f, -5907.9023f, 96.6674f, 6.2620f);
        }

        void JustDied(Unit* /* killer */)
        {
            me->AI()->Talk(0);
            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);

            if (auto tentancle1 = Unit::GetCreature(*me, _firstTentancleGUID))
                if (auto tentancle2 = Unit::GetCreature(*me, _secondTentancleGUID))
                {
                    tentancle1->GetMotionMaster()->MoveJump(me->GetPositionX() - 2, me->GetPositionY(), me->GetPositionZ() + 2, 10, 10, 1);
                    tentancle2->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() + 2, 10, 10, 2);
                    tentancle1->setFaction(14);
                    tentancle2->setFaction(14);
                    tentancle1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    tentancle2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    //tentancle1->AI()->AttackStart(me->FindNearestPlayer(40));
                    //tentancle2->AI()->AttackStart(me->FindNearestPlayer(40));
                    tentancle1->DespawnOrUnsummon(90000);
                    tentancle2->DespawnOrUnsummon(90000);
                }
        }

        void EnterEvadeMode()
        {
            me->DespawnOrUnsummon(0);

            if (auto tentancle1 = Unit::GetCreature(*me, _firstTentancleGUID))
                if (auto tentancle2 = Unit::GetCreature(*me, _secondTentancleGUID))
                {
                    tentancle1->DespawnOrUnsummon(0);
                    tentancle2->DespawnOrUnsummon(0);
                }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();
        }
    private:
        uint64 _firstTentancleGUID = 0;
        uint64 _secondTentancleGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutant_goblin_azsharaAI(creature);
    }
};

class npc_experimental_raptor_standalone : public CreatureScript
{
public:
    npc_experimental_raptor_standalone() : CreatureScript("npc_experimental_raptor_standalone") { }

    struct npc_experimental_raptor_standaloneAI : public ScriptedAI
    {
        npc_experimental_raptor_standaloneAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                if (spellInfo->Id == SPELL_RAPTOR_CAGE_CUE_FROM_PLAYER)
                    if (who->GetAuraCount(SPELL_BABYSITTING_RAPTORS) == 5)
                    {
                        me->AI()->Talk(0);
                        me->DespawnOrUnsummon(1000);
                    }
                    else
                    {
                        who->CastSpell(who, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                        me->CastSpell(who, SPELL_UNLEASH_THE_RAPTOR);
                        me->DespawnOrUnsummon(1000);
                    }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_experimental_raptor_standaloneAI(creature);
    }
};
static Position RaptorEndJump = { 2596.183f, -5704.190f, 111.019f };

class npc_experimental_raptor_summoned : public CreatureScript
{
public:
    npc_experimental_raptor_summoned() : CreatureScript("npc_experimental_raptor_summoned") { }

    struct npc_experimental_raptor_summonedAI : public ScriptedAI
    {
        npc_experimental_raptor_summonedAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->CastWithDelay(500, summoner, SPELL_RIDE_VEHICLE);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_BABYSITTING_RAPTORS && !_endJump)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    me->GetMotionMaster()->MoveJump(me->GetPositionX() + urand(-5.0f, 5.0f), me->GetPositionY() + urand(-5.0f, 5.0f), me->GetPositionZ(), 10, 10, 1);
                    _events.ScheduleEvent(EVENT_EXPERIMENTAL_RAPTOR_1, urand(1000, 2000));
                }
                _endJump = true;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_actionTimer1 <= diff)
            {
                _actionTimer1 = 2000;
                if (me->GetVehicle())
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->GetAura(SPELL_BABYSITTING_RAPTORS) && !_endJump)
                        me->DespawnOrUnsummon();

                    if (player->GetAreaId() != 4829) // the secret lab
                    {
                        player->RemoveAura(SPELL_BABYSITTING_RAPTORS);
                        me->DespawnOrUnsummon();
                    }
                }
            }
            _actionTimer1 -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_EXPERIMENTAL_RAPTOR_1:
                    me->GetMotionMaster()->MoveJump(RaptorEndJump, 12, 16, 2);
                    me->DespawnOrUnsummon(2500);
                    break;
                default:
                    break;
                }
            }

        }
    private:
        EventMap _events;

        bool _endJump = false;
        uint64 _playerGUID = 0;
        uint32 _actionTimer1 = 2000;
        uint32 _actionTimer2 = 800;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_experimental_raptor_summonedAI(creature);
    };
};

uint32 const AzsharaGyroPathSize = 16;
const G3D::Vector3 AzsharaGyroPath[AzsharaGyroPathSize] =
{
    { 2681.000f, -6236.656f, 127.08f, },  // 0
    { 2750.675f, -6296.110f, 133.61f, },  // 1
    { 2887.642f, -6407.885f, 95.755f, },  // 2
    { 3205.958f, -6655.267f, 85.080f, },  // 3
    { 3349.069f, -6770.052f, 131.77f, },  // 4
    { 3512.345f, -6924.332f, 56.432f, },  // 5
    { 3579.635f, -6891.300f, 62.372f, },  // 6
    { 3592.604f, -6831.363f, 114.19f, },  // 7
    { 3603.973f, -6665.964f, 106.51f, },  // 8
    { 3541.153f, -6615.599f, 98.482f, },  // 9
    { 3447.701f, -6623.459f, 91.615f, },  // 10
    { 3398.853f, -6602.561f, 94.688f, },  // 11
    { 3412.968f, -6566.685f, 99.204f, },  // 12
    { 3460.191f, -6564.318f, 91.56f, },  // 13
    { 3507.204f, -6548.192f, 48.603f, },  // 14
    { 3521.526f, -6548.234f, 42.4f, },  // 15
};

class npc_azshara_loaned_gyrocopter : public CreatureScript
{
public:
    npc_azshara_loaned_gyrocopter() : CreatureScript("npc_azshara_loaned_gyrocopter") { }

    struct npc_azshara_loaned_gyrocopterAI : public npc_escortAI
    {
        npc_azshara_loaned_gyrocopterAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            summoner->CastSpell(me, SPELL_RIDE_VEHICLE);
            me->SetSpeed(MOVE_RUN, 3.5f);
        }

        void UpdateAI(uint32 const diff)
        {
            if (_startFlightTimer <= diff && !_startFlight)
            {
                _startFlightTimer = 2000;
                me->GetMotionMaster()->MoveSmoothPath(AzsharaGyroPath, AzsharaGyroPathSize);
                _startFlight = true;
            }
            _startFlightTimer -= diff;
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 15 && _startFlight)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY() - 5, me->GetPositionZ(), 10, 12);
                    me->DespawnOrUnsummon();
                    _startFlight = false;
                }
            }
        }
    private:
        bool _startFlight = false;
        uint32 _startFlightTimer = 500;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azshara_loaned_gyrocopterAI(creature);
    }
};

static Position playerjumpposturtle = { 4451.184f, -5726.787f, 106.23f };

class npc_gurgll_the_tracking_turtle : public CreatureScript
{
public:
    npc_gurgll_the_tracking_turtle() : CreatureScript("npc_gurgll_the_tracking_turtle") { }

    struct npc_gurgll_the_tracking_turtleAI : public npc_escortAI
    {
        npc_gurgll_the_tracking_turtleAI(Creature* creature) : npc_escortAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetDisableGravity(false);
            me->SetCanFly(false);
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (apply)
            {
                me->SetSpeed(MOVE_RUN, 2.2f);
                me->SetSpeed(MOVE_FLIGHT, 1.0f);
                Start(false, true, who->GetGUID(), NULL, false, false);
                _startPath = true;
            }
        }

        void WaypointReached(uint32 i)
        {
            if (i == 12 && _startPath)
            {
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->CastSpell(me, 49777);
            }

            if (i == 19 && _startPath)
            {
                me->SetDisableGravity(false);
                me->SetCanFly(false);
                me->RemoveAura(49777);
            }

            else if (i == 37 && _startPath)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->AI()->Talk(0);
                    player->GetMotionMaster()->MoveJump(playerjumpposturtle, 10, 12);
                    me->DespawnOrUnsummon(1000);
                    _startPath = false;
                }
            }
            else
                return;
        }

    private:
        bool _startPath = false;
        uint32 _startPathTimer = 1000;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gurgll_the_tracking_turtleAI(creature);
    }
};
// 
class npc_personal_arcane_assistant_azshara : public CreatureScript
{
public:
    npc_personal_arcane_assistant_azshara() : CreatureScript("npc_personal_arcane_assistant_azshara") { }

    struct npc_personal_arcane_assistant_azsharaAI : public ScriptedAI
    {
        npc_personal_arcane_assistant_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->GetMotionMaster()->MoveFollow(player, 1, 1);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                if (!me->GetOwner()->isAlive())
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->DespawnOrUnsummon();
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CombatStop();
                        me->DeleteThreatList();
                        me->GetMotionMaster()->MoveFollow(player, 1, 1);
                    }
                }

                if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_TROUBLE_UNDER_FOOT) == QUEST_STATUS_NONE ||
                    me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_TROUBLE_UNDER_FOOT) == QUEST_STATUS_REWARDED)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->DespawnOrUnsummon(1000);
                    }
                }
            }
            else _checkTimer -= diff;

            if (_checkSpeedTimer <= diff)
            {
                if (me->GetOwner()->ToPlayer()->GetSpeedRate(MOVE_RUN) != me->GetSpeedRate(MOVE_RUN))
                {
                    me->SetSpeed(MOVE_RUN, me->GetOwner()->ToPlayer()->GetSpeedRate(MOVE_RUN));
                    _checkSpeedTimer = 3000;
                }
            }
            else _checkSpeedTimer -= diff;
        }
    private:
        bool _questCompleted = false;

        EventMap _events;
        uint64 _playerGUID = 0;

        uint16 _checkTimer = 1000;
        uint16 _checkSpeedTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_personal_arcane_assistant_azsharaAI(creature);
    }
};

uint32 const AzsharaUnleashedMagicPathSize = 11;
const G3D::Vector3 AzsharaUnleashedMagicPath[AzsharaUnleashedMagicPathSize] =
{
    { 4626.248f, -5970.196f, 170.00f },  // 0
    { 4660.757f, -5974.849f, 171.16f },  // 1
    { 4667.871f, -6058.108f, 219.70f },  // 2
    { 4699.452f, -6264.374f, 198.23f },  // 3
    { 4802.622f, -6481.637f, 128.28f },  // 4
    { 4889.080f, -6601.195f, 68.656f },  // 5
    { 5055.205f, -6538.047f, 88.303f },  // 6
    { 5000.676f, -6403.516f, 138.73f },  // 7
    { 4870.837f, -6357.364f, 60.775f },  // 8
    { 4833.661f, -6396.419f, 37.957f },  // 9
    { 4841.736f, -6442.941f, 19.780f },  // 10
};

class npc_unleashed_magic_vehicle : public CreatureScript
{
public:
    npc_unleashed_magic_vehicle() : CreatureScript("npc_unleashed_magic_vehicle") { }

    struct npc_unleashed_magic_vehicleAI : public npc_escortAI
    {
        npc_unleashed_magic_vehicleAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetSpeed(MOVE_RUN, 3.5f);
            me->CastSpell(me, 44347); // TEMP! wrong aura TODO: no idea the correct cosmetic "arcane barrange effect"
        }

        void UpdateAI(uint32 const diff)
        {
            if (_startFlightTimer <= diff && !_startFlight)
            {
                me->GetMotionMaster()->MoveSmoothPath(AzsharaUnleashedMagicPath, AzsharaUnleashedMagicPathSize);
                _startFlight = true;
            }
            _startFlightTimer -= diff;
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 10 && _startFlight)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY() - 4, me->GetPositionZ(), 10, 12);
                    me->DespawnOrUnsummon();
                    _startFlight = false;
                }
            }
        }
    private:
        bool _startFlight = false;

        uint32 _startFlightTimer = 1000;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unleashed_magic_vehicleAI(creature);
    }
};

enum QuestWatchYourStep
{
    SPELL_ENERGIZED = 68613, // 6 stacks
    SPELL_DETONATION_AURA = 68603,
    SPELL_ARCANE_DETONATION_TARGETTING = 68612,
    SPELL_ARCANE_DETONATION_CAST = 68606,

    QUEST_WATCH_YOUR_STEP = 14296,
    NPC_ZOEY_WIZZLEPARK = 36365,
};

#define GOSSIP_ITEM_XYLEM_ON_TOP "Can you open a portal to your tower for me, Archmage Xylem?"

class npc_archmage_xylem_ontop_azshara : public CreatureScript
{
public:
    npc_archmage_xylem_ontop_azshara() : CreatureScript("npc_archmage_xylem_ontop_azshara") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(creature, 85322);
            creature->AI()->Talk(2);
            player->RemoveAura(151309);
            player->RemoveAura(151310);
            player->RemoveAura(151311);
            player->CastSpell(player, 151315);
        }

        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(creature, 85322);
            player->RemoveAura(151309);
            player->RemoveAura(151310);
            player->RemoveAura(151311);
            player->CastSpell(player, 151315);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(14299) == QUEST_STATUS_COMPLETE && !player->GetAura(151315))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_XYLEM_ON_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        
        else if (player->GetQuestStatus(14391) != QUEST_STATUS_REWARDED)
        {
            if (player->GetQuestStatus(14299) == QUEST_STATUS_REWARDED && !player->GetAura(151315))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_XYLEM_ON_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
        else if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    struct npc_archmage_xylem_ontop_azsharaAI : public ScriptedAI
    {
        npc_archmage_xylem_ontop_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (auto player = who->ToPlayer())
            {
                if (me->IsWithinDistInMap(player, 5.0f))
                {
                    if (player->GetQuestStatus(QUEST_WATCH_YOUR_STEP) == QUEST_STATUS_INCOMPLETE && player->GetAuraCount(SPELL_ENERGIZED) == 6)
                    {
                        player->KilledMonsterCredit(35171);
                        player->RemoveAura(SPELL_ENERGIZED);
                        if (auto zoey = me->FindNearestCreature(NPC_ZOEY_WIZZLEPARK, 20.0f))
                            zoey->AI()->Talk(0);
                        _events.ScheduleEvent(EVENT_XYLEM_1, 2000);
                    }

                    if (player->GetQuestStatus(QUEST_WATCH_YOUR_STEP) == QUEST_STATUS_INCOMPLETE && player->GetAuraCount(SPELL_ENERGIZED) < 6)
                    {
                        player->ToPlayer()->FailQuest(QUEST_WATCH_YOUR_STEP);
                        player->RemoveAura(SPELL_ENERGIZED);
                        me->AI()->Talk(1, player->GetGUID());
                    }
                }
            }
            else
                return;
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_XYLEM_1:
                    me->AI()->Talk(0);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        EventMap _events;

    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archmage_xylem_ontop_azsharaAI(creature);
    }
};

class npc_arcane_detonation_trigger : public CreatureScript
{
public:
    npc_arcane_detonation_trigger() : CreatureScript("npc_arcane_detonation_trigger") { }

    struct npc_arcane_detonation_triggerAI : public ScriptedAI
    {
        npc_arcane_detonation_triggerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_ARCANE_DETONATION_TARGETTING)
            {
                me->CastSpell(who, SPELL_ARCANE_DETONATION_CAST);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (auto player = who->ToPlayer())
            {
                if (me->IsWithinDistInMap(player, 35.0f))
                {
                    if (player->GetQuestStatus(QUEST_WATCH_YOUR_STEP) != QUEST_STATUS_INCOMPLETE)
                        player->RemoveAura(SPELL_DETONATION_AURA);

                    if (player->GetQuestStatus(QUEST_WATCH_YOUR_STEP) == QUEST_STATUS_INCOMPLETE && !player->GetAura(SPELL_DETONATION_AURA))
                        player->CastSpell(player, SPELL_DETONATION_AURA);
                }
            }
            else
                return;
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_detonation_triggerAI(creature);
    }
};


uint32 const AzsharaArcaneConductSize = 3;
const G3D::Vector3 AzsharaArcaneConductPath[AzsharaArcaneConductSize] =
{
    { 4287.688f, -6293.186f, 95.54f, },  // 0
    { 4306.097f, -6283.027f, 94.79f, },  // 1
    { 4329.430f, -6276.893f, 93.61f, },  // 2
};

uint32 const AzsharaArcaneConductSize2 = 2;
const G3D::Vector3 AzsharaArcaneConductPath2[AzsharaArcaneConductSize2] =
{
    { 4338.789f, -6266.059f, 94.256f },  // 0
    { 4353.041f, -6259.375f, 96.749f },  // 1
};

enum NpcArcaneConstruct
{
    SPELL_COSMETIC_ON_GOSSIP = 69639,
    SPELL_GROW = 70416,
    SPELL_CAMERA_SHAKE = 66772,
    SPELL_ARCANE_CHAOS = 68998,

    NPC_DARNASSIAN_APPRENTICE = 36604,

    QUEST_HACKING_THE_CONSTRUCT = 14430,
};

#define GOSSIP_ITEM_ARCANE_CONSTRUCT_GOSSIP_1 "Um... Al'teth ... la c'tolgar? Or something"

class npc_arcane_construct_azshara : public CreatureScript
{
public:
    npc_arcane_construct_azshara() : CreatureScript("npc_arcane_construct_azshara") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            player->KilledMonsterCredit(36599);
            CAST_AI(ScriptedAI, (creature->AI()))->DoAction(ACTION_ARCANE_CONSTRUCT_1);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_HACKING_THE_CONSTRUCT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ARCANE_CONSTRUCT_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_arcane_construct_azsharaAI : public ScriptedAI
    {
        npc_arcane_construct_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void DoAction(const int32 actionid)
        {
            if (actionid == ACTION_ARCANE_CONSTRUCT_1)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->CastSpell(me, SPELL_COSMETIC_ON_GOSSIP);
                me->CastSpell(me, SPELL_GROW);
                me->CastSpell(me, SPELL_CAMERA_SHAKE);
                me->AI()->Talk(0);
                _events.ScheduleEvent(EVENT_ARCANE_CONSTRUCT_1, 1000);
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (!_afterPause && point == 1)
            {
                if (auto enemy1 = me->SummonCreature(NPC_DARNASSIAN_APPRENTICE, 4341.938f, -6285.111f, 94.483f, 2.54f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                    if (auto enemy2 = me->SummonCreature(NPC_DARNASSIAN_APPRENTICE, 4345.760f, -6276.067f, 94.953f, 3.19f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                        if (auto enemy3 = me->SummonCreature(NPC_DARNASSIAN_APPRENTICE, 4341.878f, -6266.670f, 94.561f, 3.99f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            if (auto enemy4 = me->SummonCreature(NPC_DARNASSIAN_APPRENTICE, 4332.521f, -6261.338f, 93.953f, 4.46f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                                if (auto enemy5 = me->SummonCreature(NPC_DARNASSIAN_APPRENTICE, 4316.817f, -6255.881f, 100.22f, 5.34f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                                {
                                    enemy1->HandleEmoteCommand(EMOTE_STATE_READY2H);
                                    enemy2->HandleEmoteCommand(EMOTE_STATE_READY2H);
                                    enemy3->HandleEmoteCommand(EMOTE_STATE_READY2H);
                                    enemy4->HandleEmoteCommand(EMOTE_STATE_READY2H);
                                    enemy5->HandleEmoteCommand(EMOTE_STATE_READY2H);
                                }

            }

            if (!_afterPause && point == 2)
            {
                _events.ScheduleEvent(EVENT_ARCANE_CONSTRUCT_2, 2000);
                _afterPause = true;
            }

            if (_afterPause && point == 1)
            {
                me->Kill(me);
                _afterPause = false;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ARCANE_CONSTRUCT_1:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaArcaneConductPath, AzsharaArcaneConductSize);
                    break;
                case EVENT_ARCANE_CONSTRUCT_2:
                    me->CastSpell(me, SPELL_ARCANE_CHAOS);
                    _events.ScheduleEvent(EVENT_ARCANE_CONSTRUCT_3, 2000);
                    break;
                case EVENT_ARCANE_CONSTRUCT_3:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaArcaneConductPath2, AzsharaArcaneConductSize2);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _afterPause = false;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_construct_azsharaAI(creature);
    }
};

enum SurfaceTransport
{
    NPC_SURFACE_TRANSPORT_SUMMONED = 43534,
    SPELL_DETONATION_VISUAL = 69612,
};

class npc_surface_transport_spellclick : public CreatureScript
{
public:
    npc_surface_transport_spellclick() : CreatureScript("npc_surface_transport_spellclick") { }

    struct npc_surface_transport_spellclickAI : public ScriptedAI
    {
        npc_surface_transport_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    if (auto transport = player->SummonCreature(NPC_SURFACE_TRANSPORT_SUMMONED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
                    {
                        player->CastSpell(transport, SPELL_RIDE_VEHICLE);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->SetVisible(false);
                        _events.ScheduleEvent(EVENT_SURFACE_TRANS_1, 8000);
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
                case EVENT_SURFACE_TRANS_1:
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->SetVisible(true);
                    _playerGUID = NULL;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = NULL;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_surface_transport_spellclickAI(creature);
    }
};

uint32 const SurfaceTransportAzsharaSize = 18;
const G3D::Vector3 SurfaceTransportAzsharaPath[SurfaceTransportAzsharaSize] =
{
    { 3464.375f, -6558.770f, 60.59f }, // 0 
    { 3469.199f, -6547.743f, 76.69f }, // 1 
    { 3488.608f, -6509.274f, 115.17f }, // 2 
    { 3504.604f, -6424.518f, 75.09f }, // 3 
    { 3524.491f, -6359.298f, 35.46f }, // 4 
    { 3533.124f, -6314.991f, 7.579f }, // 5 
    { 3537.666f, -6286.177f, 6.915f }, // 6 
    { 3537.949f, -6258.007f, 7.5f }, // 7 
    { 3542.095f, -6217.479f, 7.5f }, // 8 
    { 3540.944f, -6174.809f, 8.5f }, // 9 
    { 3543.552f, -6133.339f, 9.5f }, // 10 
    { 3628.443f, -5969.853f, 5.000f }, // 11
    { 3589.393f, -5859.785f, 4.000f }, // 12
    { 3466.027f, -5808.312f, 5.597f }, // 13
    { 3394.167f, -5731.742f, 14.76f }, // 14
    { 3442.656f, -5616.338f, 18.09f }, // 15
    { 3480.462f, -5573.725f, 31.60f }, // 16
    { 3495.219f, -5504.464f, 26.81f }, // 17

};

static Position PlayerSurfToGround = { 3486.88f, -5506.218f, 26.886f };

class npc_surface_transport_vehicle : public CreatureScript
{
public:
    npc_surface_transport_vehicle() : CreatureScript("npc_surface_transport_vehicle") { }

    struct npc_surface_transport_vehicleAI : public npc_escortAI
    {
        npc_surface_transport_vehicleAI(Creature* creature) : npc_escortAI(creature) {}

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;
        }

        void IsSummonedBy(Unit* summoner)
        {
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
            me->SetRooted(true);
            me->SetSpeed(MOVE_FLIGHT, 2.5f);
            me->CastSpell(me, 52211);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = passenger->ToPlayer())
                {
                    playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_SURFACE_TRANS_SUM_1, 4000);
                }
            }
        }
        void WaypointReached(uint32 waypointId) {}

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 17)
            {
                me->DespawnOrUnsummon();
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->CastSpell(me, SPELL_DETONATION_VISUAL);
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    player->GetMotionMaster()->MoveJump(PlayerSurfToGround, 12.0f, 15.0f);
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
                case EVENT_SURFACE_TRANS_SUM_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(SurfaceTransportAzsharaPath, SurfaceTransportAzsharaSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
        uint64 playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_surface_transport_vehicleAI(creature);
    }
};

enum GobSquad
{
    SPELL_SQUAD_PET_AURA    = 69555,
    SPELL_SQUAD_PING_TICKER = 69380,
};

class npc_gob_squad_azshara : public CreatureScript
{
public:
    npc_gob_squad_azshara() : CreatureScript("npc_gob_squad_azshara") { }

    struct npc_gob_squad_azsharaAI : public ScriptedAI
    {
        struct npc_gob_squad_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void IntroParachute()
        {
            me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 42, me->GetOrientation());
            me->CastWithDelay(200, me, SPELL_PARACHUTE);
        }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                IntroParachute();
                player->CastSpell(player, SPELL_SQUAD_PET_AURA);
                if (me->GetEntry() == 36780 /*Patch*/)
                {
                    me->AI()->TalkWithDelay(6000, 0);
                }
            }
            switch (me->GetEntry())
            {
            case 36780:
                me->GetMotionMaster()->MoveFollow(summoner, 1, 0);
                me->CastSpell(me, 88403);
                break;
            case 36782:
                me->GetMotionMaster()->MoveFollow(summoner, 1, 1);
                break;
            case 36783:
                me->GetMotionMaster()->MoveFollow(summoner, 1, 2);
                break;
            case 36781:
                me->GetMotionMaster()->MoveFollow(summoner, 1, 3);
                me->CastSpell(me, 88403);
                break;
            case 36750:
                me->GetMotionMaster()->MoveFollow(summoner, 1, 4);
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_SQUAD_PING_TICKER && me->GetEntry() == 36750 /*Ticker*/ && !_isUnderPlanting)
            {
                _isUnderPlanting = true;
                if (auto patch = me->FindNearestCreature(36780, 40))
                    patch->AI()->Talk(1);

                _runestoneTriggerGUID = who->GetGUID();
                me->CastSpell(who, SPELL_DUMMY_PING);
                _events.ScheduleEvent(EVENT_SQUAD_TICKER_1, 2000);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            if (_checkZoneTimer <= diff)
            {
                _checkZoneTimer = 1000;
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetAreaId() != 1221 /*Ruins of Eldarath*/ || !me->GetOwner())
                        if (player->GetAreaId() != 1223 /*Temple of Zin-Malor*/)
                        {
                            player->RemoveAura(SPELL_SQUAD_PET_AURA);
                            me->DespawnOrUnsummon();

                            if (_isUnderPlanting)
                            {
                                if (auto runestone = Unit::GetCreature(*me, _runestoneTriggerGUID))
                                    runestone->AI()->DoAction(ACTION_RUNESTONE_TRIGGER_DESPAWN);
                            }
                        }
                    
                    if (player->GetQuestStatus(14480) == QUEST_STATUS_REWARDED &&
                        player->GetQuestStatus(14486) == QUEST_STATUS_REWARDED &&
                        player->GetQuestStatus(14484) == QUEST_STATUS_REWARDED &&
                        player->GetQuestStatus(14487) == QUEST_STATUS_REWARDED &&
                        player->GetQuestStatus(14485) == QUEST_STATUS_REWARDED)
                    {
                        player->RemoveAura(SPELL_SQUAD_PET_AURA);
                        player->DestroyItemCount(49629, 1, true);
                        me->DespawnOrUnsummon();

                        if (_isUnderPlanting)
                        {
                            if (auto runestone = Unit::GetCreature(*me, _runestoneTriggerGUID))
                                runestone->AI()->DoAction(ACTION_RUNESTONE_TRIGGER_DESPAWN);
                        }
                    }

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && !_isUnderPlanting)
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->AI()->AttackStart(player->getVictim());
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim())
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CombatStop();
                            me->DeleteThreatList();

                            switch (me->GetEntry())
                            {
                            case 36780:
                                me->GetMotionMaster()->MoveFollow(player, 1, 0);
                                break;
                            case 36782:
                                me->GetMotionMaster()->MoveFollow(player, 1, 1);
                                break;
                            case 36783:
                                me->GetMotionMaster()->MoveFollow(player, 1, 2);
                                break;
                            case 36781:
                                me->GetMotionMaster()->MoveFollow(player, 1, 3);
                                break;
                            default:
                                break;
                            }
                            // for ticker
                            if (me->GetEntry() == 36750 && !_isUnderPlanting)
                            {
                                me->GetMotionMaster()->MoveFollow(player, 1, 4);
                            }
                        }
                    }
                }
            }
            else
                _checkZoneTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SQUAD_TICKER_1:
                    if (auto runestone = Unit::GetCreature(*me, _runestoneTriggerGUID))
                    {
                        me->AI()->Talk(0);
                        me->CombatStop();
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->MovePoint(1, runestone->GetPositionX() + 3, runestone->GetPositionY(), runestone->GetPositionZ());
                        _events.ScheduleEvent(EVENT_SQUAD_TICKER_2, 3000);
                    }
                    break;
                case EVENT_SQUAD_TICKER_2:
                    me->SetRooted(true);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                    _events.ScheduleEvent(EVENT_SQUAD_TICKER_3, 4000);
                    break;
                case EVENT_SQUAD_TICKER_3:
                    if (auto enemy1 = me->SummonCreature(7885, me->GetPositionX() + 9, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000))
                        enemy1->AI()->AttackStart(me);

                    if (auto newt = me->FindNearestCreature(36782, 40))
                        newt->AI()->TalkWithDelay(1500, 0);

                    _events.ScheduleEvent(EVENT_SQUAD_TICKER_4, 8000);
                    break;
                case EVENT_SQUAD_TICKER_4:
                    if (auto enemy1 = me->SummonCreature(7885, me->GetPositionX() - 9, me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000))
                        enemy1->AI()->AttackStart(me);

                    if (auto enemy2 = me->SummonCreature(7885, me->GetPositionX(), me->GetPositionY() - 9, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000))
                        enemy2->AI()->AttackStart(me);

                    if (auto newt = me->FindNearestCreature(36782, 40))
                        newt->AI()->TalkWithDelay(1500, 0);
                    me->AI()->TalkWithDelay(6000, 1);
                    _events.ScheduleEvent(EVENT_SQUAD_TICKER_5, 15000);
                    break;
                case EVENT_SQUAD_TICKER_5:
                    me->SetRooted(false);
                    me->SetReactState(REACT_PASSIVE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        me->GetMotionMaster()->MoveFollow(player, 2, 2);

                    me->AI()->TalkWithDelay(1000, 2);

                    if (auto runestone = Unit::GetCreature(*me, _runestoneTriggerGUID))
                        runestone->AI()->DoAction(ACTION_RUNESTONE_TRIGGER_END);

                    _runestoneTriggerGUID = 0;
                    _isUnderPlanting = false;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isUnderPlanting = false;
        uint32 _checkZoneTimer = 1000;
        uint64 _playerGUID = NULL;
        uint64 _runestoneTriggerGUID = 0;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new struct npc_gob_squad_azsharaAI(creature);
    }
};

class npc_runestone_azshara_trigger : public CreatureScript
{
public:
    npc_runestone_azshara_trigger() : CreatureScript("npc_runestone_azshara_trigger") { }

    struct npc_runestone_azshara_triggerAI : public ScriptedAI
    {
        npc_runestone_azshara_triggerAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            if (auto runestone = me->FindNearestGameObject(199333, 10))
            {
                _runestoneGUID = runestone->GetGUID();
                _doPing = true;

                if (!runestone->isSpawned())
                    runestone->Respawn();
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_RUNESTONE_TRIGGER_DESPAWN)
            {
                if (auto runestone = GameObject::GetGameObject(*me, _runestoneGUID))

                    runestone->SetLootState(GO_JUST_DEACTIVATED);
                me->DespawnOrUnsummon();
            }

            if (action == ACTION_RUNESTONE_TRIGGER_END)
            {
                _events.ScheduleEvent(EVENT_RUNESTONE_EXPLOSTION, 3000);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                _doPing = false;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_pingTimer <= diff && _doPing)
            {
                _pingTimer = 3000;
                me->CastSpell(me, SPELL_SQUAD_PING_TICKER);
            }
            else
                _pingTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RUNESTONE_EXPLOSTION:
                    if (auto runestone = GameObject::GetGameObject(*me, _runestoneGUID))
                    {
                        runestone->SetLootState(GO_JUST_DEACTIVATED);
                        me->CastSpell(me, 69417); /* Exploding Runestone */
                        {
                            std::list<Player*> player = me->GetPlayersInRange(40, true);
                            for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                                (*it)->KilledMonsterCredit(36843);
                        }
                        me->DespawnOrUnsummon(2000);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        uint32 _pingTimer = 2000;
        uint64 _runestoneGUID = 0;
        bool _doPing = false;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runestone_azshara_triggerAI(creature);
    }
};

uint32 const AzsharaMilitaryGyroPathSize = 12;
const G3D::Vector3 AzsharaMilitaryGyroPath[AzsharaMilitaryGyroPathSize] =
{
    { 3778.523f, -5264.313f, 85.161f, },
    { 3799.561f, -5322.819f, 91.19f, },
    { 3791.874f, -5420.572f, 92.560f, },
    { 3820.829f, -5460.894f, 92.560f, },
    { 3883.230f, -5541.769f, 72.560f, },
    { 3848.853f, -5891.142f, 50.340f, },
    { 3822.447f, -6215.426f, 55.564f, },
    { 3747.663f, -6428.193f, 61.815f, },
    { 3641.041f, -6544.859f, 77.776f, },
    { 3542.201f, -6625.270f, 103.76f, },
    { 3531.364f, -6656.844f, 108.52f, },
    { 3530.134f, -6687.077f, 96.868f, },
};

class npc_azshara_military_gyrocopter : public CreatureScript
{
public:
    npc_azshara_military_gyrocopter() : CreatureScript("npc_azshara_military_gyrocopter") { }

    struct npc_azshara_military_gyrocopterAI : public npc_escortAI
    {
        npc_azshara_military_gyrocopterAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            summoner->CastSpell(me, SPELL_RIDE_VEHICLE);
            me->SetSpeed(MOVE_RUN, 3.5f);
        }

        void UpdateAI(uint32 const diff)
        {
            if (_startFlightTimer <= diff && !_startFlight)
            {
                _startFlightTimer = 2000;
                me->GetMotionMaster()->MoveSmoothPath(AzsharaMilitaryGyroPath, AzsharaMilitaryGyroPathSize);
                _startFlight = true;
            }
            _startFlightTimer -= diff;
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 11 && _startFlight)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->GetMotionMaster()->MoveJump(me->GetPositionX() - 3, me->GetPositionY() + 1, me->GetPositionZ(), 10, 12);
                    player->SetOrientation(5.0799f);
                    me->DespawnOrUnsummon();
                    _startFlight = false;
                }
            }
        }
    private:
        bool _startFlight = false;
        uint32 _startFlightTimer = 500;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azshara_military_gyrocopterAI(creature);
    }
};

class spell_gob_squad_flare : public SpellScriptLoader
{
public:
    spell_gob_squad_flare() : SpellScriptLoader("spell_gob_squad_flare") { }

    class spell_gob_squad_flare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gob_squad_flare_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            caster->CastWithDelay(1000, caster, 69311); // Summon Patch 36780
            caster->CastWithDelay(1000, caster, 69312); // Summon Newt 	36782
            caster->CastWithDelay(1000, caster, 69309); // Summon Grit 	36783
            caster->CastWithDelay(1000, caster, 69313); // Summon Volt	36781
            caster->CastWithDelay(1000, caster, 69314); // Summon Ticker 36750
        }
        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gob_squad_flare_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_gob_squad_flare_SpellScript();
    }
};

class areatrigger_azshara_the_secret_lab : public AreaTriggerScript
{
public:
    areatrigger_azshara_the_secret_lab()
        : AreaTriggerScript("areatrigger_azshara_the_secret_lab")
    {
    }
    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
    {
        if (player->GetQuestStatus(QUEST_RAPTOR_RAPTOR_ROCKET) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->HasAura(SPELL_BABYSITTING_RAPTORS))
            {
                uint8 stackcount = player->GetAuraCount(SPELL_BABYSITTING_RAPTORS);
                for (uint8 i = 0; i < stackcount; i++)
                {
                    player->KilledMonsterCredit(36534);
                }
                player->CastSpell(player, SPELL_EJECT_ALL_PASSENGERS);
                player->CastWithDelay(50, player, SPELL_PING_BABYSITTING_RAPTORS);
                player->RemoveAura(SPELL_BABYSITTING_RAPTORS);
            }
        }
        return false;
    }
};
// till idk how to update spellbar in other way a bit haxy is here 
class spell_runaway_shredder_update_spellbar : public SpellScriptLoader
{
public:
    spell_runaway_shredder_update_spellbar() : SpellScriptLoader("spell_runaway_shredder_update_spellbar") { }

    class spell_runaway_shredder_update_spellbar_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_runaway_shredder_update_spellbar_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto player = GetCaster())
                if (auto dest = GetHitCreature())
                {
                    dest->GetVehicleKit()->EjectPassenger(player);
                    player->CastWithDelay(0, dest, SPELL_RIDE_VEHICLE);
                }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_runaway_shredder_update_spellbar_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_runaway_shredder_update_spellbar_SpellScript();
    }
};

// 67964 
class spell_besalt_bunny_spell_spawner : public SpellScriptLoader
{
public:
    spell_besalt_bunny_spell_spawner() : SpellScriptLoader("spell_besalt_bunny_spell_spawner") { }

    class spell_besalt_bunny_spell_spawner_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_besalt_bunny_spell_spawner_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto caster = GetCaster())
            {
                caster->CastSpell(caster, 67960); // Summon Besalt Bunny
                caster->CastSpell(caster, 67961); // Summon Besalt Bunny
                caster->CastSpell(caster, 67962); // Summon Besalt Bunny
                caster->CastSpell(caster, 67962); // Summon Besalt Bunny
                caster->CastSpell(caster, 67962); // Summon Besalt Bunny
                caster->HandleEmoteCommand(EMOTE_ONESHOT_WOUND_CRITICAL);
            }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_besalt_bunny_spell_spawner_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_besalt_bunny_spell_spawner_SpellScript();
    }
};

class npc_wings_of_steel_spellclick : public CreatureScript
{
public:
    npc_wings_of_steel_spellclick() : CreatureScript("npc_wings_of_steel_spellclick") { }

    struct npc_wings_of_steel_spellclickAI : public ScriptedAI
    {
        npc_wings_of_steel_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    if (auto airplane = player->SummonCreature(37139, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
                    {
                        player->CastSpell(airplane, SPELL_RIDE_VEHICLE_SEAT_02);
                        player->KilledMonsterCredit(74960);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->SetVisible(false);
                        _events.ScheduleEvent(EVENT_AIRBORNE_SPELLCLICK, 15000);
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
                case EVENT_AIRBORNE_SPELLCLICK:
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->SetVisible(true);
                    _playerGUID = NULL;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = NULL;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wings_of_steel_spellclickAI(creature);
    }
};

uint32 const AzsharaWingsOfSteelPathSize = 16;
const G3D::Vector3 AzsharaWingsOfSteelPath[AzsharaWingsOfSteelPathSize] =
{
    { 3598.143f, -6735.666f, 86.070f, },
    { 3648.121f, -6766.064f, 94.818f, },
    { 3693.572f, -6789.290f, 104.665f, },
    { 3824.602f, -6802.274f, 121.540f, },
    { 4004.728f, -6746.646f, 138.756f, },
    { 4132.694f, -6334.678f, 128.979f, },
    { 4137.262f, -5851.200f, 183.808f, },
    { 4006.637f, -5585.204f, 165.369f, },
    { 3800.970f, -5374.453f, 184.715f, },
    { 3728.270f, -4977.077f, 185.488f, },
    { 3616.378f, -4846.019f, 222.826f, },
    { 3270.852f, -4921.681f, 249.234f, },
    { 3012.903f, -4583.831f, 300.744f, },
    { 3109.884f, -4310.421f, 272.732f, },
    { 3001.062f, -4139.711f, 160.263f, },
    { 2916.981f, -4084.540f, 173.505f, },
};

uint32 const AzsharaWingsOfSteelPathSize2 = 15;
const G3D::Vector3 AzsharaWingsOfSteelPath2[AzsharaWingsOfSteelPathSize2] =
{
    { 3519.417f, -6808.054f, 87.089f, },
    { 3519.928f, -6841.252f, 94.768f, },
    { 3519.587f, -6891.614f, 111.61f, },
    { 3597.304f, -7006.590f, 124.23f, },
    { 3744.396f, -7016.013f, 121.78f, },
    { 3772.265f, -6852.322f, 111.03f, },
    { 3568.662f, -6362.050f, 109.24f, },
    { 3418.374f, -5780.766f, 85.755f, },
    { 3272.783f, -5551.112f, 98.618f, },
    { 3116.183f, -5360.431f, 170.67f, },
    { 3013.380f, -4928.916f, 202.74f, },
    { 3055.236f, -4435.231f, 201.91f, },
    { 3104.567f, -4259.404f, 199.56f, },
    { 3012.517f, -4150.576f, 149.03f, },
    { 2893.900f, -4071.176f, 159.65f, },
};

uint32 const AzsharaWingsOfSteelPathSize3 = 17;
const G3D::Vector3 AzsharaWingsOfSteelPath3[AzsharaWingsOfSteelPathSize3] =
{
    { 3432.314f, -6752.930f, 84.633f, } ,
    { 3403.952f, -6765.738f, 98.812f, } ,
    { 3364.240f, -6774.331f, 108.80f, } ,
    { 3312.277f, -6743.876f, 89.397f, } ,
    { 3273.416f, -6563.988f, 70.778f, } ,
    { 3321.541f, -6166.822f, 78.243f, } ,
    { 3403.397f, -5840.697f, 92.472f, } ,
    { 3593.539f, -5587.754f, 128.94f, } ,
    { 3732.379f, -5407.935f, 166.37f, } ,
    { 3750.813f, -5149.458f, 182.05f, } ,
    { 3723.483f, -4926.706f, 220.39f, } ,
    { 3678.779f, -4588.167f, 221.60f, } ,
    { 3568.081f, -4239.049f, 214.76f, } ,
    { 3473.672f, -4245.525f, 217.38f, } ,
    { 3127.098f, -4245.295f, 185.17f, } ,
    { 3012.521f, -4149.638f, 197.43f, } ,
    { 2898.972f, -4071.174f, 173.59f, } ,
};

static Position GetOutFromAirplane = { 3001.557f, -4151.456f, 101.537f };

class npc_wings_of_steel_airplane : public CreatureScript
{
public:
    npc_wings_of_steel_airplane() : CreatureScript("npc_wings_of_steel_airplane") { }

    struct npc_wings_of_steel_airplaneAI : public ScriptedAI
    {
        npc_wings_of_steel_airplaneAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetDisableGravity(false);
            me->SetCanFly(false);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->SetRooted(true);
                _events.ScheduleEvent(EVENT_AIRPLANE_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY && !_eventStarted)
            {
                _eventStarted = true;
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->GetMotionMaster()->Clear();
                me->CastSpell(me, 54422);

                switch (who->ToCreature()->GetDBTableGUIDLow())
                {
                case 180138: /* left */
                    _events.ScheduleEvent(EVENT_AIRPLANE_2, 1000);
                    _pathLeft = true;
                    break;
                case 180136: /* middle */
                    _events.ScheduleEvent(EVENT_AIRPLANE_3, 1000);
                    _pathMiddle = true;
                    break;
                case 180137: /* right */
                    _events.ScheduleEvent(EVENT_AIRPLANE_4, 1000);
                    _pathRight = true;
                    break;
                default:
                    break;
                }
            }

        }
        void MovementInform(uint32 type, uint32 point) override
        {
            if (type == SPLINE_MOTION_TYPE)
            {
                if (point == 14 && _pathLeft)
                {
                    me->DespawnOrUnsummon(6000);
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_PASSENGER_2);
                        player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        player->GetMotionMaster()->MoveJump(GetOutFromAirplane, 20, 20);
                    }
                }

                if (point == 13 && _pathMiddle)
                {
                    me->DespawnOrUnsummon(6000);
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_PASSENGER_2);
                        player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        player->GetMotionMaster()->MoveJump(GetOutFromAirplane, 20, 20);
                    }
                }

                if (point == 15 && _pathRight)
                {
                    me->DespawnOrUnsummon(6000);
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_PASSENGER_2);
                        player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                        player->GetMotionMaster()->MoveJump(GetOutFromAirplane, 20, 20);
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
                case EVENT_AIRPLANE_1:
                    me->SetRooted(false);
                    if (auto trigger = me->FindNearestCreature(37141, 50))
                        me->GetMotionMaster()->MovePoint(1, trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ());
                    break;
                case EVENT_AIRPLANE_2:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaWingsOfSteelPath, AzsharaWingsOfSteelPathSize);
                    break;
                case EVENT_AIRPLANE_3:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaWingsOfSteelPath2, AzsharaWingsOfSteelPathSize2);
                    break;
                case EVENT_AIRPLANE_4:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaWingsOfSteelPath3, AzsharaWingsOfSteelPathSize3);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _pathLeft = false;
        bool _pathMiddle = false;
        bool _pathRight = false;
        bool _eventStarted = false;       
        uint64 _playerGUID = 0;
        uint64 _takeOffControllerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wings_of_steel_airplaneAI(creature);
    }
};

enum Sharpshivdatas
{
    NPC_SLINKY_SHARPSHIV = 36729,

    NPC_GRAPPLE_PLAYER = 36707,
    NPC_CLIMB_PLAYER_1 = 36716,
    NPC_CLIMB_PLAYER_2 = 36718,
    NPC_CLIMB_PLAYER_3 = 36720,

    NPC_GRAPPLE_SHARPSHIV = 36738,
    NPC_CLIMB_SHARPSHIV_1 = 36753,
    NPC_CLIMB_SHARPSHIV_2 = 36754,
    NPC_CLIMB_SHARPSHIV_3 = 36755,

    NPC_SHARPSHIV_EXPLOSIVES = 36758,

    QUEST_LIGHTNING_STRIKE_ASSASSINATION = 14464,

    SPELL_ROPE_TOWER = 66206,

};

uint32 const AzsharaSlinkySharpsivPathSize = 5;
const G3D::Vector3 AzsharaSlinkySharpsivPath[AzsharaSlinkySharpsivPathSize] =
{
    { 2889.279f, -4024.420f, 184.02f, },
    { 2941.376f, -4079.742f, 154.12f, },
    { 2978.739f, -4112.083f, 133.30f, },
    { 2992.794f, -4136.739f, 121.41f, },
    { 2974.877f, -4164.275f, 103.15f, },
};

#define GOSSIP_ITEM_SLINKY_SHARPSHIV_GOSSIP_1 "I'm ready, Slinky. Let's do this."

class npc_slinky_sharpshiv_q_giver : public CreatureScript
{
public:
    npc_slinky_sharpshiv_q_giver() : CreatureScript("npc_slinky_sharpshiv_q_giver") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();
        if (uiAction == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            player->RemoveAura(49416);
            player->SummonCreature(NPC_SLINKY_SHARPSHIV, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_LIGHTNING_STRIKE_ASSASSINATION) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SLINKY_SHARPSHIV_GOSSIP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        else if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_slinky_sharpshiv_q_giverAI : public ScriptedAI
    {
        npc_slinky_sharpshiv_q_giverAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->RemoveAura(49414);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            _events.ScheduleEvent(EVENT_SHARPSHIV_1, 1500);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _despawnInProgress = false;
                me->GetMotionMaster()->MoveJump(2882.671f, -4024.391f, 176.12f, 10, 5);
                _events.ScheduleEvent(EVENT_SHARPSHIV_7, 1000);

                std::list<Unit*> explosive = passenger->SelectNearbyUnits(NPC_SHARPSHIV_EXPLOSIVES, 50.0f);
                for (std::list<Unit*>::iterator itr = explosive.begin(); itr != explosive.end(); ++itr)
                    if (auto explosive = (*itr))
                        explosive->CastSpell(explosive, SPELL_DUMMY);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 4)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon(200);
                me->RemoveAura(70988);

                if (auto parachute = Unit::GetCreature(*me, _parachuteGUID))
                    parachute->DespawnOrUnsummon();
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_REMOVE_HOOK_EFFECT)
            {
                if (auto hook = Unit::GetCreature(*me, _hookPlayerGUID))
                    if (auto hook2 = Unit::GetCreature(*me, _hookSharpshivGUID))
                    {
                        _reachedTop = true;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        hook->RemoveAura(SPELL_ROPE_TOWER);
                        hook2->RemoveAura(SPELL_ROPE_TOWER);
                        hook->DespawnOrUnsummon();
                        hook2->DespawnOrUnsummon();
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
                case EVENT_SHARPSHIV_1:
                    me->AI()->Talk(0, _playerGUID);
                    me->SetFacingTo(2.14f);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);

                    if (auto hook = me->SummonCreature(NPC_GRAPPLE_SHARPSHIV, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000))
                    {
                        _hookSharpshivGUID = hook->GetGUID();
                        hook->CastSpell(me, SPELL_ROPE_TOWER);
                        hook->GetMotionMaster()->MoveJump(2873.32f, -4037.46f, 187.42f, 25, 15);
                    }

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (auto hook2 = player->SummonCreature(NPC_GRAPPLE_PLAYER, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000))
                        {
                            player->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);
                            _hookPlayerGUID = hook2->GetGUID();
                            hook2->CastSpell(player, SPELL_ROPE_TOWER);
                            hook2->GetMotionMaster()->MoveJump(2870.18f, -4038.71f, 187.5f, 25, 15);
                        }
                    }
                    _events.ScheduleEvent(EVENT_SHARPSHIV_2, 3000);
                    break;
                case EVENT_SHARPSHIV_2:
                    me->CastSpell(me->FindNearestCreature(NPC_CLIMB_SHARPSHIV_1, 60.0f), SPELL_RIDE_VEHICLE);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->CastSpell(player->FindNearestCreature(NPC_CLIMB_PLAYER_1, 60.0f), SPELL_RIDE_VEHICLE);
                    break;
                case EVENT_SHARPSHIV_3:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
                    _events.ScheduleEvent(EVENT_SHARPSHIV_4, 2000);
                    break;
                case EVENT_SHARPSHIV_4:
                    if (auto explosives1 = me->SummonCreature(NPC_SHARPSHIV_EXPLOSIVES, 2866.134f, -4025.855f, 179.77f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 90000))
                        if (auto explosives2 = me->SummonCreature(NPC_SHARPSHIV_EXPLOSIVES, 2869.959f, -4021.394f, 179.77f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 90000))
                            if (auto explosives3 = me->SummonCreature(NPC_SHARPSHIV_EXPLOSIVES, 2863.556f, -4031.413f, 179.77f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 90000))
                                if (auto explosives4 = me->SummonCreature(NPC_SHARPSHIV_EXPLOSIVES, 2873.211f, -4028.938f, 179.77f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 90000))
                                {
                                    me->AI()->Talk(2, _playerGUID);
                                    _events.ScheduleEvent(EVENT_SHARPSHIV_5, 1500);
                                }
                    break;
                case EVENT_SHARPSHIV_5:
                    me->GetMotionMaster()->MoveJump(2875.285f, -4025.164f, 180.944f, 10, 8);
                    _events.ScheduleEvent(EVENT_SHARPSHIV_6, 1000);
                    break;
                case EVENT_SHARPSHIV_6:
                    _despawnInProgress = true;
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);
                    me->AI()->Talk(3, _playerGUID);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED | UNIT_FLAG_NOT_SELECTABLE);

                    if (auto parachute = me->SummonCreature(36761, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 90000))
                    {
                        _parachuteGUID = parachute->GetGUID();
                        parachute->CastWithDelay(0, me, SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_SHARPSHIV_7:
                    me->GetMotionMaster()->MoveSmoothPath(AzsharaSlinkySharpsivPath, AzsharaSlinkySharpsivPathSize);
                    me->CastSpell(me, 70988); // parachute
                    break;
                default:
                    break;
                }
            }

            if (_despawnInProgress)
            {
                if (_despawnTimer <= diff)
                {
                    if (auto parachute = Unit::GetCreature(*me, _parachuteGUID))
                    {
                        _despawnTimer = 100000;
                        parachute->DespawnOrUnsummon();
                        me->DespawnOrUnsummon();
                    }
                }
                else _despawnTimer -= diff;
            }

            if (_reachedTop)
                if (!me->FindNearestCreature(36680, 40) && !me->FindNearestCreature(36687, 30))
                {
                    _reachedTop = false;
                    me->CombatStop();
                    me->GetMotionMaster()->MoveJump(2867.256f, -4026.04f, 179.77f, 10, 10);
                    me->AI()->TalkWithDelay(1000, 1, _playerGUID);
                    _events.ScheduleEvent(EVENT_SHARPSHIV_3, 2500);
                }
            DoMeleeAttackIfReady();
        }
    private:
        bool _reachedTop = false;

        bool _despawnInProgress = false;
        uint32 _despawnTimer = 20000;

        uint64 _parachuteGUID = 0;
        uint64 _playerGUID = 0;
        uint64 _hookPlayerGUID = 0;
        uint64 _hookSharpshivGUID = 0;

        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slinky_sharpshiv_q_giverAI(creature);
    }
};

class npc_climbing_slinky_player : public CreatureScript
{
public:
    npc_climbing_slinky_player() : CreatureScript("npc_climbing_slinky_player") { }

    struct npc_climbing_slinky_playerAI : public ScriptedAI
    {
        npc_climbing_slinky_playerAI(Creature* creature) : ScriptedAI(creature) {}

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply)
            {
                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();

                if (auto sharpshiv = passenger->ToCreature())
                    if (passenger->GetEntry() == NPC_SLINKY_SHARPSHIV)
                        _sharpshivGUID = sharpshiv->GetGUID();

                switch (me->GetEntry())
                {
                case NPC_CLIMB_PLAYER_1:
                    _events.ScheduleEvent(EVENT_CLIMBING_1, 1500);
                    break;
                case NPC_CLIMB_PLAYER_2:
                    _events.ScheduleEvent(EVENT_CLIMBING_3, 2800);
                    break;
                case NPC_CLIMB_PLAYER_3:
                    _events.ScheduleEvent(EVENT_CLIMBING_5, 1800);
                    break;
                case NPC_CLIMB_SHARPSHIV_1:
                    _events.ScheduleEvent(EVENT_CLIMBING_2, 2500);
                    break;
                case NPC_CLIMB_SHARPSHIV_2:
                    _events.ScheduleEvent(EVENT_CLIMBING_4, 2800);
                    break;
                case NPC_CLIMB_SHARPSHIV_3:
                    _events.ScheduleEvent(EVENT_CLIMBING_6, 1500);
                    break;
                default:
                    break;
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
                case EVENT_CLIMBING_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->CastWithDelay(200, me->FindNearestCreature(NPC_CLIMB_PLAYER_2, 30), SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_CLIMBING_2:
                    if (auto sharpshiv = Unit::GetCreature(*me, _sharpshivGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        sharpshiv->CastWithDelay(200, me->FindNearestCreature(NPC_CLIMB_SHARPSHIV_2, 30), SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_CLIMBING_3:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->CastWithDelay(200, me->FindNearestCreature(NPC_CLIMB_PLAYER_3, 30), SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_CLIMBING_4:
                    if (auto sharpshiv = Unit::GetCreature(*me, _sharpshivGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        sharpshiv->CastWithDelay(200, me->FindNearestCreature(NPC_CLIMB_SHARPSHIV_3, 30), SPELL_RIDE_VEHICLE);
                    }
                    break;
                case EVENT_CLIMBING_5:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(2865.802f, -4032.34f, 179.77f, 10, 7);
                        _playerGUID = 0;
                    }
                    break;
                case EVENT_CLIMBING_6:
                    if (auto sharpshiv = Unit::GetCreature(*me, _sharpshivGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        sharpshiv->GetMotionMaster()->MoveJump(2871.547f, -4033.58f, 180.93f, 17, 4);
                        sharpshiv->SetHomePosition(2870.0197f, -4030.549f, 179.778f, 1.5054f);
                        sharpshiv->AI()->DoAction(ACTION_REMOVE_HOOK_EFFECT);
                        _sharpshivGUID = 0;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _sharpshivGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_climbing_slinky_playerAI(creature);
    }
};

enum PushTheButton
{
    SPELL_PING_TRIGGER_DETONATION = 151306,
};

class npc_pushthebutton_master : public CreatureScript
{
public:
    npc_pushthebutton_master() : CreatureScript("npc_pushthebutton_master") { }

    struct npc_pushthebutton_masterAI : public ScriptedAI
    {
        npc_pushthebutton_masterAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_TRIGGER_DETONATION && !_inProgress)
                if (auto player = who->ToPlayer())
                    if (player->GetQuestStatus(14477) == QUEST_STATUS_INCOMPLETE)
                    {
                        player->KilledMonsterCredit(74958);
                        player->CastSpell(me, 69235);
                        _inProgress = true;
                        _events.RescheduleEvent(EVENT_PUSHTHEBUTTON_1, 1500);

                        if (auto gorek = me->FindNearestCreature(36662, 10))
                            gorek->AI()->SetData(1, 1);
                    }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PUSHTHEBUTTON_1:
                {
                    std::list<Unit*> triggers = me->SelectNearbyUnits(74959, 120.0f);
                    for (std::list<Unit*>::iterator itr = triggers.begin(); itr != triggers.end(); ++itr)
                        if (auto triggers = (*itr))
                        {
                            triggers->ToCreature()->AI()->SetData(1, 1);
                            _events.RescheduleEvent(EVENT_PUSHTHEBUTTON_2, 1250);
                        }
                }
                break;

                case EVENT_PUSHTHEBUTTON_2:
                {
                    std::list<Unit*> triggers = me->SelectNearbyUnits(74959, 120.0f);
                    for (std::list<Unit*>::iterator itr = triggers.begin(); itr != triggers.end(); ++itr)
                        if (auto triggers = (*itr))
                        {
                            triggers->ToCreature()->AI()->SetData(1, 2);
                            _inProgress = false;
                        }
                }
                break;
                default:
                    break;
                }
            }
        }

    private:
        bool _inProgress = false;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pushthebutton_masterAI(creature);
    }
};

class npc_valormok_wind_rider : public CreatureScript
{
public:
    npc_valormok_wind_rider() : CreatureScript("npc_valormok_wind_rider") { }

    struct npc_valormok_wind_riderAI : public VehicleAI
    {
        npc_valormok_wind_riderAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void WaypointReached(uint32 i) {}

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (_isInAttackArea)
                {
                    if (me->GetAreaId() != 1237)
                        if (me->GetAreaId() != 3137)
                        {
                            _isInAttackArea = false;
                            me->AI()->Talk(0, _playerGUID);
                            _events.ScheduleEvent(EVENT_VALORMOK_WIND_RIDER_1, 10000);
                        }
                }
                else
                {
                    if (me->GetAreaId() == 1237 || me->GetAreaId() == 3137)
                    {
                        _events.CancelEvent(EVENT_VALORMOK_WIND_RIDER_1);
                        _isInAttackArea = true;
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VALORMOK_WIND_RIDER_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->CastSpell(player, 44795);
                        me->DespawnOrUnsummon();
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isInAttackArea = true;
        bool _talkedAlready = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valormok_wind_riderAI(creature);
    }
};

// #### TRIAL OF FIRE ###### 
enum TrialOfFire
{
    SPELL_FIRE_DANCING = 66212,
    SPELL_FIRE_PUZZLE_CREDIT = 68648,
    SPELL_FLAME_STRIKE = 66190,
    SPELL_PRE_STRIKE = 66189,
    SPELL_ACTIVATION_TRIGGER = 66194,
    SPELL_DUMMY_SELECT_TARGETS = 151307,

    NPC_FIRE_PILLAR = 34773,
    NPC_DARWIN = 36368,
    NPC_XYLEM_FIRE = 36335,
};

class npc_trial_of_fire_controller_azshara : public CreatureScript
{
public:
    npc_trial_of_fire_controller_azshara() : CreatureScript("npc_trial_of_fire_controller_azshara") { }

    struct npc_trial_of_fire_controller_azsharaAI : public ScriptedAI
    {
        npc_trial_of_fire_controller_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (!_introDone && me->IsWithinDistInMap(who, 35.0f))
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                {
                    GetPillars();
                    _introDone = true;
                }
            }
        }
        void GetPillars()
        {
            std::list<Unit*> pillars = me->SelectNearbyUnits(NPC_FIRE_PILLAR, 30.0f);
            for (std::list<Unit*>::iterator itr = pillars.begin(); itr != pillars.end(); ++itr)
                (*itr)->ToCreature()->AI()->DoAction(ACTION_FIRE_PILLAR_1);

            _events.ScheduleEvent(EVENT_ACTION_TRIAL_FIRE_CONTROLLER_1, 500);
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ACTION_TRIAL_FIRE_CONTROLLER_1:
                    me->CastSpell(me, SPELL_DUMMY_SELECT_TARGETS);
                    _events.ScheduleEvent(EVENT_ACTION_TRIAL_FIRE_CONTROLLER_2, 6000);
                    break;
                case EVENT_ACTION_TRIAL_FIRE_CONTROLLER_2:
                    GetPillars();
                    break;
                default:
                    break;
                }
            }

            if (_checkTimer <= diff)
            {
                if (auto player = me->FindNearestPlayer(50))
                {
                    if (!player)
                    {
                        _events.CancelEvent(EVENT_ACTION_TRIAL_FIRE_CONTROLLER_2);
                        _events.CancelEvent(EVENT_ACTION_TRIAL_FIRE_CONTROLLER_1);
                        _introDone = false;
                    }

                    // TODO - Need a code to RemoveCriteriaProgress from player
                    //if (player)
                    //{
                    //    std::list<Player*> player = me->GetPlayersInRange(50, true);
                    //    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                    //        if (!(*it)->HasAchieved(5448))
                    //        {
                    //            if ((*it)->GetAura(SPELL_FLAME_STRIKE))
                    //            {
                    //                
                    //            }
                    //        }
                    //}
                    _checkTimer = 1000;
                }
            }
            else _checkTimer -= diff;
        }
    private:
        uint32 _checkTimer = 1000;

        bool _introDone = false;
        uint8 _randomTarget = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trial_of_fire_controller_azsharaAI(creature);
    }
};

class npc_trial_of_fire_pillar : public CreatureScript
{
public:
    npc_trial_of_fire_pillar() : CreatureScript("npc_trial_of_fire_pillar") { }

    struct npc_trial_of_fire_pillarAI : public ScriptedAI
    {
        npc_trial_of_fire_pillarAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            if (auto darwin = me->FindNearestCreature(NPC_DARWIN, 30))
                _darwinGUID = darwin->GetGUID();
        }

        void DoAction(const int32 param)
        {
            if (param == ACTION_FIRE_PILLAR_1)
                _events.ScheduleEvent(EVENT_FIRE_PILLAR_1, 500);
        }

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SELECT_TARGETS)
            {
                _events.ScheduleEvent(EVENT_FIRE_PILLAR_3, 0);

                if (auto darwin = Unit::GetCreature(*me, _darwinGUID))
                    me->CastSpell(darwin, SPELL_DUMMY);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FIRE_PILLAR_1:
                    me->CastSpell(me, SPELL_PRE_STRIKE);
                    _events.ScheduleEvent(EVENT_FIRE_PILLAR_2, 4000);
                    break;
                case EVENT_FIRE_PILLAR_2:
                    me->CastSpell(me, SPELL_FLAME_STRIKE);
                    break;
                case EVENT_FIRE_PILLAR_3:
                    _events.CancelEvent(EVENT_FIRE_PILLAR_1);
                    _events.CancelEvent(EVENT_FIRE_PILLAR_2);
                    me->RemoveAura(SPELL_PRE_STRIKE);
                    _events.ScheduleEvent(EVENT_FIRE_PILLAR_4, 4000);
                    break;
                case EVENT_FIRE_PILLAR_4:
                    me->CastSpell(me, SPELL_FIRE_DANCING);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _darwinGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trial_of_fire_pillarAI(creature);
    }
};

class npc_darwin_azshara : public CreatureScript
{
public:
    npc_darwin_azshara() : CreatureScript("npc_darwin_azshara") { }

    struct npc_darwin_azsharaAI : public ScriptedAI
    {
        npc_darwin_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY)
            {
                me->GetMotionMaster()->Clear();
                me->HandleEmoteCommand(EMOTE_STATE_STAND);
                me->GetMotionMaster()->MovePoint(1, who->GetPositionX(), who->GetPositionY(), who->GetPositionZ());
                _isWalking = true;
            }
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (id == 1 && _isWalking)
            {
                me->HandleEmoteCommand(EMOTE_STATE_DANCE);
                me->GetMotionMaster()->Clear();
                _isWalking = false;

                chance = urand(0, 4);
                if (chance == 1)
                    me->AI()->TalkWithDelay(4000, 0);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FIRE_PILLAR_1:
                    me->CastSpell(me, SPELL_PRE_STRIKE);
                    _events.ScheduleEvent(EVENT_FIRE_PILLAR_2, 4000);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isWalking = false;
        EventMap _events;
        uint8 chance = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darwin_azsharaAI(creature);
    }
};

uint32 const EyeOfFrostBunny1PathSize = 10;
const G3D::Vector3 EyeOfFrostBunny1Path[EyeOfFrostBunny1PathSize] =
{
    { 4619.366f, -6767.062f, 215.316f, },
    { 4621.636f, -6768.346f, 215.075f, },
    { 4625.270f, -6768.108f, 214.486f, },
    { 4627.112f, -6765.250f, 214.424f, },
    { 4626.429f, -6762.775f, 214.840f, },
    { 4625.175f, -6761.092f, 215.105f, },
    { 4622.924f, -6760.686f, 215.280f, },
    { 4621.390f, -6761.115f, 215.345f, },
    { 4620.365f, -6761.401f, 215.358f, },
    { 4619.000f, -6763.728f, 215.354f, },
};

uint32 const EyeOfFrostBunny2PathSize = 8;
const G3D::Vector3 EyeOfFrostBunny2Path[EyeOfFrostBunny2PathSize] =
{
  { 4619.081f, -6787.636f, 213.610f, },
  { 4618.272f, -6791.445f, 213.239f, },
  { 4620.015f, -6794.278f, 213.366f, },
  { 4623.891f, -6794.668f, 213.794f, },
  { 4626.923f, -6791.475f, 213.783f, },
  { 4625.862f, -6787.137f, 213.969f, },
  { 4622.903f, -6785.779f, 213.995f, },
  { 4619.579f, -6787.353f, 213.669f, },
};

uint32 const EyeOfFrostBunny3PathSize = 10;
const G3D::Vector3 EyeOfFrostBunny3Path[EyeOfFrostBunny3PathSize] =
{
    { 4647.19f, -6782.70f, 217.00f, },
    { 4645.08f, -6781.60f, 216.57f, },
    { 4642.91f, -6781.71f, 216.26f, },
    { 4642.30f, -6781.54f, 216.08f, },
    { 4640.08f, -6783.42f, 216.05f, },
    { 4639.58f, -6786.13f, 215.96f, },
    { 4641.04f, -6788.45f, 216.22f, },
    { 4643.37f, -6789.47f, 216.69f, },
    { 4645.93f, -6789.20f, 217.37f, },
    { 4648.00f, -6785.94f, 217.51f, },
};

enum TrialOfFrost
{
    GAMEOBJECT_ESSENCE_OF_ICE = 195651, // white orbs 68189 
    GAMEOBJECT_FROST_RUNE = 195345, // blue spots on the ground // 66746

    NPC_EYE_OF_FROST = 35064,   // rotating orbs
    NPC_EYE_OF_FROST_BEAM_BUNNY = 35065,
    NPC_FROST_BURST_BUNNY = 35076,
    NPC_XYLEM_FROST = 36336,

    SPELL_ESSENCE_OF_ICE = 68189,
    SPELL_ROTATE_360 = 66730, // triggering 66731
    SPELL_FROSTBURN = 66731, // triggering (69888) Cancel Frost Credit Application
    SPELL_FROSTBURN_AURA = 66748, // triggering 66731 every 0.5 secs
    SPELL_FROST_BURST = 66743,
    SPELL_CHILLED_GROUND = 66757,
    SPELL_TRIAL_OF_FROST_CREDIT = 68649,

    SPELL_SUMMON_FROST_BURST_BUNNY = 66746,

    SPELL_EYE_OF_FROST_BEAM_AURA_01 = 69032,
    SPELL_EYE_OF_FROST_BEAM_DUMMY_01 = 66764,
    SPELL_EYE_OF_FROST_BEAM_AURA_02 = 69033,
    SPELL_EYE_OF_FROST_BEAM_DUMMY_02 = 69025,
    SPELL_EYE_OF_FROST_BEAM_AURA_03 = 69036,
    SPELL_EYE_OF_FROST_BEAM_DUMMY_03 = 69026,
};

class npc_eye_of_frost_trial : public CreatureScript
{
public:
    npc_eye_of_frost_trial() : CreatureScript("npc_eye_of_frost_trial") { }

    struct npc_eye_of_frost_trialAI : public ScriptedAI
    {
        npc_eye_of_frost_trialAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->CastSpell(me, SPELL_CHILLED_GROUND);
            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);

            if (me->GetDBTableGUIDLow() == 171825)
                if (auto eyebunny = me->SummonCreature(NPC_EYE_OF_FROST_BEAM_BUNNY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
                {
                    _targetBunnyGUID = eyebunny->GetGUID();
                    me->CastSpell(eyebunny, SPELL_ROTATE_360);
                    eyebunny->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny1Path, EyeOfFrostBunny1PathSize);
                }
            if (me->GetDBTableGUIDLow() == 171823)
                if (auto eyebunny = me->SummonCreature(NPC_EYE_OF_FROST_BEAM_BUNNY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
                {
                    _targetBunnyGUID = eyebunny->GetGUID();
                    me->CastSpell(eyebunny, SPELL_ROTATE_360);
                    eyebunny->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny2Path, EyeOfFrostBunny2PathSize);
                }
            if (me->GetDBTableGUIDLow() == 171824)
                if (auto eyebunny = me->SummonCreature(NPC_EYE_OF_FROST_BEAM_BUNNY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
                {
                    _targetBunnyGUID = eyebunny->GetGUID();
                    me->CastSpell(eyebunny, SPELL_ROTATE_360);
                    eyebunny->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny3Path, EyeOfFrostBunny3PathSize);
                }
        }

        void SpellHit(Unit* /*who*/, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_EYE_OF_FROST_BEAM_DUMMY_01)
                me->CastSpell(me, SPELL_CHILLED_GROUND);
        }

    private:
        uint64 _targetBunnyGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_of_frost_trialAI(creature);
    }
};

class npc_eye_of_frost_beam_target : public CreatureScript
{
public:
    npc_eye_of_frost_beam_target() : CreatureScript("npc_eye_of_frost_beam_target") { }

    struct npc_eye_of_frost_beam_targetAI : public ScriptedAI
    {
        npc_eye_of_frost_beam_targetAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_EYE_OF_FROST_BEAM_AURA_01);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_ROTATE_360)
                _myOwner = who->GetGUID();
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto owner = Unit::GetCreature(*me, _myOwner))
                if (owner->GetDBTableGUIDLow() == 171825)
                    if (point == 9)
                    {
                        me->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny1Path, EyeOfFrostBunny1PathSize);
                    }

            if (auto owner = Unit::GetCreature(*me, _myOwner))
                if (owner->GetDBTableGUIDLow() == 171823)
                    if (point == 7)
                    {
                        me->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny2Path, EyeOfFrostBunny2PathSize);
                    }
            if (auto owner = Unit::GetCreature(*me, _myOwner))
                if (owner->GetDBTableGUIDLow() == 171824)
                    if (point == 9)
                    {
                        me->GetMotionMaster()->MoveSmoothPath(EyeOfFrostBunny3Path, EyeOfFrostBunny3PathSize);
                    }
        }
    private:
        uint64 _myOwner = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_of_frost_beam_targetAI(creature);
    }
};

// ID - 69888 Cancel Frost Credit Application
class spell_cancel_frost_credit_application : public SpellScriptLoader
{
public:
    spell_cancel_frost_credit_application() : SpellScriptLoader("spell_cancel_frost_credit_application") { }

    class spell_cancel_frost_credit_application_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_cancel_frost_credit_application_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto player = GetCaster())
                if (player->GetAuraCount(SPELL_ESSENCE_OF_ICE) > 0)
                    player->RemoveAuraFromStack(SPELL_ESSENCE_OF_ICE, player->GetGUID());
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_cancel_frost_credit_application_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_cancel_frost_credit_application_SpellScript();
    }
};

enum TrialOfShadow
{
    SPELL_SHADOW_BAIT = 69862,
    SPELL_SHADOW_PUZZLE_CREDIT = 68651,
    SPELL_UNDER_THE_LOOMING_SHADOW = 69863,
    SPELL_FORCECAST_SHADOW_SUMMONING = 69872,
    SPELL_SUMMON_WEEPING_SOUL_MISSILE = 69881,
    SPELL_SUMMON_WEEPING_SOUL = 68233,
    SPELL_SOUL_CONSUMPTION = 68242,

    NPC_XYLEM_SHADOW = 36337,
};

class npc_weeping_soul : public CreatureScript
{
public:
    npc_weeping_soul() : CreatureScript("npc_weeping_soul") { }

    struct npc_weeping_soulAI : public ScriptedAI
    {
        npc_weeping_soulAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (summoner->GetTypeId() == TYPEID_PLAYER)
            {
                _playerGUID = summoner->GetGUID();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                me->SetRooted(true);
                _events.ScheduleEvent(EVENT_WEEPING_SOUL_1, 2500);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_SOUL_CONSUMPTION)
            {
                me->CastSpell(me, SPELL_SHADOW_BAIT);
            }
        }

        void DamageDealt(Unit* victim, uint32& damage, DamageEffectType /*damageType*/)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                if (victim->GetAura(SPELL_SHADOW_BAIT))
                    if (victim->GetAuraCount(SPELL_SHADOW_BAIT) > 0)
                        victim->RemoveAuraFromStack(SPELL_SHADOW_BAIT);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (auto respawn = me->FindNearestCreature(74957, 60.0f, false))
            {
                _counter = urand(0, 2);
                if (_counter == 1)
                    respawn->Respawn(true);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WEEPING_SOUL_1:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetRooted(false);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        me->AI()->AttackStart(player);
                    break;
                default:
                    break;
                }
            }

            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (!player->isAlive())
                        me->DespawnOrUnsummon();
            }
            else _checkTimer -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 _checkTimer = 2000;
        uint32 _counter = 0;
        EventMap _events;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_weeping_soulAI(creature);
    }
};

class spell_essence_of_ice : public SpellScriptLoader
{
public:
    spell_essence_of_ice() : SpellScriptLoader("spell_essence_of_ice") { }

    class spell_essence_of_ice_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_essence_of_ice_SpellScript);

        void HandleOnHit()
        {
            if (auto target = GetHitUnit())
                if (target->GetAura(SPELL_ESSENCE_OF_ICE))
                    if (target->GetAuraCount(SPELL_ESSENCE_OF_ICE) == 20)
                        target->CastSpell(target, SPELL_TRIAL_OF_FROST_CREDIT);
        }
        void Register()
        {
            OnHit += SpellHitFn(spell_essence_of_ice_SpellScript::HandleOnHit);
        }

    };
    SpellScript* GetSpellScript() const
    {
        return new spell_essence_of_ice_SpellScript();
    }
};

class spell_fire_dancing : public SpellScriptLoader
{
public:
    spell_fire_dancing() : SpellScriptLoader("spell_fire_dancing") { }

    class spell_fire_dancing_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fire_dancing_SpellScript);

        void HandleOnHit()
        {
            if (auto target = GetHitUnit())
                if (target->GetAura(SPELL_FIRE_DANCING))
                    if (target->GetAuraCount(SPELL_FIRE_DANCING) == 10)
                        target->CastSpell(target, SPELL_FIRE_PUZZLE_CREDIT);
        }
        void Register()
        {
            OnHit += SpellHitFn(spell_fire_dancing_SpellScript::HandleOnHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_fire_dancing_SpellScript();
    }
};

class spell_shadow_bait_trial : public SpellScriptLoader
{
public:
    spell_shadow_bait_trial() : SpellScriptLoader("spell_shadow_bait_trial") { }

    class spell_shadow_bait_trial_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_shadow_bait_trial_SpellScript);

        void HandleAfterHit()
        {
            if (auto target = GetHitPlayer())
                if (target->GetAura(SPELL_SHADOW_BAIT))
                    if (target->GetAuraCount(SPELL_SHADOW_BAIT) == 20)
                    {
                        target->CastSpell(GetCaster(), SPELL_SHADOW_PUZZLE_CREDIT);
                        target->RemoveAura(SPELL_UNDER_THE_LOOMING_SHADOW);
                    }
        }
        void Register()
        {
            AfterHit += SpellHitFn(spell_shadow_bait_trial_SpellScript::HandleAfterHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_shadow_bait_trial_SpellScript();
    }
};

class playerscript_azshara_trial_of_shadow : public PlayerScript
{
public:
    playerscript_azshara_trial_of_shadow() : PlayerScript("playerscript_azshara_trial_of_shadow") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (!player->GetAura(SPELL_AURA_PREVENT_EVENT_3))
            if (newZone == 16 && newArea == 4800)
            {
                if (auto xylem = player->FindNearestCreature(NPC_XYLEM_SHADOW, 50.0f))
                {
                    player->CastSpell(player, SPELL_AURA_PREVENT_EVENT_3);
                    xylem->AI()->TalkWithDelay(2000, 0, player->GetGUID());
                    xylem->AI()->TalkWithDelay(9000, 1, player->GetGUID());
                    xylem->AI()->TalkWithDelay(17000, 2, player->GetGUID());
                    xylem->AI()->TalkWithDelay(24000, 3, player->GetGUID());
                    xylem->AI()->TalkWithDelay(30000, 4, player->GetGUID());
                    xylem->AI()->TalkWithDelay(36000, 5, player->GetGUID());
                }
            }
    }
};

class playerscript_azshara_trial_of_fire : public PlayerScript
{
public:
    playerscript_azshara_trial_of_fire() : PlayerScript("playerscript_azshara_trial_of_fire") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (!player->GetAura(SPELL_AURA_PREVENT_EVENT_2))
            if (newZone == 16 && newArea == 4799)
            {
                if (auto xylem = player->FindNearestCreature(NPC_XYLEM_FIRE, 50.0f))
                {
                    player->CastSpell(player, SPELL_AURA_PREVENT_EVENT_2);
                    xylem->AI()->TalkWithDelay(2000, 0, player->GetGUID());
                    xylem->AI()->TalkWithDelay(7000, 1, player->GetGUID());
                    xylem->AI()->TalkWithDelay(14000, 2, player->GetGUID());
                    xylem->AI()->TalkWithDelay(20000, 3, player->GetGUID());
                }
            }
    }
};

class playerscript_azshara_trial_of_frost : public PlayerScript
{
public:
    playerscript_azshara_trial_of_frost() : PlayerScript("playerscript_azshara_trial_of_frost") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (!player->GetAura(SPELL_AURA_PREVENT_EVENT_1))
            if (newZone == 16 && newArea == 5333)
            {
                if (auto xylem = player->FindNearestCreature(NPC_XYLEM_FROST, 50.0f))
                {
                    player->CastSpell(player, SPELL_AURA_PREVENT_EVENT_1);
                    xylem->AI()->TalkWithDelay(2000, 0, player->GetGUID());
                    xylem->AI()->TalkWithDelay(9000, 1, player->GetGUID());
                    xylem->AI()->TalkWithDelay(17000, 2, player->GetGUID());
                    xylem->AI()->TalkWithDelay(24000, 3, player->GetGUID());
                    xylem->AI()->TalkWithDelay(31000, 4, player->GetGUID());
                }
            }
    }
};

enum FallToDeathPortal
{
    SPELL_SAFE_FALL = 82655,
    SPELL_DEATH = 69812,
    FALL_TO_DEATH_AURA = 68744,
};

class spell_fall_to_death_portal_tick : public SpellScriptLoader
{
public:
    spell_fall_to_death_portal_tick() : SpellScriptLoader("spell_fall_to_death_portal_tick") { }

    class spell_fall_to_death_portal_tick_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_fall_to_death_portal_tick_AuraScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            return true;
        }

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (auto player = GetCaster()->ToPlayer())
                if (player->GetPositionZ() < 148.0f)
                {
                    player->RemoveAurasDueToSpell(FALL_TO_DEATH_AURA);
                    player->RemoveAurasDueToSpell(SPELL_SAFE_FALL);
                    player->CastSpell(player, SPELL_DEATH);
                    player->CastSpell(player, 93960); // root for 3 secs
                }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_fall_to_death_portal_tick_AuraScript::HandleEffectPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_fall_to_death_portal_tick_AuraScript();
    }
};

// 36638
class npc_lord_kaltrana_azshara : public CreatureScript
{
public:
    npc_lord_kaltrana_azshara() : CreatureScript("npc_lord_kaltrana_azshara") { }

    struct npc_lord_kaltrana_azsharaAI : public ScriptedAI
    {
        npc_lord_kaltrana_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void Reset()
        {
            _aggroText = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (!_aggroText)
            {
                _aggroText = true;
                me->AI()->Talk(0);

                if (auto malicon = Unit::GetCreature(*me, _maliconGUID))
                    malicon->AI()->DoAction(ACTION_MALICON_1);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            std::list<Player*> player = me->GetPlayersInRange(50, true);
            for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                (*it)->KilledMonsterCredit(36638);

            if (auto malicon = Unit::GetCreature(*me, _maliconGUID))
                malicon->AI()->DoAction(ACTION_MALICON_2);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!me->FindNearestCreature(36649, 50.0f))
                if (auto malicon = me->SummonCreature(36649, 4764.352f, -7333.022f, 128.60398f, 0.5995f, TEMPSUMMON_DEAD_DESPAWN, 15000))
                    _maliconGUID = malicon->GetGUID();

            DoMeleeAttackIfReady();
        }

    private:
        bool _isMaliconSummoned = false;
        bool _aggroText = false;
        uint64 _maliconGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_kaltrana_azsharaAI(creature);
    }
};

// 36638
class npc_malicion_azshara : public CreatureScript
{
public:
    npc_malicion_azshara() : CreatureScript("npc_malicion_azshara") { }

    struct npc_malicion_azsharaAI : public ScriptedAI
    {
        npc_malicion_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void DoAction(int32 const action)
        {
            if (action == ACTION_MALICON_1)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->GetMotionMaster()->MovePoint(1, 4764.087f, -7322.009f, 137.47f);
            }

            if (action == ACTION_MALICON_2)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                if (auto player = me->FindNearestPlayer(50.0f))
                    me->AI()->AttackStart(player);
                else
                    me->DespawnOrUnsummon();
            }
        }

        void DamageDealt(Unit* victim, uint32& damage, DamageEffectType /*damageType*/)
        {
            me->AI()->Talk(0, victim->GetGUID());

            if (victim->ToPlayer())
            {
                victim->CastSpell(victim, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                victim->EnterVehicle(me, 0);
            }
            else
            {
                if (auto player = me->FindNearestPlayer(30))
                {
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    player->EnterVehicle(me, 0);
                }
            }
            me->CombatStop();
            me->SetReactState(REACT_PASSIVE);
            me->GetMotionMaster()->MovePoint(2, 4764.087f, -7322.009f, 137.47f);
        }

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == 69841) // Frostfire Breath
            {
                if (auto player = me->GetVehicleKit()->GetPassenger(1))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->GetMotionMaster()->MoveJump(4771.34f, -7320.404f, 126.42f, 10, 10);
                }
                me->CastSpell(me, 69853); // aoe KillCredit
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (point == 1)
            {
                me->SetFacingTo(6.281f);
            }

            if (point == 2)
            {
                me->SetFacingTo(6.281f);
                _events.ScheduleEvent(EVENT_MALICON_1, 3000);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MALICON_1:
                    me->CastSpell(me, 69178); // summon kalecgos
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _reachedPoint = false;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_malicion_azsharaAI(creature);
    }
};

uint32 const RocketwayTransportPathSize1 = 20;
const G3D::Vector3 RocketwayTransportPath1[RocketwayTransportPathSize1] =
{
    {  4622.291f, -7023.918f, 154.01f + 2, },
    {  4636.256f, -6992.981f, 151.39f + 2, },
    {  4654.128f, -6962.186f, 147.56f + 2, },
    {  4700.766f, -6902.161f, 138.31f + 2, },
    {  4748.586f, -6822.872f, 115.79f + 2, },
    {  4759.801f, -6745.607f, 92.076f + 2, },
    {  4740.695f, -6584.905f, 61.758f + 2, },
    {  4709.872f, -6466.062f, 51.381f + 2, },
    {  4690.982f, -6404.700f, 71.283f + 2, },
    {  4688.257f, -6339.774f, 123.19f + 2, },
    {  4670.548f, -6291.705f, 152.28f + 2, },
    {  4588.192f, -6216.210f, 137.30f + 2, },
    {  4520.427f, -6156.446f, 105.60f + 2, },
    {  4495.361f, -6077.440f, 98.417f + 2, },
    {  4491.216f, -6010.211f, 99.853f + 2, },
    {  4484.651f, -5960.886f, 115.58f + 2, },
    {  4481.949f, -5909.565f, 132.15f + 8, },
    {  4488.86f,  -5847.820f, 122.73f, },
    {  4485.145f, -5785.646f, 131.28f + 8, },
    {  4469.816f, -5715.171f, 145.50f + 1, },
};

uint32 const RocketwayTransportPathSize2 = 39;
const G3D::Vector3 RocketwayTransportPath2[RocketwayTransportPathSize2] =
{
    { 4622.291f, -7023.918f, 154.01f + 2, },   // 0 
    { 4636.256f, -6992.981f, 151.39f + 2, },   // 1 
    { 4654.128f, -6962.186f, 147.56f + 2, },   // 2 
    { 4700.766f, -6902.161f, 138.31f + 2, },   // 3 
    { 4748.586f, -6822.872f, 115.79f + 2, },   // 4 
    { 4759.801f, -6745.607f, 92.076f + 2, },   // 5 
    { 4740.695f, -6584.905f, 61.758f + 2, },   // 6 
    { 4709.872f, -6466.062f, 51.381f + 2, },   // 7 
    { 4690.982f, -6404.700f, 71.283f + 2, },   // 8 
    { 4688.257f, -6339.774f, 123.19f + 2, },   // 9 
    { 4670.548f, -6291.705f, 152.28f + 2, },   // 10
    { 4588.192f, -6216.210f, 137.30f + 2, },   // 11
    { 4520.427f, -6156.446f, 105.60f + 2, },   // 12
    { 4495.361f, -6077.440f, 98.417f + 2, },   // 13
    { 4491.216f, -6010.211f, 99.853f + 2, },   // 14
    { 4484.651f, -5960.886f, 115.58f + 2, },   // 15
    { 4481.949f, -5909.565f, 132.15f + 8, },   // 16
    { 4488.86f,  -5847.820f, 122.73f, },       // 17
    { 4485.145f, -5785.646f, 131.28f + 5, },   // 18
    { 4469.816f, -5715.171f, 145.50f + 2, },   // 19
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 20
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 21
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 22
    { 4253.4340f, -5571.394f, 223.99f , },  // 23
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 24
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 25
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 26
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 27
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 28
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 29
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 30
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 31
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 32
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 33
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 34
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 35
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 36
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 37
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 38
};

uint32 const RocketwayTransportPathSize3 = 49;
const G3D::Vector3 RocketwayTransportPath3[RocketwayTransportPathSize3] =
{
    { 4622.291f, -7023.918f, 154.01f + 2, },   // 0 
    { 4636.256f, -6992.981f, 151.39f + 2, },   // 1 
    { 4654.128f, -6962.186f, 147.56f + 2, },   // 2 
    { 4700.766f, -6902.161f, 138.31f + 2, },   // 3 
    { 4748.586f, -6822.872f, 115.79f + 2, },   // 4 
    { 4759.801f, -6745.607f, 92.076f + 2, },   // 5 
    { 4740.695f, -6584.905f, 61.758f + 2, },   // 6 
    { 4709.872f, -6466.062f, 51.381f + 2, },   // 7 
    { 4690.982f, -6404.700f, 71.283f + 2, },   // 8 
    { 4688.257f, -6339.774f, 123.19f + 2, },   // 9 
    { 4670.548f, -6291.705f, 152.28f + 2, },   // 10
    { 4588.192f, -6216.210f, 137.30f + 2, },   // 11
    { 4520.427f, -6156.446f, 105.60f + 2, },   // 12
    { 4495.361f, -6077.440f, 98.417f + 2, },   // 13
    { 4491.216f, -6010.211f, 99.853f + 2, },   // 14
    { 4484.651f, -5960.886f, 115.58f + 2, },   // 15
    { 4481.949f, -5909.565f, 132.15f + 8, },   // 16
    { 4488.86f,  -5847.820f, 122.73f, },       // 17
    { 4485.145f, -5785.646f, 131.28f + 5, },   // 18
    { 4469.816f, -5715.171f, 145.50f + 2, },   // 19
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 20
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 21
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 22
    { 4253.4340f, -5571.394f, 223.99f , },  // 23
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 24
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 25
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 26
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 27
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 28
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 29
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 30
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 31
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 32
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 33
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 34
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 35
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 36
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 37
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 38
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 39
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 40
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 41
    { 3253.257f, -4834.690f, 170.010f, },  // 42
    { 3208.953f, -4840.385f, 163.046f, },  // 43
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 44
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 45
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 46
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 47
    { 2954.718f, -4997.358f, 165.665f + 1, },  // 48
};

uint32 const RocketwayTransportPathSize4 = 71;
const G3D::Vector3 RocketwayTransportPath4[RocketwayTransportPathSize4] =
{
   { 4622.291f, -7023.918f, 154.01f + 2, },   // 0 
    { 4636.256f, -6992.981f, 151.39f + 2, },   // 1 
    { 4654.128f, -6962.186f, 147.56f + 2, },   // 2 
    { 4700.766f, -6902.161f, 138.31f + 2, },   // 3 
    { 4748.586f, -6822.872f, 115.79f + 2, },   // 4 
    { 4759.801f, -6745.607f, 92.076f + 2, },   // 5 
    { 4740.695f, -6584.905f, 61.758f + 2, },   // 6 
    { 4709.872f, -6466.062f, 51.381f + 2, },   // 7 
    { 4690.982f, -6404.700f, 71.283f + 2, },   // 8 
    { 4688.257f, -6339.774f, 123.19f + 2, },   // 9 
    { 4670.548f, -6291.705f, 152.28f + 2, },   // 10
    { 4588.192f, -6216.210f, 137.30f + 2, },   // 11
    { 4520.427f, -6156.446f, 105.60f + 2, },   // 12
    { 4495.361f, -6077.440f, 98.417f + 2, },   // 13
    { 4491.216f, -6010.211f, 99.853f + 2, },   // 14
    { 4484.651f, -5960.886f, 115.58f + 2, },   // 15
    { 4481.949f, -5909.565f, 132.15f + 8, },   // 16
    { 4488.86f,  -5847.820f, 122.73f, },       // 17
    { 4485.145f, -5785.646f, 131.28f + 5, },   // 18
    { 4469.816f, -5715.171f, 145.50f + 2, },   // 19
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 20
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 21
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 22
    { 4253.4340f, -5571.394f, 223.99f , },  // 23
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 24
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 25
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 26
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 27
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 28
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 29
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 30
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 31
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 32
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 33
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 34
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 35
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 36
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 37
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 38
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 39
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 40
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 41
    { 3253.257f, -4834.690f, 170.010f + 2, },  // 42
    { 3208.953f, -4840.385f, 163.046f + 2, },  // 43
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 44
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 45
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 46
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 47
    { 2954.718f, -4997.358f, 165.665f + 2, },  // 48
    { 2948.488f, -5018.611f, 165.138f + 2, },  // 49
    { 2932.470f, -5058.654f, 168.622f + 2, },  // 50
    { 2923.969f, -5150.078f, 126.504f + 2, },  // 51
    { 2917.345f, -5200.492f, 130.763f + 2, },  // 52
    { 2907.605f, -5260.942f, 140.698f + 2, },  // 53
    { 2905.805f, -5304.108f, 130.926f + 2, },  // 54
    { 2873.228f, -5360.751f, 129.733f + 2, },  // 55
    { 2844.701f, -5409.658f, 143.221f + 2, },  // 56
    { 2838.625f, -5493.200f, 142.997f + 2, },  // 57
    { 2839.272f, -5551.992f, 135.089f + 2, },  // 58
    { 2825.451f, -5685.107f, 129.097f + 2, },  // 59
    { 2795.288f, -5749.038f, 121.145f + 2, },  // 60
    { 2733.443f, -5821.934f, 101.814f + 2, },  // 61
    { 2694.786f, -5861.976f, 98.8925f + 2, },  // 62
    { 2660.803f, -5942.058f, 98.9480f + 2, },  // 63
    { 2631.225f, -6004.947f, 102.054f + 2, },  // 64
    { 2637.438f, -6072.747f, 102.392f + 2, },  // 65
    { 2643.590f, -6106.774f, 124.183f + 2, },  // 66
    { 2658.743f, -6146.762f, 137.520f + 2, },  // 67
    { 2659.132f, -6191.978f, 139.175f + 1, },     // 68
    { 2657.509f, -6201.29f, 109.43f,  },     // 69
    { 2656.752f, -6214.426f, 100.570f + 1,  }, // 70
};

uint32 const RocketwayTransportPathSize5 = 22;
const G3D::Vector3 RocketwayTransportPath5[RocketwayTransportPathSize5] =
{
    { 2659.048f, -6160.734f, 140.97f + 3, },
    { 2657.841f, -6144.428f, 135.04f + 2, },
    { 2644.157f, -6108.092f, 122.18f + 2, },
    { 2637.543f, -6069.915f, 101.81f + 2, },
    { 2629.879f, -6028.963f, 102.10f + 2, },
    { 2637.767f, -5985.008f, 102.34f + 2, },
    { 2667.090f, -5926.431f, 96.035f + 2, },
    { 2686.639f, -5873.788f, 97.369f + 2, },
    { 2743.249f, -5812.438f, 101.18f + 2, },
    { 2797.007f, -5747.136f, 120.56f + 2, },
    { 2823.666f, -5693.089f, 128.24f + 2, },
    { 2836.524f, -5572.312f, 133.33f + 2, },
    { 2841.203f, -5527.855f, 134.45f + 2, },
    { 2836.639f, -5462.626f, 143.41f + 2, },
    { 2844.846f, -5409.306f, 139.65f + 2, },
    { 2884.704f, -5344.237f, 126.41f + 2, },
    { 2908.474f, -5295.138f, 130.10f + 2, },
    { 2907.078f, -5259.300f, 135.84f + 2, },
    { 2909.083f, -5226.782f, 129.88f + 2, },
    { 2923.744f, -5155.354f, 126.96f + 2, },
    { 2932.887f, -5058.783f, 161.30f + 2, },
    { 2954.772f, -4997.772f, 165.66f + 1, },
};

uint32 const RocketwayTransportPathSize6 = 32;
const G3D::Vector3 RocketwayTransportPath6[RocketwayTransportPathSize6] =
{
    { 2659.048f, -6160.734f, 140.97f + 3, },  // 0 
    { 2657.841f, -6144.428f, 135.04f + 2, },  // 1 
    { 2644.157f, -6108.092f, 122.18f + 2, },  // 2 
    { 2637.543f, -6069.915f, 101.81f + 2, },  // 3 
    { 2629.879f, -6028.963f, 102.10f + 2, },  // 4 
    { 2637.767f, -5985.008f, 102.34f + 2, },  // 5 
    { 2667.090f, -5926.431f, 96.035f + 2, },  // 6 
    { 2686.639f, -5873.788f, 97.369f + 2, },  // 7 
    { 2743.249f, -5812.438f, 101.18f + 2, },  // 8 
    { 2797.007f, -5747.136f, 120.56f + 2, },  // 9 
    { 2823.666f, -5693.089f, 128.24f + 2, },  // 10
    { 2836.524f, -5572.312f, 133.33f + 2, },  // 11
    { 2841.203f, -5527.855f, 134.45f + 2, },  // 12
    { 2836.639f, -5462.626f, 143.41f + 2, },  // 13
    { 2844.846f, -5409.306f, 139.65f + 2, },  // 14
    { 2884.704f, -5344.237f, 126.41f + 2, },  // 15
    { 2908.474f, -5295.138f, 130.10f + 2, },  // 16
    { 2907.078f, -5259.300f, 135.84f + 2, },  // 17
    { 2909.083f, -5226.782f, 129.88f + 2, },  // 18
    { 2923.744f, -5155.354f, 126.96f + 2, },  // 19
    { 2932.887f, -5058.783f, 161.30f + 2, },  // 20
    { 2954.555f, -4997.842f, 165.660f + 2, }, // 22
    { 2962.023f, -4969.535f, 166.060f + 3, }, // 23
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 24
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 25
    { 3151.059f, -4841.656f, 140.624f , }, // 26
    { 3203.451f, -4840.634f, 159.616f , }, // 27
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 28
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 29
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 30
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 31
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 31
};

uint32 const RocketwayTransportPathSize7 = 60;
const G3D::Vector3 RocketwayTransportPath7[RocketwayTransportPathSize7] =
{
    { 2659.048f, -6160.734f, 140.97f + 3, },  // 0 
    { 2657.841f, -6144.428f, 135.04f + 2, },  // 1 
    { 2644.157f, -6108.092f, 122.18f + 2, },  // 2 
    { 2637.543f, -6069.915f, 101.81f + 2, },  // 3 
    { 2629.879f, -6028.963f, 102.10f + 2, },  // 4 
    { 2637.767f, -5985.008f, 102.34f + 2, },  // 5 
    { 2667.090f, -5926.431f, 96.035f + 2, },  // 6 
    { 2686.639f, -5873.788f, 97.369f + 2, },  // 7 
    { 2743.249f, -5812.438f, 101.18f + 2, },  // 8 
    { 2797.007f, -5747.136f, 120.56f + 2, },  // 9 
    { 2823.666f, -5693.089f, 128.24f + 2, },  // 10
    { 2836.524f, -5572.312f, 133.33f + 2, },  // 11
    { 2841.203f, -5527.855f, 134.45f + 2, },  // 12
    { 2836.639f, -5462.626f, 143.41f + 2, },  // 13
    { 2844.846f, -5409.306f, 139.65f + 2, },  // 14
    { 2884.704f, -5344.237f, 126.41f + 2, },  // 15
    { 2908.474f, -5295.138f, 130.10f + 2, },  // 16
    { 2907.078f, -5259.300f, 135.84f + 2, },  // 17
    { 2909.083f, -5226.782f, 129.88f + 2, },  // 18
    { 2923.744f, -5155.354f, 126.96f + 2, },  // 19
    { 2932.887f, -5058.783f, 161.30f + 2, },  // 20
    { 2954.555f, -4997.842f, 165.660f + 2, }, // 21
    { 2962.023f, -4969.535f, 166.060f + 3, }, // 22
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 23
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 24
    { 3151.059f, -4841.656f, 140.624f, },     // 25
    { 3203.451f, -4840.634f, 159.616f, },     // 26
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 27
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 28
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 29
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 30
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 31
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 32
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 33
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 34
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 35
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 36
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 37
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 38
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 39
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 40
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 41
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 42
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 43
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 44
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 45
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 46
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 47
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 48
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 49
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 50
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 51
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 52
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 53
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 54
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 55
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 56
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 57
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 58
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 59
};

uint32 const RocketwayTransportPathSize8 = 87;
const G3D::Vector3 RocketwayTransportPath8[RocketwayTransportPathSize8] =
{
    { 2659.048f, -6160.734f, 140.97f + 3, },  // 0 
    { 2657.841f, -6144.428f, 135.04f + 2, },  // 1 
    { 2644.157f, -6108.092f, 122.18f + 2, },  // 2 
    { 2637.543f, -6069.915f, 101.81f + 2, },  // 3 
    { 2629.879f, -6028.963f, 102.10f + 2, },  // 4 
    { 2637.767f, -5985.008f, 102.34f + 2, },  // 5 
    { 2667.090f, -5926.431f, 96.035f + 2, },  // 6 
    { 2686.639f, -5873.788f, 97.369f + 2, },  // 7 
    { 2743.249f, -5812.438f, 101.18f + 2, },  // 8 
    { 2797.007f, -5747.136f, 120.56f + 2, },  // 9 
    { 2823.666f, -5693.089f, 128.24f + 2, },  // 10
    { 2836.524f, -5572.312f, 133.33f + 2, },  // 11
    { 2841.203f, -5527.855f, 134.45f + 2, },  // 12
    { 2836.639f, -5462.626f, 143.41f + 2, },  // 13
    { 2844.846f, -5409.306f, 139.65f + 2, },  // 14
    { 2884.704f, -5344.237f, 126.41f + 2, },  // 15
    { 2908.474f, -5295.138f, 130.10f + 2, },  // 16
    { 2907.078f, -5259.300f, 135.84f + 2, },  // 17
    { 2909.083f, -5226.782f, 129.88f + 2, },  // 18
    { 2923.744f, -5155.354f, 126.96f + 2, },  // 19
    { 2932.887f, -5058.783f, 161.30f + 2, },  // 20
    { 2954.555f, -4997.842f, 165.660f + 2, }, // 21
    { 2962.023f, -4969.535f, 166.060f + 3, }, // 22
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 23
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 24
    { 3151.059f, -4841.656f, 140.624f, },     // 25
    { 3203.451f, -4840.634f, 159.616f, },     // 26
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 27
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 28
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 29
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 30
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 31
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 32
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 33
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 34
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 35
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 36
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 37
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 38
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 39
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 40
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 41
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 42
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 43
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 44
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 45
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 46
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 47
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 48
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 49
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 50
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 51
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 52
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 53
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 54
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 55
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 56
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 57
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 58
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 59
    { 4474.219f, -5732.764f, 146.12f + 2, },  // 60
    { 4488.040f, -5809.625f, 125.70f + 2, },  // 61
    { 4488.664f, -5847.970f, 121.95f + 2, },  // 62
    { 4484.779f, -5875.894f, 131.12f + 2, },  // 63
    { 4482.883f, -5924.167f, 128.46f + 2, },  // 64
    { 4490.574f, -6005.869f, 102.09f + 2, },  // 65
    { 4498.412f, -6092.642f, 100.29f + 2, },  // 66
    { 4510.510f, -6147.913f, 98.430f + 2, },  // 67
    { 4525.141f, -6165.804f, 101.00f + 2, },  // 68
    { 4547.924f, -6183.804f, 114.75f + 2, },  // 69
    { 4589.117f, -6228.248f, 125.68f + 2, },  // 70
    { 4627.375f, -6249.052f, 143.51f + 2, },  // 71
    { 4651.208f, -6266.880f, 150.569f, },
    { 4673.215f, -6297.484f, 149.697f, },
    { 4687.634f, -6337.202f, 127.065f, },
    { 4696.370f, -6418.275f, 64.033f + 2, },  // 74
    { 4711.732f, -6467.191f, 54.221f + 2, },  // 75
    { 4740.886f, -6577.352f, 63.428f + 2, },  // 76
    { 4758.470f, -6711.137f, 85.886f + 2, },  // 77
    { 4757.949f, -6783.067f, 104.71f + 2, },  // 78
    { 4735.696f, -6849.752f, 124.73f + 2, },  // 79
    { 4695.719f, -6908.222f, 141.57f + 2, },  // 80
    { 4650.866f, -6966.891f, 149.94f + 2, },  // 81
    { 4625.907f, -7016.290f, 155.76f + 2, },  // 82
    { 4602.122f, -7049.343f, 156.60f + 2, },  // 83
    { 4611.300f, -7086.024f, 125.70f + 2, },  // 84
    { 4639.461f, -7043.100f, 113.55f + 1, },  // 85
};

uint32 const RocketwayTransportPathSize9 = 22;
const G3D::Vector3 RocketwayTransportPath9[RocketwayTransportPathSize9] =
{
    { 2948.488f, -5018.611f, 165.138f, },  // 0 
    { 2932.470f, -5058.654f, 167.622f + 1, },  // 1 
    { 2923.969f, -5150.078f, 126.504f + 2, },  // 2 
    { 2917.345f, -5200.492f, 130.763f + 2, },  // 3 
    { 2907.605f, -5260.942f, 140.698f + 2, },  // 4 
    { 2905.805f, -5304.108f, 130.926f + 2, },  // 5 
    { 2873.228f, -5360.751f, 129.733f + 2, },  // 6 
    { 2844.701f, -5409.658f, 143.221f + 2, },  // 7 
    { 2838.625f, -5493.200f, 142.997f + 2, },  // 8 
    { 2839.272f, -5551.992f, 135.089f + 2, },  // 9 
    { 2825.451f, -5685.107f, 129.097f + 2, },  // 10
    { 2795.288f, -5749.038f, 121.145f + 2, },  // 11
    { 2733.443f, -5821.934f, 101.814f + 2, },  // 12
    { 2694.786f, -5861.976f, 98.8925f + 2, },  // 13
    { 2660.803f, -5942.058f, 98.9480f + 2, },  // 14
    { 2631.225f, -6004.947f, 102.054f + 2, },  // 15
    { 2637.438f, -6072.747f, 102.392f + 2, },  // 16
    { 2643.590f, -6106.774f, 124.183f + 2, },  // 17
    { 2658.743f, -6146.762f, 137.520f + 2, },  // 18
    { 2659.132f, -6191.978f, 139.175f + 1, },   // 19
    { 2657.509f, -6201.29f, 109.43f,  },     // 20
    { 2656.752f, -6214.426f, 100.570f + 1,  },     // 21
};

uint32 const RocketwayTransportPathSize10 = 10;
const G3D::Vector3 RocketwayTransportPath10[RocketwayTransportPathSize10] =
{
    { 2962.023f, -4969.535f, 166.060f + 1, }, // 0 
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 1 
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 2 
    { 3151.059f, -4841.656f, 140.624f, },     // 3 
    { 3203.451f, -4840.634f, 159.616f, },     // 4 
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 5 
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 6 
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 7 
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 8 
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 9 
};

uint32 const RocketwayTransportPathSize11 = 38;
const G3D::Vector3 RocketwayTransportPath11[RocketwayTransportPathSize11] =
{
    { 2962.023f, -4969.535f, 166.060f + 1, }, // 0 
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 1 
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 2 
    { 3151.059f, -4841.656f, 140.624f, },     // 3 
    { 3203.451f, -4840.634f, 159.616f, },     // 4 
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 5 
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 6 
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 7 
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 8 
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 9 
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 10
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 11
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 12
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 13
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 14
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 15
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 16
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 17
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 18
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 19
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 20
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 21
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 22
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 23
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 24
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 25
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 26
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 27
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 28
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 29
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 30
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 31
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 32
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 33
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 34
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 35
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 36
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 37
};

uint32 const RocketwayTransportPathSize12 = 65;
const G3D::Vector3 RocketwayTransportPath12[RocketwayTransportPathSize12] =
{
    { 2962.023f, -4969.535f, 166.060f + 1, }, // 0 
    { 2987.934f, -4926.130f, 163.708f + 3, }, // 1 
    { 3068.912f, -4868.037f, 150.407f + 2, }, // 2 
    { 3151.059f, -4841.656f, 140.624f, },     // 3 
    { 3203.451f, -4840.634f, 159.616f, },     // 4 
    { 3261.932f, -4833.888f, 167.213f + 1, }, // 5 
    { 3372.745f, -4819.187f, 150.766f + 1, }, // 6 
    { 3481.600f, -4794.267f, 158.622f + 1, }, // 7 
    { 3520.638f, -4793.016f, 161.900f + 1, }, // 8 
    { 3557.184f, -4794.898f, 161.012f + 1, }, // 9 
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 10
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 11
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 12
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 13
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 14
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 15
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 16
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 17
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 18
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 19
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 20
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 21
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 22
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 23
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 24
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 25
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 26
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 27
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 28
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 29
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 30
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 31
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 32
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 33
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 34
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 35
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 36
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 37
    { 4474.219f, -5732.764f, 146.12f + 2, },  // 38
    { 4488.040f, -5809.625f, 125.70f + 2, },  // 39
    { 4488.664f, -5847.970f, 121.95f + 2, },  // 40
    { 4484.779f, -5875.894f, 131.12f + 2, },  // 41
    { 4482.883f, -5924.167f, 128.46f + 2, },  // 42
    { 4490.574f, -6005.869f, 102.09f + 2, },  // 43
    { 4498.412f, -6092.642f, 100.29f + 2, },  // 44
    { 4510.510f, -6147.913f, 98.430f + 2, },  // 45
    { 4525.141f, -6165.804f, 101.00f + 2, },  // 46
    { 4547.924f, -6183.804f, 114.75f + 2, },  // 47
    { 4589.117f, -6228.248f, 125.68f + 2, },  // 48
    { 4627.375f, -6249.052f, 143.51f + 2, },  // 49
    { 4651.208f, -6266.880f, 150.569f, },
    { 4673.215f, -6297.484f, 149.697f, },
    { 4687.634f, -6337.202f, 127.065f, },
    { 4696.370f, -6418.275f, 64.033f + 2, },  // 52
    { 4711.732f, -6467.191f, 54.221f + 2, },  // 53
    { 4740.886f, -6577.352f, 63.428f + 2, },  // 54
    { 4758.470f, -6711.137f, 85.886f + 2, },  // 55
    { 4757.949f, -6783.067f, 104.71f + 2, },  // 56
    { 4735.696f, -6849.752f, 124.73f + 2, },  // 57
    { 4695.719f, -6908.222f, 141.57f + 2, },  // 58
    { 4650.866f, -6966.891f, 149.94f + 2, },  // 59
    { 4625.907f, -7016.290f, 155.76f + 2, },  // 60
    { 4602.122f, -7049.343f, 156.60f + 2, },  // 61
    { 4611.300f, -7086.024f, 125.70f + 2, },  // 62
    { 4639.461f, -7043.100f, 113.55f + 1, },  // 63
};

uint32 const RocketwayTransportPathSize13 = 27;
const G3D::Vector3 RocketwayTransportPath13[RocketwayTransportPathSize13] =
{
    { 4474.219f, -5732.764f, 147.12f + 2, },  // 0 
    { 4488.040f, -5809.625f, 125.70f + 2, },  // 1 
    { 4488.664f, -5847.970f, 121.95f + 2, },  // 2 
    { 4484.779f, -5875.894f, 131.12f + 2, },  // 3 
    { 4482.883f, -5924.167f, 128.46f + 2, },  // 4 
    { 4490.574f, -6005.869f, 102.09f + 2, },  // 5 
    { 4498.412f, -6092.642f, 100.29f + 2, },  // 6 
    { 4510.510f, -6147.913f, 98.430f + 2, },  // 7 
    { 4525.141f, -6165.804f, 101.00f + 2, },  // 8 
    { 4547.924f, -6183.804f, 114.75f + 2, },  // 9 
    { 4589.117f, -6228.248f, 125.68f + 2, },  // 10
    { 4627.375f, -6249.052f, 143.51f + 1, },  // 11
    { 4651.208f, -6266.880f, 150.569f, },     // 12
    { 4673.215f, -6297.484f, 149.697f, },     // 13
    { 4687.634f, -6337.202f, 127.065f, },     // 14
    { 4696.370f, -6418.275f, 64.033f + 2, },  // 14 
    { 4711.732f, -6467.191f, 54.221f + 2, },  // 15
    { 4740.886f, -6577.352f, 63.428f + 2, },  // 16
    { 4758.470f, -6711.137f, 85.886f + 2, },  // 17
    { 4757.949f, -6783.067f, 104.71f + 2, },  // 18
    { 4735.696f, -6849.752f, 124.73f + 2, },  // 19
    { 4695.719f, -6908.222f, 141.57f + 2, },  // 20
    { 4650.866f, -6966.891f, 149.94f + 2, },  // 21
    { 4625.907f, -7016.290f, 155.76f + 2, },  // 22
    { 4602.122f, -7049.343f, 156.60f + 2, },  // 23
    { 4611.300f, -7086.024f, 125.70f + 2, },  // 24
    { 4639.461f, -7043.100f, 113.55f + 1, },  // 26
};

uint32 const RocketwayTransportPathSize14 = 20;
const G3D::Vector3 RocketwayTransportPath14[RocketwayTransportPathSize14] =
{
    { 4468.080f, -5702.234f, 146.662f + 2, },  // 0 
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 0 
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 1 
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 2 
    { 4253.4340f, -5571.394f, 223.99f, },  	   // 3 
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 4 
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 5 
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 6 
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 7 
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 8 
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 9 
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 10
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 11
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 12
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 13
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 14
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 15
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 16
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 17
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 18
};

uint32 const RocketwayTransportPathSize15 = 30;
const G3D::Vector3 RocketwayTransportPath15[RocketwayTransportPathSize15] =
{
    { 4468.080f, -5702.234f, 146.662f + 2, },  // 0 
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 0 
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 1 
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 2 
    { 4253.4340f, -5571.394f, 223.99f, },  	   // 3 
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 4 
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 5 
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 6 
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 7 
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 8 
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 9 
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 10
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 11
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 12
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 13
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 14
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 15
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 16
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 17
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 18
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 19
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 20
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 21
    { 3253.257f, -4834.690f, 170.010f + 2, },  // 22
    { 3208.953f, -4840.385f, 163.046f + 2, },  // 23
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 24
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 25
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 26
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 27
    { 2954.718f, -4997.358f, 165.665f + 2, },  // 28
};

uint32 const RocketwayTransportPathSize16 = 52;
const G3D::Vector3 RocketwayTransportPath16[RocketwayTransportPathSize16] =
{
    { 4468.080f, -5702.234f, 146.662f + 2, },  // 0 
    { 4460.0219f, -5675.809f, 146.41f + 2, },  // 0 
    { 4418.7592f, -5629.139f, 173.03f + 2, },  // 1 
    { 4380.5952f, -5614.462f, 200.54f + 2, },  // 2 
    { 4253.4340f, -5571.394f, 223.99f, },  	   // 3 
    { 4042.0449f, -5492.541f, 157.41f + 2, },  // 4 
    { 3968.8161f, -5482.600f, 163.31f + 2, },  // 5 
    { 3888.7045f, -5434.486f, 175.70f + 2, },  // 6 
    { 3856.6157f, -5369.654f, 170.54f + 2, },  // 7 
    { 3847.7124f, -5275.977f, 158.26f + 2, },  // 8 
    { 3812.7578f, -5179.394f, 158.57f + 2, },  // 9 
    { 3782.9211f, -5140.983f, 167.32f + 2, },  // 10
    { 3764.4763f, -5076.645f, 173.80f + 2, },  // 11
    { 3789.7263f, -5011.747f, 162.70f + 2, },  // 12
    { 3803.4770f, -4964.210f, 155.50f + 2, },  // 13
    { 3793.2106f, -4941.356f, 148.87f + 2, },  // 14
    { 3732.9296f, -4869.757f, 146.33f + 2, },  // 15
    { 3675.2270f, -4817.172f, 148.78f + 2, },  // 16
    { 3610.2160f, -4795.018f, 158.46f + 2, },  // 17
    { 3557.4160f, -4795.477f, 161.26f + 1, },  // 18
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 19
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 20
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 21
    { 3253.257f, -4834.690f, 170.010f + 2, },  // 22
    { 3208.953f, -4840.385f, 163.046f + 2, },  // 23
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 24
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 25
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 26
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 27
    { 2954.718f, -4997.358f, 165.665f + 2, },  // 28
    { 2948.488f, -5018.611f, 165.138f + 2, },  // 29
    { 2932.470f, -5058.654f, 168.622f + 2, },  // 30
    { 2923.969f, -5150.078f, 126.504f + 2, },  // 31
    { 2917.345f, -5200.492f, 130.763f + 2, },  // 32
    { 2907.605f, -5260.942f, 140.698f + 2, },  // 33
    { 2905.805f, -5304.108f, 130.926f + 2, },  // 34
    { 2873.228f, -5360.751f, 129.733f + 2, },  // 35
    { 2844.701f, -5409.658f, 143.221f + 2, },  // 36
    { 2838.625f, -5493.200f, 142.997f + 2, },  // 37
    { 2839.272f, -5551.992f, 135.089f + 2, },  // 38
    { 2825.451f, -5685.107f, 129.097f + 2, },  // 39
    { 2795.288f, -5749.038f, 121.145f + 2, },  // 40
    { 2733.443f, -5821.934f, 101.814f + 2, },  // 41
    { 2694.786f, -5861.976f, 98.8925f + 2, },  // 42
    { 2660.803f, -5942.058f, 98.9480f + 2, },  // 43
    { 2631.225f, -6004.947f, 102.054f + 2, },  // 44
    { 2637.438f, -6072.747f, 102.392f + 2, },  // 45
    { 2643.590f, -6106.774f, 124.183f + 2, },  // 46
    { 2658.743f, -6146.762f, 137.520f + 2, },  // 47
    { 2659.132f, -6191.978f, 139.175f + 1, },   // 48
    { 2657.509f, -6201.29f, 109.43f,  },       // 49
    { 2656.752f, -6214.426f, 100.570f + 1,  }, // 50
};

uint32 const RocketwayTransportPathSize17 = 10;
const G3D::Vector3 RocketwayTransportPath17[RocketwayTransportPathSize17] =
{
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 0 
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 1 
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 2 
    { 3253.257f, -4834.690f, 170.010f + 2, },  // 3 
    { 3208.953f, -4840.385f, 163.046f + 2, },  // 4 
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 5 
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 6 
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 7 
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 8 
    { 2954.718f, -4997.358f, 165.665f + 2, },  // 9 
};

uint32 const RocketwayTransportPathSize18 = 32;
const G3D::Vector3 RocketwayTransportPath18[RocketwayTransportPathSize18] =
{
    { 3544.379f, -4795.128f, 160.992f + 2, },  // 0 
    { 3485.609f, -4794.089f, 158.850f + 2, },  // 1 
    { 3390.530f, -4815.787f, 155.089f + 2, },  // 2 
    { 3253.257f, -4834.690f, 170.010f + 2, },  // 3 
    { 3208.953f, -4840.385f, 163.046f + 2, },  // 4 
    { 3157.146f, -4841.279f, 140.236f + 2, },  // 5 
    { 3116.917f, -4847.308f, 143.143f + 2, },  // 6 
    { 3028.660f, -4894.705f, 163.094f + 2, },  // 7 
    { 2973.258f, -4944.547f, 167.508f + 2, },  // 8 
    { 2954.718f, -4997.358f, 165.665f + 2, },  // 9 
    { 2948.488f, -5018.611f, 165.138f + 2, },  // 10
    { 2932.470f, -5058.654f, 168.622f + 2, },  // 11
    { 2923.969f, -5150.078f, 126.504f + 2, },  // 12
    { 2917.345f, -5200.492f, 130.763f + 2, },  // 13
    { 2907.605f, -5260.942f, 140.698f + 2, },  // 14
    { 2905.805f, -5304.108f, 130.926f + 2, },  // 15
    { 2873.228f, -5360.751f, 129.733f + 2, },  // 16
    { 2844.701f, -5409.658f, 143.221f + 2, },  // 17
    { 2838.625f, -5493.200f, 142.997f + 2, },  // 18
    { 2839.272f, -5551.992f, 135.089f + 2, },  // 19
    { 2825.451f, -5685.107f, 129.097f + 2, },  // 20
    { 2795.288f, -5749.038f, 121.145f + 2, },  // 21
    { 2733.443f, -5821.934f, 101.814f + 2, },  // 22
    { 2694.786f, -5861.976f, 98.8925f + 2, },  // 23
    { 2660.803f, -5942.058f, 98.9480f + 2, },  // 24
    { 2631.225f, -6004.947f, 102.054f + 2, },  // 25
    { 2637.438f, -6072.747f, 102.392f + 2, },  // 26
    { 2643.590f, -6106.774f, 124.183f + 2, },  // 27
    { 2658.743f, -6146.762f, 137.520f + 2, },  // 28
    { 2659.132f, -6191.978f, 139.175f + 1, },   // 29
    { 2657.509f, -6201.29f, 109.43f,  },       // 30
    { 2656.752f, -6214.426f, 100.570f + 1,  }, // 31
};

uint32 const RocketwayTransportPathSize19 = 28;
const G3D::Vector3 RocketwayTransportPath19[RocketwayTransportPathSize19] =
{
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 0 
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 1 
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 2 
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 3 
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 4 
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 5 
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 6 
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 7 
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 8 
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 9 
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 10
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 11
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 12
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 13
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 14
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 15
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 16
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 17
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 18
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 19
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 20
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 21
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 22
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 23
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 24
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 25
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 26
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 27
};

uint32 const RocketwayTransportPathSize20 = 55;
const G3D::Vector3 RocketwayTransportPath20[RocketwayTransportPathSize20] =
{
    { 3584.896f, -4794.641f, 160.781f + 2, }, // 0 
    { 3643.230f, -4801.631f, 153.374f + 2, }, // 1 
    { 3682.057f, -4821.644f, 147.264f + 2, }, // 2 
    { 3752.727f, -4888.020f, 144.921f + 2, }, // 3 
    { 3790.347f, -4937.042f, 146.737f + 2, }, // 4 
    { 3803.619f, -4973.270f, 160.295f + 2, }, // 5 
    { 3786.177f, -5015.647f, 165.906f + 2, }, // 6 
    { 3764.685f, -5094.821f, 174.188f + 2, }, // 7 
    { 3778.930f, -5135.916f, 168.029f + 2, }, // 8 
    { 3803.889f, -5165.399f, 160.258f + 2, }, // 9 
    { 3839.559f, -5242.152f, 157.292f + 2, }, // 10
    { 3851.183f, -5295.309f, 158.795f + 2, }, // 11
    { 3856.004f, -5363.675f, 168.459f + 2, }, // 12
    { 3876.015f, -5418.092f, 174.762f + 2, }, // 13
    { 3909.820f, -5453.081f, 171.952f + 2, }, // 14
    { 3958.430f, -5480.832f, 161.259f + 2, }, // 15
    { 4015.268f, -5486.690f, 153.546f + 2, }, // 16
    { 4063.040f, -5495.896f, 142.668f + 2, }, // 17
    { 4136.567f, -5534.792f, 123.514f + 2, }, // 18
    { 4207.891f, -5567.928f, 120.417f + 2, }, // 19
    { 4266.840f, -5585.207f, 121.512f + 2, }, // 20
    { 4312.626f, -5595.762f, 121.506f + 2, }, // 21
    { 4362.994f, -5610.399f, 164.568f + 2, }, // 22
    { 4397.717f, -5620.770f, 178.268f + 2, }, // 23
    { 4426.406f, -5632.926f, 161.687f + 2, }, // 24
    { 4456.124f, -5667.246f, 146.087f + 2, }, // 25
    { 4465.575f, -5695.075f, 146.627f + 2, }, // 26
    { 4469.562f, -5715.583f, 145.507f + 1, }, // 27
    { 4474.219f, -5732.764f, 146.12f + 2, },  // 28
    { 4488.040f, -5809.625f, 125.70f + 2, },  // 29
    { 4488.664f, -5847.970f, 121.95f + 2, },  // 30
    { 4484.779f, -5875.894f, 131.12f + 2, },  // 31
    { 4482.883f, -5924.167f, 128.46f + 2, },  // 32
    { 4490.574f, -6005.869f, 102.09f + 2, },  // 33
    { 4498.412f, -6092.642f, 100.29f + 2, },  // 34
    { 4510.510f, -6147.913f, 98.430f + 2, },  // 35
    { 4525.141f, -6165.804f, 101.00f + 2, },  // 36
    { 4547.924f, -6183.804f, 114.75f + 2, },  // 37
    { 4589.117f, -6228.248f, 125.68f + 2, },  // 38
    { 4627.375f, -6249.052f, 143.51f + 2, },  // 39
    { 4651.208f, -6266.880f, 150.569f, },     // 40
    { 4673.215f, -6297.484f, 149.697f, },     // 41
    { 4687.634f, -6337.202f, 127.065f, },     // 42
    { 4696.370f, -6418.275f, 64.033f + 2, },  // 43
    { 4711.732f, -6467.191f, 54.221f + 2, },  // 44
    { 4740.886f, -6577.352f, 63.428f + 2, },  // 45
    { 4758.470f, -6711.137f, 85.886f + 2, },  // 46
    { 4757.949f, -6783.067f, 104.71f + 2, },  // 47
    { 4735.696f, -6849.752f, 124.73f + 2, },  // 48
    { 4695.719f, -6908.222f, 141.57f + 2, },  // 49
    { 4650.866f, -6966.891f, 149.94f + 2, },  // 50
    { 4625.907f, -7016.290f, 155.76f + 2, },  // 51
    { 4602.122f, -7049.343f, 156.60f + 2, },  // 52
    { 4611.300f, -7086.024f, 125.70f + 2, },  // 53
    { 4639.461f, -7043.100f, 113.55f + 1, },  // 54
};

enum RocketwayTransport
{
    SPELL_ACHIEVEMENT_JOY_RIDE = 92686,
    SPELL_ROCKETWAY_TAKEOFF = 80749,
    SPELL_RIDE_VEHICLE_ROCKETWAY = 80804,
    SPELL_FORCECAST_SPEED_BOOST = 80796, // ID - 80794 SPEED BOOST
    SPELL_FORCECAST_SLOW_SPEED = 80799, // ID - 80795 Slow Speed
};

class npc_redhound_projectile_transits : public CreatureScript
{
public:
    npc_redhound_projectile_transits() : CreatureScript("npc_redhound_projectile_transits") { }

    struct npc_redhound_projectile_transitsAI : public npc_escortAI
    {
        npc_redhound_projectile_transitsAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                playerGUID = player->GetGUID();

            _rocketEntry = me->GetEntry();
            me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = passenger->ToPlayer())
                {
                    me->SetRooted(true);
                    me->CastSpell(me, SPELL_ROCKETWAY_TAKEOFF);
                    playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_ROCKETWAY_TRANSIT_1, 5000);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_rocketEntry == 43211)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 17:
                        me->RemoveAura(SPELL_FLIGHT_SPEED_280);
                        break;
                    case 19:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 5, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43240)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 38:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43251)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 38:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 48:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43256)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 38:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 48:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 70:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        me->CastSpell(me, SPELL_ACHIEVEMENT_JOY_RIDE);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43283)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 4:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 21:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43284)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 4:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 21:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 31:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43285)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 4:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 21:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 31:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 59:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43306)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 4:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 21:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 31:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 59:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 86:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        me->CastSpell(me, SPELL_ACHIEVEMENT_JOY_RIDE);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43307)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 21:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43308)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 9:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43309)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 9:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 37:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43310)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 9:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 37:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 64:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43315)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 26:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43317)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43316)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 29:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43311)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 19:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 29:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 51:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43312)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 9:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43314)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 9:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 31:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43288)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 27:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
                    }
                }
            }
            else if (_rocketEntry == 43313)
            {
                if (auto player = Unit::GetPlayer(*me, playerGUID))
                {
                    switch (point)
                    {
                    case 1:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 27:
                        player->CastSpell(player, SPELL_FORCECAST_SPEED_BOOST);
                        break;
                    case 54:
                        me->DespawnOrUnsummon(100);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ() - 1, 8.0f, 10.0f);
                        break;
                    default:
                        break;
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
                case EVENT_ROCKETWAY_TRANSIT_1:
                    switch (_rocketEntry)
                    {
                    case 43211:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath1, RocketwayTransportPathSize1);
                        break;
                    case 43240:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath2, RocketwayTransportPathSize2);
                        break;
                    case 43251:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath3, RocketwayTransportPathSize3);
                        break;
                    case 43256:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath4, RocketwayTransportPathSize4);
                        break;
                    case 43283:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath5, RocketwayTransportPathSize5);
                        break;
                    case 43284:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath6, RocketwayTransportPathSize6);
                        break;
                    case 43285:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath7, RocketwayTransportPathSize7);
                        break;
                    case 43306:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath8, RocketwayTransportPathSize8);
                        break;
                    case 43307:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath9, RocketwayTransportPathSize9);
                        break;
                    case 43308:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath10, RocketwayTransportPathSize10);
                        break;
                    case 43309:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath11, RocketwayTransportPathSize11);
                        break;
                    case 43310:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath12, RocketwayTransportPathSize12);
                        break;
                    case 43315:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath13, RocketwayTransportPathSize13);
                        break;
                    case 43317:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath14, RocketwayTransportPathSize14);
                        break;
                    case 43316:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath15, RocketwayTransportPathSize15);
                        break;
                    case 43311:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath16, RocketwayTransportPathSize16);
                        break;
                    case 43312:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath17, RocketwayTransportPathSize17);
                        break;
                    case 43314:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath18, RocketwayTransportPathSize18);
                        break;
                    case 43288:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath19, RocketwayTransportPathSize19);
                        break;
                    case 43313:
                        me->SetRooted(false);
                        me->GetMotionMaster()->MoveSmoothPath(RocketwayTransportPath20, RocketwayTransportPathSize20);
                        break;
                    default:
                        break;
                    }
                default:
                    break;
                }
            }
        }
    private:
        uint16 _rocketEntry = 0;
        EventMap _events;
        uint64 playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_redhound_projectile_transitsAI(creature);
    }
};

/*
    QUEST_THE_CAPTAINS_LOGS     = 14134,
    QUEST_UP_A_TREE             = 14135,
    QUEST_DEFEND_THE_GATES      = 14146,
    QUEST_ARBORCIDE             = 14155,
    QUEST_SURVEY_THE_LAKESHORE  = 14202,
    QUEST_RAPTOR_RAPTOR_ROCKET  = 14422,
    QUEST_TROUBLE_UNDER_FOOT	= 14226,

*/

class npc_runaway_shredder_vehicle : public CreatureScript
{
public:
    npc_runaway_shredder_vehicle() : CreatureScript("npc_runaway_shredder_vehicle") { }

    struct npc_runaway_shredder_vehicleAI : public VehicleAI
    {
        npc_runaway_shredder_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                player->CastSpell(me, SPELL_RIDE_VEHICLE);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
        {
            if (apply)
            {
                _events.CancelEvent(EVENT_SHREDDER_DESPAWN);
                if (auto player = passenger->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                }
            }

            if (!apply)
            {
                _playerGUID = 0;
                _events.ScheduleEvent(EVENT_SHREDDER_DESPAWN, 30000);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 500;

                if (_isInAzshara)
                {
                    if (me->GetZoneId() != 16)
                    {
                        _isInAzshara = false;
                        me->AI()->Talk(4, _playerGUID);
                        _events.ScheduleEvent(EVENT_SHREDDER_DESPAWN, 10000);
                    }
                }
                else
                {
                    if (me->GetZoneId() == 16)
                    {
                        _events.CancelEvent(EVENT_SHREDDER_DESPAWN);
                        _isInAzshara = true;
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_THE_CAPTAINS_LOGS) == QUEST_STATUS_INCOMPLETE && !_introLumberText)
                    {
                        _introLumberText = true;
                        player->CastSpell(player, 151301);
                        me->AI()->Talk(0, _playerGUID);
                    }
                    else if (player->GetQuestStatus(QUEST_UP_A_TREE) == QUEST_STATUS_INCOMPLETE && !_introTreeText)
                    {
                        _introTreeText = true;
                        player->CastSpell(player, 151301);
                        me->AI()->Talk(1, _playerGUID);
                    }
                    else if (player->GetQuestStatus(QUEST_DEFEND_THE_GATES) == QUEST_STATUS_INCOMPLETE && !_introDefendText)
                    {
                        _introDefendText = true;
                        player->CastSpell(player, 151301);
                        me->AI()->Talk(2, _playerGUID);
                    }
                    else if (player->GetQuestStatus(QUEST_ARBORCIDE) == QUEST_STATUS_INCOMPLETE && !_introArborcideText)
                    {
                        _introArborcideText = true;
                        player->CastSpell(player, 151301);
                        me->AI()->Talk(3, _playerGUID);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHREDDER_DESPAWN:
                    me->DespawnOrUnsummon(100);
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isInAzshara = true;
        bool _introLumberText = false;
        bool _introTreeText = false;
        bool _introDefendText = false;
        bool _introArborcideText = false;
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runaway_shredder_vehicleAI(creature);
    }
};

class spell_empowered_naga_health : public SpellScriptLoader
{
public:
    spell_empowered_naga_health() : SpellScriptLoader("spell_empowered_naga_health") { }

    class spell_empowered_naga_health_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_empowered_naga_health_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(68266) || !sSpellMgr->GetSpellInfo(68255))
                return false;
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->ToCreature()->SetCreateHealth(5);
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_empowered_naga_health_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_empowered_naga_health_AuraScript();
    }
};

class npc_power_stones_azshara : public CreatureScript
{
public:
    npc_power_stones_azshara() : CreatureScript("npc_power_stones_azshara") { }

    struct npc_power_stones_azsharaAI : public ScriptedAI
    {
        npc_power_stones_azsharaAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
            {
                if (me->GetEntry() == 35892 && !_clicked)
                {
                    if (auto silistra = me->FindNearestCreature(35880, 40))
                    {
                        me->CastSpell(me, 98715);
                        silistra->CastSpell(silistra, 98715);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        silistra->RemoveAura(68255);
                        silistra->RemoveAura(46933);
                        silistra->AI()->AttackStart(who);
                        _clicked = true;
                        _events.ScheduleEvent(EVENT_POWER_STONE_TIMER, 90000);
                    }
                }

                if (me->GetEntry() == 35968 && !_clicked)
                {
                    if (auto vesthra = me->FindNearestCreature(35881, 40))
                    {
                        me->CastSpell(me, 98715);
                        vesthra->CastSpell(vesthra, 98715);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        vesthra->RemoveAura(68266);
                        vesthra->RemoveAura(68268);
                        vesthra->AI()->AttackStart(who);
                        _clicked = true;
                        _events.ScheduleEvent(EVENT_POWER_STONE_TIMER, 90000);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto silistra = me->FindNearestCreature(35880, 40))
                {
                    if (!silistra->GetAura(68255) && !_clicked)
                        me->CastSpell(silistra, 68255);
                }

                if (auto vesthra = me->FindNearestCreature(35881, 40))
                {
                    if (!vesthra->GetAura(68266) && !_clicked)
                        me->CastSpell(vesthra, 68266);
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (eventId == EVENT_POWER_STONE_TIMER)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    _clicked = false;
                }
            }
        }
    private:
        bool _clicked = false;
        uint32 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_power_stones_azsharaAI(creature);
    }
};

void AddSC_azshara()
{
    new spell_runaway_shredder_update_spellbar();
    new npc_azshara_sapling();
    new npc_summon_sniper_controller();
    new npc_bilgewater_mortar_cannon();
    new npc_goblin_surveyor_jr_grade();
    new npc_clubniks_dozer();
    new npc_priest_dozercism();
    new npc_azsharite_core_passenger();
    new npc_azsharite_core_cosmetic_fire();
    new npc_lab_azsharite_core_transport();
    new npc_rocketway_rat_azshara();
    new npc_mutant_goblin_azshara();
    new npc_experimental_raptor_standalone();
    new npc_experimental_raptor_summoned();
    new areatrigger_azshara_the_secret_lab();
    new npc_azshara_loaned_gyrocopter();
    new npc_gurgll_the_tracking_turtle();
    new spell_besalt_bunny_spell_spawner();
    new npc_personal_arcane_assistant_azshara();
    new npc_unleashed_magic_vehicle();
    new npc_archmage_xylem_ontop_azshara();
    new npc_arcane_detonation_trigger();
    new npc_arcane_construct_azshara();
    new npc_surface_transport_spellclick();
    new npc_surface_transport_vehicle();
    new spell_gob_squad_flare();
    new npc_gob_squad_azshara();
    new npc_runestone_azshara_trigger();
    new npc_azshara_military_gyrocopter();
    new npc_wings_of_steel_spellclick();
    new npc_wings_of_steel_airplane();
    new npc_slinky_sharpshiv_q_giver();
    new npc_climbing_slinky_player();
    new npc_pushthebutton_master();
    new npc_valormok_wind_rider();
    new npc_trial_of_fire_controller_azshara();
    new npc_trial_of_fire_pillar();
    new npc_darwin_azshara();
    new playerscript_azshara_trial_of_fire();
    new playerscript_azshara_trial_of_frost();
    new playerscript_azshara_trial_of_shadow();
    new npc_eye_of_frost_trial();
    new npc_eye_of_frost_beam_target();
    new spell_cancel_frost_credit_application();
    new spell_essence_of_ice();
    new spell_shadow_bait_trial();
    new spell_fire_dancing();
    new npc_weeping_soul();
    new spell_fall_to_death_portal_tick();
    new npc_malicion_azshara();
    new npc_lord_kaltrana_azshara();
    new npc_redhound_projectile_transits();
    new spell_empowered_naga_health();
    new npc_power_stones_azshara();
    new npc_runaway_shredder_vehicle();
};