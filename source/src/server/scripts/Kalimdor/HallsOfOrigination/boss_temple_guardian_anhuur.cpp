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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "Player.h"
#include "halls_of_origination.h"

enum Texts
{
    SAY_AGGRO                    = 0,
    SAY_SHIELD                   = 1,
    EMOTE_SHIELD                 = 2,
    EMOTE_UNSHIELD               = 3,
    SAY_KILL                     = 4,
    SAY_DEATH                    = 5
};

enum Spells
{
    SPELL_DIVINE_RECKONING       = 75592,
    SPELL_BURNING_LIGHT          = 75115,
    SPELL_REVERBERATING_HYMN     = 75322,
    SPELL_SHIELD_OF_LIGHT        = 74938,
    SPELL_ACTIVATE_BEACONS       = 76599,
    SPELL_TELEPORT               = 74969,
    SPELL_SHIELD_VISUAL_RIGHT    = 83698,
    SPELL_BEAM_OF_LIGHT_RIGHT    = 76573,
    SPELL_SHIELD_VISUAL_LEFT     = 83697,
    SPELL_BEAM_OF_LIGHT_LEFT     = 74930,
    SPELL_SEARING_LIGHT          = 75194,
    SPELL_VIPER_POISON           = 74538
};

enum Events
{
    EVENT_DIVINE_RECKONING       = 1,
    EVENT_BURNING_LIGHT          = 2,
    EVENT_SEAR                   = 3
};

enum Phases
{
    PHASE_SHIELDED               = 0,
    PHASE_FIRST_SHIELD           = 1, // Ready to be shielded for the first time
    PHASE_SECOND_SHIELD          = 2, // First shield already happened, ready to be shielded a second time
    PHASE_FINAL                  = 3  // Already shielded twice, ready to finish the encounter normally.
};

enum Actions
{
    ACTION_CHECK_BEACONS = 1,
};

class boss_temple_guardian_anhuur : public CreatureScript
{
public:
    boss_temple_guardian_anhuur() : CreatureScript("boss_temple_guardian_anhuur") { }

    struct boss_temple_guardian_anhuurAI : public BossAI
    {
        boss_temple_guardian_anhuurAI(Creature* creature) : BossAI(creature, DATA_TEMPLE_GUARDIAN_ANHUUR) { }

        void CleanStalkers()
        {
            std::list<Creature*> stalkers;
            GetCreatureListWithEntryInGrid(stalkers, me, NPC_CAVE_IN_STALKER, 100.0f);
            for (std::list<Creature*>::iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
            {
                (*itr)->RemoveAurasDueToSpell(SPELL_BEAM_OF_LIGHT_RIGHT);
                (*itr)->RemoveAurasDueToSpell(SPELL_BEAM_OF_LIGHT_LEFT);
                (*itr)->DeleteThreatList();
            }
        }

        void Reset()
        {
            _phase = PHASE_FIRST_SHIELD;
            _oldPhase = PHASE_FIRST_SHIELD;
            _Reset();
            CleanStalkers();
            me->RemoveAurasDueToSpell(SPELL_SHIELD_OF_LIGHT);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if ((me->HealthBelowPctDamaged(66, damage) && _phase == PHASE_FIRST_SHIELD) ||
                (me->HealthBelowPctDamaged(33, damage) && _phase == PHASE_SECOND_SHIELD))
            {
                _phase++; // Increase the phase
                _oldPhase = _phase;

                _phase = PHASE_SHIELDED;

                me->InterruptNonMeleeSpells(true);
                me->AttackStop();
                DoCast(me, SPELL_TELEPORT);

                DoCast(me, SPELL_SHIELD_OF_LIGHT);

                if (auto go2 = me->FindNearestGameObject(GOBJ_LEVER_LEFT_NM, 100.f))
                {
                    go2->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go2->SetGoState(GO_STATE_READY);
                    if (auto g = go2->SummonCreature(54638, go2->GetPositionX(), go2->GetPositionY(), go2->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                        g->AddAura(94424, g);
                }
                if (auto go2 = me->FindNearestGameObject(GOBJ_LEVER_LEFT_HC, 100.f))
                {
                    go2->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go2->SetGoState(GO_STATE_READY);
                    if (auto g = go2->SummonCreature(54638, go2->GetPositionX(), go2->GetPositionY(), go2->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                        g->AddAura(94424, g);
                }
                if (auto go2 = me->FindNearestGameObject(GOBJ_LEVER_RIGHT_NM, 100.f))
                {
                    go2->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go2->SetGoState(GO_STATE_READY);
                    if (auto g = go2->SummonCreature(54638, go2->GetPositionX(), go2->GetPositionY(), go2->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                        g->AddAura(94424, g);
                }
                if (auto go2 = me->FindNearestGameObject(GOBJ_LEVER_RIGHT_HC, 100.f))
                {
                    go2->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go2->SetGoState(GO_STATE_READY);
                    if (auto g = go2->SummonCreature(54638, go2->GetPositionX(), go2->GetPositionY(), go2->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                        g->AddAura(94424, g);
                }

                me->SetFlag(UNIT_FIELD_FLAGS, uint32(UNIT_FLAG_UNK_31));

                DoCastAOE(SPELL_ACTIVATE_BEACONS);

                std::list<Creature*> stalkers;
                GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_ANHUUR_DOOR));
                GetCreatureListWithEntryInGrid(stalkers, me, NPC_CAVE_IN_STALKER, 100.0f);

                stalkers.remove_if(Trinity::HeightDifferenceCheck(door, 0.0f, false)); // Target only the bottom ones
                for (std::list<Creature*>::iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                {
                    if ((*itr)->GetPositionX() > door->GetPositionX())
                    {
                        (*itr)->CastSpell((*itr), SPELL_SHIELD_VISUAL_LEFT, true);
                        (*itr)->CastSpell((*itr), SPELL_BEAM_OF_LIGHT_LEFT, true);
                    }
                    else
                    {
                        (*itr)->CastSpell((*itr), SPELL_SHIELD_VISUAL_RIGHT, true);
                        (*itr)->CastSpell((*itr), SPELL_BEAM_OF_LIGHT_RIGHT, true);
                    }
                }

                std::list<Creature*> snakes;
                GetCreatureListWithEntryInGrid(snakes, me, NPC_SNAKE, 100.0f);
                for (std::list<Creature*>::iterator itr = snakes.begin(); itr != snakes.end(); ++itr)
                {
                    if (!(*itr)->isAlive())
                        (*itr)->Respawn(true);
                }

                // must be delayed because of the teleport
                me->CastWithDelay(1000, me, SPELL_REVERBERATING_HYMN);

                Talk(EMOTE_SHIELD);
                Talk(SAY_SHIELD);
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_CHECK_BEACONS)
            {
                bool activeStalkerFound = false;
                std::list<Creature*> stalkers;
                GameObject* door = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_ANHUUR_DOOR));
                GetCreatureListWithEntryInGrid(stalkers, me, NPC_CAVE_IN_STALKER, 100.0f);
                stalkers.remove_if(Trinity::HeightDifferenceCheck(door, 0.0f, false));
                for (std::list<Creature*>::iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                {
                    if ((*itr)->HasAura(SPELL_BEAM_OF_LIGHT_LEFT) || (*itr)->HasAura(SPELL_BEAM_OF_LIGHT_RIGHT))
                    {
                        activeStalkerFound = true;
                        break;
                    }
                }

                if (!activeStalkerFound)
                {
                    me->RemoveAurasDueToSpell(SPELL_SHIELD_OF_LIGHT);
                    Talk(EMOTE_UNSHIELD);
                    _phase = _oldPhase;
                }
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 1);
            Talk(SAY_AGGRO);
            _EnterCombat();

            events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(10000, 12000));
            events.ScheduleEvent(EVENT_BURNING_LIGHT, 12000);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            Talk(SAY_DEATH);
            _JustDied(); 
            CleanStalkers();
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_KILL);
        }

        void JustReachedHome()
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _JustReachedHome();
            instance->SetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR, FAIL);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() || !CheckInRoom() || me->GetCurrentSpell(CURRENT_CHANNELED_SPELL) || _phase == PHASE_SHIELDED)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_DIVINE_RECKONING:
                        DoCastRandom(SPELL_DIVINE_RECKONING, 0.0f);
                        events.ScheduleEvent(EVENT_DIVINE_RECKONING, urand(10000, 12000));
                        break;
                    case EVENT_BURNING_LIGHT:
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me));
                        if (!target)
                            DoCastRandom(SPELL_BURNING_LIGHT, 0.0f);
                        else
                            DoCast(target, SPELL_BURNING_LIGHT);
                        events.ScheduleEvent(EVENT_SEAR, 2000);
                        events.ScheduleEvent(EVENT_BURNING_LIGHT, 12000);
                        break;
                    }
                    case EVENT_SEAR:
                    {
                        Unit* target = me->FindNearestCreature(NPC_SEARING_LIGHT, 100.0f);
                        if (!target)
                            break;

                        std::list<Creature*> stalkers;
                        GetCreatureListWithEntryInGrid(stalkers, me, NPC_CAVE_IN_STALKER, 100.0f);
                        stalkers.remove_if(Trinity::HeightDifferenceCheck(ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_ANHUUR_DOOR)), 5.0f, true));

                        if (stalkers.size() < 2)
                            break;

                        stalkers.sort(Trinity::ObjectDistanceOrderPred(target));

                        // Get the closest statue face (any of its eyes)
                        Creature* eye1 = stalkers.front();
                        stalkers.remove(eye1); // Remove the eye.
                        stalkers.sort(Trinity::ObjectDistanceOrderPred(eye1)); // Find the second eye.
                        Creature* eye2 = stalkers.front();

                        eye1->CastSpell(eye1, SPELL_SEARING_LIGHT, true);
                        eye2->CastSpell(eye2, SPELL_SEARING_LIGHT, true);
                        break;
                    }
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        uint8 _phase;
        uint8 _oldPhase;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetHallsOfOriginationAI<boss_temple_guardian_anhuurAI>(creature);
    }
};

class spell_anhuur_shield_of_light : public SpellScriptLoader
{
    public:
        spell_anhuur_shield_of_light() : SpellScriptLoader("spell_anhuur_shield_of_light") { }

        class spell_anhuur_shield_of_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anhuur_shield_of_light_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (InstanceMap* instance = GetCaster()->GetMap()->ToInstanceMap())
                {
                    if (auto const script = instance->GetInstanceScript())
                    {
                        if (auto go = ObjectAccessor::GetGameObject(*GetCaster(), script->GetData64(DATA_ANHUUR_DOOR)))
                        {
                            targets.remove_if(Trinity::HeightDifferenceCheck(go, 5.0f, false));
                            targets.remove(GetCaster());
                            targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                            targets.resize(2);
                        }
                    }
                }
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_anhuur_shield_of_light_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_anhuur_shield_of_light_SpellScript();
        }
};

class spell_anhuur_disable_beacon_beams : public SpellScriptLoader
{
    public:
        spell_anhuur_disable_beacon_beams() : SpellScriptLoader("spell_anhuur_disable_beacon_beams") { }

        class spell_anhuur_disable_beacon_beams_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anhuur_disable_beacon_beams_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->RemoveAurasDueToSpell(GetEffectValue());
            }

            void CheckPhase()
            {
                if (auto instance = GetCaster()->GetInstanceScript())
                    if (auto anhuur = ObjectAccessor::GetCreature(*GetCaster(), instance->GetData64(DATA_ANHUUR_GUID)))
                        anhuur->AI()->DoAction(ACTION_CHECK_BEACONS);

                if (auto go2 = GetCaster()->FindNearestGameObject(GOBJ_LEVER_LEFT_NM, 15.f))
                {
                    go2->SetGoState(GO_STATE_ACTIVE);
                    go2->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (auto g = go2->FindNearestCreature(54638, 5.f))
                        g->DespawnOrUnsummon();
                }
                if (auto go2 = GetCaster()->FindNearestGameObject(GOBJ_LEVER_LEFT_HC, 15.f))
                {
                    go2->SetGoState(GO_STATE_ACTIVE);
                    go2->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (auto g = go2->FindNearestCreature(54638, 5.f))
                        g->DespawnOrUnsummon();
                }
                if (auto go2 = GetCaster()->FindNearestGameObject(GOBJ_LEVER_RIGHT_NM, 15.f))
                {
                    go2->SetGoState(GO_STATE_ACTIVE);
                    go2->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (auto g = go2->FindNearestCreature(54638, 5.f))
                        g->DespawnOrUnsummon();
                }
                if (auto go2 = GetCaster()->FindNearestGameObject(GOBJ_LEVER_RIGHT_HC, 15.f))
                {
                    go2->SetGoState(GO_STATE_ACTIVE);
                    go2->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (auto g = go2->FindNearestCreature(54638, 5.f))
                        g->DespawnOrUnsummon();
                }

            }

            SpellCastResult DoAnimation()
            {
                GetCaster()->SetUInt32Value(UNIT_NPC_EMOTESTATE, 69);
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_anhuur_disable_beacon_beams_SpellScript::DoAnimation);
                OnEffectHitTarget += SpellEffectFn(spell_anhuur_disable_beacon_beams_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                AfterCast += SpellCastFn(spell_anhuur_disable_beacon_beams_SpellScript::CheckPhase);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_anhuur_disable_beacon_beams_SpellScript();
        }
};

class spell_anhuur_activate_beacons : public SpellScriptLoader
{
    public:
        spell_anhuur_activate_beacons() : SpellScriptLoader("spell_anhuur_activate_beacons") { }

        class spell_anhuur_activate_beacons_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anhuur_activate_beacons_SpellScript);

            void Activate(SpellEffIndex index)
            {
                PreventHitDefaultEffect(index);
                GetHitGObj()->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_anhuur_activate_beacons_SpellScript::Activate, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_anhuur_activate_beacons_SpellScript();
        }
};

class spell_anhuur_divine_reckoning : public SpellScriptLoader
{
public:
    spell_anhuur_divine_reckoning() : SpellScriptLoader("spell_anhuur_divine_reckoning") { }

    class spell_anhuur_divine_reckoning_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_anhuur_divine_reckoning_AuraScript);

        void OnPeriodic(AuraEffect const* aurEff)
        {
            if (auto caster = GetCaster())
            {
                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_BASE_POINT0, aurEff->GetAmount());
                caster->CastCustomSpell(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, values, GetTarget());
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_anhuur_divine_reckoning_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_anhuur_divine_reckoning_AuraScript();
    }
};

class npc_pit_viper : public CreatureScript
{
public:
    npc_pit_viper() : CreatureScript("npc_pit_viper") { }

    struct npc_pit_viperAI : public ScriptedAI
    {
        npc_pit_viperAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            castTimer = 0;
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(15.f);
            castTimer = 5000;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);


            if (castTimer <= diff)
            {
                DoCastVictim(SPELL_VIPER_POISON);
                castTimer = 8000;
            } else castTimer -= diff;
    
    DoMeleeAttackIfReady();
        }
    private:
        uint32 castTimer{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pit_viperAI(creature);
    }
};

enum witch_doctor
{
    EVENT_FIREBALL          = 1,
    EVENT_METEOR,
    EVENT_BLAZING_ERUPTION,
    EVENT_MOLTEN_BARRIER,

    SPELL_FIREBALL          = 89538,
    SPELL_METEOR            = 84032,
    SPELL_BLAZING_ERUPTION  = 89547,
    SPELL_MOLTEN_BARRIER    = 89542,
};

class npc_Temple_Fireshaper : public CreatureScript
{
public:
    npc_Temple_Fireshaper() : CreatureScript("npc_Temple_Fireshaper") { }


    struct npc_Temple_FireshaperAI : public ScriptedAI
    {
        npc_Temple_FireshaperAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_BLAZING_ERUPTION, 500);
            events.ScheduleEvent(EVENT_FIREBALL,            urand(5000, 8000));
            events.ScheduleEvent(EVENT_METEOR,              urand(15000, 21000));
            events.ScheduleEvent(EVENT_MOLTEN_BARRIER,      urand(16000, 40000));
        }
        void Reset()
        {
            events.Reset();
            me->ClearUnitState(UNIT_STATE_ROOT);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            summons.DespawnAll();
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

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            switch (spell->Id)
            {
            case SPELL_BLAZING_ERUPTION:
                if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
                {
                    DoCast(me, SPELL_MOLTEN_BARRIER, true);
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();
                    events.ScheduleEvent(EVENT_BLAZING_ERUPTION, urand(25000, 35000));
                }
                break;
            }
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
                case EVENT_FIREBALL:
                    spell = SPELL_FIREBALL;         //Spell ID
                    min_repeat = 5000;                 //Minimum Delay
                    max_repeat = 8000;                //Maximum Delay
                    wait_castnext = false;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_METEOR:
                    spell = SPELL_METEOR;  //Spell ID
                    min_repeat = 15000;                //Minimum delay
                    max_repeat = 21000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_BLAZING_ERUPTION:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        me->RemoveAurasDueToSpell(SPELL_MOLTEN_BARRIER);
                        spell = SPELL_BLAZING_ERUPTION;    //Spell ID
                        min_repeat = 25000;                 //Minimum delay
                        max_repeat = 35000;                 //Maximum delay
                        wait_castnext = true;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        me->SetReactState(REACT_PASSIVE);
                        me->StopMoving();
                        me->CastStop();
                        DoCast(spell);
                    }
                    break;

                case EVENT_MOLTEN_BARRIER:
                    spell = SPELL_MOLTEN_BARRIER;    //Spell ID
                    min_repeat = 16000;                 //Minimum delay
                    max_repeat = 40000;                 //Maximum delay
                    wait_castnext = true;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                default:
                    break;
                }

            if (me->HasReactState(REACT_AGGRESSIVE))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();

            if (me->isInCombat())
                if (me->HasReactState(REACT_PASSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        me->SetReactState(REACT_AGGRESSIVE);
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Temple_FireshaperAI(creature);
    }
};

class spell_blazing_Eruption : public SpellScriptLoader
{
public:
    spell_blazing_Eruption() : SpellScriptLoader("spell_blazing_Eruption") { }
    class spell_blazing_Eruption_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_blazing_Eruption_AuraScript);

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (auto caster = GetCaster())
                if (auto c = caster->ToCreature())
                c->SetReactState(REACT_AGGRESSIVE);
        }
        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_blazing_Eruption_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_blazing_Eruption_AuraScript();
    }
};


enum swiftstalker
{
    EVENT_MULTISHOT = 1,
    EVENT_CHARGED_SHOT,
    EVENT_WEAPON_SHOOT,
    EVENT_CHECK_WIPE,

    SPELL_MULTISHOT = 84836,
    SPELL_CHARGED_SHOT = 89571,
    SPELL_NORMAL_SHOT = 83877,
};

class npc_HOO_swiftstalker : public CreatureScript
{
public:
    npc_HOO_swiftstalker() : CreatureScript("npc_HOO_swiftstalker") { }


    struct npc_HOO_swiftstalkerAI : public ScriptedAI
    {
        npc_HOO_swiftstalkerAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->SetSheath(SHEATH_STATE_RANGED);
        }

        void EnterCombat(Unit* /*who*/) 
        {
            me->SetSheath(SHEATH_STATE_RANGED);
            events.ScheduleEvent(EVENT_MULTISHOT, urand(7000, 30000));
            events.ScheduleEvent(EVENT_CHARGED_SHOT, urand(7000, 21000));
            events.ScheduleEvent(EVENT_WEAPON_SHOOT, urand(500, 1000));
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
        }
        void Reset()
        {
            me->SetSheath(SHEATH_STATE_RANGED);
            events.Reset();
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
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

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
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
                case EVENT_CHECK_WIPE:
                    //TC_LOG_ERROR("sql.sql", "Executing check wipe");
                    if (checkWipe())
                    {
                        //TC_LOG_ERROR("sql.sql", "Wipe recognized at glubtok.");
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    break;
                case EVENT_MULTISHOT:
                    spell = SPELL_MULTISHOT;         //Spell ID
                    min_repeat = 7000;                 //Minimum Delay
                    max_repeat = 30000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_CHARGED_SHOT:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_CHARGED_SHOT;  //Spell ID
                        min_repeat = 15000;                //Minimum delay
                        max_repeat = 21000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        DoCastRandom(spell, 45.f);
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_WEAPON_SHOOT:
                    spell = SPELL_NORMAL_SHOT;    //Spell ID
                    min_repeat = 3000;                 //Minimum delay
                    max_repeat = 3500;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            /*
            if (me->HasReactState(REACT_AGGRESSIVE))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            */
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_HOO_swiftstalkerAI(creature);
    }
};

class hoo_temple_runecaster : public CreatureScript
{
public:
    hoo_temple_runecaster() : CreatureScript("hoo_temple_runecaster") { }


    struct hoo_temple_runecasterAI : public ScriptedAI
    {

        hoo_temple_runecasterAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_CURSE,
            EVENT_CLEAVE,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_CURSE_OF_THE_RUNECASTER = 89551,
            SPELL_RUNIC_CLEAVE = 89554,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_CURSE,     SPELL_CURSE_OF_THE_RUNECASTER,        5000,           10000,      true,       true},
            {EVENT_CLEAVE,   SPELL_RUNIC_CLEAVE,                    5000,           10000,      true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_CURSE:     Spell_Rotation(eventId);  break;
                case EVENT_CLEAVE:   Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new hoo_temple_runecasterAI(creature);
    }
};

class npc_HOO_shadowlancer : public CreatureScript
{
public:
    npc_HOO_shadowlancer() : CreatureScript("npc_HOO_shadowlancer") { }


    struct npc_HOO_shadowlancerAI : public ScriptedAI
    {

        npc_HOO_shadowlancerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_SHADOWLANCE,
            EVENT_PACT,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_SHADOWLANCE = 89555,
            SPELL_PACT_OF_DARKNESS = 89560,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_SHADOWLANCE,     SPELL_SHADOWLANCE,          15000,           30000,      true,       true},
            {EVENT_PACT,   SPELL_PACT_OF_DARKNESS,              15000,           30000,      true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SHADOWLANCE:     Spell_Rotation(eventId);  break;
                case EVENT_PACT:            Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_HOO_shadowlancerAI(creature);
    }
};

void AddSC_boss_temple_guardian_anhuur()
{
    new boss_temple_guardian_anhuur();
    new spell_anhuur_shield_of_light();
    new spell_anhuur_disable_beacon_beams();
    new spell_anhuur_activate_beacons();
    new spell_anhuur_divine_reckoning();
    new npc_pit_viper();
    new npc_Temple_Fireshaper();
    new spell_blazing_Eruption();
    new npc_HOO_swiftstalker();
    new hoo_temple_runecaster();
    new npc_HOO_shadowlancer();
}
