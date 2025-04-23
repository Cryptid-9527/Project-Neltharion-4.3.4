#include "ScriptPCH.h"
#include "endTime.h"

#define MAX_ENCOUNTERS 5

DoorData const doorData[] =
{
    { GO_BAINE_WALL,       BOSS_ECHO_OF_BAINE,         DOOR_TYPE_ROOM,         BOUNDARY_S    },
    { 0,                   0,                          DOOR_TYPE_ROOM,         BOUNDARY_NONE }
};

class instance_end_time : public InstanceMapScript
{
public:
    instance_end_time() : InstanceMapScript("instance_end_time", 938) { }

    struct instance_end_time_InstanceMapScript: public InstanceScript
    {
        instance_end_time_InstanceMapScript(InstanceMap *map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(MAX_ENCOUNTERS);
            LoadDoorData(doorData);
        }

        void Initialize()
        {
            dataEchoOfJainaGUID = 0;
            dataEchoOfBaineGUID = 0;
            dataEchoOfSylvanasGUID = 0;
            dataEchoOfTyrandeGUID = 0;
            dataMurozondGUID = 0;
            dataTyrandeGauntlet = 0;
            fragCount = 0;
            hourGlassOfTimeGUID = 0;
            murosondCacheGUID = 0;
            JainaPickedState = NOT_STARTED;
            murozond_killcount = 0;
        }

        void FillInitialWorldStates(WorldPacket& data)
        {
            data << uint32(WORLDSTATE_SHOW_FRAGMENTS) << uint32(JainaPickedState == IN_PROGRESS && fragCount < 5);
            data << uint32(WORLDSTATE_FRAGMENTS_COLLECTED) << uint32(fragCount);
        }

        void OnCreatureCreate(Creature* pCreature)
        {
            InstanceScript::OnCreatureCreate(pCreature);

            switch (pCreature->GetEntry())
            {
                case NPC_ECHO_OF_JAINA:
                    dataEchoOfJainaGUID = pCreature->GetGUID();
                    if (fragCount < MAX_FRAGMENTS)
                    {
                        pCreature->SetVisible(false);
                        pCreature->SetReactState(REACT_PASSIVE);
                        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    }
                    if ((combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb1 != BOSS_ECHO_OF_JAINA)
                        && (combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb2 != BOSS_ECHO_OF_JAINA))
                        pCreature->DespawnOrUnsummon();
                    break;
                case NPC_ECHO_OF_BAINE:
                    dataEchoOfBaineGUID = pCreature->GetGUID();
                    if ((combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb1 != BOSS_ECHO_OF_BAINE)
                        && (combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb2 != BOSS_ECHO_OF_BAINE))
                        pCreature->DespawnOrUnsummon();
                    break;
                case NPC_ECHO_OF_SYLVANAS:
                    dataEchoOfSylvanasGUID = pCreature->GetGUID();
                    if ((combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb1 != BOSS_ECHO_OF_SYLVANAS)
                        && (combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb2 != BOSS_ECHO_OF_SYLVANAS))
                        pCreature->DespawnOrUnsummon();
                    break;
                case NPC_ECHO_OF_TYRANDE:
                    dataEchoOfTyrandeGUID = pCreature->GetGUID();
                    if ((combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb1 != BOSS_ECHO_OF_TYRANDE)
                        && (combi[GetData(DATA_RANDOM_BOSS_SELECTION)].eb2 != BOSS_ECHO_OF_TYRANDE))
                        pCreature->DespawnOrUnsummon();
                    break;
                case NPC_MUROZOND:
                    dataMurozondGUID = pCreature->GetGUID();
                    break;
                
                case NPC_VOLCANO:
                    pCreature->SetReactState(REACT_PASSIVE);
                    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_MUROZOND_CACHE:
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    murosondCacheGUID = go->GetGUID();
                    break;
                case GO_ET_TELEPORT:
                    break;
                case GO_BAINE_WALL:
                    AddDoor(go, true);
                    break;
                case GO_HOURGLASS_OF_TIME:
                    hourGlassOfTimeGUID = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        void OnPlayerLeave(Player* player) override
        {
            InstanceScript::OnPlayerLeave(player);

            player->RemoveAurasDueToSpell(101865); // Molten fists periodic
            player->RemoveAurasDueToSpell(101866); // Molten fists proc aura
        }

        uint64 GetData64(uint32 data) const
        {
            switch (data)
            {
                case DATA_ECHO_OF_JAINA_GUID:
                   return dataEchoOfJainaGUID;
                case DATA_ECHO_OF_BAINE_GUID:
                    return dataEchoOfBaineGUID;
                case DATA_ECHO_OF_SYLVANAS_GUID:
                    return dataEchoOfSylvanasGUID;
                case DATA_ECHO_OF_TYRANDE_GUID:
                    return dataEchoOfTyrandeGUID;
                case DATA_MUROZOND_GUID:
                    return dataMurozondGUID;
                case DATA_HOURGLASS_OF_TIME_GUID:
                    return hourGlassOfTimeGUID;
            }
            return 0;
        }

        uint32 GetData(uint32 data) const
        {
            switch (data)
            {
                case DATA_JAINA_PICKED_STATE:
                return JainaPickedState;
                case DATA_TYRANDE_GAUNTLET:
                   return dataTyrandeGauntlet;
                case DATA_FRAGMENTS:
                    return fragCount;
                case DATA_RANDOM_BOSS_SELECTION:
                    return RandomSelection;
                default:
                    return 0;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_FRAGMENTS:
                {
                    if (data == 0)
                    {
                        fragCount++;
                        DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, fragCount);
                        if (fragCount >= MAX_FRAGMENTS)
                        {
                            if (auto jaina = instance->GetCreature(dataEchoOfJainaGUID))
                            {
                                DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, 0);
                                jaina->SetVisible(true);
                            }
                        }
                        else
                            DoUpdateWorldState(WORLDSTATE_FRAGMENTS_COLLECTED, fragCount);
                    }
                    else
                    {
                        DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, 1);
                    }
                    break;
                }
                case DATA_TYRANDE_GAUNTLET:
                    dataTyrandeGauntlet = data;
                    break;
                case DATA_JAINA_PICKED_STATE:
                    switch (data)
                    {
                    case IN_PROGRESS:
                        //DoUpdateWorldState(WORLDSTATE_SHOW_FRAGMENTS, 1);
                        break;
                    }
                    JainaPickedState = data;
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case BOSS_ECHO_OF_JAINA:
                case BOSS_ECHO_OF_BAINE:
                case BOSS_ECHO_OF_SYLVANAS:
                case BOSS_ECHO_OF_TYRANDE:
                    break;
                case BOSS_MUROZOND:
                    if (state == DONE || state == DONE_HM)
                        if (GameObject *go = instance->GetGameObject(murosondCacheGUID))
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                default:
                    break;
            }

            return true;
        }

        void OnUnitDeath(Unit* unit)
        {
            InstanceScript::OnUnitDeath(unit);
            switch (unit->GetEntry())
            {
            case NPC_INFINITE_WARDEN:
            case NPC_INFINITE_SUPPRESSOR:
                if (unit->GetAreaId() == 5795)
                if (auto muro = Unit::GetCreature(*unit, GetData64(DATA_MUROZOND_GUID)))
                {
                    murozond_killcount++;
                    if (murozond_killcount == 4)
                    {
                        muro->AI()->Talk(0, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                    }

                    if (murozond_killcount >= 8)
                    {
                        muro->AI()->Talk(1, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                        muro->AI()->DoAction(1);
                    }
                }
                break;
            default:
                //TC_LOG_ERROR("sql.sql", "Unit Death, but entry is not spirit mender or intro invader.");
                break;
            }
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << fragCount << ' ' << dataTyrandeGauntlet;
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            data >> fragCount;
            data >> dataTyrandeGauntlet;
        }

    private:
        uint64 dataEchoOfJainaGUID          {0};
        uint64 dataEchoOfBaineGUID          {0};
        uint64 dataEchoOfSylvanasGUID       {0};
        uint64 dataEchoOfTyrandeGUID        {0};
        uint64 dataMurozondGUID             {0};
        uint64 hourGlassOfTimeGUID          {0};
        uint64 murosondCacheGUID            {0};
        uint32 fragCount                    {0};
        uint32 dataTyrandeGauntlet          {0};
        uint32 JainaPickedState             {0};
        uint32 murozond_killcount           {0};
        uint32 const RandomSelection        {urand(0, 11)};
        //uint32 const RandomSelection{ 11 };  //TEMPORARY FOR TESTING AND DEV, RETURN TO 0-11 WHEN DONE
    };

    InstanceScript* GetInstanceScript(InstanceMap *map) const
    {
        return new instance_end_time_InstanceMapScript(map);
    }
};

void AddSC_instance_end_time()
{
    new instance_end_time;
}
