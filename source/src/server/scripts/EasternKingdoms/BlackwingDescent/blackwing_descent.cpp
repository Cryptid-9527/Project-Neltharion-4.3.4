
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "blackwing_descent.h"

class spell_grievous_wound_bwd : public SpellScriptLoader
{
public:
    spell_grievous_wound_bwd() : SpellScriptLoader("spell_grievous_wound_bwd") { }

    class spell_grievous_wound_bwd_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_grievous_wound_bwd_AuraScript);

        void onPeriodicTick(AuraEffect const* /*aurEff*/)
        {
            if (auto target = GetUnitOwner())
                if (target->CountPctFromMaxHealth(90) < target->GetHealth())
                    target->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_grievous_wound_bwd_AuraScript::onPeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_grievous_wound_bwd_AuraScript();
    }
};

class npc_bwd_drakonid_slayer : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_bwd_drakonid_slayer' WHERE  `entry`=49854;
        UPDATE `creature_template` SET `ScriptName`='npc_bwd_drakonid_slayer' WHERE  `entry`=49855;
    */

public:
    npc_bwd_drakonid_slayer() : CreatureScript("npc_bwd_drakonid_slayer") { }

    struct npc_bwd_drakonid_slayerAI : ScriptedAI
    {
        npc_bwd_drakonid_slayerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {}
        void InitializeAI()
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            _events.Reset();
        }
        void EnterCombat(Unit* who)
        {
            //DoCast(SPELL_MORTALSTRIKE);
            _events.Reset();
            Start_Rotation();
            _events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
            me->RemoveAurasDueToSpell(77929);
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            _events.Reset();
            summons.DespawnAll();
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (me->isInCombat())
                return;

            if (type != WAYPOINT_MOTION_TYPE)
                return;

            std::list<Creature*> list_of_npcs;
            GetCreatureListWithEntryInGrid(list_of_npcs, me, 46083, 20.0f);
            if (list_of_npcs.size())
                for (auto c : list_of_npcs)
                {
                    if (!me->HasAura(77929, c->GetGUID()))
                        c->AddAura(77929, me);
                }
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
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        _events.DelayEvent(i, 6000);
            }
            else
                me->AddUnitState(UNIT_STATE_ROOT);
            CheckLineOfSight(spell);
        }

        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!_events.HasEvent(i))
                    _events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
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
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 40.f)
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
            me->ClearUnitState(UNIT_STATE_ROOT);
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
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_MORTALSTRIKE,
            EVENT_BLASTWAVE,
            EVENT_CLEAVE,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_MORTALSTRIKE = 80390,
            SPELL_BLASTWAVE = 80391,
            SPELL_CLEAVE = 80392,
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

            {EVENT_MORTALSTRIKE,         SPELL_MORTALSTRIKE,          3500,          5500,          true,       true},
            {EVENT_BLASTWAVE,           SPELL_BLASTWAVE,         12000,         22000,          true,       true},
            {EVENT_CLEAVE, SPELL_CLEAVE,          12000,         20000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_MORTALSTRIKE:        Spell_Rotation(eventId);  break;
                case EVENT_BLASTWAVE:           Spell_Rotation(eventId);  break;
                case EVENT_CLEAVE:              Spell_Rotation(eventId); break;
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
        bool use_melee{ false };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bwd_drakonid_slayerAI(creature);
    }

};

void AddSC_blackwing_descent()
{
    new spell_grievous_wound_bwd();
    new npc_bwd_drakonid_slayer();
};
