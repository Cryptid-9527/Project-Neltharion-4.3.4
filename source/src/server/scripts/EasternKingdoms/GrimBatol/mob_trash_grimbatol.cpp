
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "Creature.h"
#include "grimbatol.h"
#include "Unit.h"

enum npc_crimsonborne
{
    EVENT_CRIMSON_CHARGE = 1,
    EVENT_CRIMSON_SHOCKWAVE,
    EVENT_FIRST_CRIMSON_CHARGE,
    EVENT_BLAZING_TWILIGHT_SHIELD,
    EVENT_CORRUPTED_FLAME,
    EVENT_BURNING_CORRUPTION,
    EVENT_DISARMING_BLAZE,
    EVENT_EMPOWERING_TWILIGHT,

    SPELL_CRIMSON_CHARGE = 76404,
    SPELL_CRIMSON_SHOCKWAVE = 76409,
    SPELL_BLAZING_TWILIGHT_SHIELD = 76314,
    SPELL_CORRUPTED_FLAME = 76332,
    SPELL_BURNING_CORRUPTION = 76335,
    SPELL_DISARMING_BLAZE = 76679,
    SPELL_EMPOWERING_TWILIGHT = 76685
};
enum npc_azureborne
{
    EVENT_ARCANE_INFUSION = 1,
    EVENT_CURSE_OF_THE_AZUREBORNE,
    EVENT_TWISTED_ARCANE,
    EVENT_WARPED_TWILIGHT,
    EVENT_TWILIGHT_BOLT,
    EVENT_AZURE_BLAST,
    EVENT_CONJURE_TWISTED_VISAGE,

    SPELL_ARCANE_INFUSION = 76378,
    SPELL_CURSE_OF_THE_AZUREBORNE = 76394,
    SPELL_TWISTED_ARCANE = 76340,
    SPELL_WARPED_TWILIGHT = 76370,
    SPELL_TWILIGHT_BOLT = 76369,
    SPELL_AZURE_BLAST = 76620,
    SPELL_CONJURE_TWISTED_VISAGE = 76626
};
enum npc_enslaved
{
    EVENT_JAGGED_ROCK_SHIELD = 1,
    EVENT_WATER_INFUSED_BLADES,
    EVENT_WINDWALK,
    EVENT_WINDWALK_2,
    EVENT_WINDWALK_3,
    EVENT_WATER_BOLT,
    EVENT_CALL_LIGHTNING,
    EVENT_FLAME_SHOCK,

    SPELL_JAGGED_ROCK_SHIELD = 76014,
    SPELL_WATER_INFUSED_BLADES = 76036,
    //SPELL_WINDWALK = 76557,
    SPELL_WINDWALK = 76523,
    SPELL_WATER_BOLT = 90490,
    SPELL_CALL_LIGHTNING = 90852,
    SPELL_FLAME_SHOCK = 90846
};
enum npc_ascended
{
    EVENT_ERUPTING_FIRE = 1,
    EVENT_COUNFOUNDING_FLAMES,
    //EVENT_WINDWALK,
    EVENT_FIRST_CONFOUNDING_FLAMES,
    EVENT_FOCUSED_GEYSER,
    EVENT_WATER_SPOUT,
    EVENT_LIGHTNING_CLOUD,
    EVENT_PETRIFIED_SKIN,
    EVENT_ROCK_SMASH,
    EVENT_BURNING_FISTS,

    SPELL_ERUPTING_FIRE = 76517,
    SPELL_COUNFOUNDING_FLAMES = 76514,
    //SPELL_WINDWALK = 76557,
    SPELL_FOCUSED_GEYSER = 76797,
    SPELL_WATER_SPOUT = 76794,
    SPELL_LIGHTNING_CLOUD = 76097,
    SPELL_PETRIFIED_SKIN = 76792,
    SPELL_ROCK_SMASH = 76779,
    SPELL_BURNING_FISTS = 76086
};
enum beguiler
{
    EVENT_BEGUILE = 1,
    EVENT_CHAINED_MIND,
    EVENT_DECEITFUL_BLAST,
    EVENT_RP_BEGUILE,
    EVENT_RP_CHAIN_MIND,

    SPELL_BEGUILE = 90719,
    SPELL_CHAINED_MIND = 76711,
    SPELL_DECEITFUL_BLAST = 90712,
    SPELL_KNOCK_ABUSER = 76138,

    CREATURE_BEGUILER = 40167,
    SPELL_LEFT_HAND_CHAIN = 74884,
    SPELL_RIGHT_HAND_CHAIN = 74867,
    SPELL_CRUNCH_ARMOR = 76703,

    SPELL_MEAT_GRINDER = 76411,
    SPELL_DIZZY = 76415,
};
enum brutes
{
    BEG_LEFT = 0,
    BEG_MIDDLE = 1,
    BEG_RIGHT = 2,

    EVENT_ROLEPLAY_IDLE = 4,
    EVENT_CRUNCH_ARMOR
};
enum npc_twilight
{
    EVENT_CHAIN_LIGHTNING = 1,
    EVENT_OVERCHARGE,
    EVENT_MOLTEN_BLAST,
    EVENT_FLAME_CONDUIT,
    EVENT_WATERBOLT,
    EVENT_WATERSHELL,
    EVENT_SHADOW_BOLT,
    EVENT_SHADOW_WEAVE,
    EVENT_SIPHON_ESSENCE,
    EVENT_UMBRAL_MENDING,
    EVENT_TWILIGHT_BREATH,
    EVENT_FIRST_SIPHON,
    EVENT_ELECTRIC_BLAST,

    SPELL_CHAIN_LIGHTNING = 76578,
    SPELL_OVERCHARGE = 76579,
    SPELL_MOLTEN_BLAST = 76765,
    SPELL_FLAME_CONDUIT = 76766,
    SPELL_WATERBOLT = 76720,
    SPELL_WATERSHELL = 90522,
    SPELL_SHADOW_BOLT = 76416,
    SPELL_SHADOW_WEAVE = 90673,
    SPELL_SIPHON_ESSENCE = 75755,
    SPELL_UMBRAL_MENDING = 75763,
    SPELL_TWILIGHT_BREATH = 76817,
    SPELL_ELECTRIC_BLAST = 82973

};
enum Drake_IDs
{
    NPC_BATTERED_TRIGGER = 39294,
    NPC_BATTERED_DRAKE = 42571,
    NPC_BATTERED_NET = 42570,
};
enum battered_drake_events
{
    EVENT_FREEDOM_THANK = 1,
    EVENT_START_PATH_ONE,
    EVENT_START_PATH_TWO,
    EVENT_START_PATH_THREE,
    EVENT_FINISH_PATH_ONE,
    EVENT_FINISH_PATH_TWO,
    EVENT_FINISH_PATH_THREE,
    EVENT_DRAKE_RESPAWN,
};
enum battered_drake_actions
{
    ACTION_RESPAWN_DRAKE = 1,
};

/************************
** Azureborne Guardian **
*************************/
class mob_azureborne_guardian : public CreatureScript
{
public:
    mob_azureborne_guardian() : CreatureScript("mob_azureborne_guardian") {}

    struct mob_azureborne_guardianAI : public ScriptedAI
    {
        mob_azureborne_guardianAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_ARCANE_INFUSION, urand(15000, 16000));
            events.ScheduleEvent(EVENT_CURSE_OF_THE_AZUREBORNE, urand(24000, 26000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                           //Do not touch here, Spell ID
            uint32 min_repeat = 0;                      //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                      //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                 //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ARCANE_INFUSION:
                    spell = SPELL_ARCANE_INFUSION;          //Spell ID
                    min_repeat = 15000;                     //Minimum Delay
                    max_repeat = 16000;                     //Maximum Delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(me, SPELL_ARCANE_INFUSION);
                    break;

                case EVENT_CURSE_OF_THE_AZUREBORNE:
                    spell = SPELL_CURSE_OF_THE_AZUREBORNE;  //Spell ID
                    min_repeat = 24000;                     //Minimum delay
                    max_repeat = 26000;                     //Maximum delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 30.0f, true))
                        DoCast(target, SPELL_CURSE_OF_THE_AZUREBORNE);
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_ARCANE_INFUSION:
                case SPELL_CURSE_OF_THE_AZUREBORNE:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_azureborne_guardianAI(pCreature);
    }

};

/*****************************
    Crimsonborne Guardian
 ******************************/
class mob_crimsonborne_guardian : public CreatureScript
{
public:
    mob_crimsonborne_guardian() : CreatureScript("mob_crimsonborne_guardian") { }

    struct mob_crimsonborne_guardianAI : public ScriptedAI
    {
        mob_crimsonborne_guardianAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FIRST_CRIMSON_CHARGE, 100);
            events.ScheduleEvent(EVENT_CRIMSON_CHARGE, 16000);
            events.ScheduleEvent(EVENT_CRIMSON_SHOCKWAVE, urand(25000, 25000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FIRST_CRIMSON_CHARGE:
                    DoCast(76404);
                    break;

                case EVENT_CRIMSON_CHARGE:
                    spell = SPELL_CRIMSON_CHARGE;         //Spell ID
                    min_repeat = 16000;                 //Minimum Delay
                    max_repeat = 16000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.0f, true))
                        DoCast(target, SPELL_CRIMSON_CHARGE);
                    events.ScheduleEvent(EVENT_CRIMSON_CHARGE, 25000);
                    break;

                case EVENT_CRIMSON_SHOCKWAVE:
                    spell = SPELL_CRIMSON_SHOCKWAVE;  //Spell ID
                    min_repeat = 25000;                //Minimum delay
                    max_repeat = 25000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 1, 5.0f, true))
                        DoCast(target, SPELL_CRIMSON_SHOCKWAVE);
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_CRIMSON_CHARGE:
                case SPELL_CRIMSON_SHOCKWAVE:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_crimsonborne_guardianAI(pCreature);
    }

};

/*******************************
      Crimsonborne Seer
*******************************/
class mob_crimsonborne_seer : public CreatureScript
{
    // This is for Crimsonborne Seer that are located in Condemned Halls in Grim Batol.
public:
    mob_crimsonborne_seer() : CreatureScript("mob_crimsonborne_seer") {}

    struct mob_crimsonborne_seerAI : public ScriptedAI
    {
        mob_crimsonborne_seerAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/)
        {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_BLAZING_TWILIGHT_SHIELD, urand(30000, 31000));
            events.ScheduleEvent(EVENT_CORRUPTED_FLAME, urand(19000, 24000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BLAZING_TWILIGHT_SHIELD:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        Creature* target{ nullptr };
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, 39854, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39381, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39956, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39954, 20.0f);

                        for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                            if (auto c = (*itr))    //if they exist (null check)
                                if (c->isAlive())   //if they're alive
                                    if (urand(0, 1) == 0)    //50% chance to change the target to our current npc in the list
                                    {
                                        target = c; //change the target
                                    }

                        DoCast(target, SPELL_BLAZING_TWILIGHT_SHIELD);  //cast
                    }
                    break;

                case EVENT_CORRUPTED_FLAME:
                    spell = SPELL_CORRUPTED_FLAME;          //Spell ID
                    min_repeat = 19000;                     //Minimum delay
                    max_repeat = 24000;                     //Maximum delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                }
            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) <= 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_BLAZING_TWILIGHT_SHIELD:
                case SPELL_CORRUPTED_FLAME:
                case SPELL_BURNING_CORRUPTION:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_crimsonborne_seerAI(pCreature);
    }

};

/**************************************
   Crimsonborne Seer Lifebinder's Cell
 **************************************/
class mob_crimsonborne_seer_cell : public CreatureScript
{
    // This is for Crimsonborne Seer that are located in Lifebinder's Cell in Grim Batol.
public:
    mob_crimsonborne_seer_cell() : CreatureScript("mob_crimsonborne_seer_cell") {}

    struct mob_crimsonborne_seer_cellAI : public ScriptedAI
    {
        mob_crimsonborne_seer_cellAI(Creature* c) : ScriptedAI(c) {}
        void Reset()
        {
            events.Reset();
        }


        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            if (IsNormal())
                events.ScheduleEvent(EVENT_BLAZING_TWILIGHT_SHIELD, urand(30000, 31000));
            events.ScheduleEvent(EVENT_CORRUPTED_FLAME, urand(40000, 50000));
            events.ScheduleEvent(EVENT_BURNING_CORRUPTION, urand(11000, 12000));
            if (IsHeroic())
                events.ScheduleEvent(EVENT_BLAZING_TWILIGHT_SHIELD, urand(30000, 31000));
            events.ScheduleEvent(EVENT_CORRUPTED_FLAME, urand(40000, 50000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BLAZING_TWILIGHT_SHIELD:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        Creature* target{ nullptr };
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, 40448, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39909, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39873, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 40291, 20.0f);
                        GetCreatureListWithEntryInGrid(creatures, me, 39626, 20.0f);

                        for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                            if (auto c = (*itr))    //if they exist (null check)
                                if (c->isAlive())   //if they're alive
                                    if (urand(0, 1) == 0)    //50% chance to change the target to our current npc in the list
                                    {
                                        target = c; //change the target
                                    }

                        DoCast(target, SPELL_BLAZING_TWILIGHT_SHIELD);  //cast
                    }
                    break;

                case EVENT_CORRUPTED_FLAME:
                    spell = SPELL_CORRUPTED_FLAME;  //Spell ID
                    min_repeat = 40000;                //Minimum delay
                    max_repeat = 50000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_BURNING_CORRUPTION:
                    spell = SPELL_BURNING_CORRUPTION;  //Spell ID
                    min_repeat = 11000;                //Minimum delay
                    max_repeat = 12000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) <= 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_BLAZING_TWILIGHT_SHIELD:
                case SPELL_CORRUPTED_FLAME:
                case SPELL_BURNING_CORRUPTION:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_crimsonborne_seer_cellAI(pCreature);
    }

};

/*************************
     Azureborne Seer
 **************************/

class mob_azureborne_seer : public CreatureScript
{
public:
    mob_azureborne_seer() : CreatureScript("mob_azureborne_seer") {}

    struct mob_azureborne_seerAI : public ScriptedAI
    {
        mob_azureborne_seerAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_TWISTED_ARCANE, urand(15000, 16000));
            events.ScheduleEvent(EVENT_WARPED_TWILIGHT, urand(24000, 26000));
            events.ScheduleEvent(EVENT_TWILIGHT_BOLT, urand(0000, 3000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_TWISTED_ARCANE:
                    spell = SPELL_TWISTED_ARCANE;         //Spell ID
                    min_repeat = 15000;                 //Minimum Delay
                    max_repeat = 16000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_WARPED_TWILIGHT:
                    spell = SPELL_WARPED_TWILIGHT;  //Spell ID
                    min_repeat = 24000;                //Minimum delay
                    max_repeat = 26000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_TWILIGHT_BOLT:
                    spell = SPELL_TWILIGHT_BOLT;    //Spell ID
                    min_repeat = 0000;                 //Minimum delay
                    max_repeat = 3000;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_TWISTED_ARCANE:
                case SPELL_WARPED_TWILIGHT:
                case SPELL_TWILIGHT_BOLT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_azureborne_seerAI(pCreature);
    }

};

/************************
   Enslaved Water Spirit
 *************************/

class  mob_enslaved_water_spirit : public CreatureScript
{
public:
    mob_enslaved_water_spirit() : CreatureScript("mob_enslaved_water_spirit") {}

    struct mob_enslaved_water_spiritAI : public ScriptedAI
    {
        mob_enslaved_water_spiritAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }


        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_WATER_BOLT, urand(0000, 3000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                           //Do not touch here, Spell ID
            uint32 min_repeat = 0;                      //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                      //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                 //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_WATER_BOLT:
                    spell = SPELL_WATER_BOLT;           //Spell ID
                    min_repeat = 0000;                  //Minimum Delay
                    max_repeat = 3000;                  //Maximum Delay
                    wait_castnext = true;               //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_WATER_BOLT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_enslaved_water_spiritAI(pCreature);
    }

};

/*****************************
    Enslaved Rock Elemental
 ******************************/

class mob_enslaved_rock_elemental : public CreatureScript
{
public:
    mob_enslaved_rock_elemental() : CreatureScript("mob_enslaved_rock_elemental") {}

    struct mob_enslaved_rock_elementalAI : public ScriptedAI
    {
        mob_enslaved_rock_elementalAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_JAGGED_ROCK_SHIELD, 13000);
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                              //Do not touch here, Spell ID
            uint32 min_repeat = 0;                         //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                         //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                    //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_JAGGED_ROCK_SHIELD:
                    spell = SPELL_JAGGED_ROCK_SHIELD;       //Spell ID
                    min_repeat = 13000;                     //Minimum Delay
                    max_repeat = 13000;                     //Maximum Delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto pTarget = me->FindNearestCreature(39415, 20.f, true))
                        DoCast(pTarget, SPELL_JAGGED_ROCK_SHIELD);
                    break;
                }
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_JAGGED_ROCK_SHIELD:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_enslaved_rock_elementalAI(pCreature);
    }

};

/***********************
   Ascended Flameseeker
 ************************/

class mob_ascended_flameseeker : public CreatureScript
{
public:
    mob_ascended_flameseeker() : CreatureScript("mob_ascended_flameseeker") {}

    struct mob_ascended_flameseekerAI : public ScriptedAI
    {
        mob_ascended_flameseekerAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }


        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FIRST_CONFOUNDING_FLAMES, 8000);
            events.ScheduleEvent(EVENT_ERUPTING_FIRE, urand(24000, 26000));
            events.ScheduleEvent(EVENT_COUNFOUNDING_FLAMES, urand(29000, 31000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FIRST_CONFOUNDING_FLAMES:
                    if (auto pTarget = me->FindNearestCreature(39900, 20.f, true))
                        DoCast(pTarget, SPELL_COUNFOUNDING_FLAMES);
                    break;

                case EVENT_ERUPTING_FIRE:
                    spell = SPELL_ERUPTING_FIRE;         //Spell ID
                    min_repeat = 24000;                 //Minimum Delay
                    max_repeat = 26000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 2.0f, true))
                        DoCast(target, SPELL_ERUPTING_FIRE);
                    events.ScheduleEvent(EVENT_ERUPTING_FIRE, urand(24000, 26000));
                    break;

                case EVENT_COUNFOUNDING_FLAMES:
                    spell = SPELL_COUNFOUNDING_FLAMES;  //Spell ID
                    min_repeat = 29000;                //Minimum delay
                    max_repeat = 31000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 1, 20.0f, true)) // Crimson Shockwave
                        DoCast(target, SPELL_COUNFOUNDING_FLAMES);
                    events.ScheduleEvent(EVENT_COUNFOUNDING_FLAMES, urand(29000, 31000));
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_ERUPTING_FIRE:
                case SPELL_COUNFOUNDING_FLAMES:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_ascended_flameseekerAI(pCreature);
    }

};

/*********************
  Ascended Windwalker
**********************/

class mob_ascended_windwalker : public CreatureScript
{
public:
    mob_ascended_windwalker() : CreatureScript("mob_ascended_windwalker") {}

    struct mob_ascended_windwalkerAI : public ScriptedAI
    {
        mob_ascended_windwalkerAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_WATER_INFUSED_BLADES, urand(25000, 28000));
            events.ScheduleEvent(EVENT_WINDWALK, urand(15000, 20000));
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_WATER_INFUSED_BLADES:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        events.ScheduleEvent(eventId, 250);
                    else
                    {
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, 39961, 20.0f);

                        if (creatures.size())
                        {
                            for (auto c : creatures)       //go through the list of npcs
                                if (c->isAlive())   //if they're alive
                                {
                                    me->Kill(c, false, false);
                                    me->AddAura(SPELL_WATER_INFUSED_BLADES, me);
                                }
                        }
                    }
                    break;
                case EVENT_WINDWALK:
                {
                    auto list_of_players = me->GetPlayersInRange(40.f, true);

                    /*if (me->getVictim())
                        if (auto player = me->getVictim()->ToPlayer())
                            list_of_players.remove(player);*/

                    if (list_of_players.size())
                    {
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                        {
                            DoCast(random_target, SPELL_WINDWALK);
                            events.ScheduleEvent(EVENT_WINDWALK_2, 666);
                        }
                    }
                }
                break;
                case EVENT_WINDWALK_2:
                {
                    auto list_of_players = me->GetPlayersInRange(40.f, true);

                    if (list_of_players.size())
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                        {
                            DoCast(random_target, SPELL_WINDWALK);
                            events.ScheduleEvent(EVENT_WINDWALK_3, 666);
                        }
                }
                break;
                case EVENT_WINDWALK_3:
                {
                    auto list_of_players = me->GetPlayersInRange(40.f, true);

                    if (list_of_players.size())
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                        {
                            DoCast(random_target, SPELL_WINDWALK);
                            events.ScheduleEvent(EVENT_WINDWALK, urand(15000, 20000));
                        }
                }
                break;
                }
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_ascended_windwalkerAI(pCreature);
    }

};

/*****************************
       Azureborne Warlord
 ******************************/

class mob_azureborne_warlord : public CreatureScript
{
public:
    mob_azureborne_warlord() : CreatureScript("mob_azureborne_warlord") {}

    struct mob_azureborne_warlordAI : public ScriptedAI
    {
        mob_azureborne_warlordAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_AZURE_BLAST, urand(28000, 30000));
            events.ScheduleEvent(EVENT_CONJURE_TWISTED_VISAGE, urand(40000, 45000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_AZURE_BLAST:
                    spell = SPELL_AZURE_BLAST;             //Spell ID
                    min_repeat = 28000;                    //Minimum delay
                    max_repeat = 30000;                    //Maximum delay
                    wait_castnext = true;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_CONJURE_TWISTED_VISAGE:
                    spell = SPELL_CONJURE_TWISTED_VISAGE;   //Spell ID
                    min_repeat = 40000;                     //Minimum delay
                    max_repeat = 45000;                     //Maximum delay
                    wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) <= 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_AZURE_BLAST:
                case SPELL_CONJURE_TWISTED_VISAGE:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_azureborne_warlordAI(pCreature);
    }

};

/****************************************
          Crimsonborne Warlord
 *****************************************/

class mob_crimsonborne_warlord : public CreatureScript
{
public:
    mob_crimsonborne_warlord() : CreatureScript("mob_crimsonborne_warlord") {}

    struct mob_crimsonborne_warlordAI : public ScriptedAI
    {
        mob_crimsonborne_warlordAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
            case 41045:/*Empowering Twilight trigger*/
                summon->AddAura(76692, summon);
                summon->DespawnOrUnsummon(11000);
            }
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_DISARMING_BLAZE, urand(15000, 16000));
            events.ScheduleEvent(EVENT_EMPOWERING_TWILIGHT, urand(23000, 25000));
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_DISARMING_BLAZE:
                    spell = SPELL_DISARMING_BLAZE;  //Spell ID
                    min_repeat = 15000;                //Minimum delay
                    max_repeat = 16000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_EMPOWERING_TWILIGHT:
                    spell = SPELL_EMPOWERING_TWILIGHT;  //Spell ID
                    min_repeat = 23000;                //Minimum delay
                    max_repeat = 25000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    if (me->GetDistance(me->getVictim()) <= 6.f)
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_DISARMING_BLAZE:
                case SPELL_EMPOWERING_TWILIGHT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_crimsonborne_warlordAI(pCreature);
    }
};

/************************
     Twilight Beguiler
 *************************/

class npc_Twilight_Beguiler : public CreatureScript
{
public:
    npc_Twilight_Beguiler() : CreatureScript("npc_Twilight_Beguiler") { }

    struct npc_Twilight_BeguilerAI : public ScriptedAI
    {
        npc_Twilight_BeguilerAI(Creature* creature) : ScriptedAI(creature) {}

        Unit* owner;
        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            if (Spell* channel = me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                channel->cancel();
            me->ClearUnitState(UNIT_STATE_CASTING);

            /*
                Found that beguile is not used against players.
                https://www.youtube.com/watch?v=37a2bWeVeB4&t=917s&ab_channel=TotalBiscuit
                //events.ScheduleEvent(EVENT_BEGUILE, urand(25000, 45000));
            */
            events.ScheduleEvent(EVENT_DECEITFUL_BLAST, urand(3000, 5000));
            events.ScheduleEvent(EVENT_CHAINED_MIND, urand(30000, 60000));
            me->CallForHelp(20.f);
        }
        void Reset()
        {
            me->CastStop();
            events.Reset();
            me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());
        }
        void JustDied(Unit* who)
        {
            events.Reset();
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_KNOCK_ABUSER)
            {
                if (me->isInCombat())
                    return;
                events.ScheduleEvent(EVENT_RP_BEGUILE, 3500);
                owner = caster;
            }
        }
        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                DoCast(spell);
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == 8)
                switch (id)
                {
                case EVENT_RP_BEGUILE:
                    if (me->isInCombat())
                        return;

                    me->SetFacingTo((&me->GetHomePosition())->GetOrientation());
                    me->CastSpell(owner, SPELL_BEGUILE);
                    break;
                default:
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target == owner)
                if (spell->Id == SPELL_BEGUILE)
                {
                    me->CastCustomSpell(SPELL_CHAINED_MIND, SPELLVALUE_DURATION, 20000, owner);
                    owner->AddUnitState(UNIT_STATE_STUNNED);
                }
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
                case EVENT_BEGUILE:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_BEGUILE;                  //Spell ID
                        min_repeat = 30000;                     //Minimum Delay
                        max_repeat = 45000;                     //Maximum Delay
                        wait_castnext = true;                   //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        if (auto v = me->getVictim())
                            me->CastSpell(v, spell);
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_CHAINED_MIND:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_CHAINED_MIND;   //Spell ID
                        min_repeat = 10000;                //Minimum delay
                        max_repeat = 15000;                //Maximum delay
                        wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        if (auto v = me->getVictim())
                            me->CastCustomSpell(spell, SPELLVALUE_DURATION, 5000, v);

                        //DoCast(v, spell);
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_DECEITFUL_BLAST:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        spell = SPELL_DECEITFUL_BLAST;          //Spell ID
                        min_repeat = 3000;                     //Minimum delay
                        max_repeat = 5000;                     //Maximum delay
                        wait_castnext = false;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        DoCastRandom(spell, 40.f);
                        events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                    }
                    break;

                case EVENT_RP_BEGUILE:
                    me->GetMotionMaster()->MovePoint(EVENT_RP_BEGUILE, me->GetHomePosition());
                    break;
                default:
                    break;
                }

            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Twilight_BeguilerAI(creature);
    }
};

/**********************
  Enslaved Gronn Brute
 **********************/

class mob_enslaved_gronn_brute : public CreatureScript
{
public:
    mob_enslaved_gronn_brute() : CreatureScript("mob_enslaved_gronn_brute") {}

    struct mob_enslaved_gronn_bruteAI : public ScriptedAI
    {
        mob_enslaved_gronn_bruteAI(Creature* c) : ScriptedAI(c) {}

        int8 set;

        void InitializeAI() override
        {
            instance = me->GetInstanceScript();

            switch (me->GetDBTableGUIDLow())
            {
            case 194165:
                set = 0;
                break;
            case 194164:
                set = 3;
                break;
            case 194163:
                set = 6;
                break;
            default:
                set = 0;
                break;
            }

            SummonAds();
            events.ScheduleEvent(EVENT_ROLEPLAY_IDLE, urand(3000, 8000));
        }

        void Reset()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_ROLEPLAY_IDLE, 5000);
            me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());
        }

        void EnterCombat(Unit* who)
        {
            events.ScheduleEvent(EVENT_CRUNCH_ARMOR, urand(8000, 13000));
            me->ClearUnitState(UNIT_STATE_CASTING);
            me->RemoveAurasDueToSpell(SPELL_CHAINED_MIND);
            me->RemoveAurasDueToSpell(SPELL_LEFT_HAND_CHAIN);
            me->RemoveAurasDueToSpell(SPELL_RIGHT_HAND_CHAIN);
            me->CallForHelp(20.f);
        }

        void SummonAds()    //summon or correct
        {
            if (me->isInCombat())
                return;

            me->SetFacingTo((&me->GetHomePosition())->GetOrientation());
            for (auto i = 0; i < 3; ++i)
                if ((BeguilerGUID[i] == NULL) || !me->GetCreature((*me), BeguilerGUID[i]))
                {
                    if (auto begger = me->SummonCreature(CREATURE_BEGUILER, BeguilarSpawns[i + set], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                    {
                        BeguilerGUID[i] = begger->GetGUID();
                        begger->SetFacingTo(begger->GetAngle(me));
                    }
                }
                else
                {
                    switch (i)
                    {
                    case 0:
                        if (auto leftChainer = me->GetCreature((*me), BeguilerGUID[i]))
                            if (leftChainer->isAlive())
                                if (!me->HasAura(SPELL_LEFT_HAND_CHAIN))
                                    leftChainer->CastSpell(me, SPELL_LEFT_HAND_CHAIN, true);
                        break;
                    case 2:
                        if (auto rightChainer = me->GetCreature((*me), BeguilerGUID[i]))
                            if (rightChainer->isAlive())
                                if (!me->HasAura(SPELL_RIGHT_HAND_CHAIN))
                                    rightChainer->CastSpell(me, SPELL_RIGHT_HAND_CHAIN, true);
                        break;
                    }
                }
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ROLEPLAY_IDLE:
                    if (!me->isInCombat())
                    {
                        SummonAds();
                        if (auto middleAbuser = me->GetCreature((*me), BeguilerGUID[1]))
                        {
                            me->RemoveAurasDueToSpell(SPELL_CHAINED_MIND);
                            me->RemoveAurasDueToSpell(SPELL_BEGUILE);
                            me->RemoveAurasDueToSpell(90715);
                            me->ClearUnitState(UNIT_STATE_STUNNED);
                            middleAbuser->ClearUnitState(UNIT_STATE_CASTING);
                            me->CastSpell(middleAbuser, SPELL_KNOCK_ABUSER);
                            //me->CastWithDelay(1000, middleAbuser, SPELL_KNOCK_ABUSER);
                        }
                        events.ScheduleEvent(EVENT_ROLEPLAY_IDLE, 26000);
                    }
                    break;
                case EVENT_CRUNCH_ARMOR:
                    //TC_LOG_ERROR("sql.sql", "EVENT_CRUNCH_ARMOR");
                    if (auto v = me->getVictim())
                        DoCast(v, IsHeroic() ? 90710 : SPELL_CRUNCH_ARMOR);
                    events.ScheduleEvent(EVENT_CRUNCH_ARMOR, urand(8000, 13000));
                    break;
                default:
                    break;
                }

            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        Position BeguilarSpawns[9]
        {
            //First, facing west
            {-546.313477f, -486.582611f, 276.598206f, 6.212964f},//left
            {-532.891235f, -481.483154f, 276.866882f, 4.677873f},//center
            {-518.366638f, -486.665344f, 276.591095f, 3.216691f},//right
            //Second, facing north
            {-489.436951f, -515.599609f, 271.908325f, 4.689333f},//left
            {-483.764069f, -530.285522f, 272.269165f, 3.091372f},//center
            {-489.784668f, -544.723511f, 271.907684f, 1.567377f},//right;
            //Third, facing east
            {-519.687378f, -576.575378f, 276.590759f, 3.047853f},//left
            {-533.153076f, -580.275940f, 276.865967f, 1.532356f},//center
            {-546.488525f, -575.184204f, 276.598206f, 0.137951f},//right
        };
    private:
        uint64 BeguilerGUID[3];
        Creature* abuser;
        std::list<Creature*> units;
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_enslaved_gronn_bruteAI(pCreature);
    }

};

/****************************
 **Enslaved Thunder Spirit
 *****************************/
class mob_enslaved_thunder_spirit : public CreatureScript
{
public:
    mob_enslaved_thunder_spirit() : CreatureScript("mob_enslaved_thunder_spirit") {}

    struct mob_enslaved_thunder_spiritAI : public ScriptedAI
    {
        mob_enslaved_thunder_spiritAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_CALL_LIGHTNING, urand(15000, 16000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                             //Do not touch here, Spell ID
            uint32 min_repeat = 0;                        //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                        //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                   //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_CALL_LIGHTNING:
                    spell = SPELL_CALL_LIGHTNING;         //Spell ID
                    min_repeat = 15000;                   //Minimum Delay
                    max_repeat = 16000;                   //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_CALL_LIGHTNING:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_enslaved_thunder_spiritAI(pCreature);
    }

};

/*****************************
 **Twilight Thundercaller
 ******************************/
class mob_twilight_thundercaller : public CreatureScript
{
public:
    mob_twilight_thundercaller() : CreatureScript("mob_twilight_thundercaller") { }


    struct mob_twilight_thundercallerAI : public ScriptedAI
    {

        mob_twilight_thundercallerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_OVERCHARGE,
            EVENT_CHAIN_LIGHTNING,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,           0,                          9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_OVERCHARGE,          SPELL_OVERCHARGE,           1000,           3000,           true,       true},
            {EVENT_CHAIN_LIGHTNING,     SPELL_CHAIN_LIGHTNING,      13000,          15000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_OVERCHARGE:     Spell_Rotation(eventId);  break;
                case EVENT_CHAIN_LIGHTNING:   Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_thundercallerAI(creature);
    }
};

/************************
 **Enslaved Burning Ember
 *************************/
class mob_enslaved_burning_ember : public CreatureScript
{
public:
    mob_enslaved_burning_ember() : CreatureScript("mob_enslaved_burning_ember") {}

    struct mob_enslaved_burning_emberAI : public ScriptedAI
    {
        mob_enslaved_burning_emberAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_ROOT);
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FLAME_SHOCK, urand(20000, 21000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FLAME_SHOCK:
                    spell = SPELL_FLAME_SHOCK;         //Spell ID
                    min_repeat = 20000;                 //Minimum Delay
                    max_repeat = 21000;                //Maximum Delay
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                        DoCast(target, SPELL_FLAME_SHOCK);
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_FLAME_SHOCK:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_enslaved_burning_emberAI(pCreature);
    }

};

/******************************
 **Ascended Waterlasher
 *******************************/
class mob_ascended_waterlasher : public CreatureScript
{
public:
    mob_ascended_waterlasher() : CreatureScript("mob_ascended_waterlasher") {}

    struct mob_ascended_waterlasherAI : public ScriptedAI
    {
        mob_ascended_waterlasherAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FOCUSED_GEYSER, urand(28000, 32000));
            events.ScheduleEvent(EVENT_WATER_SPOUT, urand(15000, 16000));
            events.ScheduleEvent(EVENT_LIGHTNING_CLOUD, urand(22000, 23000));
        }

        void UpdateAI(const uint32 diff)
        {
            {
                uint32 spell = 0;                    //Do not touch here, Spell ID
                uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
                uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
                bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                    case EVENT_LIGHTNING_CLOUD:
                        if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Creature* target{ nullptr };
                            std::list<Creature*> creatures;
                            GetCreatureListWithEntryInGrid(creatures, me, 40269, 20.0f);

                            for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                                if (auto c = (*itr))    //if they exist (null check)
                                    if (c->isAlive())   //if they're alive
                                        me->Kill(c, false, false);

                            DoCast(me, SPELL_LIGHTNING_CLOUD);  //cast
                        }
                        break;

                    case EVENT_FOCUSED_GEYSER:
                    {
                        spell = SPELL_FOCUSED_GEYSER;  //Spell ID
                        min_repeat = 28000;                //Minimum delay
                        max_repeat = 32000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

                        auto list_of_players = me->GetPlayersInRange(40.f, true);
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            DoCast(random_target, spell);

                        events.ScheduleEvent(EVENT_FOCUSED_GEYSER, urand(min_repeat, max_repeat));
                        break;
                    }
                    case EVENT_WATER_SPOUT:
                    {
                        spell = SPELL_WATER_SPOUT;  //Spell ID
                        min_repeat = 15000;                //Minimum delay
                        max_repeat = 16000;                //Maximum delay
                        wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                        auto list_of_players = me->GetPlayersInRange(40.f, true);
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            DoCast(random_target, spell);
                        events.ScheduleEvent(EVENT_WATER_SPOUT, urand(15000, 16000));
                        break;
                    }
                    }
                if (UpdateVictim())
                    if (me->GetDistance(me->getVictim()) <= 6.f)
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            DoMeleeAttackIfReady();
            }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_LIGHTNING_CLOUD:
                case SPELL_FOCUSED_GEYSER:
                case SPELL_WATER_SPOUT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_ascended_waterlasherAI(pCreature);
    }

};

/**********************
 **Ascended Rockbreaker
 **********************/
class mob_ascended_rockbreaker : public CreatureScript
{
public:
    mob_ascended_rockbreaker() : CreatureScript("mob_ascended_rockbreaker") {}

    struct mob_ascended_rockbreakerAI : ScriptedAI
    {
        mob_ascended_rockbreakerAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_PETRIFIED_SKIN, urand(10000, 16000));
            events.ScheduleEvent(EVENT_BURNING_FISTS, urand(24000, 26000));
            events.ScheduleEvent(EVENT_ROCK_SMASH, urand(5000, 8000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BURNING_FISTS:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        std::list<Creature*> creatures;
                        GetCreatureListWithEntryInGrid(creatures, me, 39892, 20.0f);

                        for (auto itr = creatures.begin(); itr != creatures.end(); ++itr)       //go through the list of npcs
                            if (auto c = (*itr))    //if they exist (null check)
                                if (c->isAlive())   //if they're alive
                                    me->Kill(c, false, false);

                        DoCast(me, SPELL_BURNING_FISTS);  //cast
                        events.ScheduleEvent(EVENT_BURNING_FISTS, urand(34000, 36000));
                    }
                    break;

                case EVENT_ROCK_SMASH:
                    spell = SPELL_ROCK_SMASH;  //Spell ID
                    min_repeat = 19000;                //Minimum delay
                    max_repeat = 21000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 1, 15.0f, true))
                        DoCast(me, SPELL_ROCK_SMASH);

                    events.ScheduleEvent(EVENT_ROCK_SMASH, urand(19000, 21000));
                    break;

                case EVENT_PETRIFIED_SKIN:
                    spell = SPELL_PETRIFIED_SKIN;  //Spell ID
                    min_repeat = 15000;                //Minimum delay
                    max_repeat = 16000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 1, 5.0f, true))
                        DoCast(me, SPELL_PETRIFIED_SKIN);
                    events.ScheduleEvent(EVENT_PETRIFIED_SKIN, urand(15000, 16000));
                    break;
                }
            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) <= 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_BURNING_FISTS:
                case SPELL_ROCK_SMASH:
                case SPELL_PETRIFIED_SKIN:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_ascended_rockbreakerAI(pCreature);
    }

};

/*********************
 **Twilight Drake
 *********************/
class mob_twilight_drake : public CreatureScript
{
public:
    mob_twilight_drake() : CreatureScript("mob_twilight_drake") {}

    struct mob_twilight_drakeAI : public ScriptedAI
    {
        mob_twilight_drakeAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            me->SetReactState(REACT_PASSIVE);
            events.ScheduleEvent(EVENT_TWILIGHT_BREATH, urand(14000, 16000));
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void IsSummonedBy(Unit* owner) override
        {
            myOwner = owner;
            me->SetHomePosition(owner->ToCreature()->GetHomePosition());
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_TWILIGHT_BREATH:
                    spell = SPELL_TWILIGHT_BREATH;  //Spell ID
                    min_repeat = 14000;                //Minimum delay
                    max_repeat = 16000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(me, SPELL_TWILIGHT_BREATH);
                    events.ScheduleEvent(EVENT_TWILIGHT_BREATH, urand(14000, 16000));
                    break;
                }
            if (UpdateVictim())
                if (me->GetDistance(me->getVictim()) <= 6.f)
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_TWILIGHT_BREATH:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            //TC_LOG_ERROR("sql.sql", "type: %u, id: %u", type, id);
            if (type == 17)
                if (id == 1)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    myOwner->ToCreature()->CallForHelp(10.f);
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
        Unit* myOwner;
        Position myOwnerPosition;
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_twilight_drakeAI(pCreature);
    }

};

/*********************
 **Twilight Wyrmcaller
 *********************/
class mob_twilight_wyrmcaller : public CreatureScript
{
public:
    mob_twilight_wyrmcaller() : CreatureScript("mob_twilight_wyrmcaller") {}

    struct mob_twilight_wyrmcallerAI : public ScriptedAI
    {
        mob_twilight_wyrmcallerAI(Creature* c) : ScriptedAI(c) {}


        void Reset()
        {
        }

        void EnterCombat(Unit* who)
        {
            if (who->GetVehicleKit())
            {
                me->DeleteThreatList();
                return;
            }

            if (!spawned_drake)
                if (spawned_drake = me->SummonCreature(41095, (me->GetDistance(Spawn1) < me->GetDistance(Spawn2) ? Spawn1 : Spawn2), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    spawned_drake->DisableMovementFlagUpdate(true);
                    spawned_drake->SetReactState(REACT_PASSIVE);
                    spawned_drake->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    spawned_drake->GetMotionMaster()->MoveTakeoff(1, FlyDestionation);
                    spawned_drake->SetCanFly(true);
                    spawned_drake->SetDisableGravity(true);
                    spawned_drake->SetHover(true);
                }
        }

        void DamageTaken(Unit* attacker, uint32& damage) override
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);

            if (me->HealthBelowPctDamaged(75, damage))
                if (!called)
                {
                    called = true;
                    Talk(0);//"Come to my aid, Drake!"
                    Position pos;
                    me->GetPosition(&pos);
                    if (spawned_drake)
                        spawned_drake->GetMotionMaster()->MoveLand(5, pos);
                }
        }
        void DoAction(const int32 act)
        {
            switch (act)
            {
            case 0:
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    private:
        Position FlyDestionation = { -570.669006f, -717.533203f, 288.904633f };
        Position Spawn1 = { -635.467346f, -706.637146f, 243.593857f };
        Position Spawn2 = { -498.601440f, -720.841125f, 245.492096f };
        Creature* spawned_drake = nullptr;
        bool called = false;
    };



    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_twilight_wyrmcallerAI(pCreature);
    }

};

/************************
 **Faceless Corruptor
 ************************/
class mob_faceless_corruptor : public CreatureScript
{

public:
    mob_faceless_corruptor() : CreatureScript("mob_faceless_corruptor") {}

    struct mob_faceless_corruptorAI : public ScriptedAI
    {
        mob_faceless_corruptorAI(Creature* c) : ScriptedAI(c) {}

        void Reset()
        {
            events.Reset();
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_FIRST_SIPHON, 100);
            events.ScheduleEvent(EVENT_SIPHON_ESSENCE, urand(15000, 16000));
            events.ScheduleEvent(EVENT_UMBRAL_MENDING, urand(22000, 24000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FIRST_SIPHON:
                    DoCast(75755);
                    break;

                case EVENT_SIPHON_ESSENCE:
                    spell = SPELL_SIPHON_ESSENCE;         //Spell ID
                    min_repeat = 15000;                 //Minimum Delay
                    max_repeat = 16000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(spell);
                    events.ScheduleEvent(EVENT_SIPHON_ESSENCE, urand(15000, 16000));
                    break;

                case EVENT_UMBRAL_MENDING:
                    spell = SPELL_UMBRAL_MENDING;  //Spell ID
                    min_repeat = 22000;                //Minimum delay
                    max_repeat = 24000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(spell);
                    events.ScheduleEvent(EVENT_UMBRAL_MENDING, urand(22000, 24000));
                    break;
                }
            if (UpdateVictim())
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_SIPHON_ESSENCE:
                case SPELL_UMBRAL_MENDING:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_faceless_corruptorAI(pCreature);
    }

};

/***********************
 **Twilight Firecatcher
 ************************/
class mob_twilight_firecatcher : public CreatureScript
{

public:
    mob_twilight_firecatcher() : CreatureScript("mob_twilight_firecatcher") { }

    struct mob_twilight_firecatcherAI : ScriptedAI
    {
        mob_twilight_firecatcherAI(Creature* creature) : ScriptedAI(creature) { hasTarget = false; }

        void Reset()
        {
            events.Reset();
        }
        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_MOLTEN_BLAST, urand(0000, 3000));
            events.ScheduleEvent(EVENT_FLAME_CONDUIT, urand(15000, 20000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FLAME_CONDUIT:
                    spell = SPELL_FLAME_CONDUIT;  //Spell ID
                    min_repeat = 15000;                //Minimum delay
                    max_repeat = 20000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 1, 25.0f, true))
                        DoCast(target, SPELL_FLAME_CONDUIT);
                    break;

                case EVENT_MOLTEN_BLAST:
                    spell = SPELL_MOLTEN_BLAST;    //Spell ID
                    min_repeat = 0000;                 //Minimum delay
                    max_repeat = 3000;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_FLAME_CONDUIT:
                case SPELL_MOLTEN_BLAST:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_firecatcherAI(creature);
    }
};

/***********************
 **Twilight Stormbreaker
 ************************/
class mob_twilight_stormbreaker : public CreatureScript
{

public:
    mob_twilight_stormbreaker() : CreatureScript("mob_twilight_stormbreaker") { }

    struct mob_twilight_stormbreakerAI : ScriptedAI
    {
        mob_twilight_stormbreakerAI(Creature* creature) : ScriptedAI(creature) { hasTarget = false; }

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_WATERBOLT, urand(0000, 3000));
            if (IsHeroic()) //heroic mode check 
                events.ScheduleEvent(EVENT_WATERSHELL, urand(16000, 18000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_WATERSHELL:
                    spell = SPELL_WATERSHELL;  //Spell ID
                    min_repeat = 16000;                //Minimum delay
                    max_repeat = 18000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_WATERBOLT:
                    spell = SPELL_WATERBOLT;    //Spell ID
                    min_repeat = 0000;                 //Minimum delay
                    max_repeat = 3000;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_WATERSHELL:
                case SPELL_WATERBOLT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_stormbreakerAI(creature);
    }
};

/************************
 **Twilight Shadow Weaver
 ************************/
class mob_twilight_shadow_weaver : public CreatureScript
{

public:
    mob_twilight_shadow_weaver() : CreatureScript("mob_twilight_shadow_weaver") { }

    struct mob_twilight_shadow_weaverAI : ScriptedAI
    {
        mob_twilight_shadow_weaverAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            events.Reset();
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(0000, 3000));
            if (IsHeroic()) //heroic mode check 
                events.ScheduleEvent(EVENT_SHADOW_WEAVE, urand(14000, 15000));
        }

        void UpdateAI(const uint32 diff)
        {
            uint32 spell = 0;                    //Do not touch here, Spell ID
            uint32 min_repeat = 0;                    //Do not touch here, minimum delay until next cast
            uint32 max_repeat = 0;                    //Do not touch here, maximum delay until next cast
            bool wait_castnext = false;                //Do not touch here, Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SHADOW_WEAVE:
                    spell = SPELL_SHADOW_WEAVE;  //Spell ID
                    min_repeat = 14000;                //Minimum delay
                    max_repeat = 15000;                //Maximum delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_SHADOW_BOLT:
                    spell = SPELL_SHADOW_BOLT;    //Spell ID
                    min_repeat = 0000;                 //Minimum delay
                    max_repeat = 3000;                 //Maximum delay
                    wait_castnext = false;                //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (target->GetGUID() != me->GetGUID())
                switch (spell->Id)
                {
                case SPELL_SHADOW_WEAVE:
                case SPELL_SHADOW_BOLT:
                    if (auto m = me->GetMotionMaster())
                    {
                        m->Clear();
                        m->MoveChase(target, 30.f, target->GetAngle(me));
                    }
                    break;
                default:
                    break;
                }
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (UpdateVictim())
            {
                if (auto victim = me->getVictim())
                    if (me->GetDistance(victim) <= sSpellMgr->GetSpellInfo(spell)->GetMaxRange())
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            me->StopMoving();
                            DoCast(spell);
                            events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
                        }
                        else
                            events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
                    else
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(victim, sSpellMgr->GetSpellInfo(spell)->GetMaxRange() * 0.75f, victim->GetAngle(me));
                        events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
                    }
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

    private:
        float followdist{ 0.f };
        bool hasTarget{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_shadow_weaverAI(creature);
    }

};

class spell_linceul_dummy : public SpellScriptLoader
{
public:
    spell_linceul_dummy() : SpellScriptLoader("spell_linceul_dummy") { }

    class spell_linceul_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_linceul_dummy_SpellScript);

        void HandleDummyHitTarget(SpellEffIndex /*effIndex*/)
        {
            if (auto caster = GetCaster())
                /*
                if (Vehicle* veh = caster->GetVehicleKit())
                    if (auto player = veh->GetPassenger(0))
                        if (auto targ = GetHitUnit())
                            if (auto target = targ->ToCreature())
                */
            {
                caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                /*
                target->AddThreat(player, 1.0f);
                target->SetInCombatWith(player);
                target->AI()->AttackStart(player);
                */
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_linceul_dummy_SpellScript::HandleDummyHitTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_linceul_dummy_SpellScript();
    }
};

const G3D::Vector3 DrakePath_1[]
{
    {-461.137f, -351.564f,  290.418f,    },
    {-483.173f, -366.196f,  291.449f,    },
    {-527.338f, -355.072f,  302.491f,    },
    {-566.240f, -356.844f,  302.728f,    },
    {-614.352f, -376.045f,  307.435f,    },
    {-655.521f, -403.462f,  319.424f,    },
    {-674.329f, -425.987f,  314.375f,    },
    {-696.906f, -465.401f,  310.880f,    },
    {-713.041f, -505.084f,  313.530f,    },
    {-712.033f, -556.492f,  317.475f,    },
    {-692.343f, -600.161f,  312.510f,    },
    {-674.868f, -637.447f,  302.960f,    },
    {-639.596f, -675.102f,  294.382f,    },
    {-591.396f, -697.736f,  291.829f,    },
    {-546.482f, -705.313f,  291.821f,    },
    {-498.449f, -700.299f,  291.257f,    },
    {-449.724f, -696.518f,  290.411f,    },
    {-441.373f, -721.189f,  287.597f,    },
    {-479.633f, -741.720f,  286.473f,    },
    {-529.005f, -745.587f,  287.417f,    },
    {-572.855f, -738.659f,  292.633f,    },
    {-641.771f, -714.798f,  297.568f,    },
    {-669.550f, -693.368f,  294.513f,    },
    {-701.863f, -654.334f,  294.058f,    },
    {-721.477f, -599.474f,  327.733f,    },
    {-734.163f, -555.336f,  327.733f,    },
    {-734.824f, -507.679f,  332.479f,    },
    {-727.445f, -464.078f,  321.185f,    },
    {-706.246f, -417.039f,  303.262f,    },
    {-676.532f, -381.075f,  299.077f,    },
    {-647.263f, -355.194f,  300.014f,    },
    {-586.960f, -324.798f,  296.879f,    },
    {-539.730f, -319.449f,  294.611f,    },
    {-503.963f, -321.051f,  292.261f,    },
    {-458.971f, -350.922f,  281.182f     }
};

const G3D::Vector3 DrakePath_2[]
{
    { -460.4f, -340.783f, 279.976f},
    { -519.857f, -324.276f, 305.887f},
    { -572.313f, -328.549f, 330.71f},
    { -635.919f, -360.439f, 342.633f},
    { -696.427f, -429.607f, 350.01f},
    { -723.951f, -514.858f, 333.581f},
    { -730.494f, -575.586f, 344.563f},
    { -726.47f, -607.165f, 356.654f},
    { -709.01f, -630.919f, 339.835f},
    { -689.251f, -622.001f, 295.172f},
    { -672.612f, -611.589f, 284.56f},
    { -646.879f, -596.941f, 277.425f}//end
};

const G3D::Vector3 DrakePath_2_rev[]
{
    { -646.879f, -596.941f, 280.425f},//end
    { -672.612f, -611.589f, 292.56f},
    { -689.251f, -622.001f, 305.172f},
    { -709.01f, -630.919f, 339.835f},
    { -726.47f, -607.165f, 356.654f},
    { -730.494f, -575.586f, 344.563f},
    { -723.951f, -514.858f, 333.581f},
    { -696.427f, -429.607f, 350.01f},
    { -635.919f, -360.439f, 342.633f},
    { -572.313f, -328.549f, 330.71f},
    { -519.857f, -324.276f, 305.887f},
    { -460.4f, -340.783f, 279.976f}
};

const G3D::Vector3 DrakePath_3[]
{
{ -475.634f, -343.809f, 277.382f },
{ -504.694f, -351.636f, 300.61f },
{ -520.992f, -343.206f, 342.358f },
{ -499.43f, -331.458f, 358.933f },
{ -458.043f, -335.969f, 372.841f },
{ -403.091f, -381.906f, 386.374f },
{ -385.85f, -443.656f, 389.45f },
{ -371.414f, -525.252f, 387.009f },
{ -358.784f, -598.213f, 314.69f },
{ -379.68f, -639.134f, 285.236f },
{ -408.643f, -673.416f, 278.032f }
};

const G3D::Vector3 DrakePath_3_rev[]
{
{ -408.643f, -673.416f, 278.032f },//final
{ -379.68f, -639.134f, 285.236f },
{ -358.784f, -598.213f, 314.69f },
{ -371.414f, -525.252f, 387.009f },
{ -385.85f, -443.656f, 389.45f },
{ -403.091f, -381.906f, 386.374f },
{ -458.043f, -335.969f, 372.841f },
{ -499.43f, -331.458f, 358.933f },
{ -520.992f, -343.206f, 342.358f },
{ -504.694f, -351.636f, 300.61f },
{ -475.634f, -343.809f, 277.382f },
};

class npc_battered_net : public CreatureScript
{
public:
    npc_battered_net() : CreatureScript("npc_battered_net") { }

    struct npc_battered_netAI : public VehicleAI
    {
        npc_battered_netAI(Creature* creature) : VehicleAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void JustDied(Unit* /*killer*/)
        {
            //myCorpseSpawner->GetAI()->DoAction(ACTION_RESPAWN_DRAKE);
            if (auto corpseSpawner = me->FindNearestCreature(NPC_BATTERED_TRIGGER, 2.0f))
            {
                Position pos;
                corpseSpawner->GetPosition(&pos);
                me->SummonCreature(NPC_BATTERED_DRAKE, pos, TEMPSUMMON_MANUAL_DESPAWN);
                corpseSpawner->SetPhaseMask(2, true);
            }
        }

    private:
        InstanceScript* instance;
    };

    VehicleAI* GetAI(Creature* creature) const
    {
        return new npc_battered_netAI(creature);
    }
};

class npc_battered_red_drake : public CreatureScript
{
public:
    npc_battered_red_drake() : CreatureScript("npc_battered_red_drake") { }

    struct npc_battered_red_drakeAI : public VehicleAI
    {
        npc_battered_red_drakeAI(Creature* creature) : VehicleAI(creature)
        {
            events.ScheduleEvent(EVENT_FREEDOM_THANK, 1000);
            instance = me->GetInstanceScript();
        }

        void PassengerBoarded(Unit* player, int8 seatId, bool apply)
        {
            if (apply)
            {
                if (rider || riderGUID)
                    if (Vehicle* kit = me->GetVehicleKit())
                    {
                        kit->RemoveAllPassengers();
                        rider = nullptr;
                        riderGUID = 0;
                        return;
                    }

                rider = player;
                riderGUID = player->GetGUID();
                uint32 DeadBosses = instance->GetDeadBossTotal(true);
                switch (DeadBosses)
                {
                case 0:
                    if (spawnedBy == NPC_BATTERED_NET && instance->GetDeadBossTotal(true) < 1)
                    {
                        events.ScheduleEvent(EVENT_START_PATH_ONE, 2000);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        Talk(1, riderGUID);
                        me->SetHover(true);
                    }
                    else
                    {
                        if (Vehicle* kit = me->GetVehicleKit())
                            kit->RemoveAllPassengers();
                    }
                    break;
                case 1://Do nothing I guess
                    if (Vehicle* kit = me->GetVehicleKit())
                        kit->RemoveAllPassengers();
                    break;
                case 2:// Through lava crevice, over rubble ""
                    events.ScheduleEvent(EVENT_START_PATH_TWO, 2000);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    Talk(4, riderGUID);
                    break;
                case 3:// Over the edge shortcut, ""
                case 4:
                    events.ScheduleEvent(EVENT_START_PATH_THREE, 2000);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    Talk(5, riderGUID);
                    break;
                default:
                    if (Vehicle* kit = me->GetVehicleKit())
                        kit->RemoveAllPassengers();
                    break;
                }
            }
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FREEDOM_THANK:
                    if (spawnedBy == NPC_BATTERED_NET && instance->GetDeadBossTotal(true) == 0)
                        Talk(0);
                    break;
                case EVENT_START_PATH_ONE:
                    currentPath = 1;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Talk(2, riderGUID);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->DisableMovementFlagUpdate(true);

                    me->GetMotionMaster()->MoveSmoothPath(DrakePath_1, 35, 19.f);
                    break;
                case EVENT_START_PATH_TWO:
                    currentPath = 2;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->DisableMovementFlagUpdate(true);

                    me->GetMotionMaster()->MoveSmoothPath(DrakePath_2, 12, 19.f);
                    break;
                case EVENT_START_PATH_THREE:
                    currentPath = 3;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->DisableMovementFlagUpdate(true);

                    me->GetMotionMaster()->MoveSmoothPath(DrakePath_3, 11, 19.f);
                    break;
                case EVENT_FINISH_PATH_ONE:
                    currentPath = 4;
                    break;
                case EVENT_FINISH_PATH_TWO:
                    currentPath = 5;
                    me->GetMotionMaster()->MoveSmoothPath(DrakePath_2_rev, 12, 19.f);
                    break;
                case EVENT_FINISH_PATH_THREE:
                    currentPath = 6;
                    me->GetMotionMaster()->MoveSmoothPath(DrakePath_3_rev, 11, 19.f);
                    break;
                default:
                    break;
                }
            }
        }

        void IsSummonedBy(Unit* owner) override
        {
            spawnedBy = owner->GetEntry();

            if (spawnedBy != NPC_BATTERED_NET || instance->GetDeadBossTotal(true) > 0)
                me->m_spells[0] = 0;
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type == SPLINE_MOTION_TYPE && point == 34 && currentPath == 1)
            {
                me->SetHover(false);
                Talk(3, riderGUID);
                me->GetMotionMaster()->MoveLand(99, me->GetHomePosition());
                if (Vehicle* kit = me->GetVehicleKit())
                    kit->RemoveAllPassengers();
                Position pos = me->GetHomePosition();
                if (auto newDrake = me->SummonCreature(NPC_BATTERED_DRAKE, pos, TEMPSUMMON_MANUAL_DESPAWN))
                    newDrake->SetPhaseMask(1, true);
                me->DespawnOrUnsummon(1500);
            }

            if (type == SPLINE_MOTION_TYPE && point == 11 && currentPath == 2)
            {
                events.ScheduleEvent(EVENT_FINISH_PATH_TWO, 0);
                if (Vehicle* kit = me->GetVehicleKit())
                    kit->RemoveAllPassengers();
                Position pos = me->GetHomePosition();
                if (auto newDrake = me->SummonCreature(NPC_BATTERED_DRAKE, pos, TEMPSUMMON_MANUAL_DESPAWN))
                    newDrake->SetPhaseMask(1, true);
                me->DespawnOrUnsummon(3000);
            }

            if (type == SPLINE_MOTION_TYPE && point == 10 && currentPath == 3)
            {
                events.ScheduleEvent(EVENT_FINISH_PATH_THREE, 0);
                if (Vehicle* kit = me->GetVehicleKit())
                    kit->RemoveAllPassengers();
                Position pos = me->GetHomePosition();
                if (auto newDrake = me->SummonCreature(NPC_BATTERED_DRAKE, pos, TEMPSUMMON_MANUAL_DESPAWN))
                    newDrake->SetPhaseMask(1, true);
                me->DespawnOrUnsummon(3000);
            }
        }

    private:
        uint32 spawnedBy;
        uint64 riderGUID;
        uint32 currentPath = NULL;
        InstanceScript* instance;
        Unit* rider = nullptr;
        EventMap events;
    };

    VehicleAI* GetAI(Creature* creature) const
    {
        return new npc_battered_red_drakeAI(creature);
    }
};

class spell_grim_batol_engulfing_flames : public SpellScriptLoader
{
public:
    spell_grim_batol_engulfing_flames() : SpellScriptLoader("spell_grim_batol_engulfing_flames") { }

    class spell_grim_batol_engulfing_flames_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_grim_batol_engulfing_flames_SpellScript);

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (auto creature = GetHitCreature())
                creature->setRegeneratingHealth(false);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_grim_batol_engulfing_flames_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_grim_batol_engulfing_flames_SpellScript();
    }
};

class spell_grim_batol_warped_twilight : public SpellScriptLoader
{
public:
    spell_grim_batol_warped_twilight() : SpellScriptLoader("spell_grim_batol_warped_twilight") { }

    class spell_grim_batol_warped_twilight_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_grim_batol_warped_twilight_SpellScript);

        void HandleDmg(SpellEffIndex effIndex)
        {
            if (auto target = GetHitUnit())
                SetHitDamage(target->GetMaxHealth() * ((float)GetSpellInfo()->Effects[0].BasePoints / 100.f));
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_grim_batol_warped_twilight_SpellScript::HandleDmg, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_grim_batol_warped_twilight_SpellScript();
    }
};

class spell_ride_drake_fireball : public SpellScriptLoader // 74039.
{
public:
    spell_ride_drake_fireball() : SpellScriptLoader("spell_ride_drake_fireball") {}

    class spell_ride_drake_fireball_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_ride_drake_fireball_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                for (std::list<WorldObject*>::iterator itr = targets.begin(); itr != targets.end();)
                {
                    if (targets.size())
                        if ((*itr))
                            if ((*itr)->ToCreature())
                                if ((*itr)->ToCreature()->GetEntry() == 39873)
                                {
                                    targets.erase(itr++);
                                }
                                else
                                {
                                    ++itr;
                                }
                }
        }


        void HandleCooldown()
        {
            if (GetSpellInfo()->Id != 74039)
                return;

            if (auto caster = GetCaster())
            {
                caster->AddSpellCooldown(74039, 0, time(NULL) + 2);
                if (Vehicle* kit = caster->GetVehicleKit())
                    if (auto casterRider = kit->GetPassenger(0))
                        if (auto casterPlayer = casterRider->ToPlayer())
                        {
                            caster->AddSpellAndCategoryCooldowns(GetSpellInfo(), NULL, GetSpell(), false);
                            casterPlayer->AddSpellAndCategoryCooldowns(GetSpellInfo(), NULL, GetSpell(), false);

                            WorldPacket data;
                            casterPlayer->BuildCooldownPacket(data, 0, 74039, 2000);
                            casterPlayer->SendDirectMessage(&data);
                            //TC_LOG_ERROR("sql.sql", "Sending player %u packet fore spell 74039 1", casterPlayer->GetGUID());
                            return;
                        }

                if (auto casterPlayer = caster->ToPlayer())
                {
                    WorldPacket data;
                    casterPlayer->BuildCooldownPacket(data, 0x3, 74039, 2000);
                    casterPlayer->SendDirectMessage(&data);
                    //TC_LOG_ERROR("sql.sql", "Sending player %u packet fore spell 74039 2", casterPlayer->GetGUID());
                }
            }
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_ride_drake_fireball_SpellScript::HandleCooldown);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ride_drake_fireball_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ride_drake_fireball_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ride_drake_fireball_SpellScript::FilterTargets, EFFECT_0, TARGET_DEST_DEST);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ride_drake_fireball_SpellScript::FilterTargets, EFFECT_1, TARGET_DEST_DEST);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_ride_drake_fireball_SpellScript();
    }
};

class spell_GB_conf_flames : public SpellScriptLoader
{
public:
    spell_GB_conf_flames() : SpellScriptLoader("spell_GB_conf_flames") { };
    class spell_GB_conf_flamesAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_GB_conf_flamesAuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto rock = GetOwner())
                if (auto caster = GetCaster())
                    if (auto rCreature = rock->ToCreature())
                        if (rock->GetEntry() == 39900)
                        {
                            rock->ToUnit()->EnterVehicle(caster);

                            rock->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
        }

        void HandleEffectPeriodic(AuraEffect const* aurEff)
        {
            if (auto caster = GetCaster())
                if (auto rock = GetOwner())
                    if (rock->GetEntry() == 39900)
                        if (auto rCreature = rock->ToCreature())
                        {
                            rock->SetObjectScale(rock->GetFloatValue(OBJECT_FIELD_SCALE_X) * 0.50f);
                        }
        }

        void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetOwner())
                if (auto rCreature = c->ToCreature())
                    if (rCreature->GetEntry() == 39900)
                    {
                        rCreature->SetVisible(false);  //will prevent clients from seeing a death anim
                        c->ToCreature()->DespawnOrUnsummon(5000);
                    }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_GB_conf_flamesAuraScript::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_GB_conf_flamesAuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);  //good
            OnEffectRemove += AuraEffectRemoveFn(spell_GB_conf_flamesAuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_GB_conf_flamesAuraScript();
    }
};

class spell_mob_shadowweaver_shadow_weave : public SpellScriptLoader
{
public:
    spell_mob_shadowweaver_shadow_weave() : SpellScriptLoader("spell_mob_shadowweaver_shadow_weave") { }

    class spell_mob_shadowweaver_shadow_weaveSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mob_shadowweaver_shadow_weaveSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }
        void HandleAfterCast()
        {
            if (auto c = GetCaster())
                c->CastSpell(c, 90678, true);
        }

        void Register()
        {
            AfterCast += SpellCastFn(spell_mob_shadowweaver_shadow_weaveSpellScript::HandleAfterCast);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_mob_shadowweaver_shadow_weaveSpellScript();
    }
};

class spell_GB_Faceless_leech : public SpellScriptLoader
{
public:
    spell_GB_Faceless_leech() : SpellScriptLoader("spell_GB_Faceless_leech") { };
    class spell_GB_Faceless_leechAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_GB_Faceless_leechAuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        void HandleEffectPeriodic(AuraEffect const* aurEff)
        {
            if (uint32 HealAmount = aurEff->GetAmount())    //Actual hit value, not just dbc numbers
                if (auto caster = GetCaster())
                    if (auto healMult = (GetSpellInfo()->Id == 75755 ? 3 : 6))      //heroic and non heroic multipliers.
                        if (auto cCreature = caster->ToCreature())
                            if (caster->GetEntry() == 39392)
                                caster->HealBySpell(caster, GetSpellInfo(), (HealAmount * healMult), false);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_GB_Faceless_leechAuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_LEECH);  //good
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_GB_Faceless_leechAuraScript();
    }
};

class spell_GB_chained_mind : public SpellScriptLoader
{
public:
    spell_GB_chained_mind() : SpellScriptLoader("spell_GB_chained_mind") { };
    class spell_GB_chained_mindAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_GB_chained_mindAuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        void HandleApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto o = GetOwner()->ToUnit())
                if (auto c = GetCaster())
                    if (o->GetEntry() != NPC_ENSLAVED_GRONN_BRUTE)
                    {
                        if (auto visible_aura = aurEff->GetBase())
                        {
                            /*
                            visible_aura->SetDuration(5000);
                            visible_aura->SetMaxDuration(5000);
                            visible_aura->SetNeedClientUpdateForTargets();
                            */

                            if (auto stun_aura = c->AddAura(78320, o))
                            {
                                stun_aura->SetDuration(visible_aura->GetDuration());
                                stun_aura->SetMaxDuration(visible_aura->GetMaxDuration());
                            }
                        }
                    }
        }

        void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto o = GetOwner()->ToUnit())
                if (auto c = GetCaster())
                    if (o->GetEntry() != NPC_ENSLAVED_GRONN_BRUTE)
                    {
                        o->RemoveAurasDueToSpell(78320, c->GetGUID());
                    }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_GB_chained_mindAuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_GB_chained_mindAuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_GB_chained_mindAuraScript();
    }
};

//Twilight War-Mage

class npc_Twilight_WarMage : public CreatureScript
{
public:
    npc_Twilight_WarMage() : CreatureScript("npc_Twilight_WarMage") { }


    struct npc_Twilight_WarMageAI : public ScriptedAI
    {

        npc_Twilight_WarMageAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }
        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_FIRE_ENCHANT,
            EVENT_ICE_ENCHANT,
            EVENT_WARMAGE_POLYMORPH,
            EVENT_ICE_BLAST,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,


            SPELL_FIRE_ENCHANT = 76822,
            SPELL_ICE_ENCHANT = 76823,
            SPELL_WARMAGE_POLYMORPH = 76826,
            SPELL_ICE_BLAST = 76825,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_FIRE_ENCHANT,        SPELL_FIRE_ENCHANT,         13000,          30000,          true,       false},
            {EVENT_ICE_ENCHANT,         SPELL_ICE_ENCHANT,          45000,          60000,           true,       false},
            {EVENT_WARMAGE_POLYMORPH,   SPELL_WARMAGE_POLYMORPH,    20000,          28000,           true,       true},
            {EVENT_ICE_BLAST,           SPELL_ICE_BLAST,            8000,           15000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FIRE_ENCHANT:        Spell_Rotation(eventId);  break;
                case EVENT_ICE_ENCHANT:         Spell_Rotation(eventId);  break;
                case EVENT_WARMAGE_POLYMORPH:   Spell_Rotation(eventId);  break;
                case EVENT_ICE_BLAST:           Spell_Rotation(eventId);  break;

                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Twilight_WarMageAI(creature);
    }
};

class npc_Twilight_Enforcer : public CreatureScript
{
public:
    npc_Twilight_Enforcer() : CreatureScript("npc_Twilight_Enforcer") { }


    struct npc_Twilight_EnforcerAI : public ScriptedAI
    {

        npc_Twilight_EnforcerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_MEAT_GRINDER,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_MEAT_GRINDER,     SPELL_MEAT_GRINDER,      10000,          15000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_MEAT_GRINDER:     Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Twilight_EnforcerAI(creature);
    }
};

class spell_GB_meat_grinder : public SpellScriptLoader
{
public:
    spell_GB_meat_grinder() : SpellScriptLoader("spell_GB_meat_grinder") { };
    class spell_GB_meat_grinderAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_GB_meat_grinderAuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }


        void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
                c->CastSpell(c, SPELL_DIZZY, true);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_GB_meat_grinderAuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_GB_meat_grinderAuraScript();
    }
};

class spell_GB_meat_grinder_Triggered : public SpellScriptLoader
{
public:
    spell_GB_meat_grinder_Triggered() : SpellScriptLoader("spell_GB_meat_grinder_Triggered") { }
    /*
    */
    class spell_GB_meat_grinder_Triggered_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_GB_meat_grinder_Triggered_SpellScript);

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if ((c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED)) || c->HasAuraWithMechanic(MECHANIC_DISARM))
                {
                    c->RemoveAurasDueToSpell(SPELL_MEAT_GRINDER);
                    c->RemoveAurasDueToSpell(90665/*Heroic Version*/);
                    return SPELL_FAILED_DONT_REPORT;
                }
            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_GB_meat_grinder_Triggered_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_GB_meat_grinder_Triggered_SpellScript();
    }
};

class npc_GB_Twilight_EarthShaper : public CreatureScript
{
public:
    npc_GB_Twilight_EarthShaper() : CreatureScript("npc_GB_Twilight_EarthShaper") { }


    struct npc_GB_Twilight_EarthShaperAI : public ScriptedAI
    {

        npc_GB_Twilight_EarthShaperAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }
        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_SUMMON_ROCK,
            EVENT_EARTH_SPIKE,
            EVENT_STONE_SKIN,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_SUMMON_ROCK_ELEMENTAL = 74552, //Summon Rock Elemental
            SPELL_EARTH_SPIKE = 76603,
            SPELL_STONE_SKIN = 76596,
        };

        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_EARTH_SPIKE,     SPELL_EARTH_SPIKE,      13000,          30000,          true,       true},
            {EVENT_STONE_SKIN,      SPELL_STONE_SKIN,    5000,           8000,           true,       false},
            {EVENT_SUMMON_ROCK,      SPELL_SUMMON_ROCK_ELEMENTAL,    5000,           8000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_EARTH_SPIKE:     Spell_Rotation(eventId);  break;
                case EVENT_STONE_SKIN:   Spell_Rotation(eventId);  break;
                case EVENT_SUMMON_ROCK:   Spell_Rotation(eventId);  break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_GB_Twilight_EarthShaperAI(creature);
    }
};

class npc_GB_Twilight_ArmsMaster : public CreatureScript
{
public:
    npc_GB_Twilight_ArmsMaster() : CreatureScript("npc_GB_Twilight_ArmsMaster") { }


    struct npc_GB_Twilight_ArmsMasterAI : public ScriptedAI
    {

        npc_GB_Twilight_ArmsMasterAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }
        void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
        {
            if (me->isRegeneratingHealth())
                me->setRegeneratingHealth(false);
        }
        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
        }
        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }
        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            for (uint8 eff = 0; eff < MAX_SPELL_EFFECTS; ++eff)
                if (spell->Effects[eff].IsEffect())
                    if (spell->GetEffectMechanic(eff) == MECHANIC_DISARM)
                    {
                        me->ClearUnitState(UNIT_STATE_CASTING);
                        me->RemoveAurasDueToSpell(SPELL_FURY_OF_BLOWS);
                        me->CastStop();
                    }
        }
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
                for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                    if (my_spells[i].delay_after_interrupted)
                        events.DelayEvent(i, 6000);
        }
        void Start_Rotation()
        {
            for (int i = EVENT_NEVER_USE; i < SPELL_EVENT_COUNT; i++)
                if (!events.HasEvent(i))
                    events.ScheduleEvent(i, (my_spells[i].wait_CastNext ? (my_spells[i].max_repeat) : (my_spells[i].min_repeat / 1.5)));
        }

        void Spell_Rotation(uint32 eventId)//execute a spell
        {

            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (auto spellInfo = sSpellMgr->GetSpellInfo(my_spells[eventId].spell_id))
                    if (spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT)
                        if (me->isMoving())
                            me->StopMoving();

                DoCast(my_spells[eventId].spell_id);
                ReAttemptSpell(eventId, true);
            }
            else
                ReAttemptSpell(eventId, false);
        }
        void ReAttemptSpell(uint32 eventId, bool reschedule)//reschedule based on the results and conditions of casting.
        {
            if (reschedule)
                events.ScheduleEvent(eventId, urand(my_spells[eventId].min_repeat, my_spells[eventId].max_repeat));
            else
                events.ScheduleEvent(eventId, 250);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/

            EVENT_FURY_OF_BLOWS,
            EVENT_MORTAL_STRIKE,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_FURY_OF_BLOWS = 76729,
            SPELL_MORTAL_STRIKE = 76727,
        };
        struct NPC_Spell_Rotation
        {
            uint32 event_id;                    //Event 
            uint32 spell_id;                    //Spell to cast
            uint32 min_repeat;                  //fastest which this npc can repeat the spell after it's been casted
            uint32 max_repeat;                  //slowest which this npc can repeat the spell after it's been casted
            bool delay_after_interrupted;       //Should this spell be delayed after a spell gets interrupted?
            bool wait_CastNext;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
        };
        const NPC_Spell_Rotation my_spells[SPELL_EVENT_COUNT]
        { {EVENT_NEVER_USE,   0,                  9999999,        9999999,        false,      false},//DO NOT TOUCH

            {EVENT_FURY_OF_BLOWS,       SPELL_FURY_OF_BLOWS,    13000,          25000,          true,       true},
            {EVENT_MORTAL_STRIKE,       SPELL_MORTAL_STRIKE,    5000,           8000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FURY_OF_BLOWS:
                    if (me->HasAuraType(SPELL_AURA_MOD_DISARM)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        Spell_Rotation(eventId);
                    }
                    break;
                case EVENT_MORTAL_STRIKE:
                    if (me->HasAuraType(SPELL_AURA_MOD_DISARM)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        Spell_Rotation(eventId);  break;
                    }
                    break;
                case EVENT_WIPE_CHECK:
                    if (checkWipe())
                    {
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        Reset();
                        me->Respawn(true);
                    }
                    else
                        events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
                    break;
                default:
                    break;
                }

            //  Enable for Melee-using npcs only
            if (me->isInCombat())
                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                        if (UpdateVictim())
                            if (use_melee)
                                DoMeleeAttackIfReady();
        }
    private:
        bool use_melee{ true };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_GB_Twilight_ArmsMasterAI(creature);
    }
};

class spell_GB_Water_Shell : public SpellScriptLoader
{
public:
    spell_GB_Water_Shell() : SpellScriptLoader("spell_GB_Water_Shell") { };
    class spell_GB_Water_ShellAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_GB_Water_ShellAuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            //TC_LOG_ERROR("sql.sql", "HandleRemove");
            if (auto caster = GetOwner()->ToUnit())
                if (auto aura = aurEff->GetBase())
                    if (!aura->IsExpired())
                    {
                        //TC_LOG_ERROR("sql.sql", "Aura faded without being expired, casting bubble");
                        int32 basepoints0 = 800;
                        caster->CastCustomSpell(caster, 78542, &basepoints0, NULL, NULL, true, NULL, NULL, caster->GetGUID());
                    }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_GB_Water_ShellAuraScript::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_GB_Water_ShellAuraScript();
    }
};

class spell_overcharge : public SpellScriptLoader
{
public:
    spell_overcharge() : SpellScriptLoader("spell_overcharge") { }
    /*
    76579, 90858
    */
    class spell_overcharge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_overcharge_SpellScript);

        void HandleEffectZero(SpellEffIndex effIndex)
        {
            if (auto target = GetHitUnit())
                if (auto caster = GetCaster())
                    if (target->GetGUID() != caster->GetGUID())
                        target->AddAura(GetSpellInfo()->Id, target);
        }

        void Register()
        {
            //OnEffectHitTarget += SpellEffectFn(spell_overcharge_SpellScript::HandleEffectZero, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_overcharge_SpellScript();
    }


    class spell_overcharge_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_overcharge_AuraScript);

        void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto target = GetOwner())
                if (auto caster = GetCaster())
                    if (target->GetGUID() != caster->GetGUID())
                        PreventDefaultAction();
        }

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            if (auto o = GetOwner())
                if (auto c = o->ToUnit())
                    if (auto* target = eventInfo.GetProcTarget())
                    {
                        if (target->HasSpellCooldown(SPELL_ELECTRIC_BLAST))
                            return;
                        c->CastSpell(target, SPELL_ELECTRIC_BLAST, true);

                        /*
                        if (auto p = c->ToPlayer())
                            p->AddSpellCooldown(SPELL_ELECTRIC_BLAST, 0, time(NULL) + 5);
                        */
                    }
        }

        void Register()
        {
            //OnEffectApply += AuraEffectApplyFn(spell_overcharge_AuraScript::ApplyEffect, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            //OnEffectApply += AuraEffectApplyFn(spell_overcharge_AuraScript::ApplyEffect, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectProc += AuraEffectProcFn(spell_overcharge_AuraScript::OnProc, EFFECT_2, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_overcharge_AuraScript();
    }
};

class spell_water_blades_instakill : public SpellScriptLoader
{
public:
    spell_water_blades_instakill() : SpellScriptLoader("spell_water_blades_instakill") {}

    class spell_water_blades_instakill_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_water_blades_instakill_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end(); ++itr)
                    if (auto t = *itr)
                        if (t->GetEntry() != 39961)
                            targets.erase(itr);
        }

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, c, 39961, 20.0f);
                if (creatures.size() >= 2)
                    return SPELL_CAST_OK;

            }

            return SPELL_FAILED_DONT_REPORT;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_water_blades_instakill_SpellScript::CheckCast);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_water_blades_instakill_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_water_blades_instakill_SpellScript();
    }
};

class spell_electric_blast : public SpellScriptLoader
{
public:
    spell_electric_blast() : SpellScriptLoader("spell_electric_blast") { }//82973, 90859

    class spell_electric_blastSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_electric_blastSpellScript);

        void CalculateDamage(SpellEffIndex /*effIndex*/)
        {
            if (auto c = GetCaster())
                if (auto damage = GetHitDamage())
                {
                    int32 charges{ 1 };

                    if (auto aura = c->GetAura(90858))
                        charges = int32(aura->GetStackAmount());

                    if (auto aura = c->GetAura(76579))
                        charges = int32(aura->GetStackAmount());

                    SetHitDamage(damage * charges);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_electric_blastSpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_electric_blastSpellScript();
    }
};


void AddSC_mob_trash_grim_batol()
{
    new mob_azureborne_guardian();
    new mob_crimsonborne_guardian();
    new mob_crimsonborne_seer();
    new mob_crimsonborne_seer_cell();
    new mob_azureborne_seer();
    new mob_enslaved_water_spirit();
    new mob_enslaved_rock_elemental();
    new mob_ascended_flameseeker();
    new mob_ascended_windwalker();
    new mob_azureborne_warlord();
    new mob_crimsonborne_warlord();
    new npc_Twilight_Beguiler();
    new mob_enslaved_gronn_brute();
    new mob_enslaved_thunder_spirit();
    new mob_twilight_thundercaller();
    new mob_enslaved_burning_ember();
    new mob_ascended_waterlasher();
    new mob_ascended_rockbreaker();
    new mob_twilight_drake();
    new mob_twilight_wyrmcaller();
    new mob_faceless_corruptor();
    new mob_twilight_firecatcher();
    new mob_twilight_stormbreaker();
    new mob_twilight_shadow_weaver();
    new npc_battered_net();
    new npc_battered_red_drake();
    new npc_Twilight_WarMage();
    new npc_Twilight_Enforcer();
    new npc_GB_Twilight_EarthShaper();
    new npc_GB_Twilight_ArmsMaster();


    new spell_linceul_dummy();
    new spell_grim_batol_engulfing_flames();
    new spell_grim_batol_warped_twilight();
    new spell_ride_drake_fireball();
    new spell_GB_conf_flames();
    new spell_mob_shadowweaver_shadow_weave();
    new spell_GB_Faceless_leech();
    new spell_GB_chained_mind();
    new spell_GB_meat_grinder();
    new spell_GB_meat_grinder_Triggered();
    new spell_GB_Water_Shell();
    new spell_water_blades_instakill();
    new spell_overcharge();
    new spell_electric_blast();
}