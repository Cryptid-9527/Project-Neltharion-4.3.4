/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "the_stonecore.h"

#define ENCOUNTERS     4

TeleportationData const teleportationData[] =
{
    {DATA_TELEPORT_CORBORUS, DATA_CORBORUS_EVENT             },
    {DATA_TELEPORT_SLABHIDE, DATA_SLABHIDE_EVENT             },
    {DATA_TELEPORT_OZRUK,    DATA_OZRUK_EVENT                },
    {0,                      0                               }, // END
};


Position stonecore_startingPos =          { 857.616882f,  993.374084f,    317.187714f,    6.119552f };
Position stonecore_midDungeonPos =        { 1309.294800f, 1231.148560f,   247.271759f,    5.679728f };

class instance_the_stonecore : public InstanceMapScript
{
    public:
        instance_the_stonecore() : InstanceMapScript("instance_the_stonecore", 725) { }

        struct instance_the_stonecore_InstanceMapScript : public InstanceScript
        {
            instance_the_stonecore_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(ENCOUNTERS);
                LoadTeleportationData(teleportationData);
                CorborusGUID = 0;
                SlabhideGUID = 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                 return true;
            }

            uint64 GetData64(uint32 type) const
            {
                switch (type)
                {
                    case DATA_CORBORUS:
                        return CorborusGUID;
                    case DATA_SLABHIDE:
                        return SlabhideGUID;
                    case DATA_ROCKDOOR:
                        return RockdoorGUID;
                    default:
                        break;
                }

                return 0;
            }
            void OnUnitDeath(Unit* unit)
            {
                InstanceScript::OnUnitDeath(unit);
                switch (unit->GetEntry())
                {
                case NPC_STONECORE_BRUISER:
                    unit->ToCreature()->SetHomePosition(*unit);
                    unit->StopMoving();
                    unit->GetMotionMaster()->Clear();
                    break;
                }
            }

            void OnCreatureCreate(Creature* creature)
            {
                InstanceScript::OnCreatureCreate(creature);

                switch (creature->GetEntry())
                {
                case NPC_MILLHOUSE_MANASTORM:
                    if (GetBossState(DATA_CORBORUS) == DONE || GetBossState(DATA_CORBORUS) == DONE_HM)
                        creature->DespawnOrUnsummon();
                    break;
                    case BOSS_CORBORUS:
                        CorborusGUID = creature->GetGUID();
                        break;
                    case BOSS_SLABHIDE:
                        SlabhideGUID = creature->GetGUID();
                        break;
                    case NPC_ROCK_BORER:
                    case NPC_THRASHING_CHARGE:
                    case NPC_CRYSTAL_SHARD:
                        if (auto corbus = instance->GetCreature(CorborusGUID))
                        {
                            if (GetBossState(DATA_CORBORUS) == IN_PROGRESS)
                                corbus->AI()->JustSummoned(creature);

                            if (corbus->isDead())
                            {
                                creature->DeleteThreatList();
                                creature->CombatStop();
                                creature->DespawnOrUnsummon(0);
                            }
                        }
                        break;
                    case NPC_STONECORE_TELEPORTER1:
                        AddTeleporter(creature->GetGUID());
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* gameobject)
            {
                switch (gameobject->GetEntry())
                {
                    case GO_ROCKDOOR_BREAK:
                        RockdoorGUID = gameobject->GetGUID();
                        if (GetBossState(DATA_CORBORUS) == DONE || GetBossState(DATA_CORBORUS) == DONE_HM)
                        {
                            gameobject->SetGoState(GO_STATE_ACTIVE);
                            gameobject->EnableCollision(false);
                        }

                        break;
                    case 204381:    //Slabhide walls
                        if (GetBossState(DATA_SLABHIDE) == DONE || GetBossState(DATA_SLABHIDE) == DONE_HM)
                        {
                            gameobject->SetGoState(GO_STATE_ACTIVE);
                            gameobject->EnableCollision(false);

                        }
                        break;
                    default:
                        break;
                }
            }

        protected:
            EventMap Events;
            uint64 CorborusGUID;
            uint64 SlabhideGUID;
            uint64 RockdoorGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_the_stonecore_InstanceMapScript(map);
        }
};

class npc_stonecore_sentry : public CreatureScript
{
    /*
        UPDATE `creature_template` SET `ScriptName`='npc_stonecore_sentry' WHERE  `entry`=42695;
    */
public:
    npc_stonecore_sentry() : CreatureScript("npc_stonecore_sentry") { }

    struct npc_stonecore_sentryAI : ScriptedAI
    {
        npc_stonecore_sentryAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        void InitializeAI()
        {
            yelled = false;
            yellTarget = nullptr;
            me->SetReactState(REACT_PASSIVE);
        }
        void Reset()
        {
            yelled = false;
            yellTarget = nullptr;
            me->SetReactState(REACT_PASSIVE);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!yelled)
                if (!yellTarget)
                        if (!me->isInCombat())
                           if (me->GetDistance(who) < 20.f)
                               if (who->ToPlayer())
                                       FleeForHelp(who);
        }
        void UpdateAI(const uint32 diff)
        {

            if (yelled)
            if (me->HasReactState(REACT_AGGRESSIVE))
                if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

        Position const GetFleeDestination()
        {
            switch (me->GetDBTableGUIDLow())
            {
            case 209239:
                return {1446.498779f, 1225.278809f, 231.881073f, 5.082878f };
                break;
            case 209240:
                return {1543.629639f, 1216.899292f, 223.423294f, 4.454550f };
                break;
            case 209241:
                return {1535.532349f, 1137.588623f, 216.337173f, 3.731962f };
                break;
            case 209242:
                return {1588.124512f, 1175.812012f, 218.552368f, 5.137821f };
                break;
            case 209243:
                return {1594.024170f, 1114.980225f, 216.204453f, 6.139184f };
                break;
            default:
                return (me->GetHomePosition());
                break;
            }
            return (me->GetHomePosition());
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (me->isAlive())
            if (type == POINT_MOTION_TYPE)
                if (id == 404)
                if (!yelled)
                    {
                        yelled = true;
                        me->CallForHelp(10.f);
                        Talk(1);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (UpdateVictim())
                        {
                            me->AI()->AttackStart(me->getVictim());
                            me->GetMotionMaster()->Clear();
                            me->StopMoving();
                            me->GetMotionMaster()->MoveChase(me->getVictim(), me->GetMeleeReEnagementDistance(me->getVictim()) - me->GetMeleeRange(me->getVictim()));
                        }
                    }
        }

        void FleeForHelp(Unit* who)
        {
            who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
            who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH_LEVEL);
            who->RemoveAurasByType(SPELL_AURA_MOD_INVISIBILITY);
            who->RemoveAurasByType(SPELL_AURA_MOD_CAMOUFLAGE);
            yellTarget = who;
            if (!me->isInCombat())
            {
                AttackStart(who);
                me->AddThreat(who, 10000.f);
            }
                me->SetReactState(REACT_PASSIVE);
                if (auto m = me->GetMotionMaster())
                {
                    m->Clear();
                    me->GetMotionMaster()->MovePoint(404, GetFleeDestination(), true, 8.f, false);
                }
        }
        void EnterCombat(Unit* who)
        {
            if (!yelled)
            {
                if (!yellTarget)
                {
                    FleeForHelp(who);
                }
            }
        }

    private:
        Unit* yellTarget;
        bool yelled = false;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stonecore_sentryAI(creature);
    }
};

class npc_stonecore_teleporter : public CreatureScript
{
public:
    npc_stonecore_teleporter() : CreatureScript("npc_stonecore_teleporter") { }

    struct npc_stonecore_teleporterAI : public PassiveAI
    {
        npc_stonecore_teleporterAI(Creature* creature) : PassiveAI(creature)
        {
            me->SetSpeed(MOVE_FLIGHT, 0.35f);
            instance = me->GetInstanceScript();
        }

        void InitializeAI()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_PREVENT_SPELL_CLICK);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (instance->GetBossState(DATA_SLABHIDE) == DONE || instance->GetBossState(DATA_SLABHIDE) == DONE_HM)
            {
                std::list<GameObject*> objs;
                GetGameObjectListWithEntryInGrid(objs, me, 204381, 200.f);
                if (objs.size())
                    for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                        if (auto rock = (*itr))
                            if (rock->GetGoState() != GO_STATE_ACTIVE)
                                rock->SetGoState(GO_STATE_ACTIVE);
            }
        }


        void OnSpellClick(Unit* passenger, bool& result)
        {
            if (passenger->isInCombat())
            {
                if (auto passPlayer = passenger->ToPlayer())
                    if (auto s = passPlayer->GetSession())
                        s->SendNotification(s->GetTrinityString(LANG_YOU_IN_COMBAT));
                return;
            }

            if (instance)
                if (me->GetMapId() == 725/*The Stonecore*/)
                    if (instance->GetDeadBossTotal(true) >= 2)
                    {
                        if (passenger->GetDistance(stonecore_startingPos) < passenger->GetDistance(stonecore_midDungeonPos))//player is clicking the starting portal
                            passenger->NearTeleportTo(stonecore_midDungeonPos);
                        else
                            passenger->NearTeleportTo(stonecore_startingPos);
                    }
        };
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stonecore_teleporterAI(creature);
    }
};

void AddSC_instance_the_stonecore()
{
    new instance_the_stonecore();
    new npc_stonecore_sentry();
    new npc_stonecore_teleporter();
}
