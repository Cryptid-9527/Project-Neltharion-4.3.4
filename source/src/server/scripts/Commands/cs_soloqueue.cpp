#include "SoloQueue.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "BattlegroundMgr.h"

class soloqueue_commandscript : public CommandScript
{
public:
    soloqueue_commandscript() : CommandScript("soloqueue_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> soloQueueCommandTable =
        {
            { "join",   SEC_CONSOLE,     false,  &HandleSoloQueueJoinCommand,    "" },
            { "leave",  SEC_CONSOLE,     false,  &HandleSoloQueueLeaveCommand,   "" },
            { "stats",  SEC_CONSOLE,     false,  &HandleSoloQueueStatsCommand,   "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "soloqueue",  SEC_CONSOLE, false,   NULL, "", soloQueueCommandTable },
        };
        return commandTable;
    }

    static bool HandleSoloQueueJoinCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (auto player = handler->GetSession()->GetPlayer())
        {
            if (!sWorld->getBoolConfig(CONFIG_ARENA_SOLO_QUEUE_ENABLED))
            {
                handler->SendSysMessage("Solo queue is currently disabled!");
                return true;
            }

            if (ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_5v5))))
            {
                if (!at->IsSoloQueueTeam())
                {
                    handler->SendSysMessage("Your 5v5 team is not a solo queue team!");
                    return true;
                }
            }
            else
            {
                handler->SendSysMessage("You cant join solo queue without a solo queue team!");
                return true;
            }
            sSoloQueueMgr->AddPlayer(player);
        }
        return true;
    }

    static bool HandleSoloQueueLeaveCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!sWorld->getBoolConfig(CONFIG_ARENA_SOLO_QUEUE_ENABLED))
        {
            handler->SendSysMessage("Solo queue is currently disabled!");
            return true;
        }

        if (auto player = handler->GetSession()->GetPlayer())
        {
            if (sSoloQueueMgr->RemovePlayer(player->GetGUID()))
            {
                player->RemoveBattlegroundQueueId(BATTLEGROUND_QUEUE_3v3_SOLO);
            }
            else if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_1v1_SOLO))
            {
                if (auto bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA))
                {
                    WorldPacket data;
                    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, player->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_1v1_SOLO), STATUS_NONE, player->GetBattlegroundQueueJoinTime(BATTLEGROUND_AA), 0, ARENA_TYPE_1v1_SOLO);
                    player->GetSession()->SendPacket(&data);
                }

                BattlegroundQueue& queue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_1v1_SOLO);
                queue.RemovePlayer(player->GetGUID(), false);
                player->RemoveBattlegroundQueueId(BATTLEGROUND_QUEUE_1v1_SOLO);
            }
            else if (player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_3v3_SOLO))
            {
                if (auto bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA))
                {
                    WorldPacket data;
                    sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, player, player->GetBattlegroundQueueIndex(BATTLEGROUND_QUEUE_3v3_SOLO), STATUS_NONE, player->GetBattlegroundQueueJoinTime(BATTLEGROUND_AA), 0, ARENA_TYPE_3v3_SOLO);
                    player->GetSession()->SendPacket(&data);
                }

                BattlegroundQueue& queue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_3v3_SOLO);
                queue.RemovePlayer(player->GetGUID(), false);
                player->RemoveBattlegroundQueueId(BATTLEGROUND_QUEUE_3v3_SOLO);
            }
            handler->SendSysMessage("You are no longer listed for solo queue!");
        }
        return true;
    }

    static bool HandleSoloQueueStatsCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (!sWorld->getBoolConfig(CONFIG_ARENA_SOLO_QUEUE_ENABLED))
        {
            handler->SendSysMessage("Solo queue is currently disabled!");
            return true;
        }

        std::stringstream infoQueue;
        infoQueue << "Solo queue statistics:\n";
        BattlegroundQueue const& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(BATTLEGROUND_QUEUE_3v3_SOLO);
        infoQueue << "Solo teams in queue: " << bgQueue.GetQueuedGroups() << "\n";
        if (sWorld->getBoolConfig(CONFIG_SOLO_QUEUE_DPS_RESTRICTION))
        {
            infoQueue << "Queued melees: " << sSoloQueueMgr->GetPlayerCountInQueue(TALENT_CAT_MELEE) << "\n";
            infoQueue << "Queued casters: " << sSoloQueueMgr->GetPlayerCountInQueue(TALENT_CAT_RANGE) << "\n";
        }
        else
            infoQueue << "Queued dps: " << sSoloQueueMgr->GetPlayerCountInQueue(TALENT_CAT_DPS) << "\n";
        infoQueue << "Queued healers: " << sSoloQueueMgr->GetPlayerCountInQueue(TALENT_CAT_HEALER) << "\n";
        handler->SendSysMessage(infoQueue.str().c_str());
        return true;
    }
};

void AddSC_soloqueue_commandscript()
{
    new soloqueue_commandscript();
}
