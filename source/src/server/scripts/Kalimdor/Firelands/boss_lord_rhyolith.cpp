/*
* Copyright (C) 2012-2013 Reign of Darkness Network <http://reignofdarkness.net/>
*
* Este codigo es de uso exclusivo y privado para la red de gaming
* Reign of Darkness Network esta totalmente prohibida su distribucion y
* uso en proyectos ajenos, si has obtenido o encontrado este codigo
* publicamente envianos un email a administracion@reignofdarkness.net
* indicandonos como y donde has obtenido este codigo
*
* Recuerda, no distribuyas, no compartas y no subas este codigo a ningun
* lugar publico, usa siempre nuestros repositorios Git!
*/

#include "Player.h"
#include "ScriptedCreature.h"
#include "CreatureTextMgr.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "firelands.h"

enum Texts
{
    YELL_AGGRO                   = 0,
    YELL_KILLING_A_PLAYER        = 1,
    YELL_STOMP                   = 2,
    YELL_ACTIVATING_VOLCANO      = 3,
    YELL_ARMOR_WEAKENED          = 4,
    YELL_ARMOR_SHATTERED         = 5,
    YELL_ON_DEATH                = 6,
    YELL_SUMMON_ADDS             = 7,
    EMOTE_SUPERHEATED            = 8,
    EMOTE_VOLCANO                = 9,
    EMOTE_MAGMA                  = 10,
    EMOTE_PHASE2                 = 11,
};

enum Spells
{
    SPELL_BALANCE_BAR           = 98226,
    SPELL_BURNING_FEET          = 98837,
    SPELL_CONCUSSIVE_STOMP      = 97282,
    SPELL_OBSIDIAN_ARMOR        = 98632,
    SPELL_DRINK_MAGMA           = 98034,
    SPELL_IMMOLATION_AURA       = 99846,
    SPELL_IMMOLATION            = 98598,
    //    SPELL_UNLEASHED_FLAME_1     = 100974, // damage
    SPELL_UNLEASHED_FLAME       = 101313, // trigger aura
    SPELL_UNLEASHED_FLAME_SUMMON = 101324,
    SPELL_UNLEASHED_FLAME_VISUAL = 101314,
    SPELL_SUMMON_VOLCANO        = 98010,
    SPELL_LAVA_LINE             = 100650,
    SPELL_MOLTEN_ARMOR          = 98255,
    SPELL_MOLTEN_SPEW           = 98043,
    SPELL_SUPERHEATED           = 101304,

    SPELL_VOLCANO_AURA          = 98250,
    SPELL_MAGMA_FLOW_AURA       = 97225,
    SPELL_MAGMA_FLOW            = 97230,
    SPELL_MAGMA_FLOW_DAMAGE     = 97234,
    SPELL_SUMMON_ARMOR_FRAGMENT = 98558,
    SPELL_SIT_GROUND            = 84119,

    // 23:04:21.080
    SPELL_ERUPTION_PERIODIC     = 98264,
    SPELL_LAVA_TUBE             = 98265,
    SPELL_MAGMA                 = 98472,
    SPELL_LAVA_STRIKE           = 98489,
    SPELL_ERUPTION_BALLS        = 98491,
    SPELL_ERUPTION              = 98492,
    SPELL_HEATED_VOLCANO        = 98493,
    SPELL_EXPLOSION             = 97719,
    SPELL_VOLCANO_SMOKE         = 97699,
    SPELL_VOLCAN_CHECK_ACTIVE   = 98400,

    SPELL_SUMMON_SPARK          = 98553,
    SPELL_INFERNAL_RAGE         = 98596,
    SPELL_IMMOLATION_SPARK      = 98597,

    SPELL_SUMMON_FRAGMENTS      = 98135,
    SPELL_MELTDOWN              = 98646,

    SPELL_FUSE                  = 99875, // liq obsidian

    /*Pre-boss trash spells*/
    SPELL_FIRESTORM             = 99602,
    SPELL_SUMMON_LAVA_SPAWNS    = 99601,
    SPELL_SUMMON_FIRE_ELEM      = 99575,
    SPELL_SUMMON_LAVA_SPAWNS_T  = 99600,
    SPELL_SOUL_BURN             = 99567,
};

enum Events
{
    EVENT_CONCUSSIVE_STOMP          = 1,
    EVENT_SUMMON_VOLCANO            = 2,
    EVENT_MOLTEN_SPEW_END           = 3,
    EVENT_SUMMON_FRAGMENT           = 4,
    EVENT_SUMMON_SPARK              = 5,
    EVENT_DRINK_MAGMA               = 6,
    EVENT_MOLTEN_SPEW               = 7,
    EVENT_CHECK_LAVA_POOL           = 8,
    EVENT_SUPERHEATED               = 9,
    EVENT_START_PHASE_2             = 10,
    EVENT_UNLEASHED_FLAME           = 11,
    EVENT_NEW_POSITION_TO_WALK      = 12,
    EVENT_CONCUSSIVE_STOMP_PHASE_TWO = 13,
    EVENT_RESET_ENERGY              = 14,
    EVENT_ACTIVATE_VOLCANO          = 15,
    EVENT_START_PHASE_2_COMBAT      = 16,
    EVENT_REMOVE_PHASE_2_INTRO_AURA = 17,
    EVENT_CHECK_LAVA_LINE           = 18,
    EVENT_CHECK_FEET                = 1,
    EVENT_START_LAVA_LINE           = 2,
    EVENT_LAVA_LINE_EXPLOSION       = 3,
    EVENT_ERRUPTION_START           = 4,
    EVENT_ERRUPTION_END             = 5,
    EVENT_CHECK_RHYO_DISTANCE       = 6,

    EVENT_LEFT_FOOT_ORIENTATION     = 1,
    EVENT_MOVE_LEFT_FOOT            = 2,

    EVENT_RIGHT_FOOT_ORIENTATION    = 1,
    EVENT_MOVE_RIGHT_FOOT           = 2,

    EVENT_CHECK_FOR_PLAYERS         = 1,

    EVENT_INFERNAL_RAGE             = 1,

    //pre-boss mobs
    EVENT_CAST_FIRESTORM            = 101,
    EVENT_SUMMON_LAVA_SPAWNS        = 102,
    EVENT_CAST_SOULBURN             = 103,
};

enum eMiscs
{
    DATA_TURN_LEFT  = 1,
    DATA_TURN_RIGHT = 2,
    // misc
    POINT_WALK      = 3,
    MAX_PLATFORM_TOP = 18,
    POINT_CHARGE = 1003,
    NPC_UNLEASHED_FLAME = 54347,
    EVENT_GROUP_PHASE_ONE = 1,
    FOOT_SEAT_LEFT      = 0,
    FOOT_SEAT_RIGHT     = 1,
};

enum rhyolithAction
{
    ACTION_REDUCE_OBSIDIAN_ARMOR = 1,
    ACTION_CAST_MOLTEN_ARMOR     = 2,
    ACTION_PREPARE_LAVA_LINE     = 3,
    ACTION_REMOVE_MOLTEN_STACK   = 4,
    ACTION_KAR_RETREAT           = 5,
    ACTION_KAR_ALREADY_DEAD      = 6,
};

const Position CenterPlatform[1] =
{
    {-368.220f, -322.986f, 100.281f, 0}
};

static const Position platformTopPoints[18]
{
    {-359.32f, -379.60f, 100.0f, 0.0f},
    {-372.1f, -378.5f, 100.0f, 0.0f},
    {-387.6f, -372.05f, 100.0f, 0.0f},
    {-399.31f, -361.12f, 100.0f, 0.0f},
    {-414.41f, -356.56f, 100.0f, 0.0f},
    {-426.84f, -345.05f, 100.0f, 0.0f},
    {-429.56f, -326.01f, 100.0f, 0.0f},
    {-419.72f, -284.75f, 100.0f, 0.0f},
    {-409.46f, -269.25f, 100.0f, 0.0f},
    {-385.60f, -250.63f, 100.0f, 0.0f},
    {-357.71f, -255.06f, 100.0f, 0.0f},
    {-348.73f, -261.54f, 100.0f, 0.0f},
    {-314.86f, -291.96f, 100.0f, 0.0f},
    {-310.89f, -309.56f, 100.0f, 0.0f},
    {-321.74f, -327.95f, 100.0f, 0.0f},
    {-326.01f, -354.84f, 100.0f, 0.0f},
    {-334.30f, -367.43f, 100.0f, 0.0f},
    {-345.1f, -378.26f, 100.0f, 0.0f},
};

float MIN_DISTANCE_FROM_LAVA = 20.0f;
float VOLCANO_CRUSH_DISTANCE = 5.0f;

class boss_lord_rhyolith : public CreatureScript
{
    public:
        boss_lord_rhyolith() : CreatureScript("boss_lord_rhyolith") { }

    struct boss_lord_rhyolithAI : public BossAI
    {
        boss_lord_rhyolithAI(Creature* creature) : BossAI(creature, DATA_LORD_RHYOLITH), vehicle(creature->GetVehicleKit()) {}

        void InitializeAI()
        {
            Reset();
        }

        void Reset() override
        {
            if (sWorld->getBoolConfig(CONFIG_DISABLE_420_NERFS))
            {
                uint32 preNerfHealth = RAID_MODE(15427058, 46987976, 23443738, 70612804);
                me->SetMaxHealth(preNerfHealth);
                me->SetHealth(preNerfHealth);
            }

            _listPos.clear();
            me->SetReactState(REACT_AGGRESSIVE);
            _gravityCenter.m_positionX = 0;
            _gravityCenter.m_positionY = 0;
            for (uint32 i = 0; i < MAX_PLATFORM_TOP; i++)
            {
                _gravityCenter.m_positionX += platformTopPoints[i].m_positionX;
                _gravityCenter.m_positionY += platformTopPoints[i].m_positionY;
                _listPos.push_back(platformTopPoints[i]);
            }
            _gravityCenter.m_positionX /= MAX_PLATFORM_TOP;
            _gravityCenter.m_positionY /= MAX_PLATFORM_TOP;
            _gravityCenter.m_positionZ = 100.0f;
            lavaFlow = false;
            drinkMagma = false;
            phase2 = false;
            _canChangeDir = true;
            _orient = 4.94f;

            RemoveEncounterFrame();
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ERUPTION);
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_HIDE_NAMEPLATE | UNIT_FLAG2_REGENERATE_POWER | 0x01000000);
            me->SetAuraStack(SPELL_OBSIDIAN_ARMOR, me, 80);

            _energy = 25;
            me->SetDisplayId(me->GetNativeDisplayId());
            me->SetWalk(true);
            summonCount = 1; // fragment spark fragment fragment spark fragment fragment
            _Reset();
            events.Reset();

        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            //TC_LOG_ERROR("sql.sql", "Passenger boarded: %u %u %u %u", passenger->GetEntry(), seatId, apply, passenger->GetGUID());
            passenger->SetVisible(false);
            passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            passenger->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
            passenger->UpdateObjectVisibility();
        }

        void JustSummoned(Creature* summon) override
        {
            if (!me->isInCombat())
                if (summon->GetEntry() != NPC_LEFT_FOOT && summon->GetEntry() != NPC_RIGHT_FOOT)
                {
                    summon->DespawnOrUnsummon();
                    return;
                }

            if (sWorld->getBoolConfig(CONFIG_DISABLE_420_NERFS))
            {
                uint32 preNerfHealth = summon->GetMaxHealth() / 0.85f;
                summon->SetMaxHealth(preNerfHealth);
                summon->SetHealth(preNerfHealth);
            }
            summon->setActive(true);
            switch (summon->GetEntry())
            {
                case NPC_LEFT_FOOT:
                    //TC_LOG_ERROR("sql.sql", "summoned left foot");
                    summon->SetMaxHealth(me->GetMaxHealth() / 2.0f);
                    summon->SetHealth(me->GetMaxHealth() / 2.0f);
                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    if (auto v = me->GetVehicleKit())
                    {
                        //TC_LOG_ERROR("sql.sql", "left foot - me has vehicle kit");
                        if (auto p = v->GetPassenger(0))
                        {
                            //TC_LOG_ERROR("sql.sql", "left foot - me has passenger in seat 0 of vehicle kit");
                            auto npc = p->ToCreature();
                            npc->ExitVehicle();
                            npc->DespawnOrUnsummon();
                        }
                        if (auto veh_two = summon->GetVehicle())
                        {
                            //TC_LOG_ERROR("sql.sql", "left foot - already has vehicle, %u", veh_two->GetBase()->GetEntry());
                        }
                        else
                        {
                            //TC_LOG_ERROR("sql.sql", "left foot entering my vehicle");
                            summon->EnterVehicle(me, 0);
                        }
                    }
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "no vehicle kit found");
                    }

                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    summon->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                    break;
                case NPC_RIGHT_FOOT:
                    //TC_LOG_ERROR("sql.sql", "summoned right foot");
                    summon->SetMaxHealth(me->GetMaxHealth() / 2.0f);
                    summon->SetHealth(me->GetMaxHealth() / 2.0f);
                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    if (auto v = me->GetVehicleKit())
                    {
                        //TC_LOG_ERROR("sql.sql", "right foot - me has vehicle kit");
                        if (auto p = v->GetPassenger(1))
                        {
                            //TC_LOG_ERROR("sql.sql", "right foot - me has passenger in seat 1 of vehicle kit");
                            auto npc = p->ToCreature();
                            npc->ExitVehicle();
                            npc->DespawnOrUnsummon();
                        }
                        if (auto veh_two = summon->GetVehicle())
                        {
                            //TC_LOG_ERROR("sql.sql", "right foot - already has vehicle, %u", veh_two->GetBase()->GetEntry());
                        }
                        else
                        {
                            //TC_LOG_ERROR("sql.sql", "right foot entering my vehicle");
                            summon->EnterVehicle(me, 1);
                        }
                    }
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "no vehicle kit found");
                    }

                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    summon->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                    break;
                case NPC_FRAGMENT_OF_RHYOLITH:
                    summon->CastSpell(summon, SPELL_MELTDOWN, true);
                    break;
                case NPC_UNLEASHED_FLAME:
                {
                    summon->SetReactState(REACT_PASSIVE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    float angle = 0.0f;
                    Position pos;
                    std::list<Creature *> lavaTriggers;
                    me->GetCreatureListWithEntryInGrid(lavaTriggers, NPC_MAGMA_CHECK_TRIGGER, 100.0f);
                    bool cont = false;
                    while (angle < 2 * M_PI && !cont)
                    {
                        cont = true;
                        summon->GetNearPosition(pos, 10.0f, 0.0f);
                        for (auto itr = lavaTriggers.begin(); itr != lavaTriggers.end(); itr++)
                            if (Creature *trigger = *itr)
                                if (pos.GetExactDist(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ()) < 15.0f)
                                {
                                    cont = false;
                                    break;
                                }
                        angle += 0.3f;
                    }
                    summon->GetMotionMaster()->MoveAroundPoint(pos, 10.0f, 0, 254);
                    summon->AddAura(SPELL_UNLEASHED_FLAME, summon);
                    summon->AddAura(SPELL_UNLEASHED_FLAME_VISUAL, me);
                    break;
                }
                default:
                    break;
            }

            if (summon->GetEntry() == NPC_PILLAR || summon->GetEntry() == NPC_ARMOR_FRAGMENT_HEROIC)
                summons.Summon(summon);
            else
                BossAI::JustSummoned(summon);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_CAST_MOLTEN_ARMOR:
                {
                    uint32 stacks = 2;
                    uint32 moltenId = sSpellMgr->GetSpellIdForDifficulty(SPELL_MOLTEN_ARMOR, me);
                    if (Aura* armor = me->GetAura(moltenId))
                        stacks += armor->GetStackAmount();

                    me->SetAuraStack(moltenId, me, stacks);
                    if (auto leftFeet = me->FindNearestCreature(NPC_LEFT_FOOT, 50.0f))
                        me->SetAuraStack(moltenId, leftFeet, stacks);
                    if (auto rightFeet = me->FindNearestCreature(NPC_RIGHT_FOOT, 50.0f))
                        me->SetAuraStack(moltenId, rightFeet, stacks);
                    break;
                }
                case ACTION_REMOVE_MOLTEN_STACK:
                {
                    uint32 moltenId = sSpellMgr->GetSpellIdForDifficulty(SPELL_MOLTEN_ARMOR, me);
                    if (me->HasAura(moltenId))
                    {
                        me->RemoveAuraFromStack(moltenId);
                        if (auto leftFoot = me->FindNearestCreature(NPC_LEFT_FOOT, 50.0f))
                            leftFoot->RemoveAuraFromStack(moltenId);
                        if (auto rightFoot = me->FindNearestCreature(NPC_RIGHT_FOOT, 50.0f))
                            rightFoot->RemoveAuraFromStack(moltenId);
                    }
                    break;
                }
                case ACTION_REDUCE_OBSIDIAN_ARMOR:
                    if (auto leftFeet = me->FindNearestCreature(NPC_LEFT_FOOT, 50.0f))
                    {
                        if (Aura* aura = leftFeet->GetAura(SPELL_OBSIDIAN_ARMOR))
                        {
                            if (aura->GetStackAmount() <= 10)
                            {
                                Talk(YELL_ARMOR_SHATTERED);
                                leftFeet->RemoveAurasDueToSpell(SPELL_OBSIDIAN_ARMOR);
                                return;
                            }
                            Talk(YELL_ARMOR_WEAKENED);
                            aura->SetStackAmount(aura->GetStackAmount() - 10);
                        }
                    }
                    if (auto rightFeet = me->FindNearestCreature(NPC_RIGHT_FOOT, 50.0f))
                    {
                        if (Aura* aura = rightFeet->GetAura(SPELL_OBSIDIAN_ARMOR))
                        {
                            if (aura->GetStackAmount() <= 10)
                            {
                                rightFeet->RemoveAurasDueToSpell(SPELL_OBSIDIAN_ARMOR);
                                return;
                            }
                            Talk(YELL_ARMOR_WEAKENED);
                            aura->SetStackAmount(aura->GetStackAmount() - 10);
                        }
                    }
                    if (Aura* aura = me->GetAura(SPELL_OBSIDIAN_ARMOR))
                    {
                        Talk(EMOTE_VOLCANO);
                        if (aura->GetStackAmount() <= 10)
                            me->RemoveAurasDueToSpell(SPELL_OBSIDIAN_ARMOR);
                        else
                            aura->SetStackAmount(aura->GetStackAmount() - 10);
                    }
                    break;
                default:
                    break;
            }
        }

        void SummonedCreatureDespawn(Creature* summon) override
        {
            if (summon->GetEntry() == NPC_RIGHT_FOOT || summon->GetEntry() == NPC_LEFT_FOOT)
            {

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon);
            }
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(YELL_KILLING_A_PLAYER);
        }

        void JustDied(Unit* /*killer*/) override
        {
            instance->CompleteGuildCriteriaForGuildGroup(18097);
            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, 53772, 1, me);
            me->RemoveAllAuras();
            Talk(YELL_ON_DEATH);

            if (instance)
            {
                RemoveEncounterFrame();
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BALANCE_BAR);
            }

            _JustDied();

            Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
            for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (i->getSource()->HasQuestForItem(ITEM_HEART_OF_FLAME))
                {
                    DoCast(me, SPELL_SMOULDERING, true);
                    break;
                }
        }

        void EnterCombat(Unit* /*who*/) override
        {
            Talk(YELL_AGGRO);
            PositionZ = me->GetPositionZ();
            _EnterCombat();
            DoCast(SPELL_BALANCE_BAR);
            DoCast(me, SPELL_BURNING_FEET);
            _nextOrient = 0.0f;
            SetCombatMovement(false);
            AddEncounterFrame();
            events.ScheduleEvent(EVENT_CONCUSSIVE_STOMP, 14500, EVENT_GROUP_PHASE_ONE);
            events.ScheduleEvent(EVENT_SUMMON_FRAGMENT, 22500, EVENT_GROUP_PHASE_ONE);
            events.ScheduleEvent(EVENT_CHECK_LAVA_POOL, 1000, EVENT_GROUP_PHASE_ONE);
            events.ScheduleEvent(EVENT_SUPERHEATED, IsHeroic() ? 300000 : 360000);
            events.ScheduleEvent(EVENT_NEW_POSITION_TO_WALK, 1000, EVENT_GROUP_PHASE_ONE);
            events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, 30000, EVENT_GROUP_PHASE_ONE);
            events.ScheduleEvent(EVENT_CHECK_LAVA_LINE, 31000);
            me->SetReactState(REACT_PASSIVE);

            if (auto left = GetLeftLeg())
            {
                left->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                left->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                left->SetVisible(true);
                left->UpdateObjectVisibility();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, left);
            }
            else EnterEvadeMode();

            if (auto right = GetRightLeg())
            {
                right->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                right->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                right->SetVisible(true);
                right->UpdateObjectVisibility();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, right);
            }
            else EnterEvadeMode();
        }

        void RefreshAltPowerForPlayers()
        {
            Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
            if (!PlayerList.isEmpty())
                for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (Player *player = i->getSource())
                        if (player->isAlive())
                            player->SetPower(POWER_ALTERNATE_POWER, _energy);
        }

        void SetData(uint32 uiType, uint32 uiData) override
        {
            if (!_canChangeDir)
                return;

            switch (uiType)
            {
                case DATA_TURN_LEFT:
                case DATA_TURN_RIGHT:
                {
                    float coef = Is25ManRaid() ? 10000.0f : 4000.0f;
                    _energy += uiType == DATA_TURN_LEFT ? -(float)uiData / coef : (float)uiData / coef;
                    if (_energy < 0)
                        _energy = 0;
                    else if (_energy > 50)
                        _energy = 50;
                    _nextOrient = -(_energy * 2 * (M_PI / 12.0f) / 50.0f - M_PI / 12.0f);
                    RefreshAltPowerForPlayers();
                    events.CancelEvent(EVENT_RESET_ENERGY);
                    events.ScheduleEvent(EVENT_RESET_ENERGY, 2000);
                    break;
                }
            }
        }

        bool isInPlatform()
        {
            if (auto trigger = me->FindNearestCreature(NPC_MAGMA_CHECK_TRIGGER, 100.0f, true))
                if (me->IsWithinDist3d(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ(), 1.0f))
                    return false;
            return true;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!me->isInCombat())
                if (me->IsValidAttackTarget(who))
                    if (me->GetDistance(who) <= 75.f)
                        me->SetInCombatWithZone();
        }


        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                if (spell->Id == 102306 || spell->Id == 102307 || spell->Id == 102308 || spell->Id == 102309)
                    if (target->GetDistance2d(me->GetPositionX(), me->GetPositionY()) < (sWorld->getBoolConfig(CONFIG_DISABLE_420_NERFS) ? 20.0f : 10.0f))
                        target->KnockbackFrom(me->GetPositionX(), me->GetPositionY(), 10.0f, 7.5f);
        }

        void EnterEvadeMode()
        {
            summons.DespawnAll();
            RemoveEncounterFrame();
            CreatureAI::EnterEvadeMode();
            me->Respawn(true);

            if (auto left = GetLeftLeg())
            {
                left->Respawn(true);
                left->SetVisible(false);
            }
            if (auto right = GetRightLeg())
            {
                right->Respawn(true);
                right->SetVisible(false);
            }
        }


        void DamageTaken(Unit* who, uint32& damage)
        {
        }

        void UpdateAI(const uint32 diff) override
        {

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_RESET_ENERGY:
                    {
                        bool negative       { _energy < 0   };
                        float abs_energy    { fabs(_energy) };
                        if (abs_energy > 25)
                        {
                            abs_energy -= 5;
                            if (abs_energy < 5)
                                abs_energy = 25;
                        }

                        _energy = (negative ? -abs_energy : abs_energy);

                        RefreshAltPowerForPlayers();
                        events.ScheduleEvent(EVENT_RESET_ENERGY, 2000);
                        break;
                    }
                    case EVENT_NEW_POSITION_TO_WALK:
                    {
                        float x, y, z;
                        float x1, y1, z1;
                        _canChangeDir = true;
                        _orient = Position::NormalizeOrientation(_orient + _nextOrient);
                        me->GetNearPoint(NULL, x, y, z,  -me->GetObjectSize(), 10.0f, _orient);
                        me->GetNearPoint(NULL, x1, y1, z1,  -me->GetObjectSize(), 1.0f, _orient);
                        _nextOrient = 0.0f;
                        if (_energy <= 20 || _energy >= 30)
                        {
                            for (SummonList::iterator i = summons.begin(); i != summons.end(); ++i)
                                if (auto feet = ObjectAccessor::GetCreature(*me, *i))
                                    if (feet->GetEntry() == NPC_LEFT_FOOT || feet->GetEntry() == NPC_RIGHT_FOOT)
                                        feet->RemoveAurasDueToSpell(SPELL_BURNING_FEET);
                            me->RemoveAurasDueToSpell(SPELL_BURNING_FEET);
                        }
                        else
                        {
                            for (SummonList::iterator i = summons.begin(); i != summons.end(); ++i)
                                if (auto feet = ObjectAccessor::GetCreature(*me, *i))
                                    if (feet->GetEntry() == NPC_LEFT_FOOT || feet->GetEntry() == NPC_RIGHT_FOOT)
                                        feet->CastSpell(feet, SPELL_BURNING_FEET, true);
                            DoCast(me, SPELL_BURNING_FEET, true);
                        }

                        if (isInPlatform())
                        {
                            me->DisableSpline();
                            me->GetMotionMaster()->MovePoint(POINT_WALK, x, y, z);
                            events.ScheduleEvent(EVENT_NEW_POSITION_TO_WALK, 1000, EVENT_GROUP_PHASE_ONE);
                        }
                        else
                        {
                            if (!lavaFlow)
                            {
                                Talk(EMOTE_MAGMA);
                                lavaFlow = true;
                            }
                            _canChangeDir = false;
                            events.CancelEvent(EVENT_NEW_POSITION_TO_WALK);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            drinkMagma = true;
                            events.CancelEvent(EVENT_CONCUSSIVE_STOMP);
                            events.ScheduleEvent(EVENT_CONCUSSIVE_STOMP, 15000, EVENT_GROUP_PHASE_ONE);
                            events.ScheduleEvent(EVENT_DRINK_MAGMA, 0, EVENT_GROUP_PHASE_ONE);
                        }
                        break;
                    }
                    case EVENT_CHECK_LAVA_POOL:
                    {
                        if (auto lFoot = GetLeftLeg())
                            if (auto rFoot = GetRightLeg())
                            {
                                me->SetHealth(lFoot->GetHealth() + rFoot->GetHealth());
                                lFoot->SetHealth(me->GetHealth() / 2);
                                rFoot->SetHealth(me->GetHealth() / 2);

                                if (!phase2 && me->HealthBelowPct(26))
                                {
                                    events.ScheduleEvent(EVENT_START_PHASE_2, 500);
                                    me->SetSpeed(MOVE_RUN, 1.0f);
                                    phase2 = true;
                                }
                            }
                            else
                            {
                                if (!phase2)
                                {
                                    phase2 = true;
                                    if (!events.HasEvent(EVENT_START_PHASE_2))
                                        events.ScheduleEvent(EVENT_START_PHASE_2, 500);
                                }
                            }
                        else
                        {
                            if (!phase2)
                            {
                                phase2 = true;
                                if (!events.HasEvent(EVENT_START_PHASE_2))
                                    events.ScheduleEvent(EVENT_START_PHASE_2, 500);
                            }
                        }

                        if (!phase2)
                            events.ScheduleEvent(EVENT_CHECK_LAVA_POOL, 1000, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_CONCUSSIVE_STOMP:
                    {
                        float lock_angle{ me->GetOrientation() };
                        me->GetMotionMaster()->Clear();
                        me->StopMoving();
                        events.CancelEvent(EVENT_NEW_POSITION_TO_WALK);
                        Talk(YELL_STOMP);
                        DoCast(SPELL_CONCUSSIVE_STOMP);
                        me->SetFacingTo(lock_angle);

                        me->AI()->DoAction(ACTION_REMOVE_MOLTEN_STACK);
                        events.ScheduleEvent(EVENT_CONCUSSIVE_STOMP, 30500, EVENT_GROUP_PHASE_ONE);
                        events.ScheduleEvent(EVENT_SUMMON_VOLCANO, 2000, EVENT_GROUP_PHASE_ONE);
                        events.ScheduleEvent(EVENT_NEW_POSITION_TO_WALK, 3000, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_SUMMON_VOLCANO:
                    {
                        Position pos;
                        int32 vulcanos = IsHeroic() ? 3 : 2;
                        for (int32 i = 0; i < vulcanos; ++i)
                        {
                            int count = 0;
                            // weird mecanism ill rewrite this later...
                            bool isInLava;
                            std::list<Creature *> lavaTriggers;
                            me->GetCreatureListWithEntryInGrid(lavaTriggers, NPC_MAGMA_CHECK_TRIGGER, 100.0f);
                            do
                            {
                                isInLava = false;
                                me->GetRandomNearPosition(pos, urand(15,60));
                                if (++count > 100) // should never happen, prevent an infinite loop
                                    break;
                                for (auto itr = lavaTriggers.begin(); itr != lavaTriggers.end(); itr++)
                                    if (Creature *trigger = *itr)
                                        if (pos.GetExactDist(trigger->GetPositionX(), trigger->GetPositionY(), trigger->GetPositionZ()) < MIN_DISTANCE_FROM_LAVA)
                                        {
                                            isInLava = true;
                                            break;
                                        }
                            }
                            while (pos.GetPositionZ() < 100.0f || pos.GetPositionZ() > 101.0f || me->GetDistance(pos) < MIN_DISTANCE_FROM_LAVA || CenterPlatform[0].GetExactDist(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ()) > 50.0f || isInLava);

                            me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), PositionZ, SPELL_SUMMON_VOLCANO, true, 0, 0, me->GetGUID());
                        }
                        break;
                    }
                    case EVENT_SUMMON_FRAGMENT:
                    {
                        Talk(YELL_SUMMON_ADDS);
                        me->AI()->DoAction(ACTION_REMOVE_MOLTEN_STACK);
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            for (uint8 point = 0; point < 5; point++)
                                DoCast(target, SPELL_SUMMON_FRAGMENTS);

                        if (++summonCount >= 2)
                            events.ScheduleEvent(EVENT_SUMMON_SPARK, 22500, EVENT_GROUP_PHASE_ONE);
                        else
                            events.ScheduleEvent(EVENT_SUMMON_FRAGMENT, 22500, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_SUMMON_SPARK:
                    {
                        summonCount = 0;
                        Talk(YELL_SUMMON_ADDS);
                        DoCastRandom(SPELL_SUMMON_SPARK, 0.0f, true);
                        me->AI()->DoAction(ACTION_REMOVE_MOLTEN_STACK);
                        events.ScheduleEvent(EVENT_SUMMON_FRAGMENT, 22500, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_DRINK_MAGMA:
                        DoCast(SPELL_DRINK_MAGMA);
                        events.ScheduleEvent(EVENT_MOLTEN_SPEW, 3000, EVENT_GROUP_PHASE_ONE);
                        break;
                    case EVENT_MOLTEN_SPEW:
                    {
                        me->SetFacingTo(me->GetAngle(_gravityCenter.m_positionX, _gravityCenter.m_positionY));
                        _nextOrient = 0.0f;
                        _energy = 25;
                        _canChangeDir = false;
                        RefreshAltPowerForPlayers();
                        DoCast(SPELL_MOLTEN_SPEW);
                        events.ScheduleEvent(EVENT_MOLTEN_SPEW_END, 4000, EVENT_GROUP_PHASE_ONE);
                        events.ScheduleEvent(EVENT_CHECK_LAVA_POOL, 30000, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_MOLTEN_SPEW_END:
                    {
                        _orient = me->GetAngle(&_gravityCenter);
                        me->GetMotionMaster()->MovePoint(0, _gravityCenter.m_positionX, _gravityCenter.m_positionY, _gravityCenter.m_positionZ);
                        events.ScheduleEvent(EVENT_NEW_POSITION_TO_WALK, 6000, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_SUPERHEATED:
                        Talk(EMOTE_SUPERHEATED);
                        DoCast(SPELL_SUPERHEATED);
                        events.ScheduleEvent(EVENT_SUPERHEATED, 10000);
                        break;
                    case EVENT_START_PHASE_2:
                    {
                        events.CancelEventGroup(EVENT_GROUP_PHASE_ONE);
                        _canChangeDir = false;

                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ERUPTION);
                        me->RemoveAurasDueToSpell(SPELL_OBSIDIAN_ARMOR);
                        me->LowerPlayerDamageReq(me->GetMaxHealth());
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_HIDE_NAMEPLATE | 0x01000000);
                        me->SetDisplayId(38594);
                        me->RemoveAurasDueToSpell(SPELL_BURNING_FEET);
                        summons.DespawnEntry(NPC_LEFT_FOOT);
                        summons.DespawnEntry(NPC_RIGHT_FOOT);
                        summons.DespawnEntry(NPC_FRAGMENT_OF_RHYOLITH);
                        summons.DespawnEntry(NPC_SPARK_OF_RHYOLITH);
                        summons.DespawnEntry(NPC_ARMOR_FRAGMENT_HEROIC);
                        summons.DespawnEntry(NPC_VOLCANO);
                        Talk(EMOTE_PHASE2);
                        DoCast(SPELL_IMMOLATION_AURA);
                        DoCast(SPELL_IMMOLATION);
                        me->AddAura(SPELL_SIT_GROUND, me);

                        if (auto player = me->FindNearestPlayer(500.0f))
                            me->SetLootRecipient(player);

                        if (IsHeroic())
                        {
                            events.ScheduleEvent(EVENT_UNLEASHED_FLAME, 4000);
                            events.ScheduleEvent(EVENT_UNLEASHED_FLAME, 8000);
                            events.ScheduleEvent(EVENT_UNLEASHED_FLAME, 12000);
                        }
                        events.ScheduleEvent(EVENT_CONCUSSIVE_STOMP_PHASE_TWO, 13500);
                        events.ScheduleEvent(EVENT_REMOVE_PHASE_2_INTRO_AURA, 4000);
                        break;
                    }
                    case EVENT_REMOVE_PHASE_2_INTRO_AURA:
                        me->RemoveAurasDueToSpell(SPELL_SIT_GROUND);
                        events.ScheduleEvent(EVENT_START_PHASE_2_COMBAT, 2000);
                        break;
                    case EVENT_START_PHASE_2_COMBAT:
                        me->SetReactState(REACT_AGGRESSIVE);
                        SetCombatMovement(true);
                        me->SetWalk(false);
                        if (me->getVictim())
                            DoStartMovement(me->getVictim());
                        break;
                    case EVENT_UNLEASHED_FLAME:
                        DoCastRandom(SPELL_UNLEASHED_FLAME_SUMMON, 0.0f);
                        events.ScheduleEvent(EVENT_UNLEASHED_FLAME, 20000);
                        break;
                    case EVENT_CONCUSSIVE_STOMP_PHASE_TWO:
                        DoCast(me, SPELL_CONCUSSIVE_STOMP);
                        events.ScheduleEvent(EVENT_CONCUSSIVE_STOMP_PHASE_TWO, 13000);
                        break;
                    case EVENT_ACTIVATE_VOLCANO:
                    {
                        Talk(YELL_ACTIVATING_VOLCANO);
                        DoCast(SPELL_HEATED_VOLCANO);
                        events.ScheduleEvent(EVENT_ACTIVATE_VOLCANO, 25500, EVENT_GROUP_PHASE_ONE);
                        break;
                    }
                    case EVENT_CHECK_LAVA_LINE:
                        bool isCraterInProgress = false;

                        for (uint64 summonGUID : summons)
                        {
                            if (auto crater = ObjectAccessor::GetCreature(*me, summonGUID))
                            {
                                if (crater->GetEntry() == NPC_CRATER && crater->HasAura(SPELL_MAGMA_FLOW_AURA))
                                {
                                    isCraterInProgress = true;
                                    break;
                                }
                            }
                        }

                        if (!isCraterInProgress)
                        {
                            for (uint64 summonGUID : summons)
                            {
                                if (auto crater = ObjectAccessor::GetCreature(*me, summonGUID))
                                {
                                    if (crater->GetEntry() == NPC_CRATER)
                                    {
                                        crater->AI()->DoAction(ACTION_PREPARE_LAVA_LINE);
                                        break;
                                    }
                                }
                            }
                        }

                        events.ScheduleEvent(EVENT_CHECK_LAVA_LINE, 2500);
                        break;
                }

            if (phase2)
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

        Creature* GetLeftLeg()
        {
            if (Vehicle *veh = me->GetVehicleKit())
                if (Unit *passenger = veh->GetPassenger(0))
                    return passenger->ToCreature();
            return NULL;
        }

        Creature* GetRightLeg()
        {
            if (Vehicle *veh = me->GetVehicleKit())
                if (Unit *passenger = veh->GetPassenger(1))
                    return passenger->ToCreature();
            return NULL;
        }

        private:
            Vehicle* vehicle;
            bool lavaFlow, drinkMagma, phase2, _canChangeDir;
            float PositionZ;
            float _nextOrient;
            float _energy;
            std::list<Creature*> volcanos;
            std::list<Position > _listPos;
            Position _gravityCenter;
            float _orient;
            uint8 summonCount;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<boss_lord_rhyolithAI>(creature);
    }
};

class npc_left_foot : public CreatureScript
{
    public:
        npc_left_foot() : CreatureScript("npc_left_foot"){}

    struct npc_left_footAI : public ScriptedAI
    {
        npc_left_footAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            RemoveEncounterFrame();
            powerUpdate = true;
            me->SetAuraStack(SPELL_OBSIDIAN_ARMOR, me, 80);


            /*
            me->ExitVehicle();
            if (auto rhyolith = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                me->EnterVehicle(rhyolith, FOOT_SEAT_LEFT);
            */
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->SetReactState(REACT_PASSIVE);
            if (!me->GetVehicle())
            {
                //TC_LOG_ERROR("sql.sql", "left foot does not have a vehicle, despawning");
                me->DespawnOrUnsummon();
            }
            else
                AddEncounterFrame();
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (auto rhyo = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
            {
                rhyo->AI()->SetData(DATA_TURN_LEFT, damage);
                //rhyo->AI()->DamageTaken(attacker, damage);
            }
        }

    private:
        InstanceScript* instance;
        bool powerUpdate;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_left_footAI>(creature);
    }
};

class npc_right_foot : public CreatureScript
{
    public:
        npc_right_foot() : CreatureScript("npc_right_foot") { }

    struct npc_right_footAI : public ScriptedAI
    {
        npc_right_footAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset() override
        {
            me->SetReactState(REACT_PASSIVE);
            RemoveEncounterFrame();
            me->SetAuraStack(SPELL_OBSIDIAN_ARMOR, me, 80);
            powerUpdate = true;

            /*
            me->ExitVehicle();
            if (auto rhyolith = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                me->EnterVehicle(rhyolith, FOOT_SEAT_LEFT);
            */
        }

        void EnterCombat(Unit* /*who*/) override
        {
            me->SetReactState(REACT_PASSIVE);
            if (!me->GetVehicle())
            {
                //TC_LOG_ERROR("sql.sql", "right foot does not have a vehicle, despawning");
                me->DespawnOrUnsummon();
            }
            else
                AddEncounterFrame();
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (auto rhyo = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
            {
                rhyo->AI()->SetData(DATA_TURN_RIGHT, damage);
                //rhyo->AI()->DamageTaken(attacker, damage);
            }
        }

    private:
        InstanceScript* instance;
        bool powerUpdate;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_right_footAI>(creature);
    }
};

class npc_movement_controller : public CreatureScript
{
    public:
        npc_movement_controller() : CreatureScript("npc_movement_controller") { }

    struct npc_movement_controllerAI : public ScriptedAI
    {
        npc_movement_controllerAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset() override
        {
            _events.Reset();
            bossInCombat = false;
            _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYERS, 1000);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_FOR_PLAYERS:
                    {
                        if (checkWipe())
                            if (auto boss = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                            if (boss->isInCombat())
                            {
                                bossInCombat = false;
                                boss->AI()->EnterEvadeMode();
                                Reset();
                            }

                        /*
                        if (auto player = me->FindNearestPlayer(10.0f))
                        {
                            if (!bossInCombat && me->canSeeOrDetect(player) && player->isAlive())
                            {
                                if (auto boss = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                                {
                                    if (!boss->isInCombat() && boss->isAlive() && !boss->HasUnitState(UNIT_STATE_EVADE))
                                    {
                                        bossInCombat = true;
                                        boss->SetInCombatWithZone();
                                        boss->SetReactState(REACT_PASSIVE);
                                    }
                                }
                            }
                        }
                        */
                    }
                    _events.ScheduleEvent(EVENT_CHECK_FOR_PLAYERS, 1000);
                    break;
                }
            }
        }

        Creature* GetLeftLeg()
        {
            if (Creature *rhyolith = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                if (Vehicle *veh = rhyolith->GetVehicleKit())
                    if (Unit *passenger = veh->GetPassenger(0))
                        return passenger->ToCreature();
            return NULL;
        }

        Creature* GetRightLeg()
        {
            if (Creature *rhyolith = Unit::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                if (Vehicle *veh = rhyolith->GetVehicleKit())
                    if (Unit *passenger = veh->GetPassenger(1))
                        return passenger->ToCreature();
            return NULL;
        }

        private:
            InstanceScript* instance;
            EventMap _events;
            bool bossInCombat;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetFirelandsAI<npc_movement_controllerAI>(creature);
    }
};

class npc_volcano_firelands : public CreatureScript // 52582
{
    public:
        npc_volcano_firelands() : CreatureScript("npc_volcano_firelands") { }

    struct npc_volcano_firelandsAI : public ScriptedAI
    {
        npc_volcano_firelandsAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()), summons(creature) 
        {
            memset(FirePath, 0, sizeof(FirePath));
        }

        void AttackStart(Unit* attacker) {}

        void IsSummonedBy(Unit* summoner) override
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastSpell(me, SPELL_VOLCANO_AURA, true);
            me->CastSpell(me, SPELL_VOLCANO_SMOKE, true);
            isActive = false;
            point = 0;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
            _events.ScheduleEvent(EVENT_CHECK_FEET, 3000);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (spell && spell->Id == SPELL_HEATED_VOLCANO && me->GetEntry() != NPC_CRATER)
            {
                isActive = true;
                me->AddAura(SPELL_VOLCAN_CHECK_ACTIVE, me);
                _events.ScheduleEvent(EVENT_ERRUPTION_START, 3000);
            }
        }

        void JustSummoned(Creature *summon) override
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
                case NPC_PILLAR:
                {
                    summon->SetControlled(true, UNIT_STATE_STUNNED);
                    summon->AddAura(SPELL_LAVA_TUBE, summon);
                    summon->SetDisplayId(23258);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
                    summon->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_DISABLE_TURN);
                    summon->SetCanFly(true);
                    summon->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    summon->AddUnitMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);
                    summon->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 6.0f, me->GetOrientation());
                    break;
                }
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_PREPARE_LAVA_LINE)
            {
                if (auto pillar = me->SummonCreature(NPC_PILLAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 6.0f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 12000))
                    if (auto rhyolith = Creature::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                        rhyolith->AI()->JustSummoned(pillar); // rhyolith despawn the pillar if the raid wipes
                DoCast(me, SPELL_MAGMA_FLOW_AURA, true);
                _events.ScheduleEvent(EVENT_START_LAVA_LINE, 1000);
            }
        }

        float getCurve(float x)
        {
            return sin(x * 0.5f);
        }

        void GenerateFirePath()
        {
            // Clear old values
            for(uint8 line = 0; line <= 5; line++)
                for(uint8 coord = 0; coord <= 1; coord++)
                    for(uint8 point = 0; point < 50; point++)
                        FirePath[line][coord][point] = 0;

            float posX = me->GetPositionX();
            float posY = me->GetPositionY();

            // Select possible lines
            uint8 lines = urand(3,5);

            // Generate route per line
            for (uint8 line = 0; line <= lines; line++)
            {
                // Make default right path
                for (uint8 point = 0; point < 40; point++)
                {
                    switch (lines)
                    {
                        case 3:
                        {
                            switch (line)
                            {
                                case 0:
                                    FirePath[line][0][point] = posX + (1.0f * point);
                                    FirePath[line][1][point] = posY + getCurve(point);
                                    break;
                                case 1:
                                    FirePath[line][0][point] = posX - (1.0f * point);
                                    FirePath[line][1][point] = posY + getCurve(-point);
                                    break;
                                case 2:
                                    FirePath[line][0][point] = posX;
                                    FirePath[line][1][point] = posY + (1.0f * point) + getCurve(point);
                                    break;
                                case 3:
                                    FirePath[line][0][point] = posX;
                                    FirePath[line][1][point] = posY - (1.0f * point) + getCurve(point);
                                    break;
                            }
                            break;
                        }
                        case 4:
                        {
                            switch (line)
                            {
                                case 0:
                                    FirePath[line][0][point] = posX + (0.5f * point);
                                    FirePath[line][1][point] = posY + (1.2f * point) + getCurve(0.5f * point);
                                    break;
                                case 1:
                                    FirePath[line][0][point] = posX - (1.2f * point);
                                    FirePath[line][1][point] = posY + (0.5f * point) + getCurve(-1.2f * point);
                                    break;

                                case 2:
                                    FirePath[line][0][point] = posX - (0.5f * point);
                                    FirePath[line][1][point] = posY - (1.2f * point) + getCurve(- 0.5f * point);
                                    break;

                                case 3:
                                    FirePath[line][0][point] = posX + (1.2f * point);
                                    FirePath[line][1][point] = posY - (0.5f * point) + getCurve(1.2f * point);
                                    break;

                                case 4:
                                    FirePath[line][0][point] = posX + (1.2f * point);
                                    FirePath[line][1][point] = posY + (0.5f * point) + getCurve(1.2f * point);
                                    break;
                            }

                            break;
                        }

                        case 5:
                        {
                            switch (line)
                            {
                                case 0:
                                    FirePath[line][0][point] = posX + (1.0f * point);
                                    FirePath[line][1][point] = posY + (1.0f * point) + getCurve(point);
                                    break;

                                case 1:
                                    FirePath[line][0][point] = posX - (1.0f * point);
                                    FirePath[line][1][point] = posY + (1.0f * point) + getCurve(-point);
                                    break;

                                case 2:
                                    FirePath[line][0][point] = posX - (1.0f * point);
                                    FirePath[line][1][point] = posY + getCurve(-point);
                                    break;

                                case 3:
                                    FirePath[line][0][point] = posX + (1.0f * point);
                                    FirePath[line][1][point] = posY + getCurve(point);
                                    break;

                                case 4:
                                    FirePath[line][0][point] = posX + (1.0f * point);
                                    FirePath[line][1][point] = posY - (1.0f * point) + getCurve(point);
                                    break;

                                case 5:
                                    FirePath[line][0][point] = posX - (1.0f * point);
                                    FirePath[line][1][point] = posY - (1.0f * point) + getCurve(-point);
                                    break;
                            }
                            break;
                        }
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_FEET:
                    {
                        bool found{ false };
                        if (auto c = me->FindNearestCreature(BOSS_RHYOLITH, 20.f))
                        {

                            float angle1        { fabs(c->GetRelativeAngleOffset(me))                   };   //Degrees between rhyo's orientation and me.
                            float perfect_angle { M_PI / 2.0f                                           };
                            float difference    { fabs(angle1 - perfect_angle)                          };
                            float scale         { ((perfect_angle - difference) / perfect_angle * 2.f)  };
                            float bonus         { scale * VOLCANO_CRUSH_DISTANCE                        };

                            if (me->GetExactDist2d(c) <= VOLCANO_CRUSH_DISTANCE + bonus)
                                if (me->GetEntry() != NPC_CRATER)
                                {
                                    if (isActive)
                                    {
                                        c->AI()->DoAction(ACTION_REDUCE_OBSIDIAN_ARMOR);
                                        if (IsHeroic())
                                        {
                                            uint8 summonCount = Is25ManRaid() ? 10 : 5;
                                            for (uint8 i = 0; i < summonCount; i++)
                                                c->CastSpell(c, SPELL_SUMMON_ARMOR_FRAGMENT, true);
                                        }
                                    }
                                    else
                                        c->AI()->DoAction(ACTION_CAST_MOLTEN_ARMOR);
                                    _events.CancelEvent(EVENT_ERRUPTION_START);
                                    me->RemoveAurasDueToSpell(SPELL_ERUPTION_PERIODIC);
                                    me->UpdateEntry(NPC_CRATER);
                                    me->CastWithDelay(100, me, SPELL_EXPLOSION, true);
                                    GenerateFirePath();
                                }

                        }
                        
                        _events.ScheduleEvent(EVENT_CHECK_FEET, 500);
                        break;
                    }
                    case EVENT_START_LAVA_LINE:
                    {
                        for (uint8 line = 0; line <= 5; line++)
                        {
                            // Check if this line is used
                            if (FirePath[line][0][0] == 0.0f && FirePath[line][1][0] == 0.0f)
                                break;

                            if (auto targetTrigger = me->SummonCreature(NPC_DESTINATION_STALKER, FirePath[line][0][point], FirePath[line][1][point], 100.50f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10000))
                                me->CastSpell(targetTrigger, SPELL_MAGMA_FLOW, true);
                        }
                        point++;
                        if (point >= 40)
                            _events.ScheduleEvent(EVENT_LAVA_LINE_EXPLOSION, 100);
                        else
                            _events.ScheduleEvent(EVENT_START_LAVA_LINE, 200);
                        break;
                    }
                    case EVENT_LAVA_LINE_EXPLOSION:
                    {
                        for (uint64 summonGUID : summons)
                            if (auto stalker = Creature::GetCreature(*me, summonGUID))
                                if (stalker->GetEntry() == NPC_DESTINATION_STALKER)
                                    me->CastSpell(stalker, SPELL_MAGMA_FLOW_DAMAGE, true);
                        me->DespawnOrUnsummon(2000);
                        summons.DespawnAll(2000);
                        break;
                    }
                    case EVENT_ERRUPTION_START:
                    {
                        me->RemoveAurasDueToSpell(SPELL_VOLCANO_SMOKE);
                        DoCast(me, SPELL_ERUPTION_PERIODIC, true);
                        break;
                    }
                }
            }
        }
        private:
            InstanceScript* instance;
            EventMap _events;
            SummonList summons;
            uint8 point;
            bool isActive;
            float FirePath[6][2][50];
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_volcano_firelandsAI>(creature);
    }
};

class npc_spark_of_rhyolith : public CreatureScript
{
    public:
        npc_spark_of_rhyolith() : CreatureScript("npc_spark_of_rhyolith"){}

    struct npc_spark_of_rhyolithAI : public ScriptedAI
    {
        npc_spark_of_rhyolithAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void Reset() override
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
        }

        void IsSummonedBy(Unit* /*summoner*/) override
        {
            DoZoneInCombat();
            me->CastSpell(me, SPELL_IMMOLATION_SPARK, true);
        }

        void UpdateAI(const uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_INFERNAL_RAGE:
                        DoCast(SPELL_INFERNAL_RAGE);
                        _events.ScheduleEvent(EVENT_INFERNAL_RAGE, 5000);
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

        private:
            InstanceScript* instance;
            EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_spark_of_rhyolithAI>(creature);
    }
};

class npc_liquid_obsidian : public CreatureScript
{
public:
    npc_liquid_obsidian() : CreatureScript("npc_liquid_obsidian"){}

    struct npc_liquid_obsidianAI : public ScriptedAI
    {
        npc_liquid_obsidianAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

        void IsSummonedBy(Unit* summoner) override
        {
            me->SetReactState(REACT_PASSIVE);
            events.ScheduleEvent(EVENT_CHECK_RHYO_DISTANCE, 500);
            me->GetMotionMaster()->MoveFollow(summoner, 0.0f, 0.0f);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_RHYO_DISTANCE:
                        if (auto rhyolith = Creature::GetCreature(*me, instance->GetData64(DATA_LORD_RHYOLITH)))
                        {
                            if (!me->HasUnitState(UNIT_STATE_FOLLOW))
                                me->GetMotionMaster()->MoveFollow(rhyolith, 0.0f, 0.0f);

                            if (me->GetExactDist2d(rhyolith) <= 15.0f)
                                DoCast(me, SPELL_FUSE);
                            else
                                events.ScheduleEvent(EVENT_CHECK_RHYO_DISTANCE, 500);
                        }
                        else
                            me->DespawnOrUnsummon();
                        break;
                }
            }
        }
    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_liquid_obsidianAI>(creature);
    }
};

class spell_fragment_of_rhyolith_fusion : public SpellScriptLoader
{
public:
    spell_fragment_of_rhyolith_fusion() : SpellScriptLoader("spell_fragment_of_rhyolith_fusion") { }

    class spell_fragment_of_rhyolith_fusion_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fragment_of_rhyolith_fusion_SpellScript);

        void DamageAmount(SpellEffIndex /*effIndex*/)
        {
            float damage = 0;
            if (Unit *caster = GetCaster())
                damage = CalculatePct(caster->GetHealth(), GetEffectValue());

            SetHitDamage(damage);
        }

        void Suicide(SpellEffIndex /*effIndex*/)
        {
            if (Unit *caster = GetCaster())
                caster->CastSpell(caster, 7, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_fragment_of_rhyolith_fusion_SpellScript::DamageAmount, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnEffectHitTarget += SpellEffectFn(spell_fragment_of_rhyolith_fusion_SpellScript::Suicide, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_fragment_of_rhyolith_fusion_SpellScript();
    }
};

// 98646
class spell_fragment_of_rhyolith_periodic : public SpellScriptLoader
{
public:
    spell_fragment_of_rhyolith_periodic() : SpellScriptLoader("spell_fragment_of_rhyolith_periodic") { }

    class spell_fragment_of_rhyolith_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_fragment_of_rhyolith_periodic_AuraScript);

        void onPeriodicTick(AuraEffect const* aurEff)
        {
            PreventDefaultAction();
            if (Unit *caster = GetCaster())
                if (Creature *fargment = caster->ToCreature())
                    fargment->AI()->DoCastRandom(98649, 0.0f, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_fragment_of_rhyolith_periodic_AuraScript::onPeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_fragment_of_rhyolith_periodic_AuraScript();
    }
};

class spell_heated_volcano : public SpellScriptLoader
{
public:
    spell_heated_volcano() : SpellScriptLoader("spell_heated_volcano") { }

    class spell_heated_volcano_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_heated_volcano_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* obj)
            {
                Unit* target = obj->ToUnit();
                return !target
                    || target->HasAura(SPELL_ERUPTION_PERIODIC)
                    || !target->IsVisible();
            });

            if (targets.empty())
                return;

            WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
            Unit* caster = GetCaster();

            targets.remove_if([caster](WorldObject* obj)
            {
                return !caster->isInFront(obj);
            });

            if (!targets.empty())
                target = Trinity::Containers::SelectRandomContainerElement(targets);

            targets.clear();
            targets.push_back(target);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_heated_volcano_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_heated_volcano_SpellScript();
    }
};

class spell_fuse : public SpellScriptLoader
{
public:
    spell_fuse() : SpellScriptLoader("spell_fuse") { }

    class spell_fuse_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fuse_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            if (auto target = GetHitUnit())
                target->CastSpell(target, GetEffectValue(), true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_fuse_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            OnEffectHitTarget += SpellEffectFn(spell_fuse_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_fuse_SpellScript();
    }
};

class spell_fl_lava_strike : public SpellScriptLoader
{
public:
    spell_fl_lava_strike() : SpellScriptLoader("spell_fl_lava_strike") { }

    class spell_fl_lava_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_fl_lava_strike_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* target)
            {
                return target->GetTypeId() != TYPEID_PLAYER;
            });

            if (!targets.empty())
                Trinity::Containers::RandomResizeList(targets, uint32(GetCaster()->GetMap()->Is25ManRaid() ? 6 : 3));
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            GetCaster()->CastSpell(GetHitUnit(), GetSpellInfo()->Effects[effIndex].BasePoints, true);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fl_lava_strike_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_fl_lava_strike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_fl_lava_strike_SpellScript();
    }
};

// trash
class npc_lava_spawn : public CreatureScript
{
public:
    npc_lava_spawn() : CreatureScript("npc_lava_spawn") { }

    struct npc_lava_spawnAI : public ScriptedAI
    {
        npc_lava_spawnAI(Creature *c) : ScriptedAI(c) { castTimer = 3000; }

        void JustDied(Unit* /*killer*/) override
        {
            if (!me->HasAuraWithMechanic(MECHANIC_STUN))
            {
                // Split
                DoCast(101049);
                DoCast(101050);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (castTimer <= diff)
            {
                DoCast(99576); // Fireball
                castTimer = 3000;
            }
            else
                castTimer -= diff;
        }
    private:
        uint32 castTimer;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_lava_spawnAI(creature);
    }
};
/*Kar the everburning and friends*/
class Firestorm_TargetCheck
{
public:
    explicit Firestorm_TargetCheck(Creature* pile) : _eggPile(pile) { }

    bool operator()(Unit* object) const
    {
        if (object->GetEntry() != NPC_FIRESTORM_TARGET_TRIGGER)
            return false;

        if (_eggPile->GetDistance2d(object) > 12.0f)
            return false;
        return true;
    }

private:
    Creature* _eggPile;
};

class npc_kar_the_everburning : public CreatureScript
{
public:
    npc_kar_the_everburning() : CreatureScript("npc_kar_the_everburning") {}

    struct npc_kar_the_everburningAI : public ScriptedAI
    {
        npc_kar_the_everburningAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset() override
        {
        }
        void InitializeAI() override
        {
            retreatFinished = false;
            me->SetReactState(REACT_PASSIVE);
            hasRetreated = false;
            events.ScheduleEvent(EVENT_SUMMON_LAVA_SPAWNS, 15000);
            if (me->isDead())
            {
                retreatFinished = true;
                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, NPC_FIRESTORM_CASTER_TRIGGER, 2000.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_UNBOUND_PYRELORD, 500.0f);
                for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                    (*itr)->AI()->DoAction(ACTION_KAR_ALREADY_DEAD);
            }
        }
        void IsSummonedBy(Unit* /*summoner*/) override
        {
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8 && id == 0)
            {
                retreatFinished = true;
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
                DoCast(me->getVictim(), SPELL_SUMMON_FIRE_ELEM);
                events.ScheduleEvent(EVENT_CAST_SOULBURN, urand(6000, 12000));
            }
        }
        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (!hasRetreated && me->HealthBelowPctDamaged(95, damage))
            {
                std::list<Creature*> units;
                DoAction(ACTION_KAR_RETREAT);
                GetCreatureListWithEntryInGrid(units, me, NPC_FIRESTORM_CASTER_TRIGGER, 2000.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_UNBOUND_PYRELORD, 500.0f);
                for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                    (*itr)->AI()->DoAction(ACTION_KAR_RETREAT);
            }
        }
        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case ACTION_KAR_RETREAT:
                hasRetreated = true;
                sCreatureTextMgr->SendChat(me, 0, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_MAP);
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(0, retreatPosition.GetPositionX(), retreatPosition.GetPositionY(), retreatPosition.GetPositionZ());
                break;
            default:
                break;
            }
        }

        void JustSummoned(Creature* summon) override
        {
            summon->setActive(true);
            switch (summon->GetEntry())
            {
            case NPC_UNSTABLE_PYRELORD:
                summon->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                summon->GetMotionMaster()->MovePath(PATH_UNSTABLE_PYRELORD, true);
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {

                case EVENT_SUMMON_LAVA_SPAWNS:
                    if (!hasRetreated)
                    {                       
                            DoCast(me, SPELL_SUMMON_LAVA_SPAWNS);
                            events.ScheduleEvent(EVENT_SUMMON_LAVA_SPAWNS, 15000);
                    }
                    break;
                case EVENT_CAST_SOULBURN:
                    DoCast(me->getVictim(), SPELL_SOUL_BURN);
                    events.ScheduleEvent(EVENT_CAST_SOULBURN, urand(6000, 12000));
                    break;
                default:
                    break;
                }
            }
            if (hasRetreated)
                if (me->HasReactState(REACT_AGGRESSIVE))
                {
                    DoMeleeAttackIfReady();
                }
        }

    private:
        bool retreatFinished;
        bool hasRetreated;
        InstanceScript* instance;
        EventMap events;
        Position retreatPosition = { -327.758087f, -414.976227f, 103.278252f, 6.215208f };
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_kar_the_everburningAI>(creature);
    }
};

class npc_firestorm : public CreatureScript
{
public:
    npc_firestorm() : CreatureScript("npc_firestorm") {}

    struct npc_firestormAI : public ScriptedAI
    {
        npc_firestormAI(Creature* creature) : ScriptedAI(creature)
        {
            Firestorm_TargetCheck check(me);
            Trinity::CreatureListSearcher<Firestorm_TargetCheck> searcher(me, firestorm_triggers, check);
            me->VisitNearbyGridObject(15.0f, searcher);
        }

        void InitializeAI() override
        {
            events.ScheduleEvent(EVENT_CAST_FIRESTORM, urand(5000, 10000));
            me->SetReactState(REACT_PASSIVE);
            hasRetreated = false;
        }
        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
                case ACTION_KAR_ALREADY_DEAD:
                case ACTION_KAR_RETREAT:
                    events.CancelEventGroup(EVENT_CAST_FIRESTORM);
                    hasRetreated = true;
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CAST_FIRESTORM:
                    if (!hasRetreated)
                    {
                        if (!firestorm_triggers.empty())
                        {
                            Creature* firestorm_target = Trinity::Containers::SelectRandomContainerElement(firestorm_triggers);
                            if (auto karr_retreated = me->FindNearestCreature(NPC_KAR_EVERBURNING, 1000.0f, true))
                                if (karr_retreated->isAlive())
                                    DoCast(firestorm_target, SPELL_FIRESTORM);
                        }
                        events.ScheduleEvent(EVENT_CAST_FIRESTORM, urand(3000, 7000));
                    }                
                    break;
                }
            }            
        }

    private:
        bool hasRetreated;
        EventMap events;
        std::list<Creature*> firestorm_triggers;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_firestormAI>(creature);
    }
};

class npc_kar_spawn_trigger : public CreatureScript
{
public:
    npc_kar_spawn_trigger() : CreatureScript("npc_kar_spawn_trigger") {}

    struct npc_kar_spawn_triggerAI : public ScriptedAI
    {
        npc_kar_spawn_triggerAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI() override
        {
            events.ScheduleEvent(EVENT_SUMMON_LAVA_SPAWNS, 8000);
        }

        void Reset() override
        {
        }

        void IsSummonedBy(Unit* /*summoner*/) override
        {
        }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_kar_spawn_triggerAI>(creature);
    }
};

class npc_unstable_pyrelord : public CreatureScript
{
public:
    npc_unstable_pyrelord() : CreatureScript("npc_unstable_pyrelord") {}

    struct npc_unstable_pyrelordAI : public ScriptedAI
    {
        npc_unstable_pyrelordAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI() override
        {
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void Reset() override
        {
        }
        void EnterCombat(Unit* who) override
        {
            me->GetMotionMaster()->Clear();
            if (isLeader)
            {
                /*
                if (follower1->isAlive())
                if (!follower1->isInCombat())
                {
                    follower1->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    follower1->AI()->AttackStart(who);
                    follower1->SetInCombatWithZone();
                }

                if (follower2->isAlive())
                if (!follower2->isInCombat())
                {
                    follower2->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    follower2->AI()->AttackStart(who);
                    follower2->SetInCombatWithZone();
                }

                if (follower3->isAlive())
                if (!follower3->isInCombat())
                {
                    follower3->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    follower3->AI()->AttackStart(who);
                    follower3->SetInCombatWithZone();
                }
                */
            }
            else
            {
                if (originalSource)
                    if (!originalSource->isInCombat())
                    {
                        originalSource->ToCreature()->ClearUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                        originalSource->ToCreature()->SetInCombatWithZone();
                        originalSource->ToCreature()->AI()->EnterCombat(who);
                        originalSource->ToCreature()->AI()->AttackStart(who);
                    }
            }
        }
        void JustDied(Unit* /*killer*/) override
        {
            if (isLeader)
            {
                // me->DespawnOrUnsummon();
            }
        }
        void IsSummonedBy(Unit* summoner) override
        {
            originalSource = summoner;
            switch (summoner->GetEntry())
            {
            case NPC_KAR_EVERBURNING:
                isLeader = true;
                if (TempSummon* follow1 = me->SummonCreature(NPC_UNSTABLE_PYRELORD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                    if (TempSummon* follow2 = me->SummonCreature(NPC_UNSTABLE_PYRELORD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                        if (TempSummon* follow3 = me->SummonCreature(NPC_UNSTABLE_PYRELORD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            follower1 = follow1;
                            follower2 = follow2;
                            follower3 = follow3;

                            follow1->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                            follow2->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                            follow3->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

                            follow1->GetMotionMaster()->MoveFollow(me, 2.0f, M_PI * 1.5f);
                            follow2->GetMotionMaster()->MoveFollow(me, 5.0f, M_PI * 0.5f);
                            follow3->GetMotionMaster()->MoveFollow(me, 8.0f, M_PI * 1.5f);
                        }
                break;
            case NPC_UNSTABLE_PYRELORD:
                break;
            default:
                break;
            }
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            //TC_LOG_ERROR("misc", "Pyrelord movenet informed of type %u and id %u of %u", type, id, PYRELORD_BOTTOM_OF_HILL);
            if (type == MOVEMENT_INFORM_WAYPOINT_REACHED)
            {
                switch (id)
                {
                case PYRELORD_BOTTOM_OF_HILL://DB waypoint ID minus one, bottom of hill
                    if (isLeader)
                    {
                        if (follower1)
                            follower1->DespawnOrUnsummon();
                        if (follower2)
                            follower2->DespawnOrUnsummon();
                        if (follower3)
                            follower3->DespawnOrUnsummon();
                        me->DespawnOrUnsummon();
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool isLeader = false;
        TempSummon* follower1;
        TempSummon* follower2;
        TempSummon* follower3;
        Unit* originalSource;
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_unstable_pyrelordAI>(creature);
    }
};

class npc_unbound_pyrelord : public CreatureScript
{
public:
    npc_unbound_pyrelord() : CreatureScript("npc_unbound_pyrelord") {}

    struct npc_unbound_pyrelordAI : public ScriptedAI
    {
        npc_unbound_pyrelordAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI() override
        {
        }

        void Reset() override
        {
        }
        void EnterCombat(Unit* who) override
        {

        }
        void JustDied(Unit* /*killer*/) override
        {

        }
        void IsSummonedBy(Unit* summoner) override
        {

        }
        void MovementInform(uint32 type, uint32 id) override
        {

        }
        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case ACTION_KAR_RETREAT:
                if (auto karr_retreated = me->FindNearestCreature(NPC_KAR_EVERBURNING, 200.0f, true))
                {
                    me->GetMotionMaster()->MoveFollow(karr_retreated, 5.0f, M_PI * 1.5f);
                    me->SetInCombatWithZone();
                }
                break;
            default:
                break;
            }
        }
    private:

    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetFirelandsAI<npc_unbound_pyrelordAI>(creature);
    }
};

void AddSC_boss_lord_rhyolith()
{
    new boss_lord_rhyolith();
    new npc_left_foot();
    new npc_right_foot();
    new npc_movement_controller();
    new npc_volcano_firelands();
    new npc_spark_of_rhyolith();
    new npc_liquid_obsidian();
    new spell_fragment_of_rhyolith_fusion();
    new spell_fragment_of_rhyolith_periodic();
    new spell_heated_volcano();
    new spell_fuse();
    new spell_fl_lava_strike();
    new npc_lava_spawn();
    new npc_kar_the_everburning();
    new npc_firestorm();
    new npc_kar_spawn_trigger();
    new npc_unstable_pyrelord();
    new npc_unbound_pyrelord();
}
