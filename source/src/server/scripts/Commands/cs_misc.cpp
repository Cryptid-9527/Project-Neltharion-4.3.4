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

#include "Chat.h"
#include "CreatureGroups.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "InstanceSaveMgr.h"
#include "InfoMgr.h"
#include "Language.h"
#include "MovementGenerator.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "SpellAuras.h"
#include "TargetedMovementGenerator.h"
#include "WeatherMgr.h"
#include "ace/INET_Addr.h"
#include "Player.h"
#include "Pet.h"
#include "LFG.h"
#include "GroupMgr.h"
#include "Transport.h"

class misc_commandscript : public CommandScript
{
public:
    misc_commandscript() : CommandScript("misc_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> utilityCommandTable =
        {
            { "changerace",     SEC_CONSOLE,              false,  &HandleUtilityChangeRaceCommand,    "" },
            { "changefaction",  SEC_CONSOLE,              false,  &HandleUtilityChangeFactionCommand, "" },
            { "customize",      SEC_CONSOLE,              false,  &HandleUtilityCustomizeCommand,     "" },
        };
        static std::vector<ChatCommand> groupCommandTable =
        {
            { "leader",         SEC_CONSOLE,          false,  &HandleGroupLeaderCommand,          "" },
            { "disband",        SEC_CONSOLE,          false,  &HandleGroupDisbandCommand,         "" },
            { "remove",         SEC_CONSOLE,          false,  &HandleGroupRemoveCommand,          "" },
            { "join",           SEC_CONSOLE,          false,  &HandleGroupJoinCommand,            "" },
            { "list",           SEC_CONSOLE,          false,  &HandleGroupListCommand,            "" },
        };
        static std::vector<ChatCommand> petCommandTable =
        {
            { "create",             SEC_CONSOLE,         false, &HandleCreatePetCommand,             "" },
            { "learn",              SEC_CONSOLE,         false, &HandlePetLearnCommand,              "" },
            { "unlearn",            SEC_CONSOLE,         false, &HandlePetUnlearnCommand,            "" },
        };

        static std::vector<ChatCommand> itemCommandTable =
        {
            { "move",           SEC_CONSOLE,         false, &HandleItemMoveCommand,              "" },
            { "delete",         SEC_CONSOLE,         false, &HandleItemDeleteCommand,            "" },
            { "transfer",       SEC_CONSOLE,         false, &HandleItemTransferCommand,          "" },
        };

        static std::vector<ChatCommand> sendCommandTable =
        {
            { "items",              SEC_CONSOLE,      true,  &HandleSendItemsCommand,             "" },
            { "mail",               SEC_CONSOLE,          true,  &HandleSendMailCommand,              "" },
            { "message",            SEC_CONSOLE,      true,  &HandleSendMessageCommand,           "" },
            { "money",              SEC_CONSOLE,      true,  &HandleSendMoneyCommand,             "" },
        };
        static std::vector<ChatCommand> deserterPvECommandTable =
        {
            { "apply",              SEC_CONSOLE,      false,  &HandlePvEDeserterApplyCommand,     "" },
            { "remove",             SEC_CONSOLE,      false,  &HandlePvEDeserterRemoveCommand,    "" },
        };


        static std::vector<ChatCommand> deserterPvPCommandTable =
        {
            { "apply",              SEC_CONSOLE,      false,  &HandlePvPDeserterApplyCommand,     "" },
            { "remove",             SEC_CONSOLE,      false,  &HandlePvPDeserterRemoveCommand,    "" },
        };

        static std::vector<ChatCommand> deserterCommandTable =
        {
            { "pvp",                SEC_CONSOLE,      false, NULL,                                "", deserterPvPCommandTable },
            { "pve",                SEC_CONSOLE,      false, NULL,                                "", deserterPvECommandTable },
        };

        static std::vector<ChatCommand> guestCommandTable =
        {
            { "add",                 SEC_CONSOLE,      false, &HandleGuestAdd,                      "" },
            { "remove",              SEC_CONSOLE,      false, &HandleGuestRemove,                   "" },
            { "list",                SEC_CONSOLE,      false, &HandleGuestList,                     "" }
        };


        static std::vector<ChatCommand> SniffingCommandTable =
        {
            { "start",              SEC_CONSOLE,      false,  &HandleStartSniffingCommand,             "" },
            { "stop",               SEC_CONSOLE,      false,  &HandleStopSniffingCommand,              "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "item",               SEC_CONSOLE,      false, NULL,                                "", itemCommandTable },
            { "send",               SEC_CONSOLE,      true,  NULL,                                "", sendCommandTable },
            { "pet",                SEC_CONSOLE,      false, NULL,                                "", petCommandTable },
            { "group",              SEC_CONSOLE,      false, NULL,                                "", groupCommandTable },
            { "utility",            SEC_CONSOLE,      false, NULL,                                "", utilityCommandTable },
            { "deserter",           SEC_CONSOLE,      false, NULL,                                "", deserterCommandTable },
            { "guest",              SEC_CONSOLE,      false, NULL,                                "", guestCommandTable },
            { "sniff",              SEC_CONSOLE,      false, NULL,                                "", SniffingCommandTable },
            { "gps",                SEC_CONSOLE,      false, &HandleGPSCommand,                   "" },
            { "aura",               SEC_CONSOLE,      false, &HandleAuraCommand,                  "" },
            { "unaura",             SEC_CONSOLE,      false, &HandleUnAuraCommand,                "" },
            { "appear",             SEC_CONSOLE,      false, &HandleAppearCommand,                "" },
            { "summon",             SEC_CONSOLE,      false, &HandleSummonCommand,                "" },
            { "groupsummon",        SEC_CONSOLE,      false, &HandleGroupSummonCommand,           "" },
            { "commands",           SEC_CONSOLE,      true,  &HandleCommandsCommand,              "" },
            { "die",                SEC_CONSOLE,      false, &HandleDieCommand,                   "" },
            { "revive",             SEC_CONSOLE,      true,  &HandleReviveCommand,                "" },
            { "reviveunstuck",      SEC_CONSOLE,      true,  &HandleReviveUnstuckCommand,         "" },
            { "dismount",           SEC_CONSOLE,      false, &HandleDismountCommand,              "" },
            { "guid",               SEC_CONSOLE,      false, &HandleGUIDCommand,                  "" },
            { "help",               SEC_CONSOLE,      true,  &HandleHelpCommand,                  "" },
            { "cooldown",           SEC_CONSOLE,      false, &HandleCooldownCommand,              "" },
            { "distance",           SEC_CONSOLE,      false, &HandleGetDistanceCommand,           "" },
            { "recall",             SEC_CONSOLE,      false, &HandleRecallCommand,                "" },
            { "save",               SEC_CONSOLE,      false, &HandleSaveCommand,                  "" },
            { "saveall",            SEC_CONSOLE,      true,  &HandleSaveAllCommand,               "" },
            { "kick",               SEC_CONSOLE,      true,  &HandleKickPlayerCommand,            "" },
            { "unstuck",            SEC_CONSOLE,      true,  &HandleUnstuckCommand,               "" },
            { "linkgrave",          SEC_CONSOLE,      false, &HandleLinkGraveCommand,             "" },
            { "neargrave",          SEC_CONSOLE,      false, &HandleNearGraveCommand,             "" },
            { "showarea",           SEC_CONSOLE,      false, &HandleShowAreaCommand,              "" },
            { "hidearea",           SEC_CONSOLE,      false, &HandleHideAreaCommand,              "" },
            { "additem",            SEC_CONSOLE,      false, &HandleAddItemCommand,               "" },
            { "additemset",         SEC_CONSOLE,      false, &HandleAddItemSetCommand,            "" },
            { "bank",               SEC_CONSOLE,      false, &HandleBankCommand,                  "" },
            { "wchange",            SEC_CONSOLE,      false, &HandleChangeWeather,                "" },
            { "maxskill",           SEC_CONSOLE,      false, &HandleMaxSkillCommand,              "" },
            { "setskill",           SEC_CONSOLE,      false, &HandleSetSkillCommand,              "" },
            { "pinfo",              SEC_CONSOLE,      true,  &HandlePInfoCommand,                 "" },
            { "testcode",           SEC_CONSOLE,      true,  &HandleTestCodeCommand,                 "" },
            { "respawn",            SEC_CONSOLE,      false, &HandleRespawnCommand,               "" },
            { "mute",               SEC_CONSOLE,      true,  &HandleMuteCommand,                  "" },
            { "shadowmute",         SEC_CONSOLE,      true,  &HandleShadowMuteCommand,            "" },
            { "unmute",             SEC_CONSOLE,      true,  &HandleUnmuteCommand,                "" },
            { "movegens",           SEC_CONSOLE,      false, &HandleMovegensCommand,              "" },
            { "cometome",           SEC_CONSOLE,      false, &HandleComeToMeCommand,              "" },
            { "damage",             SEC_CONSOLE,      false, &HandleDamageCommand,                "" },
            { "combatstop",         SEC_CONSOLE,      true,  &HandleCombatStopCommand,            "" },
            { "repairitems",        SEC_CONSOLE,      true,  &HandleRepairitemsCommand,           "" },
            { "freeze",             SEC_CONSOLE,      false, &HandleFreezeCommand,                "" },
            { "unfreeze",           SEC_CONSOLE,      false, &HandleUnFreezeCommand,              "" },
            { "listfreeze",         SEC_CONSOLE,      false, &HandleListFreezeCommand,            "" },
            { "possess",            SEC_CONSOLE,      false, HandlePossessCommand,                "" },
            { "unpossess",          SEC_CONSOLE,      false, HandleUnPossessCommand,              "" },
            { "bindsight",          SEC_CONSOLE,      false, HandleBindSightCommand,              "" },
            { "unbindsight",        SEC_CONSOLE,      false, HandleUnbindSightCommand,            "" },
            { "playall",            SEC_CONSOLE,      false, HandlePlayAllCommand,                "" },
            { "wintrade",           SEC_CONSOLE,      false, HandleWintradeCommand,               "" },
            { "arena",              SEC_CONSOLE,      false, HandleArenaCommand,                  "" },
			{ "uberbuff",			SEC_CONSOLE,	  false, HandleUberBuffCommand,				  "" },
            { "reveal",             SEC_CONSOLE,      false,  &HandleSetRevealToCommand,          "" },
        };
        return commandTable;
    }
    
    static bool HandleUberBuffCommand(ChatHandler* handler, char const* /*args*/)
	{
		Player* player = handler->GetSession()->GetPlayer();
		if (player->HasAura(1908))
			handler->PSendSysMessage("You already have the buff!");
		else
			player->CastSpell(player, 1908, false);
		return true;
	}

    static bool HandlePvPDeserterApplyCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();
        
        if (!target)
        {
            handler->PSendSysMessage("You must select a target");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 SPELL_DESERTER_PVP = 26013;

        if (*args)
        {
            char* durationStr = strtok((char*)args, " ");
            uint32 duration = 0;

            if (!atoi(durationStr))
            {
                handler->PSendSysMessage("Invalid time, use time in s m h d format, or enter time in seconds");
                handler->SetSentErrorMessage(true);
                return false;
            }
            else
            {
                if (target)
                    target->AddAura(SPELL_DESERTER_PVP, target);

                // Check for s m h d (seconds, minutes, hours, days)
                uint32 realTime = TimeStringToSecs(durationStr);

                if (realTime > 0)
                    duration = realTime;
                else
                    duration = atoi(durationStr);
                
                // Controllers
                if (duration < 1 * SECOND)
                    duration = 1 * SECOND;

                if (duration > 24 * DAY)
                    duration = 24 * DAY; // Max 45 days, client sided -- Edit: Seems to be max 24 days core sided

                if (Aura* aura = target->GetAura(SPELL_DESERTER_PVP))
                    aura->SetDuration(duration * IN_MILLISECONDS);

                std::string durationMsg; // duration string back to GM
                durationMsg = secsToTimeString(duration);
                
                handler->PSendSysMessage("You have applied PvP Deserter to %s\nDuration of Deserter: %s", target->GetName().c_str(), durationMsg.c_str());
            }
        }
        else // Default timer of 15 minutes
        {
            if (target)
                target->AddAura(SPELL_DESERTER_PVP, target);

            handler->PSendSysMessage("You have applied PvP Deserter to %s", target->GetName().c_str());
        }

        return true;
    }

    static bool HandlePvPDeserterRemoveCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage("You must select a target");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 SPELL_DESERTER_PVP = 26013;

        if (Aura* aura = target->GetAura(SPELL_DESERTER_PVP))
        {
            target->RemoveAurasDueToSpell(SPELL_DESERTER_PVP);
            handler->PSendSysMessage("You removed PvP Deserter from %s", target->GetName().c_str());
        }
        else
        {
            handler->PSendSysMessage("Target doesn't have PvP Deserter debuff");
            handler->SetSentErrorMessage(true);
            return false;
        }
        return true;
    }

    static bool HandlePvEDeserterApplyCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage("You must select a target");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 SPELL_DESERTER_PVE = 71041;

        if (*args)
        {
            char* durationStr = strtok((char*)args, " ");
            uint32 duration = 0;

            if (!atoi(durationStr))
            {
                handler->PSendSysMessage("Invalid time, use time in s m h d format, or enter time in seconds");
                handler->SetSentErrorMessage(true);
                return false;
            }
            else
            {
                if (target)
                    target->AddAura(SPELL_DESERTER_PVE, target);

                // Check for s m h d (seconds, minutes, hours, days)
                uint32 realTime = TimeStringToSecs(durationStr);

                if (realTime > 0)
                    duration = realTime;
                else
                    duration = atoi(durationStr);

                // Controllers
                if (duration < 1 * SECOND)
                    duration = 1 * SECOND;

                if (duration > 24 * DAY)
                    duration = 24 * DAY; // Max 45 days, client sided -- Edit: Seems to be max 24 days core sided

                if (Aura* aura = target->GetAura(SPELL_DESERTER_PVE))
                    aura->SetDuration(duration * IN_MILLISECONDS);

                std::string durationMsg; // duration string back to GM
                durationMsg = secsToTimeString(duration);
                
                handler->PSendSysMessage("You have applied PvE Deserter to %s\nDuration of Deserter: %s", target->GetName().c_str(), durationMsg.c_str());
            }
        }
        else // Default timer of 30 minutes
        {
            if (target)
                target->AddAura(SPELL_DESERTER_PVE, target);

            handler->PSendSysMessage("You have applied PvE Deserter to %s", target->GetName().c_str());
        }

        return true;
    }

    static bool HandlePvEDeserterRemoveCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();

        if (!target)
        {
            handler->PSendSysMessage("You must select a target");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 SPELL_DESERTER_PVE = 71041;

        if (Aura* aura = target->GetAura(SPELL_DESERTER_PVE))
        {
            target->RemoveAurasDueToSpell(SPELL_DESERTER_PVE);
            handler->PSendSysMessage("You removed PvE Deserter from %s", target->GetName().c_str());
        }
        else
        {
            handler->PSendSysMessage("Target doesn't have PvE Deserter debuff");
            handler->SetSentErrorMessage(true);
            return false;
        }
        return true;
    }
	
	static bool HandleUtilityCustomizeCommand(ChatHandler* handler, char const* /*args*/)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
        stmt->setUInt16(0, uint16(AT_LOGIN_CUSTOMIZE));
        handler->PSendSysMessage("Relog to customize your character.");
        handler->GetSession()->GetPlayer()->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
        stmt->setUInt32(1, handler->GetSession()->GetPlayer()->GetGUID());
        CharacterDatabase.Execute(stmt);
        return true;
    }

    static bool HandleUtilityChangeFactionCommand(ChatHandler* handler, char const* /*args*/)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
        stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_FACTION));
        handler->PSendSysMessage("Relog to change your faction.");
        handler->GetSession()->GetPlayer()->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
        stmt->setUInt32(1, handler->GetSession()->GetPlayer()->GetGUID());
        CharacterDatabase.Execute(stmt);
        return true;
    }

    static bool HandleUtilityChangeRaceCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* session = handler->GetSession()->GetPlayer();
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
        stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_RACE));
        handler->PSendSysMessage("Relog to change your race.");
        session->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        stmt->setUInt32(1, session->GetGUIDLow());
        CharacterDatabase.Execute(stmt);
        return true;
    }

    static bool HandleGPSCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* object = NULL;
        if (*args)
        {
            uint64 guid = handler->extractGuidFromLink((char*)args);
            if (guid)
                object = (WorldObject*)ObjectAccessor::GetObjectByTypeMask(*handler->GetSession()->GetPlayer(), guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);

            if (!object)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            object = handler->getSelectedUnit();

            if (!object)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        CellCoord cellCoord = Trinity::ComputeCellCoord(object->GetPositionX(), object->GetPositionY());
        Cell cell(cellCoord);

        uint32 zoneId, areaId;
        object->GetZoneAndAreaId(zoneId, areaId);

        MapEntry const* mapEntry = sMapStore.LookupEntry(object->GetMapId());
        AreaTableEntry const* zoneEntry = GetAreaEntryByAreaID(zoneId);
        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(areaId);

        float zoneX = object->GetPositionX();
        float zoneY = object->GetPositionY();

        Map2ZoneCoordinates(zoneX, zoneY, zoneId);

        Map const* map = object->GetMap();
        float groundZ = map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), true, MAX_FALL_DISTANCE);
        float floorZ = std::max<float>(map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ()),
                                       map->GetHeight(object->GetPhaseMask(), object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), true, MAX_FALL_DISTANCE));
        GridCoord gridCoord = Trinity::ComputeGridCoord(object->GetPositionX(), object->GetPositionY());

        // 63? WHY?
        int gridX = 63 - gridCoord.x_coord;
        int gridY = 63 - gridCoord.y_coord;

        uint32 haveMap = Map::ExistMap(object->GetMapId(), gridX, gridY) ? 1 : 0;
        uint32 haveVMap = Map::ExistVMap(object->GetMapId(), gridX, gridY) ? 1 : 0;

        if (haveVMap)
        {
            if (map->IsOutdoors(object->GetPositionX(), object->GetPositionY(), object->GetPositionZ()))
                handler->PSendSysMessage("You are outdoors");
            else
                handler->PSendSysMessage("You are indoors");
        }
        else
            handler->PSendSysMessage("no VMAP available for area info");

        handler->PSendSysMessage(LANG_MAP_POSITION,
            object->GetMapId(), (mapEntry ? mapEntry->name : "<unknown>"),
            zoneId, (zoneEntry ? zoneEntry->area_name : "<unknown>"),
            areaId, (areaEntry ? areaEntry->area_name : "<unknown>"),
            object->GetPhaseMask(),
            object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), object->GetOrientation(),
            cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), object->GetInstanceId(),
                                 zoneX, zoneY, groundZ, floorZ, haveMap, haveVMap);

        LiquidData liquidStatus;
        ZLiquidStatus status = map->getLiquidStatus(object->GetPositionX(), object->GetPositionY(), object->GetPositionZ(), MAP_ALL_LIQUIDS, &liquidStatus);

        if (status)
            handler->PSendSysMessage(LANG_LIQUID_STATUS, liquidStatus.level, liquidStatus.depth_level, liquidStatus.entry, liquidStatus.type_flags, status);



        if (handler)
            if (auto sess = handler->GetSession())
                if (auto target = handler->getSelectedUnit())
                    if (auto player = sess->GetPlayer())
                        {
                            if (!target)
                            {
                                handler->SendSysMessage(LANG_SELECT_CREATURE);
                                handler->SetSentErrorMessage(true);
                                return false;
                            }

                            std::ostringstream ostream;
                            LocaleConstant loc_idx = handler->GetSession()->GetSessionDbLocaleIndex();
                            WorldPacket data(SMSG_MESSAGECHAT, 200);
                            if (auto t = target->GetTransport())
                            {
                                Position pos{ *target };
                                t->CalculatePassengerOffset(pos.m_positionX, pos.m_positionY, pos.m_positionZ, pos.m_orientation);

                                ostream << "\nHas Transport Offsets:";
                                ostream << "\ndX: " << pos.m_positionX;
                                ostream << ", dY: " << pos.m_positionY;
                                ostream << ", dZ: " << pos.m_positionZ;
                                ostream << ", dOri: " << pos.m_orientation;
                                auto fullmsg{ ostream.str() };
                                ChatHandler(sess).PSendSysMessage("%s", fullmsg.c_str());
                            }
                        }

        return true;
    }

    static bool HandleAuraCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
            Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, target, target);

        return true;
    }

    static bool HandleUnAuraCommand(ChatHandler* handler, char const* args)
    {
        Unit* target = handler->getSelectedUnit();
        if (!target)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string argstr = args;
        if (argstr == "all")
        {
            target->RemoveAllAuras();
            return true;
        }

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellId = handler->extractSpellIdFromLink((char*)args);
        if (!spellId)
            return false;

        target->RemoveAurasDueToSpell(spellId);

        return true;
    }
    // Teleport to Player
    static bool HandleAppearCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        Player* _player = handler->GetSession()->GetPlayer();
        if (target == _player || targetGuid == _player->GetGUID())
        {
            handler->SendSysMessage(LANG_CANT_TELEPORT_SELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            if (auto s = target->GetSession())
                if (s->HasAccessFlag(ACCOUNT_FLAG_HEAD_GAMEMASTER))
                    return false;

            std::string chrNameLink = handler->playerLink(targetName);

            Map* map = target->GetMap();
            if (map->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!_player->isGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, chrNameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (_player->GetBattlegroundId() && _player->GetBattlegroundId() != target->GetBattlegroundId())
                    _player->LeaveBattleground(false); // Note: should be changed so _player gets no Deserter debuff

                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                _player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
                // remember current position as entry point for return at bg end teleportation
                if (!_player->GetMap()->IsBattlegroundOrArena())
                    _player->SetBattlegroundEntryPoint();
            }
            else if (map->IsDungeon())
            {
                // we have to go to instance, and can go to player only if:
                //   1) we are in his group (either as leader or as member)
                //   2) we are not bound to any group and have GM mode on
                if (_player->GetGroup())
                {
                    // we are in group, we can go only if we are in the player group
                    if (_player->GetGroup() != target->GetGroup())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, chrNameLink.c_str());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                }
                else
                {
                    // we are not in group, let's verify our GM mode
                    if (!_player->isGameMaster())
                    {
                        handler->PSendSysMessage(LANG_CANNOT_GO_TO_INST_GM, chrNameLink.c_str());
                        handler->SetSentErrorMessage(true);
                        return false;
                    }
                    _player->UnbindInstance(target->GetMapId(), target->GetDungeonDifficulty(), false, false);
                    _player->UnbindInstance(target->GetMapId(), target->GetDungeonDifficulty(), false, true);
                }

                // if the player or the player's group is bound to another instance
                // the player will not be bound to another one
                if (_player->isGameMaster() && target->GetGroup() && target->GetGroup()->IsNonPermanentLFGGroup())
                {
                    _player->UnbindInstance(target->GetMapId(), target->GetDungeonDifficulty(), false, false);
                    _player->UnbindInstance(target->GetMapId(), target->GetDungeonDifficulty(), false, true);
                }

                InstancePlayerBind* bind = _player->GetBoundInstance(target->GetMapId(), target->GetDifficulty(map->IsRaid()), target->GetGroup() ? target->GetGroup()->IsNonPermanentLFGGroup() : false);
                if (!bind)
                {
                    Group* group = _player->GetGroup();
                    // if no bind exists, create a solo bind
                    InstanceGroupBind* gBind = group ? group->GetBoundInstance(target, group->IsNonPermanentLFGGroup()) : NULL;                // if no bind exists, create a solo bind

                    if (!gBind)
                        if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
                            _player->BindToInstance(save, !save->CanReset());
                }

                if (map->IsRaid())
                    _player->SetRaidDifficulty(target->GetRaidDifficulty());
                else
                    _player->SetDungeonDifficulty(target->GetDungeonDifficulty());
            }

            handler->PSendSysMessage(LANG_APPEARING_AT, chrNameLink.c_str());

            // stop flight if need
            if (_player->isInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            // to point to see at target with same orientation
            float x, y, z;
            target->GetContactPoint(_player, x, y, z);

            _player->TeleportTo(target->GetMapId(), x, y, z, _player->GetAngle(target), TELE_TO_GM_MODE);
            _player->SetPhaseMask(target->GetPhaseMask(), true);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            std::string nameLink = handler->playerLink(targetName);

            handler->PSendSysMessage(LANG_APPEARING_AT, nameLink.c_str());

            // to point where player stay (if loaded)
            float x, y, z, o;
            uint32 map;
            bool in_flight;
            if (!Player::LoadPositionFromDB(map, x, y, z, o, in_flight, targetGuid))
                return false;

            // stop flight if need
            if (_player->isInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                _player->SaveRecallPosition();

            _player->TeleportTo(map, x, y, z, _player->GetOrientation());
        }

        return true;
    }
    // Summon Player
    static bool HandleSummonCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        Player* _player = handler->GetSession()->GetPlayer();
        if (target == _player || targetGuid == _player->GetGUID())
        {
            handler->PSendSysMessage(LANG_CANT_TELEPORT_SELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target)
        {
            std::string nameLink = handler->playerLink(targetName);
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            if (target->IsBeingTeleported())
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, nameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            Map* map = handler->GetSession()->GetPlayer()->GetMap();

            if (map->IsBattlegroundOrArena())
            {
                // only allow if gm mode is on
                if (!_player->isGameMaster())
                {
                    handler->PSendSysMessage(LANG_CANNOT_GO_TO_BG_GM, nameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                // if both players are in different bgs
                else if (target->GetBattlegroundId() && handler->GetSession()->GetPlayer()->GetBattlegroundId() != target->GetBattlegroundId())
                    target->LeaveBattleground(false); // Note: should be changed so target gets no Deserter debuff

                // all's well, set bg id
                // when porting out from the bg, it will be reset to 0
                target->SetBattlegroundId(handler->GetSession()->GetPlayer()->GetBattlegroundId(), handler->GetSession()->GetPlayer()->GetBattlegroundTypeId());
                // remember current position as entry point for return at bg end teleportation
                if (!target->GetMap()->IsBattlegroundOrArena())
                    target->SetBattlegroundEntryPoint();
            }
            else if (map->IsDungeon())
            {
                Map* destMap = target->GetMap();

                if (destMap->Instanceable() && destMap->GetInstanceId() != map->GetInstanceId())
                    target->UnbindInstance(map->GetId(), target->GetDungeonDifficulty(), true);

                // we are in an instance, and can only summon players in our group with us as leader
                if (!handler->GetSession()->GetPlayer()->GetGroup() || !target->GetGroup() ||
                    (target->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()) ||
                    (handler->GetSession()->GetPlayer()->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()))
                    // the last check is a bit excessive, but let it be, just in case
                {
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, nameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), "");
            if (handler->needReportToTarget(target))
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_SUMMONED_BY, handler->playerLink(_player->GetName()).c_str());

            // stop flight if need
            if (target->isInFlight())
            {
                target->GetMotionMaster()->MovementExpired();
                target->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                target->SaveRecallPosition();

            // before GM
            float x, y, z;
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, target->GetObjectSize());
            target->TeleportTo(handler->GetSession()->GetPlayer()->GetMapId(), x, y, z, target->GetOrientation());
            target->SetPhaseMask(handler->GetSession()->GetPlayer()->GetPhaseMask(), true);
        }
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            std::string nameLink = handler->playerLink(targetName);

            handler->PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), handler->GetTrinityString(LANG_OFFLINE));

            // in point where GM stay
            SQLTransaction dummy;
            Player::SavePositionInDB(WorldLocation(handler->GetSession()->GetPlayer()->GetMapId(),
                handler->GetSession()->GetPlayer()->GetPositionX(),
                handler->GetSession()->GetPlayer()->GetPositionY(),
                handler->GetSession()->GetPlayer()->GetPositionZ(),
                handler->GetSession()->GetPlayer()->GetOrientation()),
                handler->GetSession()->GetPlayer()->GetZoneId(),
                targetGuid, dummy);
        }

        return true;
    }
    // Summon group of player
    static bool HandleGroupSummonCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        Group* group = target->GetGroup();

        std::string nameLink = handler->GetNameLink(target);

        if (!group)
        {
            handler->PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* gmMap = handler->GetSession()->GetPlayer()->GetMap();
        bool toInstance = gmMap->Instanceable();

        // we are in instance, and can summon only player in our group with us as lead
        if (toInstance && (
            !handler->GetSession()->GetPlayer()->GetGroup() || (group->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID()) ||
            (handler->GetSession()->GetPlayer()->GetGroup()->GetLeaderGUID() != handler->GetSession()->GetPlayer()->GetGUID())))
            // the last check is a bit excessive, but let it be, just in case
        {
            handler->SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
            handler->SetSentErrorMessage(true);
            return false;
        }

        for (auto itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* player = itr->getSource();

            if (!player || player == handler->GetSession()->GetPlayer() || !player->GetSession())
                continue;

            // check online security
            if (handler->HasLowerSecurity(player, 0))
                return false;

            std::string plNameLink = handler->GetNameLink(player);

            if (player->IsBeingTeleported() == true)
            {
                handler->PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (toInstance)
            {
                Map* playerMap = player->GetMap();

                if (playerMap->Instanceable() && playerMap->GetInstanceId() != gmMap->GetInstanceId())
                {
                    // cannot summon from instance to instance
                    handler->PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, plNameLink.c_str());
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            handler->PSendSysMessage(LANG_SUMMONING, plNameLink.c_str(), "");
            if (handler->needReportToTarget(player))
                ChatHandler(player->GetSession()).PSendSysMessage(LANG_SUMMONED_BY, handler->GetNameLink().c_str());

            // stop flight if need
            if (player->isInFlight())
            {
                player->GetMotionMaster()->MovementExpired();
                player->CleanupAfterTaxiFlight();
            }
            // save only in non-flight case
            else
                player->SaveRecallPosition();

            // before GM
            float x, y, z;
            handler->GetSession()->GetPlayer()->GetClosePoint(x, y, z, player->GetObjectSize());
            player->TeleportTo(handler->GetSession()->GetPlayer()->GetMapId(), x, y, z, player->GetOrientation());
        }

        return true;
    }

    static bool HandleCommandsCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->ShowHelpForCommand(handler->getCommandTable(), "");
        return true;
    }

    static bool HandleDieCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* target = handler->getSelectedUnit();

        if (!target || !handler->GetSession()->GetPlayer()->GetSelection())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if (handler->HasLowerSecurity((Player*)target, 0, false))
                return false;
        }

        if (target->isAlive())
        {
            if (sWorld->getBoolConfig(CONFIG_DIE_COMMAND_MODE))
                handler->GetSession()->GetPlayer()->Kill(target, false, false);
            else
                handler->GetSession()->GetPlayer()->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        return true;
    }

    static bool HandleReviveCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid))
            return false;

        if (target)
        {
            target->ResurrectPlayer(!AccountMgr::IsPlayerAccount(target->GetSession()->GetSecurity()) ? 1.0f : 0.5f);
            target->SpawnCorpseBones();
            target->SaveToDB();
        }
        else
            // will resurrected at login without corpse
            sObjectAccessor->ConvertCorpseForPlayer(targetGuid);

        return true;
    }
    static bool HandleSetRevealToCommand(ChatHandler* handler, char const* args)
    {
        Player* gm = handler->GetSession()->GetPlayer();
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (!target)
            if (WorldSession* session = sWorld->FindSession(accountId))
                target = session->GetPlayer();

        // must have strong lesser security level
        if (handler->HasLowerSecurity(target, targetGuid, false))
            return false;

        if (target)
        {
            gm->SetIsolatedRevealTo(target->GetGUID());
            gm->UpdateObjectVisibility(true);
            handler->PSendSysMessage("You have revealed yourself to %s. You will only ever be revealed to one player at a time.", target->GetName().c_str());
        }

        return true;
    }

    static bool HandleStartSniffingCommand(ChatHandler* handler, char const* args)
    {
        Player* gm = handler->GetSession()->GetPlayer();
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (WorldSession* target_session = sWorld->FindSession(accountId))
            if (auto gm_session = handler->GetSession())
            {

                if (!target)
                    target = target_session->GetPlayer();

                // must have strong lesser security level
                if (handler->HasLowerSecurity(target, targetGuid, false))
                    return false;

                if (target)
                    {
                        if (WorldSession* current_sniff = sWorld->FindSession(gm_session->GetSniffing()))
                            current_sniff->RemoveSniffer(gm_session->GetAccountId());

                        gm_session->StartSniffing(target_session->GetAccountId());
                        handler->PSendSysMessage("You have begun sniffing packets from %s's account. This will continue through character changes. you may sniff only one session at a time.", target->GetName().c_str());
                    }
            }


        return true;
    }

    static bool HandleStopSniffingCommand(ChatHandler* handler, char const* args)
    {
        if (auto gm_session = handler->GetSession())
        if (auto gm = gm_session->GetPlayer())
        {
            if (auto acc = gm_session->GetSniffing())
                    if (auto session = sWorld->FindSession(acc))
                        {
                            session->RemoveSniffer(gm_session->GetAccountId());
                            gm_session->StopSniffing();
                            handler->PSendSysMessage("You have stopped all sniffing.");
                        }
                    else handler->PSendSysMessage("Could not find session being sniffed.");
                else handler->PSendSysMessage("Could not find player to sniff connected to account %u.", acc);

            gm_session->StopSniffing();
        }

        return true;
    }

    static bool HandleTestCodeCommand(ChatHandler* handler, char const* args)
    {
        if (auto gm_session = handler->GetSession())
            if (auto gm = gm_session->GetPlayer())
            {
                gm_session->SendNameQueryOpcode(gm->GetGUID());
            }

        return true;
    }

    static bool HandleReviveUnstuckCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid))
            return false;

        if (target)
        {
            target->ResurrectPlayer(!AccountMgr::IsPlayerAccount(target->GetSession()->GetSecurity()) ? 1.0f : 0.5f);
            target->SpawnCorpseBones();
            target->TeleportTo(target->m_homebindMapId, target->m_homebindX, target->m_homebindY, target->m_homebindZ, target->GetOrientation());
            target->SaveToDB();
        }
        else
        {
             // will resurrected at login without corpse
            sObjectAccessor->ConvertCorpseForPlayer(targetGuid);

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_HOMEBIND);
            stmt->setUInt32(0, targetGuid);
            PreparedQueryResult resultDB = CharacterDatabase.Query(stmt);

            if (resultDB)
            {
                Field* fieldsDB = resultDB->Fetch();
                WorldLocation loc(fieldsDB[0].GetUInt16(), fieldsDB[2].GetFloat(), fieldsDB[3].GetFloat(), fieldsDB[4].GetFloat(), 0.0f);
                uint32 zoneId = fieldsDB[1].GetUInt16();

                SQLTransaction dummy;
                Player::SavePositionInDB(loc, zoneId, targetGuid, dummy);
            }
        }

        return true;
    }

    static bool HandleDismountCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // If player is not mounted, so go out :)
        if (!player->IsMounted())
        {
            handler->SendSysMessage(LANG_CHAR_NON_MOUNTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isInFlight())
        {
            handler->SendSysMessage(LANG_YOU_IN_FLIGHT);
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->Dismount();
        player->RemoveAurasByType(SPELL_AURA_MOUNTED);
        return true;
    }

    static bool HandleGUIDCommand(ChatHandler* handler, char const* /*args*/)
    {
        uint64 guid = handler->GetSession()->GetPlayer()->GetSelection();

        if (guid == 0)
        {
            handler->SendSysMessage(LANG_NO_SELECTION);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_OBJECT_GUID, GUID_LOPART(guid), GUID_HIPART(guid));
        return true;
    }

    static bool HandleHelpCommand(ChatHandler* handler, char const* args)
    {
        char const* cmd = strtok((char*)args, " ");
        if (!cmd)
        {
            handler->ShowHelpForCommand(handler->getCommandTable(), "help");
            handler->ShowHelpForCommand(handler->getCommandTable(), "");
        }
        else
        {
            if (!handler->ShowHelpForCommand(handler->getCommandTable(), cmd))
                handler->SendSysMessage(LANG_NO_HELP_CMD);
        }

        return true;
    }
    // move item to other slot
    static bool HandleItemMoveCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* param1 = strtok((char*)args, " ");
        if (!param1)
            return false;

        char const* param2 = strtok(NULL, " ");
        if (!param2)
            return false;

        uint8 srcSlot = uint8(atoi(param1));
        uint8 dstSlot = uint8(atoi(param2));

        if (srcSlot == dstSlot)
            return true;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, srcSlot, true))
            return false;

        if (!handler->GetSession()->GetPlayer()->IsValidPos(INVENTORY_SLOT_BAG_0, dstSlot, false))
            return false;

        uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcSlot);
        uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstSlot);

        handler->GetSession()->GetPlayer()->SwapItem(src, dst);

        return true;
    }

    static bool HandleItemDeleteCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* param1 = strtok((char*)args, " ");
        if (!param1)
            return false;

        auto item_guid = uint64(atoi(param1));
        uint64 current_owner_guid{ 0 };


        /*
        char const* param2 = strtok(NULL, " ");
        if (!param2)
            return false;
        auto target_player_guid = uint64(atoi(param2));
        */

        //                                                  0           1               2               3               4               5           6           7           8           9                           10                  11              12          13                     
        if (auto result = CharacterDatabase.PQuery("SELECT  `guid`,  `itemEntry`,  `owner_guid`,  `creatorGuid`,  `giftCreatorGuid`,  `count`,  `duration`,  `charges`,  `flags`, LEFT(`enchantments`, 256),  `randomPropertyId`,  `durability`,  `playedTime`, LEFT(`text`, 256) FROM item_instance_buyback iib WHERE iib.guid IN (%u) UNION SELECT  `guid`,  `itemEntry`,  `owner_guid`,  `creatorGuid`,  `giftCreatorGuid`,  `count`,  `duration`,  `charges`,  `flags`, LEFT(`enchantments`, 256), `randomPropertyId`,  `durability`,  `playedTime`, LEFT(`text`, 256) FROM item_instance ii WHERE ii.guid IN(%u);", item_guid, item_guid))
        {
            do
            {
                if (auto fields2 = result->Fetch())
                {
                    current_owner_guid = fields2[2].GetUInt64();
                    auto current_entry{ fields2[1].GetUInt32() };

                    std::ostringstream ostream2{ "" };
                    if (auto player = ObjectAccessor::FindPlayer(current_owner_guid))
                        if (auto item = player->GetItemByGuid(uint64(item_guid)/*, true*/))
                        {
                            SQLTransaction trans = CharacterDatabase.BeginTransaction();
                            item->DeleteFromInventoryDB(trans, uint32(item->GetGUID()));
                            item->DeleteRefundDataFromDB(&trans);
                            //player->DestroyItemByGUID(item_guid, true);

                            CharacterDatabase.CommitTransaction(trans);

                        }
                        else handler->PSendSysMessage("Item GUID %u found in database for player %u, but the player is online and the item cannot be found on them.\nThe player has likely deleted the item since the last save.", item_guid, current_owner_guid);
                    else
                    {
                        //Player is not online, safe to do simple sql action.
                        CharacterDatabase.PQuery("DELETE FROM item_instance WHERE guid IN (%u);", item_guid);
                        CharacterDatabase.PQuery("DELETE FROM item_instance_buyback WHERE guid IN (%u);", item_guid);
                    }

                    ostream2 << "Item Deleted: " << "|cffffffff|Hitem:" << current_entry << ":0:0:0:0:0:0:0:85:0|h[" << sObjectMgr->GetItemTemplate(current_entry)->Name1 << "]|h|r";
                    ostream2 << " from player " << "|cffff0000|Harea:" << "|h" << current_owner_guid << "|h|r.";

                    auto resulting_string{ ostream2.str() };
                    auto log{ resulting_string.c_str() };
                    handler->PSendSysMessage("%s", log);
                        
                }
            } while (result->NextRow());

        }
        return true;
    }
    //HandleItemTransferCommand

    static bool HandleItemTransferCommand(ChatHandler* handler, char const* args)
    {
        return false;
    }

    static bool HandleCooldownCommand(ChatHandler* handler, char const* args)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string nameLink = handler->GetNameLink(target);

        if (!*args)
        {
            target->RemoveAllSpellCooldown();
            handler->PSendSysMessage(LANG_REMOVEALL_COOLDOWN, nameLink.c_str());
        }
        else
        {
            // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
            uint32 spellIid = handler->extractSpellIdFromLink((char*)args);
            if (!spellIid)
                return false;

            if (!sSpellMgr->GetSpellInfo(spellIid))
            {
                handler->PSendSysMessage(LANG_UNKNOWN_SPELL, target == handler->GetSession()->GetPlayer() ? handler->GetTrinityString(LANG_YOU) : nameLink.c_str());
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->RemoveSpellCooldown(spellIid, true);
            handler->PSendSysMessage(LANG_REMOVE_COOLDOWN, spellIid, target == handler->GetSession()->GetPlayer() ? handler->GetTrinityString(LANG_YOU) : nameLink.c_str());
        }
        return true;
    }

    static bool HandleGetDistanceCommand(ChatHandler* handler, char const* args)
    {
        WorldObject* obj = NULL;

        if (*args)
        {
            uint64 guid = handler->extractGuidFromLink((char*)args);
            if (guid)
                obj = (WorldObject*)ObjectAccessor::GetObjectByTypeMask(*handler->GetSession()->GetPlayer(), guid, TYPEMASK_UNIT|TYPEMASK_GAMEOBJECT);

            if (!obj)
            {
                handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }
        else
        {
            obj = handler->getSelectedUnit();

            if (!obj)
            {
                handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        handler->PSendSysMessage(LANG_DISTANCE, handler->GetSession()->GetPlayer()->GetDistance(obj), handler->GetSession()->GetPlayer()->GetDistance2d(obj), handler->GetSession()->GetPlayer()->GetExactDist(obj), handler->GetSession()->GetPlayer()->GetExactDist2d(obj));
        return true;
    }
    // Teleport player to last position
    static bool HandleRecallCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        if (target->IsBeingTeleported())
        {
            handler->PSendSysMessage(LANG_IS_TELEPORTED, handler->GetNameLink(target).c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        // stop flight if need
        if (target->isInFlight())
        {
            target->GetMotionMaster()->MovementExpired();
            target->CleanupAfterTaxiFlight();
        }

        target->TeleportTo(target->m_recallMap, target->m_recallX, target->m_recallY, target->m_recallZ, target->m_recallO);
        return true;
    }

    static bool HandleSaveCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // save GM account without delay and output message
        if (!AccountMgr::IsPlayerAccount(handler->GetSession()->GetSecurity()))
        {
            if (auto target = handler->getSelectedPlayer())
                target->SaveToDB();
            else
                player->SaveToDB();
            handler->SendSysMessage(LANG_PLAYER_SAVED);
            return true;
        }

        // save if the player has last been saved over 20 seconds ago
        uint32 saveInterval = sWorld->getIntConfig(CONFIG_INTERVAL_SAVE);
        if (saveInterval == 0 || (saveInterval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= saveInterval - 20 * IN_MILLISECONDS))
            player->SaveToDB();

        return true;
    }

    // Save all players in the world
    static bool HandleSaveAllCommand(ChatHandler* handler, char const* /*args*/)
    {
        sObjectAccessor->SaveAllPlayers();
        handler->SendSysMessage(LANG_PLAYERS_SAVED);
        return true;
    }

    // kick player
    static bool HandleKickPlayerCommand(ChatHandler* handler, char const* args)
    {
        Player* target = NULL;
        std::string playerName;
        if (!handler->extractPlayerTarget((char*)args, &target, NULL, &playerName))
            return false;

        if (handler->GetSession() && target == handler->GetSession()->GetPlayer())
        {
            handler->SendSysMessage(LANG_COMMAND_KICKSELF);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        if (sWorld->getBoolConfig(CONFIG_SHOW_KICK_IN_WORLD))
            sWorld->SendWorldText(LANG_COMMAND_KICKMESSAGE, playerName.c_str());
        else
            handler->PSendSysMessage(LANG_COMMAND_KICKMESSAGE, playerName.c_str());

        target->GetSession()->KickPlayer(".kick");

        return true;
    }

    static bool HandleUnstuckCommand(ChatHandler* handler, char const* args)
    {
        //No args required for players
        if (handler->GetSession() && AccountMgr::IsPlayerAccount(handler->GetSession()->GetSecurity()))
        {
            // 7355: "Stuck"
            if (auto player = handler->GetSession()->GetPlayer())
                player->CastSpell(player, 7355, false);
            return true;
        }

        if (!*args)
            return false;

        char* player_str = strtok((char*)args, " ");
        if (!player_str)
            return false;

        std::string location_str = "inn";
        if (char const* loc = strtok(NULL, " "))
            location_str = loc;

        Player* player = NULL;
        if (!handler->extractPlayerTarget(player_str, &player))
            return false;

        if (player->isInFlight() || player->isInCombat())
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(7355);
            if (!spellInfo)
                return false;

            if (auto caster = handler->GetSession()->GetPlayer())
                Spell::SendCastResult(caster, spellInfo, 0, SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW);

            return false;
        }

        if (location_str == "inn")
        {
            player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
            return true;
        }

        if (location_str == "graveyard")
        {
            player->RepopAtGraveyard();
            return true;
        }

        if (location_str == "startzone")
        {
            player->TeleportTo(player->GetStartPosition());
            return true;
        }

        //Not a supported argument
        return false;

    }

    static bool HandleLinkGraveCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char* px = strtok((char*)args, " ");
        if (!px)
            return false;

        uint32 graveyardId = uint32(atoi(px));

        uint32 team;

        char* px2 = strtok(NULL, " ");

        if (!px2)
            team = 0;
        else if (strncmp(px2, "horde", 6) == 0)
            team = HORDE;
        else if (strncmp(px2, "alliance", 9) == 0)
            team = ALLIANCE;
        else
            return false;

        WorldSafeLocsEntry const* graveyard = sWorldSafeLocsStore.LookupEntry(graveyardId);

        if (!graveyard)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, graveyardId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        uint32 zoneId = player->GetZoneId();

        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(zoneId);
        if (!areaEntry || areaEntry->zone !=0)
        {
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDWRONGZONE, graveyardId, zoneId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sObjectMgr->AddGraveYardLink(graveyardId, zoneId, team))
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDLINKED, graveyardId, zoneId);
        else
            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDALRLINKED, graveyardId, zoneId);

        return true;
    }

    static bool HandleNearGraveCommand(ChatHandler* handler, char const* args)
    {
        uint32 team;

        size_t argStr = strlen(args);

        if (!*args)
            team = 0;
        else if (strncmp((char*)args, "horde", argStr) == 0)
            team = HORDE;
        else if (strncmp((char*)args, "alliance", argStr) == 0)
            team = ALLIANCE;
        else
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        uint32 zone_id = player->GetZoneId();

        WorldSafeLocsEntry const* graveyard = sObjectMgr->GetClosestGraveYard(
            player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), team);

        if (graveyard)
        {
            uint32 graveyardId = graveyard->ID;

            GraveYardData const* data = sObjectMgr->FindGraveYardData(graveyardId, zone_id);
            if (!data)
            {
                handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDERROR, graveyardId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            team = data->team;

            std::string team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_NOTEAM);

            if (team == 0)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ANY);
            else if (team == HORDE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (team == ALLIANCE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            handler->PSendSysMessage(LANG_COMMAND_GRAVEYARDNEAREST, graveyardId, team_name.c_str(), zone_id);
        }
        else
        {
            std::string team_name;

            if (team == 0)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ANY);
            else if (team == HORDE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_HORDE);
            else if (team == ALLIANCE)
                team_name = handler->GetTrinityString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

            if (team == ~uint32(0))
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAVEYARDS, zone_id);
            else
                handler->PSendSysMessage(LANG_COMMAND_ZONENOGRAFACTION, zone_id, team_name.c_str());
        }

        return true;
    }

    static bool HandleShowAreaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 area = GetAreaFlagByAreaID(atoi((char*)args));
        int32 offset = area / 32;
        uint32 val = uint32((1 << (area % 32)));

        if (area<0 || offset >= PLAYER_EXPLORED_ZONES_SIZE)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 currFields = playerTarget->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
        playerTarget->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, uint32((currFields | val)));

        handler->SendSysMessage(LANG_EXPLORE_AREA);
        return true;
    }

    static bool HandleHideAreaCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 area = GetAreaFlagByAreaID(atoi((char*)args));
        int32 offset = area / 32;
        uint32 val = uint32((1 << (area % 32)));

        if (area < 0 || offset >= PLAYER_EXPLORED_ZONES_SIZE)
        {
            handler->SendSysMessage(LANG_BAD_VALUE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 currFields = playerTarget->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
        playerTarget->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, uint32((currFields ^ val)));

        handler->SendSysMessage(LANG_UNEXPLORE_AREA);
        return true;
    }

    static bool HandleAddItemCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 itemId = 0;

        if (args[0] == '[')                                        // [name] manual form
        {
            char const* itemNameStr = strtok((char*)args, "]");

            if (itemNameStr && itemNameStr[0])
            {
                std::string itemName = itemNameStr+1;
                WorldDatabase.EscapeString(itemName);

                PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_ITEM_TEMPLATE_BY_NAME);
                stmt->setString(0, itemName);
                PreparedQueryResult result = WorldDatabase.Query(stmt);

                if (!result)
                {
                    handler->PSendSysMessage(LANG_COMMAND_COULDNOTFIND, itemNameStr+1);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
                itemId = result->Fetch()->GetUInt32();
            }
            else
                return false;
        }
        else                                                    // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
        {
            char const* id = handler->extractKeyFromLink((char*)args, "Hitem");
            if (!id)
                return false;
            itemId = uint32(atol(id));
        }

        char const* ccount = strtok(NULL, " ");

        int32 count = 1;

        if (ccount)
            count = strtol(ccount, NULL, 10);

        if (count == 0)
            count = 1;

        Player* player = handler->GetSession()->GetPlayer();
        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
            playerTarget = player;

        TC_LOG_DEBUG("misc", handler->GetTrinityString(LANG_ADDITEM), itemId, count);

        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(itemId);
        if (!itemTemplate)
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Subtract
        if (count < 0)
        {
            playerTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, handler->GetNameLink(playerTarget).c_str());
            return true;
        }

        // Adding items
        uint32 noSpaceForCount = 0;

        // check space and find places
        ItemPosCountVec dest;
        InventoryResult msg = playerTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
            count -= noSpaceForCount;

        if (count == 0 || dest.empty())                         // can't add any
        {
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Item* item = playerTarget->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

        // remove binding (let GM give it to another player later)
        if (player == playerTarget)
            for (ItemPosCountVec::const_iterator itr = dest.begin(); itr != dest.end(); ++itr)
                if (Item* item1 = player->GetItemByPos(itr->pos))
                    item1->SetBinding(false);

        if (count > 0 && item)
        {
            player->SendNewItem(item, count, false, true);
            if (player != playerTarget)
                playerTarget->SendNewItem(item, count, true, false);
        }

        if (noSpaceForCount > 0)
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);

        return true;
    }

    static bool HandleAddItemSetCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        char const* id = handler->extractKeyFromLink((char*)args, "Hitemset"); // number or [name] Shift-click form |color|Hitemset:itemset_id|h[name]|h|r
        if (!id)
            return false;

        uint32 itemSetId = atol(id);

        // prevent generation all items with itemset field value '0'
        if (itemSetId == 0)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemSetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        Player* playerTarget = handler->getSelectedPlayer();
        if (!playerTarget)
            playerTarget = player;

        TC_LOG_DEBUG("misc", handler->GetTrinityString(LANG_ADDITEMSET), itemSetId);

        bool found = false;
        ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
        for (ItemTemplateContainer::const_iterator itr = its->begin(); itr != its->end(); ++itr)
        {
            if (itr->second.ItemSet == itemSetId)
            {
                found = true;
                ItemPosCountVec dest;
                InventoryResult msg = playerTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itr->second.ItemId, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = playerTarget->StoreNewItem(dest, itr->second.ItemId, true);

                    // remove binding (let GM give it to another player later)
                    if (player == playerTarget)
                        item->SetBinding(false);

                    player->SendNewItem(item, 1, false, true);
                    if (player != playerTarget)
                        playerTarget->SendNewItem(item, 1, true, false);
                }
                else
                {
                    player->SendEquipError(msg, NULL, NULL, itr->second.ItemId);
                    handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itr->second.ItemId, 1);
                }
            }
        }

        if (!found)
        {
            handler->PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemSetId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleBankCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->GetSession()->SendShowBank(handler->GetSession()->GetPlayer()->GetGUID());
        return true;
    }

    static bool HandleChangeWeather(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        // Weather is OFF
        if (!sWorld->getBoolConfig(CONFIG_WEATHER))
        {
            handler->SendSysMessage(LANG_WEATHER_DISABLED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // *Change the weather of a cell
        char const* px = strtok((char*)args, " ");
        char const* py = strtok(NULL, " ");

        if (!px || !py)
            return false;

        uint32 type = uint32(atoi(px));                         //0 to 3, 0: fine, 1: rain, 2: snow, 3: sand
        float grade = float(atof(py));                          //0 to 1, sending -1 is instand good weather

        Player* player = handler->GetSession()->GetPlayer();
        uint32 zoneid = player->GetZoneId();

        Weather* weather = WeatherMgr::FindWeather(zoneid);

        if (!weather)
            weather = WeatherMgr::AddWeather(zoneid);
        if (!weather)
        {
            handler->SendSysMessage(LANG_NO_WEATHER);
            handler->SetSentErrorMessage(true);
            return false;
        }

        weather->SetWeather(WeatherType(type), grade);

        return true;
    }


    static bool HandleMaxSkillCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* SelectedPlayer = handler->getSelectedPlayer();
        if (!SelectedPlayer)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // each skills that have max skill value dependent from level seted to current level max skill value
        SelectedPlayer->UpdateSkillsToMaxSkillsForLevel();
        return true;
    }

    static bool HandleSetSkillCommand(ChatHandler* handler, char const* args)
    {
        // number or [name] Shift-click form |color|Hskill:skill_id|h[name]|h|r
        char const* skillStr = handler->extractKeyFromLink((char*)args, "Hskill");
        if (!skillStr)
            return false;

        char const* levelStr = strtok(NULL, " ");
        if (!levelStr)
            return false;

        char const* maxPureSkill = strtok(NULL, " ");

        int32 skill = atoi(skillStr);
        if (skill <= 0)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 level = uint32(atol(levelStr));

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        SkillLineEntry const* skillLine = sSkillLineStore.LookupEntry(skill);
        if (!skillLine)
        {
            handler->PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
            handler->SetSentErrorMessage(true);
            return false;
        }

        std::string tNameLink = handler->GetNameLink(target);

        if (!target->GetSkillValue(skill))
        {
            handler->PSendSysMessage(LANG_SET_SKILL_ERROR, tNameLink.c_str(), skill, skillLine->name);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint16 max = maxPureSkill ? atol (maxPureSkill) : target->GetPureMaxSkillValue(skill);

        if (level <= 0 || level > max || max <= 0)
            return false;

        target->SetSkill(skill, target->GetSkillStep(skill), level, max);
        handler->PSendSysMessage(LANG_SET_SKILL, skill, skillLine->name, tNameLink.c_str(), level, max);

        return true;
    }
    /*
    static bool HandlePFlagsCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        if (auto s = handler->GetSession())
            if (auto target = s->GetPlayer())
            {
                char const* px = strtok((char*)args, " ");
                auto flags = int32(atoi(px));                         //0 to 3, 0: fine, 1: rain, 2: snow, 3: sand

                //uint32 flags = { uint32(atol(args)) };
                target->SetFlag_Force(PLAYER_FLAGS, flags);
                handler->PSendSysMessage("Your player Flags have been updated.");

            }
        return true;
    }
    */

    // show info of player
    static bool HandlePInfoCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;

        uint32 parseGUID = MAKE_NEW_GUID(atol((char*)args), 0, HIGHGUID_PLAYER);

        if (sObjectMgr->GetPlayerNameByGUID(parseGUID, targetName))
        {
            target = sObjectMgr->GetPlayerByLowGUID(parseGUID);
            targetGuid = parseGUID;
        }
        else if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accId            = 0;
        uint64 money            = 0;
        uint32 totalPlayerTime  = 0;
        uint8 level             = 0;
        uint32 latency          = 0;
        uint8 race;
        uint8 Class;
        int64 muteTime          = 0;
        int64 banTime           = -1;
        uint32 mapId;
        uint32 areaId;
        uint32 phase            = 0;
        uint32 current_masked_ip {0};

        // get additional information from Player object
        if (target)
        {
            // check online security
            if (handler->HasLowerSecurity(target, 0))
                return false;

            accId             = target->GetSession()->GetAccountId();
            current_masked_ip = target->GetSession()->GetMaskedIP();

            money             = target->GetMoney();
            totalPlayerTime   = target->GetTotalPlayedTime();
            level             = target->getLevel();
            latency           = target->GetSession()->GetLatency();
            if (latency == 0)
                latency = 69;
            race              = target->getRace();
            Class             = target->getClass();
            muteTime          = target->GetSession()->m_muteTime;
            mapId             = target->GetMapId();
            areaId            = target->GetAreaId();
            phase             = target->GetPhaseMask();
        }
        // get additional information from DB
        else
        {
            // check offline security
            if (handler->HasLowerSecurity(NULL, targetGuid))
                return false;

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_PINFO);
            stmt->setUInt32(0, GUID_LOPART(targetGuid));
            PreparedQueryResult result = CharacterDatabase.Query(stmt);

            if (!result)
                return false;

            Field* fields     = result->Fetch();
            totalPlayerTime   = fields[0].GetUInt32();
            level             = fields[1].GetUInt8();
            money             = fields[2].GetUInt64();
            accId             = fields[3].GetUInt32();
            race              = fields[4].GetUInt8();
            Class             = fields[5].GetUInt8();
            mapId             = fields[6].GetUInt16();
            areaId            = fields[7].GetUInt16();
        }

        std::string userName    = handler->GetTrinityString(LANG_ERROR);
        std::string eMail       = handler->GetTrinityString(LANG_ERROR);
        std::string lastIp      = handler->GetTrinityString(LANG_ERROR);
        uint32 security         = 0;
        std::string lastLogin   = handler->GetTrinityString(LANG_ERROR);
        std::string op_system = "offline";

        if (target)
        if (auto s_target = target->GetSession())
            op_system = s_target->GetOperatingSystem();

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PINFO);
        stmt->setInt32(0, int32(realmID));
        stmt->setUInt32(1, accId);
        PreparedQueryResult result = LoginDatabase.Query(stmt);

        if (result)
        {
            Field* fields = result->Fetch();
            userName      = "-";

            eMail = fields[2].GetString();

            if (auto s = handler->GetSession())
            {
                if (s->GetSecurity() >= SEC_GAMEMASTER)
                    userName = fields[0].GetString();

                if (eMail.empty() || s->GetSecurity() < SEC_GAMEMASTER)
                    eMail = "-";
            }

            security      = fields[1].GetUInt8();



            muteTime      = fields[5].GetUInt64();


            if (!handler->GetSession() || handler->GetSession()->GetSecurity() >= AccountTypes(security))
            {
                lastIp = fields[3].GetString();
                lastLogin = fields[4].GetString();

                uint32 ip = inet_addr(lastIp.c_str());
#if TRINITY_ENDIAN == BIGENDIAN
                EndianConvertReverse(ip);
#endif

                PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_IP2NATION_COUNTRY);

                stmt->setUInt32(0, ip);

                PreparedQueryResult result2 = WorldDatabase.Query(stmt);

                if (result2)
                {
                    Field* fields2 = result2->Fetch();
                    lastIp.append(" (");
                    lastIp.append(fields2[0].GetString());
                    lastIp.append(")");
                }
            }
            else
            {
                lastIp = "-";
                lastLogin = "-";
            }
        }

        std::string nameLink = handler->playerLink(targetName);
        std::string online_string{ "(Online)" };

        /*
        Player %s %s guid: %u, Account: %s ID: %u, Email: %s, GMLevel: %u, Last IP: %u, Last login: %s, Latency: %u
        */
        handler->PSendSysMessage(LANG_PINFO_ACCOUNT, 
            (target ? online_string.c_str() : handler->GetTrinityString(LANG_OFFLINE)), 
            nameLink.c_str(), 
            GUID_LOPART(targetGuid), 
            userName.c_str(), 
            accId, 
            eMail.c_str(), 
            security, 
            current_masked_ip, 
            lastLogin.c_str(), 
            latency);

        std::string bannedby = "unknown";
        std::string banreason = "";

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_PINFO_BANS);
        stmt->setUInt32(0, accId);
        PreparedQueryResult result2 = LoginDatabase.Query(stmt);
        if (!result2)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PINFO_BANS);
            stmt->setUInt32(0, GUID_LOPART(targetGuid));
            result2 = CharacterDatabase.Query(stmt);
        }

        if (result2)
        {
            Field* fields = result2->Fetch();
            banTime       = int64(fields[1].GetUInt64() ? 0 : fields[0].GetUInt32());
            bannedby      = fields[2].GetString();
            banreason     = fields[3].GetString();
        }

        if (muteTime > 0)
            handler->PSendSysMessage(LANG_PINFO_MUTE, secsToTimeString(muteTime - time(NULL), true).c_str());

        if (banTime >= 0)
            handler->PSendSysMessage(LANG_PINFO_BAN, banTime > 0 ? secsToTimeString(banTime - time(NULL), true).c_str() : "permanently", bannedby.c_str(), banreason.c_str());

        std::string raceStr, ClassStr;
        switch (race)
        {
            case RACE_HUMAN:
                raceStr = "Human";
                break;
            case RACE_ORC:
                raceStr = "Orc";
                break;
            case RACE_DWARF:
                raceStr = "Dwarf";
                break;
            case RACE_NIGHTELF:
                raceStr = "Night Elf";
                break;
            case RACE_UNDEAD_PLAYER:
                raceStr = "Undead";
                break;
            case RACE_TAUREN:
                raceStr = "Tauren";
                break;
            case RACE_GNOME:
                raceStr = "Gnome";
                break;
            case RACE_TROLL:
                raceStr = "Troll";
                break;
            case RACE_BLOODELF:
                raceStr = "Blood Elf";
                break;
            case RACE_DRAENEI:
                raceStr = "Draenei";
                break;
            case RACE_GOBLIN:
                raceStr = "Goblin";
                break;
            case RACE_WORGEN:
                raceStr = "Worgen";
                break;
        }

        switch (Class)
        {
            case CLASS_WARRIOR:
                ClassStr = "Warrior";
                break;
            case CLASS_PALADIN:
                ClassStr = "Paladin";
                break;
            case CLASS_HUNTER:
                ClassStr = "Hunter";
                break;
            case CLASS_ROGUE:
                ClassStr = "Rogue";
                break;
            case CLASS_PRIEST:
                ClassStr = "Priest";
                break;
            case CLASS_DEATH_KNIGHT:
                ClassStr = "Death Knight";
                break;
            case CLASS_SHAMAN:
                ClassStr = "Shaman";
                break;
            case CLASS_MAGE:
                ClassStr = "Mage";
                break;
            case CLASS_WARLOCK:
                ClassStr = "Warlock";
                break;
            case CLASS_DRUID:
                ClassStr = "Druid";
                break;
        }

        std::string timeStr = secsToTimeString(totalPlayerTime, true, true);
        uint64 gold = money /GOLD;
        uint64 silv = (money % GOLD) / SILVER;
        uint64 copp = (money % GOLD) % SILVER;
        handler->PSendSysMessage(LANG_PINFO_LEVEL, raceStr.c_str(), ClassStr.c_str(), timeStr.c_str(), level, gold, silv, copp);


        // Add map, zone, subzone and phase to output
        std::string areaName = "unk";
        std::string zoneName = "unk";

        MapEntry const* map = sMapStore.LookupEntry(mapId);

        AreaTableEntry const* area = GetAreaEntryByAreaID(areaId);
        if (area)
        {
            areaName = area->area_name;

            AreaTableEntry const* zone = GetAreaEntryByAreaID(area->zone);
            if (zone)
                zoneName = zone->area_name;
        }

        if (target)
        {
            if (!zoneName.empty())
                handler->PSendSysMessage(LANG_PINFO_MAP_ONLINE, map->name, zoneName.c_str(), areaName.c_str(), phase, op_system);
            else
                handler->PSendSysMessage(LANG_PINFO_MAP_ONLINE, map->name, areaName.c_str(), areaName.c_str(), phase, op_system);
        }
        else
           handler->PSendSysMessage(LANG_PINFO_MAP_OFFLINE, map ? map->name : "unk_map", areaName.c_str(), phase, op_system);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER_EXTENDED);
        stmt->setUInt32(0, GUID_LOPART(targetGuid));

        result = CharacterDatabase.Query(stmt);
        if (result)
        {
            Field* fields = result->Fetch();

            uint32 guildId = fields[0].GetUInt32();
            std::string guildName = fields[1].GetString();
            std::string guildRank = fields[2].GetString();
            std::string note = fields[3].GetString();
            std::string officeNote = fields[4].GetString();

            if (note.empty())
                note = "-";
            if (officeNote.empty())
                officeNote = "-";

            handler->PSendSysMessage(LANG_PINFO_GUILD_INFO, guildName.c_str(), guildId, guildRank.c_str(), note.c_str(), officeNote.c_str());
        }

        std::string flyingStr, canFlyStr, isSwimming, Mounted, CanFlyByServer, actiallyFlying, fallingForReal, CanHover, flag_falling, flag_swimming, flag_slowfall, flag_farfall, allowedFall, WalkingOnWater, CanWalkOnWater = "false";
        if (target == nullptr || !target)
            return true;
        if (target->IsFlying())
            flyingStr = "true";
        if (target->CanFly())
            canFlyStr = "true";
        if (target->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
            isSwimming = "true";
        if (target->IsMounted())
            Mounted = "true";
        if (target->IsCanFlybyServer())
            CanFlyByServer = "true";
        if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_MASK_MOVING_FLY))
            actiallyFlying = "true";
        if (target->IsFalling())
            fallingForReal = "true";
        if (target->m_movementInfo.HasExtraMovementFlag(uint16(MOVEMENTFLAG_HOVER)))
            CanHover = "true";
        if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING))
            WalkingOnWater = "true";
        handler->PSendSysMessage("Flying: %s - CanFly: %s - IsSwimming: %s - Mounted: %s - CanFlyByServer: %s, actuallyFlying: %s, falling: %s, Hovering: %s, WalkingOnWater: %s", flyingStr.c_str(), canFlyStr.c_str(), isSwimming.c_str(), Mounted.c_str(), CanFlyByServer.c_str(), actiallyFlying.c_str(), fallingForReal.c_str(), CanHover.c_str(), WalkingOnWater.c_str());


        if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_SLOW))
            flag_slowfall = "true";
        if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING))
            flag_falling = "true";
        if (target->m_movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR))
            flag_farfall = "true";
        if (target->IsAllowedToFall())
            allowedFall = "true";
        if (target->HasAuraType(AuraType::SPELL_AURA_WATER_WALK))
            CanWalkOnWater = "true";

        handler->PSendSysMessage("Movement flags: falling: %s, slowfall: %s, farfall: %s, allowedToFall: %s, CanWalkOnWater: %s", flag_falling.c_str(), flag_slowfall.c_str(), flag_farfall.c_str(), allowedFall.c_str(), CanWalkOnWater.c_str());

        return true;
    }

    static bool HandleRespawnCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        // accept only explicitly selected target (not implicitly self targeting case)
        Unit* target = handler->getSelectedUnit();
        if (player->GetSelection() && target)
        {
            if (target->GetTypeId() != TYPEID_UNIT || target->isPet())
            {
                handler->SendSysMessage(LANG_SELECT_CREATURE);
                handler->SetSentErrorMessage(true);
                return false;
            }

            if (target->isDead())
                target->ToCreature()->Respawn();
            return true;
        }

        CellCoord p(Trinity::ComputeCellCoord(player->GetPositionX(), player->GetPositionY()));
        Cell cell(p);
        cell.SetNoCreate();

        Trinity::RespawnDo u_do;
        Trinity::WorldObjectWorker<Trinity::RespawnDo> worker(player, u_do);

        TypeContainerVisitor<Trinity::WorldObjectWorker<Trinity::RespawnDo>, GridTypeMapContainer > obj_worker(worker);
        cell.Visit(p, obj_worker, *player->GetMap(), *player, player->GetGridActivationRange());

        return true;
    }
    // mute player for some times
    static bool HandleMuteCommand(ChatHandler* handler, char const* args)
    {
        char* nameStr;
        char* delayStr;
        handler->extractOptFirstArg((char*)args, &nameStr, &delayStr);
        if (!delayStr)
            return false;

        char const* muteReason = strtok(NULL, "\r");
        std::string muteReasonStr = "No reason";
        if (muteReason != NULL)
            muteReasonStr = muteReason;

        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget(nameStr, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (!target)
            if (WorldSession* session = sWorld->FindSession(accountId))
                target = session->GetPlayer();

        uint32 notSpeakTime = uint32(atoi(delayStr));

        // must have strong lesser security level
        if (handler->HasLowerSecurity (target, targetGuid, true))
            return false;

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);

        if (target)
        {
            // Target is online, mute will be in effect right away.
            int64 muteTime = time(NULL) + notSpeakTime * MINUTE;
            target->GetSession()->m_muteTime = muteTime;
            target->GetSession()->SetMuteType(uint8(MUTE_STATE_NORMAL_MUTE));
            stmt->setInt64(0, muteTime);
            stmt->setUInt8(1, 1);
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, notSpeakTime, muteReasonStr.c_str());
        }
        else
        {
            // Target is offline, mute will be in effect starting from the next login.
            int32 muteTime = -int32(notSpeakTime * MINUTE);
            stmt->setInt64(0, muteTime);
            stmt->setUInt8(1, 1);
            if (sWorld->RemoveSession(accountId))
            {
                //TC_LOG_ERROR("misc", "Account %u successfully disconnected from character selection.");
            }
            else
            {
                //TC_LOG_ERROR("misc", "Account %u not removed from character selection.");
            }
        }

        stmt->setUInt32(2, accountId);
        LoginDatabase.Execute(stmt);
        std::string nameLink = handler->playerLink(targetName);

        handler->PSendSysMessage(target ? LANG_YOU_DISABLE_CHAT : LANG_COMMAND_DISABLE_CHAT_DELAYED, nameLink.c_str(), notSpeakTime, muteReasonStr.c_str());

        sWorld->AcknowledgePlayerComplaintsForBanAccountWide(accountId, 0);
        return true;
    }

    // silently mute a player indefinitely
    static bool HandleShadowMuteCommand(ChatHandler* handler, char const* args)
    {
        char* nameStr;
        char* delayStr;
        handler->extractOptFirstArg((char*)args, &nameStr, &delayStr);

        char const* muteReason = strtok(NULL, "\r");
        std::string muteReasonStr = "No reason";
        if (muteReason != NULL)
            muteReasonStr = muteReason;

        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget(nameStr, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (!target)
            if (WorldSession* session = sWorld->FindSession(accountId))
                target = session->GetPlayer();

        delayStr = "-1";
        uint32 notSpeakTime = uint32(atoi(delayStr));

        // must have strong lesser security level
        if (handler->HasLowerSecurity(target, targetGuid, true))
            return false;

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);

        if (target)
        {
            // Target is online, mute will be in effect right away.
            int64 muteTime = time(NULL) + notSpeakTime * MINUTE;
            target->GetSession()->m_muteTime = muteTime;
            target->GetSession()->SetMuteType(uint8(MUTE_STATE_SHADOW_MUTE));

            stmt->setInt64(0, muteTime);
            stmt->setUInt8(1, 2);
        }
        else
        {
            // Target is offline, mute will be in effect starting from the next login.
            int32 muteTime = -int32(notSpeakTime * MINUTE);
            stmt->setInt64(0, muteTime);
            stmt->setUInt8(1, 2);
            if (sWorld->RemoveSession(accountId))
            {
                //TC_LOG_ERROR("misc", "Account %u successfully disconnected from character selection.");
            }
            else
            {
                //TC_LOG_ERROR("misc", "Account %u not removed from character selection.");
            }
        }

        stmt->setUInt32(2, accountId);
        LoginDatabase.Execute(stmt);
        std::string nameLink = handler->playerLink(targetName);

        handler->PSendSysMessage(target ? LANG_YOU_DISABLE_CHAT : LANG_COMMAND_DISABLE_CHAT_DELAYED, nameLink.c_str(), notSpeakTime, muteReasonStr.c_str());
        handler->PSendSysMessage("NOTE: This mute will be a shadow mute.");

        sWorld->AcknowledgePlayerComplaintsForBanAccountWide(accountId, 0);
        return true;
    }

    // unmute player
    static bool HandleUnmuteCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr->GetPlayerAccountIdByGUID(targetGuid);

        // find only player from same account if any
        if (!target)
            if (WorldSession* session = sWorld->FindSession(accountId))
                target = session->GetPlayer();

        // must have strong lesser security level
        if (handler->HasLowerSecurity (target, targetGuid, true))
            return false;

        if (target)
        {
            if (target->CanSpeak() && target->CanSpeak(true))
            {
                handler->SendSysMessage(LANG_CHAT_ALREADY_ENABLED);
                handler->SetSentErrorMessage(true);
                return false;
            }

            target->GetSession()->m_muteTime = 0;
            target->GetSession()->SetMuteType(uint8(MUTE_STATE_UNMUTED));
        }

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
        stmt->setInt64(0, 0);
        stmt->setUInt8(1, 0);
        stmt->setUInt32(2, accountId);
        LoginDatabase.Execute(stmt);

        if (target)
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOUR_CHAT_ENABLED);

        std::string nameLink = handler->playerLink(targetName);

        handler->PSendSysMessage(LANG_YOU_ENABLE_CHAT, nameLink.c_str());

        return true;
    }


    static bool HandleMovegensCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(LANG_MOVEGENS_LIST, (unit->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), unit->GetGUIDLow());

        MotionMaster* motionMaster = unit->GetMotionMaster();
        float x, y, z;
        motionMaster->GetDestination(x, y, z);

        for (uint8 i = 0; i < MAX_MOTION_SLOT; ++i)
        {
            MovementGenerator* movementGenerator = motionMaster->GetMotionSlot(i);
            if (!movementGenerator)
            {
                handler->SendSysMessage("Empty");
                continue;
            }

            switch (movementGenerator->GetMovementGeneratorType())
            {
                case IDLE_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_IDLE);
                    break;
                case RANDOM_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_RANDOM);
                    break;
                case WAYPOINT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_WAYPOINT);
                    break;
                case ANIMAL_RANDOM_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_ANIMAL_RANDOM);
                    break;
                case CONFUSED_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_CONFUSED);
                    break;
                case CHASE_MOTION_TYPE:
                {
                    Unit* target = NULL;
                    if (unit->GetTypeId() == TYPEID_PLAYER)
                        target = static_cast<ChaseMovementGenerator<Player> const*>(movementGenerator)->GetTarget();
                    else
                        target = static_cast<ChaseMovementGenerator<Creature> const*>(movementGenerator)->GetTarget();

                    if (!target)
                        handler->SendSysMessage(LANG_MOVEGENS_CHASE_NULL);
                    else if (target->GetTypeId() == TYPEID_PLAYER)
                        handler->PSendSysMessage(LANG_MOVEGENS_CHASE_PLAYER, target->GetName().c_str(), target->GetGUIDLow());
                    else
                        handler->PSendSysMessage(LANG_MOVEGENS_CHASE_CREATURE, target->GetName().c_str(), target->GetGUIDLow());
                    break;
                }
                case FOLLOW_MOTION_TYPE:
                {
                    Unit* target = NULL;
                    if (unit->GetTypeId() == TYPEID_PLAYER)
                        target = static_cast<FollowMovementGenerator<Player> const*>(movementGenerator)->GetTarget();
                    else
                        target = static_cast<FollowMovementGenerator<Creature> const*>(movementGenerator)->GetTarget();

                    if (!target)
                        handler->SendSysMessage(LANG_MOVEGENS_FOLLOW_NULL);
                    else if (target->GetTypeId() == TYPEID_PLAYER)
                        handler->PSendSysMessage(LANG_MOVEGENS_FOLLOW_PLAYER, target->GetName().c_str(), target->GetGUIDLow());
                    else
                        handler->PSendSysMessage(LANG_MOVEGENS_FOLLOW_CREATURE, target->GetName().c_str(), target->GetGUIDLow());
                    break;
                }
                case HOME_MOTION_TYPE:
                {
                    if (unit->GetTypeId() == TYPEID_UNIT)
                        handler->PSendSysMessage(LANG_MOVEGENS_HOME_CREATURE, x, y, z);
                    else
                        handler->SendSysMessage(LANG_MOVEGENS_HOME_PLAYER);
                    break;
                }
                case FLIGHT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_FLIGHT);
                    break;
                case POINT_MOTION_TYPE:
                {
                    handler->PSendSysMessage(LANG_MOVEGENS_POINT, x, y, z);
                    break;
                }
                case FLEEING_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_FEAR);
                    break;
                case DISTRACT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_DISTRACT);
                    break;
                case EFFECT_MOTION_TYPE:
                    handler->SendSysMessage(LANG_MOVEGENS_EFFECT);
                    break;
                default:
                    handler->PSendSysMessage(LANG_MOVEGENS_UNKNOWN, movementGenerator->GetMovementGeneratorType());
                    break;
            }
        }
        return true;
    }
    /*
    ComeToMe command REQUIRED for 3rd party scripting library to have access to PointMovementGenerator
    Without this function 3rd party scripting library will get linking errors (unresolved external)
    when attempting to use the PointMovementGenerator
    */
    static bool HandleComeToMeCommand(ChatHandler* handler, char const* args)
    {
        char const* newFlagStr = strtok((char*)args, " ");
        if (!newFlagStr)
            return false;

        Creature* caster = handler->getSelectedCreature();
        if (!caster)
        {
            handler->SendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        caster->GetMotionMaster()->MovePoint(0, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        //Call for creature group update
        if (caster->GetFormation() && caster->GetFormation()->getLeader() == caster)
            caster->GetFormation()->LeaderMoveTo(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

        return true;
    }

    static bool HandleDamageCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Unit* target = handler->getSelectedUnit();
        if (!target || !handler->GetSession()->GetPlayer()->GetSelection())
        {
            handler->SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if (handler->HasLowerSecurity((Player*)target, 0, false))
                return false;
        }

        if (!target->isAlive())
            return true;

        char* damageStr = strtok((char*)args, " ");
        if (!damageStr)
            return false;

        int32 damage_int = atoi((char*)damageStr);
        if (damage_int <= 0)
            return true;

        uint32 damage = damage_int;

        char* schoolStr = strtok((char*)NULL, " ");

        // flat melee damage without resistence/etc reduction
        if (!schoolStr)
        {
            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            if (target != handler->GetSession()->GetPlayer())
                handler->GetSession()->GetPlayer()->SendAttackStateUpdate (HITINFO_AFFECTS_VICTIM, target, 1, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_HIT, 0);
            return true;
        }

        uint32 school = schoolStr ? atoi((char*)schoolStr) : SPELL_SCHOOL_NORMAL;
        if (school >= MAX_SPELL_SCHOOL)
            return false;

        SpellSchoolMask schoolmask = SpellSchoolMask(1 << school);

        if (Unit::IsDamageReducedByArmor(schoolmask))
            damage = handler->GetSession()->GetPlayer()->CalcArmorReducedDamage(target, damage, NULL, BASE_ATTACK);

        char* spellStr = strtok((char*)NULL, " ");

        // melee damage by specific school
        if (!spellStr)
        {
            uint32 absorb = 0;
            uint32 resist = 0;

            handler->GetSession()->GetPlayer()->CalcAbsorbResist(target, schoolmask, SPELL_DIRECT_DAMAGE, damage, &absorb, &resist);

            if (damage <= absorb + resist)
                return true;

            damage -= absorb + resist;

            handler->GetSession()->GetPlayer()->DealDamageMods(target, damage, &absorb);
            handler->GetSession()->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, schoolmask, NULL, false);
            handler->GetSession()->GetPlayer()->SendAttackStateUpdate (HITINFO_AFFECTS_VICTIM, target, 1, schoolmask, damage, absorb, resist, VICTIMSTATE_HIT, 0);
            return true;
        }

        // non-melee damage

        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spellid = handler->extractSpellIdFromLink((char*)args);
        if (!spellid || !sSpellMgr->GetSpellInfo(spellid))
            return false;

        handler->GetSession()->GetPlayer()->SpellNonMeleeDamageLog(target, spellid, damage);
        return true;
    }

    static bool HandleCombatStopCommand(ChatHandler* handler, char const* args)
    {
        Player* gm = handler->GetSession()->GetPlayer();

        Player* target = NULL;
        Unit* target_unit = NULL;

        if (auto tar = gm->GetSelectedUnit())
        {
            target_unit = tar;

        }
        else
        {
            if (args && strlen(args) > 0)
            {
                target = sObjectAccessor->FindPlayerByName(args);
                if (!target)
                {
                    handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            if (!target)
            {
                if (!handler->extractPlayerTarget((char*)args, &target))
                    return false;
            }

            if (target)
            target_unit = target->ToUnit();
        }


        // check online security
        if (target)
        if (handler->HasLowerSecurity(target, 0))
            return false;

        if (!target_unit)
            return false;

        HostileRefManager r = target_unit->getHostileRefManager();

        std::stringstream GM_report;
        GM_report << "--- [PN] Threat table for: " << target_unit->GetName() << " ---";

        if (auto ref = r.getFirst())
        while (ref)
        {
            HostileReference* nextRef = ref->next();
            if (auto s = ref->getSource())
                if (auto o = s->getOwner())
                {
                    GM_report << "\n \nUnit type: " << o->GetTypeId() << ", Entry: " << o->GetEntry() << ", Active Threat: " << ref->getThreat() <<"f";
                    if (auto c = o->ToCreature())
                    {
                        GM_report <<"\nName: "<< c->GetNameForLocaleIdx(LOCALE_enUS) << ", DB GUID: " << c->GetDBTableGUIDLow() << "\nScript Name: " << c->GetScriptName();
                    }

                    if (auto owner = o->GetOwner())
                    {
                        GM_report << "\nHas Owner with entry:" << owner->GetEntry();
                        if (auto ownerCreature = owner->ToCreature())
                        {
                            GM_report << ", Owner's DB GUID:" << ownerCreature->GetDBTableGUIDLow();
                        }
                    }

                    if (auto t = o->ToTempSummon())
                    if (auto sum = t->GetSummoner())
                    {
                        GM_report << "\nIs a temp summon of (Entry) " << sum->GetEntry() << ", GUID: " << sum->GetGUIDLow();
                        if (auto ownerCreature = sum->ToCreature())
                        {
                            GM_report << ", Summoner DB GUID:" << ownerCreature->GetDBTableGUIDLow();
                        }
                    }

                }

            ref = nextRef;
        }

        GM_report << "\nAttackers list:";
        for (auto atk : target_unit->getAttackers())
            GM_report << "\nGUID: " << atk->GetGUID() << ", entry: " << atk->GetEntry() << ", Target GUID: " << (atk->getVictim() ? atk->getVictim()->GetGUID() : 0);

        sWorld->SendGMText(LANG_GM_BROADCAST, GM_report.str().c_str());

        target_unit->CombatStop();
        target_unit->getHostileRefManager().deleteReferences();
        return true;
    }

    static bool HandleRepairitemsCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        if (!handler->extractPlayerTarget((char*)args, &target))
            return false;

        // check online security
        if (handler->HasLowerSecurity(target, 0))
            return false;

        // Repair items
        target->DurabilityRepairAll(false, 0, false);

        handler->PSendSysMessage(LANG_YOU_REPAIR_ITEMS, handler->GetNameLink(target).c_str());
        if (handler->needReportToTarget(target))
            ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, handler->GetNameLink().c_str());

        return true;
    }

    // Send mail by command
    static bool HandleSendMailCommand(ChatHandler* handler, char const* args)
    {
        // format: name "subject text" "mail text"
        Player* target;
        uint64 targetGuid;
        std::string targetName;
        if (!handler->extractPlayerTarget((char*)args, &target, &targetGuid, &targetName))
            return false;

        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char const* msgSubject = handler->extractQuotedArg(tail1);
        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char const* msgText = handler->extractQuotedArg(tail2);
        if (!msgText)
            return false;

        // msgSubject, msgText isn't NUL after prev. check
        std::string subject = msgSubject;
        std::string text    = msgText;

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        //- TODO: Fix poor design
        SQLTransaction trans = CharacterDatabase.BeginTransaction();
        MailDraft(subject, text)
            .SendMailTo(trans, MailReceiver(target, GUID_LOPART(targetGuid)), sender);

        CharacterDatabase.CommitTransaction(trans);

        std::string nameLink = handler->playerLink(targetName);
        handler->PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
        return true;
    }
    // Send items by mail
    static bool HandleSendItemsCommand(ChatHandler* handler, char const* args)
    {
        // format: name "subject text" "mail text" item1[:count1] item2[:count2] ... item12[:count12]
        Player* receiver;
        uint64 receiverGuid;
        std::string receiverName;
        if (!handler->extractPlayerTarget((char*)args, &receiver, &receiverGuid, &receiverName))
            return false;

        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char const* msgSubject = handler->extractQuotedArg(tail1);
        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char const* msgText = handler->extractQuotedArg(tail2);
        if (!msgText)
            return false;

        // msgSubject, msgText isn't NUL after prev. check
        std::string subject = msgSubject;
        std::string text    = msgText;

        // extract items
        typedef std::pair<uint32, uint32> ItemPair;
        typedef std::list< ItemPair > ItemPairs;
        ItemPairs items;

        // get all tail string
        char* tail = strtok(NULL, "");

        // get from tail next item str
        while (char* itemStr = strtok(tail, " "))
        {
            // and get new tail
            tail = strtok(NULL, "");

            // parse item str
            char const* itemIdStr = strtok(itemStr, ":");
            char const* itemCountStr = strtok(NULL, " ");

            uint32 itemId = atoi(itemIdStr);
            if (!itemId)
                return false;

            ItemTemplate const* item_proto = sObjectMgr->GetItemTemplate(itemId);
            if (!item_proto)
            {
                handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            uint32 itemCount = itemCountStr ? atoi(itemCountStr) : 1;
            if (itemCount < 1 || (item_proto->MaxCount > 0 && itemCount > uint32(item_proto->MaxCount)))
            {
                handler->PSendSysMessage(LANG_COMMAND_INVALID_ITEM_COUNT, itemCount, itemId);
                handler->SetSentErrorMessage(true);
                return false;
            }

            while (itemCount > item_proto->GetMaxStackSize())
            {
                items.push_back(ItemPair(itemId, item_proto->GetMaxStackSize()));
                itemCount -= item_proto->GetMaxStackSize();
            }

            items.push_back(ItemPair(itemId, itemCount));

            if (items.size() > MAX_MAIL_ITEMS)
            {
                handler->PSendSysMessage(LANG_COMMAND_MAIL_ITEMS_LIMIT, MAX_MAIL_ITEMS);
                handler->SetSentErrorMessage(true);
                return false;
            }
        }

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        // fill mail
        MailDraft draft(subject, text);

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
        {
            if (Item* item = Item::CreateItem(itr->first, itr->second, handler->GetSession() ? handler->GetSession()->GetPlayer() : 0))
            {
                item->SaveToDB(trans);                               // save for prevent lost at next mail load, if send fail then item will deleted
                draft.AddItem(item);
            }
        }

        draft.SendMailTo(trans, MailReceiver(receiver, GUID_LOPART(receiverGuid)), sender);
        CharacterDatabase.CommitTransaction(trans);

        std::string nameLink = handler->playerLink(receiverName);
        handler->PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
        return true;
    }
    /// Send money by mail
    static bool HandleSendMoneyCommand(ChatHandler* handler, char const* args)
    {
        /// format: name "subject text" "mail text" money

        Player* receiver;
        uint64 receiverGuid;
        std::string receiverName;
        if (!handler->extractPlayerTarget((char*)args, &receiver, &receiverGuid, &receiverName))
            return false;
        char* tail1 = strtok(NULL, "");
        if (!tail1)
            return false;

        char* msgSubject = handler->extractQuotedArg(tail1);
        if (!msgSubject)
            return false;

        char* tail2 = strtok(NULL, "");
        if (!tail2)
            return false;

        char* msgText = handler->extractQuotedArg(tail2);
        if (!msgText)
            return false;

        char* moneyStr = strtok(NULL, "");
        int64 money = moneyStr ? atof(moneyStr) : 0;
        if (money <= 0)
            return false;

        // msgSubject, msgText isn't NUL after prev. check
        std::string subject = msgSubject;
        std::string text    = msgText;

        // from console show not existed sender
        MailSender sender(MAIL_NORMAL, handler->GetSession() ? handler->GetSession()->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

        SQLTransaction trans = CharacterDatabase.BeginTransaction();

        MailDraft(subject, text)
            .AddMoney(money)
            .SendMailTo(trans, MailReceiver(receiver, GUID_LOPART(receiverGuid)), sender);

        CharacterDatabase.CommitTransaction(trans);

        std::string nameLink = handler->playerLink(receiverName);
        handler->PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
        return true;
    }
    /// Send a message to a player in game
    static bool HandleSendMessageCommand(ChatHandler* handler, char const* args)
    {
        /// - Find the player
        Player* player;
        if (!handler->extractPlayerTarget((char*)args, &player))
            return false;

        char* msgStr = strtok(NULL, "");
        if (!msgStr)
            return false;

        ///- Check that he is not logging out.
        if (player->GetSession()->isLogingOut())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        /// - Send the message
        // Use SendAreaTriggerMessage for fastest delivery.
        player->GetSession()->SendAreaTriggerMessage("%s", msgStr);
        player->GetSession()->SendAreaTriggerMessage("|cffff0000[Message from administrator]:|r");

        // Confirmation message
        std::string nameLink = handler->GetNameLink(player);
        handler->PSendSysMessage(LANG_SENDMESSAGE, nameLink.c_str(), msgStr);

        return true;
    }

    static bool HandleCreatePetCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Creature* creatureTarget = handler->getSelectedCreature();

        if (!creatureTarget || creatureTarget->isPet() || creatureTarget->GetTypeId() == TYPEID_PLAYER)
        {
            handler->PSendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureTemplate const* creatrueTemplate = sObjectMgr->GetCreatureTemplate(creatureTarget->GetEntry());
        // Creatures with family 0 crashes the server
        if (!creatrueTemplate->family)
        {
            handler->PSendSysMessage("This creature cannot be tamed. (family id: 0).");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetPetGUID())
        {
            handler->PSendSysMessage("You already have a pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        PetSlot newslot = player->getSlotForNewPet();
        if (newslot == PET_SLOT_FULL_LIST)
        {
            handler->PSendSysMessage("Too Many Pets");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Everything looks OK, create new pet
        Pet* pet = new Pet(player, HUNTER_PET);
        if (!pet->CreateBaseAtCreature(creatureTarget))
        {
            delete pet;
            handler->PSendSysMessage("Error 1");
            return false;
        }

        creatureTarget->setDeathState(JUST_DIED);
        creatureTarget->RemoveCorpse();
        creatureTarget->SetHealth(0); // just for nice GM-mode view

        pet->SetUInt64Value(UNIT_FIELD_CREATEDBY, player->GetGUID());
        pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->getFaction());

        if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
        {
            TC_LOG_ERROR("misc", "InitStatsForLevel() in EffectTameCreature failed! Pet deleted.");
            handler->PSendSysMessage("Error 2");
            delete pet;
            return false;
        }

        // prepare visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel()-1);

        pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
        // this enables pet details window (Shift+P)
        pet->InitPetCreateSpells();
        pet->SetFullHealth();

        pet->GetMap()->AddToMap(pet->ToCreature());

        // visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

        player->SetMinion(pet, true, newslot);
        pet->SavePet(newslot);
        player->PetSpellInitialize();

        if(player->getClass() == CLASS_HUNTER)
            player->GetSession()->SendStablePet(0);

        return true;
    }

    static bool HandlePetLearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        Pet* pet = player->GetPet();

        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (!spellId || !sSpellMgr->GetSpellInfo(spellId))
            return false;

        // Check if pet already has it
        if (pet->HasSpell(spellId))
        {
            handler->PSendSysMessage("Pet already has spell: %u", spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Check if spell is valid
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pet->learnSpell(spellId);

        handler->PSendSysMessage("Pet has learned spell %u", spellId);
        return true;
    }

    static bool HandlePetUnlearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        Pet* pet = player->GetPet();
        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (pet->HasSpell(spellId))
            pet->removeSpell(spellId, false);
        else
            handler->PSendSysMessage("Pet doesn't have that spell");

        return true;
    }

    static bool HandleFreezeCommand(ChatHandler* handler, char const* args)
    {
        std::string name;
        Player* player;
        char const* TargetName = strtok((char*)args, " "); // get entered name
        if (!TargetName) // if no name entered use target
        {
            player = handler->getSelectedPlayer();
            if (player) //prevent crash with creature as target
            {
                name = player->GetName();
                normalizePlayerName(name);
            }
        }
        else // if name entered
        {
            name = TargetName;
            normalizePlayerName(name);
            player = sObjectAccessor->FindPlayerByName(name);
        }

        if (!player)
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
            return true;
        }

        if (player == handler->GetSession()->GetPlayer())
        {
            handler->SendSysMessage(LANG_COMMAND_FREEZE_ERROR);
            return true;
        }

        // effect
        if (player && (player != handler->GetSession()->GetPlayer()))
        {
            handler->PSendSysMessage(LANG_COMMAND_FREEZE, name.c_str());

            // stop combat + make player unattackable + duel stop + stop some spells


            player->applyGMFreeze(true);
            if (auto s = player->GetSession())
            {
                s->SetMuteType(MUTE_STATE_GM_FROZEN);
                PreparedStatement* stmt3 = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                // Target is online, mute will be in effect right away.
                int64 muteTime = time(NULL);
                stmt3->setInt64(0, muteTime);
                stmt3->setUInt8(1, 3);
                stmt3->setUInt32(2, s->GetAccountId());
                LoginDatabase.Execute(stmt3);
            }

            // if player class = hunter || warlock remove pet if alive
            if ((player->getClass() == CLASS_HUNTER) || (player->getClass() == CLASS_WARLOCK))
            {
                if (Pet* pet = player->GetPet())
                {
                    pet->SavePet(PET_SLOT_ACTUAL_PET_SLOT);
                     // not let dismiss dead pet
                     if (pet && pet->isAlive())
                        player->RemovePet(pet, PET_SLOT_OTHER_PET);
                }
            }

            /*
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(27792))
                Aura::TryRefreshStackOrCreate(spellInfo, MAX_EFFECT_MASK, player, player);
            */



            player->AddAura(27792, player);
            // save player
            player->SaveToDB();
        }

        return true;
    }

    static bool HandleUnFreezeCommand(ChatHandler* handler, char const*args)
    {
        std::string name;
        Player* player;
        char* targetName = strtok((char*)args, " "); // Get entered name

        if (targetName)
        {
            name = targetName;
            normalizePlayerName(name);
            player = sObjectAccessor->FindPlayerByName(name);
        }
        else // If no name was entered - use target
        {
            player = handler->getSelectedPlayer();
            if (player)
                name = player->GetName();
        }

        if (player)
        {

            handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());

            player->applyGMFreeze(false);
            // Save player

            if (auto s = player->GetSession())
            {
                s->SetMuteType(MUTE_STATE_UNMUTED);
                PreparedStatement* stmt3 = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
                int64 muteTime = time(NULL);
                stmt3->setInt64(0, muteTime);
                stmt3->setUInt8(1, 0);
                stmt3->setUInt32(2, s->GetAccountId());
                LoginDatabase.Execute(stmt3);
            }

            player->SaveToDB();
        }
        else
        {
            if (targetName)
            {
                // Check for offline players
                PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_GUID_BY_NAME);
                stmt->setString(0, name);
                PreparedQueryResult result = CharacterDatabase.Query(stmt);

                if (!result)
                {
                    handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                    return true;
                }

                // If player found: delete his freeze aura
                Field* fields = result->Fetch();
                uint32 lowGuid = fields[0].GetUInt32();

                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_AURA_FROZEN);
                stmt->setUInt32(0, lowGuid);
                CharacterDatabase.Execute(stmt);

                handler->PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());
                return true;
            }
            else
            {
                handler->SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                return true;
            }
        }

        return true;
    }

    static bool HandleListFreezeCommand(ChatHandler* handler, char const* /*args*/)
    {
        // Get names from DB
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_AURA_FROZEN);
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (!result)
        {
            handler->SendSysMessage(LANG_COMMAND_NO_FROZEN_PLAYERS);
            return true;
        }

        // Header of the names
        handler->PSendSysMessage(LANG_COMMAND_LIST_FREEZE);

        // Output of the results
        do
        {
            Field* fields = result->Fetch();
            std::string player = fields[0].GetString();
            handler->PSendSysMessage(LANG_COMMAND_FROZEN_PLAYERS, player.c_str());
        }
        while (result->NextRow());

        return true;
    }

    static bool HandleGroupLeaderCommand(ChatHandler* handler, char const* args)
    {
        Player* player = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* nameStr = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(nameStr, player, group, guid))
            if (group && group->GetLeaderGUID() != guid)
            {
                group->ChangeLeader(guid);
                group->SendUpdate();
            }

            return true;
    }

    static bool HandleGroupDisbandCommand(ChatHandler* handler, char const* args)
    {
        Player* player = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* nameStr = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(nameStr, player, group, guid))
            if (group)
                group->Disband();

        return true;
    }

    static bool HandleGroupRemoveCommand(ChatHandler* handler, char const* args)
    {
        Player* player = NULL;
        Group* group = NULL;
        uint64 guid = 0;
        char* nameStr = strtok((char*)args, " ");

        if (handler->GetPlayerGroupAndGUIDByName(nameStr, player, group, guid, true))
            if (group)
                group->RemoveMember(guid);

        return true;
    }

    static bool HandleGroupJoinCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* playerSource = NULL;
        Player* playerTarget = NULL;
        Group* groupSource = NULL;
        Group* groupTarget = NULL;
        uint64 guidSource = 0;
        uint64 guidTarget = 0;
        char* nameplgrStr = strtok((char*)args, " ");
        char* nameplStr = strtok(NULL, " ");

        if (handler->GetPlayerGroupAndGUIDByName(nameplgrStr, playerSource, groupSource, guidSource, true))
        {
            if (groupSource)
            {
                if (handler->GetPlayerGroupAndGUIDByName(nameplStr, playerTarget, groupTarget, guidTarget, true))
                {
                    if (!groupTarget && playerTarget->GetGroup() != groupSource)
                    {
                        if (!groupSource->IsFull())
                        {
                            groupSource->AddMember(playerTarget);
                            groupSource->BroadcastGroupUpdate();
                            handler->PSendSysMessage(LANG_GROUP_PLAYER_JOINED, playerTarget->GetName().c_str(), playerSource->GetName().c_str());
                            return true;
                        }
                        else
                        {
                            // group is full
                            handler->PSendSysMessage(LANG_GROUP_FULL);
                            return true;
                        }
                    }
                    else
                    {
                        // group is full or target player already in a group
                        handler->PSendSysMessage(LANG_GROUP_ALREADY_IN_GROUP, playerTarget->GetName().c_str());
                        return true;
                    }
                }
            }
            else
            {
                // specified source player is not in a group
                handler->PSendSysMessage(LANG_GROUP_NOT_IN_GROUP, playerSource->GetName().c_str());
                return true;
            }
        }

        return true;
    }

    static bool HandleGroupListCommand(ChatHandler* handler, char const* args)
    {
        Player* playerTarget;
        uint64 guidTarget;
        std::string nameTarget;

        uint32 parseGUID = MAKE_NEW_GUID(atol((char*)args), 0, HIGHGUID_PLAYER);

        if (sObjectMgr->GetPlayerNameByGUID(parseGUID, nameTarget))
        {
            playerTarget = ObjectAccessor::FindPlayer(parseGUID);
            guidTarget = parseGUID;
        }
        else if (!handler->extractPlayerTarget((char*)args, &playerTarget, &guidTarget, &nameTarget))
            return false;

        Group* groupTarget = NULL;
        if (playerTarget)
            groupTarget = playerTarget->GetGroup();

        if (!groupTarget)
        {
            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GROUP_MEMBER);
            stmt->setUInt32(0, guidTarget);
            PreparedQueryResult resultGroup = CharacterDatabase.Query(stmt);
            if (resultGroup)
                groupTarget = sGroupMgr->GetGroupByDbStoreId((*resultGroup)[0].GetUInt32());
        }

        if (groupTarget)
        {
            handler->PSendSysMessage(LANG_GROUP_TYPE, (groupTarget->isRaidGroup() ? "raid" : "party"));
            Group::MemberSlotList const& members = groupTarget->GetMemberSlots();
            for (Group::MemberSlotList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
            {
                Group::MemberSlot const& slot = *itr;

                std::string flags;
                if (slot.flags & MEMBER_FLAG_ASSISTANT)
                    flags = "Assistant";

                if (slot.flags & MEMBER_FLAG_MAINTANK)
                {
                    if (!flags.empty())
                        flags.append(", ");
                    flags.append("MainTank");
                }

                if (slot.flags & MEMBER_FLAG_MAINASSIST)
                {
                    if (!flags.empty())
                        flags.append(", ");
                    flags.append("MainAssist");
                }

                if (flags.empty())
                    flags = "None";

                Player* p = ObjectAccessor::FindPlayer((*itr).guid);
                const char* onlineState = (p && p->IsInWorld()) ? "online" : "offline";

                handler->PSendSysMessage(LANG_GROUP_PLAYER_NAME_GUID, slot.name.c_str(), onlineState,
                    GUID_LOPART(slot.guid), flags.c_str(), lfg::GetRolesString(slot.roles).c_str());
            }
        }
        else
            handler->PSendSysMessage(LANG_GROUP_NOT_IN_GROUP, nameTarget.c_str());

        return true;
    }

    static bool HandlePlayAllCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        uint32 soundId = atoi((char*)args);

        if (!sSoundEntriesStore.LookupEntry(soundId))
        {
            handler->PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        WorldPacket data(SMSG_PLAY_SOUND, 4);
        data << uint32(soundId) << handler->GetSession()->GetPlayer()->GetGUID();
        sWorld->SendGlobalMessage(&data);

        handler->PSendSysMessage(LANG_COMMAND_PLAYED_TO_ALL, soundId);
        return true;
    }

    static bool HandlePossessCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(unit, 530, true);
        return true;
    }

    static bool HandleUnPossessCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            unit = handler->GetSession()->GetPlayer();

        unit->RemoveCharmAuras();

        return true;
    }

    static bool HandleBindSightCommand(ChatHandler* handler, char const* /*args*/)
    {
        Unit* unit = handler->getSelectedUnit();
        if (!unit)
            return false;

        handler->GetSession()->GetPlayer()->CastSpell(unit, 6277, true);
        return true;
    }

    static bool HandleUnbindSightCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (player->isPossessing())
            return false;

        player->StopCastingBindSight();
        player->RemoveAurasDueToSpell(6277);
        return true;
    }

    static bool HandleWintradeCommand(ChatHandler* /*handler*/, char const* args)
    {
        return false;
        if (!*args)
            return false;

        uint32 minDmg = atoi((char*)args);
        std::map<std::string, uint32 > wintrade;

        QueryResult result;
        for (int cnt = 0; cnt < 4; cnt++)
        {
            switch (cnt)
            {
                case 0:
                    result = WorldDatabase.PQuery("SELECT winpl1,   COUNT(winpl1) from arena_log where windmg <= %u or loosedmg <= %u Group By winpl1", minDmg, minDmg);
                    break;
                case 1:
                    result = WorldDatabase.PQuery("SELECT winpl2,   COUNT(winpl2) from arena_log where windmg <= %u or loosedmg <= %u Group By winpl2", minDmg, minDmg);
                    break;
                case 2:
                    result = WorldDatabase.PQuery("SELECT loosepl1,   COUNT(loosepl1) from arena_log where windmg <= %u or loosedmg <= %u Group By loosepl1 ", minDmg, minDmg);
                    break;
                case 3:
                    result = WorldDatabase.PQuery("SELECT loosepl2,   COUNT(loosepl2) from arena_log where windmg <= %u or loosedmg <= %u Group By loosepl2", minDmg, minDmg);
                    break;
                default:
                    return false;
            }
            if (!result)
                return false;
            do
            {
                if (Field* fields = result->Fetch())
                {
                    std::string name = fields[0].GetString();
                    uint32 wintradeCount = fields[1].GetUInt32();
                    wintrade[fields[0].GetString()] += fields[1].GetUInt32();
                }
            } while (result->NextRow());
        }
        for (std::map<std::string, uint32 >::iterator it = wintrade.begin(); it != wintrade.end(); ++it)
        {
            std::string name = it->first;
            uint32 wintradeCount = it->second;
            WorldDatabase.DirectPExecute("REPLACE INTO arena_filter VALUES('%s', %u)", name.c_str(), wintradeCount);
        }
        return true;
    }

    static bool HandleArenaCommand(ChatHandler* handler, char const* /*args*/)
    {
        BattlegroundQueue const& bgQueue2s = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_2v2);
        BattlegroundQueue const& bgQueue3s = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_3v3);
        uint32 queuedTeams = 0;
        queuedTeams = bgQueue2s.GetQueuedGroups();
        handler->PSendSysMessage("Queued 2s Teams: %u", queuedTeams);
        queuedTeams = bgQueue3s.GetQueuedGroups();
        handler->PSendSysMessage("Queued 3s Teams: %u", queuedTeams);

        uint16 groupCnt = 0;
        auto bginfomap = sInfoMgr->GetBgStore(SlotStoreType(0));
        groupCnt += bginfomap.size();
        bginfomap = sInfoMgr->GetBgStore(SlotStoreType(1));
        groupCnt += bginfomap.size();
        handler->PSendSysMessage("Teams in arena playing 2s: %u", groupCnt);

        groupCnt = 0;
        bginfomap = sInfoMgr->GetBgStore(SlotStoreType(2));
        groupCnt += bginfomap.size();
        bginfomap = sInfoMgr->GetBgStore(SlotStoreType(3));
        groupCnt += bginfomap.size();
        handler->PSendSysMessage("Teams in arena playing 3s: %u", groupCnt);
        return true;
    }

    static bool HandleGuestAdd(ChatHandler* handler, char const* args)
    {

        if (auto s = handler->GetSession())
            if (!s->HasAccessFlag(ACCOUNT_FLAG_GUEST_ACCESS_CONTROLLER))
            {
                handler->PSendSysMessage("Your account is not designated as a guest access controller.");
                return true;
            }

        if (!*args)
            return false;

        uint32 accountId{ 0 };
        char* input = strtok((char*)args, " ");

        if (atoi(input))
            accountId = uint32(atoi(input));
        else
        {
            handler->PSendSysMessage("invalid input");
            return true;
        }

        if (accountId)
        {
            uint32 realmId = sWorld->getRealmID();

            auto select_level = sWorld->GetPlayerSecurityLimit();
            auto entrance_level = sWorld->GetEntranceSecurityLimit();
            uint32 level_to_pass = uint32(entrance_level > select_level ? entrance_level : select_level);


            if (level_to_pass > 0)
            {
                if (auto result = LoginDatabase.PQuery("SELECT * FROM `account_access` WHERE id = %u AND gmlevel >= %u and RealmID = %u;", accountId, uint32(level_to_pass), realmId))
                {
                    handler->PSendSysMessage("That account already has access to this realm.");
                    return true;
                }
            }
            else
            {
                handler->PSendSysMessage("That account already has access to this realm.");
                return true;
            }

            std::ostringstream ostream2{ "" };
            ostream2 << "Guest access via account " << handler->GetSession()->GetAccountId() << ".";

            auto comment2{ ostream2.str() };
            auto comment = comment2.c_str();


            LoginDatabase.PQuery("REPLACE INTO `account_access` VALUES (%u, %u, %u, '%s', %u);", accountId, uint32(level_to_pass), realmId, comment, uint32(16));

            handler->PSendSysMessage("Account %u has been granted GM level %u for access to realm ID %u.", accountId, uint32(level_to_pass), realmId);
            return true;

        }
        else
        {

            handler->PSendSysMessage("Account ID not properly specified.", accountId);
            return true;

        }
    }

    static bool HandleGuestRemove(ChatHandler* handler, char const* args)
    {

        if (auto s = handler->GetSession())
            if (!s->HasAccessFlag(ACCOUNT_FLAG_GUEST_ACCESS_CONTROLLER))
            {
                handler->PSendSysMessage("Your account is not designated as a guest access controller.");
                return true;
            }

        if (!*args)
            return false;

        uint32 accountId{ 0 };
        char* input = strtok((char*)args, " ");

        if (atoi(input))
            accountId = uint32(atoi(input));
        else
        {
            handler->PSendSysMessage("invalid input");
            return true;
        }


        auto realmID = sWorld->getRealmID();

        auto select_level = sWorld->GetPlayerSecurityLimit();
        auto entrance_level = sWorld->GetEntranceSecurityLimit();
        uint32 level_to_pass = uint32(entrance_level > select_level ? entrance_level : select_level);

        if (accountId)
        {
            uint32 realmId = sWorld->getRealmID();
            if (auto result = LoginDatabase.PQuery("SELECT * FROM `account_access` WHERE id = %u AND (access_flags & 16) and RealmID = %u;", accountId, realmId))
            {
                if (auto fields = result->Fetch())
                {
                    auto level = fields[1].GetUInt32();
                        sWorld->RemoveSession(accountId);
                        if (auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_GUEST))
                        {
                            stmt->setUInt32(0, accountId);
                            stmt->setUInt32(1, realmID);
                            LoginDatabase.Query(stmt);
                        }

                        handler->PSendSysMessage("Account %u removed from guest list.", accountId);
                        return true;
                }
                else
                {
                    handler->PSendSysMessage("No Account specified.");
                    return true;
                }
            }
            else
            {
                handler->PSendSysMessage("That account does not have guest-flagged access to this realm.");
                return true;
            }
        }
        else
        {
            handler->PSendSysMessage("No Account specified.");
        }


        return true;
    }

    static bool HandleGuestList(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        if (auto s = handler->GetSession())
            if (!s->HasAccessFlag(ACCOUNT_FLAG_GUEST_ACCESS_CONTROLLER))
            {

                handler->PSendSysMessage("Your account is not designated as a guest access controller.");
                return true;
            }

        auto realmID = sWorld->getRealmID();

        auto select_level = sWorld->GetPlayerSecurityLimit();
        auto entrance_level = sWorld->GetEntranceSecurityLimit();
        uint32 level_to_pass = uint32(entrance_level > select_level ? entrance_level : select_level);
        level_to_pass = 10;

        if (auto result = LoginDatabase.PQuery("SELECT * FROM `account_access` WHERE (access_flags & 16) AND gmLevel = %u AND RealmID = %u;", level_to_pass, realmID))
        {
            std::ostringstream ostream2{ "" };
            ostream2 << "-- START " << "|cffff0000|Harea:" << "|h" << "Guest Access list" << "|h|r" << " --\n";

            do
            {
                if (auto fields2 = result->Fetch())
                {
                    uint32 account_id{ fields2[0].GetUInt32() };
                    auto note{ fields2[3].GetCString() };
                    ostream2 << "Account no. " << "|cffADFF2F|Harea:" << "|h" << account_id << "|h|r" << " has Guest Access with the note: ";
                    ostream2 << "|cffADFF2F|Harea:" << "|h'" << note << "'|h|r" << ".\n";
                }

            } while (result->NextRow());

            ostream2 << "-- END " << "|cffff0000|Harea:" << "|h" << "Guest Access list" << "|h|r" << " --\n";
            auto resulting_string{ ostream2.str() };
            auto log{ resulting_string.c_str() };
            handler->PSendSysMessage("%s", log);
        }
        else
        {
            handler->PSendSysMessage("There are currently no accounts with guest access.");
            return true;
        }
        return true;
    }

};


void AddSC_misc_commandscript()
{
    new misc_commandscript();
}
