#include "ScriptMgr.h"
#include "Vehicle.h"
#include "MoveSplineInit.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "zulgurub.h"

enum
{
    EVENT_YOGA_FLAME        = 1,
};

class npc_tiki_torch : public CreatureScript
{
    public:
        npc_tiki_torch() : CreatureScript("npc_tiki_torch") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return GetZulGurubAI<npc_tiki_torchAI>(creature);
        }

        struct npc_tiki_torchAI : public ScriptedAI
        {
            npc_tiki_torchAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_POSITION_ID)
                    events.ScheduleEvent(EVENT_YOGA_FLAME, 1000 * data);
            }
            /*
            void MoveInLineOfSight(Unit* who)
            {
                if (who->ToPlayer())
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        TC_LOG_ERROR("sql.sql", "player moving while im casting, distance: %u", int32(double(me->GetExactDist(who) * 10)));
                        if (me->GetExactDist(who) <= 2.5f)
                            me->DealDamage(who, 50000, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_FIRE, sSpellMgr->GetSpellInfo(97352), false);
                    }
                }
            }
            */
            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_YOGA_FLAME)
                {
                    me->CastSpell(me, 97000, false);
                    events.ScheduleEvent(EVENT_YOGA_FLAME, 6000);
                }
            }
        };
};

enum ZulgrubEnum
{
    EVENT_ROLLING_BOULDERS_LEFT             = 1,
    EVENT_ROLLING_BOULDERS_CENTER           = 2,
    EVENT_ROLLING_BOULDERS_RIGHT            = 3,

    EVENT_KNOCK_AWAY                        = 4,
    EVENT_THUNDERCLAP                       = 5,

    EVENT_BOULDER_BOOM                      = 1,

    NPC_BOULDER_LEFT                        = 52351,
    NPC_BOULDER_CENTER                      = 52354,
    NPC_BOULDER_RIGHT                       = 52353,

    POINT_BOULDER                           = 1,
    POINT_BACK                              = 2,
    ACTION_START_MOVING                     = 1,
};

class npc_berserking_boulder_roller : public CreatureScript
{
    public:
        npc_berserking_boulder_roller() : CreatureScript("npc_berserking_boulder_roller") { }

    private:

        struct npc_berserking_boulder_rollerAI : public ScriptedAI
        {
            npc_berserking_boulder_rollerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI()
            {
                JustReachedHome();
            }

            void Reset()
            {
                if (!me->HasAura(96837))
                    me->AddAura(96837, me);
            }

            void JustReachedHome()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ROLLING_BOULDERS_CENTER, 1000);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.Reset();
                events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(7000, 14000));
                events.ScheduleEvent(EVENT_THUNDERCLAP, urand(3000, 7000));
                if (me->HasAura(96837))
                    me->RemoveAura(96837);
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
            }

            void JustSummoned(Creature* summoned)
            {
                Position pos;
                summoned->GetPosition(&pos);
                if (auto boulder = summoned->SummonCreature(52350, pos, TEMPSUMMON_TIMED_DESPAWN, 9500))
                {
                    boulder->EnterVehicle(summoned);
                    boulder->SetReactState(REACT_PASSIVE);
                    boulder->CastSpell(boulder, 96833, true);
                }

                summoned->CastSpell(summoned, 96833, true);
                summoned->SetReactState(REACT_PASSIVE);
                summoned->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
                summoned->AI()->DoAction(ACTION_START_MOVING);
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (!UpdateVictim())
                {
                    if (uint32 eventId = events.ExecuteEvent())
                    {
                        switch (eventId)
                        {
                            case EVENT_ROLLING_BOULDERS_LEFT:
                                me->CastSpell(me, 96827, false);
                                events.ScheduleEvent(RAND(EVENT_ROLLING_BOULDERS_CENTER, EVENT_ROLLING_BOULDERS_RIGHT), urand(1000, 1500));
                                break;
                            case EVENT_ROLLING_BOULDERS_CENTER:
                                me->CastSpell(me, 96828, false);
                                events.ScheduleEvent(RAND(EVENT_ROLLING_BOULDERS_LEFT, EVENT_ROLLING_BOULDERS_RIGHT), urand(1000, 1500));
                                break;
                            case EVENT_ROLLING_BOULDERS_RIGHT:
                                me->CastSpell(me, 96829, false);
                                events.ScheduleEvent(RAND(EVENT_ROLLING_BOULDERS_LEFT, EVENT_ROLLING_BOULDERS_CENTER), urand(1000, 1500));
                                break;
                        }
                    }

                    return;
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_KNOCK_AWAY:
                            DoCastVictim(97616);
                            events.ScheduleEvent(EVENT_KNOCK_AWAY, urand(7000, 14000));
                            break;
                        case EVENT_THUNDERCLAP:
                            DoCast(15588);
                            events.ScheduleEvent(EVENT_THUNDERCLAP, urand(3000, 7000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_berserking_boulder_rollerAI(creature);
    }
};

class npc_boulder : public CreatureScript
{
    public:
        npc_boulder() : CreatureScript("npc_boulder") { }

    private:
        struct npc_boulderAI : public ScriptedAI
        {
            npc_boulderAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                canleap = false;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_MOVING)
                {
                    canleap = true;
                    me->GetNearPoint2D(x, y, 120.0f, me->GetOrientation());
                    me->GetMotionMaster()->MovePoint(POINT_BACK, x, y, 80.5f);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (canleap)
                    if (spell->Id == 96834)
                        target->GetMotionMaster()->MoveJumpTo(x, y, 81.0f);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE && id == POINT_BACK)
                    events.ScheduleEvent(EVENT_BOULDER_BOOM, 0);
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_BOULDER_BOOM)
                {
                    //me->CastSpell(me, 96836, false);
                    if (Vehicle* v = me->GetVehicleKit())
                        if (auto p = v->GetPassenger(0))
                            if (auto pC = p->ToCreature())
                        {
                            p->ExitVehicle();
                            pC->DespawnOrUnsummon();
                        }
                    me->DespawnOrUnsummon();
                }
            }

        private:
            float x, y;
            bool canleap;
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_boulderAI(creature);
        }
};

class spell_exploding_boulder : public SpellScriptLoader
{
    public:
        spell_exploding_boulder() : SpellScriptLoader("spell_exploding_boulder") { }

    private:
        class spell_exploding_boulder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_exploding_boulder_SpellScript)

            void CastSpell(SpellEffIndex effIndex)
            {
                GetCaster()->RemoveAura(GetSpellInfo()->Effects[effIndex].BasePoints);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_exploding_boulder_SpellScript::CastSpell, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript *GetSpellScript() const
        {
            return new spell_exploding_boulder_SpellScript();
        }
};

class BoulderSmashCheck : public std::unary_function<Unit*, bool>
{
    public:
        explicit BoulderSmashCheck(Unit* _caster) : caster(_caster) { }

        bool operator()(WorldObject* object)
        {
            if (!caster)
                return true;

            return !(caster->isInFront(object, 2.5f) && object->GetTypeId() == TYPEID_PLAYER);
        }

    private:
        Unit* caster;
};

class spell_boulder_smash : public SpellScriptLoader
{
    public:
        spell_boulder_smash() : SpellScriptLoader("spell_boulder_smash") { }

    private:
        class spell_boulder_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_boulder_smash_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                if (!GetCaster()->GetVehicleBase())
                    return;

                unitList.remove_if(BoulderSmashCheck(GetCaster()->GetVehicleBase()));
            }

            void LeapBack(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetCaster()->GetVehicleBase() || !GetHitUnit()->ToPlayer())
                    return;

                float speedXY = float(GetSpellInfo()->Effects[effIndex].MiscValue) / 10;
                float speedZ = float(GetSpellInfo()->Effects[effIndex].BasePoints / 10);
                float vcos = std::cos(GetCaster()->GetVehicleBase()->GetOrientation());
                float vsin = std::sin(GetCaster()->GetVehicleBase()->GetOrientation());
                GetHitUnit()->SendMoveKnockBack(GetHitUnit()->ToPlayer(), speedXY, -speedZ, vcos, vsin);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boulder_smash_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boulder_smash_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_boulder_smash_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_boulder_smash_SpellScript::LeapBack, EFFECT_0, SPELL_EFFECT_LEAP_BACK);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_boulder_smash_SpellScript();
        }
};

enum
{
    EVENT_BONE_VOLLEY           = 1,
};

class npc_tiki_lord_zimwae : public CreatureScript
{
    class ZimwaeEvent : public BasicEvent
    {
        public:
            ZimwaeEvent(Creature* creature) : me(creature) {}

        private:
            bool Execute(uint64 /*time*/, uint32 /*diff*/)
            {
                me->CastSpell(me, 97237, false);
                return true;
            }

        private:
            Creature* me;
    };

    public:
        npc_tiki_lord_zimwae() : CreatureScript("npc_tiki_lord_zimwae") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tiki_lord_zimwaeAI(creature);
        }

        struct npc_tiki_lord_zimwaeAI : public ScriptedAI
        {
            npc_tiki_lord_zimwaeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_BONE_VOLLEY, urand(3000, 5000));
                me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
                me->RemoveAllAuras();
                me->CastSpell(me, 97236, false);
                me->m_Events.AddEvent(new ZimwaeEvent(me), me->m_Events.CalculateTime(1500));
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case EVENT_BONE_VOLLEY:
                        DoCastVictim(96871);
                        events.ScheduleEvent(EVENT_BONE_VOLLEY, urand(3000, 5000));
                    }
            }
        };
};

enum
{
    EVENT_TIKI_TORCH           = 1,
};

class npc_tiki_lord_muloa : public CreatureScript
{
    class MuloaEvent : public BasicEvent
    {
        public:
            MuloaEvent(Creature* creature) : me(creature) {}

        private:
            bool Execute(uint64 /*time*/, uint32 /*diff*/)
            {
                me->CastSpell(me, 97147, false);
                return true;
            }

        private:
            Creature* me;
    };

    public:
        npc_tiki_lord_muloa() : CreatureScript("npc_tiki_lord_muloa") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tiki_lord_muloaAI(creature);
        }

        struct npc_tiki_lord_muloaAI : public ScriptedAI
        {
            npc_tiki_lord_muloaAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_TIKI_TORCH, 1000);
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
                me->RemoveAllAuras();
                me->CastSpell(me, 97148, false);
                me->m_Events.AddEvent(new MuloaEvent(me), me->m_Events.CalculateTime(1500));
                std::list<Creature*> stalkers;
                me->GetCreatureListWithEntryInGrid(stalkers, 45979, 15.0f);

                if (stalkers.empty())
                    return;

                for (std::list<Creature*>::const_iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                    (*itr)->RemoveAura(96885);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_BONE_VOLLEY)
                {
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                        me->CastSpell(target, 96822, false);

                    events.ScheduleEvent(EVENT_TIKI_TORCH, 8000);
                }
            }
        };
};

const Position GubFishSP[3]=
{
    {-11869.5f, -1504.78f, 8.93495f, 0.0f},
    {-11858.2f, -1510.84f, 8.10302f, 0.0f},
    {-11852.2f, -1499.59f, 8.80203f, 0.0f},
};

class npc_gub : public CreatureScript
{
    public:
        npc_gub() : CreatureScript("npc_gub") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gubAI (creature);
        }

        struct npc_gubAI : public ScriptedAI
        {
            npc_gubAI(Creature* creature) : ScriptedAI(creature) { }

            uint64 FishTargetGUID[3];
            bool Frenzy;

            void InitializeAI()
            {
                me->CastSpell(me, 97014, false);

                for (int i = 0; i < 3; ++i)
                    if (auto fish_target = me->SummonCreature(45979, GubFishSP[i]))
                    {
                        FishTargetGUID[i] = fish_target->GetGUID();
                        fish_target->CastSpell(me, 97015, false);
                    }

                Reset();
            }

            void Reset()
            {
                Frenzy = false;
            }

            void JustReachedHome()
            {
                me->CastSpell(me, 97014, false);

                for (int i = 0; i < 3; ++i)
                    if (auto fish_target = ObjectAccessor::GetCreature(*me, FishTargetGUID[i]))
                        fish_target->CastSpell(me, 97015, false);
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &/*damage*/)
            {
                if (!Frenzy && me->GetHealthPct() <= 35.0f)
                {
                    Frenzy = true;
                    me->CastSpell(me, 81173, false);
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                me->RemoveAllAuras();
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };
};

enum
{
    EVENT_POISON_BOLT_VOLLEY        = 1,
};

class npc_chosen_of_hethiss : public CreatureScript
{
    public:
        npc_chosen_of_hethiss() : CreatureScript("npc_chosen_of_hethiss") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_chosen_of_hethissAI (creature);
        }

        struct npc_chosen_of_hethissAI : public ScriptedAI
        {
            npc_chosen_of_hethissAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI()
            {
                JustReachedHome();
                Reset();
            }

            void Reset()
            {
                events.Reset();
                me->SetReactState(REACT_PASSIVE);
            }

            void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
            {
                if (spell->Id == 97019)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();


                    if (me->HasAura(97019))
                        events.ScheduleEvent(EVENT_POISON_BOLT_VOLLEY, urand(1000, 3000));
                }
            }
            void JustReachedHome()
            {
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
            }

            void EnterCombat(Unit* /*who*/)
            {
                if (!me->HasAura(97019))
                {
                    me->CastSpell(me, 97019, false);
                    me->SetReactState(REACT_PASSIVE);
                }

                me->SetStandState(UNIT_STAND_STATE_STAND);

                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 52322, 50.0f);
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 52323, 50.0f);
                if (list_of_npcs.size())
                    for (auto c : list_of_npcs)
                    {
                        if (c->GetGUID() != me->GetGUID())
                            if (!c->isInCombat())
                                if (auto ai = c->AI())
                                    c->AI()->DoAction(66);
                    }
            }

            void DoAction(int32 const action) override
            {
                if (action == 66)
                    if (!me->isInCombat())
                        if (auto p = me->FindNearestPlayer(100.f))
                            AttackStart(p);

            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
            }

            void UpdateAI(uint32 const diff)
            {

                events.Update(diff);
                if (events.ExecuteEvent() == EVENT_POISON_BOLT_VOLLEY)
                {
                    if (me->HasAura(97019))
                        me->CastSpell((Unit*)NULL, 97018, false);

                    events.ScheduleEvent(EVENT_POISON_BOLT_VOLLEY, urand(1000, 3000));
                }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }
        };
};

class npc_witch_doctor_quin : public CreatureScript
{
    public:
        npc_witch_doctor_quin() : CreatureScript("npc_witch_doctor_quin") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_witch_doctor_quinAI (creature);
        }

        struct npc_witch_doctor_quinAI : public ScriptedAI
        {
            npc_witch_doctor_quinAI(Creature* creature) : ScriptedAI(creature) { }

            void EnterCombat(Unit* /*who*/)
            {
                Talk(0);
                me->CastSpell((Unit*)NULL, 97016, false);
                me->SetTarget(0);

                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 52322, 50.0f);
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 52323, 50.0f);
                for (auto c : list_of_npcs)
                {
                    if (c->GetGUID() != me->GetGUID())
                        if (!c->isInCombat())
                            if (auto ai = c->AI())
                                c->AI()->DoAction(66);
                }
            }

            void DoAction(int32 const action) override
            {
                if (action == 66)
                    if (!me->isInCombat())
                        if (auto p = me->FindNearestPlayer(100.f))
                            AttackStart(p);
                
            }
        };
};

enum
{
    EVENT_SHADOW_BOLT_VOLLEY        = 1,
    EVENT_PSYCHIC_SCREAM            = 2,
    EVENT_SACRIFICE,
    EVENT_CLIMB_STAIRS,
    EVENT_SAY_CLIMB,
};
const Position Slave_spawns[3]
{
    {-11751.505859f, -1888.892090f, 55.759811f, 5.434881f},
    {-11700.913086f, -1896.630005f, 55.774757f, 4.237165f},
    {-11762.345703f, -1902.707275f, 56.179382f, 5.835451f},
};
const Position Slave_Path[2]
{
    {-11707.234375f, -1925.878784f, 55.822338f, 5.191442f},
    {-11658.060547f, -1896.098022f, 77.278015f, 0.522288f},
};

class npc_mortaxx : public CreatureScript
{
    public:
        npc_mortaxx() : CreatureScript("npc_mortaxx") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mortaxxAI (creature);
        }

        struct npc_mortaxxAI : public ScriptedAI
        {
            npc_mortaxxAI(Creature* creature) : ScriptedAI(creature){}

            void InitializeAI()
            {
                events.ScheduleEvent(EVENT_SACRIFICE, 5000);
            }
            void Reset()
            {
                //me->CastSpell(me, 75965, false);
                events.Reset();
                events.ScheduleEvent(EVENT_SACRIFICE, 5000);
            }

            void EnterCombat(Unit* /*who*/)
            {
                //me->RemoveAura(75965);
                events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, urand(3000, 7000));
                events.ScheduleEvent(EVENT_PSYCHIC_SCREAM, urand(7000, 21000));
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
            }

            void JustSummoned(Creature* summon) 
            {
                //TC_LOG_ERROR("sql.sql", "summon %u spawned.", summon->GetGUID());
            }
            void SummonedMovementInform(Creature* summon, uint32 type, uint32 id) 
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (id)
                    {
                    case 50:
                        events.ScheduleEvent(EVENT_SAY_CLIMB, 4000);
                        events.ScheduleEvent(EVENT_SACRIFICE, 25000);
                        break;
                    }
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_SAY_CLIMB:
                            if (!me->isInCombat())
                                Talk(0);
                        break;
                        case EVENT_SACRIFICE:
                            if (auto c = me->SummonCreature(RAND(52434, 52435, 52437), Slave_spawns[urand(0, 2)]))
                            {
                                c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                c->setFaction(35);
                                c->GetMotionMaster()->MovePoint(50, Slave_Path[0], true, 3.f);
                            }
                            break;
                        case EVENT_SHADOW_BOLT_VOLLEY:
                            me->CastSpell((Unit*)NULL, 17228, false);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, urand(3000, 7000));
                            break;
                        case EVENT_PSYCHIC_SCREAM:
                            me->CastSpell((Unit*)NULL, 34322, false);
                            events.ScheduleEvent(EVENT_PSYCHIC_SCREAM, urand(7000, 21000));
                            break;
                    }

                if (UpdateVictim())
                DoMeleeAttackIfReady();
            }
        private:
            EventMap events;
            bool climbed{ false };
        };
};

class npc_mortaxx_sacrifice : public CreatureScript
{
public:
    npc_mortaxx_sacrifice() : CreatureScript("npc_mortaxx_sacrifice") {}

    struct npc_mortaxx_sacrificeAI : public ScriptedAI
    {
        npc_mortaxx_sacrificeAI(Creature* creature) : ScriptedAI(creature) {}

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_CLIMB_STAIRS:
                    me->RemoveAura(68442);
                    me->GetMotionMaster()->MovePoint(100, Slave_Path[1], true, 3.f);
                    break;
                }
        }
        void MovementInform(uint32 type, uint32 id) 
        {
            if (type == POINT_MOTION_TYPE)
                switch (id)
                {
                case 50:
                    if (auto mort = me->FindNearestCreature(52438, 50.f))
                        me->SetFacingTo(me->GetAngle(mort));
                    me->AddAura(68442, me);
                    events.ScheduleEvent(EVENT_CLIMB_STAIRS, 5000);
                    break;
                case 100:
                    DoCast(me, 35426, true);
                    //me->Kill(me, false, false);
                    me->DespawnOrUnsummon(500);
                    break;
                }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mortaxx_sacrificeAI(creature);
    }
};
const Position MauriTotemSP[2]=
{
    {-11885.7f, -1319.80f, 78.6447f, 0.523599f},
    {-11886.0f, -1319.92f, 78.6269f, 5.550150f},
};

const Position MauriCauldronSP = {-11885.9f, -1319.81f, 77.9252f, 0.0f};

const uint32 TotemSpell[2]=
{
    96936,
    96937,
};

enum
{
    EVENT_WHISPERS_OF_HETHISS       = 1, // Script for spell implemented in Venoxis script.
    EVENT_POISON_BOLT               = 2,
};

class npc_venomancer_mauri : public CreatureScript
{
    public:
        npc_venomancer_mauri() : CreatureScript("npc_venomancer_mauri") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venomancer_mauriAI (creature);
        }

        struct npc_venomancer_mauriAI : public ScriptedAI
        {
            npc_venomancer_mauriAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint64 TotemGUID[2];
            uint64 CauldronGUID;

            void InitializeAI()
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = me->SummonCreature(45979, MauriTotemSP[i]))
                    {
                        TotemGUID[i] = totem->GetGUID();
                        totem->CastSpell(me, TotemSpell[i], false);
                    }

                if (auto cauldron = me->SummonCreature(52529, MauriCauldronSP))
                {
                    CauldronGUID = cauldron->GetGUID();

                    if (me->isAlive())
                    {
                        cauldron->CastSpell(cauldron, 97122, false);
                        cauldron->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                Reset();
            }

            void Reset()
            {
                events.Reset();
            }

            void JustReachedHome()
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = ObjectAccessor::GetCreature(*me, TotemGUID[i]))
                        totem->CastSpell(me, TotemSpell[i], false);
            }

            void EnterCombat(Unit* /*who*/)
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = ObjectAccessor::GetCreature(*me, TotemGUID[i]))
                        totem->CastStop();

                Talk(0);
                events.ScheduleEvent(EVENT_WHISPERS_OF_HETHISS, 8000);
                events.ScheduleEvent(EVENT_POISON_BOLT, 5000);
            }

            void JustDied(Unit* /*killer*/)
            {
                Talk(1);
                events.Reset();

                if (auto cauldron = ObjectAccessor::GetCreature(*me, CauldronGUID))
                {
                    cauldron->RemoveAura(97122);
                    cauldron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WHISPERS_OF_HETHISS:
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->CastSpell(target, 96466, false);

                            events.ScheduleEvent(EVENT_WHISPERS_OF_HETHISS, 15000);
                            break;
                        }
                        case EVENT_POISON_BOLT:
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                me->CastSpell(target, 96918, false);

                            events.ScheduleEvent(EVENT_POISON_BOLT, 5000);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

const Position TkuluTotemSP[2]=
{
    {-11962.6f, -1643.23f, 37.5636f, 3.54302f},
    {-11962.6f, -1643.24f, 37.5545f, 3.52556f},
};

const Position TkuluCauldronSP = {-11963.8f, -1643.33f, 36.7988f, 0.0f};

enum
{
    EVENT_TOXIC_LINK                  = 1, // Script for spell implemented in Venoxis script.
};

class npc_venomancer_tkulu : public CreatureScript
{
    public:
        npc_venomancer_tkulu() : CreatureScript("npc_venomancer_tkulu") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venomancer_tkuluAI (creature);
        }

        struct npc_venomancer_tkuluAI : public ScriptedAI
        {
            npc_venomancer_tkuluAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint64 TotemGUID[2];
            uint64 CauldronGUID;

            void InitializeAI()
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = me->SummonCreature(45979, TkuluTotemSP[i]))
                    {
                        TotemGUID[i] = totem->GetGUID();
                        totem->CastSpell(me, TotemSpell[i], false);
                    }

                if (auto cauldron = me->SummonCreature(52529, TkuluCauldronSP))
                {
                    CauldronGUID = cauldron->GetGUID();

                    if (me->isAlive())
                    {
                        cauldron->CastSpell(cauldron, 97122, false);
                        cauldron->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }

                Reset();
            }

            void Reset()
            {
                events.Reset();
            }

            void JustReachedHome()
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = ObjectAccessor::GetCreature(*me, TotemGUID[i]))
                        totem->CastSpell(me, TotemSpell[i], false);
            }

            void EnterCombat(Unit* /*who*/)
            {
                for (int i = 0; i < 2; ++i)
                    if (auto totem = ObjectAccessor::GetCreature(*me, TotemGUID[i]))
                        totem->CastStop();

                Talk(0);
                events.ScheduleEvent(EVENT_TOXIC_LINK, urand(5000, 10000));
                events.ScheduleEvent(EVENT_POISON_BOLT, urand(5000, 10000));
            }

            void JustDied(Unit* /*killer*/)
            {
                Talk(1);
                events.Reset();

                if (auto cauldron = ObjectAccessor::GetCreature(*me, CauldronGUID))
                {
                    cauldron->RemoveAura(97122);
                    cauldron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TOXIC_LINK:
                            me->CastSpell((Unit*)NULL, 96475, false);
                            events.ScheduleEvent(EVENT_TOXIC_LINK, 10000);
                            break;
                        case EVENT_POISON_BOLT:
                            {
                                if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                                    me->CastSpell(target, 96918, false);

                                events.ScheduleEvent(EVENT_POISON_BOLT, urand(5000, 10000));
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

enum
{
    EVENT_BUBBLING_MIXTURE      = 1,
    EVENT_CRUSH_HERBS           = 2,
    EVENT_GOUT_OF_FLAME         = 3,

    EVENT_DRAIN_BLUE_CAULDRON   = 4,
    EVENT_FROSTBURN_FORMULA     = 5,

    EVENT_DRAIN_RED_CAULDRON    = 6,
    EVENT_DRAGONS_BREATH        = 7,

    EVENT_DRAIN_GREEN_CAULDRON  = 8,
};

struct npc_cauldron_mixerAI : public ScriptedAI
{
    npc_cauldron_mixerAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_BUBBLING_MIXTURE, urand(7000, 21000));
        events.ScheduleEvent(EVENT_CRUSH_HERBS, urand(7000, 21000));
        events.ScheduleEvent(EVENT_GOUT_OF_FLAME, urand(7000, 21000));
    }

    void JustDied(Unit* /*killer*/)
    {
        events.Reset();
    }

    virtual void DoExtraEvent(uint32 event_Id) = 0;

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);

        if (!UpdateVictim())
        {
            if (uint32 eventId = events.ExecuteEvent())
                DoExtraEvent(eventId);

            return;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BUBBLING_MIXTURE:
                    {
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            me->CastSpell(target, 96804, false);

                        events.ScheduleEvent(EVENT_BUBBLING_MIXTURE, urand(7000, 21000));
                    }
                    break;
                case EVENT_CRUSH_HERBS:
                    me->CastSpell(me, 97386, false);
                    events.ScheduleEvent(EVENT_CRUSH_HERBS, urand(7000, 21000));
                    break;
                case EVENT_GOUT_OF_FLAME:
                    {
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                            me->CastSpell(target, 96413, false);

                        events.ScheduleEvent(EVENT_GOUT_OF_FLAME, urand(7000, 21000));
                    }
                    break;
                default:
                    DoExtraEvent(eventId);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

class npc_gurubashi_cauldron_mixer_frost : public CreatureScript
{
    public:
        npc_gurubashi_cauldron_mixer_frost() : CreatureScript("npc_gurubashi_cauldron_mixer_frost") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_cauldron_mixer_frostAI (creature);
        }

        struct npc_gurubashi_cauldron_mixer_frostAI : public npc_cauldron_mixerAI
        {
            npc_gurubashi_cauldron_mixer_frostAI(Creature* creature) : npc_cauldron_mixerAI(creature) { }

            void Reset()
            {
                npc_cauldron_mixerAI::Reset();
                events.ScheduleEvent(EVENT_DRAIN_BLUE_CAULDRON, urand(3000, 7000));
            }

            void EnterCombat(Unit* who)
            {
                events.Reset();
                events.ScheduleEvent(EVENT_FROSTBURN_FORMULA, urand(3000, 7000));
                npc_cauldron_mixerAI::EnterCombat(who);
            }

            void DoExtraEvent(uint32 event_Id)
            {
                switch (event_Id)
                {
                    case EVENT_DRAIN_BLUE_CAULDRON:
                        me->CastSpell((Unit*)NULL, 96488, false);
                        events.ScheduleEvent(EVENT_DRAIN_BLUE_CAULDRON, urand(7000, 14000));
                        break;
                    case EVENT_FROSTBURN_FORMULA:
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_NEAREST, 0, 50.0f, true))
                                me->CastSpell(target, 96449, false);

                            events.ScheduleEvent(EVENT_FROSTBURN_FORMULA, urand(7000, 14000));
                        }
                        break;
                }
            }
        };
};

class npc_gurubashi_cauldron_mixer_fire : public CreatureScript
{
    public:
        npc_gurubashi_cauldron_mixer_fire() : CreatureScript("npc_gurubashi_cauldron_mixer_fire") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_cauldron_mixer_fireAI (creature);
        }

        struct npc_gurubashi_cauldron_mixer_fireAI : public npc_cauldron_mixerAI
        {
            npc_gurubashi_cauldron_mixer_fireAI(Creature* creature) : npc_cauldron_mixerAI(creature) { }

            void Reset()
            {
                npc_cauldron_mixerAI::Reset();
                events.ScheduleEvent(EVENT_DRAIN_RED_CAULDRON, urand(3000, 7000));
            }

            void EnterCombat(Unit* who)
            {
                events.Reset();
                events.ScheduleEvent(EVENT_DRAGONS_BREATH, urand(3000, 7000));
                npc_cauldron_mixerAI::EnterCombat(who);
            }

            void DoExtraEvent(uint32 event_Id)
            {
                switch (event_Id)
                {
                    case EVENT_DRAIN_RED_CAULDRON:
                        me->CastSpell((Unit*)NULL, 96486, false);
                        events.ScheduleEvent(EVENT_DRAIN_RED_CAULDRON, urand(7000, 14000));
                        break;
                    case EVENT_DRAGONS_BREATH:
                        me->CastSpell((Unit*)NULL, 96447, false);
                        events.ScheduleEvent(EVENT_DRAGONS_BREATH, urand(7000, 14000));
                        break;
                }
            }
        };
};

class npc_gurubashi_cauldron_mixer_poison : public CreatureScript
{
    public:
        npc_gurubashi_cauldron_mixer_poison() : CreatureScript("npc_gurubashi_cauldron_mixer_poison") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_cauldron_mixer_poisonAI (creature);
        }

        struct npc_gurubashi_cauldron_mixer_poisonAI : public npc_cauldron_mixerAI
        {
            npc_gurubashi_cauldron_mixer_poisonAI(Creature* creature) : npc_cauldron_mixerAI(creature) { }

            void Reset()
            {
                npc_cauldron_mixerAI::Reset();
                events.ScheduleEvent(EVENT_DRAIN_GREEN_CAULDRON, urand(3000, 7000));
            }

            void EnterCombat(Unit* who)
            {
                events.Reset();
                me->CastSpell(me, 96456, false);
                npc_cauldron_mixerAI::EnterCombat(who);
            }

            void DoExtraEvent(uint32 event_Id)
            {
                if (me->GetDBTableGUIDLow() != 307512)
                if (event_Id == EVENT_DRAIN_GREEN_CAULDRON)
                {
                    me->CastSpell((Unit*)NULL, 96487, false);
                    events.ScheduleEvent(EVENT_DRAIN_GREEN_CAULDRON, urand(7000, 14000));
                }
            }
        };
};

class npc_gurubashi_cauldron_mixer : public CreatureScript
{
    public:
        npc_gurubashi_cauldron_mixer() : CreatureScript("npc_gurubashi_cauldron_mixer") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_cauldron_mixerAI (creature);
        }

        struct npc_gurubashi_cauldron_mixerAI : public npc_cauldron_mixerAI
        {
            npc_gurubashi_cauldron_mixerAI(Creature* creature) : npc_cauldron_mixerAI(creature) { }

            void DoExtraEvent(uint32 /*eventId*/) { }
        };
};

enum
{
    EVENT_POOL_OF_ACRID_TEARS           = 1,
    EVENT_BREATH_OF_HETHISS             = 2,
    EVENT_RETURN_TO_FIGHT               = 3,
    EVENT_CAULDRON_NEUTRALIZER          = 4,
};

class npc_venomguard_destroyer : public CreatureScript
{
    public:
        npc_venomguard_destroyer() : CreatureScript("npc_venomguard_destroyer") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venomguard_destroyerAI (creature);
        }

        struct npc_venomguard_destroyerAI : public ScriptedAI
        {
            npc_venomguard_destroyerAI(Creature* creature) : ScriptedAI(creature), summons(creature) { }

            EventMap events;
            SummonList summons;

            void Reset()
            {
                events.Reset();
                summons.DespawnAll();
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void JustSummoned(Creature* summoned)
            {
                if (summoned->GetEntry() == 52320)
                {
                    summoned->SetReactState(REACT_PASSIVE);
                    summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    summoned->CastSpell(summoned, 96756, true);
                }

                summons.Summon(summoned);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_POOL_OF_ACRID_TEARS, urand(7000, 13000));
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
                summons.DespawnAll();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POOL_OF_ACRID_TEARS:
                            me->CastSpell(me, 96754, false);
                            events.ScheduleEvent(EVENT_BREATH_OF_HETHISS, urand(5000, 8000));
                            break;
                        case EVENT_BREATH_OF_HETHISS:
                        {
                            events.ScheduleEvent(EVENT_CAULDRON_NEUTRALIZER, urand(5000, 8000));
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 10.0f, true))
                            {
                                me->SetReactState(REACT_PASSIVE);
                                me->AttackStop();
                                me->StopMoving();
                                me->SetFacingTo(me->GetAngle(target));
                                me->CastSpell(target, 96753, false);
                                events.ScheduleEvent(EVENT_RETURN_TO_FIGHT, 3000);
                            }
                            break;
                        }
                        case EVENT_RETURN_TO_FIGHT:
                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        case EVENT_CAULDRON_NEUTRALIZER:
                            me->CastSpell((Unit*)NULL, 97337, false);
                            events.ScheduleEvent(EVENT_POOL_OF_ACRID_TEARS, urand(5000, 8000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

enum
{
    EVENT_SHOOT_VISUAL          = 1,
    EVENT_SHOOT                 = 2,
    EVENT_SHADOWED_SHOT         = 3,
};

class npc_gurubashi_shadow_hunter : public CreatureScript
{
    public:
        npc_gurubashi_shadow_hunter() : CreatureScript("npc_gurubashi_shadow_hunter") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_shadow_hunterAI (creature);
        }

        struct npc_gurubashi_shadow_hunterAI : public ScriptedAI
        {
            npc_gurubashi_shadow_hunterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI()
            {
                me->LoadEquipment(me->GetEntry(), true);
                me->SetSheath(SHEATH_STATE_RANGED);
                events.ScheduleEvent(EVENT_SHOOT_VISUAL, urand(8000, 15000));

            }
            void Reset()
            {
                me->LoadEquipment(me->GetEntry(), true);
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetSheath(SHEATH_STATE_RANGED);
                events.Reset();
                events.ScheduleEvent(EVENT_SHOOT_VISUAL, urand(8000, 15000));
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                me->Attack(who, false);
            }

            void EnterCombat(Unit* /*who*/)
            {
                me->SetSheath(SHEATH_STATE_RANGED);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.ScheduleEvent(EVENT_SHOOT, 500);
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
            }

            void UpdateAI(uint32 const diff)
            {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SHOOT:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        me->CastSpell(target, 97239, false);

                    events.ScheduleEvent(EVENT_SHADOWED_SHOT, urand(3000, 6000));
                    break;
                case EVENT_SHADOWED_SHOT:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        me->CastSpell(target, 96767, false);
                    events.ScheduleEvent(EVENT_SHOOT, urand(3000, 6000));
                    break;
                case EVENT_SHOOT_VISUAL:
                    if (!me->isInCombat())
                    switch (me->GetDBTableGUIDLow())
                    {
                    case 251266:
                        if (auto c = me->SummonCreature(144951, shoot_target[0], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                    case 251267:
                        if (auto c = me->SummonCreature(144951, shoot_target[1], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                    case 251270:
                        if (auto c = me->SummonCreature(144951, shoot_target[2], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                    case 251269:
                        if (auto c = me->SummonCreature(144951, shoot_target[3], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                    case 251268:
                        if (auto c = me->SummonCreature(144951, shoot_target[4], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                    case 251265:
                        if (auto c = me->SummonCreature(144951, shoot_target[5], TEMPSUMMON_TIMED_DESPAWN, 7000))
                            DoCast(c, 44961, true);
                        break;
                        break;
                    default:
                        break;
                    }
                    //me->HandleEmoteCommand(435);
                    events.ScheduleEvent(EVENT_SHOOT_VISUAL, urand(8000, 15000));
                    break;
                }
            }
            Position shoot_target[6]
            {
                {-11506.343750f, -1475.544800f, 100.249344f, 3.090553f},
                {-11554.787109f, -1532.510620f, 60.125675f,  5.489923f},
                {-11598.196289f, -1562.022339f, 61.186886f,  5.969009f},
                {-11598.254883f, -1671.414795f, 60.728058f,  0.259159f},
                {-11550.094727f, -1711.609131f, 69.498627f,  0.950310f},
                {-11476.554688f, -1722.562012f, 88.898941f,  1.818182f},
            };
        };
};

enum
{
    EVENT_FULL_OF_MEAT          = 1,
    EVENT_FISH_SLAP             = 2,
    EVENT_SLAP_CHOP             = 3,
};

class npc_gurubashi_master_chef : public CreatureScript
{
    public:
        npc_gurubashi_master_chef() : CreatureScript("npc_gurubashi_master_chef") { }

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gurubashi_master_chefAI (creature);
        }

        struct npc_gurubashi_master_chefAI : public ScriptedAI
        {
            npc_gurubashi_master_chefAI(Creature* creature) : ScriptedAI(creature){}

            EventMap events;

            void Reset()
            {
                events.Reset();
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_FULL_OF_MEAT, urand(4000, 8000));
                events.ScheduleEvent(EVENT_FISH_SLAP, urand(3000, 7000));
                events.ScheduleEvent(EVENT_SLAP_CHOP, urand(7000, 14000));
            }

            void JustDied(Unit* /*killer*/)
            {
                events.Reset();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FULL_OF_MEAT:
                            {
                                if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true, -81252))
                                    me->CastSpell(target, 81252, false);

                                events.ScheduleEvent(EVENT_FULL_OF_MEAT, urand(4000, 8000));
                            }
                            break;
                        case EVENT_FISH_SLAP:
                            if (auto v = me->getVictim()) me->CastSpell(v, 79554, false);
                            events.ScheduleEvent(EVENT_FISH_SLAP, urand(3000, 7000));
                            break;
                        case EVENT_SLAP_CHOP:
                            if (auto v = me->getVictim()) me->CastSpell(v, 79175, false);
                            events.ScheduleEvent(EVENT_SLAP_CHOP, urand(10000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
};

class spell_slap_chop : public SpellScriptLoader
{
    public:
        spell_slap_chop() : SpellScriptLoader("spell_slap_chop") { }

    private:
        class spell_slap_chop_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_slap_chop_AuraScript)

            void HandleTick(AuraEffect const* /*aurEff*/)
            {
                // TODO: this is hack, need to remove, after trigger spell with target victim will cast on it
                PreventDefaultAction();
                GetCaster()->CastSpell(GetCaster()->getVictim(), GetSpellInfo()->Effects[EFFECT_1].TriggerSpell, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_slap_chop_AuraScript::HandleTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript *GetAuraScript() const
        {
            return new spell_slap_chop_AuraScript();
        }
};

class spell_zg_acrid_pool_resize : public SpellScriptLoader
{
    /*
        REPLACE INTO `spell_script_names` VALUES (97085, 'spell_zg_acrid_pool_resize');
        REPLACE INTO `spell_script_names` VALUES (96755, 'spell_zg_acrid_pool_resize');
    */
public:
    spell_zg_acrid_pool_resize() : SpellScriptLoader("spell_zg_acrid_pool_resize") { }

    class spell_zg_acrid_pool_resize_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_zg_acrid_pool_resize_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (auto caster = GetCaster())
            for (std::list<WorldObject*>::const_iterator itr = unitList.begin(); itr != unitList.end();)
            {
                WorldObject* temp = (*itr);
                itr++;
                if (temp->GetDistance(caster) > caster->GetFloatValue(OBJECT_FIELD_SCALE_X)*0.5f)   //criteria for being removed from target list
                    unitList.remove(temp);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zg_acrid_pool_resize_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_zg_acrid_pool_resize_SpellScript();
    }
};

class spell_zg_yoga_flames : public SpellScriptLoader
{
    /*
        REPLACE INTO `spell_script_names` VALUES (97001, 'spell_zg_yoga_flames');
        REPLACE INTO `spell_script_names` VALUES (97352, 'spell_zg_yoga_flames');
    */
public:
    spell_zg_yoga_flames() : SpellScriptLoader("spell_zg_yoga_flames") { }

    class spell_zg_yoga_flames_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_zg_yoga_flames_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            Unit* caster = GetCaster();
                std::list<Unit*> targets;
            Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(caster, caster, 0.1f);
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targets, u_check);
            caster->VisitNearbyObject(0.1f, searcher);
            if (targets.size())
            for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end();)
            {
                Unit* temp = (*itr);
                itr++;
                if (temp != NULL)
                    unitList.push_back(temp);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zg_yoga_flames_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_zg_yoga_flames_SpellScript();
    }
};


class spell_zg_plant_poison_cloud : public SpellScriptLoader
{
    /*
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('96730', 'spell_zg_plant_poison_cloud');
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('97368', 'spell_zg_plant_poison_cloud');
    */
public:
    spell_zg_plant_poison_cloud() : SpellScriptLoader("spell_zg_plant_poison_cloud") { }

    class spell_zg_plant_poison_cloud_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_zg_plant_poison_cloud_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {   
            Unit* caster = GetCaster();
            std::list<Unit*> targets;
            Trinity::AnyFriendlyUnitInObjectRangeCheck u_check(caster, caster, 4.f);
            Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(caster, targets, u_check);
            caster->VisitNearbyObject(7.f, searcher);
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                {
                    if (auto temp = (*itr))
                        if (auto p = temp->ToPlayer())
                            if (!p->isGameMaster())
                                if (!p->IsImmunedToSpell(GetSpellInfo()))
                                    unitList.push_back(temp);

                    itr++;
                }

            if (unitList.size())
            for (auto itr = unitList.begin(); itr != unitList.end();)
                if (auto t = (*itr))
                {
                    if (auto p = t->ToPlayer())
                        if (p->isGameMaster() || p->IsImmunedToSpell(GetSpellInfo()))
                            unitList.erase(itr);
                   ++itr;
                }

            /*
            if (unitList.size())
                for (auto itr = unitList.begin(); itr != unitList.end();)
                    if (auto t = (*itr))
                        if (t->GetGUID() != 500)//filter condition
                        {
                            unitList.erase(itr++);
                        }
                        else
                        {
                            ++itr;
                        }
            */




        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zg_plant_poison_cloud_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_zg_plant_poison_cloud_SpellScript();
    }
};

void AddSC_zulgurub()
{
    new npc_tiki_torch();
    new npc_berserking_boulder_roller();
    new npc_boulder();
    new npc_tiki_lord_zimwae();
    new npc_tiki_lord_muloa();
    new npc_gub();
    new npc_chosen_of_hethiss();
    new npc_witch_doctor_quin();
    new npc_mortaxx();
    new npc_mortaxx_sacrifice();
    new npc_venomancer_mauri();
    new npc_venomancer_tkulu();
    new npc_gurubashi_cauldron_mixer_frost();
    new npc_gurubashi_cauldron_mixer_fire();
    new npc_gurubashi_cauldron_mixer_poison();
    new npc_gurubashi_cauldron_mixer();
    new npc_venomguard_destroyer();
    new npc_gurubashi_shadow_hunter();
    new npc_gurubashi_master_chef();
    new spell_zg_acrid_pool_resize();
    new spell_exploding_boulder();
    new spell_boulder_smash();
    new spell_slap_chop();
    new spell_zg_yoga_flames();
    new spell_zg_plant_poison_cloud();
}
