#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "SpellScript.h"
#include "MoveSplineInit.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "dragonsoul.h"

enum SpellsTick
{
    SPELL_SPAWNING_CONTROLLER       = 105003, // caster 53879 "Deathwing"
    SPELL_CHECK_FOR_PLAYERS         = 109022, // caster 53879 "Deathwing"
    SPELL_PET_HACK                  = 95278,  // caster 53879 "Deathwing"
    SPELL_SUMMON_SLIME              = 104999, // caster 53879 "Deathwing"
    SPELL_ROLL_CONTROL              = 105036, // caster 53879 "Deathwing"
    SPELL_ROLL_CONTROL_ROLL         = 105740, // caster 53879 "Deathwing"
    SPELL_ROLL_CONTROL_AURA         = 105777,
    SPELL_RIDE_VEHICLE_HARDCODED    = 46598,  // caster 53879 "Deathwing" - target 56161
    SPELL_TELEPORT_ALL_TO_GUNSHIP   = 106051, // cast on wipe

    SPELL_DW_TALK                   = 106300,
    SPELL_DW_ROAR                   = 106302,
    SPELL_DW_PLAY_MOVIE             = 104574,

    SPELL_ACTIVATE_SPAWMER          = 100341, // caster 56161 vehicle "Corruption" - target 53888
    SPELL_EJECT_ALL_PASSENGERS      = 68576,  // caster 56161 vehicle "Corruption"

    SPELL_SUMMON_GRASPING_TENDRIS   = 105510, // caster 53888 "Spawner"
    SPELL_GRASPING_TENDRIS_AURA     = 105563,
    SPELL_SUMMON_ELEMENTARY         = 105000, // caster 53888 "Spawner"

    SPELL_SEARING_PLASMA_INIT       = 109379, // caster 53891 "Corruption"
    SPELL_SEARING_PLASMA_TR         = 105479, // caster 53891 "Corruption"
    SPELL_FIERY_GRIP                = 105490, // caster 53891 "Corruption"

    SPELL_DECREASE_CRIT             = 64481,  // caster 53889 "Corrupted Blood"
    SPELL_BURST                     = 105219, // caster 53889 "Corrupted Blood"
    SPELL_RESIDUE                   = 105223, // caster 53889 "Corrupted Blood"
    SPELL_ABSORBED_BLOOD            = 105248, // caster 53889 "Corrupted Blood" - target vehicle 53890
    SPELL_ENERGIZE                  = 109083, // caster 53889 "Corrupted Blood" - target vehicle 53890

    SPELL_ACT_BLOOD_ABSORB_BAR      = 109329, // caster 53890 "Hideous Amalgamation"
    SPELL_ABSORB_BLOOD              = 105244, // caster 53890 "Hideous Amalgamation"
    SPELL_SUPERHEATED_NUCLEUS       = 105834, // caster 53890 "Hideous Amalgamation"
    SPELL_NUCLEAR_BLAST             = 105845, // caster 53890 "Hideous Amalgamation"
    SPELL_NUCLEAR_BLAST_TRIGGERED   = 105846, // caster 53890 "Hideous Amalgamation" - target NPC_BURNING_TENDONS_L
    SPELL_DEGRADATION               = 106005, // caster 53890 "Hideous Amalgamation" - HEROIC MODE
    SPELL_BLOOD_CORRUPTION_DEATH    = 106199, // caster 53890 "Hideous Amalgamation" - HEROIC MODE
    SPELL_BLOOD_CORRUPTION_EARTH    = 106200, // caster 53890 "Hideous Amalgamation" - HEROIC MODE
    SPELL_BLOOD_OF_NELTHARION       = 106213, // caster owner of SPELL_BLOOD_CORRUPTION_EARTH - HEROIC MODE
    SPELL_BLOOD_OF_DEATHWING        = 106201, // caster owner of SPELL_BLOOD_CORRUPTION_DEATH - HEROIC MODE
    SPELL_SEAL_ARMOR_BREACH_L       = 105848, // caster 53890 "Burning Tendons"
    SPELL_SEAL_ARMOR_BREACH_R       = 105847, // caster 53890 "Burning Tendons"
};

enum Texts
{
    EMOTE_WARN_ROLL_LEFT    = -54875,
    EMOTE_WARN_ROLL_RIGHT   = -54878,
    EMOTE_ROLL_LEFT         = -54879,
    EMOTE_ROLL_RIGHT        = -54880,
    TALK_MINUTE_INTERVAL_TAUNT = 1,
};

enum Events
{
    EVENT_SEARING_PLASMA_CAST            = 1,
    EVENT_FIERY_GRIP                = 2,
    EVENT_ACTIVATE_CORRUPTION       = 3,
    EVENT_SUMMON_CORRUPTED_BLOOD    = 4,
    EVENT_SEND_ENCOUNTER_FRAME      = 5,
    EVENT_PLAYER_CHECK              = 6,
    EVENT_BLOOD_CORRUPTION          = 7,
    EVENT_SUMMON_SLIME              = 8,
    EVENT_SUMMON_SLIME_2            = 9,
    EVENT_SUMMON_SLIME_3            = 10,
    EVENT_CHECK_WIPE                = 11,
    EVENT_FIRE_GRIP,
    EVENT_ROLL,
    EVENT_ROLL_ICD_ACTIVE,
};

enum Points
{
    POINT_SPAWNER = 1,
};

enum Actions
{
    ACTION_CORRUPTION                       = 0,
    ACTION_FINISH_FIGHT                     = 1,
    ACTION_ACTIVATE_CORRUPTION              = 2,
    ACTION_ROLL_RIGHT                       = 3,
    ACTION_ROLL_LEFT                        = 4,
    ACTION_CORRUPTED_BLOOD                  = 5,
    ACTION_ACTIVATE_FIRST_CORRUPTION        = 6,
    ACTION_REACTIVATE_ACTIVATE_CORRUPTION   = 7,
    ACTION_RP_START                         = 8,
    ACTION_RP_END                           = 9,
    ACTION_ROLL_MIDDLE                      = 10,
};

enum Animations
{
    ANIM_BACKPLATE_RIGHT        = 1677,
    ANIM_LOOSE_BACKPLATE_RIGHT  = 1678,
    ANIM_LOOSE_BACKPLATE_LEFT   = 1680,
    ANIM_BACKPLATE_LEFT         = 1681
};

enum Spawners
{
    CORRUPTION_LEFT_1,
    CORRUPTION_RIGHT_1,
    CORRUPTION_LEFT_2,
    CORRUPTION_RIGHT_2,
    CORRUPTION_LEFT_3,
    CORRUPTION_RIGHT_3,
    CORRUPTION_LEFT_4,
    CORRUPTION_RIGHT_4,
};

enum BackPlateState
{
    BACK_PLATES_ALL_INTACT,
    BACK_PLATES_ONE_BROKEN,
    BACK_PLATES_TWO_BROKEN,
    BACK_PLATES_THREE_BROKEN,
};

static const std::vector<uint8> rollRight = { 1, 0, 217, 0, 0, 0 };
static const std::vector<uint8> rollLeft = { 1, 0, 216, 0, 0, 0 };

#define SPINE_MIDDLE_LR_BORDER       -13854.5f
#define SPINE_LEFT_BORDER            -13861.0f
#define SPINE_RIGHT_BORDER           -13848.0f
//13,854.5

Creature* GetNearCorruption(Unit* searcher, bool alive = true, float range = 5.0f)
{
    Creature* curruption = nullptr;
    curruption = searcher->FindNearestCreature(NPC_CORRUPTION, range, alive);
    if (!curruption)
        curruption = searcher->FindNearestCreature(NPC_CORRUPTION_VEH_L, range, alive);
    if (!curruption)
        curruption = searcher->FindNearestCreature(NPC_CORRUPTION_VEH_R, range, alive);
    return curruption;
}

class boss_dw_spine : public CreatureScript
{
public:
    boss_dw_spine() : CreatureScript("boss_dw_spine") { }

    struct boss_dw_spineAI : public BossAI
    {
        boss_dw_spineAI(Creature* creature) : BossAI(creature, DATA_SPINE_OF_DEATHWING)
        {
            init = true;
        }

        void ActivateCorruption(std::list<Creature*> spawnerList, BackPlateState progress, bool mass)
        {

            if (progress == BACK_PLATES_ALL_INTACT)
                for (Creature* crea : spawnerList)
                    switch (crea->GetDBTableGUIDLow())
                    {
                        case 254396:
                            if (corruption_spawner_guids[CORRUPTION_RIGHT_1] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_RIGHT_1] = crea->GetGUID();
                            break;
                        case 254395:
                            if (corruption_spawner_guids[CORRUPTION_RIGHT_2] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_RIGHT_2] = crea->GetGUID();
                            break;
                        case 254391:
                            if (corruption_spawner_guids[CORRUPTION_RIGHT_3] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_RIGHT_3] = crea->GetGUID();
                            break;
                        case 254393:
                            if (corruption_spawner_guids[CORRUPTION_RIGHT_4] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_RIGHT_4] = crea->GetGUID();
                            break;
                        case 254390:
                            if (corruption_spawner_guids[CORRUPTION_LEFT_1] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_LEFT_1] = crea->GetGUID();
                            break;
                        case 254389:
                            if (corruption_spawner_guids[CORRUPTION_LEFT_2] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_LEFT_2] = crea->GetGUID();
                            break;
                        case 254392:
                            if (corruption_spawner_guids[CORRUPTION_LEFT_3] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_LEFT_3] = crea->GetGUID();
                            break;
                        case 254394:
                            if (corruption_spawner_guids[CORRUPTION_LEFT_4] != crea->GetGUID())
                                corruption_spawner_guids[CORRUPTION_LEFT_4] = crea->GetGUID();
                            break;
                        default:
                            TC_LOG_ERROR("sql.sql", "boss_dw_spine:: ActivateCorruption processed unspecified DB GUID: %u", crea->GetDBTableGUIDLow());
                            break;
                    }

            switch (progress)
            {
                case BACK_PLATES_ALL_INTACT:
                    for (auto i = int(CORRUPTION_LEFT_1); i <= CORRUPTION_RIGHT_2; ++i)
                    if (auto crea = ObjectAccessor::GetCreature(*me, corruption_spawner_guids[i]))
                        crea->AI()->DoAction(ACTION_ACTIVATE_FIRST_CORRUPTION);
                    break;
                case BACK_PLATES_ONE_BROKEN:
                    for (auto i = int(mass ? CORRUPTION_LEFT_1 : CORRUPTION_LEFT_3); i <= CORRUPTION_RIGHT_3; ++i)
                        if (auto crea = ObjectAccessor::GetCreature(*me, corruption_spawner_guids[i]))
                            crea->AI()->DoAction(ACTION_ACTIVATE_CORRUPTION);
                    break;
                case BACK_PLATES_TWO_BROKEN:
                    for (auto i = int(mass ? CORRUPTION_LEFT_1 : CORRUPTION_LEFT_4); i <= CORRUPTION_RIGHT_4; ++i)
                        if (auto crea = ObjectAccessor::GetCreature(*me, corruption_spawner_guids[i]))
                            crea->AI()->DoAction(ACTION_ACTIVATE_CORRUPTION);
                    break;
                case BACK_PLATES_THREE_BROKEN:
                    TC_LOG_ERROR("sql.sql", "boss_dw_spine:: ActivateCorruption processed invalid progress state: %u", int32(progress));
                    break;
                default:
                    TC_LOG_ERROR("sql.sql", "boss_dw_spine:: ActivateCorruption processed invalid progress state: %u", int32(progress));
                    break;
            }
        }

        void SendRoll(bool left)
        {
            if (!left)
            {
                Talk(EMOTE_ROLL_RIGHT, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                // me->SendMapObjectEvents(6574436, 1, 0, 60); // head right
                me->SendMapObjectEvents(6574436, rollRight);// roll right
            }
            else
            {
                Talk(EMOTE_ROLL_LEFT, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                // me->SendMapObjectEvents(6574436, 1, 0, 60);
                me->SendMapObjectEvents(6574436, rollLeft);// roll left
            }
            DoCast(SPELL_ROLL_CONTROL_ROLL);
            instance->SetData(DATA_DW_SPINE_ROLL_INFO, 0);

            me->TalkWithDelay(3000, urand(1, 5), NULL, CHAT_MSG_MONSTER_YELL);
            std::list<Creature*> creatureList;
            GetCreatureListWithEntryInGrid(creatureList, me, NPC_HIDEOUS_AMALGAMATION, 200.0f);
            // GetCreatureListWithEntryInGrid(creatureList, me, NPC_CORRUPTED_BLOOD, 200.0f);
            for (Creature* crea : creatureList)
                crea->GetMotionMaster()->ForceMoveJump(-13875.01f, -13769.59f, 285.1783f, 15.0f, 20.0f);
        }

        void Reset() override
        {
            events.Reset();
            _Reset();
            me->RemoveAllAuras();
            me->CombatStop();
            corruptedBlood = false;
            std::list<Creature*> spawnerList;
            GetCreatureListWithEntryInGrid(spawnerList, me, NPC_DW_SPINE_SPAWNER, 200.0f);
            auto copyList = spawnerList;
            for (Creature* crea : spawnerList)
                crea->AI()->Reset();
            ActivateCorruption(copyList, BACK_PLATES_ALL_INTACT, true);

            std::list<Creature*> trendonList;
            GetCreatureListWithEntryInGrid(trendonList, me, NPC_BURNING_TENDONS_L, 200.0f);
            GetCreatureListWithEntryInGrid(trendonList, me, NPC_BURNING_TENDONS_R, 200.0f);
            for (Creature* crea : trendonList)
            {
                crea->Respawn();
                crea->SetHealth(crea->GetMaxHealth());
            }

            for (uint8 i = 0; i < 3; i++)
                if (auto go = instance->GetGameObject(DATA_DW_SPINE_BACKPLATE_1 + i))
                    go->SetGoState(GO_STATE_READY);
            instance->SetData(DATA_DW_SPINE_BACKPLATE_CNT, 0);
            instance->SetData(DATA_DW_SPINE_ROLL_INFO, 0);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEGRADATION);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_OF_NELTHARION);
            me->SetHealth(me->GetMaxHealth());
            RemoveEncounterFrame();
        }

        void MoveInLineOfSight(Unit* victim)
        {
            if (victim->GetTypeId() != TYPEID_PLAYER)
                return;

            if (init)
            {
                EnterEvadeMode();
                init = false;
                return;
            }

            if (!me->isInCombat())
                EnterCombat(victim);
        }

        void DoAction(int32 const actionid) override
        {
            switch (actionid)
            {
            case ACTION_CORRUPTION:
            {
                std::list<Creature*> spawnerList;
                GetCreatureListWithEntryInGrid(spawnerList, me, NPC_DW_SPINE_SPAWNER, 200.0f);

                ActivateCorruption(spawnerList, BackPlateState(phase), false);
                me->SetHealth(me->GetHealth() - me->GetMaxHealth() / 3);
                me->PlayDirectSound(26347 + urand(0,2));//deathwing roars
                phase += 1;
                break;
            }
            case ACTION_FINISH_FIGHT:
            {
                DoCast(SPELL_DW_PLAY_MOVIE);
                me->DespawnCreaturesInArea(NPC_CORRUPTION);
                me->DespawnCreaturesInArea(NPC_CORRUPTED_BLOOD);
                me->DespawnCreaturesInArea(NPC_HIDEOUS_AMALGAMATION);
                me->DespawnCreaturesInArea(NPC_CORRUPTION_VEH_L);
                me->DespawnCreaturesInArea(NPC_CORRUPTION_VEH_R);
                me->DespawnOrUnsummon(1000);
                RemoveEncounterFrame();
                uint32 spellId = sSpellMgr->GetSpellIdForDifficulty(SPELL_SEARING_PLASMA_TR, me);
                instance->DoRemoveAurasDueToSpellOnPlayers(spellId);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEGRADATION);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_OF_NELTHARION);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_CORRUPTION_DEATH, AURA_REMOVE_BY_ENEMY_SPELL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_OF_DEATHWING, AURA_REMOVE_BY_ENEMY_SPELL);
                instance->SetBossState(DATA_SPINE_OF_DEATHWING, IsHeroic() ? DONE_HM : DONE);
                instance->instance->LoadGrid(-12077.301f, 12165.879f); //make sure chest is loaded
                if (auto chest = instance->GetGameObject((instance->GetData(DATA_RAID_MODE) == RAID_MODE_LFR) ? DATA_SPINE_CHEST_LFR : DATA_SPINE_CHEST))
                {
                    chest->SetRespawnTime(chest->GetRespawnDelay());
                    chest->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (auto player = me->FindNearestPlayer(200.0f))
                        chest->SetLootRecipient(player);
                }

                instance->CompleteGuildCriteriaForGuildGroup(18486);

                if (instance->GetData(DATA_RAID_MODE) == RAID_MODE_NORMAL)
                {
                    Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
                    for (auto i = playerList.begin(); i != playerList.end(); ++i)
                    {
                        if (auto player = i->getSource())
                        {
                            player->ModifyCurrency(CURRENCY_TYPE_VALOR_POINTS, Is25ManRaid() ? 13500 : 11500);
                            player->ModifyCurrency(CURRENCY_TYPE_ESSENCE_OF_CORRUPTED_DEATHWING, 1);
                            player->CombatStop();
                        }
                    }
                }
                break;
            }
            case ACTION_ROLL_RIGHT:
                if (!events.HasEvent(EVENT_ROLL_ICD_ACTIVE))
                {
                    if (pending_roll_direction != instance->GetData(DATA_DW_SPINE_ROLL_INFO))
                    {
                        //TC_LOG_ERROR("sql.sql", "Spine: ACTION_ROLL_RIGHT - success");
                        pending_roll_direction = instance->GetData(DATA_DW_SPINE_ROLL_INFO);
                        if (events.HasEvent(EVENT_ROLL))
                            events.CancelEvent(EVENT_ROLL);

                        events.ScheduleEvent(EVENT_ROLL, 10000);
                    }
                }
                else
                    pending_roll_direction = ACTION_ROLL_MIDDLE;
                    break;
                case ACTION_ROLL_LEFT:
                    if (!events.HasEvent(EVENT_ROLL_ICD_ACTIVE))
                    { 
                        if (pending_roll_direction != instance->GetData(DATA_DW_SPINE_ROLL_INFO))
                        {
                            pending_roll_direction = instance->GetData(DATA_DW_SPINE_ROLL_INFO);
                            //TC_LOG_ERROR("sql.sql", "Spine: ACTION_ROLL_LEFT - success");
                            if (events.HasEvent(EVENT_ROLL))
                                events.CancelEvent(EVENT_ROLL);

                                events.ScheduleEvent(EVENT_ROLL, 10000);
                        }
                    }
                    else
                        pending_roll_direction = ACTION_ROLL_MIDDLE;
                    break;
                case ACTION_ROLL_MIDDLE:
                    if (pending_roll_direction != instance->GetData(DATA_DW_SPINE_ROLL_INFO))
                    {
                        pending_roll_direction = instance->GetData(DATA_DW_SPINE_ROLL_INFO);
                        //TC_LOG_ERROR("sql.sql", "Spine: ACTION_ROLL_MIDDLE - success");
                        if (events.HasEvent(EVENT_ROLL))
                            events.CancelEvent(EVENT_ROLL);
                    }
                    break;
                case ACTION_CORRUPTED_BLOOD:
                    if (!corruptedBlood)
                    {
                        events.ScheduleEvent(EVENT_SUMMON_CORRUPTED_BLOOD, 9000);
                        corruptedBlood = true;
                    }
                    break;
            }
        }

        void EnterCombat(Unit* victim) override
        {
            DoCast(SPELL_SPAWNING_CONTROLLER);
            DoCast(SPELL_ROLL_CONTROL);
            DoCast(SPELL_PET_HACK);
            AddEncounterFrame();
            events.ScheduleEvent(EVENT_SEND_ENCOUNTER_FRAME, 1000);
            events.ScheduleEvent(EVENT_PLAYER_CHECK, 5000);
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
            phase = 1;
            _corruptedSummoned = 0;
            _EnterCombat();
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ROLL:
                    if (instance->GetData(DATA_DW_SPINE_ROLL_INFO) == ACTION_ROLL_LEFT || instance->GetData(DATA_DW_SPINE_ROLL_INFO) == ACTION_ROLL_RIGHT)
                    {
                        //TC_LOG_ERROR("sql.sql", "EVENT_ROLL, data is %u", instance->GetData(DATA_DW_SPINE_ROLL_INFO));
                        SendRoll(instance->GetData(DATA_DW_SPINE_ROLL_INFO) == ACTION_ROLL_LEFT);
                        events.ScheduleEvent(EVENT_ROLL_ICD_ACTIVE, 20000);
                    }
                    //else
                        //TC_LOG_ERROR("sql.sql", "EVENT_ROLL failed, data is %u", instance->GetData(DATA_DW_SPINE_ROLL_INFO));
                    break;
                    case EVENT_CHECK_WIPE:
                        //TC_LOG_ERROR("sql.sql", "Executing check wipe");
                        if (checkWipe())
                        {
                            //TC_LOG_ERROR("sql.sql", "Wipe recognized at glubtok.");
                            me->DeleteThreatList();
                            me->CombatStop(true);
                            Reset();
                            me->Respawn(true);
                            me->DeleteThreatList();
                        }
                        else
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                        break;

                    case EVENT_SUMMON_CORRUPTED_BLOOD:
                    {
                        std::list<Creature*> spawnerList;
                        GetCreatureListWithEntryInGrid(spawnerList, me, NPC_DW_SPINE_SPAWNER, 200.0f);
                        spawnerList.remove_if([](WorldObject* target)
                        {
                            return !target->ToUnit()->HasAura(SPELL_SUMMON_GRASPING_TENDRIS);
                        });

                        if (!spawnerList.empty())
                        {
                            Unit* target = Trinity::Containers::SelectRandomContainerElement(spawnerList);
                            if (target)
                                target->CastSpell(target, SPELL_SUMMON_SLIME, true);
                        }

                        uint32 timer = 9000;
                        if (_corruptedSummoned <= 90)
                            _corruptedSummoned++;

                        if (IsHeroic())
                        {
                            if (_corruptedSummoned <= 9)
                                timer = 9000;
                            else if (_corruptedSummoned <= 9+11)
                                timer = 8000;
                            else if (_corruptedSummoned <= 9+11+13)
                                timer = 7000;
                            else if (_corruptedSummoned <= 9+11+13+15)
                                timer = 6000;
                            else if (_corruptedSummoned <= 9+11+13+15+26)
                                timer = 5000;
                            else if (_corruptedSummoned <= 9+11+13+15+26+12)
                                timer = 4000;
                            else if (_corruptedSummoned <= 9+11+13+15+26+12+4)
                                timer = 3000;
                            else
                                timer = 2000;
                        }

                        events.ScheduleEvent(EVENT_SUMMON_CORRUPTED_BLOOD, timer);
                        break;
                    }
                    case EVENT_SEND_ENCOUNTER_FRAME:
                        AddEncounterFrame();
                        events.ScheduleEvent(EVENT_SEND_ENCOUNTER_FRAME, 5000);
                        break;
                    case EVENT_PLAYER_CHECK:
                    {
                        auto list_of_players = me->GetPlayersInRange(1000.f, true);
                        if (list_of_players.size())
                            for (auto current_target_within_list : list_of_players)
                            {
                                if (me->GetDistance(current_target_within_list) > 120.f || me->GetPositionZ() > (current_target_within_list->GetPositionZ() + 15.f))
                                {
                                    //TC_LOG_ERROR("sql.sql", "player %u is too far away, killing.", current_target_within_list->GetGUID());
                                    me->Kill(current_target_within_list, true, false);
                                }
                                //else
                                    //TC_LOG_ERROR("sql.sql", "player %u is within range: %u, my z: %u, their z: %u", (uint32)(double)(me->GetDistance(current_target_within_list)), (uint32)(double)(me->GetPositionZ()), (uint32)(double)(current_target_within_list->GetPositionZ()));
                            }

                        DoCastAOE(SPELL_CHECK_FOR_PLAYERS);
                        events.ScheduleEvent(EVENT_PLAYER_CHECK, 5000);
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        private:
            bool init;
            bool corruptedBlood;
            uint32 phase;
            uint32 _corruptedSummoned;
            uint32 pending_roll_direction { 10 };
            uint64 corruption_spawner_guids[8]
            {
                0, 0, 0, 0,
                0, 0, 0, 0,
            };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_dw_spineAI(creature);
    }
};

class npc_ds_corruption : public CreatureScript
{
public:
    npc_ds_corruption() : CreatureScript("npc_ds_corruption") {}

    struct npc_ds_corruptionAI : public Scripted_NoMovementAI
    {
        npc_ds_corruptionAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            instance = me->GetInstanceScript();
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        void Reset() override
        {
            damageTaken = 0;
            casts = 0;
            isUnactive = false;
            events.Reset();
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply)
            {
                if (me->GetEntry() == NPC_CORRUPTION)
                {
                    //TC_LOG_ERROR("sql.sql", "NPC_CORRUPTION, removing passenger.");
                    if (auto v = me->GetVehicleKit())
                        v->RemovePassenger(passenger);
                    return;
                }

                me->SetFacingTo((3 * M_PI) / 2);
                me->SetReactState(REACT_PASSIVE);
                passenger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                passenger->ToCreature()->SetReactState(REACT_PASSIVE);

                if (auto c = passenger->ToCreature())
                    if (c->GetEntry() == NPC_SKY_CAPTAIN_SWAYZE)
                    if (auto AI = c->AI())
                        AI->DoAction(55);
            }
            else
            {
                //TC_LOG_ERROR("sql.sql", "Removing passenger: %u", passenger->GetEntry());
                me->SetFacingTo((3 * M_PI) / 2);
                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);

                isUnactive = false;
                me->SetReactState(REACT_AGGRESSIVE);

                passenger->ToCreature()->DespawnOrUnsummon(4000);
                DoAction(ACTION_ACTIVATE_CORRUPTION);

                if (me->GetEntry() != NPC_CORRUPTION)
                {
                    if (!events.HasEvent(EVENT_FIERY_GRIP))
                    events.ScheduleEvent(EVENT_FIERY_GRIP, Is25ManRaid() ? 15000: 30000);

                    if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                    events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 1);
                }

            }
        }

        void JustDied(Unit* killer) override
        {
            Creature* spawner = me->FindNearestCreature(NPC_DW_SPINE_SPAWNER, 5.0f);
            if (!spawner)
                return;


            spawner->CastSpell(spawner, SPELL_SUMMON_GRASPING_TENDRIS, true);
            spawner->CastWithDelay(urand(4500, 5000), spawner, SPELL_SUMMON_ELEMENTARY, true);
            me->CastSpell(spawner, SPELL_ACTIVATE_SPAWMER, true);
            if (auto deathwing = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
                deathwing->AI()->DoAction(ACTION_CORRUPTED_BLOOD);

            Creature* corruption = GetNearCorruption(me, true, 200.0f);
            if (!corruption)
            {
                std::list<Creature*>creatureList;
                GetCreatureListWithEntryInGrid(creatureList, me, NPC_DW_SPINE_SPAWNER, 200.0f);
                creatureList.remove_if([spawner](Creature* crea)
                {
                    return !crea->AI()->GetGUID() || crea == spawner;
                });
                if (!creatureList.empty())
                    corruption = Trinity::Containers::SelectRandomContainerElement(creatureList);
                if (corruption) // actually now its the spawner
                {
                    corruption->RemoveAllAuras();
                    corruption->AI()->DoAction(ACTION_ACTIVATE_CORRUPTION);
                }
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                if (me->HealthBelowPctDamaged((start_cast_pct- 20.f), damage))
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_ACTIVATE_FIRST_CORRUPTION:
                    isUnactive = true;
                    break;
                case ACTION_REACTIVATE_ACTIVATE_CORRUPTION:
                    // should trigger event npcs removed from tentacle
                    isUnactive = false;
                    break;
                default:
                    break;
            }
        }

        void EnterCombat(Unit* who)
        {
                if (!events.HasEvent(EVENT_FIERY_GRIP))
                    events.ScheduleEvent(EVENT_FIERY_GRIP, Is25ManRaid() ? 15000 : 30000);

                if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                    events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 0);

                if (!me->HasReactState(REACT_PASSIVE))
                    isUnactive = false;
        }

        void UpdateAI(uint32 const diff) override
        {
            if (isUnactive || me->HasReactState(REACT_PASSIVE))
            {
                //TC_LOG_ERROR("sql.sql", "entry %u is returning updateAI, reasons: %u %u", me->GetEntry(), isUnactive, me->HasReactState(REACT_PASSIVE));
                return;
            }

                //if (casts < ( ? 2 : 4))

            if (!events.HasEvent(EVENT_FIERY_GRIP))
                events.ScheduleEvent(EVENT_FIERY_GRIP, Is25ManRaid() ? 15000 : 30000);

            if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 0);


            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                case EVENT_FIERY_GRIP:
                    if (me->HasUnitState(UNIT_STATE_CASTING))events.ScheduleEvent(eventid, 250);
                    else
                    {
                        start_cast_pct = me->GetHealthPct();
                        DoCast(SPELL_FIERY_GRIP);

                        if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                            events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 1);

                        if (!events.HasEvent(EVENT_FIERY_GRIP))
                            events.ScheduleEvent(EVENT_FIERY_GRIP, Is25ManRaid() ? 15000 : 30000);

                    }
                    break;
                case EVENT_SEARING_PLASMA_CAST:
                    if (me->HasUnitState(UNIT_STATE_CASTING))events.ScheduleEvent(eventid, 250);
                    else
                    {
                        start_cast_pct = me->GetHealthPct();
                        DoCast(SPELL_SEARING_PLASMA_INIT);

                        if (events.GetTimeUntilEvent(EVENT_FIERY_GRIP) > 8000)
                        {
                            if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                                events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 1);
                        }
                        else
                            if (!events.HasEvent(EVENT_SEARING_PLASMA_CAST))
                                events.ScheduleEvent(EVENT_SEARING_PLASMA_CAST, 8500);

                    }
                    break;
                default:
                    break;
                }

        }

    private:
        InstanceScript* instance;
        uint32 damageTaken;
        uint8 casts;
        bool isUnactive;
        EventMap events;
        float start_cast_pct{ 100.f };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_corruptionAI(creature);
    }
};

class npc_ds_hideous_amalgamation : public CreatureScript
{
public:
    npc_ds_hideous_amalgamation() : CreatureScript("npc_ds_hideous_amalgamation") {}

    struct npc_ds_hideous_amalgamationAI : public ScriptedAI
    {
        npc_ds_hideous_amalgamationAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        void IsSummonedBy(Unit* /*summoner*/) override
        {
            DoCast(SPELL_ACT_BLOOD_ABSORB_BAR);
            DoCast(SPELL_ABSORB_BLOOD);
            if (auto target = me->FindNearestPlayer(200.0f))
                AttackStart(target);
            canCastBlast = true;
            if (IsHeroic())
                events.ScheduleEvent(EVENT_BLOOD_CORRUPTION, urand(3000, 8000));
        }

        void JustDied(Unit* killer) override
        {
            if (IsHeroic())
                DoCast(me, SPELL_DEGRADATION, true);
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (me->HealthBelowPctDamaged(1, damage))
            {
                if (canCastBlast && me->HasAura(SPELL_SUPERHEATED_NUCLEUS))
                {
                    canCastBlast = false;
                    damage = 0;
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->CombatStop();
                    if (IsHeroic())
                        DoCast(me, SPELL_DEGRADATION, true);
                    DoCast(SPELL_NUCLEAR_BLAST);
                }
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE && id == EVENT_JUMP)
                me->DespawnOrUnsummon();
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                case EVENT_BLOOD_CORRUPTION:
                    DoCastAOE(SPELL_BLOOD_CORRUPTION_DEATH);
                    //events.ScheduleEvent(EVENT_BLOOD_CORRUPTION, 30000);
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
            DoMeleeAttackIfReady();
        }

    private:
        InstanceScript* instance;
        EventMap events;
        bool canCastBlast;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_hideous_amalgamationAI(creature);
    }
};

class npc_ds_corrupted_blood : public CreatureScript
{
public:
    npc_ds_corrupted_blood() : CreatureScript("npc_ds_corrupted_blood") {}

    struct npc_ds_corrupted_bloodAI : public ScriptedAI
    {
        npc_ds_corrupted_bloodAI(Creature* creature) : ScriptedAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        void IsSummonedBy(Unit* /*summoner*/)
        {
            DoCast(SPELL_DECREASE_CRIT);
            canExplode = true;
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (!canExplode)
            {
                damage = 0;
            }
            else if (me->HealthBelowPctDamaged(1, damage))
            {
                canExplode = false;
                me->SetReactState(REACT_PASSIVE);
                me->CombatStop();
                damage = 0;
                // EnterEvadeMode();
                DoCastAOE(SPELL_BURST, true);
                DoCast(me, SPELL_RESIDUE, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);

                if (auto spawner = me->FindNearestCreature(NPC_DW_SPINE_SPAWNER, 100.00f))
                {
                    me->SetWalk(true);
                    Position pos(*spawner);
                    pos.m_positionZ = 264.50f;
                    me->GetMotionMaster()->MovePoint(POINT_SPAWNER, pos);
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == POINT_SPAWNER)
            {
                if (auto spawner = me->FindNearestCreature(NPC_DW_SPINE_SPAWNER, 100.00f))
                    spawner->CastSpell(spawner, SPELL_SUMMON_SLIME, true);

                me->DespawnOrUnsummon();
            }
        }
    private:
        bool canExplode;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_corrupted_bloodAI(creature);
    }
};

class npc_ds_burning_tendon : public CreatureScript
{
public:
    npc_ds_burning_tendon() : CreatureScript("npc_ds_burning_tendon") {}

    struct npc_ds_burning_tendonAI : public Scripted_NoMovementAI
    {
        npc_ds_burning_tendonAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->SetHealth(me->GetMaxHealth());
            instance = me->GetInstanceScript();
        }

        void Reset() override
        {
            RemoveEncounterFrame();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        void JustDied(Unit* killer) override
        {
            if (auto backplate = instance->GetGameObject(DATA_DW_SPINE_BACKPLATE_1 + instance->GetData(DATA_DW_SPINE_BACKPLATE_CNT)))
            {
                if (me->GetEntry() == NPC_BURNING_TENDONS_L)
                    backplate->SendActivateAnimKit(ANIM_LOOSE_BACKPLATE_LEFT);
                else
                    backplate->SendActivateAnimKit(ANIM_LOOSE_BACKPLATE_RIGHT);

                backplate->SetGoState(GO_STATE_ACTIVE);
            }
            if (instance->GetData(DATA_DW_SPINE_BACKPLATE_CNT) == 2)
            {
                if (auto spine = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
                    spine->AI()->DoAction(ACTION_FINISH_FIGHT);
                return;
            }
            if (auto spine = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
            {
                spine->AI()->DoAction(ACTION_CORRUPTION);
            }
            instance->SetData(DATA_DW_SPINE_BACKPLATE_CNT, instance->GetData(DATA_DW_SPINE_BACKPLATE_CNT) + 1);
            if (auto tendron = me->FindNearestCreature(me->GetEntry() == NPC_BURNING_TENDONS_L ? NPC_BURNING_TENDONS_R : NPC_BURNING_TENDONS_L, 20.0f))
                tendron->DespawnOrUnsummon();
            RemoveEncounterFrame();
        }

    private:
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_burning_tendonAI(creature);
    }
};

class npc_ds_spine_spawner : public CreatureScript
{
public:
    npc_ds_spine_spawner() : CreatureScript("npc_ds_spine_spawner") {}

    struct npc_ds_spine_spawnerAI : public Scripted_NoMovementAI
    {
        npc_ds_spine_spawnerAI(Creature* creature) : Scripted_NoMovementAI(creature), summons(me)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            if (instance->GetBossState(DATA_SPINE_OF_DEATHWING) == NOT_STARTED)
            {
                summons.DespawnAll();
                me->RemoveAllAuras();
                corruptionGUID = 0;
            }
        }

        void EnterEvadeMode() override 
        {

        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_ACTIVATE_CORRUPTION:
                    if (auto corruption = DoSummon(NPC_CORRUPTION, *me, 300000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT))
                        corruptionGUID = corruption->GetGUID();
                    break;
                case ACTION_ACTIVATE_FIRST_CORRUPTION:
                    if (me->GetDBTableGUIDLow() == 254390 || me->GetDBTableGUIDLow() == 254396)
                    {
                            //left
                            if (auto corruption = DoSummon(me->GetDBTableGUIDLow() == 254390 ? NPC_CORRUPTION_VEH_L : NPC_CORRUPTION_VEH_R, *me, 300000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT))
                            {
                                corruption->SetReactState(REACT_PASSIVE);

                                if (auto yeet_npc = DoSummon(me->GetDBTableGUIDLow() == 254390 ? NPC_KEANU_ORC : NPC_SKY_CAPTAIN_SWAYZE, *me, 300000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT))
                                {
                                    yeet_npc->EnterVehicle(corruption);
                                }
                                corruptionGUID = corruption->GetGUID();
                                corruption->AI()->DoAction(ACTION_ACTIVATE_FIRST_CORRUPTION);
                            }

                    }
                    else
                    if (auto corruption = DoSummon(NPC_CORRUPTION, *me, 300000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT))
                    {
                        corruptionGUID = corruption->GetGUID();
                        corruption->AI()->DoAction(ACTION_ACTIVATE_FIRST_CORRUPTION);
                    }
                    break;
                default:
                    break;
            }
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            summon->SetInCombatWithZone();
        }

        uint64 GetGUID(int32 type = 0) const override
        {
            return corruptionGUID;
        }

    private:
        SummonList summons;
        InstanceScript* instance;
        uint64 corruptionGUID;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_spine_spawnerAI(creature);
    }
};

// 109022
class spell_dw_spine_check_player : public SpellScriptLoader
{
public:
    spell_dw_spine_check_player() : SpellScriptLoader("spell_dw_spine_check_player") { }

    class spell_dw_spine_check_player_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dw_spine_check_player_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
            {
                if (auto c = GetCaster())
                {
                    c->CastSpell((Unit*)NULL, SPELL_TELEPORT_ALL_TO_GUNSHIP, true);
                    if (auto cr = c->ToCreature())
                    {
                        cr->AI()->EnterEvadeMode();
                        cr->AI()->Reset();
                        if (auto i = cr->GetInstanceScript())
                        {
                            i->DoRemoveAurasDueToSpellOnPlayers(105479);
                            i->DoRemoveAurasDueToSpellOnPlayers(109362);
                            i->DoRemoveAurasDueToSpellOnPlayers(109363);
                            i->DoRemoveAurasDueToSpellOnPlayers(109364);
                        }
                    }
                }
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dw_spine_check_player_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dw_spine_check_player_SpellScript();
    }
};

// 105241
class spell_ds_absorb_blood : public SpellScriptLoader
{
public:
    spell_ds_absorb_blood() : SpellScriptLoader("spell_ds_absorb_blood") {}

    class spell_ds_absorb_blood_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_absorb_blood_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            if (auto c = GetCaster())
                if (auto u = GetHitUnit())
                {
                    c->CastSpell(c, SPELL_ENERGIZE, true);

                    if (auto cr = u->ToCreature())
                    cr->DespawnOrUnsummon(1000);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_ds_absorb_blood_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ds_absorb_blood_SpellScript();
    }
};

// 105248
class spell_ds_absorb_blood_aura : public SpellScriptLoader
{
public:
    spell_ds_absorb_blood_aura() : SpellScriptLoader("spell_ds_absorb_blood_aura") { }

    class spell_ds_absorb_blood_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ds_absorb_blood_aura_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SUPERHEATED_NUCLEUS))
                return false;

            return true;
        }

        void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            if (GetStackAmount() >= 9)
                if (auto o = GetOwner())
                    if (auto u = o->ToUnit())
                    if (!u->HasAura(SPELL_SUPERHEATED_NUCLEUS))
                    if (!u->HasUnitState(UNIT_STATE_CASTING))
                    {
                        u->CastSpell(u, SPELL_SUPERHEATED_NUCLEUS, true);
                    }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_ds_absorb_blood_aura_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ds_absorb_blood_aura_AuraScript();
    }
};

// 109379
class spell_ds_searing_plasma : public SpellScriptLoader
{
public:
    spell_ds_searing_plasma() : SpellScriptLoader("spell_ds_searing_plasma") {}

    class spell_ds_searing_plasma_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_searing_plasma_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(Trinity::UnitAuraCheck(true, sSpellMgr->GetSpellIdForDifficulty(SPELL_SEARING_PLASMA_TR, GetCaster())));
            if (!targets.empty())
                Trinity::Containers::RandomResizeList(targets, 1);
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            GetCaster()->CastSpell(GetHitUnit(), GetEffectValue(), true);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ds_searing_plasma_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_ds_searing_plasma_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ds_searing_plasma_SpellScript();
    }
};

// 105845
class spell_ds_nuclear_blast : public SpellScriptLoader
{
public:
    spell_ds_nuclear_blast() : SpellScriptLoader("spell_ds_nuclear_blast") {}

    class spell_ds_nuclear_blast_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_nuclear_blast_SpellScript);

        void HandleAfterCast()
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_NUCLEAR_BLAST_TRIGGERED, true);
            GetCaster()->ToCreature()->DespawnOrUnsummon(500);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_ds_nuclear_blast_SpellScript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ds_nuclear_blast_SpellScript();
    }
};

// 105846
class spell_ds_nuclear_blast_triggered : public SpellScriptLoader
{
public:
    spell_ds_nuclear_blast_triggered() : SpellScriptLoader("spell_ds_nuclear_blast_triggered") { }

    class spell_ds_nuclear_blast_triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_nuclear_blast_triggered_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SEAL_ARMOR_BREACH_L) ||
                !sSpellMgr->GetSpellInfo(SPELL_SEAL_ARMOR_BREACH_R))
                return false;

            return true;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            float distance      { 100.f };
            WorldObject* target{ nullptr };

            //TC_LOG_ERROR("sql.sql", "nuke blast opening up tendons. %u found.", targets.size());

            if (auto c = GetCaster())
            {
                bool isLeft { false };

                if (c->GetPositionX() < SPINE_MIDDLE_LR_BORDER)
                    isLeft = true;


                if (targets.size())
                    for (auto itr = targets.begin(); itr != targets.end(); itr++)
                        if (auto t = (*itr))
                            if (isLeft ? (t->GetPositionX() < SPINE_MIDDLE_LR_BORDER) : (t->GetPositionX() >= SPINE_MIDDLE_LR_BORDER))//filter condition
                                targets.erase(itr);

                if (targets.size())
                    for (auto itr = targets.begin(); itr != targets.end(); itr++)
                        if (auto t = (*itr))
                            if (c->GetExactDist(t) > distance)//filter condition
                            {
                                if (targets.size() > 1)
                                    targets.erase(itr);
                            }
                            else
                            {
                                distance = c->GetExactDist(t);
                                target = t;
                            }

                if (targets.size())
                    for (auto itr = targets.begin(); itr != targets.end(); itr++)
                        if (auto t = (*itr))
                            if (t != target)//filter condition
                            {
                                if (targets.size() > 1)
                                    targets.erase(itr);
                            }

                        //TC_LOG_ERROR("sql.sql", "final filtered size: %u", targets.size());
            }
        }

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (auto u = GetHitUnit())
            if (auto instance = GetCaster()->GetInstanceScript())
            {
                if (auto backplate = instance->GetGameObject(DATA_DW_SPINE_BACKPLATE_1 + instance->GetData(DATA_DW_SPINE_BACKPLATE_CNT)))
                {
                    if (u->GetEntry() == NPC_BURNING_TENDONS_L)
                    {

                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, backplate->GetEntry());
                        if (auto trigger = u->SummonCreature(54638, *u, TEMPSUMMON_TIMED_DESPAWN, 24000))
                        {
                            trigger->ConnectToObjTransport(backplate, true);
                            u->CastCustomSpell(SPELL_SEAL_ARMOR_BREACH_L, values, trigger, TRIGGERED_NONE, NULL, NULL, u->GetGUID());
                        }

                        backplate->SendActivateAnimKit(ANIM_BACKPLATE_LEFT);
                        GetHitUnit()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        GetHitCreature()->AI()->AddEncounterFrame();
                    }
                    else
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_BASE_POINT0, backplate->GetEntry());
                        if (auto trigger = u->SummonCreature(54638, *u, TEMPSUMMON_TIMED_DESPAWN, 24000))
                        {
                            trigger->ConnectToObjTransport(backplate, true);
                            u->CastCustomSpell(SPELL_SEAL_ARMOR_BREACH_R, values, trigger, TRIGGERED_NONE, NULL, NULL, u->GetGUID());
                        }

                        backplate->SendActivateAnimKit(ANIM_BACKPLATE_RIGHT);
                        GetHitUnit()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        GetHitCreature()->AI()->AddEncounterFrame();
                    }
                } 
                //else TC_LOG_ERROR("sql.sql", "else 1");
            }
            //else TC_LOG_ERROR("sql.sql", "else 2");
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ds_nuclear_blast_triggered_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnEffectHitTarget += SpellEffectFn(spell_ds_nuclear_blast_triggered_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ds_nuclear_blast_triggered_SpellScript();
    }
};

// 104621
class spell_dw_spine_roll_control : public SpellScriptLoader
{
public:
    spell_dw_spine_roll_control() : SpellScriptLoader("spell_dw_spine_roll_control") { }

    class spell_dw_spine_roll_control_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dw_spine_roll_control_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_ROLL_CONTROL_ROLL))
                return false;

            return true;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            isLeft      = 0;
            isRight     = 0;
            isMiddle    = 0;

            targets.remove_if([](WorldObject* target)
            {
                if (!target->ToPlayer())
                        return true;

                return target->ToUnit()->HasAura(SPELL_ROLL_CONTROL_AURA);
            });

            for (WorldObject const* obj : targets)
            {
                float left_border   { SPINE_LEFT_BORDER };
                float right_border  { SPINE_RIGHT_BORDER };
                //13,854.5

                if (obj->GetPositionX() < left_border)
                    ++isLeft;
                else if (obj->GetPositionX() > right_border)
                    ++isRight;
                else
                    ++isMiddle;
            }
        }

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (auto instance = GetCaster()->GetInstanceScript())
            {
                if (isLeft > (isRight + isMiddle))
                {
                    if (instance->GetData(DATA_DW_SPINE_ROLL_INFO) != ACTION_ROLL_LEFT)
                    {
                        GetCaster()->ToCreature()->AI()->Talk(EMOTE_WARN_ROLL_LEFT, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                        std::list<Creature*> creatureList;
                        GetCaster()->GetCreatureListWithEntryInGrid(creatureList, NPC_CORRUPTION, 200.0f);
                        for (auto target : creatureList)
                            target->AI()->DoAction(ACTION_REACTIVATE_ACTIVATE_CORRUPTION);
                    }
                    instance->SetData(DATA_DW_SPINE_ROLL_INFO, ACTION_ROLL_LEFT);

                    if (auto spine = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
                        spine->AI()->DoAction(ACTION_ROLL_LEFT);
                }
                else if (isRight > (isLeft + isMiddle))
                {
                    if (instance->GetData(DATA_DW_SPINE_ROLL_INFO) != ACTION_ROLL_RIGHT)
                    {
                        GetCaster()->ToCreature()->AI()->Talk(EMOTE_WARN_ROLL_RIGHT, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                        std::list<Creature*> creatureList;
                        GetCaster()->GetCreatureListWithEntryInGrid(creatureList, NPC_CORRUPTION, 200.0f);
                        for (auto target : creatureList)
                            target->AI()->DoAction(ACTION_REACTIVATE_ACTIVATE_CORRUPTION);
                    }
                    instance->SetData(DATA_DW_SPINE_ROLL_INFO, ACTION_ROLL_RIGHT);
                    if (auto spine = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
                        spine->AI()->DoAction(ACTION_ROLL_RIGHT);
                }
                else
                {
                    instance->SetData(DATA_DW_SPINE_ROLL_INFO, ACTION_ROLL_MIDDLE);

                    if (auto spine = instance->GetCreature(DATA_SPINE_OF_DEATHWING))
                        spine->AI()->DoAction(ACTION_ROLL_MIDDLE);
                    // GetCaster()->ToCreature()->AI()->Talk(EMOTE_WARN_ROLL_RIGHT, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                }
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dw_spine_roll_control_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHit += SpellEffectFn(spell_dw_spine_roll_control_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }

    private:
        uint8 isLeft;
        uint8 isRight;
        uint8 isMiddle;
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dw_spine_roll_control_SpellScript();
    }
};

// 105740
class spell_dw_spine_roll_control_roll : public SpellScriptLoader
{
public:
    spell_dw_spine_roll_control_roll() : SpellScriptLoader("spell_dw_spine_roll_control_roll") { }

    class spell_dw_spine_roll_control_roll_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dw_spine_roll_control_roll_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            uint32 graspingTendris = sSpellMgr->GetSpellIdForDifficulty(SPELL_GRASPING_TENDRIS_AURA, GetCaster());
            uint32 fieryGrip = sSpellMgr->GetSpellIdForDifficulty(SPELL_FIERY_GRIP, GetCaster());

            targets.remove_if([graspingTendris, fieryGrip](WorldObject* target)
            {
                return target->ToUnit()->HasAura(graspingTendris) || target->ToUnit()->HasAura(fieryGrip) || target->ToUnit()->HasAura(SPELL_ROLL_CONTROL_AURA);
            });

			for (auto target : targets)
			{
				target->ToUnit()->GetMotionMaster()->ForceMoveJump(-13875.01f, -13769.59f, 285.1783f, 15.0f, 20.0f);
				target->ToUnit()->GetMotionMaster()->MoveFall();
			}
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dw_spine_roll_control_roll_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dw_spine_roll_control_roll_SpellScript();
    }
};

// 105777
class spell_ds_roll_control_aura : public SpellScriptLoader
{
public:
    spell_ds_roll_control_aura() : SpellScriptLoader("spell_ds_roll_control_aura") { }

    class spell_ds_roll_control_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ds_roll_control_aura_AuraScript);

        void Remove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            GetTarget()->Kill(GetTarget());
        }

        void Register() override
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_ds_roll_control_aura_AuraScript::Remove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ds_roll_control_aura_AuraScript();
    }
};

// 105847, 105848
class spell_ds_seal_armor_breach : public SpellScriptLoader
{
public:
    spell_ds_seal_armor_breach() : SpellScriptLoader("spell_ds_seal_armor_breach") { }

    class spell_ds_seal_armor_breach_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ds_seal_armor_breach_AuraScript);

        void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            // GetCaster()->ToCreature()->AI()->EnterEvadeMode();
            // GetCaster()->SetHealth(GetCaster()->GetMaxHealth());
            GetCaster()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
            GetCaster()->ToCreature()->AI()->RemoveEncounterFrame();
            if (auto instance = GetCaster()->GetInstanceScript())
            {
                if (auto backplate = instance->GetGameObject(DATA_DW_SPINE_BACKPLATE_1 + instance->GetData(DATA_DW_SPINE_BACKPLATE_CNT)))
                {
                    backplate->SetGoState(GO_STATE_READY);
                }
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_ds_seal_armor_breach_AuraScript::AfterApply, EFFECT_2, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ds_seal_armor_breach_AuraScript();
    }
};

// 106199, 106200
class spell_ds_blood_corruption_aura : public SpellScriptLoader
{
public:
    spell_ds_blood_corruption_aura() : SpellScriptLoader("spell_ds_blood_corruption_aura") { }

    class spell_ds_blood_corruption_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ds_blood_corruption_aura_AuraScript);

        bool Validate(SpellInfo const* spellInfo) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_OF_NELTHARION) ||
                !sSpellMgr->GetSpellInfo(SPELL_BLOOD_OF_DEATHWING))
                return false;

            return true;
        }

        void Dispel(DispelInfo* dispelInfo)
        {
            // Blood Corruption: Death generally must be dispelled 3-4 times before it turns into Blood Corruption: Earth
            uint32 aura = SPELL_BLOOD_CORRUPTION_DEATH;
            if (GetEffect(EFFECT_0)->GetAmount() >= 3 && (urand(0,100) < 50))
                aura = SPELL_BLOOD_CORRUPTION_EARTH;

            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_DURATION, GetDuration());
            values.AddSpellMod(SPELLVALUE_MAXDURATION, GetMaxDuration());
            values.AddSpellMod(SPELLVALUE_BASE_POINT0, GetEffect(EFFECT_0)->GetAmount() + 1);
            // hotfix 4.3.4: Blood Corruption: Earth will no longer morph into Blood Corruption: Death when dispelled.
            GetOwner()->ToUnit()->CastCustomSpell((GetId() == SPELL_BLOOD_CORRUPTION_EARTH) ? GetId() : aura, values, (Unit*)NULL, TRIGGERED_FULL_MASK, NULL, NULL, GetCasterGUID());
        }

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
            {
                GetTarget()->CastSpell(GetTarget(), GetId() == SPELL_BLOOD_CORRUPTION_DEATH ? SPELL_BLOOD_OF_DEATHWING : SPELL_BLOOD_OF_NELTHARION, true);
                // Blood Corruption: Earth now applies two stacks of Blood of Neltharion when it expires on 25-player heroic difficulty.
                if (GetTarget()->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC && GetId() == SPELL_BLOOD_CORRUPTION_EARTH)
                    GetTarget()->CastSpell(GetTarget(), SPELL_BLOOD_OF_NELTHARION, true);
            }
        }

        void Register() override
        {
            OnDispel += AuraDispelFn(spell_ds_blood_corruption_aura_AuraScript::Dispel);
            OnEffectRemove += AuraEffectRemoveFn(spell_ds_blood_corruption_aura_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    class spell_ds_blood_corruption_aura_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_blood_corruption_aura_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targetlist)
        {
            uint8 count = GetCaster()->ToCreature() ? (GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1) : 1;
            targetlist.remove_if(Trinity::UnitAuraCheck(true, GetSpellInfo()->Id == SPELL_BLOOD_CORRUPTION_DEATH ? SPELL_BLOOD_CORRUPTION_EARTH : SPELL_BLOOD_CORRUPTION_DEATH));
            targetlist.remove(GetCaster());
            if (!targetlist.empty())
                Trinity::Containers::RandomResizeList(targetlist, count);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ds_blood_corruption_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_ds_blood_corruption_aura_AuraScript();
    }

    SpellScript* GetSpellScript() const override
    {
        return new spell_ds_blood_corruption_aura_SpellScript();
    }
};

class spell_ds_energize_spine : public SpellScriptLoader
{
public:
    spell_ds_energize_spine() : SpellScriptLoader("spell_ds_energize_spine") { }

    class spell_ds_energize_spine_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ds_energize_spine_SpellScript);

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (auto u = GetHitUnit())
            if (auto c = u->ToCreature())
            {

                int32 alternativePower = std::min((GetHitUnit()->GetPower(POWER_ALTERNATE_POWER) + 1), 9);
                GetHitUnit()->SetPower(POWER_ALTERNATE_POWER, alternativePower);

                if (alternativePower > 1)
                {
                    if (Aura* blood = u->GetAura(SPELL_ABSORBED_BLOOD))
                        blood->SetStackAmount(alternativePower);
                    else
                    {
                        u->CastSpell(u, SPELL_ABSORBED_BLOOD, true);
                        if (Aura* blood = u->GetAura(SPELL_ABSORBED_BLOOD))
                            blood->SetStackAmount(alternativePower);
                    }
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_ds_energize_spine_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_ds_energize_spine_SpellScript();
    }
};

void AddSC_boss_dw_spine()
{
    new boss_dw_spine();
    new npc_ds_corruption();
    new npc_ds_corrupted_blood();
    new npc_ds_hideous_amalgamation();
    new npc_ds_burning_tendon();
    new npc_ds_spine_spawner();
    new spell_dw_spine_check_player();
    new spell_ds_absorb_blood();
    new spell_ds_absorb_blood_aura();
    new spell_ds_nuclear_blast();
    new spell_ds_nuclear_blast_triggered();
    new spell_dw_spine_roll_control();
    new spell_dw_spine_roll_control_roll();
    new spell_ds_roll_control_aura();
    new spell_ds_searing_plasma();
    new spell_ds_seal_armor_breach();
    new spell_ds_blood_corruption_aura();
    new spell_ds_energize_spine();
};
