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

#include "GridNotifiers.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "halls_of_origination.h"

enum Spells
{
    //Ammunae
    SPELL_WITHER                           = 76043,
    SPELL_CONSUME_LIFE_DAMAGE_EFFECT       = 79767,
    SPELL_CONSUME_LIFE_ENERGIZE_EFFECT     = 75665,
    SPELL_CONSUME_LIFE_POWER_BURN_EFFECT   = 75666,
    SPELL_RAMPANT_GROWTH                   = 75790,
    //Seeding Pod
    SPELL_SEEDLING_POD_VISUAL_AURA         = 96278,
    SPELL_SEEDLING_POD_EXPLODE              = 75687,//UNUSED
    SPELL_ENERGIZE                         = 75657,
    SPELL_ENERGIZING_GROWTH                = 89123,
    //Bloodpetal
    SPELL_BLOODPETAL_VISUAL                = 75768,
    SPELL_THORN_SLASH                      = 76044,
    //Spore
    SPELL_NOXIOUS_SPORE                    = 75702,
    SPELL_SPORE_CLOUD                      = 75701,
    //Living vine
    SPELL_SPORE_BLAST                      = 75153,
    SPELL_SUMMON_SPORE                     = 75866,
    //Nymph
    SPELL_TRANQUILITY                      = 75940,
};

enum AmunaeTexts
{
    SAY_AGGRO                = 0,
    SAY_GROWTH               = 1,
    SAY_SLAY                 = 2,
    SAY_DEATH                = 3,
};

enum Events
{
    //Ammunae
    EVENT_WITHER                = 1,
    EVENT_CONSUME_LIFE          = 2,
    EVENT_RAMPANT_GROWTH        = 3,
    EVENT_SUMMON_SPORE          = 5,
    EVENT_ENERGY_TICKER         = 6,
    EVENT_ENERGY_TICKER_STOP    = 7,
    EVENT_COMBAT                = 8,
    //Blossom
    EVENT_THORN_SLASH           = 9,
    EVENT_EMERGE                = 10,
    EVENT_ATTACK                = 11,
    //Seedling Pod
    EVENT_ENERGIZE              = 12,

    EVENT_POD_1                 = 20,
    EVENT_POD_2                 = 21,
    EVENT_POD_3                 = 22,
    EVENT_POD_4                 = 23,
};

/*

    NPC_SEEDING_POD                 = 51329,
    NPC_SEED_TRASH                  = 40716,
*/
class boss_ammunae : public CreatureScript
{
    public:
        boss_ammunae() : CreatureScript("boss_ammunae") { }

        struct boss_ammunaeAI : public BossAI
        {
            boss_ammunaeAI(Creature* creature) : BossAI(creature, DATA_AMMUNAE)
            {}

            void Reset()
            {
                _Reset();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                //DoCast(me, 69470, true);
                me->setPowerType(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                summons.DespawnAll();
            }

            void InitializeAI()
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                //DoCast(me, 69470, true);
                me->setPowerType(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
            }


            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) 
            {
                if (summon->GetEntry() == NPC_SEEDING_POD)
                    Pod_Died(summon->GetGUID());
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (setup && who->ToPlayer())
                    if (!me->isInCombat())
                        if (me->GetDistance(who) < 25.f)
                            AttackStart(who);

                if (setup || !who->ToPlayer())
                    return;
                //TC_LOG_ERROR("sql.sql", "setting up ammunae");
                //DoCast(me, 69470, true);
                me->setPowerType(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                setup = true;

            }

            void EnterCombat(Unit* /*who*/)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                _EnterCombat();
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_WITHER, 10000);
                events.ScheduleEvent(EVENT_CONSUME_LIFE, 4000);
                events.ScheduleEvent(EVENT_POD_1, 7000);
                events.ScheduleEvent(EVENT_POD_2, 20000);
                events.ScheduleEvent(EVENT_POD_3, 33000);
                events.ScheduleEvent(EVENT_POD_4, 46000);
                events.ScheduleEvent(EVENT_RAMPANT_GROWTH, 15000);
                events.ScheduleEvent(EVENT_SUMMON_SPORE, 15000);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            }

            void KilledUnit(Unit* victim)
            {
                Talk(SAY_SLAY);
            }

            void SummonRampantGrowth(uint32 entry, float distance)
            {
                std::list<Creature*> CreatureList;
                Trinity::AllCreaturesOfEntryInRange checker(me, entry, distance);
                Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, CreatureList, checker);
                me->VisitNearbyObject(distance, searcher);

                for (Creature* creature : CreatureList)
                {
                    if (auto c2 = me->SummonCreature(NPC_BLOODPETAL_BLOSSOM, *creature))
                    {
                        c2->DeleteThreatList();
                    }
                    Pod_Died(creature->GetGUID());
                    creature->DespawnOrUnsummon();
                }
            }

            void RampantGrowthSummonEffect()
            {
                Talk(SAY_GROWTH);
                DoCast(SPELL_RAMPANT_GROWTH);
            }

            void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
            {
                if (spell->Id == SPELL_RAMPANT_GROWTH || spell->Id == 89888)
                    SummonRampantGrowth(NPC_SEEDING_POD, 200.0f);
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_WITHER:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                                DoCastRandom(SPELL_WITHER, 0.0f);

                                if (!events.HasEvent(eventId))
                                events.ScheduleEvent(EVENT_WITHER, 18000);
                            }
                            break;
                        case EVENT_CONSUME_LIFE:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                                if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                {
                                    DoCast(target, SPELL_CONSUME_LIFE_ENERGIZE_EFFECT);
                                    DoCast(target, SPELL_CONSUME_LIFE_DAMAGE_EFFECT);
                                }

                                if (!events.HasEvent(eventId))
                                events.ScheduleEvent(EVENT_CONSUME_LIFE, 15000);
                            }
                            break;
                        case EVENT_RAMPANT_GROWTH:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                if (me->GetPower(POWER_ENERGY) == me->GetMaxPower(POWER_ENERGY))
                                {
                                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                                    RampantGrowthSummonEffect();
                                }

                                if (!events.HasEvent(eventId))
                                events.ScheduleEvent(EVENT_RAMPANT_GROWTH, 10000);
                            }
                            break;
                        case EVENT_POD_1:
                        case EVENT_POD_2:
                        case EVENT_POD_3:
                        case EVENT_POD_4:
                                Summon_Pod(eventId - 20);
                            break;
                        case EVENT_SUMMON_SPORE:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                Position spawn(*me);
                                me->MoveBlink(spawn, 5.f, frand(0, M_PI * 2.f));
                                me->SummonCreature(NPC_SPORE, spawn);
                                events.ScheduleEvent(EVENT_SUMMON_SPORE, 20000);
                            }
                            break;
                        default:
                            break;
                }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
            }

            void Summon_Pod(int slot)
            {
                if (active_pod_guids[slot])
                    if (auto pod = Unit::GetCreature(*me, active_pod_guids[slot]))
                        pod->DespawnOrUnsummon();

                Position pod_spawn(*me);
                me->MoveBlink(pod_spawn, 14.f, (M_PI / 2.f) * float(slot));

                if (auto c = me->SummonCreature(NPC_SEEDING_POD, pod_spawn))
                    active_pod_guids[slot] = c->GetGUID();
            }

            void Pod_Died(uint64 guid)
            {
                bool found{ false };
                uint32 eventId{ 0 };

                for (auto i = 0; i <= 3; ++i)
                    if (active_pod_guids[i] == guid)
                    { 
                        found = true;
                        eventId = 20 + i;
                        break;
                    }
                if (found)
                    events.ScheduleEvent(eventId, 52000);
                else
                    TC_LOG_ERROR("sql.sql", "BOSS_AMMUNAE Pod_Died() could not recognize pod GUID %u", guid);
            }

            void JustDied(Unit* /*who*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
                me->DeleteThreatList();
            }
            private:
                bool setup                  { false };
                uint64 active_pod_guids[4]  { 0, 0, 0, 0  };
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ammunaeAI(creature);
        }
};

struct npc_bloodpetal_blossom : public ScriptedAI
{
    npc_bloodpetal_blossom(Creature* creature) : ScriptedAI(creature)
    {
        active = false;
    }

    bool active;

    void Reset() override
    {
        me->AddAura(SPELL_BLOODPETAL_VISUAL, me);
        me->HandleEmoteCommand(EMOTE_STATE_SUBMERGED_NEW);
        events.Reset();
    }

    void IsSummonedBy(Unit* /*creator*/) override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->SetReactState(REACT_PASSIVE);
        me->HandleEmoteCommand(EMOTE_STATE_SUBMERGED);
        events.ScheduleEvent(EVENT_EMERGE, urand(3500, 4500));
    }

    void EnterCombat(Unit* /*who*/) override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_THORN_SLASH, 5000);
    }

    void UpdateAI(uint32 const diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_EMERGE:
                    me->HandleEmoteCommand(0);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                    events.ScheduleEvent(EVENT_ATTACK, 1500);
                    break;
                case EVENT_ATTACK:
                    active = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoZoneInCombat();
                    events.ScheduleEvent(EVENT_THORN_SLASH, 5000);
                    break;
                case EVENT_THORN_SLASH:
                    DoCastVictim(SPELL_THORN_SLASH);
                    events.ScheduleEvent(EVENT_THORN_SLASH, 7500);
                    break;
            }
        }
        if (active)
            if (UpdateVictim())
                DoMeleeAttackIfReady();
    }
};

class npc_seed_pod : public CreatureScript
{
public:
    npc_seed_pod() : CreatureScript("npc_seed_pod") { }

    struct npc_seed_podAI : public Scripted_NoMovementAI
    {
        npc_seed_podAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        EventMap events;

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            DoCastAOE(SPELL_SEEDLING_POD_VISUAL_AURA);
        }
        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* who) override
        {
        }

        void IsSummonedBy(Unit* creator)
        {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                events.ScheduleEvent(EVENT_ENERGIZE, 3000);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_ENERGIZE:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        if (auto target = me->FindNearestCreature(BOSS_AMMUNAE, 100.f))
                            DoCast(target, SPELL_ENERGIZE);//dummy aura
                        me->AddAura(SPELL_SEEDLING_POD_VISUAL_AURA, me);//actual aura that will energize and heal ammunae
                        events.ScheduleEvent(EVENT_ENERGIZE, 3000);
                        if (IsHeroic())
                            me->CastSpell(me, SPELL_ENERGIZING_GROWTH, true);
                        break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
            me->DespawnOrUnsummon();
        }
    private:
       bool isBossNPC{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seed_podAI(creature);
    }
};

class npc_seed_pod_trash : public CreatureScript
{
public:
    npc_seed_pod_trash() : CreatureScript("npc_seed_pod_trash") { }

    struct npc_seed_pod_trashAI : public Scripted_NoMovementAI
    {
        npc_seed_pod_trashAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        EventMap events;

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            DoCastAOE(SPELL_SEEDLING_POD_VISUAL_AURA);
            events.Reset();
        }
        void InitializeAI()
        {
            me->AddUnitState(UNIT_STATE_CANNOT_TURN);
            me->SetReactState(REACT_PASSIVE);
            DoCastAOE(SPELL_SEEDLING_POD_VISUAL_AURA);
            DoCastAOE(89098, true);
            events.ScheduleEvent(11, urand(5000, 10000));
        }

        void EnterCombat(Unit* who) override
        {
            events.ScheduleEvent(EVENT_EMERGE, 8000);
            events.CancelEvent(11);
            events.CancelEvent(12);
            events.ScheduleEvent(13, 1);
        }

        void IsSummonedBy(Unit* creator)
        {
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_EMERGE:
                    me->SummonCreature(NPC_BLOODPETAL_BLOSSOM, *me);
                    me->DespawnOrUnsummon();
                    break;
                case 11://set invisible
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->RemoveAurasDueToSpell(SPELL_SEEDLING_POD_VISUAL_AURA);
                    DoCast(me, SPELL_SEEDLING_POD_EXPLODE);
                    events.ScheduleEvent(12, 2500);
                    break;
                case 12:
                    me->SetObjectScale(0.01f);
                    if (auto c = me->FindNearestCreature(NPC_LIVING_VINE, 20.f, true))
                    {
                        Position tele_to(*c);
                        c->MoveBlink(tele_to, frand(2.f, 8.f), c->GetRelativeAngleOffset(me)+M_PI);
                        me->NearTeleportTo(tele_to);
                    }
                    events.ScheduleEvent(13, 2500);
                    break;
                case 13://set visible
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetObjectScale(1.f);
                    me->RemoveAurasDueToSpell(SPELL_SEEDLING_POD_EXPLODE);
                    DoCastAOE(SPELL_SEEDLING_POD_VISUAL_AURA);
                    if (!me->isInCombat())
                        events.ScheduleEvent(11, urand(5000, 10000));
                    break;
                }
        }

        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
            me->DespawnOrUnsummon();
        }

    private:
        bool isVisible{ true };
        bool isBossNPC{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seed_pod_trashAI(creature);
    }
};

class npc_spore : public CreatureScript
{
public:
    npc_spore() : CreatureScript("npc_spore") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sporeAI(creature);
    }

    struct npc_sporeAI : public ScriptedAI
    {
        npc_sporeAI(Creature* creature) : ScriptedAI(creature) { }

        void IsSummonedBy(Unit* creator)
        {
            if (auto victim = me->FindNearestPlayer(50.0f))
                me->Attack(victim, false);
            me->setFaction(16);
        }

        void DamageTaken(Unit* /*who*/, uint32& damage)
        {
        }
    };
};

class npc_nymph : public CreatureScript
{
public:
    npc_nymph() : CreatureScript("npc_nymph") { }

    struct npc_nymphAI : ScriptedAI
    {
        npc_nymphAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {}
        void InitializeAI()
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            _events.Reset();
            me->GetMotionMaster()->MoveRandom(8.f);
        }
        void EnterCombat(Unit* who)
        {
            DoCast(75958);
            _events.Reset();
            Start_Rotation();
            _events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            _events.Reset();
            summons.DespawnAll();
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }
        void CheckLineOfSight(SpellInfo const* spell)
        {
            
            if (auto v = me->getVictim())
                if (!me->IsWithinLOSInMap(v, spell))
                {
                    Position pos(*v);
                    v->MoveBlink(pos, 20.f, v->GetRelativeAngleOffset(me));
                    me->GetMotionMaster()->MovePoint(8, pos);
                }
            
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        _events.DelayEvent(i, 6000);
            me->ClearUnitState(UNIT_STATE_ROOT);
            CheckLineOfSight(spell);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!_events.HasEvent(i))
                    _events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation_Self(uint32 eventId)//execute a spell
        {
            me->CastSpell(me, my_spells[eventId].spell_id);
        }

        void Spell_Rotation_Friendly(uint32 eventId, float range)//execute a spell
        {
            auto friendlies = me->GetFriendlyUnitsInRange(range, me);
            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(friendlies))
                me->CastSpell(random_target, my_spells[eventId].spell_id);
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {
            if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                if (auto v = me->getVictim())
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 45.f)
                    {
                        if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                            if (me->isMoving())
                                me->SetRooted(true);
                        me->AddUnitState(UNIT_STATE_ROOT);

                        DoCast(my_spells[eventId].spell_id);
                        ReAttemptSpell(eventId, true);
                    }
                    else {
                        me->SetRooted(false);
                        UpdateVictim();
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule) {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, 250);
            }
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        };

        /*void move_or_attack()
        {
            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        };*/

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_SHOOT,
            EVENT_ENTANGLING_SHOT,
            EVENT_TRANQUILITY,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_SHOOT = 75958,
            SPELL_ENTANGLING_SHOT = 75961,
            SPELL_TRANQUILITY = 75940,
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
        { {EVENT_NEVER_USE,                               0,        9999999,      9999999,         false,      false}, //DO NOT TOUCH

            {EVENT_SHOOT,                       SPELL_SHOOT,          2000,          2000,          true,       true},
            {EVENT_ENTANGLING_SHOT,   SPELL_ENTANGLING_SHOT,         15000,         19000,          true,       true},
            {EVENT_TRANQUILITY,           SPELL_TRANQUILITY,         20000,         23000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SHOOT:                     Spell_Rotation(eventId);  break;
                case EVENT_ENTANGLING_SHOT:           Spell_Rotation(eventId);  break;
                case EVENT_TRANQUILITY:           Spell_Rotation_Self(eventId); break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        _events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }
        }
    private:
        bool use_melee{ false };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nymphAI(creature);
    }

};


class spell_spore_cloud_aoe : public SpellScriptLoader
{
public:
    spell_spore_cloud_aoe() : SpellScriptLoader("spell_spore_cloud_aoe") { }

    class spell_spore_cloud_aoe_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_spore_cloud_aoe_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (auto c = GetCaster())
            {
                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, c, BOSS_AMMUNAE, 6.0f);
                GetCreatureListWithEntryInGrid(units, c, NPC_SPORE, 6.0f);
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                    if (auto c = (*itr))
                        unitList.push_back(c);

            }
        }

        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
            auto c{ GetCaster() };
            auto target{ GetHitUnit() };
            float health{ float(c->GetMaxHealth()) };
            float damage{ health * .15f };
            int32 idamage{ int32(damage) };
            SetHitDamage(idamage);
            //TC_LOG_ERROR("sql.sql", "cast health: %f, damage: %f, idamage: %u", health, damage, idamage);

        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spore_cloud_aoe_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            OnEffectHitTarget += SpellEffectFn(spell_spore_cloud_aoe_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_spore_cloud_aoe_SpellScript();
    }
};

void AddSC_boss_ammunae()
{
    new boss_ammunae;
    RegisterCreatureAI(npc_bloodpetal_blossom);
    new npc_seed_pod;
    new npc_seed_pod_trash;
    new npc_spore;
    new npc_nymph();
    new spell_spore_cloud_aoe();
}
