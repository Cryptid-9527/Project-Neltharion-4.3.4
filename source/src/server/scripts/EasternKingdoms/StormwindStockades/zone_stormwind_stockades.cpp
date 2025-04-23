/*
 * Copyright (C) 2011-2012 Project SkyFire <http://www.projectskyfire.org/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
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
enum Poacher_spells
{
	SPELL_NET  = 31290,
	SPELL_WILDLYSTABBING = 86726,
	SPELL_SWEEP = 86729,
	SPELL_VANISH = 55964,
	SPELL_SHADOWSTEP = 55966
};
enum Sleeping_Offensive_NPCS
{
	NPC1  = 225728,
};
enum NPC_Events
{
	// Poacher
	EVENT_CAST_NET = 1,
	// Randolph Moloch
	EVENT_WILDLYSTABBING,
	EVENT_SWEEP,
	EVENT_VANISH,
	EVENT_AFTERVANISH
};
enum Randolph_text
{
	AGGRO           = 0,
	VANISH          = 1,
	DEATH           = 2,
};
class npc_stockades_sleeping : public CreatureScript
{
public:
	npc_stockades_sleeping() : CreatureScript("npc_stockades_sleeping") { }

	struct npc_stockades_sleepingAI : public ScriptedAI
	{
		npc_stockades_sleepingAI(Creature* creature) : ScriptedAI(creature)
		{
			if(me->GetDBTableGUIDLow() == NPC1) {
				me->SetStandState(UNIT_STAND_STATE_SLEEP);
			}
		}
		void EnterCombat(Unit * who)
		{
			me->SetStandState(UNIT_STAND_STATE_STAND);
		}
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stockades_sleepingAI (creature);
	}
};

class npc_stockades_poacher : public CreatureScript
{
public:
	npc_stockades_poacher() : CreatureScript("npc_stockades_poacher") { }

	struct npc_stockades_poacherAI : public ScriptedAI
	{
		npc_stockades_poacherAI(Creature* creature) : ScriptedAI(creature)
		{

		};
		void EnterCombat(Unit* who)
		{
			DoCast(who, SPELL_NET);
			events.ScheduleEvent(EVENT_CAST_NET, 15000);
		};
		void JustDied(Unit* /*killer*/) 
		{
			events.CancelEvent(EVENT_CAST_NET);
		}
		void UpdateAI(const uint32 diff) override
		{
			/*
			function called every 50 ms
			*/
			events.Update(diff);//do not touch this

			/*
			This is some magic shit that actually works to make events flow smoothly.
			you can schedule events with other events, or have events schedule themselves after completion.
			*/
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_CAST_NET:
						DoCast(me->getVictim(), SPELL_NET);
						events.ScheduleEvent(EVENT_CAST_NET, 15000); 
						break;
					default:
						break;
				}
			}
			/*
			this function controls scripted NPCs ability to melee attack. you can put conditions around it or disable it entirely.
			*/
			DoMeleeAttackIfReady();
		}
		private:
			/*
			Variables for this npc to keep in memory
			*/
			EventMap events;

	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stockades_poacherAI (creature);
	}
};

class npc_stockades_RandolphMoloch : public CreatureScript
{
public:
	npc_stockades_RandolphMoloch() : CreatureScript("npc_stockades_RandolphMoloch") { }

	struct npc_stockades_RandolphMolochAI : public ScriptedAI
	{
		npc_stockades_RandolphMolochAI(Creature* creature) : ScriptedAI(creature)
		{
			g_IsVanished = false;
			g_Vanished[0] = false;
			g_Vanished[1] = false;
		};
		void EnterCombat(Unit* who)
		{
			Talk(AGGRO);
			me->SetInCombatWith(who);
			events.ScheduleEvent(EVENT_WILDLYSTABBING, 4000);
			events.ScheduleEvent(EVENT_SWEEP, 1000);
		};
		void JustDied(Unit* /*killer*/) 
		{
			Talk(DEATH);
			events.CancelEvent(EVENT_CAST_NET);
			events.CancelEvent(EVENT_WILDLYSTABBING);
			events.CancelEvent(EVENT_SWEEP);
			events.CancelEvent(EVENT_VANISH);
			events.CancelEvent(EVENT_AFTERVANISH);
		}
		void Reset()
		{
			events.CancelEvent(EVENT_CAST_NET);
			events.CancelEvent(EVENT_WILDLYSTABBING);
			events.CancelEvent(EVENT_SWEEP);
			events.CancelEvent(EVENT_VANISH);
			events.CancelEvent(EVENT_AFTERVANISH);
			g_IsVanished = false;
			g_Vanished[0] = false;
			g_Vanished[1] = false;
		}
		void UpdateAI(const uint32 diff) override
		{
			if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
				return;
			/*
			function called every 50 ms
			*/
			events.Update(diff);//do not touch this
			if((me)->GetCurrentSpell(CURRENT_GENERIC_SPELL))
				return;

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
					case EVENT_WILDLYSTABBING:
						DoCastVictim(SPELL_WILDLYSTABBING);
						events.ScheduleEvent(EVENT_WILDLYSTABBING, 10000);
						break;
					case EVENT_SWEEP:
						DoCastVictim(SPELL_SWEEP);
						events.ScheduleEvent(EVENT_SWEEP, 5000);
						break;
					case EVENT_VANISH:
						Talk(VANISH);
						me->RemoveAllAuras();
						me->CastStop();
						DoCast(me, SPELL_VANISH);
						g_VanishHealth = me->GetHealth();
						events.CancelEvent(EVENT_WILDLYSTABBING);
						events.CancelEvent(EVENT_SWEEP);
						//me->SetReactState(REACT_PASSIVE);
						events.ScheduleEvent(EVENT_AFTERVANISH, 2000);
						g_IsVanished = true;
						break;
					case EVENT_AFTERVANISH:
						if (auto newtarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 1000.0f, true)) {
							DoCast(newtarget, SPELL_SHADOWSTEP, true);
							DoCastVictim(SPELL_SWEEP);
							me->SetInCombatWith(newtarget);
							events.ScheduleEvent(EVENT_WILDLYSTABBING, 3000);
							me->AI()->AttackStart(newtarget);
						}
						g_IsVanished = false;
						me->SetHealth(g_VanishHealth);
						//me->SetReactState(REACT_AGGRESSIVE);
						break;
					default:
						break;
				}
			}
			if(!g_IsVanished)
				DoMeleeAttackIfReady();
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage) override
		{
			if (me->HealthBelowPctDamaged(71, damage) && me->HealthAbovePct(59) && !g_Vanished[0])
			{
				g_Vanished[0] = true;
				events.ScheduleEvent(EVENT_VANISH, 1000);
			}

			if (me->HealthBelowPctDamaged(41, damage) && me->HealthAbovePct(29) && !g_Vanished[1])
			{
				g_Vanished[1] = true;
				events.ScheduleEvent(EVENT_VANISH, 1000);
			}
		}
		private:
			/*
			Variables for this npc to keep in memory
			*/
			int g_VanishHealth;
			bool g_IsVanished;
			bool g_Vanished[2];
			EventMap events;

	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stockades_RandolphMolochAI (creature);
	}
};

void AddSC_stormwind_stockades()
{
	new npc_stockades_poacher();
	new npc_stockades_sleeping();
	new npc_stockades_RandolphMoloch();
}