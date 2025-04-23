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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_CHARGE                                  = 22911,
    SPELL_CLEAVE                                  = 40504,
    SPELL_DEMORALIZING_SHOUT                      = 23511,
    SPELL_ENRAGE                                  = 8599,
    SPELL_WHIRLWIND                               = 13736,

    SPELL_NORTH_MARSHAL                           = 45828,
    SPELL_SOUTH_MARSHAL                           = 45829,
    SPELL_STONEHEARTH_MARSHAL                     = 45830,
    SPELL_ICEWING_MARSHAL                         = 45831,
    SPELL_ICEBLOOD_WARMASTER                      = 45822,
    SPELL_TOWER_POINT_WARMASTER                   = 45823,
    SPELL_WEST_FROSTWOLF_WARMASTER                = 45824,
    SPELL_EAST_FROSTWOLF_WARMASTER                = 45826
};

enum Creatures
{
    NPC_NORTH_MARSHAL                             = 14762,
    NPC_SOUTH_MARSHAL                             = 14763,
    NPC_ICEWING_MARSHAL                           = 14764,
    NPC_STONEHEARTH_MARSHAL                       = 14765,
    NPC_EAST_FROSTWOLF_WARMASTER                  = 14772,
    NPC_ICEBLOOD_WARMASTER                        = 14773,
    NPC_TOWER_POINT_WARMASTER                     = 14776,
    NPC_WEST_FROSTWOLF_WARMASTER                  = 14777
};

enum Events
{
    EVENT_CHARGE_TARGET        = 1,
    EVENT_CLEAVE               = 2,
    EVENT_DEMORALIZING_SHOUT   = 3,
    EVENT_WHIRLWIND            = 4,
    EVENT_ENRAGE               = 5,
    EVENT_CHECK_RESET          = 6
};

struct SpellPair
{
    uint32 npcEntry;
    uint32 spellId;
};

uint8 const MAX_SPELL_PAIRS = 8;
SpellPair const _auraPairs[MAX_SPELL_PAIRS] =
{
    { NPC_NORTH_MARSHAL,            SPELL_NORTH_MARSHAL },
    { NPC_SOUTH_MARSHAL,            SPELL_SOUTH_MARSHAL },
    { NPC_STONEHEARTH_MARSHAL,      SPELL_STONEHEARTH_MARSHAL },
    { NPC_ICEWING_MARSHAL,          SPELL_ICEWING_MARSHAL },
    { NPC_EAST_FROSTWOLF_WARMASTER, SPELL_EAST_FROSTWOLF_WARMASTER },
    { NPC_WEST_FROSTWOLF_WARMASTER, SPELL_WEST_FROSTWOLF_WARMASTER },
    { NPC_TOWER_POINT_WARMASTER,    SPELL_TOWER_POINT_WARMASTER },
    { NPC_ICEBLOOD_WARMASTER,       SPELL_ICEBLOOD_WARMASTER }
};

class mob_av_marshal_or_warmaster : public CreatureScript
{
    public:
        mob_av_marshal_or_warmaster() : CreatureScript("mob_av_marshal_or_warmaster") { }

        struct mob_av_marshal_or_warmasterAI : public ScriptedAI
        {
            mob_av_marshal_or_warmasterAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CHARGE_TARGET, urand(2 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CLEAVE, urand(1 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEMORALIZING_SHOUT, 2000);
                events.ScheduleEvent(EVENT_WHIRLWIND, urand(5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ENRAGE, urand(5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CHECK_RESET, 5000);

                _hasAura = false;
            }

            void JustRespawned()
            {
                Reset();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!_hasAura)
                {
                    for (uint8 i = 0; i < MAX_SPELL_PAIRS; ++i)
                    {
                        if (_auraPairs[i].npcEntry == me->GetEntry())
                        {
                            if (!me->isDead())
                            {
                                DoCast(me, _auraPairs[i].spellId);
                                _hasAura = true;
                            }
                        }
                    }
                }
                else {
                    for (uint8 i = 0; i < MAX_SPELL_PAIRS; ++i) {
                        if (_auraPairs[i].npcEntry == me->GetEntry()) {
                            if (me->isDead()) {
                                me->RemoveAurasDueToSpell(_auraPairs[i].spellId);
                                _hasAura = false;
                            }
                        }
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHARGE_TARGET:
                            DoCastVictim(SPELL_CHARGE);
                            events.ScheduleEvent(EVENT_CHARGE, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(10 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_DEMORALIZING_SHOUT:
                            DoCast(me, SPELL_DEMORALIZING_SHOUT);
                            events.ScheduleEvent(EVENT_DEMORALIZING_SHOUT, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case EVENT_WHIRLWIND:
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_WHIRLWIND, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_ENRAGE);
                            events.ScheduleEvent(EVENT_ENRAGE, urand(10 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHECK_RESET:
                        {
                            Position const& _homePosition = me->GetHomePosition();
                            if (me->GetDistance2d(_homePosition.GetPositionX(), _homePosition.GetPositionY()) > 50.0f)
                            {
                                EnterEvadeMode();
                                return;
                            }
                            else {
                                me->CallForHelp(51.0f);
                            }
                            events.ScheduleEvent(EVENT_CHECK_RESET, 5000);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            bool _hasAura;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_av_marshal_or_warmasterAI(creature);
        }
};

enum AV_FACTIONS
{
    AV_FACTION_ALLIANCE = 1732,
    AV_FACTION_HORDE = 1735,
};

enum Titan_Events
{
    EVENT_IVUS_START_PATH = 5,
    EVENT_IVUS_CAST_MASSROOT = 6,
    EVENT_IVUS_CAST_STARFIRE = 7,
    EVENT_IVUS_CAST_INNERVATE = 8,
    EVENT_IVUS_CAST_WRATH = 9,
    EVENT_IVUS_CAST_MOONFIRE = 10,
    EVENT_IVUS_CAST_FAEFIRE = 11,
    EVENT_IVUS_STOP_PATH = 12,

    EVENT_LOKHO_START_PATH = 13,
    EVENT_LOKHO_CAST_NOVA = 14,
    EVENT_LOKHO_CAST_SHOCK = 15,
    EVENT_LOKHO_CAST_BLOCK1 = 16,
    EVENT_LOKHO_CAST_BLOCK2 = 17,
    EVENT_LOKHO_CAST_BOLT = 18,
    EVENT_LOKHO_CAST_BLIZZ = 19,
    EVENT_LOKHO_STOP_PATH = 20,

    EVENT_ALLOW_SAY_KILL = 21,
};
enum Titan_Actions
{
    ACTION_IVUS_ROAM_BASE = 0,
    ACTION_LOKHO_ROAM_BASE = 1,
};
enum Titan_Spells
{
    SPELL_IVUS_MASS_ROOT = 20654,
    SPELL_IVUS_STARFIRE = 75332,
    SPELL_IVUS_INNERVATE = 29166,
    SPELL_IVUS_SHOCK_WRATH = 21667,
    SPELL_IVUS_MOONFIRE = 20690,
    SPELL_IVUS_FAEFIRE = 21670,

    SPELL_LOKHO_NOVA = 14907,//Occasional Frost nova
    SPELL_LOKHO_SHOCK = 19133,//Frost shock - not main target
    SPELL_LOKHO_BLOCK_1 = 15878,//Ice block victim
    SPELL_LOKHO_BLOCK_2 = 16869,//Ice Block - not main target
    SPELL_LOKHO_BOLT = 55802,//Cast random frostbolt
    SPELL_LOKHO_BLIZZ = 20680,//Random Target Blizzard
    SPELL_LOKHO_SWELL_SOULS = 21307,
};

enum Titan_Say_Lines
{
    IVUS_SAY_SPAWNED = 0, //We will not call this one. This will be done in the BattlegroundAV script so that players can hear it much further across the map.
    IVUS_SAY_ARRIVE_BASE = 1,
    IVUS_SAY_KILL = 2,

    LOKHO_SAY_SPAWNED = 0,
    LOKHO_SAY_ARRIVE_BASE = 1,
    LOKHO_SAY_KILL = 2,
};
enum Titan_Paths
{
    PATH_IVUS = 13419,
    PATH_LOKHOLAR = 13256,
};
enum Ivus_path_point_names
{
    IVUS_POINT_FINAL = 39,
    LOKHOLAR_POINT_FINAL = 55,
};

class npc_ivus_the_forest_lord : public CreatureScript
{
public:
    npc_ivus_the_forest_lord() : CreatureScript("npc_ivus_the_forest_lord") {}

    struct npc_ivus_the_forest_lordAI : public ScriptedAI
    {
        npc_ivus_the_forest_lordAI(Creature* creature) : ScriptedAI(creature)
        {
            //This (I believe) is the very first thing that gets run when the npc is spawned.
        }

        void Reset() override
        {
            events.CancelEvent(EVENT_IVUS_CAST_FAEFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_INNERVATE);
            events.CancelEvent(EVENT_IVUS_CAST_MASSROOT);
            events.CancelEvent(EVENT_IVUS_CAST_MOONFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_STARFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_WRATH);
        }
        void InitializeAI() override
        {
            arrived_at_base = false;
            if (me->GetZoneId() == 2597)
                spawned_in_alterac_valley = true;
            else
                spawned_in_alterac_valley = false;

            if (spawned_in_alterac_valley)
            {
                events.ScheduleEvent(EVENT_IVUS_START_PATH, 10 * MINUTE * IN_MILLISECONDS);
                me->setFaction(AV_FACTION_ALLIANCE);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->Clear(true);
                me->GetMotionMaster()->MoveRandom(100.0f);
                //TC_LOG_ERROR("misc", "IVUS 1(test) minute timer starting for %u mS", 60000);
            }
        }
        void IsSummonedBy(Unit* /*summoner*/) override
        {
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            //TC_LOG_ERROR("misc", "MOVE_INFORM: type: %u, id: %u", type, id);
            if (type == 2/*Manual point move*/)
                if (me->GetDistance2d(finishLine.GetPositionX(), finishLine.GetPositionY()) < 5.0f)
                {
                    Talk(IVUS_SAY_ARRIVE_BASE);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    events.ScheduleEvent(EVENT_IVUS_STOP_PATH, 15000);
                }
        }

        void KilledUnit(Unit* who) override
        {
            if (CanSayKill)
            {
                Talk(IVUS_SAY_KILL);
                events.ScheduleEvent(EVENT_ALLOW_SAY_KILL, 8000);
                CanSayKill = false;
            }
        }
        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case ACTION_IVUS_ROAM_BASE:
                arrived_at_base = true;
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                //TC_LOG_ERROR("misc", "Ivus reached base");
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRandom(40.0f);
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            default:
                break;
            }
        }

        void EnterCombat(Unit* who) override
        {
            me->SetReactState(REACT_AGGRESSIVE);
            if (me->GetDistance(who) <= 15.00f)
            {
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
            }
            else
            {
                DoCast(who, SPELL_IVUS_MOONFIRE);
            }

            events.CancelEvent(EVENT_IVUS_CAST_FAEFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_INNERVATE);
            events.CancelEvent(EVENT_IVUS_CAST_MASSROOT);
            events.CancelEvent(EVENT_IVUS_CAST_MOONFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_STARFIRE);
            events.CancelEvent(EVENT_IVUS_CAST_WRATH);

            //14s rotation other than innervate and mass root
            events.ScheduleEvent(EVENT_IVUS_CAST_WRATH, 2000);
            events.ScheduleEvent(EVENT_IVUS_CAST_FAEFIRE, 5000);
            events.ScheduleEvent(EVENT_IVUS_CAST_MASSROOT, 6000);
            events.ScheduleEvent(EVENT_IVUS_CAST_MOONFIRE, 7000);
            events.ScheduleEvent(EVENT_IVUS_CAST_STARFIRE, 10000);
            events.ScheduleEvent(EVENT_IVUS_CAST_INNERVATE, 14000);
        }
        void UpdateAI(const uint32 diff) override
        {

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_IVUS_START_PATH:
                    //TC_LOG_ERROR("misc", "IVUS starting path movement");
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePath(PATH_IVUS, false);
                    me->SetReactState(REACT_PASSIVE);
                    break;
                case EVENT_IVUS_CAST_FAEFIRE:
                    //TC_LOG_ERROR("misc", "EVENT IVUS FAEFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM))
                                if (!faefireTarget->HasAura(SPELL_IVUS_FAEFIRE))
                                    DoCast(faefireTarget, SPELL_IVUS_FAEFIRE, true);
                        events.ScheduleEvent(EVENT_IVUS_CAST_FAEFIRE, 4000);
                    }
                    break;
                case EVENT_IVUS_CAST_MASSROOT:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MASSROOT");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me->getVictim(), SPELL_IVUS_MASS_ROOT);
                        events.ScheduleEvent(EVENT_IVUS_CAST_MASSROOT, 25000);
                    }
                    break;
                case EVENT_IVUS_CAST_STARFIRE:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(faefireTarget, SPELL_IVUS_STARFIRE);
                        events.ScheduleEvent(EVENT_IVUS_CAST_STARFIRE, 14000);
                    }
                    break;
                case EVENT_IVUS_CAST_INNERVATE:
                    //TC_LOG_ERROR("misc", "EVENT IVUS INNERVATE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me, SPELL_IVUS_INNERVATE);
                        events.ScheduleEvent(EVENT_IVUS_CAST_INNERVATE, 25000);
                    }
                    break;
                case EVENT_IVUS_CAST_WRATH:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me->getVictim(), SPELL_IVUS_SHOCK_WRATH);
                        events.ScheduleEvent(EVENT_IVUS_CAST_WRATH, 14000);
                    }
                    break;
                case EVENT_IVUS_CAST_MOONFIRE:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM, NULL, NULL, true))
                                if (!faefireTarget->HasAura(SPELL_IVUS_MOONFIRE))
                                    DoCast(faefireTarget, SPELL_IVUS_MOONFIRE, true);
                        events.ScheduleEvent(EVENT_IVUS_CAST_MOONFIRE, 4000);
                    }
                    break;
                case EVENT_IVUS_STOP_PATH:
                    DoAction(ACTION_IVUS_ROAM_BASE);
                    break;
                case EVENT_ALLOW_SAY_KILL:
                    CanSayKill = true;
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();

        }

        void JustSummoned(Creature* summon) override
        {
            /*
                REPLACE INTO `creature_text` VALUES (53494, 0, 0, 'You are forbidden from my master\'s domain, mortals.', 14, 0, 0, 0, 0, 0, 'Ivus Spawned', 0);
            */
            //summon->setActive(true);
            //TC_LOG_ERROR("misc", "Ivus_example_justsummoned NPC with GUID %u", summon->GetGUID());
        }

    private:
        EventMap events;
        bool spawned_in_alterac_valley;
        bool arrived_at_base;
        bool CanSayKill = true;
        Position const& finishLine = { -1342.62f, -291.744f, 90.9181f };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ivus_the_forest_lordAI(creature);
    }
};


class npc_lokholar_the_ice_lord : public CreatureScript
{
public:
    npc_lokholar_the_ice_lord() : CreatureScript("npc_lokholar_the_ice_lord") {}

    struct npc_lokholar_the_ice_lordAI : public ScriptedAI
    {
        npc_lokholar_the_ice_lordAI(Creature* creature) : ScriptedAI(creature)
        {
            //This (I believe) is the very first thing that gets run when the npc is spawned.
        }

        void Reset() override
        {
            events.CancelEvent(EVENT_LOKHO_CAST_SHOCK);

            if (!me->HasAura(SPELL_LOKHO_SWELL_SOULS))
                if (getSwellStacks() > 0)
                    me->CastCustomSpell(SPELL_LOKHO_SWELL_SOULS, SPELLVALUE_AURA_STACK, getSwellStacks(), me);

            events.CancelEvent(EVENT_LOKHO_CAST_SHOCK);
            events.CancelEvent(EVENT_LOKHO_CAST_NOVA);
            events.CancelEvent(EVENT_LOKHO_CAST_BLOCK1);
            events.CancelEvent(EVENT_LOKHO_CAST_BLOCK2);
            events.CancelEvent(EVENT_LOKHO_CAST_BOLT);
            events.CancelEvent(EVENT_LOKHO_CAST_BLIZZ);
        }
        int32 getSwellStacks()
        {
            if (swellStacks > 10)
                swellStacks = 10;
            return swellStacks;
        }
        void InitializeAI() override
        {
            arrived_at_base = false;
            if (me->GetZoneId() == 2597)
                spawned_in_alterac_valley = true;
            else
                spawned_in_alterac_valley = false;

            if (spawned_in_alterac_valley)
            {
                events.ScheduleEvent(EVENT_LOKHO_START_PATH, 10 * MINUTE * IN_MILLISECONDS);
                me->setFaction(AV_FACTION_HORDE);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->Clear(true);
                me->GetMotionMaster()->MoveRandom(100.0f);
                //TC_LOG_ERROR("misc", "LOKHO 1(test) minute timer starting for %u mS", 60000);
            }
        }
        void IsSummonedBy(Unit* /*summoner*/) override
        {
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            //TC_LOG_ERROR("misc", "MOVE_INFORM: type: %u, id: %u", type, id);
            if (type == 2/*Manual point move*/)
                if (me->GetDistance2d(finishLine.GetPositionX(), finishLine.GetPositionY()) < 5.0f)
                {
                    Talk(LOKHO_SAY_ARRIVE_BASE);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    events.ScheduleEvent(EVENT_LOKHO_STOP_PATH, 15000);
                }
        }

        void KilledUnit(Unit* who) override
        {
            if (me->HasAura(SPELL_LOKHO_SWELL_SOULS))
            {
                if (swellStacks < 10)
                {
                    swellStacks++;
                    me->AddAura(SPELL_LOKHO_SWELL_SOULS, me);
                }
            }
            else
            {
                me->CastCustomSpell(SPELL_LOKHO_SWELL_SOULS, SPELLVALUE_AURA_STACK, getSwellStacks(), me);
            }

            if (CanSayKill)
            {
                Talk(LOKHO_SAY_KILL);
                events.ScheduleEvent(EVENT_ALLOW_SAY_KILL, 8000);
                CanSayKill = false;
            }
        }

        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case ACTION_LOKHO_ROAM_BASE:
                arrived_at_base = true;
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                //TC_LOG_ERROR("misc", "Ivus reached base");
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveRandom(40.0f);
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            default:
                break;
            }
        }

        void EnterCombat(Unit* who) override
        {
            me->SetReactState(REACT_AGGRESSIVE);
            if (me->GetDistance(who) <= 15.00f)
            {
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
            }
            else
            {
                DoCast(who, SPELL_LOKHO_SHOCK);
            }

            events.CancelEvent(EVENT_LOKHO_CAST_SHOCK);
            events.CancelEvent(EVENT_LOKHO_CAST_NOVA);
            events.CancelEvent(EVENT_LOKHO_CAST_BLOCK1);
            events.CancelEvent(EVENT_LOKHO_CAST_BLOCK2);
            events.CancelEvent(EVENT_LOKHO_CAST_BOLT);
            events.ScheduleEvent(EVENT_LOKHO_CAST_BLOCK1, 2000);//target block
            events.ScheduleEvent(EVENT_LOKHO_CAST_BOLT, 3000);
            events.ScheduleEvent(EVENT_LOKHO_CAST_BLOCK2, 10000);//random block
            events.ScheduleEvent(EVENT_LOKHO_CAST_NOVA, 10000);
            events.ScheduleEvent(EVENT_LOKHO_CAST_BLIZZ, 12000);
            events.ScheduleEvent(EVENT_LOKHO_CAST_SHOCK, 4000);


            if (!me->HasAura(SPELL_LOKHO_SWELL_SOULS))
                if (getSwellStacks() > 0)
                    me->CastCustomSpell(SPELL_LOKHO_SWELL_SOULS, SPELLVALUE_AURA_STACK, getSwellStacks(), me);
        }
        void UpdateAI(const uint32 diff) override
        {

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LOKHO_START_PATH:
                    //TC_LOG_ERROR("misc", "LOKHO starting path movement");
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePath(PATH_LOKHOLAR, false);
                    me->SetReactState(REACT_PASSIVE);
                    break;
                case EVENT_LOKHO_CAST_NOVA:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MASSROOT");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me->getVictim(), SPELL_LOKHO_NOVA);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_NOVA, 14000);
                    }
                    break;
                case EVENT_LOKHO_CAST_SHOCK:
                    //TC_LOG_ERROR("misc", "EVENT IVUS FAEFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM))
                                if (!faefireTarget->HasAura(SPELL_LOKHO_SHOCK))
                                    DoCast(faefireTarget, SPELL_LOKHO_SHOCK, true);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_SHOCK, 4000);
                    }
                    break;
                case EVENT_LOKHO_CAST_BLOCK1:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me->getVictim(), SPELL_LOKHO_BLOCK_2, true);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_BLOCK2, 16000);
                    }
                case EVENT_LOKHO_CAST_BLOCK2:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM))
                                if (!faefireTarget->HasAura(SPELL_LOKHO_BLOCK_2))
                                    DoCast(faefireTarget, SPELL_LOKHO_BLOCK_2, true);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_BLOCK2, 16000);
                    }
                    break;
                case EVENT_LOKHO_CAST_BLIZZ:
                    //TC_LOG_ERROR("misc", "EVENT IVUS INNERVATE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            if (auto faefireTarget = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(faefireTarget, SPELL_LOKHO_BLIZZ);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_BLIZZ, 14000);
                    }
                    break;
                case EVENT_LOKHO_CAST_BOLT:
                    //TC_LOG_ERROR("misc", "EVENT IVUS MOONFIRE");
                    if (me->isInCombat())
                    {
                        if (UpdateVictim())
                            DoCast(me->getVictim(), SPELL_LOKHO_BOLT);
                        events.ScheduleEvent(EVENT_LOKHO_CAST_BOLT, 14000);
                    }
                    break;
                case EVENT_LOKHO_STOP_PATH:
                    DoAction(ACTION_LOKHO_ROAM_BASE);
                    break;
                case EVENT_ALLOW_SAY_KILL:
                    CanSayKill = true;
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();

        }

        void JustSummoned(Creature* summon) override
        {
            /*
                REPLACE INTO `creature_text` VALUES (53494, 0, 0, 'You are forbidden from my master\'s domain, mortals.', 14, 0, 0, 0, 0, 0, 'Ivus Spawned', 0);
            */
            //summon->setActive(true);
            //TC_LOG_ERROR("misc", "Ivus_example_justsummoned NPC with GUID %u", summon->GetGUID());
        }

    private:
        int32 swellStacks = 0;
        EventMap events;
        bool spawned_in_alterac_valley;
        bool arrived_at_base = false;
        bool CanSayKill = true;
        Position const& finishLine = { -624.297f, -44.4865f, 42.1868f };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lokholar_the_ice_lordAI(creature);
    }
};

class npc_AV_Archer : public CreatureScript
{
public:
    npc_AV_Archer() : CreatureScript("npc_AV_Archer") { }


    struct npc_AV_ArcherAI : public ScriptedAI
    {
        npc_AV_ArcherAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI() {
            me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
            me->SetSheath(SHEATH_STATE_UNARMED);
            me->AddUnitState(UNIT_STATE_ROOT);
            me->SetReactState(REACT_AGGRESSIVE);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
        }

        void EnterCombat(Unit* /*who*/) {
            me->AddUnitState(UNIT_STATE_ROOT);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
            events.ScheduleEvent(1, 2000);
        }
        void Reset()
        {
            events.Reset();

            me->DeleteThreatList();
            me->CombatStop();
            me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
            me->SetSheath(SHEATH_STATE_UNARMED);
            me->AddUnitState(UNIT_STATE_ROOT);
            me->SetReactState(REACT_AGGRESSIVE);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
        }

        void ArcherAttack()
        {
            if (!events.HasEvent(1)) // Check to not add multiple events
                events.ScheduleEvent(1, 2000);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who->ToPlayer())
                return;

            if (me->isInCombat())
                return;


            
                if (me->IsValidAttackTarget(who))
                    if (me->GetDistance(who) <= 60.0f)
                    {
                        me->SetFacingTo(me->GetAngle(who));
                        AttackStart(who);
                    }            

        }

        void DecideSheathe()
        {
            if (auto v = me->getVictim())
                if (me->IsWithinMeleeRange(v))
                {
                    if (me->GetSheath() != SHEATH_STATE_MELEE)
                    {
                        me->SetSheath(SHEATH_STATE_MELEE);
                    }
                }
                else
                {
                    if (me->GetSheath() != SHEATH_STATE_RANGED)
                    {
                        me->SetSheath(SHEATH_STATE_RANGED);
                    }
                }
        }

        void DoMeleeAttackIfReady()
        {
            DecideSheathe();
            if (auto v = me->getVictim())
            {
                if (me->IsWithinMeleeRange(v) && me->isAttackReady(BASE_ATTACK))
                {
                    me->AttackerStateUpdate(v);
                    me->resetAttackTimer();
                }
                else
                    ArcherAttack();
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                    if (me->isInCombat())
                        if (!me->HasBreakableByDamageCrowdControlAura())
                            if (auto currentTarget = me->getVictim())
                                if (!me->IsWithinMeleeRange(currentTarget))
                                    if (me->GetDistance(currentTarget) <= 60.f)
                                    {
                                        me->SetFacingTo(me->GetOrientation());
                                        if (!me->HasAuraType(SPELL_AURA_MOD_FEAR))
                                            me->CastSpell(currentTarget, 36980, false);
                                    }

                    if (!events.HasEvent(1)) // Check to not add multiple events
                        events.ScheduleEvent(1, 2000);
                    break;
                default:
                    break;
                }

            if (me->isInCombat())
                if (UpdateVictim())//set to false when we leave 60 yard range
                {
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
                }
                else
                {
                    me->ClearUnitState(UNIT_STATE_EVADE);
                    Reset();
                }

        }
    };
    private:
        EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_AV_ArcherAI(creature);
    }
};

void AddSC_alterac_valley()
{
    new npc_AV_Archer();
    new npc_lokholar_the_ice_lord();
    new npc_ivus_the_forest_lord();
    new mob_av_marshal_or_warmaster();
}
