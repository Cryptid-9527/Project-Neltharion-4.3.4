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

enum Texts
{
	TALK_ENTER_COMBAT = -54769,
	TALK_PLAYER_DEATH = -54770,
	TALK_HIGHBORNE = -54773,
	TALK_WIPE = -54772,
	TALK_DEATH = -54774,
};

enum Spells
{
	// echo of sylvanas
	SPELL_CALLING_THE_HIGHBORNE_DUMMY = 102581,
	SPELL_SYLVANAS_JUMP = 101528,
	SPELL_GHOUL_SUMMON_PERIODIC = 102603,
	SPELL_GHOUL_SUMMON_MISSILE = 102604,
	SPELL_GHOUL_SUMMON = 102607,
	SPELL_BLACK_ARROW = 101404,
	SPELL_UNHOLLY_SHOT = 101411,
	SPELL_BLIGHTED_ARROWS = 101567,
	SPELL_BLIGHTED_ARROWS_DUMMY = 101549,
	SPELL_PLAYERS_SUMMON_BLIGHTED_ARROW = 101547,
	SPELL_JUMP_SUMMON = 101517, // unused because of some ugly summon bugs
	SPELL_BLIGHTED_ARROWS_MORPH = 101401,
	SPELL_SHRIEK_OF_THE_HIGHBORNE = 101412,
	SPELL_SUMMON_GHOUL_SCRIPT_EFF = 101198,
	SPELL_SUMMON_GHOUL_O = 100920,
	SPELL_SUMMON_GHOUL_SO = 100924,
	SPELL_SUMMON_GHOUL_SE = 100923,
	SPELL_SUMMON_GHOUL_S = 100921,
	SPELL_SUMMON_GHOUL_NO = 100918,
	SPELL_SUMMON_GHOUL_NE = 100917,
	SPELL_SUMMON_GHOUL_N = 100894,
	SPELL_SUMMON_GHOUL_E = 100919,
	SPELL_DEATH_GRIP = 101397,
	SPELL_DEATH_GRIP_TRIGGER = 101987,
	SPELL_SACRIFICE = 101348,
	SPELL_CALLING_THE_HIGHBORNE = 100686,
	SPELL_SYLVANAS_TELEPORT = 101398,
	// NPC_BRITTLE_GHOUL
	SPELL_GHOUL_FEIGN_DEATH = 96733, // in sniff but not on videos
	// NPC_JUMP_TRIGGER
	SPELL_JUMP_FORCE_SUMMONER = 101527,
	// NPC_BLIGHTED_ARROW
	SPELL_BLIGHTED_ARROW_AURA = 101552,
	SPELL_BLIGHTED_ARROW_KNOCKBACK = 100763,
	// NPC_GHOUL
	SPELL_GHOULD_CALLING_OF_THE_HIGHBORNE = 100867,
	SPELL_GHOUL_SUMMON_GHOUL = 101200,
	SPELL_GHOULD_CALLING_OF_THE_HIGHBORNE_DUMMY = 105766,
	SPELL_SHRINK = 101318,
	SPELL_SHRINK_TRIGGER = 101271,
	SPELL_PAIN_BACK_DAMAGE = 101258,
	SPELL_PAIN_DAMAGE = 101221,
	// NPC_RISEN_GHOUL
	SPELL_RISEN_GHOUL_CALL_OF_THE_HIGHBORNE = 100862,
	SPELL_WRACKING_PAIN_PERIODIC_DUMMY = 100865,
	SPELL_WRACKING_PAIN = 104876, // unused
	SPELL_OMAR_APPRO_SCEAU = 104684, // unused
	SPELL_WRACKING_PAIN_AREA_EFFECT = 104876,
	SPELL_DESPAWN_WRACKING_PAIN_AREA = 105778,
	SPELL_SEEPING_SHADOWS_DUMMY = 103175,
	SPELL_SEEPING_SHADOWS_AURA = 103182,
	SPELL_DISAPEAR = 101259,
};

enum Events
{
	EVENT_PRE_CALLING_OF_THE_HIGHBORNE = 1,
	EVENT_CALLING_OF_THE_HIGHBORNE,
	EVENT_SHRINK_OF_THE_HIGHBORNE,
	EVENT_DARK_ARROW,
	EVENT_BLIGHTED_ARROWS,
	EVENT_UNHOLLY_SHOT,
	EVENT_REACT_AGGRESSIVE,
	EVENT_JUMP_RECOVERY,

};

enum Miscs
{
	POINT_SACRIFICE = 1414,
	ACTION_SACRIFICE,
	DATA_ACHIEVEMENT_CHECK,
	WORLDSTATE_GHOUL_ACHIEVEMENT = 6225,
};

class MoveEvent : public BasicEvent
{
public:
	MoveEvent(uint64 summonerGUID, Creature* owner) : sylvanasGUID(summonerGUID), summon(owner) {}

	bool Execute(uint64 /*time*/, uint32 /*diff*/)
	{

		if (auto sylvanas = ObjectAccessor::GetCreature(*summon, sylvanasGUID))
			summon->GetMotionMaster()->MovePoint(POINT_SACRIFICE, sylvanas->GetPositionX(), sylvanas->GetPositionY(), summon->GetPositionZ());
		if (summon->GetEntry() == NPC_RISEN_GHOUL)
			summon->CastSpell(summon, SPELL_RISEN_GHOUL_CALL_OF_THE_HIGHBORNE, true);
		return true;
	}

private:
	Creature* summon;
	uint64 sylvanasGUID;
};

class AuraRemoveEvent : public BasicEvent
{
public:
	AuraRemoveEvent(Creature* summon) : _summon(summon) {}

	bool Execute(uint64 /*time*/, uint32 /*diff*/)
	{
		_summon->RemoveAurasDueToSpell(SPELL_GHOULD_CALLING_OF_THE_HIGHBORNE);
		return true;
	}

private:
	Creature* _summon;
};

// 54123
class boss_echo_of_sylvanas : public CreatureScript
{
public:
	boss_echo_of_sylvanas() : CreatureScript("boss_echo_of_sylvanas") { }
	struct boss_echo_of_sylvanasAI : public BossAI
	{
		boss_echo_of_sylvanasAI(Creature* creature) : BossAI(creature, BOSS_ECHO_OF_SYLVANAS)
		{
			creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
		}

		void Reset() override
		{
			_Reset();
			RemoveEncounterFrame();
			me->SetReactState(REACT_AGGRESSIVE);
			me->RemoveAurasDueToSpell(SPELL_CALLING_THE_HIGHBORNE_DUMMY);
			me->RemoveAurasDueToSpell(SPELL_GHOUL_SUMMON_PERIODIC);
			me->CastSpell(me, SPELL_GHOUL_SUMMON_PERIODIC, true);
			me->CastSpell(me, SPELL_CALLING_THE_HIGHBORNE_DUMMY, true);
			sacrificeCasted = false;
		}

		void JustDied(Unit* killer) override
		{
			Talk(TALK_DEATH);
			RemoveEncounterFrame();
			_JustDied();
			if (auto player = me->FindNearestPlayer(100.0f))
				player->SummonGameObject(209443, 3822.4487f, 935.4551f, 55.3302002f, 55.8124f, 5.2739f, 0, 1, 1, 0);
		}

		void EnterEvadeMode() override
		{
			Talk(TALK_WIPE);
			BossAI::EnterEvadeMode();
		}

		void KilledUnit(Unit* victim) override
		{
			if (victim->GetTypeId() == TYPEID_PLAYER)
				Talk(TALK_PLAYER_DEATH);
		}

		void DoAction(const int32 action) override
		{
			switch (action)
			{
			case ACTION_SACRIFICE:
				if (!sacrificeCasted)
				{
					sacrificeCasted = true;
					summons.DespawnEntry(NPC_GHOUL);
					summons.DespawnEntry(NPC_RISEN_GHOUL);

					events.ScheduleEvent(EVENT_JUMP_RECOVERY, 1000);
					me->RemoveAurasDueToSpell(SPELL_CALLING_THE_HIGHBORNE);
					DoCast(me, SPELL_SACRIFICE, true);
					events.ScheduleEvent(EVENT_BLIGHTED_ARROWS, 2000);
					events.ScheduleEvent(EVENT_SHRINK_OF_THE_HIGHBORNE, 14000);
					events.ScheduleEvent(EVENT_UNHOLLY_SHOT, 17000);
					events.ScheduleEvent(EVENT_DARK_ARROW, 23000);
					//events.ScheduleEvent(EVENT_PRE_CALLING_OF_THE_HIGHBORNE, 33000);
				}
				break;
			}
		}

		void EnterCombat(Unit* who) override
		{
			Talk(TALK_ENTER_COMBAT);
			AddEncounterFrame();
			summons.DespawnEntry(NPC_BRITTLE_GHOUL);
			me->RemoveAurasDueToSpell(SPELL_CALLING_THE_HIGHBORNE_DUMMY);
			me->RemoveAurasDueToSpell(SPELL_GHOUL_SUMMON_PERIODIC);
			events.ScheduleEvent(EVENT_SHRINK_OF_THE_HIGHBORNE, 7000);
			events.ScheduleEvent(EVENT_UNHOLLY_SHOT, 14000);
			events.ScheduleEvent(EVENT_BLIGHTED_ARROWS, 16000);
			events.ScheduleEvent(EVENT_DARK_ARROW, 28000);
			events.ScheduleEvent(EVENT_PRE_CALLING_OF_THE_HIGHBORNE, 38000);
			achievementDone = false; // worldstate update is only client side we need a bool for the achievement script... 
			instance->DoUpdateWorldState(WORLDSTATE_GHOUL_ACHIEVEMENT, 3); // Display achievement as failed until the conditions are met
			_EnterCombat();
		}

		uint32 GetData(uint32 data) const override
		{
			if (data == DATA_ACHIEVEMENT_CHECK)
				return achievementDone ? 1 : 0;
			return 0;
		}

		void JustSummoned(Creature* summon) override
		{
			switch (summon->GetEntry())
			{
			case NPC_GHOUL:
				summon->SetReactState(REACT_PASSIVE);
				summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				summon->SetFacingToObject(me);
				summon->m_Events.AddEvent(new AuraRemoveEvent(summon), summon->m_Events.CalculateTime(2600));
				summon->CastWithDelay(1000, summon, SPELL_GHOULD_CALLING_OF_THE_HIGHBORNE, true);
				summon->CastWithDelay(2000, summon, SPELL_GHOUL_SUMMON_GHOUL, true);
				summon->CastWithDelay(4000, summon, SPELL_GHOULD_CALLING_OF_THE_HIGHBORNE_DUMMY, true);
				summon->CastWithDelay(4000, summon, SPELL_SHRINK, true);
				break;
			case NPC_JUMP_TRIGGER:
				me->CastWithDelay(500, summon, SPELL_SYLVANAS_JUMP, true);
				break;
			case NPC_RISEN_GHOUL:
				summon->SetReactState(REACT_PASSIVE);
				summon->SetWalk(true);
				summon->m_Events.AddEvent(new MoveEvent(me->GetGUID(), summon), summon->m_Events.CalculateTime(5000));
				return;
			case NPC_BLIGHTED_ARROW:
				summon->SetReactState(REACT_PASSIVE);
				summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				summon->CastSpell(summon, SPELL_BLIGHTED_ARROW_AURA, true);
				break;
			}
			BossAI::JustSummoned(summon);
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer) override
		{
			if (summon->GetEntry() == NPC_RISEN_GHOUL)
			{
				if (++ghoulKillCount >= 2)
				{
					achievementDone = true;
					instance->DoUpdateWorldState(WORLDSTATE_GHOUL_ACHIEVEMENT, 1); // Achievement Worldstate
				}
			}
			BossAI::SummonedCreatureDies(summon, killer);
		}

		void SpellHitTarget(Unit* target, SpellInfo const* spellInfo) override
		{
			if (spellInfo->Id == SPELL_BLIGHTED_ARROWS_MORPH)
			{
				events.ScheduleEvent(EVENT_JUMP_RECOVERY, 1000);
			}
		}

		void UpdateAI(const uint32 diff) override
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
				case EVENT_PRE_CALLING_OF_THE_HIGHBORNE:
					//me->SetCanFly(true);
					//me->SetDisableGravity(true);
					events.Reset();
					me->SetReactState(REACT_PASSIVE);
					me->AttackStop();
					me->AddUnitState(UNIT_STATE_ROOT);
					DoCast(me, SPELL_SYLVANAS_TELEPORT, true);
					events.ScheduleEvent(EVENT_PRE_CALLING_OF_THE_HIGHBORNE, 66000);
					events.ScheduleEvent(EVENT_CALLING_OF_THE_HIGHBORNE, 2000);
					break;
				case EVENT_CALLING_OF_THE_HIGHBORNE:
					me->SetReactState(REACT_PASSIVE);
					sacrificeCasted = false;
					ghoulKillCount = 0;
					Talk(TALK_HIGHBORNE);
					DoCast(me, SPELL_CALLING_THE_HIGHBORNE, true);
					me->CastWithDelay(1000, me, SPELL_SUMMON_GHOUL_SCRIPT_EFF, true);
					me->CastWithDelay(3000, me, SPELL_DEATH_GRIP, true);
					break;
				case EVENT_SHRINK_OF_THE_HIGHBORNE:
					DoCastRandom(SPELL_SHRIEK_OF_THE_HIGHBORNE, 100.0f);
					// No repeat
					break;
				case EVENT_DARK_ARROW:
					DoCastRandom(SPELL_BLACK_ARROW, 45.0f);
					// No repeat
					break;
				case EVENT_BLIGHTED_ARROWS:
					if (auto v = me->GetVehicle())
					{
						events.ScheduleEvent(EVENT_BLIGHTED_ARROWS, 5000);
						break;
					}
					me->SetReactState(REACT_PASSIVE);
					me->AttackStop();
					//me->SetCanFly(true);
					//me->SetDisableGravity(true);
					DoCast(me, SPELL_BLIGHTED_ARROWS, true);
					me->CastWithDelay(3500, me, SPELL_BLIGHTED_ARROWS_MORPH, false);
					if (auto vehicle = me->SummonCreature(NPC_JUMP_TRIGGER, me->GetPositionX(), me->GetPositionY(), 63.00f, 0.00f, TEMPSUMMON_MANUAL_DESPAWN))
						if (auto arrow = vehicle->FindNearestCreature(NPC_BLIGHTED_ARROW, 30.00f))
							vehicle->SetFacingToObject(arrow);
					// No repeat
					return;
				case EVENT_UNHOLLY_SHOT:
					DoCastRandom(SPELL_UNHOLLY_SHOT, 45.0f);
					events.ScheduleEvent(EVENT_UNHOLLY_SHOT, 18000);
					break;
				case EVENT_JUMP_RECOVERY:
				{
					me->SetCanFly(false);
					me->SetDisableGravity(false);
					me->SetHover(false);
					Position pos(*me);

					me->ClearUnitState(UNIT_STATE_ROOT);
					if (auto v = me->getVictim())
					{
						v->MoveBlink(pos, v->GetExactDist2d(me)*0.7f, v->GetRelativeAngleOffset(me));
						pos.m_positionZ += 1.f;
					}
					else
					{
						auto players = me->GetPlayersInRange(25.f, true);
						if (players.size())
							if (auto random_target = Trinity::Containers::SelectRandomContainerElement(players))
							{

								pos = (*random_target);
								random_target->MoveBlink(pos, random_target->GetExactDist2d(me)*0.7f, v->GetRelativeAngleOffset(me));
								pos.m_positionZ += 1.f;
							}
					}

					if (auto v = me->GetVehicle())
						if (auto b = v->GetBase())
						{
							b->SetHover(false);
							b->GetMotionMaster()->MoveJump(pos, 10.f, 10.f);
						}


					events.ScheduleEvent(EVENT_REACT_AGGRESSIVE, 2000);
					break;
				}
				case EVENT_REACT_AGGRESSIVE:
				{
					if (auto v = me->GetVehicle())
						if (auto b = v->GetBase())
							me->ExitVehicle(b);

					summons.DespawnEntry(NPC_JUMP_TRIGGER, 100);
					me->ClearUnitState(UNIT_STATE_ROOT);
					me->SetReactState(REACT_AGGRESSIVE);
					break;
				}
				default:
					break;
				}
			}

			if (!me->HasReactState(REACT_PASSIVE))
				if (UpdateVictim())
					DoMeleeAttackIfReady();
		}

	private:
		bool sacrificeCasted;
		bool achievementDone;
		uint8 ghoulKillCount;
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new boss_echo_of_sylvanasAI(creature);
	}
};

class npc_sylvanas_risen_ghoul : public CreatureScript
{
public:
	npc_sylvanas_risen_ghoul() : CreatureScript("npc_sylvanas_risen_ghoul") {}

	struct npc_sylvanas_risen_ghoulAI : public ScriptedAI
	{
		npc_sylvanas_risen_ghoulAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) 
		{
			stacks = 0;
			nextThreshold = 0;
			me->DeleteThreatList();
		}

		void Reset() override
		{
			me->DeleteThreatList();
			HealthTemp = me->GetMaxHealth();
			me->SetReactState(REACT_PASSIVE);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);

			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND_SIGHT, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE_PCT, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST_2, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INSTAKILL, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INEBRIATE, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_BURN, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_DRAIN, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT_ALL, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_TAMECREATURE, true);

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);

			stacks = 0;
			nextThreshold = 95;
		}

		void JustDied(Unit* /*killer*/) override
		{
			if (me->ToTempSummon())
				if (auto ghoulVis = me->ToTempSummon()->GetSummoner())
					ghoulVis->ToCreature()->DespawnOrUnsummon(100);
			if (auto sylvanas = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_SYLVANAS_GUID)))
				sylvanas->AI()->SummonedCreatureDies(me, NULL);
		}

		bool CanAIAttack(Unit const* who) const
		{
			return false;
		}

		void IsSummonedBy(Unit* summoner) override
		{
			if (auto sylvanas = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_SYLVANAS_GUID)))
			{
				sylvanas->AI()->JustSummoned(me);
				summoner->m_Events.AddEvent(new MoveEvent(sylvanas->GetGUID(), summoner->ToCreature()), summoner->m_Events.CalculateTime(5100));
			}
		}

		void MovementInform(uint32 type, uint32 id) override
		{
			if (type == POINT_MOTION_TYPE && id == POINT_SACRIFICE)
				if (auto sylvanas = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_SYLVANAS_GUID)))
					sylvanas->AI()->DoAction(ACTION_SACRIFICE);
		}

		void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
		{
			if (me->ToUnit()->HasAura(SPELL_SEEPING_SHADOWS_AURA))
			{
				if (Aura* seeping = me->ToUnit()->GetAura(SPELL_SEEPING_SHADOWS_AURA, me->GetGUID()))
				if (seeping->GetStackAmount() < 10)
					if (me->HealthBelowPct(nextThreshold))
					{
						me->CastSpell(me, SPELL_SEEPING_SHADOWS_AURA, true);
						nextThreshold -= 5;
						stacks++;
					}
			}
			else
			{
				if (me->HealthBelowPct(95))
				{
					me->CastSpell(me, SPELL_SEEPING_SHADOWS_AURA, true);
					nextThreshold -= 5;
					stacks++;
				}
			}
		}
		uint32 stacks;
		uint32 nextThreshold;
		uint32 HealthTemp;


		void UpdateAI(const uint32 diff) override
		{
		}

	private:
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new npc_sylvanas_risen_ghoulAI(creature);
	}
};

// 54403
class npc_sylvanas_blighted_arrow : public CreatureScript
{
public:
	npc_sylvanas_blighted_arrow() : CreatureScript("npc_sylvanas_blighted_arrow") {}

	struct npc_sylvanas_blighted_arrowAI : public ScriptedAI
	{
		npc_sylvanas_blighted_arrowAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) {}

		void IsSummonedBy(Unit* summoner) override
		{
			if (auto sylvanas = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_SYLVANAS_GUID)))
				sylvanas->AI()->JustSummoned(me);

			me->SetInCombatWithZone();
		}

	private:
		InstanceScript* instance;
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new npc_sylvanas_blighted_arrowAI(creature);
	}
};

// 101198
class spell_echo_of_sylvanas_ghoul_summon_sc : public SpellScriptLoader
{
public:
	spell_echo_of_sylvanas_ghoul_summon_sc() : SpellScriptLoader("spell_echo_of_sylvanas_ghoul_summon_sc") { }

	class spell_echo_of_sylvanas_ghoul_summon_sc_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_echo_of_sylvanas_ghoul_summon_sc_SpellScript);

		void HandleScriptEffect(SpellEffIndex effIndex)
		{
			if (auto caster = GetCaster())
			{
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_O, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_SO, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_SE, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_S, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_NO, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_NE, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_N, true);
				caster->CastSpell(caster, SPELL_SUMMON_GHOUL_E, true);
				caster->SummonCreature(NPC_JUMP_TRIGGER, caster->GetPositionX(), caster->GetPositionY(), 63.00f, 0.00f, TEMPSUMMON_MANUAL_DESPAWN);
			}
		}

		void Register() override
		{
			OnEffectHitTarget += SpellEffectFn(spell_echo_of_sylvanas_ghoul_summon_sc_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_echo_of_sylvanas_ghoul_summon_sc_SpellScript();
	}
};

// 101397
class spell_echo_of_sylvanas_death_grip_sc : public SpellScriptLoader
{
public:
	spell_echo_of_sylvanas_death_grip_sc() : SpellScriptLoader("spell_echo_of_sylvanas_death_grip_sc") { }

	class spell_echo_of_sylvanas_death_grip_sc_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_echo_of_sylvanas_death_grip_sc_SpellScript);

		void HandleScriptEffect(SpellEffIndex effIndex)
		{
			if (auto caster = GetCaster())
				if (auto target = GetHitUnit())
				{
					uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
					target->CastWithDelay(100, caster, spellId, true);
				}
		}

		void Register() override
		{
			OnEffectHitTarget += SpellEffectFn(spell_echo_of_sylvanas_death_grip_sc_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_echo_of_sylvanas_death_grip_sc_SpellScript();
	}
};

// 101517
class spell_sylvanas_jump_summon : public SpellScriptLoader
{
public:
	spell_sylvanas_jump_summon() : SpellScriptLoader("spell_sylvanas_jump_summon") { }

	class spell_sylvanas_jump_summon_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_sylvanas_jump_summon_SpellScript);

		void ModDestHeight(SpellEffIndex /*effIndex*/)
		{
			Position offset = { 0.0f, 0.0f, 5.0f, 0.0f };
			const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
			GetHitDest()->RelocateOffset(offset);
		}

		void Register() override
		{
			OnEffectLaunch += SpellEffectFn(spell_sylvanas_jump_summon_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_sylvanas_jump_summon_SpellScript();
	}
};

// 100865
class spell_ghoul_calling_of_the_highborne : public SpellScriptLoader
{
public:
	spell_ghoul_calling_of_the_highborne() : SpellScriptLoader("spell_ghoul_calling_of_the_highborne") { }

	class spell_ghoul_calling_of_the_highborne_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_ghoul_calling_of_the_highborne_SpellScript);

		void FilterTargets(std::list<WorldObject*>& targets)
		{
			Unit* caster = GetCaster();
			targets.remove_if([caster](WorldObject* target) {
				return target->GetEntry() != NPC_RISEN_GHOUL;
				});
		}

		void Register() override
		{
			OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ghoul_calling_of_the_highborne_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_ghoul_calling_of_the_highborne_SpellScript();
	}

	class spell_ghoul_calling_of_the_highborne_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_ghoul_calling_of_the_highborne_AuraScript);

		void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
		{
			if (!GetCaster())
				return;

			if (auto t = GetTarget())
				if (auto m = t->GetMap())
				{
					Map::PlayerList const& playerList = m->GetPlayers();
					for (auto i = playerList.begin(); i != playerList.end(); ++i)
						if (auto ghoul_1 = GetCaster())
							if (auto ghoul_2 = GetTarget())
								if (auto sylvanas = ghoul_1->FindNearestCreature(BOSS_ECHO_OF_SYLVANAS, 200.f))
									if (auto target = i->getSource())
										if (!target->isGameMaster())
											if (target->isAlive())
											{
												if (target->IsInBetween(ghoul_2, ghoul_1, 0.01f))
													target->CastSpell(target, SPELL_PAIN_DAMAGE, true);
												else
												{
													if (sylvanas->GetExactDist2d(target) > 30.f)
														if (target->GetPositionZ() > 60.f)
															if (!target->IsJumping())
																target->CastSpell(target, SPELL_PAIN_DAMAGE, true);
												}
											}
				}
		}

		void Register() override
		{
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_ghoul_calling_of_the_highborne_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
		}
	};

	AuraScript* GetAuraScript() const override
	{
		return new spell_ghoul_calling_of_the_highborne_AuraScript();
	}
};

// 105766
class spell_calling_of_the_highborne_dummy : public SpellScriptLoader
{
public:
	spell_calling_of_the_highborne_dummy() : SpellScriptLoader("spell_calling_of_the_highborne_dummy") { }

	class spell_calling_of_the_highborne_dummy_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_calling_of_the_highborne_dummy_AuraScript);

		bool Load()
		{
			events.ScheduleEvent(EVENT_CALLING_OF_THE_HIGHBORNE, 4000);
			return true;
		}

		void OnUpdate(AuraEffect* aurEff, const uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_CALLING_OF_THE_HIGHBORNE:
					if (auto owner = GetUnitOwner())
					{
						int32 rapStack = owner->HasAura(SPELL_SHRINK_TRIGGER) ? owner->GetAura(SPELL_SHRINK_TRIGGER)->GetStackAmount() : 1;
						Map::PlayerList const& playerList = owner->GetMap()->GetPlayers();
						for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
							if (auto target = i->getSource())
								if (target->isAlive())
									if (owner->isInBack(target) && owner->GetDistance2d(target) <= (25.0f - 25.0f * 0.1f * rapStack))
										target->CastSpell(target, SPELL_PAIN_DAMAGE, true);
					}
					events.ScheduleEvent(EVENT_CALLING_OF_THE_HIGHBORNE, 1000);
					break;
				}
			}
		}

		void Register() override
		{
			OnEffectUpdate += AuraEffectUpdateFn(spell_calling_of_the_highborne_dummy_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
		}
	};

	AuraScript* GetAuraScript() const override
	{
		return new spell_calling_of_the_highborne_dummy_AuraScript();
	}
};

// 101549
class spell_blighted_arrow_dummy : public SpellScriptLoader
{
public:
	spell_blighted_arrow_dummy() : SpellScriptLoader("spell_blighted_arrow_dummy") { }

	class spell_blighted_arrow_dummy_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_blighted_arrow_dummy_SpellScript);

		void HandleDummy(SpellEffIndex /*effIndex*/)
		{
			if (auto target = GetHitUnit())
			{
				Position startPos;
				// 8M behind the player
				target->GetNearPosition(startPos, 8.00f, M_PI);
				for (uint8 i = 0; i < 5; i++)
				{
					if (i > 0)
						target->MovePosition(startPos, 4.00f, 0.00f);
					target->CastSpell(startPos.GetPositionX(), startPos.GetPositionY(), startPos.GetPositionZ(), SPELL_PLAYERS_SUMMON_BLIGHTED_ARROW, true);
				}
			}
		}

		void Register() override
		{
			OnEffectHitTarget += SpellEffectFn(spell_blighted_arrow_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_blighted_arrow_dummy_SpellScript();
	}
};

// 101401
class spell_blighted_arrow_morph : public SpellScriptLoader
{
public:
	spell_blighted_arrow_morph() : SpellScriptLoader("spell_blighted_arrow_morph") { }

	class spell_blighted_arrow_morph_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_blighted_arrow_morph_AuraScript);

		void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			GetTarget()->RemoveAurasDueToSpell(SPELL_BLIGHTED_ARROW_AURA);
			GetTarget()->CastSpell(GetTarget(), SPELL_BLIGHTED_ARROW_KNOCKBACK, true);
			if (GetTarget()->ToCreature())
				GetTarget()->ToCreature()->DespawnOrUnsummon(2000);
		}

		void Register() override
		{
			AfterEffectApply += AuraEffectApplyFn(spell_blighted_arrow_morph_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const override
	{
		return new spell_blighted_arrow_morph_AuraScript();
	}
};

class spell_blighted_arrow_distance_selector : public SpellScriptLoader
{
public:
	spell_blighted_arrow_distance_selector() : SpellScriptLoader("spell_blighted_arrow_distance_selector") { }

	class spell_blighted_arrow_distance_selector_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_blighted_arrow_distance_selector_SpellScript);

		void FilterTarget(std::list<WorldObject*>& targets)
		{
			Unit* caster = GetCaster();
			targets.remove_if([caster](WorldObject* target) {
				return caster->GetExactDist2d(target->GetPositionX(), target->GetPositionY()) > 3.00f;
				});
		}

		void Register() override
		{
			OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blighted_arrow_distance_selector_SpellScript::FilterTarget, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENEMY);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_blighted_arrow_distance_selector_SpellScript();
	}
};

class spell_echo_of_sylvanas_seeping_shadows : public SpellScriptLoader
{
public:
	spell_echo_of_sylvanas_seeping_shadows() : SpellScriptLoader("spell_echo_of_sylvanas_seeping_shadows") { }

	class spell_echo_of_sylvanas_seeping_shadows_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_echo_of_sylvanas_seeping_shadows_AuraScript);

		void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
		{
			if (!GetCaster())
				return;

			int32 amount = int32(0.2f * (100.0f - GetCaster()->GetHealthPct()));

			if (Aura* aur = GetCaster()->GetAura(SPELL_SEEPING_SHADOWS_AURA))
				aur->ModStackAmount(amount - aur->GetStackAmount());
			else
				GetCaster()->CastCustomSpell(SPELL_SEEPING_SHADOWS_AURA, SPELLVALUE_AURA_STACK, amount, GetCaster(), true);

		}

		void Register()
		{
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_echo_of_sylvanas_seeping_shadows_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);


		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_echo_of_sylvanas_seeping_shadows_AuraScript();
	}
};

class achievement_severed_ties : public AchievementCriteriaScript
{
public:
	achievement_severed_ties() : AchievementCriteriaScript("achievement_severed_ties") { }

	bool OnCheck(Player* source, Unit* /*target*/) override
	{
		if (auto instance = source->GetInstanceScript())
			if (auto sylvanas = ObjectAccessor::GetCreature(*source, instance->GetData64(DATA_ECHO_OF_SYLVANAS_GUID)))
				return sylvanas->AI()->GetData(DATA_ACHIEVEMENT_CHECK) == 1;
		return false;
	}
};

void AddSC_boss_echo_of_sylvanas()
{
	new boss_echo_of_sylvanas();
	new npc_sylvanas_risen_ghoul();
	new npc_sylvanas_blighted_arrow();
	new spell_echo_of_sylvanas_ghoul_summon_sc();
	new spell_echo_of_sylvanas_death_grip_sc();
	new spell_sylvanas_jump_summon();
	new spell_ghoul_calling_of_the_highborne();
	new spell_calling_of_the_highborne_dummy();
	new spell_blighted_arrow_dummy();
	new spell_blighted_arrow_morph();
	new spell_blighted_arrow_distance_selector();
	new spell_echo_of_sylvanas_seeping_shadows();
	new achievement_severed_ties();
}