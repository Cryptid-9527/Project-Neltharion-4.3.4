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

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "halls_of_origination.h"

enum Texts
{
    SAY_AGGRO       = 0,
    SAY_EVENT       = 1,
    SAY_SLAY        = 2,
    SAY_DEATH       = 3,
};

enum Spells
{
    //Setesh
    SPELL_CHAOS_BOLT                        = 77069,
    SPELL_SUMMON_REIGN_OF_CHAOS             = 77023,
    SPELL_REIGN_OF_CHAOS                    = 77026, // visual + damage trigger
    SPELL_REIGN_OF_CHAOS_DAMAGE             = 77030, // 89872 hc --> dummy effect need condition to setesh  ( effect #1 )
    SPELL_CHAOS_BLAST                       = 76674, // summon spell
    SPELL_CHAOS_BLAST_VISUAL                = 76676, // spliting visual need to condition to entry 41041
    SPELL_CHAOS_BLAST_AURA                  = 76681,
    SPELL_SEED_OF_CHAOS                     = 76870,
    SPELL_SUMMON_CHAOS_SEED                 = 76888, // summons entry 41126
    SPELL_SEED_OF_CHAOS_DAMAGE              = 76870,
    SPELL_CHAOS_SEED_CRYSTAL_VISUAL         = 76865, // this could trigger the damage ?
    SPELL_PORTAL_CHANNEL                    = 76784, // need condition to trigger (entry 41055)
    SPELL_NIGHTMARE_PORTAL                  = 76714, // on heroic the portal stays until encounter end, on nhc the portal can be killed
    SPELL_NIGHTMARE_PORTAL_SUMMON_VISUAL    = 77607, // casted on summon
    SPEL_CHAOS_BURN                         = 77128, // casted by chaos blast trigger (entry 41041);
    SPELL_CHAOS_BURN                        = 76684, // area damage. need more research about the spell (fuu fail hordeguides and my brain for forgeting such things)
    //Sentinel
    SPELL_VOID_BARRIER                      = 76959,
    SPELL_CHARGED_FISTS                     = 77238,
    //Seeker
    SPELL_VOID_INFUSION_BEAM                = 77461,
    SPELL_VOID_INFUSION                     = 77470,
    SPELL_SHADOWBOLT_VOLLEY                 = 89846,
    SPELL_ANTIMAGIC_PRISON                  = 76903,
    //Lord
    SPELL_GROW                              = 77466,
    SPELL_VOID_BURST                        = 77475,
};

enum Events
{
    EVENT_CHAOS_BOLT            = 1,
    EVENT_REIGN_OF_CHAOS        = 2,
    EVENT_CHAOS_BLAST           = 3,
    EVENT_SUMMON_SEED_OF_CHAOS  = 4,
    EVENT_CHAOS_PORTAL          = 5,
    EVENT_SUMMON_PORTAL         = 6,
    EVENT_CHAOS_BLAST_AURA      = 7,
    EVENT_SUMMON_PORTAL_RECOVER,
};

enum Points
{
    POINT_PORTAL = 1,
};

class boss_setesh : public CreatureScript
{
    public:
        boss_setesh() : CreatureScript("boss_setesh") { }

        struct boss_seteshAI : public BossAI
        {
            boss_seteshAI(Creature* creature) : BossAI(creature, DATA_SETESH) { }

            void InitializeAI()
            {
                me->SetMaxPower(POWER_MANA, 65170);
                me->SetPower(POWER_MANA, 65170);
            }
            void Reset()
            {
                me->SetMaxPower(POWER_MANA, 65170);
                me->SetPower(POWER_MANA, 65170);
                _Reset();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                Cleanup();
            }

            void Cleanup()
            {
                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, 41041, 100.0f);
                GetCreatureListWithEntryInGrid(units, me, 41168, 100.0f);
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                    if (auto npc = (*itr))
                        npc->DespawnOrUnsummon();

                summons.DespawnAll();
                me->DeleteThreatList();
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                Talk(SAY_AGGRO);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_CHAOS_BOLT, 6000);       // every 2 seconds
                events.ScheduleEvent(EVENT_REIGN_OF_CHAOS, 20000);
                events.ScheduleEvent(EVENT_CHAOS_BLAST, 15000);
                events.ScheduleEvent(EVENT_SUMMON_SEED_OF_CHAOS, 25000);
                events.ScheduleEvent(EVENT_CHAOS_PORTAL, urand(8000, 15000));
            }

            void KilledUnit(Unit* /*Killed*/)
            {
                Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon)
            {
                BossAI::JustSummoned(summon);

                switch (summon->GetEntry())
                {
                    case NPC_CHAOS_PORTAL:
                        summon->CastWithDelay(3500, summon, SPELL_NIGHTMARE_PORTAL, true);
                        DoCast(summon, SPELL_PORTAL_CHANNEL);
                        summon->SetReactState(REACT_PASSIVE);
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        if (!IsHeroic())
                            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId)
            {
                if (type != POINT_MOTION_TYPE)
                    return;
                
                switch (pointId)
                {
                    case POINT_PORTAL:
                        events.ScheduleEvent(EVENT_SUMMON_PORTAL, 1);
                        break;
                }
            }

            void DoAction(int32 const act)
            {
                switch (act)
                {
                case EVENT_SUMMON_PORTAL_RECOVER:
                    events.ScheduleEvent(EVENT_SUMMON_PORTAL_RECOVER, 1);
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_CHAOS_BOLT:
                            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                                events.ScheduleEvent(eventId, 250);
                            else
                            {
                                DoCastRandom(SPELL_CHAOS_BOLT, 0.0f);
                                events.ScheduleEvent(EVENT_CHAOS_BOLT, 2000);
                            }
                            break;
                        case EVENT_REIGN_OF_CHAOS:
                            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                                events.ScheduleEvent(eventId, 250);
                            else
                            {
                                DoCastAOE(SPELL_SUMMON_REIGN_OF_CHAOS);
                                events.ScheduleEvent(EVENT_REIGN_OF_CHAOS, urand(30000, 40000));
                                Talk(SAY_EVENT);
                            }
                            break;
                        case EVENT_CHAOS_BLAST:
                            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                                events.ScheduleEvent(eventId, 250);
                            else
                            {
                                DoCast(SPELL_CHAOS_BLAST);
                                DoCast(SPELL_CHAOS_BLAST_VISUAL);
                                events.ScheduleEvent(EVENT_CHAOS_BLAST, 15000);
                            }
                            break;
                        case EVENT_SUMMON_SEED_OF_CHAOS:
                            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                                events.ScheduleEvent(eventId, 250);
                            else
                            {
                                DoCast(SPELL_SUMMON_CHAOS_SEED);
                                events.ScheduleEvent(EVENT_SUMMON_SEED_OF_CHAOS, 25000);
                            }
                            break;
                        case EVENT_CHAOS_PORTAL:
                            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                                events.ScheduleEvent(eventId, 250);
                            else
                            {
                                me->SetReactState(REACT_PASSIVE);
                                me->StopMoving();
                                me->AttackStop();
                                if (auto trigger = me->SummonCreature(54638, Portal_Locations[urand(0, 3)], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    Position Walk_To(*trigger);
                                    trigger->MoveBlink(Walk_To, 15.f, trigger->NormalizeOrientation(trigger->GetRelativeAngle(me->GetPositionX(), me->GetPositionY())));
                                    me->GetMotionMaster()->MovePoint(POINT_PORTAL, Walk_To);
                                }

                            }
                            break;
                        case EVENT_SUMMON_PORTAL:
                            if (auto trigger = me->FindNearestCreature(54638, 50.f, true))
                            if (auto portal = me->SummonCreature(NPC_CHAOS_PORTAL, *trigger))
                            {
                                trigger->DespawnOrUnsummon();
                            }
                            break;
                        case EVENT_SUMMON_PORTAL_RECOVER:
                            events.ScheduleEvent(EVENT_CHAOS_PORTAL, 18000);
                            me->SetReactState(REACT_AGGRESSIVE);
                            UpdateVictim();
                            break;
                        default:
                            break;
                    }
            
                if (!me->HasUnitState(UNIT_STATE_CASTING) && me->HasReactState(REACT_AGGRESSIVE))
                {
                    UpdateVictim();
                    DoMeleeAttackIfReady();
                }
            }

            void JustDied(Unit* /*who*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                Cleanup();
            }
            private:
                Position Portal_Locations[4]
                {
                    {-475.882233f, -20.851961f, 343.921509f, 2.418205f},    //NE
                    {-477.034943f, 32.248222f,  343.923492f, 3.906531f},    //NW
                    {-536.918396f, 30.387966f,  343.920502f, 5.661885f},    //SW
                    {-539.897644f, -17.365786f, 343.915802f, 0.658895f},    //SE
                };
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_seteshAI(creature);
        }
};

struct npc_chaos_portal : public Scripted_NoMovementAI
{
    npc_chaos_portal(Creature* creature) : Scripted_NoMovementAI(creature) { }

    enum PortalEvents
    {
        EVENT_SUMMON_VOID_SENTINEL  = 1,
        EVENT_SUMMON_VOID_WYRMS     = 2,
        EVENT_SUMMON_VOID_SEEKER    = 3,
        EVENT_SUMMON_VOID_RANDOM    = 4,
    };

    void Reset()
    {
        me->SetReactState(REACT_PASSIVE);
    }


    void SpellHit(Unit* caster, SpellInfo const* spell) 
    {
            if (spell->Id == SPELL_NIGHTMARE_PORTAL)
            {

                if (auto set = me->FindNearestCreature(BOSS_SETESH, 50.f, true))
                    set->AI()->DoAction(EVENT_SUMMON_PORTAL_RECOVER);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->AddAura(SPELL_NIGHTMARE_PORTAL_SUMMON_VISUAL, me);
                if (IsHeroic())
                {
                    events.ScheduleEvent(EVENT_SUMMON_VOID_SENTINEL, 1s);
                    events.ScheduleEvent(EVENT_SUMMON_VOID_WYRMS, 7s);
                    events.ScheduleEvent(EVENT_SUMMON_VOID_SEEKER, 17s);
                }
                else
                {
                    events.ScheduleEvent(EVENT_SUMMON_VOID_WYRMS, 3s);
                    events.ScheduleEvent(EVENT_SUMMON_VOID_SEEKER, 15s);
                    events.ScheduleEvent(EVENT_SUMMON_VOID_SENTINEL, 30s);
                }
            }
    }
    void JustSummoned(Creature* summon)
    {
        summon->SetInCombatWithZone();
        summon->SetCorpseDelay(2000);

        if (auto setesh = Creature::GetCreature(*me, instance->GetData64(DATA_SETESH_GUID)))
            setesh->AI()->JustSummoned(summon);
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_SUMMON_VOID_WYRMS:
            {
                me->SummonCreature(NPC_VOID_WYRM, me->GetPositionX() - 2, me->GetPositionY(), me->GetPositionZ());
                me->SummonCreature(NPC_VOID_WYRM, me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ());
                break;
            }
            case EVENT_SUMMON_VOID_SEEKER:
            {
                me->SummonCreature(NPC_VOID_SEEKER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                break;
            }
            case EVENT_SUMMON_VOID_SENTINEL:
            {
                me->SummonCreature(NPC_VOID_SENTINEL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SUMMON_VOID_RANDOM, 15s);
                else
                    me->DespawnOrUnsummon(3000);
                break;
            }
            case EVENT_SUMMON_VOID_RANDOM:
            {
                if (urand(0, 1))
                {
                    me->SummonCreature(NPC_VOID_WYRM, me->GetPositionX() - 2, me->GetPositionY(), me->GetPositionZ());
                    me->SummonCreature(NPC_VOID_WYRM, me->GetPositionX() + 2, me->GetPositionY(), me->GetPositionZ());
                }
                else
                    me->SummonCreature(NPC_VOID_SEEKER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                events.Repeat(15s);
                break;
            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        me->DespawnOrUnsummon();
    }
};

enum sentinel
{
    EVENT_CHARGE_FIST   = 1,
};

class npc_setesh_void_sentinel : public CreatureScript
{
public:
    npc_setesh_void_sentinel() : CreatureScript("npc_setesh_void_sentinel") { }

    struct npc_setesh_void_sentinelAI : public ScriptedAI
    {
        npc_setesh_void_sentinelAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            if (!me->HasAura(SPELL_VOID_BARRIER))
                me->AddAura(SPELL_VOID_BARRIER, me);
            events.Reset();
        }
        void InitializeAI()
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
        }

        void EnterCombat(Unit* who)
        {
            if (!me->HasAura(SPELL_VOID_BARRIER))
                me->AddAura(SPELL_VOID_BARRIER, me);
            events.ScheduleEvent(EVENT_CHARGE_FIST, 5000);
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
                case EVENT_CHARGE_FIST:
                    spell = SPELL_CHARGED_FISTS;         //Spell ID
                    min_repeat = 7000;                 //Minimum Delay
                    max_repeat = 30000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
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

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon();
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_setesh_void_sentinelAI(creature);
    }
};

struct npc_setesh_void_seeker : public ScriptedAI
{
    npc_setesh_void_seeker(Creature* creature) : ScriptedAI(creature) { }

    void Reset()
    {
        if (me->GetDBTableGUIDLow())
            DoCastAOE(SPELL_VOID_INFUSION_BEAM);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        events.ScheduleEvent(SPELL_ANTIMAGIC_PRISON, 5s);
        events.ScheduleEvent(SPELL_SHADOWBOLT_VOLLEY, 12s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_ANTIMAGIC_PRISON:
                DoCastRandom(SPELL_ANTIMAGIC_PRISON, 0.f);
                events.Repeat(25s);
                break;
            case SPELL_SHADOWBOLT_VOLLEY:
                DoCastAOE(SPELL_SHADOWBOLT_VOLLEY);
                events.Repeat(12s);
                break;
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        // used to despawn corpse immediately
        me->DespawnOrUnsummon();
    }
};

struct npc_setesh_void_lord : public ScriptedAI
{
    npc_setesh_void_lord(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        SetCombatMovement(false);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        DoCastAOE(SPELL_VOID_INFUSION);

        events.ScheduleEvent(SPELL_VOID_BURST, 30200ms);
        events.ScheduleEvent(SPELL_SHADOWBOLT_VOLLEY, 42s);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_VOID_BURST:
                DoCastAOE(SPELL_VOID_BURST);
                me->AddAura(SPELL_GROW, me)->SetStackAmount(6);

                SetCombatMovement(true);
                if (auto target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                    me->GetMotionMaster()->MoveChase(target);

                break;
            case SPELL_SHADOWBOLT_VOLLEY:
                DoCastAOE(SPELL_SHADOWBOLT_VOLLEY);
                events.Repeat(12s);
                break;
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        // used to despawn corpse immediately
        me->DespawnOrUnsummon();
    }
};

class npc_chaos_blast : public CreatureScript
{
public:
    npc_chaos_blast() : CreatureScript("npc_chaos_blast") { }

    struct npc_chaos_blastAI : public ScriptedAI
    {
        npc_chaos_blastAI(Creature* creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetInCombatWithZone();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chaos_blastAI(creature);
    }
};

class npc_seed_of_chaos : public CreatureScript
{
public:
    npc_seed_of_chaos() : CreatureScript("npc_seed_of_chaos") { }

    struct npc_seed_of_chaosAI : public Scripted_NoMovementAI
    {
        npc_seed_of_chaosAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetInCombatWithZone();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
            me->SetDisplayId(me->GetCreatureTemplate()->Modelid1);
            me->setFaction(16);
            me->AddAura(SPELL_CHAOS_SEED_CRYSTAL_VISUAL, me);
            spellHit = false;
            checkTimer = 1000;
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spellInfo)
        {
            uint32 chaoesAoe = sSpellMgr->GetSpellIdForDifficulty(SPELL_SEED_OF_CHAOS_DAMAGE, me);
            if (!spellHit && spellInfo->Id == chaoesAoe)
            {
                spellHit = true; // avoid double buff
                me->AddAura(SPELL_SEED_OF_CHAOS_DAMAGE, target);
                me->DespawnOrUnsummon(500);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (checkTimer <= diff)
            {
                if (auto target = me->FindNearestPlayer(3.0f))
                    DoCastAOE(SPELL_SEED_OF_CHAOS_DAMAGE);
                checkTimer = 1000;
            } else checkTimer -= diff;
        }

    private:
        uint32 checkTimer;
        bool spellHit;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seed_of_chaosAI(creature);
    }
};

class spell_hoo_chaos_blast : public SpellScriptLoader
{
public:
    spell_hoo_chaos_blast() : SpellScriptLoader("spell_hoo_chaos_blast") { }

    class spell_hoo_chaos_blast_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hoo_chaos_blast_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* target) 
            { 
                if (target->GetTypeId() != TYPEID_UNIT || target->ToUnit()->HasAura(SPELL_CHAOS_BLAST_AURA) || target->GetEntry() != 41041)
                    return true;
                return false;
            });
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            GetHitUnit()->CastSpell(GetHitUnit(), SPELL_CHAOS_BLAST_AURA, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_hoo_chaos_blast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hoo_chaos_blast_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_hoo_chaos_blast_SpellScript();
    }
};

class npc_reign_of_chaos : public CreatureScript
{
public:
    npc_reign_of_chaos() : CreatureScript("npc_reign_of_chaos") { }

    struct npc_reign_of_chaosAI : public Scripted_NoMovementAI
    {
        npc_reign_of_chaosAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_REIGN_OF_CHAOS, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            checkTimer = 1000;
        }

        void UpdateAI(uint32 const diff)
        {
            if (checkTimer <= diff)
            {
                me->SetObjectScale(me->GetFloatValue(OBJECT_FIELD_SCALE_X) + 0.1f);
                checkTimer = 1000;
            }
            else checkTimer -= diff;
        }

    private:
        uint32 checkTimer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reign_of_chaosAI(creature);
    }
};

void AddSC_boss_setesh()
{
    new boss_setesh();
    RegisterCreatureAI(npc_chaos_portal);
    new npc_setesh_void_sentinel();
    RegisterCreatureAI(npc_setesh_void_seeker);
    RegisterCreatureAI(npc_setesh_void_lord);
    new npc_chaos_blast();
    new npc_seed_of_chaos();
    new spell_hoo_chaos_blast();
    new npc_reign_of_chaos();
}
