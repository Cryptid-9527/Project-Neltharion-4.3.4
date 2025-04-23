/*
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
SDName: boss_Akilzon
SD%Complete: 0 %
SDComment: Missing timer for Call Lightning and Sound ID's
SQLUpdate:

EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "zulaman.h"
#include "Weather.h"

enum Spells
{
    // enter combat 21:56:37.967
    SPELL_STATIC_DISRUPTION             = 43622, //21:56:44.005 trigger SPELL_STATIC_DISRUPTION_TRIGGER 21:56:56.048 21:57:08.107 21:57:20.181
    SPELL_STATIC_DISRUPTION_TRIGGER     = 44008, //97298, trigger SPELL_STATIC_VISUAL
    SPELL_STATIC_VISUAL                 = 45265,

    SPELL_CALL_LIGHTNING                = 43661, // 97299 21:56:45.206 21:56:53.646 21:57:09.324 21:57:17.763

    SPELL_GUST_OF_WIND                  = 43621, // 97319,  21:56:46.423  21:57:10.525
    SPELL_TAKE_PASSENGER                = 98869, // casted on NPC_AMANI_KIDNAPPER by players 21:56:50.042 21:57:14.160
    SPELL_PLAYER_PLUCKED                = 97318, // casted on NPC_AMANI_KIDNAPPER by players 21:56:50.042 21:57:14.160
    SPELL_PLAYER_PLUCKED_RIDING         = 97319,

    SPELL_ELECTRIC_OVERLOAD_PRIMER      = 44735, // script effect SPELL_ELECTRICAL_STORM dummy SPELL_PLAYER_PLUCKED 21:57:24.565
    SPELL_ELECTRICAL_STORM              = 43648, // script effect 44734(not exist) trigger SPELL_ELECTRICAL_OVERLOAD_0
    SPELL_ELECTRICAL_OVERLOAD_0         = 43658, //97300, trigger SPELL_ELECTRICAL_OVERLOAD_1
    SPELL_ELECTRICAL_OVERLOAD_1         = 43657,
    SPELL_ELECTRIC_STORM_DUMMY          = 44007,
    SPELL_AUTO_TELE_CASTER              = 44006,
    SPELL_ELECTRIC_ARC                  = 43653,

    SPELL_BERSERK                       = 45078,

    // MOB_SOARING_EAGLE
    SPELL_EAGLE_SWOOP                   = 44732 // casted by 24858 21:56:52.039 21:56:54.457
};

enum Events
{
    EVENT_STATIC_DISRUPTION = 1,
    EVENT_CALL_LIGHTNING,
    EVENT_ELECTRICAL_STORM_PRE,
    EVENT_ELECTRICAL_STORM,
    EVENT_ELECTRICAL_STORM_END,
    EVENT_GUST_OF_WIND,
    EVENT_AKIL_EAGLE,
    EVENT_BERSERK,

    EVENT_MOVE_TO_TARGET,
    EVENT_MOVE_AROUND_CIRCLE,
    EVENT_CHECK_WIPE,

};

enum Says
{
    SAY_AGGRO                   = 0,
    SAY_SUMMON                  = 1,
    SAY_INTRO                   = 2, // Not used in script
    SAY_ENRAGE                  = 3,
    SAY_KILL                    = 4,
    SAY_DEATH                   = 5
};

enum Misc
{
    SE_LOC_X_MAX                = 400,
    SE_LOC_X_MIN                = 335,
    SE_LOC_Y_MAX                = 1435,
    SE_LOC_Y_MIN                = 1370,
    POINT_MOVE_TARGET           = 1,
    NPC_AMANI_KIDNAPPER         = 52638,
    NPC_AKILZON_EAGLE           = 24858,
    ACTION_START_FIGHT          = 1,
    ACTION_AGGRESSIVE           = 2,
};

static const Position centerPos = {376.35f, 1407.34f, 92.38f, 0.0f};

static const Position akilEagleSpawn[8] =
{
    {355.0323f, 1411.0f, 91.6518f},
    {356.8288f, 1422.785f, 91.71573f},
    {368.7743f, 1377.933f, 91.78314f},
    {361.3212f, 1380.682f, 91.76196f},
    {396.1938f, 1398.954f, 91.53065f},
    {395.2363f, 1391.221f, 91.78426f},
    {386.3385f, 1430.294f, 92.51662f},
    {379.344f, 1434.552f, 91.64027f},
};

class boss_akilzon : public CreatureScript
{
    public:
        boss_akilzon() : CreatureScript("boss_akilzon") { }

        struct boss_akilzonAI : public BossAI
        {
            boss_akilzonAI(Creature* creature) : BossAI(creature, DATA_AKILZONEVENT), summons(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset()
            {
                _Reset();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                SetWeather(WEATHER_STATE_FINE, 0.0f);
                me->SetReactState(REACT_DEFENSIVE);
                events.Reset();
                summons.DespawnAll();
            }


            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            bool checkWipe()
            {
                auto players = me->GetPlayersInRange(200.f, true);
                for (Player* player : players)
                    if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                        return false;
                return true;
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                SetWeather(WEATHER_STATE_FOG, 0.0f);
                Talk(SAY_AGGRO);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                events.Reset();
                events.ScheduleEvent(EVENT_STATIC_DISRUPTION, 4000);
                events.ScheduleEvent(EVENT_CALL_LIGHTNING, 8000);
                events.ScheduleEvent(EVENT_ELECTRICAL_STORM_PRE, 37000);
                events.ScheduleEvent(EVENT_GUST_OF_WIND, 14000);
                events.ScheduleEvent(EVENT_AKIL_EAGLE, 18000);
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
            }

            void JustDied(Unit* /*killer*/)
            {
                SetWeather(WEATHER_STATE_FINE, 0.0f);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _JustDied();
                Talk(SAY_DEATH);
                summons.DespawnAll();
                me->DeleteThreatList();


                if (auto bakkalzu = me->FindNearestCreature(NPC_HOSTAGE_1, 200.f, true))
                    bakkalzu->AI()->DoAction(2);


                std::list<Creature*> units;

                GetCreatureListWithEntryInGrid(units, me, NPC_AKILZON_EAGLE, 100.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_AMANI_KIDNAPPER, 100.0f);
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                    if (auto c = (*itr))
                    {
                        c->RemoveAllAuras();
                        if (auto v = c->GetVehicleKit())
                            v->RemoveAllPassengers();
                        c->DeleteThreatList();
                        c->DespawnOrUnsummon();
                    }
            }

            void KilledUnit(Unit* /*victim*/)
            {
                Talk(SAY_KILL);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_AGGRESSIVE)
                {
                    SetWeather(WEATHER_STATE_FOG, 0.0f);
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.CancelEvent(EVENT_ELECTRICAL_STORM_END);
                    UpdateVictim();
                }
            }

            void SetWeather(uint32 weather, float grade)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return;

                WorldPacket data(SMSG_WEATHER, (4+4+1));
                data << uint32(weather) << float(grade) << uint8(0);

                map->SendToPlayers(&data);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!me->isInCombat())
                    if (auto player = me->FindNearestPlayer(15.0f))
                        if (me->ToUnit()->IsValidAttackTarget(player->ToUnit()))
                        {
                            me->AI()->EnterCombat(player->ToUnit());
                        }
                        else
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_WIPE:
                            //TC_LOG_ERROR("sql.sql", "Executing check wipe");
                            if (checkWipe())
                            {
                                //TC_LOG_ERROR("sql.sql", "Wipe recognized at glubtok.");
                                me->DeleteThreatList();
                                me->CombatStop(true);
                                Reset();
                                me->Respawn(true);
                                me->DeleteThreatList();
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                            break;
                        case EVENT_STATIC_DISRUPTION:
                            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 1, 100.0f, true, -SPELL_PLAYER_PLUCKED))
                                me->CastSpell(target, SPELL_STATIC_DISRUPTION, false);
                            events.ScheduleEvent(EVENT_STATIC_DISRUPTION, 12000);
                            break;
                        case EVENT_CALL_LIGHTNING:
                            DoCastVictim(SPELL_CALL_LIGHTNING);
                            events.ScheduleEvent(EVENT_CALL_LIGHTNING, 8000);
                            break;
                        case EVENT_GUST_OF_WIND:
                        {
                            auto list_of_players = me->GetPlayersInRange(100.f, true);
                            if (list_of_players.size())
                                for (auto itr = list_of_players.begin(); itr != list_of_players.end(); ++itr)
                                    if (auto t = (*itr))
                                        if (t->HasAura(SPELL_PLAYER_PLUCKED) || t->HasAura(SPELL_PLAYER_PLUCKED_RIDING) || t->HasAura(SPELL_ELECTRICAL_STORM))//filter condition
                                            list_of_players.erase(itr);

                            if (list_of_players.size())
                                if (auto v = me->getVictim())
                                    if (auto p = v->ToPlayer())
                                        list_of_players.remove(p);

                            if (list_of_players.size())
                                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                                    me->CastSpell(random_target, SPELL_GUST_OF_WIND, false);
                                
                            events.ScheduleEvent(EVENT_GUST_OF_WIND, 30000);
                        }
                            break;
                        case EVENT_ELECTRICAL_STORM_PRE:
                            SetWeather(WEATHER_STATE_MEDIUM_RAIN, 0.75f);
                            events.ScheduleEvent(EVENT_ELECTRICAL_STORM_PRE, 43000);
                            events.ScheduleEvent(EVENT_ELECTRICAL_STORM, 7000);
                            break;
                        case EVENT_ELECTRICAL_STORM:
                        {
                            auto list_of_players = me->GetPlayersInRange(100.f, true);
                            if (list_of_players.size())
                                for (auto itr = list_of_players.begin(); itr != list_of_players.end(); ++itr)
                                    if (auto t = (*itr))
                                        if (t->HasAura(SPELL_PLAYER_PLUCKED) || t->HasAura(SPELL_PLAYER_PLUCKED_RIDING) || t->HasAura(SPELL_ELECTRICAL_STORM))//filter condition
                                            list_of_players.erase(itr);

                            if (list_of_players.size())
                                if (auto v = me->getVictim())
                                    if (auto p = v->ToPlayer())
                                        list_of_players.remove(p);

                            if (list_of_players.size())
                                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                                {
                                    me->SetReactState(REACT_PASSIVE);
                                    me->StopMoving();
                                    float lock_o{ me->GetAngle(random_target) };
                                    DoCast(random_target, SPELL_ELECTRIC_OVERLOAD_PRIMER);
                                    me->SetFacingTo(lock_o);
                                }
                            events.ScheduleEvent(EVENT_ELECTRICAL_STORM_END, 8000);
                            break;
                        }
                        case EVENT_ELECTRICAL_STORM_END:
                            SetWeather(WEATHER_STATE_FOG, 0.0f);
                            me->SetReactState(REACT_AGGRESSIVE);
                            UpdateVictim();
                            break;
                        case EVENT_AKIL_EAGLE:
                        {
                            int count{ 8 };
                            for (int i = 0; i < count; i++)
                            {
                                Position pos(me->GetHomePosition());
                                float angle{ (M_PI * 2 / float(count)) * float(i) };
                                me->MovePosition(pos, frand(28.f, 35.f), angle, true);
                                pos.m_positionZ = frand(80.f, 95.f);


                                if (auto c = me->SummonCreature(NPC_AKILZON_EAGLE, akilEagleSpawn[i]))
                                    c->AI()->DoAction(ACTION_START_FIGHT);
                            }
                        }
                            break;
                        case EVENT_BERSERK:
                            DoCast(SPELL_BERSERK);
                            break;
                    }
                }
                else
                {
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
                }
            }

            private:
                InstanceScript* instance;
                EventMap events;
                SummonList summons;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_akilzonAI(creature);
        }
};

class mob_akilzon_eagle : public CreatureScript
{
    public:
        mob_akilzon_eagle() : CreatureScript("mob_akilzon_eagle") { }

        struct mob_akilzon_eagleAI : public ScriptedAI
        {
            mob_akilzon_eagleAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetSpeed(MOVE_RUN, 1.2f);
                me->SetVisible(false);
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                events.Reset();
                ray = 15.0f;
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/)
            {
                me->GetMotionMaster()->MoveFall();
                me->DespawnOrUnsummon(5000);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_FIGHT)
                {
                    ray = me->GetExactDist2d(centerPos.m_positionX, centerPos.m_positionY);
                    me->SetVisible(true);
                    events.ScheduleEvent(EVENT_MOVE_AROUND_CIRCLE, 1000);
                    events.ScheduleEvent(EVENT_MOVE_TO_TARGET, urand(2000, 15000));
                }
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != POINT_MOVE_TARGET)
                    return;
                if (TargetGUID)
                {
                    if (auto target = Unit::GetUnit(*me, TargetGUID))
                        DoCast(target, SPELL_EAGLE_SWOOP, true);
                    TargetGUID = 0;
                    events.ScheduleEvent(EVENT_MOVE_AROUND_CIRCLE, 0);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_TO_TARGET:
                        {
                            if (Creature *boss = me->FindNearestCreature(23574, 100.0f))
                                if (Unit *target = boss->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0))
                                {
                                    me->GetMotionMaster()->Clear();
                                    me->DisableSpline();
                                    TargetGUID = target->GetGUID();
                                    Position pos;
                                    target->GetPosition(&pos);
                                    me->GetMotionMaster()->MovePoint(POINT_MOVE_TARGET, pos, false, 25.f);
                                }
                            events.ScheduleEvent(EVENT_MOVE_TO_TARGET, urand(5000, 25000));
                            break;
                        }
                        case EVENT_MOVE_AROUND_CIRCLE:
                        {
                            me->GetMotionMaster()->MoveAroundPoint(centerPos, ray, urand(0, 1), uint8(300), 92.0f, true);
                            break;
                        }
                    }
                }
            }

        private:
            EventMap events;
            float ray;
            uint64 TargetGUID;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_akilzon_eagleAI(creature);
        }
};

class npc_amani_kidnapper : public CreatureScript
{
    public:
        npc_amani_kidnapper() : CreatureScript("npc_amani_kidnapper") { }

        struct npc_amani_kidnapperAI : public ScriptedAI
        {
            npc_amani_kidnapperAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                targetGUID = 0;
                me->SetReactState(REACT_PASSIVE);
                me->DisableMovementFlagUpdate(true);
                me->SetDisableGravity(true);
                me->SetCanFly(true);
            }

            void EnterCombat(Unit* /*who*/)
            {
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/)
            {
                me->GetMotionMaster()->MoveFall();
                me->DespawnOrUnsummon(5000);
            }

            void IsSummonedBy(Unit* summoner)
            {
                if (auto akilzon = me->FindNearestCreature(NPC_AKILZON, 100.f))
                    akilzon->AI()->JustSummoned(me);

                Position eagle_Spawn
                    {391.855225f, 1452.547729f, 128.601654f, 3.216211f};

                me->NearTeleportTo(eagle_Spawn);

                targetGUID = summoner->GetGUID();
                events.ScheduleEvent(EVENT_MOVE_AROUND_CIRCLE, 100);
                events.ScheduleEvent(EVENT_MOVE_TO_TARGET, 10000);
            }

            void JustReachedHome()
            {
            }

            void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
            {
                if (apply)
                    me->GetMotionMaster()->MovePoint(50, me->GetHomePosition(), false, 20.f);
                else
                {
                    Position Jump_To{ centerPos };
                    if (auto akilzon = me->FindNearestCreature(NPC_AKILZON, 100.f))
                    {
                        Jump_To = *akilzon;
                        akilzon->MoveBlink(Jump_To, 10.f, akilzon->GetRelativeAngleOffset(me));

                    }
                    if (auto p = passenger)
                    {
                        p->GetMotionMaster()->Clear();
                        p->GetMotionMaster()->ForceMoveJump(Jump_To.GetPositionX(), Jump_To.GetPositionY(), Jump_To.GetPositionZ(), 12.f, 8.f);
                    }
                    me->DespawnOrUnsummon(200);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_MOVE_TARGET)
                if (auto target = Unit::GetUnit(*me, targetGUID))
                    if (target->HasAura(SPELL_PLAYER_PLUCKED_RIDING))
                    {
                        me->DespawnOrUnsummon();
                    }
                    else
                    target->CastSpell(me, SPELL_PLAYER_PLUCKED, true);

                if (id == 50)
                    events.ScheduleEvent(EVENT_MOVE_AROUND_CIRCLE, 1);
            }

            void UpdateAI(uint32 const diff)
            {
                // SPELL_PLAYER_PLUCKED
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_TO_TARGET:
                        {
                            me->SetHomePosition({412.447144f, 1417.234375f, 92.020645f, 3.430440f});
                            me->SetSpeed(MOVE_FLIGHT, 0.8f);
                            me->SetSpeed(MOVE_RUN, 0.8f);
                            me->SetSpeed(MOVE_WALK, 0.8f);

                            if (auto target = Unit::GetUnit(*me, targetGUID))
                                me->GetMotionMaster()->MovePoint(POINT_MOVE_TARGET, *target);
                            else
                                me->DespawnOrUnsummon();
                            break;
                        }
                        case EVENT_MOVE_AROUND_CIRCLE:
                        {
                            me->GetMotionMaster()->MoveAroundPoint(centerPos, me->GetDistance(centerPos), urand(0, 1), uint8(50), me->GetPositionZ(), true);
                            break;
                        }
                    }
                }
            }

        private:
            EventMap events;
            uint64 targetGUID;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_amani_kidnapperAI(creature);
        }
};

// 44735
class spell_electric_overload_primer : public SpellScriptLoader
{
public:
    spell_electric_overload_primer() : SpellScriptLoader("spell_electric_overload_primer")
    {
    }

    class spell_electric_overload_primer_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_electric_overload_primer_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
            {
                //    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_AUTO_TELE_CASTER, true);
                GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true); // 43648 add aura ?
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_electric_overload_primer_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_electric_overload_primer_SpellScript();
    }
};

// 43648
class spell_electric_storm : public SpellScriptLoader
{
public:
    spell_electric_storm() : SpellScriptLoader("spell_electric_storm")
    {
    }

    class spell_electric_storm_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_electric_storm_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto target = GetTarget())
            {


                float o = target->GetOrientation();
                float z1 = target->GetPositionZ() + 10.0f;
                float z2 = target->GetPositionZ() + 15.0f;
                float x = target->GetPositionX();
                float y = target->GetPositionY();
                target->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), z1, target->GetOrientation());
                target->GetMotionMaster()->ForceMoveJump(x, y, z2, 0.4f, 0.1f);
                target->AddUnitState(UNIT_STATE_STUNNED);

                //target->SetRooted(true);
                //target->SetDisableGravity(true);
                //target->SetCanFly(true);
                // SMSG_SET_PLAY_HOVER_ANIM
                target->CastSpell(target, SPELL_ELECTRIC_STORM_DUMMY, true);
                target->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT | MOVEMENTFLAG_CAN_FLY);
                target->SendMovementCanFlyChange();
            }
        }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto caster = GetCaster())
            if (auto target = GetTarget())
            {
                target->ClearUnitState(UNIT_STATE_STUNNED);
                //target->SetDisableGravity(false);
                //target->SetCanFly(false);
                //target->SetRooted(false);
                target->RemoveUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_ROOT | MOVEMENTFLAG_CAN_FLY);
                target->SendMovementCanFlyChange();
                target->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), caster->GetPositionZ() + 10.f, target->GetOrientation());
                target->CastSpell(target, 53700, true);
                target->GetMotionMaster()->Clear();
                if (auto p = target->ToPlayer())
                    p->SetSkipOnePacketForASH(true);
                target->GetMotionMaster()->MoveFall();

            }

            if (auto c = GetCaster())
                if (auto cr = c->ToCreature())
                    cr->AI()->DoAction(ACTION_AGGRESSIVE);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_electric_storm_AuraScript::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_electric_storm_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_electric_storm_AuraScript();
    }
};

// 43657
class spell_arcane_storm_damages : public SpellScriptLoader
{
public:
    spell_arcane_storm_damages() : SpellScriptLoader("spell_arcane_storm_damages") { }

    class DistanceCheck
    {
    public:
        explicit DistanceCheck(Unit* _caster) : caster(_caster) { }

        bool operator() (WorldObject* unit) const
        {
            if (caster->GetExactDist2d(unit) <= 5.0f)
                return true;
            return false;
        }

        Unit* caster;
    };


    class spell_arcane_storm_damages_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_arcane_storm_damages_SpellScript);

        bool Load()
        {
            return GetCaster()->GetTypeId() == TYPEID_PLAYER;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
                return;
            if (GetCaster())
            {
                targets.remove_if(DistanceCheck(GetCaster()));
                targets.remove(GetCaster());
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcane_storm_damages_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_arcane_storm_damages_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_arcane_storm_damages_SpellScript();
    }
};

// 98869
class spell_eagle_take_passenger : public SpellScriptLoader
{
public:
    spell_eagle_take_passenger() : SpellScriptLoader("spell_eagle_take_passenger")
    {
    }

    class spell_eagle_take_passenger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_eagle_take_passenger_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true); // SPELL_PLAYER_PLUCKED
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_eagle_take_passenger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_eagle_take_passenger_SpellScript();
    }
};

class mob_akilzon_pre_event : public CreatureScript
{
    public:
        mob_akilzon_pre_event() : CreatureScript("mob_akilzon_pre_event") { }

        struct mob_akilzon_pre_eventAI : public ScriptedAI
        {
            mob_akilzon_pre_eventAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                _prGUID = 0;
            }

            bool CanSeeAlways(WorldObject const* obj)
            {
                if (auto p = obj->ToPlayer())
                    return !(p->isGameMaster() || !p->isGMVisible());

                return false;
            }

            void EnterCombat(Unit* /*who*/)
            {
                if (!me->IsVisible())
                {
                    me->DeleteThreatList();
                    me->DespawnOrUnsummon();
                    return;
                }

                me->DeleteThreatList();
                me->SetReactState(REACT_PASSIVE);
                Talk(0);
                me->GetMotionMaster()->MovePath(me->GetEntry() * 100, false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            }
            void MoveInLineOfSight(Unit* who)
            {
                if (!me->isInCombat())
                    if (who->ToPlayer())
                        if (me->GetDistance(who) < 30.f)
                            me->SetInCombatWith(who);
            }

            void JustSummoned(Creature *summon)
            {
                if (summon->GetEntry() == 24159)
                    summon->GetMotionMaster()->MovePath(summon->GetEntry() * 100, false);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != WAYPOINT_MOTION_TYPE || id != 5)
                    return;
                me->SetHomePosition(*me);
                me->DeleteThreatList();
                me->SetVisible(false);
                if (Creature *pr = me->FindNearestCreature(24549, 100))
                {
                    _prGUID = pr->GetGUID();
                    pr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pr->SetReactState(REACT_AGGRESSIVE);
                }
                events.ScheduleEvent(EVENT_MOVE_TO_TARGET, 5000);
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_TO_TARGET:
                        {
                            if (Unit *pr = Unit::GetUnit(*me, _prGUID))
                            {
                                if (!pr->isAlive())
                                {
                                    me->DespawnOrUnsummon();
                                    return;
                                }
                            }
                            else
                                return;
                            for (int i = 0; i < 5; i++)
                                me->CastSpell(me, 43487, true);
                            events.ScheduleEvent(EVENT_MOVE_TO_TARGET, 60000);
                            break;
                        }
                    }
                }
            }

        private:
            EventMap events;
            uint64 _prGUID;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_akilzon_pre_eventAI(creature);
        }
};

void AddSC_boss_akilzon()
{
    new boss_akilzon();
    new mob_akilzon_eagle();
    new npc_amani_kidnapper();
    new spell_electric_overload_primer();
    new spell_electric_storm();
    new spell_arcane_storm_damages();
    new spell_eagle_take_passenger();

    new mob_akilzon_pre_event();
}
