#include "GameObjectAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
// #include "gilneas.h"

enum GilneasDefaultSpells
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
    SPELL_FLIGHT_SPEED_180 = 47433,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum GilneasEvents
{
    EVENT_DEFEND_CROWLEY_1 = 1,
    EVENT_DEFEND_CROWLEY_2,
    EVENT_DEFEND_CROWLEY_3,
    EVENT_DEFEND_CROWLEY_4,
    EVENT_DEFEND_CROWLEY_5,
    EVENT_DEFEND_CROWLEY_6,
    EVENT_DEFEND_CROWLEY_7,
    EVENT_KING_HORSE_1,
    EVENT_MOUNTAIN_HORSE_1,
    EVENT_MOUNTAIN_HORSE_2,
    EVENT_CARRIAGE_1,
    EVENT_STUNT_HORSE_1,
    EVENT_CAPTURED_BAT_1,
    EVENT_LIAM_SCENE_1,
    EVENT_LIAM_SCENE_2,
};

enum QuestByTheSkin
{
    NPC_WORGEN_ALPHA = 35170,
    NPC_WORGEN_RUNT = 35188,

    SPELL_BY_THE_SKIN_AURA = 68218,
    SPELL_REBEL_VALOR = 67503,

    QUEST_BY_THE_SKIN_OF_HIS_TEETH = 14154,
};

const Position WorgenSpawnPosition[] =
{
    { -1632.79f, 1485.66f, 74.94f, 3.76f},
    { -1717.72f, 1486.31f, 57.22f, 5.37f},
    { -1594.07f, 1408.40f, 72.84f, 2.83f},
};

class npc_by_the_skin_of_his_teeth_controller : public CreatureScript
{
public:
    npc_by_the_skin_of_his_teeth_controller() : CreatureScript("npc_by_the_skin_of_his_teeth_controller") { }

    struct npc_by_the_skin_of_his_teeth_controllerAI : public ScriptedAI
    {
        npc_by_the_skin_of_his_teeth_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            _events.CancelEventGroup(1);
            me->setActive(true);
            _started = false;
            _summons.DespawnAll();
            _finishTimer = 119000;
            _checkTimer = 6000;

            if (me->HasAura(SPELL_REBEL_VALOR))
                me->RemoveAura(SPELL_REBEL_VALOR);

            auto player = me->GetPlayersInRange(150, true);
            for (auto it = player.begin(); it != player.end(); it++)
                if ((*it)->HasAura(SPELL_BY_THE_SKIN_AURA))
                    (*it)->RemoveAura(SPELL_BY_THE_SKIN_AURA);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = who->ToPlayer())
                {
                    if (!_started)
                    {
                        _started = true;
                        player->CastSpell(player, SPELL_BY_THE_SKIN_AURA);
                        me->CastSpell(me, SPELL_REBEL_VALOR);
                        me->AI()->Talk(0);

                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_1, 4000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_2, 18000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_3, 24000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_4, 46000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_5, 52000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_6, 73000, 1);
                        _events.ScheduleEvent(EVENT_DEFEND_CROWLEY_7, 85000, 1);
                    }
                    else
                    {
                        me->AI()->Talk(1, player->GetGUID());
                        player->CastSpell(player, SPELL_BY_THE_SKIN_AURA);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_started)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 2000;

                    if (!me->FindNearestPlayer(55.f))
                        Reset();
                }
                else _checkTimer -= diff;

                // after 119000 ms
                if (_finishTimer <= diff)
                {
                    _finishTimer = 5000;

                    auto player = me->GetPlayersInRange(100, true);
                    for (auto it = player.begin(); it != player.end(); it++)
                        if ((*it)->GetQuestStatus(QUEST_BY_THE_SKIN_OF_HIS_TEETH) == QUEST_STATUS_INCOMPLETE ||
                            (*it)->GetQuestStatus(QUEST_BY_THE_SKIN_OF_HIS_TEETH) == QUEST_STATUS_COMPLETE)
                        {
                            (*it)->AreaExploredOrEventHappens(QUEST_BY_THE_SKIN_OF_HIS_TEETH);
                            (*it)->RemoveAura(SPELL_BY_THE_SKIN_AURA);
                            Reset();
                        }
                }
                else _finishTimer -= diff;
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DEFEND_CROWLEY_1:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto runt6 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            if (auto runt7 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                {
                                                    _summons.Summon(runt1);
                                                    _summons.Summon(runt2);
                                                    _summons.Summon(runt3);
                                                    _summons.Summon(runt4);
                                                    _summons.Summon(runt5);
                                                    _summons.Summon(runt6);
                                                    _summons.Summon(runt7);
                                                    _summons.Summon(alpha1);
                                                    runt1->AI()->SetData(1, 1);
                                                    runt2->AI()->SetData(1, 1);
                                                    runt3->AI()->SetData(1, 1);
                                                    runt4->AI()->SetData(1, 1);
                                                    runt5->AI()->SetData(1, 1);
                                                    runt6->AI()->SetData(1, 1);
                                                    runt7->AI()->SetData(1, 1);
                                                    alpha1->AI()->SetData(1, 1);
                                                }
                    break;
                case EVENT_DEFEND_CROWLEY_2:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    {
                                        _summons.Summon(runt1);
                                        _summons.Summon(runt2);
                                        _summons.Summon(runt3);
                                        _summons.Summon(runt4);
                                        _summons.Summon(alpha1);
                                        runt1->AI()->SetData(1, 2);
                                        runt2->AI()->SetData(1, 2);
                                        runt3->AI()->SetData(1, 2);
                                        runt4->AI()->SetData(1, 2);
                                        alpha1->AI()->SetData(1, 2);
                                    }
                    break;
                case EVENT_DEFEND_CROWLEY_3:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto runt6 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            if (auto runt7 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                if (auto runt8 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                    if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                    {
                                                        _summons.Summon(runt1);
                                                        _summons.Summon(runt2);
                                                        _summons.Summon(runt3);
                                                        _summons.Summon(runt4);
                                                        _summons.Summon(runt5);
                                                        _summons.Summon(runt6);
                                                        _summons.Summon(runt7);
                                                        _summons.Summon(runt8);
                                                        _summons.Summon(alpha1);
                                                        runt1->AI()->SetData(1, 2);
                                                        runt2->AI()->SetData(1, 2);
                                                        runt3->AI()->SetData(1, 2);
                                                        runt4->AI()->SetData(1, 2);
                                                        runt5->AI()->SetData(1, 2);
                                                        runt6->AI()->SetData(1, 2);
                                                        runt7->AI()->SetData(1, 2);
                                                        runt8->AI()->SetData(1, 2);
                                                        alpha1->AI()->SetData(1, 2);
                                                    }
                    break;
                case EVENT_DEFEND_CROWLEY_4:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto runt6 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            {
                                                _summons.Summon(runt1);
                                                _summons.Summon(runt2);
                                                _summons.Summon(runt3);
                                                _summons.Summon(runt4);
                                                _summons.Summon(runt5);
                                                _summons.Summon(runt6);
                                                _summons.Summon(alpha1);
                                                runt1->AI()->SetData(1, 3);
                                                runt2->AI()->SetData(1, 3);
                                                runt3->AI()->SetData(1, 3);
                                                runt4->AI()->SetData(1, 3);
                                                runt5->AI()->SetData(1, 3);
                                                runt6->AI()->SetData(1, 3);
                                                alpha1->AI()->SetData(1, 3);
                                            }
                    break;
                case EVENT_DEFEND_CROWLEY_5:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        {
                                            _summons.Summon(runt1);
                                            _summons.Summon(runt2);
                                            _summons.Summon(runt3);
                                            _summons.Summon(runt4);
                                            _summons.Summon(runt5);
                                            _summons.Summon(alpha1);
                                            runt1->AI()->SetData(1, 3);
                                            runt2->AI()->SetData(1, 3);
                                            runt3->AI()->SetData(1, 3);
                                            runt4->AI()->SetData(1, 3);
                                            runt5->AI()->SetData(1, 3);
                                            alpha1->AI()->SetData(1, 3);
                                        }
                    break;
                case EVENT_DEFEND_CROWLEY_6:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto runt6 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            {
                                                _summons.Summon(runt1);
                                                _summons.Summon(runt2);
                                                _summons.Summon(runt3);
                                                _summons.Summon(runt4);
                                                _summons.Summon(runt5);
                                                _summons.Summon(runt6);
                                                _summons.Summon(alpha1);
                                                runt1->AI()->SetData(1, 1);
                                                runt2->AI()->SetData(1, 1);
                                                runt3->AI()->SetData(1, 1);
                                                runt4->AI()->SetData(1, 1);
                                                runt5->AI()->SetData(1, 1);
                                                runt6->AI()->SetData(1, 1);
                                                alpha1->AI()->SetData(1, 1);
                                            }
                    break;
                case EVENT_DEFEND_CROWLEY_7:
                    if (auto runt1 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                        if (auto runt2 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                            if (auto runt3 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                if (auto runt4 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                    if (auto runt5 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                        if (auto runt6 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                            if (auto runt7 = me->SummonCreature(NPC_WORGEN_RUNT, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                if (auto alpha1 = me->SummonCreature(NPC_WORGEN_ALPHA, WorgenSpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN, 85000))
                                                {
                                                    _summons.Summon(runt1);
                                                    _summons.Summon(runt2);
                                                    _summons.Summon(runt3);
                                                    _summons.Summon(runt4);
                                                    _summons.Summon(runt5);
                                                    _summons.Summon(runt6);
                                                    _summons.Summon(runt7);
                                                    _summons.Summon(alpha1);
                                                    runt1->AI()->SetData(1, 1);
                                                    runt2->AI()->SetData(1, 1);
                                                    runt3->AI()->SetData(1, 1);
                                                    runt4->AI()->SetData(1, 1);
                                                    runt5->AI()->SetData(1, 1);
                                                    runt6->AI()->SetData(1, 1);
                                                    runt7->AI()->SetData(1, 1);
                                                    alpha1->AI()->SetData(1, 1);
                                                }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _started = false;
        uint16 _checkTimer = 6000;
        uint64 _finishTimer = 119000;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_by_the_skin_of_his_teeth_controllerAI(creature);
    }
};

enum QuestMastiffPet
{
    QUEST_FROM_THE_SHADOWS = 14204,

    SPELL_ATTACK_LURKER = 67805,
};

class npc_gilnean_mastiff_pet : public CreatureScript
{
public:
    npc_gilnean_mastiff_pet() : CreatureScript("npc_gilnean_mastiff_pet") { }

    struct npc_gilnean_mastiff_petAI : public ScriptedAI
    {
        npc_gilnean_mastiff_petAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.5f * M_PI);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->AI()->AttackStart(who);

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    player->Attack(who, true);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                        me->DespawnOrUnsummon();

                    if (!me->getVictim() && me->GetReactState() != REACT_AGGRESSIVE && player->getVictim())
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->AI()->AttackStart(player->getVictim());
                    }

                    if (me->GetReactState() != REACT_PASSIVE && !player->getVictim())
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->CombatStop();
                        me->DeleteThreatList();
                        me->GetMotionMaster()->MoveFollow(player, 0.9f, 0.5f * M_PI);
                    }

                    if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_FROM_THE_SHADOWS) == QUEST_STATUS_NONE ||
                        me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_FROM_THE_SHADOWS) == QUEST_STATUS_REWARDED)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;
        }
    private:
        EventMap _events;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilnean_mastiff_petAI(creature);
    }
};

enum KingHorseDatas
{
    QUEST_SAVED_KRENNAN = 14293,

    SPELL_PING_SAVED_KRENNAN = 151174,
    SPELL_RESCUE_KRENNAN = 68228,

    NPC_KRENNAN_ARANAS = 35753,
    NPC_KRENNAN_ARANAS_SAVED = 35907,
};

uint32 const KingHorsePathSize = 7;
const G3D::Vector3 KingHorsePath[KingHorsePathSize] =
{
    { -1799.85f, 1400.92f, 19.90f },
    { -1793.70f, 1382.46f, 19.84f },
    { -1781.20f, 1363.38f, 19.79f },
    { -1768.12f, 1352.79f, 19.92f },
    { -1742.03f, 1360.28f, 19.78f },
    { -1722.16f, 1353.59f, 19.79f },
    { -1708.80f, 1348.28f, 19.78f }, // 6
};

uint32 const KingHorsePathSize2 = 14;
const G3D::Vector3 KingHorsePath2[KingHorsePathSize2] =
{
    { -1671.19f, 1341.73f, 15.135f },
    { -1665.35f, 1345.60f, 15.135f },
    { -1663.05f, 1350.47f, 15.135f },
    { -1664.16f, 1359.29f, 15.135f },
    { -1670.72f, 1362.11f, 15.135f },
    { -1679.50f, 1360.69f, 15.135f },
    { -1692.02f, 1351.74f, 15.135f },
    { -1703.93f, 1351.08f, 19.755f },
    { -1714.48f, 1351.79f, 19.782f },
    { -1739.10f, 1366.54f, 20.006f },
    { -1760.59f, 1395.22f, 19.913f },
    { -1766.14f, 1404.61f, 19.807f },
    { -1772.10f, 1417.43f, 19.782f },
    { -1771.86f, 1433.25f, 19.838f }, // 13
};

class npc_king_greymanes_horse : public CreatureScript
{
public:
    npc_king_greymanes_horse() : CreatureScript("npc_king_greymanes_horse") { }

    struct npc_king_greymanes_horseAI : public VehicleAI
    {
        npc_king_greymanes_horseAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetRooted(true);
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                player->CastWithDelay(150, me, SPELL_RIDE_VEHICLE);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply)
            {
                if (passenger->GetTypeId() == TYPEID_PLAYER)
                    _events.ScheduleEvent(EVENT_KING_HORSE_1, 2000);

                if (passenger->GetEntry() == NPC_KRENNAN_ARANAS_SAVED)
                {
                    me->SetRooted(false);
                    _step++;
                    me->GetMotionMaster()->MoveSmoothPath(KingHorsePath2, KingHorsePathSize2);
                }
            }
        }

        void JustDied(Unit* killer)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (auto krennan = Unit::GetCreature(*me, _savedKrennanGUID))
                    krennan->DespawnOrUnsummon();

                player->ExitVehicle();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_RESCUE_KRENNAN)
            {
                if (auto krennansaved = me->SummonCreature(NPC_KRENNAN_ARANAS_SAVED, -1674.95f, 1347.94f, 22.2527f, 0.764106f, TEMPSUMMON_TIMED_DESPAWN, 120000))
                {
                    _savedKrennanGUID = krennansaved->GetGUID();
                    krennansaved->CastWithDelay(100, me, SPELL_RIDE_VEHICLE);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_step == 0)
            {
                if (point == 6)
                {
                    me->GetMotionMaster()->MoveJump(-1681.62219f, 1346.7093f, 15.13f, 20.f, 15.f, 1);

                    if (auto krennan = me->FindNearestCreature(NPC_KRENNAN_ARANAS, 40.f))
                        if (auto krennan1 = krennan->AI())
                        {
                            krennan1->Talk(0);
                            me->AI()->TalkWithDelay(1500, 0, _playerGUID);
                        }
                }
            }

            if (_step == 1)
            {
                if (point == 13)
                {
                    me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (auto krennan = Unit::GetCreature(*me, _savedKrennanGUID))
                        {
                            player->ExitVehicle();
                            krennan->NearTeleportTo(-1772.452f, 1436.7318f, 19.7590f, 2.9347f);
                            krennan->CastWithDelay(1000, krennan, SPELL_DUMMY_PING);
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->DespawnOrUnsummon(1000);
                        }
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_KING_HORSE_1:
                    me->SetRooted(false);
                    _step = 0;
                    me->GetMotionMaster()->MoveSmoothPath(KingHorsePath, KingHorsePathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint8 _step = 0;
        uint64 _playerGUID = 0;
        uint64 _savedKrennanGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_greymanes_horseAI(creature);
    }
};

enum Stocks
{
    QUEST_LAST_STAND = 14222,
    QUEST_LAST_CHANCE_AT_HUMANITY = 14375,

    MOVIE_WORGEN_FORM = 21,

    NPC_LORD_GODFREY_STOCKS = 36330,
    NPC_KRENNAN_ARANAS_STOCKS = 36331,
    NPC_KING_GENN_GREYMANE_STOCKS = 36332,

    SPELL_PLAY_MOVIE = 93477,
    SPELL_FORCE_WORGEN_ALTERED_FORM = 98274,
    SPELL_HIDEOUS_BITE_WOUND = 76642,
    SPELL_CURSE_OF_THE_WORGEN = 68630,
    SPELL_IN_STOCKS = 69196,
    SPELL_LAST_STAND_COMPLETE = 72799,  // Learns Worgen Racials (except for Two Forms which will be learned later via quest)
    SPELL_QUEST_ZONE_SPECIFIC = 68481,
    SPELL_SUMMON_PERSONAL_GODFREY = 68635,
    SPELL_SUMMON_PERSONAL_GREYMANE = 68636,
};

class PlayerAlreadyStartWorgenZone2
{
public:
    PlayerAlreadyStartWorgenZone2() { }

    bool operator()(Player* player) const
    {
        return (player->GetQuestStatus(14375) != QUEST_STATUS_NONE || player->HasAura(68630));
    }
};

class StocksPlayerScript : public PlayerScript
{
public:
    StocksPlayerScript() : PlayerScript("StocksPlayerScript") {}

    void OnLogin(Player* player)
    {
        if (player->getRace() == RACE_WORGEN && player->GetAreaId() == 4786 && player->GetPhaseMask() == 4096)
            if (player->GetQuestRewardStatus(QUEST_LAST_STAND) && !player->GetQuestRewardStatus(QUEST_LAST_CHANCE_AT_HUMANITY))
            {
                if (player->FindNearestCreature(NPC_KRENNAN_ARANAS_STOCKS, 100.0f))
                {
                    auto krennan = player->SelectNearbyUnits(NPC_KRENNAN_ARANAS_STOCKS, 100.0f);
                    for (auto itr = krennan.begin(); itr != krennan.end(); ++itr)
                        if (auto krennan = (*itr))
                        {
                            if (krennan->GetOwnerGUID() == player->GetGUID())
                                return;
                            else if (krennan->GetOwnerGUID() != player->GetGUID())
                                player->CastSpell(player, SPELL_CURSE_OF_THE_WORGEN);
                        }
                }
                else if (!player->FindNearestCreature(NPC_KRENNAN_ARANAS_STOCKS, 100.0f))
                    player->CastSpell(player, SPELL_CURSE_OF_THE_WORGEN);
            }
    }
};

// quest=14212/sacrifices by Wondi
enum CrowleyEscortData
{   // NPC_HORSE = 35231,
    EVENT_START_HORSE_PATH_1 = 5,
    EVENT_JUMP_1 = 6,
    EVENT_AFTER_JUMP_1 = 7,
    EVENT_JUMP_2 = 8,
    EVENT_AFTER_JUMP_2 = 9,
    EVENT_END_PATH = 10,

    SPELL_INVISIBILITY_DETECTION_2_MISC_8 = 49417,
    SAY_TEXT_GR_0 = 0,

    NPC_CROWLEY_PASSENGER = 35230,
};

uint32 const CrowleysHorseVehiclePathSize1 = 2;
const G3D::Vector3 CrowleysHorseVehiclePath1[CrowleysHorseVehiclePathSize1] =
{
    { -1736.61f, 1658.84f, 20.48f }, // 0 
    { -1731.24f, 1659.45f, 20.48f }, // 1 jump
};

uint32 const CrowleysHorseVehiclePathSize2 = 15;
const G3D::Vector3 CrowleysHorseVehiclePath2[CrowleysHorseVehiclePathSize2] =
{
    { -1707.74f, 1674.33f, 20.56f }, // 0
    { -1685.93f, 1684.10f, 20.61f }, // 1
    { -1667.90f, 1667.73f, 20.48f }, // 2
    { -1670.10f, 1621.12f, 20.48f }, // 3
    { -1699.27f, 1592.87f, 20.48f }, // 4
    { -1716.76f, 1594.91f, 20.48f }, // 5
    { -1722.06f, 1608.05f, 20.48f }, // 6
    { -1696.48f, 1652.67f, 20.48f }, // 7
    { -1695.58f, 1696.04f, 20.19f }, // 8
    { -1669.93f, 1707.13f, 20.49f }, // 9
    { -1631.29f, 1711.24f, 20.48f }, // 10
    { -1624.15f, 1711.35f, 22.09f }, // 11
    { -1613.37f, 1711.46f, 22.68f }, // 12
    { -1596.80f, 1711.54f, 20.48f }, // 13
    { -1589.77f, 1711.36f, 20.48f }, // 14  jump
};

uint32 const CrowleysHorseVehiclePathSize3 = 23;
const G3D::Vector3 CrowleysHorseVehiclePath3[CrowleysHorseVehiclePathSize3] =
{
    { -1560.38f, 1707.39f, 20.485f, }, // 0 
    { -1545.91f, 1704.00f, 20.485f, }, // 1 
    { -1545.61f, 1667.73f, 20.486f, }, // 2 
    { -1541.53f, 1641.19f, 20.484f, }, // 3 
    { -1528.36f, 1623.41f, 20.485f, }, // 4 
    { -1493.87f, 1641.12f, 20.485f, }, // 5 
    { -1472.26f, 1649.06f, 20.485f, }, // 6 
    { -1458.14f, 1650.80f, 20.485f, }, // 7 
    { -1434.93f, 1622.39f, 20.485f, }, // 8 
    { -1412.84f, 1577.23f, 20.481f, }, // 9 
    { -1410.55f, 1557.72f, 20.472f, }, // 10
    { -1428.58f, 1545.91f, 20.598f, }, // 11
    { -1456.51f, 1524.52f, 20.596f, }, // 12
    { -1473.89f, 1520.94f, 20.632f, }, // 13
    { -1487.81f, 1528.81f, 20.744f, }, // 14
    { -1486.06f, 1537.53f, 20.621f, }, // 15
    { -1484.54f, 1555.14f, 20.878f, }, // 16
    { -1500.14f, 1562.72f, 20.487f, }, // 17
    { -1499.64f, 1569.71f, 20.485f, }, // 18
    { -1505.43f, 1577.94f, 20.663f, }, // 19
    { -1515.92f, 1575.88f, 26.085f, }, // 20
    { -1533.57f, 1580.88f, 26.739f, }, // 21
    { -1541.82f, 1574.20f, 29.204f, }  // 22
};

class npc_crowleys_horse_vehicle : public CreatureScript
{
public:
    npc_crowleys_horse_vehicle() : CreatureScript("npc_crowleys_horse_vehicle") { }

    struct npc_crowleys_horse_vehicleAI : public VehicleAI
    {
        npc_crowleys_horse_vehicleAI(Creature* creature) : VehicleAI(creature) {}

        uint32 pathstep = 0;

        void IsSummonedBy(Unit* summoner)
        {
            if (summoner->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = summoner->ToPlayer())
                    _playerGUID = player->GetGUID();
            }
            pathstep = 1;
            me->AI()->Talk(0);

            if (auto crowley = me->SummonCreature(NPC_CROWLEY_PASSENGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000))
            {
                _crowleyGUID = crowley->GetGUID();
                crowley->EnterVehicle(me, 1);
            }
        }
        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                {
                    crowley->AI()->Talk(0);
                    _events.ScheduleEvent(EVENT_START_HORSE_PATH_1, 1000);
                }
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->DespawnOrUnsummon(1000);

                if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                    crowley->DespawnOrUnsummon();
            }
        }

        void JustDied(Unit* killer)
        {
            me->CastSpell(me, SPELL_EJECT_ALL_PASSENGERS);

            if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                crowley->DespawnOrUnsummon();
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                if (pathstep == 1)
                {
                    _events.ScheduleEvent(EVENT_JUMP_1, 2000);
                    pathstep++;
                }
                break;
            case 14:
                if (pathstep == 2)
                {
                    _events.ScheduleEvent(EVENT_JUMP_2, 2000);
                    pathstep++;
                }
                break;
            case 22:
                if (pathstep == 3)
                    _events.ScheduleEvent(EVENT_END_PATH, 1000);
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
                case EVENT_START_HORSE_PATH_1:
                    me->GetMotionMaster()->MoveSmoothPath(CrowleysHorseVehiclePath1, CrowleysHorseVehiclePathSize1);
                    break;
                case EVENT_JUMP_1:
                    me->GetMotionMaster()->MoveJump(-1718.71f, 1662.46f, 20.50f, 15.0f, 20.0f);
                    _events.ScheduleEvent(EVENT_AFTER_JUMP_1, 1500);
                    break;
                case EVENT_AFTER_JUMP_1:
                    me->GetMotionMaster()->MoveSmoothPath(CrowleysHorseVehiclePath2, CrowleysHorseVehiclePathSize2);
                    break;
                case EVENT_JUMP_2:
                    me->GetMotionMaster()->MoveJump(-1576.42f, 1713.106f, 20.484f, 15.0f, 20.0f);
                    _events.ScheduleEvent(EVENT_AFTER_JUMP_2, 1500);
                    break;
                case EVENT_AFTER_JUMP_2:
                    me->GetMotionMaster()->MoveSmoothPath(CrowleysHorseVehiclePath3, CrowleysHorseVehiclePathSize3);
                    break;
                case EVENT_END_PATH:
                    me->CastSpell(me, 50630);
                    me->DespawnOrUnsummon(200);
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                        crowley->DespawnOrUnsummon();
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->CastSpell(player, 88467);
                        player->GetMotionMaster()->MoveJump(-1543.589f, 1572.651f, 29.202f, 10, 10);
                    }
                    break;
                default:
                    break;
                }
            }

            if (me->isInCombat())
                if (_sayInCombat <= diff)
                {
                    if (auto crowley = Unit::GetCreature(*me, _crowleyGUID))
                    {
                        _sayInCombat = urand(25000, 40000);
                        crowley->AI()->Talk(1);
                    }
                }
                else _sayInCombat -= diff;
        }
    private:
        uint32 _sayInCombat = 5000;

        EventMap _events;
        uint64 _playerGUID = 0;
        uint64 _crowleyGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crowleys_horse_vehicleAI(creature);
    }
};

enum CrowleysHorseSpellclick
{
    SPELL_DUMMY = 56685,
    EVENT_DELAY_1 = 1,
    EVENT_DELAY_2 = 2,

    NPC_CROWLEY_HORSE = 35231,
};

class npc_crowleys_horse_spellclick : public CreatureScript
{
public:
    npc_crowleys_horse_spellclick() : CreatureScript("npc_crowleys_horse_spellclick") { }

    struct npc_crowleys_horse_spellclickAI : public ScriptedAI
    {
        npc_crowleys_horse_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY)
            {
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
                _events.ScheduleEvent(EVENT_DELAY_1, 3000);

                if (auto horse = who->SummonCreature(NPC_CROWLEY_HORSE, -1737.68f, 1655.10f, 20.56f, 1.64f, TEMPSUMMON_DEAD_DESPAWN, 5000))
                {
                    who->RemoveAura(SPELL_INVISIBILITY_DETECTION_2_MISC_8);
                    who->CastWithDelay(1000, horse, 46598);
                }
            }
        }
        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DELAY_1:
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    break;
                default:
                    break;
                }
            }
        }


        void DamageTaken(Unit* attacker, uint32& damage)
        {
            float numerator{ float(me->GetHealthPct()) };
            float denominator{ 100.f };
            float dmg = float(damage) * (numerator / denominator);
            damage = (me->HealthBelowPctDamaged(5, damage) ? 0 : uint32(dmg));
        }

    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crowleys_horse_spellclickAI(creature);
    }
};

enum DrowningWatchman
{
    SPELL_SETVEHICLEID = 68735,
    SPELL_SPELLCLICK = 56685,
    SPELL_DROWNING = 68730,
    NPC_GREYMANE_2 = 36451,
    NPC_GASPING_CREDIT = 36450,
    QUEST_GASPING = 14395,
};

class npc_drowning_watchtman_spellclick : public CreatureScript
{
public:
    npc_drowning_watchtman_spellclick() : CreatureScript("npc_drowning_watchtman_spellclick") { }

    struct npc_drowning_watchtman_spellclickAI : public ScriptedAI
    {
        npc_drowning_watchtman_spellclickAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_SPELLCLICK && !_onBack)
            {
                if (auto player = who->ToPlayer())
                {
                    _playerGUID = player->GetGUID();
                    player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                    player->CastSpell(player, SPELL_SETVEHICLEID);
                    me->CastWithDelay(100, player, 47020);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    _onBack = true;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_onBack)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->FindNearestCreature(NPC_GREYMANE_2, 8.f))
                        {
                            if (!player->IsInWater())
                            {
                                me->RemoveAura(SPELL_DROWNING);
                                me->SetFacingToObject(player);
                                me->AI()->TalkWithDelay(1000, 0, _playerGUID);
                                player->KilledMonsterCredit(NPC_GASPING_CREDIT);
                                player->RemoveAura(SPELL_SETVEHICLEID);
                                me->DespawnOrUnsummon(5000);
                                _onBack = false;
                                _playerGUID = 0;
                            }
                        }

                        if (!player->isAlive() || !player->HasAura(SPELL_SETVEHICLEID))
                        {
                            player->RemoveAura(SPELL_SETVEHICLEID);
                            me->DespawnOrUnsummon();
                        }

                        if (player->GetQuestStatus(QUEST_GASPING) == QUEST_STATUS_NONE)
                        {
                            player->RemoveAura(SPELL_SETVEHICLEID);
                            me->DespawnOrUnsummon();
                        }

                        if (me->GetDistance(player) > 30.f)
                        {
                            player->RemoveAura(SPELL_SETVEHICLEID);
                            me->DespawnOrUnsummon();
                        }
                    }
                }
                else _checkTimer -= diff;
            }
        }

    private:
        bool _onBack = false;
        uint64  _playerGUID = 0;
        uint32 _checkTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drowning_watchtman_spellclickAI(creature);
    }
};

enum QuestHungryEttin
{
    QUEST_THE_HUNGRY_ETTIN = 14416,

    SPELL_ROUND_UP_HORSE = 68903,
    SPELL_ROUND_UP_HORSE_STACK = 68908,
    SPELL_THE_HUNGRY_ETTIN_CREDIT = 68917,
    SPELL_ROPE_CHANNEL = 68940,
    SPELL_LORNA_PING_HORSE = 68916,
    SPELL_CUSTOM_AURA_1 = 151100,

    NPC_CREDIT_HORSE = 36560,
    NPC_SUMMONED_MOUNTAIN_HORSE = 36555,
};

class npc_mountain_horse_worgen : public CreatureScript
{
public:
    npc_mountain_horse_worgen() : CreatureScript("npc_mountain_horse_worgen") { }

    struct npc_mountain_horse_worgenAI : public VehicleAI
    {
        npc_mountain_horse_worgenAI(Creature* creature) : VehicleAI(creature) {}

        void Reset()
        {
            me->RemoveAura(SPELL_CUSTOM_AURA_1);
            _inUse = false;
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _inUse = true;

                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _inUse = false;
                passenger->RemoveAura(SPELL_ROUND_UP_HORSE_STACK);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                if (me->HasAura(SPELL_CUSTOM_AURA_1))
                    me->DespawnOrUnsummon(7000);
                else
                    me->DespawnOrUnsummon();
            }
        }

        void JustDied(Unit* killer)
        {
            _inUse = false;
            me->RemoveAura(SPELL_CUSTOM_AURA_1);

            if (auto player = Unit::GetPlayer(*me, _playerGUID))
                player->RemoveAura(SPELL_ROUND_UP_HORSE_STACK);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_ROUND_UP_HORSE)
            {
                if (auto myself = me->GetVehicleKit())
                    if (myself->IsVehicleInUse())
                        return;
                    else
                    {
                        me->SetFacingToObject(who);
                        me->DespawnOrUnsummon(100);
                    }
            }

            if (spellInfo->Id == SPELL_LORNA_PING_HORSE)
            {
                if (_inUse)
                {
                    _inUse = false;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->CastSpell(me, SPELL_CUSTOM_AURA_1);
                        player->ExitVehicle();
                        player->KilledMonsterCredit(NPC_CREDIT_HORSE);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) { }

    private:
        bool _inUse = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mountain_horse_worgenAI(creature);
    }
};

class npc_summoned_mountain_horse_worgen : public CreatureScript
{
public:
    npc_summoned_mountain_horse_worgen() : CreatureScript("npc_summoned_mountain_horse_worgen") { }

    struct npc_summoned_mountain_horse_worgenAI : public ScriptedAI
    {
        npc_summoned_mountain_horse_worgenAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);

            if (auto player = summoner->ToPlayer())
            {
                _playerGUID = player->GetGUID();
                _events.ScheduleEvent(EVENT_MOUNTAIN_HORSE_1, 100);

                if (auto veh = player->GetVehicleBase())
                    _vehGUID = veh->GetGUID();
            }
            else
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (auto veh = Unit::GetCreature(*me, _vehGUID))
                    {
                        if (!player->GetVehicleBase())
                        {
                            if (!veh->GetAura(SPELL_CUSTOM_AURA_1))
                            {
                                player->RemoveAura(SPELL_ROPE_CHANNEL);
                                me->DespawnOrUnsummon();
                            }
                            else
                            {
                                player->RemoveAura(SPELL_ROPE_CHANNEL);
                                player->KilledMonsterCredit(NPC_CREDIT_HORSE);
                                me->DespawnOrUnsummon(8000);
                                _events.ScheduleEvent(EVENT_MOUNTAIN_HORSE_2, 2500);
                            }
                        }
                    }

                    if (me->GetDistance(player) > 100.f)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_MOUNTAIN_HORSE_1:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->CastSpell(player, SPELL_ROPE_CHANNEL); // without visual rope (spell bug: channeling stucking horses and they do not follow)
                        me->GetMotionMaster()->MoveFollow(player, frand(1.0, 3.0f), frand(0.1f, 3.f) * M_PI);
                    }
                    break;
                case EVENT_MOUNTAIN_HORSE_2:
                    me->CastSpell(me, SPELL_ROOT);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _saved = false;
        uint32 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        uint64 _vehGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_summoned_mountain_horse_worgenAI(creature);
    }
};

class spell_round_up_horse : public SpellScriptLoader
{
public:
    spell_round_up_horse() : SpellScriptLoader("spell_round_up_horse") { }

    class spell_round_up_horse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_round_up_horse_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_ROUND_UP_HORSE_STACK))
                return false;
            return true;
        }

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (caster->GetAuraCount(SPELL_ROUND_UP_HORSE_STACK) >= 4)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            else
                return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_round_up_horse_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_round_up_horse_SpellScript();
    }
};

enum QuestToGreymaneManor
{
    QUEST_TO_GREYMANE_MANOR = 14465,
    OBJECT_GREYMANE_GATE = 196401,
};

uint32 const ToGreymaneManorPathSize = 14;
const G3D::Vector3 ToGreymaneManorPath[ToGreymaneManorPathSize] =
{
    { -1896.22f, 2256.22f, 42.323f },
    { -1864.83f, 2287.13f, 42.120f },
    { -1853.79f, 2302.15f, 42.000f },
    { -1834.93f, 2319.85f, 38.337f },
    { -1812.57f, 2337.53f, 36.049f },
    { -1785.99f, 2366.07f, 39.262f },
    { -1779.09f, 2416.93f, 53.621f },
    { -1770.65f, 2448.44f, 62.412f },
    { -1751.91f, 2463.09f, 69.749f },
    { -1713.10f, 2466.28f, 82.205f },
    { -1703.41f, 2474.40f, 87.121f },
    { -1693.22f, 2491.53f, 94.415f },
    { -1687.36f, 2499.74f, 96.885f },
    { -1668.25f, 2519.52f, 97.924f }, // 13
};

class npc_swift_mountain_horse_worgen : public CreatureScript
{
public:
    npc_swift_mountain_horse_worgen() : CreatureScript("npc_swift_mountain_horse_worgen") { }

    struct npc_swift_mountain_horse_worgenAI : public VehicleAI
    {
        npc_swift_mountain_horse_worgenAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                me->GetMotionMaster()->MoveSmoothPath(ToGreymaneManorPath, ToGreymaneManorPathSize);

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 3:
                if (auto gate1 = me->FindNearestGameObject(OBJECT_GREYMANE_GATE, 40.f))
                {
                    gate1->SetLootState(GO_READY);
                    gate1->UseDoorOrButton(3, false, me);
                }
                break;
            case 11:
                if (auto gate1 = me->FindNearestGameObject(OBJECT_GREYMANE_GATE, 40.f))
                {
                    gate1->SetLootState(GO_READY);
                    gate1->UseDoorOrButton(3, false, me);
                }
                break;
            case 13:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(1000);
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
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_TO_GREYMANE_MANOR) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_TO_GREYMANE_MANOR) == QUEST_STATUS_REWARDED)
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetDistance(player) > 100.f)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;
        }
    private:
        uint16 _checkTimer = 2000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swift_mountain_horse_worgenAI(creature);
    }
};

enum TestDatas
{
    QUEST_EXODUS = 24438,

    SPELL_RIDE_STAGECOACH = 72764,
    SPELL_RIDE_VEHICLE_2 = 63313,

    NPC_STAGECOACH_CARRIAGE = 38756,
    NPC_STAGECOACH_HORSE = 43338,
};

uint32 const StageCoachPathSize = 35;
const G3D::Vector3 StageCoachPath[StageCoachPathSize] =
{
   { -1673.5428f, 2519.0561f, 97.894f },
   { -1678.7979f, 2511.5971f, 97.891f },
   { -1680.8464f, 2508.8635f, 97.857f },
   { -1687.4831f, 2500.1662f, 96.945f },
   { -1696.0086f, 2487.8830f, 92.906f },
   { -1700.8616f, 2470.8242f, 86.408f },
   { -1698.8062f, 2440.9846f, 78.897f },
   { -1704.9451f, 2417.7229f, 70.169f },
   { -1725.2908f, 2392.9248f, 61.103f },
   { -1736.5565f, 2360.5629f, 63.633f },
   { -1748.5186f, 2331.1870f, 69.581f },
   { -1760.0035f, 2294.9948f, 76.946f },
   { -1775.3609f, 2272.2072f, 81.942f },
   { -1798.2252f, 2251.7731f, 87.450f },
   { -1818.6234f, 2241.6928f, 89.314f },
   { -1842.0864f, 2210.9514f, 89.316f },
   { -1862.8450f, 2183.6892f, 89.316f },
   { -1872.1195f, 2148.3818f, 89.316f },
   { -1873.3164f, 2092.4111f, 89.316f },
   { -1877.9824f, 2056.0166f, 89.316f },
   { -1886.5048f, 2025.9160f, 89.316f },
   { -1885.1578f, 1995.0469f, 89.316f },
   { -1874.3880f, 1963.7852f, 89.195f },
   { -1879.5952f, 1917.3176f, 89.149f },
   { -1893.4104f, 1903.8516f, 89.151f },
   { -1937.7473f, 1902.1264f, 89.285f },
   { -1995.6499f, 1903.1701f, 89.285f },
   { -2035.5650f, 1913.2331f, 83.508f },
   { -2063.2224f, 1906.9561f, 73.386f },
   { -2089.9965f, 1884.4874f, 55.930f },
   { -2110.5432f, 1853.0971f, 38.629f },
   { -2131.7229f, 1823.3812f, 24.575f },
   { -2159.9445f, 1809.6879f, 15.531f },
   { -2195.5566f, 1808.4870f, 12.475f },
   { -2211.4545f, 1809.9150f, 11.911f },
};

uint32 const StageCoachPathSize2 = 5;
const G3D::Vector3 StageCoachPath2[StageCoachPathSize2] =
{
    { -2217.166f, 1809.894f, 11.8072f },
    { -2235.310f, 1806.164f, 11.8462f },
    { -2264.772f, 1793.851f, 11.7698f },
    { -2299.698f, 1783.104f, 11.2539f },
    { -2332.370f, 1746.180f, 11.6016f }, // 4
};

class npc_stagecoach_harness_worgen : public CreatureScript
{
public:
    npc_stagecoach_harness_worgen() : CreatureScript("npc_stagecoach_harness_worgen") { }

    struct npc_stagecoach_harness_worgenAI : public VehicleAI
    {
        npc_stagecoach_harness_worgenAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            _step = 0;
            me->SetReactState(REACT_PASSIVE);
            me->setActive(true);
            me->SetRooted(true);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply)
            {
                if (auto carriage = passenger->ToCreature())
                    if (carriage->GetEntry() == NPC_STAGECOACH_CARRIAGE)
                    {
                        _carriageGUID = carriage->GetGUID();

                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            player->KilledMonsterCredit(me->GetEntry());
                            player->CastWithDelay(300, carriage, SPELL_RIDE_STAGECOACH);
                            _events.ScheduleEvent(EVENT_CARRIAGE_1, 2000);
                        }
                    }

                if (auto horse = passenger->ToCreature())
                    if (horse->GetEntry() == NPC_STAGECOACH_HORSE)
                        horse->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }
        }

        void MovementInform(uint32 type, uint32 point) override
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_step == 0)
            {
                switch (point)
                {
                case 34:
                    _step = 1;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto carriage = Unit::GetCreature(*me, _carriageGUID))
                        {
                            player->ExitVehicle();
                            player->GetMotionMaster()->MoveJump(-2216.684f, 1817.42f, 12.416f, 15, 18);
                            carriage->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                            me->GetMotionMaster()->MoveSmoothPath(StageCoachPath2, StageCoachPathSize2);
                        }
                    break;
                default:
                    break;
                }
            }
            else
            {
                if (point == 4)
                    me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_EXODUS) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_EXODUS) == QUEST_STATUS_REWARDED)
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon();
                    }

                    if (me->GetDistance(player) > 1000.f)
                        me->DespawnOrUnsummon();
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CARRIAGE_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(StageCoachPath, StageCoachPathSize);

                    if (auto gate1 = me->FindNearestGameObject(OBJECT_GREYMANE_GATE, 40.f))
                    {
                        gate1->SetLootState(GO_READY);
                        gate1->UseDoorOrButton(4, false, me);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint16 _checkTimer = 2000;
        uint8 _step = 0;
        uint64 _carriageGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stagecoach_harness_worgenAI(creature);
    }
};

enum QuestFlank
{
    QUEST_FLANK_THE_FORSAKEN = 24677,
};

uint32 const StoutHorsePathSize = 47;
const G3D::Vector3 StoutHorsePath[StoutHorsePathSize] =
{
    { -2046.12f, 965.653f, 70.0887f },
    { -2053.73f, 950.248f, 70.0129f },
    { -2040.40f, 942.245f, 69.2100f },
    { -2020.79f, 938.066f, 64.9307f },
    { -1999.42f, 917.552f, 58.2501f },
    { -1992.78f, 892.795f, 57.6433f },
    { -1985.82f, 878.691f, 61.4835f },
    { -1969.01f, 861.442f, 64.4005f },
    { -1955.01f, 857.739f, 64.3161f },
    { -1926.32f, 859.228f, 71.4603f },
    { -1896.65f, 864.117f, 79.8834f },
    { -1886.11f, 871.402f, 81.8399f },
    { -1877.00f, 889.214f, 81.6814f },
    { -1876.55f, 914.794f, 79.1436f },
    { -1879.19f, 941.853f, 78.2550f },
    { -1881.50f, 963.152f, 77.0640f },
    { -1885.96f, 983.033f, 75.7762f },
    { -1876.37f, 1036.56f, 63.5033f },
    { -1867.14f, 1049.39f, 60.4200f },
    { -1853.79f, 1068.01f, 55.1499f },
    { -1843.57f, 1136.95f, 38.0490f },
    { -1860.09f, 1200.80f, 25.1542f },
    { -1857.65f, 1220.08f, 22.5519f },
    { -1851.63f, 1221.75f, 21.9952f },
    { -1843.69f, 1214.65f, 21.1310f },
    { -1829.40f, 1201.71f, 19.5830f },
    { -1809.21f, 1183.08f, 20.1528f },
    { -1793.25f, 1174.40f, 21.0935f },
    { -1773.27f, 1166.81f, 23.0610f },
    { -1752.11f, 1161.13f, 26.2432f },
    { -1724.49f, 1148.41f, 26.9646f },
    { -1693.31f, 1138.23f, 25.7202f },
    { -1647.66f, 1111.29f, 28.7171f },
    { -1631.86f, 1110.57f, 29.1591f },
    { -1607.29f, 1112.48f, 29.9700f },
    { -1592.14f, 1117.40f, 30.1835f },
    { -1541.07f, 1118.50f, 30.2189f },
    { -1506.51f, 1089.02f, 28.9787f },
    { -1481.15f, 1080.78f, 29.5806f },
    { -1424.15f, 1083.04f, 28.8977f },
    { -1399.60f, 1091.21f, 29.6781f },
    { -1369.91f, 1111.53f, 30.6228f },
    { -1337.12f, 1120.90f, 34.6645f },
    { -1314.00f, 1126.99f, 37.3011f },
    { -1337.13f, 1154.12f, 35.7425f },
    { -1362.07f, 1184.01f, 34.9061f },
    { -1369.61f, 1193.97f, 35.4457f }, //46
};

class npc_stout_mountain_horse_worgen : public CreatureScript
{
public:
    npc_stout_mountain_horse_worgen() : CreatureScript("npc_stout_mountain_horse_worgen") { }

    struct npc_stout_mountain_horse_worgenAI : public VehicleAI
    {
        npc_stout_mountain_horse_worgenAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_STUNT_HORSE_1, 2000);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 46:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(100);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (auto player = Unit::GetPlayer(*me, _playerGUID))
            {
                if (player->GetQuestStatus(QUEST_FLANK_THE_FORSAKEN) == QUEST_STATUS_NONE ||
                    player->GetQuestStatus(QUEST_FLANK_THE_FORSAKEN) == QUEST_STATUS_REWARDED)
                {
                    player->ExitVehicle();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(100);
                }

                if (me->GetDistance(player) >= 150.f)
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon(100);
                }
            }

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_STUNT_HORSE_1:
                    me->SetRooted(false);
                    me->GetMotionMaster()->MoveSmoothPath(StoutHorsePath, StoutHorsePathSize);
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
        return new npc_stout_mountain_horse_worgenAI(creature);
    }
};

enum QuestSlowingtheInevitable
{
    QUEST_SLOWING_THE_INEVITABLE = 24920,
};

uint32 const CapturedRidingBatPathSize = 8;
const G3D::Vector3 CapturedRidingBatPath[CapturedRidingBatPathSize] =
{
    { -1672.25280f, 1662.903f, 23.74166f },
    { -1686.41149f, 1656.935f, 26.62966f },
    { -1701.35949f, 1643.877f, 37.25981f },
    { -1716.85986f, 1616.549f, 56.22752f },
    { -1686.55810f, 1605.887f, 63.70630f },
    { -1618.37255f, 1613.739f, 74.93902f },
    { -1471.39941f, 1599.724f, 69.68731f },
    { -1415.45153f, 1630.738f, 55.19381f }, // 7
};

uint32 const CapturedRidingBatPathSize2 = 27;
const G3D::Vector3 CapturedRidingBatPath2[CapturedRidingBatPathSize2] =
{
    { -1338.57f, 1675.73f, 53.77f },
    { -1289.10f, 1749.31f, 45.62f },
    { -1253.82f, 1752.09f, 46.92f },
    { -1196.96f, 1701.26f, 57.61f },
    { -1122.29f, 1647.03f, 66.12f },
    { -1064.90f, 1663.90f, 88.12f },
    { -982.171f, 1647.02f, 86.98f },
    { -938.282f, 1651.12f, 98.27f },
    { -884.466f, 1668.15f, 92.17f },
    { -848.068f, 1655.16f, 91.07f },
    { -872.210f, 1527.70f, 84.00f },
    { -917.297f, 1494.65f, 86.36f },
    { -975.228f, 1521.31f, 88.73f },
    { -1050.54f, 1580.19f, 84.41f },
    { -1119.80f, 1624.44f, 75.58f },
    { -1210.75f, 1621.91f, 68.46f },
    { -1280.10f, 1557.18f, 70.70f },
    { -1319.70f, 1487.79f, 67.61f },
    { -1254.46f, 1446.86f, 63.37f },
    { -1220.60f, 1402.24f, 61.43f },
    { -1151.38f, 1403.77f, 64.11f },
    { -1041.06f, 1415.71f, 70.70f },
    { -985.144f, 1499.41f, 81.11f },
    { -978.196f, 1582.31f, 79.93f },
    { -1041.26f, 1603.94f, 81.06f },
    { -1150.07f, 1647.00f, 63.91f },
    { -1283.51f, 1640.29f, 45.46f }, // 26
};

uint32 const CapturedRidingBatPathSize3 = 9;
const G3D::Vector3 CapturedRidingBatPath3[CapturedRidingBatPathSize3] =
{
    { -1457.12f, 1643.85f, 106.63f },
    { -1485.87f, 1649.65f, 104.73f },
    { -1533.54f, 1658.16f, 107.66f },
    { -1594.63f, 1667.45f, 91.365f },
    { -1653.68f, 1679.71f, 70.252f },
    { -1711.71f, 1685.06f, 31.080f },
    { -1712.43f, 1675.55f, 28.789f },
    { -1693.55f, 1659.78f, 23.387f },
    { -1671.37f, 1653.86f, 22.686f },
};

class npc_captured_riding_bat_worgen : public CreatureScript
{
public:
    npc_captured_riding_bat_worgen() : CreatureScript("npc_captured_riding_bat_worgen") { }

    struct npc_captured_riding_bat_worgenAI : public VehicleAI
    {
        npc_captured_riding_bat_worgenAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            me->SetRooted(true);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_CAPTURED_BAT_1, 1500, 0);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(100);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (_step)
            {
            case 1:
                if (point == 7)
                {
                    _step++;
                    me->RemoveAura(SPELL_FLIGHT_SPEED_280);
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_180);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(CapturedRidingBatPath2, CapturedRidingBatPathSize2);
                }
                break;
            case 2:
                if (point == 26)
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(CapturedRidingBatPath2, CapturedRidingBatPathSize2);
                }
                break;
            case 10:
                if (point == 7)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        me->SetRooted(true);
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_SLOWING_THE_INEVITABLE) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_SLOWING_THE_INEVITABLE) == QUEST_STATUS_REWARDED)
                    {
                        player->ExitVehicle();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }

                    if (me->GetDistance(player) >= 150.f)
                    {
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }

                    if (!_complete)
                    {
                        if (player->GetQuestStatus(QUEST_SLOWING_THE_INEVITABLE) == QUEST_STATUS_COMPLETE)
                        {
                            _complete = true;
                            me->GetMotionMaster()->Clear();
                            me->RemoveAura(SPELL_FLIGHT_SPEED_180);
                            me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                            _step = 10;
                            me->GetMotionMaster()->MoveSmoothPath(CapturedRidingBatPath3, CapturedRidingBatPathSize3);
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CAPTURED_BAT_1:
                    me->CastSpell(me, SPELL_FLIGHT_SPEED_280);
                    me->SetRooted(false);
                    _step = 1;
                    me->GetMotionMaster()->MoveSmoothPath(CapturedRidingBatPath, CapturedRidingBatPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _complete = false;
        uint16 _checkTimer = 1000;
        uint16 _step = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captured_riding_bat_worgenAI(creature);
    }
};

enum QuestPatriarchsBlessing
{
    QUEST_QUEST_PATRIARCHS_BLESSING = 24679,

    NPC_SCENE_LORNA = 38612,
    NPC_SCENE_KING_GREYMANE = 38148,
    NPC_SCENE_LORD_CROWLEY = 38538,
    NPC_SCENE_DOPPLEGANGER = 38286,
    NPC_SCENE_CAMERA_TARGET = 75138,

    SPELL_TELEPORT_BACK = 151347,
    SPELL_LIAM_CHANNEL = 86285,
    SPELL_SUMMON_SCENE_DOPPLEGANGER = 71506,
    SPELL_MIRROR_IMAGE_DOPPLER = 85682,
};

const Position ActorLocations[] =
{
    { -1639.57f, 1904.22f, 29.95f, 3.3f }, // lorna
    { -1643.68f, 1903.85f, 29.97f, 3.5f }, // greymane
    { -1641.07f, 1907.56f, 29.95f, 3.9f }, // crowley
    { -1640.08f, 1905.83f, 29.95f, 3.6f }, // doppler
    { -1635.60f, 1907.064f, 32.0f, 0.0f }, // camera
    { -1634.58f, 1898.067f, 32.09f }, // camera move 1
    { -1644.41f, 1903.32f, 32.704f }, // camera move 2
};

uint32 const LiamCameraPathSize = 7;
const G3D::Vector3 LiamCameraPath[LiamCameraPathSize] =
{
    { -1645.408f, 1897.518f, 32.83f },
    { -1642.057f, 1902.055f, 32.99f },
    { -1637.147f, 1909.714f, 33.36f },
    { -1629.368f, 1914.833f, 32.96f },
    { -1621.484f, 1909.921f, 32.96f },
    { -1624.994f, 1904.242f, 32.98f },
    { -1631.336f, 1902.036f, 33.97f },
};

class npc_gilneas_liam_camera : public CreatureScript
{
public:
    npc_gilneas_liam_camera() : CreatureScript("npc_gilneas_liam_camera") { }

    struct npc_gilneas_liam_cameraAI : public VehicleAI
    {
        npc_gilneas_liam_cameraAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);

            summoner->CastSpell(me, SPELL_RIDE_VEHICLE);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply)
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = passenger->ToPlayer())
                {
                    _playerGUID = player->GetGUID();

                    if (auto lorna = me->SummonCreature(NPC_SCENE_LORNA, ActorLocations[0], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto greymane = me->SummonCreature(NPC_SCENE_KING_GREYMANE, ActorLocations[1], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto crowley = me->SummonCreature(NPC_SCENE_LORD_CROWLEY, ActorLocations[2], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto doppler = player->SummonCreature(NPC_SCENE_DOPPLEGANGER, ActorLocations[3], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto camera = me->SummonCreature(NPC_SCENE_CAMERA_TARGET, ActorLocations[4], TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        _summons.Summon(lorna);
                                        _summons.Summon(greymane);
                                        _summons.Summon(crowley);
                                        _summons.Summon(doppler);
                                        _summons.Summon(camera);

                                        _cameraGUID = camera->GetGUID();

                                        me->CastWithDelay(100, me, SPELL_LIAM_CHANNEL);
                                        _events.ScheduleEvent(EVENT_LIAM_SCENE_1, 1000, 0);
                                    }
                }
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                // problem here, on unapply i cant teleport passenger 
                //passenger->CastSpell(passenger, SPELL_TELEPORT_BACK);
                _summons.DespawnAll();
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon(1000);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                if (auto camera = Unit::GetCreature(*me, _cameraGUID))
                    camera->GetMotionMaster()->MovePoint(0, ActorLocations[5]);
                break;
            case 3:
                if (auto camera = Unit::GetCreature(*me, _cameraGUID))
                    camera->GetMotionMaster()->MovePoint(1, ActorLocations[6]);
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_QUEST_PATRIARCHS_BLESSING) == QUEST_STATUS_NONE ||
                        player->GetQuestStatus(QUEST_QUEST_PATRIARCHS_BLESSING) == QUEST_STATUS_REWARDED)
                    {
                        player->ExitVehicle();
                    }

                    if (me->GetDistance(player) >= 150.f)
                    {
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon(100);
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LIAM_SCENE_1:
                    me->GetMotionMaster()->MoveSmoothPath(LiamCameraPath, LiamCameraPathSize);

                    me->AI()->TalkWithDelay(5000, 0, _playerGUID);
                    me->AI()->TalkWithDelay(14000, 1, _playerGUID);
                    me->AI()->TalkWithDelay(21000, 2, _playerGUID);
                    me->AI()->TalkWithDelay(29500, 3, _playerGUID);
                    me->AI()->TalkWithDelay(39000, 4, _playerGUID);
                    _events.ScheduleEvent(EVENT_LIAM_SCENE_2, 47000, 0);

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->CastWithDelay(45500, player, 89147);
                    break;
                case EVENT_LIAM_SCENE_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        player->ExitVehicle();
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _cameraGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_liam_cameraAI(creature);
    }
};


/*
36798
*/
class npc_gilneas_captured_worgen : public CreatureScript
{
public:
    npc_gilneas_captured_worgen() : CreatureScript("npc_gilneas_captured_worgen") { }

    struct npc_gilneas_captured_worgenAI : public ScriptedAI
    {
        npc_gilneas_captured_worgenAI(Creature* creature) : ScriptedAI(creature) {}


        void MoveInLineOfSight(Unit* who)
        {
            if (who)
                if (auto p = who->ToPlayer())
                {
                    if (p->GetQuestStatus(14375) == QUEST_STATUS_REWARDED)
                        if (p->HasAura(68630))
                            p->RemoveAurasDueToSpell(68630);
                }
        }

    private:
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gilneas_captured_worgenAI(creature);
    }
};


void AddSC_gilneas()
{
    new npc_by_the_skin_of_his_teeth_controller();
    new npc_gilnean_mastiff_pet();
    new npc_king_greymanes_horse();
    new StocksPlayerScript();
    new npc_crowleys_horse_vehicle();
    new npc_crowleys_horse_spellclick();
    new npc_drowning_watchtman_spellclick();
    new npc_mountain_horse_worgen();
    new npc_summoned_mountain_horse_worgen();
    new spell_round_up_horse();
    new npc_swift_mountain_horse_worgen();
    new npc_stagecoach_harness_worgen();
    new npc_stout_mountain_horse_worgen();
    new npc_captured_riding_bat_worgen();
    new npc_gilneas_liam_camera();
    new npc_gilneas_captured_worgen();
}
