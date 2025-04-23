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

enum DarkshoreDefaultSpells
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235, // my own
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_PING_2 = 98915,
    SPELL_DUMMY_PING_3 = 98916,
    SPELL_DUMMY_PING_4 = 98917,
    SPELL_SANCTUARY_NO_COMBAT = 88467,
    SPELL_ROOT = 42716,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum DarkshoreEvents
{
    EVENT_SENTINEL_1 = 1,
    EVENT_SENTINEL_2,
    EVENT_SENTINEL_3,
    EVENT_SENTINEL_4,
    EVENT_SENTINEL_RESET,
    EVENT_ROBOT_DESPAWN,
    EVENT_VENGEFUL_DESPAWN,
    EVENT_ESCORT_HOLLEE_1,
    EVENT_ESCORT_HOLLEE_2,
    EVENT_ESCORT_HOLLEE_3,
    EVENT_ESCORT_HOLLEE_4,
    EVENT_ESCORT_HOLLEE_5,
    EVENT_ESCORT_HOLLEE_6,
    EVENT_ESCORT_HOLLEE_7,
    EVENT_SOOTHING_TOTEM_1,
    EVENT_SOOTHING_TOTEM_2,
    EVENT_SOOTHING_TOTEM_3,
    EVENT_SOOTHING_TOTEM_4,
    EVENT_THESSERA_CAVE_1,
    EVENT_THESSERA_VORTEX_1,
    EVENT_GROVE_HIPPO_1,
    EVENT_PROSPECTOR_1,
    EVENT_PROSPECTOR_2,
    EVENT_NIGHTSABER_1,
    EVENT_NIGHTSABER_2,
    EVENT_NIGHTSABER_3,
    EVENT_NIGHTSABER_4,
    EVENT_NIGHTSABER_5,
    EVENT_NIGHTSABER_6,
    EVENT_NIGHTSABER_7,
    EVENT_AYNASHA_1,
    EVENT_AYNASHA_2,
    EVENT_AYNASHA_3,
    EVENT_AYNASHA_4,
    EVENT_AYNASHA_5,
    EVENT_AYNASHA_ESCORT_1,
    EVENT_AYNASHA_ESCORT_2,
    EVENT_AYNASHA_ESCORT_3,
    EVENT_AYNASHA_ESCORT_4,
};

// ------------------------

enum SurviveQuest
{
    QUEST_LAST_WAVE_OF_SURVIVORS = 13518,

    NPC_VOLCOR = 33094,
    NPC_WHITECLAW = 33093,
    NPC_SHALDYN = 33095,
    NPC_GERSHALA = 32911,

    NPC_LORDANEL_SENTINEL = 32969,
};

class npc_last_wave_of_survivors : public CreatureScript
{
public:
    npc_last_wave_of_survivors() : CreatureScript("npc_last_wave_of_survivors") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LAST_WAVE_OF_SURVIVORS) == QUEST_STATUS_INCOMPLETE)
        {
            switch (creature->GetEntry())
            {
            case NPC_VOLCOR:
                player->CastSpell(creature, SPELL_DUMMY_PING);
                player->KilledMonsterCredit(creature->GetEntry());
                break;
            case NPC_WHITECLAW:
                player->CastSpell(creature, SPELL_DUMMY_PING_2);
                player->KilledMonsterCredit(creature->GetEntry());
                break;
            case NPC_SHALDYN:
                player->CastSpell(creature, SPELL_DUMMY_PING_4);
                player->KilledMonsterCredit(creature->GetEntry());
                break;
            case NPC_GERSHALA:
                player->CastSpell(creature, SPELL_DUMMY_PING_3);
                player->KilledMonsterCredit(creature->GetEntry());
                break;
            default:
                break;
            }
        }
        return true;
    }

    struct npc_last_wave_of_survivorsAI : public ScriptedAI
    {
        npc_last_wave_of_survivorsAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (!_inProgress)
                if (who->ToPlayer())
                {
                    _playerGUID = who->GetGUID();

                    switch (spellInfo->Id)
                    {
                    case SPELL_DUMMY_PING:
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            _inProgress = true;
                            if (auto sentinel = me->SummonCreature(NPC_LORDANEL_SENTINEL, 7434.94f, 89.5416f, 1.9454f, 1.23f, TEMPSUMMON_TIMED_DESPAWN, 60000))
                            {
                                sentinel->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                                _sentinelGUID = sentinel->GetGUID();
                                sentinel->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                //sentinel->Mount();
                                sentinel->SetWalk(true);
                                sentinel->GetMotionMaster()->MovePoint(1, 7439.285f, 102.818f, 0.496f);
                                _events.ScheduleEvent(EVENT_SENTINEL_1, 5000);
                            }
                        }
                        break;
                    case SPELL_DUMMY_PING_2:
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            _inProgress = true;
                            if (auto sentinel = me->SummonCreature(NPC_LORDANEL_SENTINEL, 7352.65f, 158.228f, 8.526f, 5.25f, TEMPSUMMON_TIMED_DESPAWN, 60000))
                            {
                                sentinel->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                                _sentinelGUID = sentinel->GetGUID();
                                sentinel->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                //sentinel->Mount();
                                sentinel->SetWalk(false);
                                sentinel->GetMotionMaster()->MovePoint(1, 7364.08f, 136.827f, 11.35f);
                                _events.ScheduleEvent(EVENT_SENTINEL_2, 4000);
                            }
                        }
                        break;
                    case SPELL_DUMMY_PING_3:
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            _inProgress = true;
                            if (auto sentinel = me->SummonCreature(NPC_LORDANEL_SENTINEL, 7429.82f, 159.077f, 0.2754f, 0.16f, TEMPSUMMON_TIMED_DESPAWN, 60000))
                            {
                                sentinel->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                                _sentinelGUID = sentinel->GetGUID();
                                sentinel->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                //sentinel->Mount();
                                sentinel->SetWalk(false);
                                sentinel->GetMotionMaster()->MovePoint(1, 7453.746f, 163.991f, 2.4033f);
                                _events.ScheduleEvent(EVENT_SENTINEL_3, 4000);
                            }
                        }
                        break;
                    case SPELL_DUMMY_PING_4:
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            _inProgress = true;
                            if (auto sentinel = me->SummonCreature(NPC_LORDANEL_SENTINEL, 7286.927f, 265.107f, -1.77f, 5.07266f, TEMPSUMMON_TIMED_DESPAWN, 60000))
                            {
                                sentinel->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                                _sentinelGUID = sentinel->GetGUID();
                                sentinel->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                //sentinel->Mount();
                                sentinel->SetWalk(false);
                                sentinel->GetMotionMaster()->MovePoint(1, 7294.997f, 244.214f, 1.23f);
                                _events.ScheduleEvent(EVENT_SENTINEL_4, 5000);
                            }
                        }
                        break;
                    default:
                        break;

                    }
                }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                if (eventId == EVENT_SENTINEL_RESET)
                {
                    _inProgress = false;
                    _sentinelGUID = 0;
                    _playerGUID = 0;
                }

                else if (eventId == EVENT_SENTINEL_1)
                {
                    if (auto sentinel = Unit::GetCreature(*me, _sentinelGUID))
                    {
                        sentinel->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                        sentinel->AI()->Talk(0);
                        sentinel->AI()->TalkWithDelay(5000, 1);
                        sentinel->AI()->TalkWithDelay(10000, 2);
                        sentinel->DespawnOrUnsummon(15000);
                        _events.ScheduleEvent(EVENT_SENTINEL_RESET, 25000);
                    }
                }

                else if (eventId == EVENT_SENTINEL_2)
                {
                    if (auto sentinel = Unit::GetCreature(*me, _sentinelGUID))
                    {
                        sentinel->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                        sentinel->AI()->Talk(3);
                        sentinel->AI()->TalkWithDelay(5000, 4);
                        sentinel->DespawnOrUnsummon(11000);
                        _events.ScheduleEvent(EVENT_SENTINEL_RESET, 20000);
                    }
                }

                else if (eventId == EVENT_SENTINEL_3)
                {
                    if (auto sentinel = Unit::GetCreature(*me, _sentinelGUID))
                    {
                        sentinel->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                        sentinel->AI()->Talk(5);
                        sentinel->AI()->TalkWithDelay(5000, 6);
                        sentinel->DespawnOrUnsummon(10000);
                        _events.ScheduleEvent(EVENT_SENTINEL_RESET, 20000);
                    }
                }

                else if (eventId == EVENT_SENTINEL_4)
                {
                    if (auto sentinel = Unit::GetCreature(*me, _sentinelGUID))
                    {
                        sentinel->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                        sentinel->AI()->Talk(7);
                        sentinel->AI()->TalkWithDelay(4000, 8);
                        sentinel->DespawnOrUnsummon(10000);
                        _events.ScheduleEvent(EVENT_SENTINEL_RESET, 20000);
                    }
                }
            }
        }
    private:
        uint32 _checkTimer = 1000;
        EventMap _events;
        bool _inProgress;
        uint64 _playerGUID;
        uint64 _sentinelGUID;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_last_wave_of_survivorsAI(creature);
    }
};

class go_the_final_flame_of_bashalaran : public GameObjectScript
{
public:
    go_the_final_flame_of_bashalaran() : GameObjectScript("go_the_final_flame_of_bashalaran") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(13562) == QUEST_STATUS_INCOMPLETE) // quest=13562/the-final-flame-of-bashalaran
        {
            if (auto writhing = player->SummonCreature(33180, 6741.375000f, 25.167830f, 44.261974f, 1.10492f, TEMPSUMMON_TIMED_DESPAWN, 45000))
                if (auto cursed = player->SummonCreature(33179, 6736.222168f, 30.065144f, 45.586834f, 1.10492f, TEMPSUMMON_TIMED_DESPAWN, 45000))
                {
                    writhing->AI()->AttackStart(player);
                    cursed->AI()->AttackStart(player);
                    cursed->AI()->TalkWithDelay(1000, 0);
                    player->KilledMonsterCredit(3650);
                    player->AreaExploredOrEventHappens(13562);
                }
        }
        return true;
    }
};

enum RitualBuffs
{
    QUEST_SPIRIT_OF_THE_MOONSTALKER = 13568,
    QUEST_SPIRIT_OF_THE_THISTLE_BEAR = 13597,
    QUEST_SPIRIT_OF_THE_STAG = 13567,

    QUEST_THE_RITUAL_BOND = 13567,

    SPELL_BUFF_MOONSTALKER = 64340,
    SPELL_BUFF_BEAR = 64329,
    SPELL_BUFF_STAG = 64341,
};

class playerscript_darkshore_post_ritual_bond_buffs : public PlayerScript
{
public:
    playerscript_darkshore_post_ritual_bond_buffs() : PlayerScript("playerscript_darkshore_post_ritual_bond_buffs") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
    {
        if (newZone == 148) // Darkshore
        {
            if (player->GetQuestStatus(QUEST_THE_RITUAL_BOND) == QUEST_STATUS_REWARDED)
            {
                if (player->GetQuestStatus(QUEST_SPIRIT_OF_THE_MOONSTALKER) == QUEST_STATUS_REWARDED)
                {
                    if (!player->GetAura(SPELL_BUFF_MOONSTALKER))
                        player->CastSpell(player, SPELL_BUFF_MOONSTALKER);
                }
                else if (player->GetQuestStatus(QUEST_SPIRIT_OF_THE_THISTLE_BEAR) == QUEST_STATUS_REWARDED)
                {
                    if (!player->GetAura(SPELL_BUFF_BEAR))
                        player->CastSpell(player, SPELL_BUFF_BEAR);
                }
                else if (player->GetQuestStatus(QUEST_SPIRIT_OF_THE_STAG) == QUEST_STATUS_REWARDED)
                {
                    if (!player->GetAura(SPELL_BUFF_STAG))
                        player->CastSpell(player, SPELL_BUFF_STAG);
                }
                else
                    return;
            }
        }
        else
            return;
    }
};

enum MurlocBomb
{
    QUEST_OCEAN_NOT_SO_DEEP = 13560,

    SPELL_PULSE = 35151,
    AREA_SILVER_DAWNING = 5375,
    SPELL_AURA_NO_COMBAT = 151328,
};

class npc_possessed_murloc_bomb : public CreatureScript
{
public:
    npc_possessed_murloc_bomb() : CreatureScript("npc_possessed_murloc_bomb") { }

    struct npc_possessed_murloc_bombAI : public VehicleAI
    {
        npc_possessed_murloc_bombAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PULSE)
            {
                ++_sayTextCounter;

                if (_sayTextCounter == 1)
                {
                    me->AI()->Talk(0);
                }

                else if (_sayTextCounter == 4)
                {
                    me->AI()->Talk(1);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->CastSpell(player, SPELL_AURA_NO_COMBAT);
                }

                else if (_sayTextCounter == 5)
                {
                    _sayTextCounter = 0;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->ExitVehicle();
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (_isInCorrectArea)
                {
                    if (me->GetAreaId() != AREA_SILVER_DAWNING)
                    {
                        _isInCorrectArea = false;
                        me->AI()->Talk(2, _playerGUID);
                        _events.ScheduleEvent(EVENT_ROBOT_DESPAWN, 10000);
                    }
                }
                else
                {
                    if (me->GetAreaId() == AREA_SILVER_DAWNING)
                    {
                        _events.CancelEvent(EVENT_ROBOT_DESPAWN);
                        _isInCorrectArea = true;
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_OCEAN_NOT_SO_DEEP) == QUEST_STATUS_NONE)
                    {
                        me->DespawnOrUnsummon();
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ROBOT_DESPAWN:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        me->DespawnOrUnsummon(100);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isInCorrectArea = false;
        uint8 _sayTextCounter = 0;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_possessed_murloc_bombAI(creature);
    }
};

enum QuestTheAncientsIre
{
    AREA_SHATTERSPEAR_VALE          = 4664,
    QUEST_THE_ANCIENTS_IRE          = 13514,
    NPC_VENGEFUL_PROTECTOR          = 43742,
    NPC_VENGEFUL_PROTECTOR_VEH      = 32851,
    NPC_BUILD_STRUCTURES_CREDIT     = 33913,
};

class npc_vengeful_protector_ancient_vehicle : public CreatureScript
{
public:
    npc_vengeful_protector_ancient_vehicle() : CreatureScript("npc_vengeful_protector_ancient_vehicle") { }

    struct npc_vengeful_protector_ancient_vehicleAI : public VehicleAI
    {
        npc_vengeful_protector_ancient_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->KilledMonsterCredit(NPC_BUILD_STRUCTURES_CREDIT);
                }
            }
        }

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void JustDied(Unit* killer)
        {
            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (_isInCorrectArea)
                {
                    if (me->GetAreaId() != AREA_SHATTERSPEAR_VALE)
                    {
                        _isInCorrectArea = false;
                        me->AI()->Talk(0, _playerGUID);
                        _events.ScheduleEvent(EVENT_VENGEFUL_DESPAWN, 10000);
                    }
                }
                else
                {
                    if (me->GetAreaId() == AREA_SHATTERSPEAR_VALE)
                    {
                        _events.CancelEvent(EVENT_VENGEFUL_DESPAWN);
                        _isInCorrectArea = true;
                    }
                }

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_THE_ANCIENTS_IRE) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_THE_ANCIENTS_IRE) == QUEST_STATUS_REWARDED)
                    {
                        me->DespawnOrUnsummon();
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VENGEFUL_DESPAWN:
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon(100);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isInCorrectArea = false;
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vengeful_protector_ancient_vehicleAI(creature);
    }
};

enum CoaxingTheSpirits
{
    NPC_SPIRIT_CAYLAIS = 33038,
    NPC_SPIRIT_TALDAN = 33036,
    NPC_SPIRIT_ELISSA = 33034,
    NPC_SPIRIT_THUNDRIS = 33002,

    QUEST_COAXING_THE_SPIRITS = 13547,
};

class npc_coaxing_the_spirits_companions : public CreatureScript
{
public:
    npc_coaxing_the_spirits_companions() : CreatureScript("npc_coaxing_the_spirits_companions") { }

    struct npc_coaxing_the_spirits_companionsAI : public ScriptedAI
    {
        npc_coaxing_the_spirits_companionsAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void UpdateAI(uint32 const diff) override
        {
            DoMeleeAttackIfReady();

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetEntry() == NPC_SPIRIT_CAYLAIS)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 148)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_NONE)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.5f * M_PI);
                            _isFollowing = true;
                        }
                    }
                    else if (me->GetEntry() == NPC_SPIRIT_TALDAN)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 148)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_NONE)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.8f * M_PI);
                            _isFollowing = true;
                        }
                    }

                    else if (me->GetEntry() == NPC_SPIRIT_ELISSA)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 148)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_NONE)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.1f * M_PI);
                            _isFollowing = true;
                        }
                    }

                    else if (me->GetEntry() == NPC_SPIRIT_THUNDRIS)
                    {
                        if (!me->GetOwner()->isAlive() || me->GetZoneId() != 148)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_REWARDED || player->GetQuestStatus(QUEST_COAXING_THE_SPIRITS) == QUEST_STATUS_NONE)
                        {
                            me->DespawnOrUnsummon();
                        }

                        if (!_isFollowing)
                        {
                            me->GetMotionMaster()->MoveFollow(player, 0.9f, 1.4f * M_PI);
                            _isFollowing = true;
                        }
                    }

                }
            }
            else _checkTimer -= diff;

        }

    private:
        bool _isFollowing = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coaxing_the_spirits_companionsAI(creature);
    }
};

enum QuestHollee
{
    NPC_HOLLEE = 33232,
    NPC_STEELHAND = 33231,
    SPELL_INVISIBILITY_DETECT_1 = 49416,
    SPELL_INVISIBILITY_1 = 49414,
    QUEST_THE_LAST_REFUGEE = 13605,
};

// quest = 13605 / the - last - refugee
class npc_hollee_escort : public CreatureScript
{
public:
    npc_hollee_escort() : CreatureScript("npc_hollee_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_LAST_REFUGEE)
        {
            //player->SummonCreature(NPC_HOLLEE, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 30000);
            player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
            player->CastSpell(creature, SPELL_DUMMY_PING);
        }

        return true;
    }

    struct npc_hollee_escortAI : public npc_escortAI
    {
        npc_hollee_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_1, 2000);
                }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 9:
                me->SetFacingTo(6.243f);
                me->AI()->Talk(2, _playerGUID);
                SetRun(true);
                break;
            case 11:
                me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                me->AI()->TalkWithDelay(1500, 3, _playerGUID);
                _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_3, 9000);
                break;
            case 33:
                me->AI()->TalkWithDelay(1000, 5, _playerGUID);
                me->AI()->TalkWithDelay(8000, 6, _playerGUID);
                break;
            case 58:
                me->SetFacingTo(3.99f);
                _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_4, 1000);
                break;
            case 66:
                if (auto steelhand = me->FindNearestCreature(NPC_STEELHAND, 25.f))
                {
                    steelhand->AI()->Talk(0);
                }
                else
                    me->DespawnOrUnsummon();
                break;
            case 68:
                _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_6, 1000);
                break;
            default:
                break;
            }
        }
        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);
            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ESCORT_HOLLEE_1:
                    me->AI()->Talk(0, _playerGUID);
                    _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_2, 4500);
                    break;
                case EVENT_ESCORT_HOLLEE_2:
                    _started = true;
                    me->AI()->Talk(1, _playerGUID);
                    Start(true, false, _playerGUID);
                    break;
                case EVENT_ESCORT_HOLLEE_3:
                    me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                    me->AI()->Talk(4, _playerGUID);
                    break;
                case EVENT_ESCORT_HOLLEE_4:
                    me->AI()->Talk(7, _playerGUID);
                    _events.ScheduleEvent(EVENT_ESCORT_HOLLEE_5, 3000);
                    break;
                case EVENT_ESCORT_HOLLEE_5:
                {
                    me->SetReactState(REACT_PASSIVE);

                    std::list<Player*> player = me->GetPlayersInRange(80, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_THE_LAST_REFUGEE) == QUEST_STATUS_INCOMPLETE)
                            (*it)->AreaExploredOrEventHappens(QUEST_THE_LAST_REFUGEE);

                    if (auto player = GetPlayerForEscort())
                    {
                        me->SetFacingToObject(player);
                        me->AI()->TalkWithDelay(1000, 8, _playerGUID);
                    }
                    else
                        me->DespawnOrUnsummon();

                    break;
                }

                case EVENT_ESCORT_HOLLEE_6:
                    if (auto steelhand = me->FindNearestCreature(NPC_STEELHAND, 25.f))
                    {
                        me->AI()->Talk(9);
                        steelhand->AI()->TalkWithDelay(5500, 1);
                        me->AI()->TalkWithDelay(12500, 10);
                        steelhand->AI()->TalkWithDelay(18500, 2);
                        me->AI()->TalkWithDelay(24500, 11);
                        steelhand->AI()->TalkWithDelay(28500, 3);
                        me->DespawnOrUnsummon(35000);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _started = false;
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hollee_escortAI(creature);
    }
};

enum SoothingTotem
{
    NPC_CREDIT_SOOTHING = 34371,
    NPC_RAGER = 34370,
    NPC_REMNANT = 34368,

    QUEST_SOOTHING_THE_ELEMENTS = 13580,
};

const Position spawnPosition[] =
{
    {5559.174f, 474.8648f, 26.1f, 4.49720f},
    {5547.228f, 478.6660f, 26.1f, 4.32441f},
    {5536.684f, 477.9653f, 27.99f, 4.32441f},
};

class npc_energized_soothing_totem : public CreatureScript
{
public:
    npc_energized_soothing_totem() : CreatureScript("npc_energized_soothing_totem") { }

    struct npc_energized_soothing_totemAI : public ScriptedAI
    {
        npc_energized_soothing_totemAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(500, me, SPELL_ROOT);
            me->SetRooted(true);
            me->AI()->TalkWithDelay(1500, 0);

            _events.ScheduleEvent(EVENT_SOOTHING_TOTEM_1, 4000);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
            else
                return;
        }

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();
            me->AI()->Talk(2);

            std::list<Player*> character = me->GetPlayersInRange(40, true);
            for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_SOOTHING_THE_ELEMENTS) == QUEST_STATUS_INCOMPLETE)
                    (*it)->FailQuest(QUEST_SOOTHING_THE_ELEMENTS);  //(*it)->KilledMonsterCredit(46002); // credit
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                ++_killCount;

                switch (_killCount)
                {
                case 1:
                    _events.ScheduleEvent(EVENT_SOOTHING_TOTEM_2, 5000);
                    break;
                case 3:
                    _events.ScheduleEvent(EVENT_SOOTHING_TOTEM_2, 7000);
                    break;
                case 5:
                    _events.ScheduleEvent(EVENT_SOOTHING_TOTEM_3, 3000);
                    break;
                case 6:
                    _killCount = 0;
                    _events.ScheduleEvent(EVENT_SOOTHING_TOTEM_4, 4000);
                    break;
                default:
                    break;
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
                        me->DespawnOrUnsummon();

                    if (player->GetQuestStatus(QUEST_SOOTHING_THE_ELEMENTS) == QUEST_STATUS_NONE)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SOOTHING_TOTEM_1:
                    if (auto enemy1 = me->SummonCreature(NPC_REMNANT, spawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy1);
                        enemy1->AI()->AttackStart(me);
                        enemy1->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                    }
                    break;
                case EVENT_SOOTHING_TOTEM_2:

                    if (auto enemy2 = me->SummonCreature(NPC_REMNANT, spawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto enemy3 = me->SummonCreature(NPC_REMNANT, spawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _summons.Summon(enemy2);
                            enemy2->AI()->AttackStart(me);
                            enemy2->SetHomePosition(me->GetPositionX() + 3, me->GetPositionY() - 2, me->GetPositionZ(), me->GetOrientation());
                            _summons.Summon(enemy3);
                            enemy3->AI()->AttackStart(me);
                            enemy3->SetHomePosition(me->GetPositionX() - 3, me->GetPositionY() + 3, me->GetPositionZ(), me->GetOrientation());
                        }
                    break;
                case EVENT_SOOTHING_TOTEM_3:
                    if (auto enemy4 = me->SummonCreature(NPC_RAGER, spawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy4);
                        enemy4->AI()->AttackStart(me);
                        enemy4->SetHomePosition(me->GetPositionX() + 3, me->GetPositionY() + 3, me->GetPositionZ(), me->GetOrientation());
                    }
                    break;
                case EVENT_SOOTHING_TOTEM_4:
                {
                    me->AI()->Talk(1);
                    me->DespawnOrUnsummon(5000);

                    std::list<Player*> player = me->GetPlayersInRange(40, true);
                    for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_SOOTHING_THE_ELEMENTS) == QUEST_STATUS_INCOMPLETE)
                            (*it)->KilledMonsterCredit(NPC_CREDIT_SOOTHING);

                    break;
                }
                default:
                    break;
                }
            }
        }

    private:
        uint16 _checkTimer = 2000;
        uint8 _killCount = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_energized_soothing_totemAI(creature);
    }
};

uint32 const ThesseraCavePathSize = 33;
const G3D::Vector3 ThesseraCavePath[ThesseraCavePathSize] =
{
    { 5771.03f, -167.33f, 19.72f, } ,
    { 5789.01f, -165.35f, 18.15f, } ,
    { 5801.42f, -148.14f, 10.863f, } ,
    { 5811.48f, -154.68f, 6.6987f, } ,
    { 5816.02f, -166.03f, 4.2719f, } ,
    { 5832.87f, -185.10f, 3.2217f, } ,
    { 5841.07f, -196.23f, 4.6650f, } ,
    { 5845.14f, -208.23f, 5.9648f, } ,
    { 5844.43f, -220.83f, 6.4331f, } ,
    { 5828.01f, -225.11f, 8.6220f, } ,
    { 5819.59f, -219.28f, 9.0675f, } ,
    { 5812.59f, -203.64f, 10.666f, } ,
    { 5815.07f, -189.41f, 13.183f, } ,
    { 5825.48f, -183.64f, 17.317f, } ,
    { 5839.36f, -163.27f, 22.682f, } ,
    { 5834.29f, -144.51f, 24.009f, } ,
    { 5825.59f, -128.16f, 24.000f, } ,
    { 5811.64f, -108.46f, 24.122f, } ,
    { 5808.95f, -102.51f, 29.917f, } ,
    { 5810.73f, -93.55f, 29.9024f, } ,
    { 5816.84f, -72.82f, 31.2335f, } ,
    { 5815.65f, -57.11f, 28.9574f, } ,
    { 5811.72f, -45.24f, 27.4920f, } ,
    { 5803.77f, -20.82f, 29.6897f, } ,
    { 5791.92f, -4.65f,  40.6106f, } ,
    { 5772.70f,  14.40f,  45.4115f, } ,
    { 5762.64f,  44.08f,  65.5137f, } ,
    { 5763.94f,  83.00f,  83.7097f, } ,
    { 5784.44f,  110.12f, 83.3505f, } ,
    { 5815.51f,  127.61f, 84.4188f, } ,
    { 5848.47f,  150.82f, 90.8778f, } ,
    { 5875.26f,  182.61f, 73.9002f, } ,
    { 5904.31f,  197.61f, 66.4306f, } , // 32
};

class npc_thessera_cave_vehicle : public CreatureScript
{
public:
    npc_thessera_cave_vehicle() : CreatureScript("npc_thessera_cave_vehicle") { }

    struct npc_thessera_cave_vehicleAI : public VehicleAI
    {
        npc_thessera_cave_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_THESSERA_CAVE_1, 2000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 32:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon(1000);
                }
                else
                    me->DespawnOrUnsummon();
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
                case EVENT_THESSERA_CAVE_1:
                    me->GetMotionMaster()->MoveSmoothPath(ThesseraCavePath, ThesseraCavePathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thessera_cave_vehicleAI(creature);
    }
};

enum ThesseraVortex
{
    SPELL_SUMMON_THESSERA_VORTEX_VEHICLE = 65009,
    SPELL_THESSERA_LAND = 65579,

    QUEST_THE_EYE_OF_ALL_STORMS = 13588,
};

class npc_thessera_vortex_spellclick : public CreatureScript
{
public:
    npc_thessera_vortex_spellclick() : CreatureScript("npc_thessera_vortex_spellclick") { }

    struct npc_thessera_vortex_spellclickAI : public ScriptedAI
    {
        npc_thessera_vortex_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    player->CastWithDelay(200, player, SPELL_SUMMON_THESSERA_VORTEX_VEHICLE);
                }
            }
        }
    private:
        uint64 _playerGUID = NULL;
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thessera_vortex_spellclickAI(creature);
    }
};

uint32 const ThesseraVortexPathSize = 12;
const G3D::Vector3 ThesseraVortexPath[ThesseraVortexPathSize] =
{
    { 5936.880f, 187.643f, 74.0f },
    { 5931.687f, 214.718f, 88.0f },
    { 5893.576f, 223.266f, 102.0f },
    { 5876.349f, 181.167f, 116.0f },
    { 5921.890f, 161.727f, 130.0f },
    { 5937.622f, 176.994f, 144.0f },
    { 5956.397f, 167.968f, 158.0f },
    { 5964.458f, 238.920f, 172.0f },
    { 5881.748f, 260.168f, 186.0f },
    { 5845.139f, 162.490f, 200.0f },
    { 5939.625f, 125.253f, 214.0f },
    { 5972.144f, 154.446f, 228.0f }, // 11
};

uint32 const ThesseraVortexPath2Size = 48;
const G3D::Vector3 ThesseraVortexPath2[ThesseraVortexPath2Size] =
{
    { 5956.397f, 167.968f, 235.0f },
    { 5964.458f, 238.920f, 243.0f },
    { 5881.748f, 260.168f, 251.0f },
    { 5845.139f, 162.490f, 259.0f },
    { 5939.625f, 125.253f, 267.0f },
    { 5972.144f, 154.446f, 275.0f },
    { 5956.397f, 167.968f, 283.0f },
    { 5964.458f, 238.920f, 291.0f },
    { 5881.748f, 260.168f, 299.0f },
    { 5845.139f, 162.490f, 307.0f },
    { 5939.625f, 125.253f, 315.0f },
    { 5972.144f, 154.446f, 323.0f },
    { 5956.397f, 167.968f, 331.0f },
    { 5964.458f, 238.920f, 339.0f },
    { 5881.748f, 260.168f, 347.0f },
    { 5845.139f, 162.490f, 355.0f },
    { 5939.625f, 125.253f, 363.0f },
    { 5972.144f, 154.446f, 371.0f },
    { 5956.397f, 167.968f, 379.0f },
    { 5964.458f, 238.920f, 387.0f },
    { 5881.748f, 260.168f, 395.0f },
    { 5845.139f, 162.490f, 403.0f },
    { 5939.625f, 125.253f, 411.0f },
    { 5972.144f, 154.446f, 419.0f },
    { 5956.397f, 167.968f, 419.0f },
    { 5964.458f, 238.920f, 411.0f },
    { 5881.748f, 260.168f, 403.0f },
    { 5845.139f, 162.490f, 395.0f },
    { 5939.625f, 125.253f, 387.0f },
    { 5972.144f, 154.446f, 379.0f },
    { 5956.397f, 167.968f, 371.0f },
    { 5964.458f, 238.920f, 363.0f },
    { 5881.748f, 260.168f, 355.0f },
    { 5845.139f, 162.490f, 347.0f },
    { 5939.625f, 125.253f, 339.0f },
    { 5972.144f, 154.446f, 331.0f },
    { 5956.397f, 167.968f, 323.0f },
    { 5964.458f, 238.920f, 315.0f },
    { 5881.748f, 260.168f, 307.0f },
    { 5845.139f, 162.490f, 299.0f },
    { 5939.625f, 125.253f, 291.0f },
    { 5972.144f, 154.446f, 283.0f },
    { 5956.397f, 167.968f, 275.0f },
    { 5964.458f, 238.920f, 267.0f },
    { 5881.748f, 260.168f, 259.0f },
    { 5845.139f, 162.490f, 251.0f },
    { 5939.625f, 125.253f, 243.0f },
    { 5972.144f, 154.446f, 235.0f }, // 47
};

uint32 const ThesseraVortexPath3Size = 7;
const G3D::Vector3 ThesseraVortexPath3[ThesseraVortexPath3Size] =
{
    { 5897.881f, 206.730f, 104.08f },
    { 5918.354f, 204.551f, 91.358f },
    { 5927.457f, 182.909f, 83.524f },
    { 5909.513f, 174.539f, 82.649f },
    { 5893.033f, 187.562f, 74.288f },
    { 5903.985f, 199.628f, 73.436f },
    { 5924.971f, 199.712f, 63.720f }, // 6
};

class npc_thessera_vortex_vehicle : public CreatureScript
{
public:
    npc_thessera_vortex_vehicle() : CreatureScript("npc_thessera_vortex_vehicle") { }

    struct npc_thessera_vortex_vehicleAI : public VehicleAI
    {
        npc_thessera_vortex_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_THESSERA_VORTEX_1, 2000);
            }
        }

        void GoBack()
        {
            me->SetSpeed(MOVE_RUN, 4.0f);
            me->SetSpeed(MOVE_FLIGHT, 4.0f);
            _started = false;
            _step = 0;
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveSmoothPath(ThesseraVortexPath3, ThesseraVortexPath3Size);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (_started)
            {
                if (spellInfo->Id == SPELL_THESSERA_LAND)
                {
                    GoBack();
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 6:
                if (_step == 0)
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                    me->DespawnOrUnsummon();
                }
                break;
            case 11:
                if (_step == 1)
                {
                    me->GetMotionMaster()->MoveSmoothPath(ThesseraVortexPath2, ThesseraVortexPath2Size);
                    _step++;
                }
                break;
            case 47:
                if (_step == 2)
                {
                    me->GetMotionMaster()->MoveSmoothPath(ThesseraVortexPath2, ThesseraVortexPath2Size);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            //if (_checkTimer <= diff && _started)
            //{
            //    _checkTimer = 1000;
            //
            //     if (auto player = Unit::GetPlayer(*me, _playerGUID))
            //     {
            //         if (player->GetQuestStatus(QUEST_THE_EYE_OF_ALL_STORMS) == QUEST_STATUS_COMPLETE)
            //             GoBack();
            //     }
            //}
            //_checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_THESSERA_VORTEX_1:
                    _step = 1;
                    _started = true;
                    me->GetMotionMaster()->MoveSmoothPath(ThesseraVortexPath, ThesseraVortexPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _started = false;
        uint8 _step = 0;
        uint64 _playerGUID = 0;
        // uint32 _checkTimer = 1000;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thessera_vortex_vehicleAI(creature);
    }
};

const Position CirclePosWisp[] =
{
    { 4365.666f, 442.69f, 62.0f },    // 0
    { 5157.224f, 70.88f, 56.0f },     // 1
    { 5050.267f, 348.063f, 42.6f },   // 2
    { 4292.69f, 278.519f, 62.229f },  // 3
    { 5199.65f, 328.999f, 12.209f },  // 4
    { 5093.39f, 385.935f, 42.55f },   // 5
    { 5377.15f, 410.201f, 8.3264f },  // 6
    { 4317.55f, 524.455f, 68.179f },  // 7
    { 4886.55f, 380.182f, 29.74f },   // 8
    { 5006.43f, 320.251f, 36.753f },  // 9
    { 4727.83f, 280.046f, 52.560f },  // 10
    { 4484.802f, 197.081f, 61.83f },  // 11
    { 5441.109f, 207.090f, 16.10f },  // 12
    { 5174.961f, 224.41f, 37.48f },   // 13
    { 4614.075f, 239.18f, 56.46f },   // 14
    { 4814.155f, 297.228f, 50.87f },  // 15
    { 4806.228f, 430.488f, 20.72f },  // 16
    { 4838.185f, 159.1f, 56.67f },    // 17
    { 4673.489f, 230.02f, 62.64f },   // 18
    { 5331.583f, 238.702f, 11.52f },  // 19
    { 4772.157f, 319.322f, 52.31f },  // 20
    { 4588.326f, 49.403f, 76.80f },   // 21
    { 4414.602f, 300.917f, 62.29f },  // 22
    { 4380.603f, 86.191f, 63.93f },   // 23
};

class npc_darkshore_wisp_spellclick : public CreatureScript
{
public:
    npc_darkshore_wisp_spellclick() : CreatureScript("npc_darkshore_wisp_spellclick") { }

    struct npc_darkshore_wisp_spellclickAI : public VehicleAI
    {
        npc_darkshore_wisp_spellclickAI(Creature* creature) : VehicleAI(creature) {}

        void InitializeAI() override
        {
            JustRespawned();
        }

        void JustRespawned() override
        {
            me->setActive(true);

            switch (me->GetGUIDLow())
            {
            case 168572:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[0], 5.0f, true, 8);
                break;
            case 168573:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[1], 5.0f, true, 8);
                break;
            case 168574:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[2], 8.0f, true, 8);
                break;
            case 168575:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[3], 5.0f, true, 8);
                break;
            case 168576:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[4], 5.0f, true, 8);
                break;
            case 168577:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[5], 7.0f, true, 8);
                break;
            case 168578:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[6], 5.5f, true, 8);
                break;
            case 168579:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[7], 6.0f, true, 8);
                break;
            case 168580:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[8], 5.0f, true, 8);
                break;
            case 168581:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[9], 7.0f, true, 8);
                break;
            case 168582:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[10], 5.0f, true, 8);
                break;
            case 168583:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[11], 10.0f, true, 8);
                break;
            case 168584:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[12], 9.0f, true, 8);
                break;
            case 168585:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[13], 6.0f, true, 8);
                break;
            case 168586:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[14], 6.0f, true, 8);
                break;
            case 168587:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[15], 4.0f, true, 8);
                break;
            case 168588:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[16], 7.0f, true, 8);
                break;
            case 168589:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[17], 6.0f, true, 8);
                break;
            case 168590:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[18], 8.0f, true, 8);
                break;
            case 168591:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[19], 5.0f, true, 8);
                break;
            case 168592:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[20], 5.0f, true, 8);
                break;
            case 168593:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[21], 6.0f, true, 8);
                break;
            case 168594:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[22], 5.0f, true, 8);
                break;
            case 168595:
                me->GetMotionMaster()->MoveAroundPoint(CirclePosWisp[23], 5.0f, true, 8);
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (!who->ToPlayer()->HasItemOrGemWithIdEquipped(46355, 1))
            {
                if (spellInfo->Id == SPELL_DUMMY_SPELLCLICK)
                {
                    who->CastSpell(who, 65125);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(1000);
                }
            }
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkshore_wisp_spellclickAI(creature);
    }
};

enum InDefenseOfDarkshore
{
    QUEST_IN_DEFENSE_OF_DARKSHORE = 13885,

    NPC_KATHRENA_WINTERWISP_PASSENGER = 34376,
};

uint32 const GroveHipoPath1Size = 86;
const G3D::Vector3 GroveHipoPath1[GroveHipoPath1Size] =
{
    { 4983.082f, 147.904f, 54.03f }, // 0 
    { 4998.785f, 154.791f, 55.37f }, // 1 
    { 5008.601f, 151.440f, 56.12f }, // 2 
    { 5016.255f, 141.010f, 61.09f }, // 3 
    { 5030.154f, 131.867f, 59.19f }, // 4 
    { 5059.205f, 137.100f, 54.90f }, // 5 
    { 5074.855f, 136.886f, 58.72f }, // 6 
    { 5102.326f, 113.588f, 69.01f }, // 7 
    { 5110.620f, 88.3873f, 87.71f }, // 8 
    { 5094.453f, 64.3600f, 97.72f }, // 9 
    { 5045.371f, 49.3578f, 91.24f }, // 10
    { 5018.691f, 64.4421f, 106.8f }, // 11
    { 4996.760f, 111.439f, 104.2f }, // 12
    { 4997.118f, 135.14f, 108.23f }, // 13
    { 5006.433f, 169.51f, 105.19f }, // 14
    { 5032.296f, 209.86f, 110.53f }, // 15
    { 5061.362f, 196.10f, 109.26f }, // 16
    { 5080.187f, 192.62f, 100.93f }, // 17
    { 5107.658f, 208.68f, 85.210f }, // 18
    { 5122.117f, 214.25f, 78.088f }, // 19
    { 5151.975f, 183.19f, 61.492f }, // 20
    { 5171.787f, 143.32f, 55.759f }, // 21
    { 5240.819f, 112.99f, 55.129f }, // 22
    { 5290.197f, 127.15f, 56.697f }, // 23
    { 5348.519f, 158.25f, 51.441f }, // 24
    { 5382.229f, 156.66f, 51.246f }, // 25
    { 5419.743f, 131.63f, 42.334f }, // 26
    { 5472.857f, 126.76f, 40.022f }, // 27
    { 5499.018f, 130.84f, 39.913f }, // 28
    { 5524.263f, 104.89f, 32.294f }, // 29
    { 5570.366f, 96.638f, 30.671f }, // 30
    { 5598.000f, 107.21f, 45.935f }, // 31
    { 5588.744f, 149.62f, 47.852f }, // 32
    { 5612.548f, 194.85f, 62.083f }, // 33
    { 5640.354f, 250.00f, 75.895f }, // 34
    { 5624.089f, 298.779f, 63.97f }, // 35
    { 5592.600f, 335.608f, 52.71f }, // 36
    { 5561.638f, 337.817f, 50.81f }, // 37
    { 5521.916f, 317.779f, 45.89f }, // 38
    { 5491.415f, 324.024f, 44.28f }, // 39
    { 5462.579f, 347.005f, 37.10f }, // 40
    { 5437.134f, 336.904f, 31.27f }, // 41
    { 5409.680f, 328.415f, 29.44f }, // 42
    { 5385.230f, 353.521f, 21.92f }, // 43
    { 5397.919f, 385.146f, 23.90f }, // 44
    { 5410.269f, 428.324f, 23.72f }, // 45
    { 5382.372f, 444.273f, 24.28f }, // 46
    { 5308.930f, 478.474f, 36.03f }, // 47
    { 5258.018f, 374.672f, 33.97f }, // 48
    { 5238.927f, 306.933f, 41.86f }, // 49
    { 5178.771f, 305.051f, 49.13f }, // 50
    { 5098.464f, 307.319f, 43.84f }, // 51
    { 5052.391f, 319.777f, 50.32f }, // 52
    { 5025.717f, 347.698f, 46.53f }, // 53
    { 5026.339f, 375.923f, 44.18f }, // 54
    { 5023.440f, 413.076f, 39.35f }, // 55
    { 5016.427f, 451.801f, 33.18f }, // 56
    { 4991.187f, 456.650f, 32.72f }, // 57
    { 4972.034f, 438.156f, 36.36f }, // 58
    { 4917.539f, 434.470f, 41.54f }, // 59
    { 4873.733f, 431.003f, 44.19f }, // 60
    { 4867.322f, 418.497f, 56.18f }, // 61
    { 4805.276f, 394.127f, 72.29f }, // 62
    { 4748.263f, 357.546f, 70.85f }, // 63
    { 4745.722f, 313.357f, 80.13f }, // 64
    { 4741.031f, 259.583f, 68.07f }, // 65
    { 4691.104f, 249.569f, 71.48f }, // 66
    { 4654.592f, 298.279f, 75.80f }, // 67
    { 4640.235f, 343.723f, 93.64f }, // 68
    { 4622.934f, 421.305f, 94.50f }, // 69
    { 4583.104f, 474.086f, 93.55f }, // 70
    { 4494.966f, 464.864f, 89.91f }, // 71
    { 4461.783f, 404.807f, 89.07f }, // 72
    { 4474.378f, 364.755f, 104.3f }, // 73
    { 4534.314f, 312.610f, 101.5f }, // 74
    { 4629.545f, 275.453f, 113.1f }, // 75
    { 4763.200f, 219.459f, 104.1f }, // 76
    { 4802.652f, 155.985f, 85.80f }, // 77
    { 4862.660f, 101.351f, 99.08f }, // 78
    { 4896.81f, 94.65f, 109.01f },   // 79
    { 4938.95f, 84.62f, 114.26f },   // 80
    { 4983.93f, 96.11f, 107.05f },   // 81
    { 5013.88f, 121.6f, 87.380f },   // 82
    { 5007.40f, 150.1f, 64.509f },   // 83
    { 4997.12f, 156.0f, 58.538f },   // 84
    { 4981.38f, 152.0f, 53.367f },   // 85
};

class npc_grove_hippogryph_vehicle : public CreatureScript
{
public:
    npc_grove_hippogryph_vehicle() : CreatureScript("npc_grove_hippogryph_vehicle") { }

    struct npc_grove_hippogryph_vehicleAI : public VehicleAI
    {
        npc_grove_hippogryph_vehicleAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->EnterVehicle(me, 1);
            }

            if (auto kathrena = me->SummonCreature(NPC_KATHRENA_WINTERWISP_PASSENGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                _kathrenaGUID = kathrena->GetGUID();
                _summons.Summon(kathrena);
                kathrena->EnterVehicle(me, 0);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                player->ExitVehicle();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_GROVE_HIPPO_1, 1000);
            }

            //
            //if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            //{
            //}
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (auto kathrena = Unit::GetCreature(*me, _kathrenaGUID))
            {
                switch (point)
                {
                case 22:
                    kathrena->AI()->TalkWithDelay(1000, 3, _playerGUID);
                    break;
                case 34:
                    kathrena->AI()->TalkWithDelay(1000, 4, _playerGUID);
                    break;
                case 45:
                    kathrena->AI()->TalkWithDelay(1000, 5, _playerGUID);
                    break;
                case 55:
                    kathrena->AI()->TalkWithDelay(1000, 6, _playerGUID);
                    break;
                case 70:
                    kathrena->AI()->TalkWithDelay(1000, 7, _playerGUID);
                    break;
                case 81:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetQuestStatus(QUEST_IN_DEFENSE_OF_DARKSHORE) == QUEST_STATUS_INCOMPLETE)
                        {
                            kathrena->AI()->TalkWithDelay(500, 9, _playerGUID);
                        }
                        else
                            kathrena->AI()->TalkWithDelay(500, 8, _playerGUID);
                    }
                    break;
                case 85:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }
                    break;
                default:
                    break;
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
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }

                    if (player->GetQuestStatus(QUEST_IN_DEFENSE_OF_DARKSHORE) == QUEST_STATUS_NONE)
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_GROVE_HIPPO_1:
                    if (auto kathrena = Unit::GetCreature(*me, _kathrenaGUID))
                    {
                        me->GetMotionMaster()->MoveSmoothPath(GroveHipoPath1, GroveHipoPath1Size);
                        kathrena->AI()->TalkWithDelay(4500, 1, _playerGUID);
                        kathrena->AI()->TalkWithDelay(11000, 2, _playerGUID);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _kathrenaGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grove_hippogryph_vehicleAI(creature);
    }
};

enum QuestProspector
{
    NPC_PROSPECTOR = 34343,
    NPC_GROFF = 34340,

    QUEST_THE_ABSENT_MINDED_PROSPECTOR = 13911,
};

// quest=13911/the-absent-minded-prospector
class npc_prospector_remtravel_escort : public CreatureScript
{
public:
    npc_prospector_remtravel_escort() : CreatureScript("npc_prospector_remtravel_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_ABSENT_MINDED_PROSPECTOR)
        {
            player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
            player->CastSpell(creature, SPELL_DUMMY_PING);
        }

        return true;
    }

    struct npc_prospector_remtravel_escortAI : public npc_escortAI
    {
        npc_prospector_remtravel_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);

                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_PROSPECTOR_1, 1000);
                }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 9:
                me->AI()->Talk(2, _playerGUID);
                break;
            case 22:
                me->AI()->Talk(3, _playerGUID);
                SetRun(true);
                break;
            case 33:
                me->AI()->Talk(4, _playerGUID);
                SetRun(false);
                break;
            case 35:
            {
                std::list<Player*> player = me->GetPlayersInRange(50, true);
                for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_THE_ABSENT_MINDED_PROSPECTOR) == QUEST_STATUS_INCOMPLETE)
                        (*it)->AreaExploredOrEventHappens(QUEST_THE_ABSENT_MINDED_PROSPECTOR);

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetFacingTo(5.7f);
                me->AI()->Talk(5);

                if (auto groff = me->FindNearestCreature(NPC_GROFF, 25.f))
                    groff->AI()->TalkWithDelay(6000, 0);
                break;
            }
            case 36:
                me->AI()->Talk(6);
                break;
            case 37:
                me->SetFacingTo(4.62f);
                me->AI()->Talk(7);
                me->DespawnOrUnsummon(6000);
                break;
            default:
                break;
            }
        }
        void UpdateAI(const uint32 uiDiff)
        {
            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);
            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_PROSPECTOR_1:
                    me->AI()->Talk(0, _playerGUID);
                    _events.ScheduleEvent(EVENT_PROSPECTOR_2, 6000);
                    break;
                case EVENT_PROSPECTOR_2:
                    me->AI()->Talk(1, _playerGUID);
                    Start(true, false, _playerGUID);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _started = false;
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prospector_remtravel_escortAI(creature);
    }
};

enum OfferingToAzshara
{
    QUEST_THE_OFFERING_TO_AZSHARA = 13900,
    QUEST_THE_BATTLE_OF_DARKSHORE = 13897,

    NPC_DARKSCALE_PRIESTESS = 34415,
    NPC_MALFURION = 34422,
    NPC_QUEEEN_AZSHARA = 34416,
};

const Position SummonQuestNpcsPos[] =
{
  { 4587.04f, 890.86f, 41.40f, 6.27f },
  { 4595.28f, 901.76f, 41.69f, 4.87f },
  { 4609.01f, 890.59f, 38.44f, 3.13f },
  { 4600.50f, 879.76f, 38.06f, 1.77f },
  { 4598.52f, 890.88f, 39.87f, 0.96f }, // queen
  { 4572.33f, 916.007f, 44.244f, 5.506f }, // malf
};

class npc_offering_to_azshara_controller : public CreatureScript
{
public:
    npc_offering_to_azshara_controller() : CreatureScript("npc_offering_to_azshara_controller") { }

    struct npc_offering_to_azshara_controllerAI : public ScriptedAI
    {
        npc_offering_to_azshara_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 50.0f))
                if (auto player = who->ToPlayer())
                    if (!_areSpawned)
                    {
                        if (player->GetQuestStatus(QUEST_THE_BATTLE_OF_DARKSHORE) == QUEST_STATUS_REWARDED)
                            return;


                        if (player->GetQuestStatus(QUEST_THE_OFFERING_TO_AZSHARA) == QUEST_STATUS_INCOMPLETE ||
                            player->GetQuestStatus(QUEST_THE_OFFERING_TO_AZSHARA) == QUEST_STATUS_COMPLETE ||
                            player->GetQuestStatus(QUEST_THE_OFFERING_TO_AZSHARA) == QUEST_STATUS_REWARDED)
                        {
                            if (!me->FindNearestCreature(NPC_DARKSCALE_PRIESTESS, 80, true))
                                if (!me->FindNearestCreature(NPC_MALFURION, 80, true))
                                    if (!me->FindNearestCreature(NPC_QUEEEN_AZSHARA, 80, true))
                                    {
                                        me->SummonCreature(NPC_DARKSCALE_PRIESTESS, SummonQuestNpcsPos[0], TEMPSUMMON_MANUAL_DESPAWN, 0);
                                        me->SummonCreature(NPC_DARKSCALE_PRIESTESS, SummonQuestNpcsPos[1], TEMPSUMMON_MANUAL_DESPAWN, 0);
                                        me->SummonCreature(NPC_DARKSCALE_PRIESTESS, SummonQuestNpcsPos[2], TEMPSUMMON_MANUAL_DESPAWN, 0);
                                        me->SummonCreature(NPC_DARKSCALE_PRIESTESS, SummonQuestNpcsPos[3], TEMPSUMMON_MANUAL_DESPAWN, 0);
                                        _areSpawned = true;
                                    }
                        }
                    }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 3000;

                if (_areSpawned)
                {
                    if (!me->FindNearestCreature(NPC_DARKSCALE_PRIESTESS, 80, true))
                    {
                        me->SummonCreature(NPC_QUEEEN_AZSHARA, SummonQuestNpcsPos[4], TEMPSUMMON_MANUAL_DESPAWN, 0);
                        _areSpawned = false;
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        bool _areSpawned = false;
        uint64 _playerGUID = 0;
        uint32 _checkTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_offering_to_azshara_controllerAI(creature);
    }
};

uint32 const NightsaberPath1Size = 26;
const G3D::Vector3 NightsaberPath1[NightsaberPath1Size] =
{
    { 7250.76f, -288.248f, 20.592f },
    { 7251.37f, -278.375f, 21.314f },
    { 7252.01f, -264.610f, 23.732f },
    { 7252.35f, -242.715f, 19.311f },
    { 7246.04f, -225.105f, 17.540f },
    { 7222.09f, -205.384f, 19.345f },
    { 7199.14f, -182.738f, 22.670f },
    { 7181.28f, -160.666f, 21.403f },
    { 7151.14f, -131.895f, 18.038f },
    { 7117.35f,  -99.069f, 13.841f },
    { 7091.68f,  -88.000f, 13.337f },
    { 7055.49f,  -85.627f, 15.733f },
    { 6994.65f,  -83.005f, 18.621f },
    { 6935.04f,  -88.190f, 20.896f },
    { 6878.04f,  -82.836f, 20.908f },
    { 6849.61f,  -69.955f, 19.702f },
    { 6838.02f,  -58.827f, 19.815f },
    { 6820.58f,  -36.032f, 19.184f },
    { 6819.63f,   -5.423f, 16.370f },
    { 6819.60f,   25.682f, 15.343f },
    { 6813.55f,   42.876f, 16.231f },
    { 6802.82f,   47.016f, 18.107f },
    { 6795.60f,   43.055f, 20.059f },
    { 6795.06f,   28.284f, 22.186f },
    { 6794.50f,    7.521f, 25.756f },
    { 6791.51f,   -4.606f, 26.084f },
};

uint32 const NightsaberPath2Size = 20;
const G3D::Vector3 NightsaberPath2[NightsaberPath2Size] =
{
    { 7249.38f, -289.01f, 20.55f },
    { 7249.97f, -310.84f, 19.64f },
    { 7255.83f, -358.56f, 24.51f },
    { 7253.87f, -382.79f, 28.82f },
    { 7253.08f, -412.80f, 26.47f },
    { 7249.71f, -448.20f, 25.31f },
    { 7251.85f, -487.26f, 26.83f },
    { 7248.51f, -529.88f, 27.14f },
    { 7248.69f, -577.16f, 29.00f },
    { 7250.08f, -603.16f, 28.76f },
    { 7260.33f, -636.25f, 26.33f },
    { 7296.36f, -657.06f, 23.59f },
    { 7330.43f, -687.07f, 20.26f },
    { 7344.19f, -704.80f, 17.25f },
    { 7341.76f, -717.59f, 17.76f },
    { 7329.57f, -731.19f, 16.62f },
    { 7314.83f, -749.04f, 16.35f },
    { 7331.13f, -782.09f, 15.79f },
    { 7342.40f, -773.43f, 9.148f },
    { 7356.46f, -776.02f, 7.865f },
};

uint32 const NightsaberPath3Size = 24;
const G3D::Vector3 NightsaberPath3[NightsaberPath3Size] =
{
    { 6792.26f,    9.025f, 25.676f },
    { 6798.64f,   27.847f, 21.736f },
    { 6803.56f,   39.675f, 17.972f },
    { 6813.33f,   38.711f, 15.923f },
    { 6816.97f,   26.208f, 15.279f },
    { 6819.34f,    4.709f, 15.926f },
    { 6817.92f, -25.494f, 17.518f },
    { 6824.92f, -45.468f, 19.850f },
    { 6849.04f, -70.281f, 19.675f },
    { 6865.82f, -81.352f, 20.604f },
    { 6909.79f, -86.681f, 20.853f },
    { 6939.43f, -86.698f, 20.897f },
    { 6998.87f, -82.493f, 18.361f },
    { 7043.54f, -85.826f, 16.059f },
    { 7082.37f, -86.823f, 13.971f },
    { 7110.72f, -93.817f, 13.187f },
    { 7139.01f, -119.198f, 15.902f },
    { 7163.90f, -145.957f, 19.861f },
    { 7193.70f, -176.014f, 22.942f },
    { 7220.57f, -203.163f, 19.585f },
    { 7246.36f, -225.986f, 17.534f },
    { 7252.34f, -241.509f, 18.627f },
    { 7251.95f, -258.461f, 23.561f },
    { 7249.38f, -287.733f, 20.648f },
};

uint32 const NightsaberPath4Size = 21;
const G3D::Vector3 NightsaberPath4[NightsaberPath4Size] =
{
    { 7354.59f, -776.02f, 7.8767f },
    { 7343.56f, -772.91f, 8.3217f },
    { 7336.37f, -770.90f, 13.499f },
    { 7323.98f, -770.65f, 15.341f },
    { 7318.49f, -743.04f, 16.456f },
    { 7337.14f, -724.88f, 17.491f },
    { 7342.40f, -715.54f, 17.530f },
    { 7345.06f, -699.15f, 18.191f },
    { 7333.50f, -687.23f, 20.245f },
    { 7297.40f, -657.83f, 23.539f },
    { 7271.35f, -645.05f, 24.885f },
    { 7255.52f, -631.41f, 26.683f },
    { 7248.82f, -597.57f, 28.805f },
    { 7247.94f, -549.40f, 28.001f },
    { 7249.97f, -511.83f, 27.133f },
    { 7251.12f, -469.73f, 25.698f },
    { 7249.30f, -439.32f, 26.142f },
    { 7253.57f, -400.62f, 25.791f },
    { 7253.70f, -383.62f, 28.858f },
    { 7254.66f, -340.68f, 20.858f },
    { 7248.33f, -297.12f, 20.267f },
};

uint32 const NightsaberPath5Size = 29;
const G3D::Vector3 NightsaberPath5[NightsaberPath5Size] =
{
    { 7347.541f, -773.432f, 8.15098f },
    { 7340.036f, -770.764f, 11.7481f },
    { 7328.525f, -775.856f, 15.4211f },
    { 7318.581f, -826.938f, 22.9047f },
    { 7311.656f, -877.959f, 30.4493f },
    { 7298.427f, -894.368f, 31.5843f },
    { 7276.776f, -912.840f, 33.6577f },
    { 7286.113f, -946.426f, 33.2565f },
    { 7317.980f, -975.446f, 32.2160f },
    { 7356.585f, -981.362f, 32.2535f },
    { 7365.319f, -1018.25f, 32.178f },
    { 7362.683f, -1056.59f, 37.876f },
    { 7362.844f, -1087.11f, 42.174f },
    { 7359.167f, -1121.86f, 52.103f },
    { 7357.424f, -1164.37f, 76.530f },
    { 7354.007f, -1200.62f, 101.19f },
    { 7359.784f, -1225.68f, 114.42f },
    { 7378.544f, -1249.66f, 126.47f },
    { 7392.042f, -1275.94f, 135.15f },
    { 7409.183f, -1309.85f, 145.43f },
    { 7416.612f, -1331.48f, 151.02f },
    { 7416.448f, -1344.09f, 153.68f },
    { 7416.946f, -1361.88f, 152.24f },
    { 7420.013f, -1374.23f, 153.11f },
    { 7414.659f, -1390.59f, 151.90f },
    { 7404.507f, -1403.98f, 153.52f },
    { 7400.028f, -1428.33f, 152.24f },
    { 7398.611f, -1449.91f, 153.02f },
    { 7405.112f, -1455.87f, 152.34f }, // 28
};

uint32 const NightsaberPath6Size = 29;
const G3D::Vector3 NightsaberPath6[NightsaberPath6Size] =
{
    { 7405.112f, -1455.87f, 152.34f },
    { 7398.611f, -1449.91f, 153.02f },
    { 7400.028f, -1428.33f, 152.24f },
    { 7404.507f, -1403.98f, 153.52f },
    { 7414.659f, -1390.59f, 151.90f },
    { 7420.013f, -1374.23f, 153.11f },
    { 7416.946f, -1361.88f, 152.24f },
    { 7416.448f, -1344.09f, 153.68f },
    { 7416.612f, -1331.48f, 151.02f },
    { 7409.183f, -1309.85f, 145.43f },
    { 7392.042f, -1275.94f, 135.15f },
    { 7378.544f, -1249.66f, 126.47f },
    { 7359.784f, -1225.68f, 114.42f },
    { 7354.007f, -1200.62f, 101.19f },
    { 7357.424f, -1164.37f, 76.530f },
    { 7359.167f, -1121.86f, 52.103f },
    { 7362.844f, -1087.11f, 42.174f },
    { 7362.683f, -1056.59f, 37.876f },
    { 7365.319f, -1018.25f, 32.178f },
    { 7356.585f, -981.362f, 32.2535f },
    { 7317.980f, -975.446f, 32.2160f },
    { 7286.113f, -946.426f, 33.2565f },
    { 7276.776f, -912.840f, 33.6577f },
    { 7298.427f, -894.368f, 31.5843f },
    { 7311.656f, -877.959f, 30.4493f },
    { 7318.581f, -826.938f, 22.9047f },
    { 7328.525f, -775.856f, 15.4211f },
    { 7340.036f, -770.764f, 11.7481f },
    { 7347.541f, -773.432f, 8.15098f },
};

uint32 const NightsaberPath7Size = 48;
const G3D::Vector3 NightsaberPath7[NightsaberPath7Size] =
{
    { 7405.112f, -1455.87f, 152.34f },
    { 7398.611f, -1449.91f, 153.02f },
    { 7400.028f, -1428.33f, 152.24f },
    { 7404.507f, -1403.98f, 153.52f },
    { 7414.659f, -1390.59f, 151.90f },
    { 7420.013f, -1374.23f, 153.11f },
    { 7416.946f, -1361.88f, 152.24f },
    { 7416.448f, -1344.09f, 153.68f },
    { 7416.612f, -1331.48f, 151.02f },
    { 7409.183f, -1309.85f, 145.43f },
    { 7392.042f, -1275.94f, 135.15f },
    { 7378.544f, -1249.66f, 126.47f },
    { 7359.784f, -1225.68f, 114.42f },
    { 7354.007f, -1200.62f, 101.19f },
    { 7357.424f, -1164.37f, 76.530f },
    { 7359.167f, -1121.86f, 52.103f },
    { 7362.844f, -1087.11f, 42.174f },
    { 7362.683f, -1056.59f, 37.876f },
    { 7365.319f, -1018.25f, 32.178f },
    { 7356.585f, -981.362f, 32.2535f },
    { 7317.980f, -975.446f, 32.2160f },
    { 7286.113f, -946.426f, 33.2565f },
    { 7276.776f, -912.840f, 33.6577f },
    { 7298.427f, -894.368f, 31.5843f },
    { 7311.656f, -877.959f, 30.4493f },
    { 7318.581f, -826.938f, 22.9047f },
    { 7321.827f, -791.92f, 17.62f },
    { 7317.228f, -762.53f, 15.72f },
    { 7323.808f, -740.84f, 15.97f },
    { 7338.598f, -723.60f, 17.56f },
    { 7342.538f, -715.32f, 17.49f },
    { 7344.215f, -704.13f, 17.32f },
    { 7338.493f, -691.99f, 19.60f },
    { 7322.932f, -680.03f, 21.26f },
    { 7304.341f, -664.64f, 23.50f },
    { 7288.494f, -655.47f, 23.97f },
    { 7262.349f, -641.13f, 26.01f },
    { 7254.561f, -627.22f, 26.90f },
    { 7250.260f, -600.17f, 28.78f },
    { 7247.673f, -544.87f, 27.58f },
    { 7250.404f, -504.43f, 27.26f },
    { 7249.877f, -450.46f, 25.13f },
    { 7250.912f, -410.18f, 26.39f },
    { 7253.956f, -393.32f, 26.97f },
    { 7254.810f, -376.15f, 27.64f },
    { 7255.475f, -349.61f, 22.72f },
    { 7252.389f, -324.56f, 19.05f },
    { 7252.034f, -296.93f, 20.14f },
};

enum NightSaberTransport
{
    NPC_NIGHTSABER_VEHICLE_1 = 33358,
    NPC_NIGHTSABER_VEHICLE_2 = 34051,
    NPC_NIGHTSABER_VEHICLE_3 = 34048,
    NPC_NIGHTSABER_VEHICLE_4 = 34053,
    NPC_NIGHTSABER_VEHICLE_5 = 34311,

    // from vale
    NPC_NIGHTSABER_VEHICLE_6 = 34312,
    NPC_NIGHTSABER_VEHICLE_7 = 34760,
};

class npc_nightsaber_transport_darkshore : public CreatureScript
{
public:
    npc_nightsaber_transport_darkshore() : CreatureScript("npc_nightsaber_transport_darkshore") { }

    struct npc_nightsaber_transport_darkshoreAI : public VehicleAI
    {
        npc_nightsaber_transport_darkshoreAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetRooted(true);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = passenger->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                }

                switch (me->GetEntry())
                {
                case NPC_NIGHTSABER_VEHICLE_1:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_1, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_2:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_2, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_3:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_3, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_4:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_4, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_5:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_5, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_6:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_6, 2000);
                    break;
                case NPC_NIGHTSABER_VEHICLE_7:
                    _events.ScheduleEvent(EVENT_NIGHTSABER_7, 2000);
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_1)
            {
                if (point == 25)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_2)
            {
                if (point == 19)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_3)
            {
                if (point == 23)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_4)
            {
                if (point == 20)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_5)
            {
                if (point == 28)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_6)
            {
                if (point == 28)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
                    }
                }
            }

            if (me->GetEntry() == NPC_NIGHTSABER_VEHICLE_7)
            {
                if (point == 47)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        player->ExitVehicle();
                        me->DespawnOrUnsummon(200);
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
                        me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);
                        me->DespawnOrUnsummon();
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_NIGHTSABER_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath1, NightsaberPath1Size);
                    break;
                case EVENT_NIGHTSABER_2:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath2, NightsaberPath2Size);
                    break;
                case EVENT_NIGHTSABER_3:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath3, NightsaberPath3Size);
                    break;
                case EVENT_NIGHTSABER_4:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath4, NightsaberPath4Size);
                    break;
                case EVENT_NIGHTSABER_5:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath5, NightsaberPath5Size);
                    break;
                case EVENT_NIGHTSABER_6:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath6, NightsaberPath6Size);
                    break;
                case EVENT_NIGHTSABER_7:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(NightsaberPath7, NightsaberPath7Size);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nightsaber_transport_darkshoreAI(creature);
    }
};

enum OneKillWaves
{
    NPC_BLACKWOOD_TRACKER = 11713,
    NPC_MAROSH_THE_DEVIOUS = 11714,

    SPELL_AYNASHA_SHOOT = 19767,
    SPELL_AYNASHA_ROOT = 18373,

    QUEST_ONE_SHOT = 5713,
};

const Position OneKillSpawnPosition[] =
{
    {4375.18f, -42.14f, 80.15f, 5.2f},
    {4379.17f, -42.10f, 80.33f, 5.2f},
    {4376.36f, -38.79f, 76.86f, 5.2f},
};

class npc_sentinel_aynasha : public CreatureScript
{
public:
    npc_sentinel_aynasha() : CreatureScript("npc_sentinel_aynasha") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ONE_SHOT)
            player->CastSpell(creature, SPELL_DUMMY_PING_2);

        return true;
    }

    struct npc_sentinel_aynashaAI : public ScriptedAI
    {
        npc_sentinel_aynashaAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void JustDied(Unit* /*killer*/)
        {
            _summons.DespawnAll();
            _started = false;

            std::list<Player*> character = me->GetPlayersInRange(40, true);
            for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_ONE_SHOT) == QUEST_STATUS_INCOMPLETE)
                    (*it)->FailQuest(QUEST_ONE_SHOT);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING_2 && !_started)
            {
                me->HandleEmoteCommand(EMOTE_STATE_READY_BOW);
                me->CastSpell(me, SPELL_AYNASHA_ROOT);
                me->SetRooted(true);
                _started = true;
                _canShoot = true;
                _killCount = 0;
                _events.ScheduleEvent(EVENT_AYNASHA_1, 2000);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                ++_killCount;

                switch (_killCount)
                {
                case 2:
                    _events.ScheduleEvent(EVENT_AYNASHA_3, 6000);
                    break;
                case 4:
                    _events.ScheduleEvent(EVENT_AYNASHA_4, 6000);
                    break;
                case 5:
                    _events.ScheduleEvent(EVENT_AYNASHA_5, 2000);
                    break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_canShoot)
            {
                DoSpellAttackIfReady(SPELL_AYNASHA_SHOOT);
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AYNASHA_1:
                    me->AI()->Talk(0);
                    _events.ScheduleEvent(EVENT_AYNASHA_2, 4500);
                    break;
                case EVENT_AYNASHA_2:
                    if (auto enemy1 = me->SummonCreature(NPC_BLACKWOOD_TRACKER, OneKillSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto enemy2 = me->SummonCreature(NPC_BLACKWOOD_TRACKER, OneKillSpawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _summons.Summon(enemy1);
                            enemy1->GetMotionMaster()->MoveChase(me);
                            enemy1->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() - 2, me->GetPositionZ(), me->GetOrientation());
                            _summons.Summon(enemy2);
                            enemy2->GetMotionMaster()->MoveChase(me);
                            enemy2->SetHomePosition(me->GetPositionX() - 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                        }
                    break;
                case EVENT_AYNASHA_3:
                    if (auto enemy3 = me->SummonCreature(NPC_BLACKWOOD_TRACKER, OneKillSpawnPosition[1], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto enemy4 = me->SummonCreature(NPC_BLACKWOOD_TRACKER, OneKillSpawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _summons.Summon(enemy3);
                            enemy3->GetMotionMaster()->MoveChase(me);
                            enemy3->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() - 2, me->GetPositionZ(), me->GetOrientation());
                            _summons.Summon(enemy4);
                            enemy4->GetMotionMaster()->MoveChase(me);
                            enemy4->SetHomePosition(me->GetPositionX() - 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                        }
                    break;
                case EVENT_AYNASHA_4:
                    if (auto enemy5 = me->SummonCreature(NPC_MAROSH_THE_DEVIOUS, OneKillSpawnPosition[2], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy5);
                        enemy5->GetMotionMaster()->MoveChase(me);
                        enemy5->SetHomePosition(me->GetPositionX() - 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                        me->AI()->TalkWithDelay(3000, 1);
                        _canShoot = false;
                    }
                    break;
                case EVENT_AYNASHA_5:
                {
                    std::list<Player*> character = me->GetPlayersInRange(45, true);
                    for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_ONE_SHOT) == QUEST_STATUS_INCOMPLETE)
                            (*it)->KilledMonsterCredit(me->GetEntry()); // credit
                }
                me->HandleEmoteCommand(EMOTE_STATE_NONE);
                me->AI()->TalkWithDelay(0, 2);
                me->AI()->TalkWithDelay(4500, 3);
                me->AI()->TalkWithDelay(9500, 4);
                me->DespawnOrUnsummon(15000);
                break;
                default:
                    break;
                }
            }
        }

    private:
        bool _started = false;
        bool _canShoot = false;
        uint16 _checkTimer = 2000;
        uint8 _killCount = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sentinel_aynashaAI(creature);
    }
};

enum QuestTimelyArrival
{
    SPELL_START_ESCORT_DUMMY_FROM_PLAYER = 62825,
    QUEST_TIMELY_ARRIVAL = 13510,
};

// quest=13510/timely-arrival
class npc_sentinel_aynasha_escort : public CreatureScript
{
public:
    npc_sentinel_aynasha_escort() : CreatureScript("npc_sentinel_aynasha_escort") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_TIMELY_ARRIVAL)
        {
            player->CastSpell(player, SPELL_PLAYER_UPDATE_AREA);
            player->CastSpell(creature, SPELL_DUMMY_PING);
        }

        return true;
    }

    struct npc_sentinel_aynasha_escortAI : public npc_escortAI
    {
        npc_sentinel_aynasha_escortAI(Creature* creature) : npc_escortAI(creature) { }

        void InitializeAI() override
        {
            JustRespawned();
        }

        void JustRespawned()
        {
            _isSafe = false;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
            me->SetHomePosition(7986.1098f, -1150.959f, 35.07507f, 2.116540f);
            Reset();
        }

        void JustDied(Unit* /*killer*/)
        {
            _isSafe = false;

            std::list<Player*> character = me->GetPlayersInRange(50, true);
            for (std::list<Player*>::const_iterator it = character.begin(); it != character.end(); it++)
                if ((*it)->GetQuestStatus(QUEST_TIMELY_ARRIVAL) == QUEST_STATUS_INCOMPLETE)
                    (*it)->FailQuest(QUEST_TIMELY_ARRIVAL);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_START_ESCORT_DUMMY_FROM_PLAYER && !_isSafe)
            {
                _isSafe = true;
                me->GetMotionMaster()->MovePoint(0, 7986.11f, -1150.96f, 35.074f);
                _events.ScheduleEvent(EVENT_AYNASHA_ESCORT_1, 1500);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    _events.ScheduleEvent(EVENT_AYNASHA_ESCORT_2, 1000);
                }
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
            case 25:
            {
                std::list<Player*> player = me->GetPlayersInRange(50, true);
                for (std::list<Player*>::const_iterator it = player.begin(); it != player.end(); it++)
                    if ((*it)->GetQuestStatus(QUEST_TIMELY_ARRIVAL) == QUEST_STATUS_INCOMPLETE)
                        (*it)->AreaExploredOrEventHappens(QUEST_TIMELY_ARRIVAL);

                me->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                _events.ScheduleEvent(EVENT_AYNASHA_ESCORT_3, 3000);
                break;
            }
            default:
                break;
            }
        }
        void UpdateAI(const uint32 uiDiff)
        {
            DoMeleeAttackIfReady();
            npc_escortAI::UpdateEscortAI(uiDiff);
            npc_escortAI::UpdateAI(uiDiff);
            _events.Update(uiDiff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_AYNASHA_ESCORT_1:
                    me->SetHomePosition(7986.1098f, -1150.959f, 35.07507f, 2.116540f);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    me->AI()->Talk(0);
                    break;
                case EVENT_AYNASHA_ESCORT_2:
                    Start(true, true, _playerGUID);
                    break;
                case EVENT_AYNASHA_ESCORT_3:
                    me->SetFacingTo(5.6f);
                    me->AI()->TalkWithDelay(2000, 1);
                    me->AI()->TalkWithDelay(9000, 2);
                    me->AI()->TalkWithDelay(18000, 3);
                    _events.ScheduleEvent(EVENT_AYNASHA_ESCORT_4, 21000);
                    break;
                case EVENT_AYNASHA_ESCORT_4:
                    me->GetMotionMaster()->MoveJump(7895.48f, -854.18f, -2.f, 18, 20);
                    me->DespawnOrUnsummon(3000);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isSafe = false;
        EventMap _events;
        uint64 _playerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sentinel_aynasha_escortAI(creature);
    }
};

void AddSC_darkshore()
{
    new npc_last_wave_of_survivors();
    new go_the_final_flame_of_bashalaran();
    new playerscript_darkshore_post_ritual_bond_buffs();
    new npc_possessed_murloc_bomb();
    new npc_vengeful_protector_ancient_vehicle();
    new npc_coaxing_the_spirits_companions();
    new npc_hollee_escort();
    new npc_energized_soothing_totem();
    new npc_thessera_cave_vehicle();
    new npc_thessera_vortex_spellclick();
    new npc_thessera_vortex_vehicle();
    new npc_darkshore_wisp_spellclick();
    new npc_grove_hippogryph_vehicle();
    new npc_prospector_remtravel_escort();
    new npc_offering_to_azshara_controller();
    new npc_nightsaber_transport_darkshore();
    new npc_sentinel_aynasha();
    new npc_sentinel_aynasha_escort();
}
