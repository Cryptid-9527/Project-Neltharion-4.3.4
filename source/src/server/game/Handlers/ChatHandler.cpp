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

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "CellImpl.h"
#include "Chat.h"
#include "ChannelMgr.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Guild.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Battleground.h"

void WorldSession::HandleMessagechatOpcode(WorldPacket& recvData)
{
    uint32 type = 0;
    uint32 lang = LANG_UNIVERSAL;

    switch (recvData.GetOpcode())
    {
        case CMSG_MESSAGECHAT_SAY:
            type = CHAT_MSG_SAY;
            break;
        case CMSG_MESSAGECHAT_YELL:
            type = CHAT_MSG_YELL;
            break;
        case CMSG_MESSAGECHAT_CHANNEL:
            type = CHAT_MSG_CHANNEL;
            break;
        case CMSG_MESSAGECHAT_WHISPER:
            type = CHAT_MSG_WHISPER;
            break;
        case CMSG_MESSAGECHAT_GUILD:
            type = CHAT_MSG_GUILD;
            break;
        case CMSG_MESSAGECHAT_OFFICER:
            type = CHAT_MSG_OFFICER;
            break;
        case CMSG_MESSAGECHAT_AFK:
            type = CHAT_MSG_AFK;
            break;
        case CMSG_MESSAGECHAT_DND:
            type = CHAT_MSG_DND;
            break;
        case CMSG_MESSAGECHAT_EMOTE:
            type = CHAT_MSG_EMOTE;
            break;
        case CMSG_MESSAGECHAT_PARTY:
            type = CHAT_MSG_PARTY;
            break;
        case CMSG_MESSAGECHAT_RAID:
            type = CHAT_MSG_RAID;
            break;
        case CMSG_MESSAGECHAT_BATTLEGROUND:
            type = CHAT_MSG_BATTLEGROUND;
            break;
        case CMSG_MESSAGECHAT_RAID_WARNING:
            type = CHAT_MSG_RAID_WARNING;
            break;
        default:
            TC_LOG_ERROR("network.opcode", "HandleMessagechatOpcode : Unknown chat opcode (%u)", recvData.GetOpcode());
            recvData.hexlike();
            return;
    }

    if (type >= MAX_CHAT_MSG_TYPE)
    {
        TC_LOG_ERROR("network.opcode", "CHAT: Wrong message type received: %u", type);
        recvData.rfinish();
        return;
    }

    Player* sender = GetPlayer();

    if (!sender)//null check
        return;

    //TC_LOG_DEBUG("misc", "CHAT: packet received. type %u, lang %u", type, lang);

    // no language sent with emote packet.
    if (type != CHAT_MSG_EMOTE && type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
    {
        recvData >> lang;

        if (lang == LANG_UNIVERSAL && !sender->isGameMaster())
        {
            TC_LOG_ERROR("network.opcode", "CMSG_MESSAGECHAT: Possible hacking-attempt: %s tried to send a message in universal language", GetPlayerInfo().c_str());
            SendNotification(LANG_UNKNOWN_LANGUAGE);
            recvData.rfinish();
            return;
        }

        // prevent talking at unknown language (cheating)
        LanguageDesc const* langDesc = GetLanguageDescByID(lang);
        if (!langDesc)
        {
            SendNotification(LANG_UNKNOWN_LANGUAGE);
            recvData.rfinish();
            return;
        }
        if (langDesc->skill_id != 0 && !sender->HasSkill(langDesc->skill_id))
        {
            // also check SPELL_AURA_COMPREHEND_LANGUAGE (client offers option to speak in that language)
            Unit::AuraEffectList const& langAuras = sender->GetAuraEffectsByType(SPELL_AURA_COMPREHEND_LANGUAGE);
            bool foundAura = false;
            for (Unit::AuraEffectList::const_iterator i = langAuras.begin(); i != langAuras.end(); ++i)
            {
                if ((*i)->GetMiscValue() == int32(lang))
                {
                    foundAura = true;
                    break;
                }
            }
            if (!foundAura)
            {
                SendNotification(LANG_NOT_LEARNED_LANGUAGE);
                recvData.rfinish();
                return;
            }
        }

        if (lang == LANG_ADDON)
        {
            // LANG_ADDON is only valid for the following message types
            switch (type)
            {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_RAID:
                case CHAT_MSG_GUILD:
                case CHAT_MSG_BATTLEGROUND:
                    if (sender->needsGMFreeze())
                    {
                        SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                        recvData.rfinish();
                        return;
                    }
                case CHAT_MSG_WHISPER:

                    if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
                    {
                        std::string msg = "";
                        recvData >> msg;

                        if (msg.empty())
                            return;

                        sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), lang, msg);
                    }

                    // Disabled addon channel?
                    if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
                        return;
                    break;
                default:
                    TC_LOG_ERROR("network.opcode", "Player %s (GUID: %u) sent a chatmessage with an invalid language/message type combination",
                        GetPlayer()->GetName().c_str(), GetPlayer()->GetGUIDLow());

                    recvData.rfinish();
                    return;
            }
        }
        // LANG_ADDON should not be changed nor be affected by flood control
        else
        {
            // send in universal language if player in .gm on mode (ignore spell effects)
            if (sender->isGameMaster())
                lang = LANG_UNIVERSAL;
            else
            {
                if (auto m = sender->GetMap())
                    if (auto g = sender->GetGroup())
                        if (g->isLFGGroup())
                            if (m->IsDungeon())
                                lang = LANG_UNIVERSAL;

                // send in universal language in two side iteration allowed mode
                if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT))
                    lang = LANG_UNIVERSAL;
                else
                {
                    switch (type)
                    {
                        case CHAT_MSG_PARTY:
                        case CHAT_MSG_PARTY_LEADER:
                        case CHAT_MSG_RAID:
                        case CHAT_MSG_RAID_LEADER:
                        case CHAT_MSG_RAID_WARNING:
                        case CHAT_MSG_BATTLEGROUND:
                        case CHAT_MSG_BATTLEGROUND_LEADER:
                            // allow two side chat at group channel if two side group allowed
                            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) || sWorld->getBoolConfig(BATTLEGROUND_CROSSFACTION_ENABLED) || sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_LFG))
                                lang = LANG_UNIVERSAL;
                            break;
                        case CHAT_MSG_GUILD:
                        case CHAT_MSG_OFFICER:
                            // allow two side chat at guild channel if two side guild allowed
                            if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                                lang = LANG_UNIVERSAL;
                            break;
                    }
                }

                // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
                Unit::AuraEffectList const& ModLangAuras = sender->GetAuraEffectsByType(SPELL_AURA_MOD_LANGUAGE);
                if (!ModLangAuras.empty())
                    lang = ModLangAuras.front()->GetMiscValue();
            }
        }
    }
    else
        lang = LANG_UNIVERSAL;

    if (type != CHAT_MSG_AFK && type != CHAT_MSG_DND && !sender->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        recvData.rfinish(); // Prevent warnings
        return;
    }

    uint32 textLength = 0;
    uint32 receiverLength = 0;
    std::string to, channel, msg;
    bool ignoreChecks = false;
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
            textLength = recvData.ReadBits(9);
            msg = recvData.ReadString(textLength);
            break;
        case CHAT_MSG_WHISPER:
            receiverLength = recvData.ReadBits(10);
            textLength = recvData.ReadBits(9);
            to = recvData.ReadString(receiverLength);
            msg = recvData.ReadString(textLength);
            break;
        case CHAT_MSG_CHANNEL:
            receiverLength = recvData.ReadBits(10);
            textLength = recvData.ReadBits(9);
            msg = recvData.ReadString(textLength);
            channel = recvData.ReadString(receiverLength);
            break;
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
            textLength = recvData.ReadBits(9);
            msg = recvData.ReadString(textLength);
            ignoreChecks = true;
            break;
    }

    if (!ignoreChecks)
    {
        if (msg.empty())
            return;

        if (ChatHandler(this).ParseCommands(msg.c_str()))
            return;

        if (lang != LANG_ADDON)
        {
            // Strip invisible characters for non-addon messages
            if (sWorld->getBoolConfig(CONFIG_CHAT_FAKE_MESSAGE_PREVENTING))
                stripLineInvisibleChars(msg);

            if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
            {
                TC_LOG_ERROR("network.opcode", "Player %s (GUID: %u) sent a chatmessage with an invalid link: %s", GetPlayer()->GetName().c_str(),
                    GetPlayer()->GetGUIDLow(), msg.c_str());

                if (sWorld->getIntConfig(CONFIG_CHAT_STRICT_LINK_CHECKING_KICK))
                    KickPlayer("Chat message with invalid link");

                return;
            }
        }
    }

    //values to compensate for death knight level advantage, as well as capping at (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) without issues.
    uint8 SaylevelRequired = (sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0) > (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) ? (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) : sWorld->getIntConfig(CONFIG_CHAT_SAY_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0));
    uint8 WhisperlevelRequired = (sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0) > (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) ? (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) : sWorld->getIntConfig(CONFIG_CHAT_WHISPER_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0));
    uint8 ChannelLevelRequired = (sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0) > (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) ? (sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL)) : sWorld->getIntConfig(CONFIG_CHAT_CHANNEL_LEVEL_REQ) + (sender->getClass() == CLASS_DEATH_KNIGHT ? (sWorld->getIntConfig(CONFIG_START_HEROIC_PLAYER_LEVEL)) : 0));
    switch (type)
    {
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        {
            if (_player->isDead())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                return;
            }

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

            if (_player->getLevel() < SaylevelRequired)
            {
                SendNotification(GetTrinityString(LANG_SAY_REQ), SaylevelRequired);
                return;
            }

            if (!GetPlayer()->isGameMaster())
                if (GetPlayer()->SendBattleGroundChat(type, msg))
                    return;

            if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), nullptr))
            {
                recvData.rfinish();
                return;
            }

            if (type == CHAT_MSG_SAY)
                sender->Say(msg, lang);
            else if (type == CHAT_MSG_EMOTE)
                sender->TextEmote(msg);
            else if (type == CHAT_MSG_YELL)
                sender->Yell(msg, lang);
        } break;
        case CHAT_MSG_WHISPER:
        {
            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }

            Player* receiver = sObjectAccessor->FindPlayerByName(to);
            bool senderIsPlayer = (!sender->isGMChat() && !sender->isGameMaster());
            bool receiverIsPlayer = NULL;

            if (!receiver)
            {
                SendPlayerNotFoundNotice(to);
                return;
            }

            if (receiver)//null check
                receiverIsPlayer = (!receiver->isGMChat() && !receiver->isGameMaster());
            /*
            new definition of non-player: gm chat active or gm mode active.
            this prevents hidden moderators from being caught through crossfaction whispers because they had a higher security level.
            */

            if (senderIsPlayer)
                if (receiverIsPlayer)
                {
                    if (_player->getLevel() < WhisperlevelRequired)
                    {
                        SendNotification(GetTrinityString(LANG_WHISPER_REQ), WhisperlevelRequired);
                        return;
                    }

                    if (_player->needsGMFreeze())
                    {
                        SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                        recvData.rfinish();
                        return;
                    }
                }

            if (receiver)
                if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), receiver))
                {
                    recvData.rfinish();
                    return;
                }

            if (receiverIsPlayer)
            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }


            if (!receiver->isAcceptWhispers())//Protect a GM's camoflauge
            {
                if (GetSecurity() >= SEC_MODERATOR)
                {
                    if (auto s2 = receiver->GetSession())
                        if (s2->HasAccessFlag(ACCOUNT_FLAG_HEAD_GAMEMASTER) && receiver->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND))//Protect head gm
                        {
                            SendPlayerNotFoundNotice(to);
                            return;
                        }
                }
                else
                {
                    SendPlayerNotFoundNotice(to);
                    return;
                }
            }

            if (HasAccessFlag(ACCOUNT_FLAG_HEAD_GAMEMASTER) && sender->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND))//Let head gm know the GM won't be able to respond if they're in super-stealth
                    ChatHandler(this).PSendSysMessage("You have responses from Game Masters Blocked via /DND, this person will not be able to reply.");

            if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT) && senderIsPlayer && receiverIsPlayer)
            {
                // Both players are involved in the SAME battleground
                if (GetPlayer()->GetBattleground() && receiver->GetBattleground() && (GetPlayer()->GetBattleground() == receiver->GetBattleground()))
                    if ((GetPlayer()->GetTeam()) != receiver->GetTeam())//current faction check
                    {
                        SendWrongFactionNotice();
                        return;
                    }
                // Both players are involved in different battlegrounds
                if (GetPlayer()->GetBattleground() && receiver->GetBattleground() && (GetPlayer()->GetBattleground() != receiver->GetBattleground()))
                    if ((GetPlayer()->GetOTeam()) != receiver->GetOTeam())//cold faction check
                    {
                        SendWrongFactionNotice();
                        return;
                    }
                //One player is not in a battleground
                if (!GetPlayer()->GetBattleground() || !receiver->GetBattleground())
                    if ((GetPlayer()->GetOTeam()) != receiver->GetOTeam())//cold faction check
                    {
                        SendWrongFactionNotice();
                        return;
                    }
            }
            //Handling crossfaction status of whisper interaction

            // If player is a Gamemaster and doesn't accept whisper, we auto-whitelist every player that the Gamemaster is talking to
            if (!senderIsPlayer && !sender->isAcceptWhispers() && !sender->IsInWhisperWhiteList(receiver->GetGUID()))
                sender->AddWhisperWhiteList(receiver->GetGUID());

            GetPlayer()->Whisper(msg, lang, receiver->GetGUID());
        } break;
        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER:
        {
            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

                if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), nullptr))
                {
                    recvData.rfinish();
                    return;
                }

            bool isSoloQueueArena = false;
            if (auto bg = GetPlayer()->GetBattleground())
                isSoloQueueArena = bg->isSoloQueueArena();

            // if player is in battleground, he cannot say to battleground members by /p
            Group* group = isSoloQueueArena ? GetPlayer()->GetGroup() : GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = _player->GetGroup();
                if (!group || group->isBGGroup())
                    return;
            }

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = CHAT_MSG_PARTY_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, uint8(type), lang, NULL, 0, msg.c_str(), NULL);

            if (auto p = GetPlayer())
            if (auto s = p->GetSession())
                if (p->CanSpeak() && !p->CanSpeak(true))
                    s->SendPacket(&data);
                else
                {
                    group->BroadcastPacket(&data, false, group->GetMemberGroup(GetPlayer()->GetGUID()));
                    _player->CarbonCopy(CARBON_COPY_PARTY_TEXT, "Party", msg.c_str());
                }
        } break;
        case CHAT_MSG_GUILD:
        {

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

            if (GetPlayer()->GetGuildId())
            {
                if (auto guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);

                    guild->BroadcastToGuild(this, false, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        } break;
        case CHAT_MSG_OFFICER:
        {

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

            if (GetPlayer()->GetGuildId())
            {
                if (auto guild = sGuildMgr->GetGuildById(GetPlayer()->GetGuildId()))
                {
                    sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, guild);

                    guild->BroadcastToGuild(this, true, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }
            }
        } break;
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        {

            if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), nullptr))
            {
                recvData.rfinish();
                return;
            }

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }
            // if player is in battleground, he cannot say to battleground members by /ra
            Group* group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup())
                    return;
            }

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = CHAT_MSG_RAID_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, uint8(type), lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            _player->CarbonCopy(CARBON_COPY_RAID_TEXT, "Raid", msg.c_str());
        } break;
        case CHAT_MSG_RAID_WARNING:
        {

            if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), nullptr))
            {
                recvData.rfinish();
                return;
            }

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() || !(group->IsLeader(GetPlayer()->GetGUID()) || group->IsAssistant(GetPlayer()->GetGUID())) || group->isBGGroup())
                return;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::FillMessageData(&data, this, CHAT_MSG_RAID_WARNING, lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
        } break;
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
        {

            if (_player->needsGMFreeze())
            {
                SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                recvData.rfinish();
                return;
            }

            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group* group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup())
                return;

            if (group->IsLeader(GetPlayer()->GetGUID()))
                type = CHAT_MSG_BATTLEGROUND_LEADER;

            sScriptMgr->OnPlayerChat(GetPlayer(), type, lang, msg, group);

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, uint8(type), lang, "", 0, msg.c_str(), NULL);
            group->BroadcastPacket(&data, false);
            _player->CarbonCopy(CARBON_COPY_RAID_TEXT, "Battleground", msg.c_str());
        } break;
        case CHAT_MSG_CHANNEL:
        {

            if (_player->BlockChatDueToBadgeRequirement(ChatMsg(type), nullptr))
            {
                recvData.rfinish();
                return;
            }

            if (AccountMgr::IsPlayerAccount(GetSecurity()))
            {
                if (_player->getLevel() < ChannelLevelRequired)
                {
                    SendNotification(GetTrinityString(LANG_CHANNEL_REQ), ChannelLevelRequired);
                    return;
                }
                if (_player->needsGMFreeze())
                {
                    SendNotification(GetTrinityString(LANG_TALK_WHILE_DEAD));
                    recvData.rfinish();
                    return;
                }
            }

            if (ChannelMgr* cMgr = ChannelMgr::forTeam(_player->GetOTeam()))
            {
                if (Channel* chn = cMgr->GetChannel(channel, _player))
                {
                    sScriptMgr->OnPlayerChat(_player, type, lang, msg, chn);

                    if (chn->Say(_player->GetGUID(), msg.c_str(), lang))
                        _player->CarbonCopy(CARBON_COPY_PUBLIC_CHANNELS, channel, msg.c_str());
                }
            }
            break;
        }
        case CHAT_MSG_AFK:
        {
            if (!_player->isInCombat())
            {
                if (_player->isAFK())                       // Already AFK
                {
                    if (msg.empty())
                        _player->ToggleAFK();               // Remove AFK
                    else
                        _player->autoReplyMsg = msg;        // Update message
                }
                else                                        // New AFK mode
                {
                    _player->autoReplyMsg = msg.empty() ? GetTrinityString(LANG_PLAYER_AFK_DEFAULT) : msg;

                    if (_player->isDND())
                        if (auto s = _player->GetSession())
                            if (!s->HasAccessFlag(ACCOUNT_FLAG_HEAD_GAMEMASTER))
                        _player->ToggleDND();

                    _player->ToggleAFK();
                }

                sScriptMgr->OnPlayerChat(_player, type, lang, msg);
            }
            break;
        }
        case CHAT_MSG_DND:
        {

            if (_player->isAFK())
                _player->ToggleAFK();

            _player->ToggleDND();

            sScriptMgr->OnPlayerChat(_player, type, lang, msg);
            break;
        }
        default:
            TC_LOG_ERROR("network.opcode", "CHAT: unknown message type %u, lang: %u", type, lang);
            break;
    }
}

void WorldSession::HandleAddonMessagechatOpcode(WorldPacket& recvData)
{
    Player* sender = GetPlayer();
    ChatMsg type;

    switch (recvData.GetOpcode())
    {
        case CMSG_MESSAGECHAT_ADDON_BATTLEGROUND:
            type = CHAT_MSG_BATTLEGROUND;
            break;
        case CMSG_MESSAGECHAT_ADDON_GUILD:
            type = CHAT_MSG_GUILD;
            break;
        case CMSG_MESSAGECHAT_ADDON_OFFICER:
            type = CHAT_MSG_OFFICER;
            break;
        case CMSG_MESSAGECHAT_ADDON_PARTY:
            type = CHAT_MSG_PARTY;
            break;
        case CMSG_MESSAGECHAT_ADDON_RAID:
            type = CHAT_MSG_RAID;
            break;
        case CMSG_MESSAGECHAT_ADDON_WHISPER:
            type = CHAT_MSG_WHISPER;
            break;
        default:
            TC_LOG_ERROR("network.opcode", "HandleAddonMessagechatOpcode: Unknown addon chat opcode (%u)", recvData.GetOpcode());
            recvData.hexlike();
            return;
    }

    std::string message;
    std::string prefix;
    std::string targetName;

    switch (type)
    {
        case CHAT_MSG_WHISPER:
        {
            uint32 msgLen = recvData.ReadBits(9);
            uint32 prefixLen = recvData.ReadBits(5);
            uint32 targetLen = recvData.ReadBits(10);
            message = recvData.ReadString(msgLen);
            prefix = recvData.ReadString(prefixLen);
            targetName = recvData.ReadString(targetLen);

            /*
                THIS IS WHERE ADDON WILL SPIT OUT IT"S MESSAGE CONTENTS.
                The message will be the rate
                The prefix will be something set in the script
                The target name will be something set in the script.
            */
            if (recvData.GetOpcode() == CMSG_MESSAGECHAT_ADDON_WHISPER)
            {
                //TC_LOG_ERROR("sql.sql", "received addon xp set: message: %s , prefix: %s, targetName: %s", message, prefix, targetName);
                if (prefix == "ProjectNelthXP")
                    if (targetName == "ProjectNelth")
                    {

                        auto requested_rate{ atoi(message.c_str()) };

                        //TC_LOG_ERROR("sql.sql", "xp key match verified, rate requested: %u", requested_rate);
                        bool valid{ false };

                        if (requested_rate)
                            if (auto max_rate = sWorld->getRate(RATE_XP_QUEST))
                                if (requested_rate > 0)
                                    if (float(requested_rate) <= max_rate)
                                    {
                                        valid = true;
                                        sender->SetCustomXpRate(uint32(requested_rate));
                                        ChatHandler(this).PSendSysMessage("|cffADFF2FYou have set your XP rate to|r|cffFFC125 x%u|r|cffADFF2F.", requested_rate);
                                    }
                        if (!valid)
                            ChatHandler(this).PSendSysMessage("You have requested an invalid xp rate via addon.");
                        recvData.rfinish();
                        return;
                    }
            }
            break;
        }
        case CHAT_MSG_RAID:
        case CHAT_MSG_BATTLEGROUND:
        {
            uint32 prefixLen = recvData.ReadBits(5);
            uint32 msgLen = recvData.ReadBits(9);
            prefix = recvData.ReadString(prefixLen);
            message = recvData.ReadString(msgLen);
            break;
        }
        case CHAT_MSG_PARTY:
        case CHAT_MSG_OFFICER:
        {
            uint32 prefixLen = recvData.ReadBits(5);
            uint32 msgLen = recvData.ReadBits(9);
            message = recvData.ReadString(msgLen);
            prefix = recvData.ReadString(prefixLen);
            break;
        }
        case CHAT_MSG_GUILD:
        {
            uint32 msgLen = recvData.ReadBits(9);
            uint32 prefixLen = recvData.ReadBits(5);
            message = recvData.ReadString(msgLen);
            prefix = recvData.ReadString(prefixLen);
            break;
        }
        default:
            break;
    }

    // Logging enabled?
    if (sWorld->getBoolConfig(CONFIG_CHATLOG_ADDON))
    {
        if (message.empty())
        {
            recvData.rfinish();
            return;
        }

        // Weird way to log stuff...
        sScriptMgr->OnPlayerChat(sender, uint32(CHAT_MSG_ADDON), uint32(LANG_ADDON), message);
    }

    // Disabled addon channel?
    if (!sWorld->getBoolConfig(CONFIG_ADDON_CHANNEL))
    {
        recvData.rfinish();
        return;
    }

    switch (type)
    {
        case CHAT_MSG_BATTLEGROUND:
        {
            Group* group = sender->GetGroup();
            if (!group || !group->isBGGroup())
            {
                recvData.rfinish();
                return;
            }

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, type, uint32(LANG_ADDON), "", 0, message.c_str(), NULL, prefix.c_str());
            group->BroadcastAddonMessagePacket(&data, prefix, false);
            break;
        }
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        {
            if (sender->GetGuildId())
                if (auto guild = sGuildMgr->GetGuildById(sender->GetGuildId()))
                    guild->BroadcastAddonToGuild(this, type == CHAT_MSG_OFFICER, message, prefix);
            break;
        }
        case CHAT_MSG_WHISPER:
        {
            if (!normalizePlayerName(targetName))
                break;
            Player* receiver = sObjectAccessor->FindPlayerByName(targetName.c_str());
            if (!receiver)
                break;

            sender->WhisperAddon(message, prefix, receiver);
            break;
        }
        // Messages sent to "RAID" while in a party will get delivered to "PARTY"
        case CHAT_MSG_PARTY:
        case CHAT_MSG_RAID:
        {

            Group* group = sender->GetGroup();
            if (!group || group->isBGGroup())
                break;

            WorldPacket data;
            ChatHandler::FillMessageData(&data, this, type, uint32(LANG_ADDON), "", 0, message.c_str(), NULL, prefix.c_str());
            group->BroadcastAddonMessagePacket(&data, prefix, true, -1, group->GetMemberGroup(sender->GetGUID()));
            break;
        }
        default:
        {
            TC_LOG_ERROR("misc", "HandleAddonMessagechatOpcode: unknown addon message type %u", type);
            break;
        }
    }
        recvData.rfinish();
        return;
}

void WorldSession::HandleEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->isAlive() || GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        return;

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 emote;
    recvData >> emote;
    sScriptMgr->OnPlayerEmote(GetPlayer(), emote);
    GetPlayer()->HandleEmoteCommand(emote);
}

namespace Trinity
{
    class EmoteChatBuilder
    {
        public:
            EmoteChatBuilder(Player const& player, uint32 text_emote, uint32 emote_num, Unit const* target)
                : i_player(player), i_text_emote(text_emote), i_emote_num(emote_num), i_target(target) {}

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                std::string const name(i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
                uint32 namlen = name.size();

                data.Initialize(SMSG_TEXT_EMOTE, 20 + namlen);
                data << i_player.GetGUID();
                data << uint32(i_text_emote);
                data << uint32(i_emote_num);
                data << uint32(namlen);
                if (namlen > 1)
                    data << name;
                else
                    data << uint8(0x00);
            }

        private:
            Player const& i_player;
            uint32        i_text_emote;
            uint32        i_emote_num;
            Unit const*   i_target;
    };
}                                                           // namespace Trinity

void WorldSession::HandleTextEmoteOpcode(WorldPacket& recvData)
{
    if (!GetPlayer()->isAlive())
        return;

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = secsToTimeString(m_muteTime - time(NULL));
        SendNotification(GetTrinityString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 text_emote, emoteNum;
    uint64 guid;

    recvData >> text_emote;
    recvData >> emoteNum;
    recvData >> guid;

    /*
        Prevent players from giving away an invisible GM via emotes
    */
    if (auto unit = ObjectAccessor::GetUnit(*_player, guid))
        if (auto emote_target = unit->ToPlayer())
            if (auto emote_sender = GetPlayer())
            if (!emote_target->isGMVisible() && emote_sender->isGMVisible())
                guid = 0;

    if (GetPlayer()->IsSpectator())
    {
        SendNotification(LANG_SPEC_CAN_NOT_CHAT);
        return;
    }

    sScriptMgr->OnPlayerTextEmote(GetPlayer(), text_emote, emoteNum, guid);

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(text_emote);
    if (!em)
        return;

    uint32 emote_anim = em->textid;

    switch (emote_anim)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
            // Only allow text-emotes for "dead" entities (feign death included)
            if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
                break;
            GetPlayer()->HandleEmoteCommand(emote_anim);
            break;
    }

    //shadowmute action here.

    Unit* unit = ObjectAccessor::GetUnit(*_player, guid);

    if (auto p = _player)
        if (auto s = p->GetSession())
        {
            WorldPacket pvt_data;
            std::string const name(unit ? unit->GetNameForLocaleIdx(s->GetSessionDbLocaleIndex()) : "");
            uint32 namlen = name.size();

            pvt_data.Initialize(SMSG_TEXT_EMOTE, 20 + namlen);
            pvt_data << _player->GetGUID();
            pvt_data << uint32(text_emote);
            pvt_data << uint32(emoteNum);
            pvt_data << uint32(namlen);
            if (namlen > 1)
                pvt_data << name;
            else
                pvt_data << uint8(0x00);

            if (p->CanSpeak() && !p->CanSpeak(true))
            {

                s->SendPacket(&pvt_data);
                return;
            }

            if (!p->isGMVisible())               
            {
                auto players_in_range{ p->GetPlayersInRange(sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), false, true) };
                for (auto emote_send_target : players_in_range)
                {                                                                                                                                                     
                    if (emote_send_target->canSeeOrDetect(p))                              
                        if (auto s2 = emote_send_target->GetSession())                                                                   
                            s2->SendPacket(&pvt_data);
                }
                return;
            }
        }


    CellCoord p = Trinity::ComputeCellCoord(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Trinity::EmoteChatBuilder emote_builder(*GetPlayer(), text_emote, emoteNum, unit);
    Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > emote_do(emote_builder);
    Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder > > emote_worker(GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), emote_do);
    TypeContainerVisitor<Trinity::PlayerDistWorker<Trinity::LocalizedPacketDo<Trinity::EmoteChatBuilder> >, WorldTypeMapContainer> message(emote_worker);
    cell.Visit(p, message, *GetPlayer()->GetMap(), *GetPlayer(), sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));

    GetPlayer()->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DO_EMOTE, text_emote, 0, 0, unit);

    //Send scripted event call
    if (unit && unit->GetTypeId() == TYPEID_UNIT && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), text_emote);
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;
    uint8 unk;
    //TC_LOG_DEBUG(LOG_FILTER_PACKETIO, "WORLD: Received CMSG_CHAT_IGNORED");

    recvData >> unk;                                       // probably related to spam reporting
    guid[5] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[2]);

    Player* player = ObjectAccessor::FindPlayer(guid);
    if (!player || !player->GetSession())
        return;

    WorldPacket data;
    ChatHandler::FillMessageData(&data, this, CHAT_MSG_IGNORED, LANG_UNIVERSAL, NULL, GetPlayer()->GetGUID(), GetPlayer()->GetName().c_str(), NULL);
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleChannelDeclineInvite(WorldPacket &recvPacket)
{
    TC_LOG_DEBUG("network.opcode", "Opcode %u", recvPacket.GetOpcode());
}

void WorldSession::SendPlayerNotFoundNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendPlayerAmbiguousNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_AMBIGUOUS, name.size()+1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendWrongFactionNotice()
{
    WorldPacket data(SMSG_CHAT_WRONG_FACTION, 0);
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice(ChatRestrictionType restriction)
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 1);
    data << uint8(restriction);
    SendPacket(&data);
}
