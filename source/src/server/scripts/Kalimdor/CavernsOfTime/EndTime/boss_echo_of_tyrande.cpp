/* TODO :
 * implement gauntlet event
 */

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
#include "endTime.h"
#include "CreatureTextMgr.h"


enum Texts
{
    // gauntlet event
    TALK_GAUNTLET_1        = 0,
    TALK_GAUNTLET_2        = 1,
    TALK_GAUNTLET_3        = 2,
    TALK_GAUNTLET_4        = 3,
    TALK_GAUNTLET_5        = 4,
    TALK_GAUNTLET_6        = 5,
    // fight
    TALK_ENTER_COMBAT      = 6,
    TALK_EYES_OF_GODDESS   = 7,
    TALK_MOONLANCE         = 8,
    TALK_GUIDANCE          = 9,
    EMOTE_STARFALL         = 10,
    TALK_TEARS             = 11,
    TALK_DEATH             = 12,
    // moonlight
    TALK_MOONLIGHT_DESPAWN = 5,
};

enum Spells
{
    SPELL_TYRANDE_ACHIEVEMENT_FAIL       = 102539,
    SPELL_TYRANDE_ACHIEVEMENT            = 102542,
    SPELL_TYRANDE_ACHIEVEMENT_TRACKER    = 102491,
    SPELL_IN_SHADOW                      = 101841,

    SPELL_LUNAR_BOLT                     = 102193,
    SPELL_STAR_DUST                      = 102173,
    SPELL_MOONLANCE                      = 102151,
    SPELL_LUNAR_GUIDANCE                 = 102472,
    SPELL_EYES_OF_THE_GODDESS_SC         = 102608,
    //    SPELL_SUMMON_EYES_OF_THE_GODDESS  = 102605, // unsued in sniff
    // SPELL_EYES_OF_THE_GODDESS_2       = 102248, // unsued in sniff
    SPELL_SUMMON_EYES_OF_THE_GODDESS     = 102181,
    SPELL_TEARS_OF_ELUNE                 = 102241,
    SPELL_TEARS_OF_ELUNE_SC              = 102242,
    SPELL_TEARS_OF_ELUNE_MISSILE         = 102243,
    SPELL_TEARS_OF_ELUNE_DAMAGE          = 102244,
    SPELL_TEARS_OF_ELUNE_STUN            = 102249,
    // NPC_GODDESS_EYES
    SPELL_PIERCING_GAZE_OF_ELUNE         = 102182,
    SPELL_PIERCING_GAZE_OF_ELUNE_DAMAGE  = 102182,
    // NPC_ELUNE_MOONLANCE_X
    SPELL_ELUNE_MOONLANCE_AURA           = 102150,
    SPELL_ELUNE_MOONLANCE_DAMAGE         = 102149,
    SPELL_LUNAR_MOONLANCE_DIVIDE         = 102152,
    // NPC_GEN_BUNNY
    SPELL_DARK_LUNAR_LIGHT               = 102414,
    SPELL_MOONLIGHT_AURA                 = 101946,
    SPELL_NIGHTSABER_MOONLIT             = 101842,
    SPELL_NIGHTSABER_DARK                = 101841,
};

enum Events
{
    EVENT_LUNAR_BOLT = 1,
    EVENT_STAR_DUST,
    EVENT_MOONLANCE,
    EVENT_EYES_GODDESS,
    EVENT_MOVE_TURN,
    EVENT_TEAR_OF_ELUNE,
    EVENT_LUNAR_GUIDANCE,

    EVENT_ACTIVATE_NPCS,
    EVENT_MOONLIGHT_NPCS_CHECK,
    EVENT_MOONLIGHT_NPCS_CHECK_2,
    EVENT_ACTIVATE_POOL,
    EVENT_CHECK_FOR_LIGHT,
    EVENT_CHECK_WIPE,
};


Position const PoolPositions[5] =
{
    {2906.522f,     66.774f,    3.324f,     5.326f},
    {2864.864f,     139.631f,   2.898f,     5.326f},
    {2775.498f,     125.622f,   1.148f,     5.326f},
    {2697.498f,     2.112f,     3.708f,     5.326f},
    {2810.433f,     0.764185f,  1.121087f,  5.326f},
};

enum Miscs
{
    POINT_TURN = 1,
    ACTION_START_COMBAT = 1,
    ACTION_START_INTRO_EVENT,
    ACTION_ACHIEVMENT_FAIL,
    ACTION_MOONWELL_KILLCOUNT,
    ACTION_MOONWELL_DESPAWN,
    // areatrigger enter = 6955 01:21:06.669
    // start event 01:21:46.559
    // 54544
};

class boss_echo_of_tyrande : public CreatureScript
{
public:
    boss_echo_of_tyrande() : CreatureScript("boss_echo_of_tyrande") { }
    struct boss_echo_of_tyrandeAI : public BossAI
    {
        boss_echo_of_tyrandeAI(Creature* creature) : BossAI(creature, BOSS_ECHO_OF_TYRANDE) 
        {
            creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }


        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void Reset()
        {
            _moonLanceTargetGUID = 0;
            _angle = 0.0f;
            _goddessAngle = 0.0f;
            firstTear = false;
            secondTear = false;
            tearofElune = false;
            me->CastSpell(me, SPELL_TYRANDE_ACHIEVEMENT_TRACKER, true);
            /*
            _npcsSize = 0;
            _currentTrashCount = 0;
            _gauntletNpcs.clear();
            _poolCount = 0;
            _currenMoonLightGUID = 0;
            */
            _Reset();
            if (instance->GetData(DATA_TYRANDE_GAUNTLET) != DONE)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                isInIntroPhase = true;
            }
            else
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                isInIntroPhase = false;
                DoAction(ACTION_START_COMBAT);
            }


            if (auto eyea = me->FindNearestCreature(NPC_GODDESS_EYES_A, 100.0))
                eyea->DespawnOrUnsummon();
            if (auto eyeb = me->FindNearestCreature(NPC_GODDESS_EYES_B, 100.0))
                eyeb->DespawnOrUnsummon();

        }

        void JustDied(Unit* killer)
        {
            Talk(TALK_DEATH);
            _JustDied();
            summons.DespawnAll();
            me->DeleteThreatList();


            if (auto eyea = me->FindNearestCreature(NPC_GODDESS_EYES_A, 100.0))
                eyea->DespawnOrUnsummon();
            if (auto eyeb = me->FindNearestCreature(NPC_GODDESS_EYES_B, 100.0))
                eyeb->DespawnOrUnsummon();
        }

        void IntializeGauntletTrashes(Creature* moonWell)
        {
            _gauntletNpcs.clear();
            moonWell->GetCreatureListWithEntryInGrid(_gauntletNpcs, NPC_TIMED_TWISTED_NIGHT_SABER, 100.0f);
            moonWell->GetCreatureListWithEntryInGrid(_gauntletNpcs, NPC_TIME_TWISTED_SABER_2, 100.0f);
            moonWell->GetCreatureListWithEntryInGrid(_gauntletNpcs, NPC_TIME_TWISTED_SABER_3, 100.0f);
            moonWell->GetCreatureListWithEntryInGrid(_gauntletNpcs, NPC_TIME_TWISTED_SENTINEL, 100.0f);
            moonWell->GetCreatureListWithEntryInGrid(_gauntletNpcs, NPC_TIME_TWISTED_HUNTRESS, 100.0f);
            if (_gauntletNpcs.size())
            _gauntletNpcs.sort(Trinity::ObjectDistanceOrderPred(moonwell[GetMoonWell(true)]));
        }

        uint32 GetMoonWell(bool current = false)
        {
            for (auto i = 0; i <= 4; ++i)
                if (Moonwell_state[i] == false)
                {
                    if (i > 0 && current)
                        return i - 1;
                    else
                     return i;
                }
                return 4;
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_START_COMBAT:
                {
                    me->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    me->HandleEmoteCommand(EMOTE_STATE_STAND);
                    if (auto c = me->FindNearestCreature(NPC_GEN_BUNNY, 20.0f))
                    {
                        me->AddAura(102414, me);
                        c->AddAura(108642, c);
                        c->setFaction(me->getFaction());
                    }

                    if (!achievementFailed)
                        DoCastAOE(SPELL_TYRANDE_ACHIEVEMENT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TYRANDE_ACHIEVEMENT_TRACKER);
                    isInIntroPhase = false;
                    events.CancelEvent(EVENT_ACTIVATE_NPCS);
                    events.CancelEvent(EVENT_MOONLIGHT_NPCS_CHECK);
                    events.CancelEvent(EVENT_ACTIVATE_POOL);
                    me->RemoveAurasDueToSpell(SPELL_IN_SHADOW);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                }
                case ACTION_START_INTRO_EVENT:
                    if (GetMoonWell() == 0)
                    if (moonwell[GetMoonWell()] == nullptr)
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                        isInIntroPhase = true;

                        if (!events.HasEvent(EVENT_ACTIVATE_POOL))
                        {
                            sCreatureTextMgr->SendChat(me, GetMoonWell(), 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_MAP);
                            //Talk(GetMoonWell());
                            achievementFailed = false;
                            DoCastAOE(SPELL_TYRANDE_ACHIEVEMENT_TRACKER);
                            events.ScheduleEvent(EVENT_ACTIVATE_POOL, 5000);
                        }
                        events.ScheduleEvent(EVENT_MOONLIGHT_NPCS_CHECK, 11000);
                        events.ScheduleEvent(EVENT_ACTIVATE_NPCS, 3000);
                    }
                    break;
                case ACTION_ACHIEVMENT_FAIL:
                    achievementFailed = true;
                    break;
                case ACTION_MOONWELL_KILLCOUNT:
                    moonwell_KC[GetMoonWell(true)]++;

                    if (moonwell_KC[GetMoonWell(true)] >= 15)
                    {
                        if (!moonwell_completed[GetMoonWell(true)])
                        {
                            moonwell_completed[GetMoonWell(true)] = true;
                            moonwell[GetMoonWell(true)]->AI()->Talk(5);


                            if (auto moonlight = moonwell[GetMoonWell(true)])
                                moonlight->SetObjectScale(0.1f);

                            moonwell[GetMoonWell(true)]->AI()->DoAction(ACTION_MOONWELL_DESPAWN);

                            //Talk(GetMoonWell(true));
                            sCreatureTextMgr->SendChat(me, GetMoonWell(), 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_MAP);

                            if (!events.HasEvent(EVENT_ACTIVATE_POOL))
                                events.ScheduleEvent(EVENT_ACTIVATE_POOL, 5000);
                        }
                    }
                    else
                        ManageMoonLightScale();
                    break;
                case EVENT_ACTIVATE_NPCS:
                    SendGauntletNPC();
                    break;
            }
        }

        void SendGauntletNPC()
        {
            if (auto moonwell_c = moonwell[GetMoonWell(true)])
            if (auto p = moonwell_c->SelectNearestPlayer(100.f))
            if (auto punit = p->ToUnit())
            {
                if (_gauntletNpcs.size())
                    for (auto itr = _gauntletNpcs.begin(); itr != _gauntletNpcs.end(); ++itr)
                        if (auto t = (*itr))
                        {
                            if (t->isDead())
                            {
                                t->DespawnOrUnsummon();
                                _gauntletNpcs.erase(itr);
                            }
                            else if (auto v = t->getVictim())
                            {
                                _gauntletNpcs.erase(itr);
                            }
                        }

                if (_gauntletNpcs.size())
                {
                    for (auto itr = _gauntletNpcs.begin(); itr != _gauntletNpcs.end(); itr++)
                        if (auto trash = (*itr))
                        {
                            //TC_LOG_ERROR("sql.sql", "Tyrande script: trash is out of combat and alive: %u", trash->GetDBTableGUIDLow());
                            
                                trash->SetReactState(REACT_AGGRESSIVE);
                                trash->AddThreat(punit, 1.f);
                                trash->CastWithDelay(1000, punit, 92111);
                                trash->AI()->AttackStart(punit);
                                break;
                            
                        }
                }
                else
                {
                    //TC_LOG_ERROR("sql.sql", "NPC SPAWNING FOR CREDIT");
                    Position pos(*moonwell_c);
                    moonwell_c->MoveBlink(pos, 20.f, frand(0, M_PI * 2.f));
                    if (auto c = me->SummonCreature(GetSummon(), pos))
                        {
                            //TC_LOG_ERROR("sql.sql", "Setting NPC %u in combat with zone: %u", c->GetGUID());
                            c->SetReactState(REACT_AGGRESSIVE);
                            c->AddThreat(p->ToUnit(), 10000.f);
                            c->CastWithDelay(1000, p->ToUnit(), 92111);
                            c->AI()->AttackStart(p);
                        }
                }
            }
        }

        uint32 GetSummon()
        {
            switch (urand(0, 3))
            {
            case 0:
                return NPC_TIMED_TWISTED_NIGHT_SABER;
            case 1:
                return NPC_TIME_TWISTED_SABER_2;
            case 2:
                return NPC_TIME_TWISTED_SABER_3;
            case 3:
                return NPC_TIME_TWISTED_SENTINEL;
            default:
                TC_LOG_ERROR("sql.sql", "Echo of Tyrande: Moonlight trigger invalid random value in GetSummon();");
                return NPC_TIMED_TWISTED_NIGHT_SABER;
                break;
            }
        }

        void EnterCombat(Unit* who)
        {
            Talk(TALK_ENTER_COMBAT);

            if (isInIntroPhase)
                return;

            events.ScheduleEvent(EVENT_LUNAR_BOLT, 3000);   //twice
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
            _EnterCombat();
            me->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
            me->HandleEmoteCommand(EMOTE_STATE_STAND);
        }

        uint32 GetNextEvent(uint32 CurrentEvent)
        {
            if (CurrentEvent == EVENT_LUNAR_BOLT)
            {
                if (!firstBolt)
                {
                    firstBolt = true;
                    return EVENT_LUNAR_BOLT;
                }

                switch (bolt_rotation)
                {
                case EVENT_MOONLANCE:
                    bolt_rotation = EVENT_STAR_DUST;
                    return EVENT_STAR_DUST;
                case EVENT_STAR_DUST:
                    if (!me->FindNearestCreature(NPC_GODDESS_EYES_A, 30.f, true) && !me->FindNearestCreature(NPC_GODDESS_EYES_A, 30.f, true))
                        {
                            bolt_rotation = EVENT_EYES_GODDESS;
                            return EVENT_EYES_GODDESS;
                        }
                        else
                        {
                            bolt_rotation = EVENT_MOONLANCE;
                            return EVENT_MOONLANCE;
                        }
                case EVENT_EYES_GODDESS:
                    bolt_rotation = EVENT_MOONLANCE;
                    return EVENT_MOONLANCE;
                default:
                    bolt_rotation = EVENT_MOONLANCE;
                    return EVENT_MOONLANCE;
                }
            }
            else
            {
                firstBolt = false;
                return EVENT_LUNAR_BOLT;
            }
        }

        void DamageTaken(Unit* /*caster*/, uint32& damage)
        {
            if (HealthBelowPct(31) && !tearofElune)
            {
                events.ScheduleEvent(EVENT_TEAR_OF_ELUNE, 1);
                tearofElune = true;
            }
            else if (HealthBelowPct(55) && !secondTear)
            {
                events.ScheduleEvent(EVENT_LUNAR_GUIDANCE, 1);
                secondTear = true;
            }
            else if (HealthBelowPct(80) && !firstTear)
            {
                events.ScheduleEvent(EVENT_LUNAR_GUIDANCE, 1);
                firstTear = true;
            }
        }

        void JustSummoned(Creature *summon)
        {
            //summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            switch (summon->GetEntry())
            {
                case NPC_POOL_OF_MOONLIGHT:
                {
                    _currenMoonLightGUID = summon->GetGUID();
                    summon->CastSpell(summon, SPELL_MOONLIGHT_AURA, true);
                    break;
                }
                case NPC_ELUNE_MOONLANCE:
                {
                    summon->SetReactState(REACT_PASSIVE);
                    summon->CastSpell(summon, SPELL_ELUNE_MOONLANCE_AURA, true);
                    if (Unit *target = Unit::GetUnit(*me, _moonLanceTargetGUID))
                    {
                        summon->SetOrientation(summon->GetAngle(target));
                        summon->SetFacingTo(summon->GetAngle(target));
                        _angle = summon->GetAngle(target);
                        Position pos;
                        Position pos_A;
                        Position pos_B;
                        Position pos_C;

                        summon->GetPosition(&pos);
                        summon->GetPosition(&pos_A);
                        summon->GetPosition(&pos_B);
                        summon->GetPosition(&pos_C);

                        summon->GetNearPosition(pos, 20.0f, 0.f);
                        summon->GetNearPosition(pos_A, 45.0f, (0-(M_PI/5.3f)));
                        summon->GetNearPosition(pos_B, 45.0f, M_PI/5.3f);
                        summon->GetNearPosition(pos_C, 45.0f, 0.f);

                        moonlance_destination[0] = pos;
                        moonlance_destination[1] = pos_A;
                        moonlance_destination[2] = pos_B;
                        moonlance_destination[3] = pos_C;
                        summon->GetMotionMaster()->MovePoint(NPC_ELUNE_MOONLANCE, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                    }
                    summon->CastWithDelay(1750, summon, SPELL_LUNAR_MOONLANCE_DIVIDE, false);
                    summon->DespawnOrUnsummon(1900);
                    break;
                }
                case NPC_ELUNE_MOONLANCE_A:
                {
                    summon->SetReactState(REACT_PASSIVE);
                    summon->CastSpell(summon, SPELL_ELUNE_MOONLANCE_AURA, true);

                    summon->GetMotionMaster()->MovePoint(NPC_ELUNE_MOONLANCE, moonlance_destination[1]);
                    summon->DespawnOrUnsummon(10000);
                    break;
                }
                case NPC_ELUNE_MOONLANCE_B:
                {
                    summon->SetReactState(REACT_PASSIVE);
                    summon->CastSpell(summon, SPELL_ELUNE_MOONLANCE_AURA, true);

                    summon->GetMotionMaster()->MovePoint(NPC_ELUNE_MOONLANCE, moonlance_destination[2]);
                    summon->DespawnOrUnsummon(10000);
                    break;
                }
                case NPC_ELUNE_MOONLANCE_C:
                {
                    summon->SetReactState(REACT_PASSIVE);
                    summon->CastSpell(summon, SPELL_ELUNE_MOONLANCE_AURA, true);

                    summon->GetMotionMaster()->MovePoint(NPC_ELUNE_MOONLANCE, moonlance_destination[3]);
                    summon->DespawnOrUnsummon(10000);
                    break;
                }
                case NPC_GODDESS_EYES_A:
                {
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_ALWAYS, true);
                    if (TempSummon* t = summon->ToTempSummon())
                    {
                        t->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);
                        t->InitStats(2147000000);
                    }
                    summon->CastSpell(summon, SPELL_PIERCING_GAZE_OF_ELUNE, true);
                    _goddessAngle = (float)rand_norm()*static_cast<float>(2*M_PI);
                    Position pos;
                    me->GetPosition(&pos);
                    me->GetNearPosition(pos, 21.0f, _goddessAngle);
                    summon->GetMotionMaster()->MovePoint(POINT_TURN, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                    break;
                }
                case NPC_GODDESS_EYES_B:
                {
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_ALWAYS, true);
                    if (TempSummon* t = summon->ToTempSummon())
                    {
                        t->SetTempSummonType(TEMPSUMMON_MANUAL_DESPAWN);
                        t->InitStats(2147000000);
                    }
                    summon->CastSpell(summon, SPELL_PIERCING_GAZE_OF_ELUNE, true);
                    _goddessAngle += M_PI;
                    _goddessAngle = summon->NormalizeOrientation(_goddessAngle);
                    Position pos;
                    me->GetPosition(&pos);
                    me->GetNearPosition(pos, 21.0f, _goddessAngle);
                    summon->GetMotionMaster()->MovePoint(POINT_TURN, pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                    break;
                }
                default:
                {
                    TC_LOG_ERROR("sql.sql", "just summoned npc entry %u with name %s", summon->GetEntry(), summon->GetName().c_str());
                    break;
                }
            }
        }

        void ManageMoonLightScale()
        {
            if (auto moonlight = moonwell[GetMoonWell(true)])
                moonlight->SetObjectScale(moonlight->GetFloatValue(OBJECT_FIELD_SCALE_X)*0.93f);
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            Unit* target{ nullptr };
            uint32 timer{ 3000 };
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                switch (spell)
                {
                case SPELL_LUNAR_BOLT:
                    if (secondTear)
                        timer *= 0.5f;
                    else if (firstTear)
                        timer *= 0.75f;
                    DoCastVictim(spell);
                    events.ScheduleEvent(GetNextEvent(eventId), urand(3000, 5000));
                    break;
                case SPELL_MOONLANCE:
                    if (target = SelectTarget(SELECT_TARGET_FARTHEST, 0))
                    {
                        _moonLanceTargetGUID = target->GetGUID();
                        Talk(TALK_MOONLANCE);
                        DoCast(target, spell);
                    }
                    events.ScheduleEvent(GetNextEvent(eventId), 3000);
                    break;
                case SPELL_STAR_DUST:
                    DoCast(spell);
                    events.ScheduleEvent(GetNextEvent(eventId), 3000);
                    break;
                case SPELL_EYES_OF_THE_GODDESS_SC:
                    Talk(TALK_EYES_OF_GODDESS);
                    DoCast(SPELL_EYES_OF_THE_GODDESS_SC);
                    events.ScheduleEvent(GetNextEvent(eventId), 3000);
                    break;
                case SPELL_TEARS_OF_ELUNE:
                    if (auto eyea = me->FindNearestCreature(NPC_GODDESS_EYES_A, 100.0))
                        eyea->DespawnOrUnsummon();
                    if (auto eyeb = me->FindNearestCreature(NPC_GODDESS_EYES_B, 100.0))
                        eyeb->DespawnOrUnsummon();
                    bolt_rotation = 0;
                    Talk(TALK_TEARS);
                    DoCast(SPELL_TEARS_OF_ELUNE);
                    break;
                case SPELL_LUNAR_GUIDANCE:
                    Talk(TALK_GUIDANCE);
                    DoCast(SPELL_LUNAR_GUIDANCE);
                    break;
                default:
                    DoCast(spell);
                    events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    break;
                }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

        void UpdateAI(const uint32 diff) override
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {

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
                case EVENT_ACTIVATE_POOL:
                {
                    if (GetMoonWell(true) < 4               /*&& 1 == 0   ---- DEV PURPOSES ONLY - WILL CAUSE BOSS TO BECOME FIGHTABLE AS SOON AS TRASH ENTER COMBAT*/)
                    {
                        //TC_LOG_ERROR("sql.sql", "preparing next pool.");
                        if (moonwell[GetMoonWell()] == nullptr)
                            if (moonwell[GetMoonWell()] = me->SummonCreature(NPC_POOL_OF_MOONLIGHT, PoolPositions[GetMoonWell()], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                //we've just "activated" our moonwell, from here on out referencing this moonwell requires the (true) arguement
                                IntializeGauntletTrashes(moonwell[GetMoonWell()]);
                                Moonwell_state[GetMoonWell()] = true;

                                //TC_LOG_ERROR("sql.sql", "Talking to map: %u", GetMoonWell(true));
                                if (auto c = moonwell[GetMoonWell(true)])
                                    sCreatureTextMgr->SendChat(c, GetMoonWell(true), 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_MAP);
                            }
                    }
                    else
                    {
                        instance->SetData(DATA_TYRANDE_GAUNTLET, DONE);
                        DoAction(ACTION_START_COMBAT);
                    }

                }
                case EVENT_MOONLIGHT_NPCS_CHECK:
                {
                    //nothing
                    break;
                }
                case EVENT_LUNAR_BOLT:
                    spell = SPELL_LUNAR_BOLT;                 //Spell ID
                    min_repeat = 3000;                             //Minimum Delay
                    max_repeat = 3000;                            //Maximum Delay
                    wait_castnext = true;                             //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_STAR_DUST:
                    spell = SPELL_STAR_DUST;                  //Spell ID
                    min_repeat = 15000;                            //Minimum delay
                    max_repeat = 20000;                            //Maximum delay
                    wait_castnext = true;                             //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_MOONLANCE:
                    spell = SPELL_MOONLANCE;                            //Spell ID
                    min_repeat = 7000;                                  //Minimum delay
                    max_repeat = 15000;                             //Maximum delay
                    wait_castnext = true;                            //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_EYES_GODDESS:
                    spell = SPELL_EYES_OF_THE_GODDESS_SC;     //Spell ID
                    min_repeat = 24000;                             //Minimum delay
                    max_repeat = 24000;                             //Maximum delay
                    wait_castnext = true;                            //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_TEAR_OF_ELUNE:
                    spell = SPELL_TEARS_OF_ELUNE;     //Spell ID
                    wait_castnext = true;                            //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_LUNAR_GUIDANCE:
                    spell = SPELL_LUNAR_GUIDANCE;     //Spell ID
                    wait_castnext = true;                            //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                default:
                    break;
                }

        }
    private:
        Position moonlance_destination[4]
        {
            {0.f, 0.f, 0.f, 0.f},
            {0.f, 0.f, 0.f, 0.f},
            {0.f, 0.f, 0.f, 0.f},
            {0.f, 0.f, 0.f, 0.f},
        };

        uint32 bolt_rotation{ 0 };
        bool firstBolt{ false };
        std::list<Creature*> _gauntletNpcs;
        uint64 _moonLanceTargetGUID, _currenMoonLightGUID;
        float _angle, _goddessAngle;
        bool tearofElune{false}, firstTear, secondTear, isInIntroPhase, achievementFailed;

        Creature* moonwell      [5] {   nullptr,    nullptr,    nullptr,    nullptr,    nullptr }; 
        bool Moonwell_state     [5] {   false,      false,      false,      false,      false   };
        uint32 moonwell_KC      [5] {   0,          0,          0,          0,          0       };
        bool moonwell_completed [5] {   false,      false,      false,      false,      false   };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_echo_of_tyrandeAI(creature);
    }
};

class npc_tyrande_goddess_eyes : public CreatureScript
{
public:
    npc_tyrande_goddess_eyes() : CreatureScript("npc_tyrande_goddess_eyes") {}

    struct npc_tyrande_goddess_eyesAI : public ScriptedAI
    {
        npc_tyrande_goddess_eyesAI(Creature * creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
        {
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }
        void InitializeAI()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == POINT_TURN)
                events.ScheduleEvent(EVENT_MOVE_TURN, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_MOVE_TURN:
                    {
                        if (Creature *tyrande = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                        {
                            Position pos;
                            tyrande->GetPosition(&pos);
                            float dist = me->GetExactDist2d(tyrande->GetPositionX(), tyrande->GetPositionY());
                            me->GetMotionMaster()->MoveAroundPoint(pos, dist, true, 254, 0.f, true);
                        }
                        break;
                    }
                }
            }
        }

    private:
        InstanceScript *instance;
        EventMap events;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_tyrande_goddess_eyesAI(creature);
    }
};

class npc_tyrande_trashes : public CreatureScript
{
public:
    npc_tyrande_trashes() : CreatureScript("npc_tyrande_trashes") {}

    struct npc_tyrande_trashesAI : public ScriptedAI
    {
        npc_tyrande_trashesAI(Creature * creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
        {
        }
        void InitializeAI()
        {
            //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->AddAura(101841, me);
            me->SetReactState(REACT_DEFENSIVE);
        }

        void Reset()
        {
            if (instance->GetData(DATA_TYRANDE_GAUNTLET) == DONE)
                me->DespawnOrUnsummon(1000);
            me->SetReactState(REACT_DEFENSIVE);
        }


        void MoveInLineOfSight(Unit* who)
        {
            if (!who)
                return;

            if (auto p = who->ToPlayer())
            if (me->IsWithinDistInMap(who, 30.0f, false))
                if (me->IsValidAttackTarget(who))
                    if (!me->isInCombat())
                        if (me->GetDistance(who) < 15.f)
                            {
                                me->AddThreat(who, 10.f);
                                AttackStart(who);
                            }
        }

        void JustReachedHome() 
        {
            auto myArray =
            {
                NPC_TIMED_TWISTED_NIGHT_SABER,
                NPC_TIME_TWISTED_SABER_2,
                NPC_TIME_TWISTED_SABER_3,
                NPC_TIME_TWISTED_SENTINEL,
                NPC_TIME_TWISTED_HUNTRESS,
            };
            if (std::find(begin(myArray), end(myArray), me->GetEntry()) != end(myArray)) //DOES INCLUDE, would be == end(myArray) to be "not includes".
            {
                me->RemoveAura(SPELL_NIGHTSABER_MOONLIT, AURA_REMOVE_BY_CANCEL);
                if (!events.HasEvent(EVENT_ACTIVATE_NPCS))
                    if (me->isAlive())
                        events.ScheduleEvent(EVENT_ACTIVATE_NPCS, 2000);
            }
        }
        void JustDied(Unit* killer) 
        {
            events.Reset();
            me->Dismount();

            if (auto moonwell = me->FindNearestCreature(NPC_POOL_OF_MOONLIGHT, 10.f))
                if (me->GetExactDist2d(moonwell) <= 5.f)
                    moonwell->AI()->SummonedCreatureDies(me, killer);

        }

        void EnterCombat(Unit* who)
        {
            me->SetReactState(REACT_DEFENSIVE);
            events.Reset();
            if (auto tyrande = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                tyrande->AI()->DoAction(ACTION_START_INTRO_EVENT);
                //tyrande->AI()->DoAction(ACTION_START_COMBAT);       //DEV PURPOSES ONLY
                //instance->SetData(DATA_TYRANDE_GAUNTLET, DONE);     //DEV PURPOSES ONLY

            events.ScheduleEvent(EVENT_CHECK_FOR_LIGHT, 5000);
            me->CallForHelp(2.f);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_HIDE_NAMEPLATE);
            me->SetVisible(true);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ACTIVATE_NPCS:
                    me->RemoveAura(SPELL_NIGHTSABER_MOONLIT, AURA_REMOVE_BY_CANCEL);
                    break;
                case EVENT_CHECK_FOR_LIGHT:
                    if (auto tyrande = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                    {
                        //TC_LOG_ERROR("sql.sql", "%u %u %u ", tyrande->HasReactState(REACT_AGGRESSIVE), tyrande->isDead(), tyrande->isInCombat());

                        bool removeAll{ (tyrande->GetHealthPct() < 99.f || tyrande->isDead() || tyrande->isInCombat()) };

                        if (removeAll)
                        {
                            me->RemoveAura(SPELL_NIGHTSABER_MOONLIT, AURA_REMOVE_BY_CANCEL);
                            me->RemoveAurasDueToSpell(SPELL_DARK_LUNAR_LIGHT);
                            me->RemoveAurasDueToSpell(SPELL_MOONLIGHT_AURA);
                            me->RemoveAurasDueToSpell(SPELL_NIGHTSABER_MOONLIT);
                            me->RemoveAurasDueToSpell(SPELL_NIGHTSABER_DARK);
                        }
                        else
                        {
                            if (Aura* a = me->GetAura(SPELL_NIGHTSABER_MOONLIT))
                            {
                                if (a->GetCaster() && me->GetDistance(a->GetCaster()) <= 6.f)
                                    a->Remove();
                            }
                        }
                    }

                        events.ScheduleEvent(EVENT_CHECK_FOR_LIGHT, 5000);
                    break;
                default:
                    break;
                }

            if (!me->isInCombat())
                return;

            if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript *instance;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_tyrande_trashesAI(creature);
    }
};

class npc_tyrande_moonlance : public CreatureScript
{
public:
    npc_tyrande_moonlance() : CreatureScript("npc_tyrande_moonlance") {}

    struct npc_tyrande_moonlanceAI : public ScriptedAI
    {
        npc_tyrande_moonlanceAI(Creature * creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
        {
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void IsSummonedBy(Unit *)
        {
            if (Creature *tyrande = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                tyrande->AI()->JustSummoned(me);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == NPC_ELUNE_MOONLANCE)
                if (me->GetEntry() != NPC_ELUNE_MOONLANCE)
                    me->DespawnOrUnsummon();
        }

        void UpdateAI(const uint32 diff)
        {
        }

    private:
        InstanceScript *instance;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_tyrande_moonlanceAI(creature);
    }
};

// 102608
class spell_tyrande_goddess_eyes : public SpellScriptLoader
{
public:
    spell_tyrande_goddess_eyes() : SpellScriptLoader("spell_tyrande_goddess_eyes") { }

    class spell_tyrande_goddess_eyes_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tyrande_goddess_eyes_SpellScript);

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
            {
                uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                caster->CastSpell(caster, spellId, false);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_tyrande_goddess_eyes_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_tyrande_goddess_eyes_SpellScript();
    }
};

// 102242
class spell_tyrande_tears_of_elune : public SpellScriptLoader
{
public:
    spell_tyrande_tears_of_elune() : SpellScriptLoader("spell_tyrande_tears_of_elune") { }

    class spell_tyrande_tears_of_elune_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tyrande_tears_of_elune_SpellScript);

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (caster->GetTypeId() == TYPEID_UNIT)
                {
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    caster->ToCreature()->AI()->DoCastRandom(spellId, 0.0f, true);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_tyrande_tears_of_elune_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_tyrande_tears_of_elune_SpellScript();
    }
};

class spell_tyrande_achievement_tracker : public SpellScriptLoader
{
public:
    spell_tyrande_achievement_tracker() : SpellScriptLoader("spell_tyrande_achievement_tracker") { }

    class spell_tyrande_achievement_tracker_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_tyrande_achievement_tracker_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return GetTarget()->GetTypeId() == TYPEID_PLAYER && GetTarget()->ToPlayer()->HasHealSpec();
        }

        void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            if (auto instance = GetTarget()->GetInstanceScript())
                if (auto tyrande = ObjectAccessor::GetCreature(*GetTarget(), instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                    tyrande->AI()->DoAction(ACTION_ACHIEVMENT_FAIL);
        }


        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_tyrande_achievement_tracker_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_tyrande_achievement_tracker_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_tyrande_achievement_tracker_AuraScript();
    }
};

class spell_Emerald_Moonlit : public SpellScriptLoader  
{
    /*
    * Dual Purpose script for 101946 (persistant on light bunny) and 101842 (triggered by 101946 every 500 ms)
    */
public:
    spell_Emerald_Moonlit() : SpellScriptLoader("spell_Emerald_Moonlit") { }

    class spell_Emerald_Moonlit_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_Emerald_Moonlit_AuraScript);


        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (GetOwner())
            if (auto owner = GetOwner()->ToUnit())
            {
                if (!owner->isInCombat())
                {
                    if (Aura* a = GetAura())
                        a->Remove();
                }
                else
                    owner->RemoveAurasDueToSpell(SPELL_NIGHTSABER_DARK);
            }
        }

        bool CheckAreaTarget(Unit* target)
        {
            if (target == GetCaster())
                return true;

            auto myArray =
            {
                NPC_TIMED_TWISTED_NIGHT_SABER,
                NPC_TIME_TWISTED_SABER_2,
                NPC_TIME_TWISTED_SABER_3,
                NPC_TIME_TWISTED_SENTINEL,
                NPC_TIME_TWISTED_HUNTRESS,
            };
            if (std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray)) //DOES INCLUDE, would be == end(myArray) to be "not includes".
            {
                if (target->isInCombat())
                    return true;
                return false;
            }
            return true;
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            auto myArray =
            {
                NPC_TIMED_TWISTED_NIGHT_SABER,
                NPC_TIME_TWISTED_SABER_2,
                NPC_TIME_TWISTED_SABER_3,
                NPC_TIME_TWISTED_SENTINEL,
                NPC_TIME_TWISTED_HUNTRESS,
            };

            if (GetOwner())
            if (auto owner = GetOwner()->ToUnit())
            if (std::find(begin(myArray), end(myArray), owner->GetEntry()) != end(myArray)) //DOES INCLUDE, would be == end(myArray) to be "not includes".
                    owner->AddAura(SPELL_NIGHTSABER_DARK, owner);
        }

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            PreventDefaultAction();
            bool cast{ false };
            if (auto caster = GetCaster())
            {

                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, caster, NPC_TIMED_TWISTED_NIGHT_SABER,    5.0f);
                GetCreatureListWithEntryInGrid(units, caster, NPC_TIME_TWISTED_SABER_2,         5.0f);
                GetCreatureListWithEntryInGrid(units, caster, NPC_TIME_TWISTED_SABER_3,         5.0f);
                GetCreatureListWithEntryInGrid(units, caster, NPC_TIME_TWISTED_SENTINEL,        5.0f);
                GetCreatureListWithEntryInGrid(units, caster, NPC_TIME_TWISTED_HUNTRESS,        5.0f);
                for (std::list<Creature*>::iterator itr = units.begin(); itr != units.end(); ++itr)
                    if (auto c = (*itr))
                        if (c->isInCombat())
                            if (c->HasAura(SPELL_NIGHTSABER_DARK) || !c->HasAura(101842))
                                cast = true;

                if (cast)
                    caster->CastSpell(caster, 101842);
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_Emerald_Moonlit_AuraScript::HandleOnEffectApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_Emerald_Moonlit_AuraScript::HandleEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_Emerald_Moonlit_AuraScript::CheckAreaTarget);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_Emerald_Moonlit_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_Emerald_Moonlit_AuraScript();
    }

    class spell_Emerald_Moonlit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_Emerald_Moonlit_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
            for (auto itr = targets.begin(); itr != targets.end();)
                if (auto this_itr = (*itr))
                {
                    if (this_itr->ToPlayer())
                        targets.erase(itr++);
                    else
                        ++itr;
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_Emerald_Moonlit_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_Emerald_Moonlit_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_Emerald_Moonlit_SpellScript();
    }
};


class npc_tyrande_moonwell_trigger : public CreatureScript
{
public:
    npc_tyrande_moonwell_trigger() : CreatureScript("npc_tyrande_moonwell_trigger") {}

    struct npc_tyrande_moonwell_triggerAI : public ScriptedAI
    {
        npc_tyrande_moonwell_triggerAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
        {
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            events.Reset();
        }

        void IsSummonedBy(Unit*)
        {
            me->SetReactState(REACT_PASSIVE);
            events.ScheduleEvent(EVENT_MOONLIGHT_NPCS_CHECK, 3000);
            events.ScheduleEvent(EVENT_MOONLIGHT_NPCS_CHECK_2, 3000);
        }

        void DoAction(int32 const id)
        {
            if (id == ACTION_MOONWELL_DESPAWN)
            {
                events.Reset();
                me->DespawnOrUnsummon(1000);
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) 
        {
            auto myArray =
            {
                NPC_TIMED_TWISTED_NIGHT_SABER,
                NPC_TIME_TWISTED_SABER_2,
                NPC_TIME_TWISTED_SABER_3,
                NPC_TIME_TWISTED_SENTINEL,
                NPC_TIME_TWISTED_HUNTRESS,
            };

            if (std::find(begin(myArray), end(myArray), summon->GetEntry()) != end(myArray)) //DOES INCLUDE, would be == end(myArray) to be "not includes".
                if (auto tyrande = Unit::GetCreature(*me, instance->GetData64(DATA_ECHO_OF_TYRANDE_GUID)))
                    tyrande->AI()->DoAction(ACTION_MOONWELL_KILLCOUNT);
        }
        void MobSpawn(bool credit)
        {
            auto players = me->GetPlayersInRange(10.f, true);
            if (players.size())
                if (auto tyrande = me->FindNearestCreature(NPC_ECHO_OF_TYRANDE, 500.0))
                    tyrande->AI()->DoAction(EVENT_ACTIVATE_NPCS);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_MOONLIGHT_NPCS_CHECK:
                    MobSpawn(true);
                    events.ScheduleEvent(EVENT_MOONLIGHT_NPCS_CHECK, 3000);
                    break;
                default:
                    break;
                }
        }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tyrande_moonwell_triggerAI(creature);
    }
};

class spell_moonlance_triggered : public SpellScriptLoader
{
    /*
    * spell 102149
    */
public:
    spell_moonlance_triggered() : SpellScriptLoader("spell_moonlance_triggered") { }

    class spell_moonlance_triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_moonlance_triggered_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (auto caster = GetCaster())
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                    if (auto this_itr = (*itr))
                        if (auto target = this_itr->ToUnit())
                        {
                            if (target == caster || target->HasAura(GetSpellInfo()->Id))
                                targets.erase(itr++);
                            else
                                ++itr;
                        }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_moonlance_triggered_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_moonlance_triggered_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_moonlance_triggered_SpellScript();
    }
};

void AddSC_boss_echo_of_tyrande()
{
    new boss_echo_of_tyrande();
    new npc_tyrande_goddess_eyes();
    new npc_tyrande_moonlance();
    new npc_tyrande_trashes();
    new npc_tyrande_moonwell_trigger();
    new spell_tyrande_goddess_eyes();
    new spell_tyrande_tears_of_elune();
    new spell_tyrande_achievement_tracker();
    new spell_Emerald_Moonlit();
    new spell_moonlance_triggered();
}
