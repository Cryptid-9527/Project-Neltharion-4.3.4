/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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
#include "deadmines.h"

DoorData const doorData[] =
{
    {GO_GLUBTOK_DOOR,                        BOSS_GLUBTOK_DATA,          DOOR_TYPE_PASSAGE,       BOUNDARY_N   },
    {GO_HELIX_DOOR_ENTRANCE,                 BOSS_HELIX_DATA,            DOOR_TYPE_ROOM,          BOUNDARY_N   },
    {GO_HELIX_DOOR_EXIT,                     BOSS_HELIX_DATA,            DOOR_TYPE_PASSAGE,       BOUNDARY_N   },
    {GO_FOE_ENTRANCE,                        BOSS_FOE_REAPER_5000_DATA,  DOOR_TYPE_ROOM,          BOUNDARY_N   },
    {GO_FOE_EXIT,                            BOSS_FOE_REAPER_5000_DATA,  DOOR_TYPE_PASSAGE,       BOUNDARY_N   },
    {0,                                      0,                          DOOR_TYPE_PASSAGE,       BOUNDARY_NONE}, // END
};

class instance_deadmines : public InstanceMapScript
{
    public:
        instance_deadmines() : InstanceMapScript("instance_deadmines", 36){}

        struct instance_deadmines_InstanceMapScript : public InstanceScript
        {
            instance_deadmines_InstanceMapScript(InstanceMap* map) : InstanceScript(map) {}

            void Initialize()
            {
                SetHeaders(DataHeader);
                SetBossNumber(MAX_ENCOUNTER);
                LoadDoorData(doorData);
                GlubtokGUID = 0;
                GlubtokGateGUID = 0;
                HelixGUID = 0;
                CaptainCookieGUID = 0;
                VanessaVanCleefGUID = 0;

                // vancleef event
                timerEvent = 0;
                eventStep = 0;
                MagmaVehicleGUID = 0;
                GlubtokEventGUID = 0;
                doorFOEExitGUID = 0;
                doorDefiasCanonGUID = 0;
                VanCleefEventGUID = 0;
                HelixEventGUID = 0;
                ReeperEventGUID = 0;
                paperGUID = 0;
                Kagtha_GUID = 0;
                Kagtha_GUID_entrance = 0;
                Slinky_Sharpshiv_GUID = 0;
                Slinky_Sharpshiv_GUID_entrance = 0;
                Miss_Mayhem_GUID = 0;
                Miss_Mayhem_GUID_entrance = 0;
                Mayhem_Prototype_L_GUID = 0;
                Mayhem_Prototype_L_GUID_entrance = 0;
                Mayhem_Prototype_R_GUID = 0;
                Mayhem_Prototype_R_GUID_entrance = 0;
                Lieutenant_Horatio_Laine_GUID = 0;
                Lieutenant_Horatio_Laine_GUID_entrance = 0;
                Stormwind_Investigator_GUID = 0;
                Stormwind_Investigator_GUID_entrance = 0;
                Stormwind_Investigator_2_GUID = 0;
                Stormwind_Investigator_2_GUID_entrance = 0;
                Crime_Scene_Bot_GUID = 0;
                Crime_Scene_Bot_GUID_entrance = 0;
                Crime_Scene_Bot_2_GUID = 0;
                Crime_Scene_Bot_2_GUID_entrance = 0;
                Crime_Scene_Bot_3_GUID = 0;
                Crime_Scene_Bot_3_GUID_entrance = 0;
                RipsnarlEventGUID = 0;

                dataFireWall = 1;
                dataProdigy = 1;
                ValvesDone = false;
                achFailed = true;
                _events.Reset();
                _events.SetPhase(PHASE_EVENT_NOT_STARTED);

            }

            void fail_Gauntlet()
            {
                if (!instance->IsHeroic())
                    return;
                VanCleefEventGUID = 0;
                HelixEventGUID = 0;
                ReeperEventGUID = 0;

                dataFireWall = 1;
                dataProdigy = 1;
                ValvesDone = false;
                achFailed = true;
                _events.Reset();
                _events.SetPhase(PHASE_EVENT_NOT_STARTED);

                SetData(DATA_RESET_VANCLEEF_EVENT, NOT_STARTED);
                SetData(DATA_GLUBTOK_EVENT, NOT_STARTED);
                SetData(DATA_HELIX_EVENT, NOT_STARTED);
                SetData(DATA_REEPER_EVENT, NOT_STARTED);
                SetData(DATA_WORGEN_EVENT, NOT_STARTED);
                SetData(DATA_TRAP_COMPLETE, NOT_STARTED);
                SetBossState(BOSS_EVENT_VANCLEEF, NOT_STARTED);
                eventStep = 0;
                SetActiveGauntletGroups(NM_GROUP_PAPER_ONLY, NM_GROUP_PAPER_ONLY);
            }

            void OnCreatureCreate(Creature* creature)
            {
                InstanceScript::OnCreatureCreate(creature);

                switch (creature->GetEntry())
                {
                    case NPC_VANESSA_PAPER:
                        paperGUID = creature->GetGUID();
                        creature->Respawn(true);
                        break;
                    case NPC_KAGTHA:
                        if (creature->GetDBTableGUIDLow() == 228174)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Kagtha_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 228173)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Kagtha_GUID_entrance = creature->GetGUID();
                        }
                        break;
                    case NPC_SLINKY_SHARPSHIV:
                        if (creature->GetDBTableGUIDLow() == 368074)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Slinky_Sharpshiv_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 228185)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Slinky_Sharpshiv_GUID_entrance = creature->GetGUID();
                        }
                        break;
                    case NPC_MISS_MAYHEM:
                        if (creature->GetDBTableGUIDLow() == 368075)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Miss_Mayhem_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 228180)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Miss_Mayhem_GUID_entrance = creature->GetGUID();
                        }
                        break;
                    case NPC_MAYHEM_REAPER_PROTOTYPE:
                        if (creature->GetDBTableGUIDLow() == 368076)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Mayhem_Prototype_L_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 228184)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Mayhem_Prototype_L_GUID_entrance = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 368077)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Mayhem_Prototype_R_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 228183)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Mayhem_Prototype_R_GUID_entrance = creature->GetGUID();
                        }
                        break;
                    case NPC_LIEUTENANT_HORATIO_LAINE:
                        /*
                        if (creature->GetDBTableGUIDLow() == 368113)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Lieutenant_Horatio_Laine_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 227199)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Lieutenant_Horatio_Laine_GUID_entrance = creature->GetGUID();
                        }
                        */
                        break;
                    case NPC_STORMWIND_INVESTIGATOR:
                        if (creature->GetDBTableGUIDLow() == 368114)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Stormwind_Investigator_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 307532)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Stormwind_Investigator_GUID_entrance = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 368115)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Stormwind_Investigator_2_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 227201)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Stormwind_Investigator_2_GUID_entrance = creature->GetGUID();
                        }
                        break;
                    case NPC_CRIME_SCENE_BOT:
                        if (creature->GetDBTableGUIDLow() == 368116)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Crime_Scene_Bot_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 307530)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Crime_Scene_Bot_GUID_entrance = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 368117)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Crime_Scene_Bot_2_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 307529)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Crime_Scene_Bot_2_GUID_entrance = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 368118)
                        {
                            creature->SetPhaseMask(0x4000, true);
                            Crime_Scene_Bot_3_GUID = creature->GetGUID();
                        }
                        if (creature->GetDBTableGUIDLow() == 227200)
                        {
                            creature->SetPhaseMask(0x1, true);
                            Crime_Scene_Bot_3_GUID_entrance = creature->GetGUID();
                        }
                    case NPC_GLUBTOK:
                        GlubtokGUID = creature->GetGUID();
                        break;
                    case NPC_HELIX:
                        HelixGUID = creature->GetGUID();
                        break;
                    case NPC_CAPTAIN_COOKIE:
                        CaptainCookieGUID = creature->GetGUID();
                        break;
                    case NPC_VANCLEEF:
                        VanessaVanCleefGUID = creature->GetGUID();
                        vanessas[VANESSA_BOSSFIGHT] = creature->GetGUID();
                        break;
                    case NPC_MAGMA_VEHICLE:
                        MagmaVehicleGUID = creature->GetGUID();
                        break;
                    case NPC_NIGHTMARE_FLAMES:
                        if (_events.IsInPhase(PHASE_EVENT_GLUBTOK))
                            creature->CastSpell(creature, SPELL_NIGHTMARE_FLAME, true);
                        else
                            creature->RemoveAura(SPELL_NIGHTMARE_FLAME);
                        break;
                    case NPC_DARKWEB_DEVOURER:
                    case NPC_CHATTERING_HORROR:
                    case NPC_NIGHTMARE_SKITTERLING:
                        if (!_events.IsInPhase(PHASE_EVENT_HELIX))
                        {
                            creature->SetReactState(REACT_PASSIVE);
                            creature->SetPhaseMask(0x4000, true);
                        }
                        break;
                    case NPC_GLUBTOK_EVENT:
                        GlubtokEventGUID = creature->GetGUID();
                        break;
                    case NPC_GLUBTOK_WEAK_SELF:
                        GlubtokRPLowLevelGUID = creature->GetGUID();
                        break;
                    case NPC_VANCLEEF_GAUNTLET_INTRO:
                        creature->SetReactState(REACT_PASSIVE);
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1);
                       VanCleefEventGUID = creature->GetGUID();
                       switch (creature->GetDBTableGUIDLow())
                       {
                       case 2198857:
                           vanessas[VANESSA_GLUBTOK] = creature->GetGUID();
                           break;
                       case 2198859:
                           vanessas[VANESSA_HELIX] = creature->GetGUID();
                           break;
                       case 2198861:
                           vanessas[VANESSA_FOE_REAPER] = creature->GetGUID();
                           break;
                       case 2198865:
                           vanessas[VANESSA_RIPSNARL] = creature->GetGUID();
                           break;
                       default:
                           TC_LOG_ERROR("sql.sql", "Unrecognized db table guid for deadmines vanessa, must be removed or corrected in code: %u", creature->GetDBTableGUIDLow());
                           break;
                       }
                       break;
                    case NPC_HELIX_EVENT:
                        HelixEventGUID = creature->GetGUID();
                        break;
                    case NPC_REEPER_EVENT:
                        ReeperEventGUID = creature->GetGUID();
                        break;
                    case NPC_RIPSNARL_NIGHTMARE:
                        RipsnarlEventGUID = creature->GetGUID();
                        break;
                   case NPC_VAPOR_VALVE:
                       creature->AddAura(94557, creature);
                       break;
                   case NPC_DEFIAS_CANNON:
                       creature->AddUnitState(UNIT_STATE_ROOT);
                       creature->SetRooted(true);
                       break;
                }


                //Establish relationship with gauntlet for proper phasing
                npcGroups.emplace(GetGroupFor(creature), creature->GetGUID());
                ApplyGauntletActiveToCreature(creature);

                CheckRequirements();
            }

            void OnGameObjectCreate(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_GLUBTOK_DOOR:
                    case GO_HELIX_DOOR_ENTRANCE:
                    case GO_HELIX_DOOR_EXIT:
                    //case GO_FOE_ENTRANCE:
                        AddDoor(go, true);
                        break;
                    case GO_FOE_EXIT:
                        doorFOEExitGUID = go->GetGUID();
                        AddDoor(go, true);
                        break;
                    case GO_IRON_CLAD_DOOR:
                        doorDefiasCanonGUID = go->GetGUID();
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        AddDoor(go, true);
                        break;

                }
                if (go->GetEntry() == GO_HELIX_DOOR_ENTRANCE)
                    go->SetGoState(GO_STATE_READY);
            }

            uint64 GetData64(uint32 data) const
            {
                switch (data)
                {
                    case BOSS_GLUBTOK_DATA:
                        return GlubtokGUID;
                    case BOSS_HELIX_DATA:
                        return HelixGUID;
                    case BOSS_CAPTAIN_COOKIE:
                        return CaptainCookieGUID;
                    case BOSS_GLUBTOK_EVENT_DATA:
                        return GlubtokEventGUID;
                    case BOSS_EVENT_VANCLEEF:
                        return VanCleefEventGUID;
                    case DATA_KAGTHA:
                        return Kagtha_GUID;
                    case DATA_KAGTHA_ENTRANCE:
                        return Kagtha_GUID_entrance;
                    case DATA_SLINKY_SHARPSHIV:
                        return Slinky_Sharpshiv_GUID;
                    case DATA_SLINKY_SHARPSHIV_ENTRANCE:
                        return Slinky_Sharpshiv_GUID_entrance;
                    case DATA_MISS_MAYHEM:
                        return Miss_Mayhem_GUID;
                    case DATA_MISS_MAYHEM_ENTRANCE:
                        return Miss_Mayhem_GUID_entrance;
                    case DATA_MAYHEM_PROTOTYPE_L:
                        return Mayhem_Prototype_L_GUID;
                    case DATA_MAYHEM_PROTOTYPE_L_ENTRANCE:
                        return Mayhem_Prototype_L_GUID_entrance;
                    case DATA_MAYHEM_PROTOTYPE_R:
                        return Mayhem_Prototype_R_GUID;
                    case DATA_MAYHEM_PROTOTYPE_R_ENTRANCE:
                        return Mayhem_Prototype_R_GUID_entrance;
                    case DATA_LIEUTENANT_HORATIO_LAINE:
                        return Lieutenant_Horatio_Laine_GUID;
                    case DATA_LIEUTENANT_HORATIO_LAINE_ENTRANCE:
                        return Lieutenant_Horatio_Laine_GUID_entrance;
                    case DATA_STORMWIND_INVESTIGATOR:
                        return Stormwind_Investigator_GUID;
                    case DATA_STORMWIND_INVESTIGATOR_ENTRANCE:
                        return Stormwind_Investigator_GUID_entrance;
                    case DATA_STORMWIND_INVESTIGATOR_2:
                        return Stormwind_Investigator_2_GUID;
                    case DATA_STORMWIND_INVESTIGATOR_2_ENTRANCE:
                        return Stormwind_Investigator_2_GUID_entrance;
                    case DATA_CRIME_SCENE_BOT:
                        return Crime_Scene_Bot_GUID;
                    case DATA_CRIME_SCENE_BOT_ENTRANCE:
                        return Crime_Scene_Bot_GUID_entrance;
                    case DATA_CRIME_SCENE_BOT_2:
                        return Crime_Scene_Bot_2_GUID;
                    case DATA_CRIME_SCENE_BOT_2_ENTRANCE:
                        return Crime_Scene_Bot_2_GUID_entrance;
                    case DATA_CRIME_SCENE_BOT_3:
                        return Crime_Scene_Bot_3_GUID;
                    case DATA_CRIME_SCENE_BOT_3_ENTRANCE:
                        return Crime_Scene_Bot_3_GUID_entrance;
                    default:
                        return 0;
                }
            }

            uint32 GetData(uint32 data) const
            {
                switch (data)
                {
                    case DATA_FIREWALL:
                        return dataFireWall;
                    case DATA_PRODIGY:
                        return dataProdigy;
                    default:
                        return 0;
                }
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;
                CheckRequirements();

                if (type == BOSS_VANCLEEF && state == IN_PROGRESS && !achFailed)
                    DoCompleteAchievement(ACHIEVEMENT_VIGOROUS_VANCLEEF_VINDICATOR);

                if (type == BOSS_CAPTAIN_COOKIE)
                    if (instance->IsHeroic())
                        SetActiveGauntletGroups(NM_GROUP_BEFORE_GAUNTLET, NM_GROUP_PAPER_ONLY);

                                if (auto paper = instance->GetCreature(paperGUID))
                                    paper->AI()->Talk(0);
                            

                return true;
            }


            void OnPlayerEnter(Player* player)
            {
                if (instance->IsHeroic())
                    if (GetBossState(BOSS_CAPTAIN_COOKIE) == DONE)
                    {
                        if (GetBossState(BOSS_EVENT_VANCLEEF) != DONE)
                            SetActiveGauntletGroups(NM_GROUP_PAPER_ONLY, NM_GROUP_PAPER_ONLY);
                        else
                            if (!IsDone(BOSS_VANCLEEF))
                            SetActiveGauntletGroups(NM_GROUP_AFTER_GAUNTLET, NM_GROUP_AFTER_GAUNTLET);

                    }
            }
            

            void CheckRequirements()
            {
                if (auto captainCookie = instance->GetCreature(CaptainCookieGUID))
                    captainCookie->SetVisible(IsDone(BOSS_ADMIRAL_RISNARL));

                if (Creature *vvcleef = instance->GetCreature(VanessaVanCleefGUID))
                {
                    vvcleef->SetVisible(instance->IsHeroic() && IsDone(BOSS_EVENT_VANCLEEF) && IsDone(BOSS_CAPTAIN_COOKIE));
                    vvcleef->SetReactState((instance->IsHeroic() && IsDone(BOSS_EVENT_VANCLEEF) && IsDone(BOSS_CAPTAIN_COOKIE)) ? REACT_AGGRESSIVE : REACT_PASSIVE);
                }
            }

            npc_groups GetGroupFor(Creature* c)
            {

                switch (c->GetEntry())
                {
                case NPC_VANESSA_PAPER:
                    return NM_GROUP_PAPER_ONLY;
                    break;
                case NPC_VANESSA_ABOVE_HUT:
                    return NM_GROUP_VANESSA_MONOLOGUE;
                    break;

                case NPC_VANCLEEF_GAUNTLET_INTRO:
                    switch (c->GetDBTableGUIDLow())
                    {
                    case 2198857:
                        return NM_GROUP_GLUBTOK;
                        break;
                    case 2198859:
                        return NM_GROUP_HELIX;
                        break;
                    case 2198861:
                        return NM_GROUP_REAPER;
                        break;
                    case 2198865:
                        return NM_GROUP_RIPSNARL;
                        break;
                    default:
                        TC_LOG_ERROR("sql.sql", "Unrecognized db table guid for deadmines vanessa (npc_groups GetGroupFor(Creature* c)), must be removed or corrected in code: %u", c->GetDBTableGUIDLow());
                        break;
                    }

                    break;

                case NPC_GENERAL_BUNNY:
                    if (auto o = c->GetOwner())
                        if (auto c2 = o->ToCreature())
                            return GetGroupFor(c2);

                    switch (c->GetDBTableGUIDLow())
                    {
                    case 223744:
                        return NM_GROUP_AFTER_GAUNTLET;
                        break;
                    case 223786:
                        return NM_GROUP_BEFORE_GAUNTLET;
                        break;
                    }
                    return NM_GROUP_GLUBTOK;
                    break;
                case NPC_DEFIAS_ENFORCER:
                case NPC_DEFIAS_SHADOWGUARD:
                case NPC_ROPE:
                case NPC_VANESSA_ROPE_ANCHOR:
                    return NM_GROUP_AFTER_GAUNTLET;
                    break;

                case NPC_VAPOR_VALVE:
                case NPC_MAGMA_VEHICLE:
                case NPC_VANESSA_ANCHOR_BUNNY:
                case NPC_MAGMA_TRAP_EXIT_JUMP_TARGET:
                    return NM_GROUP_TRAP;
                    break;
                case NPC_NIGHTMARE_FLAMES:
                case NPC_NM_GLUBTOK_FIRE_CONTROLLER:
                case NPC_GLUBTOK_EVENT:
                case NPC_GLUBTOK_WEAK_SELF:
                case NPC_COLLAPSING_ICICLE:
                case NPC_SNOWPACKED_ICICLE:
                    return NM_GROUP_GLUBTOK;
                    break;

                case NPC_HELIX_EVENT:
                case NPC_DARKWEB_DEVOURER:
                case NPC_CHATTERING_HORROR:
                case NPC_NIGHTMARE_SKITTERLING:
                    return NM_GROUP_HELIX;
                    break;

                case NPC_REEPER_EVENT:
                case NPC_REAPER_LIGHTNING_OBSTACLE:
                case NPC_REAPER_LIGHTNING_OBSTACLE_PASSENGER:
                    return NM_GROUP_REAPER;
                    break;

                case NPC_ENRAGED_WORGEN:
                case NPC_HERIC_HARRINGTON:
                case NPC_JAMES_HARRINGTON:
                case NPC_EMMA_HARRINGTON:
                case NPC_CALLISA_HARRINGTON:
                case NPC_RIPSNARL_NIGHTMARE:
                    return NM_GROUP_RIPSNARL;
                    break;
                case NPC_VANCLEEF:
                    return NM_GROUP_AFTER_GAUNTLET;
                    break;
                case NPC_CHALLENGE_MODE:
                default:
                    if (auto o = c->GetOwner())
                        if (auto c2 = o->ToCreature())
                            return GetGroupFor(c2);

                    return NM_GROUP_BEFORE_GAUNTLET;
                    break;
                }
                return NM_GROUP_BEFORE_GAUNTLET;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_RESET_VANCLEEF_EVENT:
                    {
                        _events.SetPhase(PHASE_EVENT_NOT_STARTED);
                        achFailed = false;
                        SetBossState(BOSS_EVENT_VANCLEEF, NOT_STARTED);
                        break;
                    }
                    case DATA_PREPARE_VANCLEEF_EVENT:
                    {
                        _events.SetPhase(PHASE_EVENT_GLUBTOK);
                        instance->SummonCreature(NPC_VANESSA_ABOVE_HUT, nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_FIRST_SPAWN]);
                        SetActiveGauntletGroups(NM_GROUP_VANESSA_MONOLOGUE, NM_GROUP_VANESSA_MONOLOGUE);

                        SetBossState(BOSS_EVENT_VANCLEEF, IN_PROGRESS);
                        _events.ScheduleEvent(CHECK_PLAYERS_ALIVE, 0);
                        _events.ScheduleEvent(CHECK_ACH_FAILED, 300000);
                        achFailed = false;
                        break;
                    }
                    case DATA_START_VANCLEEF_EVENT:
                    {
                        if (GameObject *go = instance->GetGameObject(doorFOEExitGUID))
                            go->SetGoState(GO_STATE_READY);
                        if (GameObject *go = instance->GetGameObject(doorDefiasCanonGUID))
                            go->SetGoState(GO_STATE_READY);
                        _events.ScheduleEvent(EVENT_VANCLEEF_EVENT, 0);
                        eventStep = 0;
                        timerEvent = 0;
                        break;
                    }
                    case DATA_END_VANCLEEF_EVENT:
                    {
                        if (GameObject *go = instance->GetGameObject(doorDefiasCanonGUID))
                            go->SetGoState(GO_STATE_ACTIVE);
                        if (GameObject *go = instance->GetGameObject(doorFOEExitGUID))
                            go->SetGoState(GO_STATE_ACTIVE);
                        break;
                    }
                    case DATA_GLUBTOK_EVENT:
                    {
                        switch (data)
                        {
                            case IN_PROGRESS:
                            {
                                break;
                            }
                            case DONE:
                            {
                                DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_2);
                                InitVancleefEventFlames(false);
                                if (Creature *vc = instance->GetCreature(VanCleefEventGUID))
                                    vc->NearTeleportTo(nightmare_gauntlet_positions[NMG_POS_VANESSA_AT_HELIX].GetPositionX(), nightmare_gauntlet_positions[NMG_POS_VANESSA_AT_HELIX].GetPositionY(), nightmare_gauntlet_positions[NMG_POS_VANESSA_AT_HELIX].GetPositionZ(), nightmare_gauntlet_positions[NMG_POS_VANESSA_AT_HELIX].GetOrientation());
                                eventStep++;
                                timerEvent = 5000;
                                break;
                            }
                        }
                        break;
                    }
                    case DATA_HELIX_EVENT:
                    {
                        switch (data)
                        {
                            case IN_PROGRESS:
                            {
                                _events.SetPhase(PHASE_EVENT_HELIX);
                                InitVancleefEventSpiders(true);
                                break;
                            }
                            case DONE:
                            {
                                DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_3);
                                InitVancleefEventLighting(true);
                                eventStep++;
                                timerEvent = 5000;
                                InitVancleefEventSpiders(false);
                                break;
                            }
                        }
                        break;
                    }
                    case DATA_REEPER_EVENT:
                    {
                        switch (data)
                        {
                            case IN_PROGRESS:
                            {
                                _events.SetPhase(PHASE_REEPER);
                                break;
                            }
                            case DONE:
                            {
                                DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_4);
                                InitVancleefEventLighting(false);
                                eventStep++;
                                timerEvent = 5000;
                                break;
                            }
                        }
                        break;
                    }
                    case DATA_WORGEN_EVENT:
                    {
                        switch (data)
                        {
                            case IN_PROGRESS:
                            {
                                _events.SetPhase(PHASE_WORGEN);
                                break;
                            }
                            case DONE:
                            {
                                eventStep++;
                                timerEvent = 5000;
                                break;
                            }
                            case FAIL:
                            {
                                fail_Gauntlet();
                                Map::PlayerList const &PlayerList = instance->GetPlayers();
                                if (!PlayerList.isEmpty())
                                    for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                        if (auto player = i->getSource())
                                            if (player->isAlive())
                                                player->Kill(player);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    case DATA_TRAP_COMPLETE:
                    {
                        switch (data)
                        {
                            case DONE:
                            {
                                if (!ValvesDone)
                                {
                                    ValvesDone = true;
                                    _events.RescheduleEvent(EVENT_VANCLEEF_EVENT, 0);
                                    eventStep = 3;
                                }
                                break;
                            }
                            case FAIL:
                            {
                                fail_Gauntlet();
                                Map::PlayerList const &PlayerList = instance->GetPlayers();
                                if (!PlayerList.isEmpty())
                                    for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                        if (Player *player = i->getSource())
                                            if (player->isAlive())
                                                player->Kill(player);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    case DATA_FIREWALL:
                        dataFireWall = data;
                        break;
                    case DATA_PRODIGY:
                        dataProdigy = data;
                    default:
                        break;
                }
            }

            void InitVancleefEvent(bool start)
            {
                if (!start)
                {
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE);
                    //DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_1);
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_2);
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_3);
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_4);
                    DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_NIGHTMARE_5);
                    /*
                        nightmare sprint buff should not wear off with the elixir. https://youtu.be/RAC5GmKa0jg?t=503
                    */
                    //DoRemoveAurasDueToSpellOnPlayers(SPELL_ADRENALINE);
                }
                InitVancleefEventFlames(start);
                InitVancleefEventSpiders(start);
                InitVancleefEventLighting(start);
            }

            void InitVancleefEventFlames(bool start)
            {
                // NPC_NIGHTMARE_FLAMES
                if (Creature *magmaVeh = instance->GetCreature(MagmaVehicleGUID))
                {
                    if (!magmaVeh->isAlive())
                        magmaVeh->Respawn(true);
                    std::list<Creature* > triggerList;
                    magmaVeh->GetCreatureListWithEntryInGrid(triggerList, NPC_NIGHTMARE_FLAMES, 500.0f);


                    if (triggerList.size())
                    for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                        if (Creature *shadowFlames = *itr)
                        {
                            if (start)
                            {
                                shadowFlames->CastSpell(shadowFlames, 42075, true);
                                shadowFlames->CastSpell(shadowFlames, SPELL_NIGHTMARE_FLAME, true);
                            }
                            else
                            {
                                shadowFlames->RemoveAura(SPELL_NIGHTMARE_FLAME);
                                shadowFlames->RemoveAura(42075);
                            }
                        }
                }
            }

            void InitVancleefEventSpiders(bool start)
            {
                // NPC_NIGHTMARE_FLAMES
                if (Creature *helix = instance->GetCreature(HelixEventGUID))
                {
                    std::list<Creature* > triggerList;
                    helix->GetCreatureListWithEntryInGrid(triggerList, NPC_DARKWEB_DEVOURER, 500.0f);
                    helix->GetCreatureListWithEntryInGrid(triggerList, NPC_CHATTERING_HORROR, 500.0f);
                    helix->GetCreatureListWithEntryInGrid(triggerList, NPC_NIGHTMARE_SKITTERLING, 500.0f);
                    
                    if (triggerList.size())
                        for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                        if (Creature *spider = *itr)
                        {
                            if (start)
                            {
                                spider->SetPhaseMask(0x1, true);
                                spider->SetReactState(REACT_AGGRESSIVE);
                            }
                            else
                            {
                                spider->SetPhaseMask(0x4000, true);
                                spider->SetReactState(REACT_PASSIVE);
                                if (!spider->IsInEvadeMode())
                                {
                                    spider->CombatStop();
                                    spider->AI()->EnterEvadeMode();
                                }
                            }
                        }
                }
            }

            void InitVancleefEventLighting(bool start)
            {
                if (Creature *vc = instance->GetCreature(VanCleefEventGUID))
                {
                    std::list<Creature* > triggerList;
                    vc->GetCreatureListWithEntryInGrid(triggerList, NPC_REAPER_LIGHTNING_OBSTACLE, 500.0f);
                    if (triggerList.size())
                        for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                    for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                        if (Creature *light = *itr)
                            light->AI()->DoAction(start ? ACTION_ACTIVATE : ACTION_DESACTIVATE);
                    triggerList.clear();
                    if (!start)
                    {
                        vc->GetCreatureListWithEntryInGrid(triggerList, NPC_REAPER_LIGHTNING_OBSTACLE_PASSENGER, 500.0f);
                        if (triggerList.size())
                            for (auto itr = triggerList.begin(); itr != triggerList.end(); itr++)
                            if (Creature *light = *itr)
                                light->DespawnOrUnsummon();
                    }
                }
            }
            bool ForceUpdatePhasing(Creature* c)
            {
                switch (c->GetEntry())
                {
                case NPC_VAPOR_VALVE:
                    return false;
                case NPC_VANCLEEF_GAUNTLET_INTRO:
                case NPC_HELIX_EVENT:
                case NPC_REAPER_LIGHTNING_OBSTACLE_PASSENGER:
                case NPC_REAPER_LIGHTNING_OBSTACLE:
                case NPC_NIGHTMARE_FLAMES:
                case NPC_JAMES_HARRINGTON:
                case NPC_GLUBTOK_WEAK_SELF:
                case NPC_VANESSA_PAPER:
                default:
                    return true;
                    break;
                }
                return true;
            }

            bool CheckFactionBeforePhasing(uint32 entry)
            {
                switch (entry)
                {
                case NPC_LIEUTENANT_HORATIO_LAINE:
                    return false;
                    break;

                default:
                    return true;
                }
                    return true;
            }

            void SetActiveGauntletGroups(npc_groups start, npc_groups end)
            {
                active_gauntlet_groups[0]   = start;
                active_gauntlet_groups[1]   = end;

                for (auto itr : npcGroups)
                    if (itr.first >= uint32(start) && itr.first <= uint32(end))
                    {
                        if (auto trashMob = instance->GetCreature(itr.second))
                        if (CheckFactionBeforePhasing(trashMob->GetEntry()))
                        {
                            if (itr.first != NM_GROUP_BEFORE_GAUNTLET)
                            if (trashMob->GetPhaseMask() != 0x1)
                                trashMob->Respawn(false);

                            trashMob->SetPhaseMask(0x1, ForceUpdatePhasing(trashMob));
                        }
                    }
                    else
                    {
                        if (auto trashMob = instance->GetCreature(itr.second))
                        if (CheckFactionBeforePhasing(trashMob->GetEntry()))
                        {
                            trashMob->SetPhaseMask(0x4000, ForceUpdatePhasing(trashMob));
                        }
                    }
            }

            void ApplyGauntletActiveToCreature(Creature* c)
            {
                auto group = GetGroupFor(c);

                if (group >= active_gauntlet_groups[0] && group <= active_gauntlet_groups[1])
                    c->SetPhaseMask(0x1, ForceUpdatePhasing(c));
                else
                    c->SetPhaseMask(0x4000, ForceUpdatePhasing(c));
            }

            void Update(uint32 diff)
            {
                _events.Update(diff);

                if (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_VANCLEEF_EVENT:
                        {
                            switch (eventStep)
                            {
                                case 0:
                                {

                                    SetActiveGauntletGroups(NM_GROUP_VANESSA_MONOLOGUE, NM_GROUP_TRAP);

                                    //player->NearTeleportTo(nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionX(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionY(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionZ(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetOrientation());
                                    Map::PlayerList const &PlayerList = instance->GetPlayers();
                                    if (!PlayerList.isEmpty())
                                        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                            if (auto player = i->getSource())
                                                if (player->isAlive())
                                                    player->NearTeleportTo(nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionX(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionY(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetPositionZ(), nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO].GetOrientation());

                                    timerEvent = 1000;
                                    break;
                                }
                                case 1:
                                {

                                    if (auto magmaVeh = instance->GetCreature(MagmaVehicleGUID))
                                    {
                                        std::list<Creature*> list_of_npcs;
                                        GetCreatureListWithEntryInGrid(list_of_npcs, magmaVeh, NPC_VAPOR_VALVE, 50.0f);
                                        for (auto v : list_of_npcs)
                                        {
                                            v->RemoveAurasDueToSpell(SPELL_AURA_VAPOR_EVENT);
                                            v->SetFacingTo(v->GetOrientation());
                                        }
                                        magmaVeh->AI()->DoAction(ACTION_INIT_EVENT);
                                    }
                                    timerEvent = 4000;
                                    break;
                                }
                                case 2:
                                {
                                    if (auto magmaVeh = instance->GetCreature(MagmaVehicleGUID))
                                        magmaVeh->AI()->DoAction(ACTION_EMOTE_TRAP);
                                    timerEvent = 8000;
                                    break;
                                }
                                case 3:
                                {
                                    if (Creature *magmaVeh = instance->GetCreature(MagmaVehicleGUID))
                                        magmaVeh->AI()->DoAction(ACTION_EJECT_PASSENGERS);

                                    if (ValvesDone)
                                        timerEvent = 0;
                                    else timerEvent = 1000;                                 
                                    break;
                                }
                                case 4:
                                {
                                    if (!ValvesDone)
                                    {
                                        SetData(DATA_TRAP_COMPLETE, FAIL);
                                    }
                                    else
                                    {
                                        if (auto jump = instance->SummonCreature(NPC_MAGMA_TRAP_EXIT_JUMP_TARGET, nightmare_gauntlet_positions[NMG_POS_PLATFORM_VANESSA_JUMP_TO]))
                                        {

                                        DoCastSpellOnPlayers(SPELL_FORECAST_MAGMA_TRAP_THROW);
                                        SetActiveGauntletGroups(NM_GROUP_TRAP, NM_GROUP_GLUBTOK);
                                        if (auto glubtok_fight = instance->GetCreature(GlubtokEventGUID))
                                            if (auto glubtok_ghost = instance->GetCreature(GlubtokRPLowLevelGUID))
                                            {
                                                glubtok_ghost->AddAura(44816, glubtok_ghost);
                                                glubtok_ghost->SetReactState(REACT_PASSIVE);
                                                //glubtok_ghost->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                                glubtok_ghost->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                                glubtok_ghost->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                                glubtok_ghost->HandleEmoteCommand(EMOTE_STATE_COWER);

                                                glubtok_ghost->SetTarget(glubtok_fight->GetGUID());
                                                glubtok_fight->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                                            }

                                        jump->DespawnOrUnsummon(5000);
                                        }
                                        timerEvent = 1500;
                                    }
                                    break;
                                }
                                case 5:
                                {
                                    InitVancleefEventFlames(true);
                                    DoCastSpellOnPlayers(SPELL_NIGHTMARE_SLOW);
                                    if (auto vc = instance->GetCreature(vanessas[VANESSA_GLUBTOK]))
                                        vc->AI()->Talk(0);

                                    timerEvent = 6000;
                                    break;
                                }
                                case 6:
                                {

                                    SetActiveGauntletGroups(NM_GROUP_GLUBTOK, NM_GROUP_GLUBTOK);

                                        if (auto vc = instance->GetCreature(vanessas[VANESSA_GLUBTOK]))
                                        {
                                            vc->AI()->Talk(1);
                                            auto players = vc->GetPlayersInRange(100.f, true);
                                            for (auto player : players)
                                                if (auto slow = vc->AddAura(SPELL_NIGHTMARE_SLOW, player))
                                                {
                                                    slow->SetMaxDuration(30000);
                                                    slow->SetDuration(30000);
                                                }
                                        }
                                    timerEvent = 10000;
                                    break;
                                }
                                case 7:
                                {
                                    DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE);
                                    DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_SLOW);
                                    DoCastSpellOnPlayers(SPELL_AURA_NIGHTMARE_2);
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_GLUBTOK]))
                                    {
                                        vc->AI()->Talk(2);
                                        vc->AI()->Talk(3);
                                    }
                                    timerEvent = 5000;
                                    break;
                                }
                                case 8:
                                {
                                    if (auto vc = instance->GetCreature(vanessas[VANESSA_HELIX]))
                                        vc->SetPhaseMask(0x1, true);
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                case 9:
                                {
                                    SetActiveGauntletGroups(NM_GROUP_HELIX, NM_GROUP_HELIX);
                                    DoCastSpellOnPlayers(SPELL_NIGHTMARE);
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_HELIX]))
                                        vc->AI()->Talk(4);
                                    timerEvent = 7000;
                                    break;
                                }
                                case 10:
                                {
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_HELIX]))
                                        vc->AI()->Talk(5);
                                    timerEvent = 11000;
                                    break;
                                }
                                case 11:
                                {
                                    DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE);
                                    DoCastSpellOnPlayers(SPELL_AURA_NIGHTMARE_3);
                                    if (auto vc = instance->GetCreature(vanessas[VANESSA_HELIX]))
                                        vc->SetPhaseMask(0x4000, true);
                                    if (Creature *hl = instance->GetCreature(HelixEventGUID))
                                        hl->AI()->DoAction(ACTION_START_FIGHT);
                                    timerEvent = 5000;
                                    break;
                                }
                                case 12:
                                {
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                case 13:
                                {
                                    SetActiveGauntletGroups(NM_GROUP_REAPER, NM_GROUP_REAPER);
                                    if (GameObject *go = instance->GetGameObject(doorFOEExitGUID))
                                        go->SetGoState(GO_STATE_ACTIVE);
                                    DoCastSpellOnPlayers(SPELL_AURA_NIGHTMARE_4);
                                    Position pos = nightmare_gauntlet_positions[NMG_POS_REAPER_TELETO];
                                    //pos.m_positionX += 1.0f;
                                    //pos.m_positionY += 1.0f;
                                    if (auto c = instance->SummonCreature(NPC_REEPER_EVENT, pos))
                                    {
                                        c->AddAura(88348, c);
                                        if (auto cannon = c->FindNearestGameObject(16398, 100.f))
                                            cannon->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                    }
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_FOE_REAPER]))
                                        vc->AI()->Talk(6, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                    timerEvent = 8000;
                                    break;
                                }
                                case 14:
                                {

                                    if (auto reeper = instance->GetCreature(ReeperEventGUID))
                                        reeper->NearTeleportTo(nightmare_gauntlet_positions[NMG_POS_REAPER_TELETO].GetPositionX(), nightmare_gauntlet_positions[NMG_POS_REAPER_TELETO].GetPositionY(), nightmare_gauntlet_positions[NMG_POS_REAPER_TELETO].GetPositionZ(), nightmare_gauntlet_positions[NMG_POS_REAPER_TELETO].GetOrientation());

                                    if (auto vc = instance->GetCreature(vanessas[VANESSA_FOE_REAPER]))
                                        vc->AI()->Talk(7, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                    timerEvent = 8000;
                                    break;
                                }
                                case 15:
                                {                            
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_FOE_REAPER]))
                                        vc->AI()->Talk(8, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                    if (Creature *rp = instance->GetCreature(ReeperEventGUID))
                                        rp->AI()->DoAction(ACTION_START_FIGHT);
                                    timerEvent = 5000;
                                    break;
                                }
                                case 16:
                                {
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                case 17:
                                {
                                    SetActiveGauntletGroups(NM_GROUP_RIPSNARL, NM_GROUP_RIPSNARL);
                                    DoCastSpellOnPlayers(SPELL_NIGHTMARE);
                                    if (auto vc = instance->GetCreature(vanessas[VANESSA_RIPSNARL]))
                                        if (auto rs = instance->SummonCreature(NPC_RIPSNARL_NIGHTMARE, nightmare_gauntlet_positions[NMG_POS_RP_RIPSNARL]))
                                        {
                                            vc->NearTeleportTo(nightmare_gauntlet_positions[NMG_POS_VANCLEEF_START_RIPSNARL].GetPositionX(), nightmare_gauntlet_positions[NMG_POS_VANCLEEF_START_RIPSNARL].GetPositionY(), nightmare_gauntlet_positions[NMG_POS_VANCLEEF_START_RIPSNARL].GetPositionZ(), nightmare_gauntlet_positions[NMG_POS_VANCLEEF_START_RIPSNARL].GetOrientation());
                                            vc->SetReactState(REACT_PASSIVE);
                                            vc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                            vc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                                            if (auto rp = instance->GetCreature(ReeperEventGUID))
                                                rp->DespawnOrUnsummon();
                                            vc->AI()->TalkWithDelay(2000, 9, NULL, CHAT_MSG_MONSTER_SAY);
                                        }
                                    timerEvent = 12000;
                                    break;
                                }
                                case 18:
                                {
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_RIPSNARL]))
                                        vc->AI()->Talk(10, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                    timerEvent = 10000;
                                    break;
                                }
                                case 19:
                                {
                                    DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE);
                                    DoCastSpellOnPlayers(SPELL_AURA_NIGHTMARE_5);
                                    if (GameObject *go = instance->GetGameObject(doorDefiasCanonGUID))
                                        go->SetGoState(GO_STATE_ACTIVE);
                                    if (Creature *vc = instance->GetCreature(vanessas[VANESSA_RIPSNARL]))
                                        if (auto rs = instance->GetCreature(RipsnarlEventGUID))
                                    {
                                        vc->AI()->Talk(11, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                        vc->AI()->Talk(12, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                                        vc->CastSpell(vc, SPELL_ADRENALINE, true);
                                        vc->DespawnOrUnsummon(1000);
                                        rs->DespawnOrUnsummon(1000);
                                    }
                                    instance->SummonCreature(NPC_EMMA_HARRINGTON, PartHARRINGTON[0]);
                                    timerEvent = 1000;
                                    break;
                                }
                                case 20:
                                {
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                case 21:
                                {
                                    instance->SummonCreature(NPC_HERIC_HARRINGTON, PartHARRINGTON[1]);
                                    timerEvent = 1000;
                                    break;
                                }
                                case 22:
                                {
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                case 23:
                                {
                                    instance->SummonCreature(NPC_CALLISA_HARRINGTON, PartHARRINGTON[2]);
                                    timerEvent = 1000;
                                    break;
                                }
                                case 24:
                                {
                                    eventStep--;
                                    timerEvent = 5000;
                                    break;
                                }
                                default:
                                {
                                    _events.SetPhase(PHASE_EVENT_DONE);
                                    InitVancleefEvent(false);
                                    SetBossState(BOSS_EVENT_VANCLEEF, DONE);
                                    SetActiveGauntletGroups(NM_GROUP_AFTER_GAUNTLET, NM_GROUP_AFTER_GAUNTLET);
                                    return;
                                }
                            }
                            eventStep++;
                            _events.ScheduleEvent(EVENT_VANCLEEF_EVENT, timerEvent);
                            break;
                        }
                        case CHECK_PLAYERS_ALIVE:
                        {
                            //TC_LOG_ERROR("sql.sql", "CHECK_PLAYERS_ALIVE");
                            bool failed = false;
                            Map::PlayerList const& PlayerList = instance->GetPlayers();
                            if (!PlayerList.isEmpty())
                            {
                                failed = true;
                                for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (auto player = i->getSource())
                                        if (!player->isGameMaster())
                                            if (player->isAlive())
                                                failed = false;
                            }

                            if (GetBossState(BOSS_EVENT_VANCLEEF) == IN_PROGRESS)
                            {
                                if (failed)
                                {
                                    InitVancleefEvent(false);
                                    SetBossState(BOSS_EVENT_VANCLEEF, FAIL);
                                    fail_Gauntlet();
                                    _events.Reset();
                                }
                                else
                                    _events.ScheduleEvent(CHECK_PLAYERS_ALIVE, 1000);
                            }
                            break;
                        }
                        case CHECK_ACH_FAILED:
                        {
                            achFailed = true;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            uint64 GlubtokGUID;
            uint64 GlubtokGateGUID;
            uint64 HelixGUID;
            uint64 CaptainCookieGUID;
            uint64 VanessaVanCleefGUID;
            uint64 MagmaVehicleGUID;
            uint32 timerEvent;
            uint32 eventStep;
            uint64 GlubtokEventGUID;
            uint64 GlubtokRPLowLevelGUID;
            uint64 doorDefiasCanonGUID;
            uint64 doorFOEExitGUID;
            uint64 VanCleefEventGUID;
            uint64 HelixEventGUID;
            uint64 ReeperEventGUID;
            uint64 paperGUID;
            uint64 RipsnarlEventGUID;
            uint64 Kagtha_GUID;
            uint64 Kagtha_GUID_entrance;
            uint64 Slinky_Sharpshiv_GUID;
            uint64 Slinky_Sharpshiv_GUID_entrance;
            uint64 Miss_Mayhem_GUID;
            uint64 Miss_Mayhem_GUID_entrance;
            uint64 Mayhem_Prototype_L_GUID;
            uint64 Mayhem_Prototype_L_GUID_entrance;
            uint64 Mayhem_Prototype_R_GUID;
            uint64 Mayhem_Prototype_R_GUID_entrance;
            uint64 Lieutenant_Horatio_Laine_GUID;
            uint64 Lieutenant_Horatio_Laine_GUID_entrance;
            uint64 Stormwind_Investigator_GUID;
            uint64 Stormwind_Investigator_GUID_entrance;
            uint64 Stormwind_Investigator_2_GUID;
            uint64 Stormwind_Investigator_2_GUID_entrance;
            uint64 Crime_Scene_Bot_GUID;
            uint64 Crime_Scene_Bot_GUID_entrance;
            uint64 Crime_Scene_Bot_2_GUID;
            uint64 Crime_Scene_Bot_2_GUID_entrance;
            uint64 Crime_Scene_Bot_3_GUID;
            uint64 Crime_Scene_Bot_3_GUID_entrance;
            EventMap _events;

            uint32 dataFireWall;
            uint32 dataProdigy;
            bool ValvesDone;
            bool achFailed;
            std::multimap<uint32, uint64> npcGroups;
            npc_groups active_gauntlet_groups[2]     { NM_GROUP_BEFORE_GAUNTLET, NM_GROUP_BEFORE_GAUNTLET };
            uint64 vanessas[6]  {0, 0, 0, 0, 0, 0};
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_deadmines_InstanceMapScript(map);
        }
};

void AddSC_instance_deadmines()
{
    new instance_deadmines();
}
