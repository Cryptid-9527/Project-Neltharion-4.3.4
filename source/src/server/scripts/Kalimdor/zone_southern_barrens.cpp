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

 /*
 SDName: Southern Barrens
 SD%Complete: 100 (with SAI scripts)
 SDComment: Wondi's scripts!
*/

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
#include "Group.h"

enum SouthernBarrensDefaultSpells
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_PING_2 = 98915,
    SPELL_DUMMY_PING_3 = 98916,
    SPELL_DUMMY_PING_4 = 98917,
    SPELL_DUMMY_PING_5 = 98918,
    SPELL_SANCTUARY_NO_COMBAT = 88467,
    SPELL_ROOT = 42716,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_FLIGHT_SPEED_280 = 50345,
    SPELL_FLIGHT_SPEED_200 = 55034,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum SouthernBarrensEvents
{
    EVENT_SABERNOUT_ATTACK_1 = 1,
    EVENT_SABERNOUT_ATTACK_2,
    EVENT_SABERNOUT_1,
    EVENT_SABERNOUT_2,
    EVENT_SABERNOUT_3,
    EVENT_SABERNOUT_4,
    EVENT_SABERNOUT_5,
    EVENT_ONLOOKER_FACING,
    EVENT_NARALEX_1,
    EVENT_NARALEX_2,
    EVENT_NARALEX_3,
    EVENT_NARALEX_4,
    EVENT_NARALEX_5,
    EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_1,
    EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_2,
    EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_3,
    EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_4,
    EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_5,
    EVENT_HILL_ALL_DESPAWNER,
    EVENT_HILL_HORDE_SIDE_AS_HORDE_1,
    EVENT_HILL_HORDE_SIDE_AS_HORDE_2,
    EVENT_HILL_HORDE_SIDE_AS_HORDE_3,
    EVENT_HILL_HORDE_SIDE_AS_HORDE_4,
    EVENT_HILL_HORDE_SIDE_AS_HORDE_5,
    EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_1,
    EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_2,
    EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_3,
    EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_4,
    EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_5,
    EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_1,
    EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_2,
    EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_3,
    EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_4,
    EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_5,
    EVENT_HILL_WOLF_RIDING_1,
    EVENT_HILL_WOLF_RIDING_2,
    EVENT_CARRY_BUNNY_1,
    EVENT_BUTCHER_TAURAJO_1,
    EVENT_GYRO_1,
    EVENT_GRYPHON_1,
    EVENT_GRYPHON_2,
    EVENT_GAINES_1,
    EVENT_THREETOOTH_1,
    EVENT_MARLEY_FLIGHT_1,
    EVENT_MARLEY_FLIGHT_2,
    EVENT_SABOTAGE_CAMERA_1,
    EVENT_SABOTAGE_CAMERA_2,
};

// ############# SCRIPTS ###############

enum CoiledRope
{
    NPC_COILED_ROPE_CONTROLLER = 74909,
    SPELL_ROPE = 75452,
    SPELL_PACIFY_3SEC = 97671,
};

const Position coiledRopePos[] =
{
    {  -614.41f, -1415.900f, 156.5634f, 2.69f }, // Top
};

class go_coiled_rope_top_down : public GameObjectScript
{
public:
    go_coiled_rope_top_down() : GameObjectScript("go_coiled_rope_top_down") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (auto controller = player->SummonCreature(NPC_COILED_ROPE_CONTROLLER, coiledRopePos[0], TEMPSUMMON_TIMED_DESPAWN, 5000))
            controller->SetOwnerGUID(player->GetGUID());

        return true;
    }
};

class npc_coiled_rope_controller : public CreatureScript
{
public:
    npc_coiled_rope_controller() : CreatureScript("npc_coiled_rope_controller") { }

    struct npc_coiled_rope_controllerAI : public ScriptedAI
    {
        npc_coiled_rope_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                _playerGUID = player->GetGUID();
                me->CastSpell(me, SPELL_ROPE);
                player->CastSpell(player, SPELL_PACIFY_3SEC);

                if (player->GetPositionZ() > 130.f)
                {
                    _playerOnTop = true;
                    player->JumpWithDelay(500, -589.1488f, -1412.901f, 130.83f, 25.f, 20.f, 0);
                    player->JumpWithDelay(1900, -561.4901f, -1411.315f, 92.646f, 25.f, 20.f, 1);
                }
                else
                {
                    _playerOnTop = false;
                    player->JumpWithDelay(500, -589.1488f, -1412.901f, 130.83f, 25.f, 20.f, 2);
                    player->JumpWithDelay(2300, -615.62f, -1411.41f, 157.5776f, 25.f, 20.f, 3);
                }
            }
        }

    private:
        bool _playerOnTop = false;
        uint64 _playerGUID = 0;
        uint32 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coiled_rope_controllerAI(creature);
    }
};

enum GraspingOvergrowth
{
    QUEST_WHEN_PLANTS_ATTACK = 24539,
    NPC_TAUREN_RESCUED = 37487,
};

class npc_grasping_overgrowth : public CreatureScript
{
public:
    npc_grasping_overgrowth() : CreatureScript("npc_grasping_overgrowth") { }

    struct npc_grasping_overgrowthAI : public ScriptedAI
    {
        npc_grasping_overgrowthAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->CastSpell(me, SPELL_ROOT);

            if (auto tauren = me->SummonCreature(NPC_TAUREN_RESCUED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 4, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                _taurenGUID = tauren->GetGUID();
                tauren->setActive(true);
                tauren->CastWithDelay(500, me, SPELL_RIDE_VEHICLE);
            }
        }

        void JustDied(Unit* /* killer */)
        {
            if (auto tauren = Unit::GetCreature(*me, _taurenGUID))
            {
                tauren->ExitVehicle();
                tauren->GetMotionMaster()->MoveJump(me->GetPositionX() + 4, me->GetPositionY(), me->GetPositionZ(), 15.f, 18.f);
                me->CastSpell(tauren, SPELL_DUMMY_PING);
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            DoMeleeAttackIfReady();
        }
    private:
        Position _myPos;
        uint64 _taurenGUID = 0;
        uint32 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grasping_overgrowthAI(creature);
    }
};

class go_bristleback_cage : public GameObjectScript
{
public:
    go_bristleback_cage() : GameObjectScript("go_bristleback_cage") { }

    bool getCreatureInside = false;
    uint64 taurenGUID = 0;

    void OnLootStateChanged(GameObject* go, uint32 state, Unit* /*unit*/)
    {
        if (!getCreatureInside)
        {
            if (state == GO_ACTIVATED)
            {
                if (auto tauren = go->FindNearestCreature(NPC_TAUREN_RESCUED, 10.f))
                {
                    go->setActive(true);
                    tauren->setActive(true);
                    getCreatureInside = true;
                    taurenGUID = tauren->GetGUID();
                }
            }
        }

        if (state == GO_READY)
        {
            if (auto tauren = Unit::GetCreature(*go, taurenGUID))
            {
                if (tauren->isDead())
                    tauren->Respawn(true);
            }
        }
    }
};

enum Sabersnout
{
    QUEST_SPEAKING_THEIR_LANGUAGE = 24534,
    NPC_BRISTLEBACK_ONLOOKER = 37529,
    NPC_WONDI_BUNNY_TARGET = 75042,
    NPC_ASSAULT_BOAR = 37536,

    SPELL_FEAR_1 = 70171,
    SPELL_IMPENETRABLE_SHIELD_WALL = 70166,
    SPELL_ENRAGE = 8599,
    SPELL_STRIKE = 11976,
    SPELL_QUILBOAR_RUSH = 82610,
    SPELL_BOARPOCALYPSE = 70165,
};

const Position onlookerPos[] =
{
    { -1825.81f, -1812.02f, 92.4542f, 3.6651f },
    { -1821.29f, -1809.61f, 92.8964f, 3.6128f },
    { -1823.56f, -1815.13f, 92.5176f, 3.7175f },
    { -1819.05f, -1844.84f, 92.8155f, 2.4434f },
    { -1829.41f, -1813.50f, 92.0942f, 4.3458f },
    { -1823.88f, -1805.65f, 92.6182f, 3.7001f },
    { -1828.67f, -1842.20f, 92.5514f, 1.5882f },
    { -1821.40f, -1837.85f, 91.7615f, 3.0194f },
    { -1816.97f, -1838.24f, 91.9817f, 3.2986f },
    { -1828.09f, -1807.47f, 92.2682f, 3.6651f },
    { -1819.02f, -1841.57f, 92.5813f, 2.4783f },
    { -1823.94f, -1840.92f, 92.2301f, 2.5656f },
};

class npc_sabersnout : public CreatureScript
{
public:
    npc_sabersnout() : CreatureScript("npc_sabersnout") { }

    struct npc_sabersnoutAI : public ScriptedAI
    {
        npc_sabersnoutAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                TalkWithDelay(500, 0, _playerGUID);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, -1850.59f, -1830.521f, 90.09f);
                _events.ScheduleEvent(EVENT_SABERNOUT_1, 1000);
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (!_killable)
            {
                if (damage >= me->GetHealth() / 4)
                    damage = 50.f;
            }

            if ((me->GetHealthPct() <= 25) && !_unableToKill)
            {
                _killable = false;
                _unableToMelee = true;
                _events.CancelEventGroup(0);
                _unableToKill = true;
                me->CastSpell(me, SPELL_IMPENETRABLE_SHIELD_WALL);
                me->CastSpell(me, SPELL_ROOT);
                me->setRegeneratingHealth(false);
                TalkWithDelay(1000, 3, _playerGUID);
                _events.ScheduleEvent(EVENT_SABERNOUT_3, 4000);
            }

            if ((me->GetHealthPct() <= 10) && !me->GetAura(SPELL_ENRAGE))
            {
                me->CastSpell(me, SPELL_ENRAGE);
                Talk(9, _playerGUID);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_ASSAULT_BOAR)
                _summons.Summon(summoned);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() == NPC_ASSAULT_BOAR)
            {
                _killCount++;

                if (_killCount == 8)
                    _events.ScheduleEvent(EVENT_SABERNOUT_4, 1000);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            Talk(8, _playerGUID);
            me->CastSpell(me, SPELL_FEAR_1);

            auto player = me->GetPlayersInRange(40, true);
            for (auto it = player.begin(); it != player.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_SPEAKING_THEIR_LANGUAGE) == QUEST_STATUS_INCOMPLETE)
                    (*it)->KilledMonsterCredit(me->GetEntry()); // credit
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (!_eventStarted)
            {
                Talk(2, _playerGUID);

                auto enemy = me->SelectNearbyUnits(NPC_BRISTLEBACK_ONLOOKER, 30.0f);
                for (auto itr = enemy.begin(); itr != enemy.end(); ++itr)
                    if (auto enemy = (*itr))
                        enemy->HandleEmoteCommand(EMOTE_ONESHOT_BATTLE_ROAR);

                _eventStarted = true;
            }

            if (!_unableToMelee)
            {
                _events.ScheduleEvent(EVENT_SABERNOUT_ATTACK_1, urand(3000, 8000), 0);
                _events.ScheduleEvent(EVENT_SABERNOUT_ATTACK_2, urand(9000, 22000), 0);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (!player->isAlive())
                {
                    _summons.DespawnAll();
                    me->DespawnOrUnsummon();
                }
            }

            if (!_unableToMelee)
                DoMeleeAttackIfReady();

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SABERNOUT_ATTACK_1:
                    if (auto victim = me->getVictim())
                        me->CastSpell(victim, SPELL_STRIKE);

                    _events.ScheduleEvent(EVENT_SABERNOUT_ATTACK_1, urand(6000, 12000));
                    break;
                case EVENT_SABERNOUT_ATTACK_2:
                    me->CastSpell(me->getVictim(), SPELL_STRIKE);
                    break;
                case EVENT_SABERNOUT_1:
                {
                    uint8 maxOnlooker = 12;
                    uint64 onlooker_first_pathID = 600010;

                    for (uint8 i = 0; i < maxOnlooker; i++)
                    {
                        if (auto onlooker = me->SummonCreature(NPC_BRISTLEBACK_ONLOOKER, onlookerPos[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            _summons.Summon(onlooker);
                            onlooker->SetWalk(false);
                            onlooker->GetMotionMaster()->MovePath(onlooker_first_pathID + i, false);
                        }
                    }

                    TalkWithDelay(6000, 1, _playerGUID);
                    _events.ScheduleEvent(EVENT_SABERNOUT_2, 10000);
                }
                break;
                case EVENT_SABERNOUT_2:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        AttackStart(player);
                    break;
                case EVENT_SABERNOUT_3:
                    Talk(4, _playerGUID);

                    me->CastWithDelay(2000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(3000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(4000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(5000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(6000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(7000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(7000, me, SPELL_BOARPOCALYPSE);
                    me->CastWithDelay(7000, me, SPELL_BOARPOCALYPSE);
                    break;
                case EVENT_SABERNOUT_4:
                    Talk(6, _playerGUID);
                    _events.ScheduleEvent(EVENT_SABERNOUT_5, 4000);
                    break;
                case EVENT_SABERNOUT_5:
                    me->RemoveAura(SPELL_IMPENETRABLE_SHIELD_WALL);
                    Talk(7, _playerGUID);
                    me->CastWithDelay(500, me->getVictim(), SPELL_QUILBOAR_RUSH);
                    _unableToMelee = false;
                    _killable = true;
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _killable = false;
        bool _unableToMelee = false;
        bool _eventStarted = false;
        bool _unableToKill = false;
        uint32 _checkTimer = 3000;
        uint8 _killCount = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sabersnoutAI(creature);
    }
};

class npc_bristleback_onlooker : public CreatureScript
{
public:
    npc_bristleback_onlooker() : CreatureScript("npc_bristleback_onlooker") { }

    struct npc_bristleback_onlookerAI : public ScriptedAI
    {
        npc_bristleback_onlookerAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_FEAR_1)
            {
                doEmotes = false;
                me->DespawnOrUnsummon(7000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != WAYPOINT_MOTION_TYPE)
                return;

            if (point == 3)
            {
                doEmotes = true;
                if (auto target = me->FindNearestCreature(NPC_WONDI_BUNNY_TARGET, 25.f))
                    me->SetFacingToObject(target);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (doEmotes)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = urand(6000, 12000);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BATTLE_ROAR);
                }
                else
                    _checkTimer -= diff;
            }
        }

    private:
        bool doEmotes = false;
        uint16 _checkTimer = 6000;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bristleback_onlookerAI(creature);
    }
};

enum TheNightmareScar
{
    QUEST_THE_NIGHTMARE_SCAR = 24601,

    NPC_BEAM_TARGET = 74908,
    NPC_NIGHTMARE_HORROR = 37791,
    NPC_NIGHTMARE_STALKER = 37736,
    NPC_NIGHTMARE_MASS = 37753,

    SPELL_BLACK_HOLE = 67549, // need sniff the correct id of spell
    SPELL_BLACK_HOLE_2 = 64135, // additional visual effect
    SPELL_GREEN_CHANNEL = 83481, // need sniff the correct id of spell
    SPELL_SHADOW_EXPLOSION = 78950,
};

const Position nightmareSpawns[] =
{
    { -1718.220f, -2217.9562f, 98.906f },
    { -1720.741f, -2209.7021f, 100.02f },
    { -1715.529f, -2209.8029f, 99.945f },
};

#define GOSSIP_ITEM_NARALEX_1  "I'm ready, Naralex."

class npc_naralex_scar : public CreatureScript
{
public:
    npc_naralex_scar() : CreatureScript("npc_naralex_scar") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        player->PlayerTalkClass->ClearMenus();

        if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(creature, SPELL_DUMMY_PING);
            CAST_AI(ScriptedAI, (creature->AI()))->DoAction(EVENT_NARALEX_1);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_THE_NIGHTMARE_SCAR) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NARALEX_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        else if (creature->isQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_naralex_scarAI : public ScriptedAI
    {
        npc_naralex_scarAI(Creature* creature) : ScriptedAI(creature), _summons(me) { }

        void Reset()
        {
            _killCount = 0;
            _playerGUID = 0;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

            if (auto target = Unit::GetCreature(*me, _targetGUID))
            {
                target->RemoveAura(SPELL_BLACK_HOLE);
                target->RemoveAura(SPELL_BLACK_HOLE_2);
                target->RemoveAura(SPELL_GREEN_CHANNEL);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    Talk(0, _playerGUID);
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            _summons.Summon(summoned);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            _killCount++;

            if (_killCount == 3)
                _events.ScheduleEvent(EVENT_NARALEX_4, 4000);
            else if (_killCount == 5)
                _events.ScheduleEvent(EVENT_NARALEX_5, 4500);
            else if (_killCount == 6)
            {
                if (auto target = Unit::GetCreature(*me, _targetGUID))
                    target->CastSpell(target, SPELL_SHADOW_EXPLOSION);

                Reset();

                auto player = me->GetPlayersInRange(30, true);
                for (auto it = player.begin(); it != player.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_THE_NIGHTMARE_SCAR) == QUEST_STATUS_INCOMPLETE)
                        (*it)->KilledMonsterCredit(NPC_NIGHTMARE_MASS); // credit
            }
        }

        void DoAction(const int32 actionid)
        {
            if (actionid == EVENT_NARALEX_1)
            {
                me->SetReactState(REACT_PASSIVE);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                _events.ScheduleEvent(EVENT_NARALEX_2, 5000);

                if (_targetGUID == 0)
                {
                    if (auto target = me->FindNearestCreature(NPC_BEAM_TARGET, 20.f))
                    {
                        _targetGUID = target->GetGUID();
                        target->CastSpell(target, SPELL_BLACK_HOLE);
                        target->CastSpell(target, SPELL_BLACK_HOLE_2);
                    }
                }
            }
        }

        void UpdateAI(const uint32 uiDiff)
        {
            _events.Update(uiDiff);

            if (_checkTimer <= uiDiff)
            {
                _checkTimer = 3000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive() || me->GetDistance(player) >= 50.f)
                    {
                        _summons.DespawnAll();
                        Reset();
                    }
                }
            }
            else
                _checkTimer -= uiDiff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_NARALEX_2:
                    if (auto target = Unit::GetCreature(*me, _targetGUID))
                        me->CastSpell(target, SPELL_GREEN_CHANNEL);

                    _events.ScheduleEvent(EVENT_NARALEX_3, 3000);
                    break;
                case EVENT_NARALEX_3:
                    if (auto horror1 = me->SummonCreature(NPC_NIGHTMARE_HORROR, nightmareSpawns[0], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto horror2 = me->SummonCreature(NPC_NIGHTMARE_HORROR, nightmareSpawns[1], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto horror3 = me->SummonCreature(NPC_NIGHTMARE_HORROR, nightmareSpawns[2], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                                {
                                    horror1->AI()->AttackStart(player);
                                    horror2->AI()->AttackStart(player);
                                    horror3->AI()->AttackStart(player);
                                }
                            }
                    break;
                case EVENT_NARALEX_4:
                    if (auto stalker1 = me->SummonCreature(NPC_NIGHTMARE_STALKER, nightmareSpawns[0], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stalker2 = me->SummonCreature(NPC_NIGHTMARE_STALKER, nightmareSpawns[2], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                            {
                                stalker1->AI()->AttackStart(player);
                                stalker2->AI()->AttackStart(player);
                            }
                        }
                    break;
                case EVENT_NARALEX_5:
                    if (auto mass = me->SummonCreature(NPC_NIGHTMARE_MASS, nightmareSpawns[1], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                            mass->AI()->AttackStart(player);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 6000;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _targetGUID = 0;
        uint8 _killCount = 0;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_naralex_scarAI(creature);
    }
};

enum ALineInTheDirt
{
    QUEST_A_LINE_IN_THE_DIRT_A = 25045,
    QUEST_A_LINE_IN_THE_DIRT_H = 24546,

    NPC_HORDE_FIELD_DEFENDER = 37659,
    NPC_ALLIANCE_FIELD_DEFENDER = 39040,
    NPC_DESOLATION_GRUNT = 37706,
    NPC_NORTHWATCH_RECON = 37585,

    NPC_KILLCREDIT_HORDE_1 = 37667,
    NPC_KILLCREDIT_HORDE_2 = 37668,
    NPC_KILLCREDIT_ALLIANCE_1 = 39029,
    NPC_KILLCREDIT_ALLIANCE_2 = 39030,

    SPELL_RECAPTURING_DUMMY = 70312,

    OBJECT_BARRICADE_HORDE = 201748,
    OBJECT_BARRICADE_ALLIANCE = 202462,

    BANNER_NEUTRAL_HORDE_SIDE = 201781,
    BANNER_NEUTRAL_ALLIANCE_SIDE = 201743,

    BANNER_HORDE_FIELD = 201744,
    BANNER_ALLIANCE_FIELD = 202460,

    // horde side bunny guid 39759
    // allly side bunny guid 39761
};

const Position LineInTheDirtHorde[] =
{
    { -2128.82f, -2078.96f, 99.28f, 1.031f }, // defenders place 1         // 0 
    { -2131.76f, -2079.77f, 99.35f, 2.648f }, // defenders place 1         // 1 
    { -2130.61f, -2082.43f, 99.25f, 4.186f }, // defenders place 1         // 2 
    { -2127.73f, -2082.02f, 99.03f, 5.723f }, // defenders place 1         // 3 
    { -2143.89f, -2074.03f, 98.633f, 5.87f }, // barricades place 1        // 4 
    { -2122.32f, -2067.95f, 98.462f, 4.13f }, // barricades place 1        // 5 
    { -2117.21f, -2088.52f, 97.404f, 2.56f }, // barricades place 1        // 6 
    { -2136.52f, -2092.96f, 98.858f, 1.07f }, // barricades place 1        // 7 
    { -2155.948f, -2031.558f, 91.702f },                                   // 8 
    { -2122.440f, -2122.546f, 92.523f },                                   // 9 
    { -2105.978f, -2039.745f, 93.917f },                                   // 10
    { -2158.030f, -2122.667f, 92.043f },                                   // 11
    { -2091.988f, -2060.999f, 92.094f },                                   // 12
    { -2091.694f, -2092.868f, 91.683f },                                   // 13
    { -2129.608f, -2030.341f, 93.687f },                                   // 14
    { -2168.883f, -2093.730f, 92.886f },                                   // 15
    { -2463.808f, -2304.558f, 100.214f, 1.764f }, // defenders place 2     // 16
    { -2465.194f, -2306.812f, 100.263f, 3.343f }, // defenders place 2     // 17
    { -2462.925f, -2308.535f, 100.375f, 4.886f }, // defenders place 2     // 18
    { -2461.958f, -2306.124f, 100.310f, 0.229f }, // defenders place 2     // 19
    { -2461.10f, -2320.37f, 99.850f, 1.707f }, // barricades place 2        // 20 
    { -2477.59f, -2309.04f, 99.391f, 0.135f }, // barricades place 2        // 21 
    { -2466.51f, -2292.00f, 98.851f, 4.985f }, // barricades place 2        // 22 
    { -2449.92f, -2303.22f, 100.08f, 3.402f }, // barricades place 2        // 23 
    { -2505.875f, -2299.051f, 94.78f },                                    // 24
    { -2495.299f, -2337.622f, 92.15f },                                    // 25
    { -2456.529f, -2350.426f, 93.20f },                                    // 26
    { -2425.230f, -2341.924f, 93.07f },                                    // 27
    { -2426.532f, -2312.66f, 95.83f, },                                    // 28
    { -2420.561f, -2287.840f, 92.59f },                                    // 29
    { -2463.982f, -2273.131f, 94.37f },                                    // 30
};

class npc_a_line_in_the_dirt_controller : public CreatureScript
{
public:
    npc_a_line_in_the_dirt_controller() : CreatureScript("npc_a_line_in_the_dirt_controller") { }

    struct npc_a_line_in_the_dirt_controllerAI : public ScriptedAI
    {
        npc_a_line_in_the_dirt_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->setActive(true);
            _events.CancelEventGroup(1);
            _events.Reset();
            _playerGUID = 0;
            _defenderDieCount = 0;
            _killCount = 0;

            if (auto neutralflag = GameObject::GetGameObject(*me, _neturalFlagGUID))
                neutralflag->SetPhaseMask(1, true);

            if (auto allyflag = me->FindNearestGameObject(BANNER_ALLIANCE_FIELD, 25.f))
                allyflag->RemoveFromWorld();

            if (auto hordeflag = me->FindNearestGameObject(BANNER_HORDE_FIELD, 25.f))
                hordeflag->RemoveFromWorld();

            std::list<WorldObject*> targetList;
            Trinity::AllWorldObjectsInRange objects(me, 25.f);
            Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(me, targetList, objects, GRID_MAP_TYPE_MASK_GAMEOBJECT);
            me->VisitNearbyObject(25.f, searcher);
            targetList.remove_if([](WorldObject* target) {
                return target->GetEntry() != OBJECT_BARRICADE_HORDE && target->GetEntry() != OBJECT_BARRICADE_ALLIANCE;
                });

            for (WorldObject* target : targetList)
                if (target->ToGameObject())
                    target->RemoveFromWorld();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING) // ping from player
            {
                if (auto player = who->ToPlayer())
                {
                    if (auto neutralflag = me->FindNearestGameObject(BANNER_NEUTRAL_HORDE_SIDE, 10.f))  // hill next to HORDE camp!
                    {
                        _neturalFlagGUID = neutralflag->GetGUID();
                        neutralflag->SetPhaseMask(32768, true);
                        _hordeField = true;

                        if (player->GetTeam() == ALLIANCE)
                        {
                            player->KilledMonsterCredit(NPC_KILLCREDIT_ALLIANCE_1);
                            _isHorde = false;
                            _playerGUID = player->GetGUID();
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_1, 1000, 1);
                        }
                        else // Player is horde
                        {
                            player->KilledMonsterCredit(NPC_KILLCREDIT_HORDE_1);
                            _isHorde = true;
                            _playerGUID = player->GetGUID();
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_HORDE_1, 1000, 1);
                        }
                    }

                    else if (auto neutralflag = me->FindNearestGameObject(BANNER_NEUTRAL_ALLIANCE_SIDE, 10.f)) // hill next to ALLIANCE camp!
                    {
                        _neturalFlagGUID = neutralflag->GetGUID();
                        neutralflag->SetPhaseMask(32768, true);
                        _hordeField = false;

                        if (player->GetTeam() == ALLIANCE)
                        {
                            player->KilledMonsterCredit(NPC_KILLCREDIT_ALLIANCE_1);
                            _isHorde = false;
                            _playerGUID = player->GetGUID();
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_1, 1000, 1);
                        }
                        else // Player is horde
                        {
                            player->KilledMonsterCredit(NPC_KILLCREDIT_HORDE_1);
                            _isHorde = true;
                            _playerGUID = player->GetGUID();
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_1, 1000, 1);
                        }
                    }
                }
            }

            if (spellInfo->Id == SPELL_RECAPTURING_DUMMY) // ping from grunt/recon
            {
                Talk(2, _playerGUID);
                _events.ScheduleEvent(EVENT_HILL_ALL_DESPAWNER, 3000, 1);

                if (!_isHorde)
                {
                    if (auto allyflag = me->FindNearestGameObject(BANNER_ALLIANCE_FIELD, 25.f))
                        allyflag->SetGoState(GO_STATE_ACTIVE);
                }
                else
                {
                    if (auto hordeflag = me->FindNearestGameObject(BANNER_HORDE_FIELD, 25.f))
                        hordeflag->SetGoState(GO_STATE_ACTIVE);
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            _summons.Summon(summoned);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() == NPC_ALLIANCE_FIELD_DEFENDER || summon->GetEntry() == NPC_HORDE_FIELD_DEFENDER)
            {
                _defenderDieCount++;

                if (_defenderDieCount == 4)
                {
                    _events.CancelEventGroup(1);
                    Talk(2, _playerGUID);
                    _summons.DespawnAll();
                    Reset();
                }
            }

            if (summon->GetEntry() == NPC_DESOLATION_GRUNT || summon->GetEntry() == NPC_NORTHWATCH_RECON)
            {
                _killCount++;
                TC_LOG_ERROR("sql.sql", "test successful %u", _killCount);

                if (_hordeField)
                {
                    if (_killCount == 3)
                    {
                        if (!_isHorde)
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_4, 5000, 1);
                        else
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_HORDE_4, 5000, 1);
                    }
                    else if (_killCount == 7)
                    {
                        if (!_isHorde)
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_5, 6000, 1);
                        else
                            _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_HORDE_5, 6000, 1);
                    }
                }
                else
                {
                    if (_killCount == 3)
                    {
                        if (!_isHorde)
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_4, 5000, 1);
                        else
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_4, 5000, 1);
                    }
                    else if (_killCount == 7)
                    {
                        if (!_isHorde)
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_5, 6000, 1);
                        else
                            _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_5, 6000, 1);
                    }
                }

                if (_killCount == 13)
                {
                    _events.ScheduleEvent(EVENT_HILL_ALL_DESPAWNER, 6000, 1);
                    Talk(1, _playerGUID);

                    if (!_isHorde)
                    {
                        auto player = me->GetPlayersInRange(30, true);
                        for (auto it = player.begin(); it != player.end(); it++)
                            (*it)->KilledMonsterCredit(NPC_KILLCREDIT_ALLIANCE_2);
                    }
                    else
                    {
                        auto player = me->GetPlayersInRange(30, true);
                        for (auto it : player)
                            it->KilledMonsterCredit(NPC_KILLCREDIT_HORDE_2);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (!player->isAlive())
                {
                    Talk(3, _playerGUID);
                    _summons.DespawnAll();
                    Reset();
                }

                if (me->GetDistance(player) >= 50.f)
                {
                    Talk(3, _playerGUID);
                    _summons.DespawnAll();
                    Reset();
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_1:
                    Talk(0, _playerGUID);

                    if (auto neutralflag = GameObject::GetGameObject(*me, _neturalFlagGUID))
                        me->SummonGameObject(BANNER_ALLIANCE_FIELD, neutralflag->GetPositionX(), neutralflag->GetPositionY(), neutralflag->GetPositionZ(), 1.0472f, 0.f, 0.f, 0.f, 0.f, 0);

                    for (uint8 i = 0; i < 4; i++)
                    {
                        if (auto allydefender = me->SummonCreature(NPC_ALLIANCE_FIELD_DEFENDER, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            allydefender->SetArmor(3000);
                            allydefender->SetReactState(REACT_PASSIVE);
                            allydefender->setActive(true);
                            allydefender->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                        }
                    }

                    _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_2, 9000, 0);
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_2:
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2143.89f, -2074.03f, 98.633f, 5.87f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2122.32f, -2067.95f, 98.462f, 4.13f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2117.21f, -2088.52f, 97.404f, 2.56f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2136.52f, -2092.96f, 98.858f, 1.07f, 0.f, 0.f, 0.f, 0.f, 0);
                    _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_3, 5000, 0);
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_3:
                    for (uint8 i = 8; i < 12; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_4:
                    for (uint8 i = 12; i < 16; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_ALLIANCE_5:
                    for (uint8 i = 8; i < 15; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                case EVENT_HILL_ALL_DESPAWNER:
                    _summons.DespawnAll();
                    Reset();
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_HORDE_1:
                    Talk(0, _playerGUID);

                    if (auto neutralflag = GameObject::GetGameObject(*me, _neturalFlagGUID))
                        me->SummonGameObject(BANNER_HORDE_FIELD, neutralflag->GetPositionX(), neutralflag->GetPositionY(), neutralflag->GetPositionZ(), 1.0472f, 0.f, 0.f, 0.f, 0.f, 0);

                    for (uint8 i = 0; i < 4; i++)
                    {
                        if (auto hordedefender = me->SummonCreature(NPC_HORDE_FIELD_DEFENDER, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            hordedefender->SetArmor(3000);
                            hordedefender->SetReactState(REACT_PASSIVE);
                            hordedefender->setActive(true);
                            hordedefender->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                        }
                    }

                    _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_HORDE_2, 9000, 0);
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_HORDE_2:
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2143.89f, -2074.03f, 98.633f, 5.87f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2122.32f, -2067.95f, 98.462f, 4.13f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2117.21f, -2088.52f, 97.404f, 2.56f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2136.52f, -2092.96f, 98.858f, 1.07f, 0.f, 0.f, 0.f, 0.f, 0);
                    _events.ScheduleEvent(EVENT_HILL_HORDE_SIDE_AS_HORDE_3, 5000, 0);
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_HORDE_3:
                    for (uint8 i = 8; i < 12; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_HORDE_4:
                    for (uint8 i = 12; i < 16; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                case EVENT_HILL_HORDE_SIDE_AS_HORDE_5:
                    for (uint8 i = 8; i < 15; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2127.84f, -2078.52f, 99.15f, 4.083f);
                        }
                    }
                    break;
                    // ########## ALLIANCE FIELD ###########
                case EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_1:
                    Talk(0, _playerGUID);

                    if (auto neutralflag = GameObject::GetGameObject(*me, _neturalFlagGUID))
                        me->SummonGameObject(BANNER_ALLIANCE_FIELD, neutralflag->GetPositionX(), neutralflag->GetPositionY(), neutralflag->GetPositionZ(), 0.2f, 0.f, 0.f, 0.f, 0.f, 0);

                    for (uint8 i = 16; i < 20; i++)
                    {
                        if (auto allydefender = me->SummonCreature(NPC_ALLIANCE_FIELD_DEFENDER, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            allydefender->SetArmor(3000);
                            allydefender->SetReactState(REACT_PASSIVE);
                            allydefender->setActive(true);
                            allydefender->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                        }
                    }

                    _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_2, 9000, 0);
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_2:
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2461.10f, -2320.37f, 99.850f, 1.707f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2477.59f, -2309.04f, 99.391f, 0.135f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2466.51f, -2292.00f, 98.851f, 4.985f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_ALLIANCE, -2449.92f, -2303.22f, 100.08f, 3.402f, 0.f, 0.f, 0.f, 0.f, 0);
                    _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_3, 5000, 0);
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_3:
                    for (uint8 i = 24; i < 27; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_4:
                    for (uint8 i = 27; i < 31; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_ALLIANCE_5:
                    for (uint8 i = 24; i < 31; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_DESOLATION_GRUNT, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;

                case EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_1:
                    Talk(0, _playerGUID);

                    if (auto neutralflag = GameObject::GetGameObject(*me, _neturalFlagGUID))
                        me->SummonGameObject(BANNER_HORDE_FIELD, neutralflag->GetPositionX(), neutralflag->GetPositionY(), neutralflag->GetPositionZ(), 0.2f, 0.f, 0.f, 0.f, 0.f, 0);

                    for (uint8 i = 16; i < 20; i++)
                    {
                        if (auto allydefender = me->SummonCreature(NPC_HORDE_FIELD_DEFENDER, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            allydefender->SetArmor(3000);
                            allydefender->SetReactState(REACT_PASSIVE);
                            allydefender->setActive(true);
                            allydefender->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                        }
                    }

                    _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_2, 9000, 0);
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_2:
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2461.10f, -2320.37f, 99.850f, 1.707f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2477.59f, -2309.04f, 99.391f, 0.135f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2466.51f, -2292.00f, 98.851f, 4.985f, 0.f, 0.f, 0.f, 0.f, 0);
                    me->SummonGameObject(OBJECT_BARRICADE_HORDE, -2449.92f, -2303.22f, 100.08f, 3.402f, 0.f, 0.f, 0.f, 0.f, 0);
                    _events.ScheduleEvent(EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_3, 5000, 0);
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_3:
                    for (uint8 i = 24; i < 27; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_4:
                    for (uint8 i = 27; i < 31; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;
                case EVENT_HILL_ALLIANCE_SIDE_AS_HORDE_5:
                    for (uint8 i = 24; i < 30; i++)
                    {
                        if (auto grunt = me->SummonCreature(NPC_NORTHWATCH_RECON, LineInTheDirtHorde[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            grunt->setActive(true);
                            grunt->SetHomePosition(-2460.97f, -2305.91f, 100.31f, 3.242f);
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _hordeField = false;
        bool _isHorde = false;
        uint32 _checkTimer = 3000;
        uint8 _defenderDieCount = 0;
        uint8 _killCount = 0;
        uint64 _playerGUID = 0;
        uint64 _neturalFlagGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_a_line_in_the_dirt_controllerAI(creature);
    }
};

uint32 const BloodWolfPathSize = 11;
const G3D::Vector3 BloodWolfPath[BloodWolfPathSize] =
{
    { -2111.744f, -1726.248f, 97.431f },
    { -2113.718f, -1732.964f, 96.374f },
    { -2117.447f, -1745.046f, 95.068f },
    { -2113.041f, -1753.232f, 94.617f },
    { -2105.620f, -1756.387f, 94.540f },
    { -2098.683f, -1762.115f, 93.914f },
    { -2098.631f, -1768.388f, 92.780f },
    { -2103.758f, -1774.495f, 91.884f },
    { -2118.482f, -1777.400f, 90.980f },
    { -2142.079f, -1771.818f, 90.097f },
    { -2152.257f, -1770.457f, 91.200f }, // stop 10
};

uint32 const BloodWolfPathSize2 = 42;
const G3D::Vector3 BloodWolfPath2[BloodWolfPathSize2] =
{
    { -2199.887f, -1775.315f, 91.659f },
    { -2231.999f, -1778.629f, 91.614f },
    { -2267.186f, -1781.073f, 95.420f },
    { -2297.516f, -1787.364f, 95.130f },
    { -2329.153f, -1797.327f, 95.905f },
    { -2352.575f, -1802.331f, 94.357f },
    { -2375.406f, -1798.316f, 91.851f },
    { -2405.249f, -1797.940f, 91.602f },
    { -2431.622f, -1798.070f, 91.800f },
    { -2459.281f, -1804.763f, 94.594f },
    { -2483.712f, -1812.222f, 94.231f },
    { -2505.668f, -1819.665f, 91.689f },
    { -2523.190f, -1825.604f, 93.931f },
    { -2551.935f, -1835.348f, 92.752f },
    { -2573.019f, -1852.936f, 92.300f },
    { -2594.565f, -1865.911f, 93.080f },
    { -2614.179f, -1889.986f, 95.706f },
    { -2644.265f, -1919.188f, 97.516f },
    { -2684.868f, -1939.445f, 98.201f },
    { -2709.066f, -1947.054f, 96.616f },
    { -2736.808f, -1952.786f, 94.292f },
    { -2766.079f, -1957.916f, 93.906f },
    { -2804.177f, -1990.595f, 92.435f },
    { -2834.446f, -2001.105f, 95.909f },
    { -2873.506f, -2013.833f, 92.256f },
    { -2906.278f, -2019.682f, 93.359f },
    { -2937.311f, -2021.468f, 94.805f },
    { -2972.330f, -2024.086f, 95.780f },
    { -2993.257f, -2021.287f, 95.932f },
    { -3019.951f, -2003.299f, 93.681f },
    { -3055.763f, -1988.185f, 95.910f },
    { -3074.778f, -1977.864f, 96.686f },
    { -3098.268f, -1948.479f, 96.612f },
    { -3127.906f, -1929.804f, 96.931f },
    { -3153.894f, -1915.838f, 101.42f },
    { -3176.314f, -1904.226f, 101.67f },
    { -3186.307f, -1887.324f, 99.312f },
    { -3195.102f, -1855.363f, 94.038f },
    { -3201.148f, -1820.065f, 91.792f },
    { -3207.401f, -1791.682f, 91.666f },
    { -3218.656f, -1765.452f, 92.305f },
    { -3242.373f, -1742.425f, 91.900f }, // 41
};

enum DesolationHoldInspection
{
    SPELL_SUMMON_BLOODHILT_MOUNT = 70533,
    SPELL_HORDE_BANNER = 70535,

    NPC_BLOODHILT_MOUNT = 37823,
    NPC_BLOODHILT = 37679,

};

class npc_bloodhilts_riding_wolf : public CreatureScript
{
public:
    npc_bloodhilts_riding_wolf() : CreatureScript("npc_bloodhilts_riding_wolf") { }

    struct npc_bloodhilts_riding_wolfAI : public VehicleAI
    {
        npc_bloodhilts_riding_wolfAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->SetRooted(true);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetDisableGravity(true);
            me->SetCanFly(false);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_HILL_WOLF_RIDING_1, 2000, 0);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
            }
        }

        void JustSummoned(Creature* summoned)
        {
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {}

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            TC_LOG_ERROR("sql.sql", "current point %u", point);

            // switch (point)
            // {
            // case 12:
            //     break;
            // default:
            //     break;
            // }

            if (_section == 1)
            {
                if (point == 10)
                {
                    if (auto bloodhiltmount = me->SummonCreature(NPC_BLOODHILT_MOUNT, -2150.691f, -1773.187f, 91.549f, 3.1314f))
                    {
                        _summons.Summon(bloodhiltmount);
                        bloodhiltmount->setActive(true);
                        bloodhiltmount->SetReactState(REACT_PASSIVE);
                        bloodhiltmount->GetMotionMaster()->MoveFollow(me, 1.f, 1);

                        if (auto bloodhilt = me->SummonCreature(NPC_BLOODHILT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                        {
                            _summons.Summon(bloodhilt);
                            bloodhilt->CastSpell(bloodhilt, SPELL_HORDE_BANNER);
                            bloodhilt->setActive(true);
                            bloodhilt->SetReactState(REACT_PASSIVE);
                            bloodhilt->EnterVehicle(bloodhiltmount, 0);
                        }
                    }

                    _events.ScheduleEvent(EVENT_HILL_WOLF_RIDING_2, 4000, 0);
                }
            }
            else if (_section == 2)
            {
                if (point == 41)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                    }

                    if (me->GetDistance(player) > 40.f)
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                    }
                }
            }
            _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_HILL_WOLF_RIDING_1:
                    _section = 1;
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(BloodWolfPath, BloodWolfPathSize);
                    break;
                case EVENT_HILL_WOLF_RIDING_2:
                    _section = 2;
                    me->GetMotionMaster()->MoveSmoothPath(BloodWolfPath2, BloodWolfPathSize2);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 1000;
        uint8 _section = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodhilts_riding_wolfAI(creature);
    }
};

enum Swarmers
{
    SPELL_BURNING_PITCH = 71030,
    SPELL_PC_SUMMON_GRUB = 71063,
    SPELL_PC_SUMMON_SWARMER = 71062,
};

class npc_silithid_swarmer_controller : public CreatureScript
{
public:
    npc_silithid_swarmer_controller() : CreatureScript("npc_silithid_swarmer_controller") { }

    struct npc_silithid_swarmer_controllerAI : public ScriptedAI
    {
        npc_silithid_swarmer_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_BURNING_PITCH)
            {
                if (auto player = who->ToPlayer())
                {
                    uint8 random = urand(1, 5); // 

                    if (random <= 2)
                    {
                        me->CastSpell(player, SPELL_PC_SUMMON_SWARMER);
                        me->DespawnOrUnsummon();
                    }
                    else if (random > 2 && random <= 4)
                    {
                        me->CastSpell(player, SPELL_PC_SUMMON_SWARMER);
                        me->CastSpell(player, SPELL_PC_SUMMON_SWARMER);
                        me->DespawnOrUnsummon();
                    }
                    else
                    {
                        me->CastSpell(player, SPELL_PC_SUMMON_GRUB);
                        me->DespawnOrUnsummon();
                    }
                }
            }
        }


    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silithid_swarmer_controllerAI(creature);
    }
};

enum ChangingGurdul
{
    QUEST_CHANGING_OF_THE_GARDUL = 24591,

    SPELL_PERM_KNEEL = 39656,
    NPC_BLOODHILTS_CARRY_BUNNY = 37866,
    NPC_WARLORD_GARDUL = 37811,
    NPC_WARLORD_BLOODHILT = 37837,
    NPC_CHANGING_KILL_CREDIT_1 = 37843,
    NPC_EVENT_STARTER_BUNNY = 74905,
};

class npc_changing_of_the_gardul_controller : public CreatureScript
{
public:
    npc_changing_of_the_gardul_controller() : CreatureScript("npc_changing_of_the_gardul_controller") { }

    struct npc_changing_of_the_gardul_controllerAI : public ScriptedAI
    {
        npc_changing_of_the_gardul_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->setActive(true);
            _isEventStarted = false;
            _summons.DespawnAll();
            _playerGUID = 0;
            _bloodGUID = 0;
            _gardulGUID = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 5.0f))
                if (auto player = who->ToPlayer())
                    if (!_isEventStarted)
                    {
                        if (player->GetQuestStatus(QUEST_CHANGING_OF_THE_GARDUL) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (!me->FindNearestCreature(NPC_WARLORD_GARDUL, 20.f)
                                || !me->FindNearestCreature(NPC_WARLORD_BLOODHILT, 20.f))
                            {
                                _isEventStarted = true;

                                if (auto gargul = me->SummonCreature(NPC_WARLORD_GARDUL, -3207.9194f, -1661.93f, 126.05f, 2.31f))
                                    if (auto bloodhilt = me->SummonCreature(NPC_WARLORD_BLOODHILT, -3219.161f, -1673.8629f, 119.6326f, 0.6496f))
                                    {
                                        player->KilledMonsterCredit(NPC_CHANGING_KILL_CREDIT_1);
                                        _summons.Summon(gargul);
                                        _summons.Summon(bloodhilt);
                                        gargul->setActive(true);
                                        bloodhilt->setActive(true);
                                        _bloodGUID = bloodhilt->GetGUID();
                                        _gardulGUID = gargul->GetGUID();
                                    }
                            }
                        }
                    }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto vehicle = me->SummonCreature(NPC_BLOODHILTS_CARRY_BUNNY, -3218.24f, -1661.792f, 130.94f, 5.442f))
                {
                    _summons.Summon(vehicle);

                    if (auto bloodhail = Unit::GetCreature(*me, _bloodGUID))
                        if (auto gargul = Unit::GetCreature(*me, _gardulGUID))
                        {
                            bloodhail->CastSpell(bloodhail, SPELL_SANCTUARY_NO_COMBAT);
                            gargul->CastSpell(gargul, SPELL_SANCTUARY_NO_COMBAT);
                            bloodhail->EnterVehicle(vehicle, 0);
                            gargul->EnterVehicle(vehicle, 1);

                            auto character = me->GetPlayersInRange(30, true);
                            for (auto it : character)
                                if ((it)->GetQuestStatus(QUEST_CHANGING_OF_THE_GARDUL) == QUEST_STATUS_INCOMPLETE)
                                    (it)->CastSpell(it, SPELL_SANCTUARY_NO_COMBAT);
                        }
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                _summons.DespawnAll();
                me->DespawnOrUnsummon(500);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                    }

                    if (me->GetDistance(player) > 30.f)
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                    }

                    if (auto bloodhail = Unit::GetCreature(*me, _bloodGUID))
                    {
                        if (!bloodhail->isAlive())
                        {
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon(500);
                        }
                    }
                }
            }
            _checkTimer -= diff;
        }
    private:
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        uint64 _bloodGUID = 0;
        uint64 _gardulGUID = 0;
        bool _isEventStarted = false;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_changing_of_the_gardul_controllerAI(creature);
    }
};

class npc_changing_of_the_gardul_carry_bunny : public CreatureScript
{
public:
    npc_changing_of_the_gardul_carry_bunny() : CreatureScript("npc_changing_of_the_gardul_carry_bunny") { }

    struct npc_changing_of_the_gardul_carry_bunnyAI : public VehicleAI
    {
        npc_changing_of_the_gardul_carry_bunnyAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
        {
            if (apply && passenger->GetEntry() == NPC_WARLORD_BLOODHILT)
            {
                _bloodGUID = passenger->GetGUID();
                if (auto target = me->FindNearestCreature(NPC_EVENT_STARTER_BUNNY, 20.f))
                    me->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
            }
            else if (apply && passenger->GetEntry() == NPC_WARLORD_GARDUL)
            {
                _gardulGUID = passenger->GetGUID();
            }

        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                if (auto bloodhilt = Unit::GetCreature(*me, _bloodGUID))
                    if (auto gardul = Unit::GetCreature(*me, _gardulGUID))
                    {
                        bloodhilt->ExitVehicle();
                        bloodhilt->SetFacingTo(5.511257f);
                        bloodhilt->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);
                        bloodhilt->AI()->SetData(2, 2);
                        me->GetMotionMaster()->MoveJump(-3204.17f, -1678.33f, 99.311f, 15.f, 23.f);
                        gardul->DespawnOrUnsummon(3000);
                        me->DespawnOrUnsummon(3000);
                    }
                break;
            default:
                break;
            }
        }
    private:
        uint64 _bloodGUID = 0;
        uint64 _gardulGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_changing_of_the_gardul_carry_bunnyAI(creature);
    }
};

enum ButcherOfTaurajo
{
    QUEST_THE_BUTCHER_OF_TAURAJO = 24637,

    NPC_TRIUMPTH_CAPTAIN = 37926,
    NPC_GENERAL_HAWTHORNE = 38018,
    NPC_KARTHOG = 38015,
    NPC_HORDE_FIELD_DEF = 37659,
};

const Position butcherPos[] =
{
    { -2612.2683f, -2182.560f, 95.785f, 3.175f },
    { -2611.6789f, -2177.072f, 95.781f, 3.234f },
    { -2613.4790f, -2180.021f, 95.783f, 3.195f },
};

class npc_the_butcher_of_taurajo_controller : public CreatureScript
{
public:
    npc_the_butcher_of_taurajo_controller() : CreatureScript("npc_the_butcher_of_taurajo_controller") { }

    struct npc_the_butcher_of_taurajo_controllerAI : public ScriptedAI
    {
        npc_the_butcher_of_taurajo_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->setActive(true);
            _isEventStarted = false;
            _fightInProgress = false;
            _summons.DespawnAll();
            _playerGUID = 0;
            _generalGUID = 0;

            if (auto def = Unit::GetCreature(*me, _defGUID))
                if (auto karthog = Unit::GetCreature(*me, _korthGUID))
                {
                    if (!def->isAlive())
                        def->Respawn(true);
                    else
                    {
                        def->SetHomePosition(-2669.449f, -2220.25f, 92.291f, 4.031f);
                        def->AI()->EnterEvadeMode();
                    }

                    karthog->SetHomePosition(-2670.949f, -2215.44f, 92.291f, 3.647740f);
                    karthog->AI()->EnterEvadeMode();
                    karthog->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_isEventStarted)
                if (me->IsWithinDistInMap(who, 45.0f))
                    if (auto player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_THE_BUTCHER_OF_TAURAJO) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (auto korth = me->FindNearestCreature(NPC_KARTHOG, 55.f))
                                korth->AI()->Talk(0, player->GetGUID());

                            _isEventStarted = true;
                        }
                    }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();

                    if (auto korth = me->FindNearestCreature(NPC_KARTHOG, 55.f))
                    {
                        _korthGUID = korth->GetGUID();

                        if (auto def = me->FindNearestCreature(NPC_HORDE_FIELD_DEF, 55.f))
                            _defGUID = def->GetGUID();
                    }
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (who->GetEntry() == NPC_KARTHOG)
                {
                    if (auto def = Unit::GetCreature(*me, _defGUID))
                        if (auto karthog = Unit::GetCreature(*me, _korthGUID))
                        {
                            _fightInProgress = true;
                            def->AI()->SetData(1, 1);
                            karthog->AI()->SetData(1, 1);
                            karthog->AI()->TalkWithDelay(3000, 2);

                            for (uint8 i = 0; i < 2; i++)
                            {
                                if (auto cap = me->SummonCreature(NPC_TRIUMPTH_CAPTAIN, butcherPos[i], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    _summons.Summon(cap);
                                    cap->setActive(true);
                                }
                            }

                            if (auto general = me->SummonCreature(NPC_GENERAL_HAWTHORNE, butcherPos[2], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                general->setActive(true);
                                _generalGUID = general->GetGUID();
                                _summons.Summon(general);
                            }
                        }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_fightInProgress)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 2000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (!player->isAlive())
                        {
                            Reset();
                        }

                        if (me->GetDistance(player) > 80.f)
                        {
                            Reset();
                        }

                        if (auto general = Unit::GetCreature(*me, _generalGUID))
                        {
                            if (!general->isAlive())
                            {
                                if (auto karthog = Unit::GetCreature(*me, _korthGUID))
                                {
                                    karthog->AI()->Talk(3);
                                    karthog->AI()->TalkWithDelay(4000, 4);
                                    _fightInProgress = false;

                                    _events.ScheduleEvent(EVENT_BUTCHER_TAURAJO_1, 12000);
                                }
                            }
                        }

                        if (auto karthog = Unit::GetCreature(*me, _korthGUID))
                        {
                            if (!karthog->isAlive())
                            {
                                Reset();
                            }
                        }
                    }
                }
                _checkTimer -= diff;
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BUTCHER_TAURAJO_1:
                    Reset();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isEventStarted = false;
        bool _fightInProgress = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        uint64 _defGUID = 0;
        uint64 _korthGUID = 0;
        uint64 _generalGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_butcher_of_taurajo_controllerAI(creature);
    }
};

enum TomusaSpell
{
    SPELL_HOOK_CAST = 70888,
};

// ID - 70887 Force Player to Cast Chain
class spell_tomusas_hook_cross_cast : public SpellScriptLoader
{
public:
    spell_tomusas_hook_cross_cast() : SpellScriptLoader("spell_tomusas_hook_cross_cast") { }

    class spell_tomusas_hook_cross_cast_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tomusas_hook_cross_cast_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (auto player = GetCaster())
                if (auto mercenary = GetHitUnit())
                {
                    if (auto veh = mercenary->GetVehicleBase())
                    {
                        if (auto veh1 = veh->ToCreature())
                        {
                            if (auto merc = mercenary->ToCreature())
                            {
                                veh1->CastSpell(veh1, SPELL_EJECT_ALL_PASSENGERS);
                                veh1->DespawnOrUnsummon(100);

                                player->CastSpell(merc, SPELL_HOOK_CAST);
                                merc->GetMotionMaster()->MoveJump(player->GetPositionX() + 1, player->GetPositionY() + 2, player->GetPositionZ(), 25.f, 25.f);
                                merc->SetHomePosition(player->GetPositionX() + 2, player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
                            }
                        }
                    }
                }
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_tomusas_hook_cross_cast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_tomusas_hook_cross_cast_SpellScript();
    }
};

enum GyroData
{
    QUEST_SURVEY_THE_DESTRUCTION = 25191,
    NPC_RADIO = 39327,
};

uint32 const SurveyGyroPathSize = 67;
const G3D::Vector3 SurveyGyroPath[SurveyGyroPathSize] =
{
    { -362.66296f, -1529.2839f, 91.7524f },
    { -348.52636f, -1526.2211f, 99.5801f },
    { -334.65466f, -1537.3510f, 109.226f },
    { -319.50497f, -1568.9250f, 116.379f },
    { -267.22018f, -1614.3554f, 133.556f },
    { -203.37368f, -1617.1420f, 145.423f },
    { -153.50012f, -1597.4321f, 150.087f },
    { -133.60035f, -1541.6907f, 152.916f },
    { -165.19116f, -1515.8565f, 159.500f },
    { -205.01817f, -1507.8857f, 150.332f },
    { -256.47488f, -1566.9021f, 133.306f }, // talk 1
    { -295.04129f, -1621.5162f, 114.076f },
    { -332.60885f, -1657.4831f, 93.4676f },
    { -404.40280f, -1667.4022f, 70.5040f },
    { -447.78662f, -1672.6887f, 63.4306f },
    { -484.87020f, -1689.8309f, 60.4375f },
    { -514.43695f, -1749.5383f, 51.2766f },
    { -568.70538f, -1763.8157f, 58.8984f },
    { -631.82751f, -1755.3203f, 60.3772f },
    { -678.92663f, -1793.3990f, 53.3981f },
    { -736.94628f, -1831.0126f, 69.4224f },
    { -807.59112f, -1845.0252f, 83.3911f },
    { -926.66247f, -1843.1726f, 102.111f },
    { -967.45050f, -1836.5233f, 119.179f },
    { -1039.5075f, -1831.7359f, 126.817f }, // talk 2
    { -1080.7572f, -1843.4014f, 128.401f },
    { -1134.8436f, -1877.6407f, 135.863f },
    { -1179.4713f, -1926.2630f, 135.405f },
    { -1232.7376f, -1994.6999f, 132.520f },
    { -1266.5352f, -2077.1503f, 126.080f },
    { -1259.3901f, -2194.9133f, 89.8530f },
    { -1256.5786f, -2255.7543f, 81.6558f },
    { -1260.4356f, -2326.5568f, 93.9017f },
    { -1265.5997f, -2415.9304f, 135.834f },
    { -1273.2059f, -2475.8237f, 140.850f },
    { -1300.9593f, -2544.1171f, 136.585f },
    { -1360.2458f, -2580.7783f, 131.549f },
    { -1453.3540f, -2589.9245f, 132.403f },
    { -1554.8371f, -2609.1237f, 131.795f },
    { -1607.4720f, -2626.6010f, 139.235f },
    { -1695.7536f, -2671.9880f, 165.617f },
    { -1747.4816f, -2751.7941f, 161.227f },
    { -1757.3160f, -2821.3928f, 149.769f },
    { -1741.9317f, -2909.7824f, 123.416f }, // talk 3
    { -1746.3107f, -2947.11f,   112.81f },
    { -1747.5437f, -3019.1342f, 99.8477f },
    { -1752.0081f, -3139.7963f, 88.4359f },
    { -1763.3089f, -3226.7180f, 106.484f },
    { -1792.4906f, -3299.0092f, 124.285f },
    { -1811.8585f, -3367.4663f, 131.345f },
    { -1793.1706f, -3433.3693f, 119.859f },
    { -1753.6479f, -3494.9951f, 96.3173f },
    { -1749.5487f, -3583.9279f, 78.0427f },
    { -1819.9548f, -3665.8103f, 69.1707f },
    { -1894.1661f, -3665.7854f, 66.7900f }, // talk 4
    { -1984.6876f, -3630.5681f, 75.0096f },
    { -2083.6306f, -3616.6357f, 107.918f },
    { -2154.9289f, -3627.3007f, 127.466f },
    { -2242.0981f, -3650.1794f, 151.806f },
    { -2265.2900f, -3678.2421f, 151.710f },
    { -2273.7968f, -3732.3989f, 143.700f },
    { -2249.7707f, -3789.2321f, 120.357f },
    { -2226.0126f, -3798.7612f, 114.736f },
    { -2202.8925f, -3795.9619f, 112.774f },
    { -2190.5827f, -3769.5615f, 111.726f },
    { -2196.7319f, -3747.2360f, 107.352f },
    { -2217.1606f, -3718.0390f, 92.6685f }, // 66
};

class npc_spindlegears_survey_craft : public CreatureScript
{
public:
    npc_spindlegears_survey_craft() : CreatureScript("npc_spindlegears_survey_craft") { }

    struct npc_spindlegears_survey_craftAI : public VehicleAI
    {
        npc_spindlegears_survey_craftAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
            me->setActive(true);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto radio = me->SummonCreature(NPC_RADIO, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _radioGUID = radio->GetGUID();
                    _summons.Summon(radio);
                    radio->EnterVehicle(me, 1);
                    _events.ScheduleEvent(EVENT_GYRO_1, 2000, 0);
                }
            }
            //
            // if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            // {
            // }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 10:
                if (auto radio = Unit::GetCreature(*me, _radioGUID))
                    radio->AI()->Talk(0, _playerGUID);
                break;
            case 24:
                if (auto radio = Unit::GetCreature(*me, _radioGUID))
                    radio->AI()->Talk(1, _playerGUID);
                break;
            case 44:
                if (auto radio = Unit::GetCreature(*me, _radioGUID))
                    radio->AI()->Talk(2, _playerGUID);
                break;
            case 54:
                if (auto radio = Unit::GetCreature(*me, _radioGUID))
                    radio->AI()->Talk(3, _playerGUID);
                break;
            case 66:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->AreaExploredOrEventHappens(QUEST_SURVEY_THE_DESTRUCTION);
                    player->ExitVehicle();
                    _summons.DespawnAll();
                    me->DespawnOrUnsummon(100);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetDistance(player) >= 200.f)
                    {
                        me->DespawnOrUnsummon();
                        _summons.DespawnAll();
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GYRO_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(SurveyGyroPath, SurveyGyroPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 3000;
        uint64 _playerGUID = 0;
        uint64 _radioGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spindlegears_survey_craftAI(creature);
    }
};

uint32 const GryphonTowerPathSize = 13;
const G3D::Vector3 GryphonTowerPath[GryphonTowerPathSize] =
{
    { -2214.760f, -3778.729f, 137.524f },
    { -2207.415f, -3783.643f, 140.379f },
    { -2198.474f, -3780.440f, 145.950f },
    { -2198.789f, -3752.296f, 148.135f },
    { -2222.630f, -3719.840f, 155.127f },
    { -2260.999f, -3710.493f, 153.663f },
    { -2278.846f, -3737.158f, 151.881f },
    { -2274.090f, -3780.869f, 141.145f },
    { -2248.771f, -3865.202f, 112.134f },
    { -2232.355f, -3926.611f, 89.9062f },
    { -2214.059f, -4001.146f, 63.7908f },
    { -2178.004f, -4069.745f, 34.6403f },
    { -2156.961f, -4083.741f, 23.9793f }, // 12
};

uint32 const GryphonTowerPathSize2 = 11;
const G3D::Vector3 GryphonTowerPath2[GryphonTowerPathSize2] =
{
    { -2163.204f, -4084.998f, 26.1776f },
    { -2192.667f, -4039.812f, 41.7126f },
    { -2227.653f, -3979.613f, 79.7965f },
    { -2281.591f, -3875.782f, 98.3520f },
    { -2287.708f, -3803.443f, 135.531f },
    { -2261.993f, -3754.598f, 140.081f },
    { -2231.887f, -3731.634f, 145.285f },
    { -2186.707f, -3749.415f, 150.417f },
    { -2193.546f, -3773.608f, 149.186f },
    { -2214.239f, -3787.383f, 145.042f },
    { -2219.634f, -3776.955f, 134.525f },
};

enum Gryphon
{
    AREA_NORTHWATCH_HOLD = 4847,
};

class npc_northwatch_gryphon : public CreatureScript
{
public:
    npc_northwatch_gryphon() : CreatureScript("npc_northwatch_gryphon") { }

    struct npc_northwatch_gryphonAI : public VehicleAI
    {
        npc_northwatch_gryphonAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);
            me->setActive(true);

            if (me->GetAreaId() == AREA_NORTHWATCH_HOLD)
                _isTower = true;
            else
                _isTower = false;

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();

                if (player->isInCombat())
                    player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (_isTower)
                    _events.ScheduleEvent(EVENT_GRYPHON_1, 4000);
                else
                    _events.ScheduleEvent(EVENT_GRYPHON_2, 3000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_isTower)
            {
                if (point == 12)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(100);
                    }
                }
            }
            else
            {
                if (point == 10)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(100);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetDistance(player) >= 100.f)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GRYPHON_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(GryphonTowerPath, GryphonTowerPathSize);
                    break;
                case EVENT_GRYPHON_2:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(GryphonTowerPath2, GryphonTowerPathSize2);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isTower = false;
        uint32 _checkTimer = 3000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_northwatch_gryphonAI(creature);
    }
};

enum RunOutTheGun
{
    QUEST_RUN_OUT_THE_GUN = 24939,
    NPC_RAGEROAR_BOAT = 38747,

    SPELL_VISUAL_BOAT_ALIVE = 72697,
    SPELL_VISUAL_BOAT_DEAD = 72692,

};

const Position SummonBoatPos[] =
{
    { -1877.659f, -3791.618f, 0.000f },
    { -1877.287f, -3801.895f, 0.000f },
    { -1877.324f, -3816.133f, 0.002f },
    { -1882.822f, -3834.344f, 0.001f },
    { -1890.695f, -3849.374f, 0.001f },
    { -1898.156f, -3853.090f, 0.001f },
    { -1935.167f, -3867.514f, 0.001f },
    { -1954.499f, -3871.484f, 0.001f },
    { -1967.286f, -3872.489f, 0.000f },
    { -1998.846f, -3875.190f, 0.003f },
    { -2021.616f, -3881.356f, 0.000f },
    { -2042.342f, -3886.023f, 0.000f },
    { -2064.399f, -3882.300f, 0.000f },
    { -2080.369f, -3874.949f, 0.001f }, // 13
};

class npc_northwatch_shore_battery : public CreatureScript
{
public:
    npc_northwatch_shore_battery() : CreatureScript("npc_northwatch_shore_battery") { }

    struct npc_northwatch_shore_batteryAI : public VehicleAI
    {
        npc_northwatch_shore_batteryAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = true;
                me->SetRooted(true);

                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = false;
                _playerGUID = NULL;
                _summons.DespawnAll();
                _waveCount = 0;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            _summons.Summon(summoned);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
        }

        void SummonBoat()
        {
            uint64 boat_pathID = urand(600022, 600039);  // path between 600022 and 600039
            uint8 summonPos = urand(0, 13);

            if (auto boat = me->SummonCreature(NPC_RAGEROAR_BOAT, SummonBoatPos[summonPos], TEMPSUMMON_TIMED_DESPAWN, 120000))
            {
                boat->setActive(true);
                boat->GetMotionMaster()->MovePath(boat_pathID, false);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_onUse)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 3000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetQuestStatus(QUEST_RUN_OUT_THE_GUN) == QUEST_STATUS_NONE)
                        {
                            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                            _onUse = false;
                            _playerGUID = NULL;
                            _summons.DespawnAll();
                            _waveCount = 0;
                        }

                        if (player->GetQuestStatus(QUEST_RUN_OUT_THE_GUN) == QUEST_STATUS_COMPLETE)
                        {
                            _waveCount = 0;
                            _onUse = false;
                            _playerGUID = NULL;
                            _summons.DespawnAll();
                            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        }
                    }
                }
                else
                    _checkTimer -= diff;

                if (_summonTimer <= diff)
                {
                    _summonTimer = 5000;

                    _waveCount++;
                    if (_waveCount <= 15)
                        SummonBoat();
                }
                else
                    _summonTimer -= diff;
            }
        }
    private:
        uint32 _waveCount = 0;
        bool _onUse = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
        uint16 _summonTimer = 3000;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_northwatch_shore_batteryAI(creature);
    }
};

enum KargaRageroar
{
    QUEST_KARGA_RAGEROAR = 24956,

    NPC_KARGA_RAGEROAR = 38663,
    NPC_HEXMASTER = 38826,
};
class npc_silencing_rageroar_fight_activator : public CreatureScript
{
public:
    npc_silencing_rageroar_fight_activator() : CreatureScript("npc_silencing_rageroar_fight_activator") { }

    struct npc_silencing_rageroar_fight_activatorAI : public ScriptedAI
    {
        npc_silencing_rageroar_fight_activatorAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            _isEventStarted = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_isEventStarted)
                if (me->IsWithinDistInMap(who, 10.0f))
                    if (auto player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_KARGA_RAGEROAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (auto karga = me->FindNearestCreature(NPC_KARGA_RAGEROAR, 40.f))
                                if (auto hex = me->FindNearestCreature(NPC_HEXMASTER, 40.f))
                                {
                                    if (!karga->isInCombat() && !hex->isInCombat())
                                    {
                                        karga->setActive(true);
                                        hex->setActive(true);
                                        _hexGUID = hex->GetGUID();
                                        _kargaGUID = karga->GetGUID();
                                        karga->AI()->SetData(1, 1);
                                        hex->AI()->SetData(1, 1);
                                        _isEventStarted = true;
                                    }
                                }
                        }
                    }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto hex = Unit::GetCreature(*me, _hexGUID))
                    if (auto karga = Unit::GetCreature(*me, _kargaGUID))
                    {
                        hex->AI()->Reset();
                        karga->AI()->Reset();
                        karga->RemoveAura(46934);
                        hex->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        karga->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        karga->SetReactState(REACT_PASSIVE);
                        _isEventStarted = false;

                        if (!hex->isAlive())
                            hex->Respawn();
                    }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_isEventStarted)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 2000;

                    if (auto hex = Unit::GetCreature(*me, _hexGUID))
                        if (auto karga = Unit::GetCreature(*me, _kargaGUID))
                        {
                            if (!karga->isAlive() && !hex->isAlive())
                            {
                                Reset();
                                _isEventStarted = false;
                            }
                            else if (karga->isAlive() && !hex->isAlive())
                            {
                                hex->SetRespawnTime(85);
                            }
                        }
                }
                _checkTimer -= diff;
            }
        }
    private:
        bool _isEventStarted = false;
        uint32 _checkTimer = 2000;
        uint64 _hexGUID = 0;
        uint64 _kargaGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_silencing_rageroar_fight_activatorAI(creature);
    }
};

enum GainesData
{
    QUEST_RESCUE_GAINES = 25022,

    NPC_CHAINS_GAINES = 74901,
    SPELL_CHAIN_RIGHT = 84990,
    SPELL_CHAIN_LEFT = 84991,
    SPELL_GAINES_CREDIT = 73118,

    WAYPOINT_DATA_PATH_ID = 600040,
};

class npc_ambassador_gaines_rescue : public CreatureScript
{
public:
    npc_ambassador_gaines_rescue() : CreatureScript("npc_ambassador_gaines_rescue") { }

    struct npc_ambassador_gaines_rescueAI : public ScriptedAI
    {
        npc_ambassador_gaines_rescueAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->HandleEmoteCommand(EMOTE_STATE_STUN);
            _inProgress = false;

            if (!me->HasAura(SPELL_CHAIN_RIGHT) || !me->HasAura(SPELL_CHAIN_LEFT))
            {
                auto chain = me->SelectNearbyUnits(NPC_CHAINS_GAINES, 10.0f);
                for (auto itr = chain.begin(); itr != chain.end(); ++itr)
                    if (auto chain = (*itr))
                    {
                        if (chain->GetGUIDLow() == _chainRightGUID)
                        {
                            me->RemoveAura(SPELL_CHAIN_RIGHT);
                            chain->CastSpell(me, SPELL_CHAIN_RIGHT);
                        }
                        else if (chain->GetGUIDLow() == _chainLeftGUID)
                        {
                            me->RemoveAura(SPELL_CHAIN_LEFT);
                            chain->CastSpell(me, SPELL_CHAIN_LEFT);
                        }
                    }
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_inProgress)
                if (me->IsWithinDistInMap(who, 22.0f))
                    if (auto player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_RESCUE_GAINES) == QUEST_STATUS_INCOMPLETE)
                        {
                            _inProgress = true;
                            Talk(0, player->GetGUID());
                        }
                    }
        }

        void GainesRescued()
        {
            me->CastSpell(me, SPELL_GAINES_CREDIT);
            me->HandleEmoteCommand(EMOTE_STATE_STAND);
            Talk(1);
            TalkWithDelay(5000, 2);

            _events.ScheduleEvent(EVENT_GAINES_1, 10000);
        }


        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING_2) // right
            {
                me->RemoveAura(SPELL_CHAIN_RIGHT);

                if (!me->HasAura(SPELL_CHAIN_RIGHT) && !me->HasAura(SPELL_CHAIN_LEFT))
                    GainesRescued();
            }

            if (spellInfo->Id == SPELL_DUMMY_PING) // left
            {
                me->RemoveAura(SPELL_CHAIN_LEFT);

                if (!me->HasAura(SPELL_CHAIN_RIGHT) && !me->HasAura(SPELL_CHAIN_LEFT))
                    GainesRescued();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GAINES_1:
                    me->GetMotionMaster()->MovePath(WAYPOINT_DATA_PATH_ID, false);
                    me->DespawnOrUnsummon(4000);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _inProgress = false;
        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _chainRightGUID = 39689;
        uint64 _chainLeftGUID = 39687;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ambassador_gaines_rescueAI(creature);
    }
};

enum JungleVine
{
    QUEST_YOU_FLICKED = 25027,
    SPELL_CORPSE_EXPLODE = 81876,
};

class npc_barrens_jungle_vine : public CreatureScript
{
public:
    npc_barrens_jungle_vine() : CreatureScript("npc_barrens_jungle_vine") { }

    struct npc_barrens_jungle_vineAI : public VehicleAI
    {
        npc_barrens_jungle_vineAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto threetooth = summoner->ToCreature())
            {
                if (auto player = threetooth->FindNearestPlayer(50.f))
                {
                    _threetoothGUID = threetooth->GetGUID();
                    threetooth->SetReactState(REACT_PASSIVE);
                    threetooth->EnterVehicle(me, 0);
                    threetooth->AI()->TalkWithDelay(4000, 1, player->GetGUID());
                    _events.ScheduleEvent(EVENT_THREETOOTH_1, 8500);
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_THREETOOTH_1:
                    if (auto three = Unit::GetCreature(*me, _threetoothGUID))
                    {
                        three->CastSpell(me, SPELL_CORPSE_EXPLODE);
                        three->ExitVehicle();
                        three->Kill(three);
                        three->SetHomePosition(-2271.159f, -2568.879f, 91.666f, 5.23f);
                        me->Kill(me);

                        auto player = me->GetPlayersInRange(45, true);
                        for (auto it = player.begin(); it != player.end(); it++)
                            if ((*it)->GetQuestStatus(QUEST_YOU_FLICKED) == QUEST_STATUS_INCOMPLETE)
                                (*it)->KilledMonsterCredit(three->GetEntry()); // credit
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        uint64 _threetoothGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_barrens_jungle_vineAI(creature);
    }
};

enum AtTriumphMessenger
{
    QUEST_MEET_ME = 25074,
    QUEST_DIGSITE_STATUS = 25104,
    SPELL_AURA_CHECK = 151348,
    NPC_MESSENGER = 39088,
    NPC_BODYGUARD = 39156,

    AT_TWINBRAID = 5787,
    AREATRIGGER_TALK_COOLDOWN = 20,
};

class AreaTrigger_southern_barrens_triumph_messenger : public AreaTriggerScript
{
public:
    AreaTrigger_southern_barrens_triumph_messenger() : AreaTriggerScript("at_southern_barrens_triumph_messenger") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
    {
        if (player->GetQuestStatus(QUEST_MEET_ME) == QUEST_STATUS_COMPLETE)
        {
            if (player->HasAura(SPELL_AURA_CHECK))
            {
                player->RemoveAura(SPELL_AURA_CHECK);
                player->SummonCreature(NPC_MESSENGER, player->GetPositionX(), player->GetPositionY() + 4, player->GetPositionZ() + 1, player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
            }
        }
        return false;
    }
};

class AreaTrigger_southern_barrens_twinbraid_is_up : public AreaTriggerScript
{
public:
    AreaTrigger_southern_barrens_twinbraid_is_up() : AreaTriggerScript("at_southern_barrens_twinbraid_is_up")
    {
        // Initialize for cooldown
        _triggerTimes[AT_TWINBRAID] = 0;
    }

    bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
    {
        uint32 triggerId = trigger->id;
        // Second trigger happened too early after first, skip for now
        if (sWorld->GetGameTime() - _triggerTimes[triggerId] < AREATRIGGER_TALK_COOLDOWN)
            return false;

        if (player->GetQuestStatus(QUEST_DIGSITE_STATUS) == QUEST_STATUS_COMPLETE)
        {
            if (auto body = player->FindNearestCreature(NPC_BODYGUARD, 20.f))
            {
                body->AI()->Talk(0, player->GetGUID());
            }
        }

        _triggerTimes[trigger->id] = sWorld->GetGameTime();
        return false;
    }

private:
    std::map<uint32, time_t> _triggerTimes;
};

enum MarleyFinalFlight
{
    QUEST_MARLEY_FINAL_FLIGHT = 25120,
    NPC_MARLEY_PASSENGER = 39184,
    NPC_EXPLODE_BUNNY = 74900,
    SPELL_LEFT_WING_EXPLOSION = 73587,
    SPELL_RIGHT_WING_EXPLOSION = 73588,
    SPELL_FLYING_MISSION_KILL_CREDIT = 73591,
    SPELL_BURNING = 71597,
    SPELL_EXPLODE = 65779,
    SPELL_EXPLODE_2 = 76841,

};

uint32 const MarleyPathSize = 14;
const G3D::Vector3 MarleyPath[MarleyPathSize] =
{
    { -4171.4272f, -2138.1574f, 87.f },
    { -4161.1494f, -2129.5568f, 91.f }, // 1
    { -4156.1943f, -2111.2312f, 97.f },
    { -4170.6601f, -2090.0598f, 107.f },
    { -4190.8940f, -2086.1677f, 117.f },
    { -4209.1938f, -2106.7888f, 125.f },
    { -4224.2084f, -2145.8117f, 136.f },
    { -4224.2983f, -2161.0139f, 145.f },
    { -4223.1132f, -2200.1054f, 147.f }, // 8
    { -4229.9497f, -2223.6374f, 151.f },
    { -4256.3564f, -2283.5561f, 152.f },
    { -4248.3637f, -2339.0744f, 153.f }, // 11
    { -4201.6323f, -2363.8342f, 156.f },
    { -4142.3510f, -2333.4636f, 156.f },
};

uint32 const MarleyPathSize2 = 2;
const G3D::Vector3 MarleyPath2[MarleyPathSize2] =
{
    { -4127.98f, -2324.501f, 150.02f },
    { -4108.42f, -2311.917f, 140.05f }, // 1
};

class npc_marleys_final_flight : public CreatureScript
{
public:
    npc_marleys_final_flight() : CreatureScript("npc_marleys_final_flight") { }

    struct npc_marleys_final_flightAI : public VehicleAI
    {
        npc_marleys_final_flightAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetPhaseMask(64, false);
            me->setActive(true);
            me->SetRooted(true);

            if (auto marley = me->SummonCreature(NPC_MARLEY_PASSENGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000))
            {
                _summons.Summon(marley);
                marley->EnterVehicle(me, 0);
                _marleyGUID = marley->GetGUID();
            }

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastWithDelay(500, me, SPELL_RIDE_VEHICLE);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_MARLEY_FLIGHT_1, 3000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type == SPLINE_MOTION_TYPE)
            {
                if (!_explodeEvent)
                {
                    switch (point)
                    {
                    case 1:
                        if (auto marley = Unit::GetCreature(*me, _marleyGUID))
                            marley->AI()->Talk(0, _playerGUID);
                        break;
                    case 8:
                        if (auto marley = Unit::GetCreature(*me, _marleyGUID))
                            marley->AI()->Talk(1, _playerGUID);
                        break;
                    case 11:
                        if (auto marley = Unit::GetCreature(*me, _marleyGUID))
                            marley->AI()->Talk(2, _playerGUID);
                        break;
                    case 13:
                    {
                        auto explo = me->SelectNearbyUnits(NPC_EXPLODE_BUNNY, 110.0f);
                        for (auto itr = explo.begin(); itr != explo.end(); ++itr)
                            if (auto explode = (*itr)->ToCreature())
                                explode->AI()->SetData(1, 1);

                        if (auto marley = Unit::GetCreature(*me, _marleyGUID))
                            marley->AI()->TalkWithDelay(2000, 3, _playerGUID);

                        _explodeEvent = true;
                        me->GetMotionMaster()->MoveSmoothPath(MarleyPath2, MarleyPathSize2);
                        me->CastWithDelay(500, me, 44762);
                        me->CastWithDelay(500, me, SPELL_LEFT_WING_EXPLOSION);
                        me->CastWithDelay(1000, me, SPELL_RIGHT_WING_EXPLOSION);
                    }
                    break;
                    default:
                        break;
                    }
                }
                else
                {
                    if (point == 1)
                    {
                        me->CastSpell(me, SPELL_BURNING);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(-4075.756f, -2292.65f, 124.6585f, 13.f, 11.f, 1);
                        me->CastSpell(me, SPELL_EXPLODE_2);
                        me->CastWithDelay(1000, me, SPELL_EXPLODE_2);
                        me->CastWithDelay(2000, me, SPELL_EXPLODE_2);
                        me->CastWithDelay(3000, me, SPELL_EXPLODE_2);
                    }
                }
            }

            if (type == EFFECT_MOTION_TYPE)
            {
                if (point == 1)
                {
                    me->CastSpell(me, SPELL_EXPLODE);
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    _events.ScheduleEvent(EVENT_MARLEY_FLIGHT_2, 1000);
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetDistance(player) >= 100.f)
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_MARLEY_FINAL_FLIGHT) == QUEST_STATUS_NONE)
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }
                }
            }
            else _checkTimer -= diff;


            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MARLEY_FLIGHT_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(MarleyPath, MarleyPathSize);
                    break;
                case EVENT_MARLEY_FLIGHT_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                        player->KilledMonsterCredit(39187);
                        player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _explodeEvent = false;
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _marleyGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marleys_final_flightAI(creature);
    }
};

enum QuestBaeldunRescue
{
    QUEST_BAELDUN_RESCUE = 25153,

    NPC_RESCUE_VEHICLE = 74899,
    NPC_BAELDUN_SURVIVOR_RESCUE_CREDIT = 39219,
    SPELL_DIGGING_THROUGH_THE_DEBRIS = 73679,
    SPELL_DEBRIS_VISUAL = 73677,
};

class npc_baeldun_burning_debris : public CreatureScript
{
public:
    npc_baeldun_burning_debris() : CreatureScript("npc_baeldun_burning_debris") { }

    struct npc_baeldun_burning_debrisAI : public ScriptedAI
    {
        npc_baeldun_burning_debrisAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->CastSpell(me, SPELL_DEBRIS_VISUAL);

            if (auto veh = me->SummonCreature(NPC_RESCUE_VEHICLE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
            {
                veh->setActive(true);
                _vehGUID = veh->GetGUID();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (auto player = who->ToPlayer())
            {
                if (spellInfo->Id == SPELL_DIGGING_THROUGH_THE_DEBRIS)
                {
                    player->KilledMonsterCredit(NPC_BAELDUN_SURVIVOR_RESCUE_CREDIT);

                    if (auto veh = Unit::GetCreature(*me, _vehGUID))
                        veh->AI()->SetData(1, 1);

                    me->DespawnOrUnsummon(100);
                }
            }
        }
    private:
        uint64 _vehGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_baeldun_burning_debrisAI(creature);
    }
};

enum BattleScarFlag
{
    QUEST_CLAIM_THE_BATTLESCAR_H = 24618,
    QUEST_CLAIM_THE_BATTLESCAR_A = 25081,

    NPC_FLAG_SCOUTED_CREDIT = 74997,
    OBJECT_BATTLESCAR_BANNER_HORDE = 201879,
    OBJECT_BATTLESCAR_BANNER_ALLIANCE = 201878,
    OBJECT_BATTLESCAR_FLAGPOLE_FOR_HORDE = 201876,
    OBJECT_BATTLESCAR_FLAGPOLE_FOR_ALLY = 201877,

    SPELL_BATTLESCAR_DOMINANCE_HORDE = 70694,
    SPELL_BATTLESCAR_DOMINANCE_ALLIANCE = 70697,

    SPELL_LEVEL_UP_EFFECT_RED_GIANT = 70706,
    SPELL_LEVEL_UP_EFFECT_BLUE_GIANT = 70708,

    AREAID_BATTLESCAR = 4855,
};

const Position battlescarPos[] =
{
    { -3244.45f, -2002.33f, 87.93f, 1.55f },   //  Battlescar Flagpole 
    { -3244.45f, -2001.44f, 96.14f,  1.55f },   //  Battlescar Banner - Horde 1 side
    { -3244.52f, -2003.47f, 96.16f, -1.64f },   //  Battlescar Banner - Horde 2 side
    { -3244.47f, -2002.69f, 94.37f, -1.588f },   //  Battlescar Banner - Alliance 1 side
    { -3244.42f, -2002.19f, 94.26f,  1.588f },   //  Battlescar Banner - Alliance 2 side
};

class npc_battlescar_flag_controller : public CreatureScript
{
public:
    npc_battlescar_flag_controller() : CreatureScript("npc_battlescar_flag_controller") { }

    struct npc_battlescar_flag_controllerAI : public ScriptedAI
    {
        npc_battlescar_flag_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            _isHorde = true;

            if (auto flag = me->SummonGameObject(OBJECT_BATTLESCAR_FLAGPOLE_FOR_ALLY, -3244.45f, -2002.33f, 87.93f, 1.55f, 0, 0, 0, 0, 0))
                if (auto banner1 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_HORDE, -3244.45f, -2001.44f, 96.14f, 1.55f, 0, 0, 0, 0, 0))
                    if (auto banner2 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_HORDE, -3244.52f, -2003.47f, 96.16f, -1.64f, 0, 0, 0, 0, 0))
                    {
                        flag->setActive(true);
                        banner1->setActive(true);
                        banner2->setActive(true);
                        _flagGUID = flag->GetGUID();
                        _banner1GUID = banner1->GetGUID();
                        _banner2GUID = banner2->GetGUID();
                    }
        }

        void RemoveFlagAndBanners()
        {
            if (auto flag = GameObject::GetGameObject(*me, _flagGUID))
                if (auto banner = GameObject::GetGameObject(*me, _banner1GUID))
                    if (auto banner2 = GameObject::GetGameObject(*me, _banner2GUID))
                    {
                        flag->Delete();
                        banner->Delete();
                        banner2->Delete();
                    }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetAreaId() == AREAID_BATTLESCAR)
            {
                if (auto player = who->ToPlayer())
                {
                    if (me->IsWithinDistInMap(player, 300.0f))
                    {
                        if (_isHorde)
                        {
                            if (player->GetTeam() == HORDE)
                            {
                                if (!player->HasAura(SPELL_BATTLESCAR_DOMINANCE_HORDE))
                                    player->CastSpell(player, SPELL_BATTLESCAR_DOMINANCE_HORDE);
                            }
                            else
                            {
                                if (player->HasAura(SPELL_BATTLESCAR_DOMINANCE_ALLIANCE))
                                    player->RemoveAura(SPELL_BATTLESCAR_DOMINANCE_ALLIANCE);
                            }

                        }
                        else
                        {
                            if (player->GetTeam() == ALLIANCE)
                            {
                                if (!player->HasAura(SPELL_BATTLESCAR_DOMINANCE_ALLIANCE))
                                    player->CastSpell(player, SPELL_BATTLESCAR_DOMINANCE_ALLIANCE);
                            }
                            else
                            {
                                if (player->HasAura(SPELL_BATTLESCAR_DOMINANCE_HORDE))
                                    player->RemoveAura(SPELL_BATTLESCAR_DOMINANCE_HORDE);
                            }
                        }
                    }

                    if (me->IsWithinDistInMap(player, 10.0f))
                    {
                        if (_isHorde)
                        {
                            if (player->GetQuestStatus(QUEST_CLAIM_THE_BATTLESCAR_H) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(NPC_FLAG_SCOUTED_CREDIT);
                        }
                        else
                        {
                            if (player->GetQuestStatus(QUEST_CLAIM_THE_BATTLESCAR_A) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(NPC_FLAG_SCOUTED_CREDIT);;
                        }
                    }
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = who->ToPlayer())
                {
                    if (player->GetTeam() == ALLIANCE)
                    {
                        if (!player->HasAura(SPELL_BATTLESCAR_DOMINANCE_ALLIANCE))
                            player->CastSpell(player, SPELL_BATTLESCAR_DOMINANCE_ALLIANCE);

                        if (player->GetQuestStatus(QUEST_CLAIM_THE_BATTLESCAR_A) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(NPC_FLAG_SCOUTED_CREDIT);

                        _isHorde = false;
                        RemoveFlagAndBanners();
                        Talk(0);
                        me->CastSpell(me, SPELL_LEVEL_UP_EFFECT_BLUE_GIANT);

                        if (auto flag = me->SummonGameObject(OBJECT_BATTLESCAR_FLAGPOLE_FOR_HORDE, -3244.45f, -2002.33f, 87.93f, 1.55f, 0, 0, 0, 0, 0))
                            if (auto banner1 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_ALLIANCE, -3244.45f, -2001.44f, 96.14f, 1.55f, 0, 0, 0, 0, 0))
                                if (auto banner2 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_ALLIANCE, -3244.52f, -2003.47f, 96.16f, -1.64f, 0, 0, 0, 0, 0))
                                {
                                    _flagGUID = flag->GetGUID();
                                    _banner1GUID = banner1->GetGUID();
                                    _banner2GUID = banner2->GetGUID();
                                }
                    }
                    else
                    {
                        if (!player->HasAura(SPELL_BATTLESCAR_DOMINANCE_HORDE))
                            player->CastSpell(player, SPELL_BATTLESCAR_DOMINANCE_HORDE);

                        if (player->GetQuestStatus(QUEST_CLAIM_THE_BATTLESCAR_H) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(NPC_FLAG_SCOUTED_CREDIT);

                        _isHorde = true;
                        RemoveFlagAndBanners();
                        Talk(1);
                        me->CastSpell(me, SPELL_LEVEL_UP_EFFECT_RED_GIANT);

                        if (auto flag = me->SummonGameObject(OBJECT_BATTLESCAR_FLAGPOLE_FOR_ALLY, -3244.45f, -2002.33f, 87.93f, 1.55f, 0, 0, 0, 0, 0))
                            if (auto banner1 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_HORDE, -3244.45f, -2001.44f, 96.14f, 1.55f, 0, 0, 0, 0, 0))
                                if (auto banner2 = me->SummonGameObject(OBJECT_BATTLESCAR_BANNER_HORDE, -3244.52f, -2003.47f, 96.16f, -1.64f, 0, 0, 0, 0, 0))
                                {
                                    _flagGUID = flag->GetGUID();
                                    _banner1GUID = banner1->GetGUID();
                                    _banner2GUID = banner2->GetGUID();
                                }
                    }
                }
            }
        }
    private:
        bool _isHorde = true;
        uint64 _flagGUID = 0;
        uint64 _banner1GUID = 0;
        uint64 _banner2GUID = 0;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_battlescar_flag_controllerAI(creature);
    }
};

enum Sabotage
{
    QUEST_SABOTAGE = 24747,
    SPELL_CAMERA_SHAKE = 66772,
    SPELL_RUN_FOR_IT = 71455,
    SPELL_SUMMON_CAMERA_VIEW = 71472,
};

class npc_sabotage_controller : public CreatureScript
{
public:
    npc_sabotage_controller() : CreatureScript("npc_sabotage_controller") { }

    struct npc_sabotage_controllerAI : public ScriptedAI
    {
        npc_sabotage_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->setActive(true);
            _inProgress = false;
            _nearExit = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!_nearExit)
                if (me->IsWithinDistInMap(who, 30.0f))
                    if (auto player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_SABOTAGE) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (player->HasAura(SPELL_RUN_FOR_IT))
                            {
                                _nearExit = true;
                                player->CastSpell(player, SPELL_CAMERA_SHAKE);
                                Talk(0, player->GetGUID());
                            }
                        }
                    }

            if (!_inProgress)
            {
                if (me->IsWithinDistInMap(who, 7.0f))
                {
                    if (auto player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_SABOTAGE) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (player->HasAura(SPELL_RUN_FOR_IT))
                            {
                                _inProgress = true;
                                player->RemoveAura(SPELL_RUN_FOR_IT);
                                player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                                player->CastSpell(player, SPELL_SUMMON_CAMERA_VIEW);
                                Reset();
                            }
                        }
                    }
                }
            }
        }
    private:
        bool _nearExit = false;
        bool _inProgress = false;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sabotage_controllerAI(creature);
    }
};

enum TheSabotageCamera
{
    NPC_DOPPLEGANGER = 74896,
    NPC_CHANNEL_TARGET = 75044,
    SPELL_CAMERA_CHANNEL = 82557,
    SPELL_TELEPORT_END = 98053,
    SPELL_SET_LOOK = 71505,
};

uint32 const SabotageCameraPathSize = 4;
const G3D::Vector3 SabotageCameraPath[SabotageCameraPathSize] =
{
    { -4089.252f, -2334.15f, 127.71f }, // 0
    { -4094.590f, -2321.66f, 128.45f }, // 1
    { -4098.648f, -2306.01f, 138.33f }, // 2
    { -4087.756f, -2271.02f, 157.925f }, // 3
};

class npc_sabotage_camera_view : public CreatureScript
{
public:
    npc_sabotage_camera_view() : CreatureScript("npc_sabotage_camera_view") { }

    struct npc_sabotage_camera_viewAI : public VehicleAI
    {
        npc_sabotage_camera_viewAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->SetPhaseMask(40, false);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                //  player->CastWithDelay(500, me, SPELL_RIDE_VEHICLE);

            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId*/, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_SABOTAGE_CAMERA_1, 1000);

                if (auto target = me->FindNearestCreature(NPC_CHANNEL_TARGET, 80.f))
                {
                    _targetBunnyGUID = target->GetGUID();
                    me->CastSpell(target, SPELL_CAMERA_CHANNEL);
                }
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (auto dopple = player->SummonCreature(NPC_DOPPLEGANGER, -4086.66f, -2339.53f, 126.12f, 2.041f, TEMPSUMMON_TIMED_DESPAWN, 30000))
                    {
                        dopple->setActive(true);
                        dopple->SetReactState(REACT_PASSIVE);
                        dopple->SetPhaseMask(8, false);
                        player->CastSpell(player, SPELL_SET_LOOK);
                    }
                }
                break;
            case 2:
            {
                auto explo = me->SelectNearbyUnits(NPC_EXPLODE_BUNNY, 110.0f);
                for (auto itr = explo.begin(); itr != explo.end(); ++itr)
                    if (auto explode = (*itr)->ToCreature())
                        explode->AI()->SetData(2, 2);
            }
            break;
            case 3:
                _events.ScheduleEvent(EVENT_SABOTAGE_CAMERA_2, 10000);
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SABOTAGE_CAMERA_1:
                    me->GetMotionMaster()->MoveSmoothPath(SabotageCameraPath, SabotageCameraPathSize);
                    break;
                case EVENT_SABOTAGE_CAMERA_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        // player->ExitVehicle();

                        if (auto veh = me->GetVehicleKit())
                        {
                            veh->RemovePassenger(player);
                            player->NearTeleportTo(-3864.56f, -2277.01f, 91.667f, 4.57f);
                        }

                        me->DespawnOrUnsummon();
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
        uint64 _targetBunnyGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sabotage_camera_viewAI(creature);
    }
};

void AddSC_southern_barrens()
{
    new go_coiled_rope_top_down();
    new npc_coiled_rope_controller();
    new npc_grasping_overgrowth();
    new go_bristleback_cage();
    new npc_sabersnout();
    new npc_bristleback_onlooker();
    new npc_naralex_scar();
    new npc_a_line_in_the_dirt_controller();
    new npc_bloodhilts_riding_wolf();
    new npc_silithid_swarmer_controller();
    new npc_changing_of_the_gardul_controller();
    new npc_changing_of_the_gardul_carry_bunny();
    new npc_the_butcher_of_taurajo_controller();
    new spell_tomusas_hook_cross_cast();
    new npc_spindlegears_survey_craft();
    new npc_northwatch_gryphon();
    new npc_northwatch_shore_battery();
    new npc_silencing_rageroar_fight_activator();
    new npc_ambassador_gaines_rescue();
    new npc_barrens_jungle_vine();
    new AreaTrigger_southern_barrens_triumph_messenger();
    new AreaTrigger_southern_barrens_twinbraid_is_up();
    new npc_marleys_final_flight();
    new npc_baeldun_burning_debris();
    new npc_battlescar_flag_controller();
    new npc_sabotage_controller();
    new npc_sabotage_camera_view();
}

