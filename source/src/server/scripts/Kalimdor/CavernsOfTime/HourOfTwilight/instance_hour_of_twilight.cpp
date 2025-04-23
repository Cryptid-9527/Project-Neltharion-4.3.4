#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "hour_of_twilight.h"

DoorData const doorData[] =
{
    { GO_ICEWALL,           DATA_ARCURION,      DOOR_TYPE_PASSAGE,      BOUNDARY_S    },
    { 0,                    0,                  DOOR_TYPE_ROOM,         BOUNDARY_NONE }
};

ObjectData const creatureData[] =
{
    { NPC_ARCURION,         DATA_ARCURION       },
    { NPC_ASIRA,            DATA_ASIRA          },
    { NPC_ARCHBISHOP,       DATA_ARCHBISHOP     },
    { NPC_THRALL_EVENT_0,   DATA_THRALL_EVENT_0 },
    { NPC_THRALL_EVENT_1,   DATA_THRALL_EVENT_1 },
    { NPC_THRALL_EVENT_2,   DATA_THRALL_EVENT_2 },
    { NPC_THRALL_EVENT_3,   DATA_THRALL_EVENT_3 },
    { NPC_THRALL,           DATA_THRALL         },
    { 0,                    0                   }
};

ObjectData const gameObjectData[] =
{
    { GO_HORDE_PORTAL,      DATA_HORDE_PORTAL    },
    { GO_ALLIANCE_PORTAL,   DATA_ALLIANCE_PORTAL },
    { 0,                    0                    } // END
};

class instance_hour_of_twilight : public InstanceMapScript
{
    public:
        instance_hour_of_twilight() : InstanceMapScript("instance_hour_of_twilight", 940) { }

        struct instance_hour_of_twilight_InstanceMapScript: public InstanceScript
        {
            instance_hour_of_twilight_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadObjectData(creatureData, gameObjectData);
                TeamInInstance = 0;
                for (int i = 0; i < 12; i++)
                    thrallEscortEncouters[i] = 0;
            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (creature->GetEntry() == NPC_TWILIGHT_SPARK) // Twilight Spark
                    creature->SetVisible(false);

                if (creature->GetEntry() == NPC_EYE_STALKER || creature->GetEntry() == NPC_EYE_STALKER_2)
                {
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    creature->SetReactState(REACT_PASSIVE);
                    creature->AddUnitState(UNIT_STATE_ROOT);
                    creature->AddUnitState(UNIT_STATE_EVADE);
                }

                InstanceScript::OnCreatureCreate(creature);
            }

            void OnPlayerEnter(Player* player) override
            {
                InstanceScript::OnPlayerEnter(player);

                if (!TeamInInstance)
                    TeamInInstance = player->GetTeam();
            }

            void OnUnitDeath(Unit* unit) override
            {
                InstanceScript::OnUnitDeath(unit);

                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                switch (creature->GetEntry())
                {
                    case 54633:
                    case 54632:
                    case 54686:
                        if (auto thrall = GetCreature(DATA_THRALL_EVENT_3))
                            thrall->AI()->DoAction(ACTION_KILL_CREATURE);
                        break;
                    case 54555:
                    case 55559:
                    case 55563:
                        if (auto thrall = GetCreature(DATA_THRALL_EVENT_0))
                            thrall->AI()->DoAction(ACTION_KILL_CREATURE);
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_EVENT_0:
                        if (data == DONE)
                        {
                            if (auto thrall = GetCreature(DATA_THRALL_EVENT_1))
                                thrall->SetVisible(true);
                        }
                        thrallEscortEncouters[DATA_EVENT_0] = data;
                        break;
                    case DATA_EVENT_1:
                        if (data == DONE)
                        {
                            if (auto thrall = GetCreature(DATA_THRALL_EVENT_2))
                                thrall->SetVisible(true);
                        }
                        thrallEscortEncouters[DATA_EVENT_1] = data;
                        break;
                    case DATA_EVENT_2:
                        if (data == DONE)
                        {
                            if (auto thrall = GetCreature(DATA_THRALL_EVENT_3))
                                thrall->SetVisible(true);
                        }
                        thrallEscortEncouters[DATA_EVENT_2] = data;
                        break;
                    case DATA_EVENT_3:
                        if (data == DONE)
                        {
                            if (auto thrall = GetCreature(DATA_THRALL))
                                thrall->SetVisible(true);
                        }
                        thrallEscortEncouters[DATA_EVENT_3] = data;
                        break;
                }
                if (data == DONE)
                    SaveToDB();
            }

            uint32 GetData(uint32 type) const
            {
                switch (type)
                {
                    case DATA_EVENT_0:
                    case DATA_EVENT_1:
                    case DATA_EVENT_2:
                    case DATA_EVENT_3:
                        return thrallEscortEncouters[type];
                }
                return 0;
            }

            void WriteSaveDataMore(std::ostringstream& data) override
            {
                data << thrallEscortEncouters[DATA_EVENT_0] << ' '
                     << thrallEscortEncouters[DATA_EVENT_1] << ' '
                     << thrallEscortEncouters[DATA_EVENT_2] << ' '
                     << thrallEscortEncouters[DATA_EVENT_3];
            }

            void ReadSaveDataMore(std::istringstream& data) override
            {
                data >> thrallEscortEncouters[DATA_EVENT_0];
                data >> thrallEscortEncouters[DATA_EVENT_1];
                data >> thrallEscortEncouters[DATA_EVENT_2];
                data >> thrallEscortEncouters[DATA_EVENT_3];
                SetData(DATA_EVENT_0, thrallEscortEncouters[DATA_EVENT_0]);
                SetData(DATA_EVENT_1, thrallEscortEncouters[DATA_EVENT_1]);
                SetData(DATA_EVENT_2, thrallEscortEncouters[DATA_EVENT_2]);
                SetData(DATA_EVENT_3, thrallEscortEncouters[DATA_EVENT_3]);
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                switch (type)
                {
                    case DATA_ARCHBISHOP:
                        if (state == DONE || state == DONE_HM)
                            if (auto portal = GetGameObject(TeamInInstance == ALLIANCE ? DATA_ALLIANCE_PORTAL : DATA_HORDE_PORTAL))
                                portal->SetRespawnTime(portal->GetRespawnDelay());
                        break;
                }
                return InstanceScript::SetBossState(type, state);
            }

            private:
                uint32 thrallEscortEncouters[12];
                uint32 TeamInInstance;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_hour_of_twilight_InstanceMapScript(map);
        }
};

void AddSC_instance_hour_of_twilight()
{
    new instance_hour_of_twilight();
}
