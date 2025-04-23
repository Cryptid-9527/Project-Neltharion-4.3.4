/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "AnticheatMgr.h"
#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Transport.h"
#include "Battleground.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "MovementStructures.h"
#include "InstanceScript.h"

struct SavedLocation
{
    WorldLocation location;
    uint32 lastJump = 0;
};

static std::map<uint64, SavedLocation> AntiHackSave;

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket& recvPacket)
{
    HandleMoveWorldportAckOpcode();
    recvPacket.rfinish();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    WorldLocation const& loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        LogoutPlayer(false);
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.GetMapId());
    InstanceTemplate const* mInstance = sObjectMgr->GetInstanceTemplate(loc.GetMapId());

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    Map* oldMap = GetPlayer()->GetMap();
    if (GetPlayer()->IsInWorld())
    {
        TC_LOG_ERROR("network.opcode", "Player (Name %s) is still in world when teleported from map %u to new map %u", GetPlayer()->GetName().c_str(), oldMap->GetId(), loc.GetMapId());
        oldMap->RemovePlayerFromMap(GetPlayer(), false);
    }

    // relocate the player to the teleport destination
    Map* newMap = sMapMgr->CreateMap(loc.GetMapId(), GetPlayer());
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || !newMap->CanEnter(GetPlayer()))
    {
        TC_LOG_ERROR("network.opcode", "Map %d could not be created for player %d, porting player to homebind", loc.GetMapId(), GetPlayer()->GetGUIDLow());
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }
    else
        GetPlayer()->Relocate(&loc);

    GetPlayer()->ResetMap();
    GetPlayer()->SetMap(newMap);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    if (!GetPlayer()->GetMap()->AddPlayerToMap(GetPlayer()))
    {
        TC_LOG_ERROR("network.opcode", "WORLD: failed to teleport player %s (%d) to map %d because of unknown reason!",
            GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow(), loc.GetMapId());
        GetPlayer()->ResetMap();
        GetPlayer()->SetMap(oldMap);
        GetPlayer()->TeleportTo(GetPlayer()->m_homebindMapId, GetPlayer()->m_homebindX, GetPlayer()->m_homebindY, GetPlayer()->m_homebindZ, GetPlayer()->GetOrientation());
        return;
    }

    if (_player->InBattleground())
    {
        if (!mEntry->IsBattlegroundOrArena())
        {
            // remove bg team and id when leaving bg
            _player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);
            _player->SetBGTeam(0);
        }
        else if (auto bg = _player->GetBattleground())
        {
            if (_player->IsInvitedForBattlegroundInstance(_player->GetBattlegroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if (!_player->InBattleground())
        {
            // short preparations to continue flight
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Initialize(GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    Corpse* corpse = GetPlayer()->GetCorpse();
    if (corpse && corpse->GetType() != CORPSE_BONES && corpse->GetMapId() == GetPlayer()->GetMapId() && mEntry->IsDungeon())
    {
        GetPlayer()->ResurrectPlayer(0.5f, false);
        GetPlayer()->SpawnCorpseBones();
    }

    bool allowMount = !mEntry->IsDungeon() || mEntry->IsBattlegroundOrArena();
    if (mInstance)
    {
        Difficulty diff = GetPlayer()->GetDifficulty(mEntry->IsRaid());
        if (MapDifficulty const* mapDiff = GetMapDifficultyData(mEntry->MapID, diff))
        {
            if (mapDiff->resetTime)
            {
                if (time_t timeReset = sInstanceSaveMgr->GetResetTimeFor(mEntry->MapID, diff))
                {
                    uint32 timeleft = uint32(timeReset - time(NULL));
                    GetPlayer()->SendInstanceResetWarning(mEntry->MapID, diff, timeleft);
                }
            }
        }
        allowMount = mInstance->AllowMount;
    }

    if (!allowMount)
    {
        _player->RemoveAurasByType(SPELL_AURA_MOUNTED);
        _player->RemoveAurasByType(SPELL_AURA_FLY);
    }
    else if (mEntry->IsDungeon())
    {
        _player->RemoveAurasByType(SPELL_AURA_FLY);
    }

    // update zone immediately, otherwise leave channel will cause crash in mtmap
    uint32 newzone, newarea;
    GetPlayer()->GetZoneAndAreaId(newzone, newarea);
    GetPlayer()->UpdateZone(newzone, newarea);

    if (GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true); // honorless target
    else if (GetPlayer()->IsPvP() && !GetPlayer()->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TOGGLE_STATUS))
        GetPlayer()->UpdatePvP(false, false);

    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recvPacket)
{
    ObjectGuid guid;
    uint32 ackCount = recvPacket.read<uint32>();
    recvPacket.read_skip<uint32>(); // should be new position in kind of struction

    guid[5] = recvPacket.ReadBit();
    guid[0] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[3] = recvPacket.ReadBit();
    guid[7] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[7]);
    recvPacket.ReadByteSeq(guid[6]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[0]);

    // Skip old result
    if (_player->m_movement_ack[ACK_TELEPORT] != ackCount)
        return;

    Player* plMover = _player->m_mover->ToPlayer();

    if (!plMover || !plMover->IsBeingTeleportedNear() || guid != plMover->GetGUID())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();
    plMover->UpdatePosition(dest, true);

    WorldPacket data(SMSG_MOVE_UPDATE_TELEPORT);
    MovementInfo info = MovementInfo(plMover->m_movementInfo);
    info.pos.Relocate(dest.m_positionX, dest.m_positionY, dest.m_positionZ, dest.m_orientation);
    info.guid = plMover->GetGUID();
    info.time = getMSTime();
    info.WriteToPacket(data);
    _player->SendMessageToSet(&data, _player);

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    if (old_zone != newzone)
    {
        if (plMover->pvpInfo.inHostileArea)
            plMover->CastSpell(plMover, 2479, true); // honorless target
        else if (plMover->IsPvP() && !plMover->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_TOGGLE_STATUS))
            plMover->UpdatePvP(false, false);
    }

    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleSetActiveMover(WorldPacket& recvPacket)
{
    ObjectGuid guid;

    guid[7] = recvPacket.ReadBit();
    guid[2] = recvPacket.ReadBit();
    guid[1] = recvPacket.ReadBit();
    guid[0] = recvPacket.ReadBit();
    guid[4] = recvPacket.ReadBit();
    guid[5] = recvPacket.ReadBit();
    guid[6] = recvPacket.ReadBit();
    guid[3] = recvPacket.ReadBit();

    recvPacket.ReadByteSeq(guid[3]);
    recvPacket.ReadByteSeq(guid[2]);
    recvPacket.ReadByteSeq(guid[4]);
    recvPacket.ReadByteSeq(guid[0]);
    recvPacket.ReadByteSeq(guid[5]);
    recvPacket.ReadByteSeq(guid[1]);
    recvPacket.ReadByteSeq(guid[6]);
    recvPacket.ReadByteSeq(guid[7]);

    if (_player->m_mover->GetGUID() != guid)
    {
        recvPacket.rfinish();
        return;
    }
    recvPacket.rfinish();
    return;
}

void WorldSession::HandleMovementOpcodes(WorldPacket& recvPacket)
{
    if (!_player || !this || this == nullptr || this == NULL || recvPacket.empty())
        return;
    ObjectGuid guid;
    uint16 opcode = recvPacket.GetOpcode();
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    guid = info.guid;
    //Stop Emote
    if (GetPlayer()->GetUInt32Value(UNIT_NPC_EMOTESTATE))
        GetPlayer()->HandleEmoteCommand(0);

    Unit* mover = _player->m_mover;
    ASSERT(mover != nullptr);                      // there must always be a mover

    Player* plrMover = mover->ToPlayer();
    
    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (plrMover && plrMover->IsBeingTeleported())
    {
        recvPacket.rfinish();                     // prevent warnings spam
        return;
    }

    if (_player->m_mover->GetGUID() != guid)
    {
        recvPacket.rfinish();
        //ChatHandler(_player->GetSession()).PSendSysMessage("error(1): GUIDs %u and %u TEST %u", info.guid, guid);
        return;
    }

    if (_player)
    {
        if (_player->needsGMFreeze())
        {
            if (!_player->hasGMFreeze(true))
                _player->applyGMFreeze(true);

            if (info.pos.GetExactDist(_player->m_gmFreeze_location.GetPositionX(),
                _player->m_gmFreeze_location.GetPositionY(),
                _player->m_gmFreeze_location.GetPositionZ()) > 5.f
                || _player->m_gmFreeze_location.GetMapId() != _player->GetMapId())
            {
                _player->TeleportTo(_player->m_gmFreeze_location);
            }
        }
        else
            if (_player->hasGMFreeze(false))
                _player->applyGMFreeze(false);
    }

            

    if (plrMover && !info.HasMovementFlag(MOVEMENTFLAG_FALLING) && !plrMover->IsFlying() && plrMover->IsAllowedToFall()) //when dismounted or a legit spell effect is being used
    {
        plrMover->SetUnderACKmountAdvanced(); //4.5s, +20yd
        plrMover->SetIsAllowedToFall(false); //normally you cannot just fall (or go up), you need to have a special situation - spell, mount
    }

    if (plrMover && plrMover->IsUnderLastChanceForLandOrSwimOpcode())
    {
        bool checkNorm = false;
        switch (opcode)
        {
        case MSG_MOVE_FALL_LAND:
        case 0x0CA: //MSG_MOVE_START_SWIM
            checkNorm = true;
            plrMover->SetIsAllowedToFall(false);
            break;
        }

        if (plrMover && plrMover->IsCanFlybyServer())
            checkNorm = true;

        if (plrMover && !checkNorm && !plrMover->IsMounted())
        {
            if (plrMover->isSwimming() || plrMover->IsJumping() || plrMover->isGameMaster()) //if falling into water, set as successfully landed, skip checking for this "hack"
            {
                checkNorm = true;
                plrMover->SetSuccessfullyLanded();
                plrMover->SetUnderACKmount();
                return;
            }
            if (plrMover->GetShapeshiftForm() == FORM_FLIGHT || plrMover->GetShapeshiftForm() == FORM_FLIGHT_EPIC || info.HasMovementFlag(MOVEMENTFLAG_HOVER | MOVEMENTFLAG_FALLING_SLOW | MOVEMENTFLAG_DISABLE_GRAVITY)) //when flying shapeshifting, no nofalldmg messages
                return;

            //this is annoying and does nothing.
            /*
            TC_LOG_INFO("anticheat", "MovementHandler::NoFallingDamage by Account id : %u, Player %s", plrMover->GetSession()->GetAccountId(), plrMover->GetName().c_str());
            std::string accusedOf = "NoFallDamage";
            std::stringstream HackReport;
            HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(plrMover->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << plrMover->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
            if (_player->HasAura(605))
                HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(_player->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
            HackReport << "\n|cffFF0000Additional info [debug]|r: player has |cffFF0000no fall damage hack enabled|r";
            sWorld->SendGMText(LANG_GM_BROADCAST, HackReport.str().c_str());
            */
            recvPacket.rfinish();                     // prevent warnings spam
        }
        else
        {
            if (plrMover)
            {
                plrMover->SetSuccessfullyLanded();
                plrMover->SetUnderACKmount();
            }
        }
    }
   
    recvPacket.rfinish();

    if (plrMover && opcode == CMSG_MOVE_CHNG_TRANSPORT)
        plrMover->SetSkipOnePacketForASH(true);

    if (!info.pos.IsPositionValid())
    {
        if (plrMover)
        {
            plrMover->SetSkipOnePacketForASH(true);
            plrMover->UpdateMovementInfo(info);
        }
        return;
    }

    if (!mover->movespline->Finalized())
        return;

    /* handle special cases */
    if (plrMover && (plrMover->GetTransport() || plrMover->GetLinearTransport()))
    {
        // We were teleported, skip packets that were broadcast before teleport
        if (info.pos.GetExactDist2d(mover) > SIZE_OF_GRIDS)
        {
            plrMover->SetSkipOnePacketForASH(true);
            plrMover->UpdateMovementInfo(info);
            return;
        }

        if (!Trinity::IsValidMapCoord(info.pos.GetPositionX() + info.t_pos.GetPositionX(), info.pos.GetPositionY() + info.t_pos.GetPositionY(), info.pos.GetPositionZ() + info.t_pos.GetPositionZ(), info.pos.GetOrientation() + info.t_pos.GetOrientation()))
        {

            plrMover->SetSkipOnePacketForASH(true);
            plrMover->UpdateMovementInfo(info);
            return;
        }

        // if we boarded a transport, add us to it
        if (plrMover)
        {
            if (!plrMover->GetTransport() && !plrMover->GetLinearTransport())
            {
                if (Transport* transport = plrMover->GetTransport())
                    transport->AddPassenger(plrMover);
                if (auto ltransport = plrMover->GetLinearTransport())
                    ltransport->AddPassenger(plrMover);
            }
            else
            {
                if (plrMover->GetTransport())
                {
                    if (plrMover->GetTransport()->GetGUID() != info.t_guid)
                    {
                        plrMover->SetSkipOnePacketForASH(true);
                        plrMover->GetTransport()->RemovePassenger(plrMover);
                        if (Transport* transport = plrMover->GetTransport())
                            transport->AddPassenger(plrMover);
                        else
                        {
                            /* reset */
                            info.t_guid = 0;
                            info.t_pos.Relocate(0.f, 0.f, 0.f, 0.f);
                            info.t_seat = -1;
                            info.t_time = 0;
                            info.t_time2 = 0;
                        }
                    }
                }
                else
                {
                    if (plrMover->GetLinearTransport()->GetGUID() != info.t_guid)
                    {
                        plrMover->SetSkipOnePacketForASH(true);
                        plrMover->GetLinearTransport()->RemovePassenger(plrMover);
                        if (auto transport = plrMover->GetLinearTransport())
                            transport->AddPassenger(plrMover);
                        else
                        {
                            /* reset */
                            info.t_guid = 0;
                            info.t_pos.Relocate(0.f, 0.f, 0.f, 0.f);
                            info.t_seat = -1;
                            info.t_time = 0;
                            info.t_time2 = 0;
                        }
                    }
                }
            }
        }

        if (!mover->GetTransport() && !mover->GetVehicle())
        {
            GameObject* go = mover->GetMap()->GetGameObject(info.t_guid);
            if (!go || go->GetGoType() != GAMEOBJECT_TYPE_TRANSPORT)
                info.RemoveMovementFlag(0x00000200);
        }
    }

    else if (plrMover && (plrMover->GetTransport() || plrMover->GetLinearTransport()))                // if we were on a transport, leave
    {
        plrMover->SetUnderACKmountTransport();
        if (plrMover->GetTransport())
            plrMover->GetTransport()->RemovePassenger(plrMover);
        else plrMover->GetLinearTransport()->RemovePassenger(plrMover);
        /* reset */
        info.t_guid = 0;
        info.t_pos.Relocate(0.f, 0.f, 0.f, 0.f);
        info.t_seat = -1;
        info.t_time = 0;
        info.t_time2 = 0;
    }

    // start falling time
    if (plrMover && !plrMover->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR) && info.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
        plrMover->ResetFallingData(info.pos.GetPositionZ());

    // check on NoFallingDamage
    if (plrMover && plrMover->HasUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR) && !info.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
    {
        if (!plrMover->IsCanFlybyServer())
        {
            bool checkNorm = false;
            switch (opcode)
            {
            case MSG_MOVE_FALL_LAND:
            case 0x0CA: /*MSG_MOVE_START_SWIM*/
                checkNorm = true;
                break;
            }

            if (!checkNorm && !plrMover->IsWaitingLandOrSwimOpcode())
                plrMover->StartWaitingLandOrSwimOpcode();
        }
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->isInFlight() && !plrMover->IsFlying())
    {
        plrMover->HandleFall(info);
        plrMover->SetJumpingbyOpcode(false);
    }

    // interrupt parachutes upon falling or landing in water
    if (opcode == MSG_MOVE_FALL_LAND || opcode == 0x0CA /* MSG_MOVE_START_SWIM */)
    {
        mover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_LANDING); // Parachutes
        if (plrMover)
        {
            if (plrMover->IsWaitingLandOrSwimOpcode() || plrMover->IsUnderLastChanceForLandOrSwimOpcode())
            {
                plrMover->SetSuccessfullyLanded();
                plrMover->SetUnderACKmount();
            }
            plrMover->SetJumpingbyOpcode(false);
            plrMover->ResetFallingData(info.pos.GetPositionZ()); // for MSG_MOVE_START_SWIM (no HandleFall(movementInfo))
        }
    }

    if (plrMover && ((info.flags & MOVEMENTFLAG_SWIMMING) != 0) != plrMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plrMover->SetInWater(!plrMover->IsInWater() || plrMover->GetBaseMap()->IsUnderWater(info.pos.GetPositionX(), info.pos.GetPositionY(), info.pos.GetPositionZ()));
    }

    if (plrMover)
    {
        sAnticheatMgr->StartHackDetection(plrMover, info, opcode);
        AntiHackSave[plrMover->GetGUID()].location = plrMover->GetWorldLocation();
    }
    Position npos;
    GetPlayer()->GetPosition(&npos);
    float pz = npos.GetPositionZ();
    //float zLocation = plrMover->GetMap()->GetHeight(plrMover->GetPhaseMask(), npos.GetPositionX(), npos.GetPositionY(), pz + plrMover->GetCollisionHeight(plrMover->IsMounted()) + 0.5f, true, 50.0f); // smart flyhacks -> SimpleFly

    if (plrMover)
    if (!plrMover->isGameMaster())
    if (plrMover && plrMover->IsFalling() && (info.pos.GetPositionZ() > mover->GetPositionZ()))
    {
        if (!plrMover->IsJumpingbyOpcode())
        {
            plrMover->SetJumpingbyOpcode(true);
            plrMover->SetUnderACKmount();
        }
        else if (!plrMover->UnderACKmount() && !plrMover->IsFlying() && !plrMover->IsInWater())
        {
            // fake jumper -> for example gagarin air mode with falling flag (like player jumping), but client can't sent a new coords when falling
            TC_LOG_INFO("anticheat", "MovementHandler::Fake_Jumper by Account id : %u, Player %s", plrMover->GetSession()->GetAccountId(), plrMover->GetName().c_str());
            plrMover->TeleportTo(AntiHackSave[plrMover->GetGUID()].location);

            std::string accusedOf = "fake jumping";
            std::stringstream HackReport;
            HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(plrMover->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << plrMover->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
            if (_player->HasAura(605))
                HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(_player->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
            HackReport << "\n|cffFF0000Additional info [debug]|r: player has |cffFF0000jumped illegally|r";
            sWorld->SendAntiCheat(plrMover->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
            return;
        }
    }

    bool jumpopcode = false;
    if (opcode == MSG_MOVE_JUMP)
    {
        jumpopcode = true;       
        if (plrMover)
        {
            plrMover->ProcJumpAuras();

            plrMover->SetLastJump();
            plrMover->SetUnderACKmount();
            if (!plrMover->isGameMaster())
            if (plrMover->IsFalling() && !plrMover->IsJumpingbyOpcode())
            {
                if (plrMover->GetTransport() || plrMover->GetLinearTransport())
                    return; //can cause false/positive while on transport
                TC_LOG_INFO("anticheat", "MovementHandler::DOUBLE_JUMP by Account id : %u, Player %s", plrMover->GetSession()->GetAccountId(), plrMover->GetName().c_str());
                plrMover->TeleportTo(AntiHackSave[plrMover->GetGUID()].location);

                std::string accusedOf = "double jumping";
                std::stringstream HackReport;
                HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(plrMover->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << plrMover->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
                if (_player->HasAura(605))
                    HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(_player->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
                HackReport << "\n|cffFF0000Additional info [debug]|r: player has |cffFF0000jumped multiple times|r (|cffFF0000FlyHack|r)";
                sWorld->SendAntiCheat(plrMover->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
                return;
            }
        }
    }

    if (plrMover)
    if (!plrMover->isGameMaster())
    if (plrMover && !plrMover->IsCanFlybyServer() && !plrMover->UnderACKmount() && info.HasMovementFlag(MOVEMENTFLAG_MASK_MOVING_FLY) && !plrMover->IsInWater() && !plrMover->HasAuraType(SPELL_AURA_FLY) &&
        !(((plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || plrMover->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED)) && plrMover->IsMounted())))
    {
        if (plrMover->GetShapeshiftForm() == FORM_FLIGHT || plrMover->GetShapeshiftForm() == FORM_FLIGHT_EPIC)
            return;

        TC_LOG_INFO("anticheat", "MovementHandler::Fake_flying mode (using MOVEMENTFLAG_FLYING flag doesn't restricted) by Account id : %u, Player %s", plrMover->GetSession()->GetAccountId(), plrMover->GetName().c_str());

        plrMover->TeleportTo(AntiHackSave[plrMover->GetGUID()].location);

        std::string accusedOf = "fly hacking";
        std::stringstream HackReport;
        HackReport << "[|cffFF0000Project Neltharion|r]: character " << ChatHandler(plrMover->GetSession()).GetNameLink() << "|r (account ID: |cffFF0000" << plrMover->GetSession()->GetAccountId() << "|r) is a possible cheater, accused of |cffFF0000" << accusedOf << "|r";
        if (_player->HasAura(605))
            HackReport << " (|cffFF0000has mind control|r), controlled by: " << ChatHandler(_player->GetAffectingPlayer()->ToPlayer()->GetSession()).GetNameLink();
        HackReport << "\n|cffFF0000Additional info [debug]|r: player is |cffFF0000fly hacking|r";
        sWorld->SendAntiCheat(plrMover->GetSession(), LANG_GM_BROADCAST, HackReport.str().c_str());
        _player->SetCanFlybyServer(false);
        info.RemoveMovementFlag(MOVEMENTFLAG_CAN_FLY);
        _player->SendMovementCanFlyChange();
        return;
    }

    plrMover->CheckMovementInfo(info, jumpopcode); //initiate speed hack checkups


    if (plrMover)
    {
        if (!plrMover->HasWalkingFlag() && info.HasMovementFlag(MOVEMENTFLAG_WALKING))
            plrMover->SetWalkingFlag(true);
        if (plrMover->HasWalkingFlag() && !info.HasMovementFlag(MOVEMENTFLAG_WALKING))
            plrMover->SetWalkingFlag(false);
    }

    /* process position-change */
    if (plrMover)
        plrMover->UpdateMovementInfo(info);

    /* process position-change - implement later, anticheat first!

    WorldPacket data(recvPacket.GetOpcode(), recvPacket.size());
    int64 movementTime = (int64)info.time + _timeSyncClockDelta;
    if (_timeSyncClockDelta == 0 || movementTime < 0 || movementTime > 0xFFFFFFFF)
    {
        TC_LOG_WARN("misc", "The computed movement time using clockDelta is erronous. Using fallback instead");
        info.time = getMSTime();
    }
    else
    {
        info.time = (uint32)movementTime;
    }
    info.guid = mover->GetGUID();
    info.WriteToPacket(data);
    mover->SendMessageToSet(&data, _player);
    mover->m_movementInfo = info;
    // Some vehicles allow the passenger to turn by himself
    if (Vehicle* vehicle = mover->GetVehicle())
    {
        if (VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(mover))
        {
            if (seat->m_flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING)
            {
                if (info.pos.GetOrientation() != mover->GetOrientation())
                {
                    mover->SetOrientation(info.pos.GetOrientation());
                    mover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_TURNING);
                }
            }
        }
        return;
    }
    mover->UpdatePosition(info.pos);
    /*
    if (plrMover)                                            // nothing is charmed, or player charmed
    {
        if (plrMover->IsSitState() && (info.flags & (MOVEMENTFLAG_MASK_MOVING | MOVEMENTFLAG_MASK_TURNING)))
            plrMover->SetStandState(UNIT_STAND_STATE_STAND);
        if (!info.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
            plrMover->UpdateFallInformationIfNeed(info, opcode); // don't use SetFallInformation
        if (info.pos.GetPositionZ() < plrMover->GetMap()->GetHeight(info.pos.GetPositionX(), info.pos.GetPositionY(), info.pos.GetPositionZ()))
        {
            if (!(plrMover->GetBattleground() && plrMover->GetBattleground()->HandlePlayerUnderMap(_player)))
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                /// @todo discard movement packets after the player is rooted
                if (plrMover->isAlive())
                {
                    plrMover->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
                    plrMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
                    // player can be alive if GM/etc
                    // change the death state to CORPSE to prevent the death timer from
                    // starting in the next player update
                    if (plrMover->isAlive())
                        plrMover->KillPlayer();
                }
            }
        }
        else
            plrMover->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_IS_OUT_OF_BOUNDS);
    }*/

    if (plrMover)
        AntiHackSave.erase(plrMover->GetGUID());

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size(), _player->m_mover);
}

bool WorldSession::CanMovementBeProcessed(uint16 opcode)
{
    switch (opcode)
    {
        case CMSG_MOVE_NOT_ACTIVE_MOVER:
        case CMSG_FORCE_MOVE_ROOT_ACK:
        case MSG_MOVE_STOP:
            return true;
    }

    return false;
}

bool WorldSession::HandleMovementInfo(MovementInfo &movementInfo, const uint16 opcode, const size_t packSize, Unit *mover)
{
    ASSERT(mover != NULL);

    if (movementInfo.guid != mover->GetGUID() || !movementInfo.pos.IsPositionValid())
        return false;

    Player* plrMover = mover->ToPlayer();

    if (plrMover && plrMover->IsBeingTeleported())
        return false;

    if (plrMover && plrMover->IsSplineEnabled() && !CanMovementBeProcessed(opcode))
        return false;

    if (plrMover && opcode == CMSG_MOVE_NOT_ACTIVE_MOVER || plrMover && !plrMover->CanFreeMove())
        movementInfo.SanitizeMC(mover);
    else
        movementInfo.Sanitize(mover);

    if (movementInfo.t_guid)
    {
        bool stopTransportSeq = false;
        if (mover && mover->GetMapId() == 607)
            if (!mover->GetTransport() && !mover->GetVehicle())
            {
                if (!mover->GetTransport() && !mover->GetVehicle())
                {
                    GameObject* go = mover->GetMap()->GetGameObject(movementInfo.t_guid);
                    if (go && go->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT)
                    {
                        if (go->GetGoState() != GO_STATE_TRANSPORT_SPEC)
                            return false;
                    }
                    else
                        return false;
                }
            }
        if (movementInfo.t_pos.GetPositionX() > 50.0f || movementInfo.t_pos.GetPositionY() > 50.0f || movementInfo.t_pos.GetPositionZ() > 50.0f)
        {
            if (!mover->GetTransport() && !mover->GetVehicle())
            {
                GameObject* go = mover->GetMap()->GetGameObject(movementInfo.t_guid);
                if (go && go->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT)
                {
                    movementInfo.t_guid = 0;
                    stopTransportSeq = true;
                }
                else
                    return false;
            }
            else
                return false; // moved out of transport size
        }

        if (!Trinity::IsValidMapCoord(
            movementInfo.pos.GetPositionX() + movementInfo.t_pos.GetPositionX(),
            movementInfo.pos.GetPositionY() + movementInfo.t_pos.GetPositionY(),
            movementInfo.pos.GetPositionZ() + movementInfo.t_pos.GetPositionZ(),
            movementInfo.pos.GetOrientation() + movementInfo.t_pos.GetOrientation())
            )
            return false; // just bad coords

        if (plrMover)
        {
            if (!Trinity::IsValidMapCoord(
                movementInfo.pos.GetPositionX() + movementInfo.t_pos.GetPositionX(),
                movementInfo.pos.GetPositionY() + movementInfo.t_pos.GetPositionY(),
                movementInfo.pos.GetPositionZ() + movementInfo.t_pos.GetPositionZ(),
                movementInfo.pos.GetOrientation() + movementInfo.t_pos.GetOrientation())
                )
                return false; // just bad coords

            if (plrMover)
            {
                if (!plrMover->GetTransport()) // didn't have transport. now have (why is this never called on instance map ? )
                {
                    for (MapManager::TransportSet::const_iterator iter = sMapMgr->m_Transports.begin(); iter != sMapMgr->m_Transports.end(); ++iter)
                    {
                        if ((*iter)->GetGUID() == movementInfo.t_guid)
                            if (!(*iter)->AsToBeDeleted())
                            {
                                plrMover->m_transport = *iter;
                                (*iter)->AddPassenger(plrMover);
                                break;
                            }
                    }
                }
                else if (plrMover->GetTransport()->GetGUID() != movementInfo.t_guid) // changes transport
                {
                    bool foundNewTransport = false;
                    if (!plrMover->m_transport->AsToBeDeleted())
                        plrMover->m_transport->RemovePassenger(plrMover);
                    for (MapManager::TransportSet::const_iterator iter = sMapMgr->m_Transports.begin(); iter != sMapMgr->m_Transports.end(); ++iter)
                    {
                        if ((*iter)->GetGUID() == movementInfo.t_guid)
                            if (!(*iter)->AsToBeDeleted())
                            {
                                foundNewTransport = true;
                                plrMover->m_transport = *iter;
                                (*iter)->AddPassenger(plrMover);
                                break;
                            }
                    }

                    if (!foundNewTransport)
                    {
                        plrMover->m_transport = NULL;
                        movementInfo.t_pos.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
                        movementInfo.t_time = 0;
                        movementInfo.t_seat = -1;
                    }
                }
                /*                else if (movementInfo.pos.m_positionX != movementInfo.t_pos.m_positionX)
                    plrMover->GetTransport()->UpdatePosition(movementInfo.pos.m_positionX, movementInfo.pos.m_positionY,
                    movementInfo.pos.m_positionZ, movementInfo.pos.m_orientation);*/
            }

            if (!mover->GetTransport() && !mover->GetVehicle())
            {
                GameObject* go = mover->GetMap()->GetGameObject(movementInfo.t_guid);
                if (!go || go->GetGoType() != GAMEOBJECT_TYPE_TRANSPORT)
                    movementInfo.t_guid = 0;
            }
        }
    }
    else if (plrMover && plrMover->GetTransport()) // had transport, no loger have
    {
        if (!plrMover->m_transport->AsToBeDeleted())
            plrMover->m_transport->RemovePassenger(plrMover);
        plrMover->m_transport = NULL;
        movementInfo.t_pos.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
        movementInfo.t_time = 0;
        movementInfo.t_seat = -1;
    }

    if (plrMover && plrMover->GetMapId() == 754)
    {
        if (InstanceScript *instance = plrMover->GetInstanceScript())
        {
            if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->isInFlight())
                if (!instance->IsFallDamageDisable())
                    plrMover->HandleFall(movementInfo);
        }
    }
    else if (opcode == MSG_MOVE_FALL_LAND && plrMover && !plrMover->isInFlight() && plrMover->GetMapId() != 861 && !plrMover->HasUnitState(UNIT_STATE_CHARGING))
        plrMover->HandleFall(movementInfo);

    if (plrMover && ((movementInfo.flags & MOVEMENTFLAG_SWIMMING) != 0) != plrMover->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        plrMover->SetInWater(!plrMover->IsInWater() || plrMover->GetBaseMap()->IsUnderWater(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY(), movementInfo.pos.GetPositionZ()));
    }
	
	if (plrMover)
        sAnticheatMgr->StartHackDetection(plrMover, movementInfo, opcode);

    WorldPacket data(movementInfo.GetSMSGOpcodeForCMSG(Opcodes(opcode)), packSize);
    movementInfo.time = getMSTime();
    movementInfo.guid = mover->GetGUID();
    movementInfo.WriteToPacket(data);
    _player->SendMessageToSet(&data, _player);

    mover->m_movementInfo = movementInfo;

    if (mover->GetVehicle()) // this is almost never true (not sure why it is sometimes, but it is), normally use mover->IsVehicle()
    {
        mover->SetOrientation(movementInfo.pos.GetOrientation());
        return true;
    }

    // Remove pet when starting to fly
    if (plrMover && plrMover->IsFlying() && plrMover->GetAreaId() != 5893)
        plrMover->UnsummonPetTemporaryIfAny();

    mover->UpdatePosition(movementInfo.pos);

    if (plrMover)
    {
        if (!movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
            plrMover->UpdateFallInformationIfNeed(movementInfo.pos.GetPositionZ()); // don't use SetFallInformation
        float underMapValueZ, upperLimitValueZ;
        bool check = false;
        switch (plrMover->GetMapId())
        {
            case 617: // Dalaran Arena
                underMapValueZ = 3.0f;
                upperLimitValueZ = 30.0f;
                break;
            case 562: // Blades Edge Arena
                underMapValueZ = -1.0f;
                upperLimitValueZ = 22.0f;
                break;
            case 559: // Nagrand Arena
                underMapValueZ = -1.0f;
                upperLimitValueZ = 21.0f;
                break;
            case 572: // Ruins of Lordaeron
                underMapValueZ = -1.0f;
                upperLimitValueZ = 45.0f;
                break;
            case 618: // Ring of Valor
                underMapValueZ = 28.0f;
                upperLimitValueZ = 60.0f;
                break;
            case 566: // Eye of the storm
            case 968:
                underMapValueZ = 1000.0f;
                upperLimitValueZ = MAX_HEIGHT;
                break;
            default:
                AreaTableEntry const* zone = GetAreaEntryByAreaID(plrMover->GetAreaId());
                underMapValueZ = zone ? zone->MaxDepth : -500.0f;
                upperLimitValueZ = MAX_HEIGHT;
                break;
        }

        check = movementInfo.pos.GetPositionZ() < underMapValueZ || movementInfo.pos.GetPositionZ() > upperLimitValueZ;
        if (check && plrMover->isAlive() && !(plrMover->GetBattleground() && plrMover->GetBattleground()->HandlePlayerUnderMap(_player)))
        {
            plrMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, GetPlayer()->GetMaxHealth());
            if (!plrMover->isAlive())
                plrMover->KillPlayer(); // prevent death timer
            plrMover->RepopAtGraveyard();
        }
    }

    return true;
}

void WorldSession::HandleMovementSpeedChangeAck(WorldPacket& recvPacket)
{
    UnitMoveType moveType;
    bool updatePacketNotKnown = false;

    switch (recvPacket.GetOpcode())
    {
        case CMSG_MOVE_FORCE_WALK_SPEED_CHANGE_ACK: moveType = MOVE_WALK; break;
        case CMSG_MOVE_FORCE_RUN_SPEED_CHANGE_ACK: moveType = MOVE_RUN; break;
        case CMSG_MOVE_FORCE_RUN_BACK_SPEED_CHANGE_ACK: moveType = MOVE_RUN_BACK; break;
        case CMSG_MOVE_FORCE_SWIM_SPEED_CHANGE_ACK: moveType = MOVE_SWIM; break;
        case CMSG_MOVE_FORCE_SWIM_BACK_SPEED_CHANGE_ACK: moveType = MOVE_SWIM_BACK; break;
        case CMSG_MOVE_FORCE_TURN_RATE_CHANGE_ACK: moveType = MOVE_TURN_RATE; updatePacketNotKnown = true; break;
        case CMSG_MOVE_FORCE_FLIGHT_SPEED_CHANGE_ACK: moveType = MOVE_FLIGHT; break;
        case CMSG_MOVE_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK: moveType = MOVE_FLIGHT_BACK; updatePacketNotKnown = true; break;
        case CMSG_MOVE_FORCE_PITCH_RATE_CHANGE_ACK: moveType = MOVE_PITCH_RATE; updatePacketNotKnown = true; break;
        default:
            TC_LOG_INFO("misc", "WorldSession::HandleMovementSpeedChangeAck for unknown opcode: %s", GetOpcodeNameForLogging(recvPacket.GetOpcode()).c_str());
            return;
    }

    _player->SetUnderACKmount();
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_forced_speed_changes[moveType] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(moveType) - info.ackSpeed) > 0.01f)
    {
        if (_player->GetSpeed(moveType) > info.ackSpeed)
            _player->SetSpeed(moveType, _player->GetSpeedRate(moveType), true);

        recvPacket.rfinish();
        return;
    }

    // TODO: Find all structure (remove me)
    if (updatePacketNotKnown)
    {
        WorldPacket data;
        switch (moveType)
        {
            case MOVE_TURN_RATE:
                data.Initialize(SMSG_MOVE_SET_TURN_RATE, 1 + 8 + 4 + 4);
                info.WriteToPacket(data);
                _player->SendMessageToSet(&data, _player);
                break;
            case MOVE_FLIGHT_BACK:
                data.Initialize(SMSG_MOVE_SET_FLIGHT_BACK_SPEED, 1 + 8 + 4 + 4);
                info.WriteToPacket(data);
                _player->SendMessageToSet(&data, _player);
                break;
            case MOVE_PITCH_RATE:
                data.Initialize(SMSG_MOVE_SET_PITCH_RATE, 1 + 8 + 4 + 4);
                info.WriteToPacket(data);
                _player->SendMessageToSet(&data, _player);
                break;
            default:
                break;
        }
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);

    recvPacket.rfinish();
}

void WorldSession::HandleMovementGravityAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_GRAVITY] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementHoverAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_HOVER] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    WorldPacket data(SMSG_SET_PLAY_HOVER_ANIM, 8);
    ObjectGuid guid = _player->GetGUID();
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[1]);
    data.WriteBit(_player->HasUnitMovementFlag(MOVEMENTFLAG_HOVER) ? 1 : 0);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[6]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[6]);
    _player->SendMessageToSet(&data, _player);

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementWaterWalkAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_WATER_WALK] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementCanFlyAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_CAN_FLY] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
    recvPacket.rfinish();
}

void WorldSession::HandleMovementCollisionHeightAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_COLLISION_HEIGHT] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementFeatherFallAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_FEATHER_FALL] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementRootAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_ROOT] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}

void WorldSession::HandleMovementUnrootAck(WorldPacket& recvPacket)
{
    MovementInfo info;
    info.ReadFromPacket(recvPacket);

    // skip old result
    if (_player->m_movement_ack[ACK_UNROOT] != info.ackCount)
    {
        recvPacket.rfinish();
        return;
    }

    HandleMovementInfo(info, recvPacket.GetOpcode(), recvPacket.size()-4, _player->m_mover);
}
