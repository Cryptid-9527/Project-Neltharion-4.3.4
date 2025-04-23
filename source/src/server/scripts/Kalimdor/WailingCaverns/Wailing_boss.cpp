#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "wailing_caverns.h"

//-------------------------------------------
// Kresh
//-------------------------------------------

enum Kresh_Spells
{
	SPELL_CRUSHINGBITE = 80362
};

enum Kresh_Events
{
	EVENT_CRUSHINGBITE = 1
};

class wailing_kresh : public CreatureScript
{
public:
	wailing_kresh() : CreatureScript("wailing_kresh") { }

	struct wailing_kreshAI : public ScriptedAI
	{
		wailing_kreshAI(Creature* creature) : ScriptedAI(creature)
		{
		};
		void EnterCombat(Unit* who)
		{
			TC_LOG_ERROR("Wailing", "Enter Combat");
			me->SetInCombatWith(who);
			events.ScheduleEvent(EVENT_CRUSHINGBITE, 3000);
		};
		void JustDied(Unit* /*killer*/)
		{
			events.CancelEvent(EVENT_CRUSHINGBITE);
		}
		void Reset()
		{
			events.CancelEvent(EVENT_CRUSHINGBITE);
		}
		void UpdateAI(const uint32 diff) override
		{
			/*
			function called every 50 ms
			*/
			events.Update(diff);//do not touch this

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_CRUSHINGBITE:
					DoCastVictim(SPELL_CRUSHINGBITE);
					events.ScheduleEvent(EVENT_CRUSHINGBITE, 15000);
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
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_kreshAI(creature);
	}
};

//-------------------------------------------
// Lord Anacondra
//-------------------------------------------

enum Anacondra_Spells
{
	ANA_SOMGREENCHANNELING = 13236,
	ANA_LIGHTNINGBOLT = 20295,
	ANA_SLUMBER = 8040,
	ANA_THORNS = 8148,
	ANA_HEALINGTOUCH = 23381
};

enum Anacondra_Events
{
	ANA_EVENT_LIGHTNINGBOLT = 1,
	ANA_EVENT_SLUMBER = 2,
	ANA_EVENT_THORNS = 3,
	ANA_EVENT_HEALINGTOUCH = 4
};

enum Anacondra_Texts
{
	ANA_SAY_AGGRO = 0
};

class wailing_anacondra : public CreatureScript
{
public:
	wailing_anacondra() : CreatureScript("wailing_anacondra") { }

	struct wailing_anacondraAI : public ScriptedAI
	{
		wailing_anacondraAI(Creature* creature) : ScriptedAI(creature)
		{
			DoCast(me, ANA_SOMGREENCHANNELING);
		};
		void EnterCombat(Unit* who)
		{
			me->SetInCombatWith(who);
			Talk(ANA_SAY_AGGRO);

			me->RemoveAura(ANA_SOMGREENCHANNELING);

			events.ScheduleEvent(ANA_EVENT_LIGHTNINGBOLT, 1000);
			events.ScheduleEvent(ANA_EVENT_SLUMBER, 5000);
			events.ScheduleEvent(ANA_EVENT_THORNS, 9000);
			events.ScheduleEvent(ANA_EVENT_HEALINGTOUCH, 26000);
		};
		void JustDied(Unit* /*killer*/)
		{
			InstanceScript* instance = me->GetInstanceScript();
			instance->SetData(TYPE_LADY_ANACONDRA, DONE);
			events.CancelEvent(ANA_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(ANA_EVENT_SLUMBER);
			events.CancelEvent(ANA_EVENT_THORNS);
			events.CancelEvent(ANA_EVENT_HEALINGTOUCH);
		}
		void Reset()
		{
			events.CancelEvent(ANA_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(ANA_EVENT_SLUMBER);
			events.CancelEvent(ANA_EVENT_THORNS);
			events.CancelEvent(ANA_EVENT_HEALINGTOUCH);
			DoCast(me, ANA_SOMGREENCHANNELING);
		}
		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			Unit* Target = me->getVictim();
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case ANA_EVENT_LIGHTNINGBOLT:
					DoCastVictim(ANA_LIGHTNINGBOLT);
					events.ScheduleEvent(ANA_EVENT_LIGHTNINGBOLT, 4000);
					break;
				case ANA_EVENT_SLUMBER:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, ANA_SLUMBER);
					events.ScheduleEvent(ANA_EVENT_SLUMBER, 18000);
					break;
				case ANA_EVENT_THORNS:
					DoCast(me, ANA_THORNS);
					me->SetInCombatWith(Target);
					me->AI()->AttackStart(Target);
					events.ScheduleEvent(ANA_EVENT_SLUMBER, 72000);
					break;
				case ANA_EVENT_HEALINGTOUCH:
					DoCast(me, ANA_HEALINGTOUCH);
					me->SetInCombatWith(Target);
					me->AI()->AttackStart(Target);
					events.ScheduleEvent(ANA_EVENT_HEALINGTOUCH, 24000);
					break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
		}
	private:
		EventMap events;

	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_anacondraAI(creature);
	}
};

//-------------------------------------------
// Lord Pythas
//-------------------------------------------

enum Pythas_Spells
{
	PYTHAS_SLUMBERUWU = 8040,
	PYTHAS_HEALINGTOUCH = 11431,
	PYTHAS_LIGHTNINGBOLT = 20805,
	PYTHAS_THUNDERCLAP = 8147
};

enum Pythas_Events
{
	PYTHAS_EVENT_SLUMBER = 1,
	PYTHAS_EVENT_HEALINGTOUCH = 2,
	PYTHAS_EVENT_LIGHTNINGBOLT = 3,
	PYTHAS_EVENT_THUNDERCLAP = 4
};

enum Pythas_Texts
{
	PYTHAS_SAY_AGGRO = 0
};

class wailing_pythas : public CreatureScript
{
public:
	wailing_pythas() : CreatureScript("wailing_pythas") { }

	struct wailing_pythasAI : public ScriptedAI
	{
		wailing_pythasAI(Creature* creature) : ScriptedAI(creature)
		{
		};
		void EnterCombat(Unit* who)
		{
			me->SetInCombatWith(who);
			Talk(PYTHAS_SAY_AGGRO);

			events.ScheduleEvent(PYTHAS_EVENT_LIGHTNINGBOLT, 1000);
			events.ScheduleEvent(PYTHAS_EVENT_SLUMBER, 5000);
			events.ScheduleEvent(PYTHAS_EVENT_THUNDERCLAP, 9000);
			events.ScheduleEvent(PYTHAS_EVENT_HEALINGTOUCH, 26000);
		};
		void JustDied(Unit* /*killer*/)
		{
			InstanceScript* instance = me->GetInstanceScript();
			instance->SetData(TYPE_LORD_PYTHAS, DONE);
			events.CancelEvent(PYTHAS_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(PYTHAS_EVENT_SLUMBER);
			events.CancelEvent(PYTHAS_EVENT_THUNDERCLAP);
			events.CancelEvent(PYTHAS_EVENT_HEALINGTOUCH);
		}
		void Reset()
		{
			events.CancelEvent(PYTHAS_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(PYTHAS_EVENT_SLUMBER);
			events.CancelEvent(PYTHAS_EVENT_THUNDERCLAP);
			events.CancelEvent(PYTHAS_EVENT_HEALINGTOUCH);
		}
		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case PYTHAS_EVENT_LIGHTNINGBOLT:
					DoCastVictim(PYTHAS_LIGHTNINGBOLT);
					events.ScheduleEvent(PYTHAS_EVENT_LIGHTNINGBOLT, 11000);
					break;
				case PYTHAS_EVENT_THUNDERCLAP:
					DoCastAOE(PYTHAS_THUNDERCLAP);
					events.ScheduleEvent(PYTHAS_EVENT_THUNDERCLAP, 14000);
					break;
				case PYTHAS_EVENT_SLUMBER:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, PYTHAS_SLUMBERUWU);
					events.ScheduleEvent(PYTHAS_EVENT_SLUMBER, 28000);
					break;
				case PYTHAS_EVENT_HEALINGTOUCH:
					DoCast(me, PYTHAS_HEALINGTOUCH);
					events.ScheduleEvent(PYTHAS_EVENT_HEALINGTOUCH, 31000);
					break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
		}
	private:
		EventMap events;

	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_pythasAI(creature);
	}
};

//-------------------------------------------
// Lord Cobrahn
//-------------------------------------------

enum Cobrahn_Spells
{
	COBRAHN_SERPENTFORM = 7965,
	COBRAHN_SLUMBER = 8040,
	COBRAHN_HEALINGTOUCH = 23381,
	COBRAHN_LIGHTNINGBOLT = 20805,
	COBRAHN_POISON = 744
};

enum Cobrahn_Events
{
	COBRAHN_EVENT_LIGHTNINGBOLT = 1,
	COBRAHN_EVENT_DRUIDSLUMBER = 2,
	COBRAHN_EVENT_SERPENTFORM = 3,
	COBRAHN_EVENT_POISON = 4
};

enum Cobrahn_Texts
{
	COBRAHN_SAY_AGGRO = 0
};

enum Cobrahn_Phase
{
	ELF_FORM,
	COBRA_FROM
};

class wailing_cobrahn : public CreatureScript
{
public:
	wailing_cobrahn() : CreatureScript("wailing_cobrahn") { }

	struct wailing_cobrahnAI : public ScriptedAI
	{
		wailing_cobrahnAI(Creature* creature) : ScriptedAI(creature)
		{
		};
		void EnterCombat(Unit* who)
		{
			me->SetInCombatWith(who);
			Talk(COBRAHN_SAY_AGGRO);

			events.ScheduleEvent(COBRAHN_EVENT_LIGHTNINGBOLT, 1000);
			events.ScheduleEvent(COBRAHN_EVENT_DRUIDSLUMBER, 5000);
			events.ScheduleEvent(COBRAHN_EVENT_SERPENTFORM, 35000);
		};
		void JustDied(Unit* /*killer*/)
		{
			InstanceScript* instance = me->GetInstanceScript();
			instance->SetData(TYPE_LORD_COBRAHN, DONE);
			events.CancelEvent(COBRAHN_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(COBRAHN_EVENT_DRUIDSLUMBER);
			events.CancelEvent(COBRAHN_EVENT_SERPENTFORM);
			events.CancelEvent(COBRAHN_EVENT_POISON);
		}

		void Reset()
		{
			events.CancelEvent(COBRAHN_EVENT_LIGHTNINGBOLT);
			events.CancelEvent(COBRAHN_EVENT_DRUIDSLUMBER);
			events.CancelEvent(COBRAHN_EVENT_SERPENTFORM);
			events.CancelEvent(COBRAHN_EVENT_POISON);
		}

		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{

				switch (eventId)
				{
				case COBRAHN_EVENT_LIGHTNINGBOLT:
					DoCastVictim(COBRAHN_LIGHTNINGBOLT);
					events.ScheduleEvent(COBRAHN_EVENT_LIGHTNINGBOLT, 6000);
					break;
				case COBRAHN_EVENT_DRUIDSLUMBER:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, COBRAHN_SLUMBER);
					events.ScheduleEvent(COBRAHN_EVENT_DRUIDSLUMBER, 15000);
					break;
				case COBRAHN_EVENT_SERPENTFORM:
					events.CancelEvent(COBRAHN_EVENT_LIGHTNINGBOLT);
					events.CancelEvent(COBRAHN_EVENT_DRUIDSLUMBER);
					DoCast(me, COBRAHN_SERPENTFORM);
					events.ScheduleEvent(COBRAHN_EVENT_POISON, 1500);
					events.ScheduleEvent(COBRAHN_EVENT_SERPENTFORM, 59000);
					break;
				case COBRAHN_EVENT_POISON:
					DoCastVictim(COBRAHN_POISON);
					events.ScheduleEvent(COBRAHN_EVENT_POISON, 7000);
					break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
		}
	private:
		EventMap events;
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_cobrahnAI(creature);
	}
};

//-------------------------------------------
// Lord Serpentis
//-------------------------------------------

enum Serpentis_Spells
{
	SERPENTIS_SLUMBER = 8040,
	SERPENTIS_HEALINGTOUCH = 23381,
	SERPENTIS_LIGHTNINGBOLT = 12167
};

enum Serpentis_Events
{
	SERPENTIS_EVENT_SLUMBER = 1,
	SERPENTIS_EVENT_HEALINGTOUCH = 2,
	SERPENTIS_EVENT_LIGHTNING_BOLT = 3
};

enum Serpentis_Texts
{
	SERPENTIS_SAY_AGGRO = 0
};

class wailing_serpentis : public CreatureScript
{
public:
	wailing_serpentis() : CreatureScript("wailing_serpentis") { }

	struct wailing_serpentisAI : public ScriptedAI
	{
		wailing_serpentisAI(Creature* creature) : ScriptedAI(creature)
		{
		};
		void EnterCombat(Unit* who)
		{
			me->SetInCombatWith(who);
			Talk(SERPENTIS_SAY_AGGRO);

			events.ScheduleEvent(SERPENTIS_EVENT_SLUMBER, 20000);
			events.ScheduleEvent(SERPENTIS_EVENT_HEALINGTOUCH, 30000);
			events.ScheduleEvent(SERPENTIS_EVENT_LIGHTNING_BOLT, 1000);
		};
		void JustDied(Unit* /*killer*/)
		{
			InstanceScript* instance = me->GetInstanceScript();
			instance->SetData(TYPE_LORD_SERPENTIS, DONE);
			events.CancelEvent(SERPENTIS_EVENT_SLUMBER);
			events.CancelEvent(SERPENTIS_EVENT_HEALINGTOUCH);
			events.CancelEvent(SERPENTIS_EVENT_LIGHTNING_BOLT);
		}

		void Reset()
		{
			events.CancelEvent(SERPENTIS_EVENT_SLUMBER);
			events.CancelEvent(SERPENTIS_EVENT_HEALINGTOUCH);
			events.CancelEvent(SERPENTIS_EVENT_LIGHTNING_BOLT);
		}

		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case SERPENTIS_EVENT_SLUMBER:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, SERPENTIS_SLUMBER);
					events.ScheduleEvent(SERPENTIS_EVENT_SLUMBER, 25000);
					break;
				case SERPENTIS_EVENT_HEALINGTOUCH:
					DoCast(me, SERPENTIS_HEALINGTOUCH);
					events.ScheduleEvent(SERPENTIS_EVENT_HEALINGTOUCH, 10000);
					break;
				case SERPENTIS_EVENT_LIGHTNING_BOLT:
					DoCastVictim(SERPENTIS_LIGHTNINGBOLT);
					events.ScheduleEvent(SERPENTIS_EVENT_LIGHTNING_BOLT, 8000);
					break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
		}
	private:
		EventMap events;
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_serpentisAI(creature);
	}
};

//-------------------------------------------
// Mutanus The Devourer
//-------------------------------------------

enum Mutanus_Spells
{
	MUTANUS_NIGHTMARE = 7967,
	MUTANUS_TERRIFY = 7399,
	MUTANUS_THUNDERCRACK = 8150
};

enum Mutanus_Events
{
	MUTANUS_EVENT_NIGHTMARE = 1,
	MUTANUS_EVENT_TERRIFY = 2,
	MUTANUS_EVENT_THUNDERCRACK = 3
};

class wailing_mutanus : public CreatureScript
{
public:
	wailing_mutanus() : CreatureScript("wailing_mutanus") { }

	struct wailing_mutanusAI : public ScriptedAI
	{
		wailing_mutanusAI(Creature* creature) : ScriptedAI(creature)
		{
		};
		void EnterCombat(Unit* who)
		{
			me->SetInCombatWith(who);
			Talk(SERPENTIS_SAY_AGGRO);

			events.ScheduleEvent(MUTANUS_EVENT_NIGHTMARE, 5000);
			events.ScheduleEvent(MUTANUS_EVENT_TERRIFY, 10000);
			events.ScheduleEvent(MUTANUS_EVENT_THUNDERCRACK, 1000);
		};
		void JustDied(Unit* /*killer*/)
		{
			InstanceScript* instance = me->GetInstanceScript();
			instance->SetData(TYPE_MUTANUS_THE_DEVOURER, DONE);
			events.CancelEvent(MUTANUS_EVENT_NIGHTMARE);
			events.CancelEvent(MUTANUS_EVENT_TERRIFY);
			events.CancelEvent(MUTANUS_EVENT_THUNDERCRACK);
		}

		void Reset()
		{
			events.CancelEvent(MUTANUS_EVENT_NIGHTMARE);
			events.CancelEvent(MUTANUS_EVENT_TERRIFY);
			events.CancelEvent(MUTANUS_EVENT_THUNDERCRACK);
		}

		void UpdateAI(uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case MUTANUS_EVENT_NIGHTMARE:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, MUTANUS_NIGHTMARE);
					events.ScheduleEvent(MUTANUS_EVENT_NIGHTMARE, 15000);
					break;
				case MUTANUS_EVENT_TERRIFY:
					if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
						DoCast(target, MUTANUS_TERRIFY);
					events.ScheduleEvent(MUTANUS_EVENT_TERRIFY, 20000);
					break;
				case MUTANUS_EVENT_THUNDERCRACK:
					DoCastAOE(MUTANUS_THUNDERCRACK);
					events.ScheduleEvent(MUTANUS_EVENT_THUNDERCRACK, 10000);
					break;
				default:
					break;
				}
			}

			if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
		}
	private:
		EventMap events;
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new wailing_mutanusAI(creature);
	}
};

void AddSC_wailing_bosses()
{
	new wailing_kresh();
	new wailing_anacondra();
	new wailing_pythas();
	new wailing_cobrahn();
	new wailing_serpentis();
	new wailing_mutanus();
}