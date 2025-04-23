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
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

/* ContentData
npc_forest_frog
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "zulaman.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellScript.h"

/*######
## npc_forest_frog
######*/

#define SPELL_REMOVE_AMANI_CURSE    43732
#define SPELL_PUSH_MOJO             43923
#define ENTRY_FOREST_FROG           24396

class npc_forest_frog : public CreatureScript
{
    public:

        npc_forest_frog()
            : CreatureScript("npc_forest_frog")
        {
        }

        struct npc_forest_frogAI : public ScriptedAI
        {
            npc_forest_frogAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void Reset()
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
            }

            void EnterCombat(Unit* /*who*/) {}

            void DoSpawnRandom()
            {
                if (instance)
                {
                    uint32 cEntry = 0;
                    switch (rand()%10)
                    {
                        case 0: cEntry = 24397; break;          //Mannuth
                        case 1: cEntry = 24403; break;          //Deez
                        case 2: cEntry = 24404; break;          //Galathryn
                        case 3: cEntry = 24405; break;          //Adarrah
                        case 4: cEntry = 24406; break;          //Fudgerick
                        case 5: cEntry = 24407; break;          //Darwen
                        case 6: cEntry = 24445; break;          //Mitzi
                        case 7: cEntry = 24448; break;          //Christian
                        case 8: cEntry = 24453; break;          //Brennan
                        case 9: cEntry = 24455; break;          //Hollee
                    }

                    if (!instance->GetData(TYPE_RAND_VENDOR_1))
                        if (rand()%10 == 1) cEntry = 24408;      //Gunter
                    if (!instance->GetData(TYPE_RAND_VENDOR_2))
                        if (rand()%10 == 1) cEntry = 24409;      //Kyren

                    if (cEntry)
                        me->UpdateEntry(cEntry);

                    if (cEntry == 24408)
                    {
                        instance->SetData(TYPE_RAND_VENDOR_1, DONE);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
                    }
                    else if (cEntry == 24409)
                    {
                        instance->SetData(TYPE_RAND_VENDOR_2, DONE);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_VENDOR);
                    }
                }
            }

            void SpellHit(Unit* caster, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPEID_PLAYER && me->GetEntry() == ENTRY_FOREST_FROG)
                {
                    //increase or decrease chance of mojo?
                    if (rand()%99 == 50) DoCast(caster, SPELL_PUSH_MOJO, true);
                    else DoSpawnRandom();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_forest_frogAI(creature);
        }
};

/*######
## npc_zulaman_kasha
######*/
enum ZA_Hostages
{
    NPC_HOSTAGE_HAZLEK      = 52939,
    NPC_HOSTAGE_BAKKALZU    = 52941,
    NPC_HOSTAGE_NORKANI     = 52943,
    NPC_HOSTAGE_KASHA       = 52945,

    GOBJ_KASHA_CAGE         = 187380,
    GOBJ_KASHA_VASE         = 186671,
    GOBJ_COIN_PILE_SMALL    = 186633,
    GOBJ_COIN_PILE_LARGE    = 186634,

    SPELL_DUST_CLOUD        = 43258,
    SPELL_JAR_EXPLOSION     = 43552,
    SPELL_LOOT_VISUAL       = 92406,


    GOLD_PILE_LOOT_MIN      = 5000,
    GOLD_PILE_LOOT_MAX      = 30000,

    MOVEMENT_KASHA_1        = 0,
    MOVEMENT_KASHA_2,
    MOVEMENT_KASHA_3,

    MOVEMENT_KASHA_4,
    MOVEMENT_KASHA_5,
    MOVEMENT_KASHA_6,

    MOVEMENT_KASHA_7,
    KASHA_VASE_2,

    EVENT_KASHA_CLICKABLE,
    EVENT_KASHA_EXIT_CAGE,
    EVENT_KASHA_CALL_FOR_HELP,
    EVENT_KASHA_ATTACK_VASE_2,

    EVENT_TONIGHT_WE_FEAST = 1,
    ACTION_RECOGNIZE_GONG = 1,
    EVENT_DISMOUNT = 1,
    EVENT_MOVE_GONG,
    EVENT_TALK_GONG,
    EVENT_START_GONG,
    EVENT_POST_GONG_1,
    EVENT_POST_GONG_2,
    EVENT_POST_GONG_3,
    EVENT_POST_GONG_4,
};


Position const ZA_HOSTAGE_KASHA[8]
{
    //kasha
    {   384.267456f, 1085.806763f, 5.949582f, 1.653170f    },       //Just outside cage,        MOVEMENT_KASHA_1
    {   374.485901f, 1112.129639f, 5.855717f, 1.032728f    },       //middle of room            MOVEMENT_KASHA_2
    {   344.768982f, 1105.729614f, 6.316678f, 4.429662f    },       //outside 2 vases           MOVEMENT_KASHA_3, will attack vases from here

    //second set of vases
    {   391.130188f, 1107.834595f, 6.661602f, 6.027831f    },       //checkpoint to final vase  MOVEMENT_KASHA_4
    {   409.952393f, 1092.381470f, 6.609568f, 5.772577f    },       //next to vase              MOVEMENT_KASHA_5
    {   382.405304f, 1116.325684f, 6.325772f, 3.055098f    },       //moddle of room            MOVEMENT_KASHA_6
    {   311.753418f, 1123.067749f, 9.778114f, 6.271350f    },       //Door exit                 MOVEMENT_KASHA_7
    {   332.368988f, 1084.420044f, 6.280050f, 0.0f         },       //final vase loc            KASHA_VASE_2

};

Position const KASHA_GOLD[12]
{
    //first
    {346.678040f, 1084.842163f, 7.049619f, 1.617882f},
    {344.447052f, 1086.045166f, 6.970509f, 1.570758f},
    {342.107269f, 1084.960205f, 6.888081f, 1.763181f},
    {344.026062f, 1082.846313f, 6.956691f, 1.492218f},

    //second
    {333.721100f, 1084.363525f, 6.307396f, 1.633573f},
    {332.434204f, 1085.871704f, 6.250907f, 1.610011f},
    {331.124512f, 1084.062256f, 6.285583f, 1.535398f},
    {332.277435f, 1083.035645f, 6.307634f, 1.594303f},

    //final with bag
    {423.287842f, 1082.609985f, 6.697104f, 2.399336f},
    {425.428986f, 1084.944458f, 6.502187f, 2.399336f},
    {425.604340f, 1082.722290f, 6.662596f, 2.572124f},
    {422.776581f, 1084.845947f, 6.559317f, 2.485730f},
};

#define GOSSIP_HOSTAGE_HAZLEK       "Nalorakk is dead. you're free to go."
#define GOSSIP_HOSTAGE_BAKKALZU     "The coast is clear, You're free!"
#define GOSSIP_HOSTAGE_NORANKI      "We've killed your captors. You're free to go."
#define GOSSIP_HOSTAGE_KASHA        "They must've drugged you. It's safe now."

class npc_zulaman_kasha : public CreatureScript
{
    public:
        npc_zulaman_kasha() : CreatureScript("npc_zulaman_kasha") { }

        struct npc_zulaman_kashaAI : public ScriptedAI
        {

            EventMap events;
            npc_zulaman_kashaAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset() 
            {

                if (auto cage = me->FindNearestGameObject(GOBJ_KASHA_CAGE, 10.0f))
                    cage->SetGoState(GO_STATE_READY);

                if (auto bag = me->FindNearestGameObject(186672, 200.0f))
                    bag->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            void EnterCombat(Unit* /*who*/) {}

            void JustDied(Unit* /*killer*/)
            {
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case MOVEMENT_KASHA_2:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_2");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_2, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_2], true);
                        break;
                    case MOVEMENT_KASHA_3:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_3");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_3, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_3], true);
                        break;
                    case MOVEMENT_KASHA_4:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_4");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_4, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_4], true);
                        break;
                    case MOVEMENT_KASHA_5:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_5");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_5, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_5], true);
                        break;
                    case MOVEMENT_KASHA_6:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_6");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_6, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_6], true);
                        break;
                    case MOVEMENT_KASHA_7:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_KASHA_7");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_7, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_7], true);
                        break;
                    case EVENT_KASHA_EXIT_CAGE:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_KASHA_EXIT_CAGE");
                        me->GetMotionMaster()->MovePoint(MOVEMENT_KASHA_1, ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_1], true);
                        break;
                    case EVENT_KASHA_CALL_FOR_HELP:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_KASHA_CALL_FOR_HELP");
                        me->AI()->Talk(0);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case EVENT_KASHA_ATTACK_VASE_2:
                        //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_KASHA_ATTACK_VASE_2");
                        if (auto c = me->SummonCreature(54638, ZA_HOSTAGE_KASHA[KASHA_VASE_2].GetPositionX(), ZA_HOSTAGE_KASHA[KASHA_VASE_2].GetPositionY(), ZA_HOSTAGE_KASHA[KASHA_VASE_2].GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                            if (auto vase = c->FindNearestGameObject(GOBJ_KASHA_VASE, 5.0f))
                            {
                            me->SetOrientation(me->GetAngle(c)); //preps orientation?
                            me->SetInFront(c);  //preps orientation?
                            me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                            vases[1] = c;
                            me->CastWithDelay(1000, c, 77508, true);
                            }
                        break;
                    default:
                        break;
                    }
            }

            void DoAction(const int32 action) override
            {
                switch (action)
                {
                case 2:
                    events.ScheduleEvent(EVENT_KASHA_CALL_FOR_HELP, 8000);
                    break;
                case 3:
                    //TC_LOG_ERROR("sql.sql", "DoAction 3");
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    events.ScheduleEvent(EVENT_KASHA_EXIT_CAGE, 1500);
                    break;
                case 4:
                    //TC_LOG_ERROR("sql.sql", "DoAction 4");
                    break;
                default:
                    break;
                }
            }

            Creature* vases[3]   {nullptr, nullptr, nullptr};

            void SpellHitTarget(Unit* target, SpellInfo const* spell) 
            {
                if (spell->Id == 77508)
                {
                    if (vases[0])
                    if (target->GetGUID() == vases[0]->GetGUID())
                    {
                        //TC_LOG_ERROR("sql.sql", "Vase 1 hit");
                        if (auto vase = target->FindNearestGameObject(GOBJ_KASHA_VASE, 3.0f))
                        {
                            for (auto i = 0; i <= 3; ++i)
                                if (auto pile = target->ToCreature()->SummonGameObject(urand(GOBJ_COIN_PILE_SMALL, GOBJ_COIN_PILE_LARGE), KASHA_GOLD[i].GetPositionX(), KASHA_GOLD[i].GetPositionY(), KASHA_GOLD[i].GetPositionZ(), 0.f, 0.f, 0.f, 0.f, 0.f, 2147000000))
                                {
                                    Loot* loot = &pile->loot;
                                    loot->clear();
                                    loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN, GOLD_PILE_LOOT_MAX, pile);
                                }
                            vase->SetPhaseMask(2, true);
                        }
                        target->CastSpell(target, SPELL_JAR_EXPLOSION);
                        target->CastSpell(target, SPELL_DUST_CLOUD);
                        target->AddAura(SPELL_LOOT_VISUAL, target);
                        //target->ToCreature()->DespawnOrUnsummon(1500);
                        me->AI()->Talk(2);
                        events.ScheduleEvent(EVENT_KASHA_ATTACK_VASE_2, 2000);
                        vases[0] = nullptr;
                        return;
                    }

                    if (vases[1])
                    if (target->GetGUID() == vases[1]->GetGUID())
                    {
                        //TC_LOG_ERROR("sql.sql", "Vase 2 hit");
                        vases[1] = nullptr;
                        if (auto vase = target->FindNearestGameObject(GOBJ_KASHA_VASE, 3.0f))
                        {
                            for (auto i = 4; i <= 7; ++i)
                                if (auto pile = target->ToCreature()->SummonGameObject(urand(GOBJ_COIN_PILE_SMALL, GOBJ_COIN_PILE_LARGE), KASHA_GOLD[i].GetPositionX(), KASHA_GOLD[i].GetPositionY(), KASHA_GOLD[i].GetPositionZ(), 0.f, 0.f, 0.f, 0.f, 0.f, 2147000000))
                                {
                                    Loot* loot = &pile->loot;
                                    loot->clear();
                                    loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN, GOLD_PILE_LOOT_MAX, pile);
                                }
                            vase->SetPhaseMask(2, true);
                        }

                        target->CastSpell(target, SPELL_JAR_EXPLOSION);
                        target->CastSpell(target, SPELL_DUST_CLOUD);
                        target->AddAura(SPELL_LOOT_VISUAL, target);
                        //target->ToCreature()->DespawnOrUnsummon(1500);

                        me->AI()->Talk(3); 
                        events.ScheduleEvent(MOVEMENT_KASHA_4, 1000);
                        vases[1] = nullptr;
                        return;
                    }

                    if (vases[2])
                    if (target->GetGUID() == vases[2]->GetGUID())
                    {
                        //TC_LOG_ERROR("sql.sql", "Vase 3 hit");
                        if (auto vase = target->FindNearestGameObject(GOBJ_KASHA_VASE, 3.0f))
                        {
                            for (auto i = 8; i <= 11; ++i)
                                if (auto pile = target->ToCreature()->SummonGameObject(urand(GOBJ_COIN_PILE_SMALL, GOBJ_COIN_PILE_LARGE), KASHA_GOLD[i].GetPositionX(), KASHA_GOLD[i].GetPositionY(), KASHA_GOLD[i].GetPositionZ(), 0.f, 0.f, 0.f, 0.f, 0.f, 2147000000))
                                {
                                    Loot* loot = &pile->loot;
                                    loot->clear();
                                    loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN, GOLD_PILE_LOOT_MAX, pile);
                                }
                            vase->SetPhaseMask(2, true);
                        }

                        if (auto bag = target->FindNearestGameObject(186672, 3.0f))
                        {
                            if (instance->GetDeadBossTotal(true) < 4)
                            {
                                bag->SetPhaseMask(2, true);
                            }
                            else
                            {
                                bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                            }
                        }

                        target->CastSpell(target, SPELL_JAR_EXPLOSION);
                        target->CastSpell(target, SPELL_DUST_CLOUD);
                        target->AddAura(SPELL_LOOT_VISUAL, target);

                        events.ScheduleEvent(MOVEMENT_KASHA_6, 8000);
                        me->AI()->Talk(4);
                        vases[2] = nullptr;
                        return;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type == 8)
                    switch (id)
                    {
                        case MOVEMENT_KASHA_1:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_1");
                            me->AI()->Talk(1);
                            events.ScheduleEvent(MOVEMENT_KASHA_2, 1000);
                            break;
                        case MOVEMENT_KASHA_2:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_2");
                            events.ScheduleEvent(MOVEMENT_KASHA_3, 1000);
                            break;
                        case MOVEMENT_KASHA_3:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_3");
                            if (auto vase = me->FindNearestGameObject(186671, 30.0f))
                                if (auto c = me->SummonCreature(54638, vase->GetPositionX(), vase->GetPositionY(), vase->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                                {
                                    me->SetOrientation(me->GetAngle(c)); //preps orientation?
                                    me->SetInFront(c);  //preps orientation?
                                    me->SetFacingTo(me->GetOrientation()); //force-updates orientation

                                    vases[0] = c;
                                    me->CastWithDelay(1000, c, 77508, true);
                                }
                            //events.ScheduleEvent(MOVEMENT_KASHA_4, 2000);
                            break;
                        case MOVEMENT_KASHA_4:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_4");
                            events.ScheduleEvent(MOVEMENT_KASHA_5, 1000);
                            break;
                        case MOVEMENT_KASHA_5:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_5");
                            if (auto vase = me->FindNearestGameObject(186671, 30.0f))
                                if (auto c = me->SummonCreature(54638, vase->GetPositionX(), vase->GetPositionY(), vase->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                                {
                                    me->SetOrientation(me->GetAngle(c)); //preps orientation?
                                    me->SetInFront(c);  //preps orientation?
                                    me->SetFacingTo(me->GetOrientation()); //force-updates orientation

                                    vases[2] = c;
                                    me->CastWithDelay(1000, c, 77508, true);
                                }
                            //
                            break;
                        case MOVEMENT_KASHA_6:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_6");
                            events.ScheduleEvent(MOVEMENT_KASHA_7, 500);
                            break;
                        case MOVEMENT_KASHA_7:
                            //TC_LOG_ERROR("sql.sql", "Movement KASHA_7, distance: %u", uint32(double(me->GetDistance(ZA_HOSTAGE_KASHA[MOVEMENT_KASHA_7]))));
                            me->SetOrientation(0.f); //preps orientation?
                            me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                            break;
                        break;
                    default:
                        break;
                    }
            }
        };


        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_zulaman_kashaAI(creature);
        }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) || !creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            {
                //TC_LOG_ERROR("sql.sql", "npc_zulaman_kasha: NPC %u clicked despite having UNIT_FLAG_NOT_SELECTABLE by player GUID %u", creature->GetEntry(), player->GetGUID());
                return false;
            }

            switch (creature->GetEntry())
            {
            case NPC_HOSTAGE_HAZLEK:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_HAZLEK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                break;
            case NPC_HOSTAGE_BAKKALZU:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_BAKKALZU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                break;
            case NPC_HOSTAGE_NORKANI:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_NORANKI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                break;
            case NPC_HOSTAGE_KASHA:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_KASHA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                break;
            default:
                //TC_LOG_ERROR("sql.sql", "NPC_ZA_HOSTAGE script has npc other than 4 predefined npcs: %u", creature->GetEntry());
                player->CLOSE_GOSSIP_MENU();
                return false;
                break;
            }
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {

                player->CLOSE_GOSSIP_MENU();

                if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                    return true;


                if (auto instance = creature->GetInstanceScript())
                {
                    instance->SetData(DATA_CHESTLOOTED, 0);
                    //uint8 progress = instance->GetData(DATA_CHESTLOOTED);
                    Map::PlayerList const& players = creature->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (auto player = itr->getSource())
                            player->KilledMonsterCredit(creature->GetEntry());
                    float x, y, z;
                    creature->GetPosition(x, y, z);
                    uint32 entry = creature->GetEntry();

                    switch (creature->GetEntry())
                    {
                    case NPC_HOSTAGE_KASHA:
                        if (auto cage = creature->FindNearestGameObject(187380, 10.0f))
                            cage->SetGoState(GO_STATE_ACTIVE);
                        creature->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                        creature->AI()->DoAction(3);
                        break;
                    default:
                        break;
                    }
                }
            }
            return true;
        }
};



Position const ZA_HOSTAGE_BAKKALZU[3]
{
    {298.717896f, 1464.671387f, 81.557983f, 5.293569f},     //MOVEMENT_BAKKALZU_1
    {307.422211f, 1465.167847f, 81.585793f, 2.034174f},     //MOVEMENT_BAKKALZU_2
    {303.940308f, 1465.517700f, 81.581894f, 0.918909f},     //MOVEMENT_BAKKALZU_3
};

enum Bakkalzu_enum
{
    MOVEMENT_BAKKALZU_1 = 0,
    MOVEMENT_BAKKALZU_2,
    MOVEMENT_BAKKALZU_3,
    MOVEMENT_BAKKALZU_1_final,

    EVENT_OPEN_BOX,
    EVENT_BAKKALZU_CALL_FOR_HELP, 
    EVENT_BAKKALZU_EXIT_CAGE,

    GOBJ_BAKKALZU_CAGE                  = 187378,
    GOBJ_BAKKALZU_BOX                   = 186622,
    GOBJ_BAKKALZU_BAG                   = 187021,
};

class npc_zulaman_bakkalzu : public CreatureScript
{
public:
    npc_zulaman_bakkalzu() : CreatureScript("npc_zulaman_bakkalzu") { }

    struct npc_zulaman_bakkalzuAI : public ScriptedAI
    {

        EventMap events;
        npc_zulaman_bakkalzuAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            if (auto cage = me->FindNearestGameObject(GOBJ_BAKKALZU_CAGE, 10.0f))
                cage->SetGoState(GO_STATE_READY);

            if (auto bag = me->FindNearestGameObject(GOBJ_BAKKALZU_BAG, 100.0f))
                bag->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case MOVEMENT_BAKKALZU_2:
                    me->AI()->Talk(2);
                    me->GetMotionMaster()->MovePoint(MOVEMENT_BAKKALZU_2, ZA_HOSTAGE_BAKKALZU[MOVEMENT_BAKKALZU_2], true);
                    break;
                case MOVEMENT_BAKKALZU_3:
                    me->GetMotionMaster()->MovePoint(MOVEMENT_BAKKALZU_3, ZA_HOSTAGE_BAKKALZU[MOVEMENT_BAKKALZU_3], true);
                    break;
                case EVENT_OPEN_BOX:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                    if (auto box = me->FindNearestGameObject(GOBJ_BAKKALZU_BOX, 10.0f))
                    {
                        box->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    }
                   
                    if (auto bag = me->FindNearestGameObject(GOBJ_BAKKALZU_BAG, 10.0f))
                    {
                        if (auto c = me->SummonCreature(54638, bag->GetPositionX(), bag->GetPositionY(), bag->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                            c->AddAura(SPELL_LOOT_VISUAL, c);

                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        Loot* loot = &bag->loot;
                        loot->clear();
                        loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN*4, GOLD_PILE_LOOT_MAX*4, bag);
                    }
                    me->GetMotionMaster()->MovePoint(MOVEMENT_BAKKALZU_1_final, ZA_HOSTAGE_BAKKALZU[MOVEMENT_BAKKALZU_1], true);
                    break;
                case EVENT_BAKKALZU_CALL_FOR_HELP:
                    me->AI()->Talk(0);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case EVENT_BAKKALZU_EXIT_CAGE:
                    me->GetMotionMaster()->MovePoint(MOVEMENT_BAKKALZU_1, ZA_HOSTAGE_BAKKALZU[MOVEMENT_BAKKALZU_1], true);
                    break;
                default:
                    break;
                }
        }

        void DoAction(const int32 action) override
        {
            switch (action)
            {
            case 2:
                if (auto bag = me->FindNearestGameObject(GOBJ_BAKKALZU_BAG, 100.0f))
                    bag->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                events.ScheduleEvent(EVENT_BAKKALZU_CALL_FOR_HELP, 8000);
                break;
            case 3:
                events.ScheduleEvent(EVENT_BAKKALZU_EXIT_CAGE, 1500);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 4:
                break;
            default:
                break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                switch (id)
                {
                case MOVEMENT_BAKKALZU_1:
                    me->AI()->Talk(1);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER_NO_SHEATHE);
                    events.ScheduleEvent(MOVEMENT_BAKKALZU_2, 5000);
                    break;
                case MOVEMENT_BAKKALZU_2:
                    if (auto box = me->FindNearestGameObject(GOBJ_BAKKALZU_BOX, 10.0f))
                    {
                        me->SetOrientation(me->GetAngle(box)); //preps orientation?
                        me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                    }
                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                    events.ScheduleEvent(MOVEMENT_BAKKALZU_3, 3000);
                    break;
                case MOVEMENT_BAKKALZU_3:
                    if (auto box = me->FindNearestGameObject(GOBJ_BAKKALZU_BOX, 10.0f))
                    {
                        me->SetOrientation(me->GetAngle(box)); //preps orientation?
                        me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);
                        //box->SetGoState(GO_STATE_ACTIVE);
                    }
                    events.ScheduleEvent(EVENT_OPEN_BOX, 3000);
                    break;
                case MOVEMENT_BAKKALZU_1_final:
                    me->AI()->Talk(3);
                    me->HandleEmoteCommand(EMOTE_STATE_AT_EASE);
                    me->SetOrientation(5.48f); //preps orientation?
                    me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                    break;
                default:
                    break;
                }
        }
    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zulaman_bakkalzuAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) || !creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            return false;

        switch (creature->GetEntry())
        {
        case NPC_HOSTAGE_BAKKALZU:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_BAKKALZU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            break;
        default:
            player->CLOSE_GOSSIP_MENU();
            return false;
            break;
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {

            player->CLOSE_GOSSIP_MENU();

            if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return true;


            if (auto instance = creature->GetInstanceScript())
            {
                instance->SetData(DATA_CHESTLOOTED, 0);
                Map::PlayerList const& players = creature->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (auto player = itr->getSource())
                        player->KilledMonsterCredit(creature->GetEntry());
                float x, y, z;
                creature->GetPosition(x, y, z);
                uint32 entry = creature->GetEntry();

                switch (creature->GetEntry())
                {
                case NPC_HOSTAGE_BAKKALZU:
                    if (auto cage = creature->FindNearestGameObject(GOBJ_BAKKALZU_CAGE, 10.0f))
                    {
                        cage->SetGoState(GO_STATE_ACTIVE);
                    }
                    creature->AI()->DoAction(3);
                    break;
                default:
                    break;
                }
            }
        }
        return true;
    }
};

Position const ZA_HOSTAGE_hazlek[3]
{
    {-145.281464f, 1335.400879f, 48.173862f, 0.774895f},     //MOVEMENT_hazlek_1
    {-136.526260f, 1334.040527f, 48.173828f, 5.589393f},     //MOVEMENT_hazlek_2
    {-136.526260f, 1334.040527f, 48.173828f, 5.589393f},     //MOVEMENT_hazlek_2
};

enum hazlek_enum
{
    MOVEMENT_hazlek_1 = 0,
    MOVEMENT_hazlek_2,
    MOVEMENT_hazlek_3,
    MOVEMENT_hazlek_1_final,

    EVENT_OPEN_BOX_HAZLEK,
    EVENT_hazlek_CALL_FOR_HELP,
    EVENT_hazlek_EXIT_CAGE,

    SAY_HAZLEK_HELP_ME      = 0,
    SAY_HAZLEK_FREE         = 1,
    SAY_HAZLEK_MY_THINGS    = 4,
    SAY_HAZLEK_OPENING      = 2,
    SAY_HAZLEK_GOODBYE      = 3,

    GOBJ_hazlek_CAGE = 187377,
    GOBJ_hazlek_BOX = 186648,
    GOBJ_hazlek_BAG = 186648,
};

class npc_zulaman_hazlek : public CreatureScript
{
public:
    npc_zulaman_hazlek() : CreatureScript("npc_zulaman_hazlek") { }

    struct npc_zulaman_hazlekAI : public ScriptedAI
    {

        EventMap events;
        npc_zulaman_hazlekAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            if (auto bag = me->FindNearestGameObject(GOBJ_hazlek_BAG, 100.0f))
                bag->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

            if (auto cage = me->FindNearestGameObject(GOBJ_hazlek_CAGE, 10.0f))
                cage->SetGoState(GO_STATE_READY);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case MOVEMENT_hazlek_2:
                    me->AI()->Talk(SAY_HAZLEK_MY_THINGS);
                    me->GetMotionMaster()->MovePoint(MOVEMENT_hazlek_2, ZA_HOSTAGE_hazlek[MOVEMENT_hazlek_2], true);
                    break;
                case MOVEMENT_hazlek_3:
                    me->GetMotionMaster()->MovePoint(MOVEMENT_hazlek_3, ZA_HOSTAGE_hazlek[MOVEMENT_hazlek_3], true);
                    break;
                case EVENT_OPEN_BOX_HAZLEK:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);

                    if (auto bag = me->FindNearestGameObject(GOBJ_hazlek_BAG, 10.0f))
                    {
                        if (auto c = me->SummonCreature(54638, bag->GetPositionX(), bag->GetPositionY(), bag->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                            c->AddAura(SPELL_LOOT_VISUAL, c);

                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                        Loot* loot = &bag->loot;
                        loot->clear();
                        loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN * 4, GOLD_PILE_LOOT_MAX * 4, bag);
                    }
                    me->GetMotionMaster()->MovePoint(MOVEMENT_hazlek_1_final, ZA_HOSTAGE_hazlek[MOVEMENT_hazlek_1], true);
                    break;
                case EVENT_hazlek_CALL_FOR_HELP:
                    me->AI()->Talk(SAY_HAZLEK_HELP_ME);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case EVENT_hazlek_EXIT_CAGE:
                    me->GetMotionMaster()->MovePoint(MOVEMENT_hazlek_1, ZA_HOSTAGE_hazlek[MOVEMENT_hazlek_1], true);
                    break;
                default:
                    break;
                }
        }

        void DoAction(const int32 action) override
        {
            switch (action)
            {
            case 2:
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                events.ScheduleEvent(EVENT_hazlek_CALL_FOR_HELP, 8000);
                break;
            case 3:
                events.ScheduleEvent(EVENT_hazlek_EXIT_CAGE, 1500);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 4:
                break;
            default:
                break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                switch (id)
                {
                case MOVEMENT_hazlek_1:
                    me->AI()->Talk(SAY_HAZLEK_FREE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER_NO_SHEATHE);
                    events.ScheduleEvent(MOVEMENT_hazlek_2, 5000);
                    break;
                case MOVEMENT_hazlek_2:
                    if (auto box = me->FindNearestGameObject(GOBJ_hazlek_BOX, 10.0f))
                    {
                        me->SetOrientation(me->GetAngle(box)); //preps orientation?
                        me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                    }
                    me->AI()->Talk(SAY_HAZLEK_OPENING);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                    events.ScheduleEvent(MOVEMENT_hazlek_3, 3000);
                    break;
                case MOVEMENT_hazlek_3:
                    if (auto box = me->FindNearestGameObject(GOBJ_hazlek_BOX, 10.0f))
                    {
                        me->SetOrientation(me->GetAngle(box)); //preps orientation?
                        me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                        me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING);
                        //box->SetGoState(GO_STATE_ACTIVE);
                    }
                    events.ScheduleEvent(EVENT_OPEN_BOX, 3000);
                    break;
                case MOVEMENT_hazlek_1_final:
                    me->AI()->Talk(SAY_HAZLEK_GOODBYE);
                    me->SetOrientation(0.4f); //preps orientation?
                    me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                    break;
                default:
                    break;
                }
        }
    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zulaman_hazlekAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) || !creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            return false;

        switch (creature->GetEntry())
        {
        case NPC_HOSTAGE_HAZLEK:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_HAZLEK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            break;
        default:
            player->CLOSE_GOSSIP_MENU();
            return false;
            break;
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {

            player->CLOSE_GOSSIP_MENU();

            if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return true;


            if (auto instance = creature->GetInstanceScript())
            {
                instance->SetData(DATA_CHESTLOOTED, 0);
                Map::PlayerList const& players = creature->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (auto player = itr->getSource())
                        player->KilledMonsterCredit(creature->GetEntry());
                float x, y, z;
                creature->GetPosition(x, y, z);
                uint32 entry = creature->GetEntry();

                switch (creature->GetEntry())
                {
                case NPC_HOSTAGE_HAZLEK:
                    if (auto cage = creature->FindNearestGameObject(GOBJ_hazlek_CAGE, 10.0f))
                    {
                        cage->SetGoState(GO_STATE_ACTIVE);
                    }
                    creature->AI()->DoAction(3);
                    break;
                default:
                    break;
                }
            }
        }
        return true;
    }
};


Position const ZA_HOSTAGE_norkani[9]
{
    {-74.334740f, 1158.069824f, 5.462713f, 4.555199f},      //MOVEMENT_norkani_1
    {-91.012970f, 1134.070068f, 5.594202f, 4.747648f},      //2_pt1
    {-81.506775f, 1126.151733f, 5.594070f, 5.564424f},      //MOVEMENT_norkani_2
    {-84.893593f, 1129.918579f, 5.594202f, 1.971256f},      //3_pt1
    {-84.309219f, 1137.021484f, 5.594202f, 1.138734f},      //3_pt2
    {-78.586662f, 1137.860474f, 5.381825f, 6.102413f},      //MOVEMENT_norkani_3
    {-84.309219f, 1137.021484f, 5.594202f, 1.138734f},      //4_pt1
    {-84.893593f, 1129.918579f, 5.594202f, 1.971256f},      //4_pt2
    {-83.200180f, 1130.411743f, 5.593014f, 3.612715f}       //MOVEMENT_norkani_4
};

Position const NORKANI_GOLD[4]
{
    {-80.357475f, 1124.029785f, 5.594042f, 3.121917f},
    {-80.866051f, 1125.228760f, 5.594053f, 2.725291f},
    {-80.243301f, 1126.595337f, 5.594045f, 3.153333f},
    {-79.006950f, 1125.479614f, 5.511815f, 3.121918f},
};

enum norkani_enum
{
    MOVEMENT_norkani_1 = 0,
    MOVEMENT_norkani_2_pt1,
    MOVEMENT_norkani_2,
    MOVEMENT_norkani_3_pt1,
    MOVEMENT_norkani_3_pt2,
    MOVEMENT_norkani_3,
    MOVEMENT_norkani_4_pt1,
    MOVEMENT_norkani_4_pt2,
    MOVEMENT_norkani_4,

    EVENT_OPEN_NORKANI_CHEST,
    EVENT_norkani_CALL_FOR_HELP,
    EVENT_norkani_EXIT_CAGE,

    SPELL_FUSE_SMOKE    = 94423,

    GOBJ_norkani_CAGE = 187379,
    GOBJ_norkani_BOX = 186658,
    GOBJ_norkani_BAG = 186667,
};

class npc_zulaman_norkani : public CreatureScript
{
public:
    npc_zulaman_norkani() : CreatureScript("npc_zulaman_norkani") { }

    struct npc_zulaman_norkaniAI : public ScriptedAI
    {

        EventMap events;
        npc_zulaman_norkaniAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            if (auto cage = me->FindNearestGameObject(GOBJ_norkani_CAGE, 10.0f))
                cage->SetGoState(GO_STATE_READY);

            if (auto bag = me->FindNearestGameObject(GOBJ_norkani_BAG, 100.0f))
                bag->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case MOVEMENT_norkani_2_pt1:
                    me->AI()->Talk(2);
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_2_pt1");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_2_pt1, ZA_HOSTAGE_norkani[MOVEMENT_norkani_2_pt1], true);
                    break;
                case MOVEMENT_norkani_2:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_2");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_2, ZA_HOSTAGE_norkani[MOVEMENT_norkani_2], true);
                    break;
                case MOVEMENT_norkani_3_pt1:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_3_pt1");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_3_pt1, ZA_HOSTAGE_norkani[MOVEMENT_norkani_3_pt1], true);
                    break;
                case MOVEMENT_norkani_3_pt2:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_3_pt2");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_3_pt2, ZA_HOSTAGE_norkani[MOVEMENT_norkani_3_pt2], true);
                    break;
                case MOVEMENT_norkani_3:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_3");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_3, ZA_HOSTAGE_norkani[MOVEMENT_norkani_3], true);
                    break;

                case MOVEMENT_norkani_4_pt1:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_4_pt1");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_4_pt1, ZA_HOSTAGE_norkani[MOVEMENT_norkani_4_pt1], true);
                    break;
                case MOVEMENT_norkani_4_pt2:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_4_pt2");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_4_pt2, ZA_HOSTAGE_norkani[MOVEMENT_norkani_4_pt2], true);
                    break;
                case MOVEMENT_norkani_4:
                    ////TC_LOG_ERROR("sql.sql", "UpdateAI: MOVEMENT_norkani_4");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_4, ZA_HOSTAGE_norkani[MOVEMENT_norkani_4], true);
                    break;
                case EVENT_OPEN_NORKANI_CHEST:
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    me->RemoveAllAuras();
                    summons.DespawnAll();

                    if (auto box = me->FindNearestGameObject(GOBJ_norkani_BOX, 20.0f))
                    {
                        box->SetPhaseMask(2, true);
                    }

                    if (auto bag = me->FindNearestGameObject(GOBJ_norkani_BAG, 20.0f))
                    {
                        if (auto c = me->SummonCreature(54638, bag->GetPositionX(), bag->GetPositionY(), bag->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                        {
                            c->CastSpell(c, 30934, true);
                            c->AddAura(SPELL_LOOT_VISUAL, c);
                            for (auto i = 0; i <= 3; ++i)
                                if (auto pile = c->SummonGameObject(urand(GOBJ_COIN_PILE_SMALL, GOBJ_COIN_PILE_LARGE), NORKANI_GOLD[i].GetPositionX(), NORKANI_GOLD[i].GetPositionY(), NORKANI_GOLD[i].GetPositionZ(), 0.f, 0.f, 0.f, 0.f, 0.f, 2147000000))
                                {
                                    Loot* loot = &pile->loot;
                                    loot->clear();
                                    loot->generateMoneyLoot(GOLD_PILE_LOOT_MIN, GOLD_PILE_LOOT_MAX, pile);
                                }
                        }

                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        bag->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);


                    }
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_OPEN_NORKANI_CHEST");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_4_pt1, ZA_HOSTAGE_norkani[MOVEMENT_norkani_4_pt1], false);
                    break;
                case EVENT_norkani_CALL_FOR_HELP:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_norkani_CALL_FOR_HELP");
                    me->AI()->Talk(0);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    break;
                case EVENT_norkani_EXIT_CAGE:
                    //TC_LOG_ERROR("sql.sql", "UpdateAI: EVENT_norkani_EXIT_CAGE");
                    me->GetMotionMaster()->MovePoint(MOVEMENT_norkani_1, ZA_HOSTAGE_norkani[MOVEMENT_norkani_1], true);
                    break;
                default:
                    break;
                }
        }

        void DoAction(const int32 action) override
        {
            switch (action)
            {
            case 2:
                me->SetSheath(SHEATH_STATE_UNARMED);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                events.ScheduleEvent(EVENT_norkani_CALL_FOR_HELP, 8000);
                break;
            case 3:
                events.ScheduleEvent(EVENT_norkani_EXIT_CAGE, 1500);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 4:
                break;
            default:
                break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                switch (id)
                {
                case MOVEMENT_norkani_1:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_1");
                    me->AI()->Talk(1);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER_NO_SHEATHE);
                    events.ScheduleEvent(MOVEMENT_norkani_2_pt1, 5000);
                    break;
                case MOVEMENT_norkani_2_pt1:
                    events.ScheduleEvent(MOVEMENT_norkani_2, 1);
                    break;
                case MOVEMENT_norkani_2:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_2");
                    events.ScheduleEvent(MOVEMENT_norkani_3_pt1, 3000);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                    me->AI()->Talk(3);
                    if (auto bag = me->FindNearestGameObject(GOBJ_norkani_BAG, 10.0f))
                        if (auto c = me->SummonCreature(54638, bag->GetPositionX(), bag->GetPositionY(), bag->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
                            if (Aura* a = c->AddAura(SPELL_FUSE_SMOKE, c))
                            {
                                a->SetMaxDuration(10000);
                                a->SetDuration(10000);
                            }
                    break;
                case MOVEMENT_norkani_3_pt1:
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_3_pt1");
                    events.ScheduleEvent(MOVEMENT_norkani_3_pt2, 1);
                    break;
                case MOVEMENT_norkani_3_pt2:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_3_pt2");
                    events.ScheduleEvent(MOVEMENT_norkani_3, 1);
                    break;
                case MOVEMENT_norkani_3:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_3"); //needs a kneel
                    me->AddAura(68442, me);
                    /*
                    if (auto box = me->FindNearestGameObject(GOBJ_norkani_BOX, 10.0f))
                    {
                        me->SetOrientation(me->GetAngle(box)); //preps orientation?
                        me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                        me->HandleEmoteCommand(EMOTE_ONESHOT_WORK_CHOPWOOD);
                    }
                    */
                    events.ScheduleEvent(EVENT_OPEN_NORKANI_CHEST, 5000);
                    break;
                case MOVEMENT_norkani_4_pt1:
                    me->HandleEmoteCommand(EMOTE_STATE_NONE);
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_4_pt1");
                    events.ScheduleEvent(MOVEMENT_norkani_4_pt2, 1);
                    break;
                case MOVEMENT_norkani_4_pt2:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_4_pt2");
                    events.ScheduleEvent(MOVEMENT_norkani_4, 1);
                    break;
                case MOVEMENT_norkani_4:
                    //TC_LOG_ERROR("sql.sql", "MovementInform: MOVEMENT_norkani_4");
                    me->AI()->Talk(4);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                    me->SetOrientation(3.60f); //preps orientation?
                    me->SetFacingTo(me->GetOrientation()); //force-updates orientation
                    break;
                default:
                    break;
                }
        }
    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zulaman_norkaniAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) || !creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            return false;

        switch (creature->GetEntry())
        {
        case NPC_HOSTAGE_NORKANI:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE_NORANKI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            break;
        default:
            player->CLOSE_GOSSIP_MENU();
            return false;
            break;
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {

            player->CLOSE_GOSSIP_MENU();

            if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return true;


            if (auto instance = creature->GetInstanceScript())
            {
                instance->SetData(DATA_CHESTLOOTED, 0);
                Map::PlayerList const& players = creature->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (auto player = itr->getSource())
                        player->KilledMonsterCredit(creature->GetEntry());
                float x, y, z;
                creature->GetPosition(x, y, z);
                uint32 entry = creature->GetEntry();

                switch (creature->GetEntry())
                {
                case NPC_HOSTAGE_NORKANI:
                    if (auto cage = creature->FindNearestGameObject(GOBJ_norkani_CAGE, 10.0f))
                    {
                        cage->SetGoState(GO_STATE_ACTIVE);
                    }
                    creature->AI()->DoAction(3);
                    break;
                default:
                    break;
                }
            }
        }
        return true;
    }
};

/*######
## npc_harrison_jones
######*/

enum Says
{
    SAY_HARRISON_0                    = 0,
    SAY_HARRISON_1                    = 1,
    SAY_HARRISON_2                    = 0,
    SAY_HARRISON_3                    = 1
};

enum Spells
{
    SPELL_BANGING_THE_GONG            = 45225,
    SPELL_STEALTH                     = 34189,
    SPELL_COSMETIC_SPEAR_THROW        = 43647
};

enum Events
{
    GONG_EVENT_1                      = 1,
    GONG_EVENT_2                      = 2,
    GONG_EVENT_3                      = 3,
    GONG_EVENT_4                      = 4,
    GONG_EVENT_5                      = 5,
    GONG_EVENT_6                      = 6,
    GONG_EVENT_7                      = 7,
    GONG_EVENT_8                      = 8,
    GONG_EVENT_9                      = 9,
    GONG_EVENT_10                     = 10,
    GONG_EVENT_11                     = 11,
    DATA_HIT_GONG,
};

enum Waypoints
{
    HARRISON_MOVE_1                   = 860440,
    HARRISON_MOVE_2                   = 860441,
    HARRISON_MOVE_3                   = 860442
};

enum DisplayIds
{
    MODEL_HARRISON_JONES_0              = 22340,
    MODEL_HARRISON_JONES_1              = 22354,
    MODEL_HARRISON_JONES_2              = 22347
};

enum Weapons
{
    WEAPON_MACE                         = 5301,
    WEAPON_SPEAR                        = 13631
};

class npc_harrison_jones : public CreatureScript
{
    public:

        npc_harrison_jones()
            : CreatureScript("npc_harrison_jones")
        {
        }

        struct npc_harrison_jonesAI : public ScriptedAI
        {
            npc_harrison_jonesAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint8 _gongEvent;
            uint32 _gongTimer;
            uint32 gongHitCount;

            void Reset()
            {
                _gongEvent = 0;
                _gongTimer = 0;
                gongHitCount = 0;
            }

            void EnterCombat(Unit* /*who*/) {}

            void sGossipSelect(Player* player, uint32 sender, uint32 action)
            {
                if (!instance->IsDone(DATA_GONGEVENT))
                {
                    if (me->GetCreatureTemplate()->GossipMenuId == sender && !action)
                    {
                        player->CLOSE_GOSSIP_MENU();
                        me->SetFacingToObject(player);
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(SAY_HARRISON_0);
                        _gongEvent = GONG_EVENT_1;
                        _gongTimer = 4000;
                    }
                }
                else
                {
                    if (auto gate = me->GetMap()->GetGameObject(instance->GetData64(GO_MASSIVE_GATE)))
                        gate->SetGoState(GO_STATE_ACTIVE);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
            }

            void SpellHit(Unit*, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_COSMETIC_SPEAR_THROW)
                {
                    me->RemoveAllAuras();
                    me->SetEntry(NPC_HARRISON_JONES_2);
                    me->SetDisplayId(MODEL_HARRISON_JONES_2);
                    me->SetTarget(0);
                    me->SetByteValue(UNIT_FIELD_BYTES_1, 0, UNIT_STAND_STATE_DEAD);
                    me->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    if (instance)
                        instance->SetBossState(DATA_GONGEVENT, DONE_HM);
                }
            }

            void SetData(uint32 id, uint32 value)
            {
                if (id == DATA_HIT_GONG)
                    gongHitCount += value;
                if (gongHitCount == 5)
                {
                    instance->SetBossState(DATA_GONGEVENT, IN_PROGRESS);
                    if (_gongEvent == GONG_EVENT_4)
                        _gongTimer = 1000;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (_gongEvent)
                {
                    if (_gongTimer <= diff)
                    {
                        switch (_gongEvent)
                        {
                            case GONG_EVENT_1:
                                // me->GetMotionMaster()->MovePath(HARRISON_MOVE_1, false);
                                me->GetMotionMaster()->MovePoint(0, 132.16f, 1642.87f, 42.02f, false);
                                _gongEvent = GONG_EVENT_2;
                                _gongTimer = 12000;
                                break;
                            case GONG_EVENT_2:
                                me->SetFacingTo(6.235659f);
                                Talk(SAY_HARRISON_1);
                                DoCast(me, SPELL_BANGING_THE_GONG);
                                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(WEAPON_MACE));
                                me->SetSheath(SHEATH_STATE_MELEE);
                                _gongEvent = GONG_EVENT_3;
                                _gongTimer = 4000;
                                break;
                            case GONG_EVENT_3:
                                if (auto gong = me->GetMap()->GetGameObject(instance->GetData64(GO_STRANGE_GONG)))
                                    gong->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                _gongEvent = GONG_EVENT_4;
                                instance->SetBossState(DATA_GONGEVENT, IN_PROGRESS);
                                _gongTimer = 5000;
                                //                                _gongTimer = 105000;
                                break;
                            case GONG_EVENT_4:
                                me->RemoveAura(SPELL_BANGING_THE_GONG);
                                if (auto gong = me->GetMap()->GetGameObject(instance->GetData64(GO_STRANGE_GONG)))
                                    gong->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                if (instance->GetBossState(DATA_GONGEVENT) == IN_PROGRESS)
                                {
                                    me->GetMotionMaster()->MovePath(HARRISON_MOVE_2, false);
                                    _gongEvent = GONG_EVENT_5;
                                    _gongTimer = 5000;
                                }
                                else
                                {
                                    _gongTimer = 1000;
                                    _gongEvent = GONG_EVENT_9;
                                }
                                break;
                            case GONG_EVENT_5:
                                me->SetEntry(NPC_HARRISON_JONES_1);
                                me->SetDisplayId(MODEL_HARRISON_JONES_1);
                                Talk(SAY_HARRISON_2);
                                _gongTimer = 12000;
                                _gongEvent = GONG_EVENT_6;
                                break;
                            case GONG_EVENT_6:
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                                Talk(SAY_HARRISON_3);
                                _gongTimer = 7000;
                                _gongEvent = GONG_EVENT_7;
                                break;
                            case GONG_EVENT_7:
                            {
                                if (auto gate = me->GetMap()->GetGameObject(instance->GetData64(GO_MASSIVE_GATE)))
                                    gate->SetGoState(GO_STATE_ACTIVE);
                                _gongTimer = 2000;
                                _gongEvent = GONG_EVENT_8;
                                break;
                            }
                            case GONG_EVENT_8:
                            {
                                //                                DoCast(me, SPELL_STEALTH);
                                std::list<Creature*> targetList;
                                GetCreatureListWithEntryInGrid(targetList, me, NPC_AMANISHI_GUARDIAN, 26.0f);
                                if (!targetList.empty())
                                {
                                    for (std::list<Creature*>::const_iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                                    {
                                        if (auto ptarget = *itr)
                                        {
                                            ptarget->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(WEAPON_SPEAR));
                                            ptarget->CastWithDelay(2000, me, SPELL_COSMETIC_SPEAR_THROW, true);
                                        }
                                    }
                                }
                                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(0));
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                                me->GetMotionMaster()->MovePath(HARRISON_MOVE_3, false);
                                _gongTimer = 1000;
                                _gongEvent = 0;
                                break;
                            }
                            case GONG_EVENT_9:
                                me->GetMotionMaster()->MovePoint(0, 120.687f, 1674.0f, 42.0217f);
                                _gongTimer = 12000;
                                _gongEvent = GONG_EVENT_10;
                                break;
                            case GONG_EVENT_10:
                                me->SetFacingTo(1.59044f);
                                _gongEvent = 11;
                                _gongTimer = 6000;
                                break;
                            case GONG_EVENT_11:
                                instance->SetBossState(DATA_GONGEVENT, NOT_STARTED);
                                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                                _gongEvent = 0;
                                _gongTimer = 1000;
                                break;
                        }
                    }
                    else
                    _gongTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_harrison_jonesAI(creature);
        }
};


class npc_ZA_door_trigger : public CreatureScript
{
public:

    npc_ZA_door_trigger()
        : CreatureScript("npc_ZA_door_trigger")
    {
    }

    struct npc_ZA_door_triggerAI : public ScriptedAI
    {
        npc_ZA_door_triggerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        bool event_triggered = false;
        uint8 TriggerID = NULL;
        void InitializeAI() override
        {
            EstablishTriggerID();

            if (auto hexlord = Creature::GetCreature(*me, instance->GetData64(NPC_HEXLORD)))
            {
                //Hexlord has not been spawned yet.
            }
            else
            {
                me->SummonCreature(NPC_HEXLORD, ZA_Spawns[TRIGGER_HEXLORD_SPAWN], TEMPSUMMON_MANUAL_DESPAWN);
            }
        }

        void EstablishTriggerID()
        {
            for (uint8 i = TRIGGER_HEXLORD_SPAWN; i != TRIGGER_MAX; ++i)
                if (me->GetDistance(ZA_Spawns[i]) < 5.00f)
                    TriggerID = i;
        }

        void Reset()
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        void sGossipSelect(Player* player, uint32 sender, uint32 action)
        {
        }

        void SpellHit(Unit*, const SpellInfo* spell)
        {
        }

        void SetData(uint32 id, uint32 value)
        {
        }


        void DoAction(const int32 action) override
        {
            if (action == ACTION_RECOGNIZE_GONG)
            {
                if (auto hexlord = Creature::GetCreature(*me, instance->GetData64(NPC_HEXLORD)))
                    hexlord->AI()->DoAction(1);
                events.ScheduleEvent(EVENT_TONIGHT_WE_FEAST, 8000);

                if (auto yellGuard = me->FindNearestCreature(NPC_AMANISHI_GUARDIAN, 100.0f))
                    yellGuard->AI()->DoAction(420);
                else
                    TC_LOG_ERROR("sql.sql", "trigger could not find guardian.");

            }

        }

        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        void MoveInLineOfSight(Unit* who) 
        {

            if (auto player = me->FindNearestPlayer(10.0f))
                if (!event_triggered)
                {
                    event_triggered = true;
                    //TC_LOG_ERROR("misc", "Trigger %u found player in range", TriggerID);
                    switch (TriggerID)
                    {
                    case 126263:
                        break;
                    case 312375:
                        if (auto lookout = me->FindNearestCreature(NPC_AMANASHI_LOOKOUT, 45.0f))
                            lookout->AI()->EnterCombat(player->ToUnit());
                        break;
                    default:
                        break;
                    }
                }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TONIGHT_WE_FEAST:
                    if (auto hexlord = Creature::GetCreature(*me, instance->GetData64(NPC_HEXLORD)))
                        hexlord->AI()->DoAction(2);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        std::list<Creature*> units;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ZA_door_triggerAI(creature);
    }
};

class npc_ZA_VolJin : public CreatureScript
{
public:

    npc_ZA_VolJin()
        : CreatureScript("npc_ZA_VolJin")
    {
    }

    struct npc_ZA_VolJinAI : public ScriptedAI
    {
        npc_ZA_VolJinAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }
        void InitializeAI() override
        {
            if (me->GetPositionX() < 1610.f)//entrance voljin location
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            started = false;
        }

        void Reset()
        {
            started = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            if (!player || !creature)
            {
                //TC_LOG_ERROR("sql.sql", "custom transmog npc - player or creature NULL check");
                return false;
            }
        }
        void sGossipSelect(Player* player, uint32 sender, uint32 action)
        {
            switch (action)
            {
            case 0://we're ready
                if (started)
                    return;

                initiator = player;
                initiator_class = player->getClass();
                mountID = me->GetMountID();
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                player->PlayerTalkClass->ClearMenus();
                player->CLOSE_GOSSIP_MENU();
                events.ScheduleEvent(EVENT_DISMOUNT, 2000);
                started = true;
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit*, const SpellInfo* spell)
        {
        }

        void SetData(uint32 id, uint32 value)
        {
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                switch (id)
                {
                case EVENT_POST_GONG_2:
                    me->SetFacingTo(Position::NormalizeOrientation(4.70f));
                    events.ScheduleEvent(EVENT_POST_GONG_2, 3000);
                    break;
                default:
                    break;
                }
        }


        void DoAction(const int32 action) override
        {
            switch (action)
            {
            case ACTION_RECOGNIZE_GONG:
                if (isRingingGong)
                    return;
                me->CastStop();
                me->RemoveAurasDueToSpell(45222);
                //events.ScheduleEvent(EVENT_POST_GONG_1, 3000);
                me->GetMotionMaster()->MovePoint(EVENT_POST_GONG_2, Yellspot, true, 3.0f, false);
                isRingingGong = true;
                break;
            default:
                break;
            }
        }
        Position Yellspot = { 120.768013f, 1633.795410f, 42.599533f, 4.682184f };
        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_DISMOUNT:
                    me->Dismount();
                    events.ScheduleEvent(EVENT_MOVE_GONG, 2000);
                    break;
                case EVENT_MOVE_GONG:
                        me->AI()->Talk(initiator ? initiator->getClass() : initiator_class);
                    me->GetMotionMaster()->MovePoint(0, 132.33f, 1644.71f, 42.02f, true, 3.0f, false);
                    events.ScheduleEvent(EVENT_TALK_GONG, 7000);
                    break;
                case EVENT_TALK_GONG:
                    me->AI()->Talk(12);
                    events.ScheduleEvent(EVENT_START_GONG, 3000);
                    break;
                case EVENT_START_GONG:
                    if (auto gong = me->FindNearestGameObject(187359, 15.0f))
                    {
                        gong->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        //gong->Use(me->ToUnit());
                        DoCast(me, 45222, false);
                    }
                    if (!isRingingGong)
                        events.ScheduleEvent(EVENT_START_GONG, 3000);
                    break;
                case EVENT_POST_GONG_1:
                    break;
                case EVENT_POST_GONG_2:
                    me->AI()->Talk(13);
                    events.ScheduleEvent(EVENT_POST_GONG_3, 4000);
                    break;
                case EVENT_POST_GONG_3:
                    if (auto gate = me->FindNearestGameObject(186728, 80.0f))
                    {
                        gate->Use(me->ToUnit());
                    }
                    me->Mount(mountID);
                    events.ScheduleEvent(EVENT_POST_GONG_4, 4000);
                    break;
                case EVENT_POST_GONG_4:
                {
                    me->AI()->Talk(14);
                    instance->SetBossState(DATA_GONGEVENT, DONE);
                    if (auto c = me->FindNearestCreature(NPC_ZA_DOOR_TRIGGER, 50.f))
                        c->AI()->DoAction(ACTION_RECOGNIZE_GONG);
                    break;

                }
                default:
                    break;
                }
        }
    private:
        bool isRingingGong = false;
        bool started = false;
        uint32 mountID;
        Player* initiator{nullptr};
        uint8 initiator_class{ CLASS_WARRIOR };
        InstanceScript* instance;
        EventMap events{};
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ZA_VolJinAI(creature);
    }
};

class spell_ZA_Ring_Gong : public SpellScriptLoader
{
public:
    spell_ZA_Ring_Gong() : SpellScriptLoader("spell_ZA_Ring_Gong") { }

    class spell_ZA_Ring_Gong_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ZA_Ring_Gong_SpellScript);

        void Handle_Gong_Ring()
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;
            if (!caster->ToPlayer())
                return;
            caster->RemoveAurasDueToSpell(45222);
            std::list<Creature*> units;
            GetCreatureListWithEntryInGrid(units, caster, NPC_VOLJIN_ZA, 20.0f);
            for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                (*itr)->AI()->DoAction(ACTION_RECOGNIZE_GONG);

            caster->CastStop();
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_ZA_Ring_Gong_SpellScript::Handle_Gong_Ring);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ZA_Ring_Gong_SpellScript();
    }
};

class spell_banging_the_gong : public SpellScriptLoader
{
    public:
        spell_banging_the_gong() : SpellScriptLoader("spell_banging_the_gong") { }

        class spell_banging_the_gong_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_banging_the_gong_SpellScript);

            void Activate(SpellEffIndex index)
            {
                PreventHitDefaultEffect(index);
                GetHitGObj()->SendCustomAnim(0);
                //                if (Creature *c = GetHitGObj()->FindNearestCreature(NPC_HARRISON_JONES, 100))
                //    c->AI()->SetData(DATA_HIT_GONG, 1);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_banging_the_gong_SpellScript::Activate, EFFECT_1, SPELL_EFFECT_ACTIVATE_OBJECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_banging_the_gong_SpellScript();
        }
};


class npc_Amanashi_guardian : public CreatureScript
{
public:
    npc_Amanashi_guardian() : CreatureScript("npc_Amanashi_guardian") { }

    struct npc_Amanashi_guardianAI : public ScriptedAI
    {
        npc_Amanashi_guardianAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->CallForHelp(10.f);
            if (!intro_yell)
                Talk(1);
        }
        void Reset()
        {
            me->DeleteThreatList();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if (id == 420)
                {
                    events.ScheduleEvent(499, 500);
                }
        }

        void DoAction(const int32 action) override
        {
            if (action == 420)
            {
                intro_yell = true;
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                if (auto trigger = me->FindNearestCreature(NPC_ZA_DOOR_TRIGGER, 20.f))
                    me->GetMotionMaster()->MovePoint(420, *trigger, true);

            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 499:
                    Talk(ZA_GUARD_TALK_SOUND_ALARM, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                    if (auto p = me->FindNearestPlayer(100.f))
                    {
                        me->SetFacingTo(me->GetAngle(p));
                        me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    }
                    events.ScheduleEvent(500, 2000);
                    break;
                case 500:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (auto p = me->FindNearestPlayer(50.f))
                    {
                        AttackStart(p);
                        UpdateVictim();

                        std::list<Creature*> list_of_npcs;
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, 23889, 100.0f);
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, me->GetEntry(), 50.0f);
                        if (list_of_npcs.size())
                            for (auto c : list_of_npcs)
                            {
                                c->AI()->AttackStart(p);
                                c->GetMotionMaster()->MoveChase(p);
                                c->AddThreat(p, 5.f);
                            }
                    }
                    break;
                }
            }
            if (!me->HasReactState(REACT_PASSIVE))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        bool intro_yell{ false };

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Amanashi_guardianAI(creature);
    }
};


class npc_zandalari_archon : public CreatureScript
{
public:
    npc_zandalari_archon() : CreatureScript("npc_zandalari_archon") { }

    struct npc_zandalari_archonAI : public ScriptedAI
    {
        npc_zandalari_archonAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.ScheduleEvent(1, urand(5000, 15000));
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.CancelEvent(1);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    Talk(1);
                    events.ScheduleEvent(1, urand(10000, 20000));
                    break;
                }
            }
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zandalari_archonAI(creature);
    }
};

class npc_za_savage : public CreatureScript
{
public:
    npc_za_savage() : CreatureScript("npc_za_savage") { }

    struct npc_za_savageAI : public ScriptedAI
    {
        npc_za_savageAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(1, 4000);
            events.ScheduleEvent(2, 15000);
            events.ScheduleEvent(3, 10000);
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
                        DoCast(11971);
                        events.ScheduleEvent(1, 8000);
                    }
                    break;
                case 2:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(12054);
                        events.ScheduleEvent(1, 25000);
                    }
                    break;
                case 3:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(9080);
                        events.ScheduleEvent(1, 10000);
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
        return new npc_za_savageAI(creature);
    }
};

void AddSC_zulaman()
{
    new npc_forest_frog();
    new npc_zulaman_kasha();
    new npc_zulaman_bakkalzu();
    new npc_zulaman_hazlek();
    new npc_zulaman_norkani();
    new npc_harrison_jones();
    new spell_banging_the_gong();


    new spell_ZA_Ring_Gong();
    new npc_ZA_VolJin();
    new npc_ZA_door_trigger();
    new npc_Amanashi_guardian();
    new npc_zandalari_archon();
    new npc_za_savage();
}
