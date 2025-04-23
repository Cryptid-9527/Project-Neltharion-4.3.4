/*
* Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData by Wondi
SDName: Silverpine_Forest
SD%Complete: 100
SDCategory: Silverpine Forest
EndScriptData */


#include "AchievementMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "Player.h"
#include "Vehicle.h"
#include "GameObject.h"
#include "CreatureAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SmartAI.h"
#include "SmartScript.h"


enum SilverpineDefaultSpells
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_PING_2 = 98915,
    SPELL_DUMMY_PING_3 = 98916,
    SPELL_SANCTUARY_NO_COMBAT = 88467,
    SPELL_ROOT = 42716,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_FLIGHT_SPEED_280 = 50345,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum SilverpineEvents
{
    EVENT_WARCHIEF_COMETH_SUMMONS = 1,
    EVENT_WARCHIEF_COMETH_SUMMONS_2,
    EVENT_WARCHIEF_COMETH_3,
    EVENT_WARCHIEF_COMETH_4,
    EVENT_WARCHIEF_COMETH_5,
    EVENT_FORSAKEN_BAT_1,
    EVENT_FORSAKEN_BAT_2,
    EVENT_YORICK_1,
    EVENT_YORICK_2,
    EVENT_ORC_SEA_PUP_1,
    EVENT_DOOMHOWL,
    EVENT_UNHOLY_SMITE,
    EVENT_UNHOLY_DARKNESS,
    EVENT_NO_ESCAPE_1,
    EVENT_NO_ESCAPE_2,
    EVENT_NO_ESCAPE_3,
    EVENT_NO_ESCAPE_4,
    EVENT_WARHORSE_1,
    EVENT_WARHORSE_2,
    EVENT_WARHORSE_3,
    EVENT_HORDE_CANNON_1,
    EVENT_HORDE_CANNON_2,
    EVENT_HORDE_CANNON_3,
    EVENT_HORDE_CANNON_4,
    EVENT_HORDE_CANNON_5,
    EVENT_HORDE_CANNON_6,
    EVENT_HORDE_CANNON_7,
    EVENT_FORTESKI_1,
    EVENT_FORTESKI_2,
    EVENT_FORTESKI_3,
    EVENT_FORTESKI_4,
    EVENT_FORTESKI_5,
    EVENT_FORTESKI_6,
    EVENT_FORTESKI_7,
    EVENT_FORTESKI_8,
    EVENT_INTRO_FLIGHT_ARTHURA,
    EVENT_FANOFKNIFES,
    EVENT_SHADOWSTEP,
    EVENT_ARTHURA_FLIGHT_1,
    EVENT_RISE_GODFREY_1,
    EVENT_RISE_GODFREY_2,
    EVENT_RISE_GODFREY_3,
    EVENT_RISE_GODFREY_4,
    EVENT_RISE_GODFREY_5,
    EVENT_ATAERIC_1,
    EVENT_ATAERIC_2,
    EVENT_ATAERIC_3,
    EVENT_ATAERIC_4,
    EVENT_ATAERIC_5,
    EVENT_ATAERIC_6,
    EVENT_WALDEN_SMITE,
    EVENT_WALDEN_HOLYNOVA,
    EVENT_GODFREY_FIREBALL,
    EVENT_GODFREY_BLASTWAVE,
    EVENT_GODFREY_FLAMESTRIKE,
    EVENT_ASHBURY_MINDBLAST,
    EVENT_ASHBURY_PAIN,
    EVENT_ASHBURY_PSYCHIC,
    EVENT_COWERINGTROPPER_1,
    EVENT_COWERINGTROPPER_2,
    EVENT_COWERINGTROPPER_3,
    EVENT_MAJESTY_1,
    EVENT_MAJESTY_2,
    EVENT_MAJESTY_3,
    EVENT_LORNA_CLEAVE,
    EVENT_LORNA_THUNDERCLAP,
    EVENT_LORNA_CHARGE,
    EVENT_TELESCOPE_EXIT,
    EVENT_TELESCOPE_START_PATH,
    EVENT_HAULER_NORTH_1,
    EVENT_HAULER_NORTH_2,
    EVENT_HAULER_NORTH_3,
    EVENT_HAULER_NORTH_4,
    EVENT_HAULER_SOUTH_1,
    EVENT_HAULER_SOUTH_2,
    EVENT_HAULER_SOUTH_3,
    EVENT_HAULER_SOUTH_4,
    EVENT_MUTANT_CHICKEN_1,
    EVENT_FINAL_SCENE_1,
    EVENT_FINAL_SCENE_2,
    EVENT_FINAL_SCENE_3,
    EVENT_FINAL_SCENE_FACING,
    EVENT_FINAL_SCENE_AGATHA_FACING,
    EVENT_FINAL_SCENE_FACING_2,
    EVENT_FINAL_SCENE_FACING_3,
    EVENT_FINAL_LOW_HP,
    EVENT_FINAL_SCENE_4,
    EVENT_FINAL_SCENE_5,
    EVENT_FINAL_SCENE_6,
    EVENT_FINAL_SCENE_7,
    EVENT_FINAL_SCENE_8,
    EVENT_FINAL_SCENE_9,
    EVENT_FINAL_SCENE_10,
    EVENT_FINAL_SCENE_11,
    EVENT_FINAL_SCENE_12,
    EVENT_FINAL_SCENE_13,
    EVENT_FINAL_SCENE_14,
    EVENT_FINAL_SCENE_15,
    EVENT_FINAL_SCENE_16,
    EVENT_FINAL_SCENE_17,
    EVENT_FINAL_SCENE_18,
    EVENT_FINAL_SCENE_19,
    EVENT_FINAL_SCENE_20,
    EVENT_FINAL_SCENE_21,
    EVENT_FINAL_SCENE_22,
    EVENT_FINAL_SCENE_23,
    EVENT_FINAL_SCENE_24,
    EVENT_FINAL_SCENE_25,
    EVENT_FINAL_SCENE_26,
    EVENT_FINAL_SCENE_27,
    EVENT_FINAL_SCENE_28,
    EVENT_THUNDERCLAP,
    EVENT_MORTAL_STRIKE,

    ACTION_ORC_SEA_PUP_1 = 1,

    QUEST_ON_HER_MAJESTY = 27594,
};

enum WarchiefCometh
{
    NPC_SCENE_CONTROLLER = 74954,
    NPC_PORTAL_FROM_ORGRIMMAR = 44630,
    NPC_HELLSCREAMS_ELITE = 44636,
    NPC_GARROSH_HELLSCREAM = 44629,
    NPC_HIGH_WARLORD_CROMUSH = 44640,
    NPC_LADY_SYLVANAS = 44365,
    NPC_FALLEN_HUMAN = 44593,

    NPC_VALKYR_DASCHLA = 44609,
    NPC_VALKYR_AGATHA = 44608,
    NPC_VALKYR_ARTHURA = 44610,

    SPELL_RISE_FORSAKEN = 83173,
    SPELL_FORSAKEN_TRANSFORM = 83149,
    SPELL_FORSAKEN_EXPLODE = 81876,

};

// cutscene quest=26965/the-warchief-cometh
class npc_warchief_cometh_controller : public CreatureScript
{
public:
    npc_warchief_cometh_controller() : CreatureScript("npc_warchief_cometh_controller") { }

    struct npc_warchief_cometh_controllerAI : public ScriptedAI
    {
        npc_warchief_cometh_controllerAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING && !_cutsceneInProgress)
            {
                _cutsceneInProgress = true;
                me->CastWithDelay(1000, me, 47533); // camera shake
                _events.ScheduleEvent(EVENT_WARCHIEF_COMETH_SUMMONS, 3000);

                if (auto sylv = me->SummonCreature(NPC_LADY_SYLVANAS, 1381.69f, 1041.260f, 54.317f, 4.028f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                    if (auto agatha = me->SummonCreature(NPC_VALKYR_AGATHA, 1364.02f, 1028.54f, 55.991f, 0.85521f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                        if (auto daschla = me->SummonCreature(NPC_VALKYR_DASCHLA, 1365.73f, 1024.98f, 57.024f, 1.13446f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            if (auto arthura = me->SummonCreature(NPC_VALKYR_ARTHURA, 1360.81f, 1030.57f, 56.874f, 0.64577f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                if (auto fallen1 = me->SummonCreature(NPC_FALLEN_HUMAN, 1372.98f, 1041.06f, 51.381f, 0.226f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    if (auto fallen2 = me->SummonCreature(NPC_FALLEN_HUMAN, 1378.16f, 1031.71f, 51.371f, 0.767f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                        if (auto fallen3 = me->SummonCreature(NPC_FALLEN_HUMAN, 1372.58f, 1038.07f, 50.966f, 0.890f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                            if (auto fallen4 = me->SummonCreature(NPC_FALLEN_HUMAN, 1375.42f, 1033.81f, 50.652f, 0.890f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                if (auto fallen5 = me->SummonCreature(NPC_FALLEN_HUMAN, 1369.65f, 1033.67f, 50.966f, 0.698f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                    if (auto fallen6 = me->SummonCreature(NPC_FALLEN_HUMAN, 1367.33f, 1035.38f, 51.304f, 0.890f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                        if (auto fallen7 = me->SummonCreature(NPC_FALLEN_HUMAN, 1370.66f, 1030.82f, 51.134f, 0.890f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                            if (auto fallen8 = me->SummonCreature(NPC_FALLEN_HUMAN, 1374.14f, 1036.35f, 50.921f, 5.532f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                if (auto fallen9 = me->SummonCreature(NPC_FALLEN_HUMAN, 1369.57f, 1036.90f, 50.998f, 0.209f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                    if (auto fallen10 = me->SummonCreature(NPC_FALLEN_HUMAN, 1377.30f, 1035.93f, 51.096f, 3.141f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                        if (auto fallen11 = me->SummonCreature(NPC_FALLEN_HUMAN, 1373.35f, 1031.58f, 50.618f, 0.959f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                            if (auto fallen12 = me->SummonCreature(NPC_FALLEN_HUMAN, 1376.45f, 1029.61f, 50.937f, 0.890f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                if (auto fallen13 = me->SummonCreature(NPC_FALLEN_HUMAN, 1369.71f, 1040.19f, 51.021f, 5.585f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                    if (auto fallen14 = me->SummonCreature(NPC_FALLEN_HUMAN, 1375.22f, 1038.58f, 51.245f, 4.555f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                    {
                                                                                        _agathaGUID = agatha->GetGUID();
                                                                                        summons.Summon(sylv);
                                                                                        summons.Summon(daschla);
                                                                                        summons.Summon(agatha);
                                                                                        summons.Summon(arthura);
                                                                                        summons.Summon(fallen1);
                                                                                        summons.Summon(fallen2);
                                                                                        summons.Summon(fallen3);
                                                                                        summons.Summon(fallen4);
                                                                                        summons.Summon(fallen5);
                                                                                        summons.Summon(fallen6);
                                                                                        summons.Summon(fallen7);
                                                                                        summons.Summon(fallen8);
                                                                                        summons.Summon(fallen9);
                                                                                        summons.Summon(fallen10);
                                                                                        summons.Summon(fallen11);
                                                                                        summons.Summon(fallen12);
                                                                                        summons.Summon(fallen13);
                                                                                        summons.Summon(fallen14);
                                                                                    }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2 && _cutsceneInProgress)
            {
                if (auto agatha = Unit::GetCreature(*me, _agathaGUID))
                {
                    agatha->AI()->SetData(1, 1);
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_3 && _cutsceneInProgress)
            {
                summons.DespawnAll();
                _cutsceneInProgress = false;

                std::list<Player*> player = me->GetPlayersInRange(100, true);
                for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                    if ((*it)->GetQuestStatus(26965) == QUEST_STATUS_INCOMPLETE)
                        (*it)->KilledMonsterCredit(44629); // credit
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WARCHIEF_COMETH_SUMMONS:
                    if (auto portal1 = me->SummonCreature(NPC_PORTAL_FROM_ORGRIMMAR, 1358.62f, 1054.72f, 53.12f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                        if (auto portal2 = me->SummonCreature(NPC_PORTAL_FROM_ORGRIMMAR, 1393.27f, 1021.20f, 53.222f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            if (auto portal3 = me->SummonCreature(NPC_PORTAL_FROM_ORGRIMMAR, 1407.22f, 1065.911f, 60.478f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            {
                                summons.Summon(portal1);
                                summons.Summon(portal2);
                                summons.Summon(portal3);
                            }
                    _events.ScheduleEvent(EVENT_WARCHIEF_COMETH_SUMMONS_2, 3000);
                    break;
                case EVENT_WARCHIEF_COMETH_SUMMONS_2:
                    if (auto hellscream1 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1389.79f, 1024.51f, 53.208f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                        if (auto hellscream2 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1388.05f, 1026.91f, 53.208f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            if (auto hellscream3 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1388.16f, 1020.88f, 53.255f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                if (auto hellscream4 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1385.79f, 1025.99f, 53.225f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    if (auto hellscream5 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1386.69f, 1023.26f, 53.243f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                        if (auto hellscream6 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1384.33f, 1022.04f, 53.281f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                            if (auto hellscream7 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1391.10f, 1027.73f, 53.204f, 2.8274f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                if (auto hellscream8 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1359.10f, 1046.55f, 52.970f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                    if (auto hellscream9 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1360.89f, 1051.81f, 53.197f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                        if (auto hellscream10 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1360.75f, 1048.84f, 53.128f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                            if (auto hellscream11 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1364.43f, 1053.62f, 53.293f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                if (auto hellscream12 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1363.08f, 1048.15f, 53.222f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                    if (auto hellscream13 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1364.08f, 1050.84f, 53.291f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                        if (auto hellscream14 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1366.69f, 1050.31f, 53.342f, 5.2534f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                            if (auto hellscream15 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1357.85f, 1050.12f, 52.9982f, 5.25344f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                if (auto hellscream16 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1366.69f, 1050.31f, 53.342f, 5.25344f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                    if (auto hellscream17 = me->SummonCreature(NPC_HELLSCREAMS_ELITE, 1357.85f, 1050.12f, 52.998f, 5.25344f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                        if (auto garrosh = me->SummonCreature(NPC_GARROSH_HELLSCREAM, 1403.192f, 1065.561f, 60.478f, 3.93f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                            if (auto cromush = me->SummonCreature(NPC_HIGH_WARLORD_CROMUSH, 1405.383f, 1062.260f, 60.478f, 3.93f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                                                                            {
                                                                                                summons.Summon(hellscream1);
                                                                                                summons.Summon(hellscream2);
                                                                                                summons.Summon(hellscream3);
                                                                                                summons.Summon(hellscream4);
                                                                                                summons.Summon(hellscream5);
                                                                                                summons.Summon(hellscream6);
                                                                                                summons.Summon(hellscream7);
                                                                                                summons.Summon(hellscream8);
                                                                                                summons.Summon(hellscream9);
                                                                                                summons.Summon(hellscream10);
                                                                                                summons.Summon(hellscream11);
                                                                                                summons.Summon(hellscream12);
                                                                                                summons.Summon(hellscream13);
                                                                                                summons.Summon(hellscream14);
                                                                                                summons.Summon(hellscream15);
                                                                                                summons.Summon(hellscream16);
                                                                                                summons.Summon(hellscream17);
                                                                                                summons.Summon(garrosh);
                                                                                                summons.Summon(cromush);
                                                                                            }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _cutsceneInProgress = false;

        uint64 _agathaGUID = 0;
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warchief_cometh_controllerAI(creature);
    }
};

class npc_fallen_human_silverpine : public CreatureScript
{
public:
    npc_fallen_human_silverpine() : CreatureScript("npc_fallen_human_silverpine") { }

    struct npc_fallen_human_silverpineAI : public ScriptedAI
    {
        npc_fallen_human_silverpineAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_RISE_FORSAKEN)
            {
                me->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5);
                _events.ScheduleEvent(EVENT_WARCHIEF_COMETH_4, 8500);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WARCHIEF_COMETH_4:
                    me->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 5, 10, 10, 0);
                    _events.ScheduleEvent(EVENT_WARCHIEF_COMETH_5, 1000);
                    break;
                case EVENT_WARCHIEF_COMETH_5:
                    me->CastSpell(me, SPELL_FORSAKEN_TRANSFORM);
                    me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                    me->SetFacingTo(0.7224f);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fallen_human_silverpineAI(creature);
    }
};

class spell_forsaken_tropper_rand_transform : public SpellScriptLoader
{
public:
    spell_forsaken_tropper_rand_transform() : SpellScriptLoader("spell_forsaken_tropper_rand_transform") { }

    class spell_forsaken_tropper_rand_transform_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_forsaken_tropper_rand_transform_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto caster = GetCaster())
            {
                uint32 spellId = RAND(83149, 83150, 83152, 83164, 83163, 83168, 83165, 83166, 83167); // Forsaken Troppers Tranform Spells
                caster->CastSpell(caster, spellId, true);
            }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_forsaken_tropper_rand_transform_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_forsaken_tropper_rand_transform_SpellScript();
    }
};

uint32 const ForsakenBatPathSize = 27;
const G3D::Vector3 ForsakenBatPath[ForsakenBatPathSize] =
{
    { 1415.709f, 1017.567f, 57.959f, },  // 0
    { 1343.650f, 1011.999f, 62.792f, },  // 1
    { 1264.807f, 1018.849f, 66.311f, },  // 2
    { 1139.529f, 1002.471f, 43.449f, },  // 3
    { 1095.189f, 922.1546f, 36.783f, },  // 4
    { 1126.296f, 796.8005f, 36.647f, },  // 5
    { 1195.720f, 592.9724f, 41.743f, },  // 6
    { 1212.670f, 463.0607f, 50.866f, },  // 7
    { 1226.052f, 396.07f, 51.259f, },    // 8 
    { 1218.899f, 296.40f, 55.241f, },    // 9 
    { 1119.533f, 233.61f, 56.163f, },    // 10
    { 997.1007f, 147.13f, 53.898f, },    // 11
    { 876.4819f, 57.137f, 52.808f, },    // 12
    { 746.6122f, 92.112f, 50.176f, },    // 13
    { 780.2442f, 272.93f, 57.545f, },    // 14
    { 957.4793f, 210.61f, 52.258f, },    // 15
    { 1143.990f, 226.60f, 51.364f, },    // 16
    { 1175.251f, 352.84f, 57.577f, },    // 17
    { 877.6257f, 240.52f, 56.462f, },    // 18
    { 742.6202f, 356.63f, 52.246f, },    // 19
    { 813.1855f, 567.96f, 77.066f, },    // 20
    { 1026.914f, 751.77f, 112.57f, },    // 21
    { 1201.804f, 754.66f, 63.428f, },    // 22
    { 1367.399f, 844.52f, 70.966f, },    // 23
    { 1361.137f, 969.05f, 82.911f, },    // 24
    { 1386.145f, 1004.7f,  67.94f, },    // 25
    { 1416.055f, 1016.8f,  53.45f, },    // 26
};

uint32 const ForsakenBatPathSize1 = 6;
const G3D::Vector3 ForsakenBatPath1[ForsakenBatPathSize1] =
{
    { 922.430f, 457.074f, 155.994f, },  // 0
    { 970.914f, 662.173f, 138.150f, },  // 1
    { 1084.49f, 913.504f, 103.583f, },  // 2
    { 1251.46f, 1019.04f,  89.421f, },  // 3
    { 1347.76f, 1005.42f,  91.766f, },  // 4
    { 1419.73f, 1017.69f,  53.383f, },  // 5
};

enum ForsakenBatVsMurlocs
{
    SPELL_GO_HOME = 83594,
};

class npc_forsaken_bat_vs_murlocs_vehicle : public CreatureScript
{
public:
    npc_forsaken_bat_vs_murlocs_vehicle() : CreatureScript("npc_forsaken_bat_vs_murlocs_vehicle") { }

    struct npc_forsaken_bat_vs_murlocs_vehicleAI : public VehicleAI
    {
        npc_forsaken_bat_vs_murlocs_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetCanFly(true);
            me->SetDisableGravity(true);
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void PassengerBoarded(Unit* passenger, int8 /* */seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_FORSAKEN_BAT_1, 1000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_GO_HOME && !_goHome && who->ToPlayer()->GetQuestStatus(26998) == QUEST_STATUS_COMPLETE)
            {
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovementExpired(true);
                _events.ScheduleEvent(EVENT_FORSAKEN_BAT_2, 1000);
                me->AI()->Talk(1, _playerGUID);
                _goHome = true;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (!_goHome)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetQuestStatus(26998) == QUEST_STATUS_COMPLETE)
                        {
                            player->CastSpell(player, SPELL_GO_HOME);
                        }
                    }
                }
            }
            _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FORSAKEN_BAT_1:
                    me->GetMotionMaster()->MoveSmoothPath(ForsakenBatPath, ForsakenBatPathSize);
                    break;
                case EVENT_FORSAKEN_BAT_2:
                    me->SetSpeed(MOVE_FLIGHT, 4.0f);
                    me->SetSpeed(MOVE_RUN, 4.0f);
                    me->GetMotionMaster()->MoveSmoothPath(ForsakenBatPath1, ForsakenBatPathSize1);
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 7 && !_goHome)
            {
                me->AI()->Talk(0, _playerGUID);
            }

            if (point == 26 && !_goHome)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon();
            }

            if (point == 5 && _goHome)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon();
            }
        }
    private:
        bool _goHome = false;
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_bat_vs_murlocs_vehicleAI(creature);
    }
};

enum QuestWaitingtoExsanguinate
{
    SPELL_SUMMON_YORICK_AURA = 83751,
    SPELL_SUMMON_CROWLEY_MASTER = 83762,

    SPELL_POINT_TO_CROWLEY = 83768,
    SPELL_POINT_TO_BLOODFANG = 83769,

    NPC_YORICK = 44882,
    NPC_CROWLEY_1 = 44883,
    NPC_BLOODFANG_1 = 44884,
    NPC_HIDDING_SPOT = 44893,
};

class npc_armoire_vehicle : public CreatureScript
{
public:
    npc_armoire_vehicle() : CreatureScript("npc_armoire_vehicle") { }

    struct npc_armoire_vehicleAI : public VehicleAI
    {
        npc_armoire_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetPhaseMask(8, true);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }
        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->CastSpell(me, SPELL_SUMMON_CROWLEY_MASTER);
                me->CastWithDelay(1000, me, SPELL_POINT_TO_CROWLEY);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                passenger->RemoveAura(83751);
                passenger->RemoveAura(83752);
                passenger->RemoveAura(83753);
                me->DespawnOrUnsummon();

                if (auto yorick = me->FindNearestCreature(NPC_YORICK, 30.0f))
                    if (auto crowley = me->FindNearestCreature(NPC_CROWLEY_1, 30.0f))
                        if (auto bloodfang = me->FindNearestCreature(NPC_BLOODFANG_1, 30.0f))
                        {
                            yorick->DespawnOrUnsummon();
                            crowley->DespawnOrUnsummon();
                            bloodfang->DespawnOrUnsummon();
                        }
            }
        }
        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING) // facing bloodfang
            {
                me->RemoveAura(83763);
                me->CastWithDelay(200, me, SPELL_POINT_TO_BLOODFANG);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2) // facing crowley
            {
                me->RemoveAura(83764);
                me->CastWithDelay(200, me, SPELL_POINT_TO_CROWLEY);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_3) // end
            {
                {
                    std::list<Player*> player = me->GetPlayersInRange(80, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(27045) == QUEST_STATUS_INCOMPLETE)
                        {
                            (*it)->KilledMonsterCredit(44882); // credit
                            (*it)->RemoveAura(83751);
                            (*it)->RemoveAura(83752);
                            (*it)->RemoveAura(83753);
                        }
                }

                me->RemoveAura(83764);
                me->RemoveAura(83763);

                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon(1000);

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->GetMotionMaster()->MoveJump(1314.033f, 1209.762f, 58.51f, 12.0f, 15.0f);
                }
            }
        }
    private:
        EventMap _events;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_armoire_vehicleAI(creature);
    }
};

class npc_waiting_to_exsanguinate_controller : public CreatureScript
{
public:
    npc_waiting_to_exsanguinate_controller() : CreatureScript("npc_waiting_to_exsanguinate_controller") { }

    struct npc_waiting_to_exsanguinate_controllerAI : public ScriptedAI
    {
        npc_waiting_to_exsanguinate_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (!me->FindNearestCreature(NPC_YORICK, 70.0f) && !me->FindNearestCreature(NPC_HIDDING_SPOT, 70.0f))
                    who->CastSpell(who, SPELL_SUMMON_YORICK_AURA);
                else
                    me->AI()->Talk(0);
            }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_waiting_to_exsanguinate_controllerAI(creature);
    }
};

enum QuestSteelThunder
{
    QUEST_STEEL_THUNDER = 27069,

    SPELL_KILL_CREDIT_STEEL_THUNDER = 83843,
    SPELL_SUMMON_ORC_AURA = 83839,

    NPC_ORC_CRATE = 44915,
    NPC_ORC_SEA_PUP = 44914,
};

class npc_orc_sea_pup : public CreatureScript
{
public:
    npc_orc_sea_pup() : CreatureScript("npc_orc_sea_pup") { }

    struct npc_orc_sea_pupAI : public ScriptedAI
    {
        npc_orc_sea_pupAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->SetReactState(REACT_PASSIVE);
                me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                me->GetMotionMaster()->MoveFollow(player, 1.2f, 0.7f * M_PI);
            }
            else
                return;
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                ++_crateCounter;

                switch (_crateCounter)
                {
                case 1:
                    me->CastSpell(me, SPELL_KILL_CREDIT_STEEL_THUNDER);
                    me->AI()->TalkWithDelay(1000, 1, _playerGUID);

                    if (auto crate1 = me->SummonCreature(NPC_ORC_CRATE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _crate1GUID = crate1->GetGUID();
                        crate1->EnterVehicle(me, 0);
                        summons.Summon(crate1);
                    }
                    break;
                case 2:
                    me->CastSpell(me, SPELL_KILL_CREDIT_STEEL_THUNDER);
                    me->AI()->TalkWithDelay(1000, 2, _playerGUID);

                    if (auto crate2 = me->SummonCreature(NPC_ORC_CRATE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _crate2GUID = crate2->GetGUID();
                        crate2->EnterVehicle(me, 1);
                        summons.Summon(crate2);
                    }
                    break;
                case 3:
                    me->CastSpell(me, SPELL_KILL_CREDIT_STEEL_THUNDER);
                    me->AI()->TalkWithDelay(1000, 3, _playerGUID);

                    if (auto crate3 = me->SummonCreature(NPC_ORC_CRATE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _crate3GUID = crate3->GetGUID();
                        crate3->EnterVehicle(me, 2);
                        summons.Summon(crate3);
                    }
                    break;
                case 4:
                    me->CastSpell(me, SPELL_KILL_CREDIT_STEEL_THUNDER);
                    me->AI()->TalkWithDelay(1000, 4, _playerGUID);

                    if (auto crate4 = me->SummonCreature(NPC_ORC_CRATE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _crate4GUID = crate4->GetGUID();
                        crate4->EnterVehicle(me, 3);
                        summons.Summon(crate4);
                    }
                    break;
                case 5:
                    me->CastSpell(me, SPELL_KILL_CREDIT_STEEL_THUNDER);
                    me->AI()->TalkWithDelay(1000, 5, _playerGUID);

                    if (auto crate5 = me->SummonCreature(NPC_ORC_CRATE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 600000))
                    {
                        _crate5GUID = crate5->GetGUID();
                        crate5->EnterVehicle(me, 4);
                        summons.Summon(crate5);
                    }
                    break;
                default:
                    break;
                }
            }
            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                me->AI()->TalkWithDelay(2000, 7);
                me->DespawnOrUnsummon(6000);

                if (auto crate1 = Unit::GetCreature(*me, _crate1GUID))
                {
                    crate1->GetMotionMaster()->MoveJump(me->GetPositionX() + 3, me->GetPositionY(), me->GetPositionZ(), 10, 12);
                    crate1->DespawnOrUnsummon(6000);
                }
                if (auto crate2 = Unit::GetCreature(*me, _crate2GUID))
                {
                    crate2->GetMotionMaster()->MoveJump(me->GetPositionX() - 3, me->GetPositionY(), me->GetPositionZ(), 10, 12);
                    crate2->DespawnOrUnsummon(6000);
                }
                if (auto crate3 = Unit::GetCreature(*me, _crate3GUID))
                {
                    crate3->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY() + 3, me->GetPositionZ(), 10, 12);
                    crate3->DespawnOrUnsummon(6000);
                }
                if (auto crate4 = Unit::GetCreature(*me, _crate4GUID))
                {
                    crate4->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY() - 3, me->GetPositionZ(), 10, 12);
                    crate4->DespawnOrUnsummon(6000);
                }
                if (auto crate5 = Unit::GetCreature(*me, _crate5GUID))
                {
                    crate5->GetMotionMaster()->MoveJump(me->GetPositionX() + 2, me->GetPositionY() - 1, me->GetPositionZ(), 10, 12);
                    crate5->DespawnOrUnsummon(6000);
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->RemoveAura(SPELL_SUMMON_ORC_AURA);
            }
        }
        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                    {
                        player->RemoveAura(SPELL_SUMMON_ORC_AURA);
                        me->DespawnOrUnsummon();
                    }
                    else if (player->GetQuestStatus(QUEST_STEEL_THUNDER) == QUEST_STATUS_NONE)
                    {
                        player->RemoveAura(SPELL_SUMMON_ORC_AURA);
                        me->DespawnOrUnsummon();
                        summons.DespawnAll();
                    }
                    else if (player->GetQuestStatus(QUEST_STEEL_THUNDER) == QUEST_STATUS_REWARDED && !_endEvent)
                    {
                        me->CastSpell(me, 42716);
                        me->AI()->Talk(6);
                        me->CastWithDelay(2000, me, SPELL_DUMMY_PING_2);
                        _endEvent = true;
                    }
                    else
                        return;
                }
            }
            else _checkTimer -= diff;
        }
    private:
        bool _endEvent = false;
        SummonList summons;
        uint8 _crateCounter = 0;
        uint64 _playerGUID = 0;

        uint64 _crate1GUID = 0;
        uint64 _crate2GUID = 0;
        uint64 _crate3GUID = 0;
        uint64 _crate4GUID = 0;
        uint64 _crate5GUID = 0;

        uint16 _checkTimer = 1000;
        uint16 _checkSpeedTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orc_sea_pupAI(creature);
    }
};

class spell_pick_up_orc_crate : public SpellScriptLoader
{
public:
    spell_pick_up_orc_crate() : SpellScriptLoader("spell_pick_up_orc_crate") { }

    class spell_pick_up_orc_crate_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pick_up_orc_crate_SpellScript);

        void HandleOnHit()
        {
            if (auto target = GetHitCreature())
                if (auto caster = GetCaster())
                    caster->CastSpell(target, SPELL_DUMMY_PING);
                else
                    return;
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pick_up_orc_crate_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pick_up_orc_crate_SpellScript();
    }
};

enum QuestLostintheDarkness
{
    SPELL_SUMMON_ORC_SEA_DOG_RESCUED = 83927,
    QUEST_LOST_IN_THE_DARKNESS = 27093,
};

class npc_webbed_victim_silverpine : public CreatureScript
{
public:
    npc_webbed_victim_silverpine() : CreatureScript("npc_webbed_victim_silverpine") { }

    struct npc_webbed_victim_silverpineAI : public ScriptedAI
    {
        npc_webbed_victim_silverpineAI(Creature* creature) : ScriptedAI(creature) {}

        void JustDied(Unit* victim)
        {
            if (victim->GetTypeId() != TYPEID_PLAYER)
                return;

            if (auto player = victim->ToPlayer())
            {
                if (player->GetQuestStatus(QUEST_LOST_IN_THE_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                {
                    _random = urand(0, 4);

                    switch (_random)
                    {
                    case 0:
                    case 1:
                    case 2:
                        player->CastSpell(player, SPELL_SUMMON_ORC_SEA_DOG_RESCUED);
                        break;
                    case 3:
                    case 4:
                        me->SummonCreature(44547, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000);
                    default:
                        break;
                    }
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();
        }
    private:
        uint8 _random = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_webbed_victim_silverpineAI(creature);
    }
};

class npc_orc_sea_dog_rescued : public CreatureScript
{
public:
    npc_orc_sea_dog_rescued() : CreatureScript("npc_orc_sea_dog_rescued") { }

    struct npc_orc_sea_dog_rescuedAI : public ScriptedAI
    {
        npc_orc_sea_dog_rescuedAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->SetReactState(REACT_PASSIVE);
                me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                me->GetMotionMaster()->MoveFollow(player, frand(0.9f, 2.0f), frand(0.0f * M_PI, 2.0f * M_PI));
                player->KilledMonsterCredit(44942);
            }
            else
                me->DespawnOrUnsummon(1000);
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                        me->DespawnOrUnsummon();

                    if (player->GetQuestStatus(QUEST_LOST_IN_THE_DARKNESS) == QUEST_STATUS_NONE)
                        me->DespawnOrUnsummon();

                    if (player->GetQuestStatus(QUEST_LOST_IN_THE_DARKNESS) == QUEST_STATUS_REWARDED)
                    {
                        me->CastSpell(me, SPELL_ROOT);
                        me->SetFacingToObject(player);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                        me->DespawnOrUnsummon(1500);
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->DeleteThreatList();
                        me->GetMotionMaster()->MoveFollow(player, frand(0.9f, 2.0f), frand(0.0f * M_PI, 2.0f * M_PI));
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        uint64 _playerGUID = 0;

        uint16 _checkTimer = 1000;
        uint16 _checkSpeedTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orc_sea_dog_rescuedAI(creature);
    }
};

uint32 const AgathaNoEscapePathSize = 24;
const G3D::Vector3 AgathaNoEscapePath[AgathaNoEscapePathSize] =
{
    { 980.142f, 686.44f, 77.026f + 2.6f,}, // 0 
    { 978.839f, 688.57f, 74.899f + 2.6f,}, // 1 
    { 979.800f, 671.40f, 74.899f + 2.6f,}, // 2 
    { 997.688f, 674.17f, 69.796f + 2.6f,}, // 3 
    { 998.099f, 683.46f, 69.796f + 2.6f,}, // 4 
    { 1002.66f, 683.53f, 69.543f + 2.6f,}, // 5 
    { 1012.79f, 684.41f, 64.903f + 2.6f,}, // 6 
    { 1012.05f, 694.98f, 60.975f + 2.6f,}, // 7 
    { 998.619f, 694.52f, 60.975f + 2.6f,}, // 8 
    { 995.953f, 699.57f, 60.975f + 2.6f,}, // 9 
    { 994.989f, 709.86f, 60.975f + 2.6f,}, // 10
    { 987.366f, 712.31f, 60.975f + 2.6f,}, // 11
    { 982.776f, 709.00f, 60.975f + 2.6f,}, // 12
    { 983.372f, 691.54f, 59.476f + 2.6f,}, // 13
    { 980.052f, 688.45f, 59.476f + 2.6f,}, // 14
    { 963.991f, 688.75f, 59.736f + 2.6f,}, // 15
    { 934.429f, 688.41f, 71.090f,  }, // 16
    { 883.916f, 705.75f, 91.623f,  }, // 17
    { 860.442f, 779.96f, 85.433f,  }, // 18
    { 958.342f, 909.26f, 73.901f,  }, // 19
    { 1056.32f, 985.70f, 72.973f,  }, // 20
    { 1227.49f, 1008.6f, 76.016f,  }, // 21
    { 1334.76f, 1026.9f, 65.716f,  }, // 22
    { 1373.21f, 1039.0f, 54.685f,  }, // 23
};

enum AgathaDatas
{
    QUEST_ORC_ARE_IN_ORDER = 27096,
    QUEST_RISE_FORSAKEN = 27097,
    QUEST_NO_ESCAPE = 27099,

    SPELL_SUMMON_AGATHA1_AURA = 83982,
    SPELL_DEATH_WALK = 85451,
    SPELL_RISE_FORSAKEN_2 = 83993,
    SPELL_OWNER_PING_AGATHA = 151316,

    SPELL_DOOMHOWL = 84012,
    SPELL_UNHOLY_SMITE = 84014,
    SPELL_UNHOLY_DARKNESS = 84013,

    NPC_HILLSBRAND_REFUGEE = 44966,
    NPC_HILLSBRAND_REFUGEE_2 = 44954,
};

class npc_agatha1_silverpine : public CreatureScript
{
public:
    npc_agatha1_silverpine() : CreatureScript("npc_agatha1_silverpine") { }

    struct npc_agatha1_silverpineAI : public ScriptedAI
    {
        npc_agatha1_silverpineAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->GetMotionMaster()->MoveFollow(summoner, 2.0f, 0.7f * M_PI);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (who->GetGUID() == me->GetOwnerGUID())
                if (spellInfo->Id == SPELL_OWNER_PING_AGATHA)
                {
                    who->ExitVehicle();
                    me->GetMotionMaster()->Clear();
                    who->CastWithDelay(1000, me, SPELL_RIDE_VEHICLE);
                }

            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->AI()->Talk(1, _playerGUID);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId*/, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->SetSpeed(MOVE_FLIGHT, 3.5f);
                me->SetSpeed(MOVE_RUN, 3.5f);
                me->AI()->TalkWithDelay(1000, 3, _playerGUID);
                me->GetMotionMaster()->MoveSmoothPath(AgathaNoEscapePath, AgathaNoEscapePathSize);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    me->GetMotionMaster()->MoveFollow(player, 2.0f, 0.7f * M_PI);
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 23)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_DOOMHOWL, urand(4000, 8000));
                _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(1000, 4000));
            }
            else
                _events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                        me->DespawnOrUnsummon();

                    if (me->GetZoneId() != 130)
                        me->DespawnOrUnsummon();

                    if (player->GetQuestStatus(QUEST_RISE_FORSAKEN) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_NO_ESCAPE) == QUEST_STATUS_REWARDED)
                        me->DespawnOrUnsummon();

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 2.0f, 0.7f * M_PI);
                    }
                }
            }
            else _checkTimer -= diff;

            if (_idleTalkTimer <= diff)
            {
                _idleTalkTimer = 95000;
                if (me->GetAreaId() != 235)
                    me->AI()->Talk(0, _playerGUID);
            }
            else _idleTalkTimer -= diff;

            if (!UpdateVictim())
                return;

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (player->GetHealthPct() <= 35 && !_lowHP)
                {
                    _events.Reset();
                    me->CastSpell(player, SPELL_UNHOLY_DARKNESS);
                    _lowHP = true;
                }

                if (player->GetHealthPct() >= 36 && _lowHP)
                    _lowHP = false;
            }
            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DOOMHOWL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_DOOMHOWL);
                    _events.ScheduleEvent(EVENT_DOOMHOWL, urand(22000, 35000));
                    break;
                case EVENT_UNHOLY_SMITE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_UNHOLY_SMITE);
                    _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(3000, 7000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _lowHP = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        uint16 _checkSpeedTimer = 2000;
        uint32 _idleTalkTimer = 28000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agatha1_silverpineAI(creature);
    }
};

class spell_notify_agatha_isle : public SpellScriptLoader
{
public:
    spell_notify_agatha_isle() : SpellScriptLoader("spell_notify_agatha_isle") { }

    class spell_notify_agatha_isle_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_notify_agatha_isle_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto refugee = GetCaster())
                if (auto agatha = GetHitCreature())
                {
                    agatha->CastSpell(refugee, SPELL_RISE_FORSAKEN_2);

                    uint32 spellId = RAND(83998, 83999, 84000, 84001, 84002, 84003, 84004, 84005);
                    refugee->CastWithDelay(2000, refugee, spellId);
                }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_notify_agatha_isle_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_notify_agatha_isle_SpellScript();
    }
};

enum NoEscapeScene
{
    SPELL_SUMMON_FERNIS_ACTORS = 84053,
    SPELL_ALPHA_0 = 80808,

    NPC_AGATHA_GUARDIAN = 44951,
    NPC_NO_ESCAPE_CAMERA = 45003,
    NPC_NO_ESCAPE_CROWLEY = 44989,
};

class npc_no_escape_controller : public CreatureScript
{
public:
    npc_no_escape_controller() : CreatureScript("npc_no_escape_controller") { }

    struct npc_no_escape_controllerAI : public ScriptedAI
    {
        npc_no_escape_controllerAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (auto player = who->ToPlayer())
            {
                if (!_cutsceneInProgress && !_active && me->IsWithinDistInMap(who, 12.0f) && !who->isInCombat())
                {
                    if (auto agatha = me->FindNearestCreature(NPC_AGATHA_GUARDIAN, 25.f))
                    {
                        if (player->GetQuestStatus(QUEST_NO_ESCAPE) == QUEST_STATUS_INCOMPLETE && player->GetAura(SPELL_SUMMON_AGATHA1_AURA))
                        {
                            _playerGUID = player->GetGUID();
                            _agathaGUID = agatha->GetGUID();
                            _active = true;
                            _cutsceneInProgress = true;
                            _events.ScheduleEvent(EVENT_NO_ESCAPE_1, 2000);
                        }
                    }
                }
            }
        }

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto camera = me->FindNearestCreature(NPC_NO_ESCAPE_CAMERA, 40.0f))
                    if (auto agatha = Unit::GetCreature(*me, _agathaGUID))
                    {
                        camera->SetFacingToObject(me);
                        agatha->SetFacingTo(0.046f);
                        agatha->AI()->TalkWithDelay(1000, 2);
                        _events.ScheduleEvent(EVENT_NO_ESCAPE_3, 3000);
                    }
            }

            // leave cinematic
            if (spellInfo->Id == SPELL_DUMMY_PING_2 && _cutsceneInProgress)
            {
                summons.DespawnAll();
                _cutsceneInProgress = false;
            }
        }

        void NoEscapeCompletion()
        {
            std::list<Player*> player = me->GetPlayersInRange(40, true);
            for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_NO_ESCAPE) == QUEST_STATUS_INCOMPLETE && (*it)->GetAura(SPELL_SUMMON_AGATHA1_AURA))
                {
                    (*it)->KilledMonsterCredit(44951);
                    (*it)->CastSpell((*it), SPELL_OWNER_PING_AGATHA);
                }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_NO_ESCAPE_1:
                    me->CastSpell(me, SPELL_SUMMON_FERNIS_ACTORS);
                    _events.ScheduleEvent(EVENT_NO_ESCAPE_2, 2000);

                    if (auto actor1 = me->FindNearestCreature(44990, 30.0f))
                        if (auto actor2 = me->FindNearestCreature(44989, 30.0f))
                            if (auto actor3 = me->FindNearestCreature(44993, 30.0f))
                                if (auto actor4 = me->FindNearestCreature(44994, 30.0f))
                                    if (auto actor5 = me->FindNearestCreature(44995, 30.0f))
                                        if (auto actor6 = me->FindNearestCreature(44996, 30.0f))
                                            if (auto actor7 = me->FindNearestCreature(44997, 30.0f))
                                                if (auto actor8 = me->FindNearestCreature(45002, 30.0f))
                                                {
                                                    summons.Summon(actor1);
                                                    summons.Summon(actor2);
                                                    summons.Summon(actor3);
                                                    summons.Summon(actor4);
                                                    summons.Summon(actor5);
                                                    summons.Summon(actor6);
                                                    summons.Summon(actor7);
                                                    summons.Summon(actor8);
                                                }
                    break;
                case EVENT_NO_ESCAPE_2:
                    if (auto agatha = Unit::GetCreature(*me, _agathaGUID))
                    {
                        me->CastSpell(agatha, SPELL_DUMMY_PING);
                        agatha->GetMotionMaster()->MovePoint(1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            if (auto camera = player->SummonCreature(NPC_NO_ESCAPE_CAMERA, 992.546f, 691.757f, 74.889f, 0.039f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            {
                                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                                player->CastWithDelay(100, camera, SPELL_RIDE_VEHICLE_SEAT_02);
                            }
                        }
                        break;
                    }
                    
                case EVENT_NO_ESCAPE_3:
                    _cutsceneInProgress = false;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto camera = me->FindNearestCreature(NPC_NO_ESCAPE_CAMERA, 40.0f))
                        {                           
                            NoEscapeCompletion();
                            camera->CastSpell(camera, SPELL_EJECT_ALL_PASSENGERS);
                            camera->DespawnOrUnsummon();
                            _events.ScheduleEvent(EVENT_NO_ESCAPE_4, 3000);
                        }
                    break;
                case EVENT_NO_ESCAPE_4:
                    summons.DespawnAll();
                    _active = false;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _active = false;
        bool _cutsceneInProgress = false;
        uint64 _playerGUID = 0;
        uint64 _agathaGUID = 0;
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_no_escape_controllerAI(creature);
    }
};

class npc_fenris_no_escape_camera : public CreatureScript
{
public:
    npc_fenris_no_escape_camera() : CreatureScript("npc_fenris_no_escape_camera") { }

    struct npc_fenris_no_escape_cameraAI : public VehicleAI
    {
        npc_fenris_no_escape_cameraAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(player, SPELL_ALPHA_0);
            }
            else
                return;
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId*/, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _rpInProgress = true;

                if (auto crowley = me->FindNearestCreature(NPC_NO_ESCAPE_CROWLEY, 25))
                    me->CastSpell(crowley, SPELL_DUMMY_PING);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER && _rpInProgress)
            {
                me->DespawnOrUnsummon(100);
                passenger->RemoveAura(SPELL_ALPHA_0);

                if (auto controller = me->FindNearestCreature(74952, 20))
                    controller->CastSpell(controller, SPELL_DUMMY_PING_2);
            }
        }

        void SpellHit(Unit* /* who */, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    _rpInProgress = false;
                    player->CastSpell(me, 82557);
                    me->CastWithDelay(3000, me, SPELL_EJECT_ALL_PASSENGERS);
                }
            }
        }
    private:
        bool _rpInProgress = false;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fenris_no_escape_cameraAI(creature);
    }
};

class npc_agatha_resummon_controller : public CreatureScript
{
public:
    npc_agatha_resummon_controller() : CreatureScript("npc_agatha_resummon_controller") { }

    struct npc_agatha_resummon_controllerAI : public ScriptedAI
    {
        npc_agatha_resummon_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 35.0f))
                if (auto player = who->ToPlayer())
                    if (!player->GetAura(SPELL_SUMMON_AGATHA1_AURA) && !player->isDead())
                    {
                        if (player->GetQuestStatus(QUEST_RISE_FORSAKEN) == QUEST_STATUS_INCOMPLETE)
                            player->CastWithDelay(100, player, SPELL_SUMMON_AGATHA1_AURA);
                        else if (player->GetQuestStatus(QUEST_RISE_FORSAKEN) == QUEST_STATUS_COMPLETE)
                            player->CastWithDelay(100, player, SPELL_SUMMON_AGATHA1_AURA);
                        else if (player->GetQuestStatus(QUEST_RISE_FORSAKEN) == QUEST_STATUS_REWARDED &&
                            player->GetQuestStatus(QUEST_NO_ESCAPE) == QUEST_STATUS_NONE)
                            player->CastWithDelay(100, player, SPELL_SUMMON_AGATHA1_AURA);
                        else if (player->GetQuestStatus(QUEST_RISE_FORSAKEN) == QUEST_STATUS_REWARDED &&
                            player->GetQuestStatus(QUEST_NO_ESCAPE) == QUEST_STATUS_INCOMPLETE)
                            player->CastWithDelay(100, player, SPELL_SUMMON_AGATHA1_AURA);
                        else
                            return;
                    }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agatha_resummon_controllerAI(creature);
    }
};

uint32 const SylvHorsePathSize = 28;
const G3D::Vector3 SylvHorsePath[SylvHorsePathSize] =
{
    { 1389.32f, 1023.86f, 53.131f, }, // 0 
    { 1368.86f, 1018.633f, 52.815f, }, // 1 talk 0
    { 1355.18f, 1018.536f, 52.317f, }, // 2 
    { 1345.47f, 1017.014f, 53.769f, }, // 3 talk 1
    { 1326.83f, 1012.754f, 54.600f, }, // 4 
    { 1315.39f, 1027.269f, 54.600f, }, // 5 talk 2
    { 1288.42f, 1068.989f, 53.618f, }, // 6 talk 3 with 1s delay
    { 1267.22f, 1095.244f, 51.750f, }, // 7 talk 4 with 1.5s delay
    { 1225.00f, 1130.903f, 50.475f, }, // 8 talk 5 with 2s delay
    { 1168.57f, 1169.004f, 48.617f, }, // 9 talk 6 with 1s delay
    { 1134.93f, 1191.119f, 47.752f, }, // 10 talk 7 with 2s delay
    { 1063.47f, 1249.424f, 46.168f, }, // 11 talk 8 with 1.5s delay
    { 1028.99f, 1281.528f, 45.980f, }, // 12 talk 9 with 2s delay
    { 971.798f, 1315.444f, 45.871f, }, // 13 talk 10 with 1s delay
    { 914.948f, 1349.042f, 48.216f, }, // 14 talk 11 with 0.5s delay
    { 865.714f, 1360.332f, 54.587f, }, // 15 talk 12 with 1s delay
    { 811.999f, 1360.771f, 57.923f, }, // 16 talk 13 
    { 754.691f, 1362.252f, 68.465f  }, // 17 talk 14 with 1.5s delay
    { 707.478f, 1342.741f, 76.237f, }, // 18 
    { 675.207f, 1313.741f, 80.521f, }, // 19 talk 15 with 1s delay
    { 643.180f, 1304.041f, 85.101f, }, // 20 
    { 634.199f, 1315.209f, 83.845f, }, // 21 
    { 614.233f, 1347.160f, 86.220f, }, // 22 talk 16 
    { 592.504f, 1393.575f, 91.655f, }, // 23 talk 17 with 0.5s delay
    { 572.768f, 1431.194f, 99.283f, }, // 24 talk 18 with 1s delay
    { 544.507f, 1461.334f, 108.42f, }, // 25 talk 19 with 1s delay
    { 515.305f, 1488.362f, 121.52f, }, // 26 
    { 497.88f, 1527.338f, 129.450f, }, // 27 talk 20 
    // slow mode
    //{ 496.797f, 1553.511f, 128.77f, }, // 28 
};

enum Forsaken
{
    NPC_PLAYER_HORSE = 45041,
    NPC_LADY_SYLV_HORSE = 45057,
    NPC_LADY_SYLVANAS_2 = 45051,

    SPELL_KILL_ME_AURA = 84181, // trigger: Trigger spell (84180) Kill Me (aoe ping)
    SPELL_HEARTSTRIKE = 84182,

    SPELL_DREADGUARD_SALUTE_AURA = 84200, // trigger ID - 84199 DREADGUARD SALUTE
    SPELL_EMOTE_DELAY = 151321,
};

class npc_forsaken_sylv_horse : public CreatureScript
{
public:
    npc_forsaken_sylv_horse() : CreatureScript("npc_forsaken_sylv_horse") { }

    struct npc_forsaken_sylv_horseAI : public VehicleAI
    {
        npc_forsaken_sylv_horseAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            summoner->RemoveAura(49417); // remove invisibility detection to see sylvanas
            _summons.Summon(me);
            me->SetDisableGravity(false);
            me->SetCanFly(false);

            if (auto player = summoner->ToPlayer())
            {
                player->RemoveAura(SPELL_RIDE_VEHICLE);
                _playerGUID = player->GetGUID();
                if (auto sylvanas = me->SummonCreature(NPC_LADY_SYLVANAS_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    sylvanas->EnterVehicle(me, 0);
                    _sylvanasGUID = sylvanas->GetGUID();
                    _summons.Summon(sylvanas);
                }

                if (auto pchorse = player->SummonCreature(NPC_PLAYER_HORSE, 1384.67f, 1037.459f, 54.15f, 3.7f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    _summons.Summon(pchorse);
                    _playerHorseGUID = pchorse->GetGUID();
                    pchorse->CastSpell(me, SPELL_RIDE_VEHICLE_SEAT_02);
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    player->CastWithDelay(500, pchorse, SPELL_RIDE_VEHICLE);
                }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId*/, bool apply)
        {
            if (apply && passenger->GetEntry() == NPC_PLAYER_HORSE)
                _events.ScheduleEvent(EVENT_WARHORSE_1, 1000);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type == SPLINE_MOTION_TYPE)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    if (auto sylvanas = Unit::GetCreature(*me, _sylvanasGUID))
                    {
                        switch (point)
                        {
                        case 1:
                            sylvanas->CastSpell(sylvanas, SPELL_KILL_ME_AURA);
                            sylvanas->AI()->Talk(0, _playerGUID);
                            break;
                        case 3:
                            sylvanas->AI()->Talk(1, _playerGUID);
                            break;
                        case 5:
                            sylvanas->AI()->Talk(2, _playerGUID);
                            break;
                        case 6:
                            sylvanas->AI()->TalkWithDelay(1000, 3, _playerGUID);
                            break;
                        case 7:
                            sylvanas->AI()->TalkWithDelay(1500, 4, _playerGUID);
                            break;
                        case 8:
                            sylvanas->AI()->TalkWithDelay(2000, 5, _playerGUID);
                            break;
                        case 9:
                            sylvanas->AI()->TalkWithDelay(1000, 6, _playerGUID);
                            break;
                        case 10:
                            sylvanas->AI()->TalkWithDelay(2000, 7, _playerGUID);
                            break;
                        case 11:
                            sylvanas->AI()->TalkWithDelay(1500, 8, _playerGUID);
                            break;
                        case 12:
                            sylvanas->AI()->TalkWithDelay(2000, 9, _playerGUID);
                            break;
                        case 13:
                            sylvanas->AI()->TalkWithDelay(1000, 10, _playerGUID);
                            break;
                        case 14:
                            sylvanas->AI()->TalkWithDelay(500, 11, _playerGUID);
                            break;
                        case 15:
                            sylvanas->AI()->TalkWithDelay(1000, 12, _playerGUID);
                            break;
                        case 16:
                            sylvanas->AI()->TalkWithDelay(100, 13, _playerGUID);
                            break;
                        case 17:
                            sylvanas->AI()->TalkWithDelay(1500, 14, _playerGUID);
                            break;
                        case 19:
                            sylvanas->RemoveAura(SPELL_KILL_ME_AURA);
                            sylvanas->CastSpell(sylvanas, SPELL_DREADGUARD_SALUTE_AURA);
                            sylvanas->AI()->TalkWithDelay(1000, 15, _playerGUID);
                            break;
                        case 22:
                            sylvanas->AI()->TalkWithDelay(100, 16, _playerGUID);
                            break;
                        case 23:
                            sylvanas->AI()->TalkWithDelay(500, 17, _playerGUID);
                            break;
                        case 24:
                            sylvanas->AI()->TalkWithDelay(1000, 18, _playerGUID);
                            break;
                        case 25:
                            sylvanas->RemoveAura(SPELL_DREADGUARD_SALUTE_AURA);
                            sylvanas->AI()->TalkWithDelay(1000, 19, _playerGUID);
                            break;
                        case 27:
                            sylvanas->AI()->TalkWithDelay(100, 20, _playerGUID);
                            me->GetMotionMaster()->MovementExpired(true);
                            _events.ScheduleEvent(EVENT_WARHORSE_3, 1000);
                            break;
                        default:
                            break;
                        }
                    }
            }

            if (type == POINT_MOTION_TYPE)
            {
                if (point == 1)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto pchorse = Unit::GetCreature(*me, _playerHorseGUID))
                        {
                            player->KilledMonsterCredit(45051);
                            pchorse->CastSpell(pchorse, SPELL_EJECT_ALL_PASSENGERS);
                            _summons.DespawnAll();
                            player->GetMotionMaster()->MoveJump(497.77f, 1559.04f, 128.29f, 10, 12);
                            player->CastWithDelay(100, player, SPELL_PLAYER_UPDATE_AREA);
                        }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;
                if (_tripInProgress)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (!player->GetAura(SPELL_RIDE_VEHICLE))
                            _summons.DespawnAll();
                    }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WARHORSE_1:
                    _tripInProgress = true;
                    _events.ScheduleEvent(EVENT_WARHORSE_2, 2000);
                    break;
                case EVENT_WARHORSE_2:
                    me->GetMotionMaster()->MoveSmoothPath(SylvHorsePath, SylvHorsePathSize);
                    break;
                case EVENT_WARHORSE_3:
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(1, 497.17f, 1554.972f, 128.703f, false);
                    if (auto sylvanas = Unit::GetCreature(*me, _sylvanasGUID))
                    {
                        sylvanas->AI()->TalkWithDelay(4000, 21, _playerGUID);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _tripInProgress = false;
        uint32 _checkTimer = 3000;
        uint64 _playerGUID = 0;
        uint64 _playerHorseGUID = 0;
        uint64 _sylvanasGUID = 0;

        SummonList _summons;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forsaken_sylv_horseAI(creature);
    }
};

class spell_kill_me_sylvanas : public SpellScriptLoader
{
public:
    spell_kill_me_sylvanas() : SpellScriptLoader("spell_kill_me_sylvanas") { }

    class spell_kill_me_sylvanas_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_kill_me_sylvanas_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleOnHit()
        {
            if (auto target = GetHitCreature())
                if (auto caster = GetCaster())
                    caster->CastSpell(target, SPELL_HEARTSTRIKE);
                else
                    return;
        }
        void Register()
        {
            OnHit += SpellHitFn(spell_kill_me_sylvanas_SpellScript::HandleOnHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_kill_me_sylvanas_SpellScript();
    }
};
// 84199
class spell_dreadguard_salute : public SpellScriptLoader
{
public:
    spell_dreadguard_salute() : SpellScriptLoader("spell_dreadguard_salute") { }

    class spell_dreadguard_salute_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dreadguard_salute_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleOnHit()
        {
            if (auto target = GetHitCreature())
                if (!target->GetAura(SPELL_EMOTE_DELAY))
                {
                    target->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    target->CastSpell(target, SPELL_EMOTE_DELAY);
                }
        }
        void Register()
        {
            OnHit += SpellHitFn(spell_dreadguard_salute_SpellScript::HandleOnHit);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_dreadguard_salute_SpellScript();
    }
};

enum TheWatersRunRed
{
    QUEST_WATER_RUN_RED = 27232,

    NPC_SUMMONED_HILLSBRAD_WORGEN = 45270,
};

class npc_horde_cannon_45263 : public CreatureScript
{
public:
    npc_horde_cannon_45263() : CreatureScript("npc_horde_cannon_45263") { }

    struct npc_horde_cannon_45263AI : public VehicleAI
    {
        npc_horde_cannon_45263AI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = true;
                me->SetRooted(true);

                _events.ScheduleEvent(EVENT_HORDE_CANNON_1, 3000, 0);

                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = false;
                _playerGUID = NULL;
                _summons.DespawnAll();
                _events.Reset();
            }
        }

        void SummonWorgens()
        {
            if (auto worgen1 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 805.598f, 704.482f, 54.832f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                if (auto worgen2 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 788.0078f, 724.038f, 46.58f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                    if (auto worgen3 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 778.0219f, 727.539f, 41.79f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                        if (auto worgen4 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 760.8857f, 724.518f, 39.64f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            if (auto worgen5 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 692.0272f, 715.251f, 33.90f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                if (auto worgen6 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 683.4419f, 721.917f, 33.06f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    if (auto worgen7 = me->SummonCreature(NPC_SUMMONED_HILLSBRAD_WORGEN, 670.4308f, 715.311f, 33.15f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    {
                                        _summons.Summon(worgen1);
                                        worgen1->setActive(true);
                                        worgen1->AI()->SetData(1, 1);
                                        _summons.Summon(worgen2);
                                        worgen2->setActive(true);
                                        worgen2->AI()->SetData(1, 2);
                                        _summons.Summon(worgen3);
                                        worgen3->setActive(true);
                                        worgen3->AI()->SetData(1, 3);
                                        _summons.Summon(worgen4);
                                        worgen4->setActive(true);
                                        worgen4->AI()->SetData(1, 4);
                                        _summons.Summon(worgen5);
                                        worgen5->setActive(true);
                                        worgen5->AI()->SetData(1, 5);
                                        _summons.Summon(worgen6);
                                        worgen6->setActive(true);
                                        worgen6->AI()->SetData(1, 6);
                                        _summons.Summon(worgen7);
                                        worgen7->setActive(true);
                                        worgen7->AI()->SetData(1, 7);
                                    }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff && _onUse)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_WATER_RUN_RED) == QUEST_STATUS_NONE)
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _summons.DespawnAll();
                        _events.Reset();
                        _onUse = false;
                    }

                    if (player->GetQuestStatus(QUEST_WATER_RUN_RED) == QUEST_STATUS_COMPLETE)
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _summons.DespawnAll();
                        _events.Reset();
                        _onUse = false;
                    }
                }
            }
            else
                _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HORDE_CANNON_1:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_2, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_2:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_3, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_3:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_4, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_4:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_5, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_5:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_6, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_6:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_7, urand(2000, 4500), 0);
                    break;
                case EVENT_HORDE_CANNON_7:
                    SummonWorgens();
                    _events.ScheduleEvent(EVENT_HORDE_CANNON_1, urand(10000, 15000), 0);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _onUse = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;

        SummonList _summons;
        EventMap   _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horde_cannon_45263AI(creature);
    }
};

uint32 const ForteskiPathSize = 11;
const G3D::Vector3 ForteskiPath[ForteskiPathSize] =
{
    { 370.56f, 1097.52f, 106.16f, },  // 0
    { 375.95f, 1079.73f, 106.18f, },  // 1
    { 379.15f, 1070.43f, 107.63f, },  // 2
    { 382.54f, 1057.50f, 107.02f, },  // 3
    { 387.01f, 1046.56f, 106.28f, },  // 4
    { 396.99f, 1039.98f, 106.77f, },  // 5
    { 407.40f, 1037.17f, 106.99f, },  // 6
    { 412.33f, 1033.48f, 106.83f, },  // 7
    { 415.63f, 1022.13f, 107.41f, },  // 8 
    { 412.09f, 1014.81f, 108.42f, },  // 9 
    { 408.17f, 1006.94f, 107.41f, },  // 10
};

uint32 const IvanEscapePathSize = 3;
const G3D::Vector3 IvanEscapePath[IvanEscapePathSize] =
{
    { 370.56f, 1097.52f, 106.16f, },  // 0
    { 375.95f, 1079.73f, 106.18f, },  // 1
    { 379.15f, 1070.43f, 107.63f, },  // 2
};

enum NowhereToRun
{
    QUEST_NOWHERE_TO_RUN = 27195,
    NPC_VETERAN_FORSAKEN = 45225,
    NPC_IVAN_BLOODFANG_NOWHERE = 45236,
    NPC_CHECKIFSTARTED = 74948,
    SPELL_TOSS_BOMB = 84467,
    SPELL_BOMB_EXPLOSION = 84474,
    SPELL_BURNING_CORPSE = 59216,
};

class npc_nowhere_master_forteski : public CreatureScript
{
public:
    npc_nowhere_master_forteski() : CreatureScript("npc_nowhere_master_forteski") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (!creature->FindNearestCreature(NPC_CHECKIFSTARTED, 10.f))
        {
            if (quest->GetQuestId() == QUEST_NOWHERE_TO_RUN)
            {
                player->SummonCreature(creature->GetEntry(), creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                creature->SummonCreature(NPC_CHECKIFSTARTED, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 54000);
            }
        }
        else
            creature->AI()->Talk(5, player->GetGUID());

        return true;
    }

    struct npc_nowhere_master_forteskiAI : public ScriptedAI
    {
        struct npc_nowhere_master_forteskiAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            _summons.Summon(me);
            _playerGUID = summoner->GetGUID();
            me->RemoveAura(49414);
            me->CastSpell(me, 49415);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            _events.ScheduleEvent(EVENT_FORTESKI_1, 1000);

            if (auto veteran1 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 364.993f, 1093.53f, 106.178f, 5.32f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                if (auto veteran2 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 373.549f, 1098.99f, 106.346f, 4.97f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                    if (auto veteran3 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 369.517f, 1098.67f, 106.282f, 5.07f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        if (auto veteran4 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 365.075f, 1097.21f, 106.212f, 5.27f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                            if (auto veteran5 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 368.543f, 1095.05f, 106.191f, 4.97f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                if (auto veteran6 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 376.009f, 1096.43f, 106.276f, 5.00f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                    if (auto veteran7 = me->SummonCreature(NPC_VETERAN_FORSAKEN, 372.582f, 1096.04f, 106.233f, 4.97f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                                    {
                                        _summons.Summon(veteran1);
                                        _summons.Summon(veteran2);
                                        _summons.Summon(veteran3);
                                        _summons.Summon(veteran4);
                                        _summons.Summon(veteran5);
                                        _summons.Summon(veteran6);
                                        _summons.Summon(veteran7);
                                        veteran1->RemoveAura(49414);
                                        veteran2->RemoveAura(49414);
                                        veteran3->RemoveAura(49414);
                                        veteran4->RemoveAura(49414);
                                        veteran5->RemoveAura(49414);
                                        veteran6->RemoveAura(49414);
                                        veteran7->RemoveAura(49414);
                                        veteran1->CastSpell(veteran1, 49415);
                                        veteran2->CastSpell(veteran2, 49415);
                                        veteran3->CastSpell(veteran3, 49415);
                                        veteran4->CastSpell(veteran4, 49415);
                                        veteran5->CastSpell(veteran5, 49415);
                                        veteran6->CastSpell(veteran6, 49415);
                                        veteran7->CastSpell(veteran7, 49415);
                                        _veteranGUID1 = veteran1->GetGUID();
                                        _veteranGUID2 = veteran2->GetGUID();
                                        _veteranGUID3 = veteran3->GetGUID();
                                        _veteranGUID4 = veteran4->GetGUID();
                                        _veteranGUID5 = veteran5->GetGUID();
                                        _veteranGUID6 = veteran6->GetGUID();
                                        _veteranGUID7 = veteran7->GetGUID();
                                    }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type == SPLINE_MOTION_TYPE)
            {
                if (point == 10)
                {
                    if (auto veteran1 = Unit::GetCreature(*me, _veteranGUID1))
                        if (auto veteran2 = Unit::GetCreature(*me, _veteranGUID2))
                            if (auto veteran3 = Unit::GetCreature(*me, _veteranGUID3))
                                if (auto veteran4 = Unit::GetCreature(*me, _veteranGUID4))
                                    if (auto veteran5 = Unit::GetCreature(*me, _veteranGUID5))
                                        if (auto veteran6 = Unit::GetCreature(*me, _veteranGUID6))
                                            if (auto veteran7 = Unit::GetCreature(*me, _veteranGUID7))
                                            {
                                                veteran1->GetMotionMaster()->MovePoint(1, 405.954f, 1006.33f, 107.434f);
                                                veteran2->GetMotionMaster()->MovePoint(1, 406.04f, 1005.21f, 107.42f);
                                                veteran3->GetMotionMaster()->MovePoint(1, 409.193f, 1004.6f, 107.515f);
                                                veteran4->GetMotionMaster()->MovePoint(1, 408.313f, 1003.9f, 107.49f);
                                                veteran5->GetMotionMaster()->MovePoint(1, 407.0f, 1004.42f, 107.411f);
                                                veteran6->GetMotionMaster()->MovePoint(1, 410.039f, 1005.81f, 107.52f);
                                                veteran7->GetMotionMaster()->MovePoint(1, 406.497f, 1007.49f, 107.402f);
                                                me->AI()->TalkWithDelay(500, 2);
                                                _events.ScheduleEvent(EVENT_FORTESKI_5, 3000);
                                            }
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_BOMB_EXPLOSION)
                _events.ScheduleEvent(EVENT_FORTESKI_7, 2000);
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_NOWHERE_TO_RUN) == QUEST_STATUS_NONE)
                        _summons.DespawnAll();
                }
            }
            else
                _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FORTESKI_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetFacingToObject(player);
                        me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                        _events.ScheduleEvent(EVENT_FORTESKI_2, 5500);
                    }
                    break;
                case EVENT_FORTESKI_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetFacingTo(5.05f);
                        me->AI()->TalkWithDelay(1000, 1, _playerGUID);
                        _events.ScheduleEvent(EVENT_FORTESKI_3, 4000);
                    }
                    break;
                case EVENT_FORTESKI_3:
                    if (auto veteran1 = Unit::GetCreature(*me, _veteranGUID1))
                        if (auto veteran2 = Unit::GetCreature(*me, _veteranGUID2))
                            if (auto veteran3 = Unit::GetCreature(*me, _veteranGUID3))
                                if (auto veteran4 = Unit::GetCreature(*me, _veteranGUID4))
                                    if (auto veteran5 = Unit::GetCreature(*me, _veteranGUID5))
                                        if (auto veteran6 = Unit::GetCreature(*me, _veteranGUID6))
                                            if (auto veteran7 = Unit::GetCreature(*me, _veteranGUID7))
                                                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                                                {
                                                    veteran1->GetMotionMaster()->MoveFollow(me, 1, 1.0f * M_PI);
                                                    veteran2->GetMotionMaster()->MoveFollow(me, 2, 1.0f * M_PI);
                                                    veteran3->GetMotionMaster()->MoveFollow(me, 3, 1.0f * M_PI);
                                                    veteran4->GetMotionMaster()->MoveFollow(me, 4, 1.0f * M_PI);
                                                    veteran5->GetMotionMaster()->MoveFollow(me, 5, 1.0f * M_PI);
                                                    veteran6->GetMotionMaster()->MoveFollow(me, 6, 1.0f * M_PI);
                                                    veteran7->GetMotionMaster()->MoveFollow(me, 7, 1.0f * M_PI);
                                                    _events.ScheduleEvent(EVENT_FORTESKI_4, 1000);
                                                }
                    break;
                case EVENT_FORTESKI_4:
                    me->GetMotionMaster()->MoveSmoothPath(ForteskiPath, ForteskiPathSize);
                    break;
                case EVENT_FORTESKI_5:
                    me->SetFacingTo(1.2f);
                    if (auto bloodfang = me->SummonCreature(NPC_IVAN_BLOODFANG_NOWHERE, 414.376f, 1023.287f, 107.38f, 4.35f, TEMPSUMMON_TIMED_DESPAWN, 15000))
                    {
                        _bloodfangGUID = bloodfang->GetGUID();
                        // me->GetMotionMaster()->MovePoint(1, 408.48f, 1007.89f, 107.42f);
                        bloodfang->AI()->Talk(0);
                        bloodfang->CastWithDelay(3000, NULL, SPELL_TOSS_BOMB);
                        _events.ScheduleEvent(EVENT_FORTESKI_6, 4500);
                    }
                    break;
                case EVENT_FORTESKI_6:
                    if (auto bloodfang = Unit::GetCreature(*me, _bloodfangGUID))
                    {
                        me->AI()->TalkWithDelay(0, 3, _playerGUID);
                        me->AI()->TalkWithDelay(2000, 4, _playerGUID);
                        bloodfang->GetMotionMaster()->MoveSmoothPath(IvanEscapePath, IvanEscapePathSize);
                        bloodfang->DespawnOrUnsummon(5000);
                    }
                    break;
                case EVENT_FORTESKI_7:
                {
                    std::list<Player*> player = me->GetPlayersInRange(80, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_NOWHERE_TO_RUN) == QUEST_STATUS_INCOMPLETE)
                        {
                            (*it)->AreaExploredOrEventHappens(QUEST_NOWHERE_TO_RUN);
                            (*it)->KilledMonsterCredit(45237); // credit
                        }

                    me->Kill(me);
                    me->CastSpell(me, SPELL_BURNING_CORPSE);
                    break;
                }
                default:
                    break;
                }
            }
        }

    private:
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _veteranGUID1 = 0;
        uint64 _veteranGUID2 = 0;
        uint64 _veteranGUID3 = 0;
        uint64 _veteranGUID4 = 0;
        uint64 _veteranGUID5 = 0;
        uint64 _veteranGUID6 = 0;
        uint64 _veteranGUID7 = 0;
        uint64 _bloodfangGUID = 0;

        SummonList _summons;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nowhere_master_forteskiAI(creature);
    }
};

class npc_veteran_forsaken_trooper : public CreatureScript
{
public:
    npc_veteran_forsaken_trooper() : CreatureScript("npc_veteran_forsaken_trooper") { }

    struct npc_veteran_forsaken_trooperAI : public ScriptedAI
    {
        npc_veteran_forsaken_trooperAI(Creature* creature) : ScriptedAI(creature) {}

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (point = 1)
                me->SetFacingTo(1.2f);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_BOMB_EXPLOSION)
                _events.ScheduleEvent(EVENT_FORTESKI_8, 2000);
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (eventId = EVENT_FORTESKI_8)
                {
                    me->Kill(me);
                    me->CastSpell(me, SPELL_BURNING_CORPSE);
                }
            }
        }
    private:
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_veteran_forsaken_trooperAI(creature);
    }
};

uint32 const ArthuraIntroPathSize = 15;
const G3D::Vector3 ArthuraIntroPath[ArthuraIntroPathSize] =
{
    { 492.876f, 1558.479f, 138.7f, },
    { 444.194f, 1526.996f, 154.7f, },
    { 366.077f, 1481.080f, 153.2f, },
    { 314.288f, 1470.535f, 163.3f, },
    { 124.761f, 1442.061f, 170.2f, },
    { 25.7338f, 1422.885f, 175.2f, },
    { -133.604f, 1382.786f, 173.6f, },
    { -243.001f, 1344.534f, 168.8f, },
    { -422.929f, 1367.138f, 148.2f, },
    { -586.366f, 1483.351f, 131.8f, },
    { -727.863f, 1530.276f, 151.6f, },
    { -810.104f, 1561.486f, 148.4f, },
    { -865.528f, 1578.284f, 115.9f, },
    { -893.967f, 1617.825f, 92.42f, },
    { -923.207f, 1636.401f, 72.36f, },
};

class npc_arthura_flight_sepuchler : public CreatureScript
{
public:
    npc_arthura_flight_sepuchler() : CreatureScript("npc_arthura_flight_sepuchler") { }

    struct npc_arthura_flight_sepuchlerAI : public ScriptedAI
    {
        npc_arthura_flight_sepuchlerAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                _playerGUID = player->GetGUID();
            }
            else
                return;
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _inProgress = true;
                _events.ScheduleEvent(EVENT_INTRO_FLIGHT_ARTHURA, 1000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER && _inProgress)
            {
                passenger->CastSpell(passenger, 54168); // parachute
                me->DespawnOrUnsummon();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 13)
                _inProgress = false;

            if (point == 14)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                me->DespawnOrUnsummon(1000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_INTRO_FLIGHT_ARTHURA:
                    me->GetMotionMaster()->MoveSmoothPath(ArthuraIntroPath, ArthuraIntroPathSize);
                    me->AI()->TalkWithDelay(2000, 0, _playerGUID);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _inProgress = false;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arthura_flight_sepuchlerAI(creature);
    }
};

class go_7th_legion_telescope : public GameObjectScript
{
public:
    go_7th_legion_telescope() : GameObjectScript("go_7th_legion_telescope") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(27401) == QUEST_STATUS_INCOMPLETE) // tomorrow never brings
        {
            if (auto camera = player->SummonCreature(45427, -1460.48f, 1720.85f, 24.341f, 0.9376f, TEMPSUMMON_TIMED_DESPAWN, 120000))
            {
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                // player->CastWithDelay(500, camera, SPELL_RIDE_VEHICLE);
            }
        }

        return true;
    }
};
uint32 const CameraTelescopePathSize = 5;
const G3D::Vector3 CameraTelescopePath[CameraTelescopePathSize] =
{
    { -1453.45f, 1732.00f, 28.63f, },
    { -1369.92f, 1848.82f, 31.91f, },
    { -1291.73f, 1942.79f, 29.29f, },
    { -1235.85f, 1992.15f, 15.85f, },
    { -1148.59f, 2096.81f, 13.10f, },
};

uint32 const CameraTelescopePathSize2 = 5;
const G3D::Vector3 CameraTelescopePath2[CameraTelescopePathSize2] =
{
    { -1235.85f, 1992.15f, 15.85f, },
    { -1291.73f, 1942.79f, 29.29f, },
    { -1369.92f, 1848.82f, 31.91f, },
    { -1453.45f, 1732.00f, 28.63f, },
    { -1467.25f, 1712.87f, 29.79f, },
};

// not blizzlike because of falling camera view under water during change seat

class npc_camera_silverpine_telescope : public CreatureScript
{
public:
    npc_camera_silverpine_telescope() : CreatureScript("npc_camera_silverpine_telescope") { }

    struct npc_camera_silverpine_telescopeAI : public VehicleAI
    {
        npc_camera_silverpine_telescopeAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetDisableGravity(true);
            me->SetCanFly(true);

            summoner->CastSpell(me, 98707);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _back = false;
                _inProgress = true;
                _events.ScheduleEvent(EVENT_TELESCOPE_START_PATH, 1000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (_inProgress)
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->GetMotionMaster()->Clear();
                    me->DespawnOrUnsummon(1);

                    if (auto player = passenger->ToPlayer())
                    {
                        player->CastSpell(player, 151320); // teleport to telescope
                        player->KilledMonsterCredit(45430);
                    }
                }

                if (!_inProgress)
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->GetMotionMaster()->Clear();
                    me->DespawnOrUnsummon(1000);

                    if (auto player = passenger->ToPlayer())
                    {
                        player->CastSpell(player, 151320); // teleport to telescope
                        player->KilledMonsterCredit(45430);
                    }
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 4 && !_back)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->GetMotionMaster()->Clear();
                    me->SetSpeed(MOVE_FLIGHT, 4.0f);
                    me->SetSpeed(MOVE_RUN, 4.0f);
                    _back = true;
                    me->AI()->Talk(0, _playerGUID);
                    _events.ScheduleEvent(EVENT_TELESCOPE_EXIT, 4500);
                }
            }
            else if (point == 4 && _back)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                }
            }

            else if (point == 3 && _back)
            {
                _inProgress = false;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TELESCOPE_EXIT:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->GetMotionMaster()->MoveSmoothPath(CameraTelescopePath2, CameraTelescopePathSize2);
                    }
                    break;
                case EVENT_TELESCOPE_START_PATH:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->GetMotionMaster()->MoveSmoothPath(CameraTelescopePath, CameraTelescopePathSize);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _inProgress = false;
        bool _back = false;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_camera_silverpine_telescopeAI(creature);
    }
};

enum ArthuraDatas
{
    QUEST_A_MAN_NAMED_GODFREY = 27406,
    QUEST_RESISTANCE_IS_FUTILE = 27423,

    SPELL_SUMMON_ARTHURA1_AURA = 84941,
};

// ruins of gilneas 
class npc_arthura_ruins : public CreatureScript
{
public:
    npc_arthura_ruins() : CreatureScript("npc_arthura_ruins") { }

    struct npc_arthura_ruinsAI : public ScriptedAI
    {
        npc_arthura_ruinsAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
            }
            else
                return;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_DOOMHOWL, urand(4000, 8000));
                _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(1000, 4000));
            }
            else
                _events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                    {
                        player->RemoveAura(SPELL_SUMMON_ARTHURA1_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if ((player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_NONE && player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_NONE) ||
                        (player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_REWARDED))
                    {
                        player->RemoveAura(SPELL_SUMMON_ARTHURA1_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 1.0f, 1.3f * M_PI);
                    }

                    if (!_isFollowing)
                    {
                        me->GetMotionMaster()->MoveFollow(player, 1.0f, 1.3f * M_PI);
                        _isFollowing = true;
                    }
                }
            }
            else _checkTimer -= diff;

            if (!UpdateVictim())
                return;

            // if healing spell
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (player->GetHealthPct() <= 35 && !_lowHP)
                {
                    _events.Reset();
                    me->CastSpell(player, SPELL_UNHOLY_DARKNESS);
                    _lowHP = true;
                }

                if (player->GetHealthPct() >= 36 && _lowHP)
                    _lowHP = false;
            }
            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DOOMHOWL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_DOOMHOWL);
                    _events.ScheduleEvent(EVENT_DOOMHOWL, urand(12000, 25000));
                    break;
                case EVENT_UNHOLY_SMITE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_UNHOLY_SMITE);
                    _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(3000, 7000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _isFollowing = false;
        bool _lowHP = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 500;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arthura_ruinsAI(creature);
    }
};

enum BelmontDatas
{
    SPELL_SUMMON_BELMONT_AURA = 84944,
    SPELL_FANOFKNIFES = 79584,
    SPELL_SHADOWSTEP = 78638,
    SPELL_BACKSTAB = 76520,
};

// ruins of gilneas 
class npc_belmont_guardian : public CreatureScript
{
public:
    npc_belmont_guardian() : CreatureScript("npc_belmont_guardian") { }

    struct npc_belmont_guardianAI : public ScriptedAI
    {
        npc_belmont_guardianAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_FANOFKNIFES, urand(4000, 8000));
                _events.ScheduleEvent(EVENT_SHADOWSTEP, urand(1000, 4000));
            }
            else
                _events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                    {
                        player->RemoveAura(SPELL_SUMMON_BELMONT_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if ((player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_NONE && player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_NONE) ||
                        (player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_REWARDED))
                    {
                        player->RemoveAura(SPELL_SUMMON_BELMONT_AURA);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_IN_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 1.0f, 0.7f * M_PI);
                    }

                    if (!_isFollowing)
                    {
                        me->GetMotionMaster()->MoveFollow(player, 1.0f, 0.7f * M_PI);
                        _isFollowing = true;
                    }

                    if (player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_COMPLETE)
                    {
                        if (!player->GetAura(49416)) // invis 
                        {
                            player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FANOFKNIFES:
                    me->CastSpell(me, SPELL_FANOFKNIFES);
                    _events.ScheduleEvent(EVENT_FANOFKNIFES, urand(6000, 11000));
                    break;
                case EVENT_SHADOWSTEP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_SHADOWSTEP);
                    me->CastWithDelay(1000, me->getVictim(), SPELL_BACKSTAB);
                    _events.ScheduleEvent(EVENT_SHADOWSTEP, urand(4000, 12000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _isFollowing = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 500;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_belmont_guardianAI(creature);
    }
};

enum BattlefrontSummons
{
    SPELL_BATTLEFRONT_SUMMON_GODFREY = 85517,  // 45878
    SPELL_BATTLEFRONT_SUMMON_WALDEN = 85518,  // 45879
    SPELL_BATTLEFRONT_SUMMON_ASHBURY = 85519,  // 45880

    QUEST_TAKING_BATTLEFRONT = 27542,
};

class playerscript_silverpine_quest_guardian_summons : public PlayerScript
{
public:
    playerscript_silverpine_quest_guardian_summons() : PlayerScript("playerscript_silverpine_quest_guardian_summons") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newZone == 4706) // ruins of gilneas
        {
            if (!player->GetAura(SPELL_SUMMON_BELMONT_AURA) && !player->GetAura(SPELL_SUMMON_ARTHURA1_AURA))
            {
                if (player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_A_MAN_NAMED_GODFREY) == QUEST_STATUS_COMPLETE ||
                    player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_RESISTANCE_IS_FUTILE) == QUEST_STATUS_INCOMPLETE)
                {
                    player->CastSpell(player, SPELL_SUMMON_BELMONT_AURA);
                    player->CastSpell(player, SPELL_SUMMON_ARTHURA1_AURA);
                }
            }
        }
        else if (newZone == 130 && (newArea == 5387 || newArea == 5456 || newArea == 204 || newArea == 5480))
        {
            if (player->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_REWARDED)
                return;

            if (!player->GetAura(SPELL_BATTLEFRONT_SUMMON_GODFREY) && !player->GetAura(SPELL_BATTLEFRONT_SUMMON_WALDEN) && !player->GetAura(SPELL_BATTLEFRONT_SUMMON_ASHBURY))
            {
                if (player->GetQuestStatus(QUEST_TAKING_BATTLEFRONT) == QUEST_STATUS_REWARDED)
                {
                    player->CastSpell(player, SPELL_BATTLEFRONT_SUMMON_GODFREY);
                    player->CastSpell(player, SPELL_BATTLEFRONT_SUMMON_WALDEN);
                    player->CastSpell(player, SPELL_BATTLEFRONT_SUMMON_ASHBURY);
                }
            }
        }
        else
            return;
    }
};

enum ArthuraFlight2Datas
{
    NPC_ARTHURA_VEHICLE = 45555,
    NPC_GODFREY_1 = 45556,
    NPC_BELMONT_2 = 45557,
    QUEST_GREAT_ESCAPE = 27438,
    SPELL_FREEZE_ANIM = 93199,
};

uint32 const Arthura2PathSize = 2;
const G3D::Vector3 Arthura2Path[Arthura2PathSize] =
{
    { -2117.14111f, 934.15802f, 34.3799f, },
    { -2112.79248f, 935.61163f, 82.1235f, },
};

uint32 const Arthura2PathSize2 = 22;
const G3D::Vector3 Arthura2Path2[Arthura2PathSize2] =
{
    { -2047.9951f, 937.7509f, 96.253f, },
    { -1927.1787f, 931.2210f, 94.633f, },
    { -1819.4152f, 975.2877f, 95.355f, },
    { -1803.6243f, 1100.201f, 68.498f, },
    { -1584.2254f, 1154.681f, 70.298f, },
    { -1473.6402f, 1156.272f, 67.251f, },
    { -1368.7523f, 1217.753f, 62.740f, },
    { -1333.5772f, 1407.356f, 69.552f, },
    { -1309.2415f, 1610.949f, 74.944f, },
    { -1245.4510f, 1698.926f, 71.858f, },
    { -1193.0434f, 1697.864f, 65.810f, },
    { -1108.6926f, 1638.513f, 71.605f, },
    { -993.50024f, 1576.868f, 65.549f, },
    { -904.35955f, 1559.486f, 77.697f, },
    { -825.77f, 1533.27f, 108.76f, },
    { -745.71f, 1521.80f, 121.70f, },
    { -651.12152f, 1494.841f, 93.887f, },
    { -504.50476f, 1409.479f, 92.689f, },
    { -410.55331f, 1316.347f, 85.182f, },
    { -301.21499f, 1236.661f, 75.684f, },
    { -254.06637f, 1246.742f, 79.608f, },
    { -193.93673f, 1274.894f, 44.025f, }, // 21
};

class npc_arthura_flight_escape : public CreatureScript
{
public:
    npc_arthura_flight_escape() : CreatureScript("npc_arthura_flight_escape") { }

    struct npc_arthura_flight_escapeAI : public VehicleAI
    {
        npc_arthura_flight_escapeAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                me->CastSpell(player, SPELL_FREEZE_ANIM);
                //player->EnterVehicle(me, 1);
                me->SetRooted(true);
                player->AreaExploredOrEventHappens(QUEST_GREAT_ESCAPE);

                if (auto godfrey = me->SummonCreature(NPC_GODFREY_1, -2121.17f, 931.86f, 1.727f, 5.72f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                    if (auto belmont = me->SummonCreature(NPC_BELMONT_2, -2122.47f, 930.82f, 1.76f, 0.62f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                    {
                        godfrey->EnterVehicle(me, 1);
                        _summons.Summon(godfrey);
                        _summons.Summon(belmont);
                        _summons.Summon(me);
                        _belmontGUID = belmont->GetGUID();
                        _godfreyGUID = godfrey->GetGUID();
                    }
            }

            me->SetDisableGravity(true);
            me->SetCanFly(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto belmont = Unit::GetCreature(*me, _belmontGUID))
                {
                    me->AI()->TalkWithDelay(1000, 0);
                    belmont->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                    belmont->AI()->TalkWithDelay(5000, 0);
                    belmont->AI()->TalkWithDelay(11000, 1);
                    belmont->AI()->TalkWithDelay(16000, 2);
                    belmont->DespawnOrUnsummon(26000);
                    _events.ScheduleEvent(EVENT_ARTHURA_FLIGHT_1, 20000);
                }
            }

            if (!apply)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->RemoveAura(SPELL_FREEZE_ANIM);

                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                _summons.DespawnAll();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 1 && _introFlight)
            {
                me->GetMotionMaster()->MoveSmoothPath(Arthura2Path2, Arthura2PathSize2);
                _introFlight = false;
            }

            if (point == 21 && !_introFlight)
            {
                me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                _summons.DespawnAll();

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->RemoveAura(SPELL_FREEZE_ANIM);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive())
                    {
                        _summons.DespawnAll();
                        player->RemoveAura(SPELL_FREEZE_ANIM);
                    }

                    if (player->GetQuestStatus(QUEST_GREAT_ESCAPE) == QUEST_STATUS_NONE)
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        _summons.DespawnAll();
                        player->RemoveAura(SPELL_FREEZE_ANIM);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ARTHURA_FLIGHT_1:
                    me->SetRooted(false);
                    me->AI()->TalkWithDelay(0, 1);
                    me->GetMotionMaster()->MoveSmoothPath(Arthura2Path, Arthura2PathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _introFlight = true;
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _belmontGUID = 0;
        uint64 _godfreyGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arthura_flight_escapeAI(creature);
    }
};

enum RiseGodfrey
{
    NPC_HIGH_WARLORD_CROMUSH_3 = 45612,
    NPC_LADY_SYLVANAS_3 = 45605,
    NPC_GODFREY_3 = 45608,
    NPC_ASHBURY_3 = 45606,
    NPC_WALDEN_3 = 45607,
    NPC_VALKYR_DASCHLA_3 = 45609,
    NPC_VALKYR_AGATHA_3 = 45610,
    NPC_VALKYR_ARTHURA_3 = 45611,

    SPELL_RISE_FORSAKEN_3 = 85197,
    SPELL_GODFREY_TRANSFORM = 85198,
    SPELL_ASHBURY_TRANSFORM = 85200,
    SPELL_WALDEN_TRANSFORM = 85201,

    QUEST_RISE_GODFREY = 27472,
    // SPELL_FORSAKEN_EXPLODE = 81876,
};

// cutscene quest=27472/rise-godfrey
class npc_sylvanas_rise_godfrey_scene : public CreatureScript
{
public:
    npc_sylvanas_rise_godfrey_scene() : CreatureScript("npc_sylvanas_rise_godfrey_scene") { }

    struct npc_sylvanas_rise_godfrey_sceneAI : public ScriptedAI
    {
        npc_sylvanas_rise_godfrey_sceneAI(Creature* creature) : ScriptedAI(creature), summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->AI()->TalkWithDelay(2000, 0);

            _events.ScheduleEvent(EVENT_RISE_GODFREY_1, 8000);


            if (auto cromush = me->SummonCreature(NPC_HIGH_WARLORD_CROMUSH_3, -180.199f, 1280.599f, 45.91f, 3.1241f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                if (auto agatha = me->SummonCreature(NPC_VALKYR_AGATHA_3, -193.287f, 1282.369f, 47.624f, 3.83f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                    if (auto daschla = me->SummonCreature(NPC_VALKYR_DASCHLA_3, -194.350f, 1269.229f, 49.918f, 2.19f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                        if (auto arthura = me->SummonCreature(NPC_VALKYR_ARTHURA_3, -203.306f, 1275.540f, 45.580f, 6.19f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                            if (auto ashbury = me->SummonCreature(NPC_ASHBURY_3, -197.397f, 1279.18f, 39.64f, 5.2f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                if (auto godfrey = me->SummonCreature(NPC_GODFREY_3, -198.326f, 1271.95f, 40.90f, 0.8f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    if (auto walden = me->SummonCreature(NPC_WALDEN_3, -198.587f, 1275.18f, 40.07f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 300000))
                                    {
                                        summons.Summon(daschla);
                                        summons.Summon(agatha);
                                        summons.Summon(arthura);
                                        summons.Summon(godfrey);
                                        summons.Summon(ashbury);
                                        summons.Summon(walden);
                                        summons.Summon(cromush);
                                    }
        }
        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (point = 1)
            {
                me->SetFacingTo(3.07f);
                _events.ScheduleEvent(EVENT_RISE_GODFREY_5, 2000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RISE_GODFREY_1:
                    if (auto agatha = me->FindNearestCreature(NPC_VALKYR_AGATHA_3, 30.f))
                        if (auto daschla = me->FindNearestCreature(NPC_VALKYR_DASCHLA_3, 30.f))
                            if (auto arthura = me->FindNearestCreature(NPC_VALKYR_ARTHURA_3, 30.f))
                            {
                                agatha->CastSpell(agatha, SPELL_RISE_FORSAKEN_3);
                                arthura->CastSpell(arthura, SPELL_RISE_FORSAKEN_3);
                                daschla->CastSpell(daschla, SPELL_RISE_FORSAKEN_3);
                            }
                    _events.ScheduleEvent(EVENT_RISE_GODFREY_2, 8000);
                    break;
                case EVENT_RISE_GODFREY_2:
                    if (auto ashbury = me->FindNearestCreature(NPC_GODFREY_3, 30.f))
                        if (auto walden = me->FindNearestCreature(NPC_WALDEN_3, 30.f))
                            if (auto godfrey = me->FindNearestCreature(NPC_ASHBURY_3, 30.f))
                                if (auto cromush = me->FindNearestCreature(NPC_HIGH_WARLORD_CROMUSH_3, 30.f))
                                {
                                    ashbury->RemoveAura(29266);
                                    ashbury->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                                    ashbury->CastSpell(ashbury, SPELL_ASHBURY_TRANSFORM);
                                    walden->RemoveAura(29266);
                                    walden->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                                    walden->CastSpell(walden, SPELL_WALDEN_TRANSFORM);
                                    godfrey->RemoveAura(29266);
                                    godfrey->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 7);
                                    godfrey->CastSpell(godfrey, SPELL_GODFREY_TRANSFORM);

                                    godfrey->CastSpell(godfrey, SPELL_FORSAKEN_EXPLODE);
                                    walden->CastSpell(walden, SPELL_FORSAKEN_EXPLODE);
                                    ashbury->CastSpell(ashbury, SPELL_FORSAKEN_EXPLODE);

                                    godfrey->AI()->TalkWithDelay(1000, 0);
                                    me->AI()->TalkWithDelay(5000, 1);
                                    cromush->AI()->TalkWithDelay(11000, 0);
                                    me->AI()->TalkWithDelay(15000, 2);
                                    godfrey->AI()->TalkWithDelay(18000, 1);
                                    godfrey->AI()->TalkWithDelay(24000, 2);
                                    godfrey->AI()->TalkWithDelay(29000, 3);
                                    me->AI()->TalkWithDelay(32000, 3);
                                    me->AI()->TalkWithDelay(40000, 4);
                                    godfrey->AI()->TalkWithDelay(45000, 4);
                                }
                    _events.ScheduleEvent(EVENT_RISE_GODFREY_3, 49000);
                    break;
                case EVENT_RISE_GODFREY_3:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto ashbury = me->FindNearestCreature(NPC_GODFREY_3, 30.f))
                            if (auto walden = me->FindNearestCreature(NPC_WALDEN_3, 30.f))
                                if (auto godfrey = me->FindNearestCreature(NPC_ASHBURY_3, 30.f))
                                {
                                    ashbury->AI()->SetData(1, 1);
                                    walden->AI()->SetData(1, 1);
                                    godfrey->AI()->SetData(1, 1);
                                    me->SetFacingToObject(player);
                                    me->AI()->TalkWithDelay(1000, 5, _playerGUID);
                                }
                    _events.ScheduleEvent(EVENT_RISE_GODFREY_4, 6000);
                    break;
                case EVENT_RISE_GODFREY_4:
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(1, -153.32f, 1270.27f, 51.54f);
                    break;
                case EVENT_RISE_GODFREY_5:
                {
                    std::list<Player*> player = me->GetPlayersInRange(100, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_RISE_GODFREY) == QUEST_STATUS_INCOMPLETE)
                            (*it)->KilledMonsterCredit(45617); // credit

                    summons.DespawnAll();
                    me->DespawnOrUnsummon();
                }
                break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sylvanas_rise_godfrey_sceneAI(creature);
    }
};

enum Transdimentional3
{
    NPC_CREDIT_TRANSDIMENTIONAL_3 = 45769,
    NPC_TRANSDIMENTIONAL_PORTAL = 45764,
    NPC_ARCANE_FIENDS = 45766,

    SPELL_SUMMON_ARCANE_FIEND = 87253,
    SPELL_ARCANE_SHIELD = 87251,
    SPELL_SUICIDE = 87255,
    SPELL_ZERO_POWER = 87239,
    SPELL_ARCANE_EXPULSION = 87249,
    SPELL_DIMENSIONAL_BEAM = 85373,
    SPELL_DIMENSIONAL_COLLAPSE = 85400,
    SPELL_SELF_STUN = 85379,
};

// cutscene quest=27472/rise-godfrey
class npc_archmage_ataeric_transdimentional_3 : public CreatureScript
{
public:
    npc_archmage_ataeric_transdimentional_3() : CreatureScript("npc_archmage_ataeric_transdimentional_3") { }

    struct npc_archmage_ataeric_transdimentional_3AI : public ScriptedAI
    {
        npc_archmage_ataeric_transdimentional_3AI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void InitializeAI() override
        {
            JustRespawned();
        }

        void JustRespawned() override
        {
            me->setActive(true);
            _eventStarted = false;
            me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
            me->CastWithDelay(200, me, SPELL_DIMENSIONAL_BEAM);
            me->CastSpell(me, SPELL_ZERO_POWER);
            me->CastSpell(me, SPELL_SELF_STUN);
            me->CastSpell(me, SPELL_ARCANE_SHIELD);

            _phase = 0;
            _killCount = 0;
        }

        void Reset()
        {
            me->SetRooted(true);
            _checkTimer = 3000;
            me->AI()->EnterEvadeMode();
            _eventStarted = false;
            _phase = 0;
            _killCount = 0;
            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
            _summons.DespawnAll();
            std::list<Unit*> fiends = me->SelectNearbyUnits(NPC_ARCANE_FIENDS, 60.0f);
            for (std::list<Unit*>::iterator itr = fiends.begin(); itr != fiends.end(); ++itr)
                if (auto fiends = (*itr))
                    fiends->ToCreature()->DespawnOrUnsummon();
        }

        void JustDied(Unit* /*killer*/)
        {
            std::list<Unit*> fiends = me->SelectNearbyUnits(NPC_ARCANE_FIENDS, 60.0f);
            for (std::list<Unit*>::iterator itr = fiends.begin(); itr != fiends.end(); ++itr)
                if (auto fiends = (*itr))
                    fiends->ToCreature()->DespawnOrUnsummon();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                ++_killCount;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (!_eventStarted)
            {
                _eventStarted = true;
                _phase = 1;
                _events.ScheduleEvent(EVENT_ATAERIC_1, 1000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_eventStarted)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 7000;
                    me->CastSpell(me, SPELL_SUMMON_ARCANE_FIEND);

                    if (_phase == 1)
                    {
                        if (_killCount == 3)
                        {
                            _killCount = 0;
                            _phase++;
                            _events.ScheduleEvent(EVENT_ATAERIC_2, 0);
                        }
                    }
                    else if (_phase == 2)
                    {
                        if (_killCount == 3)
                        {
                            _killCount = 0;
                            _phase++;
                            _events.ScheduleEvent(EVENT_ATAERIC_3, 0);
                        }
                    }
                    else if (_phase == 3)
                    {
                        if (_killCount == 3)
                        {
                            _killCount = 0;
                            ++_phase;
                            _events.ScheduleEvent(EVENT_ATAERIC_4, 0);
                        }
                    }
                    else if (_phase == 4)
                    {
                        if (_killCount == 3)
                        {
                            _killCount = 0;
                            ++_phase;
                            _events.ScheduleEvent(EVENT_ATAERIC_5, 0);
                        }
                    }

                    else if (_phase = 5)
                    {
                        if (_killCount == 4)
                        {
                            _killCount = 0;
                            _phase = 0;
                            me->AI()->Talk(6);
                            _events.ScheduleEvent(EVENT_ATAERIC_6, 7000);

                            std::list<Unit*> fiends = me->SelectNearbyUnits(NPC_ARCANE_FIENDS, 50.0f);
                            for (std::list<Unit*>::iterator itr = fiends.begin(); itr != fiends.end(); ++itr)
                                if (auto fiends = (*itr))
                                    fiends->ToCreature()->DespawnOrUnsummon();
                        }
                    }

                    else if (!me->FindNearestPlayer(50))
                    {
                        Reset();
                    }
                }
                else
                    _checkTimer -= diff;
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ATAERIC_1:
                    me->AI()->Talk(0);
                    me->AI()->TalkWithDelay(3000, 1);
                    break;
                case EVENT_ATAERIC_2:
                    if (auto portal1 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -135.48f, 1078.66f, 71.92f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        _summons.Summon(portal1);

                    me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
                    me->CastWithDelay(500, me, SPELL_DIMENSIONAL_BEAM);
                    me->AI()->Talk(2);
                    break;
                case EVENT_ATAERIC_3:
                    if (auto portal2 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -124.840f, 1077.902f, 76.219f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        _summons.Summon(portal2);

                    me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
                    me->CastWithDelay(500, me, SPELL_DIMENSIONAL_BEAM);
                    me->AI()->Talk(3);
                    break;
                case EVENT_ATAERIC_4:
                    if (auto portal3 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -145.70f, 1079.026f, 76.218f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        _summons.Summon(portal3);

                    me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
                    me->CastWithDelay(500, me, SPELL_DIMENSIONAL_BEAM);
                    me->AI()->Talk(4);
                    break;
                case EVENT_ATAERIC_5:
                    if (auto portal4 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -154.29f, 1078.265f, 82.811f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto portal5 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -119.86f, 1073.604f, 82.842f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            if (auto portal6 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -154.700f, 1077.630f, 68.606f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                                if (auto portal7 = me->SummonCreature(NPC_TRANSDIMENTIONAL_PORTAL, -115.630f, 1078.355f, 68.604f, 0.f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                                {
                                    _summons.Summon(portal4);
                                    _summons.Summon(portal5);
                                    _summons.Summon(portal6);
                                    _summons.Summon(portal7);
                                }
                    me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
                    me->CastWithDelay(500, me, SPELL_DIMENSIONAL_BEAM);
                    me->AI()->Talk(5);
                    break;
                case EVENT_ATAERIC_6:
                {
                    std::list<Player*> player = me->GetPlayersInRange(100, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(27518) == QUEST_STATUS_INCOMPLETE)
                        {
                            (*it)->KilledMonsterCredit(45769); // credit
                            (*it)->CastSpell(*it, SPELL_SANCTUARY_NO_COMBAT);
                        }
                    me->RemoveAura(SPELL_DIMENSIONAL_BEAM);
                    me->AI()->Talk(7);
                    me->CastSpell(me, SPELL_DIMENSIONAL_COLLAPSE);
                    me->CastSpell(me, SPELL_SUICIDE);
                    break;
                }
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 3000;
        uint8 _phase = 0;
        uint8 _killCount = 0;
        bool _eventStarted = false;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archmage_ataeric_transdimentional_3AI(creature);
    }
};

enum BattlefrontGuardianDatas
{
    SPELL_WALDEN_PENANCE = 54518,
    SPELL_WALDEN_SHIELD = 11974,
    SPELL_WALDEN_INNER_FIRE = 48168,
    SPELL_WALDEN_SMITE = 9734,
    SPELL_WALDEN_HOLYNOVA = 85965,

    SPELL_GODFREY_FIREBALL = 79854,
    SPELL_GODFREY_BLASTWAVE = 79857,
    SPELL_GODFREY_FLAMESTRIKE = 79856,
    SPELL_GODFREY_MOLTEN = 79849,

    SPELL_ASHBURY_FORM = 16592,
    SPELL_ASHBURY_MINDBLAST = 13860,
    SPELL_ASHBURY_PAIN = 589, // debuff
    SPELL_ASHBURY_PSYCHIC = 22884,

    SPELL_PING_MY_SUMMONS = 151318,
    SPELL_PING_MY_SUMMONS_2 = 151319,

    NPC_LORNA_MAJESTY = 45997,
};

// ######################### GODFREY #########################
class npc_godfrey_battlefront_guardian : public CreatureScript
{
public:
    npc_godfrey_battlefront_guardian() : CreatureScript("npc_godfrey_battlefront_guardian") { }

    struct npc_godfrey_battlefront_guardianAI : public ScriptedAI
    {
        npc_godfrey_battlefront_guardianAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_GODFREY_MOLTEN);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_GODFREY_FIREBALL, urand(0, 2000));
                _events.ScheduleEvent(EVENT_GODFREY_BLASTWAVE, urand(4000, 12000));
                _events.ScheduleEvent(EVENT_GODFREY_FLAMESTRIKE, urand(8000, 15000));
            }
            //else
            //    _events.Reset();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_MY_SUMMONS && !_duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    _duringScene = true;
                    me->GetMotionMaster()->Clear();
                    me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                    me->GetMotionMaster()->MovePoint(1, -801.97f, 1342.131f, 33.933037f);

                    if (auto lorna = me->FindNearestCreature(NPC_LORNA_MAJESTY, 60.f))
                    {
                        _lornaGUID = lorna->GetGUID();
                        me->CastSpell(lorna, SPELL_DUMMY_PING_2);
                    }
                }
            }

            if (spellInfo->Id == SPELL_PING_MY_SUMMONS_2 && _duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _duringScene = false;
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.7f * M_PI);
                    }
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                case 1:
                    me->SetFacingTo(4.09f);
                    break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_GODFREY);
                        me->DespawnOrUnsummon(500);

                        if (_duringScene)
                        {
                            if (auto lorna = Unit::GetCreature(*me, _lornaGUID))
                            {
                                lorna->AI()->EnterEvadeMode();
                                lorna->AI()->Reset();
                            }
                        }
                    }

                    if (player->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_REWARDED)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_GODFREY);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_duringScene)
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.7f * M_PI);
                    }

                    if (!_isFollowing && !_duringScene)
                    {
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 0.7f * M_PI);
                        _isFollowing = true;
                    }
                }
            }
            else _checkTimer -= diff;

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GODFREY_FIREBALL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_FIREBALL);
                    _events.ScheduleEvent(EVENT_GODFREY_FIREBALL, urand(3000, 6000));
                    break;
                case EVENT_GODFREY_BLASTWAVE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_BLASTWAVE);
                    _events.ScheduleEvent(EVENT_GODFREY_BLASTWAVE, urand(8000, 15000));
                    break;
                case EVENT_GODFREY_FLAMESTRIKE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_FLAMESTRIKE);
                    _events.ScheduleEvent(EVENT_GODFREY_FLAMESTRIKE, urand(12000, 15000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _duringScene = false;
        bool _isFollowing = false;
        bool _lowHP = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _lornaGUID = 0;
        uint16 _checkTimer = 500;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_godfrey_battlefront_guardianAI(creature);
    }
};

// ######################### WALDEN #########################
class npc_walden_battlefront_guardian : public CreatureScript
{
public:
    npc_walden_battlefront_guardian() : CreatureScript("npc_walden_battlefront_guardian") { }

    struct npc_walden_battlefront_guardianAI : public ScriptedAI
    {
        npc_walden_battlefront_guardianAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->CastSpell(me, SPELL_WALDEN_INNER_FIRE);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_MY_SUMMONS && !_duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    _duringScene = true;
                    me->GetMotionMaster()->Clear();
                    me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                    me->GetMotionMaster()->MovePoint(1, -799.808350f, 1340.446655f, 33.922699f);
                }
            }

            if (spellInfo->Id == SPELL_PING_MY_SUMMONS_2 && _duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _duringScene = false;
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.0f * M_PI);
                    }
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (!_duringScene)
                return;

            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                case 1:
                    me->SetFacingTo(3.51f);
                    break;
                default:
                    break;
                }
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_WALDEN_SMITE, urand(1000, 3000));
                _events.ScheduleEvent(EVENT_WALDEN_HOLYNOVA, urand(4000, 15000));
            }
            //else
            //    _events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_WALDEN);
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_REWARDED)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_WALDEN);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_duringScene)
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.0f * M_PI);
                    }

                    if (!_isFollowing && !_duringScene)
                    {
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.0f * M_PI);
                        _isFollowing = true;
                    }

                    // if healing spell
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetHealthPct() <= 35 && !_lowHP)
                        {
                            _events.Reset();
                            me->CastSpell(player, SPELL_WALDEN_SHIELD);
                            me->CastWithDelay(1000, player, SPELL_WALDEN_PENANCE);
                            _lowHP = true;
                        }

                        if (player->GetHealthPct() >= 36 && _lowHP)
                            _lowHP = false;
                    }
                }
            }
            else _checkTimer -= diff;

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_WALDEN_SMITE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_WALDEN_SMITE);
                    _events.ScheduleEvent(EVENT_WALDEN_SMITE, urand(3000, 6000));
                    break;
                case EVENT_WALDEN_HOLYNOVA:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_WALDEN_HOLYNOVA);
                    _events.ScheduleEvent(EVENT_WALDEN_HOLYNOVA, urand(8000, 12000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _duringScene = false;
        bool _isFollowing = false;
        bool _lowHP = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 500;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_walden_battlefront_guardianAI(creature);
    }
};

// ######################### ASHBURY #########################
class npc_ashbury_battlefront_guardian : public CreatureScript
{
public:
    npc_ashbury_battlefront_guardian() : CreatureScript("npc_ashbury_battlefront_guardian") { }

    struct npc_ashbury_battlefront_guardianAI : public ScriptedAI
    {
        npc_ashbury_battlefront_guardianAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastSpell(me, SPELL_ASHBURY_FORM);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_MY_SUMMONS && !_duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    _duringScene = true;
                    me->GetMotionMaster()->Clear();
                    me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                    me->GetMotionMaster()->MovePoint(1, -804.96f, 1344.01f, 33.87f);
                }
            }

            if (spellInfo->Id == SPELL_PING_MY_SUMMONS_2 && _duringScene)
            {
                if (who->GetGUID() == _playerGUID)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _duringScene = false;
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.3f * M_PI);
                    }
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (!_duringScene)
                return;

            switch (id)
            {
            case 1:
                me->SetFacingTo(4.83f);
                break;
            default:
                break;
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!me->GetOwner()->getVictim())
                me->AI()->AttackStart(attacker);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetReactState() == REACT_AGGRESSIVE && me->isInCombat())
            {
                _events.ScheduleEvent(EVENT_ASHBURY_MINDBLAST, urand(2000, 6000));
                _events.ScheduleEvent(EVENT_ASHBURY_PAIN, urand(1000, 12000));
                _events.ScheduleEvent(EVENT_ASHBURY_PSYCHIC, urand(12000, 18000));
            }
            // else
            //     _events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_ASHBURY);
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_REWARDED)
                    {
                        player->RemoveAura(SPELL_BATTLEFRONT_SUMMON_ASHBURY);
                        me->DespawnOrUnsummon();
                    }

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim() && player->isInCombat())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_duringScene)
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.3f * M_PI);
                    }

                    if (!_isFollowing && !_duringScene)
                    {
                        me->GetMotionMaster()->MoveFollow(player, 0.8f, 1.3f * M_PI);
                        _isFollowing = true;
                    }
                }
            }
            else _checkTimer -= diff;

            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ASHBURY_MINDBLAST:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ASHBURY_MINDBLAST);
                    _events.ScheduleEvent(EVENT_ASHBURY_MINDBLAST, urand(3000, 6000));
                    break;
                case EVENT_ASHBURY_PAIN:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ASHBURY_PAIN);
                    _events.ScheduleEvent(EVENT_ASHBURY_PAIN, urand(30000, 40000));
                    break;
                case EVENT_ASHBURY_PSYCHIC:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ASHBURY_PSYCHIC);
                    _events.ScheduleEvent(EVENT_ASHBURY_PSYCHIC, urand(15000, 20000));
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _duringScene = false;
        bool _isFollowing = false;
        bool _lowHP = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ashbury_battlefront_guardianAI(creature);
    }
};

enum CoweringTropper
{
    SPELL_DUMMY_SPELLCLICK_TROPPER = 85581,
    SPELL_RUN_RANDOMLY = 151111,
    SPELL_EXECUTION = 85584,
    NPC_FIND_GODFREY = 45878,
    NPC_COWERING_CREDIT = 45910,
};

class npc_cowering_trooper : public CreatureScript
{
public:
    npc_cowering_trooper() : CreatureScript("npc_cowering_trooper") { }

    struct npc_cowering_trooperAI : public ScriptedAI
    {
        npc_cowering_trooperAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();

                    uint8 result = 0;
                    result = urand(1, 3);

                    switch (result)
                    {
                    case 1:
                    case 2:
                        _events.ScheduleEvent(EVENT_COWERINGTROPPER_1, 0);
                        break;
                    case 3:
                        _events.ScheduleEvent(EVENT_COWERINGTROPPER_2, 0);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        void JustRespawned()
        {
            me->CastSpell(me, 91166);
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_COWERINGTROPPER_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->SetFacingToObject(player);
                        me->RemoveAura(91166); // cower emote
                        player->KilledMonsterCredit(NPC_COWERING_CREDIT);
                        me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                        me->CastWithDelay(3000, me, SPELL_RUN_RANDOMLY);
                        me->DespawnOrUnsummon(8000);

                        if (auto godfrey = me->FindNearestCreature(NPC_FIND_GODFREY, 40.f))
                        {
                            godfrey->AI()->TalkWithDelay(4000, 0);
                        }
                    }
                    break;
                case EVENT_COWERINGTROPPER_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (auto godfrey = me->FindNearestCreature(NPC_FIND_GODFREY, 25.f))
                        {
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->SetFacingToObject(player);
                            me->RemoveAura(91166); // cower emote
                            me->AI()->TalkWithDelay(1000, 0, _playerGUID);

                            _events.ScheduleEvent(EVENT_COWERINGTROPPER_3, 2000);
                        }
                        else
                        {
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->SetFacingToObject(player);
                            me->RemoveAura(91166); // cower emote
                            me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                            me->CastWithDelay(3000, me, SPELL_RUN_RANDOMLY);
                            me->DespawnOrUnsummon(8000);
                        }
                    }
                    break;
                case EVENT_COWERINGTROPPER_3:
                    if (auto godfrey = me->FindNearestCreature(NPC_FIND_GODFREY, 30.f))
                    {
                        godfrey->SetFacingToObject(me);
                        godfrey->CastWithDelay(500, me, SPELL_EXECUTION);
                        godfrey->AI()->TalkWithDelay(500, 1);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cowering_trooperAI(creature);
    }
};

enum LornaMajesty
{
    SPELL_LORNA_CLEAVE = 85240,
    SPELL_LORNA_THUNDERCLAP = 81140,
    SPELL_LORNA_CHARGE = 22120,

};

class npc_lorna_majesty : public CreatureScript
{
public:
    npc_lorna_majesty() : CreatureScript("npc_lorna_majesty") { }

    struct npc_lorna_majestyAI : public ScriptedAI
    {
        npc_lorna_majestyAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            _godfreyGUID = 0;
            _playerGUID = 0;
            me->RemoveAura(29266);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_activated)
                if (me->IsWithinDistInMap(who, 10.0f))
                    if (who->GetTypeId() == TYPEID_PLAYER)
                        if (auto player = who->ToPlayer())
                            if (!player->isInCombat() && player->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_INCOMPLETE)
                                if (auto godfrey = who->FindNearestCreature(45878, 15))
                                    if (auto walden = who->FindNearestCreature(45879, 15))
                                        if (auto ashbury = who->FindNearestCreature(45880, 15))
                                        {
                                            _playerGUID = player->GetGUID();
                                            player->CastSpell(player, SPELL_PING_MY_SUMMONS);
                                            _activated = true;
                                            me->AI()->Talk(0);
                                        }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING && _activated)
            {
                _activated = false;
                Reset();
                me->AI()->EnterEvadeMode();
                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                _events.Reset();
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                if (who->GetEntry() == NPC_FIND_GODFREY)
                    if (auto godfrey = who->ToCreature())
                    {
                        _godfreyGUID = godfrey->GetGUID();
                        _events.ScheduleEvent(EVENT_MAJESTY_1, 3500);
                    }
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_LORNA_CHARGE, 0);
            _events.ScheduleEvent(EVENT_LORNA_THUNDERCLAP, urand(4000, 11000));
            _events.ScheduleEvent(EVENT_LORNA_CLEAVE, urand(2000, 8000));
        }

        void JustDied(Unit* /* killer */)
        {
            me->AI()->Talk(4);

            std::list<Player*> player = me->GetPlayersInRange(80, true);
            for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_ON_HER_MAJESTY) == QUEST_STATUS_INCOMPLETE)
                    (*it)->KilledMonsterCredit(46002); // credit

            me->CastSpell(me, 29266);
            _activated = false;
            _events.Reset();

            if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
            {
                godfrey->AI()->TalkWithDelay(2000, 6);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff && _activated)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        _events.Reset();
                        me->AI()->EnterEvadeMode();
                        Reset();
                        _activated = false;
                        me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LORNA_CHARGE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LORNA_CHARGE);
                    break;
                case EVENT_LORNA_THUNDERCLAP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LORNA_THUNDERCLAP);
                    _events.ScheduleEvent(EVENT_LORNA_THUNDERCLAP, urand(15000, 28000));
                    break;
                case EVENT_LORNA_CLEAVE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_LORNA_CLEAVE);
                    _events.ScheduleEvent(EVENT_LORNA_CLEAVE, urand(20000, 25000));
                    break;
                case EVENT_MAJESTY_1:
                    if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                    {
                        godfrey->AI()->TalkWithDelay(1000, 2);
                        me->AI()->TalkWithDelay(8000, 1);
                        me->AI()->TalkWithDelay(13000, 2);
                        godfrey->AI()->TalkWithDelay(19500, 3);
                        godfrey->AI()->TalkWithDelay(24500, 4);
                        godfrey->AI()->TalkWithDelay(34500, 5);
                        me->AI()->TalkWithDelay(40000, 3);
                        me->CastWithDelay(42000, me, 80983);
                        _events.ScheduleEvent(EVENT_MAJESTY_2, 44000);
                    }
                    break;
                case EVENT_MAJESTY_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->AI()->AttackStart(player);
                        player->CastSpell(player, SPELL_PING_MY_SUMMONS_2);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _activated = false;
        uint64 _godfreyGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        uint16 _checkTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lorna_majestyAI(creature);
    }
};

enum TripNorthSouth
{
    NPC_ETTIN_1 = 44737,
    NPC_ENGINEER = 44734,
};

uint32 const NorthHaulerPathSize = 5;
const G3D::Vector3 NorthHaulerPath[NorthHaulerPathSize] =
{
    { 1449.092f, 667.261f, 47.12f, },
    { 1404.983f, 743.529f, 47.12f, },
    { 1371.504f, 826.372f, 48.67f, },
    { 1342.453f, 916.557f, 54.59f, },
    { 1325.742f, 1000.96f, 54.59f, },
};

uint32 const NorthHaulerPathSize2 = 16;
const G3D::Vector3 NorthHaulerPath2[NorthHaulerPathSize2] =
{
    { 1315.39f, 1027.269f, 54.600f, }, // 0 
    { 1288.42f, 1068.989f, 53.618f, }, // 1 
    { 1267.22f, 1095.244f, 51.750f, }, // 2 
    { 1225.00f, 1130.903f, 50.475f, }, // 3 
    { 1168.57f, 1169.004f, 48.617f, }, // 4 
    { 1134.93f, 1191.119f, 47.752f, }, // 5 
    { 1063.47f, 1249.424f, 46.168f, }, // 6 
    { 1028.99f, 1281.528f, 45.980f, }, // 7 
    { 971.798f, 1315.444f, 45.871f, }, // 8 
    { 914.948f, 1349.042f, 48.216f, }, // 9 
    { 865.714f, 1360.332f, 54.587f, }, // 10
    { 811.999f, 1360.771f, 57.923f, }, // 11
    { 754.691f, 1362.252f, 68.465f  }, // 12
    { 707.478f, 1342.741f, 76.237f, }, // 13
    { 675.207f, 1313.741f, 80.521f, }, // 14
    { 642.890f, 1299.860f, 85.270f, }, // 15
};

uint32 const NorthHaulerPathSize3 = 17;
const G3D::Vector3 NorthHaulerPath3[NorthHaulerPathSize3] =
{
    { 613.569f, 1288.47f, 87.504f, }, // 0 
    { 577.252f, 1251.61f, 86.69f, }, // 1 
    { 542.998f, 1231.21f, 87.57f, }, // 2 
    { 518.041f, 1228.45f, 89.23f, }, // 3 
    { 453.707f, 1227.94f, 88.06f, }, // 4 
    { 410.817f, 1232.16f, 84.21f, }, // 5 
    { 348.807f, 1241.29f, 81.26f, }, // 6 
    { 297.376f, 1259.26f, 78.32f, }, // 7 
    { 265.622f, 1268.52f, 76.82f, }, // 8 
    { 233.164f, 1267.91f, 75.74f, }, // 9 
    { 195.804f, 1259.72f, 73.50f, }, // 10
    { 138.925f, 1241.98f, 71.04f, }, // 11
    { 57.004f, 1219.25f, 66.32f, }, // 12
    { -10.992f, 1197.12f, 64.49f, }, // 13
    { -65.884f, 1181.82f, 63.95f, }, // 14
    { -101.01f, 1212.72f, 60.26f, }, // 15
    { -148.72f, 1236.33f, 52.56f, }, // 16
};

class npc_horde_hauler_north_south : public CreatureScript
{
public:
    npc_horde_hauler_north_south() : CreatureScript("npc_horde_hauler_north_south") { }

    struct npc_horde_hauler_north_southAI : public VehicleAI
    {
        npc_horde_hauler_north_southAI(Creature* creature) : VehicleAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            _events.Reset();
            _events.ScheduleEvent(EVENT_HAULER_NORTH_1, 5000);

            if (auto ettin = me->SummonCreature(NPC_ETTIN_1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                if (auto engineer = me->SummonCreature(NPC_ENGINEER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    ettin->CastWithDelay(1000, me, SPELL_RIDE_VEHICLE_SEAT_02);
                    _ettinGUID = ettin->GetGUID();
                    engineer->CastWithDelay(1000, me, SPELL_RIDE_VEHICLE);
                    _engineerGUID = engineer->GetGUID();
                    engineer->CastWithDelay(2000, me, 83464);
                    engineer->CastWithDelay(2000, me, 83467);
                }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* */seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->AI()->Talk(0, passenger->GetGUID());

                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(3, passenger->GetGUID());
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_tripPhase == 1 && point == 4)
            {
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(0);
                    _events.ScheduleEvent(EVENT_HAULER_NORTH_2, 15000);
                }
            }
            else if (_tripPhase == 2 && point == 15)
            {
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(1);
                    _events.ScheduleEvent(EVENT_HAULER_NORTH_3, 15000);
                }
            }
            else if (_tripPhase == 3 && point == 16)
            {
                _tripPhase = 0;
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_HAULER_NORTH_4, 6000);
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HAULER_NORTH_1:
                    me->GetMotionMaster()->MoveSmoothPath(NorthHaulerPath, NorthHaulerPathSize);
                    _tripStarted = true;
                    _tripPhase = 1;
                    break;
                case EVENT_HAULER_NORTH_2:
                    me->GetMotionMaster()->MoveSmoothPath(NorthHaulerPath2, NorthHaulerPathSize2);
                    _tripPhase++;
                    break;
                case EVENT_HAULER_NORTH_3:
                    me->GetMotionMaster()->MoveSmoothPath(NorthHaulerPath3, NorthHaulerPathSize3);
                    _tripPhase++;
                    break;
                case EVENT_HAULER_NORTH_4:
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    _tripStarted = false;

                    if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                    {
                        if (auto ettin = Unit::GetCreature(*me, _ettinGUID))
                        {
                            engineer->DespawnOrUnsummon();
                            ettin->DespawnOrUnsummon();
                            me->DespawnOrUnsummon();
                            _tripPhase = 0;
                            _ettinGUID = 0;
                            _engineerGUID = 0;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _tripPhase = 0;
        bool _tripStarted = false;
        uint64 _ettinGUID = 0;
        uint64 _engineerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horde_hauler_north_southAI(creature);
    }
};

enum TripSouthNorth
{
    NPC_ETTIN_2 = 46560,
    NPC_ENGINEER_2 = 46559,
};

uint32 const SouthHaulerPathSize = 2;
const G3D::Vector3 SouthHaulerPath[SouthHaulerPathSize] =
{
    { -162.75f, 1177.502f, 64.87f, },
    { -90.917f, 1177.183f, 63.82f, },
};

uint32 const SouthHaulerPathSize2 = 15;
const G3D::Vector3 SouthHaulerPath2[SouthHaulerPathSize2] =
{
    { -65.884f, 1181.82f, 63.95f, }, // 0 
    { -10.992f, 1197.12f, 64.49f, }, // 1 
    { 57.004f, 1219.25f, 66.32f, },  // 2 
    { 138.925f, 1241.98f, 71.04f, }, // 3 
    { 195.804f, 1259.72f, 73.50f, }, // 4 
    { 233.164f, 1267.91f, 75.74f, }, // 5 
    { 265.622f, 1268.52f, 76.82f, }, // 6 
    { 297.376f, 1259.26f, 78.32f, }, // 7 
    { 348.807f, 1241.29f, 81.26f, }, // 8 
    { 410.817f, 1232.16f, 84.21f, }, // 9 
    { 453.707f, 1227.94f, 88.06f, }, // 10
    { 518.041f, 1228.45f, 89.23f, }, // 11
    { 542.998f, 1231.21f, 87.57f, }, // 12
    { 577.252f, 1251.61f, 86.69f, }, // 13
    { 618.968f, 1287.34f, 87.08f, }, // 14
};

uint32 const SouthHaulerPathSize3 = 16;
const G3D::Vector3 SouthHaulerPath3[SouthHaulerPathSize3] =
{
    { 642.890f, 1299.860f, 85.270f, }, // 
    { 675.207f, 1313.741f, 80.521f, }, // 
    { 707.478f, 1342.741f, 76.237f, }, // 
    { 754.691f, 1362.252f, 68.465f  }, // 
    { 811.999f, 1360.771f, 57.923f, }, // 
    { 865.714f, 1360.332f, 54.587f, }, // 
    { 914.948f, 1349.042f, 48.216f, }, // 
    { 971.798f, 1315.444f, 45.871f, }, // 
    { 1028.99f, 1281.528f, 45.980f, }, // 
    { 1063.47f, 1249.424f, 46.168f, }, // 
    { 1134.93f, 1191.119f, 47.752f, }, // 
    { 1168.57f, 1169.004f, 48.617f, }, // 
    { 1225.00f, 1130.903f, 50.475f, }, // 
    { 1267.22f, 1095.244f, 51.750f, }, // 
    { 1288.42f, 1068.989f, 53.618f, }, // 
    { 1315.39f, 1027.269f, 54.600f, }, // 

};

class npc_horde_hauler_south_north : public CreatureScript
{
public:
    npc_horde_hauler_south_north() : CreatureScript("npc_horde_hauler_south_north") { }

    struct npc_horde_hauler_south_northAI : public VehicleAI
    {
        npc_horde_hauler_south_northAI(Creature* creature) : VehicleAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            _events.Reset();
            _events.ScheduleEvent(EVENT_HAULER_SOUTH_1, 5000);

            if (auto ettin = me->SummonCreature(NPC_ETTIN_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                if (auto engineer = me->SummonCreature(NPC_ENGINEER_2, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    ettin->CastWithDelay(1000, me, SPELL_RIDE_VEHICLE_SEAT_02);
                    _ettinGUID = ettin->GetGUID();
                    engineer->CastWithDelay(1000, me, SPELL_RIDE_VEHICLE);
                    _engineerGUID = engineer->GetGUID();
                    engineer->CastWithDelay(2000, me, 83464);
                    engineer->CastWithDelay(2000, me, 83467);
                }
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* */seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->AI()->Talk(0, passenger->GetGUID());

                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(3, passenger->GetGUID());
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_tripPhase == 1 && point == 1)
            {
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(0);
                    _events.ScheduleEvent(EVENT_HAULER_SOUTH_2, 15000);
                }
            }
            else if (_tripPhase == 2 && point == 14)
            {
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(1);
                    _events.ScheduleEvent(EVENT_HAULER_SOUTH_3, 15000);
                }
            }
            else if (_tripPhase == 3 && point == 15)
            {
                _tripPhase = 0;
                if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                {
                    engineer->AI()->Talk(2);
                    _events.ScheduleEvent(EVENT_HAULER_SOUTH_4, 6000);
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HAULER_SOUTH_1:
                    me->GetMotionMaster()->MoveSmoothPath(SouthHaulerPath, SouthHaulerPathSize);
                    _tripStarted = true;
                    _tripPhase = 1;
                    break;
                case EVENT_HAULER_SOUTH_2:
                    me->GetMotionMaster()->MoveSmoothPath(SouthHaulerPath2, SouthHaulerPathSize2);
                    _tripPhase++;
                    break;
                case EVENT_HAULER_SOUTH_3:
                    me->GetMotionMaster()->MoveSmoothPath(SouthHaulerPath3, SouthHaulerPathSize3);
                    _tripPhase++;
                    break;
                case EVENT_HAULER_SOUTH_4:
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    _tripStarted = false;

                    if (auto engineer = Unit::GetCreature(*me, _engineerGUID))
                    {
                        if (auto ettin = Unit::GetCreature(*me, _ettinGUID))
                        {
                            engineer->DespawnOrUnsummon();
                            ettin->DespawnOrUnsummon();
                            me->DespawnOrUnsummon();
                            _tripPhase = 0;
                            _ettinGUID = 0;
                            _engineerGUID = 0;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _tripPhase = 0;
        bool _tripStarted = false;
        uint64 _ettinGUID = 0;
        uint64 _engineerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horde_hauler_south_northAI(creature);
    }
};

enum MutantChicken
{
    NPC_FOREST_ETTIN = 44367,
    SPELL_CHICKEN_EXPLOSION = 83903,
};

class npc_mutant_bush_chicken : public CreatureScript
{
public:
    npc_mutant_bush_chicken() : CreatureScript("npc_mutant_bush_chicken") { }

    struct npc_mutant_bush_chickenAI : public ScriptedAI
    {
        npc_mutant_bush_chickenAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetWalk(false);
            me->SetReactState(REACT_PASSIVE);

            _events.ScheduleEvent(EVENT_MUTANT_CHICKEN_1, 3000);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto ettin = Unit::GetCreature(*me, _ettinGUID))
                {
                    _foundEttin = true;
                    me->CastSpell(ettin, SPELL_RIDE_VEHICLE);
                    me->CastWithDelay(3000, me, SPELL_CHICKEN_EXPLOSION);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (point = 1 && !_foundEttin)
            {
                if (auto ettin = me->FindNearestCreature(NPC_FOREST_ETTIN, 70.f))
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(1, ettin->GetPositionX(), ettin->GetPositionY(), ettin->GetPositionZ(), true);
                }
                else
                    me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MUTANT_CHICKEN_1:
                    if (auto ettin = me->FindNearestCreature(NPC_FOREST_ETTIN, 70.f))
                    {
                        _ettinGUID = ettin->GetGUID();
                        me->GetMotionMaster()->MovePoint(1, ettin->GetPositionX(), ettin->GetPositionY(), ettin->GetPositionZ(), true);
                    }
                    else
                        me->DespawnOrUnsummon();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _foundEttin = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _ettinGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutant_bush_chickenAI(creature);
    }
};

/*###### FINAL QUEST SCRIPTS ##########
####### PAIN IS THE ASS HEHE ##########
####### GREETINGS - WONDI ###########*/

enum CitiesInDust
{
    NPC_FINAL_SYLVANAS = 46026,
    NPC_FINAL_CROMUSH = 46031,
    NPC_FINAL_ARTHURA = 46032,
    NPC_FINAL_AGATHA = 46034,
    NPC_FINAL_DASCHLA = 46033,
    NPC_FINAL_CROWLEY = 46036,
    NPC_FINAL_BLOODFANG = 46035,
    NPC_FINAL_GODFREY = 46028,
    NPC_FINAL_WALDEN = 46029,
    NPC_FINAL_ASHBURY = 46030,

    NPC_FINAL_LORNA = 46027,
    NPC_TARGET_BUNNY = 75042,

    QUEST_CITIES_IN_DUST = 27601,

    SPELL_SONG_OF_SYLVANAS = 85874,
    SPELL_FINAL_QUEST_CREDIT = 85952,
    SPELL_FINAL_RAISE_FORSAKEN = 85950,

    SPELL_AURA_SUMMON_SYLVANAS = 85864,
    SPELL_AURA_SUMMON_CROMUSH = 85868,
    SPELL_AURA_SUMMON_ARTHURA = 85869,
    SPELL_AURA_SUMMON_AGATHA = 85870,
    SPELL_AURA_SUMMON_DASCHLA = 85871,

    SPELL_AURA_SUMMON_CROWLEY = 85877,
    SPELL_AURA_SUMMON_BLOODFANG = 85878,

    SPELL_AURA_SUMMON_GODFREY = 85925,
    SPELL_AURA_SUMMON_WALDEN = 85926,
    SPELL_AURA_SUMMON_ASBHURY = 85927,

    SPELL_PING_ACTORS_1 = 151332,
    SPELL_PING_ACTORS_DESPAWN = 151333,
    SPELL_PING_ACTORS_2 = 151334,
    SPELL_PING_ACTORS_ENEMIES = 151335,
    SPELL_ATTACK_ME = 53799,
    SPELL_GODFREY_SHOOT = 74279,
    SPELL_PERMANENT_DEATH_ANIM = 29266,
    SPELL_TELEPORT_EFFECT = 52096,

    SPELL_THUNDERCLAP = 33967,
    SPELL_MORTAL_STRIKE = 32736,
};

class npc_sylvanas_cities_in_dust : public CreatureScript
{
public:
    npc_sylvanas_cities_in_dust() : CreatureScript("npc_sylvanas_cities_in_dust") { }

    struct npc_sylvanas_cities_in_dustAI : public npc_escortAI
    {
        npc_sylvanas_cities_in_dustAI(Creature* creature) : npc_escortAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetPhaseMask(2048, false);

            _playerGUID = me->GetOwnerGUID();

            _isClose = true;
            _sylvanasGUID = me->GetGUID();
            me->CastWithDelay(100, me, SPELL_SONG_OF_SYLVANAS);
            me->CastWithDelay(100, me, SPELL_ROOT);
            me->NearTeleportTo(-153.324f, 1270.27f, 51.54f, 3.071f); // orginal
            _events.ScheduleEvent(EVENT_FINAL_SCENE_1, 1000);
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 12:
                me->AI()->TalkWithDelay(1000, 2, _playerGUID);
                me->AI()->TalkWithDelay(5500, 3, _playerGUID);
                me->AI()->TalkWithDelay(11000, 4, _playerGUID);
                me->AI()->TalkWithDelay(19000, 5, _playerGUID);
                break;
            case 25: // original 25
                _isClose = false;
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING) // ping from lorna
            {
                _lornaGUID = who->GetGUID();
                _events.ScheduleEvent(EVENT_FINAL_SCENE_12, 0);
            }

            if (spellInfo->Id == SPELL_GODFREY_SHOOT)
            {
                me->CastSpell(me, SPELL_PING_ACTORS_2);
                me->Dismount();
                me->AI()->Talk(19);
                me->CastSpell(me, SPELL_PERMANENT_DEATH_ANIM);

                if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                    me->CastSpell(godfrey, SPELL_DUMMY_PING_2);
            }

            if (spellInfo->Id == SPELL_PING_ACTORS_DESPAWN)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->RemoveAura(SPELL_AURA_SUMMON_SYLVANAS);
                    me->DespawnOrUnsummon();
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_3)
            {
                _events.ScheduleEvent(EVENT_FINAL_SCENE_26, 1000);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 50.0f))
            {
                if (_sceneInProgress)
                {
                    if (auto potential = me->FindNearestPlayer(50))
                    {
                        if (potential->GetGUID() == _playerGUID)
                            return;
                        else
                        {
                            potential->CastSpell(potential, SPELL_PING_ACTORS_DESPAWN);
                        }
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 500;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetEntry() == NPC_FINAL_SYLVANAS)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_SYLVANAS);
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_SYLVANAS);
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon();
                        }

                        if (!_isClose)
                        {
                            if (auto owner = me->FindNearestPlayer(10))
                            {
                                if (owner->GetGUID() == _playerGUID)
                                {
                                    player->CastSpell(player, SPELL_PING_ACTORS_1);
                                    _isClose = true;
                                    _events.ScheduleEvent(EVENT_FINAL_SCENE_4, 3000);
                                }
                            }
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);
            npc_escortAI::UpdateEscortAI(diff);
            npc_escortAI::UpdateAI(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FINAL_SCENE_1:
                    me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_2, 3500);
                    break;
                case EVENT_FINAL_SCENE_2:
                    me->AI()->Talk(1, _playerGUID);
                    me->RemoveAura(SPELL_ROOT);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_3, 1000);
                    break;
                case EVENT_FINAL_SCENE_3:
                    Start(false, true, _playerGUID);
                    break;
                case EVENT_FINAL_SCENE_4:
                    me->AI()->Talk(6);

                    if (auto crowley = me->SummonCreature(NPC_FINAL_CROWLEY, -745.57f, 1551.27f, 28.01f, 6.26f, TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto bloodfang = me->SummonCreature(NPC_FINAL_BLOODFANG, -745.63f, 1547.79f, 28.01f, 6.25f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            _crowleyGUID = crowley->GetGUID();
                            _bloodfangGUID = bloodfang->GetGUID();
                            crowley->SetWalk(true);
                            bloodfang->SetWalk(true);
                            _summons.Summon(crowley);
                            _summons.Summon(bloodfang);
                            crowley->GetMotionMaster()->MovePoint(1, -710.67f, 1550.983f, 28.02f);
                            bloodfang->GetMotionMaster()->MovePoint(1, -711.01f, 1547.964f, 28.02f);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_5, 12000);
                        }
                    break;
                case EVENT_FINAL_SCENE_5:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto bloodfang = Unit::GetCreature(*me, _bloodfangGUID))
                        {
                            crowley->AI()->Talk(0);
                            me->AI()->TalkWithDelay(7000, 7);
                            me->AI()->TalkWithDelay(17000, 8);
                            crowley->AI()->TalkWithDelay(25000, 1);
                            me->AI()->TalkWithDelay(30000, 9);
                            crowley->AI()->TalkWithDelay(40000, 2);
                            me->AI()->TalkWithDelay(46000, 10);
                            me->AI()->TalkWithDelay(50000, 11);
                            me->AI()->TalkWithDelay(57000, 12);
                            me->AI()->TalkWithDelay(64500, 13);
                            me->AI()->TalkWithDelay(71500, 14);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_6, 75500);
                        }
                    break;
                case EVENT_FINAL_SCENE_6:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto godfrey = me->SummonCreature(NPC_FINAL_GODFREY, -702.303f, 1513.114f, 28.755f, 1.66f, TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto walden = me->SummonCreature(NPC_FINAL_WALDEN, -704.783f, 1509.923f, 28.723f, 1.66f, TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto ashbury = me->SummonCreature(NPC_FINAL_ASHBURY, -699.660f, 1510.400f, 28.594f, 1.66f, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    _godfreyGUID = godfrey->GetGUID();
                                    _waldenGUID = walden->GetGUID();
                                    _ashburyGUID = ashbury->GetGUID();
                                    _summons.Summon(godfrey);
                                    _summons.Summon(walden);
                                    _summons.Summon(ashbury);
                                    godfrey->SetWalk(true);
                                    walden->SetWalk(true);
                                    ashbury->SetWalk(true);
                                    godfrey->GetMotionMaster()->MovePoint(1, -705.148f, 1535.31f, 28.47f);
                                    walden->GetMotionMaster()->MovePoint(1, -707.136f, 1531.97f, 28.91f);
                                    ashbury->GetMotionMaster()->MovePoint(1, -702.256f, 1533.11f, 28.74f);

                                    crowley->SetFacingTo(5.13f);
                                    crowley->AI()->TalkWithDelay(1500, 3);
                                    _events.ScheduleEvent(EVENT_FINAL_SCENE_7, 6000);
                                }
                    break;
                case EVENT_FINAL_SCENE_7:
                    if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                    {
                        godfrey->AI()->Talk(0);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_8, 4000);
                    }
                    break;
                case EVENT_FINAL_SCENE_8:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                    {
                        crowley->SetFacingTo(6.04f);
                        crowley->AI()->TalkWithDelay(1500, 4);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_9, 3500);
                    }
                    break;
                case EVENT_FINAL_SCENE_9:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto bloodfang = Unit::GetCreature(*me, _bloodfangGUID))
                        {
                            bloodfang->SetFacingToObject(crowley);
                            bloodfang->AI()->TalkWithDelay(1500, 0);
                            me->AI()->TalkWithDelay(4500, 15);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_10, 7000);
                        }
                    break;
                case EVENT_FINAL_SCENE_10:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto bloodfang = Unit::GetCreature(*me, _bloodfangGUID))
                            if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                            {
                                bloodfang->SetWalk(false);
                                bloodfang->GetMotionMaster()->MovePoint(2, -745.63f, 1547.79f, 28.01f);
                                bloodfang->DespawnOrUnsummon(3000);

                                godfrey->AI()->Talk(1);
                                me->CastSpell(godfrey, SPELL_DUMMY_PING);
                            }
                    break;
                case EVENT_FINAL_SCENE_12:
                    me->AI()->Talk(16);
                    me->AI()->TalkWithDelay(5000, 17);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_13, 10000);
                    break;
                case EVENT_FINAL_SCENE_13:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto lorna = Unit::GetCreature(*me, _lornaGUID))
                        {
                            crowley->SetFacingToObject(lorna);
                            crowley->AI()->TalkWithDelay(1000, 5);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_14, 5000);
                        }
                    break;
                case EVENT_FINAL_SCENE_14:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        if (auto lorna = Unit::GetCreature(*me, _lornaGUID))
                        {
                            lorna->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                            lorna->GetMotionMaster()->MovePoint(2, -750.377f, 1544.63f, 28.014f);
                            lorna->DespawnOrUnsummon(4000);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_15, 3000);
                        }
                    break;
                case EVENT_FINAL_SCENE_15:
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                    {
                        crowley->SetWalk(false);
                        crowley->GetMotionMaster()->MovePoint(2, -749.9f, 1549.09f, 28.01f);
                        crowley->DespawnOrUnsummon(4000);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_16, 7000);
                    }
                    break;
                case EVENT_FINAL_SCENE_16:
                    if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                        if (auto walden = Unit::GetCreature(*me, _waldenGUID))
                            if (auto ashbury = Unit::GetCreature(*me, _ashburyGUID))
                            {
                                me->AI()->Talk(18);

                                walden->SetWalk(false);
                                walden->setFaction(14);
                                walden->GetMotionMaster()->MovePoint(2, -696.88f, 1542.78f, 27.98f);
                                ashbury->SetWalk(false);
                                ashbury->setFaction(14);
                                ashbury->GetMotionMaster()->MovePoint(2, -694.49f, 1543.94f, 27.89f);
                                godfrey->SetWalk(false);
                                godfrey->setFaction(14);
                                godfrey->GetMotionMaster()->MovePoint(2, -692.46f, 1546.36f, 27.81f);
                                _events.ScheduleEvent(EVENT_FINAL_SCENE_17, 3000);
                            }
                    break;
                case EVENT_FINAL_SCENE_17:
                    if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                        if (auto walden = Unit::GetCreature(*me, _waldenGUID))
                            if (auto ashbury = Unit::GetCreature(*me, _ashburyGUID))
                            {
                                godfrey->SetSheath(SHEATH_STATE_RANGED);
                                godfrey->HandleEmoteCommand(EMOTE_STATE_READY_RIFLE);
                                godfrey->CastWithDelay(1500, me, SPELL_GODFREY_SHOOT);
                            }
                    break;
                case EVENT_FINAL_SCENE_26:
                    me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                    me->AI()->TalkWithDelay(2500, 21);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_27, 9500);
                    break;
                case EVENT_FINAL_SCENE_27:
                    if (auto cromush = me->FindNearestCreature(NPC_FINAL_CROMUSH, 60.0f))
                    {
                        me->SetFacingToObject(cromush);
                        me->AI()->TalkWithDelay(1000, 22);
                        me->AI()->TalkWithDelay(7000, 23);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_28, 12000);
                    }
                    break;
                case EVENT_FINAL_SCENE_28:
                {
                    std::list<Player*> character = me->GetPlayersInRange(70, true);
                    for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_INCOMPLETE)
                        {
                            (*it)->CastSpell(*it, SPELL_PING_ACTORS_DESPAWN);
                            (*it)->KilledMonsterCredit(46019);
                            _sceneInProgress = false;
                        }
                    break;
                }
                default:
                    break;
                }
            }
        }

    private:
        bool _sceneInProgress = false;
        bool _isClose = true;
        EventMap _events;
        SummonList _summons;
        uint64 _playerGUID = NULL;
        uint64 _sylvanasGUID = NULL;
        uint64 _crowleyGUID = NULL;
        uint64 _bloodfangGUID = NULL;
        uint64 _godfreyGUID = NULL;
        uint64 _waldenGUID = NULL;
        uint64 _ashburyGUID = NULL;
        uint64 _lornaGUID = NULL;
        uint64 _cromushGUID = NULL;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sylvanas_cities_in_dustAI(creature);
    }
};

class npc_cities_in_dust_enemies : public CreatureScript
{
public:
    npc_cities_in_dust_enemies() : CreatureScript("npc_cities_in_dust_enemies") { }

    struct npc_cities_in_dust_enemiesAI : public ScriptedAI
    {
        npc_cities_in_dust_enemiesAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (summoner->GetEntry() == NPC_FINAL_SYLVANAS)
                _sylvanasGUID = summoner->GetGUID();

            if (me->GetEntry() == NPC_FINAL_GODFREY)
            {
                _godfreyGUID = me->GetGUID();

                if (auto lorna = me->SummonCreature(NPC_FINAL_LORNA, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _lornaGUID = lorna->GetGUID();
                    lorna->EnterVehicle(me, 0);
                }
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;

            if ((me->GetHealthPct() <= 10) && _fightInProgress)
            {
                if (me->GetEntry() == NPC_FINAL_GODFREY)
                {
                    _fightInProgress = false;
                    _events.ScheduleEvent(EVENT_FINAL_LOW_HP, 0);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (point == 2)
            {
                _events.ScheduleEvent(EVENT_FINAL_SCENE_FACING_2, 100);
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetEntry() == NPC_FINAL_GODFREY)
            {
                _events.ScheduleEvent(EVENT_GODFREY_FIREBALL, urand(0, 2000));
                _events.ScheduleEvent(EVENT_GODFREY_BLASTWAVE, urand(4000, 12000));
                _events.ScheduleEvent(EVENT_GODFREY_FLAMESTRIKE, urand(8000, 15000));
            }

            if (me->GetEntry() == NPC_FINAL_WALDEN)
            {
                _events.ScheduleEvent(EVENT_WALDEN_SMITE, urand(1000, 3000));
                _events.ScheduleEvent(EVENT_WALDEN_HOLYNOVA, urand(4000, 15000));
            }

            if (me->GetEntry() == NPC_FINAL_ASHBURY)
            {
                _events.ScheduleEvent(EVENT_ASHBURY_MINDBLAST, urand(2000, 6000));
                _events.ScheduleEvent(EVENT_ASHBURY_PAIN, urand(1000, 12000));
                _events.ScheduleEvent(EVENT_ASHBURY_PSYCHIC, urand(12000, 18000));
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto lorna = me->GetVehicleKit()->GetPassenger(0))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    lorna->GetMotionMaster()->MoveJump(-706.89f, 1536.98f, 28.27f, 8, 12);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_11, 1000);
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2)
                _events.ScheduleEvent(EVENT_FINAL_SCENE_18, 6000);


            if (_fightInProgress)
            {
                if (spellInfo->Id == SPELL_PING_ACTORS_ENEMIES)
                {
                    me->DespawnOrUnsummon();
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FINAL_SCENE_11:
                    if (auto lorna = Unit::GetCreature(*me, _lornaGUID))
                    {
                        lorna->SetWalk(false);
                        lorna->GetMotionMaster()->MovePoint(1, -709.36f, 1547.89f, 28.01f);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_12, 1500);
                    }
                    break;
                case EVENT_FINAL_SCENE_12:
                    if (auto lorna = Unit::GetCreature(*me, _lornaGUID))
                        if (auto sylv = Unit::GetCreature(*me, _sylvanasGUID))
                        {
                            lorna->CastSpell(sylv, SPELL_DUMMY_PING);
                        }
                    break;
                case EVENT_FINAL_SCENE_FACING_2:
                    if (auto target = me->FindNearestCreature(NPC_FINAL_SYLVANAS, 60.0f))
                    {
                        me->SetFacingToObject(target);
                        me->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                    }
                    break;
                case EVENT_FINAL_SCENE_18:
                    me->AI()->Talk(2);
                    me->AI()->TalkWithDelay(6000, 3);
                    me->AI()->TalkWithDelay(11000, 4);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_19, 11100);
                    break;
                case EVENT_FINAL_SCENE_19:
                    _fightInProgress = true;
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetReactState(REACT_AGGRESSIVE);

                    if (auto target = me->FindNearestCreature(NPC_FINAL_CROMUSH, 60.0f))
                    {
                        target->CastSpell(me, SPELL_ATTACK_ME);
                    }

                    if (auto walden = me->FindNearestCreature(NPC_FINAL_WALDEN, 60.0f))
                    {
                        _waldenGUID = walden->GetGUID();
                        walden->SetReactState(REACT_AGGRESSIVE);
                        walden->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                        if (auto target = me->FindNearestCreature(NPC_FINAL_ARTHURA, 60.0f))
                        {
                            target->CastSpell(me, SPELL_ATTACK_ME);
                        }
                    }

                    if (auto ashbury = me->FindNearestCreature(NPC_FINAL_ASHBURY, 60.0f))
                    {
                        _ashburyGUID = ashbury->GetGUID();
                        ashbury->SetReactState(REACT_AGGRESSIVE);
                        ashbury->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                        if (auto target = me->FindNearestCreature(NPC_FINAL_DASCHLA, 60.0f))
                        {
                            target->CastSpell(me, SPELL_ATTACK_ME);
                        }
                    }
                    break;
                case EVENT_GODFREY_FIREBALL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_FIREBALL);
                    _events.ScheduleEvent(EVENT_GODFREY_FIREBALL, urand(3000, 6000));
                    break;
                case EVENT_GODFREY_BLASTWAVE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_BLASTWAVE);
                    _events.ScheduleEvent(EVENT_GODFREY_BLASTWAVE, urand(8000, 15000));
                    break;
                case EVENT_GODFREY_FLAMESTRIKE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_GODFREY_FLAMESTRIKE);
                    _events.ScheduleEvent(EVENT_GODFREY_FLAMESTRIKE, urand(12000, 15000));
                    break;
                case EVENT_WALDEN_SMITE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_WALDEN_SMITE);
                    _events.ScheduleEvent(EVENT_WALDEN_SMITE, urand(3000, 6000));
                    break;
                case EVENT_WALDEN_HOLYNOVA:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_WALDEN_HOLYNOVA);
                    _events.ScheduleEvent(EVENT_WALDEN_HOLYNOVA, urand(8000, 12000));
                    break;
                case EVENT_ASHBURY_MINDBLAST:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ASHBURY_MINDBLAST);
                    _events.ScheduleEvent(EVENT_ASHBURY_MINDBLAST, urand(3000, 6000));
                    break;
                case EVENT_ASHBURY_PAIN:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_ASHBURY_PAIN);
                    _events.ScheduleEvent(EVENT_ASHBURY_PAIN, urand(30000, 40000));
                    break;
                case EVENT_FINAL_LOW_HP:
                    me->AI()->Talk(5);
                    me->AI()->TalkWithDelay(4000, 6);
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_20, 6500);
                    break;
                case EVENT_FINAL_SCENE_20:
                    if (auto walden = Unit::GetCreature(*me, _waldenGUID))
                        if (auto ashbury = Unit::GetCreature(*me, _ashburyGUID))
                            if (auto godfrey = Unit::GetCreature(*me, _godfreyGUID))
                            {
                                godfrey->CastSpell(godfrey, SPELL_TELEPORT_EFFECT);
                                godfrey->CastSpell(godfrey, SPELL_SANCTUARY_NO_COMBAT);
                                godfrey->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                godfrey->DespawnOrUnsummon(1500);
                                ashbury->CastSpell(ashbury, SPELL_TELEPORT_EFFECT);
                                ashbury->CastSpell(ashbury, SPELL_SANCTUARY_NO_COMBAT);
                                ashbury->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                ashbury->DespawnOrUnsummon(1500);
                                walden->CastSpell(walden, SPELL_TELEPORT_EFFECT);
                                walden->CastSpell(walden, SPELL_SANCTUARY_NO_COMBAT);
                                walden->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                walden->DespawnOrUnsummon(1500);
                                _events.ScheduleEvent(EVENT_FINAL_SCENE_21, 1500);
                            }
                case EVENT_FINAL_SCENE_21:
                    if (auto cromush = me->FindNearestCreature(NPC_FINAL_CROMUSH, 30.0f))
                        if (auto agatha = me->FindNearestCreature(NPC_FINAL_AGATHA, 30.0f))
                            if (auto arthura = me->FindNearestCreature(NPC_FINAL_ARTHURA, 30.0f))
                                if (auto daschla = me->FindNearestCreature(NPC_FINAL_DASCHLA, 30.0f))
                                {
                                    cromush->CastSpell(cromush, SPELL_SANCTUARY_NO_COMBAT);
                                    cromush->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    agatha->CastSpell(agatha, SPELL_SANCTUARY_NO_COMBAT);
                                    agatha->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    arthura->CastSpell(arthura, SPELL_SANCTUARY_NO_COMBAT);
                                    arthura->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    daschla->CastSpell(daschla, SPELL_SANCTUARY_NO_COMBAT);
                                    daschla->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                    cromush->SetWalk(false);
                                    agatha->SetWalk(false);
                                    arthura->SetWalk(false);
                                    daschla->SetWalk(false);
                                    cromush->GetMotionMaster()->MovePoint(3, -694.82f, 1545.61f, 27.883f);
                                    agatha->GetMotionMaster()->MovePoint(3, -702.771f, 1547.591f, 27.997f);
                                    arthura->GetMotionMaster()->MovePoint(3, -698.006f, 1541.907f, 28.0202f);
                                    daschla->GetMotionMaster()->MovePoint(3, -697.085f, 1552.68f, 27.956f);
                                }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _fightInProgress = false;
        uint64 _lornaGUID = NULL;
        uint64 _sylvanasGUID = NULL;
        uint64 _godfreyGUID = NULL;
        uint64 _waldenGUID = NULL;
        uint64 _ashburyGUID = NULL;
        EventMap _events;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cities_in_dust_enemiesAI(creature);
    }
};

class npc_cities_in_dust_actors : public CreatureScript
{
public:
    npc_cities_in_dust_actors() : CreatureScript("npc_cities_in_dust_actors") { }

    struct npc_cities_in_dust_actorsAI : public ScriptedAI
    {
        npc_cities_in_dust_actorsAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetPhaseMask(2048, false);

            me->SetCanFly(false);
            me->SetDisableGravity(false);

            _playerGUID = me->GetOwnerGUID();
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (me->GetEntry() != NPC_FINAL_CROMUSH)
            {
                _events.ScheduleEvent(EVENT_DOOMHOWL, urand(2000, 8000));
                _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(1000, 4000));
            }

            if (me->GetEntry() == NPC_FINAL_CROMUSH)
            {
                _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(4000, 12000));  // thunder 33967 
                _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(2000, 5000)); // mortal 32736
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (me->GetEntry() != NPC_FINAL_LORNA)
            {
                if (point == 1)
                {
                    _events.ScheduleEvent(EVENT_FINAL_SCENE_FACING, 100);
                }

                if (point == 3)
                {
                    switch (me->GetEntry())
                    {
                    case NPC_FINAL_CROMUSH:
                        _events.CancelEvent(EVENT_THUNDERCLAP);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_22, 100);
                        break;
                    case NPC_FINAL_DASCHLA:
                    case NPC_FINAL_AGATHA:
                    case NPC_FINAL_ARTHURA:
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_23, 100);
                    default:
                        break;
                    }
                }
            }

            if (me->GetEntry() == NPC_FINAL_LORNA)
            {
                if (point == 1)
                {
                    me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (who->GetGUID() == me->GetOwnerGUID())
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (spellInfo->Id == SPELL_PING_ACTORS_1)
                    {
                        _stopFollow = true;

                        switch (me->GetEntry())
                        {
                        case NPC_FINAL_CROMUSH:
                            me->GetMotionMaster()->MovePoint(1, -696.53f, 1549.96f, 27.93f);
                            break;
                        case NPC_FINAL_DASCHLA:
                            me->GetMotionMaster()->MovePoint(1, -701.644f, 1542.776f, 28.011f);
                            break;
                        case NPC_FINAL_AGATHA:
                            me->SetCanFly(true);
                            me->SetDisableGravity(true);
                            me->GetMotionMaster()->MoveJump(-700.99f, 1547.657f, 33.98f, 20, 10);
                            _events.ScheduleEvent(EVENT_FINAL_SCENE_AGATHA_FACING, 2000);
                            break;
                        case NPC_FINAL_ARTHURA:
                            me->GetMotionMaster()->MovePoint(1, -700.44f, 1553.789f, 27.99f);
                            break;
                        default:
                            break;
                        }
                    }

                    if (spellInfo->Id == SPELL_PING_ACTORS_DESPAWN)
                    {
                        switch (me->GetEntry())
                        {
                        case NPC_FINAL_CROMUSH:
                            player->RemoveAura(SPELL_AURA_SUMMON_CROMUSH);
                            me->DespawnOrUnsummon();
                            break;
                        case NPC_FINAL_DASCHLA:
                            player->RemoveAura(SPELL_AURA_SUMMON_DASCHLA);
                            me->DespawnOrUnsummon();
                            break;
                        case NPC_FINAL_AGATHA:
                            player->RemoveAura(SPELL_AURA_SUMMON_AGATHA);
                            me->DespawnOrUnsummon();
                            break;
                        case NPC_FINAL_ARTHURA:
                            player->RemoveAura(SPELL_AURA_SUMMON_ARTHURA);
                            me->DespawnOrUnsummon();
                            break;
                        default:
                            break;
                        }
                    }
                }
            }

            if (spellInfo->Id == SPELL_PING_ACTORS_2)
            {
                if (auto target = me->FindNearestCreature(NPC_FINAL_GODFREY, 60.0f))
                {
                    switch (me->GetEntry())
                    {
                    case NPC_FINAL_CROMUSH:
                        me->SetFacingToObject(target);
                        me->AI()->TalkWithDelay(2000, 0);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        break;
                    case NPC_FINAL_DASCHLA:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFacingToObject(target);
                        break;
                    case NPC_FINAL_AGATHA:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFacingToObject(target);
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        me->GetMotionMaster()->MovePoint(5, -700.99f, 1547.657f, 28.0f);
                        break;
                    case NPC_FINAL_ARTHURA:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFacingToObject(target);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetEntry() == NPC_FINAL_CROMUSH)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_CROMUSH);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_CROMUSH);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_stopFollow)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.4f * M_PI);
                        }

                        if (!_isFollowing && !_stopFollow)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.4f * M_PI);
                            _isFollowing = true;
                        }
                    }

                    if (me->GetEntry() == NPC_FINAL_ARTHURA)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_ARTHURA);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_ARTHURA);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_stopFollow)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.7f * M_PI);
                        }

                        if (!_isFollowing && !_stopFollow)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.7f * M_PI);
                            _isFollowing = true;
                        }
                    }

                    if (me->GetEntry() == NPC_FINAL_AGATHA)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_AGATHA);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_AGATHA);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_stopFollow)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.1f * M_PI);
                        }

                        if (!_isFollowing && !_stopFollow)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.1f * M_PI);
                            _isFollowing = true;
                        }
                    }

                    if (me->GetEntry() == NPC_FINAL_DASCHLA)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 130)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_DASCHLA);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_CITIES_IN_DUST) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_AURA_SUMMON_DASCHLA);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetReactState() != REACT_PASSIVE && !player->getVictim() && !player->isInCombat() && !_stopFollow)
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.4f * M_PI);
                        }

                        if (!_isFollowing && !_stopFollow)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.4f * M_PI);
                            _isFollowing = true;
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FINAL_SCENE_FACING:
                    if (auto target = me->FindNearestCreature(NPC_TARGET_BUNNY, 50.0f))
                    {
                        if (me->GetEntry() == NPC_FINAL_CROMUSH)
                        {
                            me->SetFacingToObject(target);
                            me->HandleEmoteCommand(EMOTE_STATE_READY2H);
                        }

                        if (me->GetEntry() == NPC_FINAL_DASCHLA)
                            me->SetFacingToObject(target);

                        if (me->GetEntry() == NPC_FINAL_ARTHURA)
                            me->SetFacingToObject(target);
                    }
                    break;
                case EVENT_FINAL_SCENE_AGATHA_FACING:
                    if (auto target = me->FindNearestCreature(NPC_TARGET_BUNNY, 50.0f))
                        me->SetFacingToObject(target);
                    break;
                case EVENT_DOOMHOWL:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_DOOMHOWL);
                    _events.ScheduleEvent(EVENT_DOOMHOWL, urand(12000, 25000));
                    break;
                case EVENT_UNHOLY_SMITE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_UNHOLY_SMITE);
                    _events.ScheduleEvent(EVENT_UNHOLY_SMITE, urand(3000, 7000));
                    break;
                case EVENT_THUNDERCLAP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_THUNDERCLAP); // thunder 33967 
                    _events.ScheduleEvent(EVENT_THUNDERCLAP, urand(12000, 25000));
                    break;
                case EVENT_MORTAL_STRIKE:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_MORTAL_STRIKE);
                    _events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(5000, 8000));
                    break;
                case EVENT_FINAL_SCENE_22:
                    if (auto target = me->FindNearestCreature(NPC_FINAL_SYLVANAS, 50.0f))
                    {
                        me->SetFacingToObject(target);
                        me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                        me->AI()->TalkWithDelay(3000, 1);
                        _events.ScheduleEvent(EVENT_FINAL_SCENE_24, 5000);
                    }
                    break;
                case EVENT_FINAL_SCENE_23:
                    if (auto target = me->FindNearestCreature(NPC_FINAL_SYLVANAS, 50.0f))
                        me->SetFacingToObject(target);
                    break;
                case EVENT_FINAL_SCENE_24:
                    if (auto arthura = me->FindNearestCreature(NPC_FINAL_ARTHURA, 50.0f))
                        if (auto agatha = me->FindNearestCreature(NPC_FINAL_AGATHA, 50.0f))
                            if (auto daschla = me->FindNearestCreature(NPC_FINAL_DASCHLA, 50.0f))
                            {
                                _agathaGUID = agatha->GetGUID();
                                _arthuraGUID = arthura->GetGUID();
                                _daschlaGUID = daschla->GetGUID();
                                agatha->AI()->TalkWithDelay(2000, 0);
                                arthura->AI()->TalkWithDelay(6000, 0);
                                daschla->AI()->TalkWithDelay(10500, 0);
                                arthura->AI()->TalkWithDelay(15500, 1);

                                agatha->CastWithDelay(19500, agatha, SPELL_FINAL_RAISE_FORSAKEN);
                                daschla->CastWithDelay(19500, daschla, SPELL_FINAL_RAISE_FORSAKEN);
                                arthura->CastWithDelay(19500, arthura, SPELL_FINAL_RAISE_FORSAKEN);
                                _events.ScheduleEvent(EVENT_FINAL_SCENE_25, 27500);
                            }
                    break;
                case EVENT_FINAL_SCENE_25:
                    if (auto arthura = Unit::GetCreature(*me, _arthuraGUID))
                        if (auto agatha = Unit::GetCreature(*me, _agathaGUID))
                            if (auto daschla = Unit::GetCreature(*me, _daschlaGUID))
                                if (auto sylv = me->FindNearestCreature(NPC_FINAL_SYLVANAS, 50.0f))
                                {
                                    agatha->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 0);
                                    arthura->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 0);
                                    daschla->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 0);
                                    agatha->CastSpell(agatha, SPELL_PERMANENT_DEATH_ANIM);
                                    arthura->CastSpell(arthura, SPELL_PERMANENT_DEATH_ANIM);
                                    daschla->CastSpell(daschla, SPELL_PERMANENT_DEATH_ANIM);
                                    agatha->DespawnOrUnsummon(12000);
                                    arthura->DespawnOrUnsummon(12000);
                                    daschla->DespawnOrUnsummon(12000);
                                    sylv->RemoveAura(SPELL_PERMANENT_DEATH_ANIM);
                                    sylv->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                                    sylv->AI()->TalkWithDelay(3000, 20);
                                    me->CastWithDelay(5000, sylv, SPELL_DUMMY_PING_3);
                                }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _stopFollow = false;
        bool _isFollowing = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _cromushGUID = 0;
        uint64 _arthuraGUID = 0;
        uint64 _agathaGUID = 0;
        uint64 _daschlaGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cities_in_dust_actorsAI(creature);
    }
};

void AddSC_silverpine_forest()
{
    new npc_warchief_cometh_controller();
    new npc_fallen_human_silverpine();
    new spell_forsaken_tropper_rand_transform();
    new npc_forsaken_bat_vs_murlocs_vehicle();
    new npc_waiting_to_exsanguinate_controller();
    new npc_armoire_vehicle();
    new npc_orc_sea_pup();
    new spell_pick_up_orc_crate();
    new npc_webbed_victim_silverpine();
    new npc_orc_sea_dog_rescued();
    new npc_agatha1_silverpine();
    new spell_notify_agatha_isle();
    new npc_no_escape_controller();
    new npc_fenris_no_escape_camera();
    new npc_agatha_resummon_controller();
    new npc_forsaken_sylv_horse();
    new spell_kill_me_sylvanas();
    new spell_dreadguard_salute();
    new npc_horde_cannon_45263();
    new npc_nowhere_master_forteski();
    new npc_veteran_forsaken_trooper();
    new npc_arthura_flight_sepuchler();
    new npc_camera_silverpine_telescope();
    new go_7th_legion_telescope();
    new npc_arthura_ruins();
    new npc_belmont_guardian();
    new playerscript_silverpine_quest_guardian_summons();
    new npc_arthura_flight_escape();
    new npc_sylvanas_rise_godfrey_scene();
    new npc_archmage_ataeric_transdimentional_3();
    new npc_godfrey_battlefront_guardian();
    new npc_walden_battlefront_guardian();
    new npc_ashbury_battlefront_guardian();
    new npc_cowering_trooper();
    new npc_lorna_majesty();
    new npc_horde_hauler_north_south();
    new npc_horde_hauler_south_north();
    new npc_mutant_bush_chicken();
    new npc_cities_in_dust_actors();
    new npc_sylvanas_cities_in_dust();
    new npc_cities_in_dust_enemies();
}
