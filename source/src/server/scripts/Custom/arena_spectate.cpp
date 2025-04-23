#include "ArenaTeamMgr.h"
#include "BattlegroundMgr.h"
#include "Chat.h"
#include "DisableMgr.h"
#include "Language.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Battleground.h"
#include "InfoMgr.h"

int8 UsingGossip;

class arena_spectator_commands : public CommandScript
{
public:
    arena_spectator_commands() : CommandScript("arena_spectator_commands") { }

    static bool HandleSpectateCommand(ChatHandler* handler, char const* args)
    {
        Player* target;
        uint64 target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        if (target == player || target_guid == player->GetGUID())
        {
            handler->PSendSysMessage("You can't spectate yourself.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isInCombat())
        {
            handler->PSendSysMessage("You are in combat.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target)
        {
            handler->PSendSysMessage("Target is not online or does not exist.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->isGameMaster())
        {
            handler->PSendSysMessage("Target is not online or does not exist.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!target->isGMVisible())
        {
            handler->PSendSysMessage("Target is not online or does not exist.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetPet())
        {
            handler->PSendSysMessage("You must dismiss your pet.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattlegroundOrArena() && !player->IsSpectator())
        {
            handler->PSendSysMessage("You are already in a battleground or arena.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Map* cMap = target->GetMap();
        if (!cMap->IsBattleArena())
        {
            handler->PSendSysMessage("Player is not in an Arena match.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap()->IsBattleground())
        {
            handler->PSendSysMessage("You can't do that while in a battleground.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (sSoloQueueMgr->IsPlayerInSoloQueue(player))
        {
            handler->PSendSysMessage("You can't spectate while using the Solo Queue system.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_2v2) ||
            player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3) ||
            player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_5v5))
        {
            handler->PSendSysMessage("You can't spectate while in queue for rated arenas.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->isUsingLfg())
        {
            handler->PSendSysMessage("You can't spectate while you are using the dungeon finder.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (auto bg = target->GetBattleground())
            if ((bg->GetStatus() != STATUS_IN_PROGRESS) && !player->isGameMaster())
            {
                handler->PSendSysMessage("The Arena match didn't start yet.");
                handler->SetSentErrorMessage(true);
                return false;
            }

        if (target->IsSpectator())
        {
            handler->PSendSysMessage("You can't do that. Your target is a spectator.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // all's well, set bg id
        // when porting out from the bg, it will be reset to 0
        player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId());
        // remember current position as entry point for return at bg end teleportation
        if (!player->GetMap()->IsBattlegroundOrArena())
            player->SetBattlegroundEntryPoint();

        // stop flight if need
        if (player->isInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        // search for two teams
        Battleground* bGround = target->GetBattleground();
        
        if (!bGround)
            return false;

        if (bGround->isRated())
        {
            uint32 slot = bGround->GetArenaType() - 2;
            if (bGround->GetArenaType() > 3)
                slot = 2;
            uint32 firstTeamID = target->GetArenaTeamId(slot);
            uint32 secondTeamID = 0;
            Player* firstTeamMember = target;
            Player* secondTeamMember = NULL;
            for (Battleground::BattlegroundPlayerMap::const_iterator itr = bGround->GetPlayers().begin(); itr != bGround->GetPlayers().end(); ++itr)
                if (auto tmpPlayer = ObjectAccessor::FindPlayer(itr->first))
                {
                    if (tmpPlayer->IsSpectator())
                        continue;

                    uint32 tmpID = tmpPlayer->GetArenaTeamId(slot);
                    if (tmpID != firstTeamID && tmpID > 0)
                    {
                        secondTeamID = tmpID;
                        secondTeamMember = tmpPlayer;
                        break;
                    }
                }

            if (firstTeamID > 0 && secondTeamID > 0 && secondTeamMember)
            {
                ArenaTeam* firstTeam = sArenaTeamMgr->GetArenaTeamById(firstTeamID);
                ArenaTeam* secondTeam = sArenaTeamMgr->GetArenaTeamById(secondTeamID);
                if (firstTeam && secondTeam)
                {
                    handler->PSendSysMessage("You entered a Rated Arena.");
                    handler->PSendSysMessage("Teams:");
                    handler->PSendSysMessage("|cFFffffff%s|r vs |cFFffffff%s|r", firstTeam->GetName().c_str(), secondTeam->GetName().c_str());
                    handler->PSendSysMessage("|cFFffffff%u(%u)|r -- |cFFffffff%u(%u)|r", firstTeam->GetRating(), firstTeam->GetAverageMMR(firstTeamMember->GetGroup()),
                        secondTeam->GetRating(), secondTeam->GetAverageMMR(secondTeamMember->GetGroup()));
                    handler->PSendSysMessage("Available Commands:");
                    handler->PSendSysMessage(".spec leave - to leave the arena.");
                    handler->PSendSysMessage(".spec player - to teleport to the selected player's position.");
                    handler->PSendSysMessage(".spec view - to enter selected player's POV.");
                    handler->PSendSysMessage(".spec reset - to leave the spec view mode.");
                }
            }
        }

        // to point to see at target with same orientation
        float x, y, z;
        target->GetContactPoint(player, x, y, z);
        player->SetSpectate(true);
        player->TeleportTo(target->GetMapId(), x, y, z, player->GetAngle(target), TELE_TO_GM_MODE);
        player->SetPhaseMask(target->GetPhaseMask(), true);
        target->GetBattleground()->AddSpectator(player->GetGUID());

        return true;
    }

    static bool HandleSpectateCancelCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->IsSpectator() || player->isSpectateCanceled())
        {
            handler->PSendSysMessage("You are not a spectator.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        player->CancelSpectate();
        player->TeleportToBGEntryPoint();

        return true;
    }

    static bool HandleSpectateFromCommand(ChatHandler* handler, const char* args)
    {
        Player* target;
        uint64 target_guid;
        std::string target_name;
        if (!handler->extractPlayerTarget((char*)args, &target, &target_guid, &target_name))
            return false;

        Player* player = handler->GetSession()->GetPlayer();

        if (!target)
        {
            handler->PSendSysMessage("Player is not online or does not exist.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (!player->IsSpectator())
        {
            handler->PSendSysMessage("You are not a spectator, spectate someone first.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target->IsSpectator() && target != player)
        {
            handler->PSendSysMessage("You can't do that. Your target is a spectator.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetMap() != target->GetMap())
        {
            handler->PSendSysMessage("You can't do that. Your target might be in a different arena match.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target && target->HasAuraType(SPELL_AURA_MOD_STEALTH))
        {
            handler->PSendSysMessage("You can't target stealthed players.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // check for arena preperation
        // if exists than battle didn`t begin
        if (target && target->HasAura(32728) || target->HasAura(32727))
        {
            handler->PSendSysMessage("You can't do that. The Arena match didn't start yet.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (target == player)
            player->SetViewpoint(player->getSpectateFrom(), false);
        else
            player->SetViewpoint(target, true);
        return true;
    }

    // addon data
    static bool HandleSpectateResetCommand(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (!player->IsSpectator())
        {
            handler->PSendSysMessage("You are not a spectator.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Battleground* bGround = player->GetBattleground();
        if (!bGround)
            return false;

        if (bGround->GetStatus() != STATUS_IN_PROGRESS)
            return true;

        //player->SetViewpoint(player->getSpectateFrom(), false);

        for (Battleground::BattlegroundPlayerMap::const_iterator itr = bGround->GetPlayers().begin(); itr != bGround->GetPlayers().end(); ++itr)
            if (auto tmpPlayer = ObjectAccessor::FindPlayer(itr->first))
            {
                if (tmpPlayer->IsSpectator())
                    continue;

                uint32 tmpID = bGround->GetPlayerTeam(tmpPlayer->GetGUID());

                // generate addon massage
                std::string pName = tmpPlayer->GetName();
                std::string tName = "";

                if (auto target = tmpPlayer->GetSelectedPlayer())
                    tName = target->GetName();

                SpectatorAddonMsg msg;
                msg.SetPlayer(pName);
                if (tName != "")
                    msg.SetTarget(tName);
                msg.SetStatus(tmpPlayer->isAlive());
                msg.SetClass(tmpPlayer->getClass());
                msg.SetCurrentHP(tmpPlayer->GetHealth());
                msg.SetMaxHP(tmpPlayer->GetMaxHealth());
                Powers powerType = tmpPlayer->getPowerType();
                msg.SetMaxPower(tmpPlayer->GetMaxPower(powerType));
                msg.SetCurrentPower(tmpPlayer->GetPower(powerType));
                msg.SetPowerType(powerType);
                msg.SetTeam(tmpID);
                msg.SendPacket(player->GetGUID());
            }

        return true;
    }

    static bool HandleForceBGCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 bgtypeid = atoi(args);
        sWorld->SetForcedBG(BattlegroundTypeId(bgtypeid));

        return true;
    }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> spectateCommandTable =
        {
            { "player",   SEC_CONSOLE, true, &HandleSpectateCommand,      "" },
            { "view",     SEC_CONSOLE, true, &HandleSpectateFromCommand,  "" },
            { "reset",    SEC_CONSOLE, true, &HandleSpectateResetCommand, "" },
            { "leave",    SEC_CONSOLE, true, &HandleSpectateCancelCommand,"" },
            { "setbg",    SEC_CONSOLE, true, &HandleForceBGCommand,       "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "spectate", SEC_CONSOLE, false, NULL, "", spectateCommandTable },
        };
        return commandTable;
    }
};

enum NpcSpectatorAtions 
{
    NPC_SPECTATOR_ACTION_2V2_GAMES = 1000,
    NPC_SPECTATOR_ACTION_3V3_GAMES = 2000,
    NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES = 4000,
    NPC_SPECTATOR_ACTION_CANCEL = 8000,
    NPC_SPECTATOR_ACTION_SPECIFIC = 500,
    
    NPC_SPECTATOR_ACTION_SELECTED_PLAYER = 3000,
    NPC_SPECTATOR_ACTION_SELECTED_SOLO_PLAYER = 6000
};

const uint8  GamesOnPage = 15;

class npc_arena_spectator : public CreatureScript
{
public:
    npc_arena_spectator() : CreatureScript("npc_arena_spectator") { }

    struct npc_arena_spectatorAI : public ScriptedAI
    {
        npc_arena_spectatorAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            if (me->GetAreaId() == 1519)//alliance
            {
                me->SetDisplayId(17991);
                me->SetObjectScale(0.63f);
                if (auto chair = me->FindNearestGameObject(201556, 10.f))
                    chair->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }
            else if (me->GetAreaId() == 5169)//horde
            {
                me->SetDisplayId(17992);
                me->SetObjectScale(0.63f);
            }
        }

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_arena_spectatorAI(creature);
    }
    
    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {

        /*std::stringstream games2v2;
        std::stringstream games3v3;
        std::stringstream games3v3solo;

        games2v2 << "Spectate 2v2 Rated games (Games currently in progress: " << CountArenaGames(ARENA_TYPE_2v2) << ")";
        games3v3 << "Spectate 3v3 Rated games (Games currently in progress: " << CountArenaGames(ARENA_TYPE_3v3) << ")";
        games3v3solo << "Spectate Solo 3v3 games (Games currently in progress: " << CountArenaGames(ARENA_TYPE_3v3_SOLO) << ")";

        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, games2v2.str().c_str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, games3v3.str().c_str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, games3v3solo.str().c_str(), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES);
        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_INTERACT_1, "Spectate Specific Player.", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC, "", 0, true); */

        soloQpage = false;
        normalPage = false;
        
        if (pPlayer->IsMounted())
        {
            pPlayer->Dismount();
            pPlayer->RemoveAurasByType(SPELL_AURA_MOUNTED);
        }
        
        pPlayer->ADD_GOSSIP_ITEM(-1, "|TInterface\\icons\\Achievement_Arena_2v2_7:20:20:-10:0|tSpectate 2v2 games", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
        pPlayer->ADD_GOSSIP_ITEM(-1, "|TInterface\\icons\\Achievement_Arena_3v3_7:20:20:-10:0|tSpectate 3v3 games", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
        pPlayer->ADD_GOSSIP_ITEM(-1, "|TInterface\\icons\\Achievement_Arena_3v3_4:20:20:-10:0|tSpectate solo queue games", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES);
        pPlayer->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\INV_Misc_GroupLooking:20:20:-10:0|tSpectate specific player", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SPECIFIC, "", 0, true);

        pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == NPC_SPECTATOR_ACTION_2V2_GAMES)
        {
            normalPage = true;
            ShowPage(player, action - NPC_SPECTATOR_ACTION_2V2_GAMES, SLOT_2VS2);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action == NPC_SPECTATOR_ACTION_3V3_GAMES)
        {
            normalPage = true;
            ShowPage(player, action - NPC_SPECTATOR_ACTION_3V3_GAMES, SLOT_3VS3);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action == NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES)
        {
            soloQpage = true;
            ShowPage(player, action - NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES, SLOT_SOLO_QUEUE);
            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        }
        else if (action == NPC_SPECTATOR_ACTION_CANCEL)
            player->CLOSE_GOSSIP_MENU();
        else
        {
            if (normalPage)
            {
                uint32 teamid = action - NPC_SPECTATOR_ACTION_SELECTED_PLAYER;
                if (ArenaTeam* team = sArenaTeamMgr->GetArenaTeamById(teamid))
                {
                    if (auto target = team->GetFirstMemberInArena())
                    {
                        ChatHandler handler(player->GetSession());
                        arena_spectator_commands::HandleSpectateCommand(&handler, target->GetName().c_str());
                    }
                }
            }
            else if (soloQpage)
            {
                uint32 guid = action - NPC_SPECTATOR_ACTION_SELECTED_SOLO_PLAYER;

                if (auto target = ObjectAccessor::FindPlayer(guid))
                {
                    ChatHandler handler(player->GetSession());
                    arena_spectator_commands::HandleSpectateCommand(&handler, target->GetName().c_str());
                }
            }
            else
                creature->MonsterWhisper("Something went wrong..", player->GetGUID(), false);
        }
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 uiSender, uint32 uiAction, const char* code)
    {
        player->CLOSE_GOSSIP_MENU();

        if (uiSender == GOSSIP_SENDER_MAIN)
        {
            switch (uiAction)
            {
            case NPC_SPECTATOR_ACTION_SPECIFIC:

                std::string name(code);
                if (!name.empty())
                    name[0] = toupper(name[0]);

                ChatHandler handler(player->GetSession());
                arena_spectator_commands::HandleSpectateCommand(&handler, name.c_str());
                return true;
            }
        }

        return false;
    }
    
    std::string GetSpecClassNameById(uint8 Id, uint32 Spec)
    {
        std::string sClass = "";
        switch (Id)
        {
        case CLASS_WARRIOR:
            switch (Spec)
            {
            case TALENT_TREE_WARRIOR_ARMS: sClass = "AWarrior "; break;
            case TALENT_TREE_WARRIOR_FURY: sClass = "FWarrior "; break;
            case TALENT_TREE_WARRIOR_PROTECTION: sClass = "PWarrior "; break;
            }
            break;
        case CLASS_PALADIN:
            switch (Spec)
            {
            case TALENT_TREE_PALADIN_HOLY: sClass = "HPala "; break;
            case TALENT_TREE_PALADIN_PROTECTION: sClass = "PPala "; break;
            case TALENT_TREE_PALADIN_RETRIBUTION: sClass = "RPala "; break;
            }
            break;
        case CLASS_HUNTER:
            switch (Spec)
            {
            case TALENT_TREE_HUNTER_BEAST_MASTERY: sClass = "BHunt "; break;
            case TALENT_TREE_HUNTER_MARKSMANSHIP: sClass = "MHunt "; break;
            case TALENT_TREE_HUNTER_SURVIVAL: sClass = "SHunt "; break;
            }
            break;
        case CLASS_ROGUE:
            switch (Spec)
            {
            case TALENT_TREE_ROGUE_ASSASSINATION: sClass = "ARogue "; break;
            case TALENT_TREE_ROGUE_COMBAT: sClass = "CRogue "; break;
            case TALENT_TREE_ROGUE_SUBTLETY: sClass = "SRogue "; break;
            }
            break;
        case CLASS_PRIEST:
            switch (Spec)
            {
            case TALENT_TREE_PRIEST_DISCIPLINE: sClass = "DPriest "; break;
            case TALENT_TREE_PRIEST_HOLY: sClass = "HPriest "; break;
            case TALENT_TREE_PRIEST_SHADOW: sClass = "SPriest "; break;
            }
            break;
        case CLASS_DEATH_KNIGHT:
            switch (Spec)
            {
            case TALENT_TREE_DEATH_KNIGHT_BLOOD: sClass = "BDk "; break;
            case TALENT_TREE_DEATH_KNIGHT_FROST: sClass = "FDk "; break;
            case TALENT_TREE_DEATH_KNIGHT_UNHOLY: sClass = "UDk "; break;
            }
            break;
        case CLASS_SHAMAN:
            switch (Spec)
            {
            case TALENT_TREE_SHAMAN_ELEMENTAL: sClass = "ELShaman "; break;
            case TALENT_TREE_SHAMAN_ENHANCEMENT: sClass = "ENShaman "; break;
            case TALENT_TREE_SHAMAN_RESTORATION: sClass = "RShaman "; break;
            }
            break;
        case CLASS_MAGE:
            switch (Spec)
            {
            case TALENT_TREE_MAGE_ARCANE: sClass = "AMage "; break;
            case TALENT_TREE_MAGE_FIRE: sClass = "FIMage "; break;
            case TALENT_TREE_MAGE_FROST: sClass = "FRMage "; break;
            }
            break;
        case CLASS_WARLOCK:
            switch (Spec)
            {
            case TALENT_TREE_WARLOCK_AFFLICTION: sClass = "ALock "; break;
            case TALENT_TREE_WARLOCK_DEMONOLOGY: sClass = "DemoLock "; break;
            case TALENT_TREE_WARLOCK_DESTRUCTION: sClass = "DestLock "; break;
            }
            break;
        case CLASS_DRUID:
            switch (Spec)
            {
            case TALENT_TREE_DRUID_BALANCE: sClass = "BDruid "; break;
            case TALENT_TREE_DRUID_FERAL_COMBAT: sClass = "FDruid "; break;
            case TALENT_TREE_DRUID_RESTORATION: sClass = "RDruid "; break;
            }
            break;
        default:
            sClass = "UNKN"; break;
        }
        return sClass;
    }

    std::string GetGamesStringData(Player* player, uint16 mmr)
    {
        std::string teamsMember[BG_TEAMS_COUNT];
        uint32 firstTeamId = 0;
        if (auto arena = player->GetBattleground())
        {
            for (Battleground::BattlegroundPlayerMap::const_iterator itr = arena->GetPlayers().begin(); itr != arena->GetPlayers().end(); ++itr)
            {
                if (auto member = ObjectAccessor::FindPlayer(itr->first))
                {
                    if (member->IsSpectator())
                        continue;

                    if (member->isGameMaster())
                        continue;

                    uint32 team = itr->second.Team;
                    if (!firstTeamId)
                        firstTeamId = team;
                    
                    teamsMember[firstTeamId == team] += GetSpecClassNameById(member->getClass(), member->GetPrimaryTalentTree(member->GetActiveSpec()));
                }
            }

            std::stringstream ss;
            ss << "[" << mmr << "] " << teamsMember[0] << "vs. " << teamsMember[1];
            std::string data = ss.str();

            return data;
        }
        return "An error occured";
    }

    /*uint32 CountArenaGames(uint8 arType)
    {
        uint16 CountGames = 0;

        for (uint8 i = BATTLEGROUND_NA; i <= BATTLEGROUND_RV; ++i)
        {
            BattlegroundSet bgs = sBattlegroundMgr->GetBattlegroundsByType((BattlegroundTypeId)i);
            for (BattlegroundSet::iterator itr = bgs.begin(); itr != bgs.end(); ++itr)
            {
                Battleground* arena = itr->second;

                if (arena->GetArenaType() == arType && arena->isRated())
                {
                    if (!arena->GetPlayersSize())
                        continue;

                    ++CountGames;
                    return CountGames;
                }
            }
        }
        return 0;
    }*/
    
    void ShowPage(Player* player, uint16 page, SlotStoreType slot)
    {
        uint16 twoGames = 0;
        uint16 threeGames = 0;
        uint16 soloGames = 0;
        bool haveNextPage = false;
        player->PlayerTalkClass->ClearMenus();
        //player->PlayerTalkClass->SendCloseGossip();

        std::map<uint32, InfoBGEntry> bginfomap = sInfoMgr->GetBgStore(slot);
        for (InfoMgr::BGInfoMap::iterator iter = bginfomap.begin(); iter != bginfomap.end(); ++iter)
        {
            Player* member = NULL;

            if (slot == SLOT_2VS2 || slot == SLOT_3VS3)
            {
                ArenaTeam* team1 = sArenaTeamMgr->GetArenaTeamById(iter->second.team1);
                ArenaTeam* team2 = sArenaTeamMgr->GetArenaTeamById(iter->second.team2);

                if (!team1 || !team2)
                    continue;

                member = team1->GetFirstMemberInArena();
            }
            else //if (SLOT_SOLO_QUEUE)
            {
                Battleground* bg = sBattlegroundMgr->GetBattleground(iter->second.instanceID, BattlegroundTypeId(iter->second.type));
                
                if (!bg)
                    continue;
                
                member = bg->GetFirstPlayerInArena();
            }
            
            if (!member)
                continue;

            uint16 highestMMR = std::max(iter->second.MMR1, iter->second.MMR2);

            switch (slot)
            {
            case SLOT_2VS2:
            {
                twoGames++;
                if (twoGames)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(member, highestMMR), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + iter->second.team1);
                break;
            }
            case SLOT_3VS3:
            {
                threeGames++;
                if (threeGames)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(member, highestMMR), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_PLAYER + iter->second.team1);
                break;
            }
            case SLOT_SOLO_QUEUE:
            {
                soloGames++;
                if (soloGames)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, GetGamesStringData(member, highestMMR), GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_SELECTED_SOLO_PLAYER + member->GetGUID());
                break;
            }
            default:
                continue;
            }
        }

        if (page > 0)
        {
            switch (slot)
            {
            case SLOT_2VS2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Prev...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page - 1);
                break;
            case SLOT_3VS3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Prev...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page - 1);
                break;
            case SLOT_SOLO_QUEUE:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Prev...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES + page - 1);
                break;
            default:
                break;
            }
        }

        if (haveNextPage)
        {
            switch (slot)
            {
            case SLOT_2VS2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES + page + 1);
                break;
            case SLOT_3VS3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES + page + 1);
                break;
            case SLOT_SOLO_QUEUE:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Next...", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES + page + 1);
                break;
            default:
                break;
            }
        }

        switch (slot)
        {
        case SLOT_2VS2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_2V2_GAMES);
            break;
        case SLOT_3VS3:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3V3_GAMES);
            break;
        case SLOT_SOLO_QUEUE:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "Refresh", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_3v3_SOLO_GAMES);
            break;
        default:
            break;
        }

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Close", GOSSIP_SENDER_MAIN, NPC_SPECTATOR_ACTION_CANCEL);
    }
    private:
        bool soloQpage;
        bool normalPage;
};

class Arena_Scripts : public PlayerScript
{
public:
    Arena_Scripts() : PlayerScript("Arena_Scripts") {  }

    void OnUpdateZone(Player* player, uint32 zone, uint32 area)
    {
        if (auto bg = player->GetBattleground())
        {
            if (bg->isArena() && player->IsSpectator())
            {
                WorldPacket status;
                sBattlegroundMgr->BuildBattlegroundStatusPacket(&status, bg, player, 0, STATUS_IN_PROGRESS, bg->GetRemainingTime(), bg->GetElapsedTime(), bg->GetArenaType());
                player->GetSession()->SendPacket(&status);
            }
        }
    }
};

void AddSC_arena_spectator_script()
{
    new arena_spectator_commands();
    new npc_arena_spectator();
    new Arena_Scripts;
}