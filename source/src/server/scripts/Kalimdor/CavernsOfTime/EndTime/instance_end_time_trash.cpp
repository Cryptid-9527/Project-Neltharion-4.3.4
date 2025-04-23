/*
* trinity core og
*/

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "SpellScript.h"
#include "MoveSplineInit.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "endTime.h"

enum Spells
{
    SPELL_ARCANE_BLAST  = 101816,
    SPELL_BLINK         = 101812,
	    
    SPELL_FACE_KICK     = 101888,
    SPELL_PUTRID_SPIT   = 102063,
    SPELL_WALL_OF_DEAD  = 101891,

    SPELL_SHOOT         = 102410,
    SPELL_MULTISHOT     = 102411,

    SPELL_PW_SHIELD     = 102409,
    SPELL_FNT_O_LIGHT   = 102405,
    SPELL_LIGHT_RAIN    = 102406,

    SPELL_CADAVER_TOSS  = 109952,
    SPELL_CANNIBALIZE   = 109944,
    SPELL_FLESH_RIP     = 102066,

    SPELL_SHIELD_BASH   = 101817,
    SPELL_SHIELD_WALL   = 101811,
    SPELL_THUNDERCLAP   = 101820,

    SPELL_ENRAGE        = 102134,
    SPELL_FLAME_BREATH  = 102135,

    SPELL_ARCANE_WAVE   = 102601,
    SPELL_TEMP_VORTEX   = 102600,

    SPELL_VOID_SHIELD   = 102599,
    SPELL_VOID_STRIKE   = 102598,
	
	SPELL_CALL_FLAMES   = 102156,
	SPELL_SEAR_FLESH    = 102158,
	
	SPELL_BREAK_ARMOR	= 102132,
	SPELL_RUPTURE_G   	= 102124,
};

enum Events
{
    EVENT_PUTRID_SPIT       = 1,
    EVENT_FACE_KICK         = 2,
    EVENT_FOUNTAIN_O_LIGHT  = 3,
    EVENT_PW_SHIELD         = 4,
    EVENT_CADAVER_TOSS      = 5,
    EVENT_END_CANNIBALIZE       = 6,
    EVENT_FLESH_RIP         = 7,
    EVENT_SHIELD_BASH       = 8,
    EVENT_SHIELD_WALL       = 9,
    EVENT_THUNDERCLAP       = 10,
    EVENT_FLAME_BREATH      = 11,
    EVENT_ENRAGE            = 12,
    EVENT_ARCANE_WAVE       = 13,
    EVENT_TEMP_VORTEX       = 14,
    EVENT_VOID_SHIELD       = 15,
    EVENT_VOID_STRIKE       = 16,
	EVENT_CALL_FLAMES		= 17,
	EVENT_SEAR_FLESH		= 18,
	EVENT_BREAK_ARMOR		= 19,
	EVENT_RAPTURE_G			= 20,
	EVENT_ARCANE_BLAST		= 21,
    EVENT_BLINK_CHECK       = 22,
    EVENT_MOVE_RANDOM       = 23,
    EVENT_CHECK_PLAYERS     = 24
};

// 54691

class npc_et_time_twisted_sourceress : public CreatureScript
{
public:
    npc_et_time_twisted_sourceress() : CreatureScript("npc_et_time_twisted_sourceress") { }


    struct npc_et_time_twisted_sourceressAI : public ScriptedAI
    {
        npc_et_time_twisted_sourceressAI(Creature* creature) : ScriptedAI(creature) {}

        void EnterCombat(Unit* /*who*/) 
        {
            me->StopMoving();
            me->AddUnitState(UNIT_STATE_ROOT);
            events.ScheduleEvent(EVENT_ARCANE_BLAST, 2000);
        }
        void Reset()
        {
            events.Reset();
            me->ClearUnitState(UNIT_STATE_ROOT);
        }
        void DamageTaken(Unit* target, uint32& damage)
        {
            if (target->ToPlayer())
                if (me->GetDistance(target) < 6.f)
                            if (canBlink)
                            {
                                if (me->HasUnitState(UNIT_STATE_CASTING))
                                {
                                    me->CastStop();
                                    events.CancelEvent(EVENT_ARCANE_BLAST);
                                    events.CancelEventGroup(EVENT_ARCANE_BLAST);
                                }
                                me->ClearUnitState(UNIT_STATE_ROOT);
                                Position pos(*me);
                                me->MoveBlink(pos, 30.f, 0.0f);  //argument 2 = distance, argument 3 = offset from facing direction, usually 0
                                DoCast(me, 36937);
                                me->NearTeleportTo(pos);
                                me->MoveBlink(pos, -1.f, 0.0f);  //argument 2 = distance, argument 3 = offset from facing direction, usually 0
                                me->GetMotionMaster()->MovePoint(EVENT_BLINK_CHECK, pos, true);

                                canBlink = false;
                                me->AddUnitState(UNIT_STATE_ROOT);
                                me->UpdateObjectVisibility();
                                events.ScheduleEvent(EVENT_BLINK_CHECK, 15000);
                            }
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
                if (id == EVENT_BLINK_CHECK)
                    me->AddUnitState(UNIT_STATE_ROOT);
                events.ScheduleEvent(EVENT_ARCANE_BLAST, 500);
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (UpdateVictim())
                    if (me->GetDistance(me->getVictim()) <= 40.f)
                        DoCast(spell);

                if (!events.HasEvent(eventId))
                    events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason) 
        {  
            if (spell->Id == SPELL_ARCANE_BLAST)
            if (reason == SPELL_FINISHED_CANCELED)
            {
                events.CancelEvent(EVENT_ARCANE_BLAST);
                events.CancelEventGroup(EVENT_ARCANE_BLAST);
                me->ClearUnitState(UNIT_STATE_CASTING);
                me->CastStop();
                
                events.ScheduleEvent(EVENT_ARCANE_BLAST, 8000);
            }
        }
        void SpellHitTarget(Unit* /*victim*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ARCANE_BLAST)
            {
                if (events.HasEvent(EVENT_ARCANE_BLAST))
                    events.CancelEventGroup(EVENT_ARCANE_BLAST);
                events.ScheduleEvent(EVENT_ARCANE_BLAST, 500);
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
                case EVENT_ARCANE_BLAST:
                    spell = SPELL_ARCANE_BLAST;         //Spell ID
                    min_repeat = 5000;                 //Minimum Delay
                    max_repeat = 7000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_BLINK_CHECK:
                    canBlink = true;
                    break;
                }

            if (UpdateVictim())
                if (me->getVictim())
                    if (me->GetDistance(me->getVictim()) < 6.f)
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            DoMeleeAttackIfReady();
        }
        bool canBlink{ true };
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_et_time_twisted_sourceressAI(creature);
    }
};

// 54507
class npc_et_time_twisted_scourge_beast : public CreatureScript
{
public:
    npc_et_time_twisted_scourge_beast() : CreatureScript("npc_et_time_twisted_scourge_beast") {}

    struct npc_et_time_twisted_scourge_beastAI : public ScriptedAI
    {
        npc_et_time_twisted_scourge_beastAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }
        void JustDied(Unit* /*killer*/) 
        {

            std::list<Creature*> units;
            GetCreatureListWithEntryInGrid(units, me, 54511, 15.0f);
            for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
            {
                Position eatingspot;
                (*itr)->MoveBlink(eatingspot, (*itr)->GetDistance(me) - 1.f, (*itr)->GetAngle(me));
                (*itr)->GetMotionMaster()->MovePoint(28, eatingspot);
                (*itr)->SetTarget(me->GetGUID());
                (*itr)->SetReactState(REACT_PASSIVE);
            }
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_FACE_KICK, 5000);
            events.ScheduleEvent(EVENT_PUTRID_SPIT, 2000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;          

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_FACE_KICK:
                        DoCastVictim(SPELL_FACE_KICK);
                        events.ScheduleEvent(EVENT_FACE_KICK, 10000);
                        break;
                    case EVENT_PUTRID_SPIT:
                        DoCastRandom(SPELL_PUTRID_SPIT, 30.0f);
                        events.ScheduleEvent(EVENT_PUTRID_SPIT, 6000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_scourge_beastAI(creature);
    }
};

// 54693
class npc_et_time_twisted_rifleman : public CreatureScript
{
public:
    npc_et_time_twisted_rifleman() : CreatureScript("npc_et_time_twisted_rifleman") {}

    struct npc_et_time_twisted_riflemanAI : public ScriptedAI
    {
        npc_et_time_twisted_riflemanAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            casts = 0;
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (casts < 4)
            {
                DoCastRandom(SPELL_SHOOT, 40.0f);
                ++casts;
            }
            else
            {
                DoCastRandom(SPELL_MULTISHOT, 40.0f);
                casts = 0;
            }
        }

    private:
        uint8 casts;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_riflemanAI(creature);
    }
};

// 54690
class npc_et_time_twisted_priest : public CreatureScript
{
public:
    npc_et_time_twisted_priest() : CreatureScript("npc_et_time_twisted_priest") {}

    struct npc_et_time_twisted_priestAI : public ScriptedAI
    {
        npc_et_time_twisted_priestAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }

        void JustSummoned(Creature* summon) override
        {
            summon->SetReactState(REACT_PASSIVE);
			summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            summon->CastSpell(summon, SPELL_LIGHT_RAIN, true);
			summon->CastWithDelay(15000, summon, SPELL_LIGHT_RAIN, true);
			summon->CastWithDelay(290000, summon, SPELL_LIGHT_RAIN, true);
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_FOUNTAIN_O_LIGHT, 1000);
            events.ScheduleEvent(EVENT_PW_SHIELD, 5000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_FOUNTAIN_O_LIGHT:
                        DoCast(SPELL_FNT_O_LIGHT);
						events.ScheduleEvent(EVENT_FOUNTAIN_O_LIGHT, 15000);
                        break;
                    case EVENT_PW_SHIELD:
                        if (auto target = DoSelectLowestHpFriendly(40.0f))
                            DoCast(target, SPELL_PW_SHIELD);
                        events.ScheduleEvent(EVENT_PW_SHIELD, 20000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_priestAI(creature);
    }
};

// 54511
class npc_et_time_twisted_geist : public CreatureScript
{
public:
    npc_et_time_twisted_geist() : CreatureScript("npc_et_time_twisted_geist") {}

    struct npc_et_time_twisted_geistAI : public ScriptedAI
    {
        npc_et_time_twisted_geistAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }
        void MovementInform(uint32 type, uint32 id) 
        {
            if (type == POINT_MOTION_TYPE && id == 28)
                if (me->HasReactState(REACT_PASSIVE))
                    if (auto deadScourgeBeast = me->FindNearestCreature(54507, 10.f, false))
                    {
                        TC_LOG_ERROR("sql.sql", "type: %u, id: %u", type, id);
                        me->UpdateOrientation(me->GetAngle(deadScourgeBeast));
                        me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                        me->CastSpell(deadScourgeBeast, SPELL_CANNIBALIZE, true);
                        events.ScheduleEvent(EVENT_END_CANNIBALIZE, 11000); 
                    }
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_CADAVER_TOSS, 13000);
            events.ScheduleEvent(EVENT_FLESH_RIP, 2000);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_CADAVER_TOSS:
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            DoCastRandom(SPELL_CADAVER_TOSS, 45.0f);
                        else
                            events.ScheduleEvent(EVENT_CADAVER_TOSS, urand(5000, 13000));
                        break;
                    case EVENT_FLESH_RIP:
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            DoCastVictim(SPELL_FLESH_RIP);
                        events.ScheduleEvent(EVENT_FLESH_RIP, 15000);
                        break;
                    case EVENT_END_CANNIBALIZE:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            events.ScheduleEvent(EVENT_END_CANNIBALIZE, 1000);
                            TC_LOG_ERROR("sql.sql", "2");
                        }
                        else
                        {
                            TC_LOG_ERROR("sql.sql", "3");
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->ClearUnitState(UNIT_STATE_ROOT);
                            me->ClearUnitState(UNIT_STATE_NOT_MOVE);
                            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                            me->SetInCombatWithZone();
                            TC_LOG_ERROR("sql.sql", "4");
                            if (me->getVictim())
                                me->GetMotionMaster()->MoveChase(me->getVictim(), 0.5f);
                        }
                        break;
                    default:
                        break;
                }
            }

            if (!UpdateVictim())
                return;


            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasReactState(REACT_PASSIVE))
                return;

            if (me->HasReactState(REACT_AGGRESSIVE))
                if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_geistAI(creature);
    }
};

// 54687
class npc_et_time_twisted_footman : public CreatureScript
{
public:
    npc_et_time_twisted_footman() : CreatureScript("npc_et_time_twisted_footman") {}

    struct npc_et_time_twisted_footmanAI : public ScriptedAI
    {
        npc_et_time_twisted_footmanAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_SHIELD_BASH, 3000);
            events.ScheduleEvent(EVENT_SHIELD_WALL, 12000);
            events.ScheduleEvent(EVENT_THUNDERCLAP, 8000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_SHIELD_BASH:
                        DoCastVictim(SPELL_SHIELD_BASH);
                        events.ScheduleEvent(EVENT_SHIELD_BASH, 12000);
                        break;
                    case EVENT_SHIELD_WALL:
                        DoCast(SPELL_SHIELD_WALL);
                        events.ScheduleEvent(EVENT_SHIELD_WALL, 20000);
                        break;
                    case EVENT_THUNDERCLAP:
                        DoCastVictim(SPELL_THUNDERCLAP);
                        events.ScheduleEvent(EVENT_THUNDERCLAP, 16000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_footmanAI(creature);
    }
};

// 54543
class npc_et_time_twisted_drake : public CreatureScript
{
public:
    npc_et_time_twisted_drake() : CreatureScript("npc_et_time_twisted_drake") {}

    struct npc_et_time_twisted_drakeAI : public ScriptedAI
    {
        npc_et_time_twisted_drakeAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_ENRAGE, 20000);
            events.ScheduleEvent(EVENT_FLAME_BREATH, 15000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ENRAGE:
                        DoCast(SPELL_ENRAGE);
                        events.ScheduleEvent(EVENT_ENRAGE, 20000);
                        break;
                    case EVENT_FLAME_BREATH:
                        DoCast(SPELL_FLAME_BREATH);
                        events.ScheduleEvent(EVENT_FLAME_BREATH, 15000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_drakeAI(creature);
    }
};

// 54920
class npc_et_infinite_supressor : public CreatureScript
{
public:
    npc_et_infinite_supressor() : CreatureScript("npc_et_infinite_supressor") {}

    struct npc_et_infinite_supressorAI : public ScriptedAI
    {
        npc_et_infinite_supressorAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (auto hourglass = me->FindNearestCreature(NPC_HOURGLASS_TRIGGER, 35.f))
                {
                    me->CastSpell(hourglass, SPELL_HOURGLASS_SUPPRESSION, true);
                }
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_ARCANE_WAVE, 3000);
            events.ScheduleEvent(EVENT_TEMP_VORTEX, 7000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ARCANE_WAVE:
                        DoCastRandom(SPELL_ARCANE_WAVE, 45.0f);
                        events.ScheduleEvent(EVENT_ARCANE_WAVE, 20000);
                        break;
                    case EVENT_TEMP_VORTEX:
                        DoCastRandom(SPELL_TEMP_VORTEX, 45.0f);
                        events.ScheduleEvent(EVENT_TEMP_VORTEX, 15000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_infinite_supressorAI(creature);
    }
};

// 54923
class npc_et_infinite_warden : public CreatureScript
{
public:
    npc_et_infinite_warden() : CreatureScript("npc_et_infinite_warden") {}

    struct npc_et_infinite_wardenAI : public ScriptedAI
    {
        npc_et_infinite_wardenAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (auto hourglass = me->FindNearestCreature(NPC_HOURGLASS_TRIGGER, 35.f))
                {
                    me->CastSpell(hourglass, SPELL_HOURGLASS_SUPPRESSION, true);
                }
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_VOID_SHIELD, 3000);
            events.ScheduleEvent(EVENT_VOID_STRIKE, 7000);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                    case EVENT_VOID_SHIELD:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventid, 250);
                        else
                        {
                            DoCast(SPELL_VOID_SHIELD);
                            events.ScheduleEvent(EVENT_VOID_SHIELD, 20000);
                        }
                        break;
                    case EVENT_VOID_STRIKE:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventid, 250);
                        else
                        {
                            DoCastRandom(EVENT_VOID_STRIKE, 45.0f);
                            events.ScheduleEvent(EVENT_VOID_STRIKE, 5000);
                        }
                        break;
                    default:
                        break;
                }

            if (me->isInCombat())
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_infinite_wardenAI(creature);
    }
};

// 54553
class npc_et_time_twisted_sear : public CreatureScript
{
public:
    npc_et_time_twisted_sear() : CreatureScript("npc_et_time_twisted_sear") {}

    struct npc_et_time_twisted_searAI : public ScriptedAI
    {
        npc_et_time_twisted_searAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_CALL_FLAMES, 20000);
            events.ScheduleEvent(EVENT_SEAR_FLESH, 15000);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_CALL_FLAMES:
                        DoCast(SPELL_CALL_FLAMES);
                        events.ScheduleEvent(EVENT_CALL_FLAMES, 20000);
                        break;
                    case EVENT_SEAR_FLESH:
                        DoCast(SPELL_SEAR_FLESH);
                        events.ScheduleEvent(EVENT_SEAR_FLESH, 15000);
                        break;
                    default:
                        break;
                }
            }

            if (me->isInCombat())
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_searAI(creature);
    }
};
// 54552
class npc_et_time_twisted_breaker : public CreatureScript
{
public:
    npc_et_time_twisted_breaker() : CreatureScript("npc_et_time_twisted_breaker") {}

    struct npc_et_time_twisted_breakerAI : public ScriptedAI
    {
        npc_et_time_twisted_breakerAI(Creature * creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_BREAK_ARMOR, 20000);
            events.ScheduleEvent(EVENT_RAPTURE_G, 15000);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_BREAK_ARMOR:
                        DoCast(SPELL_BREAK_ARMOR);
                        events.ScheduleEvent(EVENT_BREAK_ARMOR, 20000);
                        break;
                    case EVENT_RAPTURE_G:
                        DoCast(SPELL_RUPTURE_G);
                        events.ScheduleEvent(EVENT_RAPTURE_G, 15000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_et_time_twisted_breakerAI(creature);
    }
};

// 54517
class npc_time_twisted_shadowtalon : public CreatureScript
{
public:
    npc_time_twisted_shadowtalon() : CreatureScript("npc_time_twisted_shadowtalon") {}

    struct npc_time_twisted_shadowtalonAI : public ScriptedAI
    {
        npc_time_twisted_shadowtalonAI(Creature* creature) : ScriptedAI(creature) { }


        void UpdateAI(const uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (auto scourge = me->FindNearestCreature(54507, 10.0f))
            {
                scourge->SetReactState(REACT_PASSIVE);
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_twisted_shadowtalonAI(creature);
    }
};

class npc_undying_flame : public CreatureScript
{
public:
    npc_undying_flame() : CreatureScript("npc_undying_flame") { }

    struct npc_undying_flameAI : public ScriptedAI
    {
        npc_undying_flameAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            me->NearTeleportTo(me->GetHomePosition());
            me->GetMotionMaster()->MoveRandom(8.f);

            if (!me->HasAura(102045))
            DoCast(me, 102045, true);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if (id == EVENT_CALL_FLAMES)
                {
                    me->CastSpell((Unit*)nullptr, 105238, false);
                    me->Kill(me, false, false);
                }
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case EVENT_CALL_FLAMES:
                me->GetMotionMaster()->Clear();
                me->StopMoving();
                break;
            default:
                break;
            }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_undying_flameAI(creature);
    }
};

class npc_call_flames : public CreatureScript
{
public:
    npc_call_flames() : CreatureScript("npc_call_flames") { }

    struct npc_call_flamesAI : public ScriptedAI
    {
        npc_call_flamesAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
        }

        void UpdateAI(const uint32 diff) override
        {
            if (!Warning)
            {
                Warning = true;
                DoCastAOE(102153);
                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, 54550, 30.0f);
                for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                    if (auto c = (*itr))
                    {
                        c->AI()->DoAction(EVENT_CALL_FLAMES);
                        c->GetMotionMaster()->MovePoint(EVENT_CALL_FLAMES, (*me), true, 10.f);
                        me->DespawnOrUnsummon(5000);
                    }
            }
        }
    private:
        bool Warning { false };
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_call_flamesAI(creature);
    }
};

class npc_et_alurmi : public CreatureScript
{
public:
    npc_et_alurmi() : CreatureScript("npc_et_alurmi") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        TC_LOG_ERROR("sql.sql", "creature %u OnGossipHello hook.", creature->GetGUID());
        if (creature->isQuestGiver())
        {
            TC_LOG_ERROR("sql.sql", "creature %u is quest giver..", creature->GetGUID());
            player->PrepareQuestMenu(creature->GetGUID());

            creature->CastSpell(creature, 109297, true);
            creature->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
            creature->IsHoverGroundTargetable();
        }
        else
            TC_LOG_ERROR("sql.sql", "creature %u is NOT quest giver..", creature->GetGUID());


        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_et_alurmiAI : public ScriptedAI
    {
        npc_et_alurmiAI(Creature* creature) : ScriptedAI(creature)
        {

        }
    };
    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_et_alurmiAI(creature);
    }
};

void AddSC_end_time_trash()
{
    new npc_et_infinite_supressor();
    new npc_et_time_twisted_drake();
	new npc_et_time_twisted_breaker();
	new npc_et_time_twisted_sear();
    new npc_et_time_twisted_footman();
    new npc_et_time_twisted_geist();
    new npc_et_time_twisted_priest();
    new npc_et_time_twisted_rifleman();
    new npc_et_time_twisted_scourge_beast();
    new npc_et_time_twisted_sourceress();
    new npc_et_infinite_warden();
    new npc_time_twisted_shadowtalon();
    new npc_undying_flame();
    new npc_call_flames();
    new npc_et_alurmi();
}