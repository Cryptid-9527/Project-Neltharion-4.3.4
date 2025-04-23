/*
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

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "the_stonecore.h"

/*
 * TODO: fix jump animation (jumps every time to the tank... damage part works fine)
 */

enum Spells
{
    SPELL_CLEARALLDEBUFFS           = 34098,
    SPELL_CRYSTAL_BARRAGE           = 86881,
    SPELL_CRYSTAL_CHARGE_HC         = 92012,
    SPELL_DAMPENING_WAVE            = 82415,
    SPELL_SUBMERGE                  = 81629,
    SPELL_THRASHING_CHARGE_TELEPORT = 81839,
    SPELL_THRASHING_CHARGE_VISUAL          = 81801, // dummy / visual
    SPELL_THRASHING_CHARGE_SUMMON   = 81816,
    SPELL_THRASHING_CHARGE_DAMAGE   = 81828, // casted by Trigger
    SPELL_EMERGE                    = 82185, // Rock Borer
    SPELL_EMERGE_CORBORUS           = 81948,
    SPELL_CRYSTAL_SHARDS_AURA       = 80895,
    SPELL_CRYSTAL_SHARDS_TARGET     = 80912,
    SPELL_CRYSTAL_SHARDS_DAMAGE     = 92122,
    SPELL_CORBORUS_CHARGE           = 81237, // 07:43:25.00
    SPELL_CORBORUS_KNOCK_BACK       = 81235, // 07:56:56.00

    SPELL_RANDOM_AGGRO              = 92111
};

enum Phases
{
    PHASE_NORMAL                    = 1,
    PHASE_SUBMERGED
};

enum Events
{
    EVENT_CRYSTAL_BARRAGE           = 1,
    EVENT_DAMPENING_WAVE,
    EVENT_SUBMERGE,
    EVENT_EMERGE,
    EVENT_EMERGE_END,
    EVENT_THRASHING_CHARGE,
    EVENT_THRASHING_CHARGE_CAST,
    EVENT_SET_ACTIVE,
    EVENT_TELEPORT,
    EVENT_SPAWN_MILLHOUSE,
    EVENT_FACE_PLAYERS
};

enum misc
{
    ACTION_START_AT    = 1,
    ACTION_SET_VISIBLE = 2,
    POINT_HOME         = 3
};

class boss_corborus : public CreatureScript
{
public:
    boss_corborus() : CreatureScript("boss_corborus") {}

    struct boss_corborusAI : public BossAI
    {
        boss_corborusAI(Creature* creature) : BossAI(creature, DATA_CORBORUS)
        {
            started = false;
            checkSkipTimer = 0;
        }
        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
            me->RemoveAllAuras();
            summons.DespawnAll();
            me->DeleteThreatList();
            if (instance)
            {
                instance->SetBossState(DATA_CORBORUS, DONE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
            }
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHARDS_DAMAGE);
            instance->DoRemoveAurasDueToSpellOnPlayers(80028);
            instance->DoRemoveAurasDueToSpellOnPlayers(92630);
            me->DespawnCreaturesInArea(NPC_ROCK_BORER);
            //me->DespawnCreaturesInArea(NPC_ROCK_BORER2);
        }
        void Reset()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
            thrashingCharges = 0;
            sumGUID = 0;
            events.SetPhase(PHASE_NORMAL);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_AGGRESSIVE);
            me->SetFloatValue(UNIT_FIELD_COMBATREACH, 12.0f);
            _Reset();
            me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            events.SetPhase(0);
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
                if (id == 99)
                {
                    me->HandleEmoteCommand(400);
                }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_START_AT:
                    if (!started)
                    {
                        started = true;
                        DoCast(SPELL_CORBORUS_CHARGE);
                        events.ScheduleEvent(EVENT_FACE_PLAYERS, 4000);
                        me->SetHomePosition(1147.33f, 882.51f, 284.97f, 0.0f);
                        me->GetMotionMaster()->MovePoint(99, me->GetHomePosition());
                    }
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                default:
                    break;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Remove
            _EnterCombat();
            me->SetSpeed(MOVE_WALK, 1.5f);
            events.SetPhase(PHASE_NORMAL);
            events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE, urand(8000, 10000), 0, PHASE_NORMAL);
            events.ScheduleEvent(EVENT_DAMPENING_WAVE, urand(5000, 8000), 0, PHASE_NORMAL);
            events.ScheduleEvent(EVENT_SUBMERGE, 30000, 0, PHASE_NORMAL);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
                case NPC_THRASHING_CHARGE:
                    DoCast(me, SPELL_THRASHING_CHARGE_VISUAL, false);
                    summon->CastSpell(summon, SPELL_THRASHING_CHARGE_DAMAGE, false);
                    break;
                case NPC_ROCK_BORER:
                    summon->CastSpell(summon, SPELL_EMERGE, false);
                    break;
                case NPC_CRYSTAL_SHARD:
                    break;
                default:
                    break;
            }
        }

        bool CanSeeAlways(WorldObject const* obj) 
        { 
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        void UpdateAI(uint32 const diff)
        {
            if (started && !me->isInCombat())
            {
                checkSkipTimer += diff;
                if (checkSkipTimer >= 300)
                {
                    checkSkipTimer = 0;
                    if (auto trigger = me->FindNearestCreature(22515, 100.f))
                    {
                        Map::PlayerList const& players = instance->instance->GetPlayers();
                        if (!players.isEmpty())
                            for (auto itr = players.begin(); itr != players.end(); ++itr)
                                if (auto player = itr->getSource())
                                    if (player->isAlive() && !player->isGameMaster() && (player->GetDistance(trigger) <= 20.f))
                                        me->CombatStart(player);
                    }
                }
            }

            if(!UpdateVictim() && !events.IsInPhase(0))
                return;

            events.Update(diff);

            if(me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SPAWN_MILLHOUSE:
                        //what lmao
                        /*
                        me->SummonCreature(NPC_MILLHOUSE_MANASTORM, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        */
                        break;
                    case EVENT_FACE_PLAYERS:
                        me->SetFacingTo(3.176499f);
                        break;
                    case EVENT_CRYSTAL_BARRAGE:
                    {
                        auto players = me->GetPlayersInRange(80.f, true);

                        players.sort(Trinity::ObjectDistanceOrderPred(me));
                        if (auto target = players.back())
                        {
                            DoCast(target, SPELL_CRYSTAL_BARRAGE);
                            //me->SetFacingTo(me->GetAngle(target));
                        }

                        events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE, urand(10000, 12000), 0, PHASE_NORMAL);
                        break;
                    }
                    case EVENT_DAMPENING_WAVE:
                        DoCast(SPELL_DAMPENING_WAVE);
                        events.ScheduleEvent(EVENT_DAMPENING_WAVE, urand(10000, 12000), 0, PHASE_NORMAL);
                        break;
                    case EVENT_SUBMERGE:
                        DoCast(SPELL_CLEARALLDEBUFFS);
                        thrashingCharges = 0;
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->AttackStop();

                        DoCast(SPELL_SUBMERGE);
                        events.SetPhase(PHASE_SUBMERGED);
                        events.RescheduleEvent(EVENT_TELEPORT, 3000, 0, PHASE_SUBMERGED);
                        break;
                    case EVENT_EMERGE:
                        events.SetPhase(PHASE_NORMAL);
                        events.RescheduleEvent(EVENT_DAMPENING_WAVE, 3000, 0, PHASE_NORMAL);
                        events.RescheduleEvent(EVENT_CRYSTAL_BARRAGE, 4500, 0, PHASE_NORMAL);
                        events.ScheduleEvent(EVENT_EMERGE_END, 2500, 0, PHASE_NORMAL);
                        DoCast(SPELL_EMERGE_CORBORUS);
                        break;
                    case EVENT_EMERGE_END:
                        me->RemoveAllAuras();
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 12.0f);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        if(Unit* victim = me->getVictim())
                            DoStartMovement(victim);
                        events.ScheduleEvent(EVENT_SUBMERGE, 60000, 0, PHASE_NORMAL);
                        break;
                    case EVENT_TELEPORT:
                        DoCastRandom(SPELL_THRASHING_CHARGE_TELEPORT, 100.0f);
                        ++thrashingCharges;
                        if (thrashingCharges >= 3)
                            events.ScheduleEvent(EVENT_EMERGE, 1000, 0, PHASE_SUBMERGED);
                        else
                            events.ScheduleEvent(EVENT_THRASHING_CHARGE, 1000, 0, PHASE_SUBMERGED);
                        break;
                    case EVENT_THRASHING_CHARGE:
                        DoCast(me, SPELL_THRASHING_CHARGE_SUMMON, true);
                        events.ScheduleEvent(EVENT_TELEPORT, 5000, 0, PHASE_SUBMERGED);
                        break;
                    default:
                        break;
                }
            }

            if(events.IsInPhase(PHASE_NORMAL))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                DoMeleeAttackIfReady();

            EnterEvadeIfOutOfCombatArea(diff);
        }

    private:
        uint8 thrashingCharges;
        bool started;
        uint64 sumGUID;
        uint32 checkSkipTimer;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new boss_corborusAI(creature);
    }
};

class spell_crystal_barrage : public SpellScriptLoader
{
public:
        spell_crystal_barrage() : SpellScriptLoader("spell_crystal_barrage") { }

        class spell_crystal_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_crystal_barrage_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                if (auto aura = aurEff->GetBase())
                    if (auto o = aura->GetOwner())
                        if (auto caster = o->ToUnit())
                            if (auto v = caster->getVictim())
                            {
                                Position target_dest(*v);

                                v->MoveBlink(target_dest, frand(0.f, 4.f), frand(0.f, (M_PI * 2.f)));

                                caster->CastSpell(
                                    target_dest.GetPositionX(),
                                    target_dest.GetPositionY(),
                                    target_dest.GetPositionZ(), 81637, true, NULL, NULL, NULL);

                                if (caster->GetMap()->IsHeroic())
                                if (aurEff->GetTickNumber() % 4 == 0)
                                    caster->CastSpell(
                                        target_dest.GetPositionX(),
                                        target_dest.GetPositionY(),
                                        target_dest.GetPositionZ(), SPELL_CRYSTAL_CHARGE_HC, true, NULL, NULL, NULL);
                            }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_crystal_barrage_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_crystal_barrage_AuraScript();
        }
};

class npc_crystal_shard : public CreatureScript
{
public:
    npc_crystal_shard() : CreatureScript("npc_crystal_shard") {}

    struct npc_crystal_shardAI : public BossAI
    {
        npc_crystal_shardAI(Creature * creature) : BossAI(creature, DATA_CORBORUS)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->AddAura(93403, me);
        }

        void Reset()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_SET_ACTIVE, 2500);
        }

        void SpellHitTarget(Unit * /*target*/, const SpellInfo * spell)
        {
            if(spell->Id == SPELL_CRYSTAL_SHARDS_TARGET)
            {
                DoCast(SPELL_CRYSTAL_SHARDS_DAMAGE);
                me->DespawnOrUnsummon(250);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            if(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_SET_ACTIVE:
                        me->RemoveAurasDueToSpell(93403);
                        DoCast(SPELL_CRYSTAL_SHARDS_AURA);
                        events.ScheduleEvent(EVENT_SET_ACTIVE+1, 500);
                        me->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        break;
                    case EVENT_SET_ACTIVE + 1:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetInCombatWithZone();
                        DoCast(SPELL_RANDOM_AGGRO);
                        break;
                }
            }
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_crystal_shardAI(creature);
    }
};

class AreaTrigger_at_rockdoor_break : public AreaTriggerScript
{
    public:
        AreaTrigger_at_rockdoor_break() : AreaTriggerScript("at_rockdoor_break") {}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isGameMaster() || !player->isGMVisible())
                return false;

            if (auto instance = player->GetInstanceScript())
            {
                if (auto corborus = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_CORBORUS)))
                    if (auto millhouse = corborus->FindNearestCreature(NPC_MILLHOUSE_MANASTORM, 300.f, true))
                        if (millhouse->GetDistance(*corborus) < 80.f)
                        {
                            if (auto go = ObjectAccessor::GetGameObject(*player, instance->GetData64(DATA_ROCKDOOR)))
                            {
                                if (go->getLootState() != GO_READY)
                                    return false;
                                else
                                {
                                    go->UseDoorOrButton();
                                    go->EnableCollision(false);
                                }
                            }

                            std::list<Creature*> eventCr;
                            millhouse->GetCreatureListWithEntryInGrid(eventCr, 43391, 30.0f);
                            millhouse->GetCreatureListWithEntryInGrid(eventCr, 43537, 30.0f);
                            millhouse->GetCreatureListWithEntryInGrid(eventCr, 42696, 30.0f);
                            millhouse->GetCreatureListWithEntryInGrid(eventCr, 43430, 30.0f);
                            for (auto itr = eventCr.begin(); itr != eventCr.end(); ++itr)
                                if (auto c = (*itr))
                                {
                                    c->SetFacingTo(1.641477f);
                                    c->HandleEmoteCommand(45);
                                }

                            if (corborus->isAlive())
                            {
                                corborus->SetUInt64Value(UNIT_FIELD_TARGET, millhouse->GetGUID());
                                corborus->AI()->DoAction(ACTION_START_AT);
                                millhouse->AI()->DoAction(9);
                            }
                            else
                                millhouse->DespawnOrUnsummon();
                            return true;
                        }
            }

            return false;
        }
};

class spell_corborus_sum_triggered : public SpellScriptLoader
{
public:
    spell_corborus_sum_triggered() : SpellScriptLoader("spell_corborus_sum_triggered")
    {
    }

    class script_impl : public SpellScript
    {
        PrepareSpellScript(script_impl);

        bool Load()
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (unitList.empty())
                return;
            std::list<WorldObject*>::const_iterator itr = unitList.begin();
            advance(itr, rand() % unitList.size());
            if (WorldObject * tar = *itr)
            {
                unitList.clear();
                unitList.push_back(tar);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(script_impl::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new script_impl();
    }

};

class spell_crystal_spawn : public SpellScriptLoader
{
public:
    spell_crystal_spawn() : SpellScriptLoader("spell_crystal_spawn") { }
    /*
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('81637', 'spell_crystal_spawn');
    */
    class spell_crystal_spawn_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_crystal_spawn_SpellScript);

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if (c->isDead())
                    return SPELL_FAILED_DONT_REPORT;
            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_crystal_spawn_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_crystal_spawn_SpellScript();
    }
};

void AddSC_boss_corborus()
{
    new boss_corborus;
    new spell_crystal_barrage();
    new npc_crystal_shard();
    new AreaTrigger_at_rockdoor_break();
    new spell_corborus_sum_triggered();
    new spell_crystal_spawn();
};
