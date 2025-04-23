/*
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

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "the_lost_city_of_tol_vir.h"

enum Texts
{
    SAY_LOCKMAW_DEATH = 0,
    SAY_CROCOLISK = 1,
};

enum Spells
{
    // Lockmaw
    SPELL_DUST_FLAIL = 81642,
    SPELL_SCENT_OF_BLOOD = 81690,
    SPELL_VENOMOUS_RAGE = 81706,
    SPELL_VISCOUS_POISON = 81630,
    // Augh
    SPELL_FRENZY = 91415, //OLD 23537
    SPELL_FIRE_DRAGON = 90026,
    SPELL_CLOUD = 84768,
    SPELL_WHIRLWIND = 84784,
    SPELL_PARALYTIC_BLOW_DART = 84799,
    SPELL_RANDOM_AGGRO = 50231,
    SPELL_STEALTHED = 84244,
    // misc
    SPELL_VICIOUS = 81677,
    SPELL_SUMMON_CROCOLISK = 84242,
    SPELL_SUMMON_AUGH = 84808,
    SPELL_SUMMON_AUGH_2 = 84809
};

enum Augh_IDs
{
    NPC_AUGH_FAKE_INTRO     = 45377,
    NPC_AUGH_P1             = 45378,
    NPC_AUGH_P2             = 49045,
};
enum Events
{
    //Lockmaw
    EVENT_DUST_FLAIL = 1,
    EVENT_DUST_FLAIL_PREP,
    EVENT_DUST_FLAIL_RECOVER,
    EVENT_SCENT_OF_BLOOD,
    EVENT_VISCOUS_POISON,
    EVENT_FORCE_AGGRO_CROCS,
    //Augh
    EVENT_CHECK_LOCKMAW,
    EVENT_RP_INTRO_0,
    EVENT_RP_INTRO_1,
    EVENT_RP_INTRO_2,
    EVENT_RP_INTRO_3,
    EVENT_PARALYTIC_BLOW_DART,
    EVENT_CLOUD,
    EVENT_WHIRLWIND,
    EVENT_FIRE_DRAGON,
    EVENT_SUMMON_AUGH,
    EVENT_STEALTH,
    EVENT_ANNOUNCE_APPEAR,
};

#define ACHIEVEMENT_EVENT_ACROCALYPSE    43658

Position augh_pt1_spawns[8]
{
    {-11055.007812f, -1715.559448f, 0.931111f, 0.792466f},
    {-11017.110352f, -1717.064575f, 0.597327f, 2.170838f},
    {-10997.979492f, -1679.311646f, 0.699108f, 2.492851f},
    {-10992.716797f, -1652.998657f, 1.147124f, 2.653858f},
    {-11016.977539f, -1570.583252f, 0.418110f, 4.260000f},
    {-11067.431641f, -1586.625366f, 0.412111f, 4.966857f},
    {-11094.000000f, -1597.746948f, 0.939884f, 5.332068f},
    {-11126.220703f, -1631.437134f, 0.597257f, 5.980013f},
};


enum AUGH_P1_ENUM
{
    ACTION_AUGH_P1_RANDOM_ATTACK = 10,
};

class boss_lockmaw : public CreatureScript
{
public:
    boss_lockmaw() : CreatureScript("boss_lockmaw") {}

    struct boss_lockmawAI : public BossAI
    {
        boss_lockmawAI(Creature* creature) : BossAI(creature, DATA_LOCKMAW_EVENT), summons(me)
        {
            instance = creature->GetInstanceScript();
            me->DisableMovementFlagUpdate(true);
        }

        void Reset()
        {
            _Reset();
            instance->SetData(DATA_LOCKMAW_EVENT, NOT_STARTED);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            events.Reset();
            summons.DespawnAll();
            me->DeleteThreatList();
            if (auto augh = me->FindNearestCreature(NPC_AUGH_P1, 500.0, true))
                augh->DespawnOrUnsummon();
            Rage = false;
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
            case 43650: // dust trigger
                summon->SetReactState(REACT_PASSIVE);
                summon->AddAura(81646, summon);
                break;
            case NPC_AUGH_P1:
            case NPC_AUGH_P2:
                summon->setFaction(me->getFaction());
                break;
            default:
                summon->SetInCombatWithZone();
                break;
            }
            summons.Summon(summon);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            summons.DespawnAll();
            instance->SetData(DATA_LOCKMAW_EVENT, DONE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            if (IsHeroic())
            {
                auto i = urand(0, 7);

                    if (auto augh = me->FindNearestCreature(NPC_AUGH_P2, 500.0f, true))
                    {
                        augh->NearTeleportTo(augh_pt1_spawns[i]);
                        augh->UpdateObjectVisibility();
                        augh->setFaction(me->getFaction());
                        augh->AI()->DoAction(ACTION_START_EVENT);
                        Talk(2, NULL, CHAT_MSG_RAID_BOSS_EMOTE, TEXT_RANGE_MAP);
                    }
            }
            else
                instance->SetData(DATA_AUGH_EVENT, DONE);

            if (auto augh = me->FindNearestCreature(NPC_AUGH_P1, 500.0, true))
                augh->DespawnOrUnsummon();

            std::list<Creature*> eventCr;
            me->GetCreatureListWithEntryInGrid(eventCr, NPC_FRENZIED_CROCOLISK, 200.0f);
            me->GetCreatureListWithEntryInGrid(eventCr, 45124/*Croc spawn trigger*/, 200.0f);
            for (auto itr = eventCr.begin(); itr != eventCr.end(); ++itr)
                if (auto c = (*itr))
                    c->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* /*Who*/)
        {
            _EnterCombat();
            DoZoneInCombat();
            events.ScheduleEvent(EVENT_DUST_FLAIL, urand(15000, 22000));
            events.ScheduleEvent(EVENT_VISCOUS_POISON, 12000);
            events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 30000);
            events.ScheduleEvent(EVENT_FORCE_AGGRO_CROCS, 5000);
            events.ScheduleEvent(EVENT_SUMMON_AUGH, 5000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            instance->SetData(DATA_LOCKMAW_EVENT, IN_PROGRESS);
        }

        void DamageTaken(Unit* /*target*/, uint32& damage)
        {
            if (HealthBelowPct(30) && Rage == false)
            {
                Rage = true;
                DoCast(me, SPELL_VENOMOUS_RAGE);
            }
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case ACTION_START_EVENT:
                events.ScheduleEvent(EVENT_SUMMON_AUGH, 16000);
                break;
            default:
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            std::stringstream HackReport;
            if (!UpdateVictim())
                return;


            if (me->HasUnitState(UNIT_STATE_CASTING))
                if (me->HasReactState(REACT_AGGRESSIVE))
                    return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DUST_FLAIL_PREP:
                    //me->SetOrientation(tailOrientation);
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {

                    }
                    me->ClearUnitState(UNIT_STATE_ROTATING);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    me->SetOrientation(tailOrientation);
                    /*
                    HackReport << "2. My Orientation after Lock: " << me->GetOrientation();
                    sWorld->SendGMText(LANG_GM_BROADCAST, HackReport.str().c_str());
                    */
                    break;
                case EVENT_DUST_FLAIL:
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    me->SetReactState(REACT_PASSIVE);
                    /*
                        CASTING THE FUCKING SPELL
                    */
                    //DoCast(SPELL_DUST_FLAIL);
                    //DoCastVictim(SPELL_DUST_FLAIL);
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f, true))
                    {
                        tailOrientation = me->GetAngle(target);
                        me->AttackStop();
                        me->StopMoving();
                        me->CastSpell(target, SPELL_DUST_FLAIL);
                        me->SetOrientation(me->GetAngle(target));
                        me->SetInFront(target);
                        me->SetFacingTo(me->GetOrientation());
                        /*
                        HackReport << "1. Orientation to target: " << me->GetAngle(target);
                        sWorld->SendGMText(LANG_GM_BROADCAST, HackReport.str().c_str());
                        */
                    }
                    /*
                        SETTING ORIENTATION
                    */
                    /*
                        LOCKING ORIENTATION
                    */
                    events.ScheduleEvent(EVENT_DUST_FLAIL_PREP, 1);
                    /*
                        Scheduling next events
                    */
                    events.ScheduleEvent(EVENT_DUST_FLAIL_RECOVER, 6000);
                    events.ScheduleEvent(EVENT_DUST_FLAIL, urand(15000, 22000));
                    break;
                case EVENT_DUST_FLAIL_RECOVER:
                    /*
                    HackReport << "3. My Orientation after finishing: " << me->GetOrientation();
                    sWorld->SendGMText(LANG_GM_BROADCAST, HackReport.str().c_str());
                    */

                    me->SetReactState(REACT_AGGRESSIVE);
                    me->AddUnitState(UNIT_STATE_ROTATING);
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    UpdateVictim();
                    break;
                case EVENT_VISCOUS_POISON:
                    DoCastVictim(SPELL_VISCOUS_POISON);
                    events.ScheduleEvent(EVENT_VISCOUS_POISON, 12000);
                    break;
                case EVENT_SCENT_OF_BLOOD:
                {
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                        me->CastCustomSpell(SPELL_SCENT_OF_BLOOD, SPELLVALUE_MAX_TARGETS, 1, target, false);

                    std::list<Creature*> stalker;
                    me->GetCreatureListWithEntryInGrid(stalker, NPC_ADD_STALKER, 200.0f);
                    std::vector<Creature*> random_shuffle_stacker_container;
                    for (std::list<Creature*>::iterator itr = stalker.begin(); itr != stalker.end(); ++itr)
                        random_shuffle_stacker_container.push_back(*itr);
                    std::random_shuffle(random_shuffle_stacker_container.begin(), random_shuffle_stacker_container.end());
                    for (uint32 cnt = 0; cnt < 4 && cnt < random_shuffle_stacker_container.size(); cnt++)
                        random_shuffle_stacker_container[cnt]->CastSpell(random_shuffle_stacker_container[cnt], SPELL_SUMMON_CROCOLISK);
                    events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 30000);
                    break;
                }
                case EVENT_SUMMON_AUGH: // This event has a major flaw.
                {
                    // if (auto i = urand(0, 7)) | 1-8 Chance to fail due to "if (0)"
                    // if (auto i = 0)           | if 0 will never be true
                    auto i = urand(0, 7);
                        if (auto Augh = me->SummonCreature(NPC_AUGH_P1, augh_pt1_spawns[i], TEMPSUMMON_MANUAL_DESPAWN))
                            Augh->AI()->DoAction(ACTION_AUGH_P1_RANDOM_ATTACK);
                    break;
                }
                case EVENT_FORCE_AGGRO_CROCS:
                {
                    std::list<Creature*> eventCr;
                    me->GetCreatureListWithEntryInGrid(eventCr, NPC_FRENZIED_CROCOLISK, 200.0f);
                    for (auto itr = eventCr.begin(); itr != eventCr.end(); ++itr)
                        if (auto c = (*itr))
                            if (!c->isInCombat())
                                if (c->isAlive())
                                    c->SetInCombatWithZone();
                    events.ScheduleEvent(EVENT_FORCE_AGGRO_CROCS, 5000);
                }
                    break;
                default:
                    break;
                }
            }

            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }
    private:
        float tailOrientation;
        InstanceScript* instance;
        EventMap events;
        SummonList summons;
        bool Rage;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lockmawAI(creature);
    }
};

class npc_crosilik : public CreatureScript
{
public:
    npc_crosilik() : CreatureScript("npc_crosilik") { }

    struct npc_crosilikAI : public ScriptedAI
    {
        npc_crosilikAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            vicious = 10000;
            searchTimer = 1000;
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_EVENT_ACROCALYPSE);
        }

        void IsSummonedBy(Unit* /*who*/)
        {
            if (auto lockmaw = me->FindNearestCreature(BOSS_LOCKMAW, 40.f, true))
                lockmaw->AI()->JustSummoned(me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (searchTimer <= diff)
            {
                if (auto target = SelectTarget(SELECT_TARGET_NEAREST, 0, 0.0f, true, SPELL_SCENT_OF_BLOOD))
                {
                    DoResetThreat();
                    me->AddThreat(target, 1000000.0f);
                    me->AI()->AttackStart(target);
                    me->GetMotionMaster()->MoveFollow(target, 3.f, frand(0, M_PI * 2));
                }
                searchTimer = 1000;
            }
            else searchTimer -= diff;

            if (vicious <= diff)
            {
                if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                    DoCast(target, SPELL_VICIOUS);
                vicious = 10000;
            }
            else vicious -= diff;

            DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        uint32 vicious;
        uint32 searchTimer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crosilikAI(creature);
    }
};

class npc_augh_fake : public CreatureScript
{
public:
    npc_augh_fake() : CreatureScript("npc_augh_fake") { }

    struct npc_augh_fakeAI : public ScriptedAI
    {
        npc_augh_fakeAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
        }
        void EnterCombat(Unit* /*who*/)
        {
            Talk(0);
        }
        void DamageTaken(Unit* /*target*/, uint32& damage)
        {
            if (me->HasReactState(REACT_PASSIVE))
                return;
            if (HealthBelowPct(80))
            {
                //npc 
                // spell 84768

                if (auto a = me->SummonCreature(54638, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto b = me->SummonCreature(54638, me->GetPositionX()+5.f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto c = me->SummonCreature(54638, me->GetPositionX(), me->GetPositionY()+5.f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto d = me->SummonCreature(54638, me->GetPositionX()+5.f, me->GetPositionY()+5.f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto e = me->SummonCreature(54638, me->GetPositionX()-5.f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto f = me->SummonCreature(54638, me->GetPositionX(), me->GetPositionY()-5.f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                if (auto g = me->SummonCreature(54638, me->GetPositionX()-5.f, me->GetPositionY()-5.f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                {
                    a->CastSpell(a, 84768, true);
                    b->CastSpell(b, 84768, true);
                    c->CastSpell(c, 84768, true);
                    d->CastSpell(d, 84768, true);
                    e->CastSpell(e, 84768, true);
                    f->CastSpell(f, 84768, true);
                    g->CastSpell(g, 84768, true);
                }
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_PLAY_DEATH_ANIM);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                Talk(1);
                if (auto cr = me->FindNearestCreature(43614, 1000.f))
                    me->GetMotionMaster()->MoveChase(cr);
                events.ScheduleEvent(1, 2000);
            }
        }


        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case 1:
                        me->SetPhaseMask(2, true);
                        me->DespawnOrUnsummon(1000);
                        break;
                    default:
                        break;
                }

            if (!UpdateVictim())
                return;

            if (!me->HasReactState(REACT_PASSIVE))
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_augh_fakeAI(creature);
    }
};

class npc_augh_p1 : public CreatureScript
{
public:
    npc_augh_p1() : CreatureScript("npc_augh_p1") {}

    struct npc_augh_p1AI : public ScriptedAI
    {
        npc_augh_p1AI(Creature* creature) : ScriptedAI(creature)//49045 gets spawned on death, 45378 gets spawned during fight to be a nuisance
        {
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void JustReachedHome()
        {
            if (auto lockmaw = me->FindNearestCreature(BOSS_LOCKMAW, 500.0, true))
                lockmaw->AI()->DoAction(ACTION_START_EVENT);
            me->DespawnOrUnsummon();
        }

        void DamageTaken(Unit* /*target*/, uint32& damage)
        {
            damage = 0;
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case ACTION_AUGH_P1_RANDOM_ATTACK:
            {
                auto i = (urand(0, 3) + 12);
                //TC_LOG_ERROR("sql.sql", "AUGH SPAWN TO PERFORM ACTION %u", i);
                if (auto p = me->FindNearestPlayer(100.f))
                {
                    me->AddThreat(p, 10000.f);
                    me->AI()->AttackStart(p);
                }
                events.ScheduleEvent(i, 1000);
                events.ScheduleEvent(EVENT_STEALTH, 30000);
                break;
            }
            default:
                break;
            }
        }

        void IsSummonedBy(Unit* /*who*/)
        {
            me->SetReactState(REACT_PASSIVE);
        }
        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
            if (auto p = me->FindNearestPlayer(150.f))
                me->GetMotionMaster()->MoveChase(p);
        }

        void EnterCombat(Unit* /*who*/)
        {
        }

        void OwnerAttackedBy(Unit* /*attacker*/)
        {
        }

        void JustDied(Unit* /*killer*/)
        {
        }

        void MovementInform(uint32 type, uint32 id) override
        {            
        }

        bool CanAIAttack(const Unit* /*who*/) const
        {
            return false;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (!engaged)
            {

                if (auto v = me->getVictim())
                    if (me->GetDistance(v) < 10.f)
                    {
                        engaged = true;
                    }
                    else return;
            }
            else
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_PARALYTIC_BLOW_DART:
                    //TC_LOG_ERROR("sql.sql", "AUGH_PT1 DoAction %u", act);
                    if (auto p = me->FindNearestPlayer(100.f))
                        me->CastCustomSpell(SPELL_PARALYTIC_BLOW_DART, SPELLVALUE_MAX_TARGETS, 1, p, false);
                    Talk(1);
                        events.CancelEvent(EVENT_STEALTH);
                        events.ScheduleEvent(EVENT_STEALTH, 100);
                    break;
                case EVENT_CLOUD:
                    //TC_LOG_ERROR("sql.sql", "AUGH_PT1 DoAction %u", act);
                    if (auto p = me->FindNearestPlayer(100.f))
                        DoCast(me, SPELL_CLOUD);
                    events.CancelEvent(EVENT_STEALTH);
                    events.ScheduleEvent(EVENT_STEALTH, 100);
                    break;
                case EVENT_WHIRLWIND:
                    //TC_LOG_ERROR("sql.sql", "AUGH_PT1 DoAction %u", act);
                    me->AddAura(SPELL_WHIRLWIND, me);
                    if (auto p = me->FindNearestPlayer(20.f))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveAroundPoint(*p, (me->GetDistance(p) * 0.6f), urand(0, 1), 30, 0.f, true);
                    }
                    events.CancelEvent(EVENT_STEALTH);
                    events.ScheduleEvent(EVENT_STEALTH, 10000);
                    break;

                case EVENT_FIRE_DRAGON:
                        DoCastVictim(SPELL_FIRE_DRAGON);
                        events.CancelEvent(EVENT_STEALTH);
                        events.ScheduleEvent(EVENT_STEALTH, 5000);
                case EVENT_STEALTH:
                    me->StopMoving();
                    me->DeleteThreatList();
                    me->CastStop();
                    me->RemoveAllAuras();

                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->GetMotionMaster()->MovePoint(EVENT_STEALTH, me->GetHomePosition(), true, 5.f);
                    break;
                default:
                    break;
                }
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                UpdateVictim();
            //do not melee attack as this augh.
        }
    private:
        uint32 currentEvent {0};
        EventMap events;
        InstanceScript* instance;
        bool engaged{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_augh_p1AI(creature);
    }
};

class npc_augh_p2 : public CreatureScript
{
public:
    npc_augh_p2() : CreatureScript("npc_augh_p2") {}

    struct npc_augh_p2AI : public BossAI
    {
        npc_augh_p2AI(Creature* creature) : BossAI(creature, DATA_AUGH_EVENT), summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(false);
            if (!IsHeroic())
                me->DespawnOrUnsummon();

            if (instance)
            {
                if (IsHeroic())
                if (instance->GetBossState(DATA_LOCKMAW_EVENT) == DONE || instance->GetBossState(DATA_LOCKMAW_EVENT) == DONE_HM)
                    if (instance->GetBossState(DATA_AUGH_EVENT) != DONE && instance->GetBossState(DATA_AUGH_EVENT) != DONE_HM)
                    {
                        DoAction(ACTION_START_EVENT);
                    }
            }
        }
        Position AughFightStartPos = { -11068.302734f, -1660.379272f, 0.757583f, 0.465655f };

        void Reset()
        {
            _Reset();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            events.Reset();
            summons.DespawnAll();
        }

        void JustReachedHome()
        {
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case ACTION_START_EVENT:
                if (IsHeroic())
                {
                    _cureEvent = RAND(EVENT_PARALYTIC_BLOW_DART, EVENT_WHIRLWIND);
                    events.ScheduleEvent(EVENT_RP_INTRO_0, 2000);
                }
                else
                    me->DespawnOrUnsummon();
                break;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            DoCast(me, SPELL_FRENZY);
            events.ScheduleEvent(EVENT_FIRE_DRAGON, 35000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            instance->SetData(DATA_AUGH_EVENT, IN_PROGRESS);
            events.ScheduleEvent(EVENT_CLOUD, 14000);
            events.ScheduleEvent(EVENT_WHIRLWIND, 25000);
            events.ScheduleEvent(EVENT_PARALYTIC_BLOW_DART, 6000);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            instance->SetData(DATA_AUGH_EVENT, DONE);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                if (id == EVENT_RP_INTRO_0)
                {
                    events.ScheduleEvent(EVENT_RP_INTRO_1, 500);
                }
        }

        bool CanAIAttack(const Unit* /*who*/) const
        {
            return true;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_RP_INTRO_0:
                    me->SetVisible(true);
                    me->GetMotionMaster()->MovePoint(EVENT_RP_INTRO_0, AughFightStartPos);
                    events.ScheduleEvent(EVENT_ANNOUNCE_APPEAR, 1000);
                    break;
                case EVENT_ANNOUNCE_APPEAR:
                    //Talk(0, NULL, CHAT_MSG_RAID_BOSS_EMOTE, TEXT_RANGE_ZONE);
                    break;
                case EVENT_RP_INTRO_1:
                    me->SetHomePosition(AughFightStartPos);
                    if (auto lockmaw = me->FindNearestCreature(BOSS_LOCKMAW, 100.f, false))
                        me->SetFacingTo(me->GetAngle(lockmaw));
                    Talk(1);
                    events.ScheduleEvent(EVENT_RP_INTRO_2, 5000);
                    break;
                case EVENT_RP_INTRO_2:
                    Talk(2);
                    events.ScheduleEvent(EVENT_RP_INTRO_3, 5000);
                    break;
                case EVENT_RP_INTRO_3:
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    break;

                    //Fight
                case EVENT_PARALYTIC_BLOW_DART:
                    DoCastAOE(SPELL_RANDOM_AGGRO);
                    if (auto target = me->getVictim())
                    {
                        me->CastCustomSpell(SPELL_PARALYTIC_BLOW_DART, SPELLVALUE_MAX_TARGETS, 1, target, false);
                    }
                    events.ScheduleEvent(EVENT_PARALYTIC_BLOW_DART, 25000);
                    break;
                case EVENT_CLOUD:
                    DoCastVictim(SPELL_CLOUD);
                    events.ScheduleEvent(EVENT_CLOUD, 10000);
                    break;
                case EVENT_FIRE_DRAGON:
                    if (!me->HasAura(SPELL_WHIRLWIND))
                    {
                        DoCastVictim(SPELL_FIRE_DRAGON);
                    }
                    events.ScheduleEvent(EVENT_FIRE_DRAGON, 36000);
                    break;
                case EVENT_WHIRLWIND:
                    DoCastAOE(SPELL_RANDOM_AGGRO);
                    me->AddAura(SPELL_WHIRLWIND, me);
                    events.ScheduleEvent(EVENT_WHIRLWIND, 45000);
                    break;
                default:
                    break;
                }
            }
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }
    private:
        SummonList summons;
        EventMap events;
        InstanceScript* instance;
        Events _cureEvent;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_augh_p2AI(creature);
    }
};

void AddSC_boss_lockmaw()
{
    new boss_lockmaw();
    new npc_crosilik();
    new npc_augh_fake();
    new npc_augh_p1();
    new npc_augh_p2();
}
