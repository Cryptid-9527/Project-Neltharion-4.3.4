#include "AnticheatMgr.h"
#include "AnticheatScripts.h"
#include "MapManager.h"
#include "AccountMgr.h"
#include "Unit.h"
#include "Player.h"
//#include "Transport.h"

#define CLIMB_ANGLE 1.9f

AnticheatMgr::AnticheatMgr()
{
}

AnticheatMgr::~AnticheatMgr()
{
	m_Players.clear();
}

struct SavedLocation
{
	WorldLocation location;
	bool exist = false;
	float OldZ = 0.f;
	uint32 WarningsForAirWalking = 0;
};

static std::map<uint64, SavedLocation> AntiHackSave;


void AnticheatMgr::JumpHackDetection(Player* player, MovementInfo movementInfo, uint32 opcode)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & JUMP_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUIDLow();

	if (m_Players[key].GetLastOpcode() == MSG_MOVE_JUMP && opcode == MSG_MOVE_JUMP)
	{
		BuildReport(player, JUMP_HACK_REPORT);
		//sLog->outError("AnticheatMgr:: Jump-Hack detected player GUID (low) %u",player->GetGUIDLow());
	}
}

void AnticheatMgr::WalkOnWaterHackDetection(Player* player, MovementInfo movementInfo)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & WALK_WATER_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUIDLow();
	if (!m_Players[key].GetLastMovementInfo().HasMovementFlag(MOVEMENTFLAG_WATERWALKING))
		return;

	// if we are a ghost we can walk on water
	if (!player->isAlive())
		return;

	if (player->HasAuraType(SPELL_AURA_FEATHER_FALL) ||
		player->HasAuraType(SPELL_AURA_SAFE_FALL) ||
		player->HasAuraType(SPELL_AURA_WATER_WALK))
		return;

	//sLog->outError("AnticheatMgr:: Walk on Water - Hack detected player GUID (low) %u",player->GetGUIDLow());
	BuildReport(player, WALK_WATER_HACK_REPORT);

}


void AnticheatMgr::TeleportPlaneHackDetection(Player* player, MovementInfo movementInfo)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & TELEPORT_PLANE_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUIDLow();

	if (m_Players[key].GetLastMovementInfo().pos.GetPositionZ() != 0 ||
		movementInfo.pos.GetPositionZ() != 0)
		return;

	if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING))
		return;

	//DEAD_FALLING was deprecated
	//if (player->getDeathState() == DEAD_FALLING)
	//    return;
	float x, y, z;
	player->GetPosition(x, y, z);
	float ground_Z = player->GetMap()->GetHeight(x, y, z);
	float z_diff = fabs(ground_Z - z);

	// we are not really walking there
	if (z_diff > 1.0f)
	{
		//sLog->outError("AnticheatMgr:: Teleport To Plane - Hack detected player GUID (low) %u",player->GetGUIDLow());
		BuildReport(player, TELEPORT_PLANE_HACK_REPORT);
	}
}

void AnticheatMgr::StartHackDetection(Player* player, MovementInfo movementInfo, uint32 opcode)
{
	if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE) || !player)
		return;

	if (player->isGameMaster())
		return;

	uint32 key = player->GetGUIDLow();

	if (player->isInFlight() || player->GetTransport() || player->GetVehicle())
	{
		m_Players[key].SetLastMovementInfo(movementInfo);
		m_Players[key].SetLastOpcode(opcode);
		return;
	}

	SpeedHackDetection(player, movementInfo);
	FlyHackDetection(player, movementInfo);
	WalkOnWaterHackDetection(player, movementInfo);
	JumpHackDetection(player, movementInfo, opcode);
	TeleportPlaneHackDetection(player, movementInfo);
	ClimbHackDetection(player, movementInfo, opcode);

	m_Players[key].SetLastMovementInfo(movementInfo);
	m_Players[key].SetLastOpcode(opcode);
}

// basic detection
void AnticheatMgr::ClimbHackDetection(Player *player, MovementInfo movementInfo, uint32 opcode)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & CLIMB_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUIDLow();

	if (opcode != MSG_MOVE_HEARTBEAT ||
		m_Players[key].GetLastOpcode() != MSG_MOVE_HEARTBEAT)
		return;

	// in this case we don't care if they are "legal" flags, they are handled in another parts of the Anticheat Manager.
	if (player->IsInWater() ||
		player->IsFlying() ||
		player->IsFalling())
		return;

	Position playerPos;
	player->GetPosition(&playerPos);

	float deltaZ = fabs(playerPos.GetPositionZ() - movementInfo.pos.GetPositionZ());
	float deltaXY = movementInfo.pos.GetExactDist2d(&playerPos);

	float angle = player->NormalizeOrientation(tan(deltaZ / deltaXY));

	if (angle > CLIMB_ANGLE)
	{
		//sLog->outError("AnticheatMgr:: Climb-Hack detected player GUID (low) %u", player->GetGUIDLow());
		BuildReport(player, CLIMB_HACK_REPORT);
	}




}

void AnticheatMgr::FlyHackDetection(Player* player, MovementInfo  movementInfo)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & FLY_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUID();
	if (!m_Players[key].GetLastMovementInfo().HasMovementFlag(MOVEMENTFLAG_FLYING))
		return;

	if (player->HasAuraType(SPELL_AURA_FLY) ||
		player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
		player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) || player->HasUnitMovementFlag(MOVEMENTFLAG_FALLING) || player->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_SLOW) || player->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR) || player->IsJumping()
		|| player->IsFalling())
		return;

	TC_LOG_DEBUG("entities.player.character", "AnticheatMgr:: Fly-Hack detected player GUID (low) %u", player->GetGUID());
	BuildReport(player, FLY_HACK_REPORT);
}

void AnticheatMgr::SpeedHackDetection(Player* player, MovementInfo movementInfo)
{
	if ((sWorld->getIntConfig(CONFIG_ANTICHEAT_DETECTIONS_ENABLED) & SPEED_HACK_DETECTION) == 0 || !player)
		return;

	uint32 key = player->GetGUIDLow();

	// We also must check the map because the movementFlag can be modified by the client.
	// If we just check the flag, they could always add that flag and always skip the speed hacking detection.
	// 369 == DEEPRUN TRAM
	if (player->GetMapId() == 369)
		return;

	if (!player->isAlive())
		return;

	uint32 distance2D = (uint32)movementInfo.pos.GetExactDist2d(&m_Players[key].GetLastMovementInfo().pos);
	uint8 moveType = 0;


	if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || player->isInFlight())
		return;

	if (player->IsFalling() || player->isInFlight() || player->IsLevitating())
		return;

	if (player->GetVehicleKit() && player->GetVehicleKit()->GetBase())
		return;

	if (!player->IsControlledByPlayer())
		return;

	if (player->GetTransport() || player->GetVehicle() || player->GetVehicleKit())
		return;

	if (player->HasAuraType(SPELL_AURA_CONTROL_VEHICLE))
		return;

	// we need to know HOW is the player moving
	// TO-DO: Should we check the incoming movement flags?
	if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
		moveType = MOVE_SWIM;
	else if (player->IsFlying())
		moveType = MOVE_FLIGHT;
	else if (player->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
		moveType = MOVE_WALK;
	else
		moveType = MOVE_RUN;

	// how many yards the player can do in one sec.
	uint32 speedRate = (uint32)(player->GetSpeed(UnitMoveType(moveType)) + movementInfo.j_xyspeed);

	// how long the player took to move to here.
	uint32 timeDiff = getMSTimeDiff(m_Players[key].GetLastMovementInfo().time, movementInfo.time);

	if (!timeDiff)
		timeDiff = 1;



	// this is the distance doable by the player in 1 sec, using the time done to move to this point.
	uint32 clientSpeedRate = distance2D * 1000 / timeDiff;

	// we did the (uint32) cast to accept a margin of tolerance
	if (clientSpeedRate > speedRate)
	{
		BuildReport(player, SPEED_HACK_REPORT);
		//sLog->outError("AnticheatMgr:: Speed-Hack detected player GUID (low) %u",player->GetGUIDLow());
	}
}

void AnticheatMgr::StartScripts()
{
	new AnticheatScripts();
}

void AnticheatMgr::HandlePlayerLogin(Player* player)
{
	// we must delete this to prevent errors in case of crash
	CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u", player->GetGUIDLow());
	// we initialize the pos of lastMovementPosition var.
	m_Players[player->GetGUIDLow()].SetPosition(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
	QueryResult resultDB = CharacterDatabase.PQuery("SELECT * FROM daily_players_reports WHERE guid=%u;", player->GetGUIDLow());

	if (resultDB)
		m_Players[player->GetGUIDLow()].SetDailyReportState(true);
}

void AnticheatMgr::HandlePlayerLogout(Player* player)
{
	// TO-DO Make a table that stores the cheaters of the day, with more detailed information.

	// We must also delete it at logout to prevent have data of offline players in the db when we query the database (IE: The GM Command)
	CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u", player->GetGUIDLow());
	// Delete not needed data from the memory.
	m_Players.erase(player->GetGUIDLow());
}

void AnticheatMgr::SavePlayerData(Player* player)
{
	CharacterDatabase.PExecute("REPLACE INTO players_reports_status (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES (%u,%f,%u,%u,%u,%u,%u,%u,%u,%u);", player->GetGUIDLow(), m_Players[player->GetGUIDLow()].GetAverage(), m_Players[player->GetGUIDLow()].GetTotalReports(), m_Players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT), m_Players[player->GetGUIDLow()].GetCreationTime());
}

uint32 AnticheatMgr::GetTotalReports(uint32 lowGUID)
{
	return m_Players[lowGUID].GetTotalReports();
}

float AnticheatMgr::GetAverage(uint32 lowGUID)
{
	return m_Players[lowGUID].GetAverage();
}

uint32 AnticheatMgr::GetTypeReports(uint32 lowGUID, uint8 type)
{
	return m_Players[lowGUID].GetTypeReports(type);
}

bool AnticheatMgr::MustCheckTempReports(uint8 type)
{
	if (type == JUMP_HACK_REPORT)
		return false;

	return true;
}

void AnticheatMgr::BuildReport(Player* player, uint8 reportType)
{
	uint32 key = player->GetGUIDLow();

	if (MustCheckTempReports(reportType))
	{
		uint32 actualTime = getMSTime();

		if (!m_Players[key].GetTempReportsTimer(reportType))
			m_Players[key].SetTempReportsTimer(actualTime, reportType);

		if (getMSTimeDiff(m_Players[key].GetTempReportsTimer(reportType), actualTime) < 3000)
		{
			m_Players[key].SetTempReports(m_Players[key].GetTempReports(reportType) + 1, reportType);

			if (m_Players[key].GetTempReports(reportType) < 3)
				return;
		}
		else
		{
			m_Players[key].SetTempReportsTimer(actualTime, reportType);
			m_Players[key].SetTempReports(1, reportType);
			return;
		}
	}

	// generating creationTime for average calculation
	if (!m_Players[key].GetTotalReports())
		m_Players[key].SetCreationTime(getMSTime());

	// increasing total_reports
	m_Players[key].SetTotalReports(m_Players[key].GetTotalReports() + 1);
	// increasing specific cheat report
	m_Players[key].SetTypeReports(reportType, m_Players[key].GetTypeReports(reportType) + 1);

	// diff time for average calculation
	uint32 diffTime = getMSTimeDiff(m_Players[key].GetCreationTime(), getMSTime()) / IN_MILLISECONDS;

	if (diffTime > 0)
	{
		// Average == Reports per second
		float average = float(m_Players[key].GetTotalReports()) / float(diffTime);
		m_Players[key].SetAverage(average);
	}

	if (sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT) < m_Players[key].GetTotalReports())
	{
		if (!m_Players[key].GetDailyReportState())
		{
			CharacterDatabase.PExecute("REPLACE INTO daily_players_reports (guid,average,total_reports,speed_reports,fly_reports,jump_reports,waterwalk_reports,teleportplane_reports,climb_reports,creation_time) VALUES (%u,%f,%u,%u,%u,%u,%u,%u,%u,%u);", player->GetGUIDLow(), m_Players[player->GetGUIDLow()].GetAverage(), m_Players[player->GetGUIDLow()].GetTotalReports(), m_Players[player->GetGUIDLow()].GetTypeReports(SPEED_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(FLY_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(JUMP_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(WALK_WATER_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(TELEPORT_PLANE_HACK_REPORT), m_Players[player->GetGUIDLow()].GetTypeReports(CLIMB_HACK_REPORT), m_Players[player->GetGUIDLow()].GetCreationTime());
			m_Players[key].SetDailyReportState(true);
			WorldSession* s = player->GetSession();
			s->KickPlayer("AnticheatMgr::AnticheatViolation");
			std::stringstream duration;
            duration << sWorld->getIntConfig(CONFIG_WARDEN_CLIENT_BAN_DURATION) << "s";
            std::string accountName;
            AccountMgr::GetName(s->GetAccountId(), accountName);
            std::stringstream banReason;
            banReason << "Anticheat Violation";
            //sWorld->BanAccount(BAN_ACCOUNT, accountName, duration.str(), banReason.str(),"Anticheat");
		}
	}

	if (m_Players[key].GetTotalReports() > sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION))
	{
		// display warning at the center of the screen, hacky way?
		std::string str = "";
		str = "|[[Anticheat]|cFF00FFFF[|cFF60FF00" + std::string(player->GetName()) + "|cFF00FFFF] possible cheater!";
		WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
		data << str;
		sWorld->SendGlobalGMMessage(&data);
	}
}

void AnticheatMgr::AnticheatGlobalCommand(ChatHandler* handler)
{
	// MySQL will sort all for us, anyway this is not the best way we must only save the anticheat data not whole player's data!.
	sObjectAccessor->SaveAllPlayers();

	QueryResult resultDB = CharacterDatabase.Query("SELECT guid,average,total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY average ASC LIMIT 3;");
	if (!resultDB)
	{
		handler->PSendSysMessage("No players found.");
		return;
	}
	else
	{
		handler->SendSysMessage("=============================");
		handler->PSendSysMessage("Players with the lowest averages:");
		do
		{
			Field *fieldsDB = resultDB->Fetch();

			uint32 guid = fieldsDB[0].GetUInt32();
			float average = fieldsDB[1].GetFloat();
			uint32 total_reports = fieldsDB[2].GetUInt32();

			if (auto player = sObjectMgr->GetPlayerByLowGUID(guid))
				handler->PSendSysMessage("Player: %s Average: %f Total Reports: %u", player->GetName(), average, total_reports);

		} while (resultDB->NextRow());
	}

	resultDB = CharacterDatabase.Query("SELECT guid,average,total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY total_reports DESC LIMIT 3;");

	// this should never happen
	if (!resultDB)
	{
		handler->PSendSysMessage("No players found.");
		return;
	}
	else
	{
		handler->SendSysMessage("=============================");
		handler->PSendSysMessage("Players with the more reports:");
		do
		{
			Field *fieldsDB = resultDB->Fetch();

			uint32 guid = fieldsDB[0].GetUInt32();
			float average = fieldsDB[1].GetFloat();
			uint32 total_reports = fieldsDB[2].GetUInt32();

			if (auto player = sObjectMgr->GetPlayerByLowGUID(guid))
				handler->PSendSysMessage("Player: %s Total Reports: %u Average: %f", player->GetName(), total_reports, average);

		} while (resultDB->NextRow());
	}
}

void AnticheatMgr::AnticheatDeleteCommand(uint32 guid)
{
	if (!guid)
	{
		for (AnticheatPlayersDataMap::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
		{
			(*it).second.SetTotalReports(0);
			(*it).second.SetAverage(0);
			(*it).second.SetCreationTime(0);
			for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
			{
				(*it).second.SetTempReports(0, i);
				(*it).second.SetTempReportsTimer(0, i);
				(*it).second.SetTypeReports(i, 0);
			}
		}
		CharacterDatabase.PExecute("DELETE FROM players_reports_status;");
	}
	else
	{
		m_Players[guid].SetTotalReports(0);
		m_Players[guid].SetAverage(0);
		m_Players[guid].SetCreationTime(0);
		for (uint8 i = 0; i < MAX_REPORT_TYPES; i++)
		{
			m_Players[guid].SetTempReports(0, i);
			m_Players[guid].SetTempReportsTimer(0, i);
			m_Players[guid].SetTypeReports(i, 0);
		}
		CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid=%u;", guid);
	}
}

void AnticheatMgr::ResetDailyReportStates()
{
	for (AnticheatPlayersDataMap::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
		m_Players[(*it).first].SetDailyReportState(false);
}























































/*Dandi's anticheat*/
/*
 * Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
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

#include "AccountMgr.h"
#include "DBCStores.h"
#include "Language.h"
#include "Log.h"
#include "MapManager.h"
#include "Player.h"
#include "Vehicle.h"
#include "World.h"
#include "WorldSession.h"

 // Anticheat System
void Player::SetUnderACKmount()
{
	m_mountTimer = 3000;
	m_ACKmounted = true;
}

void Player::SetUnderACKmountAdvanced()
{
	m_advancedTimer = 4500;
	//m_ACKmounted = true;
	m_ACKmounted_Advanced = true;
}

void Player::SetUnderACKmountTransport()
{
	m_transportTimer = 3000;
	//m_ACKmounted = true;
	m_ACKmounted_Transport = true;
}

void Player::SetIsAllowedToFall(bool apply)
{
	if (apply)
		m_AllowedToFall = true;
	else
		m_AllowedToFall = false;
}

void Player::SetRootACKUpd(uint32 delay)
{
	m_rootUpdTimer = 1500 + delay;
	m_rootUpd = true;
}

void Player::SetLastJump()
{
	lastJump = getMSTime();
}

void Player::UpdateMovementInfo(MovementInfo const& movementInfo)
{
	SetLastMoveClientTimestamp(movementInfo.time);
	SetLastMoveServerTimestamp(getMSTime());
}

void Player::StartWaitingLandOrSwimOpcode()
{
	m_antiNoFallDmgTimer = 3000;
	m_antiNoFallDmg = true;
}

bool Player::CheckOnFlyHack()
{
	if (m_vehicle)
		return true;	

	if (!m_mover || !this || !m_mover->ToPlayer())
		return true;

	if (m_mover->ToPlayer()->IsCanFlybyServer())
		return true;

	if (m_mover->ToPlayer()->IsFalling())
		return true;

	if (m_mover->ToPlayer()->GetShapeshiftForm() == FORM_FLIGHT || m_mover->ToPlayer()->GetShapeshiftForm() == FORM_FLIGHT_EPIC)
		return true;


	AntiHackSave[m_mover->ToPlayer()->GetGUID()].location = m_mover->ToPlayer()->GetWorldLocation();

	//Flyhacks are implemented in MovementHandler.cpp, This is just a nuisance preventing punishment for airwalking atm.
	/*
	if (m_mover->ToPlayer()->IsFlying() && !m_mover->ToPlayer()->CanFly()) // kick flyhacks
	{
		TC_LOG_INFO("anticheat", "Player::CheckMovementInfo :  FlyHack Detected for Account id : %u, Player %s", m_mover->ToPlayer()->GetSession()->GetAccountId(), m_mover->ToPlayer()->GetName().c_str());
		TC_LOG_INFO("anticheat", "Player::========================================================");
		TC_LOG_INFO("anticheat", "Player IsFlying but CanFly is false");

		std::string accusedOf = "IsFlying/CanFly manipulation";
		std::stringstream HackReport;
		std::string isFlying = "false";
		std::string canFly = "false";
		if (IsFlying())
			isFlying = "true";
		if (CanFly())
			canFly = "true";
		HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
		if (HasAura(605))
			HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
		HackReport << "\n|cffFF0000Additional info [debug]|r: currently flying: |cffFF0000" << isFlying << "|r, can fly: |cffFF0000" << canFly << "|r";

		TeleportTo(AntiHackSave[m_mover->ToPlayer()->GetGUID()].location); //port back
		m_mover->GetMotionMaster()->MoveFall();
		sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession()LANG_GM_BROADCAST, HackReport.str().c_str());
		return false;		
	}
	*/
	if (m_mover->ToPlayer()->IsFlying() || m_mover->ToPlayer()->IsLevitating() || m_mover->ToPlayer()->isInFlight())
		return true;

	if (m_mover->ToPlayer()->GetTransport() || m_mover->ToPlayer()->GetVehicle() || m_mover->ToPlayer()->GetVehicleKit())
		return true;

	if (m_mover->ToPlayer()->HasAuraType(SPELL_AURA_CONTROL_VEHICLE))
		return true;

	if (m_mover->ToPlayer()->GetTransport())	
		return true;

	if (m_mover->ToPlayer()->HasUnitState(UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED | UNIT_STATE_CHARGING | UNIT_STATE_JUMPING | UNIT_STATE_DISTRACTED | UNIT_STATE_POSSESSED)) /*ignore these states, will not count as hacks*/
		return true;

	if (m_mover->ToPlayer()->GetAreaId() == 4859) // Area: 4859 (Frozen Throne) 
		return true;

	if (m_mover->ToPlayer())
	{
		if (m_mover->ToPlayer()->UnderACKmount())
			return true;

		if (m_mover->ToPlayer()->IsSkipOnePacketForASH())
			return true;
	}
	else
		return true;

	Position npos;
	GetPosition(&npos);
	float pz = npos.GetPositionZ();
	if (!m_mover->ToPlayer()->IsInWater() && m_mover->ToPlayer()->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
	{
		float waterlevel = m_mover->ToPlayer()->GetBaseMap()->GetWaterLevel(npos.GetPositionX(), npos.GetPositionY()); // water walking
		bool hovergaura = m_mover->ToPlayer()->HasAuraType(SPELL_AURA_WATER_WALK) || m_mover->ToPlayer()->HasAuraType(SPELL_AURA_HOVER);
		if (waterlevel && (pz - waterlevel) <= (hovergaura ? GetCollisionHeight(IsMounted()) + 1.5f + m_mover->ToPlayer()->GetHoverOffset() : m_mover->ToPlayer()->GetCollisionHeight(IsMounted()) + m_mover->ToPlayer()->GetHoverOffset()))
			return true;

		TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  FlyHack Detected for Account id : %u, Player %s", m_mover->ToPlayer()->GetSession()->GetAccountId(), m_mover->ToPlayer()->GetName().c_str());
		TC_LOG_INFO("anticheat", "Player::========================================================");
		TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  Player has a MOVEMENTFLAG_SWIMMING, but not in water");
		std::string accusedOf = "swimming movement enabled";
		std::stringstream HackReport;
		HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
		if (HasAura(605))
			HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
		HackReport << "\n|cffFF0000Additional info [debug]|r: player has |cffFF0000MOVEMENTFLG_SWIMMING|r when |cffFF0000not in water|r";
		//no need to port back here
		sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(),LANG_GM_BROADCAST, HackReport.str().c_str());
		return false;
	}
	else
	{
		float z = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY(), pz + GetCollisionHeight(IsMounted()) + 0.5f, true, 50.0f); // smart flyhacks -> SimpleFly
		float diff = pz - z;
		if (diff > 6.8f)
			if (diff > 6.8f + m_mover->ToPlayer()->GetHoverOffset()) // better calculate the second time for false situations, but not call GetHoverOffset everytime (economy resource)
			{
				float waterlevel = GetBaseMap()->GetWaterLevel(npos.GetPositionX(), npos.GetPositionY()); // water walking
				if (waterlevel && waterlevel + m_mover->ToPlayer()->GetCollisionHeight(IsMounted()) + m_mover->ToPlayer()->GetHoverOffset() > pz)
					return true;

				float cx, cy, cz;
				m_mover->ToPlayer()->GetClosePoint(cx, cy, cz, 0.388999998569489f, 6.0f, 0, 6.8f); // first check
				if (pz - cz > 6.8f)
				{
					// check dynamic collision for transport (TODO navmesh for transport map)
					m_mover->ToPlayer()->GetMap()->getObjectHitPos(m_mover->ToPlayer()->GetPhaseMask(), m_mover->ToPlayer()->GetPositionX(), m_mover->ToPlayer()->GetPositionY(), m_mover->ToPlayer()->GetPositionZ() + m_mover->ToPlayer()->GetCollisionHeight(IsMounted()), cx, cy, cz + m_mover->ToPlayer()->GetCollisionHeight(IsMounted()), cx, cy, cz, -m_mover->ToPlayer()->GetCollisionHeight(IsMounted()));

					if (pz - cz > 6.8f)
					{
						TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  FlyHack Detected for Account id : %u, Player %s", m_mover->ToPlayer()->GetSession()->GetAccountId(), m_mover->ToPlayer()->GetName().c_str());
						TC_LOG_INFO("anticheat", "Player::========================================================");
						TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  playerZ = %f", pz);
						TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  normalZ = %f", z);
						TC_LOG_INFO("anticheat", "Player::CheckOnFlyHack :  checkz = %f", cz);

						std::string accusedOf = "Z-axis manipulation (FlyHack)";
						std::stringstream HackReport;
						HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
						if (HasAura(605))
							HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
						HackReport << "\n|cffFF0000Additional info [debug]|r: previous Z: |cffFF0000" << z << "|r current Z: |cffFF0000" << pz << "|r checking Z: |cffFF0000" << cz;
						TeleportTo(AntiHackSave[m_mover->ToPlayer()->GetGUID()].location); //port back
						sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
						return false;
					}
				}
			}
	}

	return true;
}

bool Player::CheckMovementInfo(MovementInfo const& movementInfo, bool jump)
{
	//if (!m_mover || !this || !m_mover->ToPlayer() || this == nullptr)
		//return true;
	if (this == nullptr || this == NULL)
		return true;

	if (isGameMaster())
		return true;

	uint32 oldctime = GetLastMoveClientTimestamp();
	if (oldctime)
	{
		if (m_mover->ToPlayer()->IsFalling() || m_mover->ToPlayer()->isInFlight())
			return true;

		bool vehicle = false;
		if (m_mover->ToPlayer()->GetVehicleKit() && m_mover->ToPlayer()->GetVehicleKit()->GetBase())
			vehicle = true;

		if (m_mover->ToPlayer()->GetVehicle())
			return true;

		if (!m_mover->ToPlayer()->IsControlledByPlayer())
			return true;

		//if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_SLOW) || movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
			//return true; 

		if (m_mover->ToPlayer()->HasUnitState(UNIT_STATE_FLEEING | UNIT_STATE_CONFUSED | UNIT_STATE_CHARGING | UNIT_STATE_JUMPING | UNIT_STATE_DISTRACTED | UNIT_STATE_POSSESSED)) /*ignore these states, will not count as hacks*/
			return true;

		if (m_mover->ToPlayer())
		{
			if (m_mover->ToPlayer()->IsSkipOnePacketForASH())
			{
				m_mover->ToPlayer()->SetSkipOnePacketForASH(false);
				return true;
			}
		}
		else
			return true;

		AntiHackSave[m_mover->ToPlayer()->GetGUID()].location = m_mover->ToPlayer()->GetWorldLocation();

		bool transportflag = (/*(movementInfo.GetMovementFlags() & 0x00000200) && */m_mover->ToPlayer()->GetTransport()); /*transport*/
		float x, y, z;
		Position npos;

		// Position coords for new point
		if (!transportflag)
			npos = movementInfo.pos;
		else
			npos = movementInfo.t_pos;

		// Position coords for previous point (old)
		//         Just CheckMovementInfo are calling before player change UnitMovementFlag MOVEMENTFLAG_ONTRANSPORT
		if (transportflag)
		{
			if (m_mover->ToPlayer()->GetTransOffsetX() == 0.f) // if it elevator or fist step - player can have zero this coord
				return true;

			x = m_mover->ToPlayer()->GetTransOffsetX();
			y = m_mover->ToPlayer()->GetTransOffsetY();
			z = m_mover->ToPlayer()->GetTransOffsetZ();
		}
		else
			m_mover->ToPlayer()->GetPosition(x, y, z);

		if (m_mover->ToPlayer()->HasUnitState(UNIT_STATE_ROOT) && !m_mover->ToPlayer()->UnderACKRootUpd())
		{
			bool unrestricted = npos.GetPositionX() != x || npos.GetPositionY() != y;
			if (unrestricted)
			{
				TC_LOG_INFO("anticheat", "CheckMovementInfo :  Ignore control Hack detected for Account id : %u, Player %s", m_mover->ToPlayer()->GetSession()->GetAccountId(), GetName().c_str());
				std::string accusedOf = "control hack";
				std::stringstream HackReport;
				HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
				if (HasAura(605))
					HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
				HackReport << "\n|cffFF0000Additional info [debug]|r: |cffFF0000player location does not match existing location|r";
				//no need to port back here
				sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
				return false;
			}
		}

		float flyspeed = 0.f;
		float distance, runspeed, difftime, normaldistance, delay, diffPacketdelay;
		uint32 ptime;
		std::string mapname = GetMap()->GetMapName();

		// calculate distance - don't use func, because x,z can be offset transport coords
		distance = sqrt((npos.GetPositionY() - y) * (npos.GetPositionY() - y) + (npos.GetPositionX() - x) * (npos.GetPositionX() - x));

		if (!jump && !m_mover->ToPlayer()->CanFly() && !m_mover->ToPlayer()->isSwimming() && !transportflag && !m_mover->ToPlayer()->IsMounted())
		{
			float diffz = fabs(movementInfo.pos.GetPositionZ() - z);
			float tanangle = distance / diffz;

			if (movementInfo.pos.GetPositionZ() > z && diffz > 1.87f && tanangle < 0.57735026919f) // 30 degrees
			{
				bool skipchecker = false;
				if (((movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || (IsWalking()) && IsAllowedToFall())) || m_mover->ToPlayer()->UnderACKmount()) //if falling & allowed to fall or walking & allowed to fall -> do nothing, permitted
				{
					//sWorld->SendServerMessage(SERVER_MSG_STRING, "falling & allowed!");
					return true;
				}

				if (movementInfo.t_guid != 0)
					skipchecker = true;

				if (skipchecker)
				{
					AntiHackSave[m_mover->ToPlayer()->GetGUID()].location = m_mover->ToPlayer()->GetWorldLocation();
					skipchecker = false;
					return false;
				}

				TC_LOG_INFO("anticheat", "Player::CheckMovementInfo :  Climb-Hack detected for Account id : %u, Player %s, diffZ = %f, distance = %f, angle = %f, Map = %s, mapId = %u, X = %f, Y = %f, Z = %f",
					m_mover->ToPlayer()->GetSession()->GetAccountId(), GetName().c_str(), diffz, distance, tanangle, mapname.c_str(), GetMapId(), x, y, z);
				std::string accusedOf = "climb hack";
				std::stringstream HackReport;
				HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
				if (HasAura(605, m_mover->GetGUID()))
					HackReport.str().append(" (has mind control)");
				HackReport << "\n|cffFF0000Additional info [debug]|r: player's |cffFF0000climbing above 30 degrees|r, impossible without client modification";
				//HackReport << "\n[temp]: distance: " << distance << ", tan-angle: " << tanangle << " diffz: " << diffz;
				m_mover->ToPlayer()->TeleportTo(AntiHackSave[m_mover->ToPlayer()->GetGUID()].location);
				sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
				return false;
			}
		}

		if (GetName() != "")
		{
			GetPosition(&npos);
			float deltaZ = fabs(npos.GetPositionZ() - movementInfo.pos.GetPositionZ());
			float deltaXY = movementInfo.pos.GetExactDist2d(&npos);

			float angle = NormalizeOrientation(tan(deltaZ / deltaXY));

			float engl = npos.GetAngle(&npos);
			float engl1 = npos.GetRelativeAngle(&npos);

			float pz = npos.GetPositionZ();

			float zLocation = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY(), pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly

			float dis_f_ground = pz - zLocation;

			bool skipchecker = false;


			//if (m_mover->ToPlayer()->IsLevitating() || m_mover->ToPlayer()->isHover())
			std::stringstream dadada, possibleAirHacker;
			dadada << "Angle: |cffFF0000" << angle << "|r, player Z (height): |cffFF0000" << pz << "|r, zLocation: |cffFF0000" << zLocation << "|r deltaXY (distance & speed in YD): |cffFF0000" << deltaXY << "|r distance from ground: |cffFF0000" << dis_f_ground << "|r";
			dadada << "engl: |cffFF0000 " << engl << "|r engl1: |cffFF0000" << engl1 << "|r";
			if (AntiHackSave[m_mover->ToPlayer()->GetGUID()].OldZ == 0.f)
				AntiHackSave[m_mover->ToPlayer()->GetGUID()].OldZ = zLocation; //define the new Z for port-down
				//dadada << ", last stored Z: |cffFF0000" << AntiHackSave[m_mover->ToPlayer()->GetGUID()].OldZ;
			//ChatHandler(m_mover->ToPlayer()->GetSession()).PSendSysMessage("%s|r\n--------------------\n\n\n", dadada.str().c_str());

			if (movementInfo.t_guid != 0) //(m_mover->ToPlayer()->FindNearestGameObjectOfType(GameobjectTypes::GAMEOBJECT_TYPE_TRANSPORT, 5.f))
			{
				AntiHackSave[m_mover->ToPlayer()->GetGUID()].location = m_mover->ToPlayer()->GetWorldLocation();
				m_mover->ToPlayer()->SetUnderACKmountTransport();
				skipchecker = true;
			}
			if (//all of these must be true
			m_mover->ToPlayer() 
			&& !m_mover->ToPlayer()->IsCanFlybyServer() 
			&& !m_mover->ToPlayer()->UnderACKmount() 
			&& !m_mover->ToPlayer()->IsInWater() 
			&& dis_f_ground >= 5.f 
			&&
				!(//all of these must be false
					(
						(
						   m_mover->ToPlayer()->HasAuraType(SPELL_AURA_FLY) 
						|| m_mover->ToPlayer()->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) 
						|| m_mover->ToPlayer()->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
						) 
						&& 
						!m_mover->ToPlayer()->IsMounted()
					)
				)
			)
			{
				if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) 
					|| (
						(
							m_mover->ToPlayer()->HasAuraType(SPELL_AURA_FLY) 
							|| m_mover->ToPlayer()->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) 
							|| m_mover->ToPlayer()->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)
						)
							&& m_mover->ToPlayer()->IsMounted()
						) 
					|| m_mover->ToPlayer()->UnderACKmountAdvanced() 
					|| m_mover->ToPlayer()->UnderACKmount() 
					|| ((m_mover->HasAuraType(SPELL_AURA_WATER_WALK) || (m_mover->ToPlayer()->GetCommandStatus(CHEAT_WATERWALK) || (m_mover->isDead() && m_mover->HasAuraType(SPELL_AURA_GHOST)))) && movementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING)) || skipchecker)
				{
					skipchecker = false;
					return true;
				}

				float zLocation_N = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() + 0.85f, npos.GetPositionY(), pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_NE = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() + 0.85f, npos.GetPositionY() - 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_E = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY() - 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_SE = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() - 0.85f, npos.GetPositionY() - 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_S = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() - 0.85f, npos.GetPositionY(), pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_SW = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() - 0.85f, npos.GetPositionY() + 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_W = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY() + 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
				float zLocation_NW = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX() + 0.85f, npos.GetPositionY() + 0.85f, pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly

				float dis_f_ground_N = pz - zLocation_N;
				float dis_f_ground_NE = pz - zLocation_NE;
				float dis_f_ground_E = pz - zLocation_E;
				float dis_f_ground_SE = pz - zLocation_SE;
				float dis_f_ground_S = pz - zLocation_S;
				float dis_f_ground_SW = pz - zLocation_SW;
				float dis_f_ground_W = pz - zLocation_W;
				float dis_f_ground_NW = pz - zLocation_NW;

				if ((dis_f_ground_N > 5.f && dis_f_ground_N < 2000.f)
					&& (dis_f_ground_NE > 5.f && dis_f_ground_NE < 2000.f)
					&& (dis_f_ground_E > 5.f && dis_f_ground_E < 2000.f)
					&& (dis_f_ground_SE > 5.f && dis_f_ground_SE < 2000.f)
					&& (dis_f_ground_S > 5.f && dis_f_ground_S < 2000.f)
					&& (dis_f_ground_SW > 5.f && dis_f_ground_SW < 2000.f)
					&& (dis_f_ground_W > 5.f && dis_f_ground_W < 2000.f)
					&& (dis_f_ground_NW > 5.f && dis_f_ground_NW < 2000.f)
					)
				AntiHackSave[m_mover->ToPlayer()->GetGUID()].WarningsForAirWalking++;
				//ChatHandler(m_mover->ToPlayer()->GetSession()).PSendSysMessage("%u", AntiHackSave[m_mover->ToPlayer()->GetGUID()].WarningsForAirWalking);

				if (AntiHackSave[m_mover->ToPlayer()->GetGUID()].WarningsForAirWalking >= 10)
				{
					std::string accusedOf = "air-walking";
					std::stringstream HackReport;
					HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
					if (HasAura(605))
						HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
					HackReport << "\n|cffFF0000Additional info [debug]|r: player is air-walking (invisible road) - distance from the ground |cffFF0000" << dis_f_ground << "|r without legal fly/flying mount";
					//m_mover->ToPlayer()->TeleportTo(AntiHackSave[m_mover->ToPlayer()->GetGUID()].location); //todo: port down, not to the last location
					sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
					AntiHackSave[m_mover->ToPlayer()->GetGUID()].WarningsForAirWalking = 0;
					m_mover->ToPlayer()->TeleportTo(m_mover->GetWorldLocation().GetMapId(), m_mover->GetWorldLocation().GetPositionX(), m_mover->GetWorldLocation().GetPositionY(), zLocation_N+0.5f, m_mover->GetWorldLocation().GetOrientation());
					AntiHackSave[m_mover->ToPlayer()->GetGUID()].OldZ = 0.f; //reset

					m_mover->ToPlayer()->SetCanFlybyServer(false);
					m_mover->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_CAN_FLY);
					m_mover->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
					m_mover->GetMotionMaster()->MoveFall();
					m_mover->ToPlayer()->SendMovementCanFlyChange();
					//m_mover->ToPlayer()->SetSkipOnePacketForASH(true);
				}
				return false;
			}
			//AntiHackSave[m_mover->ToPlayer()->GetGUID()].OldZ = pz; //store it

		}

		uint32 oldstime = m_mover->ToPlayer()->GetLastMoveServerTimestamp();
		uint32 stime = getMSTime();
		uint32 ping;
		ptime = movementInfo.time;

		if (!vehicle)
			runspeed = m_mover->ToPlayer()->GetSpeed(MOVE_RUN);
		else
			runspeed = m_mover->ToPlayer()->GetVehicleKit()->GetBase()->GetSpeed(MOVE_RUN);

		if (m_mover->ToPlayer()->isSwimming())
		{
			if (!vehicle)
				runspeed = m_mover->ToPlayer()->GetSpeed(MOVE_SWIM);
			else
				runspeed = m_mover->ToPlayer()->GetVehicleKit()->GetBase()->GetSpeed(MOVE_SWIM);
		}

		if (m_mover->ToPlayer()->IsFlying() || m_mover->CanFly())
		{
			if (!vehicle)
				flyspeed = m_mover->ToPlayer()->GetSpeed(MOVE_FLIGHT);
			else
				flyspeed = m_mover->ToPlayer()->GetVehicleKit()->GetBase()->GetSpeed(MOVE_FLIGHT);
		}

		if (flyspeed > runspeed)
			runspeed = flyspeed;

		delay = ptime - oldctime;
		diffPacketdelay = 10000000 - delay;

		if (oldctime > ptime)
		{
			TC_LOG_INFO("anticheat", "oldctime > ptime");
			delay = 0;
		}
		diffPacketdelay = diffPacketdelay * 0.0000000001f;
		difftime = delay * 0.001f + diffPacketdelay;

		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_HOVER | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_DISABLE_GRAVITY))
			m_mover->ToPlayer()->SetUnderACKmountAdvanced(); //give +20 in range/speed allowed

		/*Begin hillside falling bonus calculation*/
		Player* player = m_mover->ToPlayer();
		Position playerPos;
		player->GetPosition(&playerPos);
		float deltaZ = fabs(playerPos.GetPositionZ() - movementInfo.pos.GetPositionZ());
		float deltaXY = movementInfo.pos.GetExactDist2d(&playerPos);
		float angle = player->NormalizeOrientation(tan(deltaZ / deltaXY));
		float pz = npos.GetPositionZ();
		float zLocation = m_mover->ToPlayer()->GetMap()->GetHeight(m_mover->ToPlayer()->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY(), pz + m_mover->ToPlayer()->GetCollisionHeight(m_mover->ToPlayer()->IsMounted()) + 0.5f, true, MAX_FALL_DISTANCE); // smart flyhacks -> SimpleFly
		float dis_f_ground = pz - zLocation;


		// if movetime faked and lower, difftime should be with "-"
		normaldistance = (runspeed * difftime) + 0.500f; // 0.500f a little safe temporary hack
		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_SLOW) && movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING)) //if falling OR falling & slowfall
			normaldistance += 15.f; //momentum prevention
		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING) && movementInfo.HasMovementFlag(MOVEMENTFLAG_HOVER | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_DISABLE_GRAVITY))
			normaldistance += 60.f;
		if (m_mover->ToPlayer()->UnderACKmount())
			normaldistance += 1.0f;
		if (m_mover->ToPlayer()->UnderACKmountAdvanced())
			normaldistance += 25.0f;
		if (m_mover->ToPlayer()->UnderACKmountTransport())
			normaldistance += 850.0f;
		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) //player is falling
			&& angle > 1.0f //angle is acceptably steep
			&& dis_f_ground < 1.f) //player is right next to the ground
			normaldistance += 5.f;
		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) && m_mover->GetZoneId() == 4378)//dalaran sewers
		{
			m_mover->ToPlayer()->SetSkipOnePacketForASH(true);
			return true;
		}
		if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) && m_mover->GetZoneId() == 5638)//throne of the four winds
		{
			m_mover->ToPlayer()->SetSkipOnePacketForASH(true);
			return true;
		}

		if (distance < normaldistance || normaldistance < 0.f /*disallow negative "normal" distance*/ || movementInfo.t_guid != 0)
			return true;

		ping = uint32(diffPacketdelay * 10000.f);

		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  SpeedHack Detected for Account id : %u, Player %s", m_mover->ToPlayer()->GetSession()->GetAccountId(), m_mover->ToPlayer()->GetName().c_str());
		TC_LOG_INFO("anticheat", "Unit::========================================================");
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  oldX = %f", x);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  oldY = %f", y);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  newX = %f", npos.GetPositionX());
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  newY = %f", npos.GetPositionY());
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  packetdistance = %f", distance);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  available distance = %f", normaldistance);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  oldStime = %u", oldstime);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  oldCtime = %u", oldctime);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  serverTime = %u", stime);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  packetTime = %u", ptime);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  diff delay between old ptk and current pkt = %f", diffPacketdelay);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  FullDelay = %f", delay / 1000.f);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  difftime = %f", difftime);
		TC_LOG_INFO("anticheat", "Unit::CheckMovementInfo :  ping = %u", ping);
		std::string accusedOf = "speed hacking";
		std::stringstream HackReport;
		HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(m_mover->ToPlayer()->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << m_mover->ToPlayer()->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
		if (HasAura(605))
			HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(m_mover->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
		HackReport << "\n|cffFF0000Additional info [debug]|r: distance traveled: |cffFF0000" << distance << "|r, distance allowed: |cffFF0000" << normaldistance << "|r, ping: |cffFF0000" << ping;
		TeleportTo(AntiHackSave[m_mover->ToPlayer()->GetGUID()].location); //port back
		sWorld->SendAntiCheat(m_mover->ToPlayer()->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
		return false;
	}
	else
		return true;

	return false;
}