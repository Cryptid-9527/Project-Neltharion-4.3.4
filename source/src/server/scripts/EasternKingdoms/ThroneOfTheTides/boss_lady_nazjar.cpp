
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "throne_of_the_tides.h"

#define SPELL_WATERSPOUT_SUMMON DUNGEON_MODE(90495,90497) // summons tornado every 7/3 secs.

enum Spells
{
    // Boss
    SPELL_FUNGAL_SPORES         = 76001,
    SPELL_SHOCK_BLAST           = 76008,
    SPELL_SUMMON_GEYSER         = 75722,
    SPELL_SUMMON_WATERSPOUT     = 90471,
    SPELL_WATERSPOUT            = 75683,
    SPELL_GEYSER_START          = 75699,
    SPELL_GEYSER_END            = 75700,

    // Naz'jar Honor Guard
    SPELL_ARC_SLASH             = 75907,
    SPELL_ENRAGE                = 75998,

    // Naz'jar Tempest Witch
    SPELL_CHAIN_LIGHTNING       = 75813,
    SPELL_LIGHTNING_SURGE       = 75992,

    // Others

    // GEYSERIT
    SPELL_GEYSERIT_UNK1         = 90444,
    SPELL_GEYSERIT_UNK2         = 90484,
    SPELL_GEYSERIT_UNK3         = 90461,
    SPELL_GEYSERIT_TORNADE_AURA = 90440,
    SPELL_EJECT_PASSENGER       = 63109,
    SPELL_SUMMON_GEYSERIT       = 90471,
};
enum Yells
{
    SAY_AGGRO                   = -1643001,
    SAY_66_PRECENT              = -1643002,
    SAY_33_PRECENT              = -1643003,
    SAY_DEATH                   = -1643004,
    SAY_KILL_1                  = -1643005,
    SAY_KILL_2                  = -1643006,
};
enum Phases
{
    PHASE_ALL                   = 0,
    PHASE_NORMAL                = 1,
    PHASE_WATERSPOUT            = 2,
};

enum LADY_EVENTS
{
    EVENT_WATERSPOUT_INTERMISSION = 1,
    EVENT_END_WATERSPOUT,
    EVENT_FUNGAL_SPORES,
    EVENT_CHECK_WIPE,
    EVENT_SHOCK_BLAST,
    EVENT_SUMMON_GEYSER,
};

enum Events
{
    // Naz'jar Honor Guard
    EVENT_ARC_SLASH             = 1,

    // Naz'jar Tempest Witch
    EVENT_CHAIN_LIGHTNING,
    EVENT_LIGHTNING_SURGE,
};
Position const summonPOS[6] =
{
    { 192.330444f, 843.380005f, 840.491760f, 4.714331f }, // Left Up
    { 192.533676f, 761.279663f, 840.489746f, 1.576667f }, // Right Up
    { 232.833054f, 802.102417f, 840.491760f, 3.178883f }, // Back Up
};

/*
    Lady Naz'jar - 40586(N) & 49079(H)
*/
class boss_lady_nazjar : public CreatureScript
{
public:
    boss_lady_nazjar() : CreatureScript("boss_lady_nazjar") { }

    struct boss_lady_nazjarAI : public BossAI
    {
        boss_lady_nazjarAI(Creature* creature) : BossAI(creature, DATA_LADY_NAZJAR), summons(me)
        {
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            //me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
            me->ApplySpellImmune(SPELL_GEYSER_END, IMMUNITY_ID, SPELL_GEYSER_END, true);
            me->ApplySpellImmune(94046, IMMUNITY_ID, 94046, true);
            me->ApplySpellImmune(94047, IMMUNITY_ID, 94047, true);
            instance = creature->GetInstanceScript();
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
        }


        void Reset()
        {
            RemoveSummons();
            summons.DespawnAll();
            me->DeleteThreatList();

            Phase = PHASE_NORMAL;

            SpawnCount = 3;
            PhaseCount = 0;


            me->RemoveAurasDueToSpell(SPELL_WATERSPOUT);
            me->RemoveAurasDueToSpell(SPELL_WATERSPOUT_SUMMON);

            me->GetMotionMaster()->MoveTargetedHome();

            if (instance)
                instance->SetData(DATA_LADY_NAZJAR, NOT_STARTED);

            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

            events.Reset();
            _Reset();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            switch(summon->GetEntry())
            {
            case NPC_NAZJAR_TEMPEST_WITCH_LADY:
            case NPC_NAZJAR_HONOR_GUARD:
                check_waterspout_spawns();
                break;
            }
        }

        void RemoveSummons()
        {
            if (SummonList.empty())
                return;

            for (std::list<uint64>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                if (auto temp = Unit::GetCreature(*me, *itr))
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, temp);
                    temp->DisappearAndDie();
                }
            SummonList.clear();
        }

        void DamageTaken(Unit* who, uint32& damage) override
        {
            if (me->HealthBelowPctDamaged(60, damage))
                if (Phase == PHASE_NORMAL && PhaseCount == 0)
                {
                    events.ScheduleEvent(EVENT_WATERSPOUT_INTERMISSION, 1);
                    return;
                }
            if (me->HealthBelowPctDamaged(30, damage))
                if (Phase == PHASE_NORMAL && PhaseCount == 1)
                {
                    events.ScheduleEvent(EVENT_WATERSPOUT_INTERMISSION, 1);
                    return;
                }
        }
        
        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
            case NPC_NAZJAR_TEMPEST_WITCH_LADY:
            case NPC_NAZJAR_HONOR_GUARD:
                if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    summon->AI()->AttackStart(target);
                SummonList.push_back(summon->GetGUID());
                break;
            case NPC_SUMMONED_WATERSPOUT:
                if (!IsHeroic())
                {
                    if (Vehicle* v = summon->GetVehicleKit())
                    {
                        v->RemoveAllPendingEvents();
                        v->RemoveAllPassengers();
                    }
                    summon->DespawnOrUnsummon();
                    return;
                }
                summon->AddAura(SPELL_GEYSERIT_TORNADE_AURA, summon);
                switch (urand(0, 14))
                {
                case 0 :
                    summon->GetMotionMaster()->MovePoint(42, 216.73f, 827.56f, 809.32f);
                    break;
                case 1 :
                    summon->GetMotionMaster()->MovePoint(42, 219.73f, 813.56f, 809.32f);
                    break;
                case 2 :
                    summon->GetMotionMaster()->MovePoint(42, 227.73f, 804.56f, 809.32f);
                    break;
                case 3 :
                    summon->GetMotionMaster()->MovePoint(42, 219.73f, 790.56f, 809.32f);
                    break;
                case 4 :
                    summon->GetMotionMaster()->MovePoint(42, 214.73f, 781.56f, 809.32f);
                    break;
                case 5 :
                    summon->GetMotionMaster()->MovePoint(42, 202.73f, 774.56f, 809.32f);
                    break;
                case 6 :
                    summon->GetMotionMaster()->MovePoint(42, 192.73f, 768.56f, 809.32f);
                    break;
                case 7 :
                    summon->GetMotionMaster()->MovePoint(42, 180.73f, 774.56f, 809.32f);
                    break;
                case 8 :
                    summon->GetMotionMaster()->MovePoint(42, 170.73f, 778.56f, 809.32f);
                    break;
                case 9 :
                    summon->GetMotionMaster()->MovePoint(42, 158.73f, 800.56f, 809.32f);
                    break;
                case 10 :
                    summon->GetMotionMaster()->MovePoint(42, 163.73f, 813.56f, 809.32f);
                    break;
                case 11 :
                    summon->GetMotionMaster()->MovePoint(42, 165.73f, 827.56f, 809.32f);
                    break;
                case 12 :
                    summon->GetMotionMaster()->MovePoint(42, 180.73f, 830.56f, 809.32f);
                    break;
                case 13 :
                    summon->GetMotionMaster()->MovePoint(42, 192.73f, 840.56f, 809.32f);
                    break;
                case 14 :
                    summon->GetMotionMaster()->MovePoint(42, 205.73f, 830.56f, 809.32f);
                    break;
                default :
                    break;
                }
                break;
            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            Talk(0);
            if (instance)
                instance->SetData(DATA_LADY_NAZJAR, IN_PROGRESS);
            
            events.Reset();
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
            events.ScheduleEvent(EVENT_FUNGAL_SPORES, urand(5000, 7000));
            events.ScheduleEvent(EVENT_SHOCK_BLAST, 22000);
            events.ScheduleEvent(EVENT_SUMMON_GEYSER, urand(11000, 16000));
            _EnterCombat();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            RemoveSummons();
            summons.DespawnAll();
            me->DeleteThreatList();

            Talk(3);
            if (instance)
                instance->SetData(DATA_LADY_NAZJAR, DONE);

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void check_waterspout_spawns()
        {
            if (Phase == PHASE_WATERSPOUT)
            {
                SpawnCount--;
                if (SpawnCount == 0)
                {
                    events.Reset();
                    events.ScheduleEvent(EVENT_END_WATERSPOUT, 1);
                }
            }
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                switch (spell)
                {
                case SPELL_FUNGAL_SPORES:
                case SPELL_SUMMON_GEYSER:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, spell);
                    break;
                case SPELL_SHOCK_BLAST:
                        DoCastVictim(spell);
                    break;
                default:
                    break;
                }
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

        void UpdateAI(const uint32 diff) override
        {

            uint32 spell = 0;                               //Do not touch here, Spell ID
            uint32 min_repeat = 0;                          //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                          //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                     //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FUNGAL_SPORES:
                    spell = SPELL_FUNGAL_SPORES;            //Spell ID
                    min_repeat = 5000;                      //Minimum Delay
                    max_repeat = 7000;                      //Maximum Delay
                    wait_castnext = false;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;


                case EVENT_SHOCK_BLAST:
                    spell = SPELL_SHOCK_BLAST;              //Spell ID
                    min_repeat = 12000;                     //Minimum Delay
                    max_repeat = 15000;                     //Maximum Delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;


                case EVENT_SUMMON_GEYSER:
                    spell = SPELL_SUMMON_GEYSER;            //Spell ID
                    min_repeat = 11000;                     //Minimum Delay
                    max_repeat = 16000;                     //Maximum Delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_WATERSPOUT_INTERMISSION:
                    events.Reset();
                    events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->CastStop();
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    PhaseCount++;
                    Talk(PhaseCount);
                    Phase = PHASE_WATERSPOUT;
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                    me->NearTeleportTo(homePos);
                    me->AddUnitState(UNIT_STATE_ROOT);
                    DoCast(me, SPELL_WATERSPOUT);
                    DoCast(me, IsHeroic() ? 90495 : 90497, true);
                    DoCast(me, 97574, true);

                    for (auto i = 0; i < 3; ++i)
                    if (auto mob1 = me->SummonCreature(i == 2 ? NPC_NAZJAR_HONOR_GUARD : NPC_NAZJAR_TEMPEST_WITCH_LADY,   summonPOS[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, mob1);
                        mob1->DisableMovementFlagUpdate(true);
                        mob1->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                        mob1->SetReactState(REACT_PASSIVE);
                        //mob1->DeleteThreatList();
                        mob1->SetCanFly(true);
                        mob1->SetDisableGravity(true);
                        Position jump_to(*me);
                        me->MoveBlink(jump_to, 15.f, me->GetRelativeAngleOffset(mob1));
                        mob1->CastSpell(mob1, SPELL_WATER_JUMP_VISUAL, true);
                        mob1->GetMotionMaster()->MoveJump(jump_to, 15.f, 15.f, 555);
                    }

                    events.ScheduleEvent(EVENT_END_WATERSPOUT, 60000);
                    break;

                case EVENT_END_WATERSPOUT:
                    me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                    me->RemoveAura(90495);
                    me->RemoveAura(90497);
                    me->RemoveAura(97574);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    events.Reset();
                    events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    me->RemoveAura(SPELL_WATERSPOUT);
                    SpawnCount = 3;
                    SetCombatMovement(true);
                    Phase = PHASE_NORMAL;
                    me->RemoveAurasDueToSpell(SPELL_WATERSPOUT);
                    me->RemoveAurasDueToSpell(SPELL_WATERSPOUT_SUMMON);

                    events.ScheduleEvent(EVENT_FUNGAL_SPORES,   urand(8000, 13000));
                    events.ScheduleEvent(EVENT_SHOCK_BLAST,     22000);
                    events.ScheduleEvent(EVENT_SUMMON_GEYSER,   urand(11000, 16000));
                    break;

                case EVENT_CHECK_WIPE:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        summons.DespawnAll();
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    break;
                default:
                    break;
                }

            if (me->HasReactState(REACT_AGGRESSIVE))
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            if (UpdateVictim())
            if (auto victim = me->getVictim())
            if (me->GetDistance(victim) < 6.f)
                DoMeleeAttackIfReady();
        }

    private :
        SummonList summons;
        std::list<uint64> SummonList;
        InstanceScript* instance;
        uint8 Phase;
        int8 SpawnCount;
        uint8 PhaseCount;
        uint32 Phase2EndTimer;
        Position homePos    { me->GetHomePosition() };
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new boss_lady_nazjarAI (creature);
    }
};

class npc_lady_nazjar_geyser : public CreatureScript
{
public:
    npc_lady_nazjar_geyser() : CreatureScript("npc_lady_nazjar_geyser") { }

    struct npc_lady_nazjar_geyserAI : public ScriptedAI
    {
        npc_lady_nazjar_geyserAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
        }
    private :
        InstanceScript* instance;
        uint32 mui_geyser;

        void Reset()
        {
            DoCast(me, SPELL_GEYSER_START);
            mui_geyser = 5000;
            me->ApplySpellImmune(SPELL_GEYSER_END, IMMUNITY_ID,  SPELL_GEYSER_END, true);
            me->ApplySpellImmune(94046, IMMUNITY_ID,  94046, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() != TYPEID_PLAYER)
            {
                switch (victim->GetEntry())
                {
                case NPC_NAZJAR_TEMPEST_WITCH_LADY:
                case NPC_NAZJAR_TEMPEST_WITCH_LADY_HC:
                case NPC_NAZJAR_HONOR_GUARD:
                case NPC_NAZJAR_HONOR_GUARD_HC:
                    if (IsHeroic())
                        instance->DoCompleteAchievement(HEROIC_NAZJAR_GEYSER_KILLED_TRASH);
                    break;
                default :
                    break;
                }

            }
        }

        void DamageTaken(Unit* /*doneBy*/, uint32 &damage)
        {
            damage = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (mui_geyser <= diff)
            {
                DoCast(me, SPELL_GEYSER_END);
                me->DespawnOrUnsummon(1500);
                mui_geyser = 30000;
            }
            else mui_geyser -= diff;
        }
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_lady_nazjar_geyserAI (creature);
    }
};

class npc_lady_nazjar_tornade : public CreatureScript
{
public:
    npc_lady_nazjar_tornade() : CreatureScript("npc_lady_nazjar_tornade") { }

    struct npc_lady_nazjar_tornadeAI : public ScriptedAI
    {
        npc_lady_nazjar_tornadeAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
        }

        void Reset()
        {
            enter = false;
            mui_geyser = 500;
            me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            me->SetSpeed(MOVE_RUN, 1.1f);
            me->SetSpeed(MOVE_WALK, 1.1f);
            me->SetReactState(REACT_PASSIVE);
        }

        void AttackStart(Unit *)
        {

        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            if (id == 42)
            {
                if (Vehicle* v = me->GetVehicleKit())
                    v->RemoveAllPassengers();

                me->DespawnOrUnsummon();
            }
        }

        void DamageTaken(Unit* , uint32 &damage)
        {
            damage = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (enter)
                return;
            if (mui_geyser <= diff)
            {
                if (Player *pl = me->FindNearestPlayer(2.0f))
                {
                    if (!pl->GetVehicle())
                        if (me->IsWithinDistInMap(pl, 2.0f))
                        {
                            pl->CastSpell(pl, 90479, true);
                            pl->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                            enter = true;
                        }
                }
                mui_geyser = 100;
            }
            else mui_geyser -= diff;
        }

    private :
        InstanceScript* instance;
        uint32 mui_geyser;
        bool enter;
    };

    CreatureAI* GetAI(Creature *creature) const
    {
        return new npc_lady_nazjar_tornadeAI (creature);
    }
};

/*
    Naz'jar Honor Guard - 40633(N) & 49084(H)
*/
class npc_lady_nazjar_honor_guard : public CreatureScript
{
public:
    npc_lady_nazjar_honor_guard() : CreatureScript("npc_lady_nazjar_honor_guard") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_nazjar_honor_guardAI(creature);
    }
    struct npc_lady_nazjar_honor_guardAI : public ScriptedAI
    {
        npc_lady_nazjar_honor_guardAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (!enrage && me->GetHealthPct() <= 35.0f)
            {
                enrage = true;
                DoCast(SPELL_ENRAGE);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            //TC_LOG_ERROR("sql.sql", "type: %u, id: %u", type, id);
            if (type == EFFECT_MOTION_TYPE)
                if (id == 555 || id == 554)
                {
                    me->SetDisableGravity(false);
                    me->SetCanFly(false);
                    me->SetHover(false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                    events.ScheduleEvent(EVENT_ARC_SLASH, urand(5000, 7000));
                }
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void UpdateAI(uint32 diff)
        {
            if (!getSummoner)
            {
                if (ladyNazjar = me->FindNearestCreature(BOSS_LADY_NAZJAR, 100.0f, true))
                {
                    getSummoner = true;
                }
                else
                    me->DespawnOrUnsummon();
            }
            else if(!ladyNazjar || !ladyNazjar->isInCombat() || !ladyNazjar->isAlive())
                me->DespawnOrUnsummon();


            if (events.ExecuteEvent() == EVENT_ARC_SLASH)
            {
                DoCastVictim(SPELL_ARC_SLASH);
                events.RescheduleEvent(EVENT_ARC_SLASH, urand(9000, 11000));
            }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        Creature* ladyNazjar;
        bool getSummoner = false;
        bool enrage = false;
    };
};

/*
    Naz'jar Tempest Witch - 44404(N) & 49093(H)
*/
class npc_lady_nazjar_tempest_witch : public CreatureScript
{
public:
    npc_lady_nazjar_tempest_witch() : CreatureScript("npc_lady_nazjar_tempest_witch") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lady_nazjar_tempest_witchAI(creature);
    }
    struct npc_lady_nazjar_tempest_witchAI : public ScriptedAI
    {
        npc_lady_nazjar_tempest_witchAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            //TC_LOG_ERROR("sql.sql", "type: %u, id: %u", type, id);
            if (type == EFFECT_MOTION_TYPE)
                if (id == 555)
                {
                    me->SetDisableGravity(false);
                    me->SetCanFly(false);
                    me->SetHover(false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    me->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 100);
                    events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(4000, 6000));
                }


        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
            {
                events.DelayEvent(EVENT_CHAIN_LIGHTNING, 6000);
                events.DelayEvent(EVENT_LIGHTNING_SURGE, 6000);
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (!getSummoner)
            {
                if (ladyNazjar = me->FindNearestCreature(BOSS_LADY_NAZJAR, 100.0f, true))
                {
                    getSummoner = true;
                }
                else
                    me->DespawnOrUnsummon();
            }
            else if (!ladyNazjar || !ladyNazjar->isInCombat() || !ladyNazjar->isAlive())
                me->DespawnOrUnsummon();

            if (me->GetPositionZ() < 815.0f && me->HasAura(SPELL_WATER_JUMP_VISUAL))
                me->RemoveAura(SPELL_WATER_JUMP_VISUAL);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_CHAIN_LIGHTNING:
                        DoCastRandom(SPELL_CHAIN_LIGHTNING, 28.0f);
                        events.RescheduleEvent(EVENT_CHAIN_LIGHTNING, 4000);
                        break;
                    case EVENT_LIGHTNING_SURGE:
                        DoCastRandom(SPELL_LIGHTNING_SURGE, 40.0f);
                        events.RescheduleEvent(EVENT_LIGHTNING_SURGE, urand(18000, 21000));
                        break;
                    default:
                        break;
                }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        Creature* ladyNazjar;
        bool getSummoner = false;
    };
};

class spell_lady_nazjar_Waterspout_Intermission : public SpellScriptLoader
{
public:
    spell_lady_nazjar_Waterspout_Intermission() : SpellScriptLoader("spell_lady_nazjar_Waterspout_Intermission") {}
    SpellScript* GetSpellScript() const
    {
        return new spell_lady_nazjar_Waterspout_Intermission_SpellScript();
    }
    class spell_lady_nazjar_Waterspout_Intermission_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_lady_nazjar_Waterspout_Intermission_SpellScript);
        bool Load()
        {
            return true;
        }
        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if([](WorldObject* target)
                {
                    auto myArray = 
                    {
                        BOSS_NEPTULON,
                        BOSS_LADY_NAZJAR, 
                        /*
                        NPC_NAZJAR_TEMPEST_WITCH_LADY, 
                        NPC_NAZJAR_HONOR_GUARD, 
                        */
                        NPC_SUMMONED_WATERSPOUT, 
                    };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray); //DOES INCLUDE, would be == end(myArray) to be "not includes".
                });
        }
        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_Waterspout_Intermission_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_Waterspout_Intermission_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };
};


class spell_lady_nazjar_geyser : public SpellScriptLoader
{
public:
    spell_lady_nazjar_geyser() : SpellScriptLoader("spell_lady_nazjar_geyser") {}
    SpellScript* GetSpellScript() const
    {
        return new spell_lady_nazjar_geyser_SpellScript();
    }
    class spell_lady_nazjar_geyser_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_lady_nazjar_geyser_SpellScript);
        bool Load()
        {
            return true;
        }

        //case 75700: Boss-to-hostile (TARGET_UNIT_SRC_AREA_ENEMY):
        //case 91469: Boss-to-hostile (TARGET_UNIT_SRC_AREA_ENEMY):
        void RemoveFriendlyTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if([](WorldObject* target)
                {
                    if (auto u = target->ToUnit())
                        if (u->ToTotem())
                            return true;

                    auto myArray =
                    {
                        BOSS_NEPTULON,
                        BOSS_LADY_NAZJAR,
                        NPC_NAZJAR_TEMPEST_WITCH_LADY,
                        NPC_NAZJAR_HONOR_GUARD,
                        NPC_SUMMONED_WATERSPOUT,
                    };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray); //DOES INCLUDE, would be == end(myArray) to be "not includes".
                });
        }

        //case 94046: boss-to-friendly (TARGET_UNIT_SRC_AREA_ENTRY):
        //case 94047: boss-to-friendly (TARGET_UNIT_SRC_AREA_ENTRY):
        void RemoveHostileTargets(std::list<WorldObject*>& unitList)
        {
            bool remove{ false };
            unitList.remove_if([](WorldObject* target)
                {
                    if (target->ToPlayer())
                        return true;

                    if (auto u = target->ToUnit())
                    {
                        if (u->ToTotem())
                            return true;

                        if (auto o = u->GetOwner())
                            if (o->ToPlayer())
                                return true;
                    }

                    auto myArray =
                    {
                        BOSS_NEPTULON,
                        BOSS_LADY_NAZJAR,
                        /*
                        NPC_NAZJAR_TEMPEST_WITCH_LADY,
                        NPC_NAZJAR_HONOR_GUARD,
                        */
                        NPC_SUMMONED_WATERSPOUT,
                    };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray); //DOES INCLUDE, would be == end(myArray) to be "not includes".
                });
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_geyser_SpellScript::RemoveFriendlyTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_geyser_SpellScript::RemoveFriendlyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);

            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_geyser_SpellScript::RemoveHostileTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lady_nazjar_geyser_SpellScript::RemoveHostileTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };
};


class lady_nazjar_vortex_ride_damage : public SpellScriptLoader
{
public:
    lady_nazjar_vortex_ride_damage() : SpellScriptLoader("lady_nazjar_vortex_ride_damage") { }
    class lady_nazjar_vortex_ride_damage_AuraScript : public AuraScript
    {
        PrepareAuraScript(lady_nazjar_vortex_ride_damage_AuraScript);

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (auto owner = GetOwner()->ToUnit())
            {
                owner->ExitVehicle();
                owner->RemoveAura(VEHICLE_SPELL_RIDE_HARDCODED);
            }
        }
        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(lady_nazjar_vortex_ride_damage_AuraScript::HandleOnEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new lady_nazjar_vortex_ride_damage_AuraScript();
    }
};

void AddSC_boss_lady_nazjar()
{
    new npc_lady_nazjar_geyser();
    new npc_lady_nazjar_tornade();

    new boss_lady_nazjar();
    new npc_lady_nazjar_honor_guard();
    new npc_lady_nazjar_tempest_witch();
    new spell_lady_nazjar_Waterspout_Intermission();
    new spell_lady_nazjar_geyser();
    new lady_nazjar_vortex_ride_damage();
}
