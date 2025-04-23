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

#include "Player.h"
#include "Battleground.h"
#include "BattlegroundIC.h"
#include "Language.h"
#include "WorldPacket.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "Transport.h"
#include "WorldSession.h"

BattlegroundIC::BattlegroundIC()
{
    BgObjects.resize(MAX_NORMAL_GAMEOBJECTS_SPAWNS + MAX_AIRSHIPS_SPAWNS + MAX_HANGAR_TELEPORTERS_SPAWNS + MAX_FORTRESS_TELEPORTERS_SPAWNS);
    BgCreatures.resize(MAX_NORMAL_NPCS_SPAWNS + MAX_WORKSHOP_SPAWNS + MAX_DOCKS_SPAWNS + MAX_SPIRIT_GUIDES_SPAWNS + MAX_SHIP_NPC_EXTRA_SPAWNS);

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_IC_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_IC_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_IC_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_IC_HAS_BEGUN;

    for (uint8 i = 0; i < 2; ++i)
        factionReinforcements[i] = MAX_REINFORCEMENTS;

    for (uint8 i = 0; i < BG_IC_MAXDOOR; ++i)
        GateStatus[i] = BG_IC_GATE_OK;

    closeFortressDoorsTimer = CLOSE_DOORS_TIME; // the doors are closed again... in a special way
    doorsClosed = false;
    resourceTimer = IC_RESOURCE_TIME;

    for (uint8 i = NODE_TYPE_REFINERY; i < MAX_NODE_TYPES; ++i)
        nodePoint[i] = nodePointInitial[i];

    siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;

    gunshipHorde = NULL;
    gunshipAlliance = NULL;
}

BattlegroundIC::~BattlegroundIC()
{
    sMapMgr->UnLoadTransportFromMap(gunshipHorde);
    sMapMgr->UnLoadTransportFromMap(gunshipAlliance);
}

void BattlegroundIC::HandlePlayerResurrect(Player* player)
{
    if (nodePoint[NODE_TYPE_QUARRY].nodeState == (player->GetTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_QUARRY, true);

    if (nodePoint[NODE_TYPE_REFINERY].nodeState == (player->GetTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_OIL_REFINERY, true);
}

void BattlegroundIC::SendTransportInit(Player* player)
{
    if (!gunshipAlliance || !gunshipHorde)
        return;

    UpdateData transData(player->GetMapId());

    gunshipAlliance->BuildCreateUpdateBlockForPlayer(&transData, player);
    gunshipHorde->BuildCreateUpdateBlockForPlayer(&transData, player);

    WorldPacket packet;

    transData.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void BattlegroundIC::DoAction(uint32 action, uint64 var)
{
    if (action != ACTION_TELEPORT_PLAYER_TO_TRANSPORT)
        return;

    if (!gunshipAlliance || !gunshipHorde)
        return;

    Player* player = ObjectAccessor::FindPlayer(var);
    if (!player)
        return;

    (player->GetTeamId() == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde)->AddPassenger(player);
    player->SetTransport(player->GetTeamId() == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde);

    player->m_movementInfo.t_pos.m_positionX = TransportMovementInfo.GetPositionX();
    player->m_movementInfo.t_pos.m_positionY = TransportMovementInfo.GetPositionY();
    player->m_movementInfo.t_pos.m_positionZ = TransportMovementInfo.GetPositionZ();
    player->m_movementInfo.t_guid = (player->GetTeamId() == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde)->GetGUID();

    if (player->TeleportTo(GetMapId(), TeleportToTransportPosition.GetPositionX(),
                        TeleportToTransportPosition.GetPositionY(),
                        TeleportToTransportPosition.GetPositionZ()-30.0f,
                        player->GetOrientation(),
                        TELE_TO_NOT_LEAVE_TRANSPORT))
    {
        //player->CastSpell(player, SPELL_SLOW_FALL, true);
        player->CastSpell(player, SPELL_PARACHUTE, true);
    }
}

void BattlegroundIC::PostUpdateImpl(uint32 diff)
{

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!doorsClosed)
    {
        if (closeFortressDoorsTimer <= diff)
        {
            GetBGObject(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR01)->RemoveFromWorld();
            GetBGObject(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR01)->RemoveFromWorld();

            GetBGObject(BG_IC_GO_ALLIANCE_GATE_3)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED); // Alliance door
            GetBGObject(BG_IC_GO_HORDE_GATE_1)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED); // Horde door

            doorsClosed = true;
        } else closeFortressDoorsTimer -= diff;
    }

    for (uint8 i = NODE_TYPE_REFINERY; i < MAX_NODE_TYPES; ++i)
    {
        /*        if (nodePoint[i].nodeType == NODE_TYPE_DOCKS)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A ||
                nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                if (docksTimer <= diff)
                {
                    // we need to confirm this, i am not sure if this every 3 minutes
                    for (uint8 u = (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_A : BG_IC_NPC_CATAPULT_1_H); u < (nodePoint[i].faction  == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_4_A : BG_IC_NPC_CATAPULT_4_H); ++u)
                    {
                        if (auto catapult = GetBGCreature(u))
                        {
                            if (!catapult->isAlive())
                                catapult->Respawn(true);
                        }
                    }

                    // we need to confirm this is blizzlike, not sure if it is every 3 minutes
                    for (uint8 u = (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_A : BG_IC_NPC_GLAIVE_THROWER_1_H); u < (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_2_A : BG_IC_NPC_GLAIVE_THROWER_2_H); ++u)
                    {
                        if (auto glaiveThrower = GetBGCreature(u))
                        {
                            if (!glaiveThrower->isAlive())
                                glaiveThrower->Respawn(true);
                        }
                    }

                    docksTimer = DOCKS_UPDATE_TIME;
                } else docksTimer -= diff;
            }
        }*/

        if (nodePoint[i].nodeType == NODE_TYPE_WORKSHOP)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A ||
                nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                if (siegeEngineWorkshopTimer <= diff)
                {
                    uint8 siegeType = (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_SIEGE_ENGINE_A : BG_IC_NPC_SIEGE_ENGINE_H);

                    if (auto siege = GetBGCreature(siegeType)) // this always should be true
                    {
                        if (siege->isAlive())
                        {
                            if (siege->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_UNK_14|UNIT_FLAG_IMMUNE_TO_PC))
                                // following sniffs the vehicle always has UNIT_FLAG_UNK_14
                                siege->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_IMMUNE_TO_PC);
                            //siege->SetPhaseMask(1, true);
                        }
                        else
                        {
                            siege->Respawn(true);
                        }
                    }

                    /*
                    for (uint8 u = (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_A : BG_IC_NPC_DEMOLISHER_1_H); u < (nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_4_A : BG_IC_NPC_DEMOLISHER_4_H); ++u)
                    {
                        if (auto demolisher = GetBGCreature(u))
                        {
                            if (!demolisher->isAlive())
                                demolisher->Respawn(true);
                        }
                    }
                    */
                    siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;
                } else siegeEngineWorkshopTimer -= diff;
            }
        }

        // the point is waiting for a change on its banner
        if (nodePoint[i].needChange)
        {
            if (nodePoint[i].timer <= diff)
            {
                uint32 nextBanner = GetNextBanner(&nodePoint[i], nodePoint[i].faction, true);

                nodePoint[i].gameobject_entry = nextBanner;
                // nodePoint[i].faction = the faction should be the same one...

                GameObject* banner = GetBGObject(nodePoint[i].gameobject_type);

                if (!banner) // this should never happen
                    return;

                float cords[4] = {banner->GetPositionX(), banner->GetPositionY(), banner->GetPositionZ(), banner->GetOrientation() };

                DelObject(nodePoint[i].gameobject_type);
                AddObject(nodePoint[i].gameobject_type, nodePoint[i].gameobject_entry, cords[0], cords[1], cords[2], cords[3], 0, 0, 0, 0, RESPAWN_ONE_DAY);

                GetBGObject(nodePoint[i].gameobject_type)->SetUInt32Value(GAMEOBJECT_FACTION, nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_Factions[1] : BG_IC_Factions[0]);

                UpdateNodeWorldState(&nodePoint[i]);
                HandleCapturedNodes(&nodePoint[i], false);
                nodePoint[i].needChange = false;
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME;
            } else nodePoint[i].timer -= diff;
        }
    }

    if (resourceTimer <= diff)
    {
        for (uint8 i = 0; i < NODE_TYPE_DOCKS; ++i)
        {
            if (nodePoint[i].nodeState == NODE_STATE_CONTROLLED_A ||
                nodePoint[i].nodeState == NODE_STATE_CONTROLLED_H)
            {
                factionReinforcements[nodePoint[i].faction] += 1;
                RewardHonorToTeam(GetBonusHonorFromKill(1), nodePoint[i].faction == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                UpdateWorldState((nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_ALLIANCE_RENFORT : BG_IC_HORDE_RENFORT), factionReinforcements[nodePoint[i].faction]);
            }
        }
        resourceTimer = IC_RESOURCE_TIME;
    } else resourceTimer -= diff;
    HandleVehicleRespawns(diff);
    /************************************* GUARD RESPAWN AND PASSIVE VICTORY CHECK *******************************************/
    if (Alliance_boss_spawned == true) {
        if (GetBGCreature(1)->isAlive()) {//ALLIANCE BOSS
            if (!GetBGCreature(1)->isInCombat()) { //ALLIANCE BOSS
                for (uint8 i = 0; i < 4; i++) {
                    uint32 BgCreatureSlot = i + 6;
                    if (!GetBGCreature(BgCreatureSlot)->isAlive()) {
                        DelCreature(BgCreatureSlot);
                        AddCreature(BG_IC_NpcSpawnlocs[BgCreatureSlot].entry, BG_IC_NpcSpawnlocs[BgCreatureSlot].type, BG_IC_NpcSpawnlocs[BgCreatureSlot].team, BG_IC_NpcSpawnlocs[BgCreatureSlot].x, BG_IC_NpcSpawnlocs[BgCreatureSlot].y, BG_IC_NpcSpawnlocs[BgCreatureSlot].z, BG_IC_NpcSpawnlocs[BgCreatureSlot].o, RESPAWN_ONE_DAY);
                    }
                }
            }
        }
        else {
            EndBattleground(HORDE);
        }
    }
    if (Horde_boss_spawned == true) {
        if (GetBGCreature(0)->isAlive()) {//HORDE BOSS
            if (!GetBGCreature(0)->isInCombat()) { //HORDE BOSS
                for (uint8 i = 0; i < 4; i++) {
                    uint32 BgCreatureSlot = i + 2;
                    if (!GetBGCreature(BgCreatureSlot)->isAlive()) {
                        DelCreature(BgCreatureSlot);
                        AddCreature(BG_IC_NpcSpawnlocs[BgCreatureSlot].entry, BG_IC_NpcSpawnlocs[BgCreatureSlot].type, BG_IC_NpcSpawnlocs[BgCreatureSlot].team, BG_IC_NpcSpawnlocs[BgCreatureSlot].x, BG_IC_NpcSpawnlocs[BgCreatureSlot].y, BG_IC_NpcSpawnlocs[BgCreatureSlot].z, BG_IC_NpcSpawnlocs[BgCreatureSlot].o, RESPAWN_ONE_DAY);
                    }
                }
            }
        }
        else {
            EndBattleground(ALLIANCE);
        }
    }
    /***********************************************************************************************************************************/
}

void BattlegroundIC::StartingEventCloseDoors()
{
    // Show Full Gate Displays
    GetBGObject(BG_IC_GO_ALLIANCE_GATE_1)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
    GetBGObject(BG_IC_GO_ALLIANCE_GATE_2)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
    GetBGObject(BG_IC_GO_HORDE_GATE_2)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
    GetBGObject(BG_IC_GO_HORDE_GATE_3)->SetDestructibleState(GO_DESTRUCTIBLE_DAMAGED);
}

void BattlegroundIC::StartingEventOpenDoors()
{
    //after 20 seconds they should be despawned
    DoorOpen(BG_IC_GO_DOODAD_ND_HUMAN_GATE_CLOSEDFX_DOOR01);
    DoorOpen(BG_IC_GO_DOODAD_ND_WINTERORC_WALL_GATEFX_DOOR01);

    DoorOpen(BG_IC_GO_DOODAD_HU_PORTCULLIS01_1);
    DoorOpen(BG_IC_GO_DOODAD_HU_PORTCULLIS01_2);
    DoorOpen(BG_IC_GO_DOODAD_VR_PORTCULLIS01_1);
    DoorOpen(BG_IC_GO_DOODAD_VR_PORTCULLIS01_2);

    for (uint32 i = BG_IC_GO_TELEPORTER_EFFECTS_A_1; i <= BG_IC_GO_TELEPORTER_EFFECTS_A_6; ++i) {//fancy fireworks show
        GetBGObject(i)->SetGoState(GO_STATE_ACTIVE);
    }
    for (uint32 i = BG_IC_GO_TELEPORTER_EFFECTS_H_1; i <= BG_IC_GO_TELEPORTER_EFFECTS_H_6; ++i) {//fancy fireworks show
        GetBGObject(i)->SetGoState(GO_STATE_ACTIVE);
    }
    for (uint32 i = BG_IC_GO_TELEPORTER_1_1; i <= BG_IC_GO_TELEPORTER_2_3; ++i) {//now portals can be used.
        GetBGObject(i)->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    }
}

bool BattlegroundIC::IsAllNodesConrolledByTeam(uint32 team) const
{
    uint32 count = 0;
    ICNodeState controlledState = team == ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H;
    for (int i = 0; i < NODE_TYPE_WORKSHOP; ++i)
    {
        if (nodePoint[i].nodeState == controlledState)
            ++count;
    }

    return count == NODE_TYPE_WORKSHOP;
}

void BattlegroundIC::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    BattlegroundICScore* sc = new BattlegroundICScore;
    PlayerScores[player->GetGUID()] = sc;
    sc->BgTeam = player->GetTeam();
    sc->TalentTree = player->GetPrimaryTalentTree(player->GetActiveSpec());

    if (nodePoint[NODE_TYPE_QUARRY].nodeState == (player->GetTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_QUARRY, true);

    if (nodePoint[NODE_TYPE_REFINERY].nodeState == (player->GetTeamId() == TEAM_ALLIANCE ? NODE_STATE_CONTROLLED_A : NODE_STATE_CONTROLLED_H))
        player->CastSpell(player, SPELL_OIL_REFINERY, true);

    SendTransportInit(player);

    if (player->GetTeamId() == TEAM_HORDE)
        player->TeleportTo(628, 1213.284668f, -762.569641f, 48.917168f, 3.152061f, 0);
    if (player->GetTeamId() == TEAM_ALLIANCE)
        player->TeleportTo(628, 348.255249f, -834.533997f, 48.916340f, 0.010990f, 0);
}

void BattlegroundIC::RemovePlayer(Player* player, uint64 /*guid*/, uint32 /*team*/)
{
    if (player)
    {
        player->RemoveAura(SPELL_PARACHUTE);
        player->RemoveAura(SPELL_QUARRY);
        player->RemoveAura(SPELL_OIL_REFINERY);
    }
}

void BattlegroundIC::HandleAreaTrigger(Player* /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void BattlegroundIC::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    std::map<uint64, BattlegroundScore*>::iterator itr = PlayerScores.find(Source->GetGUID());

    if (itr == PlayerScores.end())                         // player not found...
        return;

    switch (type)
    {
        case SCORE_BASES_ASSAULTED:
            ((BattlegroundICScore*)itr->second)->BasesAssaulted += value;
            break;
        case SCORE_BASES_DEFENDED:
            ((BattlegroundICScore*)itr->second)->BasesDefended += value;
            break;
        default:
            Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
            break;
    }
}

void BattlegroundIC::FillInitialWorldStates(WorldPacket& data)
{
    data << uint32(BG_IC_ALLIANCE_RENFORT_SET) << uint32(1);
    data << uint32(BG_IC_HORDE_RENFORT_SET) << uint32(1);
    data << uint32(BG_IC_ALLIANCE_RENFORT) << uint32(factionReinforcements[TEAM_ALLIANCE]);
    data << uint32(BG_IC_HORDE_RENFORT) << uint32(factionReinforcements[TEAM_HORDE]);

    for (uint8 i = 0; i < MAX_FORTRESS_GATES_SPAWNS; ++i)
    {
        uint32 uws = GetWorldStateFromGateEntry(BG_IC_ObjSpawnlocs[i].entry, (GateStatus[GetGateIDFromEntry(BG_IC_ObjSpawnlocs[i].entry)] == BG_IC_GATE_DESTROYED ? true : false));
        data << uint32(uws) << uint32(1);
    }

    for (uint8 i = 0; i < MAX_NODE_TYPES; ++i)
        data << uint32(nodePoint[i].worldStates[nodePoint[i].nodeState]) << uint32(1);
}

bool BattlegroundIC::SetupBattleground()
{
    for (uint8 i = 0; i < MAX_NORMAL_GAMEOBJECTS_SPAWNS; ++i)
    {
        if (!AddObject(BG_IC_ObjSpawnlocs[i].type, BG_IC_ObjSpawnlocs[i].entry,
            BG_IC_ObjSpawnlocs[i].x, BG_IC_ObjSpawnlocs[i].y,
            BG_IC_ObjSpawnlocs[i].z, BG_IC_ObjSpawnlocs[i].o,
            0, 0, 0, 0, RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning gameobject %u", BG_IC_ObjSpawnlocs[i].entry);
            return false;
        }
    }

    for (uint8 i = 2; i < MAX_NORMAL_NPCS_SPAWNS; ++i)
    {
        if (!AddCreature(BG_IC_NpcSpawnlocs[i].entry, BG_IC_NpcSpawnlocs[i].type, BG_IC_NpcSpawnlocs[i].team,
            BG_IC_NpcSpawnlocs[i].x, BG_IC_NpcSpawnlocs[i].y,
            BG_IC_NpcSpawnlocs[i].z, BG_IC_NpcSpawnlocs[i].o,
            RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning creature %u", BG_IC_NpcSpawnlocs[i].entry);
            return false;
        }
        else
        {
            //TC_LOG_ERROR("bg.battleground", "Sucessfully spawned BG creature %u", BG_IC_NpcSpawnlocs[i].entry);
        }
    }

    if (!AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+5, BG_IC_SpiritGuidePos[5][0], BG_IC_SpiritGuidePos[5][1], BG_IC_SpiritGuidePos[5][2], BG_IC_SpiritGuidePos[5][3], ALLIANCE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+6, BG_IC_SpiritGuidePos[6][0], BG_IC_SpiritGuidePos[6][1], BG_IC_SpiritGuidePos[6][2], BG_IC_SpiritGuidePos[6][3], HORDE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+3, BG_IC_SpiritGuidePos[7][0], BG_IC_SpiritGuidePos[7][1], BG_IC_SpiritGuidePos[7][2], BG_IC_SpiritGuidePos[7][3], ALLIANCE)
        || !AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1+4, BG_IC_SpiritGuidePos[8][0], BG_IC_SpiritGuidePos[8][1], BG_IC_SpiritGuidePos[8][2], BG_IC_SpiritGuidePos[8][3], HORDE))
    {
        TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn initial spirit guide!");
        return false;
    }

    gunshipHorde = CreateTransport(GO_HORDE_GUNSHIP, TRANSPORT_PERIOD_TIME);
    gunshipAlliance = CreateTransport(GO_ALLIANCE_GUNSHIP, TRANSPORT_PERIOD_TIME);

    if (!gunshipAlliance || !gunshipHorde)
    {
        TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error creating gunships!");
        return false;
    }

    gunshipHorde->BuildStopMovePacket(GetBgMap());
    gunshipAlliance->BuildStopMovePacket(GetBgMap());
    //Send transport init packet to all player in map
    for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        if (auto player = ObjectAccessor::FindPlayer(itr->first))
            SendTransportInit(player);
    }

    // setting correct factions for Keep Cannons
    for (uint8 i = BG_IC_NPC_KEEP_CANNON_1; i < BG_IC_NPC_KEEP_CANNON_13; ++i)
        GetBGCreature(i)->setFaction(BG_IC_Factions[0]);
    for (uint8 i = BG_IC_NPC_KEEP_CANNON_13; i < BG_IC_NPC_KEEP_CANNON_25; ++i)
        GetBGCreature(i)->setFaction(BG_IC_Factions[1]);

    // correcting spawn time for keeps bombs
    for (uint8 i = BG_IC_GO_HUGE_SEAFORIUM_BOMBS_A_1; i <= BG_IC_GO_HUGE_SEAFORIUM_BOMBS_H_4; ++i)
        GetBGObject(i)->SetRespawnTime(10);

    for (uint8 i = 0; i < MAX_FORTRESS_TELEPORTERS_SPAWNS; ++i)
    {
        if (!AddObject(BG_IC_Teleporters[i].type, BG_IC_Teleporters[i].entry,
            BG_IC_Teleporters[i].x, BG_IC_Teleporters[i].y,
            BG_IC_Teleporters[i].z, BG_IC_Teleporters[i].o,
            0, 0, 0, 0, RESPAWN_ONE_DAY))
        {
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest | Starting Event Open Doors: There was an error spawning gameobject %u", BG_IC_Teleporters[i].entry);
        }
        GetBGObject(BG_IC_Teleporters[i].type)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    };

    GetBGObject(BG_IC_GO_HORDE_BANNER)->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[0]);
    GetBGObject(BG_IC_GO_ALLIANCE_BANNER)->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[1]);
    GetBGObject(BG_IC_GO_HORDE_BANNER)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    GetBGObject(BG_IC_GO_ALLIANCE_BANNER)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    /*
    //TODO: find a way to make your own gates not light up on mouseover. this didn't work.
        for (uint32 u = BG_IC_GO_ALLIANCE_GATE_1; u <= BG_IC_GO_ALLIANCE_GATE_3; ++u){
            GetBGObject(u)->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[1]);
        };
        for (uint32 u = BG_IC_GO_HORDE_GATE_1; u <= BG_IC_GO_HORDE_GATE_3; ++u){
            GetBGObject(u)->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[1]);
        };
    */
    //make all the cosmetic objects non-interactable
    for (uint32 u = BG_IC_GO_BENCH_1; u <= BG_IC_GO_CHAIR_9; ++u) {
        GetBGObject(u)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    };
    for (uint32 u = BG_IC_GO_FLAGPOLE_1_1; u <= BG_IC_GO_FLAGPOLE_1_6; ++u) {
        GetBGObject(u)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    };
    for (uint32 u = BG_IC_GO_STOVE_1_1; u <= BG_IC_GO_STOVE_2_2; ++u) {
        GetBGObject(u)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    };

    return true;
}

void BattlegroundIC::HandleKillUnit(Creature* unit, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
       return;

    uint32 type = (GetCreatureType(unit->GetGUID()));
    switch (type) {
    case NPC_HIGH_COMMANDER_HALFORD_WYRMBANE:
        EndBattleground(HORDE);
        break;
    case NPC_OVERLORD_AGMAR:
        EndBattleground(ALLIANCE);
    
        break;

    case BG_IC_NPC_SIEGE_ENGINE_A:
    case BG_IC_NPC_DEMOLISHER_1_A:
    case BG_IC_NPC_DEMOLISHER_2_A:
    case BG_IC_NPC_DEMOLISHER_3_A:
    case BG_IC_NPC_DEMOLISHER_4_A:
    case BG_IC_NPC_SIEGE_ENGINE_H:
    case BG_IC_NPC_DEMOLISHER_1_H:
    case BG_IC_NPC_DEMOLISHER_2_H:
    case BG_IC_NPC_DEMOLISHER_3_H:
    case BG_IC_NPC_DEMOLISHER_4_H:
        if ((nodePoint[NODE_TYPE_WORKSHOP].faction == VehicleRespawns[type - BG_IC_NPC_SIEGE_ENGINE_A].v_faction) || (VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death > 0)) {
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 2;
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
            //TC_LOG_ERROR("bg.battleground", "IOC: Unit (%u) has been killed and triggered a proper workshop respawn. new values: (%u, %u)", type, VehicleRespawnStatus[type-BG_IC_NPC_SIEGE_ENGINE_A].npc_state, VehicleRespawnStatus[type-BG_IC_NPC_SIEGE_ENGINE_A].time_since_death);
        }
        else {
            //TC_LOG_ERROR("bg.battleground", "IOC: Workshop faction (%u) is not equal to vehicle spawn faction (%u).", nodePoint[NODE_TYPE_WORKSHOP].faction, VehicleRespawns[type-BG_IC_NPC_SIEGE_ENGINE_A].v_faction);
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 0;
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
            DelCreature(type);
        }
        break;
    case BG_IC_NPC_GLAIVE_THROWER_1_A:
    case BG_IC_NPC_GLAIVE_THROWER_2_A:
    case BG_IC_NPC_CATAPULT_1_A:
    case BG_IC_NPC_CATAPULT_2_A:
    case BG_IC_NPC_CATAPULT_3_A:
    case BG_IC_NPC_CATAPULT_4_A:
    case BG_IC_NPC_GLAIVE_THROWER_1_H:
    case BG_IC_NPC_GLAIVE_THROWER_2_H:
    case BG_IC_NPC_CATAPULT_1_H:
    case BG_IC_NPC_CATAPULT_2_H:
    case BG_IC_NPC_CATAPULT_3_H:
    case BG_IC_NPC_CATAPULT_4_H:
        if (nodePoint[NODE_TYPE_DOCKS].faction == VehicleRespawns[type - BG_IC_NPC_SIEGE_ENGINE_A].v_faction) {
            //TC_LOG_ERROR("bg.battleground", "IOC: Unit (%u) has been killed and triggered a proper docks respawn.", type);
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 2;
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
        }
        else {
            //TC_LOG_ERROR("bg.battleground", "IOC: Docks faction (%u) is not equal to vehicle spawn faction (%u).", nodePoint[NODE_TYPE_DOCKS].faction, VehicleRespawns[type-BG_IC_NPC_SIEGE_ENGINE_A].v_faction);
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 0;
            VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
            DelCreature(type);
        }
        break;
    default:
        TC_LOG_ERROR("bg.battleground", "IOC: Unit (%u) has been killed. This unit does not have a reaction.", type);
        break;
    }
    //Achievement Mowed Down
    // TO-DO: This should be done on the script of each vehicle of the BG.
    if (unit->IsVehicle()) {
        killer->CastSpell(killer, SPELL_DESTROYED_VEHICLE_ACHIEVEMENT, true);
    }
}

void BattlegroundIC::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (killer)
        if (player)
            Battleground::HandleKillPlayer(player, killer);
        else TC_LOG_ERROR("bg.battleground", "BattlegroundIC::HandleKillPlayer: Killer player not found");
    else TC_LOG_ERROR("bg.battleground", "BattlegroundIC::HandleKillPlayer: Victim player not found");

    if (player)
    {
        factionReinforcements[player->GetTeamId()] -= 1;
        UpdateWorldState((player->GetTeamId() == TEAM_ALLIANCE ? BG_IC_ALLIANCE_RENFORT : BG_IC_HORDE_RENFORT), factionReinforcements[player->GetTeamId()]);

        // we must end the battleground
        if (factionReinforcements[player->GetTeamId()] < 1)
            EndBattleground(killer->GetTeam());
    }
    else
    {
        auto victim_team = killer->GetOppositeTeamId();
        factionReinforcements[victim_team] -= 1;
        UpdateWorldState((player->GetTeamId() == TEAM_ALLIANCE ? BG_IC_ALLIANCE_RENFORT : BG_IC_HORDE_RENFORT), factionReinforcements[victim_team]);

        // we must end the battleground
        if (factionReinforcements[victim_team] < 1)
            EndBattleground(killer->GetTeam());

    }
}

void BattlegroundIC::EndBattleground(uint32 winner)
{
    DistributeHonorToAll(winner);
    Battleground::EndBattleground(winner);
}

void BattlegroundIC::RealocatePlayers(ICNodePointType nodeType)
{
    // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
    std::vector<uint64> ghost_list = m_ReviveQueue[BgCreatures[BG_IC_NPC_SPIRIT_GUIDE_1+nodeType-2]];
    if (!ghost_list.empty())
    {
        WorldSafeLocsEntry const* ClosestGrave = NULL;
        for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(*itr);
            if (!player)
                continue;

            if (!ClosestGrave)                              // cache
                ClosestGrave = GetClosestGraveYard(player);

            if (ClosestGrave)
                player->TeleportTo(GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
        }
    }
}

void BattlegroundIC::EventPlayerClickedOnFlag(Player* player, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // All the node points are iterated to find the clicked one
    for (uint8 i = 0; i < MAX_NODE_TYPES; ++i)
    {
        if (nodePoint[i].gameobject_entry == target_obj->GetEntry())
        {
            // THIS SHOULD NEEVEER HAPPEN
            if (nodePoint[i].faction == player->GetTeamId())
                return;

            uint32 nextBanner = GetNextBanner(&nodePoint[i], player->GetTeamId(), false);

            // we set the new settings of the nodePoint
            nodePoint[i].faction = player->GetTeamId();
            nodePoint[i].gameobject_entry = nextBanner;


            UpdateNodeWorldState(&nodePoint[i]);
            // this is just needed if the next banner is grey
            if (nodePoint[i].banners[BANNER_A_CONTESTED] == nextBanner ||
                nodePoint[i].banners[BANNER_H_CONTESTED] == nextBanner)
            {
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME; // 1 minute for last change (real faction banner)
                nodePoint[i].needChange = true;

                RealocatePlayers(nodePoint[i].nodeType);

                // if we are here means that the point has been lost, or it is the first capture

                if (nodePoint[i].nodeType != NODE_TYPE_REFINERY && nodePoint[i].nodeType != NODE_TYPE_QUARRY)
                    if (BgCreatures[BG_IC_NPC_SPIRIT_GUIDE_1+(nodePoint[i].nodeType)-2])
                        DelCreature(BG_IC_NPC_SPIRIT_GUIDE_1+(nodePoint[i].nodeType)-2);

                UpdatePlayerScore(player, SCORE_BASES_ASSAULTED, 1);

                if (nodePoint[i].hasBeenCaptured == 0) {
                    SendMessage2ToAll(LANG_BG_IC_TEAM_ASSAULTED_NODE_1, (player->GetTeamId() == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE), player, nodePoint[i].string);
                }
                else {
                    SendMessage2ToAll(LANG_BG_IC_TEAM_ASSAULTED_NODE_2, (player->GetTeamId() == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE), player, nodePoint[i].string, (player->GetTeamId() == TEAM_ALLIANCE ? LANG_BG_IC_ALLIANCE : LANG_BG_IC_HORDE));
                }
                HandleContestedNodes(&nodePoint[i]);
            } else if (nextBanner == nodePoint[i].banners[BANNER_A_CONTROLLED] ||
                       nextBanner == nodePoint[i].banners[BANNER_H_CONTROLLED])
                       // if we are going to spawn the definitve faction banner, we dont need the timer anymore
            {
                nodePoint[i].timer = BANNER_STATE_CHANGE_TIME;
                nodePoint[i].needChange = true;
                SendMessage2ToAll(LANG_BG_IC_TEAM_DEFENDED_NODE, CHAT_MSG_BG_SYSTEM_NEUTRAL, player, nodePoint[i].string);
                HandleCapturedNodes(&nodePoint[i], true);
                UpdatePlayerScore(player, SCORE_BASES_DEFENDED, 1);
            }

            GameObject* banner = GetBGObject(nodePoint[i].gameobject_type);

            if (!banner) // this should never happen
                return;

            float cords[4] = {banner->GetPositionX(), banner->GetPositionY(), banner->GetPositionZ(), banner->GetOrientation() };

            DelObject(nodePoint[i].gameobject_type);
            AddObject(nodePoint[i].gameobject_type, nodePoint[i].gameobject_entry, cords[0], cords[1], cords[2], cords[3], 0, 0, 0, 0, RESPAWN_ONE_DAY);

            GetBGObject(nodePoint[i].gameobject_type)->SetUInt32Value(GAMEOBJECT_FACTION, nodePoint[i].faction == TEAM_ALLIANCE ? BG_IC_Factions[1] : BG_IC_Factions[0]);

            if (nodePoint[i].nodeType == NODE_TYPE_WORKSHOP)
            {
                DelObject(BG_IC_GO_SEAFORIUM_BOMBS_1);
                DelObject(BG_IC_GO_SEAFORIUM_BOMBS_2);
            }

            
            // we dont need iterating if we are here
            // If the needChange bool was set true, we will handle the rest in the Update Map function.
            //return;
        }
    }
}

void BattlegroundIC::UpdateNodeWorldState(ICNodePoint* nodePoint)
{
    //updating worldstate
    if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTROLLED])
        nodePoint->nodeState = NODE_STATE_CONTROLLED_A;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_A_CONTESTED])
        nodePoint->nodeState = NODE_STATE_CONFLICT_A;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTROLLED])
        nodePoint->nodeState = NODE_STATE_CONTROLLED_H;
    else if (nodePoint->gameobject_entry == nodePoint->banners[BANNER_H_CONTESTED])
        nodePoint->nodeState = NODE_STATE_CONFLICT_H;

    uint32 worldstate = nodePoint->worldStates[nodePoint->nodeState];

    // with this we are sure we dont bug the client
    for (uint8 i = 0; i < 4; ++i)
        UpdateWorldState(nodePoint->worldStates[i], 0);

    UpdateWorldState(worldstate, 1);
}

uint32 BattlegroundIC::GetNextBanner(ICNodePoint* nodePoint, uint32 team, bool returnDefinitve)
{
    if (returnDefinitve)// this is a clock-delayed capture
    {
        SendMessage2ToAll(LANG_BG_IC_TEAM_HAS_TAKEN_NODE, (team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE), NULL, (team == TEAM_ALLIANCE ? LANG_BG_IC_ALLIANCE : LANG_BG_IC_HORDE), nodePoint->string);
        PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_CAPTURED_ALLIANCE : BASE_CAPTURED_HORDE));
        nodePoint->hasBeenCaptured = 1;
        return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTROLLED : BANNER_H_CONTROLLED)];
    }
    else
    { //this is a fresh attack on the point.
        switch (nodePoint->nodeState) { //what is the current state of the node?
        case 0://uncontrolled
            PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_ASSAULTED_ALLIANCE : BASE_ASSAULTED_HORDE));
            return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
            break;
        case 1://assaulted by alliance
            if (nodePoint->hasBeenCaptured == 0) { //this base has never been captured
                PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_ASSAULTED_ALLIANCE : BASE_ASSAULTED_HORDE));
                return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
            }
            else {
                PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_CAPTURED_ALLIANCE : BASE_CAPTURED_HORDE));
                return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTROLLED : BANNER_H_CONTROLLED)];
            }
            break;
        case 2://assaulted by horde
            if (nodePoint->hasBeenCaptured == 0) { //this base has never been captured
                PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_ASSAULTED_ALLIANCE : BASE_ASSAULTED_HORDE));
                return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
            }
            else {
                PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_CAPTURED_ALLIANCE : BASE_CAPTURED_HORDE));
                return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTROLLED : BANNER_H_CONTROLLED)];
            }
            break;
        case 3://controlled by alliance
            PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_ASSAULTED_ALLIANCE : BASE_ASSAULTED_HORDE));
            return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
            break;
        case 4://controlled by horde
            PlaySoundToAll((team == TEAM_ALLIANCE ? BASE_ASSAULTED_ALLIANCE : BASE_ASSAULTED_HORDE));
            return nodePoint->banners[(team == TEAM_ALLIANCE ? BANNER_A_CONTESTED : BANNER_H_CONTESTED)];
            break;
    
        default:
            TC_LOG_ERROR("bg.battleground", "Isle Of Conquest: Unexpected return in GetNextBanner function");
            return 0;
            break;
        }
    }
    // we should never be here...
    return 0;
}

void BattlegroundIC::HandleContestedNodes(ICNodePoint* nodePoint)
{
    if (nodePoint->nodeType == NODE_TYPE_HANGAR)
    {
        if (gunshipAlliance && gunshipHorde)
            (nodePoint->faction == TEAM_ALLIANCE ? gunshipHorde : gunshipAlliance)->BuildStopMovePacket(GetBgMap());

        if (nodePoint->hasBeenCaptured == 1) {
            for (uint32 u = BG_IC_GO_HANGAR_TELEPORTER_2; u <= BG_IC_GO_HANGAR_TELEPORTER_EFFECT_3; ++u) {
                if (GetBGObject(u)) {
                    GetBGObject(u)->SetGoState(GO_STATE_READY);
                    GetBGObject(u)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
            }
        }
        //begin force teleport
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr) {
            if (auto p = ObjectAccessor::FindPlayer(itr->first)) {
                if (p->GetPositionZ() > 180.0f) {
                    if (p->GetVehicle()) {
                        p->ExitVehicle();
                    }
                    p->CastSpell(p, 94442, true);
                    p->TeleportTo(GetMapId(), 828.382629f, -994.389587f, 134.408859f, 3.546978f); //next to base, facing base.
                }
            }
        }
        //end force teleport
    }
}

void BattlegroundIC::HandleCapturedNodes(ICNodePoint* nodePoint, bool recapture)
{
    if (nodePoint->nodeType != NODE_TYPE_REFINERY && nodePoint->nodeType != NODE_TYPE_QUARRY)
    {
        if (!AddSpiritGuide(BG_IC_NPC_SPIRIT_GUIDE_1 + nodePoint->nodeType - 2,
            BG_IC_SpiritGuidePos[nodePoint->nodeType][0], BG_IC_SpiritGuidePos[nodePoint->nodeType][1],
            BG_IC_SpiritGuidePos[nodePoint->nodeType][2], BG_IC_SpiritGuidePos[nodePoint->nodeType][3],
            (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE)))
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: Failed to spawn spirit guide! point: %u, team: %u, ", nodePoint->nodeType, nodePoint->faction);
    }

    switch (nodePoint->gameobject_type)
    {
    case BG_IC_GO_HANGAR_BANNER:
        // all the players on the stopped transport should be teleported out
        if (!gunshipAlliance || !gunshipHorde)
            break;

        for (uint32 u = BG_IC_GO_HANGAR_TELEPORTER_1; u <= BG_IC_GO_HANGAR_TELEPORTER_EFFECT_3; ++u)
            DelObject(u);

        for (uint8 u = 0; u < 3; ++u)
        {
            uint32 type = BG_IC_GO_HANGAR_TELEPORTER_1 + u;
            AddObject(type, (nodePoint->faction == TEAM_ALLIANCE ? GO_ALLIANCE_GUNSHIP_PORTAL : GO_HORDE_GUNSHIP_PORTAL),
                BG_IC_HangarTeleporters[u].GetPositionX(), BG_IC_HangarTeleporters[u].GetPositionY(),
                BG_IC_HangarTeleporters[u].GetPositionZ(), BG_IC_HangarTeleporters[u].GetOrientation(),
                0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(type)->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
        }
        //GetBGObject(BG_IC_GO_HANGAR_TELEPORTER_1)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);//this one is never usable, just a cosmetic.
        for (uint8 u = 0; u < 3; ++u)
        {
            uint32 type = BG_IC_GO_HANGAR_TELEPORTER_EFFECT_1 + u;
            AddObject(type, (nodePoint->faction == TEAM_ALLIANCE ? GO_ALLIANCE_GUNSHIP_PORTAL_EFFECTS : GO_HORDE_GUNSHIP_PORTAL_EFFECTS),
                BG_IC_HangarTeleporters[u].GetPositionX(), BG_IC_HangarTeleporters[u].GetPositionY(),
                BG_IC_HangarTeleporters[u].GetPositionZ(), BG_IC_HangarTeleporters[u].GetOrientation(),
                0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(type)->SetGoState(GO_STATE_ACTIVE);
            GetBGObject(type)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }
        //TC_LOG_ERROR("bg.battleground", "BG_IC_GO_HANGAR_BANNER CAPTURED Faction: %u", nodePoint->faction);

        (nodePoint->faction == TEAM_ALLIANCE ? gunshipAlliance : gunshipHorde)->BuildStartMovePacket(GetBgMap());
        (nodePoint->faction == TEAM_ALLIANCE ? gunshipHorde : gunshipAlliance)->BuildStopMovePacket(GetBgMap());
        // we should spawn teleporters
        break;
    case BG_IC_GO_QUARRY_BANNER:
        RemoveAuraOnTeam(SPELL_QUARRY, (nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
        CastSpellOnTeam(SPELL_QUARRY, (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE));
        break;
    case BG_IC_GO_REFINERY_BANNER:
        RemoveAuraOnTeam(SPELL_OIL_REFINERY, (nodePoint->faction == TEAM_ALLIANCE ? HORDE : ALLIANCE));
        CastSpellOnTeam(SPELL_OIL_REFINERY, (nodePoint->faction == TEAM_ALLIANCE ? ALLIANCE : HORDE));
        break;
    case BG_IC_GO_DOCKS_BANNER:

        if (recapture)
            break;

        if (docksTimer < DOCKS_UPDATE_TIME)
            docksTimer = DOCKS_UPDATE_TIME;

        // we must del opposing faction vehicles when the node is captured (unused ones)
        for (uint8 i = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_H : BG_IC_NPC_GLAIVE_THROWER_1_A); i <= (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_2_H : BG_IC_NPC_GLAIVE_THROWER_2_A); ++i)
        {
            if (auto glaiveThrower = GetBGCreature(i))
            {
                if (Vehicle* vehicleGlaive = glaiveThrower->GetVehicleKit())
                {
                    if (!vehicleGlaive->GetPassenger(0))
                        DelCreature(i);
                    VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 0;
                    VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
                }
            }
        }

        for (uint8 i = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_H : BG_IC_NPC_CATAPULT_1_A); i <= (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_4_H : BG_IC_NPC_CATAPULT_4_A); ++i)
        {
            if (auto catapult = GetBGCreature(i))
            {
                if (Vehicle* vehicleGlaive = catapult->GetVehicleKit())
                {
                    if (!vehicleGlaive->GetPassenger(0))
                        DelCreature(i);
                    VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 0;
                    VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
                }
            }
        }

        // spawning glaive throwers
        for (uint8 i = 0; i < MAX_GLAIVE_THROWERS_SPAWNS_PER_FACTION; ++i)
        {
            uint32 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_GLAIVE_THROWER_1_A : BG_IC_NPC_GLAIVE_THROWER_1_H) + i;

            if (GetBGCreature(type) && GetBGCreature(type)->isAlive())
                continue;

            if (AddCreature(nodePoint->faction == TEAM_ALLIANCE ? NPC_GLAIVE_THROWER_A : NPC_GLAIVE_THROWER_H, type, nodePoint->faction,
                BG_IC_DocksVehiclesGlaives[i].GetPositionX(), BG_IC_DocksVehiclesGlaives[i].GetPositionY(),
                BG_IC_DocksVehiclesGlaives[i].GetPositionZ(), BG_IC_DocksVehiclesGlaives[i].GetOrientation(),
                RESPAWN_ONE_DAY)) {
                GetBGCreature(type)->setFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 1;
                VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
            }
        }

        // spawning catapults
        for (uint8 i = 0; i < MAX_CATAPULTS_SPAWNS_PER_FACTION; ++i)
        {
            uint32 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_CATAPULT_1_A : BG_IC_NPC_CATAPULT_1_H) + i;

            if (GetBGCreature(type) && GetBGCreature(type)->isAlive())
                continue;

            if (AddCreature(NPC_CATAPULT, type, nodePoint->faction,
                BG_IC_DocksVehiclesCatapults[i].GetPositionX(), BG_IC_DocksVehiclesCatapults[i].GetPositionY(),
                BG_IC_DocksVehiclesCatapults[i].GetPositionZ(), BG_IC_DocksVehiclesCatapults[i].GetOrientation(),
                RESPAWN_ONE_DAY)) {
                GetBGCreature(type)->setFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 1;
                VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
            }
        }
        break;
    case BG_IC_GO_WORKSHOP_BANNER:
    {
        if (siegeEngineWorkshopTimer < WORKSHOP_UPDATE_TIME)
            siegeEngineWorkshopTimer = WORKSHOP_UPDATE_TIME;

        if (!recapture)
        {
            /************************************************************** PURGE BASE VEHICLES ********************************************************************/
            for (uint32 i = BG_IC_NPC_SIEGE_ENGINE_A; i <= BG_IC_NPC_DEMOLISHER_4_H; ++i)
            {
                if (auto demolisher = GetBGCreature(i))
                    if (demolisher->GetVehicleKit())
                        if (!demolisher->GetVehicleKit()->GetPassenger(0))
                        {
                            //we WILL leave behind currently occupied enemy vehicles until they die.
                            //we'll just check if the vehicle is being used again when their team retakes the base before attempting to spawn it.
                            DelCreature(i);
                            VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 0;
                            VehicleRespawnStatus[i - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
                        }

                /************************************************************* SPAWN DEMOLISHERS *****************************************************************/
                for (uint8 i = 0; i < MAX_DEMOLISHERS_SPAWNS_PER_FACTION; ++i)
                {
                    uint8 type = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_DEMOLISHER_1_A : BG_IC_NPC_DEMOLISHER_1_H) + i;

                    if (GetBGCreature(type) && GetBGCreature(type)->isAlive())
                        continue;

                    if (AddCreature(NPC_DEMOLISHER, type, nodePoint->faction,
                        BG_IC_WorkshopVehicles[i].GetPositionX(), BG_IC_WorkshopVehicles[i].GetPositionY(),
                        BG_IC_WorkshopVehicles[i].GetPositionZ(), BG_IC_WorkshopVehicles[i].GetOrientation(),
                        RESPAWN_ONE_DAY)) {
                        GetBGCreature(type)->setFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);//make sure the vehicle can actually be used.
                        VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 1;
                        VehicleRespawnStatus[type - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
                    }
                }

                /************************************************************* SPAWN SIEGE ENGINE *****************************************************************/
                uint32 siegeType = (nodePoint->faction == TEAM_ALLIANCE ? BG_IC_NPC_SIEGE_ENGINE_A : BG_IC_NPC_SIEGE_ENGINE_H);
                if (!GetBGCreature(siegeType) || !GetBGCreature(siegeType)->isAlive())
                {
                    AddCreature((nodePoint->faction == TEAM_ALLIANCE ? NPC_SIEGE_ENGINE_A : NPC_SIEGE_ENGINE_H), siegeType, nodePoint->faction,
                        BG_IC_WorkshopVehicles[4].GetPositionX(), BG_IC_WorkshopVehicles[4].GetPositionY(),
                        BG_IC_WorkshopVehicles[4].GetPositionZ(), BG_IC_WorkshopVehicles[4].GetOrientation(),
                        RESPAWN_ONE_DAY);

                    if (auto siegeEngine = GetBGCreature(siegeType)) //this siege was actually spawned and exists.
                    {
                        siegeEngine->setFaction(BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                        siegeEngine->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);//make sure the vehicle can actually be used.
                        siegeEngine->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_UNK_14 | UNIT_FLAG_IMMUNE_TO_PC);
                        //siegeEngine->SetPhaseMask(2, true);
                        //siege engine, now that it has attached cannons, can't be phased away from them.
                        VehicleRespawnStatus[siegeType - BG_IC_NPC_SIEGE_ENGINE_A].npc_state = 1;
                        VehicleRespawnStatus[siegeType - BG_IC_NPC_SIEGE_ENGINE_A].time_since_death = 0;
                    }
                }
            }

            /************************************************************ SPAWN BOMBS AT WORKSHOP *****************************************************************/
            for (uint8 i = 0; i < MAX_WORKSHOP_BOMBS_SPAWNS_PER_FACTION; ++i)
            {
                AddObject(BG_IC_GO_SEAFORIUM_BOMBS_1 + i, GO_SEAFORIUM_BOMBS,
                    workshopBombs[i].GetPositionX(), workshopBombs[i].GetPositionY(),
                    workshopBombs[i].GetPositionZ(), workshopBombs[i].GetOrientation(),
                    0, 0, 0, 0, 10);

                if (auto seaforiumBombs = GetBGObject(BG_IC_GO_SEAFORIUM_BOMBS_1 + i))
                {
                    seaforiumBombs->SetRespawnTime(10);
                    seaforiumBombs->SetUInt32Value(GAMEOBJECT_FACTION, BG_IC_Factions[(nodePoint->faction == TEAM_ALLIANCE ? 0 : 1)]);
                }
            }
            /************************************************************ END CAPTURE WORKSHOP *****************************************************************/
            break;
        }
    default:
        break;
    }
    }
}

void BattlegroundIC::DestroyGate(Player* player, GameObject* go)
{
    GateStatus[GetGateIDFromEntry(go->GetEntry())] = BG_IC_GATE_DESTROYED;
    uint32 uws_open = GetWorldStateFromGateEntry(go->GetEntry(), true);
    uint32 uws_close = GetWorldStateFromGateEntry(go->GetEntry(), false);
    if (uws_open)
    {
        UpdateWorldState(uws_close, 0);
        UpdateWorldState(uws_open, 1);
    }
    //insert making banner interactable and delay the door opening until that banner is taken?
    DoorOpen((player->GetTeamId() == TEAM_ALLIANCE ? BG_IC_GO_HORDE_KEEP_PORTCULLIS : BG_IC_GO_DOODAD_PORTCULLISACTIVE02));

    GetBGObject((player->GetTeamId() == TEAM_ALLIANCE ? BG_IC_GO_HORDE_BANNER : BG_IC_GO_ALLIANCE_BANNER))->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
    uint32 lang_entry = 0;

    switch (go->GetEntry())
    {
        case GO_HORDE_GATE_1:
            lang_entry = LANG_BG_IC_SOUTH_GATE_DESTROYED;
            break;
        case GO_HORDE_GATE_2:
        case GO_ALLIANCE_GATE_1:
            lang_entry = LANG_BG_IC_WEST_GATE_DESTROYED;
            break;
        case GO_HORDE_GATE_3:
        case GO_ALLIANCE_GATE_2:
            lang_entry = LANG_BG_IC_EAST_GATE_DESTROYED;
            break;
        case GO_ALLIANCE_GATE_3:
            lang_entry = LANG_BG_IC_NORTH_GATE_DESTROYED;
            break;
    default:
        break;
    }

    if (go->GetEntry() == GO_HORDE_GATE_1 || go->GetEntry() == GO_HORDE_GATE_2 || go->GetEntry() == GO_HORDE_GATE_3)
    {
        if (!GetBgMap()->GetCreature(BgCreatures[BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].type]) && !AddCreature(BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].entry, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].type, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].team, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].x, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].y, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].z, BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].o, RESPAWN_ONE_DAY))
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning creature %u", BG_IC_NpcSpawnlocs[BG_IC_NPC_OVERLORD_AGMAR].entry);
    }
    else if (go->GetEntry() == GO_ALLIANCE_GATE_1 || go->GetEntry() == GO_ALLIANCE_GATE_2 || go->GetEntry() == GO_ALLIANCE_GATE_3)
    {
        if (!GetBgMap()->GetCreature(BgCreatures[BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].type]) && !AddCreature(BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].entry, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].type, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].team, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].x, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].y, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].z, BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].o, RESPAWN_ONE_DAY))
            TC_LOG_ERROR("bg.battleground", "Isle of Conquest: There was an error spawning creature %u", BG_IC_NpcSpawnlocs[BG_IC_NPC_HIGH_COMMANDER_HALFORD_WYRMBANE].entry);
    }

    SendMessage2ToAll(lang_entry, (player->GetTeamId() == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE), NULL, (player->GetTeamId() == TEAM_ALLIANCE ? LANG_BG_IC_HORDE_KEEP : LANG_BG_IC_ALLIANCE_KEEP));
    player->GetTeamId() == TEAM_ALLIANCE ? (Horde_boss_spawned = true) : (Alliance_boss_spawned = true);
}

void BattlegroundIC::EventPlayerDamagedGO(Player* /*player*/, GameObject* /*go*/, uint32 /*eventType*/)
{

}

WorldSafeLocsEntry const* BattlegroundIC::GetClosestGraveYard(Player* player)
{
    TeamId teamIndex = GetTeamIndexByTeamId(player->GetTeam());

    // Is there any occupied node for this team?
    std::vector<uint8> nodes;
    for (uint8 i = 0; i < MAX_NODE_TYPES; ++i)
        if (nodePoint[i].faction == player->GetTeamId())
            nodes.push_back(i);

    WorldSafeLocsEntry const* good_entry = NULL;
    // If so, select the closest node to place ghost on
    if (!nodes.empty())
    {
        float plr_x = player->GetPositionX();
        float plr_y = player->GetPositionY();

        float mindist = 999999.0f;
        for (uint8 i = 0; i < nodes.size(); ++i)
        {
            WorldSafeLocsEntry const*entry = sWorldSafeLocsStore.LookupEntry(BG_IC_GraveyardIds[nodes[i]]);
            if (!entry)
                continue;
            float dist = (entry->x - plr_x)*(entry->x - plr_x)+(entry->y - plr_y)*(entry->y - plr_y);
            if (mindist > dist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }
        nodes.clear();
    }
    // If not, place ghost on starting location
    if (!good_entry)
        good_entry = sWorldSafeLocsStore.LookupEntry(BG_IC_GraveyardIds[teamIndex+MAX_NODE_TYPES]);

    return good_entry;
}
void BattlegroundIC::HandleVehicleRespawns(uint32 diff)
{
    for (uint32 i = 0; i < MAX_RESPAWNABLE_VEHICLES; ++i) {
        if (VehicleRespawnStatus[i].npc_state == 2) {
            if (VehicleRespawnStatus[i].time_since_death < 180000) {
                VehicleRespawnStatus[i].time_since_death = (VehicleRespawnStatus[i].time_since_death + diff);
                //TC_LOG_ERROR("bg.battleground", "Respawn vehicle [%u] - Bg Creature [%u] has been dead for %u mS.", i, VehicleRespawns[i].v_type, VehicleRespawnStatus[i].time_since_death);
            }
            else {
                VehicleRespawnStatus[i].time_since_death = 0;
                DelCreature(VehicleRespawns[i].v_type);
                switch (VehicleRespawns[i].v_type) {

                case BG_IC_NPC_SIEGE_ENGINE_A:
                case BG_IC_NPC_SIEGE_ENGINE_H:
                    if ((nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_WorkshopVehicles[4].GetPositionX(), BG_IC_WorkshopVehicles[4].GetPositionY(), BG_IC_WorkshopVehicles[4].GetPositionZ(), BG_IC_WorkshopVehicles[4].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        //TC_LOG_ERROR("bg.battleground", "Failed to spawn from VehicleRespawns[%u], vehicle spawn faction is %u and base faction is %u.", i, VehicleRespawns[i].v_faction, nodePoint[NODE_TYPE_WORKSHOP].faction);
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_DEMOLISHER_1_A:
                case BG_IC_NPC_DEMOLISHER_1_H:
                    if ((nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_WorkshopVehicles[0].GetPositionX(), BG_IC_WorkshopVehicles[0].GetPositionY(), BG_IC_WorkshopVehicles[0].GetPositionZ(), BG_IC_WorkshopVehicles[0].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        //TC_LOG_ERROR("bg.battleground", "Failed to spawn from VehicleRespawns[%u], vehicle spawn faction is %u and base state is %u, cannot be equal to %u or %u.", i, VehicleRespawns[i].v_faction, nodePoint[NODE_TYPE_WORKSHOP].nodeState, (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H:NODE_STATE_CONTROLLED_A), (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A:NODE_STATE_CONFLICT_H));
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_DEMOLISHER_2_H:
                case BG_IC_NPC_DEMOLISHER_2_A:
                    if ((nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_WorkshopVehicles[1].GetPositionX(), BG_IC_WorkshopVehicles[1].GetPositionY(), BG_IC_WorkshopVehicles[1].GetPositionZ(), BG_IC_WorkshopVehicles[1].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        //TC_LOG_ERROR("bg.battleground", "Failed to spawn from VehicleRespawns[%u], vehicle spawn faction is %u and base faction is %u.", i, VehicleRespawns[i].v_faction, nodePoint[NODE_TYPE_WORKSHOP].faction);
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_DEMOLISHER_3_A:
                case BG_IC_NPC_DEMOLISHER_3_H:
                    if ((nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_WorkshopVehicles[2].GetPositionX(), BG_IC_WorkshopVehicles[2].GetPositionY(), BG_IC_WorkshopVehicles[2].GetPositionZ(), BG_IC_WorkshopVehicles[2].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        //TC_LOG_ERROR("bg.battleground", "Failed to spawn from VehicleRespawns[%u], vehicle spawn faction is %u and base faction is %u.", i, VehicleRespawns[i].v_faction, nodePoint[NODE_TYPE_WORKSHOP].faction);
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_DEMOLISHER_4_A:
                case BG_IC_NPC_DEMOLISHER_4_H:
                    if ((nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_WORKSHOP].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_WorkshopVehicles[3].GetPositionX(), BG_IC_WorkshopVehicles[3].GetPositionY(), BG_IC_WorkshopVehicles[3].GetPositionZ(), BG_IC_WorkshopVehicles[3].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                        //TC_LOG_ERROR("bg.battleground", "BgCreature[%u] states: NPC STATE: %u, time since death: %u", VehicleRespawns[i].v_type, VehicleRespawnStatus[i].npc_state, VehicleRespawnStatus[i].time_since_death);
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                        //TC_LOG_ERROR("bg.battleground", "This NPC has been reset: %u and BgCreatures[%u]", i, VehicleRespawns[i].v_type);
                    }
                    break;

                case BG_IC_NPC_GLAIVE_THROWER_1_A:
                case BG_IC_NPC_GLAIVE_THROWER_1_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesGlaives[0].GetPositionX(), BG_IC_DocksVehiclesGlaives[0].GetPositionY(), BG_IC_DocksVehiclesGlaives[0].GetPositionZ(), BG_IC_DocksVehiclesGlaives[0].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_GLAIVE_THROWER_2_A:
                case BG_IC_NPC_GLAIVE_THROWER_2_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesGlaives[1].GetPositionX(), BG_IC_DocksVehiclesGlaives[1].GetPositionY(), BG_IC_DocksVehiclesGlaives[1].GetPositionZ(), BG_IC_DocksVehiclesGlaives[1].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_CATAPULT_1_A:
                case BG_IC_NPC_CATAPULT_1_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesCatapults[0].GetPositionX(), BG_IC_DocksVehiclesCatapults[0].GetPositionY(), BG_IC_DocksVehiclesCatapults[0].GetPositionZ(), BG_IC_DocksVehiclesCatapults[0].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_CATAPULT_2_A:
                case BG_IC_NPC_CATAPULT_2_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesCatapults[1].GetPositionX(), BG_IC_DocksVehiclesCatapults[1].GetPositionY(), BG_IC_DocksVehiclesCatapults[1].GetPositionZ(), BG_IC_DocksVehiclesCatapults[1].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_CATAPULT_3_A:
                case BG_IC_NPC_CATAPULT_3_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesCatapults[2].GetPositionX(), BG_IC_DocksVehiclesCatapults[2].GetPositionY(), BG_IC_DocksVehiclesCatapults[2].GetPositionZ(), BG_IC_DocksVehiclesCatapults[2].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                case BG_IC_NPC_CATAPULT_4_A:
                case BG_IC_NPC_CATAPULT_4_H:
                    if ((nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONTROLLED_H : NODE_STATE_CONTROLLED_A)) && (nodePoint[NODE_TYPE_DOCKS].nodeState != (VehicleRespawns[i].v_faction == 0 ? NODE_STATE_CONFLICT_A : NODE_STATE_CONFLICT_H))) {
                        AddCreature(VehicleRespawns[i].v_entry, VehicleRespawns[i].v_type, VehicleRespawns[i].v_faction, BG_IC_DocksVehiclesCatapults[3].GetPositionX(), BG_IC_DocksVehiclesCatapults[3].GetPositionY(), BG_IC_DocksVehiclesCatapults[3].GetPositionZ(), BG_IC_DocksVehiclesCatapults[3].GetOrientation(), RESPAWN_VEHICLE);
                        GetBGCreature(VehicleRespawns[i].v_type)->setFaction(BG_IC_Factions[(nodePoint[NODE_TYPE_WORKSHOP].faction == TEAM_ALLIANCE ? 0 : 1)]);
                        GetBGCreature(VehicleRespawns[i].v_type)->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        VehicleRespawnStatus[i].npc_state = 1; //on successful spawn
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    else
                    {
                        VehicleRespawnStatus[i].npc_state = 0;//base is no longer taken, forget this respawn.
                        VehicleRespawnStatus[i].time_since_death = 0;
                    }
                    break;

                default:
                    //TC_LOG_ERROR("bg.battleground", "This NPC failed to respawn: %u and BgCreatures[%u]", i, VehicleRespawns[i].v_type);
                    break;
                }
            }
        } /*else {
            if (VehicleRespawns[i].v_type == 61) {
                TC_LOG_ERROR("bg.battleground", "BgCreature[%u] states: NPC STATE: %u, time since death: %u", VehicleRespawns[i].v_type, VehicleRespawnStatus[i].npc_state, VehicleRespawnStatus[i].time_since_death);
            }
        }*/
    }
}


Transport* BattlegroundIC::CreateTransport(uint32 goEntry, uint32 period)
{
    Transport *t = sMapMgr->LoadTransportInMap(GetBgMap(), goEntry, period);

    for (uint8 i = 0; i <= 5; ++i)
        t->AddNPCPassenger(i,
            (goEntry == GO_HORDE_GUNSHIP ? (i < 5 ? NPC_HORDE_GUNSHIP_CANNON : NPC_GUNSHIP_H) : (i < 5 ? NPC_ALLIANCE_GUNSHIP_CANNON : NPC_GUNSHIP_A)),
            (goEntry == GO_HORDE_GUNSHIP ? hordeGunshipPassengers[i].GetPositionX() : allianceGunshipPassengers[i].GetPositionX()),
            (goEntry == GO_HORDE_GUNSHIP ? hordeGunshipPassengers[i].GetPositionY() : allianceGunshipPassengers[i].GetPositionY()),
            (goEntry == GO_HORDE_GUNSHIP ? hordeGunshipPassengers[i].GetPositionZ() : allianceGunshipPassengers[i].GetPositionZ()),
            (goEntry == GO_HORDE_GUNSHIP ? hordeGunshipPassengers[i].GetOrientation() : allianceGunshipPassengers[i].GetOrientation()));
    return t;
}
