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

#include "MoveSplineInit.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "the_stonecore.h"

enum Spells
{
	SPELL_CURSE_OF_BLOOD = 79345,
	SPELL_FORCE_GRIP = 79351,
	SPELL_FORCE_GRIP_CHANGE_SEAT = 79359,
	SPELL_FORCE_GRIP_DAMAGE = 79358,
	SPELL_SUMMON_GRAVITY_WELL = 79340,

	SPELL_SEISMIC_SHARD = 79002, // visual
	SPELL_SEISMIC_SHARD_CHARGE = 79014, // damage + leap
	SPELL_SEISMIC_SHARD_PULL = 86862, // pulls the shard -> makes it enter vehicle
	SPELL_SEISMIC_SHARD_TAR = 80511, // target visual
	SPELL_SEISMIC_SHARD_THROW = 79015, // throw visual
	SPELL_SEISMIC_SHARD_SUMM_1 = 86856, // summons shards
	SPELL_SEISMIC_SHARD_SUMM_2 = 86858,
	SPELL_SEISMIC_SHARD_SUMM_3 = 86860,
	SPELL_SEISMIC_SHARD_VISUAL = 79009,

	SPELL_ENERGY_SHIELD = 79050,
	SPELL_ENERGY_SHIELD_HC = 92667,

	SPELL_GRAVITY_WELL_VIS_1 = 79245, // after 3 sec - removed
	SPELL_GRAVITY_WELL_PERIODIC = 79244,
	SPELL_GRAVITY_WELL_SCRIPT = 79251,
	SPELL_GRAVITY_WELL_DMG = 79249,
	SPELL_GRAVITY_WELL_PULL = 79333,
	SPELL_GRAVITY_WELL_SCALE = 92475, // hc only

	SPELL_RIDE_VEHICLE = 46598,

	SPELL_VISUAL_AZIL = 85654,
	SPELL_INTRO_IMMUNE = 82858
};

enum Events
{
	EVENT_CURSE_OF_BLOOD = 1,
	EVENT_FORCE_GRIP,
	EVENT_FORCE_GRIP_SMASH,
	EVENT_FORCE_GRIP_DAMAGE,
	EVENT_REPEAT_FORCE_GRIP_SMASH,
	EVENT_SEISMIC_SHARD_PRE,
	EVENT_SEISMIC_SHARD,
	EVENT_SEISMIC_SHARD_THROW,
	EVENT_SHIELD_PHASE_END,
	EVENT_GRAVITY_WELL,
	EVENT_ENERGY_SHIELD,
	EVENT_ENERGY_SHIELD_END,
	EVENT_ADDS_SUMMON,
	EVENT_INTRO
};

enum Phases
{
	PHASE_NORMAL = 1,
	PHASE_SHIELD
};

enum Misc
{
	VEHICLE_GRIP = 892,
	VEHICLE_NORMAL = 903,
	POINT_FLY = 1,
	POINT_PLATFORM = 2,
	POINT_INTRO = 3
};

enum Quotes
{
	SAY_AGGRO,
	SAY_DEATH,
	SAY_SLAY,
	SAY_SHIELD
};

static const Position summonPos[2] =
{
	{1271.93f, 1042.73f, 210.0f, 0.0f}, // W
	{1250.99f, 949.48f, 205.5f, 0.0f}   // E
};

class boss_azil : public CreatureScript
{
public:
	boss_azil() : CreatureScript("boss_priestess_azil") { }

	struct boss_azilAI : public BossAI
	{
		boss_azilAI(Creature* creature) : BossAI(creature, DATA_HIGH_PRIESTESS_AZIL) { ForceGripInProgress = false; }

		void Reset()
		{
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			me->AddAura(SPELL_VISUAL_AZIL, me);
			seismicShards = 0;
			me->SetReactState(REACT_AGGRESSIVE);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
			shardGUID[0] = 0;
			shardGUID[1] = 0;
			shardGUID[2] = 0;
			c = 0;
			gripGUID = 0;
			_Reset();
		}

		void EnterCombat(Unit * /*victim*/)
		{
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Remove
			Talk(SAY_AGGRO);
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->CastSpell(me, SPELL_INTRO_IMMUNE);
			events.SetPhase(PHASE_NORMAL);
			events.ScheduleEvent(EVENT_INTRO, 0);
			events.ScheduleEvent(EVENT_FORCE_GRIP, 10000);
			events.ScheduleEvent(EVENT_ENERGY_SHIELD, 45000);
			events.ScheduleEvent(EVENT_CURSE_OF_BLOOD, urand(5000, 8000));
			events.ScheduleEvent(EVENT_GRAVITY_WELL, urand(3000, 5000));
			events.ScheduleEvent(EVENT_ADDS_SUMMON, urand(10000, 15000));
			_EnterCombat();
		}

		void SpellHit(Unit * /*caster*/, const SpellInfo *spell)
		{
			Spell * curSpell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL);
			if (curSpell && curSpell->m_spellInfo->Id == SPELL_FORCE_GRIP)
				if (spell->HasEffect(SPELL_EFFECT_INTERRUPT_CAST))
					me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
		}

		void JustSummoned(Creature * summon)
		{
			switch (summon->GetEntry())
			{
			case 42355:
				summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
				summon->SetReactState(REACT_PASSIVE);
				summon->CastSpell(summon, SPELL_SEISMIC_SHARD_VISUAL, true);
				summon->GetMotionMaster()->MovePoint(0, summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ() + 30.0f);
				shardGUID[c] = summon->GetGUID();
				c++;
				if (c >= 3)
					c = 0;
				break;
			}
			BossAI::JustSummoned(summon);
		}

		void KilledUnit(Unit * victim)
		{
			if (victim->GetTypeId() == TYPEID_PLAYER)
				Talk(SAY_SLAY);
		}

		void JustDied(Unit * /*killer*/)
		{
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			Talk(SAY_DEATH);
			_JustDied();
			if (IsHeroic())
				instance->FinishLfgDungeon(me);
		}

		void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
		{
			if (who->GetTypeId() != TYPEID_PLAYER)
				return;

			if (!apply)
				return;

			if (seatId == 1)
			{
				events.ScheduleEvent(EVENT_FORCE_GRIP_SMASH, 800);
				ForceGripInProgress = true;
			}
			else if (seatId == 2)
			{
				if (ForceGripInProgress)
					events.ScheduleEvent(EVENT_FORCE_GRIP_DAMAGE, 800);
				else
					who->ExitVehicle();
			}
		}

		void MovementInform(uint32 type, uint32 id)
		{
			if (type != EFFECT_MOTION_TYPE)
				return;

			switch (id)
			{
			case POINT_PLATFORM:
			{
				events.ScheduleEvent(EVENT_SHIELD_PHASE_END, 30000);
				Movement::MoveSplineInit init(me);
				init.SetFacing(me->GetHomePosition().GetOrientation());
				DoCast(SPELL_SEISMIC_SHARD_SUMM_1);
				DoCast(SPELL_SEISMIC_SHARD_SUMM_2);
				DoCast(SPELL_SEISMIC_SHARD_SUMM_3);
				DoCast(SPELL_SEISMIC_SHARD);
				events.ScheduleEvent(EVENT_SEISMIC_SHARD_PRE, 5000);
				break;
			}
			case POINT_INTRO:
			{
				if (!UpdateVictim())
				if (auto p = me->FindNearestPlayer(100.f))
					me->AI()->AttackStart(p);
				me->SetReactState(REACT_AGGRESSIVE);
				me->RemoveAurasDueToSpell(SPELL_INTRO_IMMUNE);
				break;
			}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING) && !ForceGripInProgress)
				return;

			if (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_INTRO:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->GetMotionMaster()->MoveJump(1332.59f, 983.41f, 207.62f, 10.0f, 10.0f, POINT_INTRO);
					break;
				case EVENT_CURSE_OF_BLOOD:
					DoCastVictim(SPELL_CURSE_OF_BLOOD);
					events.ScheduleEvent(EVENT_CURSE_OF_BLOOD, urand(8000, 10000), 0, PHASE_NORMAL);
					break;
				case EVENT_FORCE_GRIP:
					DoCastVictim(SPELL_FORCE_GRIP);
					events.ScheduleEvent(EVENT_FORCE_GRIP, urand(15000, 20000), 0, PHASE_NORMAL);
					break;
				case EVENT_GRAVITY_WELL:
					DoCastRandom(SPELL_SUMMON_GRAVITY_WELL, 100.0f, false);
					events.ScheduleEvent(EVENT_GRAVITY_WELL, urand(15000, 20000), 0, PHASE_NORMAL);
					break;
				case EVENT_SEISMIC_SHARD_PRE:
				{
					if (Creature *shard = Unit::GetCreature(*me, shardGUID[seismicShards]))
					{
						if (seismicShards != 0)
							shard->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, seismicShards + 1 + 2, me, true);
						else
							shard->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, seismicShards + 1, me, true);
					}
					if (seismicShards < 2)
					{
						seismicShards++;
						events.ScheduleEvent(EVENT_SEISMIC_SHARD_PRE, 500);
					}
					else
					{
						seismicShards = 0;
						events.ScheduleEvent(EVENT_SEISMIC_SHARD, 4000);
					}
					break;
				}
				case EVENT_SEISMIC_SHARD:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
					{
						target->GetPosition(&shardPos);
						DoCast(SPELL_SEISMIC_SHARD_PULL);
						me->CastSpell(shardPos.GetPositionX(), shardPos.GetPositionY(), shardPos.GetPositionZ(), SPELL_SEISMIC_SHARD_TAR, false);
						DoCast(SPELL_SEISMIC_SHARD_THROW);
						events.ScheduleEvent(EVENT_SEISMIC_SHARD_THROW, 3000);
					}
					if (seismicShards < 2)
						events.ScheduleEvent(EVENT_SEISMIC_SHARD, 7000);
					break;
				case EVENT_SEISMIC_SHARD_THROW:
				{
					if (Unit * passenger = Unit::GetCreature(*me, shardGUID[seismicShards]))
					{
						passenger->ExitVehicle();
						passenger->CastSpell(shardPos.GetPositionX(), shardPos.GetPositionY(), shardPos.GetPositionZ(), SPELL_SEISMIC_SHARD_CHARGE, false);
					}
					++seismicShards;
					break;
				}
				case EVENT_ENERGY_SHIELD:
					me->SetReactState(REACT_PASSIVE);
					me->GetMotionMaster()->Clear();
					me->GetMotionMaster()->MoveIdle();
					me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
					Talk(SAY_SHIELD);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->CreateVehicleKit(VEHICLE_NORMAL, me->GetEntry());
					events.SetPhase(PHASE_SHIELD);
					DoCast(IsHeroic() ? SPELL_ENERGY_SHIELD_HC : SPELL_ENERGY_SHIELD);
					seismicShards = 0;
					events.ScheduleEvent(EVENT_ENERGY_SHIELD_END, 4000);
					break;
				case EVENT_ENERGY_SHIELD_END: // fly up
					me->GetMotionMaster()->MoveJump(me->GetHomePosition(), 12.0f, 15.0f, POINT_PLATFORM);
					break;
				case EVENT_SHIELD_PHASE_END:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->RemoveAurasDueToSpell(SPELL_SEISMIC_SHARD);
					me->RemoveAura(IsHeroic() ? SPELL_ENERGY_SHIELD_HC : SPELL_ENERGY_SHIELD);
					me->SetReactState(REACT_AGGRESSIVE);
					/*
					if (Unit * victim = me->getVictim())
					{
						DoStartMovement(victim);
					}
					*/
					events.ScheduleEvent(EVENT_INTRO, 1);
					events.SetPhase(PHASE_NORMAL);
					events.RescheduleEvent(EVENT_CURSE_OF_BLOOD, urand(8000, 10000), 0, PHASE_NORMAL);
					events.RescheduleEvent(EVENT_GRAVITY_WELL, urand(15000, 20000), 0, PHASE_NORMAL);
					events.RescheduleEvent(EVENT_FORCE_GRIP, urand(10000, 12000), 0, PHASE_NORMAL);
					events.ScheduleEvent(EVENT_ENERGY_SHIELD, urand(40000, 45000), 0, PHASE_NORMAL);
					break;
				case EVENT_ADDS_SUMMON:
				{
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM))
					{
						uint8 amount = 3;
						uint8 pos = urand(0, 1);
						if (events.IsInPhase(PHASE_SHIELD))
							amount = urand(8, 10);
						for (int i = 0; i < amount; ++i)
						{
							Position tarPos;
							me->GetRandomPoint(summonPos[pos], 5.0f, tarPos);
							if (Creature * summon = me->SummonCreature(NPC_FOLLOWER, tarPos, TEMPSUMMON_DEAD_DESPAWN, 1000))
							{
								summon->AI()->AttackStart(target);
								summon->AI()->DoZoneInCombat();
							}
						}
					}
					events.ScheduleEvent(EVENT_ADDS_SUMMON, urand(10000, 12000));
				}
				break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
			if (!me->HasReactState(REACT_PASSIVE))
				if (UpdateVictim())
			DoMeleeAttackIfReady();
		}
	private:
		bool first;
		uint8 seismicShards;
		Position shardPos;
		uint64 shardGUID[3];
		int c;
		uint64 gripGUID;
		bool ForceGripInProgress;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_azilAI(creature);
	}
};

class npc_gravity_well_azil : public CreatureScript
{
public:
	npc_gravity_well_azil() : CreatureScript("npc_gravity_well_azil") {}

	struct npc_gravity_well_azilAI : public ScriptedAI
	{
		npc_gravity_well_azilAI(Creature * creature) : ScriptedAI(creature) {}

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
			DoCast(SPELL_GRAVITY_WELL_VIS_1);
			active = false;
			activeTimer = 3000;
			if (!IsHeroic())
				me->DespawnOrUnsummon(20000);
			killCount = 0;
			me->ApplySpellImmune(79251, IMMUNITY_ID, 79251, true);
			me->ApplySpellImmune(79332, IMMUNITY_ID, 79332, true);
		}

		void SpellHitTarget(Unit * target, const SpellInfo * spell)
		{
			if (target->isAlive() && spell->Id == SPELL_GRAVITY_WELL_SCRIPT)
			{
				int bp = IsHeroic() ? 20000 : 10000; // evtl needs to be increased / lowered
				uint32 distFkt = uint32(me->GetDistance(target)) * 5;
				bp -= (bp * distFkt) / 100;

				me->CastCustomSpell(target, SPELL_GRAVITY_WELL_DMG, &bp, NULL, NULL, true);
			}
		}

		void KilledUnit(Unit * victim)
		{
			if (IsHeroic() && victim->GetEntry() == NPC_FOLLOWER)
			{
				if (killCount == 3)
					me->DespawnOrUnsummon();
				else
				{
					DoCast(me, SPELL_GRAVITY_WELL_SCALE, true);
					++killCount;
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			if (!active)
			{
				if (activeTimer <= diff)
				{
					active = true;
					me->RemoveAurasDueToSpell(SPELL_GRAVITY_WELL_VIS_1);
					DoCast(me, SPELL_GRAVITY_WELL_PERIODIC, true);
					DoCast(me, SPELL_GRAVITY_WELL_PULL, true);
					activeTimer = 8000;
				}
				else activeTimer -= diff;
			}
		}

	private:
		bool active;
		uint32 activeTimer;
		uint8 killCount;
	};

	CreatureAI * GetAI(Creature * creature) const
	{
		return new npc_gravity_well_azilAI(creature);
	}
};

// force grip 79351
class spell_stcore_force_grip : public SpellScriptLoader
{
public:
    spell_stcore_force_grip() : SpellScriptLoader("spell_stcore_force_grip") {}

    class spell_stcore_force_grip_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_stcore_force_grip_SpellScript);


		void HandleScript(SpellEffIndex effIndex)
		{
			if (auto caster = GetCaster())
			{
				if (auto target = GetHitUnit())
				{
					Position spawnPos;
					caster->GetPosition(&spawnPos);
					caster->GetNearPoint2D(spawnPos.m_positionX, spawnPos.m_positionY, 5.0f, caster->GetOrientation());
					if (auto veh = caster->SummonCreature(987656, spawnPos.m_positionX, spawnPos.m_positionY, spawnPos.m_positionZ, caster->GetOrientation() + M_PI))
					{
						target->SetDisableGravity(true);
						target->SendMovementDisableGravity(); 
						target->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 3, veh, false);
					}
				}
			}
		}

        void Register()
        {
			OnEffectHitTarget += SpellEffectFn(spell_stcore_force_grip_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_stcore_force_grip_SpellScript();
    }

	class spell_stcore_force_grip_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_stcore_force_grip_AuraScript);

		void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			if (auto target = GetTarget())
			{
				if (Vehicle* veh = target->GetVehicle())
					veh->RemoveAllPassengers();
			}
		}

		void Register()
		{
			OnEffectRemove += AuraEffectRemoveFn(spell_stcore_force_grip_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_stcore_force_grip_AuraScript();
	}
};

class spell_stcore_force_grip_change_seat : public SpellScriptLoader
{
public:
	spell_stcore_force_grip_change_seat() : SpellScriptLoader("spell_stcore_force_grip_change_seat") {}

	class spell_stcore_force_grip_change_seat_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_stcore_force_grip_change_seat_SpellScript);

		void HandleScript(SpellEffIndex effIndex)
		{
			if (auto caster = GetCaster())
				if (auto target = GetHitUnit())
					target->CastSpell(caster, GetSpellInfo()->Effects[effIndex].TriggerSpell, true);
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_stcore_force_grip_change_seat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_stcore_force_grip_change_seat_SpellScript();
	}
};

// custom NPC for workaround
class npc_force_grip_veh : public CreatureScript
{
public:
	npc_force_grip_veh() : CreatureScript("npc_force_grip_veh") { }

	struct npc_force_grip_vehAI : public ScriptedAI
	{
		npc_force_grip_vehAI(Creature* creature) : ScriptedAI(creature)
		{
			instance = me->GetInstanceScript();
		}

		class TimedMove : public BasicEvent
		{
		public:
			TimedMove(Unit* owner, Position const& pos, uint32 point, float speed) : _owner(owner), Pos(pos), pointId(point), _speed(speed)
			{
			}

			bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
			{
				_owner->GetMotionMaster()->MovePoint(pointId, Pos, false, _speed);
				return true;
			}

		private:
			Unit* _owner;
			Position Pos;
			uint32 pointId;
			float _speed;
		};

		void PassengerBoarded(Unit* player, int8 seatId, bool apply)
		{
			if (!apply)
			{
				player->SetDisableGravity(false);
				player->SendMovementDisableGravity();
				me->DespawnOrUnsummon(1000);
				return;
			}
			player->SetDisableGravity(true);
			player->SendMovementDisableGravity();
			Position pos;
			me->GetPosition(&pos);
			me->GetPosition(&originPos);
			originPos.m_positionZ += 0.5f;
			pos.m_positionZ = me->GetPositionZ() + 5.f;
			airPos = pos;
			me->GetMotionMaster()->MovePoint(1, airPos, false, 6.0f);
		}

		void MovementInform(uint32 type, uint32 point)
		{
			if (type != POINT_MOTION_TYPE)
				return;

			if (point == 1)
				me->m_Events.AddEvent(new TimedMove(me, originPos, 2, 24.f), me->m_Events.CalculateTime(800));

			if (point == 2)
			{
				if (auto passenger = me->GetVehicleKit()->GetPassenger(2))
					if(Creature* azil = me->FindNearestCreature(BOSS_HIGH_PRIESTESS_AZIL, 100.f))
						azil->CastSpell(passenger, SPELL_FORCE_GRIP_DAMAGE, true);
				
				me->m_Events.AddEvent(new TimedMove(me, airPos, 1, 6.0f), me->m_Events.CalculateTime(800));
			}	
		}

	private:
		InstanceScript* instance;
		Position originPos;
		Position airPos;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_force_grip_vehAI(creature);
	}
};

class npc_azil_devout_follower : public CreatureScript
{
public:
	npc_azil_devout_follower() : CreatureScript("npc_azil_devout_follower") { }


	struct npc_azil_devout_followerAI : public ScriptedAI
	{

		npc_azil_devout_followerAI(Creature* creature) : ScriptedAI(creature)
		{
		}

		void Reset()
		{

			if (ShouldKneel())
			if (!me->HasAura(68442))
				me->AddAura(68442, me);
		}

		void JustReachedHome()
		{

			if (ShouldKneel())
			if (!me->HasAura(68442))
			me->AddAura(68442, me);
		}
		void EnterCombat(Unit* /*who*/)
		{
			me->RemoveAura(68442);
		}

		void InitializeAI()
		{
			if (ShouldKneel())
			if (!me->HasAura(68442))
				me->AddAura(68442, me);
		}

		void JustDied(Unit* who)
		{
			me->RemoveAura(68442);
			me->DeleteThreatList();
			events.Reset();
			summons.DespawnAll();
		}

		bool ShouldKneel()
		{
			Position azil_spawn{ 1337.073364f, 964.726196f, 214.183334f, 1.682089f };

			if (me->GetDistance(azil_spawn) <= 40.f)
				if (me->GetDBTableGUIDLow() > 0)
					return true;
				return false;
		}

		void UpdateAI(const uint32 diff) override
		{
				if (auto who = me->FindNearestPlayer(10.f))
					if (me->IsValidAttackTarget(who))
						if (!me->isInCombat())
						{
							me->RemoveAura(68442);
							AttackStart(who);
							me->CallForHelp(5.f);
						}
							//  Enable for Melee-using npcs only
							if (me->HasReactState(REACT_AGGRESSIVE))
								if (!me->HasUnitState(UNIT_STATE_CASTING))
									if (UpdateVictim())
										DoMeleeAttackIfReady();
		}
	private:
		InstanceScript* instance{ me->GetInstanceScript() };
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_azil_devout_followerAI(creature);
	}
};

void AddSC_boss_azil()
{
	new boss_azil();
	new npc_gravity_well_azil();
	new spell_stcore_force_grip();
	new spell_stcore_force_grip_change_seat();
	new npc_force_grip_veh();
	new npc_azil_devout_follower();
}
