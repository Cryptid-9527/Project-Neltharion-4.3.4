
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "shadowfang_keep.h"

enum Spells
{
    SPELL_DESECRATION                      = 93687,
    SPELL_DESECRATION_TRIGGERED            = 93690,

    SPELL_MALEFIX_STRIKE                   = 93685,
    SPELL_SHIELD_OF_THE_PERFIDIOUS         = 93693,

    // heroic
    SPELL_SEPARATION_ANXIETY               = 96272,
    SPELL_WORD_OF_SHAME                    = 93852,
    SPELL_UNHOLY_EMPOWERMENT               = 93844,

    //Wailing Guardsman
    SPELL_MORTAL_STRIKE                    = 91801,
    SPELL_SCREAMS_OF_THE_PAST              = 7074,

    //Tormented Officer
    SPELL_FORSAKEN_ABILITY                 = 7054,
    SPELL_SHIELD_WALL                      = 91463,
    SPELL_SEPERATION_ANXIETY               = 96272,

};

enum Events
{
    EVENT_DESECRATION                      = 1,
    EVENT_MALEFIX_STRIKE,
    EVENT_SUMMONS,

    //Wailing Guardian
    EVENT_MORTAL_STRIKE,
    EVENT_SCREAMS_OF_THE_PAST,

    //Tormented Officer
    EVENT_FORSAKEN_ABILITY,
    EVENT_SHIELD_WALL,

    //Both
    EVENT_UNHOLY_EMPOWERMENT
};

enum Actions
{
    ACTION_RANDOM_CAST                     = 1
};

enum Yells
{
    SAY_AGGRO,
    SAY_SUMMON_GUARDS,
    SAY_KILL_1,
    SAY_KILL_2,
    SAY_DEATH
};

enum Adds
{
    NPC_SPRINGVALE_GUARD = 50613,
    NPC_SPRINGVALE_OFFICER = 50615,
    NOC_DESECRATION_BUNNY = 50503,
};

Position const spawnPos[2]   =
{
    {-256.5988f, 2267.021f, 100.8899f, 0.0f},
    {-247.5112f, 2232.320f,  98.5017f, 0.0f},
};

/*
*/
class boss_commander_springvale : public CreatureScript
{
    public:
        boss_commander_springvale() : CreatureScript("boss_commander_springvale") { }

        struct boss_commander_springvaleAI : public BossAI
        {
            boss_commander_springvaleAI(Creature * creature) : BossAI(creature, DATA_COMMANDER_SPRINGVALE_EVENT) { }

            void Reset()
            {
                _Reset();
                Achievement = true;
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WORD_OF_SHAME);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                Talk(RAND(SAY_KILL_1, SAY_KILL_2));
            }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (!caster || !spell)
                    return;

                if (spell->Id != SPELL_UNHOLY_EMPOWERMENT)
                    return;

                Achievement = false;
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_DESECRATION, 10000);
                events.ScheduleEvent(EVENT_MALEFIX_STRIKE, 15000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SUMMONS, 40000);
            }

            void JustSummoned(Creature* summon)
            {
                BossAI::JustSummoned(summon);

                if (summon->GetEntry() == NPC_DESECRATION_TR)
                    me->AddAura(SPELL_DESECRATION_TRIGGERED, summon);
            }

            void JustDied(Unit * /*killer*/)
            {
                Talk(SAY_DEATH);
                _JustDied();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WORD_OF_SHAME);
                if (Achievement && IsHeroic())
                    instance->DoCompleteAchievement(ACHIEVEMENT_TO_THE_GROUND);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_RANDOM_CAST)
                {
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                    {
                        if (IsHeroic())
                        {
                            switch(urand(0,1))
                            {
                                case 0:
                                {
                                    float lock_angle{ me->GetOrientation() };
                                    DoCast(SPELL_SHIELD_OF_THE_PERFIDIOUS);
                                    me->SetFacingTo(lock_angle);
                                    break;
                                }
                                case 1:
                                    DoCastRandom(SPELL_WORD_OF_SHAME, 50, true);
                                    break;
                            }
                        }
                        else
                        {
                            float lock_angle{ me->GetOrientation() };
                            DoCast(SPELL_SHIELD_OF_THE_PERFIDIOUS);
                            me->SetFacingTo(lock_angle);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                /*
                X between -247.659882 and -257.318726
                    Y between 2233.428467 and 2267.711914
                    Z must be > 89.373482


                    X: -242.666443 Y: 2249.941406 Z: 100.892349 Orientation: 3.742550
                */

                if (me->isInCombat())
                {
                    if (
                        (me->GetPositionZ() < 89.37482f)
                        ||
                        (me->GetPositionX() > -210.659882f || me->GetPositionX() < -275.318726)
                        ||
                        (me->GetPositionY() > 2267.711914f || me->GetPositionY() < 2233.428467)
                        )
                    {
                        //springvale is outside his box.
                        if (!me->HasAura(SPELL_SEPERATION_ANXIETY))
                            me->AddAura(SPELL_SEPERATION_ANXIETY, me);
                    }
                    else
                    {
                        //springvale is inside his box
                        if (me->HasAura(SPELL_SEPERATION_ANXIETY))
                            me->RemoveAura(SPELL_SEPERATION_ANXIETY);
                    }
                }

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch(eventId)
                    {
                    case EVENT_DESECRATION:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_DESECRATION);
                            events.ScheduleEvent(EVENT_DESECRATION, urand(14000, 20000));
                        }
                        break;
                    case EVENT_MALEFIX_STRIKE:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCastVictim(SPELL_MALEFIX_STRIKE);
                            events.ScheduleEvent(EVENT_MALEFIX_STRIKE, urand(12000, 18000));
                        }
                        break;
                    case EVENT_SUMMONS:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(SAY_SUMMON_GUARDS);
                            me->SummonCreature(NPC_TORMENTED_OFFICER, spawnPos[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                            me->SummonCreature(NPC_WAILING_GUARDSMAN, spawnPos[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                            events.ScheduleEvent(EVENT_SUMMONS, 60000);
                        }
                        break;
                    }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }
        private:
            bool Achievement;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_commander_springvaleAI(creature);
        }
};

class spell_springvale_unholy_power : public SpellScriptLoader
{
public:
    spell_springvale_unholy_power() : SpellScriptLoader("spell_springvale_unholy_power") { }

    class spell_springvale_unholy_power_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_springvale_unholy_power_AuraScript);

        bool Load()
        {
            _instance = GetCaster()->GetInstanceScript();
            return _instance != NULL;
        }

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (!GetCaster())
                return;

            if (Aura* aura = GetAura())
                if (aura->GetStackAmount() == 3)
                {
                    if (auto springvale = ObjectAccessor::GetCreature(*GetCaster(), _instance->GetData64(DATA_COMMANDER_SPRINGVALE)))
                        springvale->AI()->DoAction(ACTION_RANDOM_CAST);

                    aura->Remove();
                }
        }


        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_springvale_unholy_power_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }

        InstanceScript* _instance;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_springvale_unholy_power_AuraScript();
    }
};

class spell_springvale_forsaken_ability : public SpellScriptLoader
{
public:
    spell_springvale_forsaken_ability() : SpellScriptLoader("spell_springvale_forsaken_ability") { }

    class spell_springvale_forsaken_ability_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_springvale_forsaken_ability_AuraScript);

        void HandleEffectPeriodic(AuraEffect const* aurEff)
        {
            if (!GetCaster() || !GetTarget())
                return;

            if (auto target = GetTarget()->ToPlayer())
                GetCaster()->AddAura(RAND(7038, 7039, 7040, 7041, 7042), target);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_springvale_forsaken_ability_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_springvale_forsaken_ability_AuraScript();
    }
};

class npc_springvale_wailing_guardsman : public CreatureScript
{
public:
    npc_springvale_wailing_guardsman() : CreatureScript("npc_springvale_wailing_guardsman") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_springvale_wailing_guardsmanAI(pCreature);
    }
    struct npc_springvale_wailing_guardsmanAI : public ScriptedAI
    {
        npc_springvale_wailing_guardsmanAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* pWho)
        {
            events.ScheduleEvent(EVENT_MORTAL_STRIKE, 3000);
            events.ScheduleEvent(EVENT_SCREAMS_OF_THE_PAST, urand(15000, 20000));
            events.ScheduleEvent(EVENT_UNHOLY_EMPOWERMENT, 15000);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            events.Update(uiDiff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MORTAL_STRIKE:
                    DoCastVictim(SPELL_MORTAL_STRIKE);
                    events.ScheduleEvent(EVENT_MORTAL_STRIKE, 10000);
                    break;
                case EVENT_SCREAMS_OF_THE_PAST:
                    DoCast(SPELL_SCREAMS_OF_THE_PAST);
                    events.ScheduleEvent(EVENT_SCREAMS_OF_THE_PAST, urand(15000, 20000));
                    break;
                case EVENT_UNHOLY_EMPOWERMENT:
                    if (pInstance)
                        if (auto springvale = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_COMMANDER_SPRINGVALE)))
                            DoCast(springvale, SPELL_UNHOLY_EMPOWERMENT);
                    events.ScheduleEvent(EVENT_UNHOLY_EMPOWERMENT, 15000);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_springvale_tormented_officer : public CreatureScript
{
public:
    npc_springvale_tormented_officer() : CreatureScript("npc_springvale_tormented_officer") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_springvale_tormented_officerAI(pCreature);
    }
    struct npc_springvale_tormented_officerAI : public ScriptedAI
    {
        npc_springvale_tormented_officerAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        EventMap events;

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* pWho)
        {
            events.ScheduleEvent(EVENT_UNHOLY_EMPOWERMENT, 15000);
            events.ScheduleEvent(EVENT_FORSAKEN_ABILITY, urand(10000, 30000));
            events.ScheduleEvent(EVENT_SHIELD_WALL, urand(20000, 25000));
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;

            events.Update(uiDiff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHIELD_WALL:
                    if (me->GetHealthPct() <= 30.0f)
                    DoCast(SPELL_SHIELD_WALL);
                    break;
                case EVENT_FORSAKEN_ABILITY:
                    DoCastRandom(SPELL_FORSAKEN_ABILITY, 10.0f);
                    break;
                case EVENT_UNHOLY_EMPOWERMENT:
                    if (pInstance)
                        if (auto springvale = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_COMMANDER_SPRINGVALE)))
                            DoCast(springvale, SPELL_UNHOLY_EMPOWERMENT);
                    events.ScheduleEvent(EVENT_UNHOLY_EMPOWERMENT, 15000);
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class spell_springvale_shield_Perfidious : public SpellScriptLoader
{
    /*
Normal 10: 93722
Normal 25: 93737
    */
public:
    spell_springvale_shield_Perfidious() : SpellScriptLoader("spell_springvale_shield_Perfidious") { }

    class spell_springvale_shield_Perfidious_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_springvale_shield_Perfidious_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            float left_or_right_angle{ (M_PI * 0.5f * ((GetSpellInfo()->Id == 93722 ? 25.f : 150.f) / 180.f)) };

            if (targets.size())
                if (auto c = GetCaster())
                {
                    for (auto itr = targets.begin(); itr != targets.end(); ++itr)
                        if (auto t = (*itr))
                            if (c->GetRelativeAngleOffset(t) > 0.f //to the left
                                ?
                                (c->GetRelativeAngleOffset(t) < left_or_right_angle)
                                :
                                (c->GetRelativeAngleOffset(t) > -(left_or_right_angle))
                                )
                                targets.erase(itr);
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_springvale_shield_Perfidious_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_springvale_shield_Perfidious_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_springvale_shield_Perfidious_SpellScript();
    }
};

void AddSC_boss_commander_springvale()
{
    new boss_commander_springvale();
    new npc_springvale_wailing_guardsman();
    new npc_springvale_tormented_officer();
    new spell_springvale_unholy_power();
    new spell_springvale_forsaken_ability();
    new spell_springvale_shield_Perfidious();
}