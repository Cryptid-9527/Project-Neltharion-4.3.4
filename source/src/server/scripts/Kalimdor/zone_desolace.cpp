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

 /*
 SDName: Desolace
 SD%Complete: 100 (with SAI scripts)
 SDComment: Wondi's scripts!
 SDCategory: Desolace
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
#include "Group.h"

enum DesolaceDefaultSpells
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

enum DesolaceEvents
{
    EVENT_FURIEN_1_TRIP_1 = 1,
    EVENT_VORTEX_1,
    EVENT_VORTEX_2,
    EVENT_VORTEX_3,
    EVENT_SWOOP_EGG_1,
    EVENT_SWOOP_EGG_2,
    EVENT_SWOOP_EGG_3,
    EVENT_SWOOP_EGG_4,
    EVENT_SWOOP_EGG_5,
    EVENT_SWOOP_EGG_6,
    EVENT_KORRAH_MOUNT_TRIP_1,
    EVENT_TAMED_KODO_1,
    EVENT_KOLKAR_CENTAUR_1,
    EVENT_DEFEND_SPEAR_1,
    EVENT_DEFEND_SPEAR_2,
    EVENT_DEFEND_SPEAR_3,
    EVENT_DEFEND_SPEAR_4,
    EVENT_GRIMFEATHER_1,
    EVENT_ESCORT_MILAZZA_1,
    EVENT_ESCORT_MILAZZA_2,
    EVENT_CENTAUR_COMMANDER_1,
    EVENT_CENTAUR_COMMANDER_2,
    EVENT_CENTAUR_COMMANDER_3,
    EVENT_CENTAUR_COMMANDER_4,
    EVENT_MAULER_THRASH,
    EVENT_WINDCHASER_LIGHTING_BOLT,
    EVENT_AGOGRIDON_CRIPPLE,
    EVENT_AGOGRIDON_RAIN_OF_FIRE,
    EVENT_AGOGRIDON_WAR_STOMP,
    EVENT_JANKIE_1_TRIP_1,
    EVENT_GHOST_PLASM_1,
    EVENT_LORD_BELLOW,
    EVENT_LORD_TORCH,

    EVENT_CORK_1,
    EVENT_CORK_2,
    EVENT_CORK_3,
    EVENT_RIGGER_1,
    EVENT_RIGGER_2,
    EVENT_RIGGER_3,

    ACTION_CENTAUR_ATTACK_ENEMY,
    ACTION_CORK_1,
    ACTION_CORK_2,
    ACTION_NORTH_GIZELTON_1,
    ACTION_RIGGER_1,
    ACTION_RIGGER_2,
    ACTION_SOUTH_GIZELTON_1,
};

// ############# SCRIPTS ###############

uint32 const FurienDragonhawk1PathSize = 19;
const G3D::Vector3 FurienDragonhawk1Path[FurienDragonhawk1PathSize] =
{
    {  189.54478f, 1770.6824f, 91.8135f },
    {  180.45127f, 1774.7326f, 99.7835f },
    {  145.95034f, 1758.6782f, 116.339f },
    {  87.44706f,  1696.2193f, 131.3474f },
    {  -70.37023f,  1532.3067f, 145.3780f },
    { -224.63324f, 1439.4854f, 193.370f },
    { -358.75381f, 1495.7830f, 212.108f },
    { -363.46283f, 1654.5660f, 220.821f },
    { -371.92297f, 1848.8781f, 220.669f },
    { -406.84857f, 2010.6724f, 186.556f },
    { -416.38595f, 2058.2470f, 178.222f },
    { -399.41238f, 2146.5117f, 166.086f },
    { -385.06955f, 2199.2929f, 150.567f },
    { -387.83444f, 2248.3046f, 146.737f },
    { -424.78826f, 2288.6198f, 131.521f },
    { -458.28817f, 2280.8999f, 116.605f },
    { -472.04190f, 2252.4777f, 101.428f },
    { -454.59542f, 2221.5864f, 97.7159f },
    { -436.31613f, 2223.5051f, 90.4853f }, // 18
};

uint32 const FurienDragonhawk1PathSize2 = 3;
const G3D::Vector3 FurienDragonhawk1Path2[FurienDragonhawk1PathSize2] =
{
    { -430.597f, 2221.7456f, 95.995f },
    { -412.170f, 2220.4721f, 101.60f },
    { -375.403f, 2164.4614f, 120.85f },
};

enum FurienDragonhawk1
{
    SPELL_RIDE_DRAGONHAWK = 67062,
};

class npc_furiens_dragonhawk1_vehicle : public CreatureScript
{
public:
    npc_furiens_dragonhawk1_vehicle() : CreatureScript("npc_furiens_dragonhawk1_vehicle") { }

    struct npc_furiens_dragonhawk1_vehicleAI : public VehicleAI
    {
        npc_furiens_dragonhawk1_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(me, SPELL_RIDE_DRAGONHAWK);
                _playerGUID = player->GetGUID();
                me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
            }

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_FURIEN_1_TRIP_1, 1000);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (!_noPassenger)
                    me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 18:
                if (!_noPassenger)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _noPassenger = true;
                        player->ExitVehicle();
                        me->GetMotionMaster()->MoveSmoothPath(FurienDragonhawk1Path2, FurienDragonhawk1PathSize2);
                    }
                }
                break;
            case 2:
                if (_noPassenger)
                    me->DespawnOrUnsummon(100);
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
                case EVENT_FURIEN_1_TRIP_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(FurienDragonhawk1Path, FurienDragonhawk1PathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _noPassenger = false;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_furiens_dragonhawk1_vehicleAI(creature);
    }
};

uint32 const FurienDragonhawk2PathSize = 4;
const G3D::Vector3 FurienDragonhawk2Path[FurienDragonhawk2PathSize] =
{
    { -433.652f, 2277.165f, 109.58f },
    { -440.701f, 2266.361f, 104.77f },
    { -446.483f, 2254.744f, 100.77f },
    { -442.315f, 2238.854f, 96.966f }, // 3
};

uint32 const FurienDragonhawk2PathSize2 = 13;
const G3D::Vector3 FurienDragonhawk2Path2[FurienDragonhawk2PathSize2] =
{
    { -443.657f, 2228.924f, 98.950f },
    { -443.266f, 2211.017f, 104.77f },
    { -458.732f, 2154.312f, 115.85f },
    { -500.297f, 2068.079f, 145.47f },
    { -551.997f, 1979.658f, 147.41f },
    { -627.218f, 1829.195f, 153.16f },
    { -657.031f, 1731.306f, 151.77f },
    { -656.662f, 1613.024f, 150.31f },
    { -629.993f, 1470.680f, 155.97f },
    { -600.421f, 1377.230f, 157.07f },
    { -530.985f, 1249.220f, 153.31f },
    { -424.580f, 1157.255f, 118.92f },
    { -395.575f, 1142.804f, 111.08f }, // 12
};

uint32 const FurienDragonhawk2PathSize3 = 4;
const G3D::Vector3 FurienDragonhawk2Path3[FurienDragonhawk2PathSize3] =
{
    { -373.670f, 1139.7724f, 111.285f },
    { -344.728f, 1136.9790f, 115.979f },
    { -317.207f, 1134.1375f, 121.305f },
    { -269.906f, 1125.4941f, 119.767f }, // 3
};

class npc_furiens_dragonhawk2_vehicle : public CreatureScript
{
public:
    npc_furiens_dragonhawk2_vehicle() : CreatureScript("npc_furiens_dragonhawk2_vehicle") { }

    struct npc_furiens_dragonhawk2_vehicleAI : public VehicleAI
    {
        npc_furiens_dragonhawk2_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            _noPassenger = true;
            _pathMode = 0;
            me->CastWithDelay(100, me, SPELL_FLIGHT_SPEED_280);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->GetMotionMaster()->MoveSmoothPath(FurienDragonhawk2Path, FurienDragonhawk2PathSize);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _noPassenger = false;
                _pathMode = 1;
                me->GetMotionMaster()->MoveSmoothPath(FurienDragonhawk2Path2, FurienDragonhawk2PathSize2);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (!_noPassenger)
                    me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_pathMode == 0)
            {
                if (point == 3)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                        player->CastWithDelay(100, me, SPELL_RIDE_DRAGONHAWK);
                    }
                }
            }
            else if (_pathMode == 1)
            {
                if (point == 12)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _pathMode = 2;
                        _noPassenger = true;
                        player->ExitVehicle();
                        me->GetMotionMaster()->MoveSmoothPath(FurienDragonhawk2Path3, FurienDragonhawk2PathSize3);
                        me->DespawnOrUnsummon(6000);
                    }
                }
            }
            else if (_pathMode == 2)
            {
                if (point == 3)
                    me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (me->GetDistance(player) > 60.f)
                    me->DespawnOrUnsummon();
            }
        }
    private:
        bool _noPassenger = false;
        uint8 _pathMode = 0;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_furiens_dragonhawk2_vehicleAI(creature);
    }
};

enum QuestFirestarter
{
    QUEST_FIRESTARTER = 14196,

    SPELL_GROW = 94640,
    SPELL_WISPY_VORTEX = 67409,
    SPELL_GUSTY_VORTEX = 67412,
    SPELL_STORMY_VORTEX = 67421,
    SPELL_BLUSTERING_VORTEX = 67422,
    SPELL_SURGING_VORTEX = 67423,
    SPELL_SEETHING_VORTEX = 67425,
    SPELL_QUEST_KILL_CREDIT = 67431,
    SPELL_SPIRIT_SIPHON = 67411,

};

class npc_wispy_vortex : public CreatureScript
{
public:
    npc_wispy_vortex() : CreatureScript("npc_wispy_vortex") { }

    struct npc_wispy_vortexAI : public ScriptedAI
    {
        npc_wispy_vortexAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastWithDelay(500, me, SPELL_WISPY_VORTEX);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (!_endScript)
            {
                if (spellInfo->Id == SPELL_GROW)
                {
                    switch (me->GetAuraCount(SPELL_GROW))
                    {
                    case 1:
                        me->RemoveAura(SPELL_WISPY_VORTEX);
                        me->CastSpell(me, SPELL_GUSTY_VORTEX);
                        break;
                    case 2:
                        me->RemoveAura(SPELL_GUSTY_VORTEX);
                        me->CastSpell(me, SPELL_STORMY_VORTEX);
                        break;
                    case 4:
                        me->RemoveAura(SPELL_STORMY_VORTEX);
                        me->CastSpell(me, SPELL_BLUSTERING_VORTEX);
                        break;
                    case 6:
                        me->RemoveAura(SPELL_BLUSTERING_VORTEX);
                        me->CastSpell(me, SPELL_SURGING_VORTEX);
                        break;
                    case 7:
                        me->RemoveAura(SPELL_SURGING_VORTEX);
                        me->CastSpell(me, SPELL_SEETHING_VORTEX);
                        break;
                    case 8:
                    case 9:
                    case 10:
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
                        _endScript = true;
                        _events.ScheduleEvent(EVENT_VORTEX_2, 500);
                        break;
                    default:
                        break;
                    }
                }
            }

            if (spellInfo->Id == SPELL_SPIRIT_SIPHON && !_inAction)
            {
                _inAction = true;
                me->CastSpell(me, SPELL_GROW);
                me->CastSpell(me, SPELL_ROOT);
                _events.ScheduleEvent(EVENT_VORTEX_1, 3000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (_inArea)
                {
                    if (me->GetAreaId() != 603)
                    {
                        _inArea = false;
                        me->AI()->Talk(0, _playerGUID);
                        _events.ScheduleEvent(EVENT_VORTEX_3, 10000);
                    }
                }
                else
                {
                    if (me->GetAreaId() == 603)
                    {
                        _events.CancelEvent(EVENT_VORTEX_3);
                        _inArea = true;
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        me->RemoveAllAuras();
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_FIRESTARTER) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_FIRESTARTER) == QUEST_STATUS_REWARDED)
                    {
                        me->RemoveAllAuras();
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE && !_inAction)
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.4f * M_PI);
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
                case EVENT_VORTEX_1:
                    _inAction = false;
                    me->RemoveAura(SPELL_ROOT);
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.4f * M_PI);
                    }
                    break;
                case EVENT_VORTEX_2:
                    me->CastSpell(me, SPELL_QUEST_KILL_CREDIT);
                    break;
                case EVENT_VORTEX_3:
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
        bool _inArea = true;
        bool _endScript = false;
        bool _inAction = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wispy_vortexAI(creature);
    }
};

enum RiderontheStorm
{
    QUEST_RIDER_ON_THE_STORM = 14198,

    SPELL_RAGING_VISUAL = 67505,
    SPELL_RIDE_VORTEX = 67510,
    SPELL_REGEN_VORTEX = 67707,
    SPELL_VORTEX_KNOCK = 67605,

    NPC_CREDIT_DEFENDER = 35506,
    NPC_CREDIT_MERCHANT = 35510,
};

uint32 const RagingVortexPathSize = 7;
const G3D::Vector3 RagingVortexPath[RagingVortexPathSize] =
{
    { -50.4584f, 1190.304f,  93.66f},
    { -28.1587f, 1195.987f,  100.3f},
    { -5.59784f,  1209.415f, 114.3f},
    { 14.43929f,  1217.704f, 128.3f},
    { 41.97915f,  1217.292f, 143.8f},
    { 67.44736f,  1219.598f, 156.0f},
    { 100.3854f, 1224.501f,  161.6f}, // 6
};

uint32 const RagingVortexPathSize2 = 30;
const G3D::Vector3 RagingVortexPath2[RagingVortexPathSize2] =
{
    { 130.810f, 1212.399f, 165.638f },
    { 134.246f, 1197.064f, 165.974f },
    { 138.441f, 1181.552f, 166.875f },
    { 156.305f, 1169.789f, 166.707f },
    { 170.842f, 1178.322f, 166.788f },
    { 172.401f, 1199.170f, 166.548f },
    { 168.818f, 1214.374f, 166.052f },
    { 178.136f, 1228.782f, 169.938f },
    { 179.519f, 1239.892f, 172.856f },
    { 163.474f, 1247.741f, 166.904f },
    { 142.819f, 1244.730f, 165.450f },
    { 130.247f, 1256.250f, 164.658f },
    { 141.133f, 1275.226f, 172.872f },
    { 141.708f, 1292.398f, 182.506f },
    { 142.472f, 1306.044f, 188.361f },
    { 156.892f, 1310.175f, 190.388f },
    { 195.670f, 1304.071f, 190.327f },
    { 223.348f, 1293.293f, 189.900f },
    { 235.498f, 1289.858f, 189.792f },
    { 254.249f, 1300.184f, 191.572f },
    { 252.851f, 1315.001f, 191.700f },
    { 236.925f, 1316.707f, 191.045f },
    { 209.188f, 1323.690f, 190.994f },
    { 187.608f, 1332.223f, 192.667f },
    { 168.838f, 1314.298f, 190.947f },
    { 147.670f, 1303.429f, 188.025f },
    { 140.758f, 1280.405f, 175.673f },
    { 128.190f, 1255.851f, 164.287f },
    { 101.098f, 1240.222f, 163.380f },
    { 112.805f, 1221.078f, 162.363f }, // 29
};

uint32 const RagingVortexPathSize3 = 14;
const G3D::Vector3 RagingVortexPath3[RagingVortexPathSize3] =
{
    {  195.322f, 1268.201f, 233.92f },
    {  156.441f, 1272.558f, 249.24f },
    {  118.083f, 1315.916f, 248.98f },
    {  54.8224f, 1455.912f, 212.87f },
    { -40.6446f, 1580.219f, 202.46f },
    { -134.236f, 1690.558f, 209.93f },
    { -295.936f, 1870.832f, 191.63f },
    { -380.822f, 1975.816f, 142.82f },
    { -450.397f, 2076.655f, 139.22f },
    { -471.829f, 2140.735f, 139.95f },
    { -506.180f, 2201.148f, 130.65f },
    { -493.520f, 2230.750f, 114.56f },
    { -471.254f, 2238.259f, 103.34f },
    { -444.509f, 2225.365f, 93.060f }, // 13
};

class npc_raging_vortex : public CreatureScript
{
public:
    npc_raging_vortex() : CreatureScript("npc_raging_vortex") { }

    struct npc_raging_vortexAI : public VehicleAI
    {
        npc_raging_vortexAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(me, SPELL_RIDE_VORTEX);
            }

            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->SetReactState(REACT_PASSIVE);
            me->SetPhaseMask(8, false);

            me->CastWithDelay(200, me, SPELL_RAGING_VISUAL);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _step = 1;
                me->GetMotionMaster()->MoveSmoothPath(RagingVortexPath, RagingVortexPathSize);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(500);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (who->GetEntry() == NPC_CREDIT_DEFENDER)
                        player->KilledMonsterCredit(NPC_CREDIT_DEFENDER);

                    if (who->GetEntry() == NPC_CREDIT_MERCHANT)
                        player->KilledMonsterCredit(NPC_CREDIT_MERCHANT);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_step == 1)
            {
                if (point == 6)
                {
                    me->GetMotionMaster()->MoveSmoothPath(RagingVortexPath2, RagingVortexPathSize2);
                    _step++; // 2
                }
            }

            if (_step == 2)
            {
                if (point == 29)
                {
                    me->GetMotionMaster()->MoveSmoothPath(RagingVortexPath2, RagingVortexPathSize2);
                }
            }

            if (_step == 3)
            {
                if (point == 13)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                if (me->GetHealthPct() < 95)
                    me->SetFullHealth();

                _checkTimer = 3000;
                me->CastSpell(me, SPELL_VORTEX_KNOCK);

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!_questCompleted)
                    {
                        if (player->GetQuestStatus(QUEST_RIDER_ON_THE_STORM) == QUEST_STATUS_COMPLETE)
                        {
                            _questCompleted = true;
                            me->GetMotionMaster()->Clear();
                            me->SetDisableGravity(true);
                            me->SetCanFly(true);
                            me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                            me->GetMotionMaster()->MoveSmoothPath(RagingVortexPath3, RagingVortexPathSize3);
                            _step++;
                        }
                    }

                    if (me->GetDistance(player) >= 100.f)
                    {
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        bool _questCompleted = false;
        uint16 _checkTimer = 1000;
        uint8 _step = 0;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raging_vortexAI(creature);
    }
};

enum EarlyAdoption
{
    QUEST_EARLY_ADOPTION = 14246,
    OBJECT_SWOOP_NEST = 195588,

    SPELL_SUMMON_BASILISK = 67913,
    SPELL_SUMMON_BONEPAW = 68058,

    NPC_SWOOP_CREDIT = 35751,
    NPC_SWOOP_BUNNY = 51139,
    NPC_SNAPJAW_BASILISK = 35750,
    NPC_FAMISHED_BONEPAW = 35811,
    NPC_SUMMONED_DEFIANT_SWOOP = 74925,
    NPC_WONDI_TARGET_1 = 75042,
    NPC_PLACE_SWOOP_BUNNY = 74926,
};

class npc_swoop_egg_early_adoption : public CreatureScript
{
public:
    npc_swoop_egg_early_adoption() : CreatureScript("npc_swoop_egg_early_adoption") { }

    struct npc_swoop_egg_early_adoptionAI : public ScriptedAI
    {
        npc_swoop_egg_early_adoptionAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                _events.ScheduleEvent(EVENT_SWOOP_EGG_1, 3000);
            }
            else
                me->DespawnOrUnsummon();

            if (auto nest = me->FindNearestCreature(NPC_PLACE_SWOOP_BUNNY, 10.f))
                _swoopTargetGUID = nest->GetGUID();
        }

        void EggDespawner()
        {
            _summons.DespawnAll();
            me->DespawnOrUnsummon(200);
        }

        void JustDied(Unit* killer)
        {
            EggDespawner();

            if (auto nest = Unit::GetCreature(*me, _swoopTargetGUID))
            {
                nest->setActive(true);
                nest->AI()->SetData(1, 1);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                _killCount++;
                _mobKilled = true;
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                if (who->GetEntry() == NPC_SNAPJAW_BASILISK)
                {
                    if (auto basilisk = who->ToCreature())
                    {
                        _mobKilled = false;
                        _summons.Summon(basilisk);
                        basilisk->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                        basilisk->AI()->AttackStart(me);
                    }
                }

                if (who->GetEntry() == NPC_FAMISHED_BONEPAW)
                {
                    if (auto bone = who->ToCreature())
                    {
                        _mobKilled = false;
                        _summons.Summon(bone);
                        bone->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                        bone->AI()->AttackStart(me);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;
                if (!_completed)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (!player->isAlive())
                        {
                            EggDespawner();

                            if (auto nest = Unit::GetCreature(*me, _swoopTargetGUID))
                            {
                                nest->setActive(true);
                                nest->AI()->SetData(1, 1);
                            }
                        }

                        if (player->GetQuestStatus(QUEST_EARLY_ADOPTION) == QUEST_STATUS_NONE)
                        {
                            EggDespawner();

                            if (auto nest = Unit::GetCreature(*me, _swoopTargetGUID))
                            {
                                nest->setActive(true);
                                nest->AI()->SetData(1, 1);
                            }
                        }

                        if (me->GetDistance(player) >= 50.f)
                        {
                            EggDespawner();

                            if (auto nest = Unit::GetCreature(*me, _swoopTargetGUID))
                            {
                                nest->setActive(true);
                                nest->AI()->SetData(1, 1);
                            }
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            if (_mobKilled)
            {
                if (_summonTimer <= diff)
                {
                    _summonTimer = 6000;
                    _mobKilled = false;

                    switch (_killCount)
                    {
                    case 1:
                    case 2:
                        _events.ScheduleEvent(EVENT_SWOOP_EGG_1, 1000);
                        break;
                    case 3:
                        _events.ScheduleEvent(EVENT_SWOOP_EGG_2, 1000);
                        break;
                    default:
                        break;
                    }

                }
                else _summonTimer -= diff;
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SWOOP_EGG_1:
                {
                    _random = urand(0, 1);

                    if (_random == 0)
                        me->CastSpell(me, SPELL_SUMMON_BASILISK);
                    else
                        me->CastSpell(me, SPELL_SUMMON_BONEPAW);
                }
                break;
                case EVENT_SWOOP_EGG_2:
                {
                    _completed = true;
                    me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);

                    if (auto bird = me->SummonCreature(NPC_SUMMONED_DEFIANT_SWOOP, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 1, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        _birdGUID = bird->GetGUID();
                        _summons.Summon(bird);
                        bird->SetReactState(REACT_PASSIVE);
                        me->CastWithDelay(2500, bird, SPELL_RIDE_VEHICLE);

                        _events.ScheduleEvent(EVENT_SWOOP_EGG_3, 5000);
                    }

                    std::list<Player*> character = me->GetPlayersInRange(35, true);
                    for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_EARLY_ADOPTION) == QUEST_STATUS_INCOMPLETE)
                            (*it)->KilledMonsterCredit(NPC_SWOOP_CREDIT);

                    break;
                }
                case EVENT_SWOOP_EGG_3:
                    if (auto bird = Unit::GetCreature(*me, _birdGUID))
                    {
                        _events.ScheduleEvent(EVENT_SWOOP_EGG_4, 6000);

                        if (auto target = me->FindNearestCreature(NPC_WONDI_TARGET_1, 150.f))
                            bird->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

                        if (auto nest = Unit::GetCreature(*me, _swoopTargetGUID))
                        {
                            nest->setActive(true);
                            nest->AI()->SetData(1, 1);
                        }
                    }
                    break;
                case EVENT_SWOOP_EGG_4:
                    EggDespawner();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _mobKilled = false;
        bool _completed = false;
        uint64 _playerGUID = 0;
        uint64 _birdGUID = 0;
        uint64 _swoopTargetGUID = 0;
        uint16 _checkTimer = 5000;
        uint16 _summonTimer = 5000;
        uint8 _killCount = 0;
        uint8 _random = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swoop_egg_early_adoptionAI(creature);
    }
};

uint32 const KorrahMountPathSize = 42;
const G3D::Vector3 KorrahMountPath[KorrahMountPathSize] =
{
    { -370.24029f, 2477.579f, 80.209f },
    { -376.90493f, 2472.198f, 81.082f },
    { -409.48123f, 2449.598f, 98.512f },
    { -399.99780f, 2409.487f, 99.638f },
    { -372.13464f, 2380.301f, 92.125f },
    { -331.96896f, 2413.492f, 69.996f },
    { -290.17837f, 2478.473f, 56.505f },
    { -286.95330f, 2528.732f, 49.538f },
    { -316.28176f, 2606.651f, 44.810f },
    { -462.14660f, 2627.378f, 57.046f },
    { -619.86383f, 2597.076f, 90.720f },
    { -804.93273f, 2550.137f, 120.38f },
    { -872.18072f, 2570.880f, 113.29f },
    { -909.66186f, 2652.369f, 100.02f },
    { -884.61114f, 2717.906f, 80.174f },
    { -797.44171f, 2738.630f, 62.466f },
    { -723.78393f, 2746.646f, 75.613f },
    { -576.45929f, 2743.627f, 72.276f },
    { -530.66754f, 2775.914f, 95.728f },
    { -474.99652f, 2871.047f, 79.596f },
    { -464.47195f, 2920.295f, 66.024f },
    { -460.61358f, 2979.830f, 56.143f },
    { -442.35427f, 3023.833f, 51.640f },
    { -408.20153f, 3029.605f, 49.662f },
    { -369.92126f, 2993.807f, 53.596f },
    { -327.71133f, 2913.775f, 55.650f },
    { -274.56967f, 2802.495f, 66.368f },
    { -172.40809f, 2743.242f, 60.889f },
    { -37.048424f, 2787.308f, 53.796f },
    {  58.245975f, 2872.004f, 62.332f },
    {  116.36671f, 2942.554f, 63.614f },
    {  213.47683f, 2991.035f, 57.938f },
    {  280.00134f, 2991.489f, 49.837f },
    {  313.89428f, 2900.658f, 46.192f },
    {  255.45864f, 2841.172f, 42.379f },
    {  184.31662f, 2786.819f, 47.051f },
    {   -7.52555f, 2618.498f, 75.136f },
    { -123.11344f, 2520.063f, 88.554f },
    { -266.07891f, 2430.364f, 90.668f },
    { -377.00158f, 2431.889f, 88.579f },
    { -383.70642f, 2462.756f, 85.470f },
    { -364.11938f, 2482.552f, 77.959f }, // 41
};

enum KorrahMountTrip
{
    SPELL_RIDE_KORRAH_MOUNT = 68050,    
    NPC_KORRAH_VOICE        = 35815,
};

class npc_korrahs_mount_trip : public CreatureScript
{
public:
    npc_korrahs_mount_trip() : CreatureScript("npc_korrahs_mount_trip") { }

    struct npc_korrahs_mount_tripAI : public VehicleAI
    {
        npc_korrahs_mount_tripAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetRooted(true);
            summoner->CastSpell(summoner, SPELL_SANCTUARY_NO_COMBAT);
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);

            if (auto* player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(me, SPELL_RIDE_KORRAH_MOUNT);

                if (auto* voice = me->SummonCreature(NPC_KORRAH_VOICE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000))
                {
                    _voiceGUID = voice->GetGUID();
                    voice->EnterVehicle(me, 1);
                }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_KORRAH_MOUNT_TRIP_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1000);

                if (auto* voice = Unit::GetCreature(*me, _voiceGUID))
                    voice->DespawnOrUnsummon();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 9:
                if (auto voice = Unit::GetCreature(*me, _voiceGUID))
                    if (auto ai = voice->AI())
                        ai->Talk(0, _playerGUID);
                break;
            case 11:
                if (auto voice = Unit::GetCreature(*me, _voiceGUID))
                    if (auto ai = voice->AI())
                        ai->Talk(1, _playerGUID);
                break;
            case 19:
                if (auto voice = Unit::GetCreature(*me, _voiceGUID))
                    if (auto ai = voice->AI())
                        ai->Talk(2, _playerGUID);
                break;
            case 29:
                if (auto voice = Unit::GetCreature(*me, _voiceGUID))
                    if (auto ai = voice->AI())
                        ai->Talk(3, _playerGUID);
                break;
            case 41:
                me->GetMotionMaster()->Clear();
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1000);

                if (auto* player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->KilledMonsterCredit(me->GetEntry());
                    player->ExitVehicle();
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
                case EVENT_KORRAH_MOUNT_TRIP_1:
                    me->SetRooted(false);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->GetMotionMaster()->MoveSmoothPath(KorrahMountPath, KorrahMountPathSize);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        uint64 _voiceGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korrahs_mount_tripAI(creature);
    }
};

enum SlitherbladeCharm
{
    SPELL_NAGA_DISGUISE_MALE = 68086,
    SPELL_NAGA_DISGUISE_FEMALE = 68165,
    SPELL_NAGA_DISGUISE_OVERRIDE_SPELLS = 68121,

    NPC_SLITHERBLADE_CHARM_CREDIT = 51145,
};

// item=49064/slitherblade-charm
// ID - 68167 Naga Disguise
class item_slitherblade_charm : public ItemScript
{
public:
    item_slitherblade_charm() : ItemScript("item_slitherblade_charm") { }

    bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
    {
        player->KilledMonsterCredit(NPC_SLITHERBLADE_CHARM_CREDIT);

        if (player->getGender() == GENDER_MALE)
        {
            if (!player->HasAura(SPELL_NAGA_DISGUISE_MALE))
                player->CastSpell(player, SPELL_NAGA_DISGUISE_MALE);
            else
                player->RemoveAura(SPELL_NAGA_DISGUISE_MALE);
        }
        else
        {
            if (!player->HasAura(SPELL_NAGA_DISGUISE_FEMALE))
                player->CastSpell(player, SPELL_NAGA_DISGUISE_FEMALE);
            else
                player->RemoveAura(SPELL_NAGA_DISGUISE_FEMALE);
        }

        return true;
    }
};

enum FightWithValishj
{
    QUEST_A_REVENANTS_VENGEANCE = 14284,

    NPC_LADY_VALISHJ = 35898,

    SPELL_INVIS_1_RANAZJAR = 68229,
};

class npc_lord_hydronis_vs_vashjir_vehicle : public CreatureScript
{
public:
    npc_lord_hydronis_vs_vashjir_vehicle() : CreatureScript("npc_lord_hydronis_vs_vashjir_vehicle") { }

    struct npc_lord_hydronis_vs_vashjir_vehicleAI : public VehicleAI
    {
        npc_lord_hydronis_vs_vashjir_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->InitCharmInfo();
            me->SetReactState(REACT_AGGRESSIVE);
            summoner->CastSpell(summoner, SPELL_SANCTUARY_NO_COMBAT);
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();

                if (!me->FindNearestCreature(NPC_LADY_VALISHJ, 150.f))
                {
                    if (auto valishj = me->SummonCreature(NPC_LADY_VALISHJ, 298.852f, 2958.92f, -1.6418f, 2.7137f, TEMPSUMMON_MANUAL_DESPAWN))
                        _valishjGUID = valishj->GetGUID();
                }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                    player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(100);

                    if (auto valishj = Unit::GetCreature(*me, _valishjGUID))
                        valishj->DespawnOrUnsummon();
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                player->ExitVehicle();
                player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(100);

                if (auto valishj = Unit::GetCreature(*me, _valishjGUID))
                    valishj->DespawnOrUnsummon();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 500;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_A_REVENANTS_VENGEANCE) == QUEST_STATUS_NONE)
                    {
                        player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                        player->ExitVehicle();
                        player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);

                        if (auto valishj = Unit::GetCreature(*me, _valishjGUID))
                            valishj->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_A_REVENANTS_VENGEANCE) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (player->HasAura(SPELL_INVIS_1_RANAZJAR))
                            player->RemoveAura(SPELL_INVIS_1_RANAZJAR);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_KORRAH_MOUNT_TRIP_1:
                    me->SetRooted(false);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->GetMotionMaster()->MoveSmoothPath(KorrahMountPath, KorrahMountPathSize);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _valishjGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_hydronis_vs_vashjir_vehicleAI(creature);
    }
};

enum KodoRoundup
{
    QUEST_KODO_ROUNDUP = 5561,
    SPELL_KODO_KOMBOBULATOR = 18153,
    SPELL_KODO_FOLLOW_AURA = 18172,
    SPELL_KODO_KOMBOBULATOR_AURA = 18377,

    NPC_TAMED_KODO = 11627,
    NPC_SMEED = 11596,
};

class npc_aged_dying_ancient_kodo : public CreatureScript
{
public:
    npc_aged_dying_ancient_kodo() : CreatureScript("npc_aged_dying_ancient_kodo") { }

    struct npc_aged_dying_ancient_kodoAI : public ScriptedAI
    {
        npc_aged_dying_ancient_kodoAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_KODO_KOMBOBULATOR)
            {
                if (auto player = who->ToPlayer())
                {
                    if (auto tamed = player->SummonCreature(NPC_TAMED_KODO, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        tamed->SetReactState(REACT_PASSIVE);
                        tamed->SetOwnerGUID(player->GetGUID());
                        me->DespawnOrUnsummon();
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aged_dying_ancient_kodoAI(creature);
    }
};

class npc_tamed_kodo_roundup : public CreatureScript
{
public:
    npc_tamed_kodo_roundup() : CreatureScript("npc_tamed_kodo_roundup") { }

    struct npc_tamed_kodo_roundupAI : public ScriptedAI
    {
        npc_tamed_kodo_roundupAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _inAction = false;
                me->CastSpell(me, SPELL_KODO_KOMBOBULATOR_AURA);
                player->CastSpell(player, SPELL_KODO_FOLLOW_AURA);
                _playerGUID = player->GetGUID();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (!_inAction)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (!player->isAlive())
                        {
                            me->DespawnOrUnsummon();
                            me->RemoveAura(SPELL_KODO_KOMBOBULATOR_AURA);
                            player->RemoveAura(SPELL_KODO_FOLLOW_AURA);
                        }

                        if (player->GetQuestStatus(QUEST_KODO_ROUNDUP) == QUEST_STATUS_NONE)
                        {
                            me->DespawnOrUnsummon();
                            me->RemoveAura(SPELL_KODO_KOMBOBULATOR_AURA);
                            player->RemoveAura(SPELL_KODO_FOLLOW_AURA);
                        }

                        if (me->GetDistance(player) >= 100.f)
                        {
                            me->DespawnOrUnsummon();
                            me->RemoveAura(SPELL_KODO_KOMBOBULATOR_AURA);
                            player->RemoveAura(SPELL_KODO_FOLLOW_AURA);
                        }

                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE && !_inAction)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->GetMotionMaster()->MoveFollow(player, 1.0f, 0.5f * M_PI);
                        }

                        if (auto smeed = me->FindNearestCreature(NPC_SMEED, 16.f))
                        {
                            me->SetRooted(true);
                            _inAction = true;
                            smeed->AI()->TalkWithDelay(1000, 0);
                            me->GetMotionMaster()->Clear();
                            me->SetFacingToObject(smeed);
                            player->KilledMonsterCredit(NPC_TAMED_KODO);
                            player->RemoveAura(SPELL_KODO_FOLLOW_AURA);
                            me->DespawnOrUnsummon(8000);
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            if (!_inAction)
            {
                if (_checkTimeExpire <= diff)
                {
                    _checkTimeExpire = 600000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->DespawnOrUnsummon();
                        me->RemoveAura(SPELL_KODO_KOMBOBULATOR_AURA);
                        player->RemoveAura(SPELL_KODO_FOLLOW_AURA);
                    }
                }
                else _checkTimeExpire -= diff;
            }
        }

    private:
        bool _inAction = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        uint64 _checkTimeExpire = 600000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tamed_kodo_roundupAI(creature);
    }
};

uint32 const KolkarCentaurPathSize = 37;
const G3D::Vector3 KolkarCentaurPath[KolkarCentaurPathSize] =
{
    { -1023.84f, 1617.6987f, 59.946f },
    { -1013.59f, 1613.8128f, 59.859f },
    { -986.0253f, 1598.4188f, 59.865f },
    { -972.3847f, 1590.9101f, 59.905f },
    { -969.4982f, 1572.6729f, 59.893f },
    { -982.5756f, 1550.8031f, 60.510f },
    { -999.4603f, 1534.6722f, 61.254f },
    { -1036.959f, 1518.8234f, 61.904f },
    { -1063.177f, 1514.2203f, 61.991f },
    { -1087.931f, 1518.5961f, 60.957f },
    { -1101.026f, 1501.7114f, 60.372f },
    { -1097.700f, 1475.7010f, 60.951f },
    { -1087.796f, 1434.3657f, 60.492f },
    { -1116.231f, 1392.8315f, 61.188f },
    { -1134.611f, 1363.5212f, 66.975f },
    { -1146.451f, 1347.8260f, 73.771f },
    { -1153.053f, 1342.1407f, 76.224f },
    { -1153.474f, 1335.5660f, 77.342f },
    { -1138.594f, 1320.9178f, 81.610f },
    { -1128.885f, 1308.0842f, 85.534f },
    { -1118.358f, 1284.8085f, 88.148f },
    { -1124.203f, 1267.5887f, 88.416f },
    { -1132.294f, 1257.2712f, 88.434f },
    { -1131.747f, 1232.8867f, 89.477f },
    { -1128.052f, 1205.5478f, 89.704f },
    { -1107.253f, 1189.5843f, 89.739f },
    { -1069.925f, 1181.1441f, 89.739f },
    { -1023.652f, 1177.5064f, 89.755f },
    { -982.7374f, 1181.2740f, 89.969f },
    { -942.2819f, 1180.3692f, 90.795f },
    { -919.2614f, 1165.3123f, 93.309f },
    { -918.2727f, 1141.2469f, 92.529f },
    { -912.6387f, 1115.6679f, 91.423f },
    { -904.8049f, 1093.7053f, 88.221f },
    { -891.5697f, 1069.4613f, 91.379f },
    { -890.7672f, 1043.6712f, 92.590f },
    { -905.5683f, 1035.4495f, 96.172f }, // 36
};

uint32 const KolkarCentaurPathSize2 = 11;
const G3D::Vector3 KolkarCentaurPath2[KolkarCentaurPathSize2] =
{
    { -905.568f, 1035.4494f, 96.171f },
    { -907.133f, 1033.6938f, 96.123f },
    { -910.371f, 1032.6053f, 96.086f },
    { -914.356f, 1033.2337f, 95.951f },
    { -916.916f, 1037.0460f, 95.986f },
    { -916.394f, 1040.4786f, 96.229f },
    { -914.464f, 1042.9509f, 96.673f },
    { -911.110f, 1043.8485f, 96.936f },
    { -907.741f, 1042.7310f, 96.846f },
    { -905.712f, 1039.0914f, 96.454f },
    { -905.674f, 1037.5021f, 96.366f }, // 10
};

uint32 const KolkarCentaurPathSize3 = 7;
const G3D::Vector3 KolkarCentaurPath3[KolkarCentaurPathSize3] =
{
    { -913.3723f, 1034.471f, 96.3792f },
    { -909.5444f, 1034.404f, 96.5497f },
    { -901.230f, 1035.089f, 94.823f },
    { -896.526f, 1038.326f, 93.005f },
    { -887.681f, 1046.979f, 92.327f },
    { -888.486f, 1059.674f, 92.660f },
    { -891.123f, 1078.981f, 90.734f }, // 6
};

enum KolkarCentaur
{
    QUEST_DELICATE_NOTIFY = 14318,

    SPELL_RIDE_CENTAUR = 68384,
    SPELL_PLACE_SPEAR = 68385,
    SPELL_NOTIFICATION_CREDIT = 68414,

    NPC_SPEAR_BUNNY = 36141,
    NPC_SUMMONED_SPEAR = 36142,
    NPC_KOLKAR_WARMMONGER = 36150,
    NPC_MAGRAM_DEFENDER = 36159,
    NPC_KAMMAH = 36163,
};

class npc_kolkar_centaur_vehicle : public CreatureScript
{
public:
    npc_kolkar_centaur_vehicle() : CreatureScript("npc_kolkar_centaur_vehicle") { }

    struct npc_kolkar_centaur_vehicleAI : public VehicleAI
    {
        npc_kolkar_centaur_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetRooted(true);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(me, SPELL_RIDE_CENTAUR);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_KOLKAR_CENTAUR_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (!_dismounted)
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(500);
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    _dismounted = true;
                    _step = 3;
                    me->GetMotionMaster()->Clear();
                    player->ExitVehicle();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->AI()->Talk(2, _playerGUID);
                    me->GetMotionMaster()->MoveSmoothPath(KolkarCentaurPath3, KolkarCentaurPathSize3);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_step == 1)
            {
                if (point == 36)
                {
                    if (me->FindNearestCreature(NPC_SUMMONED_SPEAR, 40.0f))
                    {
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            player->ExitVehicle();
                            me->AI()->Talk(4, _playerGUID);
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->DespawnOrUnsummon(500);
                        }
                    }
                    else
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MoveSmoothPath(KolkarCentaurPath2, KolkarCentaurPathSize2);
                        me->AI()->Talk(0, _playerGUID);
                        me->TalkWithDelay(15000, 1, _playerGUID);
                        _step = 2;
                    }
                }
            }

            if (_step == 2)
            {
                if (_failCounter < 3)
                {
                    if (point == 10)
                    {
                        ++_failCounter;
                        me->GetMotionMaster()->MoveSmoothPath(KolkarCentaurPath2, KolkarCentaurPathSize2);
                    }
                }
                else
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->GetMotionMaster()->Clear();
                        player->ExitVehicle();
                        me->AI()->Talk(3, _playerGUID);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }
                }
            }

            if (_step == 3)
            {
                if (point == 6)
                {
                    me->DespawnOrUnsummon();
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!_dismounted)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetQuestStatus(QUEST_DELICATE_NOTIFY) == QUEST_STATUS_NONE ||
                            player->GetQuestStatus(QUEST_DELICATE_NOTIFY) == QUEST_STATUS_COMPLETE)
                        {
                            player->ExitVehicle();
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->DespawnOrUnsummon(1000);
                        }

                        if (me->GetDistance(player) >= 200.f)
                        {
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->DespawnOrUnsummon(1000);
                        }
                    }
                }
                else _checkTimer -= diff;
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_KOLKAR_CENTAUR_1:
                    me->SetRooted(false);
                    _step = 1;
                    me->GetMotionMaster()->MoveSmoothPath(KolkarCentaurPath, KolkarCentaurPathSize);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _dismounted = false;
        uint8 _step = 0;
        uint8 _failCounter = 0;
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kolkar_centaur_vehicleAI(creature);
    }
};

const Position KolkarDefendersSpawnPosition[] =
{
    { -887.708f, 1080.444f, 89.905f},
    { -864.556f, 1049.039f, 91.501f},
    { -859.377f, 1009.417f, 89.354f},
    { -924.886f, 990.935f, 89.353f},
    { -943.508f, 1021.72f, 89.365f},
    { -948.316f, 1053.474f, 89.449f},
    { -945.753f, 1075.882f, 88.217f},
};

class npc_spear_of_the_kolkar_khan : public CreatureScript
{
public:
    npc_spear_of_the_kolkar_khan() : CreatureScript("npc_spear_of_the_kolkar_khan") { }

    struct npc_spear_of_the_kolkar_khanAI : public ScriptedAI
    {
        npc_spear_of_the_kolkar_khanAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            _active = true;
            me->setActive(true);
            me->SetPhaseMask(48, true);
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(500, me, SPELL_ROOT);

            if (auto defender1 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                if (auto defender2 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                    if (auto defender3 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        if (auto defender4 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            if (auto defender5 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[4], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                if (auto defender6 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[5], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                    if (auto defender7 = me->SummonCreature(NPC_KOLKAR_WARMMONGER, KolkarDefendersSpawnPosition[6], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                    {
                                        _summons.Summon(defender1);
                                        _summons.Summon(defender2);
                                        _summons.Summon(defender3);
                                        _summons.Summon(defender4);
                                        _summons.Summon(defender5);
                                        _summons.Summon(defender6);
                                        _summons.Summon(defender7);

                                        defender1->AI()->SetData(1, 1);
                                        defender2->AI()->SetData(1, 2);
                                        defender3->AI()->SetData(1, 3);
                                        defender4->AI()->SetData(1, 4);
                                        defender5->AI()->SetData(1, 5);
                                        defender6->AI()->SetData(1, 6);
                                        defender7->AI()->SetData(1, 7);

                                        _events.ScheduleEvent(EVENT_DEFEND_SPEAR_1, 8000);
                                    }
        }

        void JustDied(Unit* killer)
        {
            _active = false;
            _summons.DespawnAll();
            me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff)
        {
            if (_active)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 2000;

                    std::list<Player*> character = me->GetPlayersInRange(80, true);
                    if (character.empty())
                    {
                        _active = false;
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }
                }
                else _checkTimer -= diff;

                if (_wave == 1)
                {
                    if (_waveTimer <= diff)
                    {
                        _waveTimer = 5000;

                        if (!me->FindNearestCreature(NPC_MAGRAM_DEFENDER, 100.f))
                        {
                            ++_wave;
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_2, 3000);
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_3, 6000);
                        }
                    }
                    else _waveTimer -= diff;
                }

                if (_wave == 2)
                {
                    if (_waveTimer <= diff)
                    {
                        _waveTimer = 5000;

                        if (!me->FindNearestCreature(NPC_MAGRAM_DEFENDER, 100.f))
                        {
                            ++_wave;
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_2, 5000);
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_3, 5000);
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_2, 9000);
                        }
                    }
                    else _waveTimer -= diff;
                }

                if (_wave == 3)
                {
                    if (_waveTimer <= diff)
                    {
                        _waveTimer = 5000;

                        if (!me->FindNearestCreature(NPC_MAGRAM_DEFENDER, 100.f))
                        {
                            _wave = 4;
                            _events.CancelEvent(EVENT_DEFEND_SPEAR_1);
                            _events.CancelEvent(EVENT_DEFEND_SPEAR_2);
                            _events.CancelEvent(EVENT_DEFEND_SPEAR_3);
                            me->CastSpell(me, SPELL_NOTIFICATION_CREDIT);
                            _events.ScheduleEvent(EVENT_DEFEND_SPEAR_4, 3000);

                            if (auto khan = me->FindNearestCreature(NPC_KAMMAH, 100.0f))
                                khan->AI()->TalkWithDelay(2000, 0);
                        }
                    }
                    else _waveTimer -= diff;
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DEFEND_SPEAR_1:
                {
                    _wave = 1;

                    if (auto enemy1 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        if (auto enemy2 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            if (auto enemy3 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                if (auto enemy4 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                    if (auto enemy5 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[4], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                        if (auto enemy6 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[5], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                            if (auto enemy7 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[6], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                            {
                                                _summons.Summon(enemy1);
                                                _summons.Summon(enemy2);
                                                _summons.Summon(enemy3);
                                                _summons.Summon(enemy4);
                                                _summons.Summon(enemy5);
                                                _summons.Summon(enemy6);
                                                _summons.Summon(enemy7);

                                                enemy1->AI()->SetData(1, 1);
                                                enemy2->AI()->SetData(1, 2);
                                                enemy3->AI()->SetData(1, 3);
                                                enemy4->AI()->SetData(1, 4);
                                                enemy5->AI()->SetData(1, 5);
                                                enemy6->AI()->SetData(1, 6);
                                                enemy7->AI()->SetData(1, 7);
                                            }
                }
                break;
                case EVENT_DEFEND_SPEAR_2:
                    if (auto enemy2 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[1], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        if (auto enemy4 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            if (auto enemy6 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[5], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            {
                                _summons.Summon(enemy2);
                                _summons.Summon(enemy4);
                                _summons.Summon(enemy6);

                                enemy2->AI()->SetData(1, 2);
                                enemy4->AI()->SetData(1, 4);
                                enemy6->AI()->SetData(1, 6);
                            }
                    break;
                case EVENT_DEFEND_SPEAR_3:
                    if (auto enemy1 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[0], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        if (auto enemy3 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            if (auto enemy5 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[4], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                if (auto enemy7 = me->SummonCreature(NPC_MAGRAM_DEFENDER, KolkarDefendersSpawnPosition[6], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                {
                                    _summons.Summon(enemy1);
                                    _summons.Summon(enemy3);
                                    _summons.Summon(enemy5);
                                    _summons.Summon(enemy7);

                                    enemy1->AI()->SetData(1, 1);
                                    enemy3->AI()->SetData(1, 3);
                                    enemy5->AI()->SetData(1, 5);
                                    enemy7->AI()->SetData(1, 7);
                                }
                    break;
                case EVENT_DEFEND_SPEAR_4:
                    me->CastSpell(me, SPELL_NOTIFICATION_CREDIT);
                    me->DespawnOrUnsummon();
                    _summons.DespawnAll();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _wave = 0;
        bool _active = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 3000;
        uint16 _waveTimer = 2000;
        uint8 _random = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spear_of_the_kolkar_khanAI(creature);
    }
};

uint32 const GrimfeatherPathSize1 = 10;
const G3D::Vector3 GrimfeatherPath1[GrimfeatherPathSize1] =
{
    { -978.1230f, 1648.570f, 75.299f },
    { -979.7001f, 1665.318f, 72.777f },
    { -983.1795f, 1699.635f, 66.073f },
    { -992.0355f, 1745.915f, 66.772f },
    { -1020.736f, 1783.261f, 75.255f },
    { -1115.896f, 1812.911f, 85.134f },
    { -1185.453f, 1818.974f, 82.896f },
    { -1264.873f, 1862.843f, 95.955f },
    { -1309.349f, 1954.603f, 88.398f },
    { -1299.343f, 2082.491f, 76.706f }, // 9
};

uint32 const GrimfeatherPathSize2 = 34;
const G3D::Vector3 GrimfeatherPath2[GrimfeatherPathSize2] =
{
    { -1269.797f, 2126.144f, 76.736f },
    { -1235.327f, 2177.053f, 71.728f },
    { -1171.230f, 2248.760f, 57.966f },
    { -1146.507f, 2311.015f, 60.139f },
    { -1094.127f, 2351.334f, 63.777f },
    { -998.4194f, 2406.554f, 64.245f },
    { -970.2021f, 2417.570f, 67.128f },
    { -909.1663f, 2429.309f, 73.703f },
    { -826.4439f, 2437.467f, 81.327f },
    { -782.3252f, 2466.712f, 75.278f },
    { -723.6654f, 2557.489f, 59.089f },
    { -701.5316f, 2626.066f, 66.251f },
    { -733.8027f, 2685.256f, 61.860f },
    { -771.8588f, 2702.019f, 69.895f },
    { -871.4902f, 2718.827f, 97.881f },
    { -936.0208f, 2671.051f, 100.77f },
    { -935.9998f, 2607.727f, 95.514f },
    { -890.2870f, 2572.925f, 109.25f },
    { -764.8320f, 2533.725f, 73.727f },
    { -691.5296f, 2567.691f, 68.567f },
    { -767.8505f, 2693.386f, 80.134f },
    { -895.0585f, 2747.916f, 79.675f },
    { -945.1499f, 2743.275f, 79.530f },
    { -949.6843f, 2613.425f, 55.040f },
    { -936.2460f, 2515.080f, 54.854f },
    { -945.5621f, 2446.602f, 65.810f },
    { -991.3972f, 2412.791f, 70.189f },
    { -1091.349f, 2349.526f, 69.687f },
    { -1138.255f, 2287.244f, 61.740f },
    { -1202.265f, 2210.449f, 56.583f },
    { -1286.029f, 2150.461f, 71.996f },
    { -1341.254f, 2112.680f, 76.911f },
    { -1330.822f, 2062.434f, 92.907f },
    { -1298.469f, 2081.918f, 77.926f }, // 33
};

// close end
uint32 const GrimfeatherPathSize3 = 8;
const G3D::Vector3 GrimfeatherPath3[GrimfeatherPathSize3] =
{
    { -1145.008f, 2239.969f, 156.95f },
    { -1142.796f, 2191.504f, 155.20f },
    { -1045.333f, 2034.784f, 131.31f },
    { -987.8483f, 1923.099f, 112.21f },
    { -972.1526f, 1815.104f, 90.324f },
    { -988.4124f, 1769.685f, 71.337f },
    { -983.9562f, 1718.032f, 76.540f },
    { -978.9747f, 1654.955f, 68.740f }, // 7
};

// far end
uint32 const GrimfeatherPathSize4 = 8;
const G3D::Vector3 GrimfeatherPath4[GrimfeatherPathSize4] =
{
    { -872.5846f, 2536.672f, 179.70f },
    { -895.5260f, 2439.710f, 185.97f },
    { -914.9714f, 2198.830f, 142.08f },
    { -916.3588f, 1931.692f, 111.86f },
    { -972.6976f, 1801.156f, 81.101f },
    { -988.3582f, 1748.452f, 72.130f },
    { -980.7103f, 1702.117f, 70.221f },
    { -978.8278f, 1654.523f, 68.559f }, // 7
};

enum Grimfeather
{
    QUEST_CLEANSING_OUR_CREVASSE_A = 14381,
    QUEST_CLEANSING_OUR_CREVASSE_H = 14346,

    NPC_END_TRIP_CONTROLLER = 74924,

    NPC_CREDIT_SIREN = 36228,
    NPC_CREDIT_INVADER = 36229,
    NPC_CREDIT_GIANT = 36230,

    SPELL_GRIMFEATHER_RETURN = 68577,
    SPELL_RIDE_GRIMFEATHER = 68557,
    SPELL_UNLOCK_GRIMFEATHER_SPELLS = 68579,
};

class npc_grimfeather_vehicle : public CreatureScript
{
public:
    npc_grimfeather_vehicle() : CreatureScript("npc_grimfeather_vehicle") { }

    struct npc_grimfeather_vehicleAI : public VehicleAI
    {
        npc_grimfeather_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetRooted(true);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(me, SPELL_RIDE_GRIMFEATHER);
                player->KilledMonsterCredit(me->GetEntry());

                if (player->GetTeam() == HORDE)
                    _playerIsHorde = true;
                else
                    _playerIsHorde = false;
            }
            else
                me->DespawnOrUnsummon();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_GRIMFEATHER_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(500);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (_step)
            {
            case 0:
                if (point == 9)
                {
                    me->RemoveAura(SPELL_FLIGHT_SPEED_200);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_100);
                    _step = 1;
                    me->CastSpell(me, SPELL_UNLOCK_GRIMFEATHER_SPELLS);
                    me->SetSpeed(MOVE_RUN, 2.0f);
                    me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath2, GrimfeatherPathSize2);
                }
                break;
            case 1:
                if (point == 33)
                {
                    me->SetSpeed(MOVE_RUN, 2.0f);
                    me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath2, GrimfeatherPathSize2);
                }
                break;
            case 2:
                if (point == 7)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }
                }
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_GRIMFEATHER_RETURN)
            {
                if (auto end = me->FindNearestCreature(NPC_END_TRIP_CONTROLLER, 200.0f))
                {
                    _complete = true;
                    _step = 2;
                    me->RemoveAura(SPELL_UNLOCK_GRIMFEATHER_SPELLS);
                    me->RemoveAura(SPELL_FLIGHT_SPEED_100);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->GetMotionMaster()->Clear();

                    if (end->GetGUIDLow() == 44709) // far
                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath4, GrimfeatherPathSize4);
                    else // close
                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath3, GrimfeatherPathSize3);
                }
            }

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (spellInfo->Id == SPELL_DUMMY_PING)
                    player->KilledMonsterCredit(NPC_CREDIT_SIREN);

                if (spellInfo->Id == SPELL_DUMMY_PING_2)
                    player->KilledMonsterCredit(NPC_CREDIT_INVADER);

                if (spellInfo->Id == SPELL_DUMMY_PING_3)
                    player->KilledMonsterCredit(NPC_CREDIT_GIANT);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        player->ExitVehicle();
                        me->DespawnOrUnsummon();
                    }

                    if (_playerIsHorde)
                    {
                        if (player->GetQuestStatus(QUEST_CLEANSING_OUR_CREVASSE_H) == QUEST_STATUS_NONE)
                        {
                            player->ExitVehicle();
                            me->DespawnOrUnsummon();
                        }
                    }
                    else
                    {
                        if (player->GetQuestStatus(QUEST_CLEANSING_OUR_CREVASSE_A) == QUEST_STATUS_NONE)
                        {
                            player->ExitVehicle();
                            me->DespawnOrUnsummon();
                        }
                    }

                    if (!_complete)
                    {
                        if (_playerIsHorde)
                        {
                            if (player->GetQuestStatus(QUEST_CLEANSING_OUR_CREVASSE_H) == QUEST_STATUS_COMPLETE)
                            {
                                _complete = true;
                                me->GetMotionMaster()->Clear();
                                me->RemoveAura(SPELL_UNLOCK_GRIMFEATHER_SPELLS);
                                me->RemoveAura(SPELL_FLIGHT_SPEED_100);
                                me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                                _step = 2;

                                if (auto end = me->FindNearestCreature(NPC_END_TRIP_CONTROLLER, 200.0f))
                                {
                                    if (end->GetGUIDLow() == 44709) // far
                                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath4, GrimfeatherPathSize4);
                                    else // close
                                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath3, GrimfeatherPathSize3);
                                }
                            }
                        }
                        else
                        {
                            if (player->GetQuestStatus(QUEST_CLEANSING_OUR_CREVASSE_A) == QUEST_STATUS_COMPLETE)
                            {
                                _complete = true;
                                me->GetMotionMaster()->Clear();
                                me->RemoveAura(SPELL_UNLOCK_GRIMFEATHER_SPELLS);
                                me->RemoveAura(SPELL_FLIGHT_SPEED_100);
                                me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                                _step = 2;

                                if (auto end = me->FindNearestCreature(NPC_END_TRIP_CONTROLLER, 200.0f))
                                {
                                    if (end->GetGUIDLow() == 44709) // far
                                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath4, GrimfeatherPathSize4);
                                    else // close
                                        me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath3, GrimfeatherPathSize3);
                                }
                            }
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GRIMFEATHER_1:
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_200);
                    _step = 0;
                    me->SetRooted(false);
                    me->SetSpeed(MOVE_RUN, 3.2f);
                    me->GetMotionMaster()->MoveSmoothPath(GrimfeatherPath1, GrimfeatherPathSize1);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _step = 0;
        bool _playerIsHorde = false;
        bool _complete = false;
        uint16 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grimfeather_vehicleAI(creature);
    }
};

enum QuestMelizza
{
    QUEST_GET_ME_OUT_OF_HERE = 6132,

    NPC_HORNIZZ = 6019,
};

// quest = 13605 / the - last - refugee
class npc_melizza_brimbuzzle_escort : public CreatureScript
{
public:
    npc_melizza_brimbuzzle_escort() : CreatureScript("npc_melizza_brimbuzzle_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_GET_ME_OUT_OF_HERE)
        {
            player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
            player->CastSpell(creature, SPELL_DUMMY_PING);
        }

        return true;
    }

    struct npc_melizza_brimbuzzle_escortAI : public npc_escortAI
    {
        npc_melizza_brimbuzzle_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->setActive(true);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_ESCORT_MILAZZA_1, 2000);
                }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 24:
            {
                _started = false;
                me->AI()->Talk(1, _playerGUID);
                SetRun(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CombatStop();
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);

                std::list<Player*> player = me->GetPlayersInRange(70, true);
                for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_GET_ME_OUT_OF_HERE) == QUEST_STATUS_INCOMPLETE)
                        (*it)->AreaExploredOrEventHappens(QUEST_GET_ME_OUT_OF_HERE);
            }
            break;
            case 40:
                me->AI()->Talk(2);
                me->AI()->TalkWithDelay(4500, 3);
                me->AI()->TalkWithDelay(9000, 4);

                if (auto hornizz = me->FindNearestCreature(NPC_HORNIZZ, 20.f))
                    me->CastSpell(hornizz, SPELL_DUMMY_PING);
                break;
            case 48:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);

            _events.Update(uiDiff);


            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ESCORT_MILAZZA_1:
                    if (auto cage = me->FindNearestGameObject(177706, 10.f))
                        cage->SetGoState(GO_STATE_ACTIVE);

                    _events.ScheduleEvent(EVENT_ESCORT_MILAZZA_2, 1000);
                    break;
                case EVENT_ESCORT_MILAZZA_2:
                    _started = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->AI()->TalkWithDelay(2000, 0, _playerGUID);
                    Start(true, false, _playerGUID);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _started = true;
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_melizza_brimbuzzle_escortAI(creature);
    }
};

enum QuestIntoTheFray
{
    QUEST_INTO_THE_FRAY = 14393,
    QUEST_DEATH_TO_AGOGRIDON = 14394,

    NPC_CENTAUR_MAULER_1 = 36402,
    NPC_CENTAUR_MAULER_2 = 74922,
    NPC_CENTAUR_MAULER_3 = 74921,
    NPC_CENTAUR_WINDCHASER = 36401,
    SPELL_ATTACK_ORDER = 68672,
    SPELL_RETURN_ORDER = 68676,
    SPELL_PING_ATTACK = 151344,
};

class npc_centaur_commander_vehicle : public CreatureScript
{
public:
    npc_centaur_commander_vehicle() : CreatureScript("npc_centaur_commander_vehicle") { }

    struct npc_centaur_commander_vehicleAI : public VehicleAI
    {
        npc_centaur_commander_vehicleAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->EnterVehicle(me, 0);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                player->ExitVehicle();
                _summons.DespawnAll();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_RETURN_ORDER)
            {
                if (auto mauler1 = Unit::GetCreature(*me, _mauler1_GUID))
                    if (auto mauler2 = Unit::GetCreature(*me, _mauler2_GUID))
                        if (auto mauler3 = Unit::GetCreature(*me, _mauler3_GUID))
                            if (auto windchaser = Unit::GetCreature(*me, _windchaser_GUID))
                            {
                                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                                mauler1->CastSpell(mauler1, SPELL_SANCTUARY_NO_COMBAT);
                                mauler2->CastSpell(mauler2, SPELL_SANCTUARY_NO_COMBAT);
                                mauler3->CastSpell(mauler3, SPELL_SANCTUARY_NO_COMBAT);
                                windchaser->CastSpell(windchaser, SPELL_SANCTUARY_NO_COMBAT);
                            }
            }

            if (spellInfo->Id == SPELL_PING_ATTACK)
            {
                if (auto mauler1 = Unit::GetCreature(*me, _mauler1_GUID))
                    if (auto mauler2 = Unit::GetCreature(*me, _mauler2_GUID))
                        if (auto mauler3 = Unit::GetCreature(*me, _mauler3_GUID))
                            if (auto windchaser = Unit::GetCreature(*me, _windchaser_GUID))
                            {
                                mauler1->AI()->DoAction(ACTION_CENTAUR_ATTACK_ENEMY);
                                mauler2->AI()->DoAction(ACTION_CENTAUR_ATTACK_ENEMY);
                                mauler3->AI()->DoAction(ACTION_CENTAUR_ATTACK_ENEMY);
                                windchaser->AI()->DoAction(ACTION_CENTAUR_ATTACK_ENEMY);
                            }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto mauler1 = me->SummonCreature(NPC_CENTAUR_MAULER_1, me->GetPositionX(), me->GetPositionY() + 2, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                    if (auto mauler2 = me->SummonCreature(NPC_CENTAUR_MAULER_2, me->GetPositionX(), me->GetPositionY() - 2, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto mauler3 = me->SummonCreature(NPC_CENTAUR_MAULER_3, me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto windchaser = me->SummonCreature(NPC_CENTAUR_WINDCHASER, me->GetPositionX() - 2, me->GetPositionY(), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                _summons.Summon(mauler1);
                                _summons.Summon(mauler2);
                                _summons.Summon(mauler3);
                                _summons.Summon(windchaser);

                                _mauler1_GUID = mauler1->GetGUID();
                                _mauler2_GUID = mauler2->GetGUID();
                                _mauler3_GUID = mauler3->GetGUID();
                                _windchaser_GUID = windchaser->GetGUID();

                                mauler1->AI()->SetData(1, 1);
                                mauler2->AI()->SetData(1, 2);
                                mauler3->AI()->SetData(1, 3);
                                windchaser->AI()->SetData(1, 4);
                            }
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _summons.DespawnAll();
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(500);
            }
        }

        void UpdateAI(uint32 const diff)  override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (_isInAttackArea)
                {
                    if (me->GetAreaId() != 604)
                        if (me->GetAreaId() != 2198)
                        {
                            _isInAttackArea = false;
                            me->AI()->Talk(0, _playerGUID);
                            _events.ScheduleEvent(EVENT_CENTAUR_COMMANDER_1, 10000);
                        }
                }
                else
                {
                    if (me->GetAreaId() == 604 || me->GetAreaId() == 2198)
                    {
                        _events.CancelEvent(EVENT_CENTAUR_COMMANDER_1);
                        _isInAttackArea = true;
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }

                    if (player->GetQuestStatus(QUEST_INTO_THE_FRAY) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_DEATH_TO_AGOGRIDON) == QUEST_STATUS_REWARDED)
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CENTAUR_COMMANDER_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(500);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isInAttackArea = true;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        uint64 _mauler1_GUID = 0;
        uint64 _mauler2_GUID = 0;
        uint64 _mauler3_GUID = 0;
        uint64 _windchaser_GUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_centaur_commander_vehicleAI(creature);
    }
};

enum CentaurPetsData
{
    SPELL_THRASH = 3391,
    SPELL_LIGHTING_BOLT = 68670,
};

// ######## MAULER AND WINDCHASER
class npc_centaur_commander_companions : public CreatureScript
{
public:
    npc_centaur_commander_companions() : CreatureScript("npc_centaur_commander_companions") { }

    struct npc_centaur_commander_companionsAI : public ScriptedAI
    {
        npc_centaur_commander_companionsAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            //if (auto player = summoner->ToPlayer())
            //    _ownerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);

            if (auto owner = summoner->ToCreature())
                _ownerGUID = owner->GetGUID();
        }

        void DoAction(int32 const actionId)
        {
            if (auto centaur = Unit::GetCreature(*me, _ownerGUID))
            {
                if (actionId == ACTION_CENTAUR_ATTACK_ENEMY)
                {
                    std::list<Unit*> enemy = me->SelectNearbyUnits(0, 45.0f);
                    for (std::list<Unit*>::iterator itr = enemy.begin(); itr != enemy.end(); ++itr)
                        if (auto enemy = (*itr))
                        {
                            if (enemy->GetAura(SPELL_ATTACK_ORDER))
                            {
                                if (enemy->GetAura(SPELL_ATTACK_ORDER)->GetCasterGUID() == _ownerGUID)
                                {
                                    me->AI()->AttackStart(enemy);
                                }
                            }
                        }
                }
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetEntry() == NPC_CENTAUR_MAULER_1 || me->GetEntry() == NPC_CENTAUR_MAULER_2 || me->GetEntry() == NPC_CENTAUR_MAULER_3)
            {
                _events.ScheduleEvent(EVENT_MAULER_THRASH, urand(2000, 9000));
            }
            else // windchaser  36401
            {
                _events.ScheduleEvent(EVENT_WINDCHASER_LIGHTING_BOLT, urand(0, 1000));
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto owner = Unit::GetCreature(*me, _ownerGUID))
                {
                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && owner->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(owner->getVictim());
                    }

                    if (!owner->getVictim() && !owner->isInCombat())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);

                        if (me->GetEntry() == NPC_CENTAUR_MAULER_1)
                            me->GetMotionMaster()->MoveFollow(owner, 1.f, 0.5f * M_PI);
                        else if (me->GetEntry() == NPC_CENTAUR_MAULER_2)
                            me->GetMotionMaster()->MoveFollow(owner, 1.f, 1.8f * M_PI);
                        else if (me->GetEntry() == NPC_CENTAUR_MAULER_3)
                            me->GetMotionMaster()->MoveFollow(owner, 3.0f, 0.5f * M_PI);
                        else if (me->GetEntry() == NPC_CENTAUR_WINDCHASER)
                            me->GetMotionMaster()->MoveFollow(owner, 3.0f, 1.8f * M_PI);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MAULER_THRASH:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_THRASH);
                    _events.ScheduleEvent(EVENT_MAULER_THRASH, urand(10000, 24000));
                    break;
                case EVENT_WINDCHASER_LIGHTING_BOLT:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LIGHTING_BOLT);
                    _events.ScheduleEvent(EVENT_WINDCHASER_LIGHTING_BOLT, urand(3100, 4000));
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isFollowing = false;
        uint32 _checkTimer = 2000;
        uint64 _ownerGUID = 0;
        uint64 _playerGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_centaur_commander_companionsAI(creature);
    }
};


enum AgogridonData
{
    NPC_KHAN_LEHPRAH = 36444,
    NPC_KHAN_KAMMAH = 36446,
    NPC_KHAN_SHODO = 36445,
    NPC_AGO_CREDIT = 36442,

    SPELL_CRIPPLE = 20812,
    SPELL_RAIN_OF_FIRE = 11990,
    SPELL_WAR_STOMP = 16727,
    SPELL_AGOGRIDON_CREDIT = 68734,
};

// Agogridon
class npc_agogridon : public CreatureScript
{
public:
    npc_agogridon() : CreatureScript("npc_agogridon") { }

    struct npc_agogridonAI : public ScriptedAI
    {
        npc_agogridonAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->CastSpell(me, SPELL_ROOT);
            me->setActive(true);
            _fightInProgress = false;
            _summons.DespawnAll();
        }

        void EnterEvadeMode()
        {
            _fightInProgress = false;
            _summons.DespawnAll();
        }

        void JustDied(Unit* killer)
        {
            if (auto lehprah = Unit::GetCreature(*me, _lehprahGUID))
                if (auto kammah = Unit::GetCreature(*me, _kammahGUID))
                    if (auto shodo = Unit::GetCreature(*me, _shodoGUID))
                    {
                        lehprah->GetMotionMaster()->Clear();
                        lehprah->CombatStop();
                        lehprah->SetReactState(REACT_PASSIVE);
                        lehprah->AI()->SetData(1, 1);
                        kammah->GetMotionMaster()->Clear();
                        kammah->CombatStop();
                        kammah->SetReactState(REACT_PASSIVE);
                        kammah->AI()->SetData(1, 1);
                        shodo->GetMotionMaster()->Clear();
                        shodo->CombatStop();
                        shodo->SetReactState(REACT_PASSIVE);
                        shodo->AI()->SetData(1, 1);
                    }

            std::list<Player*> player = me->GetPlayersInRange(55, true);
            for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
            {
                if ((*it)->GetQuestStatus(QUEST_DEATH_TO_AGOGRIDON) == QUEST_STATUS_INCOMPLETE)
                    (*it)->KilledMonsterCredit(NPC_AGO_CREDIT);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->CastSpell(me, SPELL_ROOT);
            _events.ScheduleEvent(EVENT_AGOGRIDON_CRIPPLE, urand(6000, 10000));
            _events.ScheduleEvent(EVENT_AGOGRIDON_RAIN_OF_FIRE, urand(8000, 15000));
            _events.ScheduleEvent(EVENT_AGOGRIDON_WAR_STOMP, urand(7000, 20000));

            if (!_fightInProgress)
            {
                _fightInProgress = true;

                if (auto lehprah = me->SummonCreature(NPC_KHAN_LEHPRAH, -1993.44f, 562.820f, 165.76f, 5.6f, TEMPSUMMON_MANUAL_DESPAWN))
                    if (auto kammah = me->SummonCreature(NPC_KHAN_KAMMAH, -1993.36f, 567.990f, 166.34f, 5.6f, TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto shodo = me->SummonCreature(NPC_KHAN_SHODO, -1995.62f, 559.359f, 165.70f, 5.6f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            _lehprahGUID = lehprah->GetGUID();
                            _kammahGUID = kammah->GetGUID();
                            _shodoGUID = shodo->GetGUID();
                            _summons.Summon(lehprah);
                            _summons.Summon(kammah);
                            _summons.Summon(shodo);
                            lehprah->setActive(true);
                            kammah->setActive(true);
                            shodo->setActive(true);
                        }
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
                case EVENT_AGOGRIDON_CRIPPLE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_CRIPPLE);
                    _events.ScheduleEvent(EVENT_AGOGRIDON_CRIPPLE, urand(15000, 20000));
                    break;
                case EVENT_AGOGRIDON_RAIN_OF_FIRE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_RAIN_OF_FIRE);
                    _events.ScheduleEvent(EVENT_AGOGRIDON_RAIN_OF_FIRE, urand(15000, 20000));
                    break;
                case EVENT_AGOGRIDON_WAR_STOMP:
                    me->CastSpell(me, SPELL_WAR_STOMP);
                    _events.ScheduleEvent(EVENT_AGOGRIDON_WAR_STOMP, urand(15000, 20000));
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _fightInProgress = false;
        uint64 _lehprahGUID = 0;
        uint64 _kammahGUID = 0;
        uint64 _shodoGUID = 0;
        SummonList _summons;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agogridonAI(creature);
    }
};


uint32 const JankieMountPathSize = 9;
const G3D::Vector3 JankieMountPath[JankieMountPathSize] =
{
    {  171.54538f, 1789.1031f, 91.1964f },
    {  165.23950f, 1780.6389f, 99.3984f },
    {  135.93684f, 1745.8359f, 117.152f },
    {  93.624428f, 1682.4067f, 138.9768f },
    {  90.296234f, 1578.1204f, 183.2577f },
    {  91.958420f, 1483.3758f, 210.4224f },
    {  95.258705f, 1390.8143f, 215.9511f },
    {  114.40130f, 1344.0339f, 203.498f },
    {  133.90341f, 1324.7130f, 194.346f }, // 8
};

enum JankiesMount
{
    SPELL_RIDE_JANKIE_MOUNT = 68701,
};

class npc_jankies_gryphon_vehicle : public CreatureScript
{
public:
    npc_jankies_gryphon_vehicle() : CreatureScript("npc_jankies_gryphon_vehicle") { }

    struct npc_jankies_gryphon_vehicleAI : public VehicleAI
    {
        npc_jankies_gryphon_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(me, SPELL_RIDE_JANKIE_MOUNT);
                _playerGUID = player->GetGUID();
            }

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_JANKIE_1_TRIP_1, 1000);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->DespawnOrUnsummon(100);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 8:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->DespawnOrUnsummon(100);
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
                case EVENT_JANKIE_1_TRIP_1:
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(JankieMountPath, JankieMountPathSize);
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
        return new npc_jankies_gryphon_vehicleAI(creature);
    }
};

enum QuestPlasm
{
    QUEST_GHOST_O_PLASM = 6134,

    OBJECT_GHOST_MAGNET = 177746,
    OBJECT_GHOST_MAGNET_AURA = 177749,

    NPC_MAGRAMI_SPECTRE = 11560,
};

const Position MagramiSpectreSpawnPosition[] =
{
    { -2300.654f, 1408.65f, 63.5479f },
    { -2297.776f, 1409.36f, 63.5866f },
    { -2294.812f, 1409.35f, 63.5866f },
    { -2292.138f, 1408.08f, 63.5866f },
    { -2289.688f, 1406.55f, 63.5866f },
    { -2287.295f, 1404.49f, 63.5866f },
    { -2284.004f, 1401.51f, 64.9644f },
    { -2281.634f, 1399.04f, 65.5009f },
    { -2279.749f, 1396.85f, 66.1232f },
    { -2278.088f, 1394.51f, 66.6504f },
    { -2276.403f, 1392.05f, 67.1463f },
    { -2274.839f, 1389.76f, 67.4841f },
    { -2273.314f, 1387.54f, 67.8212f },
    { -2271.739f, 1385.22f, 68.0883f },
    { -2270.298f, 1383.05f, 68.3103f },
    { -2283.376f, 1315.33f, 64.7970f },
    { -2280.313f, 1315.39f, 64.3474f },
    { -2277.544f, 1315.67f, 63.9537f },
    { -2273.462f, 1316.54f, 63.6814f },
    { -2270.630f, 1317.69f, 63.6653f },
    { -2268.536f, 1318.85f, 63.6741f },
    { -2264.367f, 1321.63f, 63.6522f },
    { -2262.552f, 1323.39f, 63.6168f },
    { -2261.046f, 1327.60f, 63.5935f },
};

class npc_ghost_o_plasm_controller : public CreatureScript
{
public:
    npc_ghost_o_plasm_controller() : CreatureScript("npc_ghost_o_plasm_controller") { }

    struct npc_ghost_o_plasm_controllerAI : public ScriptedAI
    {
        npc_ghost_o_plasm_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto magnet = me->SummonGameObject(OBJECT_GHOST_MAGNET, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.f, 0, 0, 0, 0, 0))
                if (auto magnetaura = me->SummonGameObject(OBJECT_GHOST_MAGNET_AURA, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.f, 0, 0, 0, 0, 0))
                {
                    me->setActive(true);
                    magnetaura->setActive(true);
                    magnet->setActive(true);
                    _magnetGUID = magnet->GetGUID();
                    _magnetAuraGUID = magnetaura->GetGUID();
                }

            _events.ScheduleEvent(EVENT_GHOST_PLASM_1, 3000);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 14.0f))
            {
                if (auto spectre = who->ToCreature())
                {
                    if (spectre->GetEntry() == NPC_MAGRAMI_SPECTRE)
                    {
                        if (spectre->getFaction() == 190)
                        {
                            spectre->SetHomePosition(spectre->GetPositionX(), spectre->GetPositionY(), spectre->GetPositionZ(), spectre->GetOrientation());
                            spectre->setFaction(16);
                            spectre->AI()->Talk(1, _playerGUID);

                            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                                spectre->AI()->AttackStart(player);
                        }
                    }
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                //_summons.RemoveNotExisting();

                if (auto spectre1 = me->SummonCreature(NPC_MAGRAMI_SPECTRE, MagramiSpectreSpawnPosition[urand(0, 23)], TEMPSUMMON_MANUAL_DESPAWN))
                    _summons.Summon(spectre1);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        me->DespawnOrUnsummon();
                        _summons.DespawnAll();

                        if (auto magnet = GameObject::GetGameObject(*me, _magnetGUID))
                            if (auto magnetaura = GameObject::GetGameObject(*me, _magnetAuraGUID))
                            {
                                magnetaura->Delete();
                                magnet->Delete();
                            }
                    }

                    if (player->GetQuestStatus(QUEST_GHOST_O_PLASM) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_GHOST_O_PLASM) == QUEST_STATUS_COMPLETE)
                    {
                        me->DespawnOrUnsummon();
                        _summons.DespawnAll();

                        if (auto magnet = GameObject::GetGameObject(*me, _magnetGUID))
                            if (auto magnetaura = GameObject::GetGameObject(*me, _magnetAuraGUID))
                            {
                                magnetaura->Delete();
                                magnet->Delete();
                            }
                    }

                    if (me->GetDistance(player) > 40.f)
                    {
                        me->AI()->Talk(0, _playerGUID);
                        me->DespawnOrUnsummon();
                        _summons.DespawnAll();

                        if (auto magnet = GameObject::GetGameObject(*me, _magnetGUID))
                            if (auto magnetaura = GameObject::GetGameObject(*me, _magnetAuraGUID))
                            {
                                magnetaura->Delete();
                                magnet->Delete();
                            }
                    }
                }
            }
            else _checkTimer -= diff;
            
            if (_expireTimer <= diff)
            {
                _checkTimer = 5000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->AI()->Talk(1, _playerGUID);
                    me->DespawnOrUnsummon();
                    _summons.DespawnAll();

                    if (auto magnet = GameObject::GetGameObject(*me, _magnetGUID))
                        if (auto magnetaura = GameObject::GetGameObject(*me, _magnetAuraGUID))
                        {
                            magnetaura->Delete();
                            magnet->Delete();
                        }
                }
            }
            else _expireTimer -= diff;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GHOST_PLASM_1:
                    if (auto spectre1 = me->SummonCreature(NPC_MAGRAMI_SPECTRE, MagramiSpectreSpawnPosition[urand(0, 23)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto spectre2 = me->SummonCreature(NPC_MAGRAMI_SPECTRE, MagramiSpectreSpawnPosition[urand(0, 23)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto spectre3 = me->SummonCreature(NPC_MAGRAMI_SPECTRE, MagramiSpectreSpawnPosition[urand(0, 23)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto spectre4 = me->SummonCreature(NPC_MAGRAMI_SPECTRE, MagramiSpectreSpawnPosition[urand(0, 23)], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    _summons.Summon(spectre1);
                                    _summons.Summon(spectre2);
                                    _summons.Summon(spectre3);
                                    _summons.Summon(spectre4);
                                }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        uint64 _magnetGUID = 0;
        uint64 _magnetAuraGUID = 0;
        uint16 _checkTimer = 1000;
        uint64 _expireTimer = 175000;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghost_o_plasm_controllerAI(creature);
    }
};

// this madaf*** crashed my local eight times! finally it works :D possessing/charming thing is so unstable (perhaps i didnt understand this mechanic well xd)
enum QuestJukarsUndoing
{
    QUEST_JUGKARS_UNDOING_A = 14376,
    QUEST_JUGKARS_UNDOING_H = 14344,
    SPELL_POSSES_AURA = 68525,
    SPELL_LORD_BELLOW = 68545,
    SPELL_LORD_SHADOW_TORCH = 68548,
    SPELL_LORD_ENRAGE = 68541,
};

class npc_lord_azrethoc : public CreatureScript
{
public:
    npc_lord_azrethoc() : CreatureScript("npc_lord_azrethoc") { }

    struct npc_lord_azrethocAI : public ScriptedAI
    {
        npc_lord_azrethocAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->RestoreFaction();
            me->SetReactState(REACT_AGGRESSIVE);
            me->AI()->EnterEvadeMode();
        }

        void OnCharmed(bool apply)
        {
            if (apply)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                _events.CancelEvent(EVENT_LORD_BELLOW);
                _events.CancelEvent(EVENT_LORD_TORCH);

                if (auto player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                    _playerGUID = player->GetGUID();
            }
            else
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->DespawnOrUnsummon();

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->RemoveAura(SPELL_POSSES_AURA);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (!me->HasAura(SPELL_POSSES_AURA))
            {
                _events.ScheduleEvent(EVENT_LORD_BELLOW, urand(4000, 8000));
                _events.ScheduleEvent(EVENT_LORD_TORCH, urand(3000, 12000));
            }
        }

        void EnterEvadeMode() {}

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (me->HasAura(SPELL_POSSES_AURA))
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (_checkTimer <= diff)
                    {
                        _checkTimer = 500;

                        if (player->GetTeam() == ALLIANCE)
                        {
                            if (player->GetQuestStatus(QUEST_JUGKARS_UNDOING_A) == QUEST_STATUS_COMPLETE)
                            {
                                player->RemoveAura(SPELL_POSSES_AURA);
                                me->DespawnOrUnsummon();
                            }
                        }
                        else
                        {
                            if (player->GetQuestStatus(QUEST_JUGKARS_UNDOING_H) == QUEST_STATUS_COMPLETE)
                            {
                                player->RemoveAura(SPELL_POSSES_AURA);
                                me->DespawnOrUnsummon();
                            }
                        }
                    }
                    else _checkTimer -= diff;
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LORD_BELLOW:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LORD_BELLOW);
                    _events.ScheduleEvent(EVENT_LORD_BELLOW, urand(18000, 24000));
                    break;
                case EVENT_LORD_TORCH:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LORD_SHADOW_TORCH);
                    _events.ScheduleEvent(EVENT_LORD_TORCH, urand(18000, 24000));
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_azrethocAI(creature);
    }
};

enum Gizelton
{
    NPC_VENDOR_TRON_1000 = 12245,
    NPC_SUPER_SELLER_680 = 12246,

    NPC_KODO = 11564,
    // bodyguard quest
    NPC_CORK = 11625,
    NPC_RIGGER = 11626,
    // caravan quest
    NPC_CORK_2 = 74911,
    NPC_RIGGER_2 = 74910,

};

const Position spawnGizeltonPosition1[] =
{
    { -690.114f, 1521.270f, 90.206f, 0.2905f},
    { -691.728f, 1526.099f, 90.158f, 0.0431f},
    { -696.245f, 1530.040f, 91.152f, 5.6940f},
    { -691.721f, 1513.770f, 91.372f, 1.1741f},
};

class npc_north_gizelton_escort_controller : public CreatureScript
{
public:
    npc_north_gizelton_escort_controller() : CreatureScript("npc_north_gizelton_escort_controller") { }

    struct npc_north_gizelton_escort_controllerAI : public ScriptedAI
    {
        npc_north_gizelton_escort_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            _mode = 0;
            me->setActive(true);

            if (auto cork = me->SummonCreature(NPC_CORK, spawnGizeltonPosition1[0], TEMPSUMMON_MANUAL_DESPAWN))
            {
                cork->setActive(true);
                _corkGUID = cork->GetGUID();
                cork->SetOwnerGUID(me->GetGUID());
                cork->SetPhaseMask(1024, true);
                cork->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                cork->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }

        void DoAction(int32 const actionId)
        {
            if (actionId == ACTION_NORTH_GIZELTON_1)
            {
                if (auto cork = Unit::GetCreature(*me, _corkGUID))
                    if (auto vendortron = Unit::GetCreature(*me, _vendortronGUID))
                    {
                        cork->AI()->DoAction(ACTION_CORK_2);
                        vendortron->DespawnOrUnsummon();
                    }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_eventTimer <= diff)
            {
                if (_mode == 0)
                {
                    _eventTimer = 600000; // 10 minutes
                    _mode = 1;

                    if (auto vendortron = me->SummonCreature(NPC_VENDOR_TRON_1000, -692.742f, 1522.199f, 90.361f, 0.558f, TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _vendortronGUID = vendortron->GetGUID();
                        vendortron->setActive(true);
                    }

                    if (auto cork = Unit::GetCreature(*me, _corkGUID))
                        cork->AI()->DoAction(ACTION_CORK_1);
                }

                else if (_mode == 1)
                {
                    _eventTimer = 3600000; // 1 hour 
                    Reset();
                }
            }
            else
                _eventTimer -= diff;
        }
    private:
        uint16 _mode = 0;
        uint64 _corkGUID = 0;
        uint64 _vendortronGUID = 0;
        uint64 _eventTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_north_gizelton_escort_controllerAI(creature);
    }
};

enum QuestBodyguardForHire
{
    QUEST_BODYGUARD_FOR_HIRE = 5821,

    NPC_KOLKAR_AMBUSHER = 12977,
    NPC_KOLKAR_WAYLAYER = 12976,
};

const Position KolkarEnemySpawnPosition[] =
{
    // wave 1
    { -884.93719f, 1181.7094f, 97.0736f},
    { -872.50024f, 1194.4683f, 99.0812f},
    { -871.48632f, 1175.4042f, 98.9239f},
    // wave 2
    { -976.94134f, 1197.4110f, 92.1735f},
    { -963.12506f, 1182.8619f, 91.0069f},
    { -991.66088f, 1177.5477f, 90.0335f},
    // wave 3
    { -1092.95617f, 1180.11450f, 89.7401f},
    { -1089.25488f, 1200.31298f, 89.7401f},
    { -1068.59484f, 1179.45459f, 89.7949f},
};

class npc_cork_gizelton_escort : public CreatureScript
{
public:
    npc_cork_gizelton_escort() : CreatureScript("npc_cork_gizelton_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BODYGUARD_FOR_HIRE)       
            player->CastSpell(creature, SPELL_DUMMY_PING);        

        return true;
    }

    struct npc_cork_gizelton_escortAI : public npc_escortAI
    {
        npc_cork_gizelton_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            _started = false;

            if (auto owner = summoner->ToCreature())
                _controllerGUID = owner->GetGUID();

            if (auto rigger = me->SummonCreature(NPC_RIGGER, spawnGizeltonPosition1[1], TEMPSUMMON_MANUAL_DESPAWN))
                if (auto kodo1 = me->SummonCreature(NPC_KODO, spawnGizeltonPosition1[2], TEMPSUMMON_MANUAL_DESPAWN))
                    if (auto kodo2 = me->SummonCreature(NPC_KODO, spawnGizeltonPosition1[3], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        rigger->setActive(true);
                        rigger->SetPhaseMask(1024, true);
                        _riggerGUID = rigger->GetGUID();
                        kodo1->setActive(true);
                        kodo1->SetPhaseMask(1024, true);
                        _kodo1GUID = kodo1->GetGUID();
                        kodo2->setActive(true);
                        kodo2->SetPhaseMask(1024, true);
                        _kodo2GUID = kodo2->GetGUID();
                        me->SetReactState(REACT_PASSIVE);
                        rigger->SetReactState(REACT_PASSIVE);
                        kodo2->SetReactState(REACT_PASSIVE);
                        kodo1->SetReactState(REACT_PASSIVE);
                        rigger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                    }
        }

        void JustDied(Unit* /*killer*/)
        {
            auto character = me->GetPlayersInRange(55, true);
            for (auto it : character)
                if (it->GetQuestStatus(QUEST_BODYGUARD_FOR_HIRE) == QUEST_STATUS_INCOMPLETE)
                {
                    it->FailQuest(QUEST_BODYGUARD_FOR_HIRE);

                    if (auto group = it->GetGroup())
                    {
                        for (auto groupRef = group->GetFirstMember(); groupRef != NULL; groupRef = groupRef->next())
                            if (auto member = groupRef->getSource())
                                if (member->GetQuestStatus(QUEST_BODYGUARD_FOR_HIRE) == QUEST_STATUS_INCOMPLETE)
                                    member->FailQuest(QUEST_BODYGUARD_FOR_HIRE);
                    }
                }

            if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                    if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                    {
                        me->DespawnOrUnsummon();
                        rigger->DespawnOrUnsummon();
                        kodo1->DespawnOrUnsummon();
                        kodo2->DespawnOrUnsummon();
                    }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                    if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                    {
                        rigger->SetHomePosition(rigger->GetPositionX(), rigger->GetPositionY(), rigger->GetPositionZ(), rigger->GetOrientation());
                        kodo1->SetHomePosition(kodo1->GetPositionX(), kodo1->GetPositionY(), kodo1->GetPositionZ(), kodo1->GetOrientation());
                        kodo2->SetHomePosition(kodo2->GetPositionX(), kodo2->GetPositionY(), kodo2->GetPositionZ(), kodo2->GetOrientation());
                    }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (!_started)
                {
                    SetEscortPaused(false);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    _started = true;
                    _waitingForStart = false;;

                    if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->SetWalk(true);
                                rigger->SetWalk(true);
                                kodo1->SetWalk(true);
                                kodo2->SetWalk(true);
                                rigger->setFaction(1806);
                                kodo1->setFaction(1806);
                                kodo2->setFaction(1806);
                                me->setFaction(1806);
                                rigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                kodo1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                kodo2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                me->SetReactState(REACT_AGGRESSIVE);
                                rigger->SetReactState(REACT_AGGRESSIVE);
                                kodo2->SetReactState(REACT_AGGRESSIVE);
                                kodo1->SetReactState(REACT_AGGRESSIVE);
                            }
                }
            }
        }

        void DoAction(int32 const actionId)
        {
            if (actionId == ACTION_CORK_1)
            {
                if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                    if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                        if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                        {
                            me->SetPhaseMask(1025, true);
                            rigger->SetPhaseMask(1025, true);
                            kodo1->SetPhaseMask(1025, true);
                            kodo2->SetPhaseMask(1025, true);

                            _events.ScheduleEvent(EVENT_CORK_3, 600000);
                        }
            }

            if (actionId == ACTION_CORK_2)
            {
                if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                    if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                        if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                        {
                            kodo1->GetMotionMaster()->MoveFollow(me, 3.0f, M_PI, MOTION_SLOT_ACTIVE);
                            rigger->GetMotionMaster()->MoveFollow(kodo1, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            kodo2->GetMotionMaster()->MoveFollow(rigger, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            rigger->SetWalk(false);
                            kodo1->SetWalk(false);
                            kodo2->SetWalk(false);
                            me->AI()->Talk(0);

                            _events.ScheduleEvent(EVENT_CORK_1, 0);
                        }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 22:
            {
                _waitingForStart = true;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                SetEscortPaused(true);
                me->AI()->TalkWithDelay(2000, 1);
            }
            break;
            case 28:
            {
                me->AI()->TalkWithDelay(3000, 2);

                if (auto enemy1 = me->SummonCreature(NPC_KOLKAR_WAYLAYER, KolkarEnemySpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 180000))
                    if (auto enemy2 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        if (auto enemy3 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        {
                            enemy1->setActive(true);
                            enemy2->setActive(true);
                            enemy3->setActive(true);
                            enemy1->AI()->AttackStart(me);
                            enemy2->AI()->AttackStart(me);
                            enemy3->AI()->AttackStart(me);
                        }
            }
            break;

            case 33:
            {
                me->AI()->TalkWithDelay(1000, 3);

                if (auto enemy1 = me->SummonCreature(NPC_KOLKAR_WAYLAYER, KolkarEnemySpawnPosition[3], TEMPSUMMON_TIMED_DESPAWN, 180000))
                    if (auto enemy2 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[4], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        if (auto enemy3 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[5], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        {
                            enemy1->setActive(true);
                            enemy2->setActive(true);
                            enemy3->setActive(true);
                            enemy1->AI()->AttackStart(me);
                            enemy2->AI()->AttackStart(me);
                            enemy3->AI()->AttackStart(me);
                        }
            }
            break;
            case 38:
            {
                me->AI()->TalkWithDelay(1000, 4);

                if (auto enemy1 = me->SummonCreature(NPC_KOLKAR_WAYLAYER, KolkarEnemySpawnPosition[6], TEMPSUMMON_TIMED_DESPAWN, 180000))
                    if (auto enemy2 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[7], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        if (auto enemy3 = me->SummonCreature(NPC_KOLKAR_AMBUSHER, KolkarEnemySpawnPosition[8], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        {
                            enemy1->setActive(true);
                            enemy2->setActive(true);
                            enemy3->setActive(true);
                            enemy1->AI()->AttackStart(me);
                            enemy2->AI()->AttackStart(me);
                            enemy3->AI()->AttackStart(me);
                        }
            }
            break;
            case 45:
            {
                _events.ScheduleEvent(EVENT_CORK_2, 8000);
                me->AI()->Talk(5);

                auto character = me->GetPlayersInRange(55, true);
                for (auto it = character.begin(); it != character.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_BODYGUARD_FOR_HIRE) == QUEST_STATUS_INCOMPLETE)
                    {
                        (*it)->AreaExploredOrEventHappens(QUEST_BODYGUARD_FOR_HIRE);

                        if (auto group = (*it)->GetGroup())
                        {
                            for (auto groupRef = group->GetFirstMember(); groupRef != NULL; groupRef = groupRef->next())
                                if (auto member = groupRef->getSource())
                                    if (member->GetQuestStatus(QUEST_BODYGUARD_FOR_HIRE) == QUEST_STATUS_INCOMPLETE)
                                        member->AreaExploredOrEventHappens(QUEST_BODYGUARD_FOR_HIRE);
                        }
                    }
            }
            break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            DoMeleeAttackIfReady();

            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);

            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CORK_1:
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                    Start(true, true);
                    SetDespawnAtEnd(false);
                    SetDespawnAtFar(false);
                    break;
                case EVENT_CORK_2:
                    if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->DespawnOrUnsummon();
                                rigger->DespawnOrUnsummon();
                                kodo1->DespawnOrUnsummon();
                                kodo2->DespawnOrUnsummon();
                            }
                    break;
                case EVENT_CORK_3:
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    if (auto controller = Unit::GetCreature(*me, _controllerGUID))
                        controller->AI()->DoAction(ACTION_NORTH_GIZELTON_1);
                    break;
                default:
                    break;
                }
            }

            if (_started)
            {
                if (!me->isInCombat())
                {
                    if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->SetWalk(true);
                                rigger->SetWalk(true);
                                kodo1->SetWalk(true);
                                kodo2->SetWalk(true);
                                kodo1->GetMotionMaster()->MoveFollow(me, 3.0f, M_PI, MOTION_SLOT_ACTIVE);
                                rigger->GetMotionMaster()->MoveFollow(kodo1, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                                kodo2->GetMotionMaster()->MoveFollow(rigger, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            }
                }
            }

            if (_waitingForStart)
            {
                if (_eventTimer <= uiDiff)
                {
                    _eventTimer = 900000;

                    if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->DespawnOrUnsummon();
                                rigger->DespawnOrUnsummon();
                                kodo1->DespawnOrUnsummon();
                                kodo2->DespawnOrUnsummon();
                            }
                }
                else
                    _eventTimer -= uiDiff;
            }
        }
    private:
        bool _waitingForStart = false;
        bool _started = false;
        EventMap _events;
        uint64 _riggerGUID = 0;
        uint64 _kodo1GUID = 0;
        uint64 _kodo2GUID = 0;
        uint64 _controllerGUID = 0;
        uint64 _eventTimer = 900000; // wait 15 mins if no player turn in a quest then despawn
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cork_gizelton_escortAI(creature);
    }
};


const Position spawnGizeltonPosition2[] =
{
    { -1925.033f, 2409.871f, 60.5681f, 0.33241f },
    { -1926.546f, 2415.700f, 60.7123f, 0.05359f },
    { -1933.208f, 2415.571f, 60.5599f, 5.95193f },
    { -1930.787f, 2406.372f, 60.2427f, 0.68976f },
};

class npc_south_gizelton_escort_controller : public CreatureScript
{
public:
    npc_south_gizelton_escort_controller() : CreatureScript("npc_south_gizelton_escort_controller") { }

    struct npc_south_gizelton_escort_controllerAI : public ScriptedAI
    {
        npc_south_gizelton_escort_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            _mode = 0;

            if (auto rigger = me->SummonCreature(NPC_RIGGER_2, spawnGizeltonPosition2[0], TEMPSUMMON_MANUAL_DESPAWN))
            {
                rigger->setActive(true);
                _riggerGUID = rigger->GetGUID();
                rigger->SetOwnerGUID(me->GetGUID());
                rigger->SetPhaseMask(1024, true);
                rigger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                rigger->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }

        void DoAction(int32 const actionId)
        {
            if (actionId == ACTION_SOUTH_GIZELTON_1)
            {
                if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                    if (auto superseller = Unit::GetCreature(*me, _supersellerGUID))
                    {
                        rigger->AI()->DoAction(ACTION_RIGGER_2);
                        superseller->DespawnOrUnsummon();
                    }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_eventTimer <= diff)
            {
                if (_mode == 0)
                {
                    _eventTimer = 600000; // 10 minutes
                    _mode = 1;

                    if (auto superseller = me->SummonCreature(NPC_SUPER_SELLER_680, -1926.599f, 2412.79f, 60.695599f, 0.174533f, TEMPSUMMON_TIMED_DESPAWN, 599999))
                    {
                        _supersellerGUID = superseller->GetGUID();
                        superseller->setActive(true);
                    }

                    if (auto rigger = Unit::GetCreature(*me, _riggerGUID))
                        rigger->AI()->DoAction(ACTION_RIGGER_1);                    
                }
                else if (_mode == 1)
                {
                    _eventTimer = 3600000; // 1 hour
                    Reset();
                }
            }
            else
                _eventTimer -= diff;
        }
    private:
        uint16 _mode = 0;
        uint64 _riggerGUID = 0;
        uint64 _supersellerGUID = 0;
        uint64 _eventTimer = 1800000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_south_gizelton_escort_controllerAI(creature);
    }
};

enum QuestGizeltonCaravan
{
    QUEST_GIZELTON_CARAVAN = 5943,

    NPC_LESSER_INFERNAL = 4676,
    NPC_NETHER_SORCERESS = 4684,
    NPC_DOOMWARDER = 4677,
};

const Position LegionEnemySpawnPosition[] =
{
    // wave 1
    { -1810.0797f, 2002.772f, 59.305f },
    { -1808.5693f, 1984.814f, 58.954f },
    { -1827.1892f, 1996.136f, 59.521f },
    // wave 2
    { -1727.8033f, 1913.257f, 59.375f },
    { -1728.1431f, 1923.500f, 59.231f },
    { -1751.2062f, 1921.807f, 59.001f },
};

class npc_rigger_gizelton_escort : public CreatureScript
{
public:
    npc_rigger_gizelton_escort() : CreatureScript("npc_rigger_gizelton_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_GIZELTON_CARAVAN)       
            player->CastSpell(creature, SPELL_DUMMY_PING);
        
        return true;
    }

    struct npc_rigger_gizelton_escortAI : public npc_escortAI
    {
        npc_rigger_gizelton_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            _started = false;

            if (auto owner = summoner->ToCreature())
                _controllerGUID = owner->GetGUID();


            if (auto cork = me->SummonCreature(NPC_CORK_2, spawnGizeltonPosition2[1], TEMPSUMMON_MANUAL_DESPAWN))
                if (auto kodo1 = me->SummonCreature(NPC_KODO, spawnGizeltonPosition2[2], TEMPSUMMON_MANUAL_DESPAWN))
                    if (auto kodo2 = me->SummonCreature(NPC_KODO, spawnGizeltonPosition2[3], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        cork->setActive(true);
                        cork->SetPhaseMask(1024, true);
                        _corkGUID = cork->GetGUID();
                        kodo1->setActive(true);
                        kodo1->SetPhaseMask(1024, true);
                        _kodo1GUID = kodo1->GetGUID();
                        kodo2->setActive(true);
                        kodo2->SetPhaseMask(1024, true);
                        _kodo2GUID = kodo2->GetGUID();
                        me->SetReactState(REACT_PASSIVE);
                        cork->SetReactState(REACT_PASSIVE);
                        kodo2->SetReactState(REACT_PASSIVE);
                        kodo1->SetReactState(REACT_PASSIVE);
                        cork->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                    }
        }

        void JustDied(Unit* /*killer*/)
        {
            auto character = me->GetPlayersInRange(55, true);
            for (auto it : character)
                if (it->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                {
                    it->FailQuest(QUEST_GIZELTON_CARAVAN);

                    if (auto group = it->GetGroup())
                    {
                        for (auto groupRef = group->GetFirstMember(); groupRef != NULL; groupRef = groupRef->next())
                            if (auto member = groupRef->getSource())
                                if (member->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                                    member->FailQuest(QUEST_GIZELTON_CARAVAN);
                    }
                }

            if (auto cork = Unit::GetCreature(*me, _corkGUID))
                if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                    if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                    {
                        me->DespawnOrUnsummon();
                        cork->DespawnOrUnsummon();
                        kodo1->DespawnOrUnsummon();
                        kodo2->DespawnOrUnsummon();
                    }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (auto cork = Unit::GetCreature(*me, _corkGUID))
                if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                    if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                    {
                        cork->SetHomePosition(cork->GetPositionX(), cork->GetPositionY(), cork->GetPositionZ(), cork->GetOrientation());
                        kodo1->SetHomePosition(kodo1->GetPositionX(), kodo1->GetPositionY(), kodo1->GetPositionZ(), kodo1->GetOrientation());
                        kodo2->SetHomePosition(kodo2->GetPositionX(), kodo2->GetPositionY(), kodo2->GetPositionZ(), kodo2->GetOrientation());
                    }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (!_started)
                {
                    SetEscortPaused(false);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    _started = true;
                    _waitingForStart = false;;

                    if (auto cork = Unit::GetCreature(*me, _corkGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->SetWalk(true);
                                cork->SetWalk(true);
                                kodo1->SetWalk(true);
                                kodo2->SetWalk(true);
                                cork->setFaction(1806);
                                kodo1->setFaction(1806);
                                kodo2->setFaction(1806);
                                me->setFaction(1806);
                                cork->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                kodo1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                kodo2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                me->SetReactState(REACT_AGGRESSIVE);
                                cork->SetReactState(REACT_AGGRESSIVE);
                                kodo2->SetReactState(REACT_AGGRESSIVE);
                                kodo1->SetReactState(REACT_AGGRESSIVE);
                            }
                }
            }
        }

        void DoAction(int32 const actionId)
        {
            if (actionId == ACTION_RIGGER_1)
            {
                if (auto cork = Unit::GetCreature(*me, _corkGUID))
                    if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                        if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                        {
                            me->SetPhaseMask(1025, true);
                            cork->SetPhaseMask(1025, true);
                            kodo1->SetPhaseMask(1025, true);
                            kodo2->SetPhaseMask(1025, true);

                            _events.ScheduleEvent(EVENT_RIGGER_3, 600000);
                        }
            }

            if (actionId == ACTION_RIGGER_2)
            {
                if (auto cork = Unit::GetCreature(*me, _corkGUID))
                    if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                        if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                        {
                            kodo1->GetMotionMaster()->MoveFollow(me, 3.0f, M_PI, MOTION_SLOT_ACTIVE);
                            cork->GetMotionMaster()->MoveFollow(kodo1, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            kodo2->GetMotionMaster()->MoveFollow(cork, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            cork->SetWalk(false);
                            kodo1->SetWalk(false);
                            kodo2->SetWalk(false);
                            me->AI()->Talk(0);

                            _events.ScheduleEvent(EVENT_RIGGER_1, 0);
                        }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 12:
            {
                _waitingForStart = true;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                SetEscortPaused(true);
                me->AI()->TalkWithDelay(2000, 1);
            }
            break;
            case 27:
            {
                me->AI()->TalkWithDelay(3000, 2);

                if (auto enemy1 = me->SummonCreature(NPC_LESSER_INFERNAL, LegionEnemySpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 180000))
                    if (auto enemy2 = me->SummonCreature(NPC_NETHER_SORCERESS, LegionEnemySpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        if (auto enemy3 = me->SummonCreature(NPC_DOOMWARDER, LegionEnemySpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        {
                            enemy1->setActive(true);
                            enemy2->setActive(true);
                            enemy3->setActive(true);
                            enemy1->AI()->AttackStart(me);
                            enemy2->AI()->AttackStart(me);
                            enemy3->AI()->AttackStart(me);
                        }
            }
            break;

            case 35:
            {
                me->AI()->TalkWithDelay(1000, 3);

                if (auto enemy1 = me->SummonCreature(NPC_LESSER_INFERNAL, LegionEnemySpawnPosition[3], TEMPSUMMON_TIMED_DESPAWN, 180000))
                    if (auto enemy2 = me->SummonCreature(NPC_NETHER_SORCERESS, LegionEnemySpawnPosition[4], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        if (auto enemy3 = me->SummonCreature(NPC_DOOMWARDER, LegionEnemySpawnPosition[5], TEMPSUMMON_TIMED_DESPAWN, 180000))
                        {
                            enemy1->setActive(true);
                            enemy2->setActive(true);
                            enemy3->setActive(true);
                            enemy1->AI()->AttackStart(me);
                            enemy2->AI()->AttackStart(me);
                            enemy3->AI()->AttackStart(me);
                        }
            }
            break;
            case 49:
            {
                _events.ScheduleEvent(EVENT_RIGGER_2, 8000);
                me->AI()->Talk(4);

                auto character = me->GetPlayersInRange(55, true);
                for (auto it = character.begin(); it != character.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                    {
                        (*it)->AreaExploredOrEventHappens(QUEST_GIZELTON_CARAVAN);

                        if (auto group = (*it)->GetGroup())
                        {
                            for (auto groupRef = group->GetFirstMember(); groupRef != NULL; groupRef = groupRef->next())
                                if (auto member = groupRef->getSource())
                                    if (member->GetQuestStatus(QUEST_GIZELTON_CARAVAN) == QUEST_STATUS_INCOMPLETE)
                                        member->AreaExploredOrEventHappens(QUEST_GIZELTON_CARAVAN);
                        }
                    }
            }
            break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            DoMeleeAttackIfReady();

            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);

            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RIGGER_1:
                    Start(true, true);
                    SetDespawnAtEnd(false);
                    SetDespawnAtFar(false);
                    break;
                case EVENT_RIGGER_2:
                    if (auto cork = Unit::GetCreature(*me, _corkGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->DespawnOrUnsummon();
                                cork->DespawnOrUnsummon();
                                kodo1->DespawnOrUnsummon();
                                kodo2->DespawnOrUnsummon();
                            }
                    break;
                case EVENT_RIGGER_3:
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    if (auto controller = Unit::GetCreature(*me, _controllerGUID))
                        controller->AI()->DoAction(ACTION_SOUTH_GIZELTON_1);
                    break;
                default:
                    break;
                }
            }

            if (_started)
            {
                if (!me->isInCombat())
                {
                    if (auto cork = Unit::GetCreature(*me, _corkGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->SetWalk(true);
                                cork->SetWalk(true);
                                kodo1->SetWalk(true);
                                kodo2->SetWalk(true);
                                kodo1->GetMotionMaster()->MoveFollow(me, 3.0f, M_PI, MOTION_SLOT_ACTIVE);
                                cork->GetMotionMaster()->MoveFollow(kodo1, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                                kodo2->GetMotionMaster()->MoveFollow(cork, 3.f, M_PI, MOTION_SLOT_ACTIVE);
                            }
                }
            }

            if (_waitingForStart)
            {
                if (_eventTimer <= uiDiff)
                {
                    _eventTimer = 900000;

                    if (auto cork = Unit::GetCreature(*me, _corkGUID))
                        if (auto kodo1 = Unit::GetCreature(*me, _kodo1GUID))
                            if (auto kodo2 = Unit::GetCreature(*me, _kodo2GUID))
                            {
                                me->DespawnOrUnsummon();
                                cork->DespawnOrUnsummon();
                                kodo1->DespawnOrUnsummon();
                                kodo2->DespawnOrUnsummon();
                            }
                }
                else
                    _eventTimer -= uiDiff;
            }
        }
    private:
        bool _waitingForStart = false;
        bool _started = false;
        EventMap _events;
        uint64 _corkGUID = 0;
        uint64 _kodo1GUID = 0;
        uint64 _kodo2GUID = 0;
        uint64 _controllerGUID = 0;
        uint64 _eventTimer = 900000; // wait 15 mins if no player turn in a quest then despawn
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rigger_gizelton_escortAI(creature);
    }
};

void AddSC_desolace()
{
    new npc_furiens_dragonhawk1_vehicle();
    new npc_furiens_dragonhawk2_vehicle();
    new npc_wispy_vortex();
    new npc_raging_vortex();
    new npc_swoop_egg_early_adoption();
    new npc_korrahs_mount_trip();
    new item_slitherblade_charm();
    new npc_lord_hydronis_vs_vashjir_vehicle();
    new npc_aged_dying_ancient_kodo();
    new npc_tamed_kodo_roundup();
    new npc_kolkar_centaur_vehicle();
    new npc_spear_of_the_kolkar_khan();
    new npc_grimfeather_vehicle();
    new npc_melizza_brimbuzzle_escort();
    new npc_centaur_commander_vehicle();
    new npc_centaur_commander_companions();
    new npc_agogridon();
    new npc_jankies_gryphon_vehicle();
    new npc_ghost_o_plasm_controller();
    new npc_lord_azrethoc();
    new npc_north_gizelton_escort_controller();
    new npc_cork_gizelton_escort();
    new npc_south_gizelton_escort_controller();
    new npc_rigger_gizelton_escort();
}