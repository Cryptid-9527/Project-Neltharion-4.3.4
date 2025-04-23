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

#include "BattlegroundSA.h"
#include "Language.h"
#include "Player.h"
#include "GameObject.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Transport.h"

BattlegroundSA::BattlegroundSA()
{
    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SA_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_SA_HAS_BEGUN;
    BgObjects.resize(BG_SA_MAXOBJ);
    BgCreatures.resize(BG_SA_MAXNPC + BG_SA_MAX_GY);
    TimerEnabled = false;
    UpdateWaitTimer = 0;
    SignaledRoundTwo = false;
    SignaledRoundTwoHalfMin = false;
    InitSecondRound = false;

    //! This is here to prevent an uninitialised variable warning
    //! The warning only occurs when SetUpBattleGround fails though.
    //! In the future this function should be called BEFORE sending initial worldstates.
    memset(&GraveyardStatus, 0, sizeof(GraveyardStatus));
}

BattlegroundSA::~BattlegroundSA()
{
}

void BattlegroundSA::Reset()
{
    TotalTime = 0;
    Attackers = ((urand(0, 1)) ? TEAM_ALLIANCE : TEAM_HORDE);
    for (uint8 i = 0; i <= 5; i++)
        GateStatus[i] = BG_SA_GATE_OK;
    ShipsStarted = false;
    gateDestroyed = false;
    _notEvenAScratch[TEAM_ALLIANCE] = true;
    _notEvenAScratch[TEAM_HORDE] = true;
    Status = BG_SA_WARMUP;
}

bool BattlegroundSA::SetupBattleground()
{
    return ResetObjs();
}

bool BattlegroundSA::ResetObjs()
{
    for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (auto player = ObjectAccessor::FindPlayer(itr->first))
            SendTransportsRemove(player);
    uint8 removal_timer_BG_SA_BLUE_GATE = 0;
    uint8 removal_timer_BG_SA_RED_GATE = 0;
    uint8 removal_timer_BG_SA_GREEN_GATE = 0;
    uint8 removal_timer_BG_SA_PURPLE_GATE = 0;
    uint8 removal_timer_BG_SA_YELLOW_GATE = 0;

    uint32 atF = BG_SA_Factions[Attackers];
    uint32 defF = BG_SA_Factions[Attackers ? TEAM_ALLIANCE : TEAM_HORDE];

    for (uint8 i = 0; i < BG_SA_MAXOBJ; i++)
        DelObject(i);

    for (uint8 i = 0; i < BG_SA_MAXNPC; i++)
        DelCreature(i);

    for (uint8 i = BG_SA_MAXNPC; i < BG_SA_MAXNPC + BG_SA_MAX_GY; i++)
        DelCreature(i);

    for (uint8 i = 0; i < MAX_GATES; i++)
        GateStatus[i] = BG_SA_GATE_OK;

    for (uint8 i = 0; i <= BG_SA_PORTAL_DEFFENDER_RED; i++)
        if (!AddObject(i, BG_SA_ObjEntries[i], BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, RESPAWN_ONE_DAY))
            return false;

    for (uint8 i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
    {
        uint32 boatid = 0;
        switch (i)
        {
            case BG_SA_BOAT_ONE:
                boatid = Attackers ? BG_SA_BOAT_ONE_H : BG_SA_BOAT_ONE_A;
                break;
            case BG_SA_BOAT_TWO:
                boatid = Attackers ? BG_SA_BOAT_TWO_H : BG_SA_BOAT_TWO_A;
                break;
            default:
                break;
        }
        if (!AddObject(i, boatid, BG_SA_ObjSpawnlocs[i][0],
          BG_SA_ObjSpawnlocs[i][1],
          BG_SA_ObjSpawnlocs[i][2]+(Attackers ? -3.750f: 0),
          BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, RESPAWN_ONE_DAY))
            return false;
    }

    /*
    */

    for (int i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
        if (auto ship = GetBGObject(i))
        {
            ship->SetControlableTransport();
            ship->SetGoState(GO_STATE_TRANSPORT_SPEC);

            //ship->SetGoState(GO_STATE_ACTIVE);    //GO_STATE_ACTIVE: appears in water, but causes anticheat issues.
            //ship->SetGoState(GO_STATE_READY);       //GO_STATE_READY: appears at the dock, only visible from near water spawn.
            //ship->SetGoState(GO_STATE_TRANSPORT_READY);     //GO_STATE_TRANSPORT_READY: appears at the dock, only visible from near water spawn.
        }


    for (uint8 i = BG_SA_SIGIL_1; i <= BG_SA_LEFT_FLAGPOLE; i++)
    {
        if (!AddObject(i, BG_SA_ObjEntries[i],
            BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
            BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
            0, 0, 0, 0, RESPAWN_ONE_DAY))
        return false;
    }

    // MAD props for Kiper for discovering those values - 4 hours of his work.
    if (auto boat_one = GetBGObject(BG_SA_BOAT_ONE))
        boat_one->UpdateRotationFields(1.0f, 0.0002f);

    if (auto boat_two = GetBGObject(BG_SA_BOAT_TWO))
        boat_two->UpdateRotationFields(1.0f, 0.00001f);

    SpawnBGObject(BG_SA_BOAT_ONE, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_SA_BOAT_TWO, RESPAWN_IMMEDIATELY);

    //Cannons and demolishers - NPCs are spawned
    //By capturing GYs.
    for (uint8 i = 0; i < BG_SA_DEMOLISHER_5; i++)
    {
        if (auto creature = AddCreature(BG_SA_NpcEntries[i], i, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
            BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1],
            BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3], 600))
            creature->SetReactState(REACT_PASSIVE);
        else
        return false;
    }

    for (uint8 i = BG_SA_FLAGPOLE_TRIGGER_1; i <= BG_SA_FLAGPOLE_TRIGGER_3; i++)
    {
        if (auto creature = AddCreature(BG_SA_NpcEntries[i], i, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
            BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1],
            BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3], 600))
            creature->SetReactState(REACT_PASSIVE);
        else
            return false;
    }
    OverrideGunFaction();
    DemolisherStartState(true);

    for (uint8 i = 0; i <= BG_SA_PORTAL_DEFFENDER_RED; i++)
    {
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
        GetBGObject(i)->SetUInt32Value(GAMEOBJECT_FACTION, defF);
    }

    GetBGObject(BG_SA_TITAN_RELIC)->SetUInt32Value(GAMEOBJECT_FACTION, atF);
    GetBGObject(BG_SA_TITAN_RELIC)->Refresh();

    TotalTime = 0;
    destroyedGates = 0;
    ShipsStarted = false;

    //Graveyards
    for (uint8 i = 0; i < BG_SA_MAX_GY; i++)
    {
        WorldSafeLocsEntry const* sg = NULL;
        sg = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);

        if (!sg)
        {
            TC_LOG_ERROR("bg.battleground", "SOTA: Can't find GY entry %u", BG_SA_GYEntries[i]);
            return false;
        }

        if (i == BG_SA_BEACH_GY)
        {
            GraveyardStatus[i] = Attackers;
            AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], ((Attackers == TEAM_HORDE)? HORDE : ALLIANCE));
        }
        else
        {
            GraveyardStatus[i] = ((Attackers == TEAM_HORDE)? TEAM_ALLIANCE : TEAM_HORDE);
            if (!AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], ((Attackers == TEAM_HORDE)? ALLIANCE : HORDE)))
                TC_LOG_ERROR("bg.battleground", "SOTA: couldn't spawn GY: %u", i);
        }
    }

    //GY capture points
    for (uint8 i = BG_SA_CENTRAL_FLAG; i <= BG_SA_LEFT_FLAG; i++)
    {
        AddObject(i, (BG_SA_ObjEntries[i] - (Attackers == TEAM_ALLIANCE ? 1 : 0)),
                  BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
                  BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
                  0, 0, 0, 0, RESPAWN_ONE_DAY);
        GetBGObject(i)->SetUInt32Value(GAMEOBJECT_FACTION, atF);
    }

    UpdateObjectInteractionFlags();

    for (uint8 i = BG_SA_BOMB_FIRST_STAGE; i < BG_SA_MAXOBJ; i++)
    {
        if (BG_SA_ObjSpawnlocs[i][0] > 1500)
        {
            AddObject(i, BG_SA_ObjEntries[BG_SA_BOMB_FIRST_STAGE],
                      BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
                      BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
                      0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(i)->SetUInt32Value(GAMEOBJECT_FACTION, atF);
        }
    }

    //Player may enter BEFORE we set up BG - lets update his worldstates anyway...
    UpdateWorldState(BG_SA_RIGHT_GY_HORDE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    UpdateWorldState(BG_SA_LEFT_GY_HORDE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
    UpdateWorldState(BG_SA_CENTER_GY_HORDE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);

    UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
    UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);

    if (Attackers == TEAM_ALLIANCE)
    {
        UpdateWorldState(BG_SA_ALLY_ATTACKS, 1);
        UpdateWorldState(BG_SA_HORDE_ATTACKS, 0);

        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_ALL, 1);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_ALL, 1);
        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_HRD, 0);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_HRD, 0);

        UpdateWorldState(BG_SA_HORDE_DEFENCE_TOKEN, 1);
        UpdateWorldState(BG_SA_ALLIANCE_DEFENCE_TOKEN, 0);
    }
    else
    {
        UpdateWorldState(BG_SA_HORDE_ATTACKS, 1);
        UpdateWorldState(BG_SA_ALLY_ATTACKS, 0);

        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_ALL, 0);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_ALL, 0);
        UpdateWorldState(BG_SA_RIGHT_ATT_TOKEN_HRD, 1);
        UpdateWorldState(BG_SA_LEFT_ATT_TOKEN_HRD, 1);

        UpdateWorldState(BG_SA_HORDE_DEFENCE_TOKEN, 0);
        UpdateWorldState(BG_SA_ALLIANCE_DEFENCE_TOKEN, 1);
    }

    UpdateWorldState(BG_SA_PURPLE_GATEWS, 1);
    UpdateWorldState(BG_SA_RED_GATEWS, 1);
    UpdateWorldState(BG_SA_BLUE_GATEWS, 1);
    UpdateWorldState(BG_SA_GREEN_GATEWS, 1);
    UpdateWorldState(BG_SA_YELLOW_GATEWS, 1);
    UpdateWorldState(BG_SA_ANCIENT_GATEWS, 1);


    for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (auto player = ObjectAccessor::FindPlayer(itr->first))
            SendTransportInit(player);

    // set status manually so preparation is cast correctly in 2nd round too
    if (InitSecondRound) {
        SetStatus(STATUS_WAIT_JOIN);
        SetElapsedTime(45000);
    }
    TeleportPlayers();
    return true;
}

void BattlegroundSA::StartShips()
{
    if (ShipsStarted)
        return;

    for (int i = BG_SA_BOAT_ONE; i <= BG_SA_BOAT_TWO; i++)
        if (auto ship = GetBGObject(i))
        {
            ship->SetGoState(GOState(GO_STATE_TRANSPORT_SPEC + 1));
            for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (auto p = ObjectAccessor::FindPlayer(itr->first))
                    SendTransportUpdate(p);
        }

    ShipsStarted = true;
}

void BattlegroundSA::PostUpdateImpl(uint32 diff)
{
    if (Rigspark_Yell_status == 1) {
        if (Rigspark_Yell_counter >= 4000) {
            YellToAll(GetBGCreature(BG_SA_NPC_RIGSPARK), "Rigspark's vehicle shop is open for business!", LANG_UNIVERSAL);
            Rigspark_Yell_counter = 0;
            Rigspark_Yell_status = 0;
        }
        else {
            Rigspark_Yell_counter += diff;
        }
    }
    if (Sparklight_Yell_status == 1) {
        if (Sparklight_Yell_counter >= 4000) {
            YellToAll(GetBGCreature(BG_SA_NPC_SPARKLIGHT), "Sparklight's vehicle emporium is open for business!", LANG_UNIVERSAL);
            Sparklight_Yell_counter = 0;
            Sparklight_Yell_status = 0;
        }
        else {
            Sparklight_Yell_counter += diff;
        }
    }
    if (startships_delayed_trigger == 1) {
        if (startships_delayed < 10000) {
            startships_delayed += diff;
        }
        else {
            StartShips();
            startships_delayed_trigger = 0;
            startships_delayed = 0;
        }
    }
    //begin removing gates after destruction
//commented out for now, since removing the objects will NOT fix the LOS issue.
//I moved the blue gate spawn and everything nearby away so that nothing would spawn near blue gate. The LOS issue persisted.
/*
if (removal_timer_BG_SA_BLUE_GATE > 0)
{
    if (removal_timer_BG_SA_BLUE_GATE < 5000) {
        removal_timer_BG_SA_BLUE_GATE += diff;
    } else {
        //DelObject(BG_SA_BLUE_GATE);
        removal_timer_BG_SA_BLUE_GATE = 0;
    }
}
if (removal_timer_BG_SA_RED_GATE > 0)
{
    if (removal_timer_BG_SA_RED_GATE < 5000) {
        removal_timer_BG_SA_RED_GATE += diff;
    } else {
        //DelObject(BG_SA_RED_GATE);
        removal_timer_BG_SA_RED_GATE = 0;
    }
}
if (removal_timer_BG_SA_YELLOW_GATE > 0)
{
    if (removal_timer_BG_SA_YELLOW_GATE < 5000) {
        removal_timer_BG_SA_YELLOW_GATE += diff;
    } else {
        //DelObject(BG_SA_YELLOW_GATE);
        removal_timer_BG_SA_YELLOW_GATE = 0;
    }
}
if (removal_timer_BG_SA_PURPLE_GATE > 0)
{
    if (removal_timer_BG_SA_PURPLE_GATE < 5000) {
        removal_timer_BG_SA_PURPLE_GATE += diff;
    } else {
        //DelObject(BG_SA_PURPLE_GATE);
        removal_timer_BG_SA_PURPLE_GATE = 0;
    }
}
if (removal_timer_BG_SA_GREEN_GATE > 0)
{
    if (removal_timer_BG_SA_GREEN_GATE < 5000) {
        removal_timer_BG_SA_GREEN_GATE += diff;
    } else {
        //DelObject(BG_SA_GREEN_GATE);
        removal_timer_BG_SA_GREEN_GATE = 0;
    }
}
*/
//end removing gates after destruction
    if (InitSecondRound)
    {
        if (UpdateWaitTimer < diff)
        {
            if (!SignaledRoundTwo)
            {
                SignaledRoundTwo = true;
                InitSecondRound = false;
                SendMessageToAll(LANG_BG_SA_ROUND_TWO_ONE_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            }
        }
        else
        {
            UpdateWaitTimer -= diff;
            return;
        }
    }
    TotalTime += diff;

    if (Status == BG_SA_WARMUP)
    {
        EndRoundTimer = BG_SA_ROUNDLENGTH;
        if (TotalTime >= BG_SA_WARMUPLENGTH)
        {
            TotalTime = 0;
            ToggleTimer();
            DemolisherStartState(false);
            Status = BG_SA_ROUND_ONE;
            StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, (Attackers == TEAM_ALLIANCE) ? 23748 : 21702);
        }
        if (TotalTime >= BG_SA_BOAT_START)
            StartShips();
        return;
    }
    else if (Status == BG_SA_SECOND_WARMUP)
    {
        //SetStartDelayTime(GetStartDelayTime()-diff);
        if (RoundScores[0].time<BG_SA_ROUNDLENGTH)
            EndRoundTimer = RoundScores[0].time;
        else
            EndRoundTimer = BG_SA_ROUNDLENGTH;

        if (TotalTime >= 60000)
        {
            SendWarningToAll(LANG_BG_SA_HAS_BEGUN);
            TotalTime = 0;
            ToggleTimer();
            DemolisherStartState(false);
            Status = BG_SA_ROUND_TWO;
            StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, (Attackers == TEAM_ALLIANCE) ? 23748 : 21702);
            // status was set to STATUS_WAIT_JOIN manually for Preparation, set it back now
            SetStatus(STATUS_IN_PROGRESS);
            for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (auto p = ObjectAccessor::FindPlayer(itr->first))
                    p->RemoveAurasDueToSpell(SPELL_PREPARATION);
        }
        if (TotalTime >= 30000)
        {
            if (!SignaledRoundTwoHalfMin)
            {
                SignaledRoundTwoHalfMin = true;
                SendMessageToAll(LANG_BG_SA_ROUND_TWO_START_HALF_MINUTE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            }
        }
        startships_delayed_trigger = 1;
        return;
    }
    else if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (Status == BG_SA_ROUND_ONE)
        {
            if (TotalTime >= BG_SA_ROUNDLENGTH)
            {
                RoundScores[0].winner = Attackers;
                RoundScores[0].time = BG_SA_ROUNDLENGTH;
                TotalTime = 0;
                Status = BG_SA_SECOND_WARMUP;
                Attackers = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;
                UpdateWaitTimer = 15000;
                SignaledRoundTwo = false;
                SignaledRoundTwoHalfMin = false;
                InitSecondRound = true;
                ToggleTimer();
                ResetObjs();
                return;
            }
        }
        else if (Status == BG_SA_ROUND_TWO)
        {
            if (TotalTime >= EndRoundTimer)
            {
                RoundScores[1].time = BG_SA_ROUNDLENGTH;
                RoundScores[1].winner = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;
                if (RoundScores[0].time == RoundScores[1].time)
                    EndBattleground(0);
                else if (RoundScores[0].time < RoundScores[1].time)
                    EndBattleground(RoundScores[0].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                else
                    EndBattleground(RoundScores[1].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                return;
            }
        }
        if (Status == BG_SA_ROUND_ONE || Status == BG_SA_ROUND_TWO)
            UpdateDemolisherSpawns();
    }
    Gate_Structure_Anticheat();
}

void BattlegroundSA::StartingEventCloseDoors()
{
}

void BattlegroundSA::StartingEventOpenDoors()
{
}

void BattlegroundSA::FillInitialWorldStates(WorldPacket& data)
{
  uint32 ally_attacks = uint32(Attackers == TEAM_ALLIANCE ? 1 : 0);
  uint32 horde_attacks = uint32(Attackers == TEAM_HORDE ? 1 : 0);

  data << uint32(BG_SA_ANCIENT_GATEWS) << uint32(GateStatus[BG_SA_ANCIENT_GATE]);
  data << uint32(BG_SA_YELLOW_GATEWS) << uint32(GateStatus[BG_SA_YELLOW_GATE]);
  data << uint32(BG_SA_GREEN_GATEWS) << uint32(GateStatus[BG_SA_GREEN_GATE]);
  data << uint32(BG_SA_BLUE_GATEWS) << uint32(GateStatus[BG_SA_BLUE_GATE]);
  data << uint32(BG_SA_RED_GATEWS) << uint32(GateStatus[BG_SA_RED_GATE]);
  data << uint32(BG_SA_PURPLE_GATEWS) << uint32(GateStatus[BG_SA_PURPLE_GATE]);

  data << uint32(BG_SA_HORDE_ATTACKS) << horde_attacks;
  data << uint32(BG_SA_ALLY_ATTACKS) << ally_attacks;

  data << uint32(BG_SA_ENABLE_TIMER) << ((TimerEnabled) ? uint32(1) : uint32(0));
  data << uint32(BG_SA_TIMER_MINS) << ((TimerEnabled) ? uint32(time(NULL) + ((EndRoundTimer - TotalTime) / 1000)) : uint32(0));

  data << uint32(BG_SA_RIGHT_GY_HORDE) << uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
  data << uint32(BG_SA_LEFT_GY_HORDE) << uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);
  data << uint32(BG_SA_CENTER_GY_HORDE) << uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_HORDE ? 1 : 0);

  data << uint32(BG_SA_RIGHT_GY_ALLIANCE) << uint32(GraveyardStatus[BG_SA_RIGHT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
  data << uint32(BG_SA_LEFT_GY_ALLIANCE) << uint32(GraveyardStatus[BG_SA_LEFT_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);
  data << uint32(BG_SA_CENTER_GY_ALLIANCE) << uint32(GraveyardStatus[BG_SA_CENTRAL_CAPTURABLE_GY] == TEAM_ALLIANCE ? 1 : 0);

  data << uint32(BG_SA_HORDE_DEFENCE_TOKEN) << ally_attacks;
  data << uint32(BG_SA_ALLIANCE_DEFENCE_TOKEN) << horde_attacks;

  data << uint32(BG_SA_LEFT_ATT_TOKEN_HRD) << horde_attacks;
  data << uint32(BG_SA_RIGHT_ATT_TOKEN_HRD) << horde_attacks;
  data << uint32(BG_SA_RIGHT_ATT_TOKEN_ALL) <<  ally_attacks;
  data << uint32(BG_SA_LEFT_ATT_TOKEN_ALL) <<  ally_attacks;
}

void BattlegroundSA::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);
    //create score and add it to map, default values are set in constructor
    BattlegroundSAScore* sc = new BattlegroundSAScore;

    if (!ShipsStarted)
    {
        if (player->GetTeamId() == Attackers)
        {
            player->CastSpell(player, 12438, true);//Without this player falls before boat loads...

            if (urand(0, 1))
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                    player->TeleportTo(607, 2686.973877f, -825.393127f, 14.005457f, 2.554503f, 0); // //Blue Side Spawn Alliance
                else // if horde
                    player->TeleportTo(607, 2675.524170f, -829.219788f, 9.389289f, 3.476983f, 0); //Blue Side Spawn Horde
            }
            else
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                    player->TeleportTo(607, 2569.583984f, 984.663940f, 12.657445f, 3.925040f, 0); //Green Side Spawn Alliance
                else // if horde
                    player->TeleportTo(607, 2571.561768f, 973.249817f, 7.580028f, 4.726141f, 0); //Green Side Spawn Horde 
            }

        }
        else // If not attackers
            // player->TeleportTo(607, 1209.7f, -65.16f, 70.1f, 0.0f, 0);
            player->TeleportTo(607, 1227.190186f, -65.382912f, 70.084213f, 3.129815f, 0);
    }
    SendTransportInit(player);
    PlayerScores[player->GetGUID()] = sc;
    sc->BgTeam = player->GetTeam();
    sc->TalentTree = player->GetPrimaryTalentTree(player->GetActiveSpec());
}

void BattlegroundSA::RemovePlayer(Player* /*player*/, uint64 /*guid*/, uint32 /*team*/)
{
}

void BattlegroundSA::HandleAreaTrigger(Player* /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void BattlegroundSA::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    auto itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found...
        return;

    if (type == SCORE_DESTROYED_DEMOLISHER)
        ((BattlegroundSAScore*)itr->second)->demolishers_destroyed += value;
    else if (type == SCORE_DESTROYED_WALL)
        ((BattlegroundSAScore*)itr->second)->gates_destroyed += value;
    else
        Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
}

class DelayedTeleportEvent : public BasicEvent
{
public:
    DelayedTeleportEvent(uint64 _guid, bool _attacker) : victimGUID(_guid), attacker(_attacker) {}

    bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
    {
        if (auto player = ObjectAccessor::FindPlayer(victimGUID))
        {           
            if (player->GetMapId() != 607)
                return false;

            if (!player->GetBattleground())
                return false;

            if (attacker)
            {
                player->CastSpell(player, 12438, true); //Without this player falls before boat loads...

                if (urand(0, 1))
                {
                    if (player->GetTeamId() == TEAM_ALLIANCE)
                        player->TeleportTo(607, 2686.973877f, -825.393127f, 14.005457f, 2.554503f, 0); // //Blue Side Spawn Alliance
                    else // if horde
                        player->TeleportTo(607, 2675.524170f, -829.219788f, 9.389289f, 3.476983f, 0); //Blue Side Spawn Horde
                }
                else
                {
                    if (player->GetTeamId() == TEAM_ALLIANCE)
                        player->TeleportTo(607, 2569.583984f, 984.663940f, 12.657445f, 3.925040f, 0); //Green Side Spawn Alliance
                    else // if horde
                        player->TeleportTo(607, 2571.561768f, 973.249817f, 7.580028f, 4.726141f, 0); //Green Side Spawn Horde 
                }

            }
            else
                player->TeleportTo(607, 1227.190186f, -65.382912f, 70.084213f, 3.129815f, 0); // If not attackers
        }
        return true;
    }

private:
    uint64 victimGUID;
    bool attacker;
};

void BattlegroundSA::TeleportPlayers()
{
    for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        if (auto player = ObjectAccessor::FindPlayer(itr->first))
        {
            // should remove spirit of redemption
            if (player->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
                player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

            if (!player->isAlive())
            {
                player->ResurrectPlayer(1.0f);
                player->SpawnCorpseBones();
            }
            player->ResetAllPowers();
            player->CombatStopWithPets(true);
            player->m_Events.AddEvent(new DelayedTeleportEvent(player->GetGUID(), player->GetTeamId() == Attackers), player->m_Events.CalculateTime(5000));
        }
    }
}

void BattlegroundSA::Gate_Structure_Anticheat() {
    for (uint32 i = BG_SA_GREEN_GATE; i <= BG_SA_ANCIENT_GATE; i++)
    {
        switch (i) {
        case BG_SA_RED_GATE:
        case BG_SA_PURPLE_GATE:
            if (
                (GateStatus[BG_SA_BLUE_GATE] != BG_SA_GATE_DESTROYED)
                &&
                (GateStatus[BG_SA_GREEN_GATE] != BG_SA_GATE_DESTROYED)
                ) {
                GetBGObject(i)->SetDestructibleState(GO_DESTRUCTIBLE_INTACT, NULL, true);
            }
            break;

        case BG_SA_YELLOW_GATE:
            if (
                (GateStatus[BG_SA_RED_GATE] != BG_SA_GATE_DESTROYED)
                &&
                (GateStatus[BG_SA_PURPLE_GATE] != BG_SA_GATE_DESTROYED)
                ) {
                GetBGObject(i)->SetDestructibleState(GO_DESTRUCTIBLE_INTACT, NULL, true);
            }
            break;
        case BG_SA_ANCIENT_GATE:
            if (
                (GateStatus[BG_SA_YELLOW_GATE] != BG_SA_GATE_DESTROYED)
                ) {
                GetBGObject(i)->SetDestructibleState(GO_DESTRUCTIBLE_INTACT, NULL, true);
            }
        default:
            break;
        }
    }
}

void BattlegroundSA::EventPlayerDamagedGO(Player* player, GameObject* go, uint32 eventType)
{
    if (!go || !go->GetGOInfo())
        return;
    uint32 i = getGateIdFromDamagedOrDestroyEventId(eventType);
    if (eventType == go->GetGOInfo()->building.damagedEvent)
    {
       
        GateStatus[i] = BG_SA_GATE_DAMAGED;
        uint32 uws = getWorldStateFromGateId(i);
        if (uws)
            UpdateWorldState(uws, GateStatus[i]);
    }

    if (eventType == go->GetGOInfo()->building.destroyedEvent)
    {
        if (go->GetGOInfo()->building.destroyedEvent == BG_SA_EVENT_ANCIENT_GATE_DESTROYED)
        {
            SendWarningToAll(LANG_BG_SA_CHAMBER_BREACHED);
        }
        else
        {       
            SendWarningToAll(LANG_BG_SA_WAS_DESTROYED, go->GetGOInfo()->name.c_str());
    
            //When a gate is destroyed, update teleporter location destinations.
            /*
    { 1453.49f, -250.453f, 30.896f, 4.2883f}, //Blue Teleport = 10
    { 1377.05f, 97.036f, 30.8605f, 2.46539f}, //Green Teleport = 11
    { 1186.05f, 58.8048f, 56.5491f, 2.75992f}, //Purple Teleport = 12
    { 1042.83f, -72.839f, 84.8145f, 3.58615f}, //Gold Teleport = 13
    { 1233.62f, -250.49f, 55.4036f, 3.7016f}, //Red Teleport = 14
            */
            switch (i) {
            case BG_SA_BLUE_GATE:
                removal_timer_BG_SA_BLUE_GATE = 1;
                GetBGCreature(BG_SA_NPC_TRIGGER_BLUE)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                GetBGCreature(BG_SA_NPC_TRIGGER_GREEN)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                break;
            case BG_SA_GREEN_GATE:
                removal_timer_BG_SA_GREEN_GATE = 1;
                GetBGCreature(BG_SA_NPC_TRIGGER_BLUE)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                GetBGCreature(BG_SA_NPC_TRIGGER_GREEN)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                break;
            case BG_SA_RED_GATE:
                removal_timer_BG_SA_RED_GATE = 1;
                GetBGCreature(BG_SA_NPC_TRIGGER_RED)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                GetBGCreature(BG_SA_NPC_TRIGGER_PURPLE)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                break;
            case BG_SA_PURPLE_GATE:
                removal_timer_BG_SA_PURPLE_GATE = 1;
                GetBGCreature(BG_SA_NPC_TRIGGER_RED)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                GetBGCreature(BG_SA_NPC_TRIGGER_PURPLE)->Relocate(BG_SA_NpcSpawnlocs[13][0] - 50, BG_SA_NpcSpawnlocs[13][1], BG_SA_NpcSpawnlocs[13][2]);
                break;
            case BG_SA_YELLOW_GATE:
                removal_timer_BG_SA_YELLOW_GATE = 1;
                break;

            default:
                break;
            }
        }
    }
    else {

        // if (eventType == go->GetGOInfo()->building.damageEvent)
        SendWarningToAll(LANG_BG_SA_IS_UNDER_ATTACK, go->GetGOInfo()->name.c_str());
    }
}

void BattlegroundSA::HandleKillUnit(Creature* creature, Player* killer)
{
    if (creature->GetEntry() == NPC_DEMOLISHER_SA)
    {
        UpdatePlayerScore(killer, SCORE_DESTROYED_DEMOLISHER, 1);
        _notEvenAScratch[Attackers] = false;
    }
}

/*
  You may ask what the fuck does it do?
  Prevents owner overwriting guns faction with own.
 */

void BattlegroundSA::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (killer)
        if (player)
            Battleground::HandleKillPlayer(player, killer);
        else TC_LOG_ERROR("bg.battleground", "BattlegroundSA::HandleKillPlayer: Killer player not found");
    else TC_LOG_ERROR("bg.battleground", "BattlegroundSA::HandleKillPlayer: Victim player not found");

    if (killer->GetGUID() != player->GetGUID())
    {
        if (player->GetTeamId() == TEAM_ALLIANCE || player->GetTeamId() == TEAM_HORDE)
        {
            player->DestroyItemCount(39213, 1, true);
        }
    }

}

void BattlegroundSA::OverrideGunFaction()
{
    if (!BgCreatures[0])
        return;

    for (uint8 i = BG_SA_GUN_1; i <= BG_SA_GUN_10; i++)
    {
        if (auto gun = GetBGCreature(i))
            gun->setFaction(BG_SA_Factions[Attackers ? TEAM_ALLIANCE : TEAM_HORDE]);
    }

    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_4; i++)
    {
        if (auto dem = GetBGCreature(i))
            dem->setFaction(BG_SA_Factions[Attackers]);
    }
}

void BattlegroundSA::DemolisherStartState(bool start)
{
    if (!BgCreatures[0])
        return;

    // set flags only for the demolishers on the beach, factory ones dont need it
    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_4; i++)
    {
        if (auto dem = GetBGCreature(i))
        {
            if (start)
                dem->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            else
                dem->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }
    }
}

void BattlegroundSA::DestroyGate(Player* player, GameObject* go)
{
    uint32 i = getGateIdFromDamagedOrDestroyEventId(go->GetGOInfo()->building.destroyedEvent);
    if (!GateStatus[i])
        return;

    if (auto g = GetBGObject(i))
    {
        if (g->GetGOValue()->Building.Health == 0)
        {
            GateStatus[i] = BG_SA_GATE_DESTROYED;
            UpdateObjectInteractionFlags();
            uint32 uws = getWorldStateFromGateId(i);
            if (uws)
                UpdateWorldState(uws, GateStatus[i]);

            bool rewardHonor = true;
            gateDestroyed = true;

            switch (i)
            {
                case BG_SA_GREEN_GATE:
                    if (GateStatus[BG_SA_BLUE_GATE] == BG_SA_GATE_DESTROYED) {
                        rewardHonor = false;
                    }
                    else
                        destroyedGates++;
                    break;
                case BG_SA_BLUE_GATE:
                    if (GateStatus[BG_SA_GREEN_GATE] == BG_SA_GATE_DESTROYED) {
                        rewardHonor = false;
                    }
                    else 
                        destroyedGates++;
                    break;
                case BG_SA_RED_GATE:
                    if (GateStatus[BG_SA_PURPLE_GATE] == BG_SA_GATE_DESTROYED) {
                        rewardHonor = false;
                    }
                    else
                        destroyedGates++;
                    break;
                case BG_SA_PURPLE_GATE:
                    if (GateStatus[BG_SA_RED_GATE] == BG_SA_GATE_DESTROYED) {
                        rewardHonor = false;
                    }
                    else
                        destroyedGates++;
                    break;
                case BG_SA_YELLOW_GATE:
                    destroyedGates++;
                    break;
                default:
                    break;
            }

            if (i < 5)
                DelObject(i + 14);
            UpdatePlayerScore(player, SCORE_DESTROYED_WALL, 1);
            if (rewardHonor)
                UpdatePlayerScore(player, SCORE_BONUS_HONOR, GetBonusHonorFromKill(1));

            /*            if (destroyedGates == 1 && CanInteractWithObject(BG_SA_BOMB_SECOND_STAGE) ||
                destroyedGates == 2 && CanInteractWithObject(BG_SA_BOMB_THIRD_STAGE) ||
                destroyedGates == 3 && CanInteractWithObject(BG_SA_BOMB_LAST_STAGE))*/
            if (CanInteractWithObject(BG_SA_BOMB_SECOND_STAGE) ||
                CanInteractWithObject(BG_SA_BOMB_THIRD_STAGE) ||
                CanInteractWithObject(BG_SA_BOMB_LAST_STAGE))
            {
                for (uint8 i = BG_SA_BOMB_FIRST_STAGE; i < BG_SA_MAXOBJ; i++)
                {
                    //this is the rule for spawning bombs. wtf. okay.
                    //So this is spawning bombs with an x value of less than 1000(lower x = further south) only when 5 gates have been destroyed.
                    if (CanInteractWithObject(BG_SA_BOMB_SECOND_STAGE) && BG_SA_ObjSpawnlocs[i][0] < 1400.f && BG_SA_ObjSpawnlocs[i][0] > 1200.f ||
                        CanInteractWithObject(BG_SA_BOMB_THIRD_STAGE) && BG_SA_ObjSpawnlocs[i][0] < 1200.f && BG_SA_ObjSpawnlocs[i][0] > 1000.f ||
                        CanInteractWithObject(BG_SA_BOMB_LAST_STAGE) && BG_SA_ObjSpawnlocs[i][0] < 1000.f)
                    {
                        AddObject(i, BG_SA_ObjEntries[BG_SA_BOMB_FIRST_STAGE],
                                  BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1],
                                  BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3],
                                  0, 0, 0, 0, RESPAWN_ONE_DAY);
                        GetBGObject(i)->SetUInt32Value(GAMEOBJECT_FACTION, BG_SA_Factions[Attackers]);
                    }
                }
            }
        }
    }
}

WorldSafeLocsEntry const* BattlegroundSA::GetClosestGraveYard(Player* player)
{
    uint32 safeloc = 0;
    WorldSafeLocsEntry const* ret;
    WorldSafeLocsEntry const* closest;
    float dist, nearest;
    float x, y, z;

    player->GetPosition(x, y, z);

    if (player->GetTeamId() == Attackers)
        safeloc = BG_SA_GYEntries[BG_SA_BEACH_GY];
    else
        safeloc = BG_SA_GYEntries[BG_SA_DEFENDER_LAST_GY];

    closest = sWorldSafeLocsStore.LookupEntry(safeloc);
    nearest = sqrt((closest->x - x)*(closest->x - x) + (closest->y - y)*(closest->y - y) + (closest->z - z)*(closest->z - z));

    for (uint8 i = BG_SA_RIGHT_CAPTURABLE_GY; i < BG_SA_MAX_GY; i++)
    {
        if (GraveyardStatus[i] != player->GetTeamId())
            continue;

        ret = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
        dist = sqrt((ret->x - x)*(ret->x - x) + (ret->y - y)*(ret->y - y) + (ret->z - z)*(ret->z - z));
        if (dist < nearest)
        {
            closest = ret;
            nearest = dist;
        }
    }

    return closest;
}

void BattlegroundSA::EventPlayerClickedOnFlag(Player* Source, GameObject* target_obj)
{
    switch (target_obj->GetEntry())
    {
        case 191307:
        case 191308:
            if (CanInteractWithObject(BG_SA_LEFT_FLAG)) {
                CaptureGraveyard(BG_SA_LEFT_CAPTURABLE_GY, Source);

            }
            break;
        case 191305:
        case 191306:
            if (CanInteractWithObject(BG_SA_RIGHT_FLAG)) {
                CaptureGraveyard(BG_SA_RIGHT_CAPTURABLE_GY, Source);

            }
            break;
        case 191310:
        case 191309:
            if (CanInteractWithObject(BG_SA_CENTRAL_FLAG)) {
                CaptureGraveyard(BG_SA_CENTRAL_CAPTURABLE_GY, Source);

            }
            break;
        default:
            return;
    };
}

void BattlegroundSA::CaptureGraveyard(BG_SA_Graveyards i, Player* Source)
{
    if (GraveyardStatus[i] == Attackers)
        return;

    DelCreature(BG_SA_MAXNPC + i);
    GraveyardStatus[i] = Source->GetTeamId();
    WorldSafeLocsEntry const* sg = sWorldSafeLocsStore.LookupEntry(BG_SA_GYEntries[i]);
    if (!sg)
    {
        TC_LOG_ERROR("bg.battleground", "BattlegroundSA::CaptureGraveyard: non-existant GY entry: %u", BG_SA_GYEntries[i]);
        return;
    }

    AddSpiritGuide(i + BG_SA_MAXNPC, sg->x, sg->y, sg->z, BG_SA_GYOrientation[i], (GraveyardStatus[i] == TEAM_ALLIANCE?  ALLIANCE : HORDE));
    uint32 npc = 0;
    uint32 flag = 0;

    switch (i)
    {
        case BG_SA_LEFT_CAPTURABLE_GY:
            flag = BG_SA_LEFT_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetTeamId() == TEAM_ALLIANCE ? 0 : 1)),
            BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
            BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(flag)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            npc = BG_SA_NPC_RIGSPARK;
            AddCreature(BG_SA_NpcEntries[npc], npc, Attackers,
              BG_SA_NpcSpawnlocs[npc][0], BG_SA_NpcSpawnlocs[npc][1],
              BG_SA_NpcSpawnlocs[npc][2], BG_SA_NpcSpawnlocs[npc][3]);

            Rigspark_Yell_status = 1;
            for (uint8 j = BG_SA_DEMOLISHER_7; j <= BG_SA_DEMOLISHER_8; j++)
            {
                AddCreature(BG_SA_NpcEntries[j], j, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
                    BG_SA_NpcSpawnlocs[j][0], BG_SA_NpcSpawnlocs[j][1],
                    BG_SA_NpcSpawnlocs[j][2], BG_SA_NpcSpawnlocs[j][3], 600);

                if (auto dem = GetBGCreature(j))
                    dem->setFaction(BG_SA_Factions[Attackers]);
            }

            UpdateWorldState(BG_SA_LEFT_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_LEFT_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_WEST);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_WEST);

            break;
        case BG_SA_RIGHT_CAPTURABLE_GY:
            flag = BG_SA_RIGHT_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetTeamId() == TEAM_ALLIANCE ? 0 : 1)),
              BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
              BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(flag)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            npc = BG_SA_NPC_SPARKLIGHT;
            AddCreature(BG_SA_NpcEntries[npc], npc, Attackers,
              BG_SA_NpcSpawnlocs[npc][0], BG_SA_NpcSpawnlocs[npc][1],
              BG_SA_NpcSpawnlocs[npc][2], BG_SA_NpcSpawnlocs[npc][3]);

            Sparklight_Yell_status = 1;
            for (uint8 j = BG_SA_DEMOLISHER_5; j <= BG_SA_DEMOLISHER_6; j++)
            {
                AddCreature(BG_SA_NpcEntries[j], j, (Attackers == TEAM_ALLIANCE ? TEAM_HORDE : TEAM_ALLIANCE),
                    BG_SA_NpcSpawnlocs[j][0], BG_SA_NpcSpawnlocs[j][1],
                    BG_SA_NpcSpawnlocs[j][2], BG_SA_NpcSpawnlocs[j][3], 600);

                if (auto dem = GetBGCreature(j))
                    dem->setFaction(BG_SA_Factions[Attackers]);
            }

            UpdateWorldState(BG_SA_RIGHT_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_RIGHT_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_EAST);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_EAST);
            break;
        case BG_SA_CENTRAL_CAPTURABLE_GY:
            flag = BG_SA_CENTRAL_FLAG;
            DelObject(flag);
            AddObject(flag, (BG_SA_ObjEntries[flag] - (Source->GetTeamId() == TEAM_ALLIANCE ? 0 : 1)),
              BG_SA_ObjSpawnlocs[flag][0], BG_SA_ObjSpawnlocs[flag][1],
              BG_SA_ObjSpawnlocs[flag][2], BG_SA_ObjSpawnlocs[flag][3], 0, 0, 0, 0, RESPAWN_ONE_DAY);
            GetBGObject(flag)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            UpdateWorldState(BG_SA_CENTER_GY_ALLIANCE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 1 : 0));
            UpdateWorldState(BG_SA_CENTER_GY_HORDE, (GraveyardStatus[i] == TEAM_ALLIANCE ? 0 : 1));
            if (Source->GetTeamId() == TEAM_ALLIANCE)
                SendWarningToAll(LANG_BG_SA_A_GY_SOUTH);
            else
                SendWarningToAll(LANG_BG_SA_H_GY_SOUTH);
            break;
        default:
            ASSERT(false);
            break;
    };
    PlaySoundToAll(Source->GetTeam() == ALLIANCE ? BG_SA_SOUND_ALLIANCE_CAPTURE : BG_SA_SOUND_HORDE_CAPTURE);
}


void BattlegroundSA::EventPlayerUsedGO(Player* Source, GameObject* object)
{
    if (Source->GetTeamId() == Attackers)
    {
        if (CanInteractWithObject(BG_SA_TITAN_RELIC))
        {
            if (Source->GetTeamId() == TEAM_ALLIANCE)
                SendMessageToAll(LANG_BG_SA_ALLIANCE_CAPTURED_RELIC, CHAT_MSG_BG_SYSTEM_NEUTRAL);
            else SendMessageToAll(LANG_BG_SA_HORDE_CAPTURED_RELIC, CHAT_MSG_BG_SYSTEM_NEUTRAL);

            if (Status == BG_SA_ROUND_ONE)
            {
                Sparklight_Yell_status = 0;
                Sparklight_Yell_counter = 0;
                Rigspark_Yell_status = 0;
                Rigspark_Yell_counter = 0;
                RoundScores[0].winner = Attackers;
                RoundScores[0].time = TotalTime;
                //Achievement Storm the Beach (1310)
                for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                {
                    if (auto player = ObjectAccessor::FindPlayer(itr->first)) {
                        player->CastCustomSpell(52459/*Round completed debuff*/, SPELLVALUE_AURA_STACK, 0, player);
                        if (player->GetTeamId() == Attackers)
                            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 65246);
                    }

                }
                Attackers = (Attackers == TEAM_ALLIANCE) ? TEAM_HORDE : TEAM_ALLIANCE;               
                TotalTime = 0;
                Status = BG_SA_SECOND_WARMUP;
                ToggleTimer();
                SendWarningToAll(LANG_BG_SA_ROUND_ONE_END);
                UpdateWaitTimer = 15000;
                SignaledRoundTwo = false;
                SignaledRoundTwoHalfMin = false;
                InitSecondRound = true;
                ResetObjs();
            }
            else if (Status == BG_SA_ROUND_TWO)
            {
                RoundScores[1].winner = Attackers;
                RoundScores[1].time = TotalTime;
                ToggleTimer();
                //Achievement Storm the Beach (1310)
                for (auto itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                {
                    if (auto player = ObjectAccessor::FindPlayer(itr->first))
                        if (player->GetTeamId() == Attackers && RoundScores[1].winner == Attackers)
                            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 65246);
                }

                if (RoundScores[0].time == RoundScores[1].time)
                    EndBattleground(0);
                else if (RoundScores[0].time < RoundScores[1].time)
                    EndBattleground(RoundScores[0].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
                else
                    EndBattleground(RoundScores[1].winner == TEAM_ALLIANCE ? ALLIANCE : HORDE);
            }
        }
        else
        {
            Source->Kill(Source, 0);
        }
    }
}

void BattlegroundSA::ToggleTimer()
{
    TimerEnabled = !TimerEnabled;
    UpdateWorldState(BG_SA_ENABLE_TIMER, (TimerEnabled) ? 1 : 0);
    UpdateWorldState(BG_SA_TIMER_MINS, ((TimerEnabled) ? uint32(time(NULL) + ((EndRoundTimer - TotalTime) / 1000)) : uint32(0)));
}

void BattlegroundSA::EndBattleground(uint32 winner)
{
    //honor reward for winning
   
    DistributeHonorToAll(winner);
    Battleground::EndBattleground(winner);
}

void BattlegroundSA::UpdateDemolisherSpawns()
{
    for (uint8 i = BG_SA_DEMOLISHER_1; i <= BG_SA_DEMOLISHER_8; i++)
        if (BgCreatures[i])
            if (auto Demolisher = GetBGCreature(i))
                if (Demolisher->isDead())
                    if (DemoliserRespawnList.find(i) == DemoliserRespawnList.end())
                        DemoliserRespawnList[i] = getMSTime()+30000;
                    else
                    {
                        if (DemoliserRespawnList[i] < getMSTime())
                        {
                            Demolisher->Relocate(BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1],
                            BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3]);
                            Demolisher->Respawn();
                            DemoliserRespawnList.erase(i);
                        }
                    }
    
}

void BattlegroundSA::SendTransportInit(Player* player)
{
    if (BgObjects[BG_SA_BOAT_ONE] ||  BgObjects[BG_SA_BOAT_TWO])
    {
        UpdateData transData(player->GetMapId());
        if (BgObjects[BG_SA_BOAT_ONE])

        if (auto ship1 = GetBGObject(BG_SA_BOAT_ONE))
            ship1->BuildCreateUpdateBlockForPlayer(&transData, player);

        if (auto ship2 = GetBGObject(BG_SA_BOAT_TWO))
            ship2->BuildCreateUpdateBlockForPlayer(&transData, player);

        WorldPacket packet;
        transData.BuildPacket(&packet);
        player->GetSession()->SendPacket(&packet);
    }
}

void BattlegroundSA::SendTransportUpdate(Player* player)
{
        UpdateData transData(player->GetMapId());

        if (auto ship1 = GetBGObject(BG_SA_BOAT_ONE))
            ship1->BuildValuesUpdateBlockForPlayer(&transData, player);
        
        if (auto ship2 = GetBGObject(BG_SA_BOAT_TWO))
            ship2->BuildValuesUpdateBlockForPlayer(&transData, player);

        if (BgObjects[BG_SA_BOAT_ONE] || BgObjects[BG_SA_BOAT_TWO])
        {
            WorldPacket packet;
            transData.BuildPacket(&packet);
            if (auto s = player->GetSession())
                s->SendPacket(&packet);
        }
}

void BattlegroundSA::SendTransportsRemove(Player* player)
{
    if (BgObjects[BG_SA_BOAT_ONE] ||  BgObjects[BG_SA_BOAT_TWO])
    {
        UpdateData transData(player->GetMapId());
        if (BgObjects[BG_SA_BOAT_ONE])
            GetBGObject(BG_SA_BOAT_ONE)->BuildOutOfRangeUpdateBlock(&transData);
        if (BgObjects[BG_SA_BOAT_TWO])
            GetBGObject(BG_SA_BOAT_TWO)->BuildOutOfRangeUpdateBlock(&transData);
        WorldPacket packet;
        transData.BuildPacket(&packet);
        player->GetSession()->SendPacket(&packet);
    }
}

bool BattlegroundSA::CanInteractWithObject(uint32 objectId)
{
    switch (objectId)
    {
        case BG_SA_TITAN_RELIC:
            if (GateStatus[BG_SA_ANCIENT_GATE] != BG_SA_GATE_DESTROYED || GateStatus[BG_SA_YELLOW_GATE] != BG_SA_GATE_DESTROYED)
                return false;
            // no break
        case BG_SA_BOMB_LAST_STAGE:
            if (GateStatus[BG_SA_YELLOW_GATE] != BG_SA_GATE_DESTROYED)
                return false;
            // no break
        case BG_SA_CENTRAL_FLAG:
        case BG_SA_BOMB_THIRD_STAGE:
            if (GateStatus[BG_SA_RED_GATE] != BG_SA_GATE_DESTROYED && GateStatus[BG_SA_PURPLE_GATE] != BG_SA_GATE_DESTROYED)
                return false;
            // no break
        case BG_SA_LEFT_FLAG:
        case BG_SA_RIGHT_FLAG:
        case BG_SA_BOMB_SECOND_STAGE:
            if (GateStatus[BG_SA_GREEN_GATE] != BG_SA_GATE_DESTROYED && GateStatus[BG_SA_BLUE_GATE] != BG_SA_GATE_DESTROYED)
                return false;
        case BG_SA_BOMB_FIRST_STAGE:
            if (Status == BG_SA_SECOND_WARMUP || Status == BG_SA_WARMUP)
                return false;
            break;
        default:
            ASSERT(false);
            break;
    }

    return true;
}

void BattlegroundSA::UpdateObjectInteractionFlags(uint32 objectId)
{
    if (auto go = GetBGObject(objectId))
    {
        if (CanInteractWithObject(objectId))
            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        else
        {
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

            if (objectId == BG_SA_TITAN_RELIC)
            {
                GameObject* ancientGate = GetBGObject(BG_SA_ANCIENT_GATE);
                GameObject* yellowGate = GetBGObject(BG_SA_YELLOW_GATE); //fuck this ok testing 

                /*
                    if (ancientGate && yellowGate)
                    TC_LOG_ERROR("bg.battleground.sota", "BattlegroundSA::UpdateObjectInteractionFlags: Titan Relic got GO_FLAG_NOT_SELECTABLE: Ancient Gate state: %u, Yellow Gate state: %u", ancientGate->GetDestructibleState(), yellowGate->GetDestructibleState());
            
                */
                }
        }
    }
}

void BattlegroundSA::UpdateObjectInteractionFlags()
{
    for (uint8 i = BG_SA_CENTRAL_FLAG; i <= BG_SA_LEFT_FLAG; ++i)
        UpdateObjectInteractionFlags(i);
    UpdateObjectInteractionFlags(BG_SA_TITAN_RELIC);
}