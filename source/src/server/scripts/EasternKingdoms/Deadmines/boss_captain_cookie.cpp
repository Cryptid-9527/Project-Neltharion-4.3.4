/*
 * Copyright (C) 2005-2011 MaNGOS <http://www.getmangos.com/>
 *
 * Copyright (C) 2008-2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 *
 * Copyright (C) 2010-2011 Project Trinity <http://www.projecttrinity.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ToDo
 * Cookie Walk
 * Food despawn after looting
 * Mount on Cauldron or is it a gobject?
 */

#include "PassiveAI.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "deadmines.h"

enum Spells
{
    SPELL_SUMMON_CAULDRON   = 89250,
    SPELL_CAULDRON_VISUAL   = 89251,
    SPELL_CAULDRON_FIRE     = 89252,

    // Good Food
    SPELL_THROW_FOOD1       = 90557, // 48301
    SPELL_THROW_FOOD2       = 90560, // 48297
    SPELL_THROW_FOOD3       = 90556, // 48300
    SPELL_THROW_FOOD4       = 90559, // 48296
    SPELL_THROW_FOOD5       = 89263, // 48006
    SPELL_THROW_FOOD6       = 90555, // 48294

    // Bad Food
    SPELL_THROW_FOOD7       = 90606, // 48302
    SPELL_THROW_FOOD8       = 90603, // 48295
    SPELL_THROW_FOOD9       = 89739, // 48276
    SPELL_THROW_FOOD10      = 90605, // 48299
    SPELL_THROW_FOOD11      = 90602, // 48293
    SPELL_THROW_FOOD12      = 90604, // 48298

    // MISC (MURLOC
    SPELL_THROW_MURLOC      = 90680, // 48672

    // FOOD NPC
    SPELL_ROTTEN_AURA       = 89734,
    SPELL_FED               = 89267,
    SPELL_ILLNESS           = 89732,
    SPELL_GOOD_FOOD_AURA    = 89730,

    SPELL_BABY_MURLOC_DANCE = 25165,

    // Achievement
    SPELL_COOKIE_ACHIEVEMENT_AURA = 95650,
};

enum Texts
{
   EMOTE_START             = 0,
};

enum Events
{
   EVENT_MOVE_TO_FIGHT          = 1,
   EVENT_THROW_FOOD,        
   EVENT_FIRE,
   EVENT_ENRAGE,
   ACTION_APPEAR_TO_PLAYERS,
   EVENT_CHECK_WIPE,
   EVENT_BADFOOD_PUNISH,
   EVENT_HORATIO_MOVE,
};

enum npcs
{
    NPC_CAULDRON = 47754,
    NPC_BABY_MURLOCK = 48672,
};

enum points
{
    POINT_CAULDRON = 42,
};

const G3D::Vector3 Kagtha_run[]
{
    { -99.826f, -825.75f, 38.269f },
    { -92.6954f, -830.409f, 38.8053f },
    { -77.2938f, -836.352f, 40.0502f },
    { -66.5865f, -839.458f, 40.994f },
    { -66.6379f, -837.898f, 40.9929f }

};

const G3D::Vector3 Miss_Mayhem_run[]
{
    { -98.762634f, -804.494202f, 30.697504f },
    { -90.096367f, -799.921082f, 34.999245f },
    { -81.497688f, -796.536377f, 38.229774f },
    { -72.402077f, -792.562439f, 39.111553f },
    { -59.006256f, -790.838562f, 39.110596f },
    { -41.263443f, -797.242981f, 39.333950f },
    { -47.250263f, -806.734619f, 42.788483f },
    { -56.431343f, -810.484741f, 41.902466f },
    { -65.439270f, -820.195312f, 41.035927f },
    { -58.471279f, -827.580994f, 41.736206f },
    { -59.809074f, -826.444519f, 41.605595f }

};

const G3D::Vector3 Mayhem_Proto_Left_run[]
{
    { -98.762634f, -801.494202f, 30.697504f }, // .go -98.762634, -804.494202, 30.697504
    { -90.096367f, -797.921082f, 34.999245f }, // .go -90.096367, -799.921082, 34.999245
    { -81.497688f, -794.536377f, 38.229774f }, // .go -81.497688, -796.536377, 38.229774
    { -71.402077f, -790.562439f, 39.111553f },
    { -58.006256f, -788.838562f, 39.110596f },
    { -41.263443f, -797.242981f, 39.333950f },
    { -45.250263f, -804.734619f, 42.788483f }, // .go -47.250263, -806.734619, 42.788483
    { -62.851414f, -820.626160f, 41.293972f },
    { -55.058231f, -826.380493f, 42.059212f },
    { -56.641712f, -825.403931f, 41.907295f }

};

const G3D::Vector3 Mayhem_Proto_Right_run[]
{
    { -98.762634f, -801.494202f, 30.697504f }, // .go -98.762634, -804.494202, 30.697504
    { -90.096367f, -797.921082f, 34.999245f }, // .go -90.096367, -799.921082, 34.999245
    { -81.497688f, -794.536377f, 38.229774f }, // .go -81.497688, -796.536377, 38.229774
    { -72.402077f, -793.562439f, 39.111553f }, //GOOD
    //{ -42.006256f, -796.838562f, 39.110596f }, // .go -58.006256, -788.838562, 39.110596
    { -44.263443f, -797.242981f, 39.333950f }, // GOOD
    { -48.552677f, -805.394226f, 42.659866f }, // .go -47.250263, -806.734619, 42.788483
    { -68.777275f, -821.433533f, 40.733269f },
    { -57.576515f, -831.629761f, 41.828472f },
    { -59.296200f, -830.424500f, 41.663521f }

};

const G3D::Vector3 Lieutenant_Horatio_Laine_run[]
{
    { -98.762634f, -804.494202f, 30.697504f },
    { -90.096367f, -799.921082f, 34.999245f },
    { -81.497688f, -796.536377f, 38.229774f },
    { -72.402077f, -792.562439f, 39.111553f },
    { -59.006256f, -790.838562f, 39.110596f },
    { -41.263443f, -797.242981f, 39.333950f },
    { -47.250263f, -806.734619f, 42.788483f },
    { -55.982803f, -811.536072f, 41.946941f }, // .go -56.431343, -810.484741, 41.902466

};

const G3D::Vector3 Stormwind_Investigator_run[]
{
    { -93.745529f, -788.862488f, 27.169783f },
    { -102.679680f, -793.596985f, 28.102270f },
    { -97.977379f, -802.583923f, 30.798628f },
    { -90.586357f, -798.360657f, 35.110725f },
    { -81.410851f, -794.055420f, 38.392918f },
    { -68.857101f, -790.157593f, 39.641346f },
    { -52.276066f, -791.714783f, 38.529137f },
    { -39.780216f, -797.388184f, 39.335670f },
    { -46.057426f, -808.062134f, 42.895882f },
    { -56.206532f, -811.157654f, 41.924984f },
    { -66.359985f, -809.776306f, 41.000595f },
    { -71.886368f, -804.578491f, 40.469692f },
};

const G3D::Vector3 Stormwind_Investigator_2_run[]
{
    { -95.152588f, -785.911865f, 27.416113f },
    { -105.865555f, -791.364746f, 28.253323f },
    { -98.632912f, -805.970154f, 30.639496f },
    { -89.045677f, -801.170166f, 34.887001f },
    { -81.773285f, -798.731384f, 38.106625f },
    { -67.003166f, -792.011475f, 39.630203f },
    { -52.579319f, -793.230225f, 38.478104f },
    { -43.637516f, -797.223450f, 39.329601f },
    { -48.540726f, -806.356812f, 42.663017f },
    { -65.561554f, -808.510803f, 41.069321f },
    { -65.809486f, -805.925476f, 41.032944f },
    { -64.047653f, -805.002991f, 41.189972f },
};

const G3D::Vector3 Crime_Scene_bot_run[]
{
    { -93.745529f, -788.862488f, 27.169783f },
    { -102.679680f, -793.596985f, 28.102270f },
    { -97.977379f, -802.583923f, 30.798628f },
    { -90.586357f, -798.360657f, 35.110725f },
    { -81.410851f, -794.055420f, 38.392918f },
    { -68.857101f, -790.157593f, 39.641346f },
    { -52.276066f, -791.714783f, 38.529137f },
    { -39.780216f, -797.388184f, 39.335670f },
    { -46.057426f, -808.062134f, 42.895882f },
    { -56.206532f, -811.157654f, 41.924984f },//
    { -63.462822f, -825.485718f, 41.244377f },
};

const G3D::Vector3 Crime_Scene_bot_2_run[]
{
    { -95.152588f, -785.911865f, 27.416113f },
    { -105.865555f, -791.364746f, 28.253323f },
    { -98.632912f, -805.970154f, 30.639496f },
    { -89.045677f, -801.170166f, 34.887001f },
    { -81.773285f, -798.731384f, 38.106625f },
    { -67.003166f, -792.011475f, 39.630203f },
    { -52.579319f, -793.230225f, 38.478104f },
    { -43.637516f, -797.223450f, 39.329601f },
    { -48.540726f, -806.356812f, 42.663017f },
    { -65.561554f, -808.510803f, 41.069321f },//
    { -69.956352f, -812.792603f, 40.660450f },
};

const G3D::Vector3 Crime_Scene_bot_3_run[]
{
    { -92.872581f, -786.244995f, 27.145144f },
    { -105.822609f, -792.681335f, 28.186781f },
    { -98.604141f, -804.443848f, 30.703867f },
    { -89.893517f, -800.002747f, 34.986633f },
    { -82.361069f, -797.165527f, 38.169106f },
    { -69.163544f, -791.040527f, 39.556664f },
    { -50.927315f, -793.170532f, 38.638634f },
    { -41.502441f, -797.789551f, 39.326340f },
    { -47.405899f, -806.736572f, 42.773640f },
    { -57.065559f, -809.464661f, 41.840321f },
    { -59.158985f, -816.365417f, 41.652596f },
    { -56.799427f, -815.565430f, 41.875877f },
};

class boss_captain_cookie : public CreatureScript
{
    public:
        boss_captain_cookie() : CreatureScript("boss_captain_cookie") {}

    struct boss_captain_cookieAI : public BossAI
    {
        boss_captain_cookieAI(Creature * creature) : BossAI(creature, BOSS_CAPTAIN_COOKIE)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, 89734, true);
            me->ApplySpellImmune(0, IMMUNITY_ID, 89252, true);
            me->SetVisible(false);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER);
        }

        void Reset()
        {
            me->SetPower(POWER_UNUSED, -1);
            events.Reset();
            me->ExitVehicle();
            summons.DespawnAll();
            me->NearTeleportTo(me->GetHomePosition(), true);

            me->AddAura(89339, me);
            me->AddAura(88873, me);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            _isSpecial = false;
            _Reset();

            me->DespawnCreaturesInArea(48672);
            me->DespawnCreaturesInArea(48293);
            me->DespawnCreaturesInArea(48302);
            me->DespawnCreaturesInArea(48295);
            me->DespawnCreaturesInArea(48276);
            me->DespawnCreaturesInArea(48299);
            me->DespawnCreaturesInArea(48293);
            me->DespawnCreaturesInArea(48298);
            me->DespawnCreaturesInArea(48300);
            me->DespawnCreaturesInArea(48294);
            me->DespawnCreaturesInArea(48006);
            me->DespawnCreaturesInArea(48296);
            me->DespawnCreaturesInArea(48301);
            me->DespawnCreaturesInArea(48297);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_TRAINER);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            if (instance->GetBossState(BOSS_ADMIRAL_RISNARL) == DONE)
                if (instance->GetBossState(BOSS_CAPTAIN_COOKIE) != DONE)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    active = false;
                    waiting = true;
                    me->SetVisible(true);
                    if (!events.HasEvent(ACTION_APPEAR_TO_PLAYERS))
                    events.ScheduleEvent(ACTION_APPEAR_TO_PLAYERS, 2500);
                    me->SetHomePosition(CauldronSpot);
                }
            instance->DoRemoveAurasDueToSpellOnPlayers(95650);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void Punish_BadFood()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            if (players.size())
                for (Player* player : players)
                {
                    if (auto gas = player->FindNearestCreature(48293, 3.5f, true))
                    {
                        if (gas->HasAura(SPELL_ROTTEN_AURA))
                            gas->AI()->DoCast(player, 89735, true);
                    }
                    else if (auto gas = player->FindNearestCreature(48295, 3.5f, true))
                    {
                        if (gas->HasAura(SPELL_ROTTEN_AURA))
                            gas->AI()->DoCast(player, 89735, true);
                    }
                    else if (auto gas = player->FindNearestCreature(48276, 3.5f, true))
                    {
                        if (gas->HasAura(SPELL_ROTTEN_AURA))
                            gas->AI()->DoCast(player, 89735, true);
                    }
                    else if (auto gas = player->FindNearestCreature(48299, 3.5f, true))
                    {
                        if (gas->HasAura(SPELL_ROTTEN_AURA))
                            gas->AI()->DoCast(player, 89735, true);
                    }
                    else if (auto gas = player->FindNearestCreature(48298, 3.5f, true))
                    {
                        if (gas->HasAura(SPELL_ROTTEN_AURA))
                            gas->AI()->DoCast(player, 89735, true);
                    }
                }
        }

        void InitializeAI()
        {
            me->SetPower(POWER_UNUSED, -1);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
            me->AddAura(89339, me);
            me->AddAura(88873, me);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            me->SetHomePosition(CauldronSpot);

            if (instance->GetBossState(BOSS_ADMIRAL_RISNARL) == DONE)
            if (instance->GetBossState(BOSS_CAPTAIN_COOKIE) != DONE)
            {
                active = false;
                waiting = true;
                me->SetVisible(true);
                if (!events.HasEvent(ACTION_APPEAR_TO_PLAYERS))
                    events.ScheduleEvent(ACTION_APPEAR_TO_PLAYERS, 2500);
                me->SetHomePosition(CauldronSpot);
            }
        }
        void EnterCombat(Unit * /*who*/)
        {
            _isSpecial = false;
            me->SetHomePosition(CauldronSpot);
            HasFought = true;
            _EnterCombat();
            DoCastAOE(SPELL_COOKIE_ACHIEVEMENT_AURA);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_BADFOOD_PUNISH, 1000);
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
            if (me->GetDistance(CauldronSpot) < 2.f)
                if (!me->FindNearestCreature(NPC_CAULDRON, 5.f))
                    MovementInform(POINT_MOTION_TYPE, POINT_CAULDRON);
        }

        void JustDied(Unit* killer)
        {
            events.Reset();
            me->DespawnCreaturesInArea(48522, 300.f);
            me->DespawnCreaturesInArea(48521, 300.f);
            me->DespawnCreaturesInArea(48447, 300.f);
            me->DespawnCreaturesInArea(48450, 300.f);
            _JustDied();
            summons.DespawnAll();
            me->ClearUnitState(UNIT_STATE_CASTING);
            me->CastStop();
            me->SetFacingTo(3.417274f);

            //foods
            me->DespawnCreaturesInArea(48301, 300.f);
            me->DespawnCreaturesInArea(48297, 300.f);
            me->DespawnCreaturesInArea(48300, 300.f);
            me->DespawnCreaturesInArea(48296, 300.f);
            me->DespawnCreaturesInArea(48006, 300.f);
            me->DespawnCreaturesInArea(48294, 300.f);
            me->DespawnCreaturesInArea(48302, 300.f);
            me->DespawnCreaturesInArea(48295, 300.f);
            me->DespawnCreaturesInArea(48276, 300.f);
            me->DespawnCreaturesInArea(48299, 300.f);
            me->DespawnCreaturesInArea(48293, 300.f);
            me->DespawnCreaturesInArea(48298, 300.f);

            if (auto v = killer->ToPlayer())
                if (v->GetTeam() == HORDE)
                {
                    //Despawn Alliance
                    if (auto guid = instance->GetData64(DATA_LIEUTENANT_HORATIO_LAINE))
                        if (auto laine = ObjectAccessor::GetCreature(*me, guid))
                            laine->DespawnOrUnsummon();
                }
                else
                {
                    //Despawn Horde
                    if (auto guid = instance->GetData64(DATA_KAGTHA))
                        if (auto kagtha = ObjectAccessor::GetCreature(*me, guid))
                            kagtha->DespawnOrUnsummon();
                }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT_ENTRANCE))
                    if (auto Crime_Scene_Bot = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT))
                    if (auto Crime_Scene_Bot = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot->SetVisible(true);
                        Crime_Scene_Bot->GetMotionMaster()->MoveSmoothPath(Crime_Scene_bot_run, 11, 5.5f);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT_2_ENTRANCE))
                    if (auto Crime_Scene_Bot_2 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot_2->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT_2))
                    if (auto Crime_Scene_Bot_2 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot_2->SetVisible(true);
                        Crime_Scene_Bot_2->GetMotionMaster()->MoveSmoothPath(Crime_Scene_bot_2_run, 11, 6.f);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT_3_ENTRANCE))
                    if (auto Crime_Scene_Bot_3 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot_3->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_CRIME_SCENE_BOT_3))
                    if (auto Crime_Scene_Bot_3 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Crime_Scene_Bot_3->SetVisible(true);
                        Crime_Scene_Bot_3->GetMotionMaster()->MoveSmoothPath(Crime_Scene_bot_3_run, 12, 6.f);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_STORMWIND_INVESTIGATOR_2_ENTRANCE))
                    if (auto Stormwind_Investigator_2 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Stormwind_Investigator_2->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_STORMWIND_INVESTIGATOR_2))
                    if (auto Stormwind_Investigator_2 = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Stormwind_Investigator_2->SetVisible(true);
                        Stormwind_Investigator_2->GetMotionMaster()->MoveSmoothPath(Stormwind_Investigator_2_run, 12, 8.f);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_STORMWIND_INVESTIGATOR_ENTRANCE))
                    if (auto Stormwind_Investigator = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Stormwind_Investigator->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_STORMWIND_INVESTIGATOR))
                    if (auto Stormwind_Investigator = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Stormwind_Investigator->SetVisible(true);
                        Stormwind_Investigator->GetMotionMaster()->MoveSmoothPath(Stormwind_Investigator_run, 12, 7.5f);
                        Stormwind_Investigator->SetStandState(UNIT_STAND_STATE_KNEEL);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_LIEUTENANT_HORATIO_LAINE_ENTRANCE))
                    if (auto Lieutenant_Horatio_Laine = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Lieutenant_Horatio_Laine->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_LIEUTENANT_HORATIO_LAINE))
                    if (auto Lieutenant_Horatio_Laine = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Lieutenant_Horatio_Laine->SetVisible(true);
                        Lieutenant_Horatio_Laine->GetMotionMaster()->MoveSmoothPath(Lieutenant_Horatio_Laine_run, 8, 8.f);
                    }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MAYHEM_PROTOTYPE_R_ENTRANCE))
                    if (auto Prototype_Mayhem_R = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Prototype_Mayhem_R->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MAYHEM_PROTOTYPE_R))
                    if (auto Prototype_Mayhem_R = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Prototype_Mayhem_R->SetVisible(true);
                        Prototype_Mayhem_R->GetMotionMaster()->MoveSmoothPath(Mayhem_Proto_Right_run, 9, 8.f);
                    }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MAYHEM_PROTOTYPE_L_ENTRANCE))
                    if (auto Prototype_Mayhem_L = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Prototype_Mayhem_L->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MAYHEM_PROTOTYPE_L))
                    if (auto Prototype_Mayhem_L = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Prototype_Mayhem_L->SetVisible(true);
                        Prototype_Mayhem_L->GetMotionMaster()->MoveSmoothPath(Mayhem_Proto_Left_run, 10, 8.f);
                    }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MISS_MAYHEM_ENTRANCE))
                    if (auto Miss_Mayhem = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Miss_Mayhem->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_MISS_MAYHEM))
                    if (auto Miss_Mayhem = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Miss_Mayhem->SetVisible(true);
                        Miss_Mayhem->GetMotionMaster()->MoveSmoothPath(Miss_Mayhem_run, 11, 8.f);
                    }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_SLINKY_SHARPSHIV_ENTRANCE))
                    if (auto Slinky_Sharpshiv = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Slinky_Sharpshiv->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_SLINKY_SHARPSHIV))
                    if (auto Slinky_Sharpshiv = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Slinky_Sharpshiv->SetVisible(true);
                    }

            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_KAGTHA_ENTRANCE))
                    if (auto Kagtha = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Kagtha->SetVisible(false);
                    }
            if (!IsHeroic())
                if (auto guid = instance->GetData64(DATA_KAGTHA))
                    if (auto Kagtha = ObjectAccessor::GetCreature(*me, guid))
                    {
                        Kagtha->SetVisible(true);
                        Kagtha->GetMotionMaster()->MoveSmoothPath(Kagtha_run, 5, 8.f);
                        Kagtha->TalkWithDelay(0, 0, NULL, CHAT_MSG_MONSTER_YELL);
                        Kagtha->TalkWithDelay(6000, 1, NULL, CHAT_MSG_MONSTER_YELL);
                        Kagtha->TalkWithDelay(9000, 2, NULL, CHAT_MSG_MONSTER_YELL);
                    }

            if (!IsHeroic())
                return;
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
            if (id == POINT_CAULDRON)
            {
                events.ScheduleEvent(EVENT_ENRAGE, 180000);
                events.ScheduleEvent(EVENT_THROW_FOOD, 5000);
                events.ScheduleEvent(EVENT_FIRE, 10000);
                DoCast(SPELL_SUMMON_CAULDRON);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_AGGRESSIVE);
                if (auto p = me->FindNearestPlayer(100.f))
                    me->AI()->AttackStart(p);
                me->RemoveAura(89339);
                me->SetReactState(REACT_PASSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            }
        }

        void DoAction(const int32 act)
        {
            switch (act)
            {
            case ACTION_ACTIVATE:
                if (instance->GetBossState(BOSS_ADMIRAL_RISNARL) == DONE)
                {
                    me->SetVisible(true);
                    if (!events.HasEvent(ACTION_APPEAR_TO_PLAYERS))
                        events.ScheduleEvent(ACTION_APPEAR_TO_PLAYERS, 2500);
                }
                break;
            default:
                break;
            }
        }

        void JustSummoned(Creature *summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_CAULDRON:
                    summon->CastSpell(summon, SPELL_CAULDRON_FIRE, true);
                    summon->CastSpell(summon, SPELL_CAULDRON_VISUAL, false);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                    break;
                case NPC_BABY_MURLOCK: // wtf ?
                    summon->CastSpell(summon, SPELL_BABY_MURLOC_DANCE, true);
                    break;
                default:
                    break;
            }
            BossAI::JustSummoned(summon);
            if (me->isDead())
                summon->DespawnOrUnsummon();
        }

        uint32 GetRandomFoodSpell()
        {
            if (_isSpecial)
                return SPELL_THROW_MURLOC;
            switch (urand(0, 11))
            {
                case 0:
                    return SPELL_THROW_FOOD1;
                case 1:
                    return SPELL_THROW_FOOD2;
                case 2:
                    return SPELL_THROW_FOOD3;
                case 3:
                    return SPELL_THROW_FOOD4;
                case 4:
                    return SPELL_THROW_FOOD5;
                case 5:
                    return SPELL_THROW_FOOD6;
                case 6:
                    return SPELL_THROW_FOOD7;
                case 7:
                    return SPELL_THROW_FOOD8;
                case 8:
                    return SPELL_THROW_FOOD9;
                case 9:
                    return SPELL_THROW_FOOD10;
                case 10:
                    return SPELL_THROW_FOOD11;
                case 11:
                    return SPELL_THROW_FOOD12;
                default:
                    return SPELL_THROW_FOOD1;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->isInCombat())
            if (who->ToPlayer())
                if (me->GetDistance(who) < 10.f)
                    if (!active)
                    if (waiting)
                    {
                        waiting = false;
                            events.ScheduleEvent(EVENT_MOVE_TO_FIGHT, 1000);
                    }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_BADFOOD_PUNISH:
                        if (!me->HasAura(88873))
                            DoCast(me, 88873, true);
                        Punish_BadFood();
                        events.ScheduleEvent(EVENT_BADFOOD_PUNISH, 1000);
                        break;
                    case EVENT_MOVE_TO_FIGHT:
                        active = true;
                        me->PlayDistanceSound(416);
                        me->GetMotionMaster()->MovePoint(POINT_CAULDRON, CauldronSpot);
                        me->SetHomePosition(CauldronSpot);
                        break;
                    case EVENT_THROW_FOOD:
                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            events.ScheduleEvent(eventId, 250);
                        else
                        {
                            DoCastRandom(GetRandomFoodSpell(), 0.0f);
                            events.ScheduleEvent(EVENT_THROW_FOOD, 2000);
                        }
                        break;
                    case EVENT_ENRAGE:
                        _isSpecial = true;
                        break;
                    case ACTION_APPEAR_TO_PLAYERS:
                        Talk(0);
                        break;
                    case EVENT_CHECK_WIPE:
                        if (checkWipe())
                        {
                            me->DeleteThreatList();
                            me->CombatStop(true);
                            me->Respawn(true);
                            Reset();
                        }
                        else
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                        break;
                }
        }

        private:
            EventMap events;
            Position HomePosOnDeath{ -64.46f, -820.76f, 41.14f, 3.417274f };
            Position CauldronSpot{ -64.46f, -820.76f, 41.14f, 3.417274f };
            bool _isSpecial{ false };
            bool HasFought{ false };
            bool active{ false };
            bool waiting{ true };
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new boss_captain_cookieAI(creature);
    }
};

class npc_horatio_laine : public CreatureScript
{
public:
    npc_horatio_laine() : CreatureScript("npc_horatio_laine") { }

    struct npc_horatio_laineAI : public ScriptedAI
    {
        npc_horatio_laineAI(Creature* creature) : ScriptedAI(creature)
        {}

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == SPLINE_MOTION_TYPE && id == 7)
            {
                me->TalkWithDelay(1000, 0, NULL, CHAT_MSG_MONSTER_SAY);
                me->TalkWithDelay(9000, 1, NULL, CHAT_MSG_MONSTER_SAY);
                me->TalkWithDelay(21000, 2, NULL, CHAT_MSG_MONSTER_SAY);
                events.ScheduleEvent(EVENT_HORATIO_MOVE, 42000);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                if (auto guid = instance->GetData64(BOSS_CAPTAIN_COOKIE))
                    if (auto captainCook = ObjectAccessor::GetCreature(*me, guid))
                        captainCook->SetFacingTo(3.417274f);

            }
            if (type == POINT_MOTION_TYPE && id == 28)
            {
                events.ScheduleEvent(2, 1000);
                events.ScheduleEvent(4, 2000);
                me->TalkWithDelay(4000, 3, NULL, CHAT_MSG_MONSTER_SAY);
                me->TalkWithDelay(8000, 4, NULL, CHAT_MSG_MONSTER_SAY);
                events.ScheduleEvent(3, 28000);
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                case EVENT_HORATIO_MOVE:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        events.ScheduleEvent(EVENT_HORATIO_MOVE, 1000);
                    }
                    else
                    {
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        me->ClearUnitState(UNIT_STATE_NOT_MOVE);
                        me->ClearUnitState(UNIT_STATE_CANNOT_TURN);

                        if (auto guid = instance->GetData64(BOSS_CAPTAIN_COOKIE))
                            if (auto captainCook = ObjectAccessor::GetCreature(*me, guid))
                            {
                                Position cookieDeathPos
                                {
                                    -62.932137f, -818.606262f, 41.282516f, 3.868091f
                                };
                                me->GetMotionMaster()->MovePoint(28, cookieDeathPos, true, 3.f, false);
                            }
                    }
                    break;
                case 2:
                    me->SetFacingTo(3.868091f);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    break;
                case 3:
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    break;
                case 4:
                    DoCast(78935);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horatio_laineAI(creature);
    }
};

class npc_food : public CreatureScript
{
public:
    npc_food() : CreatureScript("npc_food") {}

    struct npc_foodAI : public PassiveAI
    {
        npc_foodAI(Creature* pCreature) : PassiveAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
            me->ApplySpellImmune(0, IMMUNITY_ID, 89252, true);
        }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
            if (IsRottenFood())
                me->AddAura(SPELL_ROTTEN_AURA, me);
            else
                me->AddAura(SPELL_GOOD_FOOD_AURA, me);
        }

        void IsSummonedBy(Unit* /*who*/)
        {
            if (auto creature = Unit::GetCreature(*me, pInstance->GetData64(BOSS_CAPTAIN_COOKIE)))
            {
                creature->AI()->JustSummoned(me);

            }
        }

        bool IsRottenFood()
        {
            switch (me->GetEntry())
            {
                case 48302:
                case 48295:
                case 48276:
                case 48299:
                case 48293:
                case 48298:
                    return true;
            }
            return false;
        }

        void OnSpellClick(Unit* player, bool& result)
        {
            if (pInstance)
            {
                //Pendulum mechanic between rotten food and good food
                uint32 aura_Satiated = (pInstance->instance->IsHeroic() ? 92834 : 89267);
                uint32 aura_Nauseated = (pInstance->instance->IsHeroic() ? 92066 : 89732);

                if (IsRottenFood())
                {
                    if (auto nauseated = player->GetAura(aura_Nauseated))
                        if (nauseated->GetStackAmount() > 1)
                            player->RemoveAura(SPELL_COOKIE_ACHIEVEMENT_AURA);
                }
                else
                {
                    if (auto nauseated = player->GetAura(aura_Nauseated))
                        nauseated->ModStackAmount(-1);
                }
            }
            me->DespawnOrUnsummon(100);
        }

    private:
        InstanceScript *pInstance;
    };

    CreatureAI* GetAI(Creature *pCreature) const
    {
        return new npc_foodAI(pCreature);
    }
};

class npc_cc_baby_murlock : public CreatureScript
{
public:
    npc_cc_baby_murlock() : CreatureScript("npc_cc_baby_murlock") {}

    struct npc_cc_baby_murlockAI : public PassiveAI
    {
        npc_cc_baby_murlockAI(Creature* pCreature) : PassiveAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        void Reset()
        {
        }

        void IsSummonedBy(Unit* /*who*/)
        {
            if (pInstance)
                if (Creature *creature = Unit::GetCreature(*me, pInstance->GetData64(BOSS_CAPTAIN_COOKIE)))
                    creature->AI()->JustSummoned(me);
        }
    private:
        InstanceScript *pInstance;
    };

    CreatureAI* GetAI(Creature *pCreature) const
    {
        return new npc_cc_baby_murlockAI(pCreature);
    }
};

void AddSC_boss_captain_cookie()
{
    new boss_captain_cookie();
    new npc_horatio_laine();
    new npc_food();
    new npc_cc_baby_murlock();
}
