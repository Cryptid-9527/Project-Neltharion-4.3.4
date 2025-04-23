/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Vehicle.h"
#include "Player.h"
#include "Log.h"
#include "ObjectAccessor.h"

void WorldSession::HandleDismissControlledVehicle(WorldPacket &recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Recvd CMSG_DISMISS_CONTROLLED_VEHICLE");

    uint64 vehicleGUID = _player->GetCharmGUID();

    if (!vehicleGUID)                                       // something wrong here...
    {
        recvData.rfinish();                                // prevent warnings spam
        return;
    }

    MovementInfo mi;
    mi.ReadFromPacket(recvData);
    mi.Sanitize(_player);

    _player->m_movementInfo = mi;

    _player->ExitVehicle();
}

void WorldSession::HandleChangeSeatsOnControlledVehicle(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Recvd CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");

    Unit* vehicle_base = GetPlayer()->GetVehicleBase();
    if (!vehicle_base)
    {
        recvData.rfinish();                                // prevent warnings spam
        return;
    }

    VehicleSeatEntry const* seat = GetPlayer()->GetVehicle()->GetSeatForPassenger(GetPlayer());
    if (!seat->CanSwitchFromSeat())
    {
        recvData.rfinish();                                // prevent warnings spam
        TC_LOG_ERROR("network.opcode", "HandleChangeSeatsOnControlledVehicle, Opcode: %u, Player %u tried to switch seats but current seatflags %u don't permit that.",
            recvData.GetOpcode(), GetPlayer()->GetGUIDLow(), seat->m_flags);
        return;
    }

    switch (recvData.GetOpcode())
    {
        case CMSG_REQUEST_VEHICLE_PREV_SEAT:
            GetPlayer()->ChangeSeat(-1, false);
            break;
        case CMSG_REQUEST_VEHICLE_NEXT_SEAT:
            GetPlayer()->ChangeSeat(-1, true);
            break;
        case CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE:
        {
            /*
            //TC_LOG_ERROR("network.opcode", "handling opcode: CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");
            uint64 guid;        // current vehicle guid
            recvData.readPackGUID(guid);

            int8 seatId;
            recvData >> seatId;

            int8 NEW_SEAT;
            //NEW_SEAT = seatId;
            NEW_SEAT = 2;
            if (Vehicle* mainVehicle = _player->GetVehicle())
            {
                 //TC_LOG_ERROR("network.opcode", "vehicle situation 1");
                _player->ExitVehicle();
                if (mainVehicle->GetBase())
                {
                    if (Vehicle* vehicleOwnerVehicle = mainVehicle->GetBase()->GetVehicle())
                    {
                        //TC_LOG_ERROR("network.opcode", "vehicle situation 2");//this player's current vehicle is mounted on another
                        if (auto VehicleUnitInSeat = vehicleOwnerVehicle->GetPassenger(2))
                        {
                            //TC_LOG_ERROR("network.opcode", "vehicle situation 3");//A player has clicked on a seat occupied by passenger
                            if (Vehicle* VehicleInSeat = VehicleUnitInSeat->GetVehicleKit())
                            {
                                //TC_LOG_ERROR("network.opcode", "vehicle situation 4");
                                if (!VehicleInSeat->GetPassenger(NEW_SEAT))
                                    VehicleUnitInSeat->HandleSpellClick(GetPlayer(), NEW_SEAT);
                            }
                            else
                            {
                                TC_LOG_ERROR("network.opcode", "vehicle situation 5");//this seat is occupied by a non vehicle.
                                return;
                            }
                        }
                        else if (auto vehicleOwnerVehicle = mainVehicle->GetBase())
                        {
                            vehicleOwnerVehicle->HandleSpellClick(GetPlayer(), NEW_SEAT);
                            TC_LOG_ERROR("network.opcode", "vehicle situation 6");//A player has clicked on a seat not occupied by a vehicle
                        }
                    }
                    else if (Vehicle* vehicleOwnerVehicle = mainVehicle)
                    {
                        //TC_LOG_ERROR("network.opcode", "vehicle situation 7, seat id: %u", seatId);//the player is riding on a seat of the main vehicle
                        if (auto VehicleUnitInSeat = vehicleOwnerVehicle->GetPassenger(2))
                        {
                            //TC_LOG_ERROR("network.opcode", "vehicle situation 8");//A player has clicked on a seat occupied by passenger
                            if (Vehicle* VehicleInSeat = VehicleUnitInSeat->GetVehicleKit())
                            {
                                //TC_LOG_ERROR("network.opcode", "vehicle situation 9");
                                if (!VehicleInSeat->GetPassenger(NEW_SEAT))
                                    VehicleUnitInSeat->HandleSpellClick(GetPlayer(), NEW_SEAT);
                            }
                            else
                            {
                                //TC_LOG_ERROR("network.opcode", "vehicle situation 10");//this seat is occupied by a non vehicle.
                                return;
                            }
                        }
                        else if (auto vehicleOwnerVehicle = mainVehicle->GetBase())
                        {
                            //TC_LOG_ERROR("network.opcode", "vehicle situation 11");//A player has clicked on a seat not occupied by a vehicle
                            vehicleOwnerVehicle->HandleSpellClick(GetPlayer(), NEW_SEAT);
                        }
                    }
                }
            }
            //TC_LOG_ERROR("network.opcode", "breaking after functions");
            break;
        */
        }
        case CMSG_REQUEST_VEHICLE_SWITCH_SEAT:
        {
            uint64 guid;        // current vehicle guid
            recvData.readPackGUID(guid);

            int8 seatId;
            recvData >> seatId;

            if (vehicle_base->GetGUID() == guid)
                GetPlayer()->ChangeSeat(seatId);
            else if (auto vehUnit = Unit::GetUnit(*GetPlayer(), guid))
                if (Vehicle* vehicle = vehUnit->GetVehicleKit())
                    if (vehicle->HasEmptySeat(seatId))
                        vehUnit->HandleSpellClick(GetPlayer(), seatId);
            break;
        }
        default:
            break;
    }
}

void WorldSession::HandleEnterPlayerVehicle(WorldPacket& data)
{
    // Read guid
    uint64 guid;
    data >> guid;

    if (auto player = ObjectAccessor::FindPlayer(guid))
    {
        if (!player->GetVehicleKit())
            return;
        if (!player->IsInRaidWith(_player))
            return;
        if (!player->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        _player->EnterVehicle(player);
    }
}

void WorldSession::HandleEjectPassenger(WorldPacket& data)
{
    Vehicle* vehicle = _player->GetVehicleKit();
    if (!vehicle)
    {
        data.rfinish();                                     // prevent warnings spam
        TC_LOG_ERROR("network.opcode", "HandleEjectPassenger: Player %u is not in a vehicle!", GetPlayer()->GetGUIDLow());
        return;
    }

    uint64 guid;
    data >> guid;

    if (IS_PLAYER_GUID(guid))
    {
        Player* player = ObjectAccessor::FindPlayer(guid);
        if (!player)
        {
            TC_LOG_ERROR("network.opcode", "Player %u tried to eject player %u from vehicle, but the latter was not found in world!", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        if (!player->IsOnVehicle(vehicle->GetBase()))
        {
            TC_LOG_ERROR("network.opcode", "Player %u tried to eject player %u, but they are not in the same vehicle", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(player);
        ASSERT(seat);
        if (seat->IsEjectable())
            player->ExitVehicle();
        else
            TC_LOG_ERROR("network.opcode", "Player %u attempted to eject player %u from non-ejectable seat.", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
    }

    else if (IS_CREATURE_GUID(guid))
    {
        Unit* unit = ObjectAccessor::GetUnit(*_player, guid);
        if (!unit) // creatures can be ejected too from player mounts
        {
            TC_LOG_ERROR("network.opcode", "Player %u tried to eject creature guid %u from vehicle, but the latter was not found in world!", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        if (!unit->IsOnVehicle(vehicle->GetBase()))
        {
            TC_LOG_ERROR("network.opcode", "Player %u tried to eject unit %u, but they are not in the same vehicle", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
            return;
        }

        VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(unit);
        ASSERT(seat);
        if (seat->IsEjectable())
        {
            ASSERT(GetPlayer() == vehicle->GetBase());
            unit->ExitVehicle();
        }
        else
            TC_LOG_ERROR("network.opcode", "Player %u attempted to eject creature GUID %u from non-ejectable seat.", GetPlayer()->GetGUIDLow(), GUID_LOPART(guid));
    }
    else
        TC_LOG_ERROR("network.opcode", "HandleEjectPassenger: Player %u tried to eject invalid GUID " UI64FMTD, GetPlayer()->GetGUIDLow(), guid);
}

void WorldSession::HandleRequestVehicleExit(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network.opcode", "WORLD: Recvd CMSG_REQUEST_VEHICLE_EXIT");

    if (Vehicle* vehicle = GetPlayer()->GetVehicle())
    {
        if (VehicleSeatEntry const* seat = vehicle->GetSeatForPassenger(GetPlayer()))
        {
            if (seat->CanEnterOrExit())
                GetPlayer()->ExitVehicle();
            else
                TC_LOG_ERROR("network.opcode", "Player %u tried to exit vehicle, but seatflags %u (ID: %u) don't permit that.",
                GetPlayer()->GetGUIDLow(), seat->m_ID, seat->m_flags);
        }
    }
}
