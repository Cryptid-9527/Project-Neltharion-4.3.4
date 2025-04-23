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
 */

 /* ScriptData
 SDName: Hinterlands
 SD%Complete: 100
 SDComment: Wondi Scripts!
 SDCategory: The Hinterlands
 EndScriptData */

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

enum TheHinterlandsDefaultSpells
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
    SPELL_FLIGHT_SPEED_200 = 55034,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum TheHinterlandsEvents
{
    EVENT_ONGOLONGO_1 = 1,
    EVENT_ONGOLONGO_2,
    EVENT_ONGOLONGO_3,
    EVENT_ONGOLONGO_SLAM,
    EVENT_ONGOLONGO_ENRAGE,
    EVENT_RAZORBEAK_1,
    EVENT_RAZORBEAK_REND,
    EVENT_SHARPBEAK_1,
    EVENT_OOX_1,
};

// ############# SCRIPTS ###############

uint32 const OngolongoPathSize = 4;
const G3D::Vector3 OngolongoPath[OngolongoPathSize] =
{
    { -469.184f, -3974.17f, 203.761f },
    { -472.668f, -3977.35f, 205.099f },
    { -480.201f, -3983.75f, 208.808f },
    { -490.780f, -3993.32f, 214.347f }, // 3
};

uint32 const OngolongoPathSize2 = 4;
const G3D::Vector3 OngolongoPath2[OngolongoPathSize2] =
{
    { -495.2925f, -3997.7351f, 215.4202f },
    { -501.7660f, -4004.6193f, 216.3574f },
    { -509.5754f, -4012.9509f, 215.7775f },
    { -523.3663f, -4027.2805f, 215.8602f }, // 3
};

enum QuestOngolongosRevenge
{
    QUEST_ONGOLONGO_A = 26515,
    QUEST_ONGOLONGO_H = 26367,

    NPC_ONGOLONGOS_RIGHT_HAND = 42816,
    NPC_ONGOLONGOS_RIGHT_HAND_TARGET = 75042,
    NPC_ONGOLONGOS_LEFT_HAND = 42817,
    NPC_ONGOLONGOS_LEFT_HAND_TARGET = 75044,
    NPC_VILEBRANCH_HANDLER = 42843,

    SPELL_ONGOLONGO_ENRAGE = 63227,
    SPELL_ONGOLONGO_SLAM = 90325,
    SPELL_ONGOLONGO_SMASH = 79875,
    SPELL_ONGOLONGO_CHAIN_VISUAL_1 = 79806,
    SPELL_ONGOLONGO_CHAIN_VISUAL_2 = 79806,
};

class npc_ongolongo : public CreatureScript
{
public:
    npc_ongolongo() : CreatureScript("npc_ongolongo") { }

    struct npc_ongolongoAI : public ScriptedAI
    {
        npc_ongolongoAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->setActive(true);
            _escapeEventEnabled = false;
            _pathMode = 0;
            me->GetMotionMaster()->Clear();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            me->setFaction(21);

            if (auto righthand = me->SummonCreature(NPC_ONGOLONGOS_RIGHT_HAND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                if (auto lefthand = me->SummonCreature(NPC_ONGOLONGOS_RIGHT_HAND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {                  
                    righthand->setActive(true);
                    lefthand->setActive(true);
                    _righthandGUID = righthand->GetGUID();
                    _lefthandGUID = lefthand->GetGUID();
                    _summons.Summon(righthand);
                    _summons.Summon(lefthand);
                    righthand->CastWithDelay(300, me, SPELL_RIDE_VEHICLE);
                    lefthand->CastWithDelay(500, me, SPELL_RIDE_VEHICLE);

                    if (auto* target1 = me->FindNearestCreature(NPC_ONGOLONGOS_LEFT_HAND_TARGET, 40.f))
                        target1->CastWithDelay(300, lefthand, SPELL_ONGOLONGO_CHAIN_VISUAL_1);

                    if (auto* target2 = me->FindNearestCreature(NPC_ONGOLONGOS_RIGHT_HAND_TARGET, 40.f))
                        target2->CastWithDelay(300, righthand, SPELL_ONGOLONGO_CHAIN_VISUAL_2);
                }
        }

        void JustDied(Unit* killer)
        {
            _pathMode = 0;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            _escapeEventEnabled = false;
            _summons.DespawnAll();
        }

        void EnterEvadeMode()
        {
            _summons.DespawnAll();
            Reset();
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (_escapeEventEnabled)
            {
                if (damage >= me->GetHealth())
                    damage = 0;
                if ((me->GetHealthPct() <= 10))
                {
                    _escapeEventEnabled = false;

                    if (auto righthand = Unit::GetCreature(*me, _righthandGUID))
                        if (auto lefthand = Unit::GetCreature(*me, _lefthandGUID))
                        {
                            me->CastStop();
                            _events.CancelEvent(EVENT_ONGOLONGO_SLAM);
                            _events.CancelEvent(EVENT_ONGOLONGO_ENRAGE);
                            me->SetReactState(REACT_PASSIVE);
                            me->DeleteThreatList();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            Talk(0);

                            righthand->GetMotionMaster()->MoveJump(me->GetPositionX() + 4, me->GetPositionY(), me->GetPositionZ(), 10, 12);
                            lefthand->GetMotionMaster()->MoveJump(me->GetPositionX() - 4, me->GetPositionY(), me->GetPositionZ(), 10, 12);
                           
                            _events.ScheduleEvent(EVENT_ONGOLONGO_1, 3000);
                            auto player = me->GetPlayersInRange(50, true);
                            for (auto it : player)
                                if (it->GetQuestStatus(QUEST_ONGOLONGO_A) == QUEST_STATUS_INCOMPLETE ||
                                    it->GetQuestStatus(QUEST_ONGOLONGO_H) == QUEST_STATUS_INCOMPLETE)
                                {
                                    it->CastSpell(it, SPELL_SANCTUARY_NO_COMBAT);
                                    it->KilledMonsterCredit(me->GetEntry());
                                }
                        }                    
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_pathMode == 1)
            {
                if (point == 1)
                {
                    if (!me->FindNearestCreature(NPC_VILEBRANCH_HANDLER, 30.f))
                    {
                        if (auto handler = me->SummonCreature(NPC_VILEBRANCH_HANDLER, -494.8376f, -3997.462f, 215.35f, 0.828f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000))
                        {
                            handler->SetReactState(REACT_PASSIVE);
                            handler->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                        }
                    }
                }

                if (point == 3)
                {
                    _pathMode = 2;
                    _events.ScheduleEvent(EVENT_ONGOLONGO_2, 2500);

                    if (auto handler = me->FindNearestCreature(NPC_VILEBRANCH_HANDLER, 15.f))
                        if (auto handler1 = handler->AI())
                            handler1->TalkWithDelay(1000, 0);
                }
            }
            else
            {
                if (point == 3)
                {
                    _escapeEventEnabled = false;
                    _summons.DespawnAll();
                    Talk(2);
                    me->DespawnOrUnsummon(3000);
                }
            }
        }

        void EnterCombat(Unit* who)
        {
            _events.ScheduleEvent(EVENT_ONGOLONGO_SLAM, urand(3000, 8000));
            _events.ScheduleEvent(EVENT_ONGOLONGO_ENRAGE, urand(8000, 15000));

            if (auto player = who->ToPlayer())
            {
                if (player->GetQuestStatus(QUEST_ONGOLONGO_A) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_ONGOLONGO_H) == QUEST_STATUS_INCOMPLETE)
                    _escapeEventEnabled = true;
                else
                        _escapeEventEnabled = false;                
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ONGOLONGO_1:
                    _pathMode = 1;
                    me->GetMotionMaster()->MoveSmoothPath(OngolongoPath, OngolongoPathSize);
                    break;
                case EVENT_ONGOLONGO_2:
                    if (auto handler = me->FindNearestCreature(NPC_VILEBRANCH_HANDLER, 15.f))
                    {
                        me->CastSpell(handler, SPELL_ONGOLONGO_SMASH);
                        me->AI()->Talk(1);
                        _events.ScheduleEvent(EVENT_ONGOLONGO_3, 1500);
                    }
                    break;
                case EVENT_ONGOLONGO_3:
                    me->GetMotionMaster()->MoveSmoothPath(OngolongoPath2, OngolongoPathSize2);
                    break;
                case EVENT_ONGOLONGO_SLAM:
                    if (auto victim = me->getVictim())
                        me->CastSpell(victim, SPELL_ONGOLONGO_SLAM);

                    _events.ScheduleEvent(EVENT_ONGOLONGO_SLAM, urand(7000, 12000));
                    break;
                case EVENT_ONGOLONGO_ENRAGE:
                    me->CastSpell(me, SPELL_ONGOLONGO_ENRAGE);
                    _events.ScheduleEvent(EVENT_ONGOLONGO_ENRAGE, urand(16000, 26000));
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _pathMode = true;
        bool _escapeEventEnabled = false;
        uint64 _righthandGUID = 0;
        uint64 _lefthandGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ongolongoAI(creature);
    }
};

enum FinalQuestHinterlands
{
    NPC_SHADRA = 43007,

    NPC_DEATHSTALKER_LOOKOUT = 43299,
    NPC_WILDHAMMER_LOOKOUT = 43298,

    NPC_HORDE_DEATHSTALKER_BODYCHOPPER = 43016,
    NPC_HORDE_DARKCLERIC_MARNAL = 43018,

    NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER = 43213,
    NPC_ALLIANCE_GRYPHON_MASTER_TALONAXE = 43215,

    SPELL_BURN_SHADRAS_CORPSE = 80753,
};

const Position ActorSpawnPosition[] =
{
    { -298.918f, -2915.861f, 80.708f, 2.570f }, // leader
    { -295.794f, -2914.231f, 81.249f, 2.556f },
    { -292.560f, -2912.134f, 82.023f, 2.674f },
    { -295.164f, -2918.012f, 81.276f, 2.595f },
    { -299.470f, -2919.327f, 81.077f, 2.489f },
    { -300.035f, -2923.544f, 81.749f, 2.462f },
    { -348.900f, -2884.642f, 63.197f, 5.76f}, // shadra
    { -308.681f, -2910.082f, 79.29f, 5.757f}, // shadra home pos
};

class npc_shadra_the_venom_queen_controller : public CreatureScript
{
public:
    npc_shadra_the_venom_queen_controller() : CreatureScript("npc_shadra_the_venom_queen_controller") { }

    struct npc_shadra_the_venom_queen_controllerAI : public ScriptedAI
    {
        npc_shadra_the_venom_queen_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (!_eventInProgress)
            {
                if (spellInfo->Id == SPELL_DUMMY_PING)
                {
                    if (who->GetEntry() == NPC_WILDHAMMER_LOOKOUT)
                    {
                        me->setActive(true);
                        _eventInProgress = true;

                        if (auto* talonaxe = me->SummonCreature(NPC_ALLIANCE_GRYPHON_MASTER_TALONAXE, ActorSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto* spirithunter1 = me->SummonCreature(NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER, ActorSpawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto* spirithunter2 = me->SummonCreature(NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER, ActorSpawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto* spirithunter3 = me->SummonCreature(NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER, ActorSpawnPosition[3], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto* spirithunter4 = me->SummonCreature(NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER, ActorSpawnPosition[4], TEMPSUMMON_MANUAL_DESPAWN))
                                            if (auto* spirithunter5 = me->SummonCreature(NPC_ALLIANCE_WILDHAMMER_SPIRITHUNTER, ActorSpawnPosition[5], TEMPSUMMON_MANUAL_DESPAWN))
                                                if (auto* shadra = me->SummonCreature(NPC_SHADRA, ActorSpawnPosition[6], TEMPSUMMON_MANUAL_DESPAWN))
                                                {
                                                    _summons.Summon(shadra);
                                                    _summons.Summon(talonaxe);
                                                    _summons.Summon(spirithunter1);
                                                    _summons.Summon(spirithunter2);
                                                    _summons.Summon(spirithunter3);
                                                    _summons.Summon(spirithunter4);
                                                    _summons.Summon(spirithunter5);

                                                    shadra->SetHomePosition(ActorSpawnPosition[7]);
                                                }
                    }
                    else if (who->GetEntry() == NPC_DEATHSTALKER_LOOKOUT)
                    {
                        me->setActive(true);
                        _eventInProgress = true;

                        if (auto* marnal = me->SummonCreature(NPC_HORDE_DARKCLERIC_MARNAL, ActorSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto* bodychopper1 = me->SummonCreature(NPC_HORDE_DEATHSTALKER_BODYCHOPPER, ActorSpawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto* bodychopper2 = me->SummonCreature(NPC_HORDE_DEATHSTALKER_BODYCHOPPER, ActorSpawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto* bodychopper3 = me->SummonCreature(NPC_HORDE_DEATHSTALKER_BODYCHOPPER, ActorSpawnPosition[3], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto* bodychopper4 = me->SummonCreature(NPC_HORDE_DEATHSTALKER_BODYCHOPPER, ActorSpawnPosition[4], TEMPSUMMON_MANUAL_DESPAWN))
                                            if (auto* bodychopper5 = me->SummonCreature(NPC_HORDE_DEATHSTALKER_BODYCHOPPER, ActorSpawnPosition[5], TEMPSUMMON_MANUAL_DESPAWN))
                                                if (auto* shadra = me->SummonCreature(NPC_SHADRA, ActorSpawnPosition[6], TEMPSUMMON_MANUAL_DESPAWN))
                                                {
                                                    _summons.Summon(shadra);
                                                    _summons.Summon(marnal);
                                                    _summons.Summon(bodychopper1);
                                                    _summons.Summon(bodychopper2);
                                                    _summons.Summon(bodychopper3);
                                                    _summons.Summon(bodychopper4);
                                                    _summons.Summon(bodychopper5);

                                                    shadra->SetHomePosition(ActorSpawnPosition[7]);
                                                }
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_eventInProgress)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 2000;

                    if (!me->FindNearestCreature(NPC_SHADRA, 80.f))
                    {
                        _eventInProgress = false;
                    }
                }
                else _checkTimer -= diff;
            }

            _events.Update(diff);
        }
    private:
        bool _eventInProgress = false;
        uint16 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadra_the_venom_queen_controllerAI(creature);
    }
};

enum QuestRazorbeakFriends
{
    NPC_TRAINED_CREDIT = 43236,
    SPELL_FEED_TRAINED_RAZORBEAK = 80782,
    SPELL_CUSTOM_AURA = 151100,
    SPELL_REND = 3147,
};

class npc_trained_razorbeak : public CreatureScript
{
public:
    npc_trained_razorbeak() : CreatureScript("npc_trained_razorbeak") { }

    struct npc_trained_razorbeakAI : public ScriptedAI
    {
        npc_trained_razorbeakAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_FEED_TRAINED_RAZORBEAK)
            {
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->CastSpell(me, SPELL_CUSTOM_AURA);
                me->GetMotionMaster()->MoveIdle();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                if (auto* player = who->ToPlayer())
                    player->KilledMonsterCredit(NPC_TRAINED_CREDIT);

                _events.ScheduleEvent(EVENT_RAZORBEAK_1, 3000);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_RAZORBEAK_REND, urand(3000, 8000));
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RAZORBEAK_1:
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);
                    me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 80, false);
                    me->DespawnOrUnsummon(5000);
                    break;
                case EVENT_RAZORBEAK_REND:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_REND);
                    _events.ScheduleEvent(EVENT_RAZORBEAK_REND, urand(13000, 20000));
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _eventInProgress = false;
        uint16 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trained_razorbeakAI(creature);
    }
};

uint32 const SharpbeakPathSize = 12;
const G3D::Vector3 SharpbeakPath[SharpbeakPathSize] =
{
    { -69.694f, -4035.593f, 124.25f },
    { -66.864f, -4033.497f, 132.16f },
    { -53.678f, -4024.429f, 148.80f },
    { -37.427f, -3992.712f, 178.74f },
    { -38.435f, -3951.580f, 245.24f },
    { -38.942f, -3911.865f, 264.22f },
    { -21.801f, -3824.561f, 273.07f },
    { -26.527f, -3712.926f, 270.27f },
    { -67.182f, -3636.416f, 276.71f },
    { -143.59f, -3600.477f, 275.83f },
    { -203.80f, -3611.066f, 257.19f },
    { -244.47f, -3637.044f, 237.47f }, // 11
};

class npc_sharpbeak_vehicle_transport : public CreatureScript
{
public:
    npc_sharpbeak_vehicle_transport() : CreatureScript("npc_sharpbeak_vehicle_transport") { }

    struct npc_sharpbeak_vehicle_transportAI : public VehicleAI
    {
        npc_sharpbeak_vehicle_transportAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                // player->CastSpell(me, SPELL_RIDE_DRAGONHAWK);
                _playerGUID = player->GetGUID();
            }

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_SHARPBEAK_1, 1000);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->DespawnOrUnsummon(200);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 11:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->DespawnOrUnsummon(200);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHARPBEAK_1:
                    me->SetRooted(false);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->GetMotionMaster()->MoveSmoothPath(SharpbeakPath, SharpbeakPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sharpbeak_vehicle_transportAI(creature);
    }
};

enum MechChickenHinterlands
{
    QUEST_RESQUE_OOX_09 = 836,

    NPC_MARAUDING_OWL = 7808,
    NPC_VILE_AMBUSHER = 7809,

    FACTION_ESCORTEE_A = 774,
    FACTION_ESCORTEE_H = 775,

    SPELL_OOX_LIFT_OFF = 68499,
};

const Position OwlSpawnPosition[] =
{
    { 189.514f, -3822.89f, 132.708f },
    { 184.506f, -3826.05f, 131.626f },
    { 173.017f, -3828.27f, 130.453f },
};

class npc_00x09hl : public CreatureScript
{
public:
    npc_00x09hl() : CreatureScript("npc_00x09hl") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_RESQUE_OOX_09)
        {
            creature->SetStandState(UNIT_STAND_STATE_STAND);

            if (player->GetTeam() == ALLIANCE)
                creature->setFaction(FACTION_ESCORTEE_A);
            else if (player->GetTeam() == HORDE)
                creature->setFaction(FACTION_ESCORTEE_H);

            creature->AI()->Talk(0, player->GetGUID());

            if (npc_00x09hlAI* pEscortAI = CAST_AI(npc_00x09hl::npc_00x09hlAI, creature->AI()))
                pEscortAI->Start(true, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_00x09hlAI : public npc_escortAI
    {
        npc_00x09hlAI(Creature* creature) : npc_escortAI(creature), _summons(me) { }

        void Reset()
        {
            me->setActive(true);
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 37:
                Talk(2);
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);

                if (auto* owl1 = me->SummonCreature(NPC_MARAUDING_OWL, OwlSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN))
                    if (auto* owl2 = me->SummonCreature(NPC_MARAUDING_OWL, OwlSpawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto* owl3 = me->SummonCreature(NPC_MARAUDING_OWL, OwlSpawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            _summons.Summon(owl1);
                            _summons.Summon(owl2);
                            _summons.Summon(owl3);
                        }
                break;
            case 38:
                Talk(4);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->GetMotionMaster()->MoveIdle();

                if (auto player = GetPlayerForEscort())
                    player->GroupEventHappens(QUEST_RESQUE_OOX_09, me);

                me->CastSpell(me, SPELL_OOX_LIFT_OFF);
                _events.ScheduleEvent(EVENT_OOX_1, 2100);
                break;
            }
        }

        void EnterCombat(Unit* who)
        {
            if (who->GetEntry() == NPC_MARAUDING_OWL || who->GetEntry() == NPC_VILE_AMBUSHER)
                return;

            Talk(1);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
        }

        void EscortAbandonTooFarDespawn()
        {
            _summons.DespawnAll();
        }

        void JustDied(Unit* killer)
        {
            _summons.DespawnAll();
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateAI(uiDiff);
            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_OOX_1:
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_100);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, 118.56f, -3895.203f, 161.33f, false);
                    me->DespawnOrUnsummon(10000);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        EventMap _events;
        uint64 _playerGUID = 0;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_00x09hlAI(creature);
    }
};

void AddSC_hinterlands()
{
    new npc_ongolongo();
    new npc_shadra_the_venom_queen_controller();
    new npc_trained_razorbeak();
    new npc_sharpbeak_vehicle_transport();
    new npc_00x09hl();
}