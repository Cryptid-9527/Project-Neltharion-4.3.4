#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "endTime.h"
#include "Spell.h"
#include "GameObjectAI.h"


class go_end_time_teleporter : public GameObjectScript
{
public:
    go_end_time_teleporter() : GameObjectScript("go_end_time_teleporter")
    {
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();
        uint32 _combi = instance->GetData(DATA_RANDOM_BOSS_SELECTION);

        if (player->isInCombat())
            return true;

        if (InstanceScript *instance = go->GetInstanceScript())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Back to Nozdormu", GOSSIP_SENDER_MAIN, START_TELEPORT);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, combi[_combi].text1, GOSSIP_SENDER_MAIN, combi[_combi].boss1);

            if (instance->IsDone(combi[_combi].eb1))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, combi[_combi].text2, GOSSIP_SENDER_MAIN, combi[_combi].boss2);

            if (instance->IsDone(combi[_combi].eb1) && instance->IsDone(combi[_combi].eb2))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bronze Dragonshrine", GOSSIP_SENDER_MAIN, DRAKE_SANCTUM_BRONZE);
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (player->isInCombat())
            return true;
        player->CastSpell(player, action, true);

        if (auto instance = go->GetInstanceScript())
            if (action == DRAKE_SANCTUM_AZUR && instance->GetData(DATA_FRAGMENTS) < 16)
                instance->SetData(DATA_JAINA_PICKED_STATE, IN_PROGRESS);

        return true;
    }
};

void AddSC_end_time_teleporter()
{
    new go_end_time_teleporter();
}
