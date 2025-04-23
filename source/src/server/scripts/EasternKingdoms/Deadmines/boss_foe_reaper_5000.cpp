/*
 * Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
 *
 * Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 *
 * Copyright (C) 2010-2011 Project Trinity <http://www.projecttrinity.org/>
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

/* ToDo
 * Make Overdrive Randomize (Like Marrowgar)
 * Spell Harvest needs Voids + Charge (need Retail Infos)
 * Correct Timers
 * Add the OFF_LINE Spell, but first the Creatures to Activate Foe Reaper 5000
 */

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "deadmines.h"
#include "GameObjectAI.h"

enum Spells
{
    SPELL_ENERGIZE = 89132,
    SPELL_ACTIVATE = 89198,
    SPELL_ACTIVATE_2 = 89121,
    SPELL_RED_EYES = 24263,
    SPELL_ENERGIZE_2 = 91846, // hit 45979 or boss
    // All Modes
    SPELL_OFF_LINE                  = 88348,
    SPELL_OVERDRIVE                 = 88481,
    SPELL_HARVEST                   = 88495,
    SPELL_HARVEST_READY             = 88508,
    SPELL_HARVEST_2                 = 88497,
    SPELL_HARVEST_SWEEP             = 88521,
    SPELL_REAPER_STRIKE             = 88490,
    SPELL_SAFETY_RESTRICTIONS       = 88522,

    // heroic
    SPELL_SUMMON_MOLTEN_SLAG = 91839,
    SPELL_MOLTEN_SHIELD = 91815,

    // scoried
    SPELL_FIXATE = 91830,
};

enum Events
{
    EVENT_OVERDRIVE                 = 1,
    EVENT_OVERDRIVE_MOVEMENTS,
    EVENT_OVERDRIVE_END,
    EVENT_HARVEST,
    EVENT_HARVEST_SWEEP,
    EVENT_HARVEST_MOVEMENTS,
    EVENT_WARN_MOLTEN_SLAG,
    EVENT_SUMMON_MOLTEN_SLAG,

    EVENT_INTRO,
    EVENT_INTRO_FINAL, 
    EVENT_ACTIVATE_FRIENDLY,
};

enum Texts
{
    SAY_AGGRO                       = 0,
    EMOTE_OVERDRIVE                 = 1,
    SAY_OVERDRIVE                   = 2,
    SAY_HARVEST                     = 3,
    HARVESTING_SERVOS_ENGAGED       = 4,
    SAY_HARVEST_PREP                = 5,
    SAY_DEATH                       = 6,
    SAY_MOLTEN_SLAG_SUMMON          = 7,

    SAY_KILLED_TARGET               = 9,
};

enum npcs
{
    NPC_REAPER_DEFIAS              = 47403,
    NPC_REAPER_WATCHER             = 47404,
    NPC_FRIENDLY_PROTOTYPE         = 49208,
    NPC_HARVEST_DUMMY              = 47468,
    NPC_MOLTEN_SLAG                = 49229,
};

enum points
{
    POINT_HARVEST = 1,
};

enum Phases
{
    PHASE_INTRO = 2,
    PHASE_FIGHT,
};

class boss_foe_reaper_5000 : public CreatureScript
{
    struct boss_foe_reaper_5000AI : public BossAI
    {
        boss_foe_reaper_5000AI(Creature* creature) : BossAI(creature, BOSS_FOE_REAPER_5000_DATA) { PrototypesInited = false; }

        void Reset()
        {
            if (me->isAlive())
            {
                _reaperCount = 4;
                introCount = 0;
                enraged = false;
                me->GetPosition(&_targetPos);
                _Reset();
                if (!PrototypesInited)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->CastSpell(me, SPELL_OFF_LINE, true);
                    InitPrototypes();
                    PrototypesInited = true;
                }
                else me->SetReactState(REACT_AGGRESSIVE);

                events.SetPhase(PHASE_INTRO);
                friendly_initiated = false;

                if (IsHeroic())
                    me->DespawnCreaturesInArea(NPC_MOLTEN_SLAG);
                    me->SummonCreature(NPC_FRIENDLY_PROTOTYPE, -205.700134f, -553.420410f, 51.229660f, 4.692599f);
            }
        }

        void InitializeAI()
        {
            Reset();
        }

        void InitPrototypes()
        {
            me->DespawnCreaturesInArea(NPC_REAPER_DEFIAS);
            me->DespawnCreaturesInArea(NPC_REAPER_WATCHER);
            me->DespawnCreaturesInArea(NPC_MOLTEN_SLAG);
            me->SummonCreature(NPC_REAPER_DEFIAS, -182.74f, -565.97f, 19.31f, 3.35f);
            me->SummonCreature(NPC_REAPER_WATCHER, -205.53f, -552.74f, 19.31f, 4.54f);
            me->SummonCreature(NPC_REAPER_DEFIAS, -228.67f, -565.75f, 19.31f, 5.98f);
            me->SummonCreature(NPC_REAPER_WATCHER, -229.72f, -590.37f, 19.31f, 0.71f);
            if (IsHeroic())
            {
                me->DespawnCreaturesInArea(NPC_FRIENDLY_PROTOTYPE);
                me->DespawnCreaturesInArea(NPC_MOLTEN_SLAG);
                //me->SummonCreature(NPC_FRIENDLY_PROTOTYPE, -205.700134f, -553.420410f, 51.229660f, 4.692599f);
            }

            /*
                friendly->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                friendly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                friendly->CastSpell(friendly, SPELL_OFF_LINE, true);
                friendly->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                friendly->SetPower(friendly->getPowerType(), friendly->GetMaxPower(friendly->getPowerType()));
            */

        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() == NPC_REAPER_DEFIAS || summon->GetEntry() == NPC_REAPER_WATCHER)
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                {
                    summon->CastSpell(me, SPELL_ENERGIZE, true);

                    _reaperCount--;
                    if (_reaperCount == 0)
                    {
                        if (!events.HasEvent(EVENT_INTRO))
                            events.ScheduleEvent(EVENT_INTRO, 1000);
                    }
                }

            if (summon->GetEntry() == NPC_FRIENDLY_PROTOTYPE)
            {
                summon->SetHomePosition(*summon);
            }

        }

        void JustSummoned(Creature *summon)
        {
            if (summon->GetEntry() == NPC_HARVEST_DUMMY)
            {
                Talk(HARVESTING_SERVOS_ENGAGED);
                summon->CastSpell(summon, SPELL_HARVEST_READY, true);
                summon->GetPosition(&_targetPos);
                events.ScheduleEvent(EVENT_HARVEST_MOVEMENTS, 0);
            }
            else if (summon->GetEntry() == NPC_MOLTEN_SLAG)
            {
                //summon->NearTeleportTo(summon->GetPositionX(), summon->GetPositionY(), 23.347355f, summon->GetOrientation());
                summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
                summon->SetInCombatWithZone();
            }
            else
            {
                BossAI::JustSummoned(summon);
            }

        }

        void EnterCombat(Unit * who)
        {
            events.SetPhase(PHASE_FIGHT);
            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
            events.ScheduleEvent(EVENT_OVERDRIVE, 12000);
            events.ScheduleEvent(EVENT_HARVEST, 38000);
            events.ScheduleEvent(EVENT_ACTIVATE_FRIENDLY, 3000);

            if (IsHeroic())
                events.ScheduleEvent(EVENT_WARN_MOLTEN_SLAG, 8000);
            /*if (PrototypesInited)
                    if (IsHeroic())
                    if (!me->FindNearestCreature(NPC_FRIENDLY_PROTOTYPE, 100.f, true))
                    {
                        me->DespawnCreaturesInArea(NPC_FRIENDLY_PROTOTYPE, 300.f);
                        if (auto friendly = me->SummonCreature(NPC_FRIENDLY_PROTOTYPE, -206.09f, -594.097f, 20.97f, 1.53f))
                        {
                            friendly->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            friendly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            friendly->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                            friendly->SetPower(friendly->getPowerType(), friendly->GetMaxPower(friendly->getPowerType()));
                        }                       
                    }
            }*/        
            _EnterCombat();
            me->DespawnCreaturesInArea(NPC_REAPER_DEFIAS);
        }

        void JustDied(Unit * /*killer*/)
        {
            std::list<Creature* > triggerList;
            me->GetCreatureListWithEntryInGrid(triggerList, NPC_FRIENDLY_PROTOTYPE, 150.0f);
            me->GetCreatureListWithEntryInGrid(triggerList, NPC_MOLTEN_SLAG, 150.0f);
            for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                if (Creature *bot = *itr)
                    if (!bot->isAlive() || bot->HealthBelowPct(11))
                    {
                        instance->SetData(DATA_PRODIGY, 0);
                        break;
                    }
            Talk(SAY_DEATH);
            _JustDied();
        }

        void KilledUnit(Unit * victim)
        {
            Talk(SAY_KILLED_TARGET);
        }

        void DoMeleeAttackIfReady()
        {
            if (me->getVictim())
                if (me->isAttackReady() && me->IsWithinMeleeRange(me->getVictim()))
                {
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_REAPER_STRIKE, true);
                    me->resetAttackTimer();
                }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
            if (id == POINT_HARVEST)
                events.ScheduleEvent(EVENT_HARVEST_SWEEP, 2000);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (!enraged && me->HealthBelowPct(30))
            {
                enraged = true;
                DoCast(SPELL_SAFETY_RESTRICTIONS);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() && !events.IsInPhase(PHASE_INTRO))
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (me->HasAura(SPELL_OVERDRIVE))
                    me->ClearUnitState(UNIT_STATE_CASTING);
                if (me->HasAura(SPELL_HARVEST_2))
                    me->ClearUnitState(UNIT_STATE_CASTING);
                return;
            }

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch(eventId)
                {
                    case EVENT_INTRO:
                    {
                        std::list<Creature*> stalker;
                        me->GetCreatureListWithEntryInGrid(stalker, 45979, 10.0f);
                        for (auto itr = stalker.begin(); itr != stalker.end(); itr++)
                            (*itr)->CastSpell(me, SPELL_ENERGIZE, true);
                        events.ScheduleEvent(EVENT_INTRO_FINAL, 5000);
                        break;
                    }
                    case EVENT_INTRO_FINAL:
                    {
                        Talk(SAY_AGGRO);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAura(SPELL_OFF_LINE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        //DoZoneInCombat();
                        break;
                    }
                    case EVENT_ACTIVATE_FRIENDLY:
                        if (IsHeroic())
                            if (!friendly_initiated)
                            {
                                if (auto friendly = me->FindNearestCreature(NPC_FRIENDLY_PROTOTYPE, 200.f, true))
                                {
                                    friendly_initiated = true;
                                    friendly->RemoveAura(SPELL_OFF_LINE);
                                    friendly->CastSpell(me, SPELL_ENERGIZE, true);
                                    friendly->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                                    friendly->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    friendly->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                                    friendly->SetPower(friendly->getPowerType(), friendly->GetMaxPower(friendly->getPowerType()));


                                    // Heal & dispel ...
                                    friendly->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, false);
                                    friendly->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, false);
                                    //_me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
                                    friendly->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, false);

                                }
                            }
                        break;
                    case EVENT_OVERDRIVE:
                        if (!me->HasAura(SPELL_HARVEST_2))
                        {
                            Talk(EMOTE_OVERDRIVE);
                            Talk(SAY_OVERDRIVE);
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_OVERDRIVE);
                            events.ScheduleEvent(EVENT_OVERDRIVE_MOVEMENTS, 2000);
                            events.ScheduleEvent(EVENT_OVERDRIVE_END, 11000);
                            events.ScheduleEvent(EVENT_OVERDRIVE, 48000);
                        }
                        else
                            events.ScheduleEvent(EVENT_OVERDRIVE, 48000);
                        break;
                    case EVENT_OVERDRIVE_MOVEMENTS:
                    {
                        Position pos{ *me };
                        me->GetFirstCollisionPosition(pos, (float)urand(20, 50) * (float)rand_norm(), (float)rand_norm() * static_cast<float>(2 * M_PI));
                        me->GetMotionMaster()->MovePoint(0, pos);
                        if (me->HasAura(SPELL_OVERDRIVE))
                            events.ScheduleEvent(EVENT_OVERDRIVE_MOVEMENTS, 1000);
                        break;
                    }
                    case EVENT_OVERDRIVE_END:
                        events.CancelEvent(EVENT_OVERDRIVE_MOVEMENTS);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit *victim = me->getVictim())
                            DoStartMovement(victim);
                        break;
                    case EVENT_HARVEST:
                        if (!me->HasAura(SPELL_OVERDRIVE))
                        {
                            Talk(SAY_HARVEST_PREP);
                            me->SetReactState(REACT_PASSIVE);
                            DoCastRandom(SPELL_HARVEST, 0.0f);
                            events.ScheduleEvent(EVENT_HARVEST, 50000);
                        }
                        else
                            events.ScheduleEvent(EVENT_HARVEST, 50000);
                        break;
                    case EVENT_HARVEST_MOVEMENTS:
                    {
                        me->GetMotionMaster()->MovePoint(POINT_HARVEST, _targetPos);
                        break;
                    }
                    case EVENT_HARVEST_SWEEP:
                    {
                        DoCast(SPELL_HARVEST_SWEEP);
                        me->RemoveAura(SPELL_HARVEST);
                        me->RemoveAura(SPELL_HARVEST_2);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (me->getVictim())
                            DoStartMovement(me->getVictim());
                        if (Creature *target = me->FindNearestCreature(NPC_HARVEST_DUMMY, 200))
                            target->DespawnOrUnsummon(1);
                        break;
                    }
                    case EVENT_WARN_MOLTEN_SLAG:
                    {
                        Talk(SAY_MOLTEN_SLAG_SUMMON);
                        if (auto a = me->SummonCreature(54638, foundry_controls[0]))
                        {
                            a->CastWithDelay(urand(0, 500), me, SPELL_ENERGIZE, true);
                            a->DespawnOrUnsummon(2500);
                        }
                        if (auto a = me->SummonCreature(54638, foundry_controls[1]))
                        {
                            a->CastWithDelay(urand(500, 1000), me, SPELL_ENERGIZE, true);
                            a->DespawnOrUnsummon(2500);
                        }
                        if (auto a = me->SummonCreature(54638, foundry_controls[2]))
                        {
                            a->CastWithDelay(urand(1000, 1500), me, SPELL_ENERGIZE, true);
                            a->DespawnOrUnsummon(2500);
                        }
                        if (auto a = me->SummonCreature(54638, foundry_controls[3]))
                        {
                            a->CastWithDelay(urand(1500, 2000), me, SPELL_ENERGIZE, true);
                            a->DespawnOrUnsummon(2500);
                        }

                        TalkWithDelay(1500, 8);
                        events.ScheduleEvent(EVENT_SUMMON_MOLTEN_SLAG, 8000);
                        break;
                    }
                    case EVENT_SUMMON_MOLTEN_SLAG:
                    {
                        switch (rand() % 4)
                        {
                        case 0:
                            me->SummonCreature(NPC_MOLTEN_SLAG, foundry_controls[0]);
                            break;
                        case 1:
                            me->SummonCreature(NPC_MOLTEN_SLAG, foundry_controls[1]);
                            break;
                        case 2:
                            me->SummonCreature(NPC_MOLTEN_SLAG, foundry_controls[2]);
                            break;
                        case 3:
                            me->SummonCreature(NPC_MOLTEN_SLAG, foundry_controls[3]);
                            break;
                            default:
                                break;
                        }
                        events.ScheduleEvent(EVENT_SUMMON_MOLTEN_SLAG, 17000);
                    }
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        bool enraged;
        int8 _reaperCount, introCount;
        Position _targetPos;
        bool PrototypesInited;
        bool friendly_initiated{ false };
        Position foundry_controls[4]
        {
            {-209.995224f, -579.379761f, 23.347355f, 0.290563f},
            {-206.483627f, -583.511780f, 23.347355f, 1.504004f},
            {-201.513443f, -579.538147f, 23.347355f, 3.117998f},
            {-205.885620f, -575.061951f, 23.347355f, 4.830152f},
        };
    };
public:
    boss_foe_reaper_5000() : CreatureScript("boss_foe_reaper_5000") {}

    CreatureAI * GetAI(Creature * creature) const
    {
        return new boss_foe_reaper_5000AI(creature);
    }
};

class npc_molten_slag : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_molten_slag' WHERE  `entry`=49229;
    */

public:
    npc_molten_slag() : CreatureScript("npc_molten_slag") { }

    struct npc_molten_slagAI : ScriptedAI
    {
        npc_molten_slagAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }
        void InitializeAI()
        {
            _events.Reset();
            if (auto p = me->FindNearestPlayer(100.f))
                AttackStart(p);

            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            _fixateTarget = 0;
            _events.Reset();
        }
        void EnterCombat(Unit* who)
        {
            _events.Reset();
            me->CastSpell(me, SPELL_MOLTEN_SHIELD, true);
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
            if (id == 8)
                UpdateVictim();
        }

        bool CheckLineOfSight(SpellInfo const* spell)
        {
            if (auto v = me->getVictim())
                if (spell->GetMaxRange() > 15.f)
                    if (me->IsWithinLOSInMap(v, spell))
                        return true;
                    else
                    {
                        Position pos(*v);
                        v->MoveBlink(pos, 20.f, v->GetRelativeAngleOffset(me));
                        me->GetMotionMaster()->MovePoint(8, pos);
                        return false;
                    }
                else
                {
                    return false;
                }
            else
            {
                return false;
            }
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
            {
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        _events.DelayEvent(i, 6000);
            }
            else
                if (!CheckLineOfSight(spell))
                    UpdateVictim();
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!_events.HasEvent(i)) {
                    _events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
                }
        }
        void Spell_Rotation(uint32 eventId)//execute a spell
        {
            if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                if (auto v = me->getVictim())
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 50000.f)
                    {
                        //DoCastRandom(my_spells[eventId].spell_id, 50000.f);
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50000.0f, false))
                        {
                            DoCastRandom(my_spells[eventId].spell_id, 50000.f);
                            _fixateTarget = target->GetGUID();
                        }
                        _events.ScheduleEvent(EVENT_FIXATE_CHECK, 1000);
                        _events.ScheduleEvent(EVENT_FIXATE, 30000);
                        ReAttemptSpell(eventId, true);
                    }
                    else
                    {
                        UpdateVictim();
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule) //Reschedule based on the results and conditions of casting.
        {
            if (reschedule)
            {
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else
            {
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
        void move_or_attack()
        {
            // Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        };
        enum spells_and_events
        {
            EVENT_NEVER_USE = 0, /*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_FIXATE,
            EVENT_FIXATE_CHECK,

            SPELL_EVENT_COUNT,   /*END SPELL EVENTS   - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_FIXATE = 91830,
            SPELL_MOLTEN_SHIELD = 91815,
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
        { {EVENT_NEVER_USE,                         0,        9999999,      9999999,         false,      false}, //DO NOT TOUCH

            {EVENT_FIXATE,               SPELL_FIXATE,          15000,          15000,        true,       true},
        };
        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FIXATE:             Spell_Rotation(eventId);   break;
                case EVENT_FIXATE_CHECK:
                    if (_fixateTarget != 0)
                    {
                        if (auto target = Unit::GetUnit(*me, _fixateTarget))
                        {
                            if (target->HasAura(SPELL_FIXATE) && me->getVictim() != target)
                                me->AddThreat(target, 50000000.0f);
                            else if (!target->HasAura(SPELL_FIXATE))
                            {
                                me->getThreatManager().addThreat(target, -90000000);
                                _fixateTarget = 0;
                            }
                        }
                        else
                            _fixateTarget = 0;

                            if (_fixateTarget)
                                _events.ScheduleEvent(EVENT_FIXATE_CHECK, 1000);
                    }
                    break;
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
            move_or_attack();
        }

    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
        uint64 _fixateTarget;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_molten_slagAI(creature);
    }
};

class npc_defias_reaper : public CreatureScript
{
public:
    npc_defias_reaper() : CreatureScript("npc_defias_reaper") { }

    struct npc_defias_reaperAI : public ScriptedAI
    {
        npc_defias_reaperAI(Creature* creature) : ScriptedAI(creature)
        {
            inFriendlyState = false;
        }

        Unit* chargeVictim {nullptr};
        void Reset()
        {
            Cleave = true;
            Energize = true;
            events.Reset();
        }

        void InitializeAI()
        {
            Reset();
            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            float hpercent = me->GetHealthPct();
            if (Energize && (hpercent <= 60) && (hpercent >= 25))
            {
                me->CastSpell(me, 90978, false);
                Energize = false;
            }

            if ((hpercent <= 25) && !inFriendlyState)
            {
                me->CastSpell(me, 91737, false);
                me->setFaction(35);
                me->setRegeneratingHealth(false);
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop(true);

                Unit::AuraMap& auras = me->GetOwnedAuras();
                for (auto itr = auras.begin(); itr != auras.end();)
                {
                    if (Aura* aura = itr->second)
                    {
                        if (aura->GetId() != 91737) // Spell: On Fire!
                        {
                            me->RemoveAura(aura);
                            auras = me->GetOwnedAuras();
                            itr = auras.begin();
                        }
                        else  ++itr;
                    }
                }

                inFriendlyState = true;
            }               
        }
        /*
        void SpellHitTarget(Unit* target, SpellInfo const* spell) 
        {
            if (spell->Id == 91726 || spell->Id == 91735)
            {
                chargeVictim = target;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
        }
        */

        void UpdateAI(const uint32 diff)
        {
            if (inFriendlyState)
            {
                if (me->getFaction() != 35)
                    me->setFaction(35);
            }
            else
            {

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                DoMeleeAttackIfReady();

                if (!inFriendlyState && Cleave && me->IsInRange(me->getVictim(), 0, 5) && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (auto v = me->getVictim()) me->CastSpell(v, 90980, false);
                    events.ScheduleEvent(1, urand(7000, 9000));
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case 1:
                        Cleave = false;
                        break;
                    }
                }
            }
        }

    private:
        EventMap events;
        bool Cleave;
        bool Energize;
        bool inFriendlyState;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_reaperAI(creature);
    }
};

// 16210
class achievement_prototype_prodigy : public AchievementCriteriaScript
{
public:
    achievement_prototype_prodigy() : AchievementCriteriaScript("achievement_prototype_prodigy") { }

    bool OnCheck(Player* /*source*/, Unit* target)
    {
        if (!target)
            return false;
        if (InstanceScript *instance = target->GetInstanceScript())
            return instance->instance->IsHeroic() && instance->GetData(DATA_PRODIGY);
        return false;
    }
};

/*
    This script is only needed for MoveLineofSight to trigger goblins running out of the foe reaper area
*/
const G3D::Vector3 Goblin_flee_engineer[]
{
    { -230.693f, -571.304f, 51.2384f },
    { -234.955f, -578.741f, 51.2281f },
    { -249.272f, -578.834f, 51.1489f },
    { -271.282f, -579.361f, 49.7145f },
    { -278.264f, -593.539f, 51.2101f },
    { -287.929f, -601.031f, 48.2452f },
    { -299.921f, -603.927f, 47.6466f },
    { -305.184f, -593.573f, 47.8031f },
    { -298.917f, -578.906f, 47.3756f },
    { -290.884f, -566.772f, 49.1559f },
    { -290.912f, -554.259f, 49.4486f },
    { -290.373f, -537.343f, 49.4472f },
    { -290.103f, -515.992f, 49.5425f },
    { -278.886f, -489.154f, 49.1339f },
    { -273.563f, -477.863f, 49.2281f }
    /*
    {.go -230.693, -571.304, 51.2384 },
    {.go -231.149, -577.759, 51.2254 },
    {.go -249.272, -578.834, 51.1489 },
    {.go -271.282, -579.361, 49.7145 },
    {.go -278.264, -593.539, 51.2101 },
    {.go -287.929, -601.031, 48.2452 },
    {.go -299.921, -603.927, 47.6466 },
    {.go -305.184, -593.573, 47.8031 },
    {.go -298.917, -578.906, 47.3756 },
    {.go -290.884, -566.772, 49.1559 },
    {.go -290.912, -554.259, 49.4486 }
    */
};

const G3D::Vector3 Goblin_flee_one[]
{
    { -230.693f, -571.304f, 51.2384f },
    { -234.955f, -578.741f, 51.2281f },
    { -249.272f, -578.834f, 51.1489f },
    { -271.282f, -579.361f, 49.7145f },
    { -278.264f, -593.539f, 51.2101f },
    { -287.929f, -601.031f, 48.2452f },
    { -299.921f, -603.927f, 47.6466f },
    { -305.184f, -593.573f, 47.8031f },
    { -298.917f, -578.906f, 47.3756f },
    { -290.884f, -566.772f, 49.1559f },
    { -290.912f, -554.259f, 49.4486f },
    { -290.373f, -537.343f, 49.4472f },
    { -290.103f, -515.992f, 49.5425f },
    { -278.886f, -489.154f, 49.1339f },
    { -282.608f, -479.362f, 49.1283f }
};

const G3D::Vector3 Goblin_flee_two[]
{
    { -230.693f, -571.304f, 51.2384f },
    { -234.955f, -578.741f, 51.2281f },
    { -249.272f, -578.834f, 51.1489f },
    { -271.282f, -579.361f, 49.7145f },
    { -278.264f, -593.539f, 51.2101f },
    { -287.929f, -601.031f, 48.2452f },
    { -299.921f, -603.927f, 47.6466f },
    { -305.184f, -593.573f, 47.8031f },
    { -298.917f, -578.906f, 47.3756f },
    { -290.884f, -566.772f, 49.1559f },
    { -290.912f, -554.259f, 49.4486f },
    { -290.373f, -537.343f, 49.4472f },
    { -290.103f, -515.992f, 49.5425f },
    { -278.886f, -489.154f, 49.1339f },
    { -273.535f, -487.340f, 49.1222f },
    { -272.515f, -489.321f, 49.4158f }
};

const G3D::Vector3 Goblin_flee_three[]
{
    { -230.693f, -571.304f, 51.2384f },
    { -234.955f, -578.741f, 51.2281f },
    { -249.272f, -578.834f, 51.1489f },
    { -271.282f, -579.361f, 49.7145f },
    { -278.264f, -593.539f, 51.2101f },
    { -287.929f, -601.031f, 48.2452f },
    { -299.921f, -603.927f, 47.6466f },
    { -305.184f, -593.573f, 47.8031f },
    { -298.917f, -578.906f, 47.3756f },
    { -290.884f, -566.772f, 49.1559f },
    { -290.912f, -554.259f, 49.4486f },
    { -290.373f, -537.343f, 49.4472f },
    { -290.103f, -515.992f, 49.5425f },
    { -278.886f, -489.154f, 49.1339f },
    { -277.581f, -495.771f, 49.6778f }
};

class go_foe_reaper_entrance : public GameObjectScript
{
public:
    go_foe_reaper_entrance() : GameObjectScript("go_foe_reaper_entrance") {}

    struct go_foe_reaper_entranceAI : public GameObjectAI
    {
        go_foe_reaper_entranceAI(GameObject* go) : GameObjectAI(go)
        {
            instance = go->GetInstanceScript();
        }

        bool GossipHello(Player* player) override
        {
            go->SetGoState(GO_STATE_ACTIVE);
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            if (auto p = player->ToUnit())
            {
                if (instance->GetBossState(BOSS_FOE_REAPER_5000_DATA) != DONE)
                    if (!flee_rp_activated)
                        if (go->GetDistance(player) < 30.f)
                            if (auto engineer = go->SummonCreature(48439, spawns[0]))
                                if (auto goblin2 = go->SummonCreature(48280, spawns[1]))
                                    if (auto goblin3 = go->SummonCreature(48280, spawns[2]))
                                        if (auto goblin4 = go->SummonCreature(48280, spawns[3]))
                                        {
                                            engineer->RemoveAllAuras();
                                            goblin2->RemoveAllAuras();
                                            goblin3->RemoveAllAuras();
                                            goblin4->RemoveAllAuras();

                                            engineer->AddAura(81959, engineer);
                                            goblin2->AddAura(81959, goblin2);
                                            goblin3->AddAura(81959, goblin3);
                                            goblin4->AddAura(81959, goblin4);

                                            engineer->TalkWithDelay(2000, 0, NULL, CHAT_MSG_MONSTER_YELL);
                                            goblin2->TalkWithDelay(3000, 0, NULL, CHAT_MSG_MONSTER_YELL);
                                            goblin3->TalkWithDelay(4000, 1, NULL, CHAT_MSG_MONSTER_YELL);

                                            engineer->GetMotionMaster()->MoveSmoothPath(Goblin_flee_engineer, 15, 8.f);
                                            goblin2->GetMotionMaster()->MoveSmoothPath(Goblin_flee_one, 15, 8.f);
                                            goblin3->GetMotionMaster()->MoveSmoothPath(Goblin_flee_two, 16, 8.f);
                                            goblin4->GetMotionMaster()->MoveSmoothPath(Goblin_flee_three, 15, 8.f);

                                            /*
                                            engineer->DespawnOrUnsummon(12000);
                                            goblin2->DespawnOrUnsummon(12000);
                                            goblin3->DespawnOrUnsummon(12000);
                                            */
                                        }
            }

            return true;
        }

    private:
        bool flee_rp_activated{ false };
        InstanceScript* instance;
        Position spawns[4]
        {
            {-230.138031f, -589.568481f, 51.236027f, 1.878462f}, // .go -230.138031, -589.568481, 51.23602
            {-221.528183f, -558.828552f, 51.229362f, 3.915991f}, // .go -218.217148, -555.559937, 51.229362
            {-214.327271f, -550.789062f, 51.229973f, 3.931694f}, // .go -210.132507, -547.894714, 51.229973
            {-203.658432f, -546.450989f, 51.229610f, 3.751052f}  // .go -200.391983, -546.961426, 51.229610
        };
    };

    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_foe_reaper_entranceAI(go);
    }
};

void AddSC_boss_foe_reaper_5000()
{
    new boss_foe_reaper_5000();
    new npc_molten_slag();
    new npc_defias_reaper();
    new achievement_prototype_prodigy();
    new go_foe_reaper_entrance();
}
