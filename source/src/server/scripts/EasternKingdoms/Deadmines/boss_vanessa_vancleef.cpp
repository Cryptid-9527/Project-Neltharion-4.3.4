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

#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "deadmines.h"

enum Spells
{
   SPELL_SUMMON_SMALL_MOONBEAM     = 98724,
   SPELL_SUMMON_ENFORCER           = 92616,
   SPELL_SUMMON_SORCER             = 92617,
   SPELL_SUMMON_SHADOW_GUARD        = 92618,

   SPELL_DEFLECTION_2              = 92615,
   SPELL_DEFLECTION                = 92614,

   SPELL_DEADLY_BLADES             = 92622, // trigger SPELL_BACKSLASH_TARGET
   SPELL_BACKSLASH_TARGET          = 92620, // dummy SPELL_BACKSLASH_TARGET
   SPELL_BACKSLASH                 = 92619,

   SPELL_VENGEANCE_OF_VANCLEEF     = 95542,


   SPELL_POWDER_EXPLOSION          = 96283,

   // 45979
   SPELL_FIERY_BLAZE               = 93484, //  Entry: 45979
   SPELL_FIERY_BLAZE_DAMAGE        = 93485, //  Entry: 45979

   // 49850
   SPELL_CHARGE                    = 90928, // ?? casted by ype: Unit Entry: 49850
   SPELL_BLOOD_BAIN                = 90925,
   SPELL_BLOOD_BAIN_2              = 90923,
   SPELL_RECKLESSNESS              = 90929,
   // 49852
   SPELL_CAMOUFLAGE                = 90954,
   SPELL_CAMOUFLAGE_HC             = 90955,
   SPELL_EVASION                   = 90958,
   SPELL_SHADOWSTEP                = 90956,
   SPELL_SINISTER_STRIKE           = 90951,
   SPELL_WHIRLING_BLADES           = 90960,
   SPELL_SINISTER_STRIKE_HC        = 90952,
   // 49854
   SPELL_BLOODBOLT                 = 90938,
   SPELL_RAGE_ZONE                 = 90932,
   // 49550
   SPELL_CLICK_ME                  = 95527,
   SPELL_RIDE_VEHICLE              = 89731,

   SPELL_BLOOD_WASH_HC             = 90947,
};

enum vancleefEvents
{
    EVENT_DEFLECTION = 1,
    EVENT_DEADLY_BLADE,
    EVENT_FLAME,
    EVENT_ACTIVATE_FIRE,
    EVENT_ACTIVATE_ROPE,
    EVENT_ACTIVATE_FIRE_END,
    EVENT_VENGEANCE_OF_VANCLEEF,
    EVENT_SUMMON_SHADOWGUARD,
    EVENT_SUMMON_ENFORCER,
    EVENT_SUMMON_SORCER,
    EVENT_POWDER_EXPLOSION,
    EVENT_SELECT_VALVE_VISIBLE,
    EVENT_OUTRO,
    EVENT_OUTRO_1,
    POINT_ROPE_FAR,
    POINT_ROPE_ORIG,
    EVENT_VANESSA_SET_O,
    EVENT_VENGEANCE_OF_VANCLEEF_2,

    EVENT_RECKLESSNESS,
    EVENT_ENFORCER_CHARGE,
    EVENT_BLOOD_BAIN,
    EVENT_BLOOD_BAIN_RECOVER,

    EVENT_BLOODBOLT,
    EVENT_RAGE_ZONE,
    EVENT_BLOOD_WASH_HC,


    EVENT_EVASION,
    EVENT_SHADOWSTEP,
    EVENT_SINISTER_STRIKE,
    EVENT_WHIRLING_BLADES,
    EVENT_SINISTER_STRIKE_HC,
    EVENT_FLAME_ORIENTATION,
    EVENT_CHECK_WIPE,
};

Position const vancleefEventFire = { -52.813679f, -820.460144f, 51.473946f, 3.101540f };

#define FLAMMES_NBR 48

Position const initialFlammesPos[48] =
{
    {-100.849f, -703.773f, 9.294073f, 0.0f},
    {-88.4253f, -724.722f, 8.675033f, 0.0f},
    {-72.559f, -731.221f, 8.586903f, 0.0f},
    {-30.6337f, -727.731f, 8.521023f, 0.0f},
    {-49.3264f, -730.056f, 9.320483f, 0.0f},

    {-12.0556f, -740.252f, 9.109463f, 0.0f},
    {0.512153f, -768.229f, 9.801343f, 0.0f},
    //{-54.396954f, -826.355469f,  42.122112f , 0.0f}, //MOVE
    {-96.04688f, -694.434f, 8.508634f, 0.0f},
    {-70.9809f, -795.5f, 39.14447f, 0.0f},

    {-66.18229f, -789.2778f, 39.78834f, 0.0f},
    {-75.50694f, -787.1597f, 39.16325f, 0.0f},
    {-46.48438f, -791.8073f, 39.12098f, 0.0f},
    {-64.02257f, -797.4323f, 39.12309f, 0.0f},
    {-38.11458f, -795.2952f, 39.45137f, 0.0f},

    {-70.9809f, -795.5f, 39.14447f, 0.0f},
    {-66.18229f, -789.2778f, 39.78834f, 0.0f},
    {-75.50694f, -787.1597f, 39.16325f, 0.0f},
    {-46.48438f, -791.8073f, 39.12098f, 0.0f},
    {-64.02257f, -797.4323f, 39.12309f, 0.0f},

    {-38.11458f, -795.2952f, 32.45137f, 0.0f},
    {-70.9809f, -795.5f, 32.14447f, 0.0f},
    {-66.18229f, -789.2778f, 32.78834f, 0.0f},
    {-75.50694f, -787.1597f, 32.16325f, 0.0f},
    {-46.48438f, -791.8073f, 32.12098f, 0.0f},

    {-64.02257f, -797.4323f, 32.12309f, 0.0f},
    {-38.11458f, -795.2952f, 19.45137f, 0.0f},
    {-70.9809f, -795.5f, 19.14447f, 0.0f},
    {-66.18229f, -789.2778f, 19.78834f, 0.0f},
    {-75.50694f, -787.1597f, 19.16325f, 0.0f},

    {-46.48438f, -791.8073f, 19.12098f, 0.0f},
    {-64.02257f, -797.4323f, 19.12309f, 0.0f},
    {-95.849f, -703.773f, 9.294073f, 0.0f},
    {-83.4253f, -724.722f, 8.675033f, 0.0f},
    {-67.559f, -731.221f, 8.586903f, 0.0f},

    {-25.6337f, -727.731f, 8.521023f, 0.0f},
    {-43.3264f, -730.056f, 9.320483f, 0.0f},
    {-5.0556f, -740.252f, 9.109463f, 0.0f},
    {2.512153f, -768.229f, 9.801343f, 0.0f},
    {-95.04688f, -694.434f, 8.508634f, 0.0f},

    //{-55.29f, -788.46f, 38.94f, 0.0f}, //MOVE/REMOVE
    {-86.29f, -797.46f, 37.24f, 0.0f},
    {-106.31f, -792.07f, 28.22f, 0.0f},
    {-83.55f, -780.84f, 26.62f, 0.0f},
    {-124.80f, -793.47f, 17.0f, 0.0f},

    {-55.29f, -788.46f, 32.94f, 0.0f},
    {-86.29f, -797.46f, 32.24f, 0.0f},
    {-106.31f, -792.07f, 19.22f, 0.0f},
    {-83.55f, -780.84f, 19.62f, 0.0f},
    {-124.80f, -793.47f, 17.0f, 0.0f},
};

#define FIGHT_FLAMMES_NBR 46

Position const fightFlamesPos[46] =
{
    {-75.13f, -810.34f, 40.17f, 0.0f},
    {-101.12f, -813.30f, 38.2f, 0.0f},
    {-102.95f, -819.37f, 38.04f, 0.0f},
    {-100.33f, -826.24f, 38.23f, 0.0f},
    {-94.05f, -830.19f, 38.70f, 0.0f},

    {-36.527000f, -823.042542f, 43.814671f, 0.0f},//
    {-46.68f, -808.97f, 42.83f, 0.0f},
    {-33.68f, -814.40f, 40.44f, 0.0f},
    {-33.68f, -814.40f, 47.44f, 0.0f},
    {-107.40f, -827.68f, 30.30f, 0.0f},

    {-99.17f, -833.52f, 30.65f, 0.0f},
    {-94.41f, -836.89f, 30.93f, 0.0f},
    {-53.59f, -841.63f, 33.54f, 0.0f},
    {-45.66f, -838.46f, 34.15f, 0.0f},
    {-38.77f, -829.36f, 34.59f, 0.0f},

    {-45.20f, -845.15f, 23.06f, 0.0f},
    {-55.29f, -847.36f, 22.53f, 0.0f},
    {-80.29f, -847.32f, 18.58f, 0.0f},
    {-92.84f, -843.78f, 21.23f, 0.0f},
    {-99.39f, -840.64f, 21.09f, 0.0f},

    {-106.49f, -836.40f, 20.96f, 0.0f},
    {-112.20f, -832.48f, 20.10f, 0.0f},
    {-125.22f, -826.18f, 16.87f, 0.0f},
    {-122.74f, -835.18f, 16.90f, 0.0f},
    {-114.95f, -842.11f, 16.90f, 0.0f},

    {-105.15f, -848.39f, 16.98f, 0.0f},
    {-92.843475f, -842.677185f, 21.199102f, 0.0f},//MOVE
    {-91.10f, -853.79f, 17.17f, 0.0f},
    {-86.62f, -855.95f, 17.20f, 0.0f},
    //{-81.10f, -857.79f, 17.27f, 0.0f},//REMOVE

    //{-81.62f, -858.95f, 17.20f, 0.0f},//REMOVE
    //{-76.10f, -858.79f, 17.27f, 0.0f},//REMOVE
    //{-71.62f, -858.95f, 17.20f, 0.0f},//REMOVE
    //{-66.10f, -858.79f, 17.27f, 0.0f},//REMOVE
    //{-61.62f, -858.95f, 17.20f, 0.0f},//REMOVE

    {-56.15f, -857.39f, 19.98f, 0.0f},
    {-51.62f, -855.95f, 19.10f, 0.0f},
    {-46.10f, -853.79f, 19.17f, 0.0f},
    {-41.62f, -851.95f, 19.20f, 0.0f},
    {-36.10f, -848.79f, 19.27f, 0.0f},

    {-31.15f, -846.39f, 19.98f, 0.0f},
    {-26.62f, -842.15f, 19.10f, 0.0f},
    {-21.10f, -838.89f, 19.17f, 0.0f},
    {-16.62f, -834.65f, 19.20f, 0.0f},
    {-11.10f, -830.79f, 19.27f, 0.0f},

    {-77.71f, -815.47f, 40.0f, 0.0f},
    {-88.06f, -815.51f, 40.0f, 0.0f},
    {-88.64f, -822.94f, 40.0f, 0.0f},
    {-88.76f, -819.22f, 40.0f, 0.0f},
    {-54.396954f, -826.355469f,  42.122112f , 0.0f},
    {-69.739983f, -802.786377f, 40.655277f, 0.0f},
    {-52.246147f, -790.997314f, 38.562691f, 0.0f},
};

#define TEMP_FLAMMES_NBR 31

Position const tempFlammesPos[31] =
{
    {-73.389f, -838.31f, 40.4f, 0.0f},
    {-69.389f, -838.31f, 40.7f, 0.0f},
    {-65.389f, -838.31f, 41.1f, 0.0f},
    {-61.389f, -837.31f, 41.5f, 0.0f},
    {-58.389f, -834.31f, 41.7f, 0.0f},

    {-73.389f, -834.31f, 41.4f, 0.0f},
    {-69.389f, -833.31f, 41.7f, 0.0f},
    {-65.389f, -834.31f, 41.4f, 0.0f},
    {-61.389f, -833.31f, 41.5f, 0.0f},
    {-58.389f, -832.31f, 41.7f, 0.0f},

    {-73.389f, -827.31f, 41.4f, 0.0f},
    {-69.389f, -827.31f, 41.7f, 0.0f},
    {-65.389f, -827.31f, 41.4f, 0.0f},
    {-61.389f, -827.31f, 41.5f, 0.0f},
    {-58.389f, -827.31f, 41.7f, 0.0f},

    {-81.57f, -807.92f, 39.67f, 0.0f},
    {-88.90f, -807.93f, 39.67f, 0.0f},
    //{-95.62f, -908.84f, 38.5f, 0.0f},
    {-49.36f, -831.14f, 41.62f, 0.0f},
    {-63.10f, -819.36f, 41.26f, 0.0f},

    {-60.46f, -814.94f, 41.52f, 0.0f},
    {-63.77f, -808.26f, 41.23f, 0.0f},
    {-66.69f, -813.01f, 40.96f, 0.0f},
    {-71.48f, -819.45f, 40.49f, 0.0f},
    {-83.17f, -819.23f, 39.53f, 0.0f},

    //{-73.17f, -844.01f, 32.21f, 0.0f},
    //{-65.17f, -843.01f, 32.71f, 0.0f},

    {-84.14f, -833.91f, 40.0f, 0.0f},
    {-83.99f, -829.78f, 40.0f, 0.0f},
    {-78.39f, -831.00f, 40.0f, 0.0f},
    {-77.67f, -836.49f, 40.0f, 0.0f},
    //{-88.06f, -815.51f, 40.0f, 0.0f},
    //{-88.64f, -822.94f, 40.0f, 0.0f},
    //{-88.76f, -819.22f, 40.0f, 0.0f},
    {-77.99f, -825.09f, 40.0f, 0.0f},
    //{-77.71f, -815.47f, 40.0f, 0.0f},
};

#define FINAL_FLAMMS_NBR 7

Position const finalFlamesPos[7] =
{
    {-86.057365f, -832.512695f, 39.335075f, 0.0f},
    {-82.329521f, -832.704468f, 39.642193f, 0.0f},
    {-55.397701f, -834.869751f, 42.040058f, 0.0f},
    {-47.267067f, -831.662598f, 42.813328f, 0.0f},
    {-56.791569f, -809.525024f, 41.866543f, 0.0f},

    {-61.388245f, -804.920593f, 42.374432f, 0.0f},
    {-81.242508f, -824.795227f, 39.692085f, 0.0f},
};

class boss_vanessa_vancleef : public CreatureScript
{
public:
    boss_vanessa_vancleef() : CreatureScript("boss_vanessa_vancleef") { }

    struct boss_vanessa_vancleefAI : public BossAI
    {
        boss_vanessa_vancleefAI(Creature* creature) : BossAI(creature, BOSS_VANCLEEF) {}

        void Reset()
        {
            initialFlames = false;
            firstFlames = false;
            secondFlames = false;
            phaseCbt = false;
            endEvent = false;
            if (instance && instance->IsDone(BOSS_EVENT_VANCLEEF))
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetReactState(REACT_DEFENSIVE);
            }
            BossAI::Reset();
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void JustDied(Unit* killer)
        {

            if (instance)
            {
                Position pos;
                me->GetPosition(&pos);
                Map::PlayerList const &PlayerList = instance->instance->GetPlayers();
                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (auto player = i->getSource())
                            if (player->isAlive())
                                player->ExitVehicle(&pos);
                instance->FinishLfgDungeon(me);
            }
            _JustDied();
            for (auto cnt = 0; cnt < FIGHT_FLAMMES_NBR; cnt++)
                me->SummonCreature(NPC_GENERAL_BUNNY, fightFlamesPos[cnt]);
            for (auto cnt = 0; cnt < FINAL_FLAMMS_NBR; cnt++)
                me->SummonCreature(NPC_GENERAL_BUNNY, finalFlamesPos[cnt]);


            std::list<Creature*> list_of_npcs;
            GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_GENERAL_BUNNY, 50.0f);
            for (auto npc : list_of_npcs)
                npc->DespawnOrUnsummon(10000);
        }

        void JustSummoned(Creature * summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_GENERAL_BUNNY:
                {
                    summon->CastSpell(summon, SPELL_FIERY_BLAZE, true);
                    break;
                }
                case NPC_ROPE:
                {
                    Position rope(*summon);
                    rope.m_positionZ += 180.f;
                    if (Creature *marker = summon->FindNearestCreature(NPC_VANESSA_ROPE_ANCHOR, 200))
                        summon->CastSpell(marker, SPELL_ROPE_RAY, true);
                    else if (auto marker = summon->SummonCreature(NPC_VANESSA_ROPE_ANCHOR, rope))
                        summon->CastSpell(marker, SPELL_ROPE_RAY, true);
                    SetInact(true);
                    break;
                }
            }
            BossAI::JustSummoned(summon);
        }

        void EnterCombat(Unit * /*who*/)
        {
            Talk(0);
            //MOVED over to DamageTaken
            /*if (!initialFlames && HealthBelowPct(85))
            {
                for (auto cnt = 0; cnt < FLAMMES_NBR; cnt++)
                    me->SummonCreature(NPC_GENERAL_BUNNY, flammesPos[cnt]);
                        initialFlames = true;
            }*/

            StartEvents();
            events.ScheduleEvent(EVENT_SUMMON_ENFORCER, 15000);
            _EnterCombat();
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
        }

        void StartEvents()
        {
            if (!secondFlames)
                events.ScheduleEvent(EVENT_DEFLECTION, 5000);
            events.ScheduleEvent(EVENT_DEADLY_BLADE, 15000);
            if (secondFlames)
                events.ScheduleEvent(EVENT_VENGEANCE_OF_VANCLEEF_2, 6000);
        }

        void DamageTaken(Unit* caster, uint32& damage)
        {
            if (phaseCbt)
            {
                SetInact(false);
                phaseCbt = false;
            }

            if (!initialFlames && HealthBelowPct(85))
            {
                for (auto cnt = 0; cnt < FLAMMES_NBR; cnt++)
                    me->SummonCreature(NPC_GENERAL_BUNNY, initialFlammesPos[cnt]);
                initialFlames = true;
            }

            if (!firstFlames && HealthBelowPct(50))
            {
                events.ScheduleEvent(EVENT_FLAME, 0);
                events.ScheduleEvent(EVENT_FLAME_ORIENTATION, 2000);
                firstFlames = true;
            }

            if (!secondFlames && HealthBelowPct(25))
            {
                events.ScheduleEvent(EVENT_FLAME, 0);
                events.ScheduleEvent(EVENT_FLAME_ORIENTATION, 2000);
                secondFlames = true;
            }

            if (damage > me->GetHealth())
            {
                damage = 0;
                if (!endEvent)
                {
                    killer = caster;
                    events.Reset();
                    SetInact(true);
                    Talk(6);
                    events.ScheduleEvent(EVENT_OUTRO, 5000);
                }
                endEvent = true;
            }
        }

        void DespawnCreatures(uint32 entry, float distance)
        {
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, entry, distance);

            if (creatures.empty())
                return;

            for (auto iter = creatures.begin(); iter != creatures.end(); ++iter)
            {
                Creature* creature = (*iter);
                
                if ((entry == NPC_ROPE) && !(creature->GetVehicleKit()->GetPassenger(0)))
                    creature->DespawnOrUnsummon();
            }
                
        }

        void SummonRopes()
        {
            me->SummonCreature(NPC_ROPE, -62.17014f, -839.8438f, 41.48514f, 5.044002f);
            me->SummonCreature(NPC_ROPE, -64.97049f, -840.0087f, 41.22668f, 4.834562f);
            me->SummonCreature(NPC_ROPE, -67.79166f, -840.1736f, 40.96664f, 4.712389f);
            me->SummonCreature(NPC_ROPE, -70.62153f, -840.0104f, 40.71846f, 4.537856f);
            me->SummonCreature(NPC_ROPE, -73.42361f, -839.8646f, 40.47259f, 4.34587f);
        }

        void ActivateFire(bool on)
        {
            if (on)
            {
                if (!secondFlames)
                    for (auto cnt = 0; cnt < FIGHT_FLAMMES_NBR; cnt++)
                        me->SummonCreature(NPC_GENERAL_BUNNY, fightFlamesPos[cnt]);
                if (secondFlames)
                    for (auto cnt = 0; cnt < FINAL_FLAMMS_NBR; cnt++)
                        me->SummonCreature(NPC_GENERAL_BUNNY, finalFlamesPos[cnt]);

                _tempFlame.clear();
                for (unsigned int cnt = 0; cnt < TEMP_FLAMMES_NBR; cnt++)
                    if (Creature *c = me->SummonCreature(NPC_GENERAL_BUNNY, tempFlammesPos[cnt]))
                        _tempFlame.push_back(c->GetGUID());
            }
            else
                for (auto itr = _tempFlame.begin(); itr != _tempFlame.end(); itr++)
                    if (Creature *flame = Unit::GetCreature(*me, *itr))
                        flame->DespawnOrUnsummon(1000);
        }

        void SetInact(bool on)
        {
            if (on)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                //me->RemoveAllAuras();
                me->RemoveAurasDueToSpell(SPELL_DEFLECTION);
                me->RemoveAurasDueToSpell(SPELL_DEFLECTION_2);
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->GetMotionMaster()->MoveIdle();
            }
            else
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                DespawnCreatures(NPC_ROPE, 200);
                events.CancelEvent(EVENT_VENGEANCE_OF_VANCLEEF);
                me->SetReactState(REACT_AGGRESSIVE);
                if (me->getVictim())
                {
                    DoStartMovement(me->getVictim());
                    AttackStart(me->getVictim());
                }
                StartEvents();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CHECK_WIPE:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    break;                    
                case EVENT_DEFLECTION:
                    me->CastSpell(me, SPELL_DEFLECTION, true);
                    events.ScheduleEvent(EVENT_DEFLECTION, 30000);
                    break;
                case EVENT_DEADLY_BLADE:
                    DoCast(SPELL_DEADLY_BLADES);
                    events.ScheduleEvent(EVENT_DEADLY_BLADE, 20000);
                    break;
                case EVENT_FLAME:
                    events.Reset();
                    events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    me->GetMotionMaster()->MoveJump(vancleefEventFire.m_positionX, vancleefEventFire.m_positionY, vancleefEventFire.m_positionZ, 14.0f, 14.0f);
                    SetInact(true);
                    Talk(secondFlames ? 4 : 2);
                    events.ScheduleEvent(EVENT_FLAME_ORIENTATION, 2000);
                    events.ScheduleEvent(EVENT_ACTIVATE_ROPE, 6000);
                    break;
                case EVENT_FLAME_ORIENTATION:
                    me->SetFacingTo(vancleefEventFire.m_orientation);
                    break;
                case EVENT_ACTIVATE_ROPE:
                    //me->SetFacingTo(vancleefEventFire.m_orientation);
                    if (!secondFlames)
                        Talk(3);
                    else
                        Talk(5);
                    SummonRopes();
                    events.ScheduleEvent(EVENT_ACTIVATE_FIRE, 5000);
                    break;
                case EVENT_ACTIVATE_FIRE:
                    ActivateFire(true);
                    SetInact(true);
                    events.ScheduleEvent(EVENT_ACTIVATE_FIRE_END, 3000);
                    break;
                case EVENT_ACTIVATE_FIRE_END:
                    ActivateFire(false);
                    SetInact(true);
                    me->GetMotionMaster()->MoveJump(-66.944809f, -814.825745f, 40.916416f, 20.0f, 20.0f);
                    if (secondFlames)
                    {
                        events.ScheduleEvent(EVENT_VENGEANCE_OF_VANCLEEF_2, 2500);
                        events.ScheduleEvent(EVENT_VANESSA_SET_O, 2000);
                        phaseCbt = true;
                    }
                    if (!secondFlames)
                    {
                        events.ScheduleEvent(EVENT_VENGEANCE_OF_VANCLEEF, 2500);
                        events.ScheduleEvent(EVENT_VANESSA_SET_O, 2000);
                        phaseCbt = true;
                    }
                    break;
                case EVENT_VENGEANCE_OF_VANCLEEF_2:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(SPELL_VENGEANCE_OF_VANCLEEF);
                        events.ScheduleEvent(EVENT_VENGEANCE_OF_VANCLEEF, 6000);
                    }
                    break;
                case EVENT_VENGEANCE_OF_VANCLEEF:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(SPELL_VENGEANCE_OF_VANCLEEF);
                        events.ScheduleEvent(EVENT_VENGEANCE_OF_VANCLEEF, 6000);
                    }
                    break;
                case EVENT_VANESSA_SET_O:
                    me->SetFacingTo(4.657103f);
                    me->HandleEmoteCommand(EMOTE_STATE_READY1H);
                    break;
                case EVENT_SUMMON_ENFORCER:
                {
                    Talk(1);
                    //Position summon_at(*me);
                    //me->MoveBlink(summon_at, 10.f, frand(0.f, 2.f * M_PI));
                    //if (auto c = me->SummonCreature(49850, summon_at))
                    if (auto c = me->SummonCreature(49850, -86.082199f, -819.749023f, 39.307556f, 6.271025f))
                        c->SetInCombatWithZone();

                    //DoCast(SPELL_SUMMON_ENFORCER);//49850
                    events.ScheduleEvent(EVENT_SUMMON_SHADOWGUARD, 8000);
                    break;
                }
                case EVENT_SUMMON_SHADOWGUARD:
                {
                    Talk(1);
                    //Position summon_at(*me);
                    //me->MoveBlink(summon_at, 10.f, frand(0.f, 2.f * M_PI));
                    //if (auto c = me->SummonCreature(49852, summon_at))
                    if (auto c = me->SummonCreature(49852, -86.082199f, -819.749023f, 39.307556f, 6.271025f))
                        c->SetInCombatWithZone();

                    //DoCast(SPELL_SUMMON_SHADOW_GUARD);//49852
                    events.ScheduleEvent(EVENT_SUMMON_SORCER, 8000);
                    break;
                }
                case EVENT_SUMMON_SORCER:
                {
                    Talk(1);
                    //Position summon_at(*me);
                    //me->MoveBlink(summon_at, 10.f, frand(0.f, 2.f * M_PI));
                    //if (auto c = me->SummonCreature(49854, summon_at))
                    if (auto c = me->SummonCreature(49854, -86.082199f, -819.749023f, 39.307556f, 6.271025f))
                        c->SetInCombatWithZone();

                    //DoCast(SPELL_SUMMON_SORCER);//49854
                    events.ScheduleEvent(EVENT_SUMMON_ENFORCER, 15000);
                    break;
                }
                case EVENT_OUTRO:
                    Talk(7);
                    events.ScheduleEvent(EVENT_OUTRO_1, 4000);
                    break;
                case EVENT_OUTRO_1:
                    Talk(8);
                    //SummonRopes();
                    events.ScheduleEvent(EVENT_POWDER_EXPLOSION, 5000);
                    break;
                case EVENT_POWDER_EXPLOSION:
                    Talk(9);
                    if (killer)
                        killer->Kill(me, false, true);

                    DoCast(me, SPELL_POWDER_EXPLOSION, true);
                    break;                        
                    default:
                        break;
                }
            }

            Position pos;
            me->GetPosition(&pos);
            if (((pos.m_positionX > -110.552f) && (pos.m_positionX < -27.0f)) && ((pos.m_positionY > -844.6f) && (pos.m_positionY < -795.57f)))
            {
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
            }
            else EnterEvadeMode();
        }

    private:
        Unit* killer{ nullptr };
        bool initialFlames, firstFlames, secondFlames, phaseCbt, endEvent;
        std::list<uint64 > _tempFlame;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vanessa_vancleefAI (creature);
    }
};

// 92620
class spell_vc_backslash_target_selector : public SpellScriptLoader
{
public:
    spell_vc_backslash_target_selector() : SpellScriptLoader("spell_vc_backslash_target_selector") { }

    class NonPreviousTarget
    {
    public:
        explicit NonPreviousTarget(Unit* unit) : _lastTarget(unit){};

        bool operator()(WorldObject* target) const
        {
            return _lastTarget == target;
        }

    private:
        Unit const* _lastTarget;
    };


    class spell_vc_backslash_target_selector_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_vc_backslash_target_selector_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        bool Load()
        {
            previousTarget = NULL;
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (unitList.size() > 1)
                unitList.remove_if(NonPreviousTarget(previousTarget));
            if (!unitList.empty())
                Trinity::Containers::RandomResizeList(unitList, 1);
        }


        void HandleDummyEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                {
                    previousTarget = target;
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    caster->CastSpell(target, spellId, true);
                }
        }

    private:
        Unit *previousTarget;

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_vc_backslash_target_selector_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_vc_backslash_target_selector_SpellScript::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_vc_backslash_target_selector_SpellScript();
    }
};

// 49550
class npc_vancleef_rope : public CreatureScript
{
public:
    npc_vancleef_rope() : CreatureScript("npc_vancleef_rope") { }

    class BackHome : public BasicEvent
    {
    public:
        BackHome(Unit* owner, Position const& cu) : _owner(owner), _spawnPos(cu)
        {
        }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
        {
            _owner->GetMotionMaster()->MovePoint(POINT_ROPE_ORIG, _spawnPos, false, 12.0f);
            return true;
        }

    private:
        Unit* _owner;
        Position _spawnPos;
    };


    struct npc_vancleef_ropeAI : public PassiveAI
    {
        npc_vancleef_ropeAI(Creature* creature) : PassiveAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }

        void OnSpellClick(Unit* player, bool& result)
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }


        void PassengerBoarded(Unit* player, int8 seatId, bool apply)
        {
            if (!apply)
            {
                player->SetDisableGravity(false);
                player->SendMovementDisableGravity();
                return;
            }
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
            player->SetDisableGravity(true);
            player->SendMovementDisableGravity();
            Position pos;
            me->GetNearPoint2D(pos.m_positionX, pos.m_positionY, 55.0f, 4.71f);
            pos.m_positionZ = me->GetPositionZ() + 15.0f;
            me->GetNearPoint2D(_spawnPos.m_positionX, _spawnPos.m_positionY, 5.0f, 4.71f);
            _spawnPos.m_positionZ = me->GetPositionZ() + 5.0f;
            me->GetMotionMaster()->MovePoint(POINT_ROPE_FAR, pos, false, 12.0f);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;
            if (point == POINT_ROPE_FAR)
                me->m_Events.AddEvent(new BackHome(me, _spawnPos), me->m_Events.CalculateTime(100));
            else if (point == POINT_ROPE_ORIG)
            {
                Position Jump_To{ me->GetHomePosition() };
                Jump_To.m_positionY += 6.f;
                Jump_To.m_positionZ += 0.2f;

                if (auto v = me->GetVehicleKit())
                    if (auto p = v->GetPassenger(0))
                    {
                        p->ExitVehicle();
                        p->GetMotionMaster()->Clear();
                        p->GetMotionMaster()->ForceMoveJump(Jump_To.GetPositionX(), Jump_To.GetPositionY(), Jump_To.GetPositionZ(), 12.f, 8.f);
                    }
                me->DespawnOrUnsummon(1000);
            }
        }

        void UpdateAI(const uint32 diff)
        {
        }

    private:
        InstanceScript* instance;
        Position _spawnPos;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vancleef_ropeAI (creature);
    }
};

// 49850
class npc_vc_enforcer : public CreatureScript
{
public:
    npc_vc_enforcer() : CreatureScript("npc_vc_enforcer") { }

    struct npc_vc_enforcerAI : public ScriptedAI
    {
        npc_vc_enforcerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            _events.Reset();            
        }

        void EnterCombat(Unit * who)
        {
            me->CastSpell(who, SPELL_CHARGE, true);
            _events.ScheduleEvent(EVENT_ENFORCER_CHARGE, 13000);
            _events.ScheduleEvent(EVENT_BLOOD_BAIN, 5000);
            _events.ScheduleEvent(EVENT_RECKLESSNESS, 9000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ENFORCER_CHARGE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else {
                        auto list_of_players = me->GetPlayersInRange(140.f, true);
                        if (list_of_players.size())
                            for (auto itr = list_of_players.begin(); itr != list_of_players.end();)
                                if (auto t = (*itr))
                                    if (me->GetDistance(t) > 5.f && me->GetDistance(t) < 30.f)//filter condition
                                        list_of_players.erase(itr++);
                                    else
                                        ++itr;

                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            me->CastSpell(random_target, 90928);
                    }
                    break;
                    case EVENT_BLOOD_BAIN:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->StopMoving();
                            DoCastVictim(SPELL_BLOOD_BAIN);//before we clear victim with attack stop, we need to start the cast
                            me->AttackStop();
                            _events.ScheduleEvent(EVENT_BLOOD_BAIN_RECOVER, 4000);//channel is 3000
                        }
                        break;
                    case EVENT_BLOOD_BAIN_RECOVER:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            me->SetReactState(REACT_AGGRESSIVE);
                            UpdateVictim();
                            _events.ScheduleEvent(EVENT_BLOOD_BAIN, 10000);
                        }
                        break;
                    case EVENT_RECKLESSNESS:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else {
                            DoCast(SPELL_RECKLESSNESS);
                            _events.ScheduleEvent(EVENT_RECKLESSNESS, 10000);
                        }
                        break;
                    default:
                        break;
                }
            }
            if (me->HasReactState(REACT_AGGRESSIVE))        // Don't do anything while set to passive
                if (!me->HasUnitState(UNIT_STATE_CASTING))  // Don't interrupt active casts
                    if (UpdateVictim())                     // Update and respect aggro
                        DoMeleeAttackIfReady();
        }

    private :
        InstanceScript *instance;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vc_enforcerAI (creature);
    }
};

class npc_vc_blood_wizard : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_vc_blood_wizard' WHERE  `entry`=49854;
        UPDATE `creature_template` SET `ScriptName`='npc_vc_blood_wizard' WHERE  `entry`=49855;
    */

public:
    npc_vc_blood_wizard() : CreatureScript("npc_vc_blood_wizard") { }

    struct npc_vc_blood_wizardAI : ScriptedAI
    {
        npc_vc_blood_wizardAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {}
        void InitializeAI()
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            _events.Reset();           
        }
        void EnterCombat(Unit* who)
        {
            //DoCast(SPELL_BLOODBOLT);
            _events.Reset();
            Start_Rotation();
            _events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            _events.Reset();
            summons.DespawnAll();
        }
        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }
        void CheckLineOfSight(SpellInfo const* spell)
        {
            if (auto v = me->getVictim())
                if (!me->IsWithinLOSInMap(v, spell))
                {
                    Position pos(*v);
                    v->MoveBlink(pos, 20.f, v->GetRelativeAngleOffset(me));
                    me->GetMotionMaster()->MovePoint(8, pos);
                }            
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        _events.DelayEvent(i, 6000);
            }
            else
                me->AddUnitState(UNIT_STATE_ROOT);
            CheckLineOfSight(spell);
        }

        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!_events.HasEvent(i))
                    _events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation_Friendly(uint32 eventId, float range)//execute a spell
        {
            auto friendlies = me->GetFriendlyUnitsInRange(range, me);
            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(friendlies))
                me->CastSpell(random_target, my_spells[eventId].spell_id);
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {
            if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                if (auto v = me->getVictim())
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 40.f)
                    {
                        if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                            if (me->isMoving())
                                me->SetRooted(true);
                        me->AddUnitState(UNIT_STATE_ROOT);

                        DoCast(my_spells[eventId].spell_id);
                        ReAttemptSpell(eventId, true);
                    }
                    else {
                        me->SetRooted(false);
                        UpdateVictim();
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            me->ClearUnitState(UNIT_STATE_ROOT);
            if (reschedule) 
            {
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else 
            {
                _events.ScheduleEvent(eventId, 250);
            }
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        };

        void move_or_attack()
        {
            // Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        };

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_BLOODBOLT,
            EVENT_RAGEZONE,
            EVENT_BLOOD_WASH_HC,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_BLOODBOLT = 90938,
            SPELL_RAGEZONE = 90932,
            SPELL_BLOOD_WASH_HC = 90947,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,                         0,        9999999,      9999999,         false,      false}, //DO NOT TOUCH

            {EVENT_BLOODBOLT,         SPELL_BLOODBOLT,          3500,          3500,          true,       true},
            {EVENT_RAGEZONE,           SPELL_RAGEZONE,         12000,         22000,          true,       true},
            {EVENT_BLOOD_WASH_HC, SPELL_BLOOD_WASH_HC,          8000,         20000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BLOODBOLT:          Spell_Rotation(eventId);  break;
                case EVENT_RAGEZONE:           Spell_Rotation(eventId);  break;
                case EVENT_BLOOD_WASH_HC:      Spell_Rotation_Friendly(eventId, 20.f); break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        _events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }      
            move_or_attack();
        }

    private:
        bool use_melee{ false };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vc_blood_wizardAI(creature);
    }

};

// 49852
class npc_vc_shadowguard : public CreatureScript
{
public:
    npc_vc_shadowguard() : CreatureScript("npc_vc_shadowguard") { }

    struct npc_vc_shadowguardAI : public ScriptedAI
    {
        npc_vc_shadowguardAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_SINISTER_STRIKE_HC, 8000);

            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (IsHeroic())
                _events.ScheduleEvent(EVENT_SINISTER_STRIKE_HC, 8000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {

                case EVENT_SINISTER_STRIKE_HC:
                    DoCastVictim(SPELL_SINISTER_STRIKE_HC);
                    _events.ScheduleEvent(EVENT_SINISTER_STRIKE_HC, 8000);
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        InstanceScript* instance;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vc_shadowguardAI(creature);
    }
};

class spell_blood_bain : public SpellScriptLoader
{
public:
    spell_blood_bain() : SpellScriptLoader("spell_blood_bain") { }
    class spell_blood_bain_AuraScript : public AuraScript
    {

        PrepareAuraScript(spell_blood_bain_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_BAIN))
                return false;
            return true;
        }

        void HandleTriggerSpell(AuraEffect const* aurEff)
        {
            //TC_LOG_ERROR("sql.sql", " Hook executed ");
            PreventDefaultAction();
            if (auto caster = GetCaster())//caster
                if (auto target = GetTarget())//target who the aura is sitting on (channels are invis auras)
                {
                    //TC_LOG_ERROR("sql.sql", " Hook executed 2 caster: %u, target: %u", caster->GetGUID(), target-> GetGUID());//variables fill in %u in order
                    uint32 triggerSpell = GetSpellInfo()->Effects[EFFECT_1].TriggerSpell;
                    /*
                    uint64 damage = target->GetMaxHealth() / GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
                    target->CastCustomSpell(triggerSpell, SPELLVALUE_BASE_POINT0, damage, target, true, nullptr, aurEff, caster->GetGUID());//
                    */
                    caster->CastSpell(target, triggerSpell, true);
                    //TC_LOG_ERROR("sql.sql", "Damage Calculation: %u / %u = %u ", target->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, damage);
                }
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_blood_bain_AuraScript::HandleTriggerSpell, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }

    };

    AuraScript* GetAuraScript() const
    {
        return new spell_blood_bain_AuraScript();
    }
};

void AddSC_boss_vanessa_vancleef()
{
    new spell_vc_backslash_target_selector();
    new boss_vanessa_vancleef();
    new npc_vancleef_rope();
    new npc_vc_enforcer();
    new npc_vc_blood_wizard();
    new npc_vc_shadowguard();
    new spell_blood_bain();
}
