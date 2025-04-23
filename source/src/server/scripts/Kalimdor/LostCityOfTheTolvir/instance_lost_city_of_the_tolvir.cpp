/*
 * Copyright (C) 2011- 2013 ArkCORE <http://www.arkania.net/>
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
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "the_lost_city_of_tol_vir.h"

#define ENCOUNTERS 5

/* Boss Encounters
   General Husam
   High Prophet Barim
   Lockmaw
   Augh
   Siamat
 */
const Position aughPos = {-11059.720703f, -1615.721924f, 0.000794f, 4.628290f };

class instance_lost_city_of_the_tolvir : public InstanceMapScript
{
public:
    instance_lost_city_of_the_tolvir() : InstanceMapScript("instance_lost_city_of_the_tolvir", 755) { }

    struct instance_lost_city_of_the_tolvir_InstanceMapScript: public InstanceScript
    {
        instance_lost_city_of_the_tolvir_InstanceMapScript(InstanceMap* map) : InstanceScript(map) 
        {
            SetHeaders(DataHeader);
            SetBossNumber(ENCOUNTERS);
        }

        uint32 uiEncounter[ENCOUNTERS];

        uint64 GeneralHusamGUID         { 0 };
        uint64 HighProphetBarimGUID     { 0 };
        uint64 LockmawGUID              { 0 };
        uint64 AughGUID                 { 0 };
        uint64 SiamatGUID               { 0 };
        uint64 SiamatPlatform           { 0 };

        void Initialize()
        {
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (!introDone
                && IsDone(DATA_GENERAL_HUSAM_EVENT)
                && IsDone(DATA_HIGH_PROPHET_BARIM_EVENT)
                && IsDone(DATA_LOCKMAW_EVENT)
                && (!instance->IsHeroic() || IsDone(DATA_AUGH_EVENT)))
            {
                introDone = true;
                if (auto siamat = instance->GetCreature(SiamatGUID))
                {
                    siamat->AI()->DoAction(ACTION_INTRO);
                    if (auto platform = siamat->FindNearestGameObject(GO_SIAMAT_PLATFORM, 200.f))
                        platform->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                }
            }
            return true;
        }

        void OnCreatureCreate(Creature* creature)
        {
            InstanceScript::OnCreatureCreate(creature);

            switch(creature->GetEntry())
            {
                 case BOSS_GENERAL_HUSAM:
                     GeneralHusamGUID = creature->GetGUID();
                     break;
                 case BOSS_HIGH_PROPHET_BARIM:
                     HighProphetBarimGUID = creature->GetGUID();
                     break;
                 case BOSS_LOCKMAW:
                     LockmawGUID = creature->GetGUID();
                     break;
                 case BOSS_AUGH:
                     AughGUID = creature->GetGUID();
                     break;
                 case BOSS_SIAMAT:
                     SiamatGUID = creature->GetGUID();
                     break;
            }
        }

        void OnGameObjectCreate(GameObject* gameobject)
        {
            switch (gameobject->GetEntry())
            {
                case GO_SIAMAT_PLATFORM:
                    if (!introDone
                        && IsDone(DATA_GENERAL_HUSAM_EVENT)
                        && IsDone(DATA_HIGH_PROPHET_BARIM_EVENT)
                        && IsDone(DATA_LOCKMAW_EVENT)
                        && (!instance->IsHeroic() || IsDone(DATA_AUGH_EVENT)))
                    {
                        introDone = true;
                        gameobject->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                    }
                    SiamatPlatform = gameobject->GetGUID();
                    break;
                case 206660: 
                case 206661:
                    gameobject->SetPhaseMask(0x1, true);
                    gameobject->setActive(true);
                    //TC_LOG_ERROR("sql.sql", "spawning gameobject %u", gameobject->GetEntry());
                    break;
                default:
                    break;
            }
        }

        uint64 GetData64(uint32 identifier) const
        {
            switch(identifier)
            {
                case DATA_GENERAL_HUSAM:
                    return GeneralHusamGUID;
                case DATA_HIGH_PROPHET_BARIM:
                    return HighProphetBarimGUID;
                case DATA_LOCKMAW:
                    return LockmawGUID;
                case DATA_AUGH:
                    return AughGUID;
                case DATA_SIAMAT:
                    //TC_LOG_ERROR("sql.sql", "siamat guid found");
                    return SiamatGUID;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {

                case DATA_GENERAL_HUSAM_EVENT:
                    uiEncounter[0] = data;
                    break;
                case DATA_HIGH_PROPHET_BARIM_EVENT:
                    uiEncounter[1] = data;
                    break;
                case DATA_LOCKMAW_EVENT:
                    uiEncounter[2] = data;
                    break;
                case DATA_AUGH_EVENT:
                    uiEncounter[3] = data;
                    break;
                case DATA_SIAMAT_EVENT:
                    uiEncounter[4] = data;
                    break;
            }

           if (data == DONE)
               SaveToDB();
        }

        uint32 GetData(uint32 type) const
        {
            switch(type)
            {
                case DATA_GENERAL_HUSAM_EVENT:
                    return uiEncounter[0];
                case DATA_HIGH_PROPHET_BARIM_EVENT:
                    return uiEncounter[1];
                case DATA_LOCKMAW_EVENT:
                    return uiEncounter[2];
                case DATA_AUGH_EVENT:
                    return uiEncounter[3];
                case DATA_SIAMAT_EVENT:
                    return uiEncounter[4];
            }
            return 0;
        }

        private:
            bool introDone{ false };
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_lost_city_of_the_tolvir_InstanceMapScript(map);
    }
};


void AddSC_instance_lost_city_of_the_tolvir()
{
    new instance_lost_city_of_the_tolvir();
}
