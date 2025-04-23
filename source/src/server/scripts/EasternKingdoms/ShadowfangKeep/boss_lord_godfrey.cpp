#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "shadowfang_keep.h"

enum Spells
{
    SPELL_CURSED_BULLETS                   = 93629,
    SPELL_MORTAL_WOUND                     = 93675,
    SPELL_PISTOL_BARRAGE                   = 93520,
    SPELL_PISTOL_BARRAGE_DAMAGE            = 93564,
    SPELL_PISTOL_BARRAGE_TARGETS           = 96343,
    SPELL_SUMMON_GHOUL_CHANNELED           = 93707,
    SPELL_SUMMON_GHOUL_1                   = 93709,
    SPELL_SUMMON_GHOUL_2                   = 93714,
    SPELL_ACHIEVEMENT_CREDIT               = 95929
};

enum Events
{
    EVENT_CURSED_BULLETS                   = 1,
    EVENT_MORTAL_WOUND,
    EVENT_PISTOL_BARRAGE,
    EVENT_SUMMON_BLOODTHIRSTY_GHOULS,
    ACTION_CORRECT_ORIENTATION,
};

enum eTexts
{
    SAY_AGGRO_A,
    SAY_AGGRO_H,
    SAY_DEATH,
    SAY_PISTOLS,
};

class boss_lord_godfrey : public CreatureScript
{
    public:
        boss_lord_godfrey() : CreatureScript("boss_lord_godfrey") {}

        struct boss_lord_godfreyAI : public BossAI
        {
            boss_lord_godfreyAI(Creature* creature) : BossAI(creature, DATA_LORD_GODFREY_EVENT) 
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_PISTOL_BARRAGE_DAMAGE, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, 93784, true);
            }

            void Reset()
            {
                _Reset();
                if (auto ivar = me->FindNearestCreature(instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_QUESTGIVER_BELMONT : NPC_QUESTGIVER_IVAR, 200.f))
                {
                    ivar->SetVisible(true);
                }
            }

            void EnterCombat(Unit* who)
            {
                _EnterCombat();

                Talk(instance->GetData(TEAM_IN_INSTANCE) == HORDE ? SAY_AGGRO_H : SAY_AGGRO_A);
                events.ScheduleEvent(EVENT_SUMMON_BLOODTHIRSTY_GHOULS, 25000);
                events.ScheduleEvent(EVENT_MORTAL_WOUND, 18000);
                events.ScheduleEvent(EVENT_PISTOL_BARRAGE, 10000);
                events.ScheduleEvent(EVENT_CURSED_BULLETS, 15000);
                Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    i->getSource()->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEVEMENT_CRITERIA_CONDITION_NONE, SPELL_ACHIEVEMENT_CREDIT, true);
                if (auto ivar = me->FindNearestCreature(instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_QUESTGIVER_BELMONT : NPC_QUESTGIVER_IVAR, 200.f))
                {
                    ivar->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    ivar->setFaction(35);
                    ivar->SetVisible(false);
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
                if (IsHeroic())
                    instance->FinishLfgDungeon(me);
                if (auto ivar = me->FindNearestCreature(instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_QUESTGIVER_BELMONT : NPC_QUESTGIVER_IVAR, 200.f))
                {
                    ivar->SetVisible(true);
                }
            }

            void KilledUnit(Unit* victim)
            {
                //has no quotes for killing players.
            }

            void DoAction(int32 const id)
            {
                if (id == ACTION_CORRECT_ORIENTATION)
                    me->SetFacingTo(lock_orientation);
            }

            void JustSummoned(Creature* summon)
            {
                BossAI::JustSummoned(summon);
                if (summon->GetEntry() == 52065)
                {
                    DoCast(me, SPELL_PISTOL_BARRAGE);
                    DoAction(ACTION_CORRECT_ORIENTATION);
                }
            }

            void SummonedCreatureDies(Creature* summoned, Unit* killer)
            {
                if (IsHeroic() && summoned->GetEntry() == 50561 && killer == me) // Bullet Time Achievement
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_CREDIT);
            }

            void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
            {
                //TC_LOG_ERROR("sql.sql", "Finished executing %u with reason %u", spell->Id, uint32(reason));
                if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
                switch (spell->Id)
                {
                case SPELL_SUMMON_GHOUL_CHANNELED:
                case SPELL_PISTOL_BARRAGE:
                    //TC_LOG_ERROR("sql.sql", "entered return-to-hostility block.");
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();
                    break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch(eventId)
                    {
                        case EVENT_SUMMON_BLOODTHIRSTY_GHOULS:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //TC_LOG_ERROR("sql.sql", "EVENT_SUMMON_BLOODTHIRSTY_GHOULS");
                                DoCast(me, SPELL_SUMMON_GHOUL_CHANNELED);
                                events.ScheduleEvent(EVENT_SUMMON_BLOODTHIRSTY_GHOULS, 25000);
                            }
                            return;
                        case EVENT_MORTAL_WOUND:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //TC_LOG_ERROR("sql.sql", "EVENT_MORTAL_WOUND");
                                DoCastVictim(SPELL_MORTAL_WOUND);
                                events.ScheduleEvent(EVENT_MORTAL_WOUND, 7000);
                            }
                            break;
                        case EVENT_PISTOL_BARRAGE:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                Talk(SAY_PISTOLS);
                                //TC_LOG_ERROR("sql.sql", "EVENT_PISTOL_BARRAGE");
                                lock_orientation = me->GetOrientation();
                                DoCast(SPELL_PISTOL_BARRAGE_TARGETS);
                                events.ScheduleEvent(EVENT_PISTOL_BARRAGE, 30000);
                            }
                            break;
                        case EVENT_CURSED_BULLETS:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                                //TC_LOG_ERROR("sql.sql", "EVENT_CURSED_BULLETS");
                                DoCastRandom(SPELL_CURSED_BULLETS, 100.0f);
                                events.ScheduleEvent(EVENT_CURSED_BULLETS, 15000);
                            }
                            break;
                    }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                    DoMeleeAttackIfReady();
            }
        private:
            float lock_orientation{ 0.f };
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_lord_godfreyAI(creature);
        }
};

class spell_godfrey_summon_ghouls : public SpellScriptLoader
{
public:
    spell_godfrey_summon_ghouls() : SpellScriptLoader("spell_godfrey_summon_ghouls") { }

    class spell_godfrey_summon_ghouls_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_godfrey_summon_ghouls_AuraScript);

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            GetCaster()->CastSpell(GetCaster(), RAND(SPELL_SUMMON_GHOUL_1, SPELL_SUMMON_GHOUL_2), true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_godfrey_summon_ghouls_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_godfrey_summon_ghouls_AuraScript();
    }
};

class spell_pistol_barrage_trigger : public SpellScriptLoader
{
public:
    spell_pistol_barrage_trigger() : SpellScriptLoader("spell_pistol_barrage_trigger") { }

    class spell_pistol_barrage_trigger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pistol_barrage_trigger_SpellScript);

        void SpawnInAngle(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);

            if (auto c = GetCaster())
            for(uint8 i = 0; i < 6; ++i)
            {
                Position pos(*c);
                float angle = -5 * M_PI / 24 + M_PI * i / 12;
                c->MoveBlink(pos, 10.f, angle);
                c->SummonCreature(entry, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000);

                if (auto cr = c->ToCreature())
                    if (auto ai = cr->AI())
                        ai->DoAction(ACTION_CORRECT_ORIENTATION);
            }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_pistol_barrage_trigger_SpellScript::SpawnInAngle, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pistol_barrage_trigger_SpellScript();
    }
};


class spell_pistol_barrage : public SpellScriptLoader
{
public:
    spell_pistol_barrage() : SpellScriptLoader("spell_pistol_barrage") { }

    class spell_pistol_barrage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pistol_barrage_AuraScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PISTOL_BARRAGE))
                return false;
            return true;
        }

        void OnPeriodic(AuraEffect const* aurEff)
        {
            if (auto caster = GetCaster())
                caster->CastSpell((Unit*)NULL, SPELL_PISTOL_BARRAGE_DAMAGE, true);

            if (auto c = GetCaster())
            if (auto cr = c->ToCreature())
                if (auto ai = cr->AI())
                    ai->DoAction(ACTION_CORRECT_ORIENTATION);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_pistol_barrage_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pistol_barrage_AuraScript();
    }
};

class spell_cursed_bullets : public SpellScriptLoader
{
public:
    spell_cursed_bullets() : SpellScriptLoader("spell_cursed_bullets") { }

    class spell_cursed_bullets_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_cursed_bullets_AuraScript);

        void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
        {
            uint32 bp = (GetOwner()->GetMap()->IsHeroic() ? 10000 : 50);
            aurEff->SetAmount(bp * aurEff->GetTickNumber());
        }

        void Register()
        {
            OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_cursed_bullets_AuraScript::HandleEffectPeriodicUpdate, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_cursed_bullets_AuraScript();
    }
};

class spell_pistol_barrage_dmg : public SpellScriptLoader
{
public:
    spell_pistol_barrage_dmg() : SpellScriptLoader("spell_pistol_barrage_dmg") { }

    class spell_pistol_barrage_dmg_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pistol_barrage_dmg_SpellScript);


        void ChangeDamage(SpellEffIndex /*effIndex*/)
        {
            int32 damage = (GetHitDamage() / 2);
            SetHitDamage(damage);
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                if (auto c = GetCaster())
                    for (auto itr = targets.begin(); itr != targets.end();)
                        if (auto t = (*itr))
                        {
                            if (t->GetEntry() == NPC_QUESTGIVER_IVAR || t->GetEntry() == NPC_QUESTGIVER_BELMONT)
                            {
                                targets.erase(itr++);
                            }
                            else
                                ++itr;
                        }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_pistol_barrage_dmg_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pistol_barrage_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_24);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pistol_barrage_dmg_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENTRY);
        }

    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_pistol_barrage_dmg_SpellScript();
    }
};



void AddSC_boss_lord_godfrey()
{
    new boss_lord_godfrey();
    new spell_godfrey_summon_ghouls();
    new spell_pistol_barrage_trigger();
    new spell_pistol_barrage();
    new spell_cursed_bullets();
    new spell_pistol_barrage_dmg();
}
