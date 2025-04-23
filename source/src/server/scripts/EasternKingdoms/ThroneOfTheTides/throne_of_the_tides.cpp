/*
 * Copyright (C) 2018-2020 <http://www.projectnelth.com/>
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
#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "throne_of_the_tides.h"

enum eSpells
{
    // Lady Naz'jar Intro
    SPELL_MURLOC_LEACH_VISUAL       = 74563,
    SPELL_MURLOC_LEACH_VISUAL2      = 74568,

    // Naz'jar Spiritmender
    SPELL_HEALING_WAVE              = 76813,
    SPELL_HEX                       = 76820,
    SPELL_WRATH                     = 76815,

    // Naz'jar Tempest Witch
    SPELL_CHAIN_LIGHTNING           = 75813,
    SPELL_LIGHTNING_SURGE           = 75992,

    // Naz'jar Invader
    SPELL_BUBBLE_SHIELD             = 76790,
    SPELL_BUBBLE_SHIELD_DAMAGE      = 76793,
    SPELL_LACERATE                  = 76807,

    // Gilgoblin Hunter
    SPELL_BACKSTAB                  = 76520,
    SPELL_POISONED_SPEAR            = 76516,

    // Gilgoblin Aquamage
    SPELL_TSUNAMI                   = 76560,
    SPELL_WATERBOLT                 = 76535,
    SPELL_WATER_JET_CASTER          = 76410,
    SPELL_WATER_JET_TARGET          = 76406,
    
    // Faceless Watcher
    SPELL_GROUND_POUND              = 76590,
    SPELL_VOID_RIP                  = 76604,
    SPELL_ROOT                      = 75215,

    // Unstable Corruption
    SPELL_UNSTABLE_CORRUPTION_GROWTH= 76362,
    SPELL_WAVE_OF_CORRUPTION        = 76363,

    // Tainted Sentry
    SPELL_SWELL                     = 76634,
    SPELL_SUMMON_UNSTABLE_CORRUPTION= 76625,

    //Defense system
    SPELL_TRIGGERS_SHOCK_EVERYTHING = 85170,
};
enum eEvents
{
    // Lady Naz'jar Intro
    EVENT_LADY_NAZJAR_START         = 1,
    EVENT_LADY_NAZJAR_TALK,
    EVENT_LADY_NAZJAR_WAVE,

    // Naz'jar Spiritmender
    EVENT_HEALING_WAVE,
    EVENT_HEX,
    EVENT_WRATH,

    // Naz'jar Tempest Witch
    EVENT_CHAIN_LIGHTNING,
    EVENT_LIGHTNING_SURGE,

    // Naz'jar Invader
    EVENT_BUBBLE_SHIELD,
    EVENT_LACERATE,

    // Gilgoblin Aquamage
    EVENT_TSUNAMI,

    // Faceless Watcher
    EVENT_VOID_RIP,
    EVENT_CHECK_WIPE,
    EVENT_GROUND_POUND,
    EVENT_GROUND_POUND_END,
    EVENT_FACELESS_1,
    EVENT_FACELESS_2,
    EVENT_FACELESS_3,

    // Tainted Sentry
    EVENT_SWELL,
};
Position const posMurlocs[10] =
{
    { -29.68f, 798.68f, 803.3132f, 0.3405f }, // Right Wave 1 (her left)
    { -29.67f, 795.47f, 803.5563f, 0.3185f }, // Right Wave 2
    { -32.14f, 799.65f, 801.7324f, 0.2109f }, // Right Wave 3
    { -31.54f, 793.97f, 802.3622f, 0.5251f }, // Right Wave 4
    { -33.78f, 796.35f, 801.1160f, 0.2737f }, // Right Wave 5

    { -29.79f, 805.50f, 802.9849f, 6.0818f }, // Left  Wave 1 (her right)


    { -29.32f, 808.99f, 803.1836f, 5.8147f }, // Left  Wave 2
    { -31.90f, 804.70f, 801.8583f, 6.0370f }, // Left  Wave 3
    { -31.46f, 810.28f, 802.3192f, 5.8406f }, // Left  Wave 4
    { -33.85f, 807.58f, 801.2056f, 5.9859f }, // Left  Wave 5
};
Position const unstableCurroptionEvent[9] =
{
    { -312.23f, 950.814f, 245.214f, 5.9632f }, // First Summoner POS
    { -297.58f, 978.202f, 244.794f, 0.1756f }, // Seccond Summoner POS
    { -290.54f, 981.425f, 242.316f, 4.1285f }, // Third Summoner POS

    { -305.37f, 948.440f, 235.376f, 5.8328f }, // First Summon POS
    { -288.02f, 979.956f, 235.164f, 5.3027f }, // Second Summon POS
    { -293.82f, 976.808f, 235.108f, 5.3262f }, // Third Summon POS

    { -309.89f, 890.134f, 245.379f, 1.6584f }, // First Dest POS
    { -293.19f, 958.854f, 236.278f, 1.0694f }, // Second Dest POS
    { -276.50f, 971.481f, 236.816f, 0.5000f }, // Third Dest POS
};
Position const GurshasMobsPOS[8] =
{
    { -308.25f, 778.45f, 260.9903f, 1.1691f }, // Gilgoblin Hunter
    { -313.01f, 778.22f, 261.3322f, 1.6191f },
    { -304.68f, 735.07f, 274.6939f, 1.5390f },
    { -307.23f, 735.15f, 274.6437f, 1.5390f },
    { -310.39f, 735.25f, 274.5816f, 1.5390f },
    { -313.66f, 735.35f, 274.5139f, 1.5390f },
    { -316.47f, 735.16f, 274.5468f, 1.5276f }, // Gilgoblin Aquamage
    { -301.19f, 734.49f, 274.9954f, 1.5273f },
};
Position const FacelessWatcherEventPOS[5] =
{
    { -306.02f, 670.71f, 279.9280f, 3.3492f },
    { -301.64f, 670.99f, 280.0060f, 1.3205f },
    { -303.65f, 665.38f, 280.5880f, 3.0232f },
    { -298.78f, 666.80f, 280.5690f, 6.0965f },
    { -305.61f, 668.56f, 280.0500f, 0.6843f },
};

class at_neptulon_pinacle : public AreaTriggerScript
{
public:
    at_neptulon_pinacle() : AreaTriggerScript("at_neptulon_pinacle") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
    {
        if (player->isGameMaster())
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance || instance->GetData(DATA_PROGRESS_EVENT) >= 2)
            return true;
        instance->SetData(DATA_PROGRESS_EVENT, 2);
        return true;
    }
};

class at_abyssal_halls : public AreaTriggerScript
{
public:
    at_abyssal_halls() : AreaTriggerScript("at_abyssal_halls") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
    {
        if (player->isGameMaster())
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance || instance->GetData(DATA_PROGRESS_EVENT) >= 1)
            return true;
        instance->SetData(DATA_PROGRESS_EVENT, 1);
        return true;
    }
};

class go_defence_throne: public GameObjectScript
{
public:
    go_defence_throne () : GameObjectScript("go_defence_throne") { }

    enum events
    {
        EVENT_OPEN_GATE = 1,
        EVENT_LEFT_TENTACLE,
        EVENT_RIGHT_TENTACLE,
        EVENT_OZUMAT_LEAVE,
    };

    bool OnGossipHello(Player* pPlayer, GameObject* go)
    {
        if (auto instance = go->GetInstanceScript())
        {
            if (instance->GetData(DATA_LADY_NAZJAR_EVENT) != DONE || instance->GetData(DATA_PROGRESS_EVENT) >= 3)
                return true;
            //TC_LOG_ERROR("sql.sql", "Sending cutscene and scheduling open gate.");
            go->AI()->DoAction(EVENT_OPEN_GATE);
            return false;
        }
        return true;
    }

    struct go_defence_throneAI : public GameObjectAI
    {

        go_defence_throneAI(GameObject* go) : GameObjectAI(go)
        {
        }
        void DoAction(const int32 param) 
        {
            switch (param)
            {
            case EVENT_OPEN_GATE:
                events.ScheduleEvent(EVENT_OPEN_GATE, 5000);

                events.ScheduleEvent(EVENT_LEFT_TENTACLE, 15000);
                events.ScheduleEvent(EVENT_RIGHT_TENTACLE, 20000);
                events.ScheduleEvent(EVENT_OZUMAT_LEAVE, 28000);


                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);



                if (auto ozumat = Unit::GetCreature(*go, instance->GetData64(BOSS_OZUMAT_EVENT)))
                {
                    
                    ozumat->AI()->DoAction(1007);
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, ozumat, 361710, 65.f);
                    for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                        if (auto c = (*itr))
                        {
                            c->CastWithDelay(urand(0, 1500), ozumat, 86618, true);
                            c->CastWithDelay(urand(1500, 2500), ozumat, 86618, true);
                            c->CastWithDelay(urand(2500, 3500), ozumat, 86618, true);
                            c->CastWithDelay(urand(3500, 4500), ozumat, 86618, true);
                            c->CastWithDelay(urand(4500, 5500), ozumat, 86618, true);
                            c->CastWithDelay(urand(5500, 6500), ozumat, 86618, true);
                            c->CastWithDelay(urand(6500, 7500), ozumat, 86618, true);
                            c->CastWithDelay(urand(7500, 8500), ozumat, 86618, true);
                            c->CastWithDelay(urand(8500, 9500), ozumat, 86618, true);
                            c->CastWithDelay(urand(9500, 10500), ozumat, 86618, true);
                            c->CastWithDelay(urand(10500, 11500), ozumat, 86618, true);
                            c->CastWithDelay(urand(11500, 12500), ozumat, 86618, true);
                            c->CastWithDelay(urand(12500, 13500), ozumat, 86618, true);
                            c->CastWithDelay(urand(13500, 14500), ozumat, 86618, true);
                            c->CastWithDelay(urand(14500, 15500), ozumat, 86618, true);
                            c->CastWithDelay(urand(15500, 16500), ozumat, 86618, true);
                            c->CastWithDelay(urand(16500, 17500), ozumat, 86618, true);
                            c->CastWithDelay(urand(17500, 18500), ozumat, 86618, true);
                            c->CastWithDelay(urand(18500, 19500), ozumat, 86618, true);
                            c->CastWithDelay(urand(19500, 20500), ozumat, 86618, true);
                            c->CastWithDelay(urand(20500, 21500), ozumat, 86618, true);
                            c->CastWithDelay(urand(21500, 22500), ozumat, 86618, true);
                            c->CastWithDelay(urand(22500, 23500), ozumat, 86618, true);
                            c->CastWithDelay(urand(23500, 24500), ozumat, 86618, true);
                            c->CastWithDelay(urand(24500, 25500), ozumat, 86618, true);
                            c->CastWithDelay(urand(25500, 26500), ozumat, 86618, true);
                            c->CastWithDelay(urand(26500, 27500), ozumat, 86618, true);
                            c->CastWithDelay(urand(27500, 28500), ozumat, 86618, true);
                            c->CastWithDelay(urand(28500, 29500), ozumat, 86618, true);
                        }


                    instance->DoOnCreatures([](Creature* c)
                    {
                        if (c->GetEntry() == 361710) //"not includes", would be != end(myArray) to be includes.
                            c->AddAura(85170, c);
                    });


                }



                instance->SendCinematicStartToPlayers(169);
                break;
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_OPEN_GATE:
                    instance->SetData(DATA_PROGRESS_EVENT, 3);
                    instance->SetData(DATA_EVENT_COMMANDER_ULTHOK_START, IN_PROGRESS);
                    break;
                case EVENT_LEFT_TENTACLE:
                    if (auto go_guid = instance->GetData64(GO_WALL_TENTACLE_LEFT))
                        if (auto go = instance->instance->GetGameObject(go_guid))
                            go->Delete();

                    if (auto go_guid = instance->GetData64(GO_TENTACLE_LEFT))
                        if (auto go = instance->instance->GetGameObject(go_guid))
                            go->AI()->DoAction(5);
                    break;
                case EVENT_RIGHT_TENTACLE:
                    if (auto go_guid = instance->GetData64(GO_WALL_TENTACLE_RIGHT))
                        if (auto go = instance->instance->GetGameObject(go_guid))
                            go->Delete();

                    if (auto go_guid = instance->GetData64(GO_TENTACLE_RIGHT))
                        if (auto go = instance->instance->GetGameObject(go_guid))
                            go->AI()->DoAction(5);

                    break;
                case EVENT_OZUMAT_LEAVE:
                    if (auto ozumat = Unit::GetCreature(*go, instance->GetData64(BOSS_OZUMAT_EVENT)))
                        ozumat->AI()->DoAction(1008);
                    break;
                }
        }

    private:
        InstanceScript* instance{ go->GetInstanceScript()};
        EventMap events;

    };

    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_defence_throneAI(go);
    }
};

/*
    Abyssal Hall 6774 & 5873
*/
class at_abyssal_halls_x : public AreaTriggerScript
{
public:
    at_abyssal_halls_x() : AreaTriggerScript("at_abyssal_halls_x") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
    {
        if (player->isGameMaster())
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance || instance->GetData(DATA_LADY_NAZJAR_EVENT) != DONE || instance->GetData(DATA_COMMANDER_ULTHOK) == DONE)
            return true;
        Position pos = { 50.57f, 802.39f, 845.0332f, 0.0f };
        if (!instance->instance->GetCreature(instance->GetData64(DATA_COMMANDER_ULTHOK)))
            if (Creature *c = instance->instance->SummonCreature(BOSS_COMMANDER_ULTHOK, pos))
                c->AI()->DoAction(42);
        return true;
    }
};

/*
    Coral Garden 6646
*/
class at_coral_garden : public AreaTriggerScript
{
public:
    at_coral_garden() : AreaTriggerScript("at_coral_garden") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
    {
        if (player->isGameMaster())
            return false;

        InstanceScript* instance = player->GetInstanceScript();
        if (!instance || instance->GetData(DATA_COMMANDER_ULTHOK) != DONE)
            return true;
        if (auto neptulon = instance->instance->GetCreature(instance->GetData64(DATA_NEPTULON)))
        {
            neptulon->AI()->DoAction(ACTION_NEPTULON_INTRO);
        }
        return true;
    }
};

/*
    Throne of the Tides Teleporter - 51395 & 51391
*/
class npc_throne_of_tides_teleporter : public CreatureScript
{
public:
    npc_throne_of_tides_teleporter() : CreatureScript("npc_throne_of_tides_teleporter") { }

    struct npc_throne_of_tides_teleporterAI : public PassiveAI
    {
        npc_throne_of_tides_teleporterAI(Creature* creature) : PassiveAI(creature)
        {
            me->SetSpeed(MOVE_FLIGHT, 0.35f);
            instance = me->GetInstanceScript();
        }

        void InitializeAI()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_PREVENT_SPELL_CLICK);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void OnSpellClick(Unit* passenger, bool& result)
        {
            //TC_LOG_ERROR("sql.sql", "recognized spellclick.");
            if (passenger->isInCombat())
            {
                if (auto passPlayer = passenger->ToPlayer())
                    if (auto s = passPlayer->GetSession())
                        s->SendNotification(s->GetTrinityString(LANG_YOU_IN_COMBAT));
                return;
            }

            if (instance)
                if (me->GetMapId() == 643/*Throne of tides*/)
                    if (instance->GetDeadBossTotal(true) >= 2)
                    {
                        switch (me->GetEntry())
                        {
                        case NPC_TOT_TELEPORTER_F1:
                            passenger->NearTeleportTo(-14.49f, 796.29f, 808.1588f, 1.7414f);
                            break;
                        case NPC_TOT_TELEPORTER_F2:
                            passenger->NearTeleportTo(-560.16f, 819.19f, 245.2784f, 5.0670f);
                            break;
                        default:
                            break;
                        }
                    }
        };
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_throne_of_tides_teleporterAI(creature);
    }
};

/*
    Lady Naz'jar - 39959(N) & 49080(H)
*/
class npc_lady_nazjar_intro : public CreatureScript
{
public:
    npc_lady_nazjar_intro() : CreatureScript("npc_lady_nazjar_intro") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_nazjar_introAI(creature);
    }
    struct npc_lady_nazjar_introAI : public ScriptedAI
    {
        npc_lady_nazjar_introAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_PASSIVE);
            instance->HandleGameObject(instance->GetData64(GO_ABYSSAL_MAW_01), false, instance->instance->GetGameObject(instance->GetData64(GO_ABYSSAL_MAW_01)));
            events.Reset();
            for (int i = 0; i < 10; i++)
                if (auto murloc = me->SummonCreature(NPC_DEEP_MURLOC_DRUDGE, posMurlocs[i], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    murlocGUIDs[i] = murloc->GetGUID();
                    if (i <= 4)
                        murloc->AI()->DoCast(SPELL_MURLOC_LEACH_VISUAL2);
                    else
                        murloc->AI()->DoCast(SPELL_MURLOC_LEACH_VISUAL);
                    murloc->SetReactState(REACT_PASSIVE);
                    murloc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                    murlocs.push_back(murloc);
                }
        }
        void DoAction(int const action)
        {
            switch (action)
            {
            case ACTION_LADY_NAZJAR_INTRO_START:
                events.ScheduleEvent(EVENT_LADY_NAZJAR_START, 1000);
                break;
            case ACTION_LADY_NAZJAR_INTRO_END:
                events.CancelEvent(EVENT_LADY_NAZJAR_WAVE);
                instance->DoUseDoorOrButton(instance->GetData64(GO_ABYSSAL_MAW_01));
                instance->SetData(DATA_LADY_NAZJAR_INTRO, DONE);
                me->GetMotionMaster()->MovePoint(1, 34.73f, 799.56f, 806.32f);
                me->DespawnOrUnsummon(6000);
                events.ScheduleEvent(EVENT_LADY_NAZJAR_TALK, 1);
                break;
            case ACTION_LADY_NAZJAR_INTRO_COUNTER:
                //TC_LOG_ERROR("sql.ql", "Lady nazjar received and recognizes ACTION_LADY_NAZJAR_INTRO_COUNTER");
                deadMobs += 1;

                if (deadMobs == 1 || deadMobs == 3)
                {
                    //TC_LOG_ERROR("sql.ql", "Executing Release murlocs");
                    ReleaseMurlocs(deadMobs);
                }

                if (deadMobs == 6)
                {
                    //TC_LOG_ERROR("sql.ql", "Executing ACTION_LADY_NAZJAR_INTRO_END");
                    DoAction(ACTION_LADY_NAZJAR_INTRO_END);
                }
                break;
            default:
                break;
            }
        }
        void ReleaseMurlocs(int stage)
        {
            if (stage != 1 && stage != 3)
                return;

            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, NPC_DEEP_MURLOC_DRUDGE, 30.0f);

            if (creatures.size())
            for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                if (auto c = (*itr))    //if they exist (null check)
                    for (auto i = (stage == 1 ? 0 : 5); i < (stage == 1 ? 5 : 10); i++)
                    if (c->GetGUID() == murlocGUIDs[i])
                    {
                        //TC_LOG_ERROR("sql.sql", "HAHA WUFC RED TEXT GO BRRRRRRRRRRRRRRRRRRRRRRRRRRR");
                        c->CastStop();
                        c->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                        c->RemoveAura(SPELL_MURLOC_LEACH_VISUAL2);
                        c->RemoveAura(SPELL_MURLOC_LEACH_VISUAL);
                        c->SetDisableGravity(false);
                        c->SetSpeed(MOVE_RUN, 1.5f, true);
                        c->GetMotionMaster()->MovePoint(1, -147.0f + frand(-5.0f, 5.0f), 802.55f + frand(-5.0f, 5.0f), 796.1f);
                        c->SetUnitMovementFlags(MOVE_RUN);
                        c->SetReactState(REACT_AGGRESSIVE);
                    }

        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_LADY_NAZJAR_START:
                        Talk(0);
                        break;
                    case EVENT_LADY_NAZJAR_TALK:
                    {
                        Talk(1);

                        /*
                            Clean up any remaining leashed murlocs
                        */
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, NPC_DEEP_MURLOC_DRUDGE, 20.0f);
                        if (creatures.size())
                            for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                                if (auto c = (*itr))    //if they exist (null check)
                                    if (c->HasReactState(REACT_PASSIVE))
                                    {
                                        c->RemoveAura(SPELL_MURLOC_LEACH_VISUAL2);
                                        c->RemoveAura(SPELL_MURLOC_LEACH_VISUAL);
                                        c->SetDisableGravity(false);
                                        c->SetSpeed(MOVE_RUN, 1.5f, true);
                                        c->SetUnitMovementFlags(MOVE_RUN);
                                        c->SetReactState(REACT_AGGRESSIVE);
                                        c->SetInCombatWithZone();
                                    }
                    }
                        break;
                    case EVENT_LADY_NAZJAR_WAVE:
                        break;
                    default:
                        break;
                }
        }
    private:
        uint64 murlocGUIDs[10]
        {
            NULL, NULL, NULL,
            NULL, NULL, NULL,
            NULL, NULL, NULL,
            NULL,
        };
        EventMap events;
        InstanceScript* const instance;
        std::list<Creature*> murlocs;
        int deadMobs = 0;
    };
};

/*
    Naz'jar Spiritmender - 41139(N) & 49091(H)
*/
class npc_nazjar_spiritmenders : public CreatureScript
{
public:
    npc_nazjar_spiritmenders() : CreatureScript("npc_nazjar_spiritmenders") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nazjar_spiritmendersAI(creature);
    }
    struct npc_nazjar_spiritmendersAI : public ScriptedAI
    {
        npc_nazjar_spiritmendersAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_HEALING_WAVE, 4000);
            events.ScheduleEvent(EVENT_HEX, urand(3000, 4000));
            events.ScheduleEvent(EVENT_WRATH, urand(500, 1000));
        }
        void JustDied(Unit* /*killer*/)
        {
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == EFFECT_MOTION_TYPE)
                if (id == 554 || id == 553)
                {
                    me->SetHomePosition(*me);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H_ALLOW_MOVEMENT);
                    me->SetDisableGravity(false);
                    me->SetCanFly(false);
                    me->SetHover(false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                    //me->GetMotionMaster()->MovePoint(500, me->GetHomePosition());
                    me->SetFacingTo(M_PI);
                }
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HEALING_WAVE:
                    events.RescheduleEvent(EVENT_HEALING_WAVE, 4000);
                    if (auto ally = me->FindNearestCreature(NPC_NAZJAR_INVADER, 35.0f))
                    {
                        if (ally->isInCombat() && ally->GetHealthPct() <= 75.0f)
                        {
                            me->CastSpell(ally, SPELL_HEALING_WAVE);
                            return;
                        }
                    }
                    if (auto ally = me->FindNearestCreature(NPC_NAZJAR_TEMPEST_WITCH, 35.0f))
                    {
                        if (ally->isInCombat() && ally->GetHealthPct() <= 75.0f)
                        {
                            me->CastSpell(ally, SPELL_HEALING_WAVE);
                            return;
                        }
                    }
                    if (me->GetHealthPct() <= 75.0f)
                        DoCast(me, SPELL_HEALING_WAVE);
                    break;
                case EVENT_HEX:
                    if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 35.0f, true))
                        DoCast(victim, SPELL_HEX);
                    events.RescheduleEvent(EVENT_HEX, urand(11000, 12000));
                    break;
                case EVENT_WRATH:
                    if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 35.0f, true))
                        DoCast(victim, SPELL_WRATH);
                    events.RescheduleEvent(EVENT_WRATH, 6000);
                    break;
                default:
                    break;
                }
            }
            if (auto v = me->getVictim())
                if (v->isAlive())
                    if (me->GetDistance(v) < 35.f)
                        me->StopMoving();

            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };
};

/*
    Naz'jar Tempest Witch - 40634(N) & 49092(H)
*/
class npc_nazjar_tempest_witch : public CreatureScript
{
public:
    npc_nazjar_tempest_witch() : CreatureScript("npc_nazjar_tempest_witch") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nazjar_tempest_witchAI(creature);
    }
    struct npc_nazjar_tempest_witchAI : public ScriptedAI
    {
        npc_nazjar_tempest_witchAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(6000, 8000));
            events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(8500, 10500));
        }
        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CHAIN_LIGHTNING:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 28.0f, true))
                            DoCast(victim, SPELL_CHAIN_LIGHTNING);
                        events.RescheduleEvent(EVENT_CHAIN_LIGHTNING, urand(13000, 17000));
                    }
                    break;
                case EVENT_LIGHTNING_SURGE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                            DoCast(victim, SPELL_LIGHTNING_SURGE);
                        events.RescheduleEvent(EVENT_LIGHTNING_SURGE, urand(18000, 21000));
                    }
                    break;
                default:
                    break;
                }
            }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };
};

/*
    Naz'jar Invader - 39616(N) & 49085(H)
*/
class npc_nazjar_invader : public CreatureScript
{
public:
    npc_nazjar_invader() : CreatureScript("npc_nazjar_invader") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nazjar_invaderAI(creature);
    }
    struct npc_nazjar_invaderAI : public ScriptedAI
    {
        npc_nazjar_invaderAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
            shield = false;
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_LACERATE, urand(2000, 3000));
        }
        void DamageTaken(Unit* /*attacker*/, uint32&/*damage*/)
        {
            if (!shield && me->GetHealthPct() <= 50.0f)
            {
                shield = true;
                events.ScheduleEvent(EVENT_BUBBLE_SHIELD, 500);
            }
        }
        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LACERATE:
                    DoCastVictim(SPELL_LACERATE);
                    events.RescheduleEvent(EVENT_LACERATE, urand(4000, 5000));
                    break;
                case EVENT_BUBBLE_SHIELD:
                    DoCast(SPELL_BUBBLE_SHIELD);
                    events.RescheduleEvent(EVENT_BUBBLE_SHIELD, urand(8000, 10000));
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        bool shield;
    };
};

enum gilgob_hunt
{
    // Gilgoblin Hunter
    EVENT_BACKSTAB = 1,
    EVENT_POISONED_SPEAR,
    EVENT_WEAPON_SHOOT,
    EVENT_CORRECT_EMOTE,

    SPELL_NORMAL_SHOT = 83877,
};

class npc_gilgoblin_hunter : public CreatureScript
{
public:
    npc_gilgoblin_hunter() : CreatureScript("npc_gilgoblin_hunter") { }


    struct npc_gilgoblin_hunterAI : public ScriptedAI
    {

        npc_gilgoblin_hunterAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_BACKSTAB,
            EVENT_POISONED_SPEAR,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

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
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_BACKSTAB,            SPELL_BACKSTAB,         5000,           10000,          true,       true},
            {EVENT_POISONED_SPEAR,      SPELL_POISONED_SPEAR,   7000,           20000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BACKSTAB:     Spell_Rotation(eventId);  break;
                case EVENT_POISONED_SPEAR:   Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilgoblin_hunterAI(creature);
    }
};

enum gilgob_mage
{
    EVENT_WATERBOLT = 2,
};

class npc_gilgoblin_aquamage : public CreatureScript
{
public:
    npc_gilgoblin_aquamage() : CreatureScript("npc_gilgoblin_aquamage") { }


    struct npc_gilgoblin_aquamageAI : public ScriptedAI
    {

        npc_gilgoblin_aquamageAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_TSUNAMI,
            EVENT_WATERBOLT,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_TSUNAMI = 76560,
            SPELL_WATERBOLT = 76535,
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
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_TSUNAMI,     SPELL_TSUNAMI,      13000,          30000,          true,       true},
            {EVENT_WATERBOLT,   SPELL_WATERBOLT,    5000,           8000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_TSUNAMI:     Spell_Rotation(eventId);  break;
                case EVENT_WATERBOLT:   Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilgoblin_aquamageAI(creature);
    }
};

/*
    Faceless Watcher - 40936(N) & 49074(H)
*/
class npc_faceless_watcher : public CreatureScript
{
public:
    npc_faceless_watcher() : CreatureScript("npc_faceless_watcher") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_faceless_watcherAI(creature);
    }
    struct npc_faceless_watcherAI : public ScriptedAI
    {
        npc_faceless_watcherAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
            if (me->GetDBTableGUIDLow() == 197849)
                isAbuser = true;
                
            if (isAbuser && !summon)
            {
                me->HandleEmoteCommand(EMOTE_STATE_ROAR);
                summon = true;
                for (int i = 0; i < 4; i++)
                    if (auto c = me->SummonCreature(i < 2 ? NPC_GILGOBLIN_AQUAMAGE : NPC_GILGOBLIN_HUNTER, FacelessWatcherEventPOS[i], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        mobGUIDs[i] = (c->GetGUID());
                        c->SetFacingTo(c->GetAngle(me));
                        c->HandleEmoteCommand(EMOTE_STATE_COWER);
                    }
            }
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_VOID_RIP, urand(5000, 7000));
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
            summons.DespawnAll();
            me->DeleteThreatList();
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
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
                case EVENT_VOID_RIP:
                    lockOrientation = me->GetOrientation();
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->StopMoving();
                    me->AddUnitState(UNIT_STATE_ROOT);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    DoCast(SPELL_VOID_RIP);
                    me->SetFacingTo(lockOrientation);
                    events.ScheduleEvent(EVENT_GROUND_POUND, 3000);
                    break;
                case EVENT_GROUND_POUND:
                {
                    auto players = me->GetPlayersInRange(40.f, true);
                    if (players.size())
                        for (Player* player : players)
                            player->RemoveAura(76609);

                                DoCast(SPELL_GROUND_POUND);
                                me->SetFacingTo(lockOrientation);
                    events.ScheduleEvent(EVENT_GROUND_POUND_END, 6000);
                }
                    break;
                case EVENT_GROUND_POUND_END:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetFacingTo(lockOrientation);
                    UpdateVictim();
                    events.ScheduleEvent(EVENT_VOID_RIP, urand(18000,20000));
                    break;
                case EVENT_FACELESS_1:  //ground pound and roar
                {
                    //me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                    //mobs stunned
                    //events.ScheduleEvent(EVENT_FACELESS_2, 3000);
                    break;
                }
                case EVENT_FACELESS_2:
                {
                    //me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    //mobs terrified
                            //(*itr)->HandleEmoteCommand(EMOTE_STATE_COWER);
                    //events.ScheduleEvent(EVENT_FACELESS_3, 10000);
                    break;
                }
                case EVENT_FACELESS_3:
                {
                    //me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    //events.ScheduleEvent(EVENT_FACELESS_1, urand(20000, 30000));
                    break;
                }
                default:
                    break;
                }

            if (me->HasReactState(REACT_AGGRESSIVE))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        float lockOrientation{ 0.f };
        bool isAbuser{ false };
        EventMap events;
        bool summon{ false };
        uint64 mobGUIDs[4] {0, 0, 0, 0};
    };
};

/*
    Unstable Corruption Trigger - 361711
*/
class npc_unstable_corruption_trigger : public CreatureScript
{
public:
    npc_unstable_corruption_trigger() : CreatureScript("npc_unstable_corruption_trigger") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unstable_corruption_triggerAI(creature);
    }
    struct npc_unstable_corruption_triggerAI : public ScriptedAI
    {
        npc_unstable_corruption_triggerAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
        }

        void Reset(){ }
        void UpdateAI(uint32 diff)
        {
            if (summonerId == 0)
            {
                if (me->GetDistance2d(unstableCurroptionEvent[0].m_positionX, unstableCurroptionEvent[0].m_positionY) <= 2.0f)
                    summonerId = 1;
                if (me->GetDistance2d(unstableCurroptionEvent[1].m_positionX, unstableCurroptionEvent[1].m_positionY) <= 2.0f)
                    summonerId = 2;
                if (me->GetDistance2d(unstableCurroptionEvent[2].m_positionX, unstableCurroptionEvent[2].m_positionY) <= 2.0f)
                    summonerId = 3;
            }

            if (instance->GetData(DATA_COMMANDER_ULTHOK) == DONE && me->FindNearestPlayer(50.0f))
            {
                if (summonTimer <= diff)
                {
                    me->SummonCreature(NPC_UNSTABLE_CORRUPTION, unstableCurroptionEvent[summonerId + 2], TEMPSUMMON_MANUAL_DESPAWN);
                    if (!started)
                    {
                        summonTimer = 1000;
                        started = true;
                    }
                    else
                        summonTimer = 15000;
                }
                summonTimer -= diff;
            }
        }
    private:
        int summonerId = 0;
        bool started = false;
        uint32 summonTimer = 15000;
        InstanceScript* instance;
    };
};

/*
    Unstable Corruption - 40923(N) & 49103(H)
*/
class npc_unstable_corruption : public CreatureScript
{
public:
    npc_unstable_corruption() : CreatureScript("npc_unstable_corruption") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unstable_corruptionAI(creature);
    }
    struct npc_unstable_corruptionAI : public ScriptedAI
    {
        npc_unstable_corruptionAI(Creature* creature) : ScriptedAI(creature) { }
        
        void Reset(){ }
        void JustDied(Unit* /*killer*/)
        {
            me->CastSpell(me, SPELL_WAVE_OF_CORRUPTION, true);
            me->DespawnOrUnsummon(5000);
        }
        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(15.0f);
        }
        void MovementInform(uint32 Type, uint32 Id)
        {
            if (Id == 1001)
            {
                me->SetHomePosition(unstableCurroptionEvent[summonId + 5]);
                me->GetMotionMaster()->MoveRandom(6.0f);
            }
        }
        void UpdateAI(uint32 diff)
        {
            if (DeathTimer <= diff)
                me->Kill(me, false, false);
            else
                DeathTimer -= diff;

            if (!UpdateVictim())
            {
                if (summonId == 0)
                {
                    if (me->GetDistance2d(unstableCurroptionEvent[3].m_positionX, unstableCurroptionEvent[3].m_positionY) <= 2.0f)
                    {
                        summonId = 1;
                        summon = true;
                        DeathTimer = 78000;
                    }
                    if (me->GetDistance2d(unstableCurroptionEvent[4].m_positionX, unstableCurroptionEvent[4].m_positionY) <= 2.0f)
                    {
                        summonId = 2;
                        summon = true;
                        DeathTimer = 78000;
                    }
                    if (me->GetDistance2d(unstableCurroptionEvent[5].m_positionX, unstableCurroptionEvent[5].m_positionY) <= 2.0f)
                    {
                        summonId = 3;
                        summon = true;
                        DeathTimer = 78000;
                    }
                    DoCast(SPELL_UNSTABLE_CORRUPTION_GROWTH);
                }

                if (summon)
                {
                    if (moveTimer <= diff)
                    {
                        me->GetMotionMaster()->MovePoint(1001, unstableCurroptionEvent[5 + summonId].m_positionX + frand(-7.0f, 7.0f), unstableCurroptionEvent[5 + summonId].m_positionY + frand(-7.0f, 7.0f),
                            unstableCurroptionEvent[5 + summonId].m_positionZ);
                        summon = false;
                    }
                    moveTimer -= diff;
                }
                return;
            }

            DoMeleeAttackIfReady();
        }
    private:
        int summonId = 0;
        uint32 moveTimer = 3000;
        uint32 DeathTimer = 300000;
        bool summon = false;
    };
};

/*
    Tainted Sentry - 40925(N) & 49102(H)
*/
class npc_tainted_sentry : public CreatureScript
{
public:
    npc_tainted_sentry() : CreatureScript("npc_tainted_sentry") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tainted_sentryAI(creature);
    }
    struct npc_tainted_sentryAI : public ScriptedAI
    {
        npc_tainted_sentryAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*attacker*/)
        {
            events.ScheduleEvent(EVENT_SWELL, urand(2000, 3000));
        }
        void JustDied(Unit* /*killer*/)
        {
            DoCast(SPELL_SUMMON_UNSTABLE_CORRUPTION);
            if (isEventMob)
                if (auto door = me->FindNearestGameObject(GO_ABYSSAL_MAW_04, 50.0f))
                    door->AI()->DoAction(1001);
        }
        void UpdateAI(uint32 diff)
        {
            if (!isEventMob && me->GetDistance2d(-227.027f, 981.130f) <= 5.0f)
                isEventMob = true;

            if (!UpdateVictim())
                return;

            events.Update(diff);
            if (events.ExecuteEvent() == EVENT_SWELL)
            {
                DoCast(SPELL_SWELL);
                events.RescheduleEvent(EVENT_SWELL, urand(14000, 16000));
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        bool isEventMob = false;
    };
};

/*
    Doodad Abyssal Maw Door 4 - 204341
*/
class go_doodad_abyssal_maw_door_4 : public GameObjectScript
{
public:
    go_doodad_abyssal_maw_door_4() : GameObjectScript("go_doodad_abyssal_maw_door_4") { }
    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_doodad_abyssal_maw_door_4AI(go);
    }
    struct go_doodad_abyssal_maw_door_4AI : public GameObjectAI
    {
        go_doodad_abyssal_maw_door_4AI(GameObject* go): GameObjectAI(go) 
        { 
            instance = go->GetInstanceScript();
        }

        void DoAction(int const action)
        {
            if (action == 1001)
                deadMobs += 1;
        }
    private:
        int deadMobs{ 0 };
        InstanceScript* instance;
    };
};

/*
    Bubble Shield - 76790
*/
class spell_bubble_shield : public SpellScriptLoader
{
public:
    spell_bubble_shield() : SpellScriptLoader("spell_bubble_shield") { }
    class spell_bubble_shieldAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_bubble_shieldAuraScript);
        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }
        void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastCustomSpell(SPELL_BUBBLE_SHIELD_DAMAGE, SPELLVALUE_BASE_POINT0, eventInfo.GetDamageInfo()->GetDamage(), eventInfo.GetProcTarget(), true);
        }
        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_bubble_shieldAuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_bubble_shieldAuraScript();
    }
};

/*
    Void Rip - 76609
*/
class spell_void_rip : public SpellScriptLoader
{
public:
    spell_void_rip() : SpellScriptLoader("spell_void_rip") { };
    class spell_void_ripAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_void_ripAuraScript);
        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_VOID_RIP))
                return false;
            return true;
        }
        bool Load()
        {
            return true;
        }
        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit * caster = GetCaster())
                if (Unit * target = GetTarget())
                if (auto p = target->ToPlayer())
                if (!target->IsJumping())
                if (!target->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY))
                {
                    float o = caster->GetOrientation();
                    float z = caster->GetPositionZ() + 13.0f;
                    float x = caster->GetPositionX();
                    float y = caster->GetPositionY();
                    float dist{ caster->GetExactDist(target) };

                    target->GetMotionMaster()->ForceMoveJump(x, y, z, dist > 9.f ? dist : 9.f, 9.0f);
                    target->SetDisableGravity(true);
                }
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto caster = GetCaster())
                if (auto target = GetTarget())
                if (target->ToPlayer())
                {
                    target->SetDisableGravity(false);
                    target->GetMotionMaster()->MoveFall();
                }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_void_ripAuraScript::HandleOnEffectApply, EFFECT_2, SPELL_AURA_MOD_PACIFY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_void_ripAuraScript::HandleOnEffectRemove, EFFECT_2, SPELL_AURA_MOD_PACIFY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_void_ripAuraScript();
    }
};

class npc_ToT_WaveSpeaker : public CreatureScript
{
public:
    npc_ToT_WaveSpeaker() : CreatureScript("npc_ToT_WaveSpeaker") { }

    struct npc_ToT_WaveSpeakerAI : public ScriptedAI
    {
        npc_ToT_WaveSpeakerAI(Creature* creature) : ScriptedAI(creature)
        {
        }

    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ToT_WaveSpeakerAI(creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
            if (action == 1)
            {
                player->PlayerTalkClass->ClearMenus();
                player->CLOSE_GOSSIP_MENU();

                switch (creature->GetDBTableGUIDLow())
                {
                case 221438:    //outside instance
                    if (player->GetTeam() == ALLIANCE)
                        player->TeleportTo(0, -5313.601074f, 3920.409668f, 3.374406f, 0.752197f);
                    else
                        player->TeleportTo(0, -5668.102539f, 3722.001709f, 2.915726f, 3.179066f);
                    break;
                case 351439:    //teleporter location
                case 370974:
                    player->TeleportTo(0, -5583.251465f, 5434.647949f, -1798.082520f, 4.532623f);
                    break;
                default:
                    break;
                }
            }
        return true;
    }
};

class npc_nazjar_intro_murloc : public CreatureScript
{
public:
    npc_nazjar_intro_murloc() : CreatureScript("npc_nazjar_intro_murloc") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nazjar_intro_murlocAI(creature);
    }
    struct npc_nazjar_intro_murlocAI : public ScriptedAI
    {
        npc_nazjar_intro_murlocAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->SetSpeed(MOVE_RUN, 1.5f, true);
            me->SetUnitMovementFlags(MOVE_RUN);
        }

        void UpdateAI(uint32 diff)
        {
            if (!me->isInCombat())
            {
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (auto p = me->FindNearestPlayer(40.f))
                        AttackStart(p);
            }
            else
            {
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }
        }

    private:
        InstanceScript* instance;
    };
};

class go_tot_tentacle : public GameObjectScript
{
public:
    go_tot_tentacle() : GameObjectScript("go_tot_tentacle") { }

    struct go_tot_tentacleAI : public GameObjectAI
    {

        go_tot_tentacleAI(GameObject* go) : GameObjectAI(go)
        {
        }

        void InitializeAI()
        {
        }


        void DoAction(const int32 param) 
        {
            if (param == 5)
            {
                events.ScheduleEvent(1, 4000);
                go->SetLootState(GO_NOT_READY);
                go->SetGoState(GO_STATE_READY);
                go->SendObjectDeSpawnAnim(go->GetGUID());
                go->SetPhaseMask(2, true);
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                    //go->SetPhaseMask(2, true);
                    go->Delete();
                    break;
                }
        }

    private:
        InstanceScript* instance{ go->GetInstanceScript() };
        EventMap events;

    };

    GameObjectAI* GetAI(GameObject* go) const
    {
        return new go_tot_tentacleAI(go);
    }
};

class npc_TOT_hallway_invader : public CreatureScript
{
public:
    npc_TOT_hallway_invader() : CreatureScript("npc_TOT_hallway_invader") { }

    struct npc_TOT_hallway_invaderAI : public ScriptedAI
    {
        npc_TOT_hallway_invaderAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(1, 2000);
            events.ScheduleEvent(2, 5000);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == EFFECT_MOTION_TYPE)
                if (id == 554 || id == 553)
                {
                    me->SetHomePosition(*me);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H_ALLOW_MOVEMENT);
                    //me->MonsterYell("ARRIVED", LANG_UNIVERSAL, 0);
                    me->SetDisableGravity(false);
                    me->SetCanFly(false);
                    me->SetHover(false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                    me->SetFacingTo(M_PI);
                }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(me, 76790);
                        events.ScheduleEvent(1, 15000);
                    }
                    break;
                case 2:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(76807);
                        events.ScheduleEvent(1, urand(5000, 8000));
                    }
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_TOT_hallway_invaderAI(creature);
    }
};

void AddSC_throne_of_the_tides()
{
    new go_defence_throne();
    new at_neptulon_pinacle();
    new at_abyssal_halls();

    // ST Scripts
    new at_abyssal_halls_x();
    new at_coral_garden();

    new npc_throne_of_tides_teleporter();
    new npc_lady_nazjar_intro();
    new npc_nazjar_spiritmenders();
    new npc_nazjar_tempest_witch();
    new npc_nazjar_invader();
    new npc_nazjar_intro_murloc();
    new npc_gilgoblin_hunter();
    new npc_gilgoblin_aquamage();
    new npc_faceless_watcher();
    new npc_unstable_corruption_trigger();
    new npc_unstable_corruption();
    new npc_tainted_sentry();

    new go_doodad_abyssal_maw_door_4();
    new go_tot_tentacle();

    new spell_bubble_shield();
    new spell_void_rip();
    new npc_ToT_WaveSpeaker();
    new npc_TOT_hallway_invader();
};