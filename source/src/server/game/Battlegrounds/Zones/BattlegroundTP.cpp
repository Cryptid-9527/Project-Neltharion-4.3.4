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

#include "Battleground.h"
#include "BattlegroundTP.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "Chat.h" //REMOVE ONCE FINISHED DEVELOPING

BattlegroundTP::BattlegroundTP()
{
    /// Resize BG miscellaneous
    BgObjects.resize(BG_TP_OBJECT_MAX);
    BgCreatures.resize(BG_CREATURES_MAX_TP);

    /// Start Messages Initialization
    StartMessageIds[BG_STARTING_EVENT_FIRST] = LANG_BG_TP_START_TWO_MINUTES;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_TP_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD] = LANG_BG_TP_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_TP_HAS_BEGUN;

    /// Misc Stuff - See declaration for more info
    _minutesElapsed = 0;
    _bothFlagsKept = false;
    //ASSAULT_FULL_INITIALIZE HERE
    setup_ASSAULT_VALUES();
    _flagSpellForceTimer = 0;
}

BattlegroundTP::~BattlegroundTP()
{
}

void BattlegroundTP::PostUpdateImpl(uint32 diff) //Tick tock this is the clock
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        /// Total time it's supposed to be 25 (game) + 2 (when the battle begins and doors are closed) minutes
        if (GetElapsedTime() >= 27 * MINUTE * IN_MILLISECONDS) ///< End of game - Verify score
        {
            if (GetTeamScore(TEAM_ALLIANCE) == 0)
            {
                if (GetTeamScore(TEAM_HORDE) == 0) ///< No one scored
                    EndBattleground(WINNER_NONE);
                else ///< Horde has more points (Alliance has 0) - Horde Wins
                    EndBattleground(HORDE);
            }
            else if (GetTeamScore(TEAM_HORDE) == 0) ///< Alliance has more than 0, Horde has 0 so Alliance Wins
                EndBattleground(ALLIANCE);
            else if (GetTeamScore(TEAM_HORDE) == GetTeamScore(TEAM_ALLIANCE)) ///< Team Score equal and both scored at least once so winer is the last team witch captured the flag
                EndBattleground(_lastFlagCaptureTeam);
            else if (GetTeamScore(TEAM_HORDE) > GetTeamScore(TEAM_ALLIANCE)) // Last but not least, check who has the higher score
                EndBattleground(HORDE);
            else
                EndBattleground(ALLIANCE);
        }
        // First Update is needed only after 1 minute in battle
        else if (GetElapsedTime() > uint32((_minutesElapsed + 1) * MINUTE * IN_MILLISECONDS) + 2 * MINUTE * IN_MILLISECONDS)
        {
            ++_minutesElapsed;
            UpdateWorldState(BG_TP_STATE_TIMER, 25 - _minutesElapsed); //< Time remaining showed on top of the screen via world state
        }

        /// Flags state update:
        for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
        {
            switch (_flagState[team])
            {
            case BG_TP_FLAG_STATE_WAIT_RESPAWN: ///< Flag is nowhere - Is in this situation if a team captured the flag
            {
                _flagsTimer -= diff; ///< Update Flag Timer with the time between the last server update and now
                if (_flagsTimer <= 0)
                {
                    _flagsTimer = 0;
                    RespawnFlag(team, 3);
                    UpdateWorldState(BG_TP_FLAG_UNKNOWN, 0); ///< From sniffs
                }
                break;
            }
            case BG_TP_FLAG_STATE_ON_GROUND: ///< Flag is on the ground waiting someone to click
            {
                _flagsDropTimer[team] -= diff; ///< Update Flag Timer with the time between the last server update and now
                if (_flagsDropTimer[team] < 0)
                {
                    _flagsDropTimer[team] = 0;

                    /// Respawn flag
                    RespawnFlag(team, 2);

                    /// Delete flag from ground
                    if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team]))
                        obj->Delete();
                    else
                        TC_LOG_ERROR("bg.battleground", "BattlegroundTP: An error has occurred in PostUpdateImpl: Unknown dropped flag GUID: %u", GUID_LOPART(_droppedFlagGUID[team]));

                    _droppedFlagGUID[team] = 0;
                }
                break;
            }

            }
        }
        if (_bothFlagsKept)
        {
            //insert brutal assault ticking
            clock_tick_ASSAULT_MANAGEMENT();
            //insert brutal assault ticking
        }
        //door removal mechanic to open up LOS at doorways
        if (tp_door_removal_timer > 0) {
            if (tp_door_removal_timer < 15000) {
                tp_door_removal_timer += diff;
            }
            else {
                for (uint32 i = BG_TP_OBJECT_DOOR_A_1; i <= BG_TP_OBJECT_DOOR_A_4; ++i)
                    DelObject(i);
                for (uint32 i = BG_TP_OBJECT_DOOR_H_1; i <= BG_TP_OBJECT_DOOR_H_4; ++i)
                    DelObject(i);
                tp_door_removal_timer = 0;
            }
        }
        //end door removal
        //waterfall killzone mechanic
        for (BattlegroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr) {
            if (auto p = ObjectAccessor::FindPlayer(itr->first)) {
                if (p->GetPositionZ() < -95) {
                    if (!p->isDead()) {
                        p->Kill(p, 0);
                    }
                }
            }
        }
        //end waterfall kill zone mechanic
        reset_flag_in_GY(); //Check and reset flags if they've gotten too close to the graveyard.

        /*begin PickedUpTooSoon counter*/
        if (pickedUpTooSoon_counter > 0) {
            if (pickedUpTooSoon_counter < 20000) {
                pickedUpTooSoon_counter += diff;
            }
            else {
                pickedUpTooSoon_counter = 0;
            }
        }
        /*End PickedUpTooSoon counter*/
        /*begin time since flag has been pulled counter tick*/
        for (uint32 team = 0; team <= 1; ++team) {
            if (ANTICHEAT_time_since_pull[team] > 0) {
                if (ANTICHEAT_time_since_pull[team] < 20000) {
                    ANTICHEAT_time_since_pull[team] += diff;
                }
                else {
                    ANTICHEAT_time_since_pull[team] = 0;
                }
            }
        }
        /*End time since flag has been pulled counter tick*/
    }
}

void BattlegroundTP::AddPlayer(Player* player)
{
    Battleground::AddPlayer(player);

    ///Create score for player
    BattlegroundTPScore* sc = new BattlegroundTPScore;
    PlayerScores[player->GetGUID()] = sc;
    sc->BgTeam = player->GetTeam();
    sc->TalentTree = player->GetPrimaryTalentTree(player->GetActiveSpec());
}


void BattlegroundTP::StartingEventCloseDoors()
{
    for (uint32 i = BG_TP_OBJECT_DOOR_A_1; i <= BG_TP_OBJECT_DOOR_H_4; ++i)
    {
        DoorClose(i);
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);
    }
    for (uint32 i = BG_TP_OBJECT_A_FLAG; i <= BG_TP_OBJECT_BERSERKBUFF_2; ++i)
        SpawnBGObject(i, RESPAWN_ONE_DAY);
}

void BattlegroundTP::StartingEventOpenDoors()
{
    for (uint32 i = BG_TP_OBJECT_DOOR_A_1; i <= BG_TP_OBJECT_DOOR_A_4; ++i)
        DoorOpen(i);
    for (uint32 i = BG_TP_OBJECT_DOOR_H_1; i <= BG_TP_OBJECT_DOOR_H_4; ++i)
        DoorOpen(i);

    for (uint32 i = BG_TP_OBJECT_A_FLAG; i <= BG_TP_OBJECT_BERSERKBUFF_2; ++i)
        SpawnBGObject(i, RESPAWN_IMMEDIATELY);

    // players joining later are not eligibles
    StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, TP_EVENT_START_BATTLE);

    UpdateWorldState(BG_TP_STATE_TIMER_ACTIVE, 1);
    UpdateWorldState(BG_TP_STATE_TIMER, 25);
    tp_door_removal_timer = 1;
    pickedUpTooSoon_counter = 1;
    /*DEV PURPOSES DO NOT APPLY*/
    //SetRatedBattleground(true);
    /*DEV PURPOSES DO NOT APPLY*/
}

bool BattlegroundTP::SetupBattleground()
{
    // To Do: Move this to DB!
    if (!AddObject(BG_TP_OBJECT_A_FLAG, BG_OBJECT_A_FLAG_TP_ENTRY, 2118.210f, 191.621f, 44.052f, 5.741259f, 0, 0, 0.9996573f, 0.02617699f, BG_TP_FLAG_RESPAWN_TIME / 1000)
        || !AddObject(BG_TP_OBJECT_H_FLAG, BG_OBJECT_H_FLAG_TP_ENTRY, 1578.380f, 344.037f, 2.419f, 3.055978f, 0, 0, 0.008726535f, 0.9999619f, BG_TP_FLAG_RESPAWN_TIME / 1000)
        /// Buffs
        || !AddObject(BG_TP_OBJECT_SPEEDBUFF_1, BG_OBJECTID_SPEEDBUFF_ENTRY, 1545.402f, 304.028f, 0.5923f, 6.1732f, 0, 0, 0.7313537f, -0.6819983f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_TP_OBJECT_SPEEDBUFF_2, BG_OBJECTID_SPEEDBUFF_ENTRY, 2171.279f, 222.334f, 43.8001f, 2.663309f, 0, 0, 0.7313537f, 0.6819984f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_TP_OBJECT_REGENBUFF_1, BG_OBJECTID_REGENBUFF_ENTRY, 1753.957f, 242.092f, -14.1170f, 1.105848f, 0, 0, 0.1305263f, -0.9914448f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_TP_OBJECT_REGENBUFF_2, BG_OBJECTID_REGENBUFF_ENTRY, 1952.121f, 383.857f, -10.2870f, 4.192612f, 0, 0, 0.333807f, -0.9426414f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_TP_OBJECT_BERSERKBUFF_1, BG_OBJECTID_BERSERKERBUFF_ENTRY, 1934.369f, 226.064f, -17.0441f, 2.499154f, 0, 0, 0.5591929f, 0.8290376f, BUFF_RESPAWN_TIME)
        || !AddObject(BG_TP_OBJECT_BERSERKBUFF_2, BG_OBJECTID_BERSERKERBUFF_ENTRY, 1736.290f, 436.383f, -8.0848f, 5.709677f, 0, 0, 0.9396926f, -0.3420201f, BUFF_RESPAWN_TIME)
        /// Alliance gates
        || !AddObject(BG_TP_OBJECT_DOOR_A_1, BG_OBJECT_DOOR_A_1_TP_ENTRY, 2116.7f, 152.521f, 43.80f, 2.615f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY) //:FEELSCLEANMAN:
        || !AddObject(BG_TP_OBJECT_DOOR_A_2, BG_OBJECT_DOOR_A_2_TP_ENTRY, 2155.851f, 219.024f, 43.649f, 2.613f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY) //:FEELSCLEANMAN:
        || !AddObject(BG_TP_OBJECT_DOOR_A_3, BG_OBJECT_DOOR_A_3_TP_ENTRY, 2127.512f, 223.711f, 43.640f, 2.544690f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TP_OBJECT_DOOR_A_4, BG_OBJECT_DOOR_A_4_TP_ENTRY, 2096.102f, 166.920f, 54.230f, 2.544690f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        /// Horde gates
        || !AddObject(BG_TP_OBJECT_DOOR_H_1, BG_OBJECT_DOOR_H_1_TP_ENTRY, 1556.595f, 314.502f, 1.2230f, 6.179126f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TP_OBJECT_DOOR_H_2, BG_OBJECT_DOOR_H_2_TP_ENTRY, 1587.093f, 319.853f, 1.5233f, 6.179126f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TP_OBJECT_DOOR_H_3, BG_OBJECT_DOOR_H_3_TP_ENTRY, 1591.463f, 365.732f, 13.494f, 6.179126f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TP_OBJECT_DOOR_H_4, BG_OBJECT_DOOR_H_4_TP_ENTRY, 1558.315f, 372.709f, 1.4840f, 6.179126f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TP_OBJECT_alligate_z_forceChange, BG_OBJECT_WUFC_SOCALLED_MASSIVE_DOOR, 2156.00f, 220.0f, 8.43f, 1.042205f, 0, 0, 0, 0, RESPAWN_IMMEDIATELY)
        )
    {
        TC_LOG_ERROR("misc", "BatteGroundTP: Failed to spawn some objects. Battleground not created!");
        return false;
    }

    /// Load spirit guides
    for (uint32 i = TP_GRAVEYARD_START_ALLIANCE; i < TP_MAX_GRAVEYARDS; ++i)
    {
        WorldSafeLocsEntry const* grave = sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[i]);

        if (grave)
        {
            uint8 team = i % 2; ///< If 0 team == TEAM_ALLIANCE else TEAM_HORDE
            if (!AddSpiritGuide(team == TEAM_ALLIANCE ? TP_SPIRIT_ALLIANCE : TP_SPIRIT_HORDE, grave->x, grave->y, grave->z, team == TEAM_ALLIANCE ? M_PI : 0, team == TEAM_ALLIANCE ? ALLIANCE : HORDE))
            {
                TC_LOG_ERROR("misc", "BatteGroundTP: Failed to spawn spirit guide id: %u. Battleground not created!", grave->ID);
                return false;
            }
        }
        else
        {
            TC_LOG_ERROR("misc", "BatteGroundTP: Failed to find grave %u. Battleground not created!", BG_TP_GraveyardIds[i]);
            return false;
        }
    }
    ANTICHEAT_original_Flag_Puller[TEAM_ALLIANCE] = 0;
    ANTICHEAT_original_Flag_Puller[TEAM_HORDE] = 0;
    setup_ASSAULT_VALUES();
    return true;
}

void BattlegroundTP::Reset()
{
    /// Call Main Battleground reset
    Battleground::Reset();

    /// Start reset for Battleground specific miscelanouse
    for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
    {
        /// Unbind all flag stuff
        _flagKeepers[team] = 0;
        _droppedFlagGUID[team] = 0;
        _flagsDropTimer[team] = 0;
        _flagState[team] = BG_TP_FLAG_STATE_ON_BASE;

        /// Reset score
        m_TeamScores[team] = 0;
    }

    _flagsTimer = 0;
}

void BattlegroundTP::FillInitialWorldStates(WorldPacket& data)
{

    /// Show how many flags had been captured
    data << uint32(BG_TP_FLAG_CAPTURES_ALLIANCE) << uint32(GetTeamScore(TEAM_ALLIANCE));
    data << uint32(BG_TP_FLAG_CAPTURES_HORDE) << uint32(GetTeamScore(TEAM_HORDE));

    /// Show MAX number of flags (x/3)
    data << uint32(BG_TP_FLAG_CAPTURES_MAX) << uint32(BG_TP_MAX_TEAM_SCORE);

    /// Next Stuff showed only if BG is in progress
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        /// Show Flag state - if flag is on player / ground / in base
        for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
        {
            switch (_flagState[team])
            {
            case BG_TP_FLAG_STATE_ON_GROUND:
                data << uint32(BG_TP_FLAG_UNK_ALLIANCE + team) << uint32(-1);
                data << uint32(BG_TP_FLAG_STATE_HORDE + team) << uint32(3); ///< Show if team's flag is carried
                break;
            case BG_TP_FLAG_STATE_ON_PLAYER:
                data << uint32(BG_TP_FLAG_UNK_ALLIANCE + team) << uint32(1);
                data << uint32(BG_TP_FLAG_STATE_HORDE + team) << uint32(2); ///< Show if team's flag is carried
                break;
            default: ///< In Base
                data << uint32(BG_TP_FLAG_UNK_ALLIANCE + team) << uint32(0);
                data << uint32(BG_TP_FLAG_STATE_HORDE + team) << uint32(1); ///< Show if team's flag is carried
                break;
            }
        }

        /// Show Timer
        data << uint32(BG_TP_STATE_TIMER_ACTIVE) << uint32(1);
        data << uint32(BG_TP_STATE_TIMER) << uint32(25 - _minutesElapsed);
    }
    else
    {
        /// No timer for begining
        data << uint32(BG_TP_STATE_TIMER_ACTIVE) << uint32(0);

        /// Just show the maxscore and actual score (0)
        data << uint32(BG_TP_FLAG_UNK_ALLIANCE) << uint32(0);
        data << uint32(BG_TP_FLAG_UNK_HORDE) << uint32(0);
        data << uint32(BG_TP_FLAG_STATE_HORDE) << uint32(1);
        data << uint32(BG_TP_FLAG_STATE_ALLIANCE) << uint32(1);
    }
}

void BattlegroundTP::EndBattleground(uint32 winner)
{
    /// If BG ends with equal flag captured (draw) and both flags are kept the debuffs + flag aura stays on player, and it shouldn't
    for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
        if (auto player = ObjectAccessor::FindPlayer(_flagKeepers[team]))
        {
            //remove brutal assault on BG end
            player->RemoveAurasDueToSpell(TP_SPELL_FOCUSED_ASSAULT);
            player->RemoveAurasDueToSpell(TP_SPELL_BRUTAL_ASSAULT);
        }


    /// Update a lot of world states
    UpdateWorldState(BG_TP_FLAG_UNK_ALLIANCE, 0);
    UpdateWorldState(BG_TP_FLAG_UNK_HORDE, 0);
    UpdateWorldState(BG_TP_FLAG_STATE_ALLIANCE, 0);
    UpdateWorldState(BG_TP_FLAG_STATE_HORDE, 0);
    UpdateWorldState(BG_TP_STATE_TIMER_ACTIVE, 0);

    DistributeHonorToAll(winner);
    Battleground::EndBattleground(winner);
}

void BattlegroundTP::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    /// Call event player droped flag
    if (player->HasAura(BG_TP_SPELL_HORDE_FLAG) || player->HasAura(BG_TP_SPELL_ALLIANCE_FLAG))
        EventPlayerDroppedFlag(player);

    if (killer)
        if (player)
            Battleground::HandleKillPlayer(player, killer);
        else TC_LOG_ERROR("bg.battleground", "BattlegroundTP::HandleKillPlayer: Killer player not found");
    else TC_LOG_ERROR("bg.battleground", "BattlegroundTP::HandleKillPlayer: Victim player not found");
}

void BattlegroundTP::HandleAreaTrigger(Player* player, uint32 trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (trigger)
    {
    case 5904: // Alliance Flag spawn
        if (_flagState[TEAM_HORDE] && !_flagState[TEAM_ALLIANCE])
            if (_flagKeepers[TEAM_HORDE] == player->GetGUID())
                EventPlayerCapturedFlag(player);
        break;
    case 5905: // Horde Flag spawn
        if (_flagState[TEAM_ALLIANCE] && !_flagState[TEAM_HORDE])
            if (_flagKeepers[TEAM_ALLIANCE] == player->GetGUID())
                EventPlayerCapturedFlag(player);
        break;
    case 5908: // Horde Tower
    case 5909: // Twin Peak House big
    case 5910: // Horde House
    case 5911: // Twin Peak House small
    case 5914: // Alliance Start right
    case 5916: // Alliance Start
    case 5917: // Alliance Start left
    case 5918: // Horde Start
    case 5920: // Horde Start Front entrance
    case 5921: // Horde Start left Water channel
        break;
    default:
        Battleground::HandleAreaTrigger(player, trigger);
        break;
    }
}

void BattlegroundTP::UpdatePlayerScore(Player* player, uint32 type, uint32 value, bool doAddHonor)
{
    /// Find player in map
    BattlegroundScoreMap::iterator itr = PlayerScores.find(player->GetGUID());
    if (itr == PlayerScores.end()) ///< Player not found
        return;

    /// Update Achievements + scores
    switch (type)
    {
    case SCORE_FLAG_CAPTURES:
        ((BattlegroundTPScore*)itr->second)->FlagCaptures += value;
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, TP_OBJECTIVE_CAPTURE_FLAG);
        break;
    case SCORE_FLAG_RETURNS:
        ((BattlegroundTPScore*)itr->second)->FlagReturns += value;
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, TP_OBJECTIVE_RETURN_FLAG);
        break;
    default:///< else only count another kill
        Battleground::UpdatePlayerScore(player, type, value, doAddHonor);
        break;
    }
}

void BattlegroundTP::UpdateTeamScore(uint32 team)
{
    ///< After a capture update score
    if (team == TEAM_ALLIANCE)
        UpdateWorldState(BG_TP_FLAG_CAPTURES_ALLIANCE, GetTeamScore(team));
    else
        UpdateWorldState(BG_TP_FLAG_CAPTURES_HORDE, GetTeamScore(team));
}

/**
* Next method finds the right graveyard for player to resurect
* As 4.1: The right graveyard is not the nearest one
* So:
* Players will now only spawn at their base graveyard when they die in the enemy base.
* Defending players will respawn at the middle graveyard.
* Midfield players will respawn at the middle graveyard.
* Attacking players will respawn at their base graveyard.
* It's not the best "name" for this method
*/
WorldSafeLocsEntry const* BattlegroundTP::GetClosestGraveYard(Player* player)
{
    if (!player)
        return NULL;

    uint8 team = player->GetTeamId();

    if (GetStatus() != STATUS_IN_PROGRESS) ///< If battle didn't start yet and player is death (unprobably) revive in flagroom
        return sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_FLAGROOM_ALLIANCE + team]);

    if (isRatedBattleground())
    {
        if ((player->GetAreaId() == 5680 && team == TEAM_HORDE) || (player->GetAreaId() == 5681 && team == TEAM_ALLIANCE))
            return sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_START_ALLIANCE + team]);
        else
            return sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_MIDDLE_ALLIANCE + team]);
    }
    else
    {
        /// Check if player if is closer to the enemy base than the center
        WorldSafeLocsEntry const* grave_enemy_base = sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_FLAGROOM_ALLIANCE + (team ^ 1)]);
        WorldSafeLocsEntry const* grave_enemy_middle = sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_MIDDLE_ALLIANCE + (team ^ 1)]);

        if (player->GetDistance2d(grave_enemy_base->x, grave_enemy_base->y) < player->GetDistance2d(grave_enemy_middle->x, grave_enemy_middle->y))
            return sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_START_ALLIANCE + team]);
        else
            return sWorldSafeLocsStore.LookupEntry(BG_TP_GraveyardIds[TP_GRAVEYARD_MIDDLE_ALLIANCE + team]);
    }
}

void BattlegroundTP::EventPlayerDroppedFlag(Player* source)
{
    uint8 team = source->GetTeamId();

    /// Mainly used when a player captures the flag, it prevents spawn the flag on ground
    if (!_flagKeepers[team ^ 1])
        return;

    /// Most probably useless - If a GM applies the aura on a player
    if (_flagKeepers[team ^ 1] != source->GetGUID())
    {
        TC_LOG_ERROR("bg.battleground", "BattlegroundTP: An error have occured in EventPlayerDroppedFlag, player: %u who carried the flag is not the flag keeper: %u.", source->GetGUID(), _flagKeepers[team ^ 1]);
        return;
    }

    //remove brutal assault on flag drop
    remove_ASSAULTS(source);
    remove_REMAINING_ASSAULTS();

    /// Update flag state and flagkeepers
    UpdateFlagState(team ^ 1, BG_TP_FLAG_STATE_ON_GROUND);

    /// "Drop the flag" + Set timer for flag
    source->CastSpell(source, team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG_DROPPED : BG_TP_SPELL_ALLIANCE_FLAG_DROPPED, true);
    _flagsDropTimer[team ^ 1] = BG_TP_FLAG_DROP_TIME;

    /// Remove flag
    source->RemoveAurasDueToSpell(team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);

    /// Apply debuff for recently dropped flag
    source->CastSpell(source, SPELL_RECENTLY_DROPPED_FLAG, true);

    /// Send message to all
    SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_TP_DROPPED_HF : LANG_BG_TP_DROPPED_AF, team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_HORDE : CHAT_MSG_BG_SYSTEM_ALLIANCE, source);
}


void BattlegroundTP::EventPlayerClickedOnFlag(Player* source, GameObject* target_obj)
{
    if ((GetStatus() != STATUS_IN_PROGRESS) || (source->isGameMaster()) || (!source->isGMVisible()))
        return;

    uint8 team = source->GetTeamId();

    if (source->IsWithinDistInMap(target_obj, 10)) ///< Check if target is in distance with flag
    {
        switch (_flagState[team ^ 1]) ///< Check opposite team faction state
        {
        case BG_TP_FLAG_STATE_ON_BASE: ///< If opposite team has flag in base
        {
            if (BgObjects[BG_TP_OBJECT_A_FLAG + (team ^ 1)] == target_obj->GetGUID()) ///< If clicked object is what we want
            {
                //BEGIN "TOOFAST" check
                if (pickedUpTooSoon_counter > 0 && pickedUpTooSoon_counter < 20000) {
                    SendMessageToAll(team == TEAM_HORDE ? LANG_BG_TP_RETURNED_AF : LANG_BG_TP_RETURNED_HF, team == TEAM_HORDE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, source);
                    source->LeaveBattleground();
                    source->CastCustomSpell(26013, SPELLVALUE_DURATION, 86400000, source);
                    return;
                }
                ///Dispawn BGObject
                SpawnBGObject(team == TEAM_ALLIANCE ? BG_TP_OBJECT_H_FLAG : BG_TP_OBJECT_A_FLAG, RESPAWN_ONE_DAY);

                /// Update flag state + world state
                UpdateFlagState(team ^ 1, BG_TP_FLAG_STATE_ON_PLAYER, source->GetGUID());

                /// "Pick up the flag"
                source->CastSpell(source, team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG, true);

                /// Start achievement criteria
                source->StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_SPELL_TARGET, team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG_PICKED : BG_TP_SPELL_ALLIANCE_FLAG_PICKED);

                /// Verify if both flag are carried
                if (_flagKeepers[team])
                    _bothFlagsKept = true;

                /// Reapply debuff <- no, you just resume the counter which should be at 0 if they're picking it up from the base.
                setup_ASSAULT_VALUES(); //manually reset the counters to zero if they were, for some reason, not zeros before.

                /// Send message to all players + Play sound
                SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_TP_PICKEDUP_HF : LANG_BG_TP_PICKEDUP_AF, team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, source);
                PlaySoundToAll(team == TEAM_ALLIANCE ? BG_TP_SOUND_HORDE_FLAG_PICKED_UP : BG_TP_SOUND_ALLIANCE_FLAG_PICKED_UP);
            }
            break;
        }
        case BG_TP_FLAG_STATE_ON_GROUND:
        {
            if (_droppedFlagGUID[team ^ 1] == target_obj->GetGUID()) ///< Check opposite team faction flag
            {
                /// Reapply flag on target
                source->CastSpell(source, team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG, true);

                /// Update flag state
                UpdateFlagState(team ^ 1, BG_TP_FLAG_STATE_ON_PLAYER, source->GetGUID());

                /// Reset flag on the ground counter
                _flagsDropTimer[team ^ 1] = 0;

                /// Announce players
                PlaySoundToAll(team == TEAM_ALLIANCE ? BG_TP_SOUND_HORDE_FLAG_PICKED_UP : BG_TP_SOUND_ALLIANCE_FLAG_PICKED_UP);
                SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_TP_PICKEDUP_HF : LANG_BG_TP_PICKEDUP_AF, team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, source);

                /// Reapply debuff <- correct
                update_ALL_ASSAULTS();
            }
            break;
        }
        }

        /// If player from same side click on the flag that stand on ground return to base
        if (_flagState[team] == BG_TP_FLAG_STATE_ON_GROUND && _droppedFlagGUID[team] == target_obj->GetGUID())
        {
            // Announce players      
            RespawnFlag(team, 1);
            SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_TP_RETURNED_AF : LANG_BG_TP_RETURNED_HF, team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, source);
            /// Update world states
            UpdateFlagState(team, BG_TP_FLAG_STATE_ON_BASE);
            UpdatePlayerScore(source, SCORE_FLAG_RETURNS, 1);
            source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
            setup_ASSAULT_VALUES();
            remove_REMAINING_ASSAULTS();           
        }
           
    }
}

void BattlegroundTP::EventPlayerCapturedFlag(Player* source)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 team = source->GetTeamId();
    /*anticheat check here*/
    if ((ANTICHEAT_time_since_pull[team ^ 1] > 0) && (ANTICHEAT_time_since_pull[team ^ 1] < 20000) && (ANTICHEAT_original_Flag_Puller[team ^ 1] == source->GetGUID())) //he cheated. too fast.
    {
        source->RemoveAurasDueToSpell(team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
        _flagsDropTimer[team ^ 1] = 0;
        RespawnFlag(team ^ 1, 4);
        if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team ^ 1]))
            obj->Delete();
        _droppedFlagGUID[team ^ 1] = 0;
        PlaySoundToAll(BG_TP_SOUND_FLAG_PLACED);
        setup_ASSAULT_VALUES();
        remove_REMAINING_ASSAULTS();
        source->LeaveBattleground();
        source->CastCustomSpell(26013, SPELLVALUE_DURATION, 86400000, source);
        /*
                UpdateFlagState(team ^ 1, BG_TP_FLAG_STATE_ON_BASE);
                _flagsTimer = BG_TP_FLAG_RESPAWN_TIME; //necessary
                remove_ASSAULTS(source);
                setup_ASSAULT_VALUES();
                SpawnBGObject(BG_TP_OBJECT_A_FLAG, RESPAWN_ONE_DAY);
                SpawnBGObject(BG_TP_OBJECT_H_FLAG, RESPAWN_ONE_DAY);
                source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);*/

    }
    else
    {
        remove_ASSAULTS(source);
        setup_ASSAULT_VALUES();
        SpawnBGObject(BG_TP_OBJECT_A_FLAG, RESPAWN_ONE_DAY);
        SpawnBGObject(BG_TP_OBJECT_H_FLAG, RESPAWN_ONE_DAY);
        source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
        UpdateWorldState(BG_TP_FLAG_UNKNOWN, -1); ///< From sniffs
        UpdateFlagState(team ^ 1, BG_TP_FLAG_STATE_WAIT_RESPAWN);
        UpdateWorldState(BG_TP_STATE_UNKNOWN, 1); ///< From sniffs
        /// Update scores + flag state + count flag caputured to carrier
        AddPoint(team == TEAM_ALLIANCE ? ALLIANCE : HORDE);
        UpdateTeamScore(team);
        /*Everything below here is non-critical and rewards. this stuff can be skipped over safely and allow the battleground to continue.*/       
        /// Reward flag capture with 2x honorable kills
        RewardHonorToTeam(GetBonusHonorFromKill(2), source->GetTeam());       
        UpdatePlayerScore(source, SCORE_FLAG_CAPTURES, 1);       
        /// Set last team that captured flag
        _lastFlagCaptureTeam = source->GetTeam();
        /// Play sound + Send message to all
        PlaySoundToAll(team == TEAM_ALLIANCE ? BG_TP_SOUND_FLAG_CAPTURED_ALLIANCE : BG_TP_SOUND_FLAG_CAPTURED_HORDE);
        SendMessageToAll(team == TEAM_ALLIANCE ? LANG_BG_TP_CAPTURED_HF : LANG_BG_TP_CAPTURED_AF, team == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE : CHAT_MSG_BG_SYSTEM_HORDE, source);
        /// IMPORTANT: Do not remove aura before flag update! Makes the flag spawn like is dropped
        source->RemoveAurasDueToSpell(team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
        /// Last but not least verify if score is max to end
        if (GetTeamScore(team) == BG_TP_MAX_TEAM_SCORE)
        {
            EndBattleground(source->GetTeam());
        }
        else
        {
            _flagsTimer = BG_TP_FLAG_RESPAWN_TIME;
        }
    }
}

void BattlegroundTP::RemovePlayer(Player* player, uint64 guid, uint32 /* team */)
{
    for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
    {
        if (_flagKeepers[team ^ 1] == guid)//this was a flag carrier being removed
        {
            if (_bothFlagsKept)
            {
                /// Remove aura from both of them
                remove_REMAINING_ASSAULTS();

                if (player)
                    player->RemoveAurasDueToSpell(team == TEAM_ALLIANCE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
            }
            EventPlayerDroppedFlag(player);
            //RespawnFlag(team ^ 1, 4);
        }
    }
}

void BattlegroundTP::RespawnFlag(uint32 team, uint8 respawnReason)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch (respawnReason)
    {
    case 1://This flag was returned through a manual return.
        SpawnBGObject(BG_TP_OBJECT_A_FLAG + team, RESPAWN_IMMEDIATELY);
        PlaySoundToAll(BG_TP_SOUND_FLAG_RETURNED);
        break;
    case 2://This flag was returned via timer delay after being dropped.
        SpawnBGObject(BG_TP_OBJECT_A_FLAG + team, RESPAWN_IMMEDIATELY);
        SendMessageToAll(LANG_BG_TP_ALLIANCE_FLAG_RESPAWNED + team, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll(BG_TP_SOUND_FLAGS_RESPAWNED);
        break;
    case 3://Both flags get spawned several seconds after a capture.
        SpawnBGObject(BG_TP_OBJECT_H_FLAG, RESPAWN_IMMEDIATELY);
        SpawnBGObject(BG_TP_OBJECT_A_FLAG, RESPAWN_IMMEDIATELY);
        SendMessageToAll(LANG_BG_TP_F_PLACED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        PlaySoundToAll(BG_TP_SOUND_FLAGS_RESPAWNED);
        break;
    case 4://This flag was returned through a forced automatic return (Anticheat timer, Player Removal, Graveyard Proximity)
        SpawnBGObject(BG_TP_OBJECT_A_FLAG + team, RESPAWN_IMMEDIATELY);
        PlaySoundToAll(BG_TP_SOUND_FLAGS_RESPAWNED);
        SendMessageToAll(LANG_BG_TP_ALLIANCE_FLAG_RESPAWNED + team, CHAT_MSG_BG_SYSTEM_NEUTRAL);
        break;
    default:
        TC_LOG_ERROR("bg.battleground", "BattlegroundTP: Flag returned with an unidentifiable return source value.");
        break;
    }
    _bothFlagsKept = false;
    if (!_flagKeepers[team ^ 1])
    {
        _flagSpellForceTimer = 0;
    }
    UpdateFlagState(team, BG_TP_FLAG_STATE_ON_BASE);

    if (respawnReason == 1 || respawnReason == 2 || respawnReason == 4)
        for (uint8 teamTemp = TEAM_ALLIANCE; teamTemp <= TEAM_HORDE; ++teamTemp)
        {
            if (auto activeCarrier = ObjectAccessor::FindPlayer(_flagKeepers[teamTemp]))
            {
                if (activeCarrier->GetDistance((team == TEAM_ALLIANCE ? 2118.210f : 1578.380f), (team == TEAM_ALLIANCE ? 191.621f : 344.037f), (team == TEAM_ALLIANCE ? 44.052f : 2.419f)) <= 3.0f)
                { //exact duplicates of flag locations
                    return EventPlayerCapturedFlag(activeCarrier);
                }
                else
                {
                    PlaySoundToAll(BG_TP_SOUND_FLAG_RETURNED);//the flag capture has it's own sound, so we'll only play the return sound if it can be played alone.
                }
            }
        }
    //end check for instant flag cap
}

void BattlegroundTP::UpdateFlagState(uint32 team, uint32 value, uint64 flagKeeperGUID)
{
    switch (value)
    {
        /// Values from sniffs
    case BG_TP_FLAG_STATE_WAIT_RESPAWN:
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_UNK_ALLIANCE : BG_TP_FLAG_UNK_HORDE, 0);
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_STATE_HORDE : BG_TP_FLAG_STATE_ALLIANCE, 1);
        ANTICHEAT_original_Flag_Puller[team] = 0;
        ANTICHEAT_time_since_pull[team] = 0;
        break;
    case BG_TP_FLAG_STATE_ON_BASE:
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_UNK_ALLIANCE : BG_TP_FLAG_UNK_HORDE, 0);
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_STATE_HORDE : BG_TP_FLAG_STATE_ALLIANCE, 1);
        ANTICHEAT_original_Flag_Puller[team] = 0;
        ANTICHEAT_time_since_pull[team] = 0;
        break;
    case BG_TP_FLAG_STATE_ON_GROUND:
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_UNK_ALLIANCE : BG_TP_FLAG_UNK_HORDE, -1);
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_STATE_HORDE : BG_TP_FLAG_STATE_ALLIANCE, BG_TP_FLAG_STATE_ON_GROUND);
        break;
    case BG_TP_FLAG_STATE_ON_PLAYER:
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_UNK_ALLIANCE : BG_TP_FLAG_UNK_HORDE, 1);
        UpdateWorldState(team == TEAM_ALLIANCE ? BG_TP_FLAG_STATE_HORDE : BG_TP_FLAG_STATE_ALLIANCE, 2);

        if (ANTICHEAT_original_Flag_Puller[team] == 0) {//this flag has been picked up, but it is not on pl
            ANTICHEAT_original_Flag_Puller[team] = flagKeeperGUID;
        }
        if (ANTICHEAT_time_since_pull[team] == 0) {
            ANTICHEAT_time_since_pull[team] = 1;
        }
        break;
    default:
        break;
    }

    _flagState[team] = value;
    _flagKeepers[team] = flagKeeperGUID;
}

//BEGIN ASSAULT METHODS
void BattlegroundTP::clock_tick_ASSAULT_MANAGEMENT() {
    if (_bothFlagsKept) {//both flags being held set this value to TRUE, maybe. if not, I can check.
        ++ASSAULT_TICKS_COUNT;

        if (ASSAULT_TICKS_COUNT >= 1200) {//1 minute has passed
            ASSAULT_TICKS_COUNT = 0;

            if (ASSAULT_STACKS < 10) {
                ++ASSAULT_TICK_MINUTES;

                if (ASSAULT_TICK_MINUTES >= 3) {
                    if (get_ASSAULT_STACKS() == 0) {
                        SendMessageToAll(LANG_BG_CARRIERS_VULNERABLE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    }
                    else {
                        SendMessageToAll(LANG_BG_CARRIERS_MORE_VULNERABLE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    }
                    ASSAULT_STACKS = (ASSAULT_TICK_MINUTES - 2);
                    return update_ALL_ASSAULTS();

                }
                else {
                    return;
                }
            }
            else {
                return;
            }
        }
        else {
            return;
        }
    }
    else {
        return;
    }
}

uint16 BattlegroundTP::get_ASSAULT_ID(bool wantCurrentDebuff) {
    if (get_ASSAULT_STACKS() < 4) {
        return wantCurrentDebuff ? TP_SPELL_FOCUSED_ASSAULT : TP_SPELL_BRUTAL_ASSAULT;
    }
    else {
        return !wantCurrentDebuff ? TP_SPELL_FOCUSED_ASSAULT : TP_SPELL_BRUTAL_ASSAULT;
    }
}

void BattlegroundTP::remove_ASSAULTS(Player* target) {
    target->RemoveAurasDueToSpell(get_ASSAULT_ID(true));
    target->RemoveAurasDueToSpell(get_ASSAULT_ID(false));
}

void BattlegroundTP::apply_correct_ASSAULT(Player* target) {
    target->RemoveAurasDueToSpell(get_ASSAULT_ID(true));//remove first debuff if it exists
    target->RemoveAurasDueToSpell(get_ASSAULT_ID(false));//remove the second debuff if it exists
    if (get_ASSAULT_STACKS() > 0) {
        target->CastCustomSpell(get_ASSAULT_ID(true), SPELLVALUE_AURA_STACK, get_ASSAULT_STACKS(), target); //add the new buff, at the updated correct stacks.
    }
}
void BattlegroundTP::update_ALL_ASSAULTS() {
    for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
        if (auto carrier = ObjectAccessor::FindPlayer(_flagKeepers[team])) {
            apply_correct_ASSAULT(carrier);
        }
    return;
}
void BattlegroundTP::remove_REMAINING_ASSAULTS()
{
    for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
        if (auto carrier = ObjectAccessor::FindPlayer(_flagKeepers[team])) {
            remove_ASSAULTS(carrier);
        }
}
void BattlegroundTP::setup_ASSAULT_VALUES() {
    ASSAULT_STACKS = 0;
    ASSAULT_TICKS_COUNT = 0;
    ASSAULT_TICK_MINUTES = 0;
}
    void BattlegroundTP::reset_flag_in_GY() {
        for (uint8 team = TEAM_ALLIANCE; team <= TEAM_HORDE; ++team)
            if (auto carrier = ObjectAccessor::FindPlayer(_flagKeepers[team]))
            {
                if (team == TEAM_HORDE)
                {
                    //alliance FC, middle gy.
                    if
                        (
                            (carrier->GetDistance(1877.150024f, 439.110992f, -4.000170f) <= 15.0f)
                            &&
                            (carrier->GetPositionZ() - (-4.000170f) > -2.0f)
                            ) { //within 15 units of friendly spirit guide and less than 2 z lower.
                        carrier->RemoveAurasDueToSpell(team == TEAM_HORDE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
                        _flagsDropTimer[team] = 0;
                        RespawnFlag(team, 4);
                        if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team]))
                            obj->Delete();
                        _droppedFlagGUID[team] = 0;
                        PlaySoundToAll(BG_TP_SOUND_FLAG_PLACED);
                        setup_ASSAULT_VALUES();
                        remove_REMAINING_ASSAULTS();
                    }
                    //alliance FC, outer gy.
                    if
                        (
                            (carrier->GetDistance(2176.116211f, 330.332733f, 33.978676f) <= 26.0f)
                            &&
                            (carrier->GetPositionZ() - (33.978676f) > -2.0f)
                            ) { //within 15 units of friendly spirit guide and less than 2 z lower.
                        carrier->RemoveAurasDueToSpell(team == TEAM_HORDE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
                        _flagsDropTimer[team] = 0;
                        RespawnFlag(team, 4);
                        if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team]))
                            obj->Delete();
                        _droppedFlagGUID[team] = 0;
                        PlaySoundToAll(BG_TP_SOUND_FLAG_PLACED);
                        setup_ASSAULT_VALUES();
                        remove_REMAINING_ASSAULTS();
                    }
                }
                else
                {
                    //horde fc, middle gy.
                    if
                        (
                            (carrier->GetDistance(1816.869995f, 160.082993f, 1.806440f) <= 25.0f)
                            &&
                            (carrier->GetPositionZ() - 1.806440f > -2.0f)
                            ) { //within 25 units of friendly spirit guide and less than 2 z lower.
                        carrier->RemoveAurasDueToSpell(team == TEAM_HORDE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
                        _flagsDropTimer[team] = 0;
                        RespawnFlag(team, 4);
                        if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team]))
                            obj->Delete();
                        _droppedFlagGUID[team] = 0;
                        PlaySoundToAll(BG_TP_SOUND_FLAG_PLACED);
                        setup_ASSAULT_VALUES();
                        remove_REMAINING_ASSAULTS();
                    }
                    //horde FC, outer gy.
                    if
                        (
                            (carrier->GetDistance(1548.786987f, 211.927460f, 14.100624f) <= 30.0f)
                            &&
                            (carrier->GetPositionZ() - (14.100624f) > -2.0f)
                            ) { //within 15 units of friendly spirit guide and less than 2 z lower.
                        carrier->RemoveAurasDueToSpell(team == TEAM_HORDE ? BG_TP_SPELL_HORDE_FLAG : BG_TP_SPELL_ALLIANCE_FLAG);
                        _flagsDropTimer[team] = 0;
                        RespawnFlag(team, 4);
                        if (auto obj = GetBgMap()->GetGameObject(_droppedFlagGUID[team]))
                            obj->Delete();
                        _droppedFlagGUID[team] = 0;
                        PlaySoundToAll(BG_TP_SOUND_FLAG_PLACED);
                        setup_ASSAULT_VALUES();
                        remove_REMAINING_ASSAULTS();
                    }
                }
            }
    }