/*
 * Copyright (C) 2010 - 2012 ProjectSkyfire <http://www.projectskyfire.org/>
 *
 * Copyright (C) 2011 - 2012 ArkCORE <http://www.arkania.net/>
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

#include "InstanceScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "throne_of_the_tides.h"

#define ENCOUNTERS 5

Position const AquaEvent[11] =
{
    {-406.363708f, 840.636108f, 265.319763f, 4.623423f},
    {-418.183716f, 846.622314f, 265.459961f, 5.047527f},
    {-386.503174f, 771.906982f, 265.832428f, 2.117997f},
    {-405.178345f, 770.422424f, 265.216675f, 1.717444f},
    {-18.99f, 802.45f, 808.5f, 3.14745329f},
    {-134.99f, 820.45f, 803.0f, 0.01745329f},
    {-134.00f, 780.45f, 803.0f, 0.01745329f},
    {-97.99f, 820.45f, 803.0f, 0.01745329f},
    {-97.04f, 780.45f, 803.0f, 0.01745329f},
    {-68.99f, 820.45f, 803.0f, 0.01745329f},
    {-68.00f, 780.45f, 803.0f, 0.01745329f}
};

class instance_throne_of_the_tides: public InstanceMapScript
{
public:
    instance_throne_of_the_tides() :
        InstanceMapScript("instance_throne_of_the_tides", 643) {}

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_throne_of_the_tides_InstanceMapScript(map);
    }

    struct instance_throne_of_the_tides_InstanceMapScript : public InstanceScript
    {
        instance_throne_of_the_tides_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(ENCOUNTERS);
        }

        void Initialize()
        {
            uiLadyNazjar = 0;
            uiCommanderUlthok = 0;
            uiErunakStonespeaker = 0;
            uiMindbenderGhrusha = 0;
            uiNeptulon = 0;
            uiOzumat = 0;
            uiProgressEvent = 0;
            uiLadynazjarIntro = 0;
            go_abyssal = 0;
            go_abyssal2 = 0;
            go_abyssal3 = 0;
            go_abyssal4 = 0;
            go_corail = 0;
            go_console = 0;
            go_ozumat_chest = 0;
            go_tentacle_left = 0;
            go_tentacle_right = 0;
            go_wall_tentacle_left = 0;
            go_wall_tentacle_right = 0;
            uiBossOzumatEvent = 0;

            startUlthorEvent = false;
            mui_timerUlthokEvent = 5000;

            startAreaTriggerEvent = false;
            mui_timerAreaTriggerEvent = 3600000;

            mui_timerAreaTriggerEvent = 3600000;
            startAreaTriggerEvent = false;

            mui_voidripTimer = 1000;

            TeamInInstance = 0;
            tainted_sentry_dead = 0;
            for (uint8 i = 0; i < ENCOUNTERS; ++i)
                uiEncounter[i] = NOT_STARTED;
        }

        void OnPlayerEnter(Player* player)
        {
            if (!TeamInInstance)
                TeamInInstance = player->GetTeam();
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < ENCOUNTERS; ++i)
            {
                if (uiEncounter[i] == IN_PROGRESS)
                    return true;
            }
            return false;
        }

        void OnCreatureRemove(Creature* creature)
        {
            switch (creature->GetEntry())
            {
            case NPC_FACELESS_SAPPER:
            case BOSS_OZUMAT:
                SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
                break;
            }
        }

        void OnCreatureCreate(Creature* pCreature)
        {
            InstanceScript::OnCreatureCreate(pCreature);

            if (!TeamInInstance)
            {
                Map::PlayerList const& players = instance->GetPlayers();
                if (!players.isEmpty())
                    if (auto player = players.begin()->getSource())
                        TeamInInstance = player->GetTeam();
            }

            switch (pCreature->GetEntry())
            {
            case BOSS_LADY_NAZJAR:
                uiLadyNazjar = pCreature->GetGUID();
                break;
            case BOSS_COMMANDER_ULTHOK:
                uiCommanderUlthok = pCreature->GetGUID();
                break;
            case BOSS_ERUNAK_STONESPEAKER:
                uiErunakStonespeaker = pCreature->GetGUID();
                break;
            case BOSS_MINDBENDER_GHURSHA:
                uiMindbenderGhrusha = pCreature->GetGUID();
                break;
            case BOSS_OZUMAT:
                uiOzumat = pCreature->GetGUID();
                break;
            case BOSS_RP_OZUMAT:
                uiBossOzumatEvent = pCreature->GetGUID();
                break;
            case BOSS_NEPTULON:
                uiNeptulon = pCreature->GetGUID();
                break;
            case NPC_CINEMATIC_TRIGGER:
                cinematicTrigger.push_back(pCreature->GetGUID());
                break;
            case NPC_CAPTAIN_TAYLOR:
                if (TeamInInstance == HORDE)
                    pCreature->UpdateEntry(NPC_LEGIONNAIRE_NAZGRIM);
                instance->SummonCreature(NPC_TOT_TELEPORTER_F1, teleportersPOS[0]);
                instance->SummonCreature(NPC_TOT_TELEPORTER_F2, teleportersPOS[1]);
                break;
            case NPC_LADY_NAZJAR_INTRO:
                uiLadynazjarIntro = pCreature->GetGUID();
                if (!pCreature->isAlive())
                    SetData(DATA_LADY_NAZJAR_INTRO, DONE);
                break;
            case NPC_GILGOBLIN_AQUAMAGE:
            case NPC_GILGOBLIN_HUNTER:
            case NPC_FACELESS_WATCHER:
            case NPC_TAINTED_SENTRY:
                wingTrash.push_back(pCreature->GetGUID());
                 pCreature->SetPhaseMask(IsDone(DATA_COMMANDER_ULTHOK) ? 0x1 : 0x2, true);
                break;
            default:
                break;
            }
        }

        void OnUnitDeath(Unit* unit)
        {
            InstanceScript::OnUnitDeath(unit);
            switch (unit->GetEntry())
            {
            case NPC_NAZJAR_SPIRITMENDER:
            case NPC_NAZJAR_INTRO_INVADER:
                if (GetData(DATA_PROGRESS_EVENT) == 2)
                    if (auto c = unit->ToCreature())
                        if (c->GetHomePosition().GetPositionZ() > 725.f)    //make sure this is on the right floor
                            if (auto lady = c->FindNearestCreature(NPC_LADY_NAZJAR_INTRO, 200.0f, true))
                                lady->AI()->DoAction(ACTION_LADY_NAZJAR_INTRO_COUNTER);

                break;
            case NPC_TAINTED_SENTRY:
                tainted_sentry_dead++;
                if (tainted_sentry_dead >= 4)
                {
                    if (auto go = instance->GetGameObject(go_abyssal4))
                        HandleGameObject(NULL, true, go);
                }
                break;
            case BOSS_OZUMAT:
            case NPC_FACELESS_SAPPER:
                SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, unit);
                break;
            default:
                //TC_LOG_ERROR("sql.sql", "Unit Death, but entry is not spirit mender or intro invader.");
                break;
            }
        }


        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
            case GO_ABYSSAL_MAW_01:
                go_abyssal = go->GetGUID();
                HandleGameObject(go_abyssal, true, go);
                break;
            case GO_ABYSSAL_MAW_02:
                go_abyssal2 = go->GetGUID();
                HandleGameObject(go_abyssal2, true, go);
                break;
            case GO_ABYSSAL_MAW_03:
                go_abyssal3 = go->GetGUID();
                if (GetData(DATA_COMMANDER_ULTHOK) == DONE)
                    HandleGameObject(go_abyssal3, true, go);
                else
                    HandleGameObject(go_abyssal3, false, go);
                break;
            case GO_ABYSSAL_MAW_04:
                go_abyssal4 = go->GetGUID();
                HandleGameObject(go_abyssal4, (GetData(DATA_COMMANDER_ULTHOK) == DONE), go);
                break;
            case GO_CORAIL:
                go_corail = go->GetGUID();
                break;
            case GO_CONSOLE:
                go_console = go->GetGUID();
                HandleGameObject(go_console, false, go);
                break;
            case GO_OZUMAT_CHEST_NORMAL:
            case GO_OZUMAT_CHEST_HEROIC:
                go_ozumat_chest = go->GetGUID();
                //                HandleGameObject(go_ozumat_chest, false, go);
                break;
            case GO_TENTACLE_LEFT:
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                go_tentacle_left = go->GetGUID();
                if (GetData(DATA_LADY_NAZJAR) == DONE)
                    go->Delete();
                break;
            case GO_TENTACLE_RIGHT:
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                go_tentacle_right = go->GetGUID();
                if (GetData(DATA_LADY_NAZJAR) == DONE)
                    go->Delete();
                break;
            case GO_WALL_TENTACLE_LEFT:
                go_wall_tentacle_left = go->GetGUID();
                if (GetData(DATA_LADY_NAZJAR) == DONE)
                    HandleGameObject(go_wall_tentacle_left, true, go);
                else
                    HandleGameObject(go_wall_tentacle_left, false, go);
                break;
            case GO_WALL_TENTACLE_RIGHT:
                go_wall_tentacle_right = go->GetGUID();
                if (GetData(DATA_LADY_NAZJAR) == DONE)
                    HandleGameObject(go_wall_tentacle_right, true, go);
                else
                    HandleGameObject(go_wall_tentacle_right, false, go);
                break;
            }
        }

        uint64 GetData64(uint32 identifier) const
        {
            switch (identifier)
            {
            case DATA_LADY_NAZJAR:
                return uiLadyNazjar;
            case DATA_COMMANDER_ULTHOK:
                return uiCommanderUlthok;
            case DATA_ERUNAK_STONESPEAKER:
                return uiErunakStonespeaker;
            case DATA_MINDBENDER_GHURSHA:
                return uiMindbenderGhrusha;
            case DATA_OZUMAT:
                return uiOzumat;
            case DATA_NEPTULON:
                return uiNeptulon;
            case GO_ABYSSAL_MAW_01:
                return go_abyssal;
            case GO_ABYSSAL_MAW_02:
                return go_abyssal2;
            case GO_ABYSSAL_MAW_03:
                return go_abyssal3;
            case GO_ABYSSAL_MAW_04:
                return go_abyssal4;
            case GO_CORAIL:
                return go_corail;
            case GO_CONSOLE:
                return go_console;
            case GO_OZUMAT_CHEST_NORMAL:
            case GO_OZUMAT_CHEST_HEROIC:
                return go_ozumat_chest;
            case GO_TENTACLE_LEFT:
                return go_tentacle_left;
            case GO_TENTACLE_RIGHT:
                return go_tentacle_right;
            case GO_WALL_TENTACLE_LEFT:
                return go_wall_tentacle_left;
            case GO_WALL_TENTACLE_RIGHT:
                return go_wall_tentacle_right;
            case BOSS_OZUMAT_EVENT:
                return uiBossOzumatEvent;
            default:
                return 0;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
            case DATA_LADY_NAZJAR:
                uiEncounter[0] = data;
                if (data == NOT_STARTED)
                {
                    if (auto go = instance->GetGameObject(go_console))
                        HandleGameObject(go_console, false, go);
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, true, go);
                }
                else if (data == IN_PROGRESS)
                {
                    if (auto go = instance->GetGameObject(go_console))
                        HandleGameObject(go_console, true, go);
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, false, go);
                }
                else if (data == DONE)
                {
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, false, go);
                }
                break;
            case DATA_COMMANDER_ULTHOK:
                uiEncounter[1] = data;
                if (data == NOT_STARTED)
                {
                    if (auto go = instance->GetGameObject(go_abyssal))
                        HandleGameObject(go_abyssal, true, go);
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, true, go);
                    if (auto go = instance->GetGameObject(go_abyssal3))
                        HandleGameObject(go_abyssal3, true, go);
                }
                else if (data == IN_PROGRESS)
                {
                    if (auto go = instance->GetGameObject(go_abyssal))
                        HandleGameObject(go_abyssal, false, go);
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, false, go);
                    if (auto go = instance->GetGameObject(go_abyssal4))
                        HandleGameObject(go_abyssal4, false, go);
                    if (auto go = instance->GetGameObject(go_abyssal3))
                        HandleGameObject(go_abyssal3, false, go);
                }
                else if (data == DONE)
                {
                    if (auto go = instance->GetGameObject(go_abyssal))
                        HandleGameObject(go_abyssal, true, go);
                    if (auto go = instance->GetGameObject(go_abyssal2))
                        HandleGameObject(go_abyssal2, true, go);
                    if (auto go = instance->GetGameObject(go_abyssal4))
                        HandleGameObject(go_abyssal4, false, go);
                    if (auto go = instance->GetGameObject(go_abyssal3))
                        HandleGameObject(go_abyssal3, true, go);

                    if (IsDone(DATA_COMMANDER_ULTHOK))
                        for (uint64 trashGUID : wingTrash)
                            if (auto trashMob = instance->GetCreature(trashGUID))
                                trashMob->SetPhaseMask(0x1, true);
                }
                break;
            case DATA_ERUNAK_STONESPEAKER:
                uiEncounter[2] = data;
                break;
            case DATA_MINDBENDER_GHURSHA:
                uiEncounter[3] = data;
                break;
            case DATA_OZUMAT:
                uiEncounter[4] = data;
                if (data == DONE)
                    if (!InstanceScript::IsChallengeModeStarted() && !InstanceScript::IsChallengeModeFinished())
                        if (auto go = instance->GetGameObject(go_ozumat_chest))
                            go->SetRespawnTime(go->GetRespawnDelay());
                break;
            case DATA_PROGRESS_EVENT:
                uiProgressEvent = data;
                switch (uiProgressEvent)
                {
                case 1:
                {
                    const Position intro_jump[4]
                    {
                        {-411.89f, 810.28f, 249.0f, 3.14f},
                        {-408.20f, 807.12f, 249.0f, 3.14f},
                        {-410.44f, 798.90f, 249.0f, 3.14f},
                        {-407.68f, 803.43f, 249.0f, 3.14f}
                    };

                    for (auto i = 0; i < 4; ++i)
                        if (auto c = instance->SummonCreature((i == 0 || i == 2) ? uint32(NPC_NAZJAR_INTRO_INVADER) : uint32(NPC_NAZJAR_SPIRITMENDER), AquaEvent[i]))
                        {
                            c->SetHover(false);
                            c->DisableMovementFlagUpdate(true);
                            c->SetCanFly(true);
                            c->SetDisableGravity(true);
                            c->GetMotionMaster()->Clear();

                            c->AddAura(SPELL_WATER_JUMP_VISUAL, c);
                            c->SetReactState(REACT_PASSIVE);

                            c->GetMotionMaster()->MoveJump(intro_jump[i], 12.f, 20.f, 554);
                        }
                    mui_timerAreaTriggerEvent = 2800;
                    startAreaTriggerEvent = true;
                    break;
                }
                case 2:
                {
                        const Position upstairs_spawns[6]
                        {
                            { -134.0f,  804.0f, 797.0f, 0.f },
                            { -134.0f,  799.0f, 797.0f, 0.f },
                            { -97.0f,   804.0f, 797.0f, 0.f },
                            { -97.0f,   799.0f, 797.0f, 0.f },
                            {-68.0f,    804.0f, 797.0f, 0.f },
                            {-68.0f,    799.0f, 797.0f, 0.f }
                        };

                        for (auto i = 0; i < 6; ++i)
                            if (auto c = instance->SummonCreature((i == 0 || i == 3 || i == 4) ? uint32(NPC_NAZJAR_INTRO_INVADER) : uint32(NPC_NAZJAR_SPIRITMENDER), AquaEvent[i+5]))
                            {
                                c->SetHover(false);
                                c->DisableMovementFlagUpdate(true);
                                c->SetCanFly(true);
                                c->SetDisableGravity(true);
                                c->GetMotionMaster()->Clear();

                                c->AddAura(SPELL_WATER_JUMP_VISUAL, c);
                                c->SetReactState(REACT_PASSIVE);

                                c->GetMotionMaster()->MoveJump(upstairs_spawns[i], 15.f, 15.f, 553);
                            }

                        if (auto ladyNazjarIntro = instance->GetCreature(uiLadynazjarIntro))
                            ladyNazjarIntro->AI()->DoAction(ACTION_LADY_NAZJAR_INTRO_START);
                        mui_timerAreaTriggerEvent = 1500;
                        startAreaTriggerEvent = true;
                        break;
                    }
                    case 3:
                    {
                        /*for (std::list<uint64 >::const_iterator itr = cinematicTrigger.begin(); itr != cinematicTrigger.end(); itr++)
                            if (Creature *c = instance->GetCreature(*itr))
                                c->CastSpell(c, 85170, true);
                        if (Creature *c = instance->GetCreature(uiBossOzumatEvent))
                        {
                            c->CastSpell(c, 85170, true);
                            mui_timerCinematicEvent = 15000;
                            startCinematicEvent = true;
                        }*/
                        if (auto go = instance->GetGameObject(GetData64(GO_ABYSSAL_MAW_02)))
                            HandleGameObject(GetData64(GO_ABYSSAL_MAW_02), true, go);
                        break;
                    }
                    default :
                        break;
                }
                break;
            case DATA_EVENT_COMMANDER_ULTHOK_START:
                startUlthorEvent = true;
                break;
            case DATA_LADY_NAZJAR_INTRO:
                break;
            default :
                break;
            }
            if (type != DATA_PROGRESS_EVENT)
                if (data == DONE)
                {
                    OUT_SAVE_INST_DATA;

                    std::ostringstream saveStream;
                    saveStream << "T T" << ' ' << uiEncounter[0] << ' ' << uiEncounter[1] << ' ' << uiEncounter[2] << ' '
                        << uiEncounter[3] << uiEncounter[4];

                    strInstData = saveStream.str();

                    SaveToDB();
                    OUT_SAVE_INST_DATA_COMPLETE;
                }
        }

        uint32 GetData(uint32 type) const
        {

            switch (type)
            {
                case DATA_LADY_NAZJAR_EVENT:
                case DATA_COMMANDER_ULTHOK_EVENT:
                case DATA_ERUNAK_STONESPEAKER_EVENT:
                case DATA_MINDBENDER_GHURSHA_EVENT:
                case DATA_OZUMAT_EVENT:
                    return uiEncounter[type];
                case DATA_PROGRESS_EVENT:
                    return uiProgressEvent;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            return strInstData;
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> uiEncounter[0] >> uiEncounter[1] >> uiEncounter[2] >> uiEncounter[3] >> uiEncounter[4];
            if (dataHead1 == 'T' && dataHead2 == 'T')
            {
                for (uint8 i = 0; i < ENCOUNTERS; ++i)
                {
                    if (uiEncounter[i] == IN_PROGRESS)
                        uiEncounter[i] = NOT_STARTED;
                }
            }
            else OUT_LOAD_INST_DATA_FAIL;
            OUT_LOAD_INST_DATA_COMPLETE;
        }

        void Update(uint32 diff)
        {
            if (startAreaTriggerEvent)
            {
                if (mui_timerAreaTriggerEvent <= diff)
                {
                    mui_timerAreaTriggerEvent = 3600000;
                    startAreaTriggerEvent = false;
                }
                else
                    mui_timerAreaTriggerEvent -= diff;
            }

            if (startUlthorEvent)
            {
                if (mui_timerUlthokEvent <= diff)
                {
                    if (!instance->GetCreature(uiCommanderUlthok))
                    {
                        bool start_event = true;
                        Map::PlayerList const &PlayerList = instance->GetPlayers();
                        if (!PlayerList.isEmpty())
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                if (auto player = i->getSource())
                                    if (player->IsWatchingCinematic())
                                    {
                                        start_event = false;
                                        break;
                                    }
                        startUlthorEvent = false;
                    }
                    mui_timerUlthokEvent = 5000;
                }
                else
                    mui_timerUlthokEvent -= diff;
            }
             
            if (GetData(DATA_COMMANDER_ULTHOK) == DONE)
            {
                if (auto go = instance->GetGameObject(go_wall_tentacle_left))
                    go->Delete();
                if (auto go = instance->GetGameObject(go_wall_tentacle_right))
                    go->Delete();
                if (auto go = instance->GetGameObject(go_tentacle_left))
                    go->Delete();
                if (auto go = instance->GetGameObject(go_tentacle_right))
                    go->Delete();
            }

            if (mui_voidripTimer <= diff)
            {
                Position pos;
                Map::PlayerList const& players = instance->GetPlayers();
                for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
                    if (auto player = i->getSource())
                    {
                        player->GetPosition(&pos);
                        if (player->GetPositionZ() >= 310.0f && (player->FindNearestCreature(40936, 30.0f) || player->FindNearestCreature(49074, 30.0f)))
                            player->NearTeleportTo(pos.m_positionX, pos.m_positionY, 310.0f, pos.m_orientation);
                    }
                mui_voidripTimer = 1000;
            }
            else
                mui_voidripTimer -= diff;
        }

    private :
        uint32 uiEncounter[ENCOUNTERS]  {0, 0, 0, 0, 0};
        uint32 uiProgressEvent          {0};

        uint64 uiLadyNazjar             {0};
        uint64 uiCommanderUlthok        {0};
        uint64 uiErunakStonespeaker     {0};
        uint64 uiMindbenderGhrusha      {0};
        uint64 uiOzumat                 {0};
        uint64 uiNeptulon               {0};
        uint64 uiBossOzumatEvent        {0};
        uint64 uiLadynazjarIntro        {0};

        uint64 go_abyssal               {0};
        uint64 go_abyssal2              {0};
        uint64 go_abyssal3              {0};
        uint64 go_abyssal4              {0};
        uint64 go_corail                {0};
        uint64 go_console               {0};
        uint64 go_ozumat_chest          {0};

        uint64 go_tentacle_left         {0};
        uint64 go_tentacle_right        {0};

        uint64 go_wall_tentacle_left    {0};
        uint64 go_wall_tentacle_right   {0};

        bool startUlthorEvent           {false};
        bool startAreaTriggerEvent      {false};
        bool startCinematicEvent        {false};
        uint32 tainted_sentry_dead      {0};
        uint32 mui_timerCinematicEvent  {0};
        uint32 mui_timerUlthokEvent     {0};
        uint32 mui_timerAreaTriggerEvent{0};
        uint32 mui_voidripTimer         {0};
        std::string strInstData         {""};

        std::list<uint64 > cinematicTrigger {0};
        std::vector<uint64> wingTrash       {0};
        uint32 TeamInInstance               {0};
    };
};

void AddSC_instance_throne_of_the_tides()
{
    new instance_throne_of_the_tides();
}
