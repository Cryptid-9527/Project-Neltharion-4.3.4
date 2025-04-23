
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "shadowfang_keep.h"

enum Spells
{
    SPELL_SUMMON_WORGEN_SPIRIT      = 93857,
    SPELL_VEIL_OF_SHADOW            = 23224,
    SPELL_CURSED_VEIL_H             = 93956,
    SPELL_CLAW                      = 16827,
    SPELL_SUMMON_LUPINE_SPECTRE     = 94199,
};

enum Events
{
    EVENT_VEIL_OF_SHADOW            = 1,
    EVENT_CURSED_VEIL,
    EVENT_CLAW,
    EVENT_SUMMON_WORGEN,
};

enum Texts
{
    SAY_AGGRO,
    SAY_DEATH,
    SAY_KILLER_1,
    SAY_KILLER_2
};

enum Action
{
    ACTION_SUMMON                   = 1,
};

uint32 spellIds [4]     = { 93859, 93896, 93921, 93925};

class boss_baron_silverlaine : public CreatureScript
{
    public:
        boss_baron_silverlaine() : CreatureScript("boss_baron_silverlaine") { }

        struct boss_baron_silverlaineAI : public BossAI
        {
            boss_baron_silverlaineAI(Creature* creature) : BossAI(creature, DATA_BARON_SILVERLAINE_EVENT) { }

            void Reset()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _Reset();
                isFirst         = false;
                isSecond        = false;
                isThird         = false;
                spellSet.clear();
                for (uint8 i=0; i < 4; ++i)
                    spellSet.insert(spellIds[i]);
                std::list<Creature*> summons1;
                me->GetCreatureListWithEntryInGrid(summons1, 50835, 100.0f);
                for (Creature* sum1 : summons1)
                    sum1->DespawnOrUnsummon();

                std::list<Creature*> summons2;
                me->GetCreatureListWithEntryInGrid(summons2, 50857, 100.0f);
                for (Creature* sum2 : summons2)
                    sum2->DespawnOrUnsummon();

                std::list<Creature*> summons3;
                me->GetCreatureListWithEntryInGrid(summons3, 50923, 100.0f);
                for (Creature* sum3 : summons3)
                    sum3->DespawnOrUnsummon();

                std::list<Creature*> summons4;
                me->GetCreatureListWithEntryInGrid(summons4, 50869, 100.0f);
                for (Creature* sum4 : summons4)
                    sum4->DespawnOrUnsummon();

                std::list<Creature*> summons5;
                me->GetCreatureListWithEntryInGrid(summons5, 50851, 100.0f);
                for (Creature* sum5 : summons5)
                    sum5->DespawnOrUnsummon();

                std::list<Creature*> summons6;
                me->GetCreatureListWithEntryInGrid(summons6, 50834, 100.0f);
                for (Creature* sum6 : summons6)
                    sum6->DespawnOrUnsummon();
            }

            void EnterCombat(Unit* /*who*/)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                _EnterCombat();
                Talk(SAY_AGGRO);
                events.ScheduleEvent(DUNGEON_MODE(EVENT_VEIL_OF_SHADOW, EVENT_CURSED_VEIL), 10000);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(SAY_DEATH);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                std::list<Creature*> summons1;
                me->GetCreatureListWithEntryInGrid(summons1, 50835, 100.0f);
                for (Creature* sum1 : summons1)
                    sum1->DespawnOrUnsummon();

                std::list<Creature*> summons2;
                me->GetCreatureListWithEntryInGrid(summons2, 50857, 100.0f);
                for (Creature* sum2 : summons2)
                    sum2->DespawnOrUnsummon();

                std::list<Creature*> summons3;
                me->GetCreatureListWithEntryInGrid(summons3, 50923, 100.0f);
                for (Creature* sum3 : summons3)
                    sum3->DespawnOrUnsummon();

                std::list<Creature*> summons4;
                me->GetCreatureListWithEntryInGrid(summons4, 50869, 100.0f);
                for (Creature* sum4 : summons4)
                    sum4->DespawnOrUnsummon();

                std::list<Creature*> summons5;
                me->GetCreatureListWithEntryInGrid(summons5, 50851, 100.0f);
                for (Creature* sum5 : summons5)
                    sum5->DespawnOrUnsummon();

                std::list<Creature*> summons6;
                me->GetCreatureListWithEntryInGrid(summons6, 50834, 100.0f);
                for (Creature* sum6 : summons6)
                    sum6->DespawnOrUnsummon();
            }


            void SummonedCreatureDies(Creature* creature, Unit* /*killer*/) 
            {
                switch (creature->GetEntry())
                {
                case ODO_THE_BLINDWATCHER:
                case RAZORCLAW_THE_BUTCHER:
                case WOLF_MASTER_NANDOS:
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                }
            }

            void JustSummoned(Creature* summon) 
            {
                BossAI::JustSummoned(summon);
                if (summon->GetEntry() == 50934)
                {
                    if (auto odo = me->SummonCreature(ODO_THE_BLINDWATCHER, *summon, TEMPSUMMON_MANUAL_DESPAWN))
                        odo->SetInCombatWithZone();
                    //summon->AI()->DoCast(93860);
                    summon->DespawnOrUnsummon(1000);
                }
                DoZoneInCombat(summon);
                 
                switch (summon->GetEntry())
                {
                case ODO_THE_BLINDWATCHER:
                case RAZORCLAW_THE_BUTCHER:
                case WOLF_MASTER_NANDOS:
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, summon);
                    break;
                }
            }

            void KilledUnit(Unit* victim)
            {
                Talk(RAND(SAY_KILLER_1, SAY_KILLER_2));
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_SUMMON)
                {
                    if (spellSet.empty())
                        return;

                    uint32 summonspell = Trinity::Containers::SelectRandomContainerElement(spellSet);
                    DoCast(summonspell);
                    spellSet.erase(summonspell);
                }
            }

            void DamageTaken(Unit* /*dealer*/, uint32& damage)
            {
                if (IsHeroic())
                {
                    if (me->HealthBelowPctDamaged(90, damage) && !isFirst)
                    {
                        isFirst = true;
                        events.ScheduleEvent(EVENT_SUMMON_WORGEN, 1);
                    }
                    else if (me->HealthBelowPctDamaged(60, damage) && !isSecond)
                    {
                        isSecond = true;
                        events.ScheduleEvent(EVENT_SUMMON_WORGEN, 1);
                    }
                    else if (me->HealthBelowPctDamaged(30, damage) && !isThird)
                    {
                        isThird = true;
                        events.ScheduleEvent(EVENT_SUMMON_WORGEN, 1);
                    }
                }
                else
                {
                    if (me->HealthBelowPctDamaged(70, damage) && !isFirst)
                    {
                        isFirst = true;
                        events.ScheduleEvent(EVENT_SUMMON_WORGEN, 1);
                    }
                    if (me->HealthBelowPctDamaged(30, damage) && !isSecond)
                    {
                        isSecond = true;
                        events.ScheduleEvent(EVENT_SUMMON_WORGEN, 1);
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch(eventId)
                    {
                    case EVENT_VEIL_OF_SHADOW:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_VEIL_OF_SHADOW);
                            events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, 10000);
                        }
                        break;
                    case EVENT_CURSED_VEIL:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_CURSED_VEIL_H);
                            events.ScheduleEvent(EVENT_CURSED_VEIL, urand(20000, 40000));
                        }
                        break;
                    case EVENT_SUMMON_WORGEN:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_SUMMON_WORGEN_SPIRIT);
                        }
                        break;
                    }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                DoMeleeAttackIfReady();
            }
        private:
            std::set<uint32> spellSet;
            bool isFirst    { false };
            bool isSecond   { false };
            bool isThird    { false };
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_baron_silverlaineAI(creature);
        }
};

class spell_summon_worgen_spirits : public SpellScriptLoader
{
public:
    spell_summon_worgen_spirits() : SpellScriptLoader("spell_summon_worgen_spirits") { }

    class spell_summon_worgen_spirits_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_summon_worgen_spirits_SpellScript);

        bool Load()
        {
            _instance = GetCaster()->GetInstanceScript();
            return _instance != NULL;
        }
        
        void AfterCast(SpellEffIndex /*effIndex*/)
        {
            if (auto silverlaine = ObjectAccessor::GetCreature(*GetCaster(), _instance->GetData64(DATA_BARON_SILVERLAINE)))
                silverlaine->AI()->DoAction(ACTION_SUMMON);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_summon_worgen_spirits_SpellScript::AfterCast, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }

        InstanceScript* _instance;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_summon_worgen_spirits_SpellScript();
    }
};

class npc_wolf_master_nandos : public CreatureScript
{
public:
    npc_wolf_master_nandos() : CreatureScript("npc_wolf_master_nandos") { }

    struct npc_wolf_master_nandosAI : public ScriptedAI
    {
        npc_wolf_master_nandosAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

        void IsSummonedBy(Unit* /*summoner*/) override
        {
            if (auto silverlaine = _instance->GetCreature(DATA_BARON_SILVERLAINE))
                silverlaine->AI()->JustSummoned(me);

            _instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            events.ScheduleEvent(EVENT_CLAW, 1600);
            for (uint8 i = 0; i < 3; i++);
            DoCast(me, SPELL_SUMMON_LUPINE_SPECTRE, true);
            DoCast(me, SPELL_SUMMON_LUPINE_SPECTRE, true);
            DoCast(me, SPELL_SUMMON_LUPINE_SPECTRE, true);

            DoZoneInCombat();
            UpdateVictim();
        }

        void JustDied(Unit* /*killer*/) override
        {
            me->DespawnOrUnsummon();
        }


        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CLAW:
                    DoCastVictim(SPELL_CLAW);
                    events.RescheduleEvent(EVENT_CLAW, 3000);
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
            DoMeleeAttackIfReady();
        }

    private:
        InstanceScript* _instance;
        EventMap events;
    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wolf_master_nandosAI(creature);
    }
};

class npc_silverlaine_worgen_summon_trigger : public CreatureScript
{
public:
    npc_silverlaine_worgen_summon_trigger() : CreatureScript("npc_silverlaine_worgen_summon_trigger") { }

    struct npc_silverlaine_worgen_summon_triggerAI : public ScriptedAI
    {
        npc_silverlaine_worgen_summon_triggerAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

        void IsSummonedBy(Unit* /*summoner*/) override
        {
        }
        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
            case ODO_THE_BLINDWATCHER:
            case RAZORCLAW_THE_BUTCHER:
            case WOLF_MASTER_NANDOS:
            case RETHILGORE:
                if (instance)
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, summon);
                me->DespawnOrUnsummon(1000);
                break;
            }
        }

    private:
        InstanceScript* _instance;
        EventMap events;
    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silverlaine_worgen_summon_triggerAI(creature);
    }
};


void AddSC_boss_baron_silverlaine()
{
    new boss_baron_silverlaine();
    new spell_summon_worgen_spirits();
    new npc_wolf_master_nandos();
    new npc_silverlaine_worgen_summon_trigger();
}