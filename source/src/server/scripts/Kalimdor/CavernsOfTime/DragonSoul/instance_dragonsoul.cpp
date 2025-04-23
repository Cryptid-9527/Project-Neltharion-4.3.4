#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "CreatureTextMgr.h"
#include "ScriptMgr.h"
#include "dragonsoul.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Pet.h"

ObjectData const creatureData[] =
{
    { NPC_MORCHOK,                      DATA_MORCHOK                    },
    { NPC_WARLORD_ZONOZZ,               DATA_WARLORD_ZONOZZ             },
    { NPC_HAGARA,                       DATA_HAGARA                     },
    { NPC_ALEXSTRASZA_ULTRAXION,        DATA_ALEXSTRASZA_ULTRAXION      },
    { NPC_KALECGOS_ULTRAXION,           DATA_KALECGOS_ULTRAXION         },
    { NPC_YSERA_ULTRAXION,              DATA_YSERA_ULTRAXION            },
    { NPC_NOZDORMU_ULTRAXION,           DATA_NOZDORMU_ULTRAXION         },
    { NPC_THRALL_ULTRAXION,             DATA_THRALL_ULTRAXION           },
    { NPC_DEATHWING_ULTRAXION,          DATA_DEATHWING_ULTRAXION        },
    { NPC_ULTRAXION_GAUNTLET,           DATA_GAUNTLET_TRIGGER           },
    { NPC_THE_DRAGON_SOUL_ULTRAXION,    DATA_THE_DRAGON_SOUL_ULTRAXION  },
    { NPC_YORSAHJ,                      DATA_YORSAHJ                    },
    { NPC_ULTRAXION,                    DATA_ULTRAXION                  },
    { NPC_WARMASTER_BLACKHORN,          DATA_WARMASTER_BLACKHORN        },
    { NPC_DW_SPINE,                     DATA_SPINE_OF_DEATHWING         },
    { NPC_DW_MADNESS,                   DATA_MADNESS_OF_DEATHWING       },
    { NPC_ALEXSTRASZA,                  NPC_ALEXSTRASZA                 },
    { NPC_NOZDORMU,                     NPC_NOZDORMU                    },
    { NPC_YSERA,                        NPC_YSERA                       },
    { NPC_KALECGOS,                     NPC_KALECGOS                    },
    { NPC_DW_MADNESS_THRALL,            NPC_DW_MADNESS_THRALL           },
    { NPC_TYRYGOSA,                     NPC_TYRYGOSA                    }, 
    { 0,                                0                               } // END
};

ObjectData const gameObjectData[] =
{
    { GO_ALLIANCE_SHIP_CINEMATIC,               DATA_ALLIANCE_SHIP_CINEMATIC},
    { GO_HORDE_SHIP_CINEMATIC,                  DATA_HORDE_SHIP_CINEMATIC   },
    { GO_ALLIANCE_SHIP_BATTLE,                  DATA_ALLIANCE_BATTLE_SHIP   },
    { GO_DW_BACKPLATE_1,                        DATA_DW_SPINE_BACKPLATE_1   },
    { GO_DW_BACKPLATE_2,                        DATA_DW_SPINE_BACKPLATE_2   },
    { GO_DW_BACKPLATE_3,                        DATA_DW_SPINE_BACKPLATE_3   },
    { GO_LESSER_CACHE_OF_THE_ASPECTS_10,        DATA_ULTRAXION_CHEST        },
    { GO_LESSER_CACHE_OF_THE_ASPECTS_25,        DATA_ULTRAXION_CHEST        },
    { GO_LESSER_CACHE_OF_THE_ASPECTS_10_HC,     DATA_ULTRAXION_CHEST        },
    { GO_LESSER_CACHE_OF_THE_ASPECTS_25_HC,     DATA_ULTRAXION_CHEST        },
    { GO_LESSER_CACHE_OF_THE_ASPECTS_LFR,       DATA_ULTRAXION_CHEST_LFR    },
    { GO_GREATER_CACHE_OF_THE_ASPECTS_10,       DATA_SPINE_CHEST            },
    { GO_GREATER_CACHE_OF_THE_ASPECTS_25,       DATA_SPINE_CHEST            },
    { GO_GREATER_CACHE_OF_THE_ASPECTS_10_HC,    DATA_SPINE_CHEST            },
    { GO_GREATER_CACHE_OF_THE_ASPECTS_25_HC,    DATA_SPINE_CHEST            },
    { GO_GREATER_CACHE_OF_THE_ASPECTS_LFR,      DATA_SPINE_CHEST_LFR        },
    { GO_ELEMENTIUM_FRAGMENT_10,                DATA_MADNESS_CHEST          },
    { GO_ELEMENTIUM_FRAGMENT_25,                DATA_MADNESS_CHEST          },
    { GO_ELEMENTIUM_FRAGMENT_10_HC,             DATA_MADNESS_CHEST          },
    { GO_ELEMENTIUM_FRAGMENT_25_HC,             DATA_MADNESS_CHEST          },
    { GO_ELEMENTIUM_FRAGMENT_LFR,               DATA_MADNESS_CHEST_LFR      },
    { 0,                                0                               } // END
};

class instance_dragonsoul : public InstanceMapScript
{
    public:
        instance_dragonsoul() : InstanceMapScript(DragonsoulScriptName, 967) { }

        struct instance_dragonsoul_InstanceMapScript : public InstanceScript
        {
            instance_dragonsoul_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(MAX_ENCOUNTERS);
                LoadObjectData(creatureData, gameObjectData);
                raidMode = RAID_MODE_UNKNOWN;
                /*
                    SET POWER OF THE ASPECTS NERF HERE
                */
                raidNerfData = RAID_NERF_INACTIVE;
                //raidNerfData = RAID_NERF_ACTIVE_7_STACKS;
                backplateCount = 0;
                mana = 0;
                activePlatform = 0;
                LfrId = 0;
                platformCount = 0;
            }

            void FillInitialWorldStates(WorldPacket& data) override
            {
                data << uint32(WS_BLACK_YELLOW) << uint32(0);
                data << uint32(WS_RED_GREEN) << uint32(0);
                data << uint32(WS_BLACK_BLUE) << uint32(0);
                data << uint32(WS_PURPLE_YELLOW) << uint32(0);
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!raidMode)
                {
                    if (Group* grp = player->GetGroup())
                    {
                        raidMode = (instance->Is25ManRaid() && grp->isLFRGroup()) ? RAID_MODE_LFR : RAID_MODE_NORMAL;

                        if (raidMode == RAID_MODE_LFR && LfrId == 0)
                            LfrId = sLFGMgr->GetDungeon(grp->GetGUID());
                    }
                }

                if (auto dw = GetCreature(DATA_DEATHWING_ULTRAXION))
                {

                    UpdateData transData(dw->GetMapId());
                    dw->BuildCreateUpdateBlockForPlayer(&transData, player);
                    WorldPacket packet;
                    transData.BuildPacket(&packet);

                    if (auto s = player->GetSession())
                        s->SendPacket(&packet);
                }

                if (auto dw = GetCreature(DATA_MORCHOK))
                {

                    UpdateData transData(dw->GetMapId());
                    dw->BuildCreateUpdateBlockForPlayer(&transData, player);
                    WorldPacket packet;
                    transData.BuildPacket(&packet);

                    if (auto s = player->GetSession())
                        s->SendPacket(&packet);
                }

            }

            void OnPlayerLeave(Player* player) override
            {
                player->RemoveAurasDueToSpell(106005); // SPELL_DEGRADATION
                player->RemoveAurasDueToSpell(106213);

                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(106005);
            }

            void OnCreatureRemove(Creature* creature)
            {
                SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);

            }

            void OnCreatureCreate(Creature* creature) override
            {
                if (!raidMode)
                {
                    Map::PlayerList const& players = instance->GetPlayers();
                    if (!players.isEmpty())
                        if (auto player = players.begin()->getSource())
                            if (Group* grp = player->GetGroup())
                            {
                                raidMode = (instance->Is25ManRaid() && grp->isLFRGroup()) ? RAID_MODE_LFR : RAID_MODE_NORMAL;

                                if (raidMode == RAID_MODE_LFR && LfrId == 0)
                                    LfrId = sLFGMgr->GetDungeon(grp->GetGUID());
                            }
                }

                if (raidMode == RAID_MODE_LFR)
                {
                    creature->CastSpell(creature, SPELL_PRESENCE_OF_THE_DRAGON_SOUL, true);
                    creature->SetLootMode(LOOT_MODE_LFR);
                }
                else if (raidNerfData >= RAID_NERF_ACTIVE)
                {
                    dragonSoulNpcs.push_back(creature->GetGUID());
                    int32 currentNormalAndHcNerfAmount{ -5 * int32(GetData(DATA_RAID_NERF)) };
                    creature->CastCustomSpell(creature, SPELL_PRESENCE_OF_THE_DRAGON_SOUL, &currentNormalAndHcNerfAmount, &currentNormalAndHcNerfAmount, NULL, true);
                    creature->CastSpell(creature, GetData(DATA_RAID_NERF_SPELL), true);
                }
                else
                {
                    creature->RemoveAurasDueToSpell(SPELL_PRESENCE_OF_THE_DRAGON_SOUL);
                    creature->RemoveAurasDueToSpell(uint32(109250 + 1));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 2));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 3));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 4));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 5));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 6));
                    creature->RemoveAurasDueToSpell(uint32(109250 + 7));
                }

                switch (creature->GetEntry())
                {
                    case 56143://waypoint drakes
                        creature->DisableMovementFlagUpdate(true);
                        creature->SetHover(true);
                        creature->SetCanFly(true);
                        creature->SetDisableGravity(true);
                        break;
                    case NPC_DW_MADNESS_THRALL:
                        if (!IsDone(DATA_MADNESS_OF_DEATHWING))
                            creature->Respawn(true);
                        break;
                    case NPC_THRALL_ULTRAXION: // ultraxion event
                    case NPC_KALECGOS_ULTRAXION:
                    case NPC_ALEXSTRASZA_ULTRAXION:
                    case NPC_YSERA_ULTRAXION:
                    case NPC_NOZDORMU_ULTRAXION:
                    case NPC_THE_DRAGON_SOUL_ULTRAXION:
                        if (IsDone(DATA_ULTRAXION))
                            creature->SetVisible(false);
                        break;

                    case NPC_GEN_TRIGGER_PORTALS:
                    case NPC_TWILIGHT_SIEGE_BREAKER:
                    case NPC_ANCIENT_WATER_LORD: // morchok trash
                    case NPC_EARTHEN_DESTROYER:
                    case NPC_TWILIGHT_SIEGE_CAPTAIN:
                    case NPC_TWILIGHT_PORTAL:
                    case NPC_EARTHEN_SOLDIER:
                        npcGroups.emplace(GROUP_MORCHOK_GROUND_TRASH, creature->GetGUID());
                        if (IsDone(DATA_MORCHOK) || LfrId == LFR_PART_TWO)
                            creature->SetPhaseMask(0x2, true);
                        break;
                    case NPC_FLAIL_OF_GORATH_TRASH: // wing trash + bosses
                    case NPC_EYE_OF_GORATH_TRASH:
                    case NPC_GLOWING_GLOBULE:
                    case NPC_ACIDIC_GLOBULE:
                    case NPC_CRIMSON_GLOBULE:
                    case NPC_DARK_GLOBULE:
                    case NPC_SHADOWED_GLOBULE:
                    case NPC_COBALT_GLOBULE:
                    case NPC_WARLORD_ZONOZZ:
                    case NPC_YORSAHJ:
                        wingTrash.push_back(creature->GetGUID());
                        if (!IsDone(DATA_MORCHOK) || LfrId == LFR_PART_TWO)
                            creature->SetPhaseMask(0x2, true);
                        break;

                        /*
                            Begin Friendly first-level NPCs to be phased
                        */


                    case NPC_TRAVEL_GNOME:              //andorgos, next to portals
                    case NPC_NETHESTRASZ:               //drake to roof
                    case NPC_TRAVEL_TO_SUMMIT:          //ultraxion portal, near entrance
                        creature->SetPhaseMask(IsDone(DATA_YORSAHJ) && IsDone(DATA_WARLORD_ZONOZZ) ? 0x1 : 0x2, true);
                        npcGroups.emplace(GROUP_RAID_TEPORTERS, creature->GetGUID());
                        break;
                    case NPC_VALEERA:                   //drake to zonozz
                    case NPC_EIENDORMI:                 //drake to yorsajh
                        creature->SetPhaseMask(IsDone(DATA_MORCHOK) ? 0x1 : 0x2, true);
                        npcGroups.emplace(GROUP_RAID_TEPORTERS, creature->GetGUID());
                        break;
                    case NPC_TRAVEL_TO_THE_MAELSTROM:   //portal at entrance
                        creature->SetPhaseMask(IsDone(DATA_SPINE_OF_DEATHWING) ? 0x1 : 0x2, true);
                        npcGroups.emplace(GROUP_RAID_TEPORTERS, creature->GetGUID());
                        break;

                    case NPC_LORD_AFRASASTRASZ:
                        if (creature->GetDBTableGUIDLow() == 255819)
                            SetData64(NPC_LORD_AFRASASTRASZ, creature->GetGUID());
                    case NPC_CRIMSON_LIFEBINDER:        //flanks lord afra left and right
                    case NPC_WYRMREST_CAPTAIN:          //big patrol center of packs at entrance, also guards temple
                    case NPC_WYRMREST_DEFENDER:
                    case NPC_WYRMREST_DEFENDER_2:
                    case NPC_WYRMREST_PROTECTOR:
                    case NPC_WYRMREST_PROTECTOR_2:

                    case NPC_TYRYGOSA:                  //belf center of temple
                    case NPC_TYRYGOSA_IMAGE:            //ghost that appears at bill cosby
                    case NPC_DEMESTRASZ:                //temple food and drink
                    case NPC_DALORMI:                   //temple sw corner, wands
                    case NPC_DASNURIMI:                 //temple sw corner, geodes for cata gems
                    case NPC_TRIZORMU:                  //temple se corner, inscription goods
                    case NPC_MR_STOMP:                  //temple se corner, pet roaming
                    case NPC_GORIN:                     //temple se corner, roaming does nothing
                    case NPC_LETHECUS:                  //temple general goods
                    case NPC_FERITHOS:                  //temple reagants
                    {
                        npcGroups.emplace(creature->GetPositionX() > -2231.0f ? GROUP_TOWER_WYRMREST_ACCORD : GROUP_ENTRANCE_WYRMREST_ACCORD, creature->GetGUID());
                        bool isTower{ (creature->GetPositionX() > -2231.0f) };
                        bool morch_done{ IsDone(DATA_MORCHOK) };
                        creature->SetPhaseMask(isTower == morch_done ? 0x1 : 0x2, true);
                        break;
                    }

                    case NPC_FACELESS_DESTROYER: // part 1 show and temple spawns
                    case NPC_FACELESS_CORRUPTOR:
                    case NPC_RUIN_TENTACLE:
                        wingTrash.push_back(creature->GetGUID());
                        if (!IsDone(DATA_MORCHOK) && creature->GetPositionX() > -2231.0f)
                            creature->SetPhaseMask(0x2, true);
                        else
                            creature->SetPhaseMask(0x1, true);
                        break;
                    case NPC_THRALL_OUTRO: // madness outro
                    case NPC_AGGRA_OUTRO:
                    case NPC_ASPECT_1_OUTRO:
                    case NPC_ASPECT_2_OUTRO:
                    case NPC_ASPECT_3_OUTRO:
                    case NPC_ASPECT_4_OUTRO:
                        if (!IsDone(DATA_MADNESS_OF_DEATHWING))
                            creature->SetVisible(false);
                        npcGroups.emplace(DATA_MADNESS_OF_DEATHWING, creature->GetGUID());
                        break;
                    case NPC_MORCHOK:
                        if (LfrId == LFR_PART_TWO)
                            creature->SetPhaseMask(0x2, true);
                        break;
                    case NPC_DEATHWING_ULTRAXION:
                    {

                        if (IsDone(DATA_YORSAHJ) && IsDone(DATA_WARLORD_ZONOZZ))
                        {
                            creature->SetPhaseMask(0x2, true);
                        }
                        else
                        {
                            Position center{ -1786.910034f, -2393.550049f, 341.354004f, 0.000000f };
                            creature->GetMotionMaster()->MoveAroundPoint(center, 195.f, false, 100, creature->GetPositionZ(), true);
                        }
                        break;

                        case NPC_DEATHWING_TAIL:
                        case NPC_DEATHWING_ARM_LEFT:
                        case NPC_DEATHWING_ARM_RIGHT:
                        case NPC_DEATHWING_WING_LEFT:
                        case NPC_DEATHWING_WING_RIGHT:
                        case NPC_DEATHWING_HEAD:
                        case NPC_ARM_TENTACLE:
                        case NPC_ARM_TENTACLE_2:
                        case NPC_WING_TENTACLE:
                        case NPC_BLISTERING_TENTACLE:
                        case NPC_ELEMENTIUM_FRAGMENT:
                        case NPC_MUTETED_CORRUPTION:
                        case NPC_CRUSH_TARGET:
                        case NPC_REGENERATIVE_BLOOD:
                        case NPC_SUMMON_ENABLER:
                        case NPC_ELEMENTIUM_BOLT_TARGET:
                        case NPC_TIME_ZONE:
                        case NPC_TIME_ZONE_TARGET:
                        case NPC_CORRUPTING_PARASITE:
                        case NPC_CORRUPTING_PARASITE_TENTACLE:
                        case NPC_CONGAELING_BLOOD_TARGET:
                            if (IsDone(DATA_MADNESS_OF_DEATHWING))
                                creature->DespawnOrUnsummon();
                            break;

                    }
                    default:
                        break;
                }

                InstanceScript::OnCreatureCreate(creature);
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_FOCUSING_IRIS:
                        if (IsDone(DATA_HAGARA))
                            go->m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GM, SEC_ADMINISTRATOR);
                        break;
                    case GO_ALLIANCE_SHIP_BATTLE:
                        if (IsDone(DATA_ULTRAXION))
                        {
                            instance->SummonCreature(NPC_SKY_CAPTAIN_SWAYZE, skyCaptainPos);
                            go->SetDestructibleState(GO_DESTRUCTIBLE_INTACT);
                        }
                        break;
                    case GO_ALLIANCE_SHIP_CINEMATIC:
                    case GO_HORDE_SHIP_CINEMATIC:
                        if (IsDone(DATA_ULTRAXION))
                            go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        break;
                    default:
                        break;
                }
                InstanceScript::OnGameObjectCreate(go);
            }

            uint32 GetData(uint32 data) const override
            {
                switch (data)
                {
                    case DATA_RAID_MODE:
                        return raidMode;
                    case DATA_DW_SPINE_BACKPLATE_CNT:
                        return backplateCount;
                    case DATA_DW_SPINE_ROLL_INFO:
                        return next_roll;
                    case DATA_LFR_PART:
                        return LfrId;
                    case DATA_MADNESS_PLATFORM_COUNT:
                        return platformCount;
                    case DATA_RAID_NERF:
                        return raidNerfData;
                        break;
                    case DATA_RAID_NERF_SPELL:
                        switch (raidNerfData)
                        {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                            return uint32(raidNerfData + 109250);
                            break;
                        default:
                            TC_LOG_FATAL("sql.sql", "DRAGON SOUL DATA_RAID_NERF_SPELL IS SET TO AN OUT OF RANGE VALUE, RESORTING TO 7-STACK spellID.");
                            return uint32(7 + 109250);
                            break;
                        }
                        break;
                    case DATA_SPINE_IMBALANCE:
                        return next_roll;
                        break;
                }
                return 0;
            }

            void SetData(uint32 type, uint32 value) override
            {
                switch (type)
                {
                    case DATA_DW_SPINE_BACKPLATE_CNT:
                        backplateCount = value;
                        break;
                    case DATA_DW_SPINE_ROLL_INFO:
                    {
                        if (GetData(DATA_DW_SPINE_ROLL_INFO) != value)
                        {
                            //TC_LOG_ERROR("sql.sql", "instance::Setdata Setting roll value to %u", value);
                            next_roll = value;
                        }
                    }
                    case DATA_SPINE_IMBALANCE:
                        break;
                    case DATA_MADNESS_PLATFORM_COUNT:
                        platformCount = value;
                        break;
                    case DATA_RAID_NERF:
                        raidNerfData = value;
                        if (value == RAID_NERF_INACTIVE && raidMode != RAID_MODE_LFR)
                        {
                            for (uint64 guid : dragonSoulNpcs)
                            {
                                if (auto creature = instance->GetCreature(guid))
                                {
                                    creature->RemoveAurasDueToSpell(SPELL_PRESENCE_OF_THE_DRAGON_SOUL);
                                    creature->RemoveAurasDueToSpell(SPELL_POWER_OF_THE_ASPECTS);
                                }
                            }
                        }
                        break;
                    case DATA_AFRASABI_DELAYED_PHASING:
                    {
                        for (auto itr : npcGroups)
                            if (itr.first == GROUP_ENTRANCE_WYRMREST_ACCORD)
                                if (auto trashMob = instance->GetCreature(itr.second))
                                {
                                    trashMob->SetPhaseMask(0x2, true);
                                }


                        for (auto itr : npcGroups)
                            if (itr.first == GROUP_TOWER_WYRMREST_ACCORD)
                                if (auto trashMob = instance->GetCreature(itr.second))
                                {
                                    if (trashMob->GetEntry() == NPC_TYRYGOSA)
                                    {
                                        trashMob->TalkWithDelay(1000, 0, NULL, CHAT_MSG_MONSTER_SAY);
                                        trashMob->TalkWithDelay(5000, 1, NULL, CHAT_MSG_MONSTER_SAY);
                                        trashMob->TalkWithDelay(16000, 2, NULL, CHAT_MSG_MONSTER_SAY);
                                        trashMob->TalkWithDelay(25000, 3, NULL, CHAT_MSG_MONSTER_SAY);
                                    }
                                    trashMob->SetPhaseMask(0x1, true);
                                }
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetData64(uint32 type, uint64 value) override
            {
                switch (type)
                {
                case DATA_ACTIVE_PLATFORM:
                    activePlatform = value;
                    break;
                case NPC_LORD_AFRASASTRASZ:
                    afro_guid = value;
                    break;
                default:
                    break;
                }
            }

            uint64 GetData64(uint32 type) const override
            {
                if (type == DATA_ACTIVE_PLATFORM)
                    return activePlatform;


                if (type == NPC_LORD_AFRASASTRASZ)
                    return afro_guid;

                return 0;
            }

            void UpdateTaxi()
            {
                for (auto itr : npcGroups)
                    if (itr.first == GROUP_RAID_TEPORTERS)
                        if (auto transporter = instance->GetCreature(itr.second))
                            switch (transporter->GetEntry())
                            {
                                case NPC_TRAVEL_GNOME:              //andorgos, next to portals
                                case NPC_NETHESTRASZ:               //drake to roof
                                case NPC_TRAVEL_TO_SUMMIT:          //ultraxion portal, near entrance
                                    transporter->SetPhaseMask((IsDone(DATA_YORSAHJ) && IsDone(DATA_WARLORD_ZONOZZ)) ? 0x01 : 0x02, true);
                                    break;
                                case NPC_VALEERA:                   //drake to zonozz
                                case NPC_EIENDORMI:                 //drake to yorsajh
                                    transporter->SetPhaseMask(IsDone(DATA_MORCHOK) ? 0x01 : 0x02, true);
                                    if ((IsDone(DATA_YORSAHJ) && IsDone(DATA_WARLORD_ZONOZZ)))
                                    {
                                        transporter->RemoveAurasDueToSpell(94557);
                                        transporter->RemoveAurasDueToSpell(94558);
                                        transporter->RemoveAurasDueToSpell(95514);
                                        transporter->RemoveAurasDueToSpell(95527);
                                        transporter->RemoveAurasDueToSpell(98702);
                                        transporter->RemoveAurasDueToSpell(110468);
                                        transporter->RemoveAurasDueToSpell(110469);
                                    }
                                    break;
                                case NPC_TRAVEL_TO_THE_MAELSTROM:   //portal at entrance
                                    transporter->SetPhaseMask(IsDone(DATA_SPINE_OF_DEATHWING) ? 0x01 : 0x02, true);
                                    break;
                                default:
                                    break;
                            }

            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_MORCHOK:
                        if (state == DONE || state == DONE_HM)
                        {
                            for (auto itr : npcGroups)
                                if (itr.first == GROUP_MORCHOK_GROUND_TRASH)
                                    if (auto trashMob = instance->GetCreature(itr.second))
                                        trashMob->SetPhaseMask(0x2, true);

                            for (auto itr : npcGroups)
                                if (itr.first == GROUP_ENTRANCE_WYRMREST_ACCORD)
                                    if (auto trashMob = instance->GetCreature(itr.second))
                                    {
                                        Creature* follow_this{ nullptr };
                                        trashMob->SetSpeed(MOVE_RUN, 1.4f, true, true);
                                        switch (trashMob->GetEntry())
                                        {
                                        case NPC_TYRYGOSA_IMAGE:
                                            break;
                                        case NPC_LORD_AFRASASTRASZ:
                                        case NPC_WYRMREST_CAPTAIN:
                                            {
                                            trashMob->GetMotionMaster()->MovePath(trashMob->GetDBTableGUIDLow(), false);
                                                break;
                                            }
                                        default:
                                            if (auto lord = trashMob->FindNearestCreature(NPC_LORD_AFRASASTRASZ, 100.f))
                                                if (auto captain = trashMob->FindNearestCreature(NPC_WYRMREST_CAPTAIN, 100.f))
                                                {
                                                    follow_this = (trashMob->GetExactDist(lord) > trashMob->GetExactDist(captain) ? captain : lord);
                                                    trashMob->GetMotionMaster()->MoveFollow(follow_this, trashMob->GetExactDist(follow_this), follow_this->GetAngle(trashMob));
                                                }
                                            break;
                                        }
                                    }


                            for (auto itr : wingTrash)
                                if (auto trashMob = instance->GetCreature(itr))
                                    trashMob->SetPhaseMask(0x1, true);

                        }
                        break;
                    case DATA_MADNESS_OF_DEATHWING:
                        if (state == DONE || state == DONE_HM)
                        {
                            for (auto itr : npcGroups)
                                if (itr.first == DATA_MADNESS_OF_DEATHWING)
                                    if (auto trashMob = instance->GetCreature(itr.second))
                                        trashMob->SetVisible(true);
                        }
                        if (state == IN_PROGRESS || state == FAIL)
                        {
                            for (auto itr : npcGroups)
                                if (itr.first == DATA_SPINE_OF_DEATHWING)
                                    if (auto trashMob = instance->GetCreature(itr.second))
                                        trashMob->SetVisible(state == FAIL ? true : false);
                        }
                        break;
                    case DATA_SPINE_OF_DEATHWING:
                        if (state == DONE || state == DONE_HM)
                        {
                            for (auto itr : npcGroups)
                                if (itr.first == DATA_SPINE_OF_DEATHWING)
                                    if (auto trashMob = instance->GetCreature(itr.second))
                                        trashMob->SetVisible(true);
                        }
                    case DATA_YORSAHJ:
                    case DATA_WARLORD_ZONOZZ:
                        if (state == DONE || state == DONE_HM)
                        {
                            if (IsDone(DATA_YORSAHJ) && IsDone(DATA_WARLORD_ZONOZZ))
                            {
                                for (auto itr : npcGroups)
                                    if (itr.first == DATA_YORSAHJ)
                                        if (auto trashMob = instance->GetCreature(itr.second))
                                            trashMob->SetVisible(true);
                            }
                            if (auto c = GetCreature(DATA_DEATHWING_ULTRAXION))
                            {
                                Position tele_to{ -1571.789307f, -2338.345703f, 275.926636f, 3.476679f };
                                c->SetHomePosition(tele_to);
                                c->GetMotionMaster()->MovePoint(105, tele_to, false, 50.f);
                                c->SetPhaseMask(0x2, true);
                            }
                        }
                        break;
                    case DATA_HAGARA:
                        if (auto c = GetCreature(DATA_DEATHWING_ULTRAXION))
                        {
                            Position tele_to{ -1571.789307f, -2338.345703f, 275.926636f, 3.476679f };
                            c->SetHomePosition(tele_to);
                            c->GetMotionMaster()->MovePoint(105, tele_to, false, 50.f);
                            c->SetPhaseMask(0x2, true);
                        }
                        break;
                    default:
                        break;
                }

                UpdateTaxi();
                return true;
            }

        private:
            uint32 raidMode         { 0  };
            uint32 mana             { 0  };
            uint32 backplateCount   { 0  };
            uint32 LfrId            { 0  };
            uint32 platformCount    { 0  };
            uint32 raidNerfData     { 0  };
            uint64 activePlatform   { 0  };
            uint64 afro_guid        { 0  };
            uint32 next_roll        { 10 };
            std::map<uint32, uint32> rollInfo;
            std::multimap<uint32, uint64> npcGroups;
            std::vector<uint64> wingTrash;
            std::vector<uint64> dragonSoulNpcs;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_dragonsoul_InstanceMapScript(map);
        }
};

void AddSC_instance_dragonsoul()
{
    new instance_dragonsoul();
}
