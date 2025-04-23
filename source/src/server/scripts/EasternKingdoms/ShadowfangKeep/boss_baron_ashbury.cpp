/*
* ToDo:
* Find the correct Sound Texts from DBC's
* Find a better Way for his Archangel Cast
*/

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "shadowfang_keep.h"
#include "Unit.h"

enum Spells
{
	// Normal Mode
	SPELL_ASPHYXIATE = 93423,
	SPELL_ASPHYXIATE_ROOT = 93422,
	SPELL_ASPHYXIATE_DAMAGE = 93424,
	SPELL_PAIN_AND_SUFFERING = 93581,
	SPELL_PAIN_AND_SUFFERING_HC = 93712,
	SPELL_STAY_OF_EXECUTION = 93468,
	SPELL_STAY_OF_EXECUTION_HC = 93705,

	// Heroic Mode
	SPELL_DARK_ARCHANGEL_FORM = 93757,
	SPELL_CALAMITY = 93812,
	SPELL_WRACKING_PAIN = 93720
};

enum gates
{
	GOBJ_PRISONER_GATE		= 18934,
	GOBJ_ALLI_MASTIFF_GATE	= 18935,
	GOBJ_HORDE_GUARD_GATE	= 18936,
};

enum Events
{
	EVENT_ASPHYXIATE = 1,
	EVENT_PAIN_AND_SUFFERING,
	EVENT_STAY_OF_EXECUTION,
	EVENT_WRACKING_PAIN,
	EVENT_SPAWN_PRISONER,
	EVENT_PRISONER_SHITTALK,
	EVENT_HARRASS_PRISONER,
	EVENT_WIPE_CHECK,
};

enum Texts
{
	SAY_AGGRO,
	SAY_KILLEDPLAYER,			//not actually used.
	SAY_STAY_OF_EXE_ANNOUNCE,
	SAY_STAY_OF_EXE_GOSSIP,
	SAY_ARCHANGEL,
	SAY_JUSTDIED,
	SAY_ASPHY,
	SAY_HARASS_HORDE,
	SAY_HARASS_ALLIANCE,

	PRISONER_SAY_SHITTALK			= 0,
	PRISONER_SAY_HARASS_RESPONSE	= 1,
};

const G3D::Vector3 Prisoner_Escape[]
{
	{-255.288025f, 2116.147461f, 81.179672f},
	{-255.917358f, 2120.336426f, 81.179787f},
	{-254.258530f, 2129.532471f, 81.179787f},
	{-250.280029f, 2142.750244f, 87.188080f},
	{-248.019272f, 2144.078857f, 87.012917f},
	{-240.480164f, 2140.677002f, 87.012871f},
	{-233.327057f, 2148.801025f, 91.331093f},
	{-211.455780f, 2140.344238f, 90.625214f},
};

class boss_baron_ashbury : public CreatureScript
{
public:
	boss_baron_ashbury() : CreatureScript("boss_baron_ashbury") { }

	struct boss_baron_ashburyAI : public BossAI
	{
		boss_baron_ashburyAI(Creature* creature) : BossAI(creature, DATA_BARON_ASHBURY_EVENT) { }

		void Reset()
		{
			_Reset();
			me->SetReactState(REACT_AGGRESSIVE);
			enraged = false;
			me->RemoveAllAuras();
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WRACKING_PAIN);
			mui_checkWipe = 2000;
			events.ScheduleEvent(EVENT_SPAWN_PRISONER, 5000);
			events.ScheduleEvent(EVENT_HARRASS_PRISONER, 25000);
		}

		void wipe_reset()
		{
			me->DeleteThreatList();
			me->CombatStop(true);
			Reset();
			me->Respawn(true);
		}

		bool checkWipe()
		{
			auto players = me->GetPlayersInRange(200.f, true);
			for (Player* player : players)
				if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
					return false;
			return true;
		}

		void EnterCombat(Unit* /*who*/)
		{
			_EnterCombat();
			Talk(SAY_AGGRO);
			events.Reset();
			events.ScheduleEvent(EVENT_PAIN_AND_SUFFERING, 4000);
			events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
			pain_and_suffering_reset = true;//if true, this is the final pain and suffering in the rotation and asphyxiate needs to be scheduled next.
		}

		void JustDied(Unit* /*killer*/)
		{
			_JustDied();
			events.Reset();
			Talk(SAY_JUSTDIED);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DARK_ARCHANGEL_FORM);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WRACKING_PAIN);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CALAMITY);

			if (auto gate1 = me->FindNearestGameObject(GOBJ_PRISONER_GATE, 50.f))
				gate1->SetGoState(GO_STATE_ACTIVE);
			if (auto gate2 = me->FindNearestGameObject((instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? GOBJ_HORDE_GUARD_GATE : GOBJ_ALLI_MASTIFF_GATE), 50.f))
				gate2->SetGoState(GO_STATE_ACTIVE);

			if (auto c = me->FindNearestCreature((instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_FORSAKEN_TROOPER : NPC_MAGISTRATE_HALEB), 40.f, true))
			{
				c->GetMotionMaster()->MoveSmoothPath(Prisoner_Escape, 8, 5.f);
				c->DespawnOrUnsummon(15000);
			}
		}

		void KilledUnit(Unit* victim)
		{
			//Talk(SAY_KILLEDPLAYER);
		}
		void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
		{
			switch (spell->Id)
			{
			case SPELL_ASPHYXIATE:
			case 93710://asphyxiate hc
				if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
				{
					if (!events.HasEvent(EVENT_STAY_OF_EXECUTION))
						events.ScheduleEvent(EVENT_STAY_OF_EXECUTION, 1);
					else
						events.RescheduleEvent(EVENT_STAY_OF_EXECUTION, 1);
				}
				break;
			case SPELL_STAY_OF_EXECUTION:
			case 93705: //stay of execution hc
				if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
				{
					me->SetReactState(REACT_AGGRESSIVE);
					UpdateVictim();
					if (!events.HasEvent(EVENT_PAIN_AND_SUFFERING))
						events.ScheduleEvent(EVENT_PAIN_AND_SUFFERING, 10000);
					else
						events.RescheduleEvent(EVENT_PAIN_AND_SUFFERING, 10000);
				}
				break;
			case SPELL_PAIN_AND_SUFFERING:
			case 93712://pain and suffering hc
				if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
				{
					events.ScheduleEvent(EVENT_WRACKING_PAIN, 6100);
					me->SetReactState(REACT_AGGRESSIVE);
					UpdateVictim();
				}
				break;
			}
		}
		void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
		{
			if (IsHeroic())
				if (!enraged && HealthBelowPct(25))
				{
					me->CastStop();
					me->ClearUnitState(UNIT_STATE_CASTING);
					Talk(SAY_ARCHANGEL);
					if (auto wing = me->SummonCreature(50604, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
					{
						wing->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, me, false);
						wing->CastSpell(me, 93766, false);
						DoCast(me, SPELL_DARK_ARCHANGEL_FORM);
						enraged = true;
					}					
				}
		}

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
		{
			/*
			for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
				if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST || spell->Effects[i].Mechanic == MECHANIC_INTERRUPT)
					me->SetReactState(REACT_AGGRESSIVE);
			*/
		}

		bool check_freeze()
		{
			if (me->HasReactState(REACT_PASSIVE))
				if (!me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
					if (!me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
						if (!me->HasUnitState(UNIT_STATE_CASTING))
							return true;
			return false;
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);
			while (uint32 eventId = events.ExecuteEvent())
				switch (eventId)
				{
				case EVENT_WIPE_CHECK:
					if (me->isInCombat())
					{
						if (checkWipe())
						{
							wipe_reset();
						}
						else if (check_freeze())
						{
							me->SetReactState(REACT_AGGRESSIVE);
							UpdateVictim();
						}
					}
					events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
					break;
				case EVENT_HARRASS_PRISONER:
					if (me->isAlive())
					if (auto c = me->FindNearestCreature((instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_FORSAKEN_TROOPER : NPC_MAGISTRATE_HALEB), 40.f, true))
					{
						Talk(c->GetEntry() == NPC_FORSAKEN_TROOPER ? SAY_HARASS_HORDE : SAY_HARASS_ALLIANCE, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
						c->TalkWithDelay(5000, PRISONER_SAY_HARASS_RESPONSE, NULL, CHAT_MSG_MONSTER_SAY);
						events.ScheduleEvent(EVENT_HARRASS_PRISONER, urand(90000, 120000));
					}
					break;
				case EVENT_PRISONER_SHITTALK:
					if (me->isAlive())
					if (me->isInCombat())
					if (auto c = me->FindNearestCreature((instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_FORSAKEN_TROOPER : NPC_MAGISTRATE_HALEB), 40.f, true))
						c->AI()->Talk(PRISONER_SAY_SHITTALK);
					events.ScheduleEvent(EVENT_PRISONER_SHITTALK, urand(15000, 30000));
					break;
				case EVENT_SPAWN_PRISONER:
					if (!instance->GetData64(DATA_ASHBURY_PRISONER))
						if (auto prisoner = instance->instance->SummonCreature((instance->GetData(uint32(TEAM_IN_INSTANCE)) == uint32(HORDE) ? NPC_FORSAKEN_TROOPER : NPC_MAGISTRATE_HALEB), baron_prisoner_spawn))
						{
							//TC_LOG_ERROR("sql.sql", "team in instance: %u triggering spawn of npc %u", uint32(instance->GetData(TEAM_IN_INSTANCE)), prisoner->GetEntry());
							prisoner->setFaction(35);
							prisoner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
						}
					break;
				case EVENT_ASPHYXIATE:
					if (me->HasUnitState(UNIT_STATE_CASTING))
						events.ScheduleEvent(eventId, 250);
					else
					{
						//TC_LOG_ERROR("sql.sql", "EVENT_ASPHYXIATE");
						me->SetReactState(REACT_PASSIVE);
						me->AttackStop();
						me->StopMoving();
						Talk(SAY_ASPHY);
						DoCast(SPELL_ASPHYXIATE);
					}
					break;
				case EVENT_STAY_OF_EXECUTION:
					if (me->HasUnitState(UNIT_STATE_CASTING))
						events.ScheduleEvent(eventId, 250);
					else
					{
						//TC_LOG_ERROR("sql.sql", "EVENT_STAY_OF_EXECUTION");
						Talk(SAY_STAY_OF_EXE_ANNOUNCE);
						Talk(SAY_STAY_OF_EXE_GOSSIP);
						me->CastSpell(me, SPELL_STAY_OF_EXECUTION, false);
					}
					break;

				case EVENT_PAIN_AND_SUFFERING:
					if (me->HasUnitState(UNIT_STATE_CASTING))
						events.ScheduleEvent(eventId, 250);
					else
					{
						//TC_LOG_ERROR("sql.sql", "EVENT_PAIN_AND_SUFFERING");
						me->AttackStop();
						me->SetReactState(REACT_PASSIVE);
						me->StopMoving();
						DoCastRandom(SPELL_PAIN_AND_SUFFERING, 50.f, false);
					}
					break;
				case EVENT_WRACKING_PAIN:
					if (me->HasUnitState(UNIT_STATE_CASTING))
						events.ScheduleEvent(eventId, 250);
					else
					{
						auto players = me->GetPlayersInRange(60.f, true);
						Player* selection{ nullptr };
						if (auto meUnit = me->ToUnit())
							if (players.size())
								for (Player* player : players)
								{
									if (!selection || urand(0, 1) == 0)
										selection = player;
								}
						if (selection)
						{
							//TC_LOG_ERROR("sql.sql", "EVENT_WRACKING_PAIN on %u", selection->GetGUID());
							//me->AddAura(SPELL_WRACKING_PAIN, selection);
							me->ClearUnitState(UNIT_STATE_CASTING);
							if (IsHeroic())
							DoCast(selection->ToUnit(), SPELL_WRACKING_PAIN, true);
						}
						else
						{
							//TC_LOG_ERROR("sql.sql", "EVENT_WRACKING_PAIN failed to find target");
						}

						events.ScheduleEvent(pain_and_suffering_reset ? EVENT_ASPHYXIATE : EVENT_PAIN_AND_SUFFERING, 6100);
						pain_and_suffering_reset = !pain_and_suffering_reset;
					}
					break;
				}


			if (!enraged)
			if (!me->HasUnitState(UNIT_STATE_CASTING))
			if (me->HasReactState(REACT_AGGRESSIVE))
			if (UpdateVictim())
				DoMeleeAttackIfReady();
		}
	private:
		uint32 most_recent_spell{ 0 };
		bool pain_and_suffering_reset{ true };	//if true, this is the final pain and suffering in the rotation and asphyxiate needs to be scheduled instead.
		Position baron_prisoner_spawn { -247.531174f, 2114.237061f, 81.179703f, 2.690423f };
		bool enraged{false};
		uint32 mui_checkWipe;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_baron_ashburyAI(creature);
	}
};

class spell_ashbury_archangel : public SpellScriptLoader
{
public:
	spell_ashbury_archangel() : SpellScriptLoader("spell_ashbury_archangel") { }

	class spell_ashbury_archangel_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_ashbury_archangel_SpellScript);

		void HandleScript(SpellEffIndex effIndex)
		{
			if (auto target = GetHitUnit())
			{
				target->CastSpell(target, SPELL_CALAMITY, true);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_ashbury_archangel_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_ashbury_archangel_SpellScript();
	}
};


class spell_ashbury_pain_and_suffering : public SpellScriptLoader
{
public:
	spell_ashbury_pain_and_suffering() : SpellScriptLoader("spell_ashbury_pain_and_suffering") { }

	class spell_ashbury_pain_and_suffering_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_ashbury_pain_and_suffering_AuraScript);

		void CalcPeriodic(AuraEffect const* effect, bool& isPeriodic, int32& amplitude)
		{
			isPeriodic = true;
		}

		void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
		{
			int32 bp { aurEff->GetBaseAmount() };	//getamount causes compounding increases
			int32 bp_log = bp;
			AddPct(bp, float(aurEff->GetTickNumber()) * 20.f);
			aurEff->SetAmount(bp);

			//TC_LOG_ERROR("sql.sql", "BP Recalculation from %u to %u", bp_log, bp);
		}

		void HandleOnDispel(DispelInfo* /*dispelData*/)
		{
			if (auto caster = GetCaster())
				if (auto cc = caster->ToCreature())
					if (auto ai = cc->AI())
						ai->OnSpellCastFinished(GetSpellInfo(), SPELL_FINISHED_CANCELED);
		}


		void Register()
		{
			OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_ashbury_pain_and_suffering_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
			OnDispel += AuraDispelFn(spell_ashbury_pain_and_suffering_AuraScript::HandleOnDispel);
		}

	};

	AuraScript* GetAuraScript() const override
	{
		return new spell_ashbury_pain_and_suffering_AuraScript();
	}
};

class spell_ashbury_asphyxiate : public SpellScriptLoader
{
public:
	spell_ashbury_asphyxiate() : SpellScriptLoader("spell_ashbury_asphyxiate") { }

	class spell_ashbury_asphyxiate_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_ashbury_asphyxiate_AuraScript);

		bool Validate(SpellInfo const* /*spellInfo*/) override
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_ASPHYXIATE_ROOT))
				return false;
			return true;
		}

		void HandleTriggerSpell(AuraEffect const* aurEff)
		{
			PreventDefaultAction();

			int32 ticks_remaining{ (int32(GetSpellInfo()->GetMaxTicks()) - int32(aurEff->GetTickNumber())) };
			if (ticks_remaining < 0)
				return;

			if (auto caster = GetCaster())
				if (auto target = GetTarget())
				{
					uint32 triggerSpell = GetSpellInfo()->Effects[EFFECT_2].TriggerSpell;
					uint32 health_goal{ (target->GetMaxHealth() / GetSpellInfo()->GetMaxTicks()) * ticks_remaining };

					if (ticks_remaining == 0)
						health_goal = 1;

					uint32 damage{ 0 };

					if (target->GetHealth() > health_goal)
						damage = target->GetHealth() - health_goal;

					if (damage >= target->GetHealth() || ticks_remaining == 0)
						damage = (target->GetHealth() - 1);

					target->CastCustomSpell(triggerSpell, SPELLVALUE_BASE_POINT0, damage, target, true, nullptr, aurEff, caster->GetGUID());

					if (target->GetHealth() > health_goal)
						target->SetHealth(health_goal);
				}
		}

		void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			//TC_LOG_ERROR("sql.sql", "float script activated by apply.");
			if (auto caster = GetCaster())
				if (auto target = GetTarget())
				{
					caster->CastSpell(target, SPELL_ASPHYXIATE_ROOT, true);
				}
		}

		void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			if (auto caster = GetCaster())
				if (auto target = GetTarget())
					if (target->ToPlayer())
					{
						target->RemoveAura(SPELL_ASPHYXIATE_ROOT, caster->GetGUID());
					}
		}

		void Register() override
		{
			OnEffectApply += AuraEffectApplyFn(spell_ashbury_asphyxiate_AuraScript::HandleOnEffectApply, EFFECT_1, SPELL_AURA_FLY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
			OnEffectRemove += AuraEffectRemoveFn(spell_ashbury_asphyxiate_AuraScript::HandleOnEffectRemove, EFFECT_1, SPELL_AURA_FLY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_ashbury_asphyxiate_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_ashbury_asphyxiate_AuraScript();
	}

	class spell_ashbury_asphyxiate_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_ashbury_asphyxiate_SpellScript);

		void FilterTargets(std::list<WorldObject*>& targets)
		{
			for (auto itr = targets.begin(); itr != targets.end();)
				if (auto target = (*itr))
					if (auto unit = target->ToUnit())
						if (unit->ToPlayer() && unit->isAlive())
							++itr;
						else
							targets.erase(itr++);
		}

		void Register()
		{
			OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ashbury_asphyxiate_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
			OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ashbury_asphyxiate_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
			OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ashbury_asphyxiate_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
		}
	};
	SpellScript* GetSpellScript() const
	{
		return new spell_ashbury_asphyxiate_SpellScript();
	}
};

class spell_ashbury_asphyxiate_move_players : public SpellScriptLoader
{
	/*
		93422
	*/
public:
    spell_ashbury_asphyxiate_move_players() : SpellScriptLoader("spell_ashbury_asphyxiate_move_players") { };
    class spell_ashbury_asphyxiate_move_playersAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ashbury_asphyxiate_move_playersAuraScript);

        bool Load()
        {
            return true;
        }

        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
			//TC_LOG_ERROR("sql.sql", "float script activated by apply.");
			if (auto caster = GetCaster())
				if (auto target = GetTarget())
				{
					float o = target->GetOrientation();
					float z = target->GetPositionZ() + 15.0f;
					float x = target->GetPositionX();
					float y = target->GetPositionY();
					target->GetMotionMaster()->ForceMoveJump(x, y, z, 0.4f, 0.4f);
					//target->AddUnitState(UNIT_STATE_STUNNED);
					//target->AddUnitState(UNIT_STATE_NOT_MOVE);
				}
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
			//TC_LOG_ERROR("sql.sql", "float script activated by remove");
            if (auto caster = GetCaster())
                if (auto target = GetTarget())
                if (target->ToPlayer())
                {
					target->ClearUnitState(UNIT_STATE_STUNNED);
					//target->ClearUnitState(UNIT_STATE_NOT_MOVE);
                    target->GetMotionMaster()->MoveFall();
                }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_ashbury_asphyxiate_move_playersAuraScript::HandleOnEffectApply, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_ashbury_asphyxiate_move_playersAuraScript::HandleOnEffectRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_ashbury_asphyxiate_move_playersAuraScript();
    }
};

void AddSC_boss_baron_ashbury()
{
	new boss_baron_ashbury();
	new spell_ashbury_archangel();
	new spell_ashbury_pain_and_suffering();
	new spell_ashbury_asphyxiate();
	new spell_ashbury_asphyxiate_move_players();
}
