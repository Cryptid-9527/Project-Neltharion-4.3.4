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

#include "halls_of_origination.h"
#include "GameObjectAI.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

struct go_halls_of_origination_transit_device : public GameObjectAI
{
    go_halls_of_origination_transit_device(GameObject* go) : GameObjectAI(go) { }

    bool CanBeSeenBy(Unit* who) override
    {
        if (auto script = go->GetInstanceScript())
            if (script->GetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR) != DONE)
                return false;

        return true;
    }

    bool GossipHello(Player* player) override
    {
        if (player->isInCombat())
            return true;

        if (auto script = go->GetInstanceScript())
            if (script->GetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR) != DONE)
                return true;

        return false;
    }
};

#define GOSSIP_SENDER_PORT 1000

enum Elevator
{
    EVENT_REENABLE = 1,
};

struct npc_earthrager_tomb_aggro_stalker : public ScriptedAI
{
    npc_earthrager_tomb_aggro_stalker(Creature* creature) : ScriptedAI(creature) { }

    uint8 summonGroupId = 0;
    bool hasSummoned = false;

    void Reset() override
    {
        if (me->GetPositionY() > 60.f)
            summonGroupId = 1;
        else if (me->GetPositionY() < 10.f)
            summonGroupId = 3;
        else
            summonGroupId = 2;
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!hasSummoned && who->IsPlayer() && me->GetDistance(who) < 25.f)
        {
            std::list<TempSummon*> summonList;
            me->SummonCreatureGroup(summonGroupId, &summonList);

            for (TempSummon* summon : summonList)
                if (summon->Attack(who, true))
                    summon->GetMotionMaster()->MoveChase(who);

            hasSummoned = true;
        }
    }
};

// 84032
class spell_temple_fireshaper_meteor : public SpellScript
{
    PrepareSpellScript(spell_temple_fireshaper_meteor);

    void CountTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size())
            for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end();)
            {
                if (targets.size())
                    if ((*itr))
                        if (!(*itr)->ToPlayer())
                            targets.erase(itr++);
                        else
                            ++itr;
            }
        targetCount = targets.size();
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        SetHitDamage(GetHitDamage() / float(targetCount));
    }
    void FilterTargets(std::list<WorldObject*>& targets)
    {
    }
    void Register()
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_temple_fireshaper_meteor::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_temple_fireshaper_meteor::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    uint32 targetCount = 0;
};


struct go_hoo_anhur_lever : public GameObjectAI
{
    go_hoo_anhur_lever(GameObject* go) : GameObjectAI(go) { }

    bool GossipHello(Player* player) override
    {
        player->CastSpell(player, GetSpell());
        return true;
    }

    uint32 GetSpell()
    {
        switch (go->GetEntry())
        {
        case GOBJ_LEVER_LEFT_NM:
        case GOBJ_LEVER_LEFT_HC:
            return SPELL_LEVER_LEFT;
            break;
        case GOBJ_LEVER_RIGHT_NM:
        case GOBJ_LEVER_RIGHT_HC:
            return SPELL_LEVER_RIGHT;
            break;
        default: 
            TC_LOG_ERROR("sql.sql", "Gameobject other than 4 known entries for HOO_ANHUR lever: %u", go->GetEntry());
            return 0;
            break;
        }
    }

private:
    EventMap events;
};

enum elevator
{
    ACTION_ELEVATOR_UP = 2,
    ACTION_ELEVATOR_DOWN = 3,
};

struct go_hoo_makers_lift_controller : public GameObjectAI
{
    go_hoo_makers_lift_controller(GameObject* go) : GameObjectAI(go) { }


    void DoAction(int32 const action)
    {
        switch (action)
        {
        case ACTION_ELEVATOR_UP:
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            events.ScheduleEvent(EVENT_REENABLE, 11000);
            break;
        case ACTION_ELEVATOR_DOWN:
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            events.ScheduleEvent(EVENT_REENABLE, 19333);
            break;
        }
    }

    bool GossipHello(Player* player) override
    {
        if (player->isGameMaster() || go->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE))
        {
            TC_LOG_ERROR("sql.sql", "HOO ELEVATOR CONTROLLER: player %u is either a gamemaster or selected object with GO_FLAG_NOT_SELECTABLE active.", player->GetGUID());
            return false;
        }

        InstanceScript* script = player->GetInstanceScript();
        if (!script)
        {
            TC_LOG_ERROR("sql.sql", "HOO MAIN ELEVATOR: Could not get instance script for player %u", player->GetGUID());
            return false;
        }

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bring elevator to first floor", GOSSIP_SENDER_PORT, 0);

        if (script->GetBossState(DATA_ANRAPHET) == DONE && script->GetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR) == DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bring elevator to second floor", GOSSIP_SENDER_PORT, 1);

        
        if (script->GetBossState(DATA_RAJH) == DONE)
            if (script->GetBossState(DATA_AMMUNAE) == DONE)
                if (script->GetBossState(DATA_SETESH) == DONE)
                    if (script->GetBossState(DATA_ISISET) == DONE)
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bring elevator to third floor", GOSSIP_SENDER_PORT, 2);
        

        player->SEND_GOSSIP_MENU(1, go->GetGUID());

        return true;
    }

    bool GossipSelect(Player* player, uint32 sender, uint32 action) override
    {
        if (player->isGameMaster() || go->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE))
        {
            TC_LOG_ERROR("sql.sql", "HOO ELEVATOR CONTROLLER: player %u is either a gamemaster or selected object with GO_FLAG_NOT_SELECTABLE active.", player->GetGUID());
            return false;
        }

        if (auto script = go->GetInstanceScript())
        {
            //if (auto go2 = script->GetGameObject(script->GetData64(GO_LIFT_OF_THE_MAKER
            if (auto go2 = go->FindNearestGameObject(GO_LIFT_OF_THE_MAKER, 500.f))
            {
                //go2->SetTransportState(action);
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                auto players = go->GetPlayersInRange(40.f, true);
                if (players.size())
                    for (Player* player : players)
                        player->CLOSE_GOSSIP_MENU();

                //TC_LOG_ERROR("sql.sql", "sender: %u, actioN: %u", sender, action);
                switch (action)
                {
                case 0:
                case 2:
                    go2->AI()->DoAction(ACTION_ELEVATOR_DOWN);
                    break;
                case 1:
                    go2->AI()->DoAction(ACTION_ELEVATOR_UP);//doesn't seem to work. just seems to go "next" with GO_STATE_TRANSPORT_SPEC
                    break;
                }
            }
            else TC_LOG_ERROR("sql.sql", "HOO MAIN ELEVATOR: Could not find elevator object for player %u", player->GetGUID());
        }
        else TC_LOG_ERROR("sql.sql", "HOO MAIN ELEVATOR: COULD NOT FIND INSTANCE SCRIPT FOR PLAYER %u", player->GetGUID());

        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        return false;
    }
    void UpdateAI(uint32 diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
            case EVENT_REENABLE:
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                break;
            }
    }
private:
    EventMap events;
};


class go_HOO_elevator : public GameObjectScript
{
public:
    go_HOO_elevator() : GameObjectScript("go_HOO_elevator") { }

    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_HOO_elevatorAI(go);
    }
    struct go_HOO_elevatorAI : public GameObjectAI
    {
        go_HOO_elevatorAI(GameObject* go) : GameObjectAI(go)
        {
            go->SetControlableTransport(true);
            go->SetGoState(GO_STATE_TRANSPORT_SPEC);
        }


        void DoAction(int32 const action)
        {
            switch (action)
            {
            case ACTION_ELEVATOR_UP:
            {
                //TC_LOG_ERROR("sql.sql", "ACTION_ELEVATOR_UP");
                //go->SetGoState(GO_STATE_TRANSPORT_READY);

                /*
                
            m_goValue.Transport.PathProgress = goinfo->transport.startOpen ? GetGOInfo()->transport.pause : GetGOInfo()->transport.startPos;
            m_goValue.Transport.CurrSeg = goinfo->transport.startOpen ? GetGOInfo()->transport.pause : GetGOInfo()->transport.startPos;
            m_goValue.Transport.AnimInfo = TransportAnimation::GetTransportAnimation(goinfo->entry);
            SetByteValue(GAMEOBJECT_BYTES_1, 0, goinfo->transport.startOpen ? GO_STATE_TRANSPORT_READY : GO_STATE_TRANSPORT_SPEC);
            SetUInt32Value(GAMEOBJECT_LEVEL, getMSTime());
                */
                //go->SetByteValue(GAMEOBJECT_BYTES_1, 3, 0);
                go->SetUInt32Value(GAMEOBJECT_LEVEL, getMSTime() + 10000);//Time until next position
                /*
                    27 = instant freeze
                    26 = instant freeze
                    25 = up
                    24 = down
                    23 = auto, freezes client at timer (GAMEOBJECT_LEVEL) expiration
                    22 = nothing
                    21 = auto, freezes client at timer (GAMEOBJECT_LEVEL) expiration
                    20 = nothing
                    19 = nothing
                */
                go->SetByteValue(GAMEOBJECT_BYTES_1, 0, 25);//position id

                sScriptMgr->OnGameObjectStateChanged(go, GO_STATE_TRANSPORT_READY);


                std::list<GameObject*> objs;
                GetGameObjectListWithEntryInGrid(objs, go, 207669, 1000.f);
                if (objs.size())
                    for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                        if (auto panel = (*itr))
                            panel->AI()->DoAction(ACTION_ELEVATOR_UP);

                break;
            }
            case ACTION_ELEVATOR_DOWN:
            {
                //TC_LOG_ERROR("sql.sql", "ACTION_ELEVATOR_DOWN");
                go->SetUInt32Value(GAMEOBJECT_LEVEL, getMSTime() + 18333);
                go->SetByteValue(GAMEOBJECT_BYTES_1, 0, GO_STATE_TRANSPORT_SPEC);
                sScriptMgr->OnGameObjectStateChanged(go, GO_STATE_TRANSPORT_SPEC);

                std::list<GameObject*> objs;
                GetGameObjectListWithEntryInGrid(objs, go, 207669, 1000.f);
                if (objs.size())
                    for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                        if (auto panel = (*itr))
                            panel->AI()->DoAction(ACTION_ELEVATOR_DOWN);
                break;
            }
            default:
                break;
            }
        }
    private:
        EventMap events;
    };
};

void AddSC_halls_of_origination()
{
    RegisterGameObjectAI(go_halls_of_origination_transit_device);
    RegisterGameObjectAI(go_hoo_makers_lift_controller);
    RegisterGameObjectAI(go_hoo_anhur_lever);
    RegisterCreatureAI(npc_earthrager_tomb_aggro_stalker);
    RegisterSpellScript(spell_temple_fireshaper_meteor);

    new go_HOO_elevator();
}
