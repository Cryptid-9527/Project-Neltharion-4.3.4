/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
SDName: Deadmines
SD%Complete: 0
SDComment: Placeholder
SDCategory: Deadmines
EndScriptData */

#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "deadmines.h"
#include "Creature.h"
#include "Unit.h"
#include "MotionMaster.h"

#define SPELL_TRAP_EXPLOSION 90392

struct FireData
{
    uint32 cannonGUID;
    uint32 userGUID;
    uint32 triggerGUID;
};

enum mine_monkey
{
    EVENT_THROW = 1,
    EVENT_DESPAWN = 2,

    SPELL_THROW = 91038,
};

class go_canon_defias : public GameObjectScript
{
public:
    go_canon_defias() : GameObjectScript("go_canon_defias") {  }

    struct go_canon_defiasAI : public GameObjectAI
    {
        go_canon_defiasAI(GameObject* gameobject) : GameObjectAI(gameobject) { executed = false; }


        bool GossipHello(Player* player)
        {
            if (!executed) // to prevent multi executing
            {
                go->SendCustomAnim(0);
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                if (auto door = player->FindNearestGameObject(GO_IRON_CLAD_DOOR, 100))
                {
                    door->UseDoorOrButton();
                    door->EnableCollision(false);
                }
                if (auto c = go->FindNearestCreature(45979, 20.0f))
                {
                    c->AI()->Talk(2);
                    c->CastSpell(c, SPELL_TRAP_EXPLOSION, true);
                }
                if (auto trash = go->SummonCreature(48502, -101.09f, -673.01f, 7.51f, 1.80f))
                {
                    trash->SetInCombatWithZone();
                    trash->CallForHelp(10.f);
                }
                if (auto trash = go->SummonCreature(48505, -98.48f, -672.39f, 7.51f, 1.80f))
                    trash->SetInCombatWithZone();
                if (auto trash1 = go->SummonCreature(48417, -97.46f, -675.72f, 7.51f, 1.80f))
                    if (auto trash2 = go->SummonCreature(48417, -100.88f, -676.35f, 7.51f, 1.80f))
                    {
                        trash1->SetReactState(REACT_AGGRESSIVE);
                        trash1->SetInCombatWithZone();
                        trash1->CallForHelp(10.f);

                        trash2->SetReactState(REACT_AGGRESSIVE);
                        trash2->SetInCombatWithZone();
                        trash2->CallForHelp(10.f);

                        executed = true;
                    }

            }
            return true;
            }
    private:
        bool executed{ false };
    };

    GameObjectAI* GetAI(GameObject* gameobject) const
    {
        return new go_canon_defiasAI(gameobject);
    }


};

enum npcs_defias_canon_spells
{
    SPELL_LAUNCH_BULLET = 89757,
};

enum lastEventSpells
{
    SPELL_SIT_DOWN = 89279,

    SPELL_STUN_PLAYERS = 92100,
    SPELL_NIGHTAMRE_ELEXIR_DUMMY = 92113,
    SPELL_VANESSA_BLACKOUT = 92120,

    //    SPELL_MAGMA_VEHICLE_DUMMY   = 92378, // trigger 92379 cast by 49454
    //    SPELL_MAGMA_VEHICLE_CONTROL = 92379, // hit 49454

    //    SPELL_NIGHTMARE_FLAME = 92169, // entry 51594
};

enum lastEventsNpcs
{
    //    NPC_VANESSA_ABOVE_HUT = 49429,
    NPC_ANCHOR_BUNNY = 51624,
    //    NPC_MAGMA_VEHICLE = 49454,
};

enum lastEventEnents
{
    EVENT_INTRO = 1,
    EVENT_GLACIATE,
    ACTION_START_EVENT,
};

class boss_vancleef_event : public CreatureScript
{
public:
    boss_vancleef_event() : CreatureScript("boss_vancleef_event") { }

    struct boss_vancleef_eventAI : public ScriptedAI
    {
        boss_vancleef_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            _introCount = 0;
            me->CastSpell(me, SPELL_SIT_DOWN, true);
            me->SetReactState(REACT_PASSIVE);
            _events.Reset();
            DoAction(ACTION_START_EVENT);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }


        void EnterCombat(Unit* /*who*/)
        {
            me->DeleteThreatList();
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_START_EVENT:
                    _events.ScheduleEvent(EVENT_INTRO, 9000);
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_INTRO:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                        uint32 timer = 5000;
                        switch (_introCount)
                        {
                            case 0:
                                Talk(0);
                                timer = 4000;
                                break;
                            case 1:
                                Talk(1);
                                me->RemoveAura(SPELL_SIT_DOWN);
                                me->GetMotionMaster()->MoveJump(-64.5677f, -820.161f, 41.123f, 14.0f, 14.0f);
                                timer = 9000;
                                break;
                            case 2:
                                Talk(2);
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(0, -65.476540f, -838.231934f, 41.098988f);
                                timer = 10000;
                                break;
                            case 3:
                                Talk(3);
                                timer = 6000;
                                break;
                            case 4:
                                Talk(4);
                                DoCast(SPELL_STUN_PLAYERS);
                                timer = 5000;
                                break;
                            case 5:
                                Talk(5);
                                Talk(6);
                                me->SetFacingTo(WEST);
                                DoCast(SPELL_NIGHTAMRE_ELEXIR_DUMMY);
                                timer = 5000;
                                break;
                            case 6:
                                DoCast(SPELL_VANESSA_BLACKOUT);
                                instance->SetData(DATA_START_VANCLEEF_EVENT, IN_PROGRESS);
                                timer = 5000;
                                break;
                            case 7:
                                Talk(7);
                                me->DespawnOrUnsummon(1000);
                                break;
                            default:
                                return;
                        }
                        _events.ScheduleEvent(EVENT_INTRO, timer);
                        _introCount++;
                        break;
                    }
                    default:
                        break;
                }
            }

        }

    private:
        InstanceScript *instance;
        EventMap _events;
        uint32 _introCount;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vancleef_eventAI(creature);
    }
};

// 92378
class spell_magma_vehicle_dummy : public SpellScriptLoader
{
public:
    spell_magma_vehicle_dummy() : SpellScriptLoader("spell_magma_vehicle_dummy") { }

    class spell_magma_vehicle_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_magma_vehicle_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            _seat = 0;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        _seat++;
                        uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                        target->SetDisableGravity(true); // hack fix camera fall on vehicle
                        target->SendMovementDisableGravity(); // hack fix camera fall on vehicle
                        target->CastCustomSpell(spellId, SPELLVALUE_BASE_POINT0, _seat, caster, false);
                    }
        }

    private:
        uint8 _seat;

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_magma_vehicle_dummy_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_magma_vehicle_dummy_SpellScript();
    }
};

class boss_glubtok_event : public CreatureScript
{
public:
    boss_glubtok_event() : CreatureScript("boss_glubtok_event") { }

    struct boss_glubtok_eventAI : public ScriptedAI
    {
        boss_glubtok_eventAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            me->RemoveAllAuras();
            _events.Reset();
            _events.ScheduleEvent(EVENT_GLACIATE, 10000);
            me->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit * /*who*/)
        {
            me->HandleEmoteCommand(EMOTE_STATE_NONE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void JustDied(Unit * /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            if (instance)
                instance->SetData(DATA_GLUBTOK_EVENT, DONE);
        }

        void JustSummoned(Creature * summon)
        {
            if (summon->GetEntry() == NPC_COLLAPSING_ICICLE)
                summon->CastSpell(summon, SPELL_GLACIATE_DUMMY, false);
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_GLACIATE:
                    {
                        Position pos;
                        if (instance)
                        {
                            std::list<Unit*> targets;
                            auto players = me->GetPlayersInRange(40.f, true);
                            if (players.size())
                                for (Player* pl : players)
                                    if (!pl->GetVehicle())
                                        targets.push_back(pl);

                            if (targets.size())
                            {
                                Trinity::Containers::RandomResizeList(targets, 1);
                                if (auto target = targets.front())
                                {
                                    target->GetPosition(&pos);
                                    if (auto c = me->SummonCreature(45979, pos, TEMPSUMMON_TIMED_DESPAWN, 10000))
                                        c->CastSpell(c, SPELL_SUMMON_ICETOMB, true);
                                }
                            }
                        }
                        _events.ScheduleEvent(EVENT_GLACIATE, 3000);
                        break;
                    }
                    default:
                        break;
                }
            }

            if (UpdateVictim())
            DoMeleeAttackIfReady();
        }

    private:
        InstanceScript *instance;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_glubtok_eventAI(creature);
    }
};

// 92202
class spell_glaciate_dummy : public SpellScriptLoader
{
public:
    spell_glaciate_dummy() : SpellScriptLoader("spell_glaciate_dummy") { }

    class spell_glaciate_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_glaciate_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleDummyEffect(SpellEffIndex effIndex)
        {
            if (Unit *target = GetHitUnit())
            {
                if (target->ToTempSummon())
                    if (Unit *owner = target->ToTempSummon()->GetSummoner())
                    {
                        if (owner->GetEntry() != NPC_COLLAPSING_ICICLE)
                        {
                            owner->CastSpell(owner, SPELL_GLACIATE, false);
                            target->CastSpell(target, SPELL_SUMMON_ICETOMB, true);
                        }
                    }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_glaciate_dummy_SpellScript::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_glaciate_dummy_SpellScript();
    }
};

// 49481
class npc_glubtok_icicle : public CreatureScript
{
public:
    npc_glubtok_icicle() : CreatureScript("npc_glubtok_icicle") {}

    struct npc_glubtok_icicleAI : public ScriptedAI
    {
        npc_glubtok_icicleAI(Creature * creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
        }

        void IsSummonedBy(Unit *who)
        {
            if (instance)
                if (Creature *c = Unit::GetCreature(*me, instance->GetData64(BOSS_GLUBTOK_EVENT_DATA)))
                {
                    if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() != me->GetEntry())
                        me->SetVisible(false);
                    c->AI()->JustSummoned(me);
                }
        }

    private:
        InstanceScript *instance;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_glubtok_icicleAI(creature);
    }
};

// 92210
class spell_gb_summon_icicle_target_selector : public SpellScriptLoader
{
public:
    spell_gb_summon_icicle_target_selector() : SpellScriptLoader("spell_gb_summon_icicle_target_selector") { }

    class spell_gb_summon_icicle_target_selector_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gb_summon_icicle_target_selector_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleDummyEffect(SpellEffIndex effIndex)
        {
            if (Unit *target = GetHitUnit())
            {
                uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                target->CastSpell(target, spellId, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gb_summon_icicle_target_selector_SpellScript::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gb_summon_icicle_target_selector_SpellScript();
    }
};

enum helxevEvts
{
    EVENT_RIDE_PLAYERS = 1,
    EVENT_ACTIVATE_SPIDERS,
    ACTION_RIDE_END = 5, // dont change this value
};

enum heEvSpells
{
    SPELL_HELIX_SET_VEHICLE = 88337,
    SPELL_HELIX_RIDE_FACING_TARGET = 88349,
};

class boss_helix_gearbreaker_event : public CreatureScript
{
public:
    boss_helix_gearbreaker_event() : CreatureScript("boss_helix_gearbreaker_event") { }

    struct boss_helix_gearbreaker_eventAI : public ScriptedAI
    {
        boss_helix_gearbreaker_eventAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
            waveNumber = 2;
        }

        void Reset()
        {
            _events.Reset();
            _summons.DespawnAll();
            waveNumber = 2;
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit* who)
        {
            Talk(1);
            if (instance)
                instance->SetData(DATA_HELIX_EVENT, IN_PROGRESS);
            _events.ScheduleEvent(EVENT_ACTIVATE_SPIDERS, 1000);
            _events.ScheduleEvent(EVENT_RIDE_PLAYERS, 5000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            if (instance)
                instance->SetData(DATA_HELIX_EVENT, DONE);
            _events.Reset();
            _summons.DespawnAll();

            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
            for (auto i = playerList.begin(); i != playerList.end(); ++i)
                if (auto player = i->getSource())
                    if (player->GetVehicleKit())
                    {
                        player->RemoveVehicleKit();
                        WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, player->GetPackGUID().size() + 4);
                        data.appendPackGUID(player->GetGUID());
                        data << uint32(0);
                        player->SendMessageToSet(&data, true);
                    }
        }


        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_START_FIGHT:
                {
                    me->RemoveAllAuras();
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    Talk(0);
                    break;
                }
                case ACTION_RIDE_END:
                {
                    me->ExitVehicle();
                    break;
                }
            }
        }

        void JustSummoned(Creature* summon)
        {
            _summons.Summon(summon);
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_RIDE_PLAYERS:
                        DoCast(SPELL_HELIX_RIDE_FACING_TARGET);
                        _events.ScheduleEvent(EVENT_RIDE_PLAYERS, 12000);
                        break;
                    case EVENT_ACTIVATE_SPIDERS:
                    {
                        bool bigSpider{ false };
                        
                        int aggressive_spiders{ 0 };

                        if (waveNumber > 100)
                        {
                            auto players = me->GetPlayersInRange(500.f, true);
                            for (auto p : players)
                                me->Kill(p);
                            return;
                        }
                        /*                        
                            NPC_DARKWEB_DEVOURER            = 49494,
                            NPC_CHATTERING_HORROR           = 49495,
                            NPC_NIGHTMARE_SKITTERLING       = 49493,
                        */
                        for (auto itr = 0; itr != waveNumber; itr++)
                        {
                            if (waveNumber < 6)
                                bigSpider = true;//a "big" spider has already been spawned, we do not need to spawn another. also do this at waves before 6 to prevent early spawns.

                            bool giga_spider{ false };

                            if (waveNumber >= 10)
                                giga_spider = true;

                            if (auto spider = me->SummonCreature
                            (
                                ((!bigSpider) && waveNumber > 6) ? NPC_DARKWEB_DEVOURER : RAND(NPC_NIGHTMARE_SKITTERLING, NPC_NIGHTMARE_SKITTERLING, NPC_CHATTERING_HORROR, NPC_NIGHTMARE_SKITTERLING),
                                {
                                    spiderSpawn.GetPositionX() + frand(-10.f,   10.f),
                                    spiderSpawn.GetPositionY() + frand(-10.f,   10.f),
                                    spiderSpawn.GetPositionZ() + frand(0.f,     2.f),
                                },
                                TEMPSUMMON_MANUAL_DESPAWN
                                ))
                            {
                                if (spider->GetEntry() == NPC_DARKWEB_DEVOURER)
                                    bigSpider = true;

                                if (aggressive_spiders < 2)
                                {
                                    aggressive_spiders += 1;
                                }
                                else
                                {
                                    spider->DeleteThreatList();
                                }

                                spider->SetReactState(REACT_AGGRESSIVE);
                            }
                        }
                        waveNumber += 1;
                        _events.ScheduleEvent(EVENT_ACTIVATE_SPIDERS, 7000);
                        break;
                    }
                }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }
    private:
        uint32  waveNumber{ 2 };
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
        Position spiderSpawn{ -185.742538f, -574.475830f, 21.373955f, 6.05f };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_helix_gearbreaker_eventAI(creature);
    }
};

enum lgpEvents
{
    EVENT_ROTATE = 1,
    EVENT_BURN_NEARBY,
};

class npc_vanessa_lighting_patter : public CreatureScript
{
public:
    npc_vanessa_lighting_patter() : CreatureScript("npc_vanessa_lighting_patter") { }

    struct npc_vanessa_lighting_patterAI : public ScriptedAI
    {
        npc_vanessa_lighting_patterAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            start = false;
            cnt = 0;
            _events.Reset();
            _summons.DespawnAll();
            me->SetReactState(REACT_PASSIVE);
            bool turnRight = rand() % 2;
            me->SetHover(true);
        }

        void EnterCombat(Unit* who)
        {
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_ACTIVATE:
                    me->CastSpell(me, SPELL_LIGHTING, true);
                    start = false;
                    _summons.DespawnAll();
                    _events.ScheduleEvent(EVENT_ROTATE, 0);
                    _events.ScheduleEvent(EVENT_BURN_NEARBY, 300);
                    break;
                case ACTION_DESACTIVATE:
                    me->RemoveAura(SPELL_LIGHTING);
                    start = true;
                    _events.CancelEvent(EVENT_ROTATE);
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            _events.Reset();
            _summons.DespawnAll();
        }

        void Punish_ZapZap()
        {
            auto players = me->GetPlayersInRange(9.f, true);
            if (players.size())
                for (Player* player : players)
                {
                    if (auto gas = player->FindNearestCreature(49521, 0.5f, true))
                    {
                        if (gas->HasAura(92277))
                            gas->AI()->DoCast(player, 92278, true);
                    }
                }
        }

        void JustSummoned(Creature* summon)
        {
            cnt++;
            summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, cnt, me, false);
            summon->AddAura(SPELL_LIGHTING_2, summon);
            _summons.Summon(summon);
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_ROTATE:
                        if (!start)
                            for (int i = 0; i < 8; i++)
                                me->SummonCreature(NPC_REAPER_LIGHTNING_OBSTACLE_PASSENGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f);
                        me->GetMotionMaster()->MoveRotate(20000, turnRight ? ROTATE_DIRECTION_RIGHT : ROTATE_DIRECTION_LEFT);
                        start = true;
                        _events.ScheduleEvent(EVENT_ROTATE, 20000);
                        break;
                    case EVENT_BURN_NEARBY:
                        Punish_ZapZap();
                        _events.ScheduleEvent(EVENT_BURN_NEARBY, 300);
                        break;
                }
            }
        }
    private:
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
        int cnt;
        bool start, turnRight;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vanessa_lighting_patterAI(creature);
    }
};

class npc_Glubtok_Fire_patter : public CreatureScript
{
public:
    npc_Glubtok_Fire_patter() : CreatureScript("npc_Glubtok_Fire_patter") { }

    struct npc_Glubtok_Fire_patterAI : public ScriptedAI
    {
        npc_Glubtok_Fire_patterAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void InitializeAI()
        {
            _events.ScheduleEvent(EVENT_BURN_NEARBY, 1000);
        }

        void Punish_Sizzle_Sizzle()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            if (players.size())
                for (Player* player : players)
                    if (auto gas = player->FindNearestCreature(NPC_NIGHTMARE_FLAMES, 3.5f, true))
                            gas->AI()->DoCast(player, 92171, true);
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ROTATE:
                    break;
                case EVENT_BURN_NEARBY:
                        Punish_Sizzle_Sizzle();
                    _events.ScheduleEvent(EVENT_BURN_NEARBY, 1000);
                    break;
                }
        }
    private:
        EventMap _events;
        SummonList _summons;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Glubtok_Fire_patterAI(creature);
    }
};

enum reeperEventDatas
{
    EVENT_RREP_SPIRIT = 1,
    SPELL_SPRIT_HIT = 59304,
};

class boss_foe_reeper_event : public CreatureScript
{
public:
    boss_foe_reeper_event() : CreatureScript("boss_foe_reeper_event") { }

    struct boss_foe_reeper_eventAI : public ScriptedAI
    {
        boss_foe_reeper_eventAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_DEFENSIVE);
        }

        void Reset()
        {
            _events.Reset();
            _summons.DespawnAll();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->SetReactState(REACT_DEFENSIVE);
            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
        }

        void EnterCombat(Unit* who)
        {
            if (instance)
                instance->SetData(DATA_REEPER_EVENT, IN_PROGRESS);
            _events.ScheduleEvent(EVENT_RREP_SPIRIT, 1000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            me->RemoveAllAuras();
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            if (instance)
                instance->SetData(DATA_REEPER_EVENT, DONE);
            _events.Reset();
            _summons.DespawnAll();
        }


        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_START_FIGHT:
                {
                    me->RemoveAllAuras();
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff)
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
                    case EVENT_RREP_SPIRIT:
                        DoCastVictim(SPELL_SPRIT_HIT);
                        _events.ScheduleEvent(EVENT_RREP_SPIRIT, 5000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_foe_reeper_eventAI(creature);
    }
};

class npc_heric_HARRINGTON : public CreatureScript
{
public:
    npc_heric_HARRINGTON() : CreatureScript("npc_heric_HARRINGTON") { }

    struct npc_heric_HARRINGTONAI : public ScriptedAI
    {
        npc_heric_HARRINGTONAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            //TC_LOG_ERROR("sql.sql", "npc_heric_HARRINGTON - InitializeAAI");
            cnt = 3;
            if (auto c1 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[0]))
                if (auto c2 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[1]))
                    if (auto c3 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[2]))
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c1->ToUnit());
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c2->ToUnit());
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c3->ToUnit());
                    }
            me->AddUnitState(UNIT_STATE_ROOT);
        }

        void JustSummoned(Creature* summon)
        {
            summon->AI()->AttackStart(me);
            _summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon->ToUnit());
            summon->DespawnOrUnsummon();
            --cnt;
            if (cnt <= 0)
            {
                me->CastSpell(me, SPELL_ADRENALINE, true);
                if (instance)
                    instance->SetData(DATA_WORGEN_EVENT, DONE);
                me->DespawnOrUnsummon(5000);
                Talk(1, NULL, CHAT_MSG_RAID_BOSS_EMOTE, TEXT_RANGE_MAP);

                me->PlayDirectSound(21448);
            }
        }

        void EnterCombat(Unit* who)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(DATA_WORGEN_EVENT, FAIL);
            _summons.DespawnAll();
        }


        void UpdateAI(uint32 const diff)
        {
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }
    private:
        bool summoned{ false };
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
        int cnt;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_heric_HARRINGTONAI(creature);
    }
};

class npc_emma_HARRINGTON : public CreatureScript
{
public:
    npc_emma_HARRINGTON() : CreatureScript("npc_emma_HARRINGTON") { }

    struct npc_emma_HARRINGTONAI : public ScriptedAI
    {
        npc_emma_HARRINGTONAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            cnt = 3;
            if (auto c1 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[3]))
            if (auto c2 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[4]))
            if (auto c3 = me->SummonCreature(NPC_ENRAGED_WORGEN, PartWorgen[5]))
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c1->ToUnit());
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c2->ToUnit());
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, c3->ToUnit());
            }
            me->AddUnitState(UNIT_STATE_ROOT);
        }

        void JustSummoned(Creature* summon)
        {
            summon->AI()->AttackStart(me);
            _summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon->ToUnit());
            summon->DespawnOrUnsummon();

            --cnt;
            if (cnt <= 0)
            {
                me->CastSpell(me, SPELL_ADRENALINE, true);
                if (instance)
                    instance->SetData(DATA_WORGEN_EVENT, DONE);
                me->DespawnOrUnsummon(5000);
                Talk(1, NULL, CHAT_MSG_RAID_BOSS_EMOTE, TEXT_RANGE_MAP);
            }
        }

        void EnterCombat(Unit* who)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(DATA_WORGEN_EVENT, FAIL);
            _summons.DespawnAll();
        }


        void UpdateAI(uint32 const diff)
        {

        }
    private:
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
        int cnt;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emma_HARRINGTONAI(creature);
    }
};

class npc_callisa_HARRINGTON : public CreatureScript
{
public:
    npc_callisa_HARRINGTON() : CreatureScript("npc_callisa_HARRINGTON") { }

    struct npc_callisa_HARRINGTONAI : public ScriptedAI
    {
        npc_callisa_HARRINGTONAI(Creature* creature) : ScriptedAI(creature), _summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {          
            stage[0] = false;
            stage[1] = false;
            stage[2] = false;
            stage[3] = false;
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (auto c = me->FindNearestCreature(NPC_JAMES_HARRINGTON, 10.f, true))
            {
                c->SetTarget(me->GetGUID());
            }
        }
        void JustSummoned(Creature* summon)
        {

            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, summon->ToUnit());

            me->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, summon, false);
            _summons.Summon(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon->ToUnit());
            me->SetFacingTo(me->GetAngle(summon));
            me->SetByteValue(UNIT_FIELD_BYTES_0, 0, 8);
            me->HandleEmoteCommand(EMOTE_ONESHOT_CRY);

            if (instance)
            {
                instance->SetData(DATA_WORGEN_EVENT, DONE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NIGHTMARE_1);
            }
            me->RemoveAllAuras();
            me->DespawnOrUnsummon(5000);
            Talk(1, NULL, CHAT_MSG_RAID_BOSS_EMOTE, TEXT_RANGE_MAP);
        }

        void EnterCombat(Unit* who)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
            if (instance)
                instance->SetData(DATA_WORGEN_EVENT, FAIL);
            _summons.DespawnAll();
        }


        void UpdateAI(uint32 const diff)
        {
            if (me->HealthBelowPct(75) && !stage[1])
            {
                me->PlayDirectSound(2680);
                stage[1] = true;
                Talk(2, TEXT_RANGE_MAP);
                if (auto c = me->FindNearestCreature(NPC_JAMES_HARRINGTON, 10.f, true))
                {
                    c->SetTarget(me->GetGUID());
                }
            }

            if (me->HealthBelowPct(50) && !stage[2])
            {
                me->PlayDirectSound(2680);
                stage[2] = true;
                Talk(3);
                if (auto c = me->FindNearestCreature(NPC_JAMES_HARRINGTON, 10.f, true))
                {
                    c->TalkWithDelay(1000, 2, NULL, CHAT_MSG_MONSTER_SAY);
                    c->SetTarget(me->GetGUID());
                }
            }

            if (stage[0])
                return;
            stage[0] = true;
            if (auto c = me->SummonCreature(NPC_JAMES_HARRINGTON, PartHARRINGTON[2]))
            {
                c->SetReactState(REACT_PASSIVE);
                c->SetTarget(me->GetGUID());
            }
            me->setRegeneratingHealth(false);
        }
    private:
        EventMap _events;
        SummonList _summons;
        InstanceScript *instance;
        bool needCry{ false };
        bool stage[4]{false, false, false, false};
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_callisa_HARRINGTONAI(creature);
    }
};

class npc_vanessa_paper : public CreatureScript
{
public:
    npc_vanessa_paper() : CreatureScript("npc_vanessa_paper") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (InstanceScript *instance = creature->GetInstanceScript())
            if (!instance->IsDone(BOSS_EVENT_VANCLEEF) 
                && instance->GetBossState(BOSS_EVENT_VANCLEEF) != IN_PROGRESS 
                //&& instance->GetData64(BOSS_EVENT_VANCLEEF) == 0 
                && instance->IsDone(BOSS_CAPTAIN_COOKIE))
            {
                instance->SetData(DATA_PREPARE_VANCLEEF_EVENT, IN_PROGRESS);
                //creature->DespawnOrUnsummon(0);
            }
        return true;
    }
};

#define DOSSIP_TEL_1 "Press the button labeled 'Wood and Lumber.'"
#define DOSSIP_TEL_2 "Press the button labeled 'Metal and Scraps.'"
#define DOSSIP_TEL_3 "Press the button labeled 'Ship Parts.'"

enum teleporterSpells
{
    SPELL_TELEPORT_ENTRANCE = 79590, // unused ?
};

// 208002
class go_deadmine_teleporter : public GameObjectScript
{
public:
    go_deadmine_teleporter() : GameObjectScript("go_deadmine_teleporter") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (InstanceScript *instance = go->GetInstanceScript())
        {
            go->SendCustomAnim(0);
            if (instance->IsDone(BOSS_HELIX_DATA))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, DOSSIP_TEL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            if (instance->IsDone(BOSS_FOE_REAPER_5000_DATA))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, DOSSIP_TEL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            if (instance->IsDone(BOSS_CAPTAIN_COOKIE))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, DOSSIP_TEL_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:
            player->NearTeleportTo(-304.96f, -491.73f, 50.04f, 5.92f);
            for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); )
            {
                Position pos(*player);
                Unit* unit = *itr;
                ++itr;
                if (!unit)
                    continue;
                unit->SendTeleportPacket(pos);
                unit->UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), true);
                unit->UpdateObjectVisibility();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->NearTeleportTo(-201.09f, -605.88f, 20.12f, 1.86f);
            for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); )
            {
                Position pos(*player);
                Unit* unit = *itr;
                ++itr;
                if (!unit)
                    continue;
                unit->SendTeleportPacket(pos);
                unit->UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), true);
                unit->UpdateObjectVisibility();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->NearTeleportTo(-129.64f, -789.04f, 18.16f, 5.57f);
            for (Unit::ControlList::iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); )
            {
                Position pos(*player);
                Unit* unit = *itr;
                ++itr;
                if (!unit)
                    continue;
                unit->SendTeleportPacket(pos);
                unit->UpdatePosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), true);
                unit->UpdateObjectVisibility();
            }
            break;
        }
        return true;
    }
};

// 49454
class npc_vancleef_trap : public CreatureScript
{
public:
    npc_vancleef_trap() : CreatureScript("npc_vancleef_trap") { }


    struct npc_vancleef_trapAI : public PassiveAI
    {
        npc_vancleef_trapAI(Creature* creature) : PassiveAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            _reaperCount = 4;
            start = false;
        }

        void PassengerBoarded(Unit* player, int8 seatId, bool apply)
        {
            if (!apply)
            {
                player->SetDisableGravity(false);
                player->SendMovementDisableGravity();
                return;
            }
            player->SetDisableGravity(true);
            player->SendMovementDisableGravity();
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_INIT_EVENT:
                {
                    me->DisableMovementFlagUpdate(true);
                    me->SetDisableGravity(true);
                    me->SetCanFly(true);
                    me->NearTeleportTo(me->GetHomePosition());
                    Position pos;
                    me->GetPosition(&pos);
                    pos.m_positionZ += 20.0f;
                    if (Creature *marker = me->SummonCreature(49552, pos))
                        me->CastSpell(marker, SPELL_ROPE_RAY, true);
                    me->CastSpell(me, SPELL_MAGMA_VEHICLE_DUMMY, true);
                    break;
                }
                case ACTION_EJECT_PASSENGERS:
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS, true);
                    me->DespawnOrUnsummon(1);
                    break;
                }
                case ACTION_EMOTE_TRAP:
                {
                    if (start)
                        return;
                    start = true;
                    Talk(0);
                    break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
        }

    private:
        bool start;
        int8 _reaperCount;
        InstanceScript* instance;
        Position _spawnPos;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vancleef_trapAI (creature);
    }
};

class npc_trap_vapor_event : public CreatureScript
{
public:
    npc_trap_vapor_event() : CreatureScript("npc_trap_vapor_event") {}

    struct npc_trap_vapor_eventAI : public PassiveAI
    {
        npc_trap_vapor_eventAI(Creature* pCreature) : PassiveAI(pCreature)
        {
            instance = pCreature->GetInstanceScript();
        }

        void Reset()
        {
            me->RemoveAura(SPELL_AURA_VAPOR_EVENT);
        }

        void OnSpellClick(Unit* player, bool& result)
        {
            me->CastSpell(me, SPELL_AURA_VAPOR_EVENT, true);

            uint16 ValvesUsed = 0;
            std::list<Creature* > triggerList;
            me->GetCreatureListWithEntryInGrid(triggerList, NPC_VAPOR_VALVE, 50.0f);
            for (std::list<Creature* >::iterator itr = triggerList.begin(); itr != triggerList.end(); itr++)
            {
                if (auto valve = *itr)
                {
                    if (valve->HasAura(SPELL_AURA_VAPOR_EVENT))
                        ValvesUsed++;
                }
            }

            if (ValvesUsed > 3)
                instance->SetData(DATA_TRAP_COMPLETE, DONE);

        }
    private:
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature *pCreature) const
    {
        return new npc_trap_vapor_eventAI(pCreature);
    }
};

// 207079
class go_liberate_singe : public GameObjectScript
{
public:
    go_liberate_singe() : GameObjectScript("go_liberate_singe") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
        go->SendCustomAnim(0);

        std::list<Creature*> units;
        GetCreatureListWithEntryInGrid(units, go, 48440, 5.f);
        GetCreatureListWithEntryInGrid(units, go, 48441, 5.f);
        GetCreatureListWithEntryInGrid(units, go, 48442, 5.f);
        GetCreatureListWithEntryInGrid(units, go, 48278, 5.f);
        for (auto itr = units.begin(); itr != units.end(); ++itr)
            if (auto monkey = (*itr))
            {
                monkey->SetReactState(REACT_AGGRESSIVE);
                monkey->setFaction(player->getFaction());
                monkey->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                monkey->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                monkey->AI()->DoAction(EVENT_THROW);
            }
        return true;
    }

    bool forceMoonkiAttack(Creature *moonki, Creature *target, Player *player)
    {
    }
};

class npc_defias_miner : public CreatureScript
{
    /*
       UPDATE `creature_template` SET `ScriptName`='npc_defias_miner' WHERE  `entry`=48419;
       UPDATE `creature_template` SET `ScriptName`='npc_defias_miner' WHERE  `entry`=48420;
   */


public:
    npc_defias_miner() : CreatureScript("npc_defias_miner") { }

    struct npc_defias_minerAI : ScriptedAI
    {
        npc_defias_minerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            work = true;
            _events.Reset();
        }

        void EnterCombat(Unit* who)
        {
            work = false;
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
                //me->AddUnitState(UNIT_STATE_ROOT);
            CheckLineOfSight(spell);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!_events.HasEvent(i))
                    _events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
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

                        DoCastRandom(my_spells[eventId].spell_id, 40.f);
                        ReAttemptSpell(eventId, true);
                    }
                    else
                    {
                        me->SetRooted(false);
                        UpdateVictim();
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule) //Reschedule based on the results and conditions of casting.
        {
            if (reschedule)
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
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
            EVENT_NEVER_USE = 0, /*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_TUNNEL,

            SPELL_EVENT_COUNT,   /*END SPELL EVENTS   - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_TUNNEL = 90994,
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

             {EVENT_TUNNEL,               SPELL_TUNNEL,          9000,          12000,        true,       true},
        };
        void UpdateAI(const uint32 diff) override
        {
            if (work)
                me->HandleEmoteCommand(467);

            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_TUNNEL:             Spell_Rotation(eventId);   break;
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
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
        bool work;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_minerAI(creature);
    }
};

class npc_defias_envoker : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_defias_envoker' WHERE  `entry`=48418;
    */


public:
    npc_defias_envoker() : CreatureScript("npc_defias_envoker") { }

    struct npc_defias_envokerAI : ScriptedAI
    {
        npc_defias_envokerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

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
            DoCast(SPELL_ENVOKER_SHIELD);
            DoCast(SPELL_HOLY_FIRE);
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
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 30.f)
                    {
                        if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                            if (me->isMoving())
                                me->SetRooted(true);
                        me->AddUnitState(UNIT_STATE_ROOT);

                        DoCast(my_spells[eventId].spell_id);
                        ReAttemptSpell(eventId, true);
                    }
                    else
                    {
                        me->SetRooted(false);
                        UpdateVictim();
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule) //Reschedule based on the results and conditions of casting.
        {
            if (reschedule)
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
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
                            {
                                me->ClearUnitState(UNIT_STATE_ROOT);
                                DoMeleeAttackIfReady();
                            }
        };
        enum spells_and_events
        {
            EVENT_NEVER_USE = 0, /*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_HOLY_FIRE,
            EVENT_RENEGADE_STRENGTH,

            SPELL_EVENT_COUNT,   /*END SPELL EVENTS   - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_HOLY_FIRE = 91004,
            SPELL_RENEGADE_STRENGTH = 90047,
            SPELL_ENVOKER_SHIELD = 92001,
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

            {EVENT_HOLY_FIRE,                  SPELL_HOLY_FIRE,          14000,          18000,          true,       true},
            {EVENT_RENEGADE_STRENGTH,  SPELL_RENEGADE_STRENGTH,         17000,         21000,          true,       true},
        };
        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_HOLY_FIRE:                         Spell_Rotation(eventId);   break;
                case EVENT_RENEGADE_STRENGTH:  Spell_Rotation_Friendly(eventId, 55.f);   break;
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
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_envokerAI(creature);
    }
};

class npc_defias_cannon : public CreatureScript
{
public:
    npc_defias_cannon() : CreatureScript("npc_defias_cannon") { }

    struct npc_defias_cannonAI : public ScriptedAI
    {
        npc_defias_cannonAI(Creature* creature) : ScriptedAI(creature)
        {
            StopCannon = false;
            me->SetReactState(REACT_PASSIVE);

            if (me->isAlive())
                me->SetHealth(me->GetMaxHealth());
        }

        Creature* FindCreatureByDBGuid(WorldObject* searchObject, uint32 guid)
        {
            Creature* creature = NULL;
            CellCoord p(Trinity::ComputeCellCoord(searchObject->GetPositionX(), searchObject->GetPositionY()));
            Cell cell(p);

            Trinity::CreatureWithDbGUIDCheck target_check(searchObject, guid);
            Trinity::CreatureSearcher<Trinity::CreatureWithDbGUIDCheck> checker(searchObject, creature, target_check);

            TypeContainerVisitor<Trinity::CreatureSearcher <Trinity::CreatureWithDbGUIDCheck>, GridTypeMapContainer > unit_checker(checker);
            cell.Visit(p, unit_checker, *searchObject->GetMap(), *searchObject, 100.f);

            return creature;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->setRegeneratingHealth(false);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) 
        {
            //TC_LOG_ERROR("sql.sql", "DEFIAS CANNON PASSENGER: %u %u %u", passenger->GetGUID(), seatId, apply);
            StopCannon = true;
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            if (!StopCannon)
            {
                std::vector<FireData> FireDatas
                {
                    {234805, 234609, 191793},
                    {234804, 236459, 236053},
                    {234800, 236445, 236055},
                    {234801, 236462, 236054},
                    {234803, 236453, 236051},
                    {234798, 236455, 236052},
                    {234799, 236457, 236050},
                    {234802, 236443, 236056}
                };

                for (uint16 i = 0; i < 8; ++i)
                {
                    if (FireDatas[i].cannonGUID == me->GetDBTableGUIDLow())
                    {
                        MyFireData = FireDatas[i];
                        events.ScheduleEvent(1, 9000);
                        break;
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
         
            if (me->isInCombat())
            {
                bool FoundNonPlayer = false;
                ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
                ThreatContainer::StorageType::const_iterator i = threatlist.begin();
                for (i = threatlist.begin(); i != threatlist.end(); ++i)
                {
                    Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                    if (unit && (unit->GetTypeId() != TYPEID_PLAYER))
                    {
                        FoundNonPlayer = true;
                        break;
                    }
                }

                if (!FoundNonPlayer)
                {
                    //EnterEvadeMode();
                    me->DeleteThreatList();
                    me->CombatStop(true);
                }
            }

            if (!StopCannon)
            {
                if (auto user = FindCreatureByDBGuid(me, MyFireData.userGUID))
                {
                    if (user->isInCombat() || user->isDead())
                        StopCannon = true;
                }
                else StopCannon = true;
            }

            if (uint32 eventID = events.ExecuteEvent())
            {
                if (eventID == 1)
                {
                    if (!StopCannon)
                    {
                        if (auto user = FindCreatureByDBGuid(me, MyFireData.userGUID))
                            user->HandleEmoteCommand(432);

                        if (auto trigger = FindCreatureByDBGuid(me, MyFireData.triggerGUID))
                            me->CastSpell(trigger, 89757, false);

                        events.ScheduleEvent(1, 9000);
                    }
                }
            }
        }

    private:
        EventMap events;
        FireData MyFireData;
        bool StopCannon;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defias_cannonAI(creature);
    }
};

class npc_mine_bunny : public CreatureScript
{
public:
    npc_mine_bunny() : CreatureScript("npc_mine_bunny") { }

    struct npc_mine_bunnyAI : public ScriptedAI
    {
        npc_mine_bunnyAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            DoCast(me, 89842);
        }

        void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
        {
            if (apply)
                PassengerGuids.push_back(passenger->GetGUID());
            else
                PassengerGuids.erase(std::remove(PassengerGuids.begin(), PassengerGuids.end(), passenger->GetGUID()), PassengerGuids.end());
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(1, urand(2000, 4000));
        }

        void JustDied(Unit* /*killer*/)
        {
            for (uint16 i = 0; i < PassengerGuids.size(); ++i)
            {
                if (auto passenger = ObjectAccessor::GetUnit(*me, PassengerGuids[i]))
                {
                    if (passenger->GetEntry() != 48340)
                        passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }          
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            DoMeleeAttackIfReady();

            if (uint32 eventId = events.ExecuteEvent())
            {
                if (eventId == 1)
                {
                    DoCast(91032);
                    events.ScheduleEvent(1, urand(5000, 6000));
                }
            }
        }

    private:
        EventMap events;
        std::vector<uint64> PassengerGuids;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mine_bunnyAI(creature);
    }
};

// 91032
class spell_drunken_haze : public SpellScriptLoader
{
public:
    spell_drunken_haze() : SpellScriptLoader("spell_drunken_haze") { }

    class spell_drunken_haze_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_drunken_haze_AuraScript);

        void Remove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto ply = GetTarget()->ToPlayer())
                ply->SetDrunkValue(0);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_drunken_haze_AuraScript::Remove, EFFECT_2, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_drunken_haze_AuraScript();
    }
};

class npc_gun_powder : public CreatureScript
{
public:
    npc_gun_powder() : CreatureScript("npc_gun_powder") { }

    struct npc_gun_powderAI : public ScriptedAI
    {
        npc_gun_powderAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (!boom)
            {
                boom = true;
                me->CastSpell(me, 89769, false);
                me->CastSpell(me, 34602, true);
                me->Kill(me);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnOrUnsummon(3000);
        }
    private:
        bool boom{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gun_powderAI(creature);
    }
};

/*
Stop Moving at 15 Yards
Cast Throw
Start Moving when not in Throw Range
Cast Throw on Aggro 91038

Flee at 15% HP
Talk(0) at 15% HP
*/

class npc_Deadmines_Mine_Monkey : public CreatureScript
{
public:
    npc_Deadmines_Mine_Monkey() : CreatureScript("npc_Deadmines_Mine_Monkey") { }


    struct npc_Deadmines_Mine_MonkeyAI : public ScriptedAI
    {
        npc_Deadmines_Mine_MonkeyAI(Creature* creature) : ScriptedAI(creature) {}

        bool turnedByPlayers = false;

        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_THROW, urand(2000, 3000));
        }
        void Reset()
        {
            events.CancelEvent(EVENT_THROW);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
        }


        void DoAction(int32 const action)
        {
            switch (action)
            {
            case EVENT_THROW:
                me->GetMotionMaster()->MoveRandom(2.f);
                events.ScheduleEvent(EVENT_THROW, 2000);
                break;
            }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!events.HasEvent(EVENT_DESPAWN))
                if (me->HasReactState(REACT_AGGRESSIVE))
                    events.ScheduleEvent(EVENT_DESPAWN, 20000);

            if (!me->HasUnitState(UNIT_STATE_CASTING) && me->GetDistance(me->getVictim()) <= 15.f)
            {
                me->StopMoving();
                DoCast(spell);
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
            {
                me->GetMotionMaster()->MoveChase(me->getVictim(), 10.f);
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
            }
        }

        void UpdateAI(const uint32 diff) override
        {

            uint32 spell = 0;                         //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;               //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_THROW:
                    spell = SPELL_THROW;               //Spell ID
                    min_repeat = 2500;                 //Minimum Delay
                    max_repeat = 3500;                 //Maximum Delay
                    wait_castnext = true;              //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (UpdateVictim())
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    else
                    {
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, false))
                        {
                            me->AddThreat(target, 1000000.0f);
                            CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        }
                    }
                    break;
                case EVENT_DESPAWN:
                    events.Reset();
                    me->DeleteThreatList();
                    me->DespawnOrUnsummon(0);
                default:
                    break;
                }
        }
    };
private:
    EventMap events;

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Deadmines_Mine_MonkeyAI(creature);
    }
};

enum shadowguard_npc
{
    EVENT_EVASION = 1,
    EVENT_SHADOWSTEP,
    EVENT_SINISTER_STRIKE,
    EVENT_WHIRLING_BLADES,
    EVENT_SINISTER_STRIKE_HC,

    // 49852
    SPELL_CAMOUFLAGE = 90954,
    SPELL_CAMOUFLAGE_HC = 90955,
    SPELL_EVASION = 90958,
    SPELL_SHADOWSTEP = 90956,
    SPELL_SINISTER_STRIKE = 90951,
    SPELL_WHIRLING_BLADES = 90960,
    SPELL_SINISTER_STRIKE_HC = 90952,
};

// 48505
class npc_shadowguard : public CreatureScript
{
public:
    npc_shadowguard() : CreatureScript("npc_shadowguard") { }

    struct npc_shadowguardAI : public ScriptedAI
    {
        npc_shadowguardAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
            if (IsNormal())
                DoCast(SPELL_CAMOUFLAGE);
            if (IsHeroic())
                DoCast(SPELL_CAMOUFLAGE_HC);
        }

        void Reset()
        {
            _events.Reset();
            me->setPowerType(POWER_ENERGY, 100);
            me->SetPower(POWER_ENERGY, 100);
            if (IsNormal())
                DoCast(SPELL_CAMOUFLAGE);
            if (IsHeroic())
                DoCast(SPELL_CAMOUFLAGE_HC);
        }

        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_EVASION, 7000);
            _events.ScheduleEvent(EVENT_SHADOWSTEP, 10000);
            _events.ScheduleEvent(EVENT_SINISTER_STRIKE, 8000);
            _events.ScheduleEvent(EVENT_WHIRLING_BLADES, 15000);
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
                case EVENT_EVASION:
                    DoCast(SPELL_EVASION);
                    if (HealthBelowPct(30))
                        DoCast(SPELL_EVASION);
                    _events.ScheduleEvent(EVENT_EVASION, 7000);
                    break;
                case EVENT_SHADOWSTEP:
                    if (auto v = me->getVictim()) me->CastSpell(v, SPELL_SHADOWSTEP, false);
                    _events.ScheduleEvent(EVENT_SHADOWSTEP, 10000);
                    break;
                case EVENT_SINISTER_STRIKE:
                    DoCastVictim(SPELL_SINISTER_STRIKE);
                    _events.ScheduleEvent(EVENT_SINISTER_STRIKE, 8000);
                    break;
                case EVENT_WHIRLING_BLADES:
                    DoCast(SPELL_WHIRLING_BLADES);
                    _events.ScheduleEvent(EVENT_WHIRLING_BLADES, 15000);
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
        return new npc_shadowguardAI(creature);
    }
};

class npc_blood_wizard : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_blood_wizard' WHERE  `entry`=48417;
        UPDATE `creature_template` SET `ScriptName`='npc_blood_wizard' WHERE  `entry`=48792;
    */

public:
    npc_blood_wizard() : CreatureScript("npc_blood_wizard") { }

    struct npc_blood_wizardAI : ScriptedAI
    {
        npc_blood_wizardAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }
        void InitializeAI()
        {
            _events.Reset();
            me->CastWithDelay(500, me, 90389);
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            _events.Reset();
            me->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
            me->CastWithDelay(500, me, 90389);
        }
        void EnterCombat(Unit* who)
        {
            DoCast(SPELL_BLOODBOLT);
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
                    else
                    {
                        me->SetRooted(false);
                        UpdateVictim();
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        ReAttemptSpell(eventId, false);
                    }
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else
            {
                me->ClearUnitState(UNIT_STATE_ROOT);
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
            EVENT_NEVER_USE = 0, /*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_BLOODBOLT,
            EVENT_RAGEZONE,
            EVENT_BLOOD_WASH_HC,

            SPELL_EVENT_COUNT,   /*END SPELL EVENTS   - THIS EVENT CAN NEVER BE EXECUTED*/
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
                case EVENT_BLOODBOLT:       Spell_Rotation(eventId);                  break;
                case EVENT_RAGEZONE:        Spell_Rotation(eventId);                  break;
                case EVENT_BLOOD_WASH_HC:   Spell_Rotation_Friendly(eventId, 20.f);   break;
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
            me->HandleEmoteCommand(EMOTE_STATE_NONE);
            move_or_attack();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blood_wizardAI(creature);
    }

};

enum enforcer_npc
{
    EVENT_RECKLESSNESS = 1,
    EVENT_ENFORCER_CHARGE,
    EVENT_BLOOD_BAIN,
    EVENT_BLOOD_BAIN_RECOVER,

    // 48502
    SPELL_CHARGE = 90928, // ?? casted by ype: Unit Entry: 48502
    SPELL_BLOOD_BAIN = 90925,
    SPELL_BLOOD_BAIN_2 = 90923,
    SPELL_RECKLESSNESS = 90929,
};

// 48502
class npc_enforcer : public CreatureScript
{
public:
    npc_enforcer() : CreatureScript("npc_enforcer") { }

    struct npc_enforcerAI : public ScriptedAI
    {
        npc_enforcerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            _events.Reset();
        }

        void EnterCombat(Unit* who)
        {
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

                        if (!list_of_players.empty())
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
            if (me->HasReactState(REACT_AGGRESSIVE))        //don't do anything while set to passive
                if (!me->HasUnitState(UNIT_STATE_CASTING))  //dont interrupt active casts
                    if (UpdateVictim())                     //update and respect aggro
                        DoMeleeAttackIfReady();
        }

    private:
        InstanceScript* instance;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enforcerAI(creature);
    }
};

enum npc_parrot
{
    EVENT_EYE_GOUGE = 1,
    EVENT_EYE_PECK,

    SPELL_EYE_GOUGE = 90913,
    SPELL_EYE_PECK = 90920,
};

class npc_Deadmines_parrot : public CreatureScript
{
public:
    npc_Deadmines_parrot() : CreatureScript("npc_Deadmines_parrot") { }

    struct npc_Deadmines_parrotAI : public ScriptedAI
    {

        npc_Deadmines_parrotAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
            hasMoved = false;
        }

        void InitializeAI()
        {
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->isMoving())
                if (!hasMoved)
                {
                    me->GetMotionMaster()->MoveAroundPoint(circle_center_pos, circle_radius, urand(0, 1), circle_point_count, 0.f, true);
                    hasMoved = true;
                }

            if (!me->isInCombat())
                if (who->ToPlayer())
                    if (me->IsValidAttackTarget(who))
                        if (me->GetExactDist2d(who) < 15.f)
                            AttackStart(who);
        }

        void EnterCombat(Unit* who)
        {
            events.Reset();
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_EYE_GOUGE, urand(9000, 12000));
            events.ScheduleEvent(EVENT_EYE_PECK, urand(16000, 19000));
            //if (IsHeroic())//heroic mode check 
        }

        void UpdateAI(const uint32 diff)
        {
            {
                uint32 spell = 0;                    //Do not touch here, Spell ID
                uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
                uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
                bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case EVENT_EYE_GOUGE:
                        spell = SPELL_EYE_GOUGE;         //Spell ID
                        min_repeat = 9000;                 //Minimum Delay
                        max_repeat = 12000;                //Maximum Delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    case EVENT_EYE_PECK:
                        spell = SPELL_EYE_PECK;  //Spell ID
                        min_repeat = 16000;                //Minimum delay
                        max_repeat = 19000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                        break;

                    }

                if (UpdateVictim())
                    if (auto v = me->getVictim())
                    if (me->GetDistance(v) < 6.f)
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            DoMeleeAttackIfReady();
            }
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            Position pos(*me);
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        EventMap events;
        uint8 circle_point_count{ 30 };
        float circle_radius{ frand(5.f, 8.f) };
        bool hasMoved{ false };
        InstanceScript* instance;
        Position circle_center_pos{ me->GetHomePosition().GetPositionX() - circle_radius, me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ() + frand(0.f, 6.f) };
        float followdist;
        bool hasTarget;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Deadmines_parrotAI(creature);
    }
};

class spell_deadmines_gunpowder : public SpellScriptLoader
{
    /*
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('89769', 'spell_deadmines_gunpowder');
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('91063', 'spell_deadmines_gunpowder');
    */
public:
    spell_deadmines_gunpowder() : SpellScriptLoader("spell_deadmines_gunpowder") { }

    class spell_deadmines_gunpowder_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_deadmines_gunpowder_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (auto caster = GetCaster())
            if (unitList.size())
                for (auto itr = unitList.begin(); itr != unitList.end();)
                if (auto npc = (*itr))
                {
                    if (npc->GetEntry() == caster->GetEntry())
                        unitList.erase(itr++);
                    else
                        ++itr;
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deadmines_gunpowder_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deadmines_gunpowder_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_deadmines_gunpowder_SpellScript();
    }
};

enum oaf
{
    SPELL_AXE_THROW = 90098,
    EVENT_AXE_THROW,
};

class npc_Oaf_Lackey : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_Oaf_Lackey' WHERE  `entry`=48445;
    */

public:
    npc_Oaf_Lackey() : CreatureScript("npc_Oaf_Lackey") { }

    struct npc_Oaf_LackeyAI : ScriptedAI
    {
        npc_Oaf_LackeyAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
        }

        void Reset()
        {
            events.Reset();
            Berserk = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_AXE_THROW, urand(5000, 15000));
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (me->HealthBelowPctDamaged(30, damage))
                if (!Berserk)
                {
                    Berserk = true;
                    me->CastSpell(me, 8599, true);
                    Talk(0);
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
                        case EVENT_AXE_THROW:
                        {
                            auto list_of_players = me->GetPlayersInRange(40.f, true);
                            if (list_of_players.size())
                            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            {
                                me->SetFacingTo(me->GetAngle(random_target));
                                me->CastSpell(random_target, SPELL_AXE_THROW);
                            }

                            events.ScheduleEvent(EVENT_AXE_THROW, urand(15000, 20000));
                            break;
                        }
                    }

            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        bool Berserk{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Oaf_LackeyAI(creature);
    }

};

class spell_ogre_motivation : public SpellScriptLoader
{
public:
    spell_ogre_motivation() : SpellScriptLoader("spell_ogre_motivation") { }

    class spell_ogre_motivation_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ogre_motivation_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (auto caster = GetCaster())
            {
                if (unitList.size())
                    for (auto itr = unitList.begin(); itr != unitList.end();)
                        if (auto npc = (*itr))
                        {
                            if (npc->GetEntry() == caster->GetEntry())
                            {
                                unitList.erase(itr++);
                            }
                            else
                            {
                                ++itr;
                            }
                        }

                //add kobolds

                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, caster, 48229, 15.0f);
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                    if (auto c = (*itr))
                        unitList.push_back(c);
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ogre_motivation_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ogre_motivation_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ogre_motivation_SpellScript::FilterTargets, EFFECT_0, TARGET_SRC_CASTER);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ogre_motivation_SpellScript::FilterTargets, EFFECT_1, TARGET_SRC_CASTER);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ogre_motivation_SpellScript();
    }
};

enum squallshaper_npc
{
    ACTION_STOP = 1,
    ACTION_RESUME,
    ACTION_SAVE
};
#define PATH_SQUALLSHAPER 2364350 
#define PATH_SQUALLSHAPER_TWO 2364360
class npc_squallshaper : public CreatureScript
{
public:
    npc_squallshaper() : CreatureScript("npc_squallshaper") { }

    struct npc_squallshaperAI : ScriptedAI
    {
        npc_squallshaperAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {}
        void InitializeAI()
        {
            _events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
            DoAction(55);

            switch (me->GetDBTableGUIDLow())
            {
            case 236435:
                me->GetMotionMaster()->MovePath(PATH_SQUALLSHAPER, true);
                break;
            case 236436:
                me->GetMotionMaster()->MovePath(PATH_SQUALLSHAPER_TWO, true);
                break;

            default:
                return;
            }
        }
        void Reset()
        {
            _events.Reset();
            DoAction(55);
        }
        void EnterCombat(Unit* who)
        {
            DoCast(SPELL_SEASWELL);
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
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        _events.DelayEvent(i, 6000);
            CheckLineOfSight(spell);
            me->AddUnitState(UNIT_STATE_ROOT);
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
                    if (!me->HasUnitState(UNIT_STATE_CASTING) && v->IsWithinLOSInMap(v, spellInfo) && me->GetExactDist(v) <= 30.f)
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
            if (reschedule) {
                me->ClearUnitState(UNIT_STATE_ROOT);
                _events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            }
            else {
                me->ClearUnitState(UNIT_STATE_ROOT);
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

        /*void move_or_attack()
        {
            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        };*/

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_SEASWELL,
            EVENT_RIPTIDE_HC,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_SEASWELL = 90398,
            SPELL_RIPTIDE_HC = 90914,
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

            {EVENT_SEASWELL,           SPELL_SEASWELL,          3400,          4700,          true,       true},
            {EVENT_RIPTIDE_HC,       SPELL_RIPTIDE_HC,          20000,         30000,         true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);
            while (uint32 eventId = _events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SEASWELL:          Spell_Rotation(eventId);  break;
                case EVENT_RIPTIDE_HC:      Spell_Rotation_Friendly(eventId, 20.f); break;
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
                        me->GetMotionMaster()->MoveTargetedHome();
                    }
                case ACTION_SAVE:
                    /*
                    */
                    break;
                    break;
                default:
                    break;
                }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
            case 55:
                if (!events.HasEvent(420))
                    events.ScheduleEvent(420, 4000);
                break;
            case 65:
                HandleWaypointAction(ACTION_STOP);
                break;
            }
        }

    private:
        bool use_melee{ false };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap _events;
        SummonList summons;
        uint32 progress_wp{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_squallshaperAI(creature);
    }

};

void AddSC_deadmines()
{
    new go_canon_defias();
    new boss_vancleef_event();
    new spell_magma_vehicle_dummy();
    new boss_glubtok_event();
    new spell_glaciate_dummy();
    new npc_glubtok_icicle();
    new spell_gb_summon_icicle_target_selector();
    new boss_helix_gearbreaker_event();
    new npc_vanessa_lighting_patter();
    new npc_Glubtok_Fire_patter();
    new boss_foe_reeper_event();
    new npc_heric_HARRINGTON();
    new npc_emma_HARRINGTON();
    new npc_callisa_HARRINGTON();
    new npc_vanessa_paper();
    new go_deadmine_teleporter();
    new npc_vancleef_trap();
    new npc_trap_vapor_event();
    new go_liberate_singe();
    new npc_defias_miner();
    new npc_defias_envoker();
    new npc_defias_cannon();
    new npc_mine_bunny();
    new spell_drunken_haze();
    new npc_gun_powder();
    new npc_Deadmines_Mine_Monkey();
    new npc_shadowguard();
    new npc_blood_wizard();
    new npc_enforcer();
    new npc_Deadmines_parrot();
    new spell_deadmines_gunpowder();
    new npc_Oaf_Lackey();
    new spell_ogre_motivation();
    new npc_squallshaper();
}
