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

/* ScriptData
SDName: Guards
SD%Complete: 100
SDComment:
SDCategory: Guards
EndScriptData */

/* ContentData
guard_generic
guard_shattrath_aldor
guard_shattrath_scryer
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GuardAI.h"
#include "Player.h"
#include "SpellInfo.h"
#include "WaypointMovementGenerator.h"
#include "WaypointManager.h"

enum GuardGeneric
{
    GENERIC_CREATURE_COOLDOWN       = 5000,

    SAY_GUARD_SIL_AGGRO             = 0,

    NPC_CENARION_HOLD_INFANTRY      = 15184,
    NPC_STORMWIND_CITY_GUARD        = 68,
    NPC_STORMWIND_CITY_PATROLLER    = 1976,
    NPC_ORGRIMMAR_GRUNT             = 3296
};

enum guard_waypoint_actions
{
    ACTION_STOP = 1,
    ACTION_RESUME,
    ACTION_SAVE
};

class guard_generic : public CreatureScript
{
public:
    guard_generic() : CreatureScript("guard_generic") { }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pPlayer)
        {
            if (pCreature->GetWaypointPath())
                pCreature->AI()->DoAction(65);
            pPlayer->PrepareGossipMenu(pCreature, pCreature->GetCreatureTemplate()->GossipMenuId, true);
            pPlayer->SendPreparedGossip(pCreature);
        }

        return true;
    }
        

    struct guard_genericAI : public GuardAI
    {
        guard_genericAI(Creature* creature) : GuardAI(creature) {}

        void Reset()
        {
            globalCooldown = 0;
            buffTimer = 0;
            DoAction(55);
        }

        void InitializeAI()
        {
        }

        void HandleWaypointAction(uint32 action, uint32 data = 0)
        {
            if (me->GetWaypointPath())
            switch (action)
            {
            case ACTION_STOP:
                progress_wp = me->GetCurrentWaypointID();
                me->ClearUnitState(UNIT_STATE_MOVING);
                me->NearTeleportTo(*me);
                DoAction(55);
                break;
            case ACTION_RESUME:
                if (me->GetWaypointPath())
                {
                    me->AddUnitState(UNIT_STATE_MOVING);
                    me->ClearUnitState(UNIT_STATE_NOT_MOVE);
                    //me->GetMotionMaster()->InitDefault();
                    me->GetMotionMaster()->MoveTargetedHome();
                }
                break;
            case ACTION_SAVE:
                /*
                */
                break;
            default:
                break;
            }
        }

        void EnterCombat(Unit* who)
        {
            if (me->GetEntry() == NPC_CENARION_HOLD_INFANTRY)
                Talk(SAY_GUARD_SIL_AGGRO, who->GetGUID());
            if (SpellInfo const* spell = me->reachWithSpellAttack(who))
                DoCast(who, spell->Id);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
            case 55:
                if (!events.HasEvent(1))
                    events.ScheduleEvent(1, 4000);
                break;
            case 65:
                HandleWaypointAction(ACTION_STOP);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                    if (me->FindNearestPlayer(10.f) || me->isInCombat())    events.ScheduleEvent(1, 4000);
                    else
                    {
                        HandleWaypointAction(ACTION_RESUME);
                    }
                    break;
                default:
                    break;
                }


            //Always decrease our global cooldown first
            if (globalCooldown > diff)
                globalCooldown -= diff;
            else
                globalCooldown = 0;

            //Buff timer (only buff when we are alive and not in combat
            if (me->isAlive() && !me->isInCombat())
            {
                if (buffTimer <= diff)
                {
                    //Find a spell that targets friendly and applies an aura (these are generally buffs)
                    SpellInfo const* info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_AURA);

                    if (info && !globalCooldown)
                    {
                        //Cast the buff spell
                        DoCast(me, info->Id);

                        //Set our global cooldown
                        globalCooldown = GENERIC_CREATURE_COOLDOWN;

                        //Set our timer to 10 minutes before rebuff
                        buffTimer = 600000;
                    }                                                   //Try again in 30 seconds
                    else buffTimer = 30000;
                }
                else buffTimer -= diff;
            }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            // Make sure our attack is ready and we arn't currently casting
            if (me->isAttackReady() && !me->IsNonMeleeSpellCasted(false))
            {
                //If we are within range melee the target
                if (me->IsWithinMeleeRange(me->getVictim()))
                {
                    bool healing = false;
                    SpellInfo const* info = NULL;

                    //Select a healing spell if less than 30% hp
                    if (me->HealthBelowPct(30))
                        info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

                    //No healing spell available, select a hostile spell
                    if (info)
                        healing = true;
                    else
                        info = SelectSpell(me->getVictim(), 0, 0, SELECT_TARGET_ANY_ENEMY, 0, 0, 0, 0, SELECT_EFFECT_DONTCARE);

                    //20% chance to replace our white hit with a spell
                    if (info && urand(0, 99) < 20 && !globalCooldown)
                    {
                        //Cast the spell
                        if (healing)
                            DoCast(me, info->Id);
                        else
                            DoCastVictim(info->Id);

                        //Set our global cooldown
                        globalCooldown = GENERIC_CREATURE_COOLDOWN;
                    }
                    else
                        me->AttackerStateUpdate(me->getVictim());

                    me->resetAttackTimer();
                }
            }
            else
            {
                //Only run this code if we arn't already casting
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    bool healing = false;
                    SpellInfo const* info = NULL;

                    //Select a healing spell if less than 30% hp ONLY 33% of the time
                    if (me->HealthBelowPct(30) && 33 > urand(0, 99))
                        info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

                    //No healing spell available, See if we can cast a ranged spell (Range must be greater than ATTACK_DISTANCE)
                    if (info)
                        healing = true;
                    else
                        info = SelectSpell(me->getVictim(), 0, 0, SELECT_TARGET_ANY_ENEMY, 0, 0, NOMINAL_MELEE_RANGE, 0, SELECT_EFFECT_DONTCARE);

                    //Found a spell, check if we arn't on cooldown
                    if (info && !globalCooldown)
                    {
                        //If we are currently moving stop us and set the movement generator
                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveIdle();
                        }

                        //Cast spell
                        if (healing)
                            DoCast(me, info->Id);
                        else
                            DoCastVictim(info->Id);

                        //Set our global cooldown
                        globalCooldown = GENERIC_CREATURE_COOLDOWN;
                    }                                               //If no spells available and we arn't moving run to target
                    else if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        //Cancel our current spell and then mutate new movement generator
                        me->InterruptNonMeleeSpells(false);

                        if (auto v = me->getVictim())
                            AttackStart(v);
                    }
                }
            }

            if (me->HasReactState(REACT_AGGRESSIVE))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
        }

        void DoReplyToTextEmote(uint32 emote)
        {
            switch (emote)
            {
                case TEXT_EMOTE_KISS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    break;

                case TEXT_EMOTE_WAVE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    break;

                case TEXT_EMOTE_SALUTE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    break;

                case TEXT_EMOTE_SHY:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
                    break;

                case TEXT_EMOTE_RUDE:
                case TEXT_EMOTE_CHICKEN:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    break;
            }
        }

        void ReceiveEmote(Player* player, uint32 textEmote)
        {
            switch (me->GetEntry())
            {
                case NPC_STORMWIND_CITY_GUARD:
                case NPC_STORMWIND_CITY_PATROLLER:
                case NPC_ORGRIMMAR_GRUNT:
                    break;
                default:
                    return;
            }

            if (!me->IsFriendlyTo(player))
                return;

            DoReplyToTextEmote(textEmote);
        }

    private:
        uint32 progress_wp{ 0 };
        uint32 globalCooldown;
        uint32 buffTimer;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
       return new guard_genericAI(creature);
    }
};

enum GuardShattrath
{
    SPELL_BANISHED_SHATTRATH_A = 36642,
    SPELL_BANISHED_SHATTRATH_S = 36671,
    SPELL_BANISH_TELEPORT      = 36643,
    SPELL_EXILE                = 39533
};

class guard_shattrath_scryer : public CreatureScript
{
public:
    guard_shattrath_scryer() : CreatureScript("guard_shattrath_scryer") { }

    struct guard_shattrath_scryerAI : public GuardAI
    {
        guard_shattrath_scryerAI(Creature* creature) : GuardAI(creature) {}

        void Reset()
        {
            banishTimer = 5000;
            exileTimer = 8500;
            playerGUID = 0;
            canTeleport = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (canTeleport)
            {
                if (exileTimer <= diff)
                {
                    if (auto temp = Unit::GetUnit(*me, playerGUID))
                    {
                        temp->CastSpell(temp, SPELL_EXILE, true);
                        temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                    }
                    playerGUID = 0;
                    exileTimer = 8500;
                    canTeleport = false;
                } else exileTimer -= diff;
            }
            else if (banishTimer <= diff)
            {
                Unit* temp = me->getVictim();
                if (temp && temp->GetTypeId() == TYPEID_PLAYER)
                {
                    DoCast(temp, SPELL_BANISHED_SHATTRATH_A);
                    banishTimer = 9000;
                    playerGUID = temp->GetGUID();
                    if (playerGUID)
                        canTeleport = true;
                }
            } else banishTimer -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 exileTimer;
        uint32 banishTimer;
        uint64 playerGUID;
        bool canTeleport;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new guard_shattrath_scryerAI(creature);
    }
};

class guard_shattrath_aldor : public CreatureScript
{
public:
    guard_shattrath_aldor() : CreatureScript("guard_shattrath_aldor") { }

    struct guard_shattrath_aldorAI : public GuardAI
    {
        guard_shattrath_aldorAI(Creature* creature) : GuardAI(creature) {}

        void Reset()
        {
            banishTimer = 5000;
            exileTimer = 8500;
            playerGUID = 0;
            canTeleport = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (canTeleport)
            {
                if (exileTimer <= diff)
                {
                    if (auto temp = Unit::GetUnit(*me, playerGUID))
                    {
                        temp->CastSpell(temp, SPELL_EXILE, true);
                        temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                    }
                    playerGUID = 0;
                    exileTimer = 8500;
                    canTeleport = false;
                } else exileTimer -= diff;
            }
            else if (banishTimer <= diff)
            {
                Unit* temp = me->getVictim();
                if (temp && temp->GetTypeId() == TYPEID_PLAYER)
                {
                    DoCast(temp, SPELL_BANISHED_SHATTRATH_S);
                    banishTimer = 9000;
                    playerGUID = temp->GetGUID();
                    if (playerGUID)
                        canTeleport = true;
                }
            } else banishTimer -= diff;

            DoMeleeAttackIfReady();
        }
    private:
        uint32 exileTimer;
        uint32 banishTimer;
        uint64 playerGUID;
        bool canTeleport;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new guard_shattrath_aldorAI(creature);
    }
};

void AddSC_guards()
{
    new guard_generic;
    new guard_shattrath_aldor;
    new guard_shattrath_scryer;
}
