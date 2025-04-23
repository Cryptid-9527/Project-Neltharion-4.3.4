/*
 * Copyright (C) 2013-2014 trinity core og
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "the_stonecore.h"

enum spells
{
    // rock borer
    SPELL_ROCK_BORE          = 80028,
    SPELL_ROCK_BORE_HC       = 92630,
    // millhouse manastorm
    SPELL_BLUR              = 81216,
    SPELL_FEAR              = 81442,
    SPELL_FROSTBOLT_VOLLEY  = 81440,
    SPELL_IMPENDING_DOOM    = 86830,
    SPELL_SHADOW_BOLT       = 81439,
    SPELL_SHADOWFURY        = 81441,
    SPELL_TIGULE            = 81220,
    SPELL_IMPEMDING_DOOM_E  = 86838,
    //rift conjurer
    SPELL_SHADOW_BOLT_CJ    = 80279,
    SPELL_SUM_IMP           = 80308,
};

enum events
{
    EVENT_MILL_FEAR         = 1,
    EVENT_FROSTBOLT_VOLLEY,
    EVENT_IMPENDING_DOOM,
    EVENT_IMPENDING_DOOM_FAILED,
    EVENT_SHADOW_BOLT,
    EVENT_SHADOWFURY,
    EVENT_SHADOW_BOLT_CJ,
    EVENT_SUM_IMP,
    ACTION_MILLHOUSE_YEETED,
};

/*
Position const millhouseWP[3] =
{
    {985.726f, 889.674f, 304.323f, 2.140f},
    {1057.546f, 864.379f, 293.793f, 2.799f},
    {1149.005f, 883.497f, 284.963f, 3.322f}
};

*/
// Rock Borer AI
class mob_rock_borer : public CreatureScript
{
public:
    mob_rock_borer() : CreatureScript("mob_rock_borer") { }

    struct mob_rock_borerAI : public ScriptedAI
    {
        mob_rock_borerAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            rockboreTimer = urand(5000, 7000);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (rockboreTimer <= diff)
            {
                if (auto victim = me->getVictim())
                {
                    if (Aura* aur = victim->GetAura(IsHeroic() ? SPELL_ROCK_BORE_HC : SPELL_ROCK_BORE))
                    {
                        aur->ModStackAmount(1);
                        aur->RefreshDuration();
                    }
                    else
                        DoCastVictim(SPELL_ROCK_BORE);
                }
                rockboreTimer = urand(10000, 12000);
            }else rockboreTimer -= diff;

            DoMeleeAttackIfReady();
        }
    private:
        uint32 rockboreTimer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_rock_borerAI(creature);
    }
};

// Millhouse Manastorm AI
#define SAY_PRISON "Prison taught me one very important lesson, well, two if you count how to hold your soap, but yes! SURVIVAL!"
enum millhouseTalks
{
    MILLHOUSE_SAY_RETREAT,
    MILLHOUSE_SAY_FINAL_POSITION,
    MILLHOUSE_SAY_FINAL_POSITION_2,

    SPELL_MILLHOUSE_TELEPORT        = 41232,
};

class mob_millhouse_manastorm : public CreatureScript
{
public:
    mob_millhouse_manastorm() : CreatureScript("mob_millhouse_manastorm") { }

    struct mob_millhouse_manastormAI : public ScriptedAI
    {
        mob_millhouse_manastormAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
            changePhase = false;
        }

        void JustReachedHome()
        {
        }

        bool CanRespawn() 
        {
            if (instance->GetBossState(DATA_CORBORUS) == DONE || instance->GetBossState(DATA_CORBORUS) == DONE_HM)
                return false;

            return !end;
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (end)
            {
                me->DeleteThreatList();
                return;
            }

            if (changePhase)
                me->CombatStop(true);
            else
            {
                me->RemoveAura(SPELL_TIGULE);
                events.ScheduleEvent(EVENT_MILL_FEAR, 10000);
                events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, urand(7000, 17000));
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
                events.ScheduleEvent(EVENT_SHADOWFURY, urand(10000, 15000));
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!changePhase && !end)
            if (me->isAlive())
                if (auto p = who->ToPlayer())
                    if (!p->isGameMaster())
                    if (me->GetDistance(who) < 15.f)
                        if (me->HasReactState(REACT_AGGRESSIVE))
                        if (!me->isInCombat())
                        {
                            me->AddThreat(who, 10000.f);
                            me->AI()->AttackStart(who);
                        }
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case ACTION_MILLHOUSE_YEETED:
                events.ScheduleEvent(ACTION_MILLHOUSE_YEETED, 1500);
                break;
            }
        }


        void MovementInform(uint32 type, uint32 id)
        {

            if (type == 16 && id == 1337)
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (type == POINT_MOTION_TYPE)
                if (id == phase)
                {
                    if (!changePhase)
                    {
                        //TC_LOG_ERROR("sql.sql", "failed 2");
                        return;
                    }
                    if (!me->HasAura(81216))
                    {
                        //TC_LOG_ERROR("sql.sql", "failed 3");
                        return;
                    }
                    if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    {
                        //TC_LOG_ERROR("sql.sql", "failed 4");
                        return;
                    }
                    if (!me->HasReactState(REACT_PASSIVE))
                    {
                        //TC_LOG_ERROR("sql.sql", "failed 5");
                        return;
                    }

                    changePhase = false;
                    //me->setFaction(16);
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (phase == 3)
                    {
                        end = true;
                        events.ScheduleEvent(EVENT_IMPENDING_DOOM, 4500);
                        Talk(MILLHOUSE_SAY_FINAL_POSITION);
                        TalkWithDelay(4500, MILLHOUSE_SAY_FINAL_POSITION_2);
                    }
                    else
                    {
                        //me->setRegeneratingHealth(true);
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        DoCast(me, SPELL_TIGULE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    }

                    me->RemoveAura(81216);
                    switch (phase)
                    {
                    case 1:
                        me->SetFacingTo(2.579836f);
                        break;
                    case 2:
                        me->SetFacingTo(5.637407f);
                        break;
                    case 3:
                        me->SetFacingTo(3.347171f);
                        break;
                    default:
                        me->SetFacingTo(3.347171f);
                        break;
                    }
                }

        }

        void TeleportHome()
        {
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) 
        {
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {

            Position pos;

            if (changePhase || me->HasAura(81216) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) || me->HasReactState(REACT_PASSIVE))
            {
                damage = 0;
                me->setRegeneratingHealth(false);
                me->SetHealth(me->GetMaxHealth() * 0.5);
                return;
            }
            else
            if (me->HealthBelowPctDamaged(50, damage))
            {
                changePhase = true;

                me->setRegeneratingHealth(false);
                me->SetHealth(me->GetMaxHealth() * 0.5);
                me->SetReactState(REACT_PASSIVE);
                me->CastStop();
                events.Reset();
                me->DeleteThreatList();
                me->ClearUnitState(UNIT_STATE_CASTING);
                me->AddAura(81216, me);
                //me->setFaction(35);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetSpeed(MOVE_RUN, 2.5f);
                Talk(MILLHOUSE_SAY_RETREAT);
                if (auto p = attacker->ToPlayer())
                {
                    if (!p->isGameMaster())
                        damage = 0;
                }
                else
                {
                    damage = 0;
                }

                switch (phase)
                {
                case 0:
                    //TC_LOG_ERROR("sql.sql", "setting home position for value %u", phase);
                    pos = { 978.909485f, 893.106079f, 305.773102f, 0.0f }; // 2.579836
                    me->SetHomePosition(pos);
                    break;
                case 1:
                    //TC_LOG_ERROR("sql.sql", "setting home position for value %u", phase);
                    pos = { 1051.763672f, 871.441528f, 294.806061f, 0.0f }; // 5.891862
                    me->SetHomePosition(pos);
                    break;
                case 2:
                    //TC_LOG_ERROR("sql.sql", "setting home position for value %u", phase);
                    pos = { 1147.33f, 882.51f, 284.97f, 0.0f }; // 3.347171
                    me->SetHomePosition(pos);
                    end = true;
                    break;
                }
                if (phase <= 2)
                phase++;
                me->GetMotionMaster()->MovePoint(phase, me->GetHomePosition(), true);   //placeholder, will do move smooth path
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (changePhase)
                return;

            if (me->isInCombat())
            if (phase < 3)
            {
                auto players = me->GetPlayersInRange(40.f, true);
                if (!players.size())
                        EnterEvadeMode();
            }

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_MILL_FEAR:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCastRandom(SPELL_FEAR, 0.0f);
                            events.ScheduleEvent(EVENT_MILL_FEAR, 10000);
                        }
                        break;
                    case EVENT_SHADOW_BOLT_CJ:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCastVictim(SPELL_SHADOW_BOLT_CJ);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT_CJ, 2000);
                        }
                        break;
                    case EVENT_FROSTBOLT_VOLLEY:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCast(SPELL_FROSTBOLT_VOLLEY);
                            events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, urand(8000, 15000));
                        }
                        break;
                    case EVENT_IMPENDING_DOOM:
                        me->AddAura(SPELL_IMPEMDING_DOOM_E, me);
                        DoCast(me, SPELL_IMPENDING_DOOM);
                        events.ScheduleEvent(EVENT_IMPENDING_DOOM_FAILED, urand(6000, 12000));
                        break;
                    case EVENT_IMPENDING_DOOM_FAILED:
                        me->CastStop();
                        me->RemoveAllAuras();
                        DoCast(me, 42963, true);
                        events.ScheduleEvent(EVENT_IMPENDING_DOOM, 3000);
                        break;
                    case EVENT_SHADOWFURY:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCastRandom(SPELL_SHADOWFURY, 0.0f);
                            events.ScheduleEvent(EVENT_SHADOWFURY, urand(8000, 20000));
                        }
                        break;
                    case ACTION_MILLHOUSE_YEETED:

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);

                        events.Reset();
                        me->PlayDirectSound(18353);
                        me->CastStop();
                        me->RemoveAllAuras();
                        me->DeleteThreatList();
                        Position somewhere_out_of_map_and_sight{ 1161.518311f, 621.752869f, 267.934174f, 3.378545f };
                        std::list<Creature*> eventCr;
                        me->GetCreatureListWithEntryInGrid(eventCr, 43430, 30.0f);
                        me->GetCreatureListWithEntryInGrid(eventCr, 42696, 30.0f);
                        me->GetCreatureListWithEntryInGrid(eventCr, 43537, 30.0f);
                        for (auto c : eventCr)
                            {
                                c->GetMotionMaster()->MoveJump(1169.36f, 789.21f, 312.0f, 20.0f, 20.0f);    //the yeet happening
                                c->DespawnOrUnsummon(2000);
                                c->SetHomePosition(somewhere_out_of_map_and_sight);
                            }
                        me->GetMotionMaster()->MoveJump(1169.36f, 789.21f, 312.0f, 20.0f, 20.0f, 1337);
                        me->AddUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR);
                        me->DespawnOrUnsummon(1500);
                        me->SetHomePosition(somewhere_out_of_map_and_sight);
                        break;
                }

            if  (!me->HasUnitState(UNIT_STATE_CASTING))
                if (!end && !changePhase)
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
        }
    private:
        Position previous_Home;
        EventMap events;
        uint32 phase{ 0 };
        bool changePhase{ false };
        bool end{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_millhouse_manastormAI(creature);
    }
};

class mob_stonecore_rift_conjurer : public CreatureScript
{
public:
    mob_stonecore_rift_conjurer() : CreatureScript("mob_stonecore_rift_conjurer") { }

    struct mob_stonecore_rift_conjurerAI : public ScriptedAI
    {
        mob_stonecore_rift_conjurerAI(Creature* creature) : ScriptedAI(creature), _summons(creature) { }

        void Reset()
        {
            events.Reset();
            _summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(5000, 7000));
            events.ScheduleEvent(EVENT_SUM_IMP, 15000);
        }

        void JustSummoned(Creature* summoned)
        {
            _summons.Summon(summoned);
        }

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOW_BOLT:
                        DoCastRandom(SPELL_SHADOW_BOLT, 0.0f);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 7000));
                        break;
                    case EVENT_SUM_IMP:
                        DoCast(SPELL_SUM_IMP);
                        events.ScheduleEvent(EVENT_SUM_IMP, 30000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_stonecore_rift_conjurerAI(creature);
    }
};

enum giantEvent
{
    EVENT_SECOUSSE = 1,
    EVENT_CRISTAL_ECLAT,
};

enum giantSpell
{
    SPELL_SECOUSSE = 81008,
    SPELL_CRISTAL_ECLAT = 81015,
    SPELL_CRISTAL_ECLAT_TRIG_1 = 92165, // summon 49473
    SPELL_CRISTAL_ECLAT_TRIG_2 = 92176,
    SPELL_CRISTAL_AURA = 80919,
    SPELL_CRISTAL_SUMMON = 92150,

    SPELL_RANDOM_AGGRO = 92111,
    SPELL_CRISTAL_EXPLOSION = 80913,
};

class CristalEvent : public BasicEvent
{
public:
    CristalEvent(Creature* caster) : _caster(caster) {}

    bool Execute(uint64 execTime, uint32 /*diff*/)
    {
        _caster->CastSpell(_caster, SPELL_CRISTAL_SUMMON, true);
        _caster->DespawnOrUnsummon(100);
        return true;
    }

private:
    Creature* _caster;
};

class mob_stonecore_crystalspawn_giant : public CreatureScript
{
public:
    mob_stonecore_crystalspawn_giant() : CreatureScript("mob_stonecore_crystalspawn_giant") { }

    struct mob_stonecore_crystalspawn_giantAI : public ScriptedAI
    {
        mob_stonecore_crystalspawn_giantAI(Creature* creature) : ScriptedAI(creature), _summons(creature) 
		{ 
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
		}
		
		
        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        void Reset()
        {
            events.Reset();
            _summons.DespawnAll();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_CRISTAL_ECLAT, urand(5000, 7000));
            events.ScheduleEvent(EVENT_SECOUSSE, 5000);
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
            summon->CastSpell(summon, SPELL_CRISTAL_AURA, true);
            summon->m_Events.AddEvent(new CristalEvent(summon), summon->m_Events.CalculateTime(4000));
            _summons.Summon(summon);
        }

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CRISTAL_ECLAT:
                        DoCastRandom(SPELL_CRISTAL_ECLAT, 0.0f);
                        events.ScheduleEvent(EVENT_CRISTAL_ECLAT, urand(5000, 11000));
                        break;
                    case EVENT_SECOUSSE:
                        DoCast(SPELL_SECOUSSE);
                        events.ScheduleEvent(EVENT_SECOUSSE, 30000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_stonecore_crystalspawn_giantAI(creature);
    }
};

class mob_stonecore_cristal : public CreatureScript
{
public:
    mob_stonecore_cristal() : CreatureScript("mob_stonecore_cristal") { }

    struct mob_stonecore_cristalAI : public ScriptedAI
    {
        mob_stonecore_cristalAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            mui_check_dist = 3000; // safe timer
            mui_check_aggro = 100;
            me->SetInCombatWithZone();
            me->SetSpeed(MOVE_WALK, 0.9f);
            me->SetSpeed(MOVE_RUN, 0.9f);
        }


        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (mui_check_aggro <= diff)
            {
                DoCastAOE(SPELL_RANDOM_AGGRO);
                mui_check_aggro = 300000;
            }
            else
                mui_check_aggro -= diff;

            if (mui_check_dist <= diff)
            {
                if (Unit *c = me->getVictim())
                    if (me->GetDistance(c) <= 1.5f)
                    {
                        me->CastSpell(c, SPELL_CRISTAL_EXPLOSION, true);
                        me->DespawnOrUnsummon(500);
                    }
                mui_check_dist = 500;
            }
            else
                mui_check_dist -= diff;
        }

    private:
        uint32 mui_check_dist;
        uint32 mui_check_aggro;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_stonecore_cristalAI(creature);
    }
};

class spell_cristal_giant_secousse : public SpellScriptLoader
{
    class SecousseCheck
    {
    public:
        SecousseCheck()
        {
        }

        bool operator() (WorldObject* obj)
        {
            if (Unit *target = obj->ToUnit())
                if (target->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                    return true;
            return false;
        }
    };

public:
    spell_cristal_giant_secousse() : SpellScriptLoader("spell_cristal_giant_secousse")
    {
    }

    class script_impl : public SpellScript
    {
        PrepareSpellScript(script_impl);

        bool Load()
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if(SecousseCheck());
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(script_impl::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new script_impl();
    }
};

enum StoneCoreQuest  {
	Quest0 = 28815,
	Quest1 = 28814,
};

class stonecore_quest_line : public CreatureScript
{
public:
	stonecore_quest_line() : CreatureScript("stonecore_quest_line") { }

	struct stonecore_quest_lineAI : public ScriptedAI
	{
		stonecore_quest_lineAI(Creature* creature) : ScriptedAI(creature) {}

		void UpdateAI(uint32 const diff)
		{
			if (auto player = me->FindNearestPlayer(100.0f))
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			if (auto player = me->FindNearestPlayer(5.0f))
			{
				if (player->GetQuestStatus(Quest0) == QUEST_STATUS_COMPLETE && !player->IsActiveQuest(Quest1))
				{
					if (Quest const* quest = sObjectMgr->GetQuestTemplate(Quest1))
					{
						if (player->CanAddQuest(quest, false))
							player->AddQuest(quest, NULL);
					}
				}

			}

		}

	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new stonecore_quest_lineAI(creature);
	}
};

#define spell_beat_down 80221
#define spell_body_slam 80180
#define spell_shockwave 92640

class mob_stonecore_bruiser : public CreatureScript
{
public:
	mob_stonecore_bruiser() : CreatureScript("mob_stonecore_bruiser") {}

	struct mob_stonecore_bruiserAI : public ScriptedAI
	{
		mob_stonecore_bruiserAI(Creature * pCreature) : ScriptedAI(pCreature) 
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
		}

		uint32 beat_down;
		uint32 body_slam;
		uint32 shockwave;

		void Reset()
		{
			beat_down = urand(5000, 8000);
			body_slam = 15000;
			shockwave = urand(20000, 25000);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (beat_down <= diff)
			{
				DoCast(me->getVictim(), spell_beat_down);
				beat_down = urand(5000, 8000);
			}
			else beat_down -= diff;

			if (body_slam <= diff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, -5.0f, true))
					DoCast(pTarget, body_slam);
				body_slam = 15000;
			}
			else body_slam -= diff;

			if (shockwave <= diff)
			{
				DoCastVictim(spell_shockwave);
				shockwave = urand(20000, 25000);
			}
			else shockwave -= diff;

			DoMeleeAttackIfReady();
		}

	};

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new mob_stonecore_bruiserAI(pCreature);
	}

};

class mob_stonecore_earthshaper : public CreatureScript
{
public:
    mob_stonecore_earthshaper() : CreatureScript("mob_stonecore_earthshaper") {}

    struct mob_stonecore_earthshaperAI : public ScriptedAI
    {
        mob_stonecore_earthshaperAI(Creature* creature) : ScriptedAI(creature) { maxdamage = 0; }

        void Reset()
        {
            events.Reset();
            me->RestoreDisplayId();
            if (maxdamage == 0)
            {
                mindamage = me->GetFloatValue(UNIT_FIELD_MINDAMAGE);
                maxdamage = me->GetFloatValue(UNIT_FIELD_MAXDAMAGE);
            }
            else
            {
                me->SetFloatValue(UNIT_FIELD_MINDAMAGE, mindamage);
                me->SetFloatValue(UNIT_FIELD_MAXDAMAGE, maxdamage);
            }              
        }

        void EnterCombat(Unit* /*victim*/)
        {
            events.ScheduleEvent(1, urand(500, 1000));
            events.ScheduleEvent(2, urand(3500, 4000));
            events.ScheduleEvent(3, urand(5000, 8000));
        }

        void SetData(uint32 /*id*/, uint32 /*value*/)
        {
            me->SetDisplayId(26693);
            events.ScheduleEvent(4, 3000);     
            me->SetFloatValue(UNIT_FIELD_MINDAMAGE, mindamage * 2);
            me->SetFloatValue(UNIT_FIELD_MAXDAMAGE, maxdamage * 2);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case 1:
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(81576);
                            events.ScheduleEvent(1, urand(5000, 6000));
                        }
                        else events.ScheduleEvent(1, 500);
                    }
                    break;
                    case 2:
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(81530);
                            events.ScheduleEvent(1, urand(15000, 20000));
                        }
                        else events.ScheduleEvent(2, 500);
                    }
                    break;
                    case 3:
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(81459);
                        }
                        else events.ScheduleEvent(3, 500);
                    }
                    break;
                    case 4:
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(81463);
                            events.ScheduleEvent(4, urand(18000, 20000));
                        }
                        else events.ScheduleEvent(4, 500);
                    }
                    break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        float mindamage;
        float maxdamage;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_stonecore_earthshaperAI(creature);
    }
};

class spell_force_of_earth : public SpellScriptLoader
{
public:
    spell_force_of_earth() : SpellScriptLoader("spell_force_of_earth") {}

    class spell_force_of_earth_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_force_of_earth_SpellScript);

        void EffectHit(SpellEffIndex /*effIndex*/)
        {
            if (auto caster = GetCaster())
                if (caster->isAlive())
                    caster->GetAI()->SetData(0, 1);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_force_of_earth_SpellScript::EffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_force_of_earth_SpellScript();
    }
};

class npc_stonecore_shale : public CreatureScript
{
public:
    npc_stonecore_shale() : CreatureScript("npc_stonecore_shale") { }

    struct npc_stonecore_shaleAI : public ScriptedAI
    {
        npc_stonecore_shaleAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* who)
        {
            me->DeleteThreatList();
            me->CombatStop();
        }
    };    
    
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stonecore_shaleAI(creature);
    }
};


void AddSC_the_stonecore()
{
    new mob_rock_borer();
    new mob_millhouse_manastorm();
    new mob_stonecore_rift_conjurer();
    new mob_stonecore_crystalspawn_giant();
    new spell_cristal_giant_secousse();
    new mob_stonecore_cristal();
	new stonecore_quest_line();
	new mob_stonecore_bruiser();
    new mob_stonecore_earthshaper();
    new spell_force_of_earth();
    new npc_stonecore_shale();
}
