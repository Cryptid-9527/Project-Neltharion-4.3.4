/*
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

enum LostIslesDefaultSpells
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
    SPELL_ESCORT_DESPAWN_PING = 95306,
};

enum LostIslesEvents
{
    EVENT_MINER_CART_0 = 1,
    EVENT_MINER_CART_1,
    EVENT_MINER_CART_2,
    EVENT_MINER_CART_3,
    EVENT_MINER_CART_4,
    EVENT_MINER_CART_5,
    EVENT_ORC_SCOUT_REND,
    EVENT_ORC_SCOUT_HEROIC_STRIKE,
    EVENT_ORC_SCOUT_THUNDERCLAP,
    EVENT_ORC_SCOUT_CHARGE,
    EVENT_BASTIA_1,
    EVENT_BASTIA_2,
    EVENT_GYRO_CARGO_1,
    EVENT_GYRO_CARGO_2,
    EVENT_GYRO_WARCHIEF_1,
    EVENT_GYRO_WARCHIEF_2,
    EVENT_SLING_ROCKET_1,
    EVENT_SLING_ROCKET_2,
    EVENT_TRADING_EGG_1,
    EVENT_ACE_ESCORT_1,
    EVENT_ACE_ESCORT_2,
    EVENT_ACE_ESCORT_3,
    EVENT_BOMBER_OLD_FRIEND_1,
    EVENT_BOMBER_OLD_FRIEND_2,
    EVENT_SECOND_BASTIA_1,
    EVENT_SECOND_BASTIA_2,
    EVENT_WILD_MINE_CART_1,
    EVENT_WILD_MINE_CART_2,
    EVENT_WILD_MINE_CART_3,
    EVENT_FOOTBOMB_1,
    EVENT_FOOTBOMB_2,
    EVENT_BATTLEWORG_TRIP_1,
};


uint32 const TornadoPathSize = 19;
const G3D::Vector3 TornadoPath[TornadoPathSize] =
{
    { 1005.34f, 3833.63f, 22.0369f, },
    { 1028.67f, 3853.77f, 31.9566f, },
    { 1058.27f, 3859.05f, 31.9566f, },
    { 1089.8f, 3858.78f, 36.4087f,  },
    { 1099.2f, 3820.22f, 30.7438f,  },
    { 1102.56f, 3774.42f, 31.4983f, },
    { 1065.74f, 3760.76f, 47.5232f, },
    { 1016.95f, 3778.4f, 37.0304f,  },
    { 1019.36f, 3812.09f, 31.6335f, },
    { 1032.87f, 3863.57f, 29.1444f, },
    { 1006.83f, 3884.45f, 29.9545f, },
    { 968.738f, 3868.76f, 29.8179f, },
    { 939.323f, 3838.03f, 24.0947f, },
    { 876.827f, 3819.14f, 32.0445f, },
    { 871.681f, 3887.09f, 30.5029f, },
    { 895.26f, 3926.64f, 24.8991f,  },
    { 937.414f, 3940.34f, 30.4263f, },
    { 962.997f, 3923.82f, 27.0686f, },
    { 1011.63f, 3861.53f, 21.6433f, },
};

uint32 const TornadoPathSize2 = 16;
const G3D::Vector3 TornadoPath2[TornadoPathSize2] =
{
    { 991.952f, 3717.09f, 49.8286f, } ,
    { 971.315f, 3629.89f, 48.4463f, } ,
    { 920.88f, 3428.51f, 40.5365f,  } ,
    { 908.201f, 3338.8f, 46.4996f,  } ,
    { 923.23f, 3297.07f, 60.7812f,  } ,
    { 957.667f, 3239.63f, 99.1091f, } ,
    { 980.808f, 3182.79f, 131.938f, } ,
    { 999.814f, 3122.99f, 150.173f, } ,
    { 1006.72f, 3079.52f, 153.136f, } ,
    { 999.487f, 3040.7f, 149.398f,  } ,
    { 963.701f, 3002.99f, 148.363f, } ,
    { 910.52f, 2960.18f, 146.836f,  } ,
    { 897.085f, 2918.85f, 143.468f, } ,
    { 888.763f, 2879.01f, 141.124f, } ,
    { 871.851f, 2802.26f, 130.364f, } ,
    { 872.536f, 2763.4f, 119.279f,  } ,
};

enum Tornado
{
    QUEST_WARCHIEF = 14243,
};

class npc_cyclone_of_the_elements_ship : public CreatureScript
{
public:
    npc_cyclone_of_the_elements_ship() : CreatureScript("npc_cyclone_of_the_elements_ship") { }

    struct npc_cyclone_of_the_elements_shipAI : public VehicleAI
    {
        npc_cyclone_of_the_elements_shipAI(Creature* creature) : VehicleAI(creature) {}

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
                me->GetMotionMaster()->MoveSmoothPath(TornadoPath, TornadoPathSize);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->GetVehicleKit()->EjectPassenger(passenger, EJECT_DIR_RIGHT);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (!_completed && point == 18)
            {
                me->GetMotionMaster()->MoveSmoothPath(TornadoPath, TornadoPathSize);
            }

            if (_completed && point == 15)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->GetVehicleKit()->EjectPassenger(player, EJECT_DIR_RIGHT);
                    me->DespawnOrUnsummon(500);
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                    {
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_WARCHIEF) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_WARCHIEF) == QUEST_STATUS_NONE)
                    {
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_WARCHIEF) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_WARCHIEF) == QUEST_STATUS_NONE)
                    {
                        me->DespawnOrUnsummon();
                    }

                    if (!_completed && player->GetQuestStatus(QUEST_WARCHIEF) == QUEST_STATUS_COMPLETE)
                    {
                        _completed = true;
                        me->GetMotionMaster()->Clear();
                        me->SetSpeed(MOVE_FLIGHT, 3.5f);
                        me->SetSpeed(MOVE_RUN, 3.5f);
                        me->GetMotionMaster()->MoveSmoothPath(TornadoPath2, TornadoPathSize2);
                        me->AI()->Talk(0, _playerGUID);
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        bool _completed = false;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cyclone_of_the_elements_shipAI(creature);
    }
};

enum QuestMinerTroubles
{
    QUEST_MINER_TROUBLES = 14021,

    NPC_FRIGHTENED_MINER_SUMMONED = 35813,
    NPC_MINER_TROUBLES_ORE_CART = 35814,
    NPC_MINER_TROUBLES_CREDIT = 35816,

    SPELL_CHAINS_TO_CART = 68122, // this spell visual chain makes root npc and cant follow...
    SPELL_MINER_TROUBLES_AURA = 68062,

    SPELL_WEED_WHACKER = 68212,
};

// quest=14021/miner-troubles
class npc_miner_and_cart_escort : public CreatureScript
{
public:
    npc_miner_and_cart_escort() : CreatureScript("npc_miner_and_cart_escort") { }

    struct npc_miner_and_cart_escortAI : public npc_escortAI
    {
        npc_miner_and_cart_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(100, me, SPELL_ROOT);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto cart = me->SummonCreature(NPC_MINER_TROUBLES_ORE_CART, 490.37f, 2976.471f, 7.87f, 5.91f, TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                _cartGUID = cart->GetGUID();
                //cart->CastWithDelay(1000, me , SPELL_CHAINS_TO_CART); // this spell visual chain makes root npc and cant follow...
                me->AI()->TalkWithDelay(2000, 0);
                _events.ScheduleEvent(EVENT_MINER_CART_0, 4000);
            }
        }

        void OnEscortAbandonTooFarDespawn()
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                if (auto cart = Creature::GetCreature(*me, _cartGUID))
                {
                    player->RemoveAura(SPELL_MINER_TROUBLES_AURA);
                    cart->DespawnOrUnsummon();
                    me->DespawnOrUnsummon();
                }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 10:
                me->SetReactState(REACT_AGGRESSIVE);
                me->AI()->Talk(1, _playerGUID);
                break;
            case 19:
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                _events.ScheduleEvent(EVENT_MINER_CART_1, 7000);
                break;
            case 20:
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            case 30:
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                _events.ScheduleEvent(EVENT_MINER_CART_2, 7000);
                break;
            case 31:
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            case 39:
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                _events.ScheduleEvent(EVENT_MINER_CART_3, 7000);
                break;
            case 40:
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            case 53:
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->HandleEmoteCommand(EMOTE_STATE_WORK_MINING);
                _events.ScheduleEvent(EVENT_MINER_CART_4, 7000);
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
                case EVENT_MINER_CART_0:
                    me->RemoveAura(SPELL_ROOT);
                    Start(true, false, _playerGUID);

                    if (auto cart = Unit::GetCreature(*me, _cartGUID))
                        cart->GetMotionMaster()->MoveFollow(me, 0.5f, 0.0f * M_PI);
                    break;
                case EVENT_MINER_CART_1:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    me->AI()->TalkWithDelay(1000, 9, _playerGUID);
                    me->AI()->TalkWithDelay(4000, 7, _playerGUID);
                    break;
                case EVENT_MINER_CART_2:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    me->AI()->TalkWithDelay(1000, 4, _playerGUID);
                    me->AI()->TalkWithDelay(4000, 10, _playerGUID);
                    break;
                case EVENT_MINER_CART_3:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    me->AI()->TalkWithDelay(1000, 3, _playerGUID);
                    me->AI()->TalkWithDelay(4000, 11, _playerGUID);
                    break;
                case EVENT_MINER_CART_4:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        me->SetFacingToObject(player);

                    me->AI()->TalkWithDelay(1000, 12, _playerGUID);
                    me->AI()->TalkWithDelay(4000, 6, _playerGUID);
                    _events.ScheduleEvent(EVENT_MINER_CART_5, 4000);
                    break;
                case EVENT_MINER_CART_5:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto cart = Unit::GetCreature(*me, _cartGUID))
                        {
                            player->KilledMonsterCredit(NPC_MINER_TROUBLES_CREDIT);
                            player->RemoveAura(SPELL_MINER_TROUBLES_AURA);
                            cart->DespawnOrUnsummon(5000);
                            me->DespawnOrUnsummon(5000);
                        }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        EventMap _events;
        uint64 _cartGUID = 0;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_miner_and_cart_escortAI(creature);
    }
};

class spell_weed_whacker : public SpellScriptLoader
{
public:
    spell_weed_whacker() : SpellScriptLoader("spell_weed_whacker") { }

    class spell_weed_whacker_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_weed_whacker_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetCastItem())
                if (auto caster = GetCastItem()->GetOwner())
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* player = caster->ToPlayer();
                        if (player->HasAura(SPELL_WEED_WHACKER))
                            player->RemoveAura(SPELL_WEED_WHACKER);
                        else if (player->GetQuestStatus(14236) == QUEST_STATUS_INCOMPLETE)
                            player->CastSpell(player, SPELL_WEED_WHACKER, true);
                    }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_weed_whacker_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_weed_whacker_SpellScript();
    }
};

enum QuestInfrared
{
    QUEST_INFRARED_INFRADEAD = 14238,

    SPELL_ORC_SCOUT_SUMMON_AURA = 68338,

    SPELL_ORC_SCOUT_CHARGE = 7922,
    SPELL_ORC_SCOUT_HEROIC_STRIKE = 25710,
    SPELL_ORC_SCOUT_REND = 11977,
    SPELL_ORC_SCOUT_THUNDERCLAP = 8078,

    SPELL_CAMERA_SHAKE = 45849,
    SPELL_CRASH_EXPLOSION = 71495,
};

class npc_orc_scout_infrared_infradead : public CreatureScript
{
public:
    npc_orc_scout_infrared_infradead() : CreatureScript("npc_orc_scout_infrared_infradead") { }

    struct npc_orc_scout_infrared_infradeadAI : public ScriptedAI
    {
        npc_orc_scout_infrared_infradeadAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
            }

            me->AI()->TalkWithDelay(2000, 0, _playerGUID);
        }

        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_ORC_SCOUT_REND, urand(1000, 14000));
            _events.ScheduleEvent(EVENT_ORC_SCOUT_HEROIC_STRIKE, urand(2000, 9000));
            _events.ScheduleEvent(EVENT_ORC_SCOUT_THUNDERCLAP, urand(9000, 18000));

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (auto enemy = player->getVictim())
                {
                    if (me->GetDistance(enemy) >= 8.f)
                        me->CastSpell(enemy, SPELL_ORC_SCOUT_CHARGE);
                }
            }
        }

        void JustDied(Unit* killer)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                player->RemoveAura(SPELL_ORC_SCOUT_SUMMON_AURA);
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        me->DespawnOrUnsummon();
                        player->RemoveAura(SPELL_ORC_SCOUT_SUMMON_AURA);
                    }

                    if (player->GetQuestStatus(QUEST_INFRARED_INFRADEAD) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_INFRARED_INFRADEAD) == QUEST_STATUS_REWARDED)
                    {
                        me->DespawnOrUnsummon();
                        player->RemoveAura(SPELL_ORC_SCOUT_SUMMON_AURA);
                    }

                    if (!me->getVictim() && player->isInCombat())
                    {
                        if (me->GetReactState() != REACT_AGGRESSIVE)
                            me->SetReactState(REACT_AGGRESSIVE);

                        if (player->getVictim())
                            me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->isInCombat())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 0.7f, 0.4f * M_PI);
                        _events.Reset();
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
                case EVENT_ORC_SCOUT_REND:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ORC_SCOUT_REND);
                    _events.ScheduleEvent(EVENT_ORC_SCOUT_REND, urand(5000, 14000));
                    break;
                case EVENT_ORC_SCOUT_HEROIC_STRIKE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ORC_SCOUT_HEROIC_STRIKE);
                    _events.ScheduleEvent(EVENT_ORC_SCOUT_HEROIC_STRIKE, urand(8000, 18000));
                    break;
                case EVENT_ORC_SCOUT_THUNDERCLAP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ORC_SCOUT_THUNDERCLAP);
                    _events.ScheduleEvent(EVENT_ORC_SCOUT_THUNDERCLAP, urand(12000, 28000));
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
        return new npc_orc_scout_infrared_infradeadAI(creature);
    }
};

uint32 const BastiaPathSize = 31;
const G3D::Vector3 BastiaPath[BastiaPathSize] =
{
    { 861.10f, 2824.70f, 106.63f },
    { 858.30f, 2835.89f, 103.61f },
    { 866.55f, 2843.87f, 102.81f },
    { 867.92f, 2861.88f, 101.27f },
    { 878.13f, 2872.62f, 100.69f },
    { 895.03f, 2875.17f, 99.774f },
    { 915.46f, 2889.66f, 99.996f },
    { 919.59f, 2903.76f, 100.85f },
    { 935.48f, 2912.66f, 103.82f },
    { 942.64f, 2920.07f, 105.82f },
    { 946.20f, 2940.30f, 108.97f },
    { 959.15f, 2947.32f, 109.99f },
    { 997.08f, 2947.70f, 108.77f },
    { 1030.2f, 2949.62f, 109.56f },
    { 1043.3f, 2958.40f, 111.14f },
    { 1047.8f, 2976.46f, 112.43f },
    { 1060.8f, 2985.52f, 114.61f },
    { 1075.6f, 2996.37f, 117.06f },
    { 1073.1f, 3018.00f, 120.64f },
    { 1079.1f, 3029.21f, 122.25f },
    { 1090.2f, 3046.60f, 123.52f },
    { 1078.3f, 3081.64f, 124.81f },
    { 1068.5f, 3103.80f, 125.28f },
    { 1045.7f, 3120.78f, 125.13f },
    { 1025.5f, 3128.08f, 124.99f },
    { 1022.1f, 3154.31f, 122.13f },
    { 1024.6f, 3169.04f, 119.62f },
    { 1042.1f, 3177.34f, 117.58f },
    { 1056.1f, 3195.75f, 113.87f },
    { 1058.1f, 3209.42f, 107.31f },
    { 1059.5f, 3223.76f, 97.778f }, // 30
};

class npc_bastia_trip_vehicle : public CreatureScript
{
public:
    npc_bastia_trip_vehicle() : CreatureScript("npc_bastia_trip_vehicle") { }

    struct npc_bastia_trip_vehicleAI : public VehicleAI
    {
        npc_bastia_trip_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_BASTIA_1, 2500);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 30:
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveJump(1078.16f, 3239.356f, 81.33f, 20.0f, 20.0f);
                _events.ScheduleEvent(EVENT_BASTIA_2, 4000);
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
                case EVENT_BASTIA_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(BastiaPath, BastiaPathSize);
                    break;
                case EVENT_BASTIA_2:
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
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bastia_trip_vehicleAI(creature);
    }
};

uint32 const GyroCargoPathSize = 12;
const G3D::Vector3 GyroCargoPath[GyroCargoPathSize] =
{
    { 843.239f, 3334.04f, 10.08f },
    { 810.679f, 3341.13f, 15.75f },
    { 772.779f, 3362.99f, 24.10f },
    { 769.840f, 3411.37f, 26.54f },
    { 803.242f, 3466.73f, 25.30f },
    { 910.527f, 3527.75f, 23.91f },
    { 1018.23f, 3558.25f, 20.39f },
    { 1150.64f, 3616.52f, 20.82f },
    { 1201.92f, 3752.62f, 51.84f },
    { 1118.19f, 3780.97f, 64.00f },
    { 1007.17f, 3789.32f, 34.78f },
    { 971.682f, 3803.31f, 14.74f }, // 11
};

class npc_gyro_precious_cargo_vehicle : public CreatureScript
{
public:
    npc_gyro_precious_cargo_vehicle() : CreatureScript("npc_gyro_precious_cargo_vehicle") { }

    struct npc_gyro_precious_cargo_vehicleAI : public VehicleAI
    {
        npc_gyro_precious_cargo_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
            summoner->RemoveAura(49416);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_GYRO_CARGO_1, 1500);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->DespawnOrUnsummon(100);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 11:
                me->GetMotionMaster()->Clear();

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->CastSpell(me, SPELL_CAMERA_SHAKE);
                    me->CastSpell(me, SPELL_CRASH_EXPLOSION);
                    me->AI()->Talk(0, _playerGUID);
                }

                _events.ScheduleEvent(EVENT_GYRO_CARGO_2, 500);
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
                case EVENT_GYRO_CARGO_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(GyroCargoPath, GyroCargoPathSize);
                    break;
                case EVENT_GYRO_CARGO_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
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
        return new npc_gyro_precious_cargo_vehicleAI(creature);
    }
};

uint32 const GyroWarchiefPathSize = 19;
const G3D::Vector3 GyroWarchiefPath[GyroWarchiefPathSize] =
{
    { 1016.68f, 3835.6f, 28.161f },
    { 1028.67f, 3853.77f, 29.0f },
    { 1058.27f, 3859.05f, 29.0f },
    { 1089.80f, 3858.78f, 29.0f },
    { 1099.20f, 3820.22f, 29.0f },
    { 1102.56f, 3774.42f, 29.0f },
    { 1065.74f, 3760.76f, 29.0f },
    { 1016.95f, 3778.40f, 29.0f },
    { 1019.36f, 3812.09f, 29.0f },
    { 1032.87f, 3863.57f, 29.0f },
    { 1006.83f, 3884.45f, 29.0f },
    { 968.738f, 3868.76f, 29.0f },
    { 939.323f, 3838.03f, 29.0f },
    { 876.827f, 3819.14f, 29.0f },
    { 871.681f, 3887.09f, 29.0f },
    { 895.260f, 3926.64f, 29.0f },
    { 937.414f, 3940.34f, 29.0f },
    { 962.997f, 3923.82f, 29.0f },
    { 1011.63f, 3861.53f, 29.0f }, // 18
};

uint32 const GyroWarchiefPathSize2 = 16;
const G3D::Vector3 GyroWarchiefPath2[GyroWarchiefPathSize2] =
{
    { 991.952f, 3717.09f, 49.828f },
    { 971.315f, 3629.89f, 48.446f },
    { 920.880f, 3428.51f, 40.536f },
    { 908.201f, 3338.80f, 46.499f },
    { 923.230f, 3297.07f, 60.781f },
    { 957.667f, 3239.63f, 99.109f },
    { 980.808f, 3182.79f, 131.93f },
    { 999.814f, 3122.99f, 150.17f },
    { 1006.72f, 3079.52f, 153.13f },
    { 999.487f, 3040.70f, 149.39f },
    { 963.701f, 3002.99f, 148.36f },
    { 910.520f, 2960.18f, 146.83f },
    { 897.085f, 2918.85f, 143.46f },
    { 888.763f, 2879.01f, 141.12f },
    { 871.851f, 2802.26f, 130.36f },
    { 872.536f, 2763.40f, 119.27f }, // 15
};

enum QuestWarchiefsRevenge
{
    SPELL_BACK_TO_COAST_AURA = 68409,
    SPELL_WARCHIEF_REVENGE_ON_GOAL_PING = 68439,
    SPELL_WARCHIEF_REVENGE_ON_ABANDON = 68438,

    QUEST_WARCHIEF_REVENGE = 14243,
};

class npc_tornado_warchiefs_revange_vehicle : public CreatureScript
{
public:
    npc_tornado_warchiefs_revange_vehicle() : CreatureScript("npc_tornado_warchiefs_revange_vehicle") { }

    struct npc_tornado_warchiefs_revange_vehicleAI : public VehicleAI
    {
        npc_tornado_warchiefs_revange_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_GYRO_WARCHIEF_1, 500);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->DespawnOrUnsummon(100);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_pathMode == 1)
            {
                if (point == 18)
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(GyroWarchiefPath, GyroWarchiefPathSize);
                }
            }

            if (_pathMode == 2)
            {
                if (point == 15)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (player->GetQuestStatus(QUEST_WARCHIEF_REVENGE) == QUEST_STATUS_NONE)
                {
                    player->ExitVehicle();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(100);
                }

                if (!_isQuestCompleted)
                {
                    if (player->GetQuestStatus(QUEST_WARCHIEF_REVENGE) == QUEST_STATUS_COMPLETE)
                    {
                        _isQuestCompleted = true;
                        me->GetMotionMaster()->Clear();
                        me->CastSpell(me, SPELL_FLIGHT_SPEED_100);
                        me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                        me->GetMotionMaster()->MoveSmoothPath(GyroWarchiefPath2, GyroWarchiefPathSize2);
                        _pathMode++;
                    }
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GYRO_WARCHIEF_1:
                    _pathMode = 1;
                    me->GetMotionMaster()->MoveSmoothPath(GyroWarchiefPath, GyroWarchiefPathSize);
                    break;
                case EVENT_GYRO_WARCHIEF_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isQuestCompleted = false;
        uint8 _pathMode = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tornado_warchiefs_revange_vehicleAI(creature);
    }
};

uint32 const SlingRocketPathSize = 9;
const G3D::Vector3 SlingRocketPath[SlingRocketPathSize] =
{
    { 887.5f, 2707.27f, 152.0f },
    { 891.9f, 2689.06f, 172.0f },
    { 898.9f, 2655.59f, 197.0f },
    { 911.2f, 2599.17f, 213.0f },
    { 919.8f, 2555.37f, 188.0f },
    { 930.6f, 2502.59f, 133.0f },
    { 938.0f, 2454.35f, 84.0f  },
    { 942.7f, 2417.70f, 33.0f  },
    { 945.5f, 2396.28f, 4.0f   }, // 8
};

enum QuestUpUpAway
{
    SPELL_UP_UP_CREDIT_EXPLOSION = 66127,
    NPC_WONDI_TARGET = 75042,
};

class npc_sling_rocket : public CreatureScript
{
public:
    npc_sling_rocket() : CreatureScript("npc_sling_rocket") { }

    struct npc_sling_rocketAI : public VehicleAI
    {
        npc_sling_rocketAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_SLING_ROCKET_1, 600);

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
                me->GetMotionMaster()->Clear();

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    player->ExitVehicle();
                    player->KilledMonsterCredit(50046);
                    me->DespawnOrUnsummon(1000);

                    if (auto target = me->FindNearestCreature(NPC_WONDI_TARGET, 10.0f))
                        target->CastSpell(target, SPELL_UP_UP_CREDIT_EXPLOSION);
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
                case EVENT_SLING_ROCKET_1:
                    me->SetRooted(false);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_100);
                    me->GetMotionMaster()->MoveSmoothPath(SlingRocketPath, SlingRocketPathSize);
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
        return new npc_sling_rocketAI(creature);
    }
};

enum QuestTradingUp
{
    NPC_SPINY_RAPTOR = 38187,
    NPC_TRAP_SUMMONER = 75113,
    OBJECT_TRAP = 202472,

    SPELL_SUMMON_SATISFY_OBJECT = 66726,
    SPELL_LOOK_LIKE_AN_EGG = 71355,
};

class npc_wild_clucker_egg_trading_up : public CreatureScript
{
public:
    npc_wild_clucker_egg_trading_up() : CreatureScript("npc_wild_clucker_egg_trading_up") { }

    struct npc_wild_clucker_egg_trading_upAI : public ScriptedAI
    {
        npc_wild_clucker_egg_trading_upAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastSpell(me, SPELL_ROOT);
            me->CastSpell(me, SPELL_LOOK_LIKE_AN_EGG);
            _events.ScheduleEvent(EVENT_TRADING_EGG_1, 500);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void UpdateAI(uint32 const diff)
        {
            if (_raptorFound)
            {
                if (auto raptor = Unit::GetCreature(*me, _raptorGUID))
                {
                    if (!raptor->isAlive())
                    {
                        _raptorFound = false;
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetDistance(raptor) <= 1.f)
                    {
                        if (auto raptor = Unit::GetCreature(*me, _raptorGUID))
                        {
                            if (auto trapsummoner = me->FindNearestCreature(NPC_TRAP_SUMMONER, 5.0f))
                            {
                                trapsummoner->CastSpell(trapsummoner, SPELL_DUMMY_PING);
                                me->Kill(raptor);
                                me->CastSpell(me, SPELL_SUMMON_SATISFY_OBJECT);
                                me->DespawnOrUnsummon(100);
                                _raptorFound = false;
                            }
                        }
                    }
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TRADING_EGG_1:
                    if (auto raptor = me->FindNearestCreature(NPC_SPINY_RAPTOR, 40.0f))
                    {
                        _raptorFound = true;
                        _raptorGUID = raptor->GetGUID();
                        raptor->CastSpell(raptor, SPELL_SANCTUARY_NO_COMBAT);
                        raptor->SetReactState(REACT_PASSIVE);
                        raptor->GetMotionMaster()->Clear();
                        raptor->SetWalk(false);
                        raptor->GetMotionMaster()->MovePoint(2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    }
                    else
                    {
                        me->AI()->Talk(0, _playerGUID);
                        me->DespawnOrUnsummon(5000);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _raptorFound = false;
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _raptorGUID = 0;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_clucker_egg_trading_upAI(creature);
    }
};

enum QuestSurrenderOrElse
{
    QUEST_SURRENDER_OR_ELSE = 24868,

    NPC_SUMMONED_NAGA_HATCHLINGS = 38457,
    NPC_MEGS_DREADSHREDDER = 38432,
    NPC_FACELESS_OF_THE_DEEP = 38448,
    NPC_FACELESS_VISUAL_VOID = 38450,

    SPELL_SURRENDER_OR_ELSE_SUMMONS_MASTER = 72086,
    SPELL_SURRENDER_OR_ELSE_SUMMON_ACE = 72058,
    SPELL_SURRENDER_OR_ELSE_SUMMON_NAGA_HATCHLINGS = 72073,
    SPELL_SURRENDER_OR_ELSE_KILL_CREDIT = 72035,
    SPELL_SURRENDER_OR_ELSE_FACELESS_OF_THE_DEEP_BEAM_EFFECT = 72076,
    SPELL_SURRENDER_OR_ELSE_GOBLIN_BANNER_AURA = 72077,
    SPELL_SURRENDER_OR_ELSE_DESPAWN_SUMMONS = 72085,
};

const Position AceSummonNagasPosition[] =
{
    { 684.5736f, 2027.0081f, 51.280f, 3.1f },
    { 684.7362f, 2024.1544f, 50.361f, 2.7f },
    { 682.4774f, 2021.5471f, 49.281f, 2.4f },
    { 678.2301f, 2021.4848f, 48.525f, 1.3f },
};

// quest=24868/surrender-or-else
class npc_ace_surrender_escort : public CreatureScript
{
public:
    npc_ace_surrender_escort() : CreatureScript("npc_ace_surrender_escort") { }

    struct npc_ace_surrender_escortAI : public npc_escortAI
    {
        npc_ace_surrender_escortAI(Creature* creature) : npc_escortAI(creature), _summons(me) { }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(100, me, SPELL_ROOT);
            summoner->RemoveAura(49416); // standalone ace invis detection

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto naga1 = me->SummonCreature(NPC_SUMMONED_NAGA_HATCHLINGS, AceSummonNagasPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                if (auto naga2 = me->SummonCreature(NPC_SUMMONED_NAGA_HATCHLINGS, AceSummonNagasPosition[1], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    if (auto naga3 = me->SummonCreature(NPC_SUMMONED_NAGA_HATCHLINGS, AceSummonNagasPosition[2], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto naga4 = me->SummonCreature(NPC_SUMMONED_NAGA_HATCHLINGS, AceSummonNagasPosition[3], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _summons.Summon(naga1);
                            _summons.Summon(naga2);
                            _summons.Summon(naga3);
                            _summons.Summon(naga4);
                            naga1->GetMotionMaster()->MoveFollow(me, 0.8f, 0.5f * M_PI);
                            naga2->GetMotionMaster()->MoveFollow(me, 0.8f, 0.8f * M_PI);
                            naga3->GetMotionMaster()->MoveFollow(me, 0.8f, 1.1f * M_PI);
                            naga4->GetMotionMaster()->MoveFollow(me, 0.8f, 1.4f * M_PI);
                            naga1->SetReactState(REACT_PASSIVE);
                            naga2->SetReactState(REACT_PASSIVE);
                            naga3->SetReactState(REACT_PASSIVE);
                            naga4->SetReactState(REACT_PASSIVE);
                        }

            _events.ScheduleEvent(EVENT_ACE_ESCORT_1, 1000);
        }

        void EscortAbandonTooFarDespawn()
        {
            _summons.DespawnAll();
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 13:
                me->AI()->Talk(2, _playerGUID);
                break;
            case 20:
                me->AI()->Talk(3, _playerGUID);
                break;
            case 35:
                if (me->FindNearestCreature(NPC_FACELESS_OF_THE_DEEP, 80.0f))
                {
                    me->AI()->Talk(6, _playerGUID);
                    _events.ScheduleEvent(EVENT_ACE_ESCORT_3, 3000);
                }
                else
                {
                    me->CastSpell(me, SPELL_CAMERA_SHAKE);
                    me->AI()->Talk(4, _playerGUID);
                    me->SummonCreature(NPC_FACELESS_OF_THE_DEEP, 139.27f, 1933.99f, 7.181f, 0.189f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    me->SummonCreature(NPC_FACELESS_VISUAL_VOID, 139.27f, 1933.99f, 7.181f, 0.189f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 12000);

                    me->AI()->TalkWithDelay(23000, 6, _playerGUID);
                    _events.ScheduleEvent(EVENT_ACE_ESCORT_3, 25000);
                }
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

            if (!_isQuestCompleted)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_SURRENDER_OR_ELSE) == QUEST_STATUS_COMPLETE)
                    {
                        _isQuestCompleted = true;
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(100);
                    }

                    if (player->GetQuestStatus(QUEST_SURRENDER_OR_ELSE) == QUEST_STATUS_NONE)
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(100);
                    }
                }
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ACE_ESCORT_1:
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                    me->AI()->TalkWithDelay(4000, 1, _playerGUID);

                    if (auto megs = me->FindNearestCreature(NPC_MEGS_DREADSHREDDER, 10.0f))
                        me->SetFacingToObject(megs);

                    _events.ScheduleEvent(EVENT_ACE_ESCORT_2, 6000);
                    break;
                case EVENT_ACE_ESCORT_2:
                    me->RemoveAura(SPELL_ROOT);
                    Start(false, true, _playerGUID);
                    break;
                case EVENT_ACE_ESCORT_3:
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(1, 215.52f, 1945.00f, 0.26f);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _isQuestCompleted = false;
        EventMap _events;
        SummonList _summons;
        uint64 _cartGUID = 0;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ace_surrender_escortAI(creature);
    }
};

enum QuestEliminator
{
    QUEST_TOWN_IN_A_BOX_UNDER_ATTACK = 24901,

    NPC_OOMLOT_WARRIOR = 75116,

};

const Position WarriorsSumPosition[] =
{
    { 912.748f, 2179.97f, 57.46f, 1.2f },
    { 903.941f, 2189.92f, 51.84f, 1.2f },
    { 897.093f, 2196.67f, 50.12f, 1.2f },
    { 907.892f, 2202.30f, 43.60f, 1.2f },
    { 914.377f, 2217.78f, 33.41f, 1.2f },
    { 904.475f, 2214.55f, 37.13f, 1.2f },
};

class npc_bc_eliminator : public CreatureScript
{
public:
    npc_bc_eliminator() : CreatureScript("npc_bc_eliminator") { }

    struct npc_bc_eliminatorAI : public VehicleAI
    {
        npc_bc_eliminatorAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void InitializeAI()
        {
            me->setActive(true);
            me->CastWithDelay(100, me, SPELL_ROOT);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = true;
                me->CastWithDelay(100, me, SPELL_ROOT);

                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _summons.DespawnAll();
                _onUse = false;
                _playerGUID = 0;
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;
        }

        void UpdateAI(uint32 const diff)  override
        {
            if (_onUse)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_TOWN_IN_A_BOX_UNDER_ATTACK) == QUEST_STATUS_NONE)
                    {
                        _summons.DespawnAll();
                        player->ExitVehicle();
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _onUse = false;
                    }

                    if (player->GetQuestStatus(QUEST_TOWN_IN_A_BOX_UNDER_ATTACK) == QUEST_STATUS_COMPLETE)
                    {
                        _summons.DespawnAll();
                        player->ExitVehicle();
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _onUse = false;
                    }

                    if (_checkTimer <= diff)
                    {
                        _checkTimer = urand(500, 2000);

                        if (auto warrior1 = me->SummonCreature(NPC_OOMLOT_WARRIOR, WarriorsSumPosition[urand(0, 5)], TEMPSUMMON_TIMED_DESPAWN, 90000))
                        {
                            warrior1->setActive(true);
                            _summons.Summon(warrior1);
                        }
                    }
                    else
                        _checkTimer -= diff;
                }
            }
        }

    private:
        bool _onUse = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bc_eliminatorAI(creature);
    }
};

enum QuestZombieKillBoots
{
    QUEST_ZOMBIE = 24942,
    NPC_CROSSCHECK = 38738,
    SPELL_ZOMBIE_BURNING_AURA = 72885,
};

class npc_super_booster_rocket_boots : public CreatureScript
{
public:
    npc_super_booster_rocket_boots() : CreatureScript("npc_super_booster_rocket_boots") { }

    struct npc_super_booster_rocket_bootsAI : public VehicleAI
    {
        npc_super_booster_rocket_bootsAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            _onUse = true;

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                passenger->RemoveAura(SPELL_ZOMBIE_BURNING_AURA);
        }

        void UpdateAI(uint32 const diff)
        {
            if (_onUse)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetDistance(player) >= 20.f)
                    {
                        _onUse = false;
                        player->RemoveAura(SPELL_ZOMBIE_BURNING_AURA);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(100);
                    }

                    if (player->GetQuestStatus(QUEST_ZOMBIE) == QUEST_STATE_COMPLETE)
                    {
                        if (me->FindNearestCreature(NPC_CROSSCHECK, 7.0f))
                        {
                            _onUse = false;
                            player->RemoveAura(SPELL_ZOMBIE_BURNING_AURA);
                            player->ExitVehicle();
                            me->DespawnOrUnsummon(100);
                        }
                    }
                }
            }
        }

    private:
        bool _onUse = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_super_booster_rocket_bootsAI(creature);
    }
};

uint32 const OldFriendsPathSize = 32;
const G3D::Vector3 OldFriendsPath[OldFriendsPathSize] =
{
    { 1161.62f, 1091.40f, 131.98f },
    { 1140.88f, 1126.67f, 161.32f },
    { 1121.84f, 1146.31f, 161.46f },
    { 1083.85f, 1167.90f, 155.89f },
    { 1057.32f, 1184.25f, 150.56f },
    { 1029.45f, 1207.69f, 143.07f },
    { 1005.11f, 1239.05f, 132.34f },
    { 992.114f, 1286.59f, 118.70f },
    { 1003.66f, 1341.23f, 108.43f },
    { 1017.20f, 1385.50f, 102.72f },
    { 1026.98f, 1438.47f, 109.38f },
    { 1021.24f, 1498.46f, 124.74f },
    { 996.677f, 1537.77f, 141.29f },
    { 967.980f, 1558.59f, 152.88f },
    { 933.495f, 1576.55f, 167.29f },
    { 906.417f, 1583.72f, 173.14f },
    { 858.140f, 1589.72f, 179.02f },
    { 810.699f, 1587.94f, 171.76f },
    { 785.992f, 1591.09f, 166.97f },
    { 753.694f, 1595.30f, 157.78f },
    { 736.484f, 1624.25f, 145.67f },
    { 733.635f, 1648.79f, 135.47f },
    { 730.551f, 1683.62f, 128.90f },
    { 723.960f, 1749.11f, 124.60f },
    { 715.259f, 1774.61f, 124.20f },
    { 763.820f, 1882.63f, 124.02f },
    { 785.018f, 1976.12f, 117.23f },
    { 844.826f, 2105.08f, 104.17f },
    { 895.777f, 2161.22f, 91.028f },
    { 917.274f, 2289.11f, 47.144f },
    { 935.914f, 2365.25f, 42.937f },
    { 942.882f, 2464.44f, 40.527f }, // 31
};

uint32 const OldFriendsPathSize2 = 16;
const G3D::Vector3 OldFriendsPath2[OldFriendsPathSize2] =
{
    { 942.882f, 2464.44f, 40.527f },
    { 944.167f, 2503.76f, 38.623f },
    { 858.936f, 2565.72f, 37.654f },
    { 783.400f, 2554.27f, 37.247f },
    { 722.149f, 2496.84f, 44.193f },
    { 769.014f, 2408.59f, 59.973f },
    { 844.295f, 2359.58f, 66.246f },
    { 1040.56f, 2277.87f, 75.403f },
    { 1172.10f, 2245.09f, 92.272f },
    { 1229.56f, 2231.58f, 100.03f },
    { 1326.57f, 2284.36f, 108.67f },
    { 1400.32f, 2356.59f, 114.23f },
    { 1475.96f, 2446.52f, 120.97f },
    { 1533.96f, 2552.74f, 123.13f },
    { 1569.66f, 2629.42f, 118.22f },
    { 1583.08f, 2692.19f, 89.804f }, // 15
};

enum QuestOldFriends
{
    QUEST_OLD_FRIENDS = 25023,

    NPC_BOMBER_OLD_FRIENDS = 38918,
    NPC_SASSY_HARDWRENCH_2 = 38869,

    SPELL_BOMBER_ROPE_LADDER = 73149,
    SPELL_SUMMON_EXPLODES = 73127,
    SPELL_BOMBER_RIDE_AURA = 73137,
    SPELL_RIDE_SEAT_3 = 98707, // seat 3-1 = 2 
    SPELL_RIDE_SEAT_1 = 99372, // seat 1-1 = 0 

};

class npc_flying_bomber_old_friends : public CreatureScript
{
public:
    npc_flying_bomber_old_friends() : CreatureScript("npc_flying_bomber_old_friends") { }

    struct npc_flying_bomber_old_friendsAI : public VehicleAI
    {
        npc_flying_bomber_old_friendsAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_BOMBER_ROPE_LADDER);
            me->SetSpeed(MOVE_RUN, 4.0f);
            _pathMode = false;


            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            if (auto sassy = me->SummonCreature(NPC_SASSY_HARDWRENCH_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                _summons.Summon(sassy);
                _sassyGUID = sassy->GetGUID();
                sassy->CastWithDelay(200, me, 46598);
                sassy->AI()->TalkWithDelay(1000, 0, _playerGUID);
                sassy->AI()->TalkWithDelay(6000, 1, _playerGUID);
                sassy->AI()->TalkWithDelay(9000, 2, _playerGUID);
                sassy->AI()->TalkWithDelay(22000, 3, _playerGUID);
                me->CastWithDelay(7000, me, SPELL_SUMMON_EXPLODES);
                summoner->RemoveAura(49416);

                _events.ScheduleEvent(EVENT_BOMBER_OLD_FRIEND_1, 1000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (me->GetDistance(player) >= 120.f)
                {
                    _summons.DespawnAll();
                    player->RemoveAura(SPELL_BOMBER_RIDE_AURA);
                    player->ExitVehicle();
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon();
                }
            }

            if (auto sassy = Unit::GetCreature(*me, _sassyGUID))
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!_pathMode)
                    {
                        switch (point)
                        {
                        case 3:
                            player->RemoveAura(49416);
                            break;
                        case 25:
                            sassy->AI()->TalkWithDelay(1000, 4, _playerGUID);
                            break;
                        case 28:
                            sassy->AI()->TalkWithDelay(1000, 7, _playerGUID);
                            break;
                        case 31:
                            _disableDespawn = true;
                            sassy->AI()->Talk(5, _playerGUID);
                            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                            player->CastSpell(me, SPELL_RIDE_SEAT_3);
                            _summons.Despawn(sassy);
                            _sassyGUID = 0;
                            me->RemoveAura(SPELL_BOMBER_ROPE_LADDER);

                            if (auto sassy = me->SummonCreature(NPC_SASSY_HARDWRENCH_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                            {
                                _summons.Summon(sassy);
                                _sassyGUID = sassy->GetGUID();
                                sassy->CastWithDelay(100, me, 46598);
                            }

                            _events.ScheduleEvent(EVENT_BOMBER_OLD_FRIEND_2, 1500);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                    {
                        switch (point)
                        {
                        case 9:
                            sassy->AI()->TalkWithDelay(1000, 6, _playerGUID);
                            break;
                        case 15:
                            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                            player->RemoveAura(SPELL_BOMBER_RIDE_AURA);
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon(1000);
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
                case EVENT_BOMBER_OLD_FRIEND_1:
                    me->GetMotionMaster()->MoveSmoothPath(OldFriendsPath, OldFriendsPathSize);
                    break;
                case EVENT_BOMBER_OLD_FRIEND_2:
                    me->GetMotionMaster()->MoveSmoothPath(OldFriendsPath2, OldFriendsPathSize2);
                    _pathMode = true;
                    _disableDespawn = false;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _disableDespawn = false;
        bool _pathMode = false;
        uint64 _sassyGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flying_bomber_old_friendsAI(creature);
    }
};

// quest=13510/timely-arrival
class npc_sassy_quest_giver_volcanoth : public CreatureScript
{
public:
    npc_sassy_quest_giver_volcanoth() : CreatureScript("npc_sassy_quest_giver_volcanoth") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_OLD_FRIENDS)
        {
            player->RemoveAura(49416);
            player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);

            if (auto bomber = player->SummonCreature(NPC_BOMBER_OLD_FRIENDS, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                player->CastSpell(bomber, SPELL_BOMBER_RIDE_AURA);
            }
        }

        return true;
    }

    struct npc_sassy_quest_giver_volcanothAI : public ScriptedAI
    {
        npc_sassy_quest_giver_volcanothAI(Creature* creature) : ScriptedAI(creature) { }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = me->FindNearestPlayer(200.0f))
                {
                    me->AI()->Talk(0, player->GetGUID());
                }
            }
        }

    private:
        bool _isSafe = false;
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sassy_quest_giver_volcanothAI(creature);
    }
};

uint32 const SecondBastiaPathSizeSize = 42;
const G3D::Vector3 SecondBastiaPath[SecondBastiaPathSizeSize] =
{
    { 1714.18f, 2849.06f, 14.8606f, },
    { 1718.428f, 2839.144f, 19.16f, },
    { 1716.049f, 2822.322f, 29.00f },
    { 1714.292f, 2805.66f, 34.546f, },
    { 1696.967f, 2775.845f, 45.92f, },
    { 1671.3f, 2759.96f, 62.0572f,  },
    { 1656.83f, 2748.15f, 70.7494f, },
    { 1638.9f, 2736.74f, 76.9307f,  },
    { 1620.21f, 2723.36f, 81.3722f, },
    { 1593.85f, 2717.39f, 83.2812f, },
    { 1571.31f, 2713.33f, 84.445f,  },
    { 1567.41f, 2704.22f, 86.5833f, },
    { 1562.53f, 2683.54f, 91.4772f, },
    { 1565.9f, 2666.91f, 94.3245f,  },
    { 1582.4f, 2653.23f, 96.8425f,  },
    { 1613.45f, 2638.93f, 97.9436f, },
    { 1676.03f, 2614.49f, 96.2468f, },
    { 1708.33f, 2587.47f, 97.6503f, },
    { 1718.07f, 2573.23f, 101.262f, },
    { 1735.43f, 2563.94f, 102.468f, },
    { 1752.7f, 2551.5f, 101.515f, },
    { 1753.32f, 2538.87f, 104.689f, },
    { 1746.72f, 2513.26f, 117.521f, },
    { 1752.16f, 2507.49f, 120.716f, },
    { 1771.75f, 2504.89f, 128.886f, },
    { 1780.95f, 2496.05f, 136.34f,  },
    { 1784.54f, 2477.97f, 144.515f, },
    { 1786.73f, 2463.85f, 147.214f, },
    { 1776.49f, 2438.82f, 151.912f, },
    { 1774.27f, 2434.41f, 152.443f, },
    { 1768.78f, 2424.26f, 147.447f, },
    { 1761.74f, 2411.84f, 144.935f, },
    { 1753.29f, 2394.9f, 147.186f,  },
    { 1746.12f, 2380.83f, 155.252f, },
    { 1742.91f, 2374.61f, 159.723f, },
    { 1736.67f, 2361.77f, 165.572f, },
    { 1734.4f, 2347.06f, 172.116f,  },
    { 1740.72f, 2342.79f, 173.34f,  },
    { 1743.96f, 2338.04f, 178.013f, },
    { 1747.6f, 2327.54f, 184.882f,  },
    { 1747.43f, 2316.07f, 187.805f, },
    { 1750.56f, 2313.03f, 186.861f, }, // 41
};

uint32 const SecondBastiaPathSizeSize2 = 22;
const G3D::Vector3 SecondBastiaPath2[SecondBastiaPathSizeSize2] =
{
    { 1776.43f, 2285.24f, 188.987f, },
    { 1786.98f, 2278.15f, 189.079f, },
    { 1788.69f, 2273.73f, 190.114f, },
    { 1799.24f, 2271.12f, 186.654f, },
    { 1806.02f, 2267.88f, 179.17f,  },
    { 1813.18f, 2260.99f, 180.48f,  },
    { 1821.56f, 2253.59f, 180.713f, },
    { 1822.09f, 2243.75f, 180.49f,  },
    { 1829.35f, 2216.56f, 183.696f, },
    { 1841.69f, 2190.7f, 181.517f,  },
    { 1852.2f, 2163.93f, 182.418f,  },
    { 1868.71f, 2144.15f, 183.319f, },
    { 1880.96f, 2126.54f, 185.168f, },
    { 1895.17f, 2106.89f, 185.292f, },
    { 1900.1f, 2083.26f, 186.287f,  },
    { 1897.83f, 2073.46f, 185.847f, },
    { 1891.74f, 2053.04f, 190.319f, },
    { 1883.71f, 2016.99f, 202.638f, },
    { 1874.55f, 1996.08f, 211.534f, },
    { 1865.47f, 1983.31f, 217.868f, },
    { 1854.61f, 1976.29f, 221.325f, },
    { 1832.92f, 1960.68f, 220.603f, }, // 21
};

class npc_bastia_second_trip_vehicle : public CreatureScript
{
public:
    npc_bastia_second_trip_vehicle() : CreatureScript("npc_bastia_second_trip_vehicle") { }

    struct npc_bastia_second_trip_vehicleAI : public VehicleAI
    {
        npc_bastia_second_trip_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_SECOND_BASTIA_1, 1500);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_firstPath)
            {
                switch (point)
                {
                case 41:
                    me->GetMotionMaster()->MoveJump(1770.78f, 2293.47f, 186.918f, 20.0f, 15.0f);
                    _events.ScheduleEvent(EVENT_SECOND_BASTIA_2, 2000);
                    break;
                default:
                    break;
                }
            }
            else
            {
                switch (point)
                {
                case 21:
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon();
                    break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (me->GetDistance(player) >= 120.f)
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon();
                }
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SECOND_BASTIA_1:
                    me->SetRooted(false);
                    _firstPath = true;
                    me->GetMotionMaster()->MoveSmoothPath(SecondBastiaPath, SecondBastiaPathSizeSize);
                    break;
                case EVENT_SECOND_BASTIA_2:
                    _firstPath = false;
                    me->GetMotionMaster()->MoveSmoothPath(SecondBastiaPath2, SecondBastiaPathSizeSize2);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _firstPath = true;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bastia_second_trip_vehicleAI(creature);
    }
};

enum QuestKajacola
{
    QUEST_MORALE_BOOST = 25122,
    QUEST_WILD_CART = 25184,
    QUEST_KAJACOLA = 25110,

    NPC_SUMMONED_WILD_CART = 39329,

    SPELL_KAJACOLA_DUMMY_PING = 73631,
    SPELL_THROW_DUMMY_PING = 73711,
    SPELL_IDEA_VISUAL = 73605,

    SPELL_SUMMON_GREEDY = 73648,
    SPELL_AURA_GREEDY = 73616,

    SPELL_PLAYER_TO_HIS_ACTORS = 151154,
    SPELL_PLAYER_TO_HIS_ACTORS_DESPAWN = 151143,
};

class npc_assistant_greedy_kajacola : public CreatureScript
{
public:
    npc_assistant_greedy_kajacola() : CreatureScript("npc_assistant_greedy_kajacola") { }

    struct npc_assistant_greedy_kajacolaAI : public ScriptedAI
    {
        npc_assistant_greedy_kajacolaAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_KAJACOLA_DUMMY_PING)
            {
                me->CastSpell(me, SPELL_IDEA_VISUAL);
                me->AI()->TalkWithDelay(1000, 0, _playerGUID);
            }

            if (spellInfo->Id == SPELL_THROW_DUMMY_PING)
            {
                me->AI()->TalkWithDelay(500, 2, _playerGUID);
            }

            if (spellInfo->Id == SPELL_PLAYER_TO_HIS_ACTORS)
            {
                if (me->GetOwnerGUID() == who->GetGUID())
                {
                    std::list<Unit*> cart = me->SelectNearbyUnits(NPC_SUMMONED_WILD_CART, 30.0f);
                    for (std::list<Unit*>::iterator itr = cart.begin(); itr != cart.end(); ++itr)
                        if (auto cart = (*itr))
                        {
                            if (cart->GetOwnerGUID() == me->GetOwnerGUID())
                                me->EnterVehicle(cart, 1);
                        }
                }
            }

            if (spellInfo->Id == SPELL_PLAYER_TO_HIS_ACTORS_DESPAWN)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->RemoveAura(SPELL_AURA_GREEDY);
                    me->DespawnOrUnsummon();
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        player->RemoveAura(SPELL_AURA_GREEDY);
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_REWARDED ||
                        player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_COMPLETE)
                    {
                        player->RemoveAura(SPELL_AURA_GREEDY);
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetDistance(player) >= 120.f)
                    {
                        player->RemoveAura(SPELL_AURA_GREEDY);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->isInCombat())
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 3.0f * M_PI);
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);


            //while (uint32 eventId = _events.ExecuteEvent())
            //{
            //    switch (eventId)
            //    {
            //    case EVENT_ORC_SCOUT_REND:
            //        if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ORC_SCOUT_REND);
            //        _events.ScheduleEvent(EVENT_ORC_SCOUT_REND, urand(5000, 14000));
            //        break;
            //    default:
            //        break;
            //    }
            //}
        }
    private:
        uint64 _cartGUID = 0;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_assistant_greedy_kajacolaAI(creature);
    }
};

enum QuestMortal
{
    NPC_SUMMONED_ACE = 39198,
    NPC_SUMMONED_IZZY = 39200,
    NPC_SUMMONED_GOBBER = 39201,

    NPC_SUMMONED_ACE_2 = 39203,
    NPC_SUMMONED_IZZY_2 = 39204,
    NPC_SUMMONED_GOBBER_2 = 39205,

    SPELL_KAJACOLA_ZERO = 73599,

    SPELL_RESUMMON_ACE = 73608,
    SPELL_RESUMMON_IZZY = 73610,
    SPELL_RESUMMON_GOBBER = 73612,

    SPELL_AURA_ACE = 73633,
    SPELL_AURA_IZZY = 73635,
    SPELL_AURA_GOBBER = 73637,
};

class npc_summoned_izzy_gobber_ace : public CreatureScript
{
public:
    npc_summoned_izzy_gobber_ace() : CreatureScript("npc_summoned_izzy_gobber_ace") { }

    struct npc_summoned_izzy_gobber_aceAI : public ScriptedAI
    {
        npc_summoned_izzy_gobber_aceAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(100, me, SPELL_KAJACOLA_ZERO);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();

                if (me->GetEntry() == NPC_SUMMONED_ACE)
                {
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                    player->KilledMonsterCredit(38441);
                    player->RemoveAura(49416);
                }

                if (me->GetEntry() == NPC_SUMMONED_IZZY)
                {
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                    player->KilledMonsterCredit(38647);
                    player->RemoveAura(49417);
                }

                if (me->GetEntry() == NPC_SUMMONED_GOBBER)
                {
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                    player->KilledMonsterCredit(38746);
                    player->RemoveAura(60922);
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PLAYER_TO_HIS_ACTORS)
            {
                if (me->GetOwnerGUID() == who->GetGUID())
                {
                    if (me->GetEntry() == NPC_SUMMONED_ACE || me->GetEntry() == NPC_SUMMONED_ACE_2)
                    {
                        if (me->GetOwnerGUID() == who->GetGUID())
                        {
                            std::list<Unit*> cart = me->SelectNearbyUnits(NPC_SUMMONED_WILD_CART, 30.0f);
                            for (std::list<Unit*>::iterator itr = cart.begin(); itr != cart.end(); ++itr)
                                if (auto cart = (*itr))
                                {
                                    if (cart->GetOwnerGUID() == me->GetOwnerGUID())
                                    {
                                        me->EnterVehicle(cart, 2);
                                    }
                                }
                        }
                    }

                    if (me->GetEntry() == NPC_SUMMONED_IZZY || me->GetEntry() == NPC_SUMMONED_IZZY_2)
                    {
                        if (me->GetOwnerGUID() == who->GetGUID())
                        {
                            std::list<Unit*> cart = me->SelectNearbyUnits(NPC_SUMMONED_WILD_CART, 30.0f);
                            for (std::list<Unit*>::iterator itr = cart.begin(); itr != cart.end(); ++itr)
                                if (auto cart = (*itr))
                                {
                                    if (cart->GetOwnerGUID() == me->GetOwnerGUID())
                                    {
                                        me->EnterVehicle(cart, 3);
                                    }
                                }
                        }
                    }

                    if (me->GetEntry() == NPC_SUMMONED_GOBBER || me->GetEntry() == NPC_SUMMONED_GOBBER_2)
                    {
                        if (me->GetOwnerGUID() == who->GetGUID())
                        {
                            std::list<Unit*> cart = me->SelectNearbyUnits(NPC_SUMMONED_WILD_CART, 30.0f);
                            for (std::list<Unit*>::iterator itr = cart.begin(); itr != cart.end(); ++itr)
                                if (auto cart = (*itr))
                                {
                                    if (cart->GetOwnerGUID() == me->GetOwnerGUID())
                                    {
                                        me->EnterVehicle(cart, 4);
                                    }
                                }
                        }
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

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetEntry() == NPC_SUMMONED_ACE || me->GetEntry() == NPC_SUMMONED_ACE_2)
                    {
                        if (!player->isAlive())
                        {
                            player->RemoveAura(SPELL_AURA_ACE);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_REWARDED ||
                            player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_COMPLETE)
                        {
                            player->RemoveAura(SPELL_AURA_ACE);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetDistance(player) >= 120.f)
                        {
                            player->RemoveAura(SPELL_AURA_ACE);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->isInCombat())
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.4f * M_PI);
                    }

                    if (me->GetEntry() == NPC_SUMMONED_IZZY || me->GetEntry() == NPC_SUMMONED_IZZY_2)
                    {
                        if (!player->isAlive())
                        {
                            player->RemoveAura(SPELL_AURA_IZZY);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_REWARDED ||
                            player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_COMPLETE)
                        {
                            player->RemoveAura(SPELL_AURA_IZZY);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetDistance(player) >= 120.f)
                        {
                            player->RemoveAura(SPELL_AURA_IZZY);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->isInCombat())
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.9f * M_PI);
                    }

                    if (me->GetEntry() == NPC_SUMMONED_GOBBER || me->GetEntry() == NPC_SUMMONED_GOBBER_2)
                    {
                        if (!player->isAlive())
                        {
                            player->RemoveAura(SPELL_AURA_GOBBER);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_REWARDED ||
                            player->GetQuestStatus(QUEST_WILD_CART) == QUEST_STATUS_COMPLETE)
                        {
                            player->RemoveAura(SPELL_AURA_GOBBER);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetDistance(player) >= 120.f)
                        {
                            player->RemoveAura(SPELL_AURA_GOBBER);
                            me->DespawnOrUnsummon();
                        }

                        if (!me->isInCombat())
                            me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.7f * M_PI);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);


            //while (uint32 eventId = _events.ExecuteEvent())
            //{
            //    switch (eventId)
            //    {
            //    case EVENT_ORC_SCOUT_REND:
            //        if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ORC_SCOUT_REND);
            //        _events.ScheduleEvent(EVENT_ORC_SCOUT_REND, urand(5000, 14000));
            //        break;
            //    default:
            //        break;
            //    }
            //}
        }
    private:
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summoned_izzy_gobber_aceAI(creature);
    }
};

class playerscript_summon_greedy_izzy_gobber_ace : public PlayerScript
{
public:
    playerscript_summon_greedy_izzy_gobber_ace() : PlayerScript("playerscript_summon_greedy_izzy_gobber_ace") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newArea == 4924)
        {
            if (player->GetQuestStatus(QUEST_KAJACOLA) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_WILD_CART) != QUEST_STATUS_REWARDED)
            {
                if (!player->GetAura(SPELL_AURA_GREEDY))
                {
                    player->CastWithDelay(500, player, SPELL_SUMMON_GREEDY);
                }

                if (player->GetQuestStatus(QUEST_MORALE_BOOST) == QUEST_STATUS_REWARDED ||
                    player->GetQuestStatus(QUEST_MORALE_BOOST) == QUEST_STATUS_COMPLETE)
                {
                    if (!player->GetAura(SPELL_AURA_ACE))
                        player->CastWithDelay(500, player, SPELL_RESUMMON_ACE);

                    if (!player->GetAura(SPELL_AURA_IZZY))
                        player->CastWithDelay(500, player, SPELL_RESUMMON_IZZY);

                    if (!player->GetAura(SPELL_AURA_GOBBER))
                        player->CastWithDelay(500, player, SPELL_RESUMMON_GOBBER);
                }
            }
        }
        else
            return;
    }
};


uint32 const WildCartRidePathSize = 5;
const G3D::Vector3 WildCartRidePath[WildCartRidePathSize] =
{
    { 2072.210f, 1846.002f, 138.143f }, // 0
    { 2080.459f, 1849.126f, 137.606f },
    { 2105.403f, 1855.833f, 129.191f },
    { 2126.280f, 1861.491f, 126.503f },
    { 2147.560f, 1868.554f, 127.346f }, // 4 
};

uint32 const WildCartRidePathSize2 = 12;
const G3D::Vector3 WildCartRidePath2[WildCartRidePathSize2] =
{
    { 2181.068f, 1879.653f, 105.94f }, // 0
    { 2199.793f, 1885.604f, 84.518f },
    { 2216.008f, 1889.728f, 69.419f },
    { 2236.289f, 1895.935f, 59.542f },
    { 2253.223f, 1901.180f, 52.231f },
    { 2273.656f, 1905.713f, 40.891f },
    { 2292.110f, 1911.351f, 34.743f },
    { 2316.186f, 1919.141f, 30.829f },
    { 2332.839f, 1925.787f, 28.113f },
    { 2343.340f, 1928.780f, 26.394f },
    { 2354.215f, 1929.784f, 25.113f },
    { 2368.307f, 1933.982f, 21.221f }, // 11
};

enum QuestWildMineCartRide
{
    QUEST_WILD_MINE_CART_RIDE = 25184,

    SPELL_PLAYER_TO_ACTOR_DESPAWNER = 151156,
    SPELL_WOUND_VISUAL = 95694,
};

class npc_wild_mine_cart_ride_vehicle : public CreatureScript
{
public:
    npc_wild_mine_cart_ride_vehicle() : CreatureScript("npc_wild_mine_cart_ride_vehicle") { }

    struct npc_wild_mine_cart_ride_vehicleAI : public VehicleAI
    {
        npc_wild_mine_cart_ride_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 4 && _firstPath)
            {
                me->GetMotionMaster()->MoveJump(2170.37f, 1876.81f, 117.25f, 21.0f, 15.0f);
                _events.ScheduleEvent(EVENT_WILD_MINE_CART_2, 1500);
            }

            if (point == 10 && !_firstPath)
            {
                _despawnMode = true;
            }

            if (point == 11 && !_firstPath)
            {
                me->CastSpell(me, SPELL_WOUND_VISUAL);
                me->CastSpell(me, SPELL_CAMERA_SHAKE);
                me->CastSpell(me, 66127); // temp explosion visual
                _events.ScheduleEvent(EVENT_WILD_MINE_CART_3, 500);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                passenger->CastSpell(passenger, SPELL_PLAYER_TO_HIS_ACTORS);
                _events.ScheduleEvent(EVENT_WILD_MINE_CART_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (_despawnMode)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->CastSpell(player, SPELL_PLAYER_TO_ACTOR_DESPAWNER);
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WILD_MINE_CART_1:
                    _firstPath = true;
                    me->GetMotionMaster()->MoveSmoothPath(WildCartRidePath, WildCartRidePathSize);
                    break;
                case EVENT_WILD_MINE_CART_2:
                    _firstPath = false;
                    me->SetSpeed(MOVE_RUN, 4.0f);
                    me->GetMotionMaster()->MoveSmoothPath(WildCartRidePath2, WildCartRidePathSize2);
                    break;
                case EVENT_WILD_MINE_CART_3:
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _firstPath = true;
        bool _despawnMode = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_mine_cart_ride_vehicleAI(creature);
    }
};

uint32 const FootbombPathSize = 25;
const G3D::Vector3 FootbombPath[FootbombPathSize] =
{
    { 2365.49f, 1942.29f, 21.224f },
    { 2359.22f, 1946.24f, 22.413f },
    { 2341.64f, 1942.09f, 23.674f },
    { 2316.55f, 1957.84f, 23.435f },
    { 2307.16f, 1971.95f, 23.958f },
    { 2284.55f, 1989.17f, 25.115f },
    { 2267.28f, 2006.27f, 26.550f },
    { 2246.44f, 2042.32f, 28.628f },
    { 2234.32f, 2052.00f, 30.211f },
    { 2215.76f, 2050.94f, 34.113f },
    { 2191.81f, 2045.65f, 39.198f },
    { 2167.20f, 2053.24f, 43.216f },
    { 2154.83f, 2064.97f, 45.489f },
    { 2145.78f, 2097.88f, 51.337f },
    { 2132.32f, 2116.20f, 58.521f },
    { 2120.27f, 2133.91f, 62.754f },
    { 2119.43f, 2158.10f, 63.095f },
    { 2129.43f, 2175.00f, 63.176f },
    { 2138.17f, 2199.10f, 61.931f },
    { 2142.83f, 2231.22f, 59.599f },
    { 2128.46f, 2263.92f, 58.627f },
    { 2112.64f, 2303.77f, 57.118f },
    { 2106.37f, 2335.64f, 55.333f },
    { 2119.57f, 2365.56f, 49.236f },
    { 2122.44f, 2402.79f, 46.250f }, // 24
};

class npc_footbomb_uniform_trip_vehicle : public CreatureScript
{
public:
    npc_footbomb_uniform_trip_vehicle() : CreatureScript("npc_footbomb_uniform_trip_vehicle") { }

    struct npc_footbomb_uniform_trip_vehicleAI : public VehicleAI
    {
        npc_footbomb_uniform_trip_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_FOOTBOMB_1, 2500);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 24:
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
                case EVENT_FOOTBOMB_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(FootbombPath, FootbombPathSize);
                    break;
                case EVENT_FOOTBOMB_2:
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
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_footbomb_uniform_trip_vehicleAI(creature);
    }
};

uint32 const BattleWorgPathSize = 36;
const G3D::Vector3 BattleWorgPath[BattleWorgPathSize] =
{
    { 1458.919f, -5013.229f, 11.647f },
    { 1450.878f, -5014.488f, 12.084f },
    { 1444.092f, -5014.792f, 12.194f },
    { 1440.011f, -5012.454f, 12.329f },
    { 1436.843f, -4980.531f, 11.925f },
    { 1434.400f, -4960.017f, 11.588f },
    { 1429.171f, -4912.390f, 12.218f },
    { 1428.088f, -4903.157f, 11.384f },
    { 1418.006f, -4850.895f, 14.086f },
    { 1407.784f, -4814.345f, 21.267f },
    { 1390.980f, -4778.851f, 24.968f },
    { 1374.477f, -4737.574f, 28.158f },
    { 1373.398f, -4722.540f, 28.308f },
    { 1363.248f, -4667.407f, 27.046f },
    { 1346.240f, -4636.245f, 24.967f },
    { 1321.170f, -4589.424f, 23.817f },
    { 1312.717f, -4557.069f, 23.060f },
    { 1313.103f, -4504.559f, 22.856f },
    { 1315.369f, -4466.924f, 24.443f },
    { 1311.400f, -4424.871f, 24.658f },
    { 1318.695f, -4392.494f, 25.842f },
    { 1344.744f, -4374.012f, 26.173f },
    { 1375.743f, -4371.148f, 26.025f },
    { 1420.074f, -4364.755f, 25.569f },
    { 1429.544f, -4368.806f, 25.569f },
    { 1432.290f, -4398.414f, 25.569f },
    { 1435.177f, -4417.790f, 25.569f },
    { 1443.945f, -4423.411f, 25.569f },
    { 1466.578f, -4420.283f, 25.569f },
    { 1487.111f, -4417.994f, 25.339f },
    { 1509.548f, -4414.372f, 22.517f },
    { 1529.572f, -4409.431f, 19.143f },
    { 1548.646f, -4404.722f, 18.120f },
    { 1568.439f, -4399.161f, 16.147f },
    { 1588.867f, -4386.721f, 18.080f },
    { 1602.249f, -4378.555f, 20.955f }, // 35
};

class npc_battleworg_trip_vehicle : public CreatureScript
{
public:
    npc_battleworg_trip_vehicle() : CreatureScript("npc_battleworg_trip_vehicle") { }

    struct npc_battleworg_trip_vehicleAI : public VehicleAI
    {
        npc_battleworg_trip_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                _events.ScheduleEvent(EVENT_BATTLEWORG_TRIP_1, 1000);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 35:
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
                case EVENT_BATTLEWORG_TRIP_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(BattleWorgPath, BattleWorgPathSize);
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
        return new npc_battleworg_trip_vehicleAI(creature);
    }
};

void AddSC_lost_isle()
{
    new npc_cyclone_of_the_elements_ship();
    new npc_miner_and_cart_escort();
    new spell_weed_whacker();
    new npc_orc_scout_infrared_infradead();
    new npc_bastia_trip_vehicle();
    new npc_gyro_precious_cargo_vehicle();
    new npc_tornado_warchiefs_revange_vehicle();
    new npc_sling_rocket();
    new npc_wild_clucker_egg_trading_up();
    new npc_ace_surrender_escort();
    new npc_bc_eliminator();
    new npc_super_booster_rocket_boots();
    new npc_flying_bomber_old_friends();
    new npc_sassy_quest_giver_volcanoth();
    new npc_bastia_second_trip_vehicle();
    new npc_assistant_greedy_kajacola();
    new playerscript_summon_greedy_izzy_gobber_ace();
    new npc_summoned_izzy_gobber_ace();
    new npc_wild_mine_cart_ride_vehicle();
    new npc_footbomb_uniform_trip_vehicle();
    new npc_battleworg_trip_vehicle();
};