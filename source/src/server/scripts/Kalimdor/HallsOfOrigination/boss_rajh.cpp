/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "halls_of_origination.h"

enum Texts
{
    SAY_AGGRO       = 0,
    SAY_ENERGIZE    = 1,
    SAY_SLAY        = 2,
    SAY_DEATH       = 3
};

enum Spells
{
    //Rajh
    SPELL_SUN_STRIKE                = 73872,
    SPELL_INFERNO_JUMP              = 87653,
    SPELL_INFERNO_JUMP_EXPLOSION    = 87647,
    SPELL_INFERNO_JUMP_VISUAL       = 87645,
    SPELL_INFERNO_JUMP_TRIGGER      = 87650,

    SPELL_SUMMON_SOLAR_WIND         = 74104,
    SPELL_SUMMON_SUN_ORB            = 80352,
    SPELL_BLAZING_INFERNO           = 76195, //casted by orb at destination
    SPELL_BLESSING_OF_THE_SUN       = 76352,
    //Solar Wind
    SPELL_FIRE_VORTEX               = 74109,
    SPELL_SOLAR_FIRE_AURA           = 74107,
    SPELL_SUMMON_SOLAR_FIRE         = 89130,
    //Solar Fire Mob
    SPELL_SOLAR_FIRE                = 89131,
    //Sun Orb
    SPELL_SUN                       = 73658,
    SPELL_SUN_SUN_STRIKE            = 73874,
    SPELL_CHARGE                    = 82856,
    //Sun-Touched Servant
    SPELL_DISPERSION                = 88097,
    //Sun-Touched Speaker
    SPELL_STOMP                     = 75241,
    SPELL_FIRE_STORM                = 73861
};

enum Events
{
    // Rajh
    EVENT_SUN_STRIKE            = 1,
    EVENT_INFERNO_JUMP          = 2,
    EVENT_BLESSING_OF_THE_SUN   = 3,
    EVENT_SUMMON_SUN_ORB        = 4,
    EVENT_SUMMON_SOLAR_WIND     = 5,
    EVENT_ENERGIZE              = 6,
    // Inferno Leap
    EVENT_LEAP_EXPLOSION        = 7,
    // Solar Wind
    EVENT_MOVE_ARROUND          = 8,
    EVENT_SUMMON_FIRE           = 9,
    // Sun Orb
    EVENT_MOVE_UP               = 10,
    EVENT_MOVE_DOWN             = 11,
    EVENT_DESPAWN               = 12,
    // Fire Elementars
    EVENT_SUMMON_2              = 13,
    EVENT_SUMMON_3              = 14,
    EVENT_KILL_2                = 15,
    EVENT_FLAME_WAVE            = 16,
    EVENT_SEARING_FLAME         = 17,
    // Sun Touched Speaker
    EVENT_STOMP                 = 18,
    EVENT_FIRE_STORM            = 19,
    EVENT_SUMMON_SUN_ORB_RECOVER = 20,
    EVENT_POWER_CHECK,
    EVENT_WIPE_CHECK,
    EVENT_ENERGIZE_RECOVER,
    EVENT_MAZE_PUNISH,
    EVENT_BACKUP_ROTATION_RESET,
};

enum Points
{
    POINT_CENTER    = 1,
    POINT_UP        = 2,
    POINT_SUN_EXP   = 3
};

Position const CenterPos    = {-319.455f, 193.440f, 343.946f, 3.133f};
Position const SunPoint     = {-317.685f, 192.550f, 379.702f, 0.0f};

struct boss_rajh : public BossAI
{
    boss_rajh(Creature* creature) : BossAI(creature, DATA_RAJH)
    {
        Achievement = true;
        Energized   = true;
    }

    bool Achievement;
    bool Energized;
    bool Moving{ false };
    bool Leaping{ false };

    void InitializeAI()
    {
        me->setPowerType(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
        me->ModifyPower(POWER_ENERGY, 100);
    }
    void DoAction(int32 const act)
    {
        switch (act)
        {
        case EVENT_INFERNO_JUMP:
            Leaping = true;
            ////TC_LOG_ERROR("sql.sql", "Leaping set to: %u", Leaping);
            break;
        default:
            break;
        }
    }

    void Reset()
    {

        me->SetReactState(REACT_AGGRESSIVE);
        me->ClearUnitState(UNIT_STATE_ROOT);
        _Reset();
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
        //DoCast(me, 69470, true);
        me->setPowerType(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 100);
        me->ModifyPower(POWER_ENERGY, 100);
        me->HandleEmoteCommand(EMOTE_STATE_NONE);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        Achievement = true;
        Energized   = true;
        Leaping = false;
        ////TC_LOG_ERROR("sql.sql", "Leaping set to: %u", Leaping);
        std::list<Creature*> creatureList;
        me->GetCreatureListWithEntryInGrid(creatureList, 47922, 200.0f);

        if (creatureList.size())
        for (auto itr = creatureList.begin(); itr != creatureList.end(); ++itr)
            if (auto c = (*itr))
                c->DespawnOrUnsummon();
    }

    bool checkWipe()
    {
        auto players = me->GetPlayersInRange(200.f, true);
        for (Player* player : players)
            if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                return false;
        return true;
    }

    void EnterCombat(Unit* /*who*/)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
        _EnterCombat();
        Talk(SAY_AGGRO);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        events.ScheduleEvent(EVENT_MAZE_PUNISH, 1000);

        /*
            Rajh combat:
            Sun strike 10s then every 25s

            Inferno jump (weight x1) 
            OR Sun Orb (weight x2) 
            OR Summon Solar wind (Weight x2) 
            ->
                every 25s.
        */
        events.ScheduleEvent(EVENT_SUN_STRIKE, 10000);    //sniffed
        ScheduleNewRotation(false);
    }

    void JustSummoned(Creature* summon)
    {
        if (summon->GetEntry() != NPC_INFERNO_TRIGGER)
        BossAI::JustSummoned(summon);

        switch (summon->GetEntry())
        {
            case NPC_INFERNO_TRIGGER:
                summon->SetReactState(REACT_PASSIVE);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
                summon->CastSpell(summon, SPELL_INFERNO_JUMP_VISUAL, true);
                summon->DespawnOrUnsummon(3000);
                break;
            case NPC_SOLAR_FIRE:
                summon->CastSpell(summon, 74106, true);
                break;
            case NPC_SUN_ORB:
                me->ModifyPower(POWER_ENERGY, -20);                                                 //20 ENERGY COST
                break;
            default:
                break;
        }
    }
    void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
    {

        Spell* curSpell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL);
        if(curSpell && curSpell->m_spellInfo->Id == SPELL_INFERNO_JUMP)
            for (uint8 i = 0; i < 3; ++i)
                if(spell && spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
                    if (auto trigger = me->FindNearestCreature(NPC_INFERNO_TRIGGER, 200.0f))
                        trigger->DespawnOrUnsummon();
                }
    }
    void SpellHitTarget(Unit* /*target*/, SpellInfo const* spellinfo)
    {
    }
    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_SLAY);
    }
    void MovementInform(uint32 type, uint32 pointId)
    {
        //16, 1004
        if (type == EFFECT_MOTION_TYPE)
            if (pointId == 1004)
            {
                //if (spellinfo->Id == SPELL_INFERNO_JUMP)
                //TC_LOG_ERROR("sql.sql", "MOTION_TYPE: %u, with id %u", type, pointId);
                me->ModifyPower(POWER_ENERGY, -20);                                         //20 ENERGY COST
                DoCast(SPELL_INFERNO_JUMP_EXPLOSION);
                Leaping = false;
                if (auto c = me->FindNearestCreature(SPELL_INFERNO_JUMP_TRIGGER, 10.f))
                    c->DespawnOrUnsummon(3000);
                return;
            }

        if (type == POINT_MOTION_TYPE)
        switch (pointId)
        {
            case POINT_CENTER:
                Moving = false;
                me->SetFacingTo(M_PI);
                me->AddUnitState(UNIT_STATE_ROOT);
                events.ScheduleEvent(EVENT_ENERGIZE, 1000);
                break;
            default:
                break;
        }
    }
    
    void ScheduleNewRotation(bool freeze_reset)
    {
        uint32 newEvent {   GetEventForPower(me->GetPower(POWER_ENERGY))    };

        events.CancelEvent(newEvent);
        events.ScheduleEvent(newEvent,15000);    //sniffed
        events.CancelEvent(EVENT_BACKUP_ROTATION_RESET);
        events.CancelEvent(EVENT_BACKUP_ROTATION_RESET);
        events.CancelEvent(EVENT_BACKUP_ROTATION_RESET);
        events.ScheduleEvent(EVENT_BACKUP_ROTATION_RESET, 18000);

        if (freeze_reset)
        {
            me->SetReactState(REACT_AGGRESSIVE);
            UpdateVictim();
        }
    }

    uint32 GetEventForPower(int32 power_level)
    {
        uint32 selected_event{ EVENT_SUMMON_SUN_ORB };//default to be safe

        if (power_level == 100)
        {
            selected_event = RAND(EVENT_INFERNO_JUMP,       EVENT_INFERNO_JUMP,         EVENT_INFERNO_JUMP,         EVENT_INFERNO_JUMP,         EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SOLAR_WIND);
            return selected_event;
        }
        if (power_level >= 80)
        {
            selected_event = RAND(EVENT_SUMMON_SUN_ORB,     EVENT_SUMMON_SUN_ORB,       EVENT_SUMMON_SUN_ORB,       EVENT_SUMMON_SUN_ORB,       EVENT_INFERNO_JUMP,         EVENT_INFERNO_JUMP);
            return selected_event;
        }
        if (power_level >= 60)
        {
            selected_event = RAND(EVENT_INFERNO_JUMP,       EVENT_INFERNO_JUMP,         EVENT_INFERNO_JUMP,         EVENT_INFERNO_JUMP,         EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SUN_ORB);
            return selected_event;
        }
        if (power_level >= 40)
        {
            selected_event = RAND(EVENT_SUMMON_SOLAR_WIND,  EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SUN_ORB,       EVENT_SUMMON_SUN_ORB);
            return selected_event;
        }
        if (power_level >= 20)
        {
            selected_event = RAND(EVENT_SUMMON_SUN_ORB,     EVENT_SUMMON_SUN_ORB,       EVENT_SUMMON_SUN_ORB,       EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SOLAR_WIND,    EVENT_SUMMON_SOLAR_WIND);
            return selected_event;
        }
        return selected_event;
    }

    void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
    {
        switch (spell->Id)
        {
        /*
            Inferno Leap
        */
        case SPELL_INFERNO_JUMP:
            if (reason == SPELL_FINISHED_CANCELED)//someone kicked the leap
            {
                //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: Inferno leap");
                Leaping = false;
                if (PowerCheck(Energized))
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                    ScheduleNewRotation(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();
                }
            }
            break;
        case 87647://success
        case 89876:
            //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: Inferno leap");
            Leaping = false;
            if (PowerCheck(Energized))
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                ScheduleNewRotation(false);
                me->SetReactState(REACT_AGGRESSIVE);
                UpdateVictim();
            }
            break;
        /*
            Blessing of the sun
        */
        case 76355:
        case 89879:
            //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: Blessing of the sun (Periodic)");
            if (PowerCheck(Energized))
            {
                events.ScheduleEvent(EVENT_SUN_STRIKE, 15000);    //sniffed
                ScheduleNewRotation(false);
                me->SetReactState(REACT_AGGRESSIVE);
                UpdateVictim();
            }
            else
            {
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 474);
                    me->SetFacingTo(M_PI);
            }
            break;
        /*
            Summon Sun Orb
        */
        case SPELL_SUMMON_SUN_ORB:
            if (reason != SPELL_FINISHED_SUCCESSFUL_CAST)
            {
                //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: SPELL_SUMMON_SUN_ORB");
                me->ClearUnitState(UNIT_STATE_ROOT);
                if (PowerCheck(Energized))
                {
                    ScheduleNewRotation(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();
                }
            }
             break;
        /*
            Solar wind
        */
        case SPELL_SUMMON_SOLAR_WIND:
            //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: SPELL_SUMMON_SOLAR_WIND");
            if (reason == SPELL_FINISHED_SUCCESSFUL_CAST)
            if (PowerCheck(Energized))
                ScheduleNewRotation(false);
            break;
        /*
            Sun strike
        */
        case SPELL_SUN_STRIKE:
        case 89887: //HC sun strike
            //TC_LOG_ERROR("sql.sql", "SPELL CAST FINISHED: SPELL_SUN_STRIKE");
            if (PowerCheck(Energized))
            events.ScheduleEvent(EVENT_SUN_STRIKE, 25000);    //sniffed
            break;
        default:
            ////TC_LOG_ERROR("sql.sql", "unlisted spell cast completed: %u with reason %u", spell->Id, uint32(reason));
            break;
        }
    }
    /*
        This function's intention is to prevent multiple fire triggers from hurting players at the same time.
    */

    void Punish_Venom_Maze()
    {
        auto players = me->GetPlayersInRange(100.f, true);
        if (players.size())
            for (Player* player : players)
                if (auto gas = player->FindNearestCreature(NPC_SOLAR_WIND, 1.f, true))
                    if (gas->HasAura(SPELL_SOLAR_FIRE))
                        gas->AI()->DoCast(player, IsHeroic() ? 89878 : 89133, true);
    }
    /*
        This function will serve as a check as to whether or not we are good to proceed the fight based on current power level.
        If the npc is in an "energized" state, it is looking for permission to perform another attack.
        If the npc is a non-"energized" state, it is looking for permission to exit the recharge channel and resume the fight.
    */
    bool PowerCheck(bool energized)
    {
        if (energized)
        {
            if (me->GetPower(POWER_ENERGY) < 20)
            {
                ////TC_LOG_ERROR("sql.sql", "resetting events for EVENT_POWER_CHECK");
                Energized = false;
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->CastStop(true);
                events.Reset();
                events.ScheduleEvent(EVENT_MAZE_PUNISH, 1000);
                events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                if (!Moving)
                {
                    Moving = true;
                    me->StopMoving();
                    me->GetMotionMaster()->MovePoint(POINT_CENTER, CenterPos, true);
                }
                //TC_LOG_ERROR("sql.sql", "PowerCheck(%u) returning false.", energized);
                return false;
            }
            else
            {
                //TC_LOG_ERROR("sql.sql", "PowerCheck(%u) returning true", energized);
                return true;
            }
        }
        else
        {
            if (me->GetPower(POWER_ENERGY) >= 100)
            {
                if (me->GetPower(POWER_ENERGY) > 100)
                    me->SetPower(POWER_ENERGY, 100);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetReactState(REACT_AGGRESSIVE);
                UpdateVictim();
                Energized = true;
                Achievement = false;
                me->RemoveAurasDueToSpell(SPELL_BLESSING_OF_THE_SUN);
                //TC_LOG_ERROR("sql.sql", "PowerCheck(%u) returning true", energized);
                return true;
            }
            else
            {
                //TC_LOG_ERROR("sql.sql", "PowerCheck(%u) returning false, energy at %u of 100", energized, me->GetPower(POWER_ENERGY));
                return false;
            }
        }
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
            case EVENT_BACKUP_ROTATION_RESET:
                if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                else
                {
                    if (!events.HasEvent(EVENT_INFERNO_JUMP))
                        if (!events.HasEvent(EVENT_SUMMON_SUN_ORB))
                            if (!events.HasEvent(EVENT_SUMMON_SOLAR_WIND))
                                ScheduleNewRotation(true);

                    if (!events.HasEvent(EVENT_BACKUP_ROTATION_RESET))
                        events.ScheduleEvent(EVENT_BACKUP_ROTATION_RESET, 8000);
                }
                break;
                case EVENT_MAZE_PUNISH:
                    Punish_Venom_Maze();
                    events.ScheduleEvent(EVENT_MAZE_PUNISH, 1000);
                    break;
                case EVENT_INFERNO_JUMP:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing EVENT_INFERNO_JUMP");
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        {
                            DoCast(target, SPELL_INFERNO_JUMP_TRIGGER, false);
                            if (auto trigger = me->FindNearestCreature(NPC_INFERNO_TRIGGER, 200.0f))
                            {
                                lock_orientation = me->GetAngle(trigger);
                                me->SetReactState(REACT_PASSIVE);
                                me->StopMoving();
                                me->AttackStop();
                                me->ModifyPower(POWER_ENERGY, -10);                                                 //20 ENERGY COST
                                //Leaping = true;
                                DoCast(trigger, SPELL_INFERNO_JUMP);    //dummy
                                me->SetFacingTo(lock_orientation);
                            }
                        }
                    }
                    break;
                case EVENT_SUMMON_SUN_ORB:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing EVENT_SUMMON_SUN_ORB");
                        //Energy is removed when the npc is actually spawned.

                        if (auto v = me->getVictim())
                            lock_orientation = me->GetAngle(v);

                        me->SetReactState(REACT_PASSIVE);
                        me->StopMoving();
                        me->AttackStop();
                        DoCast(SPELL_SUMMON_SUN_ORB);
                            me->SetFacingTo(lock_orientation);
                    }
                    break;
                case EVENT_SUMMON_SOLAR_WIND:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing EVENT_SUMMON_SOLAR_WIND");
                        DoCast(SPELL_SUMMON_SOLAR_WIND);
                    }
                    break;
                case EVENT_SUN_STRIKE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing EVENT_SUN_STRIKE");
                            DoCastVictim(SPELL_SUN_STRIKE);
                    }
                    break;
                case EVENT_ENERGIZE:
                    Talk(SAY_ENERGIZE);
                    if (!Energized)
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing EVENT_ENERGIZE");
                        DoCastAOE(SPELL_BLESSING_OF_THE_SUN);
                    }
                    break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->CombatStop(true);
                        me->DeleteThreatList();
                        EnterEvadeMode();
                        me->Respawn(true);
                            Reset();
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
            }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (!me->HasReactState(REACT_PASSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*who*/)
    {
        _JustDied();
        Talk(SAY_DEATH);
        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        instance->DoCompleteAchievement(4841);
        if (IsHeroic())
        {
            instance->DoCompleteAchievement(5065);
            if (Achievement)
                instance->DoCompleteAchievement(5295);
        }

        std::list<Creature*> creatureList;
        me->GetCreatureListWithEntryInGrid(creatureList, 47922, 100.0f);

        if (creatureList.size())
            for (auto itr = creatureList.begin(); itr != creatureList.end(); ++itr)
                if (auto c = (*itr))
                    c->DespawnOrUnsummon();

        if (IsHeroic())
            instance->FinishLfgDungeon(me);
        me->DeleteThreatList();
        summons.DespawnAll();
    }
    private:
        float lock_orientation { 0.f };
        bool  backup_action_required{ false };
};

enum solar_wind_vortex_enum
{
    POINT_RANDOM_1,
    POINT_RANDOM_2,
    POINT_SPAWN,
};

struct npc_solar_wind_vortex : public ScriptedAI
{
    npc_solar_wind_vortex(Creature* creature) : ScriptedAI(creature) { }


    uint32 previous_point[2];
    Position random[2];

    void IsSummonedBy(Unit* /*summoner*/)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
        me->SetReactState(REACT_PASSIVE);

        DoCastAOE(SPELL_FIRE_VORTEX);
        DoCastAOE(SPELL_SOLAR_FIRE_AURA);
        me->SetSpeed(MOVE_WALK, 3.f, false);
        me->SetWalk(true);

        //Set up 2 random positions with spawn being elbow
        me->SetFacingTo(M_PI);
        me->MoveBlink(random[0], 20.f, frand(M_PI * 0.2f, M_PI * 0.8f));
        me->MoveBlink(random[1], 20.f, frand(M_PI * 1.2f, M_PI * 1.8f));

        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MovePoint(POINT_RANDOM_1, random[1]);

        if (auto rajh = Creature::GetCreature(*me, instance->GetData64(DATA_RAJH_GUID)))
            rajh->AI()->JustSummoned(me);
        events.ScheduleEvent(EVENT_SUMMON_FIRE, 100);
    }

    void MovementInform(uint32 type, uint32 id)
    {

        if (/*type == SPLINE_MOTION_TYPE || */type == POINT_MOTION_TYPE)
        {
            previous_point[0] = id;     //remember what point was just reached
            if (id != POINT_SPAWN)      //update "outside" point
                previous_point[1] = (id == POINT_RANDOM_1 ? POINT_RANDOM_2 : POINT_RANDOM_1);
            events.ScheduleEvent(EVENT_MOVE_ARROUND, 1500);
        }
    }
        
    void JustSummoned(Creature* summon)
    {
        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_DISABLE_MOVE);
        summon->AI()->DoCast(SPELL_SOLAR_FIRE);
    }

    void UpdateAI(const uint32 diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SUMMON_FIRE:
                {
                    bool spawn{ false };

                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_SOLAR_WIND, 10.f);
                    if (list_of_npcs.size())
                    list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                    if (list_of_npcs.size())
                    {
                        if (auto closest = list_of_npcs.front())
                            if (me->GetExactDist2d(closest) > 2.f)
                                spawn = true;
                    }
                    else
                        spawn = true;

                    if (spawn)
                        if (auto rajh = Creature::GetCreature(*me, instance->GetData64(DATA_RAJH_GUID)))
                            if (auto c = rajh->SummonCreature(NPC_SOLAR_WIND, *me, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                c->CastSpell(c, SPELL_SOLAR_FIRE, true);
                            }

                    events.ScheduleEvent(EVENT_SUMMON_FIRE, 100);
                }
                    break;
                case EVENT_MOVE_ARROUND:
                    me->GetMotionMaster()->MovePoint(
                        (previous_point[0] == POINT_SPAWN ? previous_point[1]           :   POINT_SPAWN), //either the spawn or the outside point
                        (previous_point[0] == POINT_SPAWN ? random[previous_point[1]]   :   me->GetHomePosition()),
                        true, 0.0f, true);
                    break;
                default:
                    break;
            }
        }
    }
};

struct npc_sunball : public ScriptedAI
{
    npc_sunball(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner)
    {
        Position spawnedAt = {summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ()+40.f, summoner->GetOrientation()};
        me->NearTeleportTo(spawnedAt);

        me->setFaction(summoner->getFaction());
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC);
        DoCast(me, SPELL_SUN);
        me->SetHover(true);
        me->SetCanFly(true);
        me->SetDisableGravity(true);
        events.ScheduleEvent(EVENT_MOVE_DOWN, 1);
    }

    void MovementInform(uint32 type, uint32 id) override
    {

        if (type == POINT_MOTION_TYPE && id == EVENT_JUMP)
        {
            DoCast(me, SPELL_BLAZING_INFERNO, true);
            me->DespawnOrUnsummon(1000);
        }
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MOVE_DOWN:
                    if (auto p = me->FindNearestPlayer(200.f))
                    if (auto rajh = Creature::GetCreature(*me, instance->GetData64(DATA_RAJH_GUID)))
                    {
                        me->GetMotionMaster()->MovePoint(EVENT_JUMP, *p, false, 45.f, false);
                    }
                    break;
                default:
                    break;
            }
        }
    }
};

struct npc_elementar_1 : public ScriptedAI
{
    npc_elementar_1(Creature* creature) : ScriptedAI(creature) { }

    void EnterCombat(Unit * /*who*/)
    {
        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
        events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
    }

    void JustDied(Unit* who)
    {
    }


    void DamageTaken(Unit* damageDealer, uint32& damage)
    {
        if (channeling)
        {
            damage = 0;
            return;
        }
        else
        if (me->HealthBelowPctDamaged(1, damage))
        {
            channeling = true;
            me->CastStop();
            me->ClearUnitState(UNIT_STATE_CASTING);
            damage = (me->GetHealth() - 1);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
            me->ClearUnitState(UNIT_STATE_CASTING);
            me->CastStop();
            DoCastAOE(SPELL_DISPERSION);
            events.ScheduleEvent(EVENT_SUMMON_3, 4000);
        }
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SUMMON_3:
                    me->Kill(me, false, false);
                    break;
                case EVENT_FLAME_WAVE:
                    if (IsHeroic())
                    {
                        DoCastAOE(89852);
                        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
                    }
                    else
                    {
                        DoCastAOE(76160);
                        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
                    }
                    break;
                case EVENT_SEARING_FLAME:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        if(IsHeroic())
                        {
                            DoCast(target, 89849);
                            events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
                        }
                        else
                        {
                            DoCast(target, 74101);
                            events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
                        }
                    break;
                default:
                    break;
            }
        if (!channeling)
        if (!me->HasUnitState(UNIT_STATE_CASTING))
            if (UpdateVictim())
                DoMeleeAttackIfReady();
    }
private:
    bool channeling{ false };
};

struct npc_elementar_2 : public ScriptedAI
{
    npc_elementar_2(Creature* creature) : ScriptedAI(creature) { }

    void EnterCombat(Unit * /*who*/)
    {
        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
        events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
        DoCastAOE(76158);
    }
    void JustDied(Unit* who)
    {
    }
    void DamageTaken(Unit* damageDealer, uint32& damage)
    {
        if (channeling)
        {
            damage = 0;
            return;
        }
        else
            if (me->HealthBelowPctDamaged(1, damage))
            {
            damage = (me->GetHealth() - 1);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
            me->ClearUnitState(UNIT_STATE_CASTING);
            me->CastStop();
            DoCastAOE(88100);
            events.ScheduleEvent(EVENT_SUMMON_3, 3500);
        }
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
            case EVENT_SUMMON_3:
                me->Kill(me, false, false);
                break;

                case EVENT_FLAME_WAVE:
                    if (IsHeroic())
                    {
                        DoCastAOE(89852);
                        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
                    }
                    else
                    {
                        DoCastAOE(76160);
                        events.ScheduleEvent(EVENT_FLAME_WAVE, 9000);
                    }
                    break;
                case EVENT_SEARING_FLAME:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        if(IsHeroic())
                        {
                            DoCast(target, 89849);
                            events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
                        }
                        else
                        {
                            DoCast(target, 74101);
                            events.ScheduleEvent(EVENT_SEARING_FLAME, 6000);
                        }
                    break;
                default:
                    break;
            }
        if (!channeling)
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
    }
private:
    bool channeling{ false };
};

struct npc_suntouched_speaker : public ScriptedAI
{
    npc_suntouched_speaker(Creature* creature) : ScriptedAI(creature) { }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_FIRE_STORM, 10000);
        events.ScheduleEvent(EVENT_STOMP, 6000);
    }

    void UpdateAI(uint32 const diff)
    {
        events.Update(diff);

        if (!UpdateVictim())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FIRE_STORM:
                    DoCastVictim(SPELL_FIRE_STORM);
                    events.ScheduleEvent(SPELL_FIRE_STORM, 15000);
                    break;
                case EVENT_STOMP:
                    DoCastVictim(SPELL_STOMP);
                    events.ScheduleEvent(EVENT_STOMP, 9000);
                    break;
                default:
                    break;
            }
        }
        DoMeleeAttackIfReady();
    }
};

class spell_rajh_leap : public SpellScriptLoader
{
public:
    spell_rajh_leap() : SpellScriptLoader("spell_rajh_leap") { }

    class spell_rajh_leapSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_rajh_leapSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        void HandleAfterCast()
        {
            if (auto c = GetCaster())
                if (auto cr = c->ToCreature())
                    if (auto ai = cr->AI())
                        ai->DoAction(EVENT_INFERNO_JUMP);
        }

        void HandleOnEffectHitTarget(SpellEffIndex effIndex)
        {
            //TC_LOG_ERROR("sql.sql", "Rajh leap has hit target with entry %u", GetHitUnit()->GetEntry());
            //PreventHitEffect(effIndex);

            //if (auto c = GetCaster())
                //if (auto t = GetHitUnit())
                    //c->CastSpell(t, 67382, true);
                    //c->GetMotionMaster()->MoveJump(*t, float(c->GetDistance(t) * 0.2f), 9.f, 55);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_rajh_leapSpellScript::HandleAfterCast);
            //OnEffectHitTarget += SpellEffectFn(spell_rajh_leapSpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_rajh_leapSpellScript();
    }
};

void AddSC_boss_rajh()
{
    RegisterCreatureAI(boss_rajh);
    RegisterCreatureAI(npc_sunball);
    RegisterCreatureAI(npc_solar_wind_vortex);
    RegisterCreatureAI(npc_elementar_1);
    RegisterCreatureAI(npc_elementar_2);
    RegisterCreatureAI(npc_suntouched_speaker);
    new spell_rajh_leap();
}