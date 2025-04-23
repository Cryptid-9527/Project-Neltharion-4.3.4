/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Loch_Modan
SD%Complete: 100% (SAI + THIS CPP)
SDComment: Wondi scripts!
EndScriptData */

/* ContentData
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Player.h"

// fix for quest=27116/the-winds-of-loch-modan
enum SkystriderFlightData
{
	SPELL_SKYSTRIDER_EXHAUST_FLAMES = 83984,
	SPELL_FLIGHT_SPEED_300 = 54422,
	SPELL_EJECT_ALL_PASSENGERS = 50630,
	SPELL_EJECT_PASSENGER_2 = 64630,
	SPELL_INVISIBILITY_DETECTION_2_MISC_8 = 49417,
	SPELL_FLY_EMOTE = 37744,
	SPELL_RIDE_VEHICLE = 46598,
};

enum SkystriderFlightEvents
{
	EVENT_START_GROUND_PATH = 1,
	EVENT_END_GROUND_PATH,
	EVENT_END_FLY_PATH_1,

	SPELL_FLY_AURA = 34873,
	NPC_ANDO_BLASTENHEIMER = 44870,
};

uint32 const SkystriderVehiclePathSize1 = 4;
const G3D::Vector3 SkystriderVehiclePath1[SkystriderVehiclePathSize1] =
{
	// ground
	{ -5011.16f, -3618.53f, 299.58f }, // 0
	{ -5006.26f, -3643.46f, 303.90f }, // 1
	{ -4998.19f, -3652.02f, 304.80f }, // 2
	{ -4987.19f, -3651.67f, 305.30f }, // 3
};

uint32 const SkystriderVehiclePathSize2 = 9;
const G3D::Vector3 SkystriderVehiclePath2[SkystriderVehiclePathSize2] =
{
	// fly
	{ -4886.28f, -3668.09f, 320.23f }, // 0
	{ -4806.85f, -3602.40f, 316.79f }, // 1
	{ -4739.00f, -3381.25f, 320.10f }, // 2
	{ -4623.74f, -3177.14f, 343.90f }, // 3
	{ -4464.84f, -3260.79f, 293.74f }, // 4
	{ -4715.38f, -3345.62f, 307.94f }, // 5
	{ -4863.97f, -3188.68f, 323.72f }, // 6
	{ -4716.85f, -2917.63f, 359.98f }, // 7
	{ -4803.05f, -2707.70f, 330.17f }, // 8 
};

uint32 const SkystriderVehiclePathSize3 = 4;
const G3D::Vector3 SkystriderVehiclePath3[SkystriderVehiclePathSize3] =
{
	{ -4816.835f, -2709.238f, 334.604f },
	{ -4852.358f, -2713.718f, 344.151f },
	{ -4918.095f, -2737.509f, 360.719f },
	{ -4989.943f, -2781.558f, 372.539f }, // 3
};

class npc_skystrider_vehicle : public CreatureScript
{
public:
	npc_skystrider_vehicle() : CreatureScript("npc_skystrider_vehicle") { }

	struct npc_skystrider_vehicleAI : public VehicleAI
	{
		npc_skystrider_vehicleAI(Creature* creature) : VehicleAI(creature) { }

		uint8 flymode = 0;

		void IsSummonedBy(Unit* summoner)
		{
			me->setActive(true);
			me->SetReactState(REACT_PASSIVE);
			me->SetCanFly(false);
			me->SetDisableGravity(false);

			if (auto* ando = me->SummonCreature(NPC_ANDO_BLASTENHEIMER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
			{
				andoGUID = ando->GetGUID();
				ando->EnterVehicle(me, 0);
			}

			if (auto player = summoner->ToPlayer())
			{
				playerGUID = player->GetGUID();
				player->CastWithDelay(500, me, SPELL_RIDE_VEHICLE);
			}
		}

		void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
		{
			if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
			{
				flymode = 0;
				events.ScheduleEvent(EVENT_START_GROUND_PATH, 1000);
			}

			if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
			{
				if (flymode < 2)
				{
					if (auto* ando = Unit::GetCreature(*me, andoGUID))
					{
						ando->DespawnOrUnsummon();
						me->DespawnOrUnsummon();
					}
				}
			}			
		}
		
		void MovementInform(uint32 type, uint32 point)
		{
			if (type != SPLINE_MOTION_TYPE)
				return;

			switch (flymode)
			{
			case 0:
				if (point == 3)
				{
					events.ScheduleEvent(EVENT_END_GROUND_PATH, 0);
				}
				break;
			case 1:
				if (point == 8)
				{
					events.ScheduleEvent(EVENT_END_FLY_PATH_1, 1000);
				}
				break;
			case 2:
				if (point == 3)
				{
					if (auto* ando = Unit::GetCreature(*me, andoGUID))
					{
						ando->DespawnOrUnsummon();
						me->DespawnOrUnsummon();
					}
				}
				break;
			default:
				break;
			}
		}
		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_START_GROUND_PATH:
					me->GetMotionMaster()->MoveSmoothPath(SkystriderVehiclePath1, SkystriderVehiclePathSize1);

					if (auto player = Unit::GetPlayer(*me, playerGUID))
						player->RemoveAura(SPELL_INVISIBILITY_DETECTION_2_MISC_8);
					break;
				case EVENT_END_GROUND_PATH:
					flymode++;
					me->CastSpell(me, SPELL_FLY_AURA);
					me->SetCanFly(true);
					me->SetDisableGravity(true);
					me->AI()->DoCast(SPELL_SKYSTRIDER_EXHAUST_FLAMES);
					me->AI()->DoCast(SPELL_FLIGHT_SPEED_300);
					me->GetMotionMaster()->MoveSmoothPath(SkystriderVehiclePath2, SkystriderVehiclePathSize2);
					break;
				case EVENT_END_FLY_PATH_1:
					flymode++;
					if (auto player = Unit::GetPlayer(*me, playerGUID))
					{
						player->ExitVehicle();
						me->GetMotionMaster()->MoveSmoothPath(SkystriderVehiclePath3, SkystriderVehiclePathSize3);
					}
					break;
				default:
					break;
				}
			}
		}
	private:
		EventMap events;
		uint64 playerGUID = 0;
		uint64 andoGUID = 0;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_skystrider_vehicleAI(creature);
	}
};

void AddSC_loch_modan()
{
	new npc_skystrider_vehicle();
}
