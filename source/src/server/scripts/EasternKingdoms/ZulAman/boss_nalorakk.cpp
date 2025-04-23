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

#include "GridNotifiers.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "zulaman.h"

//Trash Waves
float NalorakkWay[9][3] =
{
    { 18.569f, 1414.512f, 11.42f},   //phase 0, pause 1
    {-17.264f, 1419.551f, 12.62f},   //phase 1
    {-52.642f, 1419.357f, 27.31f},   //phase 2, waypoint 2
    {-69.908f, 1419.721f, 27.31f},   //phase 3
    {-79.929f, 1395.958f, 27.31f},   //phase 4
    {-80.072f, 1374.555f, 40.87f},   //phase 5  waypoint 3
    {-79.885f, 1314.76f,  41.21f},   //phase 6, waypoint 4
    {-79.885f, 1298.18f, 48.501f},   //phase 7, waypoint 4
    {-79.885f, 1298.18f, 48.501f},   //phase 8, waypoint 4
};

// Fight
enum says
{
    YELL_AGGRO          = 0,
    YELL_KILL           = 1,
    YELL_DEATH          = 2,
    YELL_BERSERK        = 3,
    YELL_SURGE          = 4,
    YELL_SHIFTEDTOTROLL = 5,
    YELL_SHIFTEDTOBEAR  = 6,
    YELL_NALORAKK_WAVE1 = 7,
    YELL_NALORAKK_WAVE2 = 8,
    YELL_NALORAKK_WAVE3 = 9,
    YELL_NALORAKK_WAVE4 = 10,
};

enum Spells
{
    // Normal Form
    SPELL_BRUTAL_STRIKE             = 42384,
    SPELL_SURGE                     = 42402,
    SPELL_BEAR_FORM                 = 42594,

    // Bear Form
    SPELL_LACERATING_SLASH          = 42395,
    SPELL_REND_FLESH                = 42397,
    SPELL_DEAFENING_ROAR            = 42398,

    SPELL_BERSERK                   = 45078
};

enum Events
{
    EVENT_SURGE = 1,
    EVENT_BRUTAL_STRIKE,
    EVENT_BEAR_FORM,

    EVENT_LACERATING_SLASH,
    EVENT_REND_FLESH,
    EVENT_DEAFENING_ROAR,
    EVENT_NORMAL_FORM,

    EVENT_BERSERK,
    EVENT_ACTIVATE,
};

enum Nalorakk_position_names
{
    POSITION_NAL_FIRST_SIGHT_PREJUMP_START = 0,
    POSITION_NAL_FIRST_SIGHT_JUMP_DEST,
    POSITION_NAL_MAX
};

Position const Nalorakk_Positions[POSITION_NAL_MAX] =
{
    {17.305f, 1414.83f, 11.82f, 5.968f},      // POSITION_NAL_FIRST_SIGHT_PREJUMP_START
    {33.086f, 1412.20f, 2.3159f, 6.280f},     // POSITION_NAL_FIRST_SIGHT_JUMP_DEST
};

enum nalorakk_trash_actions
{
    ACTION_NALORAKK_ATTACK_ORDER_1 = 1,
    EVENT_MOVE_START_1,
    EVENT_JUMP_1,
    EVENT_ATTACK_1,
    ACTION_NALORAKK_ATTACK_ORDER_2,

    /*
        Medicine Man
    */
    EVENT_CHAIN_HEAL,
    EVENT_HEALING_WARD,
    EVENT_LIGHTNING_BOLT,
    EVENT_PROTECTIVE_WARD,
    /*
        Tribesman
    */
    EVENT_CYCLONE_STRIKE,
    EVENT_KICK,
    /*
        AxeThrower
    */
    EVENT_KNOCKDOWN,
    EVENT_RAPTOR_STRIKE,
    /*
        Warbringer
    */
    EVENT_CLEAVE,
    EVENT_FRENZY,
    EVENT_FURIOUS_ROAR,
};

class boss_nalorakk : public CreatureScript
{
    public:
        boss_nalorakk() : CreatureScript("boss_nalorakk") { }

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_nalorakkAI(creature);
        }

        struct boss_nalorakkAI : public BossAI
        {
            boss_nalorakkAI(Creature* creature) : BossAI(creature, DATA_NALORAKKEVENT)
            {
                MoveEvent = true;
                MovePhase = 0;
                instance = creature->GetInstanceScript();
                waitTimer = 0;
                ReadyFight = false;
                CommenceFight = false;
            }
           
            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            }
            void Reset()
            {
                events.Reset();
                _Reset();
                if (MoveEvent)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                else
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
                inMove = false;
                waitTimer = 0;
                me->SetSpeed(MOVE_RUN, 2);
                me->SetWalk(false);
                //MovePhase = 0;
            }

            void SendAttacker(Unit* target)
            {
                std::list<Creature*> templist;
                float x, y, z;
                me->GetPosition(x, y, z);

                {
                    Trinity::AllFriendlyCreaturesInGrid check(me);
                    Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(me, templist, check);
                    me->VisitNearbyGridObject(10.f, searcher);
                }

                if (templist.empty())
                    return;

                for (std::list<Creature*>::const_iterator i = templist.begin(); i != templist.end(); ++i)
                {
                    if ((*i)->GetGUID() != me->GetGUID())
                    {
                        switch (MovePhase)
                        {
                        case 1:
                            (*i)->AI()->DoAction(ACTION_NALORAKK_ATTACK_ORDER_1);
                            break;
                        default:
                            (*i)->AI()->DoAction(ACTION_NALORAKK_ATTACK_ORDER_2);
                            break;
                        }

                    }
                }
            }

            void AttackStart(Unit* who)
            {
                if (MovePhase < 7)
                    return;

                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
                    ScriptedAI::AttackStart(who);
            }

            bool CanSeeAlways(WorldObject const* obj)
            {
                if (auto p = obj->ToPlayer())
                {
                    if (me->IsInCombatWith(obj->ToUnit()))
                        return true;

                    return !(p->isGameMaster() || !p->isGMVisible());
                }

                return false;
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!MoveEvent)
                {
                    //TC_LOG_ERROR("misc", "MoveInLineofSight: moveEvent is false.");
                    ScriptedAI::MoveInLineOfSight(who);
                }
                else
                {
                    if (who->ToPlayer())
                        if (!inMove)
                        {
                            //TC_LOG_ERROR("misc", "Nalorak Movephase3: %u", MovePhase);
                            switch (MovePhase)
                            {
                                case 0:
                                    if (me->IsWithinDistInMap(who, 50))
                                    {
                                        /*
                                            Our NPC is waiting at the start for a player to come into range.
                                        */
                                        MovePhase = 1;
                                        me->SetHomePosition(NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2], me->GetOrientation());
                                        //TC_LOG_ERROR("misc", "Valid MoveInLineOfSight Update, movephase increasing from %u to %u.", MovePhase, MovePhase+1);
                                        Talk(YELL_NALORAKK_WAVE1);                                       
                                        SendAttacker(who);
                                        waitTimer = 0;
                                        needMove = true;
                                    }
                                    break;
                                case 2:
                                    if (me->IsWithinDistInMap(who, 40))
                                    {
                                        /*
                                            Our NPC is waiting at the second wave at the top of the stairs.
                                        */
                                        MovePhase = 3;
                                        me->SetHomePosition(NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2], me->GetOrientation());
                                        //TC_LOG_ERROR("misc", "Valid MoveInLineOfSight Update, movephase increasing from %u to %u.", MovePhase, MovePhase+1);
                                        Talk(YELL_NALORAKK_WAVE2);                                      
                                        SendAttacker(who);

                                        waitTimer = 2000;
                                        needMove = true;
                                    }
                                    break;
                                case 5:
                                    if (me->IsWithinDistInMap(who, 40))
                                    {
                                        /*
                                            Our NPC is waiting at the third wave atop the stairs, has just sighted a player in range.
                                        */
                                        MovePhase = 6;
                                        me->SetHomePosition(NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2], me->GetOrientation());
                                        Talk(YELL_NALORAKK_WAVE3);                                       
                                        //TC_LOG_ERROR("misc", "Valid MoveInLineOfSight Update, movephase increasing from %u to %u.", MovePhase, MovePhase+1);
                                        SendAttacker(who);
                                        waitTimer = 2000;
                                        needMove = true;
                                    }
                                    break;
                                case 8:
                                    if (me->IsWithinDistInMap(who, 60))
                                    {
                                        /*
                                            Our NPC is waiting at his final boss spot.
                                        */
                                        Talk(YELL_NALORAKK_WAVE4);
                                        ReadyFight = true;  //This bool will cause the boss to remove all his immunity flags
                                        waitTimer = 8000;  //this Timer will decide when his immunity flags are removed.
                                        needMove = false;
                                        inMove = true;
                                        MovePhase = 9;
                                        me->SetHomePosition(NalorakkWay[int(MovePhase)][0], NalorakkWay[int(MovePhase)][1], NalorakkWay[int(MovePhase)][2], me->GetOrientation());
                                        SendAttacker(who);
                                    }
                                    break;
                            }
                        }
                        else
                        {
                            if (MovePhase == 0)
                            {
                                inMove = false;
                                //TC_LOG_ERROR("misc", "In MOve was true, setting inmove to false at MovePhase: %u", MovePhase);
                            }
                            else
                            {
                                //TC_LOG_ERROR("misc", "in move is true, MovePhase is: %u", MovePhase);
                            }
                        }
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                if (MovePhase < 7)
                    return;

                _EnterCombat();
                events.Reset();
                events.ScheduleEvent(EVENT_SURGE, 8000);
                events.ScheduleEvent(EVENT_BRUTAL_STRIKE, 13000);
                events.ScheduleEvent(EVENT_BEAR_FORM, 30000);
                events.ScheduleEvent(EVENT_BERSERK, 600000);

                Talk(YELL_AGGRO);
                DoZoneInCombat();
                MoveEvent = false;
                Position my_home{ -80.124603f, 1297.388306f, 49.155731f, 1.577466f };
                me->SetHomePosition(my_home);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(YELL_DEATH);

                if (auto hazlek = me->FindNearestCreature(NPC_HOSTAGE_2, 200.f, true))
                    hazlek->AI()->DoAction(2);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(YELL_KILL);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (MoveEvent)
                    if (inMove)
                {
                        //TC_LOG_ERROR("misc", "Nalorak MovementInform, MovePhase: %u, inMove: %u, id: %u, type: %u", MovePhase, inMove, id, type);
                    if (type != POINT_MOTION_TYPE)
                        return;

                    if (!inMove)
                        return;

                    if (MovePhase != id)
                    {
                        //TC_LOG_ERROR("misc", "Returning on Movephase != ID, MovePhase: %u, inMove: %u, id: %u, type: %u", MovePhase, inMove, id, type);
                        return;
                    }

                    switch (MovePhase)
                    {
                    case 1:
                    {
                        //TC_LOG_ERROR("misc", "Transit from Wave 1 to wave 2 is halfway complete. He needs one more waypoint.");
                        me->SetHomePosition(NalorakkWay[MovePhase + 1][0], NalorakkWay[MovePhase + 1][1], NalorakkWay[MovePhase + 1][2], me->GetOrientation());
                        waitTimer = 1000;
                        MovePhase = 2;
                        needMove = true;

                        return;
                        break;
                    }
                        case 2:
                        {
                            //TC_LOG_ERROR("misc", "Transit from Wave 1 to wave 2 is complete.");
                            me->SetFacingTo(0.0f);
                            inMove = false;
                            needMove = false;

                            return;
                            break;
                        }
                        case 3:
                        {
                            //TC_LOG_ERROR("misc", "This is the elbow in his movement before the second set of stairs. He is moving, but still needs another waypoint assigned.");
                            me->SetHomePosition(NalorakkWay[MovePhase + 1][0], NalorakkWay[MovePhase + 1][1], NalorakkWay[MovePhase + 1][2], me->GetOrientation());
                            waitTimer = 1000;
                            MovePhase = 4;
                            inMove = true;
                            needMove = true;

                            return;
                            break;
                        }
                        case 4:
                        {
                            //TC_LOG_ERROR("misc", "Commencing final leg of elbow up second stairs.");
                            me->SetHomePosition(NalorakkWay[MovePhase + 1][0], NalorakkWay[MovePhase + 1][1], NalorakkWay[MovePhase + 1][2], me->GetOrientation());
                            MovePhase = 5;
                            inMove = true;
                            needMove = true;

                            return;
                            break;
                        }
                        case 5://Transit C from Wave 2 to wave 3
                        {
                            //TC_LOG_ERROR("misc", "He has completed ascending the second set of stairs after the elbow.");
                            me->SetFacingTo(1.56f);
                            inMove = false;
                            needMove = false;

                            return;
                            break;
                        }
                        case 6://Transit from wave 3 to wave 4
                        {
                            //TC_LOG_ERROR("misc", "Boss has reached wave 4 from wave 3");
                            me->SetHomePosition(NalorakkWay[MovePhase + 1][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2], me->GetOrientation());
                            MovePhase = 7;
                            waitTimer = 0;
                            inMove = true;
                            needMove = true;
                            return;
                            break;
                        }
                        case 7://transit from wave 4 to top of steps
                        {
                            //TC_LOG_ERROR("misc", "Boss has reached final resting place for fight.");
                            MovePhase = 8;
                            inMove = false;
                            needMove = false;
                            me->SetFacingTo(1.56f);
                        }
                    }

                }
            }


            void UpdateAI(uint32 const diff)
            {
                if (needMove)
                {
                    if (waitTimer <= diff)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MovePoint(MovePhase, NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2]);
                        me->SetHomePosition(NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2], me->GetOrientation());

                        inMove = true;
                        needMove = false;
                    } else waitTimer -= diff;
                }
                else if (ReadyFight)
                    if (waitTimer <= diff)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        ReadyFight = false;
                        CommenceFight = true;
                    }
                    else waitTimer -= diff;

                if (!UpdateVictim())
                    if (!me->isInCombat())
                        if (auto player = me->FindNearestPlayer(15.0f))
                            if (me->ToUnit()->IsValidAttackTarget(player->ToUnit()))
                                if (CommenceFight)
                                {
                                    me->AI()->EnterCombat(player->ToUnit());
                                }
                                else
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SURGE:
                            if(Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0))
                                me->CastSpell(target, SPELL_SURGE, false);
                            events.ScheduleEvent(EVENT_SURGE, 8000);
                            break;
                        case EVENT_BRUTAL_STRIKE:
                            DoCastVictim(SPELL_BRUTAL_STRIKE);
                            events.ScheduleEvent(EVENT_BRUTAL_STRIKE, 8000);
                            break;
                        case EVENT_BEAR_FORM:
                            me->CastSpell(me, SPELL_BEAR_FORM, false);
                            events.CancelEvent(EVENT_SURGE);
                            events.CancelEvent(EVENT_BRUTAL_STRIKE);
                            events.ScheduleEvent(EVENT_LACERATING_SLASH, 1500);
                            events.ScheduleEvent(EVENT_REND_FLESH, 3000);
                            events.ScheduleEvent(EVENT_DEAFENING_ROAR, 11000);
                            events.ScheduleEvent(EVENT_NORMAL_FORM, 30000);
                            break;
                        case EVENT_LACERATING_SLASH:
                            DoCastVictim(SPELL_LACERATING_SLASH);
                            break;
                        case EVENT_REND_FLESH:
                            DoCastVictim(SPELL_REND_FLESH);
                            events.ScheduleEvent(EVENT_REND_FLESH, 10000);
                            break;
                        case EVENT_DEAFENING_ROAR:
                            me->CastSpell(me, SPELL_DEAFENING_ROAR, false);
                            events.ScheduleEvent(EVENT_DEAFENING_ROAR, 10000);
                            break;
                        case EVENT_NORMAL_FORM:
                            me->RemoveAura(SPELL_BEAR_FORM);
                            events.CancelEvent(EVENT_LACERATING_SLASH);
                            events.CancelEvent(EVENT_REND_FLESH);
                            events.CancelEvent(EVENT_DEAFENING_ROAR);
                            events.ScheduleEvent(EVENT_SURGE, 2000);
                            events.ScheduleEvent(EVENT_BRUTAL_STRIKE, 7000);
                            events.ScheduleEvent(EVENT_BEAR_FORM, 30000);       // 22:32:02.015
                            events.ScheduleEvent(EVENT_SURGE, 10000);
                            events.ScheduleEvent(EVENT_BRUTAL_STRIKE, 15000);
                            events.ScheduleEvent(EVENT_BEAR_FORM, 30000);
                            break;
                        case EVENT_BERSERK:
                            me->CastSpell(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_ACTIVATE:
                            //TC_LOG_ERROR("misc", "Removing Flags");
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                InstanceScript* instance;
                EventMap events;
                bool MoveEvent;
                bool inMove;
                bool needMove;
                bool ReadyFight;
                bool CommenceFight;
                uint32 MovePhase;
                uint32 waitTimer;
        };
};


class mob_nalorakk_preTrash : public CreatureScript
{
public:
    mob_nalorakk_preTrash() : CreatureScript("mob_nalorakk_preTrash") { }

    struct mob_nalorakk_preTrashAI : public ScriptedAI
    {
        mob_nalorakk_preTrashAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* who)
        {
            switch (me->GetEntry())
            {
            case NPC_AMANASHI_MEDICINE_MAN:
                trash_events.ScheduleEvent(EVENT_CHAIN_HEAL, urand(5000, 15000));
                trash_events.ScheduleEvent(EVENT_HEALING_WARD, urand(15000, 35000));
                trash_events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(5000, 10000));
                trash_events.ScheduleEvent(EVENT_PROTECTIVE_WARD, urand(15000, 35000));
                break;
            case NPC_AMANASHI_TRIBESMAN:
                trash_events.ScheduleEvent(EVENT_CYCLONE_STRIKE, urand(5000, 15000));
                trash_events.ScheduleEvent(EVENT_KICK, urand(5000, 15000));
                break;
            case NPC_AMANISHI_AXETHROWER:
                trash_events.ScheduleEvent(EVENT_KNOCKDOWN, urand(5000, 15000));
                trash_events.ScheduleEvent(EVENT_RAPTOR_STRIKE, urand(5000, 15000));
                break;
            case NPC_AMANASHI_WARBRINGER:
                trash_events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 15000));
                trash_events.ScheduleEvent(EVENT_FRENZY, urand(15000, 35000));
                trash_events.ScheduleEvent(EVENT_FURIOUS_ROAR, urand(15000, 35000));
                break;
            }
        }
        void DoAction(int32 const action)
        {
            switch (action)
            {
            case ACTION_NALORAKK_ATTACK_ORDER_1:
                me->SetHomePosition(Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_JUMP_DEST]);
                trash_events.ScheduleEvent(EVENT_MOVE_START_1, urand(0, 2500));
                break;
            case ACTION_NALORAKK_ATTACK_ORDER_2:
                if (auto player = me->FindNearestPlayer(150.0f))
                    me->AI()->AttackStart(player->ToUnit());
                break;
            default:
                break;
            }
        }
        void InitializeAI() override
        {
            if (me->GetEntry() == NPC_AMANASHI_WARBRINGER)
                me->Mount(22467);
        }
        void JustDied(Unit* /*killer*/)
        {
            if (me->GetEntry() == NPC_AMANASHI_WARBRINGER)
            {
                HasSummonedBear = true;
                if (me->IsMounted())
                    me->Dismount();
            }
        }

        void Reset()
        {
            trash_events.Reset();
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (me->GetEntry() == NPC_AMANASHI_WARBRINGER)
                if (HealthBelowPct(30))
                    if (me->IsMounted())
                        if (!HasSummonedBear)
                        {
                            HasSummonedBear = true;
                            me->Dismount();
                            me->CastSpell(attacker, 43274, false);
                        }
        }
        void UpdateAI(uint32 const diff)
        {
            trash_events.Update(diff);

            if (uint32 eventId = trash_events.ExecuteEvent())
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    trash_events.ScheduleEvent(eventId, 250);
                else
                switch (eventId)
                {
                case EVENT_MOVE_START_1:
                    me->GetMotionMaster()->MovePoint(0, Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_PREJUMP_START].GetPositionX(),
                        Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_PREJUMP_START].GetPositionY(),
                        Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_PREJUMP_START].GetPositionZ(), true, 5.0f, false);
                    trash_events.ScheduleEvent(EVENT_JUMP_1, 1000);
                    break;
                case EVENT_JUMP_1:
                    me->GetMotionMaster()->MoveJump(Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_JUMP_DEST].GetPositionX(),
                        Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_JUMP_DEST].GetPositionY(),
                        Nalorakk_Positions[POSITION_NAL_FIRST_SIGHT_JUMP_DEST].GetPositionZ(), 10.0f, 10.0f);
                    trash_events.ScheduleEvent(EVENT_ATTACK_1, 3000);
                    break;
                case EVENT_ATTACK_1:
                    if (auto player = me->FindNearestPlayer(50.0f))
                        me->AI()->AttackStart(player->ToUnit());
                    break;
                case EVENT_CHAIN_HEAL:
                {
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_TRIBESMAN,    25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_WARBRINGER,   25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANISHI_AXETHROWER,   25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANI_BEAR_MOUNT,      25.f);
                    if (list_of_npcs.size())
                    {
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto t = (*itr))
                                if (!t->isInCombat())//filter condition
                                    list_of_npcs.erase(itr);

                        list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                        if (auto target = list_of_npcs.front())
                            me->CastSpell(target, 42477);
                    }
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_HEALING_WARD:
                {
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_TRIBESMAN, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_WARBRINGER, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANISHI_AXETHROWER, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANI_BEAR_MOUNT, 25.f);
                    if (list_of_npcs.size())
                    {
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto t = (*itr))
                                if (!t->isInCombat())//filter condition
                                    list_of_npcs.erase(itr);

                        list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                        if (auto target = list_of_npcs.front())
                            me->CastSpell(target, 42376);
                    }
                    trash_events.ScheduleEvent(eventId, urand(35000, 45000));
                    break;
                }
                case EVENT_LIGHTNING_BOLT:
                {
                    DoCastRandom(97474, 25.f);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_PROTECTIVE_WARD:
                {
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_TRIBESMAN, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANASHI_WARBRINGER, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANISHI_AXETHROWER, 25.f);
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_AMANI_BEAR_MOUNT, 25.f);
                    if (list_of_npcs.size())
                    {
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto t = (*itr))
                                if (!t->isInCombat())//filter condition
                                    list_of_npcs.erase(itr);

                        list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                        if (auto target = list_of_npcs.front())
                            me->CastSpell(target, 42478);
                    }
                    trash_events.ScheduleEvent(eventId, urand(35000, 45000));
                    break;
                }
                case EVENT_CYCLONE_STRIKE:
                {
                    DoCastVictim(42495);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_KICK:
                {
                    DoCastVictim(36033);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_KNOCKDOWN:
                {
                    DoCastVictim(35011);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_RAPTOR_STRIKE:
                {
                    DoCastVictim(31566);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_CLEAVE:
                {
                    DoCastVictim(43273);
                    trash_events.ScheduleEvent(eventId, urand(5000, 15000));
                    break;
                }
                case EVENT_FRENZY:
                {
                    DoCast(40743);
                    trash_events.ScheduleEvent(eventId, urand(25000, 35000));
                    break;
                }
                case EVENT_FURIOUS_ROAR:
                {
                    DoCast(42496);
                    trash_events.ScheduleEvent(eventId, urand(25000, 35000));
                    break;
                }
                default:
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        bool HasSummonedBear = false;
        EventMap trash_events;
        Creature bear;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_nalorakk_preTrashAI(creature);
    }
};

void AddSC_boss_nalorakk()
{
    new mob_nalorakk_preTrash();
    new boss_nalorakk();
}
