#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "well_of_eternity.h"

DoorData const doorData[] =
{
    { GO_LARGE_FIREWALL_DOOR,       DATA_PEROTHARN,     DOOR_TYPE_ROOM,         BOUNDARY_NONE },
    { GO_SMALL_FIREWALL_DOOR,       DATA_PEROTHARN,     DOOR_TYPE_ROOM,         BOUNDARY_NONE },  
    { GO_INVISIBLE_FIREWALL_DOOR,   DATA_PEROTHARN,     DOOR_TYPE_PASSAGE,      BOUNDARY_NONE },
    { 0,                        0,                      DOOR_TYPE_ROOM,         BOUNDARY_NONE }
};

ObjectData const creatureData[] =
{
    { NPC_PEROTHARN,            DATA_PEROTHARN              },
    { NPC_ILLIDAN_PART_ONE,     DATA_ILLIDAN_PART_ONE       },
    { NPC_QUEEN_AZSHARA,        DATA_QUEEN_AZSHARA          },
    { NPC_TYRANDE_WHISPERWIND,  DATA_TYRANDE_WHISPERWIND    },
    { NPC_MALFURION_STORMRAGE,  DATA_MALFURION_STORMRAGE    },
    { NPC_ILLIDAN_PART_TWO,     DATA_ILLIDAN_PART_TWO       },
    { NPC_CAPTAIN_VAROTHEN,     DATA_CAPTAIN_VAROTHEN       },
    { NPC_MANNOROTH,            DATA_MANNOROTH              },
    { NPC_ABYSSAL_DOOMBRINGER,  DATA_DOOMBRINGER            },
    { NPC_GENERAL_PURPOSE_BUNNY, DATA_PORTAL                },
    { NPC_DOOMGUARD_PORTAL,     DATA_DOOMGUARD_PORTAL       },
    { NPC_CHROMIE,              DATA_CHROMIE                },
    { NPC_NOZDORMU,             DATA_NOZDORMU               },
    { NPC_RP_SCENE_ALEXTRASZA,  DATA_RP_ALEX                },
    { 0,                        0                           }
};

ObjectData const gameObjectData[] =
{
    { GO_COURTYARD_DOOR_01,         DATA_COURTYYARD_DOOR_ONE    },
    { GO_PORTAL_ENERGY_FOCUS_ONE,   DATA_ENERGY_FOCUS_ONE       },
    { GO_PORTAL_ENERGY_FOCUS_TWO,   DATA_ENERGY_FOCUS_TWO       },
    { GO_PORTAL_ENERGY_FOCUS_THREE, DATA_ENERGY_FOCUS_THREE     },
    { GO_LIGHT_OF_ELUNE,            DATA_LIGHT_OF_ELUNE         },
    { GO_ROYAL_CACHE,               DATA_ROYAL_CACHE            },
    { GO_MINOR_CACHE_OF_THE_ASPECTS, DATA_MANNOROTH_CACHE       },
    { 0                           , 0                           }
};

class instance_well_of_eternity : public InstanceMapScript
{
    public:
        instance_well_of_eternity() : InstanceMapScript("instance_well_of_eternity", 940) { }

        struct instance_well_of_eternity_InstanceMapScript: public InstanceScript
        {
            instance_well_of_eternity_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
                LoadObjectData(creatureData, gameObjectData);
                firewallOneGUID = 0;
                firewallTwoGUID = 0;
                firewallThreeGUID = 0;
            }

            void OnPlayerEnter(Player* player) override
            {
                InstanceScript::OnPlayerEnter(player);

                if (player->getGender() == GENDER_MALE)
                    player->CastSpell(player, SPELL_NIGHT_ELF_ILLUSION_MALE, true);
                else
                    player->CastSpell(player, SPELL_NIGHT_ELF_ILLUSION_FEMALE, true);
            }

            void OnPlayerLeave(Player* player) override
            {
                InstanceScript::OnPlayerLeave(player);

                player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOWCLOAK);
                player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK);
                player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                player->RemoveAurasDueToSpell(105009); // Gift of Sageras
                player->RemoveAurasDueToSpell(102951); // SPELL_PLAYER_SHADOWCLOAK_COSMETIC
                player->RemoveAurasDueToSpell(103018); // SPELL_PLAYER_SHADOW_AMBUSHER
                player->RemoveAurasDueToSpell(103420); // SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH
                player->RemoveAurasDueToSpell(SPELL_NIGHT_ELF_ILLUSION_MALE);
                player->RemoveAurasDueToSpell(SPELL_NIGHT_ELF_ILLUSION_FEMALE);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_LARGE_FIREWALL_DOOR:
                    case GO_SMALL_FIREWALL_DOOR:
                        if (go->GetDistance2d(3335.379f, -4891.729f) > 90.00f)
                        {
                            if (go->GetPositionX() > 3192.00f && go->GetPositionX() < 3194.00f)
                            {
                                go->SetGoState(GO_STATE_ACTIVE);
                                firewallOneGUID = go->GetGUID();
                            }
                            else if (go->GetPositionX() > 3470.00f && go->GetPositionX() < 3472.00f)
                                firewallTwoGUID = go->GetGUID();
                            else if (go->GetPositionX() > 3451.00f && go->GetPositionX() < 3453.00f)
                                firewallThreeGUID = go->GetGUID();
                            return;
                        }
                        break;
                    case GO_PORTAL_ENERGY_FOCUS_ONE:
                    case GO_PORTAL_ENERGY_FOCUS_TWO:
                    case GO_PORTAL_ENERGY_FOCUS_THREE:
                        if (!IsDone(DATA_PEROTHARN))
                            go->Respawn();
                        break;
                    case GO_COURTYARD_DOOR_01:
                        if (IsDone(DATA_PEROTHARN))
                            go->SetGoState(GO_STATE_ACTIVE);
                        else
                            go->SetGoState(GO_STATE_READY);
                        break;
                    default:
                        break;
                }
                InstanceScript::OnGameObjectCreate(go);
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_DOOMGUARD_ANNIHILATOR:
                    case NPC_ABYSSAL_DOOMBRINGER:
                    case NPC_CAPTAIN_VAROTHEN:
                        if (!IsDone(DATA_MANNOROTH) && !creature->isAlive())
                            creature->Respawn(true);
                        break;
                    case NPC_DREADLORD_DEFENDER:
                    case NPC_CORRUPTED_ARCANIST:
                    case NPC_FEL_CRYSTAL_STALKER:
                    case NPC_FEL_CRYSTAL:
                    case NPC_LEGION_DEMON:
                    case NPC_GUARDIAN_DEMON:
                        if (!IsDone(DATA_PEROTHARN) && !creature->isAlive())
                            creature->Respawn(true);
                        break;
                    case NPC_CHROMIE:
                        if (!IsDone(DATA_MANNOROTH))
                            creature->SetVisible(false);
                        break;
                    case NPC_NOZDORMU:
                        if (IsDone(DATA_PEROTHARN) && !IsDone(DATA_QUEEN_AZSHARA))
                            creature->NearTeleportTo(NozdormuPositions[0]);
                        else if (IsDone(DATA_PEROTHARN) && IsDone(DATA_QUEEN_AZSHARA))
                            creature->NearTeleportTo(NozdormuPositions[1]);
                        break;
                    case NPC_ILLIDAN_PART_ONE:
                        if (IsDone(DATA_PEROTHARN))
                            creature->SetVisible(false);
                        break;
                    case 55541:
                    case 55542:
                    case 55543:
                        if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                            perotharn->AI()->JustSummoned(creature);
                        break;
                    default:
                        break;
                }
                InstanceScript::OnCreatureCreate(creature);
            }

            uint64 GetData64(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_FIREWALL_ONE:
                        return firewallOneGUID;
                    case DATA_FIREWALL_TWO:
                        return firewallTwoGUID;
                    case DATA_FIREWALL_THREE:
                        return firewallThreeGUID;
                    case DATA_RP_ALEX:
                        return ds_rp_alex_GUID;
                        break;
                }
                return InstanceScript::GetData64(type);
            }

            uint32 GetData(uint32 data) const
            {
                switch (data)
                {
                case DATA_LEGION_MOVEMENT:
                    return legionMovementAllowed;
                        break;
                case DATA_LEGION_PORTAL_STATE:
                    return legion_portals_status;
                        break;
                }
                return InstanceScript::GetData(data);
            }
            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                case DATA_LEGION_MOVEMENT:
                    ////TC_LOG_ERROR("sql.sql", "DATA_LEGION_MOVEMENT set to %u", data);
                    legionMovementAllowed = data;
                    break;
                case DATA_LEGION_PORTAL_STATE:
                    legion_portals_status = data;
                    break;
                case DATA_RP_SCENE_STATE:
                    dragon_soul_scene_state = data;
                    //TC_LOG_ERROR("sql.sql", "dragon_soul_scene_state set to %u", dragon_soul_scene_state);

                        if (dragon_soul_scene_state == 1)
                            if (auto alex = GetCreature(DATA_RP_ALEX))
                            for (auto i = 0; i < 5; i++)
                                if (auto drake = instance->SummonCreature(NPC_RP_SCENE_BRONZE_DRAKE, bronze_drake_spawns[i]))
                                {
                                    drake->AI()->SetData(DATA_PLAYER_DRAKE_SLOT, uint32(i));
                                    drake->CastWithDelay(200, drake, 107493, true);
                                    //do something with the drakes.
                                }
                        break;
                default:
                    break;
                }
                //TC_LOG_ERROR("sql.sql", "InstanceScript::SetData( %u, %u)", type, data);
                InstanceScript::SetData(type, data);
                //TC_LOG_ERROR("sql.sql", "InstanceScript::SetData( %u, %u) succeeded", type, data);
            }

        private:
            uint32 dragon_soul_scene_state{ 0 };
            uint32 rp_vote_portal{ 0 };
            uint32 rp_vote_drake{ 0 };
            uint32 legion_portals_status{ 0 };
            uint32 legionMovementAllowed{ 0 };
            uint64 firewallOneGUID;
            uint64 firewallTwoGUID;
            uint64 firewallThreeGUID;
            uint64 ds_rp_alex_GUID;
            Position bronze_drake_spawns[5]
            {
                {3478.753174f, -5276.328613f, 230.724472f, 1.501289f},
                {3468.753174f, -5280.328613f, 234.724472f, 1.501289f},
                {3458.753174f, -5276.328613f, 230.724472f, 1.501289f},
                {3448.753174f, -5280.328613f, 234.724472f, 1.501289f},
                {3438.753174f, -5276.328613f, 230.724472f, 1.501289f},
            };
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_well_of_eternity_InstanceMapScript(map);
        }
};

void AddSC_instance_well_of_eternity()
{
    new instance_well_of_eternity();
}
