/*
 * Copyright (C) 2013-2014 trinity core og
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

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "the_stonecore.h"

enum Spells
{
    SPELL_SPIKE_SHIELD              = 78835,
    SPELL_ENRAGE                    = 80467,
    SPELL_SHATTER                   = 78807,
    SPELL_GROUND_SLAM               = 78903,
    SPELL_BULWARK                   = 78939,
    SPELL_PARALYZE                  = 92426,
    SPELL_PARALYZE_DUMMY            = 92427,
    SPELL_PARALYZE_INIT             = 92428,

    SPELL_RUPTURE                   = 92383,
    SPELL_RUPTURE_DAM               = 92381,
    SPELL_RUPTURE_DAMAGE            = 95346,
    SPELL_RUPTURE_SUM_CONTROLLER    = 95669,
    SPELL_RUPTURE_PERIODIC          = 92393
};

enum Events
{
    EVENT_BULWARK               = 1,
    EVENT_GROUND_SLAM,
    EVENT_GROUND_SLAM_END,
    EVENT_SHATTER,
    EVENT_PARALYZE,
    EVENT_SPIKE_SHIELD
};

enum Say
{
    SAY_AGGRO       = 0,
    SAY_SLAY        = 1,
    SAY_SHIELD      = 2,
    SAY_DEATH       = 3,
    EMOTE_BULWARK   = 4,
    EMOTE_ENRAGE    = 5
};

class boss_ozruk : public CreatureScript
{
public:
    boss_ozruk() : CreatureScript("boss_ozruk") {}

    struct boss_ozrukAI : public BossAI
    {
        boss_ozrukAI(Creature * creature) : BossAI(creature, DATA_OZRUK) {}

        void Reset()
        {
            enraged = false;
            _Reset();
            me->SetReactState(REACT_AGGRESSIVE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
        }

        void EnterCombat(Unit * /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Remove
            Talk(SAY_AGGRO);
            events.ScheduleEvent(EVENT_BULWARK, 6000);
            _EnterCombat();
        }


        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        void JustSummoned(Creature * summon)
        {
            if (summon->GetEntry() == NPC_RUPTURE_CONTROLLER)
            {
                summon->SetReactState(REACT_PASSIVE);
                summon->CastSpell(summon, SPELL_RUPTURE_PERIODIC, false);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                summon->AddUnitState(UNIT_STATE_ROOT);
                summon->AddUnitState(UNIT_STATE_CANNOT_TURN);
                summon->DespawnOrUnsummon(5000);
            }
        }

        void JustDied(Unit * /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
            Talk(SAY_DEATH);
            _JustDied();
        }

        void KilledUnit(Unit * victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_SLAY);
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_PARALYZE_DUMMY)
            {
                DoCast(SPELL_PARALYZE);
                events.ScheduleEvent(EVENT_SHATTER, 3000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch(eventId)
                {
                    case EVENT_BULWARK:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(EMOTE_BULWARK);
                            DoCast(SPELL_BULWARK);
                            events.ScheduleEvent(EVENT_SPIKE_SHIELD, 5000);
                        }
                        break;
                    case EVENT_SPIKE_SHIELD:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(SAY_SHIELD);
                            DoCast(SPELL_SPIKE_SHIELD);
                            if (IsHeroic())
                                DoCast(SPELL_PARALYZE_INIT);
                            events.ScheduleEvent(EVENT_GROUND_SLAM, 2000);
                        }
                        break;
                    case EVENT_GROUND_SLAM:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            if (auto v = me->getVictim())
                            {
                                me->SetReactState(REACT_PASSIVE);
                                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                                me->StopMoving();
                                me->AttackStop();
                                me->SetFacingToObject(v);
                                me->CastWithDelay(500, v, SPELL_GROUND_SLAM);
                            }
                            events.ScheduleEvent(EVENT_GROUND_SLAM_END, 600);
                        }
                        break;
                    case EVENT_GROUND_SLAM_END:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                            me->SetReactState(REACT_AGGRESSIVE);
                            UpdateVictim();
                            if (!IsHeroic())
                                events.ScheduleEvent(EVENT_BULWARK, 10000);
                        }
                        break;
                    case EVENT_SHATTER:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_SHATTER);
                            events.ScheduleEvent(EVENT_BULWARK, 3000);
                        }
                        break;
                    default:
                        break;
                }
            if (!enraged && me->HealthBelowPct(30))
            {
                Talk(EMOTE_ENRAGE);
                enraged = true;
                DoCast(me, SPELL_ENRAGE, true);
            }

            if (UpdateVictim())
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            if (!me->HasReactState(REACT_PASSIVE))
            DoMeleeAttackIfReady();
        }
    private:
        bool enraged;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new boss_ozrukAI(creature);
    }
};

class spell_rupture_periodic : public SpellScriptLoader
{
public:
    spell_rupture_periodic() : SpellScriptLoader("spell_rupture_periodic") { }

    class spell_rupture_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_rupture_periodic_AuraScript)

        void HandleEffectPeriodic(AuraEffect const* aurEff)
        {
            Unit * caster = GetCaster();
            if (!caster)
                return;

            float dist = aurEff->GetTickNumber() * 8.0f;
            float angle = caster->GetOrientation() - 0.2f;

            for (uint8 i = 0; i < 3; i++)
            {
                float x = caster->GetPositionX() + cos(angle) * dist;
                float y = caster->GetPositionY() + sin(angle) * dist;
                float z = caster->GetMap()->GetStaticHeight(x, y, caster->GetPositionZ() + 3.0f);
                if (std::abs(z) < caster->GetPositionZ() + 5.0f)
                {
                    if (auto creature = caster->SummonCreature(NPC_RUPTURE, x, y, z, 0.f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                    {
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                        creature->CastSpell(creature, SPELL_RUPTURE_DAM, false);
                        creature->HandleEmoteCommand(449);
                    }
                }
                angle += 0.2f;
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_rupture_periodic_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_rupture_periodic_AuraScript();
    }
};

class spell_rupture_damage : public SpellScriptLoader
{
public:
    spell_rupture_damage() : SpellScriptLoader("spell_rupture_damage") {}

    class spell_rupture_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rupture_damage_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end();)
            {
                if (GetCaster()->isInFront((*itr)))
                    ++itr;
                else
                    targets.erase(itr++);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rupture_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rupture_damage_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_rupture_damage_SpellScript();
    }
};

//Paralyze 92426
class spell_paralyze : public AuraScript
{
    PrepareAuraScript(spell_paralyze);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (!target)
            return;

        target->CastSpell(target, 94661, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_paralyze::OnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};


void AddSC_boss_ozruk()
{
    new boss_ozruk();
    new spell_rupture_periodic();
    new spell_rupture_damage();
	new spell_paralyze();
};
