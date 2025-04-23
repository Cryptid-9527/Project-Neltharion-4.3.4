
#include "AccountMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "throne_of_the_four_winds.h"

enum Spells
{
    SPELL_TOO_CLOSE_KNOCK_BACK_EFFECT = 90663,
    SPELL_FIXATE_ON_HIT_PASSIVE = 92039,

    // phase 1
    SPELL_SUMMON_ICE_MOBILE_SPAWNER         = 88239,//spawn periodic
    SPELL_SUMMON_ICE_PATCH       = 87055,//spawn original one
    SPELL_LIGHTNING_STRIKE         = 91327,
    SPELL_LIGHTNING_STRIKE_TRIGGER = 91326,
    SPELL_ELECTROCUTE              = 88427,
    SPELL_SUMMON_SQUALL_LINE_SE    = 91104,
    SPELL_SUMMON_SQUALL_LINE_SW    = 88781,
    SPELL_SQUALL_LINE              = 91129,
    SPELL_SQUALL_LINE_2            = 91110,
    SPELL_SQUALL_PERIODIC          = 87652,
    SPELL_WIND_BURST               = 87770,

    SPELL_SUMMON_CHEST             = 95386,

    // phase 2
    SPELL_ACID_RAIN                = 88290,
    SPELL_ACID_RAIN_2              = 91216,
    SPELL_SUMMON_STORMLING         = 88272,
    SPELL_STATIC_SHOCK             = 87873,

    // phase 3
    SPELL_BIG_RAIN                 = 82651,
    SPELL_RELENTLESS_STORM_INITIAL_VEHICLE_RIDE_TRIGGER = 89528,
    SPELL_RELENTLESS_STORM_INITIAL_VEHICLE_RIDE = 89527,
    SPELL_RELENTLESS_STORM_CHANNEL = 88875,
    SPELL_EYE_OF_THE_STORM         = 89107,
    SPELL_WIND_BURST_2             = 89638,
    SPELL_LIGHTNING                = 89644,
    SPELL_LIGHTNING_ROD            = 89690,
    SPELL_LIGHTNING_2              = 89641,
    SPELL_LIGHTNING_CLOUD_SUMMON   = 89565,
    SPELL_LIGHTNING_CLOUD_PERIODIC = 89639,
    SPELL_LIGHTING_CLOUD_DUMMY     = 89628,

    // ice storm SUMMONER
    SPELL_ICE_STORM_SUMMON_OTHER_ICE_STORM_PERIODIC       = 87053,
    SPELL_ICE_STORM                = 87408,
    SPELL_ICE_STORM_MISSLE         = 87103,
    SPELL_ICE_STORM_2              = 87406,
    // ice storm
    SPELL_ICESTORM_PERSISTENT_AREA_AURA = 87472,
    SPELL_ICESTORM_STATIONARY_REPEAT_SPELL        = 87469,

    // squall line
    SPELL_SQUALL_TRIGGERED      = 88779,
    SPELL_SQUALL_LINE_BOARDED      = 87856,
    SPELL_SQUALL_LINE_AURA         = 87621,

    // stormling
    SPELL_STORMLING                = 87913,
    SPELL_STORMLING_SUMMON         = 87914,
    // stormlings summon o0?
    SPELL_STORMLING_PERIODIC       = 87906,
    SPELL_STORMLING_PERIODIC_2     = 87905,
    SPELL_FEEDBACK                 = 87904,
    // lightning cloud
    SPELL_LIGHTNING_CLOUDS_SUMMON  = 89577,
    SPELL_LIGHTNING_CLOUDS         = 89575,
    // lightning cloud's summon
    SPELL_LIGHTNING_CLOUDS_PERIODIC = 89564,
    SPELL_LIGHTNING_CLOUDS_DUMMY    = 89569,
    SPELL_LIGHTING_CLOUD_DAMAGE     = 89587,
    SPELL_LIGHTING_STRIKE_DUMMY     = 88230,
    SPELL_LIGHTING_STRIKE_CONE_DAMAGE = 88214,
    SPELL_LIGHTING_STRIKE_PERIODIC_PULSE         = 93247,
    SPELL_LIGHTING_STRIKE_SC_EFF    = 88238,
    SPELL_LIGHTING_STRIKE_PLAYER    = 95764,
    // npc fall catcher
    SPELL_FALL_CATCHER              = 85282,
    SPELL_EJECT_PASSENGER           = 68576,
    SPELL_CATCH_FALL                = 85274,
    SPELL_CATCH_FALL_TRIGGER_1      = 85275,
    SPELL_CATCH_FALL_TRIGGER_2      = 85269,
    SPELL_CATCHER_SUMMON            = 85268,

    SPELL_PLAYERS_CAN_FLIGHT        = 82724,
};

enum Events
{
    EVENT_ICE_STORM = 1,
    EVENT_ELECTROCUTE,
    EVENT_LIGHTING_STRIKE,
    EVENT_SQUALL_LINE_W,
    EVENT_SQUALL_LINE_E,
    EVENT_WIND_BURST,

    EVENT_ACID_RAIN,
    EVENT_SUMMON_STORMLING,
    EVENT_STATIC_SHOCK,

    EVENT_REMOVE_ACID_RAID,
    EVENT_WIND_BURST_P3,
    EVENT_LIGHTING_CLOUD,
    EVENT_LIGHTING_CLOUD_SUMMON,
    EVENT_LIGHTING_ROD,
    EVENT_LIGHTING,

    EVENT_DESTROY_PLATFORM,
};

enum Npcs
{
    NPC_ICESTORM_MOBILE = 46734,
    NPC_ICE_PATCH_STATIONARY = 46973,
    NPC_ICE_PATCH_STATIONARY_STARTER = 46766,
    NPC_INFINITE_AOE_TRIGGER = 36171,
    NPC_LIGHTING_STRIKE_TRIGGER = 48977,
    NPC_SQUALL_LINE_VEHICLE_W = 47034,
    NPC_SQUALL_LINE_W = 48854,
    NPC_SQUALL_LINE_VEHICLE_E = 48852,
    NPC_SQUALL_LINE_E = 48855,
    NPC_FALL_CATCHER = 45710,
    NPC_WORLD_TRIGGER = 21252,
    NPC_STORMLING_PRE_EFFECT = 47177,
    NPC_STORMLING = 47175,
    NPC_RELENTLESS_STORM_INITIAL_VEHICLE = 47806,
    NPC_LIGHTING_CLOUD = 48190,
    NPC_LIGHTING_CLOUD_EXTRA_VISUAL = 48196,
};

enum alak_phases
{
    PHASE_INTRO,
    PHASE_1,
    PHASE_2,
    PHASE_3,
    PHASE_OUTRO,
};

enum actions
{
    ACTION_9_SEC_SHOCK = 2,
    ACTION_STOP_SHOCKING = 3,
};

/*
class SquallBoardEvent : public BasicEvent
{
public:
    SquallBoardEvent(Unit *trash) : _trash(trash)
    {
    }


    bool Execute(uint64 execTime, uint32 diff)
    {
    std::list<Player*> targets = _trash->GetPlayersInRange(1, true);
    for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); itr++)
        if (auto player = *itr)
            if (!player->HasAura(SPELL_SQUALL_LINE_BOARDED) && !player->HasAura(95751) && !player->GetVehicle() && !player->HasAura(85269))
                player->CastSpell(_trash, SPELL_SQUALL_LINE_BOARDED, true);
    if (!_trash->HasAura(SPELL_SQUALL_PERIODIC))
        _trash->AddAura(SPELL_SQUALL_PERIODIC, _trash);
    _trash->m_Events.AddEvent(this, execTime + 500);
    return false;
    }

private:
    Unit* _trash;
};
*/
/*
class IntroP3VisualEvent : public BasicEvent
{
public:
    IntroP3VisualEvent(Unit *trash, uint64 playerGUID) : _trash(trash), _playerGUID(playerGUID)
    {
    }

    bool Execute(uint64 execTime, uint32 diff)
    {
        if (_trash)
        {
            if (auto player = ObjectAccessor::GetUnit(*_trash, _playerGUID))
                player->UpdatePosition(player->GetPositionX(), player->GetPositionY(), _trash->GetPositionZ(), true);
            _trash->SetFacingTo(_trash->GetOrientation() + 0.01f);
            _trash->m_Events.AddEvent(this, execTime + 100);
        }
        return false;
    }
private:
    Unit *_trash;
    uint64 _playerGUID;
};

class EndIntroP3VisualEvent : public BasicEvent
{
public:
    EndIntroP3VisualEvent(Unit *trash, uint64 player) : _trash(trash), _playerGUID(player)
    {
    }

    bool Execute(uint64 execTime, uint32 diff)
    {
        if (!_trash)
            return true;
        if (Unit *_player = Unit::GetUnit(*_trash, _playerGUID))
            if (_player->GetTypeId() == TYPEID_PLAYER)
            {
                _player->ExitVehicle();
                _player->RemoveAurasDueToSpell(SPELL_RELENTLESS_STORM_INITIAL_VEHICLE_RIDE);
                _player->UpdatePosition(_player->GetPositionX(), _player->GetPositionY(), 341.0f, true);
                _player->ToPlayer()->SetViewpoint(_trash, false);
                _trash->ToCreature()->DespawnOrUnsummon(1000);
                _player->CastSpell(_player, SPELL_BIG_RAIN, true);
            }
        return true;
    }
private:
    Unit *_trash;
    uint64 _playerGUID;
};

*/

class boss_alakir : public CreatureScript
{
public:
    boss_alakir() : CreatureScript("boss_alakir") {}

    CreatureAI * GetAI(Creature * creature) const
    {
        return new boss_alakirAI(creature);
    }

    struct boss_alakirAI : public BossAI
    {
        boss_alakirAI(Creature * creature) : BossAI(creature, DATA_ALAKIR)
        {
        }

        void Reset()
        {
            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            me->SetReactState(REACT_PASSIVE);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYERS_CAN_FLIGHT);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BIG_RAIN);
            instance->DoRemoveAurasDueToSpellOnPlayers(95751);
            instance->DoRemoveAurasDueToSpellOnPlayers(85269);
            if (GameObject *go = instance->instance->GetGameObject(instance->GetData64(GOB_CENTER_PLATFORM)))
                go->SetDestructibleState(GO_DESTRUCTIBLE_INTACT);
            _phase = PHASE_INTRO;
            if (instance->GetBossState(DATA_CONCLAVE_OF_WIND_EVENT) != DONE && instance->GetBossState(DATA_CONCLAVE_OF_WIND_EVENT) != DONE_HM)
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            HideTrigger(true);
            instance->DisableFallDamage(false);
            _Reset();

            me->DespawnCreaturesInArea(47034);
            me->DespawnCreaturesInArea(48852);
            me->DespawnCreaturesInArea(48854);
            me->DespawnCreaturesInArea(48855);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_DISABLE_SLIPSTREAM_TRIGGER:
                    HideTrigger(true);
                    break;
                case ACTION_9_SEC_SHOCK:
                {

                    //TC_LOG_ERROR("sql.sql", "alakir AI recognizes ACTION_9_SEC_SHOCK");
                    float limits{ 2.f * (M_PI / 9.f) };//40 degrees
                    float start_angle{ shocking_angle };
                    float left_bound{ start_angle - limits };
                    float right_bound{ start_angle + limits };


                    auto list_of_players = me->GetPlayersInRange(140.f, true);
                    if (list_of_players.size())
                        for (auto current_target_within_list : list_of_players)
                        {
                            float compare_angle{ me->GetRelativeAngleOffset(current_target_within_list) };
                            if (compare_angle < left_bound || compare_angle > right_bound)
                            {
                                //TC_LOG_ERROR("sql.sql", "shocking player %u with damage", current_target_within_list->GetGUID());
                                DoCast(current_target_within_list, SPELL_LIGHTING_STRIKE_PLAYER, true);
                            }
                        }

                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_LIGHTING_STRIKE_TRIGGER, 100.0f);

                    if (list_of_npcs.size())
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto t = (*itr))
                            {
                                float compare_angle{ me->GetRelativeAngleOffset(t) };

                                if (compare_angle < left_bound || compare_angle > right_bound)
                                    list_of_npcs.erase(itr);
                            }

                    if (list_of_npcs.size())
                        for (auto c : list_of_npcs)
                        {
                            //TC_LOG_ERROR("sql.sql", "creature %u should be shocking me.", c->GetGUID());
                            c->CastSpell(me, SPELL_LIGHTING_STRIKE_DUMMY, true);
                        }

                    break;
                }
                default:
                    break;
            }
        }

        void HideTrigger(bool hide)
        {

            if (hide)
                summons.DespawnEntry(NPC_LIGHTING_STRIKE_TRIGGER);
            else
            {
                me->SummonCreature(NPC_LIGHTING_STRIKE_TRIGGER, *me);
                int count{ 90 };
                for (int i = 0; i < count; i++)
                {
                    Position pos(*me);
                    float angle{ (M_PI * 2 / float(count)) * float(i) };
                    me->MovePosition(pos, 90.f, angle, true);
                    pos.m_positionZ += 2.f;

                    me->MovePosition(pos, 5.f, angle, false);
                    if (auto c = me->SummonCreature(NPC_LIGHTING_STRIKE_TRIGGER, pos))
                    {
                        c->SetVisible(true);
                        c->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
                        c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
            }
        }

        void JustSummoned(Creature * summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_ICE_PATCH_STATIONARY://STATIONARY
                    /*
                    summon->CastWithDelay(2000, summon, SPELL_ICE_STORM, true);
                    summon->CastWithDelay(2000, summon, SPELL_ICE_STORM_MISSLE, true);
                    summon->CastWithDelay(2000, summon, SPELL_ICE_STORM_2, true);
                    */
                    summon->AddAura(SPELL_ICESTORM_STATIONARY_REPEAT_SPELL, summon);
                    summon->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    summon->DespawnOrUnsummon(21000);
                    summon->CastSpell(summon, SPELL_ICESTORM_PERSISTENT_AREA_AURA, true);//ice storm visual
                    break;
                case NPC_ICESTORM_MOBILE://SUMMONED
                {
                    summon->AddAura(SPELL_ICE_STORM_SUMMON_OTHER_ICE_STORM_PERIODIC, summon);
                    summon->DespawnOrUnsummon(21000);
                    break;
                }
                case NPC_SQUALL_LINE_VEHICLE_W:
                {
                    summon->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    summon->DespawnOrUnsummon(50000);
                    //summon->NearTeleportTo({ -81.178429f, 847.460754f, 191.017654f, 4.045910f });
                    break;
                }
                case NPC_SQUALL_LINE_VEHICLE_E:
                {
                    summon->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    //summon->NearTeleportTo({ -67.139030f, 771.901245f, 191.017654f, 6.095755f });
                    summon->DespawnOrUnsummon(50000);
                    break;
                }
                case NPC_STORMLING_PRE_EFFECT:
                    summon->CastSpell(summon, SPELL_STORMLING, true);
                    summon->CastWithDelay(4000, summon, SPELL_STORMLING_SUMMON, true);
                    break;
                case NPC_STORMLING:
                    summon->CastSpell(summon, SPELL_STORMLING_PERIODIC, true);
                    summon->CastSpell(summon, SPELL_STORMLING_PERIODIC_2, true);
                    break;
                case NPC_LIGHTING_CLOUD:
                {
                    summon->DespawnOrUnsummon(RAID_MODE(20000, 20000, 30000, 30000));
                    float orient = 0.0f;
                    int clouds{ 18 };
                    for (int i = 0; i < clouds; i++)
                    {
                        float x, y;
                        float x2, y2;
                        float x3, y3;
                        float x4, y4;
                        float x5, y5;

                        float change_next_orientation{ 2.0f * M_PI / float(clouds) };
                        float inbetween_next_orientation{ change_next_orientation / 5.f };
                        summon->GetNearPoint2D(x,  y,   120.f, orient);
                        summon->GetNearPoint2D(x2, y2,  100.f, orient + (inbetween_next_orientation * 1.f));
                        summon->GetNearPoint2D(x3, y3,  80.f, orient + (inbetween_next_orientation * 2.f));
                        summon->GetNearPoint2D(x4, y4,  60.f, orient + (inbetween_next_orientation * 3.f));
                        summon->GetNearPoint2D(x5, y5,  30.f, orient + (inbetween_next_orientation * 4.f));

                        summon->CastSpell(x, y, summon->GetPositionZ(), SPELL_LIGHTNING_CLOUDS_SUMMON, true);
                        summon->CastSpell(x2, y2, summon->GetPositionZ(), SPELL_LIGHTNING_CLOUDS_SUMMON, true);
                        summon->CastSpell(x3, y3, summon->GetPositionZ(), SPELL_LIGHTNING_CLOUDS_SUMMON, true);
                        summon->CastSpell(x4, y4, summon->GetPositionZ(), SPELL_LIGHTNING_CLOUDS_SUMMON, true);
                        summon->CastSpell(x5, y5, summon->GetPositionZ(), SPELL_LIGHTNING_CLOUDS_SUMMON, true);

                        orient += change_next_orientation;
                    }
                    summon->CastWithDelay(5000, summon, SPELL_LIGHTING_CLOUD_DAMAGE, true);
                    break;
                }
                case NPC_LIGHTING_CLOUD_EXTRA_VISUAL:
                    summon->DespawnOrUnsummon(RAID_MODE(20000, 20000, 30000, 30000));
                    summon->CastSpell(summon, SPELL_LIGHTNING_CLOUDS_PERIODIC, true);
                    summon->CastSpell(summon, SPELL_LIGHTNING_CLOUDS_DUMMY, true);
                    break;
                default:
                    break;
            }
            BossAI::JustSummoned(summon);
        }

        void SetGUID(uint64 guid, int32 id) 
        {
            if (id == 99)
            {
                if (auto lightning_center = Unit::GetCreature(*me, current_lightning_strike_guid))
                    lightning_center->DespawnOrUnsummon();

                current_lightning_strike_guid = guid;
            }
        }

        uint64 GetGUID(int32 id) const 
        {
            if (id == 99)
                return current_lightning_strike_guid;
            return 0;
        }

        void EnterCombat(Unit * /*who*/)
        {
            HideTrigger(false);
            instance->DisableFallDamage(true);
            _phase = PHASE_1;
            Talk(0);
            events.SetPhase(_phase);
            DoCast(SPELL_TOO_CLOSE_KNOCK_BACK_EFFECT);
            instance->DoCastSpellOnPlayers(SPELL_BIG_RAIN);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYERS_CAN_FLIGHT);
            events.ScheduleEvent(EVENT_ICE_STORM, 1000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_ELECTROCUTE, 1000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_LIGHTING_STRIKE, 9000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_SQUALL_LINE_W, 1000);
            events.ScheduleEvent(EVENT_WIND_BURST, 25000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_STATIC_SHOCK, 5000, 0, PHASE_1);
            _EnterCombat();
        }

        void EnterEvadeMode()
        {
            me->Respawn(true);
            Reset();
        }

        void DamageTaken(Unit* caster, uint32& damage)
        {
            if (me->HasReactState(REACT_PASSIVE) && _phase != PHASE_3)
                me->SetReactState(REACT_AGGRESSIVE);
            if (_phase == PHASE_1 && HealthBelowPct(81))
            {
                uint32 nextElectrocute = events.GetNextEventTime(EVENT_ELECTROCUTE);
                uint32 nextStaticShock = events.GetNextEventTime(EVENT_STATIC_SHOCK);
                _phase = PHASE_2;
                Talk(3);
                events.Reset();
                events.SetPhase(_phase);
                events.ScheduleEvent(EVENT_ACID_RAIN, 0, 0, PHASE_2);
                events.ScheduleEvent(EVENT_STATIC_SHOCK, nextStaticShock, 0, PHASE_2);
                events.ScheduleEvent(EVENT_SUMMON_STORMLING, 10000, 0, PHASE_2);
                events.ScheduleEvent(EVENT_ELECTROCUTE, nextElectrocute, 0, PHASE_2);
                events.ScheduleEvent(EVENT_SQUALL_LINE_W, 0, 0, PHASE_2);
            }
            else if (_phase == PHASE_2 && HealthBelowPct(26))
            {
                float lock_orientation{ me->GetOrientation() };
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                _phase = PHASE_3;
                Talk(5);
                summons.DespawnAll();
                events.Reset();
                events.SetPhase(_phase);
                events.ScheduleEvent(EVENT_REMOVE_ACID_RAID, 0, 0, PHASE_3);
                //DoCast(SPELL_RELENTLESS_STORM_INITIAL_VEHICLE_RIDE_TRIGGER);
                DoCast(SPELL_RELENTLESS_STORM_CHANNEL);
                events.ScheduleEvent(EVENT_WIND_BURST_P3, 5000, 0, PHASE_3);
                events.ScheduleEvent(EVENT_LIGHTING_CLOUD, 5000, 0, PHASE_3);
                events.ScheduleEvent(EVENT_LIGHTING_CLOUD_SUMMON, 20000, 0, PHASE_3);
                events.ScheduleEvent(EVENT_LIGHTING_ROD, 5000, 0, PHASE_3);
                events.ScheduleEvent(EVENT_LIGHTING, 8000, 0, PHASE_3);
                me->CastSpell(me->GetPositionX(), me->GetPositionY(), 192.0f, SPELL_LIGHTNING_CLOUD_SUMMON, true); // like on video's
                me->SetFacingTo(lock_orientation);
                StartP3();
            }
            else if (_phase == PHASE_3 && damage > me->GetHealth())
            {
                _phase = PHASE_OUTRO;
                Talk(6);
                DoCast(SPELL_SUMMON_CHEST);
            }
        }

        void StartP3()
        {

            HideTrigger(true);
            events.ScheduleEvent(EVENT_DESTROY_PLATFORM, 1000, 0, PHASE_3);

        }


        void StartP3_per_player(Player* player)
        {

            if (!player->isGameMaster())
                if (player->isAlive()/* && !AccountMgr::IsGMAccount(player->GetSession()->GetSecurity())*/)
                    Findp3_seat(player);
            /*
                else TC_LOG_ERROR("sql.sql", "StartP3_per_player failed 1 for %u", player->GetGUID());
            else TC_LOG_ERROR("sql.sql", "StartP3_per_player failed 2 for %u", player->GetGUID());
            */

                
                    //player->CastSpell(transportPl, SPELL_RELENTLESS_STORM_INITIAL_VEHICLE_RIDE, true);
                    //player->SetViewpoint(transportPl, true);
                    //transportPl->m_Events.AddEvent(new IntroP3VisualEvent(transportPl, player->GetGUID()), transportPl->m_Events.CalculateTime(100));
                    //transportPl->m_Events.AddEvent(new EndIntroP3VisualEvent(transportPl, player->GetGUID()), transportPl->m_Events.CalculateTime(8000));
                
        }


        void Findp3_seat(Player* player)
        {
            if (!player)
                return;
            Position pos(*me);
            me->MovePosition(pos, me->GetExactDist2d(player) + 50.f, player->GetAngle(me), false);
            pos.m_positionZ = player->GetPositionZ() + 50.f;
            pos.m_orientation = (player->GetOrientation());
                if (auto c = me->SummonCreature(NPC_RELENTLESS_STORM_INITIAL_VEHICLE, pos))
                    player->EnterVehicle(c);
        }

        void JustDied(Unit * /*killer*/)
        {
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BIG_RAIN);
            instance->DoRemoveAurasDueToSpellOnPlayers(95751);
            instance->DoRemoveAurasDueToSpellOnPlayers(85269);
            instance->CompleteGuildCriteriaForGuildGroup(CRITERIA_GUILD_RUN_ALAKIR);
            instance->CompleteGuildCriteriaForGuildGroup(CRITERIA_GUILD_RUN_ALAKIR_2);
            _JustDied();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {

                return;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_ICE_STORM:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            //TC_LOG_ERROR("sql.sql", "EVENT_ICE_STORM");

                            DoCast(me, SPELL_SUMMON_ICE_MOBILE_SPAWNER, true);
                            events.ScheduleEvent(EVENT_ICE_STORM, urand(25000, 30000), 0, PHASE_1);
                        }
                        break;
                    case EVENT_ELECTROCUTE:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            if (auto victim = me->getVictim())
                                if (victim->GetDistance2d(me->GetPositionX(), me->GetPositionY()) > 35.0f)
                                    DoCastVictim(SPELL_ELECTROCUTE);
                            events.ScheduleEvent(EVENT_ELECTROCUTE, 1000, 0, _phase);
                        }
                        break;
                    case EVENT_LIGHTING_STRIKE:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            //me->MonsterYell("EVENT_LIGHTING_STRIKE", LANG_UNIVERSAL, 0);

                            auto list_of_players = me->GetPlayersInRange(140.f, true);
                            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            {
                                DoCast(random_target, SPELL_LIGHTING_STRIKE_CONE_DAMAGE);

                                std::list<Creature*> list_of_npcs;
                                GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_LIGHTING_STRIKE_TRIGGER, 100.0f);

                                if (list_of_npcs.size())
                                    for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                                        if (auto t = (*itr))
                                        {
                                            float limits        { 2.f * (M_PI / 9.f)                       };//40 degrees
                                            float start_angle   { me->GetRelativeAngleOffset(random_target) };
                                            shocking_angle = start_angle;
                                            float left_bound    { start_angle - limits                      };
                                            float right_bound   { start_angle + limits                      };
                                            float compare_angle { me->GetRelativeAngleOffset(t)             };
                                            
                                            if (compare_angle < left_bound || compare_angle > right_bound)
                                            {
                                                list_of_npcs.erase(itr);
                                            }
                                        }

                                if (list_of_npcs.size())
                                    for (auto c : list_of_npcs)
                                        c->CastSpell(me->GetPositionX(), me->GetPositionY(), c->GetPositionZ(), SPELL_LIGHTING_STRIKE_DUMMY, true);
                            }

                            DoCast(me, SPELL_LIGHTING_STRIKE_PERIODIC_PULSE, true);
                            //89583 cosmetic lightning
                            events.ScheduleEvent(EVENT_LIGHTING_STRIKE, 20000, 0, PHASE_1);
                        }
                        break;
                    case EVENT_SQUALL_LINE_W:
                    case EVENT_SQUALL_LINE_E:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(2);
                            me->CastSpell(me, SPELL_SUMMON_SQUALL_LINE_SE, true);
                            DoCast(SPELL_SQUALL_LINE_2);
                            events.ScheduleEvent(EVENT_SQUALL_LINE_W, urand(55000, 65000), 0, _phase);
                        }
                        break;
                    case EVENT_WIND_BURST:
                        Talk(1);
                        DoCast(SPELL_WIND_BURST);
                        events.ScheduleEvent(EVENT_WIND_BURST, urand(25000, 30000), 0, PHASE_1);
                        break;
                    case EVENT_SUMMON_STORMLING:
                        Talk(5);
                        DoCastRandom(SPELL_SUMMON_STORMLING, 0.0f);
                        events.ScheduleEvent(EVENT_SUMMON_STORMLING, 20000, 0, PHASE_2);
                        break;
                    case EVENT_ACID_RAIN:
                        DoCast(SPELL_ACID_RAIN);
                        break;
                    case EVENT_REMOVE_ACID_RAID:
                        me->RemoveAurasDueToSpell(RAID_MODE(88290, 101451, 101452, 101453));
                        DoCast(SPELL_ACID_RAIN_2);
                        break;
                    case EVENT_WIND_BURST_P3:
                        DoCast(SPELL_WIND_BURST_2);
                        break;
                    case EVENT_LIGHTING_CLOUD:
                        DoCast(SPELL_LIGHTNING_CLOUD_PERIODIC);
                        break;
                    case EVENT_LIGHTING_ROD:
                        DoCast(SPELL_LIGHTNING_ROD);
                        break;
                    case EVENT_LIGHTING:
                        DoCastRandom(SPELL_LIGHTNING_2, 0.0f);
                        events.ScheduleEvent(EVENT_LIGHTING, 2000, 0, PHASE_3);
                        break;
                    case EVENT_LIGHTING_CLOUD_SUMMON:
                    {
                        float z = 350.0f;
                        if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            z = target->GetPositionZ();
                        me->CastSpell(me->GetPositionX(), me->GetPositionY(), z, SPELL_LIGHTNING_CLOUD_SUMMON, true);
                        events.ScheduleEvent(EVENT_LIGHTING_CLOUD_SUMMON, RAID_MODE(15000, 15000, 10000, 10000), 0, PHASE_3);
                        break;
                    }
                    // heroic part
                    case EVENT_STATIC_SHOCK:
                    {
                        DoCast(SPELL_STATIC_SHOCK);
                        events.ScheduleEvent(EVENT_STATIC_SHOCK, 5000, 0, _phase);
                        break;
                    }
                    case EVENT_DESTROY_PLATFORM:
                    {

                        auto players = me->GetPlayersInRange(500.f, true);
                        for (auto player : players)
                            StartP3_per_player(player);

                        //me->DespawnCreaturesInArea(NPC_FALL_CATCHER, 500.f);
                        me->DespawnCreaturesInArea(NPC_ICE_PATCH_STATIONARY, 500.f);
                        me->DespawnCreaturesInArea(NPC_ICESTORM_MOBILE, 500.f);
                        me->DespawnCreaturesInArea(NPC_SQUALL_LINE_E, 500.f);
                        me->DespawnCreaturesInArea(NPC_SQUALL_LINE_W, 500.f);
                        me->DespawnCreaturesInArea(NPC_SQUALL_LINE_VEHICLE_E, 500.f);
                        me->DespawnCreaturesInArea(NPC_SQUALL_LINE_VEHICLE_W, 500.f);

                        //me->MonsterYell("PHASE 3 STARTING", LANG_UNIVERSAL, 0);
                        
                        if (auto go = instance->instance->GetGameObject(instance->GetData64(GOB_CENTER_PLATFORM)))
                            go->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        

                        //instance->DoCastSpellOnPlayers(SPELL_PLAYERS_CAN_FLIGHT);
                        


                        
                        me->GetMotionMaster()->Clear();
                        DoStopAttack();
                        SetCombatMovement(false);
                        /*
                        Map::PlayerList const& players = me->GetMap()->GetPlayers();
                        if (!players.isEmpty())
                            for (auto ref : players)
                                if (auto player = ref.getSource())
                        */
                        
                        break;
                    }
                    default:
                        break;
                }
            
            if (!me->HasReactState(REACT_PASSIVE))
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        float shocking_angle{ 0.f };
        uint64 current_lightning_strike_guid{ 0 };
        uint8 _phase;
    };
};

class npc_alakir_icestorm : public CreatureScript
{
public:
    npc_alakir_icestorm() : CreatureScript("npc_alakir_icestorm") { }

    struct npc_alakir_icestormAI : public ScriptedAI
    {
        npc_alakir_icestormAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
                if (id == me->GetEntry())
                    next_guid = 0;
        }
        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_ICE_PATCH_STATIONARY://
                    summon->StopMoving();
                    summon->AddUnitState(UNIT_STATE_ROOT);
                    summon->SetReactState(REACT_PASSIVE);
                    if (auto alakir = Unit::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                        alakir->AI()->JustSummoned(summon);

                    //TC_LOG_ERROR("sql.sql", "NPC_ICE_PATCH_STATIONARY just summoned by NPC_ICESTORM_MOBILE");

                    break;
                case NPC_ICESTORM_MOBILE://SUMMONED
                    //TC_LOG_ERROR("sql.sql", "NPC_ICE_PATCH_STATIONARY just summoned NPC_ICE_PATCH_STATIONARY");
                    summon->DespawnOrUnsummon();
                    break;
                default:
                    break;
            }
        }
        void UpdateAI(const uint32 diff)
        {
            if (!next_guid)
                GetNextTarget();
        }

        void GetNextTarget()
        {
            if (instance)
                if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                {
                    me->SetFacingTo(me->GetAngle(alakir) + (clockwise ? (M_PI / 2.f) : (-M_PI / 2.f)));

                    //"me" is the npc executing this script. This argument specifies the centerpoint of the circle to check.
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, 46766, 50.0f);
                    if (list_of_npcs.size())
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto t = (*itr))
                                if (fabs(me->GetRelativeAngleOffset(t)) > (M_PI / 2.f) || me->GetDistance(t) < 3.f)//filter condition
                                    list_of_npcs.erase(itr);

                    if (list_of_npcs.size())
                    {
                        list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                        next_guid = list_of_npcs.front()->GetGUID();
                        me->GetMotionMaster()->MovePoint(me->GetEntry(), *list_of_npcs.front(), true, 2.f);
                        //TC_LOG_ERROR("sql.sql", "I am %u, my next guid: %u", me->GetGUID(), next_guid);
                    }
                }
        }

        void IsSummonedBy(Unit* summoner)
        {
            //TC_LOG_ERROR("sql.sql", "%u was just summoned by: %u", me->GetGUID(), summoner->GetEntry());

            if (instance)
                if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                {
                    alakir->AI()->JustSummoned(me);
                }
                else me->DespawnOrUnsummon();
            else me->DespawnOrUnsummon();
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        bool clockwise{ bool(urand(0,1)) };
        uint64 next_guid{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_icestormAI(creature);
    }
};

class spell_alakir_electrocute : public SpellScriptLoader
{
public:
    spell_alakir_electrocute() : SpellScriptLoader("spell_alakir_electrocute") { }

    class spell_alakir_electrocute_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_alakir_electrocute_AuraScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            return true;
        }

        void OnPeriodic(AuraEffect const* /*aurEff*/)
        {
            Unit* target = GetTarget();
            Unit *caster = GetCaster();
            if (!caster || !target)
                return;
            if (target->GetDistance2d(caster->GetPositionX(), caster->GetPositionY()) <= 35.0f)
                target->RemoveAurasDueToSpell(SPELL_ELECTROCUTE);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_alakir_electrocute_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_alakir_electrocute_AuraScript();
    }
};

class npc_azril_lighting_strike : public CreatureScript
{
public:
    npc_azril_lighting_strike() : CreatureScript("npc_azril_lighting_strike") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azril_lighting_strikeAI(creature);
    }

    struct npc_azril_lighting_strikeAI : public ScriptedAI
    {
        npc_azril_lighting_strikeAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            me->SetCanFly(true);
            me->SetDisableGravity(true);
            me->AddUnitState(UNIT_STATE_ROOT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            /*
            triggerDamage = 0;
            playerGUID[0] = 0;
            playerGUID[1] = 0;
            strikeCount[0] = 0;
            strikeCount[1] = 0;
            */
        }

        void PlayerInflictDamages()
        {
            /*
            int currentStrike = 0;
            if (strikeCount[0] != 0)
                currentStrike = 1;
            Unit *caster = Unit::GetUnit(*me, playerGUID[currentStrike]);
            if (!caster)
                return;
            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            if (!players.isEmpty())
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (auto player = itr->getSource())
                        if (player->GetGUID() != caster->GetGUID())
                            if (caster->GetDistance2d(player) < 30.0f)
                            {
                                Position pos;
                                player->GetPosition(&pos);
                                if (caster->HasInArc(static_cast<float>(M_PI / 3), &pos))
                                    caster->CastSpell(player, SPELL_LIGHTING_STRIKE_PLAYER, true);
                            }
            */
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason) 
        {  
            //TC_LOG_ERROR("sql.sql", "lighning trigger spell finished: %u %u", spell->Id, int32(reason));
        }

        void visualEffect()
        {
            if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                me->CastSpell(alakir->GetPositionX(), alakir->GetPositionY(), alakir->GetPositionZ(), SPELL_LIGHTING_STRIKE_DUMMY, true);
        }

        void damagePlayers()
        {
            /*
            int currentStrike = 0;
            if (strikeCount[0] != 0)
                currentStrike = 1;
            if (Unit *caster = Unit::GetUnit(*me, playerGUID[currentStrike]))
            {
                me->SetOrientation(orient[currentStrike]);
                visualEffect();
                //                me->CastSpell(caster, SPELL_LIGHTING_STRIKE_DUMMY, false);
                me->CastSpell(caster, SPELL_LIGHTING_STRIKE_CONE_DAMAGE, false);
                me->CastSpell(me, SPELL_LIGHTING_STRIKE_SC_EFF, false);
            }
            */
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
           // if (spell->Id == SPELL_LIGHTNING_STRIKE_TRIGGER)

        }

        void DoAction(int32 const action)
        {
            /*
            switch (action)
            {
                case ACTION_9_SEC_SHOCK:
                    events.ScheduleEvent(950, 1);
                    events.ScheduleEvent(951, 9100);
                    break;
                case ACTION_STOP_SHOCKING:
                    events.Reset();
                default:
                    break;
            }
            */
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 950:
                    visualEffect();
                    events.ScheduleEvent(950, 1000);
                    break;
                case 951:
                    events.Reset();
                    break;
                }
        }

    private:
        EventMap events;
        uint64 playerGUID[2];
        float orient[2];
        uint32 triggerDamage;
        uint8 strikeCount[2];
        InstanceScript* instance{ me->GetInstanceScript() };
    };
};

class npc_alakir_stormling : public CreatureScript
{
public:
    npc_alakir_stormling() : CreatureScript("npc_alakir_stormling") { }

    struct npc_alakir_stormlingAI : public ScriptedAI
    {
        npc_alakir_stormlingAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            end = false;
        }

        void DamageTaken(Unit* caster, uint32& damage)
        {
            if (damage > me->GetHealth())
                if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                    me->AddAura(SPELL_FEEDBACK, alakir);
        }

        void IsSummonedBy(Unit* summoner)
        {
            if (instance)
                if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                    alakir->AI()->JustSummoned(me);
        }
    private:
        InstanceScript* instance;
        bool end;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_stormlingAI(creature);
    }
};

class spell_alakir_wind_burst : public SpellScriptLoader
{
public:
    spell_alakir_wind_burst() : SpellScriptLoader("spell_alakir_wind_burst") { }

    class spell_alakir_wind_burst_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_alakir_wind_burst_AuraScript);

        void RemoveFlags(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {

            if (auto caster = GetCaster())
                if (auto target = GetTarget())
                {
                    /*
                    float x, y;
                    target->GetNearPoint2D(x, y, 4, target->GetAngle(caster) + M_PI);
                    target->NearTeleportTo(x, y, target->GetPositionZ(), target->GetOrientation());
                    */
                    caster->CastSpell(target, SPELL_PLAYERS_CAN_FLIGHT, true);
                }
        }


        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_alakir_wind_burst_AuraScript::RemoveFlags, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_alakir_wind_burst_AuraScript();
    }

    class spell_alakir_wind_burst_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_alakir_wind_burst_SpellScript);


        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (auto unit = GetHitUnit())
                unit->RemoveAurasDueToSpell(SPELL_PLAYERS_CAN_FLIGHT);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_alakir_wind_burst_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_alakir_wind_burst_SpellScript();
    }
};

class spell_lighting_cloud_dummy : public SpellScriptLoader
{
public:
    spell_lighting_cloud_dummy() : SpellScriptLoader("spell_lighting_cloud_dummy") { }

    class spell_lighting_cloud_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_lighting_cloud_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Player *target = GetHitPlayer())
            {
                target->CastSpell(target, SPELL_LIGHTNING_CLOUD_SUMMON, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_lighting_cloud_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_lighting_cloud_dummy_SpellScript();
    }
};

class npc_alakir_lighting_cloud : public CreatureScript
{
public:
    npc_alakir_lighting_cloud() : CreatureScript("npc_alakir_lighting_cloud") { }

    struct npc_alakir_lighting_cloudAI : public ScriptedAI
    {
        npc_alakir_lighting_cloudAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
        }

        void IsSummonedBy(Unit* summoner)
        {
            if (instance)
                if (auto alakir = Creature::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
                    alakir->AI()->JustSummoned(me);
        }
    private:
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_lighting_cloudAI(creature);
    }
};

class spell_lighting_cloud_damage : public SpellScriptLoader
{
public:
    spell_lighting_cloud_damage() : SpellScriptLoader("spell_lighting_cloud_damage") { }

    class LightingCloudTargetSelector
    {
    public:
        LightingCloudTargetSelector(Unit *caster): _caster(caster) { }

        bool operator() (WorldObject* target)
        {
            float dist = target->GetPositionZ() - _caster->GetPositionZ();
            if (dist < 0)
                dist *= -1;
            if (dist > 5)
                return true;
            return false;
        }
    private:
        Unit *_caster;
    };

    class spell_lighting_cloud_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_lighting_cloud_damage_SpellScript);

        bool Load()
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (Unit *caster = GetCaster())
                unitList.remove_if(LightingCloudTargetSelector(caster));
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lighting_cloud_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_lighting_cloud_damage_SpellScript();
    }
};

// 95751
class spell_alakir_eject_platform : public SpellScriptLoader
{
public:
    spell_alakir_eject_platform() : SpellScriptLoader("spell_alakir_eject_platform") { }

    class spell_alakir_eject_platform_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_alakir_eject_platform_AuraScript);

        void SetFlags(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
        }

        void RemoveFlags(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit *caster = GetUnitOwner())
                if (InstanceScript *instance = caster->GetInstanceScript())
                    if (auto alakir = Creature::GetCreature(*caster, instance->GetData64(DATA_ALAKIR)))
                    {
                        float x, y;
                        alakir->GetNearPoint2D(x, y, 50.0f, alakir->GetAngle(caster));
                        caster->AddAura(85269, caster);
                        caster->GetMotionMaster()->MoveJump(x, y, alakir->GetPositionZ(), 15.0f, 17.0f);
                    }
        }


        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_alakir_eject_platform_AuraScript::SetFlags, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_alakir_eject_platform_AuraScript::RemoveFlags, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_alakir_eject_platform_AuraScript();
    }
};

class npc_alakir_squall_vehicle : public CreatureScript
{
public:
    npc_alakir_squall_vehicle() : CreatureScript("npc_alakir_squall_vehicle") { }

    struct npc_alakir_squall_vehicleAI : public ScriptedAI
    {
        npc_alakir_squall_vehicleAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        int8 GetSeatForWaveSlot(int8 slot)
        {
            switch (slot)//inside to outside
            {
            case 1: 
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 6 : 7); break;
            case 2:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 4 : 6); break;
            case 3:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 2 : 4); break;
            case 4:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 0 : 2); break;
            case 5:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 1 : 0); break;
            case 6:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 3 : 1); break;
            case 7:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 5 : 3); break;
            case 8:
                return (me->GetEntry() == NPC_SQUALL_LINE_VEHICLE_E ? 7 : 5); break;
            }
            return 0;
        }

        void IsSummonedBy(Unit* summoner) override
        {
            if (summoner->GetEntry() != BOSS_ALAKIR)
                return;

            me->SetReactState(REACT_PASSIVE);
            me->DeleteThreatList();
            me->AttackStop();
            me->CombatStop();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE /*| UNIT_FLAG_NON_ATTACKABLE  | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_ATTACKABLE_1*/);
            me->DisableMovementFlagUpdate(true);
            me->SetDisableGravity(true);
            me->SetCanFly(true);

            Position start_moving(*me);
            start_moving.m_positionZ = me->GetPositionZ() + 2.f;
            me->NearTeleportTo(start_moving);

            /*
                //48852 has 3 on the inside
                //47034 has 4 on the inside
            */
            if (auto alakir = Unit::GetCreature(*me, instance->GetData64(DATA_ALAKIR)))
            {
                if (auto exclude1 = urand(1, 7))
                    if (auto exclude2 = exclude1 + 1)
                    {
                        for (auto i = 1; i <= 7; ++i)
                            if (auto c = me->SummonCreature(NPC_SQUALL_LINE_W, *me))
                            {
                                c->SetReactState(REACT_PASSIVE);
                                c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                //TC_LOG_ERROR("sql.sql", "squall attempting to enter seat %u", i);
                                c->EnterVehicle(me, i);

                                if (i != GetSeatForWaveSlot(exclude1) && i != GetSeatForWaveSlot(exclude2))
                                    c->AddAura(87621, c);
                                //else TC_LOG_ERROR("sql.sql", "not applying aura because seat is %u and excluded slots are %u and %u", i, GetSeatForWaveSlot(exclude1), GetSeatForWaveSlot(exclude2));
                                c->DespawnOrUnsummon(50000);
                            }

                        if (0 != GetSeatForWaveSlot(exclude1) && 0 != GetSeatForWaveSlot(exclude2))
                            me->AddAura(87621, me);
                        //else TC_LOG_ERROR("sql.sql", "not applying aura because seat is %u and excluded slots are %u and %u", 0, GetSeatForWaveSlot(exclude1), GetSeatForWaveSlot(exclude2));
                    }
                        
                //else TC_LOG_ERROR("sql.sql", "excluding seat %u", i);

                me->GetMotionMaster()->MoveAroundPoint(*alakir, me->GetExactDist(alakir), (me->GetEntry() == 47034), 40, alakir->GetPositionZ(), true);
                //events.ScheduleEvent(500, 500);
            }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
        }
        void PassengerBoarded(Unit* /*passenger*/, int8 seatId, bool /*apply*/) 
        {
            //TC_LOG_ERROR("sql.sql", "squall succeeded to enter seat %u", seatId);
        }

        void Reset() override
        {

        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 500:
                {
                    /*
                    if (auto v = me->GetVehicleKit())
                        for (auto i = 1; i <= 7; ++i)
                            if (auto p = v->GetPassenger(i))
                            {
                                std::ostringstream ostream;
                                ostream << "seat #" << i;
                                auto fullmsg{ ostream.str() };
                                auto fullmsg_char{ fullmsg.c_str() };

                                p->ToCreature()->MonsterYell(fullmsg_char, LANG_UNIVERSAL, NULL);
                            }
                    events.ScheduleEvent(500, 5000);
                    */
                    break;
                }
                default:
                    break;
                }
        }

    private:
        EventMap events;
        InstanceScript* instance{ me->GetInstanceScript() };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_squall_vehicleAI(creature);
    }
};

class spell_alakir_squall_wall : public SpellScriptLoader
{
public:
    spell_alakir_squall_wall() : SpellScriptLoader("spell_alakir_squall_wall") { }

    class spell_alakir_squall_wallSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_alakir_squall_wallSpellScript);

        uint32 summon_vehicles[2]
        {
            47034, 48852
        };

        void HandleAfterCast()
        {
            if (auto c = GetCaster())
            {
                Position spawn{ *c };
                Position spawn2{ *c };
                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, c, NPC_SQUALL_LINE_VEHICLE_W, 150.0f);

                if (!list_of_npcs.size())
                {
                    auto dist = 50.f;
                    auto dist2 = 47.f;

                    c->MovePosition(spawn, dist, (M_PI / 2.f));
                    c->MovePosition(spawn2, dist2, -(M_PI / 2.f));

                    if (auto c1 = c->SummonCreature(summon_vehicles[0], spawn))
                        c1->DespawnOrUnsummon(50000);
                    if (auto c2 = c->SummonCreature(summon_vehicles[1], spawn2))
                        c2->DespawnOrUnsummon(50000);
                }
                else if (list_of_npcs.size() == 1)
                {
                    for (auto cr : list_of_npcs)
                    {
                        auto random_entry = 47034;//(cr->GetEntry() == 47034 ? 48852 : 47034);
                        auto dist = (cr->GetEntry() == 47034 ? 50.f : 47.f);
                        c->MovePosition(spawn, dist, c->GetRelativeAngleOffset(cr) + M_PI);
                        c->SummonCreature(random_entry, spawn);
                    }
                }
                //else TC_LOG_ERROR("sql.sql", "could not spawn vehicle, as 2 or more are already existing.");
            }
        }
        void Register()
        {
            AfterCast += SpellCastFn(spell_alakir_squall_wallSpellScript::HandleAfterCast);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_alakir_squall_wallSpellScript();
    }
};

/*
    87855 -> cause player to cast 87856 on caster
*/
class spell_alakir_tornado_on_player : public SpellScriptLoader
{
public:
    spell_alakir_tornado_on_player() : SpellScriptLoader("spell_alakir_tornado_on_player") { }

    class spell_alakir_tornado_on_player_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_alakir_tornado_on_player_SpellScript);

        void HandleOnEffectHitTarget(SpellEffIndex /*effIndex*/)
        {
            if (auto caster = GetCaster())
            if (auto target = GetHitUnit())
                if (!target->GetVehicle())
                    if (caster->ToCreature()->AI()->GetData(5000) == 1)
                        target->CastSpell(caster, sSpellMgr->GetSpellForDifficultyFromSpell(sSpellMgr->GetSpellInfo(SPELL_SQUALL_LINE_BOARDED), caster), true);
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (auto d = GetCaster())
            {
                if (targets.size())
                    for (auto itr = targets.begin(); itr != targets.end(); ++ itr)
                        if (auto t = (*itr))
                            if (auto target = t->ToUnit())
                            {
                                auto n = target->FindNearestCreature(d->GetEntry(), 15.f);
                                if (target->GetExactDist(d) > 4.f)
                                {
                                    targets.erase(itr++);
                                }
                                else if (d->GetGUID() != n->GetGUID())
                                {
                                    targets.erase(itr++);
                                }
                            }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_alakir_tornado_on_player_SpellScript::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_alakir_tornado_on_player_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_alakir_tornado_on_player_SpellScript();
    }
};

class npc_alakir_squall_rider_tornado : public CreatureScript
{
public:
    npc_alakir_squall_rider_tornado() : CreatureScript("npc_alakir_squall_rider_tornado") { }

    struct npc_alakir_squall_rider_tornadoAI : public ScriptedAI
    {
        npc_alakir_squall_rider_tornadoAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
        }
        void PassengerBoarded(Unit* /*passenger*/, int8 seatId, bool apply)
        {
            if (!apply)
            {
                allowPlayer = false;
                events.ScheduleEvent(500, 10000);
            }
        }

        uint32 GetData(uint32 data) const override
        {
            if (data == 5000)
                return uint32(allowPlayer);

            return 0;
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 500:
                {
                    allowPlayer = true;
                    break;
                }
                default:
                    break;
                }
        }

    private:
        bool allowPlayer{ true };
        EventMap events;
        InstanceScript* instance{ me->GetInstanceScript() };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_squall_rider_tornadoAI(creature);
    }
};

class spell_alakir_summon_IceStorm : public SpellScriptLoader
{
public:
    spell_alakir_summon_IceStorm() : SpellScriptLoader("spell_alakir_summon_IceStorm") { }

    class spell_alakir_summon_IceStormSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_alakir_summon_IceStormSpellScript);

        void HandleAfterCast()
        {
            if (auto c = GetCaster())
                if (auto info = GetSpellInfo())
                    if (auto npc_id = 46734)
                    {
                        auto list_of_players = c->GetPlayersInRange(140.f, true);
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            c->SummonCreature(npc_id, *random_target);
                    }
        }
        void Register()
        {
            AfterCast += SpellCastFn(spell_alakir_summon_IceStormSpellScript::HandleAfterCast);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_alakir_summon_IceStormSpellScript();
    }
};

enum storm_vehicle_events
{   
    EVENT_EJECT_SEAT_0 = 1,
    EVENT_EJECT_SEAT_1,
    EVENT_EJECT_SEAT_2,
    EVENT_EJECT_SEAT_3,
    EVENT_EJECT_SEAT_4,

};

class npc_alakir_phaseThree_playerVehicle : public CreatureScript
{
public:
    npc_alakir_phaseThree_playerVehicle() : CreatureScript("npc_alakir_phaseThree_playerVehicle") { }

    struct npc_alakir_phaseThree_playerVehicleAI : public ScriptedAI
    {
        npc_alakir_phaseThree_playerVehicleAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
        }

        void IsSummonedBy(Unit* summoner) override
        {
            me->AddAura(89428, me);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell) override
        {
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply)
        {
            if (apply)
            {
                //TC_LOG_ERROR("sql.sql", "npc %u adding passenger %u into seat %u", me->GetGUID(), passenger->GetGUID(), seatId);
                passenger->RemoveAurasDueToSpell(SPELL_PLAYERS_CAN_FLIGHT);
                //events.ScheduleEvent(seatId+1, 8000);

                if (auto player = passenger->ToPlayer())
                    player->SetViewpoint(me, false);
                me->GetMotionMaster()->MovePoint(500, Top_Dest, false, 10.f, true);
            }
            else
            {
                if (auto player = passenger->ToPlayer())
                    player->SetViewpoint(player, false);
                //TC_LOG_ERROR("sql.sql", "npc %u removing passenger %u from seat %u", me->GetGUID(), passenger->GetGUID(), seatId);
                passenger->AddAura(SPELL_PLAYERS_CAN_FLIGHT, passenger);
                Position pos(*me);
                me->MovePosition(pos, 50.f, frand(0.f, M_PI*2.f), false);
                pos.m_positionZ = me->GetPositionZ();
                if (auto c = me->SummonCreature(144951, pos))
                {
                    pos.m_orientation = c->GetAngle(me);
                    passenger->NearTeleportTo(pos, true);
                    c->DespawnOrUnsummon(1000);
                }

                //if (auto p = passenger->ToPlayer())
                    //p->SetViewpoint(p, true);

            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
            {
                if (id == 500 || id == 501)
                {
                    if (auto v = me->GetVehicleKit())
                        v->RemoveAllPassengers();

                    me->DespawnOrUnsummon(5000);
                }
            }
        }

        void Reset() override
        {

        }

        void UpdateAI(const uint32 diff) override
        {
        

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_EJECT_SEAT_0:
                case EVENT_EJECT_SEAT_1:
                case EVENT_EJECT_SEAT_2:
                case EVENT_EJECT_SEAT_3:
                case EVENT_EJECT_SEAT_4:
                    if (auto v = me->GetVehicleKit())
                        if (auto passenger = v->GetPassenger(eventId-1))
                            v->RemovePassenger(passenger);
                    break;
                default:
                    break;
                }
        }

    private:
        bool moving                 { false                         };
        uint32 next_point           { 500                           };
        EventMap events;
        InstanceScript* instance    { me->GetInstanceScript()       };
        float taxi_top              { me->GetPositionZ() + 60.f    };
        float taxi_bottom           { me->GetPositionZ()            };
        Position Top_Dest           { me->GetPositionX(), me->GetPositionY(), taxi_top, me->GetOrientation() };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alakir_phaseThree_playerVehicleAI(creature);
    }
};

class spell_lightning_cloud_visuals : public SpellScriptLoader
{
public:
    spell_lightning_cloud_visuals() : SpellScriptLoader("spell_lightning_cloud_visuals") { }

    class spell_lightning_cloud_visuals_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_lightning_cloud_visuals_AuraScript);

        void OnPeriodic(AuraEffect const* aurEff)
        {
            auto index  { aurEff->GetEffIndex()                         };
            auto spell  { GetSpellInfo()->Effects[index].TriggerSpell   };

            if (auto c = GetCaster())
            {
                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, c, c->GetEntry(), 80.0f);
                if (list_of_npcs.size())
                    for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                        if (auto t = (*itr))
                            if (t->GetGUID() == c->GetGUID() || t->GetGUID() == target_guids[0] || t->GetGUID() == target_guids[1])
                                list_of_npcs.erase(itr);

                list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(c));
                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_npcs))
                {
                    c->CastWithDelay(delay, random_target, spell, true);
                    target_guids[index] = random_target->GetGUID();
                }
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_lightning_cloud_visuals_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_lightning_cloud_visuals_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        }
    private:
        uint32 delay{ urand(100, 900) };
        uint64 target_guids[2]
        { 0, 0 };
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_lightning_cloud_visuals_AuraScript();
    }
};

class spell_alakir_lightning_pulse : public SpellScriptLoader
{
public:
    spell_alakir_lightning_pulse() : SpellScriptLoader("spell_alakir_lightning_pulse") { }

    class spell_alakir_lightning_pulse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_alakir_lightning_pulse_AuraScript);

        bool Validate(SpellInfo const* /*spell*/)
        {
            return true;
        }

        void OnPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (auto c = GetCaster())
                if (auto cr = c->ToCreature())
                    if (auto ai = cr->AI())
                    {
                        //TC_LOG_ERROR("sql.sql", "spell_alakir_lightning_pulse succeeded in initiating doaction.");
                        ai->DoAction(ACTION_9_SEC_SHOCK);
                    }
            /*
            */
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_alakir_lightning_pulse_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_alakir_lightning_pulse_AuraScript();
    }
};

void AddSC_boss_alakir()
{
    new boss_alakir();

    new npc_alakir_icestorm();
    new npc_azril_lighting_strike();
    new npc_alakir_stormling();
    new npc_alakir_lighting_cloud();
    new npc_alakir_squall_vehicle();
    new npc_alakir_squall_rider_tornado();
    new npc_alakir_phaseThree_playerVehicle();

    new spell_alakir_electrocute();
    new spell_alakir_wind_burst();
    new spell_lighting_cloud_damage();
    new spell_alakir_eject_platform();
    new spell_alakir_squall_wall();
    new spell_alakir_tornado_on_player();
    new spell_alakir_summon_IceStorm();
    new spell_lightning_cloud_visuals();
    new spell_alakir_lightning_pulse();
}
