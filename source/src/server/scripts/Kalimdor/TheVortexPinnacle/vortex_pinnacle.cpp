/*
 * Copyright (C) 2018 - 2020 ProjectNelth <http://www.projectnelth.com/>
 *
 * Copyright (C) 2008 - 2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "the_vortex_pinnacle.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "G3D/Vector3.h"
#include "G3D/Triangle.h"

enum Slipstream
{
    SLIPSTREAM_ENTERANCE_LEFT = 0,
    SLIPSTREAM_ENTERANCE_RIGHT,

    SLIPSTREAM_ERTAN_1,
    SLIPSTREAM_ERTAN_2,
    SLIPSTREAM_ERTAN_3,
    SLIPSTREAM_ERTAN_ARRIVAL,

    SLIPSTREAM_ALTAIRUS_1,
    SLIPSTREAM_ALTAIRUS_2,
    SLIPSTREAM_ALTAIRUS_3,
    SLIPSTREAM_ALTAIRUS_4,
    SLIPSTREAM_ALTAIRUS_5,
    SLIPSTREAM_ALTAIRUS_ARRIVAL,

    SLIPSTREAM_ASAAD_1,

    MAX_SLIPSTREAM,
};
enum eSpells
{
    // Gust Soldier
    SPELL_AIR_NOVA                      = 87933,
    SPELL_CHARGE                        = 87930,
    SPELL_WIND_BLAST                    = 87923,

    // Wild Vortex
    SPELL_CYCLONE                       = 88010,
    SPELL_LIGHNING_BOLT                 = 88032,
    SPELL_WIND_SHOCK                    = 88029,

    // Armored Mistral
    SPELL_GALE_STRIKE                   = 88061,
    SPELL_RISING_WINDS                  = 88058,
    SPELL_STORM_SURGE                   = 88055,

    // Cloud Prince
    SPELL_STARFALL                      = 88073,
    SPELL_TYPHON                        = 88075,

    // Slipstream
    SPELL_RIDE_SLIPSTREAM               = 84965,
    SPELL_TO_THE_VORTEX_PINNACLE        = 88776,

    // Turbulent Squall
    SPELL_ASPHYXIATE                    = 88175,
    SPELL_CLOUDBURST                    = 88170,
    SPELL_HURRICANE                     = 88171,

    // Howling Gale
    SPELL_HOWLING_GALE                  = 85084,    //dummy visual
    SPELL_HOWLING_GALE_MISSILE1         = 85086,
    SPELL_HOWLING_GALE_MISSILE2         = 85136,
    SPELL_HOWLING_GALE_MISSILE3         = 85137,
    SPELL_HOWLING_GALE_KNOCKBACK        = 85085,

    // Young Storm Dragon
    SPELL_HEALING_WELL                  = 88201,
    SPELL_CHILLING_BLAST                = 88194,

    // Grounding Field
    SPELL_GROUNDING_FIELD_VISUAL_BEAM   = 87517,

    // Executor of the Caliph
    SPELL_DEVASTATE                     = 78660,
    SPELL_RALLY                         = 87761,
    SPELL_SHOCKWAVE                     = 87759,

    // Minister of Air
    SPELL_LIGHNING_LASH                 = 87762,
    SPELL_LIGHNING_LASH_PERIODIC        = 88963,
    SPELL_LIGHNING_NOVA                 = 87768,

    // Servant of Asaad
    SPELL_DIVINE_STORM                  = 58127,
    SPELL_CRUSADER_STRIKE               = 87771,
    SPELL_HAND_OF_PROTECTION            = 87772,

    // Temple Adept
    SPELL_HOLY_SMITE                    = 88959,
    SPELL_GREATER_HEAL                  = 87779,
    SPELL_DESPERATE_SPEED               = 87780,
};
enum eEvents
{
    EVENT_NONE,

    // Slipstrem
    EVENT_TELEPORT,

    // Gust Soldier
    EVENT_WIND_BLAST,
    EVENT_GUST_CHARGE,

    // Wild Vortex
    EVENT_CYCLONE,
    EVENT_LIGHNING_BOLT,
    EVENT_WIND_SHOCK,

    // Armored Mistral
    EVENT_GALE_STRIKE,
    EVENT_RISING_WINDS,
    EVENT_STORM_SURGE,

    // Cloud Prince
    EVENT_STARFALL,
    EVENT_TYPHON,

    // Turbulent Squall
    EVENT_ASPHYXIATE,
    EVENT_CLOUDBURST,
    EVENT_HURRICANE,

    // Young Storm Dragon
    EVENT_CHILLING_BLAST_PREP,
    EVENT_CHILLING_BLAST,
    EVENT_CHILLING_BLAST_ORIENTATION,
    EVENT_CHILLING_BLAST_ORIENTATION_RECOVER,
    EVENT_HEALING_RAIN_FINISHED,
    EVENT_FLY,
    EVENT_ACTIVATE,

    // Executor of the Caliph
    EVENT_DEVASTATE,
    EVENT_RALLY,
    EVENT_SHOCKWAVE,

    // Minister of Air
    EVENT_LIGHNING_LASH,
    EVENT_LIGHNING_NOVA,

    // Servant of Asaad
    EVENT_DIVINE_STROM,
    EVENT_CRUSADER_STRIKE,
    EVENT_HAND_OF_PROTECTION,

    // Temple Adept
    EVENT_HOLY_SMITE,
    EVENT_GREATER_HEAL,
    EVENT_DESPERATE_SPEED,
};

#define NB_STARS 8
#define NPC_STAR 45932 // 45981
#define UNDER_MAP_ARRIVAL 42

/*
    SlipStream - 45455
*/
class npc_slipstream : public CreatureScript
{
public:
    npc_slipstream() : CreatureScript("npc_slipstream") { }

    void generateSlipStreamMap(Player* player, Creature* me)
    {
        std::list<Creature*> slipStreams;
        player->GetCreatureListWithEntryInGrid(slipStreams, NPC_SLIPSTREAM, 533.0f);
        slipStreams.sort(Trinity::ObjectDistanceOrderPred(player));
        for (std::list<Creature*>::iterator itr = slipStreams.begin(); itr != slipStreams.end(); ++itr)
        {
            for (int i = 0; i < MAX_SLIPSTREAM; i++)
                if (CAST_AI(npc_slipstream::npc_slipstreamAI, (*itr)->AI())->isSleapStream((Slipstream)i))
                {
                    CAST_AI(npc_slipstream::npc_slipstreamAI, me->AI())->_slipStream[i] = (*itr)->GetGUID();
                    break;
                }
        }
    }
    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (auto instance = creature->GetInstanceScript())
        {
            if (CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ENTERANCE_LEFT)
                || CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ERTAN_1))
            {
                player->NearTeleportTo(slipstreamPos[5]);
                return true;
            }
       /*     else if (CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ERTAN_1))
            {
                generateSlipStreamMap(player, creature);
                CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->AddSleapStreamEvent(player->GetGUID(), SLIPSTREAM_ERTAN_1);
            }*/
            else if (CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ENTERANCE_RIGHT)
                || CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ALTAIRUS_1))
            {
                player->NearTeleportTo(slipstreamPos[11]);
                return true;
            }
     /*       else if (CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->isSleapStream(SLIPSTREAM_ALTAIRUS_1))
            {
                generateSlipStreamMap(player, creature);
                CAST_AI(npc_slipstream::npc_slipstreamAI, creature->AI())->AddSleapStreamEvent(player->GetGUID(), SLIPSTREAM_ALTAIRUS_1);
            }*/
        }
        player->PlayerTalkClass->ClearMenus();
        return true;
    }
    struct playerAction {
        uint64 _playerGUID;
        Slipstream _action;
        uint32 _sleapStreamTimer;
        bool _lastMove;
    };
    class MoveEnded {
    public:
        MoveEnded(Creature* c) : me(c) {}
        bool operator() (const playerAction& pa)
        {
            Unit* player = Unit::GetUnit(*me, pa._playerGUID);
            if (!player)
                return true;
            if (pa._lastMove)
                return false;
            return CAST_AI(npc_slipstream::npc_slipstreamAI, me->AI())->isSleapStreamWayEnded(pa._action);
        }

    private:
        Creature* me;
    };
    struct npc_slipstreamAI : public ScriptedAI
    {
        npc_slipstreamAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset()
        {
            sleapStreamRefresh = 250;
        }
        void AddSleapStreamEvent(uint64 guid, Slipstream ident)
        {
            playerAction act;
            act._playerGUID = guid;
            act._action = ident;
            act._sleapStreamTimer = 3000;
            act._lastMove = false;
            bool canPush = true;
            for (std::list<playerAction >::iterator itr = passengersGUID.begin(); itr != passengersGUID.end(); itr++)
                if ((*itr)._playerGUID == guid)
                {
                    canPush = false;
                    break;
                }
            if (canPush)
                passengersGUID.push_back(act);
        }
        bool isSleapStream(Slipstream ident)
        {
            Position pos = slipstreamPos[ident];
            if (me->GetDistance2d(pos.m_positionX, pos.m_positionY) < 2.0f)
                return true;
            return false;
        }
        bool isSleapStreamWayEnded(Slipstream ident)
        {
            switch (ident)
            {
            case SLIPSTREAM_ERTAN_3:
            case SLIPSTREAM_ALTAIRUS_5:
            case SLIPSTREAM_ASAAD_1:
            case SLIPSTREAM_ENTERANCE_LEFT:
            case SLIPSTREAM_ENTERANCE_RIGHT:
                return true;
            default:
                break;
            }
            return false;
        }
        void UpdateSleapStreamEvent(const uint32 diff)
        {
            passengersGUID.remove_if(MoveEnded(me));
            for (std::list<playerAction >::iterator itr = passengersGUID.begin(); itr != passengersGUID.end(); itr++)
            {
                if ((*itr)._sleapStreamTimer <= diff)
                {
                    if (auto player = Unit::GetUnit(*me, (*itr)._playerGUID))
                    {
                        if ((*itr)._action != SLIPSTREAM_ERTAN_ARRIVAL && (*itr)._action != SLIPSTREAM_ALTAIRUS_ARRIVAL)
                        {
                            if (auto sl = Unit::GetCreature(*me, _slipStream[(int)(*itr)._action]))
                            {
                                if (sl->GetVehicleKit())
                                    if (sl->GetVehicleKit()->GetPassenger(0) != NULL)
                                    {
                                        (*itr)._sleapStreamTimer = 2500;
                                        continue;
                                    }
                                player->ExitVehicle();
                                player->CastSpell(sl, 84988, true);
                            }
                        }
                        else
                        {
                            player->ExitVehicle(&slipstreamPos[(*itr)._action]);
                        }
                    }
                    int act = (*itr)._action;
                    act++;
                    if (isSleapStreamWayEnded((Slipstream)act))
                    {
                        (*itr)._lastMove = !(*itr)._lastMove;
                        if ((*itr)._lastMove)
                            (*itr)._action = (Slipstream)act;
                    }
                    else
                        (*itr)._action = (Slipstream)act;
                    (*itr)._sleapStreamTimer = 2000;
                }
                else
                    (*itr)._sleapStreamTimer -= diff;
            }
        }
        void MovementInform(uint32, uint32 point)
        {
            if (point == UNDER_MAP_ARRIVAL)
                me->DespawnOrUnsummon(1000);
        }
        void SetGUID(uint64 guid, int32)
        {
            me->RemoveAllAuras();
            me->SetSpeed(MOVE_FLIGHT, 3.0f);
            if (auto player = Unit::GetPlayer(*me, guid))
                player->CastSpell(me, 84988, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
        void UpdateAI(const uint32 diff)
        {
            if (sleapStreamRefresh <= diff)
            {
                if (me->GetVehicleKit())
                    if (auto passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        Position pos(*me);
                        passenger->Relocate(&pos);
                    }

                UpdateSleapStreamEvent(diff + 250);
                sleapStreamRefresh = 500;
            }
            else
                sleapStreamRefresh -= diff;
        }

        std::list<playerAction > passengersGUID;
        std::map<int, uint64 > _slipStream;
    private:
        uint32 sleapStreamRefresh;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slipstreamAI(creature);
    }
};

class mob_vortex_wind_rotation_manager : public CreatureScript
{
public:
    mob_vortex_wind_rotation_manager() : CreatureScript("mob_vortex_wind_rotation_manager") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_vortex_wind_rotation_managerAI(pCreature);
    }

    struct mob_vortex_wind_rotation_managerAI : public ScriptedAI
    {
        mob_vortex_wind_rotation_managerAI(Creature* pCreature) : ScriptedAI(pCreature), Summons(me)
        {
            instance = pCreature->GetInstanceScript();
        }

        void Reset()
        {
            m_timer = 0;
            radius = 14;
            own_stars.clear();
            orient = 0.0f;
            Summons.DespawnAll();
            for (int i = 0; i < NB_STARS; i++)
            {
                orient += 2 * M_PI / NB_STARS;
                float x, y;
                me->GetNearPoint2D(x, y, radius, orient);
                me->SummonCreature(NPC_STAR, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
            }
            start = false;
            me->AddAura(85719, me);
        }

        void JustDied(Unit* /*who*/)
        {
            Summons.DespawnAll();
        }

        void DamageTaken(Unit* pDone_by, uint32& uiDamage)
        {
            uiDamage = 0;
        }

        void AttackStart(Unit* who)
        {
            return;
        }

        void JustSummoned(Creature* summoned)
        {
            own_stars.push_back(summoned->GetGUID());
            Summons.Summon(summoned);
        }

        void checkForDespawn(const uint32 diff)
        {
            if (mui_timer_despawn <= diff)
            {
                bool despawn = true;
                for (std::list<uint64 >::iterator itr = own_stars.begin(); itr != own_stars.end(); itr++)
                    if (auto vortex = Unit::GetCreature(*me, (*itr)))
                        if (vortex->isAlive())
                        {
                            despawn = false;
                            break;
                        }
                if (despawn)
                    me->DespawnOrUnsummon();
                mui_timer_despawn = 1000;
            }
            else
                mui_timer_despawn -= diff;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetDistance2d(who) < 40.0f)
                start = true;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!start)
                return;

            checkForDespawn(uiDiff);

            if (m_timer <= uiDiff)
            {
                orient = fmod(orient + 0.1f, 2.0f * static_cast<float>(M_PI));
                for (std::list<uint64 >::iterator itr = own_stars.begin(); itr != own_stars.end(); itr++)
                {
                    if (auto vortex = Unit::GetCreature(*me, (*itr)))
                    {
                        float x, y;
                        orient += 2 * M_PI / NB_STARS;
                        me->GetNearPoint2D(x, y, radius, orient);
                        vortex->GetMotionMaster()->Clear();
                        vortex->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                    }
                }
                m_timer = 200;
            }
            else m_timer -= uiDiff;
        }

    private:
        InstanceScript* instance;
        SummonList Summons;
        std::list<uint64 > own_stars;

        uint32 m_timer;
        uint32 radius;
        uint32 mui_timer_despawn;

        float orient;

        bool start;
    };
};

class mob_skyfall_star : public CreatureScript
{
public:
    mob_skyfall_star() : CreatureScript("mob_skyfall_star") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_skyfall_starAI(pCreature);
    }

    struct mob_skyfall_starAI : public ScriptedAI
    {
        mob_skyfall_starAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
        }

        void Reset()
        {
            m_timer = 0;
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            if (m_timer <= uiDiff)
            {
                DoCastRandom(87854, 0.0f);
                m_timer = urand(3000, 10000);
            }
            else m_timer -= uiDiff;
        }

    private:
        InstanceScript* instance;
        uint32 m_timer;
    };
};

/*
    Golden Orb - 51157
*/
class mob_golden_orb : public CreatureScript
{
public:
    mob_golden_orb() : CreatureScript("mob_golden_orb") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (auto instance = creature->GetInstanceScript())
        {
            instance->SetData(DATA_GOLDEN_ORB, DONE);
            creature->DespawnOrUnsummon();
        }
        player->PlayerTalkClass->ClearMenus();
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    } 
};

/*
    Turbulent Squall - 45924(N) & 45925(H)
*/
class mob_turbulent_squall : public CreatureScript
{
public:
    mob_turbulent_squall() : CreatureScript("mob_turbulent_squall") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_turbulent_squallAI(creature);
    }
    struct mob_turbulent_squallAI : public ScriptedAI
    {
        mob_turbulent_squallAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _events.Reset();
        }
        void EnterCombat(Unit* who)
        {
            _events.ScheduleEvent(EVENT_ASPHYXIATE, 3 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_HURRICANE, 7 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_CLOUDBURST, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            me->CallForHelp(10.0f);
        }
        void UpdateAI(uint32 diff)
        {

            _events.Update(diff);
            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_ASPHYXIATE:
                    DoCastVictim(SPELL_ASPHYXIATE);
                    _events.RescheduleEvent(EVENT_ASPHYXIATE, urand(22 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                    break;
                case EVENT_HURRICANE:
                    if (auto _target = SelectTarget(SELECT_TARGET_RANDOM, 0, 28.0f, true))
                        DoCast(_target, SPELL_HURRICANE);
                    _events.RescheduleEvent(EVENT_HURRICANE, urand(12 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                    break;
                case EVENT_CLOUDBURST:
                    DoCast(SPELL_CLOUDBURST);
                    _events.RescheduleEvent(EVENT_CLOUDBURST, urand(35 * IN_MILLISECONDS, 45 * IN_MILLISECONDS));
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
    };
};

/*
    Howling Gale - 45572
*/
enum gale
{
    EVENT_COSMETIC_SWIRL = 1,
    EVENT_ACTUAL_KNOCKBACK,
    EVENT_REACTIVATE,
};
class mob_howling_gale : public CreatureScript
{
public:
    mob_howling_gale() : CreatureScript("mob_howling_gale") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_howling_galeAI(creature);
    }
    struct mob_howling_galeAI : public ScriptedAI
    {
        mob_howling_galeAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
            if (disabled)
            {
                events.ScheduleEvent(EVENT_REACTIVATE, 6000);
            }
            else
            {
                events.ScheduleEvent(EVENT_ACTUAL_KNOCKBACK, 1000);
                events.ScheduleEvent(EVENT_COSMETIC_SWIRL, 500);
            }
        }

        void Reset() 
        {
            me->SetReactState(REACT_PASSIVE);
            events.Reset();
            if (disabled)
            {
                events.ScheduleEvent(EVENT_REACTIVATE, 6000);
            }
            else
            {
                events.ScheduleEvent(EVENT_ACTUAL_KNOCKBACK, 6000);
                events.ScheduleEvent(EVENT_COSMETIC_SWIRL, 500);
            }
        }
        void DamageTaken(Unit* attacker, uint32& damage)
        {
            damage = 0;
            me->SetHealth(me->GetMaxHealth());
            if (!attacker || !attacker->ToPlayer())
                return;
            DisableMe();
            me->DeleteThreatList();
        }

        void DisableMe()
        {
            if (!disabled)
            {
                disabled = true;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                events.Reset();
                events.ScheduleEvent(EVENT_REACTIVATE, 6000);
            }
        }

        void UpdateAI(uint32 diff)
        {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case EVENT_COSMETIC_SWIRL:
                        DoCast(spells[_castId]);
                        _castId++;
                        if (_castId > 2)
                            _castId = 0;
                        events.ScheduleEvent(EVENT_COSMETIC_SWIRL, 500);
                        break;
                    case EVENT_ACTUAL_KNOCKBACK:
                        DoCast(SPELL_HOWLING_GALE_KNOCKBACK);
                        if (me->isInCombat())
                            me->DeleteThreatList();
                        events.ScheduleEvent(EVENT_ACTUAL_KNOCKBACK, 1000);
                        break;
                    case EVENT_REACTIVATE:
                        if (!me->HasAura(85084))
                            me->AddAura(85084, me);
                        disabled = false;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                        events.ScheduleEvent(EVENT_ACTUAL_KNOCKBACK, 2000);
                        events.ScheduleEvent(EVENT_COSMETIC_SWIRL, 500);
                        break;
                    default:
                        break;
                    }
        }

    private:
        EventMap events;
        int _castId = 1;
        uint32 spells[3]    { SPELL_HOWLING_GALE_MISSILE1, SPELL_HOWLING_GALE_MISSILE2, SPELL_HOWLING_GALE_MISSILE3 };
        bool disabled{ false };
    };
};

/*
    Young Storm Dragon - 45919(N) & 45920(H)
*/
enum youngDrake
{

};

class mob_young_storm_dragon : public CreatureScript
{
public:
    mob_young_storm_dragon() : CreatureScript("mob_young_storm_dragon") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_young_storm_dragonAI(creature);
    }
    struct mob_young_storm_dragonAI : public ScriptedAI
    {
        mob_young_storm_dragonAI(Creature* creature) : ScriptedAI(creature)
        {
            groundZ = me->GetPositionZ();
        }
        bool elevating{ false };

        void Reset()
        {
            prep = false;
            me->DisableMovementFlagUpdate(true);
            events.Reset();
            me->SetHover(false);
            me->SetCanFly(false);
            me->SetDisableGravity(false);
            me->DisableSpline();
            me->GetMotionMaster()->MoveLand(88888, me->GetHomePosition());
            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            me->SetReactState(REACT_AGGRESSIVE);
        }
     
        void MovementInform(uint32 type, uint32 point) override
        {
            if (type == EFFECT_MOTION_TYPE)
                if (point == 88888/*can be changed*/)
                    me->SetFacingTo(5.474139f);
        }

        void EnterCombat(Unit* who)
        {
            me->DisableMovementFlagUpdate(true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            me->SetPosition(me->GetPositionX(), me->GetPositionY(), groundZ, me->GetOrientation());
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
            me->SetReactState(REACT_PASSIVE);


            me->AddThreat(who, 200.f);
            me->CastWithDelay(200, me, SPELL_HEALING_WELL);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->isInCombat())
                if (who->ToPlayer())
                    if (me->GetDistance(who) < 26.f)
                    {
                        AttackStart(who);
                    }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->SetDisableGravity(true);
            me->SetCanFly(false);
            me->SetHover(false);
            me->DeleteThreatList();
            summons.DespawnAll();
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
                case EVENT_CHILLING_BLAST:
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                    spell = SPELL_CHILLING_BLAST;         //Spell ID
                    min_repeat = 20000;                 //Minimum Delay
                    max_repeat = 24000;                //Maximum Delay
                    DoCast(me, spell);
                    events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    break;
                case EVENT_CHILLING_BLAST_ORIENTATION_RECOVER:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->AddUnitState(UNIT_STATE_ROTATING);
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                case EVENT_FLY:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        me->SetDisableGravity(true);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        events.ScheduleEvent(EVENT_ACTIVATE, 2500);
                    }
                    break;
                case EVENT_ACTIVATE:
                {
                    me->SetCanFly(true);
                    me->SetHover(true);
                    Position up_a_little
                    {
                        me->GetPositionX(),
                        me->GetPositionY(),
                        702.f,
                        me->GetOrientation()
                    };
                    me->NearTeleportTo(up_a_little);
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.ScheduleEvent(EVENT_CHILLING_BLAST, urand(3000, 5000));
                    break;
                }
                default:
                    break;
                }

            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (me->HasReactState(REACT_AGGRESSIVE))
                        DoMeleeAttackIfReady();
        }

        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case EVENT_HEALING_RAIN_FINISHED:
                events.ScheduleEvent(EVENT_FLY, 1);
                break;
            case EVENT_CHILLING_BLAST_PREP:
                if (!prep)
                {
                    prep = true;
                    LockOrientation = me->GetAngle(me->getVictim());
                    me->AttackStop();
                    me->StopMoving();
                    me->AddUnitState(UNIT_STATE_ROOT);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    me->ClearUnitState(UNIT_STATE_ROTATING);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetOrientation(LockOrientation);
                    me->SetFacingTo(LockOrientation);
                }
                break;
            case EVENT_CHILLING_BLAST_ORIENTATION:
                prep = false;
                me->AttackStop();
                me->StopMoving();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                me->ClearUnitState(UNIT_STATE_ROTATING);
                me->SetReactState(REACT_PASSIVE);
                me->SetOrientation(LockOrientation);
                me->SetFacingTo(LockOrientation);
                events.ScheduleEvent(EVENT_CHILLING_BLAST_ORIENTATION_RECOVER, 2500);
                break;
            default:
                break;
            }
        }

    private:
        float LockOrientation{ 0.f };
        EventMap events;
        float groundZ;
        bool prep{ false };
    };
};

/*
    Grounding Field - 47000
*/
class mob_grounding_field : public CreatureScript
{
public:
    mob_grounding_field() : CreatureScript("mob_grounding_field") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_grounding_fieldAI(creature);
    }
    struct mob_grounding_fieldAI : public ScriptedAI
    {
        mob_grounding_fieldAI(Creature* creature) : ScriptedAI(creature) { }
        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }
        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            damage = 0;
        }
        void UpdateAI(uint32 diff)
        {
            if (_castTimer <= diff)
            {
                DoCast(SPELL_GROUNDING_FIELD_VISUAL_BEAM);
                _castTimer = 45 * IN_MILLISECONDS;
            }
            _castTimer -= diff;
        }
    private:
        uint32 _castTimer = 5 * IN_MILLISECONDS;
    };
};

/*
    Grounding Field Visual Beam - 87517
*/
class spell_grounding_field_vp : public SpellScriptLoader
{
public:
    spell_grounding_field_vp() : SpellScriptLoader("spell_grounding_field_vp") {}
    SpellScript* GetSpellScript() const
    {
        return new spell_grounding_field_vp_SpellScript();
    }
    class spell_grounding_field_vp_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_grounding_field_vp_SpellScript);
        bool Load()
        {
            return true;
        }
        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if([](WorldObject* target)
            {
                return target->GetEntry() != NPC_GROUNDING_FIELD;
            });
        }
        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grounding_field_vp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grounding_field_vp_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grounding_field_vp_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENTRY);
        }
    };
};



class spell_grounding_field_mechanic : public SpellScriptLoader
{
public:
    spell_grounding_field_mechanic() : SpellScriptLoader("spell_grounding_field_mechanic") {}
    SpellScript* GetSpellScript() const
    {
        return new spell_grounding_field_mechanic_SpellScript();
    }
    class spell_grounding_field_mechanic_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_grounding_field_mechanic_SpellScript);
        bool Load()
        {
            return true;
        }

        G3D::Vector3 PositionToVector3(Position const& position) const
        {
            return G3D::Vector3(position.GetPositionX(), position.GetPositionY(), position.GetPositionZ());
        }

        bool IsInTriangle(WorldObject* _target, Position _position1, Position _position2, Position _position3) const
        {
            G3D::Triangle const triangle(PositionToVector3(_position1), PositionToVector3(_position2), PositionToVector3(_position3));
            G3D::Vector3 const vector(_target->GetPositionX(), _target->GetPositionY(), _target->GetPositionZ());
            float b[3];

            return G3D::CollisionDetection::isPointInsideTriangle(triangle.vertex(0), triangle.vertex(1), triangle.vertex(2), triangle.normal(), vector, b, triangle.primaryAxis());
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            int position_index{ 0 };
            Position position[3]
            {
                {0.f, 0.f, 0.f, 0.f},
                {0.f, 0.f, 0.f, 0.f},
                {0.f, 0.f, 0.f, 0.f},
            };

            if (auto c = GetCaster())
            {
                std::list<Creature* > triggerList;
                c->GetCreatureListWithEntryInGrid(triggerList, c->GetEntry(), 30.0f);
                if (triggerList.size())
                {
                    for (auto trigger : triggerList)
                        if (trigger->GetGUID() != c->GetGUID())
                            if (position_index <= 2)
                            {
                                position[position_index] = (*trigger);
                                position_index++;
                            }
                }

                if (unitList.size())
                    if (position_index == 3)
                    for (auto itr = unitList.begin(); itr != unitList.end();)
                            if (auto t = (*itr))
                                if (!IsInTriangle(t, position[0], position[1], position[2]))
                                {
                                    if (auto u = t->ToUnit())
                                        u->RemoveAura(GetSpellInfo()->Id);

                                    unitList.erase(itr++);
                                }
                                else
                                {
                                    ++itr;
                                }
            }

            if (unitList.size())
            unitList.remove_if([](WorldObject* target)
                {
                    auto myArray = { 45935, 45930, 45928, 45926, 45935 };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) == end(myArray); //DOES NOT INCLUDE, would be != end(myArray) to be "includes".
                });
        }
        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grounding_field_mechanic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };
};
/*
    Executor of the Caliph - 45928(N) & 45929(H)
*/
class mob_executor_of_the_caliph : public CreatureScript
{
public:
    mob_executor_of_the_caliph() : CreatureScript("mob_executor_of_the_caliph") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_executor_of_the_caliphAI(creature);
    }
    struct mob_executor_of_the_caliphAI : public ScriptedAI
    {
        mob_executor_of_the_caliphAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _events.Reset();
            _rally = false;
        }
        void JustReachedHome()
        {
            Position above_surface
            {
                me->GetHomePosition().GetPositionX(),
                me->GetHomePosition().GetPositionY(),
                me->GetHomePosition().GetPositionZ(),
                me->GetHomePosition().GetOrientation(),
            };
            me->NearTeleportTo(above_surface);
            me->GetMotionMaster()->MoveFall();
        }

        void EnterCombat(Unit* who)
        {
            me->CallForHelp(13.0f);
            _events.ScheduleEvent(EVENT_DEVASTATE, 4 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_SHOCKWAVE, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
        }
        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (!_rally && me->GetHealthPct() < 50.0f)
            {
                _rally = true;
                _events.ScheduleEvent(EVENT_RALLY, 1 * IN_MILLISECONDS);
            }
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_DEVASTATE:
                    DoCastVictim(SPELL_DEVASTATE);
                    _events.RescheduleEvent(EVENT_DEVASTATE, urand(7 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
                case EVENT_RALLY:
                    DoCast(SPELL_RALLY);
                    _events.RescheduleEvent(EVENT_RALLY, urand(13 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    break;
                case EVENT_SHOCKWAVE:
                    DoCast(me, SPELL_SHOCKWAVE, true);
                    _events.RescheduleEvent(EVENT_SHOCKWAVE, urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
            if (me->GetPositionZ() <= 600.0f)
            {
                me->RemoveAllAuras();
                me->NearTeleportTo(me->GetHomePosition());
            }
        }

    private:
        EventMap _events;
        bool _rally = false;
    };
};

/*
    Minister of Air - 45930(N) & 45931(H)
*/
class mob_minister_of_air : public CreatureScript
{
public:
    mob_minister_of_air() : CreatureScript("mob_minister_of_air") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_minister_of_airAI(creature);
    }
    struct mob_minister_of_airAI : public ScriptedAI
    {
        mob_minister_of_airAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _events.Reset();
        }
        void JustReachedHome()
        {
            Reset();
            Position above_surface
            {
                me->GetHomePosition().GetPositionX(),
                me->GetHomePosition().GetPositionY(),
                me->GetHomePosition().GetPositionZ(),
                me->GetHomePosition().GetOrientation(),
            };
            me->NearTeleportTo(above_surface);
            me->GetMotionMaster()->MoveFall();
        }

        void EnterCombat(Unit* who)
        {
            me->CallForHelp(13.0f);
            _events.ScheduleEvent(EVENT_LIGHNING_LASH, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_LIGHNING_NOVA, 1 * IN_MILLISECONDS);
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                me->StopMoving();
                return;
            }

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_LIGHNING_LASH:
                    DoCastVictim(SPELL_LIGHNING_LASH);
                    _events.RescheduleEvent(EVENT_LIGHNING_LASH, urand(6 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                    break;
                case EVENT_LIGHNING_NOVA:
                    DoCast(SPELL_LIGHNING_NOVA);
                    _events.RescheduleEvent(EVENT_LIGHNING_NOVA, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
            if (me->GetPositionZ() <= 600.0f)
            {
                me->RemoveAllAuras();
                me->NearTeleportTo(me->GetHomePosition());
            }
        }

    private:
        EventMap _events;
    };
};

/*
    Servant of Asaad - 45926(N) & 45927(H)
*/
class mob_servant_of_asaad : public CreatureScript
{
public:
    mob_servant_of_asaad() : CreatureScript("mob_servant_of_asaad") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_servant_of_asaadAI(creature);
    }
    struct mob_servant_of_asaadAI : public ScriptedAI
    {
        mob_servant_of_asaadAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            me->AI()->OnCharmed(false);
            me->SetReactState(REACT_AGGRESSIVE);
            _events.Reset();
            _hand = false;
        }
        void JustReachedHome()
        {
            Reset();
            Position above_surface
            {
                me->GetHomePosition().GetPositionX(),
                me->GetHomePosition().GetPositionY(),
                me->GetHomePosition().GetPositionZ(),
                me->GetHomePosition().GetOrientation(),
            };
            me->NearTeleportTo(above_surface);
            me->GetMotionMaster()->MoveFall();
        }

        void EnterCombat(Unit* who)
        {
            me->CallForHelp(13.0f);
            _events.ScheduleEvent(EVENT_DIVINE_STROM, 5 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_CRUSADER_STRIKE, 2 * IN_MILLISECONDS);
        }
        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (!_hand && me->GetHealthPct() < 40.0f)
            {
                _hand = true;
                _events.ScheduleEvent(EVENT_HAND_OF_PROTECTION, 1 * IN_MILLISECONDS);
            }
        }
        void UpdateAI(uint32 diff)
        {
            if (me->GetPositionZ() <= 600.0f)
            {
                if (me->GetCharmer())
                    me->RemoveCharmedBy(me->GetCharmer());
                me->NearTeleportTo(me->GetHomePosition());
                me->RemoveAllAuras();
                me->AI()->OnCharmed(false);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
            }

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_DIVINE_STROM:
                    DoCast(SPELL_DIVINE_STORM);
                    _events.RescheduleEvent(EVENT_DIVINE_STROM, urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                    break;
                case EVENT_CRUSADER_STRIKE:
                    DoCastVictim(SPELL_CRUSADER_STRIKE);
                    _events.RescheduleEvent(EVENT_CRUSADER_STRIKE, 4 * IN_MILLISECONDS);
                    break;
                case EVENT_HAND_OF_PROTECTION:
                    DoCast(me, SPELL_HAND_OF_PROTECTION, true);
                    _events.RescheduleEvent(EVENT_HAND_OF_PROTECTION, urand(16 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        bool _hand = false;
    };
};

/*
    Temple Adept - 45935(N) & 45936(H)
*/
class mob_temple_adept : public CreatureScript
{
public:
    mob_temple_adept() : CreatureScript("mob_temple_adept") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_temple_adeptAI(creature);
    }
    struct mob_temple_adeptAI : public ScriptedAI
    {
        mob_temple_adeptAI(Creature* creature) : ScriptedAI(creature) { }

        void JustReachedHome()
        {
            Position above_surface
            {
                me->GetHomePosition().GetPositionX(),
                me->GetHomePosition().GetPositionY(),
                me->GetHomePosition().GetPositionZ(),
                me->GetHomePosition().GetOrientation(),
            };
            me->NearTeleportTo(above_surface);
            me->GetMotionMaster()->MoveFall();
        }
        void Reset()
        {
            _events.Reset();
        }
        void EnterCombat(Unit* who)
        {
            me->CallForHelp(13.0f);
            _events.ScheduleEvent(EVENT_HOLY_SMITE, 1 * IN_MILLISECONDS);
            _events.ScheduleEvent(EVENT_DESPERATE_SPEED, urand(8 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            _events.ScheduleEvent(EVENT_GREATER_HEAL, urand(8 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_HOLY_SMITE:
                    if (auto _victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 39.0f, true))
                        DoCast(_victim, SPELL_HOLY_SMITE);
                    _events.RescheduleEvent(EVENT_HOLY_SMITE, urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                    break;
                case EVENT_GREATER_HEAL:
                {
                    _events.RescheduleEvent(EVENT_GREATER_HEAL, 5 * IN_MILLISECONDS);
                    if (auto _ally = me->FindNearestCreature(NPC_EXECUTOR_OF_THE_CALIPH, 35.0f, true))
                        if (_ally->GetHealthPct() <= 50.0f)
                        {
                            DoCast(_ally, SPELL_GREATER_HEAL);
                            return;
                        }
                    if (auto _ally = me->FindNearestCreature(NPC_SERVANT_OF_ASAAD, 35.0f, true))
                        if (_ally->GetHealthPct() <= 50.0f)
                        {
                            DoCast(_ally, SPELL_GREATER_HEAL);
                            return;
                        }
                    if (auto _ally = me->FindNearestCreature(NPC_MINISTER_OF_AIR, 35.0f, true))
                        if (_ally->GetHealthPct() <= 50.0f)
                        {
                            DoCast(_ally, SPELL_GREATER_HEAL);
                            return;
                        }
                    DoCast(me, SPELL_GREATER_HEAL);
                }
                break;
                case EVENT_DESPERATE_SPEED:
                    DoCast(SPELL_DESPERATE_SPEED);
                    _events.RescheduleEvent(EVENT_DESPERATE_SPEED, urand(8 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
            if (me->GetPositionZ() <= 600.0f)
            {
                me->RemoveAllAuras();
                me->NearTeleportTo(me->GetHomePosition());
            }
        }

    private:
        EventMap _events;
    };
};

/*
    Lighning Lash - 87762
*/
class spell_lightning_lash_vp : public SpellScriptLoader
{
public:
    spell_lightning_lash_vp() : SpellScriptLoader("spell_lightning_lash_vp") { }

    class spell_lightning_lash_vp_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_lightning_lash_vp_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }
        Player* SelectRandomPlayer(float range = 0.0f, bool checkLoS = true)
        {
            Map* map = GetCaster()->GetMap();
            if (!map->IsDungeon())
                return NULL;

            Map::PlayerList const& PlayerList = map->GetPlayers();
            if (PlayerList.isEmpty())
                return NULL;

            std::list<Player*> temp;
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if ((GetCaster()->IsWithinLOSInMap(i->getSource()) || !checkLoS) && GetCaster()->IsWithinDistInMap(i->getSource(), range)
                    && i->getSource()->isAlive() && !i->getSource()->isGameMaster())
                    temp.push_back(i->getSource());

            if (!temp.empty())
            {
                std::list<Player*>::const_iterator j = temp.begin();
                advance(j, rand() % temp.size());
                return (*j);
            }
            return NULL;
        }
        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            PreventDefaultAction();
            if (!GetCaster())
                return;
            if (GetCaster() && GetCaster()->isAlive())
                GetCaster()->CastSpell(SelectRandomPlayer(45.0f), SPELL_LIGHNING_LASH_PERIODIC, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_lightning_lash_vp_AuraScript::HandleEffectPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_lightning_lash_vp_AuraScript();
    }
};

/*
    Gust Soldier - 45477(N) & 45478(H)
*/
class mob_gust_soldier : public CreatureScript
{
public:
    mob_gust_soldier() : CreatureScript("mob_gust_soldier") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_gust_soldierAI(creature);
    }
    struct mob_gust_soldierAI : public ScriptedAI
    {
        mob_gust_soldierAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* who)
        {
            me->CallForHelp(22.0f);
            events.ScheduleEvent(EVENT_WIND_BLAST, 8000);
            events.ScheduleEvent(EVENT_GUST_CHARGE, 500);
        }
        void JustDied(Unit* /*killer*/)
        {
            DoCast(me, SPELL_AIR_NOVA, true);
        }

        void UpdateAI(uint32 diff)
        {

            events.Update(diff);
            while (uint32 _eventId = events.ExecuteEvent())
                switch (_eventId)
                {
                case EVENT_WIND_BLAST:
                {
                    DoCastVictim(SPELL_WIND_BLAST);
                    events.RescheduleEvent(EVENT_WIND_BLAST, urand(5000, 12000));
                    break;
                }
                case EVENT_GUST_CHARGE:
                    DoCastVictim(SPELL_CHARGE);
                    break;
                }
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };
};

/*
    Wild Vortex - 45912(N) & 45913(H)
*/
class mob_wild_vortex : public CreatureScript
{
public:
    mob_wild_vortex() : CreatureScript("mob_wild_vortex") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_wild_vortexAI(pCreature);
    }

    struct mob_wild_vortexAI : public ScriptedAI
    {
        mob_wild_vortexAI(Creature* pCreature) : ScriptedAI(pCreature) { }

        void Reset()
        {
            _events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(15.0f);
            _events.ScheduleEvent(EVENT_CYCLONE, urand(15000, 17000));
            _events.ScheduleEvent(EVENT_LIGHNING_BOLT, 2000);
            _events.ScheduleEvent(EVENT_WIND_SHOCK, 4000);
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
            {
                _events.DelayEvent(EVENT_CYCLONE, 6000);
                _events.DelayEvent(EVENT_LIGHNING_BOLT, 6000);
                _events.DelayEvent(EVENT_WIND_SHOCK, 6000);
            }
        }
        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);
            if (auto _victim = me->getVictim())
                if (_victim && _victim->IsWithinLOSInMap(me) && me->GetDistance2d(_victim) <= 25.0f)
                    me->StopMoving();
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_CYCLONE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) _events.RescheduleEvent(_eventId, 250);
                    else
                    {
                        if (auto _target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(_target, SPELL_CYCLONE);
                        _events.RescheduleEvent(EVENT_CYCLONE, urand(18000, 20000));
                    }
                    break;
                case EVENT_LIGHNING_BOLT:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) _events.RescheduleEvent(_eventId, 250);
                    else
                    {
                        DoCastVictim(SPELL_LIGHNING_BOLT);
                        _events.RescheduleEvent(EVENT_LIGHNING_BOLT, 1500);
                    }
                    break;
                case EVENT_WIND_SHOCK:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) _events.RescheduleEvent(_eventId, 250);
                    else
                    {
                        DoCastRandom(SPELL_WIND_SHOCK, 25.0f, true);
                        _events.RescheduleEvent(EVENT_WIND_SHOCK, urand(5000, 6000));
                    }
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
    };
};

/*
    Armored Mistral - 45915(N) & 45916(H)
*/
class mob_armored_mistral : public CreatureScript
{
public:
    mob_armored_mistral() : CreatureScript("mob_armored_mistral") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_armored_mistralAI(creature);
    }
    struct mob_armored_mistralAI : public ScriptedAI
    {
        mob_armored_mistralAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            _events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(15.0f);
            _events.ScheduleEvent(EVENT_STORM_SURGE, urand(12000, 20000));
            _events.ScheduleEvent(EVENT_GALE_STRIKE, urand(4000, 8000));
        }
        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (!_wind && me->GetHealthPct() <= 30.0f)
            {
                _wind = true;
                _events.ScheduleEvent(EVENT_RISING_WINDS, 1000);
            }
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_GALE_STRIKE:
                    DoCast(SPELL_GALE_STRIKE);
                    _events.RescheduleEvent(EVENT_GALE_STRIKE, urand(13000, 17000));
                    break;
                case EVENT_STORM_SURGE:
                    DoCast(SPELL_STORM_SURGE);
                    _events.RescheduleEvent(EVENT_STORM_SURGE, urand(22000, 30000));
                    break;
                case EVENT_RISING_WINDS:
                    DoCast(SPELL_RISING_WINDS);
                    _events.RescheduleEvent(EVENT_RISING_WINDS, urand(1000, 1800));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        bool _wind = false;
    };
};

/*
    Cloud Prince - 45917(N) & 45918(H)
*/
class mob_cloud_prince : public CreatureScript
{
public:
    mob_cloud_prince() : CreatureScript("mob_cloud_prince") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_cloud_princeAI(creature);
    }
    struct mob_cloud_princeAI : public ScriptedAI
    {
        mob_cloud_princeAI(Creature* creature) : ScriptedAI(creature), _summon(me) { }
        void Reset()
        {
            _events.Reset();
            _summon.DespawnAll();
        }
        void JustDied(Unit* /*who*/)
        {
            _summon.DespawnAll();
        }
        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(22.0f);
            _events.ScheduleEvent(EVENT_STARFALL, 3500);
            _events.ScheduleEvent(EVENT_TYPHON, 5000);
            for (int i = 0; i < 4; i++)
                me->SummonCreature(NPC_WHIPPING_WIND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6000);
        }
        void JustSummoned(Creature* summon)
        {
            _summon.Summon(summon);
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;
            _events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
            if (uint32 _eventId = _events.ExecuteEvent())
            {
                switch (_eventId)
                {
                case EVENT_STARFALL:
                    DoCastVictim(SPELL_STARFALL);
                    _events.RescheduleEvent(EVENT_STARFALL, urand(8000, 10000));
                    break;
                case EVENT_TYPHON:
                    DoCastVictim(SPELL_TYPHON);
                    _events.RescheduleEvent(EVENT_TYPHON, urand(15000, 20000));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
        SummonList _summon;
    };
};


class spell_young_drake_chilling_blast : public SpellScriptLoader
{
public:
    spell_young_drake_chilling_blast() : SpellScriptLoader("spell_young_drake_chilling_blast") { }

    class spell_young_drake_chilling_blastSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_young_drake_chilling_blastSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                    cCreature->AI()->DoAction(EVENT_CHILLING_BLAST_PREP);
            return SPELL_CAST_OK;
        }
        void HandleAfterCast()
        {
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                    cCreature->AI()->DoAction(EVENT_CHILLING_BLAST_ORIENTATION);
        }
        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_young_drake_chilling_blastSpellScript::CheckCast);
            AfterCast += SpellCastFn(spell_young_drake_chilling_blastSpellScript::HandleAfterCast);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_young_drake_chilling_blastSpellScript();
    }
};

class spell_young_drake_healing_rain : public SpellScriptLoader
{
public:
    spell_young_drake_healing_rain() : SpellScriptLoader("spell_young_drake_healing_rain") { }

    class spell_young_drake_healing_rainSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_young_drake_healing_rainSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }
        void HandleAfterCast()
        {
            if (!checked)
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                {
                    checked = true;
                    cCreature->AI()->DoAction(EVENT_HEALING_RAIN_FINISHED);
                }
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_young_drake_healing_rainSpellScript::HandleAfterCast);
        }
    private:
        bool checked{ false };
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_young_drake_healing_rainSpellScript();
    }
};

class spell_VP_young_drake_cleave : public SpellScriptLoader
{
public:
    spell_VP_young_drake_cleave() : SpellScriptLoader("spell_VP_young_drake_cleave") { }    //

    class spell_VP_young_drake_cleave_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_VP_young_drake_cleave_SpellScript);

        void CountTargets(std::list<WorldObject*>& targetList)
        {
            _targetCount = targetList.size();
        }

        void ChangeDamage(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (caster && target && _targetCount)
            {
                int32 damage = GetHitDamage();
                damage /= _targetCount;
                SetHitDamage(damage);
            }
        }
    private:
        uint32 _targetCount;

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_VP_young_drake_cleave_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_VP_young_drake_cleave_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_VP_young_drake_cleave_SpellScript();
    }
};

void AddSC_vortex_pinnacle()
{
    new npc_slipstream();
    new mob_vortex_wind_rotation_manager();
    new mob_skyfall_star();
    new mob_golden_orb();
    // ST Scripts
    new mob_turbulent_squall();
    new mob_howling_gale();
    new mob_young_storm_dragon();
    new mob_grounding_field();
    new spell_grounding_field_vp();
    new mob_executor_of_the_caliph();
    new mob_minister_of_air();
    new mob_servant_of_asaad();
    new mob_temple_adept();
    new spell_lightning_lash_vp();
    new mob_gust_soldier();
    new mob_wild_vortex();
    new mob_armored_mistral();
    new mob_cloud_prince();
    new spell_grounding_field_mechanic();
    new spell_young_drake_chilling_blast();
    new spell_young_drake_healing_rain();
    new spell_VP_young_drake_cleave();
}