/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "PassiveAI.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "the_lost_city_of_tol_vir.h"

enum Texts
{
    SAY_AGGRO       = 0,
    SAY_SHOCKWAVE   = 1,
    SAY_DETONATE    = 2,
    SAY_DEATH       = 3
};

enum Spells
{
    SPELL_BAD_INTENTIONS             = 83113,
    SPELL_HAMMER_FIST                = 83655,

    SPELL_MYSTIC_TRAP_PLANT_TARGET   = 83646,
    SPELL_MYSTIC_TRAP_PLANT          = 83122,
    SPELL_MYSTIC_TRAP_VISUAL         = 83110,
    SPELL_MYSTIC_TRAP_SEARCHER       = 83111,
    SPELL_MYSTIC_TRAP_EXPLODE        = 83171,
    SPELL_DETONATE_TIMED             = 85523,
    SPELL_DETONATE_TRAPS             = 91263,

    SPELL_SHOCKWAVE_VISUAL_TRIGGER   = 83127,
    SPELL_SHOCKWAVE_SUMMON_EFFECT    = 83128,
    SPELL_SHOCKWAVE_VISUAL_PERIODIC  = 83129,
    SPELL_SHOWKWAVE_VISUAL_DAMAGE    = 83130,
    SPELL_SHOCKWAVE_TARGET_N         = 83131,
    SPELL_SHOCKWAVE_TARGET_S         = 83132,
    SPELL_SHOCKWAVE_TARGET_E         = 83133,
    SPELL_SHOCKWAVE_TARGET_W         = 83134,
    SPELL_SHOCKWAVE                  = 83445,
    SPELL_SHOCKWAVE_DAMAGE           = 83454
};

enum Events
{
    EVENT_BAD_INTENTIONS    = 1,
    EVENT_SHOCKWAVE         = 2,
    EVENT_HAMMER_FIST       = 3,
    EVENT_PLANT_MINE        = 4,
    EVENT_DETONATE_MINES    = 5,
    EVENT_MINE_ACTIVATION   = 6,
    EVENT_MINE_EXPLOSION    = 7
};

enum npc_oathsworn
{
    EVENT_SHOOT = 1,
    EVENT_WING_CLIP,
    EVENT_FORKED_FIRE_SHOT,
    EVENT_BLINK,
    EVENT_EVASION,
    EVENT_FIRST_SHOT,

    SPELL_SHOOT = 83877,
    SPELL_WING_CLIP = 82764,
    SPELL_FORKED_FIRE_SHOT_HC = 89992,
    SPELL_BLINK = 84123,
    SPELL_EVASION = 84122,
};

enum Misc
{
    POINT_DROP_PLAYER       = 0
};

class SummonShockwaveStalker : public BasicEvent
{
public:
    SummonShockwaveStalker(Unit* caster) : _caster(caster) {}

    bool Execute(uint64 execTime, uint32 /*diff*/)
    {
        _caster->CastSpell(_caster, SPELL_SHOCKWAVE_SUMMON_EFFECT);
        _caster->m_Events.AddEvent(this, execTime + 1000);
        return false;
    }

private:
    Unit* _caster;
};

class boss_general_husam : public CreatureScript
{
public:
    boss_general_husam() : CreatureScript("boss_general_husam") { }

    struct boss_general_husamAI : public BossAI
    {
        boss_general_husamAI(Creature* creature) : BossAI(creature, DATA_GENERAL_HUSAM), summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            _Reset();
            me->CastStop();
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, NPC_SHOCKWAVE_VISUAL,         200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_SHOCKWAVE,                200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_LAND_MINE_VEHICLE,        200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_LAND_MINE_TARGET,         200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_BAD_INTENTIONS_TARGET,    200.0f);
            if (creatures.size())
                for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)
                    if (auto npc = (*itr))
                        npc->DespawnOrUnsummon();

            instance->SetData(DATA_GENERAL_HUSAM_EVENT, NOT_STARTED);
            events.Reset();
            summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            instance->SetData(DATA_GENERAL_HUSAM_EVENT, IN_PROGRESS);
            Talk(SAY_AGGRO);
            events.ScheduleEvent(EVENT_BAD_INTENTIONS, 13000);
            events.ScheduleEvent(EVENT_SHOCKWAVE, 18000);
            events.ScheduleEvent(EVENT_HAMMER_FIST, 5000);
            events.ScheduleEvent(EVENT_PLANT_MINE, 9000);

            if (IsHeroic())
                events.ScheduleEvent(EVENT_DETONATE_MINES, 25000);
        }

        void JustDied(Unit* /*Killer*/)
        {
            _JustDied();
            Talk(SAY_DEATH);
            summons.DespawnAll();
            instance->SetData(DATA_GENERAL_HUSAM_EVENT, DONE);


            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, NPC_SHOCKWAVE_VISUAL, 200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_SHOCKWAVE, 200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_LAND_MINE_VEHICLE, 200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_LAND_MINE_TARGET, 200.0f);
            GetCreatureListWithEntryInGrid(creatures, me, NPC_BAD_INTENTIONS_TARGET, 200.0f);
            if (creatures.size())
                for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)
                    if (auto npc = (*itr))
                        npc->DespawnOrUnsummon();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
                case NPC_LAND_MINE_TARGET:
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_DISABLE_MOVE);
                    DoCast(summon, SPELL_MYSTIC_TRAP_PLANT);
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_BAD_INTENTIONS:
                        DoCastRandom(SPELL_BAD_INTENTIONS, 100.0f);
                        events.ScheduleEvent(EVENT_BAD_INTENTIONS, 26000);
                        break;
                    case EVENT_SHOCKWAVE:
                    {
                        Talk(SAY_SHOCKWAVE);
                        me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                        DoCast(SPELL_SHOCKWAVE_TARGET_N);
                        DoCast(SPELL_SHOCKWAVE_TARGET_S);
                        DoCast(SPELL_SHOCKWAVE_TARGET_E);
                        DoCast(SPELL_SHOCKWAVE_TARGET_W);
                        DoCastAOE(SPELL_SHOCKWAVE);

                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, NPC_SHOCKWAVE_VISUAL, 200.0f);
                        if (!creatures.empty())
                            for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                            {
                                Position pos;
                                (*iter)->GetPosition(&pos);
                                (*iter)->Relocate(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f);
                                if (auto creature = (*iter)->ToCreature())
                                    creature->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0);
                                (*iter)->AddAura(SPELL_SHOCKWAVE_VISUAL_PERIODIC, (*iter));
                                (*iter)->GetMotionMaster()->MovePoint(0, pos);
                                (*iter)->m_Events.AddEvent(new SummonShockwaveStalker((*iter)), (*iter)->m_Events.CalculateTime(1000));
                            }
                        events.ScheduleEvent(EVENT_SHOCKWAVE, 40000);
                    }
                        break;
                    case EVENT_HAMMER_FIST:
                        DoCastVictim(SPELL_HAMMER_FIST);
                        events.ScheduleEvent(EVENT_HAMMER_FIST, 12000);
                        break;
                    case EVENT_PLANT_MINE:
                        for(uint8 i = 0; i < 3; i++)
                            DoCastRandom(SPELL_MYSTIC_TRAP_PLANT_TARGET, 100.0f, true);
                        events.ScheduleEvent(EVENT_PLANT_MINE, (IsHeroic() ? 10000 : 17000));
                        break;
                    case EVENT_DETONATE_MINES:
                        Talk(SAY_DETONATE);
                        DoCastAOE(SPELL_DETONATE_TRAPS);
                        events.ScheduleEvent(EVENT_DETONATE_MINES, 25000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_general_husamAI (creature);
    }
};

class npc_husam_mine : public CreatureScript
{
public:
    npc_husam_mine() : CreatureScript("npc_husam_mine") { }

    struct npc_husam_mineAI : public ScriptedAI
    {
        npc_husam_mineAI(Creature *creature) : ScriptedAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_DISABLE_MOVE);
        }

        void AttackStart(Unit* /*target*/) {}

        void IsSummonedBy(Unit* /*summoner*/)
        {
            me->SetInCombatWithZone();
            events.ScheduleEvent(EVENT_MINE_ACTIVATION, (IsHeroic() ? 1000 : 2000));
            events.ScheduleEvent(EVENT_MINE_EXPLOSION, 20000);
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DETONATE_TRAPS)
            {
                events.CancelEvent(EVENT_MINE_EXPLOSION);
                DoCast(me, SPELL_DETONATE_TIMED);
                me->DespawnOrUnsummon(5400);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while(uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_MINE_ACTIVATION:
                        me->AddAura(SPELL_MYSTIC_TRAP_VISUAL, me);
                        DoCast(SPELL_MYSTIC_TRAP_SEARCHER);
                        break;
                    case EVENT_MINE_EXPLOSION:
                        DoCast(me, SPELL_DETONATE_TIMED);
                        me->DespawnOrUnsummon(5400);
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
        return new npc_husam_mineAI(creature);
    }
};

class npc_bad_intentions_vehicle : public CreatureScript
{
public:
    npc_bad_intentions_vehicle() : CreatureScript("npc_bad_intentions_vehicle") { }

    struct npc_bad_intentions_vehicleAI : public PassiveAI
    {
        npc_bad_intentions_vehicleAI(Creature* creature) : PassiveAI(creature)
        {
            me->SetSpeed(MOVE_FLIGHT, 0.35f);
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!apply)
                return;

            me->GetVehicleKit()->RemoveAllPassengers();
            who->GetMotionMaster()->MoveFall();
            /*
            me->SetCanFly(true);
            me->SetDisableGravity(true);
            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 4.0f);
            */
        }
        /*
        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == POINT_DROP_PLAYER)
            {
                if(me->GetVehicleKit())
                    if (auto passenger = me->GetVehicleKit()->GetPassenger(0))
                    {
                        me->GetVehicleKit()->RemovePassenger(passenger);
                        passenger->GetMotionMaster()->MoveFall();
                    }

                me->GetMotionMaster()->MoveTargetedHome();
            }
        }
        */
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bad_intentions_vehicleAI(creature);
    }
};

class spell_mine_player_search_effect : public SpellScriptLoader
{
public:
    spell_mine_player_search_effect() : SpellScriptLoader("spell_mine_player_search_effect") { }

    class spell_mine_player_search_effect_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mine_player_search_effect_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            GetCaster()->RemoveAurasDueToSpell(SPELL_MYSTIC_TRAP_SEARCHER);
            GetCaster()->CastSpell((Unit*)NULL, SPELL_MYSTIC_TRAP_EXPLODE);

            if (auto veh = GetCaster()->GetVehicleBase())
                veh->ToCreature()->DespawnOrUnsummon();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_mine_player_search_effect_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_mine_player_search_effect_SpellScript();
    }
};

class spell_bad_intentions_effect : public SpellScriptLoader
{
public:
    spell_bad_intentions_effect() : SpellScriptLoader("spell_bad_intentions_effect") { }

    class spell_bad_intentions_effect_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_bad_intentions_effect_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            std::list<Creature*> triggers;
            GetCreatureListWithEntryInGrid(triggers, GetHitUnit(), NPC_BAD_INTENTIONS_TARGET, 100.0f);
            if (!triggers.empty())
            {
                triggers.sort(Trinity::ObjectDistanceOrderPred(GetHitUnit(), true));
                Creature* trigger = triggers.front();
                GetHitUnit()->CastSpell(trigger, uint32(GetEffectValue()), true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_bad_intentions_effect_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_bad_intentions_effect_SpellScript();
    }
};

class spell_husam_shockwave : public SpellScriptLoader
{
public:
    spell_husam_shockwave() : SpellScriptLoader("spell_husam_shockwave") { }

    class spell_husam_shockwave_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_husam_shockwave_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            GetCaster()->CastSpell(GetHitUnit(), SPELL_SHOWKWAVE_VISUAL_DAMAGE, true);

            GetCaster()->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            std::list<Creature*> shockwave;
            GetCreatureListWithEntryInGrid(shockwave, GetCaster(), NPC_SHOCKWAVE, 200.0f);
            for (std::list<Creature*>::iterator iter = shockwave.begin(); iter != shockwave.end(); ++iter)
            {
                (*iter)->CastSpell((*iter), SPELL_SHOCKWAVE_DAMAGE);
                (*iter)->DespawnOrUnsummon(200); // ~ 200ms visual effect delay
            }

            std::list<Creature*> shockwaveVisual;
            GetCreatureListWithEntryInGrid(shockwaveVisual, GetCaster(), NPC_SHOCKWAVE_VISUAL, 200.0f);
            for (std::list<Creature*>::iterator iter = shockwaveVisual.begin(); iter != shockwaveVisual.end(); ++iter)
                (*iter)->DespawnOrUnsummon(200);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_husam_shockwave_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_husam_shockwave_SpellScript();
    }
};

class spell_shockwave_debug : public SpellScriptLoader
{
public:
    spell_shockwave_debug() : SpellScriptLoader("spell_shockwave_debug") { }

    class spell_shockwave_debug_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_shockwave_debug_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            Unit* victim = GetHitUnit();
            Unit* caster = GetCaster();
            TC_LOG_ERROR("sql.sql", "caster: %u, victim: %u", caster->GetGUID(), victim->GetGUID());
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_shockwave_debug_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_shockwave_debug_SpellScript();
    }
};

class spell_husam_shockwave_summon_search : public SpellScriptLoader
{
public:
    spell_husam_shockwave_summon_search() : SpellScriptLoader("spell_husam_shockwave_summon_search") { }

    class spell_husam_shockwave_summon_search_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_husam_shockwave_summon_search_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void HandleEffectDummy(SpellEffIndex /*effIndex*/)
        {
            GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SHOCKWAVE_VISUAL_TRIGGER);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_husam_shockwave_summon_search_SpellScript::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_husam_shockwave_summon_search_SpellScript();
    }
};

enum Oathsworn_Captain
{
    EVENT_EARTHQUAKE = 1,
    EVENT_SKULL_CRACK,
    EVENT_REND,

    SPELL_EARTHQUAKE = 84249,
    SPELL_SKULL_CRACK = 82670,
    SPELL_REND_TANK = 93670,

    NPC_EARTHQUAKE_TRIGGER = 45126,
};

class npc_TolVir_Oathsworn_Captain : public CreatureScript
{
public:
    npc_TolVir_Oathsworn_Captain() : CreatureScript("npc_TolVir_Oathsworn_Captain") { }


    struct npc_TolVir_Oathsworn_CaptainAI : public ScriptedAI
    {
        npc_TolVir_Oathsworn_CaptainAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_EARTHQUAKE,      10000);
            events.ScheduleEvent(EVENT_SKULL_CRACK,     5000);
            events.ScheduleEvent(EVENT_REND,            3000);
        }
        void Reset()
        {
            events.Reset();
        }
        void JustDied(Unit* who)
        {
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
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
                case EVENT_EARTHQUAKE:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.f, true))
                        if (auto quake = me->SummonCreature(NPC_EARTHQUAKE_TRIGGER, *target, TEMPSUMMON_TIMED_DESPAWN, 15000))
                        {
                            quake->SetMaxHealth(200000000);
                            quake->SetHealth(quake->GetMaxHealth());

                            quake->setFaction(me->getFaction());
                            quake->SetReactState(REACT_PASSIVE);
                            quake->AddUnitState(UNIT_STATE_ROOT);
                            me->AddAura(SPELL_EARTHQUAKE, quake);
                        }
                    //target random player
                    //summon 45126 on them for 15 seconds
                    //apply 84249 to it
                    events.ScheduleEvent(EVENT_EARTHQUAKE, 20000);
                    break;

                case EVENT_SKULL_CRACK:
                    spell = SPELL_SKULL_CRACK;  //Spell ID
                    min_repeat = 6000;                //Minimum delay
                    max_repeat = 12000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCastRandom(spell, 5.f);
                    events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    break;

                case EVENT_REND:
                    spell = SPELL_REND_TANK;    //Spell ID
                    min_repeat = 3000;                 //Minimum delay
                    max_repeat = 7000;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                default:
                    break;
                }

            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) < 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_TolVir_Oathsworn_CaptainAI(creature);
    }
};


class npc_oathsworn_pathfinder : public CreatureScript
{

public:
    npc_oathsworn_pathfinder() : CreatureScript("npc_oathsworn_pathfinder") { }

    struct npc_oathsworn_pathfinderAI : ScriptedAI
    {
        npc_oathsworn_pathfinderAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FIRST_SHOT, 100);
            events.ScheduleEvent(EVENT_SHOOT, urand(0000, 2000));
            events.ScheduleEvent(EVENT_WING_CLIP, urand(15000, 18000));
            //if (IsHeroic())//heroic mode check
        }
        void UpdateAI(const uint32 diff)
        {
            {
                uint32 spell = 0;                    //Do not touch here, Spell ID
                uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
                uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
                bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {

                    case EVENT_FIRST_SHOT:
                        DoCast(83877);
                        break;

                    case EVENT_SHOOT:
                        spell = SPELL_SHOOT;         //Spell ID
                        min_repeat = 0000;                 //Minimum Delay
                        max_repeat = 2000;                //Maximum Delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    case EVENT_WING_CLIP:
                        spell = SPELL_WING_CLIP;  //Spell ID
                        min_repeat = 15000;                //Minimum delay
                        max_repeat = 18000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;
                    }
                /*
                if (UpdateVictim())
                    if (me->GetDistance(me->getVictim()) <= 6.f)
                        if (!me->HasUnitState(UNIT_STATE_CASTING));
                            DoMeleeAttackIfReady();
                */
            }
        }
        void SpellHitTarget(Unit* target, SpellInfo const* /*spell*/)
        {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target, 30.f, target->GetAngle(me));
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }
    private:
        float followdist;
        bool hasTarget;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oathsworn_pathfinderAI(creature);
    }
};

class npc_oathsworn_wanderer : public CreatureScript
{

public:
    npc_oathsworn_wanderer() : CreatureScript("npc_oathsworn_wanderer") { }

    struct npc_oathsworn_wandererAI : ScriptedAI
    {
        npc_oathsworn_wandererAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FIRST_SHOT, 100);
            events.ScheduleEvent(EVENT_SHOOT, urand(0000, 2000));
            if (IsHeroic())//heroic mode check
                events.ScheduleEvent(EVENT_FORKED_FIRE_SHOT, urand(15000, 15000));
        }
        void UpdateAI(const uint32 diff)
        {
            {
                uint32 spell = 0;                    //Do not touch here, Spell ID
                uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
                uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
                bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {

                    case EVENT_FIRST_SHOT:
                        DoCast(83877);
                        break;

                    case EVENT_SHOOT:
                        spell = SPELL_SHOOT;         //Spell ID
                        min_repeat = 0000;                 //Minimum Delay
                        max_repeat = 2000;                //Maximum Delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    case EVENT_FORKED_FIRE_SHOT:
                        spell = SPELL_FORKED_FIRE_SHOT_HC;  //Spell ID
                        min_repeat = 15000;                //Minimum delay
                        max_repeat = 15000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;
                    }
                //DoMeleeAttackIfReady();
            }
        }
        void SpellHitTarget(Unit* target, SpellInfo const* /*spell*/)
        {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target, 30.f, target->GetAngle(me));
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist;
        bool hasTarget;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oathsworn_wandererAI(creature);
    }

};

class npc_oathsworn_scorpid_keeper : public CreatureScript
{

public:
    npc_oathsworn_scorpid_keeper() : CreatureScript("npc_oathsworn_scorpid_keeper") { }

    struct npc_oathsworn_scorpid_keeperAI : ScriptedAI
    {
        npc_oathsworn_scorpid_keeperAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_BLINK, urand(18000, 26000));
            events.ScheduleEvent(EVENT_EVASION, urand(15000, 20000));
            //if (IsHeroic())//heroic mode check 
        }
        void UpdateAI(const uint32 diff)
        {
            {
                uint32 spell = 0;                    //Do not touch here, Spell ID
                uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
                uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
                bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case EVENT_BLINK:
                        spell = SPELL_BLINK;         //Spell ID
                        min_repeat = 18000;                 //Minimum Delay
                        max_repeat = 26000;                //Maximum Delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    case EVENT_EVASION:
                        spell = SPELL_EVASION;  //Spell ID
                        min_repeat = 15000;                //Minimum delay
                        max_repeat = 20000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
                if (spell == (84123))
                {
                    Position pos(*me);
                    Position pos2(*me);
                    me->MoveBlink(pos, 45.f, 0.0f);

                    if (auto dist = me->GetExactDist(&pos))
                        if (auto angle = me->GetRelativeAngleOffset(&pos))
                        {
                            me->MoveBlink(pos2, dist - 2.f, angle);
                            me->NearTeleportTo(pos2);
                        }
                }
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }
    private:
        float followdist;
        bool hasTarget;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oathsworn_scorpid_keeperAI(creature);
    }

};

void AddSC_boss_general_husam()
{
    new spell_shockwave_debug();
    new boss_general_husam();
    new npc_husam_mine();
    new npc_bad_intentions_vehicle();
    new spell_mine_player_search_effect();
    new spell_bad_intentions_effect();
    new spell_husam_shockwave();
    new spell_husam_shockwave_summon_search();
    new npc_TolVir_Oathsworn_Captain();
    new npc_oathsworn_pathfinder();
    new npc_oathsworn_wanderer();
    new npc_oathsworn_scorpid_keeper();
}
