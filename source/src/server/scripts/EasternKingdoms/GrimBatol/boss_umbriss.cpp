
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "grimbatol.h"

enum Texts
{
    SAY_ENGAGE          = 0,
    SAY_EVENT           = 1,
    SAY_TROGGS          = 2,
    SAY_BOMB            = 3,
    SAY_SLAY            = 4,
    SAY_DEATH           = 5,
    EMOTE_BLITZ         = 6,
    EMOTE_GROUND_SIEGE  = 7,
    EMOTE_FRENZY        = 8
};

enum Spells
{
    SPELL_BLEEDING_WOUND            = 74846,
    SPELL_BLEEDING_WOUND_H          = 91937,
    SPELL_GROUND_SIEGE              = 74634,
    SPELL_GROUND_SIEGE_H            = 90249,

    SPELL_BLITZ_TRIGGER             = 74668,

    SPELL_BLITZ                     = 74670,
    SPELL_BLITZ_H                   = 90250,
    SPELL_FRENZY                    = 74853,
    // Adds
    SPELL_CLAW_PUNCTURE             = 76507,
    SPELL_CLAW_PUNCTURE_H           = 90212,

    SPELL_MODGUD_MALICE             = 74699,
    SPELL_MODGUD_MALICE_H           = 90169,

    SPELL_MODGUDS_MALADY            = 74837,
    SPELL_MODGUD_MALICE_AOE         = 90170,
};

struct Locations
{
    float x, y, z, o;
};

static Locations SpawnPosition[]=
{
    {-722.15f, -442.53f, 268.77f, 0.54f},
    {-702.22f, -450.9f, 268.77f, 1.34f}
};

class boss_general_umbriss : public CreatureScript
{
    public:

        boss_general_umbriss() : CreatureScript("boss_general_umbriss") {}

        struct boss_general_umbrissAI : public BossAI
        {
            boss_general_umbrissAI(Creature *c) : BossAI(c, DATA_GENERAL_UMBRISS_EVENT)
            {
                instance = c->GetInstanceScript();

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
            }

            void Reset()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                woundTimer = 12000;
                groundTimer = 20000;
                blitzTimer = 10000;
                addTimer = 18000;
                groundTimer2 = 200000;
                tempGUID = 0;
                isMalignantSpawn = false;
                enrage = false;
                me->RemoveAurasDueToSpell(SPELL_MODGUD_MALICE);
                me->RemoveAurasDueToSpell(SPELL_FRENZY);
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                if (instance)
                    instance->SetData(DATA_GENERAL_UMBRISS_EVENT, NOT_STARTED);

                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, NPC_MALIGNANT_TROGG, 200.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_TROGG_DWELLER, 200.0f);
                for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                    if ((*itr)->ToTempSummon())
                        (*itr)->DespawnOrUnsummon();
            }


            void EnterCombat(Unit* /*who*/)
            {
                if (instance)
                    instance->SetData(DATA_GENERAL_UMBRISS_EVENT, IN_PROGRESS);
                Talk(SAY_ENGAGE);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void JustDied(Unit* /*killer*/)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _JustDied();
                instance->SetBossState(DATA_GENERAL_UMBRISS, DONE);
                instance->SetData(DATA_GENERAL_UMBRISS_EVENT, DONE);
                Talk(SAY_DEATH);
                if (instance)
                {
                    instance->SetData(DATA_GENERAL_UMBRISS_EVENT, DONE);
                    if (IsHeroic() && me->HasAura(SPELL_MODGUD_MALICE_H))
                        instance->DoCompleteAchievement(ACHIEV_UMBRAGE_FOR_UMBRISS);
                }

                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, NPC_MALIGNANT_TROGG, 200.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_TROGG_DWELLER, 200.0f);
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                    if ((*itr)->ToTempSummon())
                        (*itr)->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* /*victim*/)
            {
                Talk(SAY_SLAY);
            }

            void UpdateAI(const uint32 uiDiff)
            {
                if (!UpdateVictim())
                    return;

                if (woundTimer <= uiDiff)
                {
                    DoCastVictim(SPELL_BLEEDING_WOUND);
                    woundTimer = 25000;
                }
                else
                    woundTimer -= uiDiff;

                if (groundTimer <= uiDiff)
                {
                    if (auto unit = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me, true)))
                    {
                        Talk(EMOTE_GROUND_SIEGE);
                        Position pos;
                        unit->GetPosition(&pos);
                        if (auto temp = me->SummonCreature(NPC_GROUND_SIEGE_STALKER, pos, TEMPSUMMON_TIMED_DESPAWN, 5000))
                        {
                            me->StopMoving();
                            me->SetFacingToObject(temp);
                            me->SendMeleeAttackStop();
                            DoCast(me, SPELL_GROUND_SIEGE);
                        }
                    }
                    blitzTimer = 12000;
                    groundTimer = 20000;
                }
                else
                    groundTimer -= uiDiff;

                if (blitzTimer <= uiDiff)
                {
                    if (auto unit = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    {
                        Talk(EMOTE_BLITZ, unit->GetGUID());
                        if (Creature *c = me->SummonCreature(NPC_TRIGGER_BLITZ, unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                        {
                            me->StopMoving();
                            me->SetFacingToObject(c);
                            me->SendMeleeAttackStop();
                            me->CastSpell(c, SPELL_BLITZ, false);
                        }
                    }
                    groundTimer = 13000;
                    blitzTimer = 20000;
                }
                else
                    blitzTimer -= uiDiff;

                if(!enrage)
                {
                    if(addTimer <= uiDiff)
                    {
                        Talk(SAY_TROGGS);
                        SpawnMalignant();
                        SpawnAdd();
                        addTimer = 60000;

                    }
                    else
                        addTimer -= uiDiff;

                    if (HealthBelowPct(30))
                    {
                        enrage = true;
                        Talk(EMOTE_FRENZY);
                        me->AddAura(SPELL_FRENZY, me);
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SpawnAdd()
            {
                for(int i = 0; i < 3; i ++)
                    SummonCreatureWithRandomTarget(NPC_TROGG_DWELLER, SpawnPosition[0].x,SpawnPosition[0].y,SpawnPosition[0].z,SpawnPosition[0].o);
            }

            void SpawnMalignant()
            {
                if (!isMalignantSpawn && !enrage)
                {
                    SummonCreatureWithRandomTarget(NPC_MALIGNANT_TROGG, SpawnPosition[1].x,SpawnPosition[1].y,SpawnPosition[1].z,SpawnPosition[1].o);
                    isMalignantSpawn = true;
                }
            }

            void SummonCreatureWithRandomTarget(uint32 creatureId, float x, float y, float z, float o)
            {
                Creature* Summoned = me->SummonCreature(creatureId, x, y, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5000);
                if (Summoned)
                {
                    Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0);
                    if (pTarget)
                    {
                        Summoned->AddThreat(pTarget, 1.0f);
                        if (Summoned->AI())
                            Summoned->AI()->AttackStart(pTarget);
                    }
                }
            }


            bool isMalignantSpawn;
        private :
            uint32 blitzTimer;
            uint32 groundTimer;
            uint32 groundTimer2;
            uint32 woundTimer;
            uint32 addTimer;
            bool enrage;
            uint64 tempGUID;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new boss_general_umbrissAI(pCreature);
        }
};


class npc_trogg_dweller : public CreatureScript
{
public:

    npc_trogg_dweller() : CreatureScript("npc_trogg_dweller")
    {
    }

    struct npc_trogg_dwellerAI : public ScriptedAI
    {
        npc_trogg_dwellerAI(Creature* c) : ScriptedAI(c)
        {
            if (me->GetEntry() == NPC_MALIGNANT_TROGG)
                if (!me->HasAura(IsHeroic() ? SPELL_MODGUD_MALICE_H : SPELL_MODGUD_MALICE))
                    me->AddAura(IsHeroic() ? SPELL_MODGUD_MALICE_H : SPELL_MODGUD_MALICE, me);
        }

        void DamageTaken(Unit* /*done_by*/, uint32& damage)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!me->FindNearestCreature(BOSS_GENERAL_UMBRISS, 50.0f, true))
                if (me->ToTempSummon())
                    me->DespawnOrUnsummon();

            if (UpdateVictim())
                DoSpellAttackIfReady(SPELL_CLAW_PUNCTURE);
        }
        void SpellHit(Unit* caster, SpellInfo const* spell) 
        {
            if (spell->Id == SPELL_MODGUD_MALICE_AOE)
                if (IsHeroic())
                    if (me->GetEntry() != NPC_MALIGNANT_TROGG)
                    {
                        me->UpdateEntry(NPC_MALIGNANT_TROGG);
                        if (!me->HasAura(IsHeroic() ? SPELL_MODGUD_MALICE_H : SPELL_MODGUD_MALICE))
                            me->AddAura(IsHeroic() ? SPELL_MODGUD_MALICE_H : SPELL_MODGUD_MALICE, me);
                    }
        }

        void JustDied(Unit* killer)
        {
            if (me->GetEntry() == NPC_MALIGNANT_TROGG)
            {
                DoCastAOE(SPELL_MODGUDS_MALADY, true);
                DoCastAOE(SPELL_MODGUD_MALICE_AOE, true);
                if (auto umbriss = me->FindNearestCreature(BOSS_GENERAL_UMBRISS, 100.0f, true))
                    if (auto ai = umbriss->AI())
                        CAST_AI(boss_general_umbriss::boss_general_umbrissAI, ai)->isMalignantSpawn = false;
                me->DespawnOrUnsummon();
            }
            else
            {
                if (killer->IsVehicle())
                    if (auto scr = me->GetInstanceScript())
                        scr->DoOnPlayers([](Player* player)
                            {
                                player->KilledMonsterCredit(51182);
                            });
            }

        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_trogg_dwellerAI(pCreature);
    }
};


class spell_bleeding_wound : public SpellScriptLoader
{
public:
    spell_bleeding_wound() : SpellScriptLoader("spell_bleeding_wound") { }

    class spell_bleeding_wound_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_bleeding_wound_AuraScript);

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            if (GetUnitOwner()->HealthAbovePct(90))
            {
                PreventDefaultAction();
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_bleeding_wound_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_bleeding_wound_AuraScript();
    }
};

class spell_GB_umbriss_trogg_malady : public SpellScriptLoader
{
public:
    spell_GB_umbriss_trogg_malady() : SpellScriptLoader("spell_GB_umbriss_trogg_malady") {}

    class spell_GB_umbriss_trogg_malady_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_GB_umbriss_trogg_malady_SpellScript);

        void FilterTargets_Damage(std::list<WorldObject*>& targets)
        {
            //TC_LOG_ERROR("sql.sql", "FilterTargets_Damage");
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                    if (auto this_target = (*itr))
                        if (auto unit = this_target->ToUnit())
                            if (unit->GetEntry() == BOSS_GENERAL_UMBRISS)
                                itr++;
                            else
                            {
                                targets.erase(itr++);
                            }
        }

        void FilterTargets_ProcOthers(std::list<WorldObject*>& targets)
        {
            //TC_LOG_ERROR("sql.sql", "FilterTargets_ProcOthers");
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                    if (auto this_target = (*itr))
                        if (auto unit = this_target->ToUnit())
                            if (unit->GetEntry() == NPC_TROGG_DWELLER || unit->GetEntry() == NPC_MALIGNANT_TROGG)
                                itr++;
                            else
                            {
                                targets.erase(itr++);
                            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_GB_umbriss_trogg_malady_SpellScript::FilterTargets_Damage, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_GB_umbriss_trogg_malady_SpellScript::FilterTargets_Damage, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_GB_umbriss_trogg_malady_SpellScript::FilterTargets_ProcOthers, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_GB_umbriss_trogg_malady_SpellScript();
    }
};


void AddSC_boss_general_umbriss()
{
    new boss_general_umbriss();
    new npc_trogg_dweller();
    new spell_bleeding_wound();
    new spell_GB_umbriss_trogg_malady();
}
