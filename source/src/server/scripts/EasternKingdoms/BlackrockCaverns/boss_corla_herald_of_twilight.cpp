/*
 * Copyright (C) 2013-2014 trinity core og
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
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

/*
 * TODO:
 * Script: 90% Complete
 * - Implement Netherbeam
 */

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "blackrock_caverns.h"

enum Texts
{
    TALK_AGGRO  = 0,
    TALK_ZEALOT = 1,
    TALK_SLAY   = 2,
    TALK_DEATH  = 3
};

enum Spells
{
    SPELL_TWILIGHT_EVOLUTION        = 75732,    //actual transformation
    SPELL_AURA_OF_ACCELERATION      = 75817,    //no visual, just corla's passive aoe.
    SPELL_DARK_COMMAND              = 75823,    //fear
    SPELL_KNEELING_IN_SUPPLICATION  = 75608,    //kneeling
    SPELL_DRAIN_ESSENCE_CHANNELING  = 75645,    //corla's 1-second interval slow drain

    SPELL_EVOLUTION_AURA            = 75697,    //dummy stack, scripted to evolve at 100 stacks.
    SPELL_EVOLUTION_AURA_HC         = 87378,    // seems spell difficulty is not working for aura checks
    SPELL_NETHERESSENCE_AURA        = 75649,
    SPELL_NETHERESSENCE_VISUAL_1    = 75650,
    SPELL_NETHERESSENCE_VISUAL_2    = 75653, 
    SPELL_NETHERESSENCE_VISUAL_3    = 75654,

    SPELL_NETHER_BREAM              = 75706,
    SPELL_EVOLUTION                 = 75610,
    SPELL_EVOLUTION_COSMETIC_HELIX  = 75677,

    SPELL_FORCE_BLAST               = 76522,
    SPELL_GRAVITY_STRIKE            = 76561,
    SPELL_GRIEVOUS_WHIRL            = 93658,
    SPELL_SHADOW_STRIKE             = 66134,
};

enum Events
{
    EVENT_DARK_COMMAND      = 1,
    EVENT_FORCE_BLAST       = 2,
    EVENT_GRAVITY_STRIKE    = 3,
    EVENT_GRIEVOUS_WHIRL    = 4,
    EVENT_SHADOW_STRIKE     = 5,
    EVENT_EVOLUTION_CAST    = 6,
    EVENT_NETHER_ESSENCE    = 7
};

enum Actions
{
    ACTION_TRIGGER_START_CHANNELING = 1,
    ACTION_COUNT_EVOLVE             = 2,
    ACTION_DESPAWN_ESSENCE          = 3,
};

#define ACHIEVEMENT_ARRESTED_DEVELOPMENT 5282

Position const summonPositions[3] =
{
    {561.513306f, 981.955078f, 155.353607f, 1.571635f}, // Twilight Zealot spawn positions 
    {584.655701f, 982.496094f, 155.353607f, 1.571635f},
    {573.646851f, 978.236938f, 155.353607f, 1.571635f},
};

Position const beam_control_spawns[3]       //immdiately behind the twilight zealots
{
    {561.102356f, 978.787170f, 155.354324f, 1.547330f},
    {584.423523f, 978.721130f, 155.352310f, 1.504133f},
    {573.970581f, 975.362488f, 155.353668f, 1.594006f},
};

Position const NetherEssencePos[3]          //on the drake
{
    {574.839783f, 909.387878f, 174.507294f, 1.575160f},
    {574.839783f, 909.387878f, 174.507294f, 1.575160f},
    {574.839783f, 909.387878f, 174.507294f, 1.575160f},
};

struct evolutionHelper
{
    uint8 data;
    uint8 data64;
    Position const position;
};

static const evolutionHelper positionData[3] =
{
    {DATA_SPAWN_LEFT, DATA_ZEALOT_LEFT, summonPositions[0]},
    {DATA_SPAWN_RIGHT, DATA_ZEALOT_RIGHT, summonPositions[1]},
    {DATA_SPAWN_CENTER, DATA_ZEALOT_CENTER, summonPositions[2]},
};

class boss_corla_herald_of_twilight : public CreatureScript
{
public:
    boss_corla_herald_of_twilight() : CreatureScript("boss_corla_herald_of_twilight") { }

    struct boss_corla_herald_of_twilightAI : public BossAI
    {
        boss_corla_herald_of_twilightAI(Creature* creature) : BossAI(creature, BOSS_CORLA) {}

        void Reset()
        {
            instance->SetBossState(BOSS_CORLA, NOT_STARTED);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_EVOLUTION);
            events.Reset();
            spawnZealots();
            DoCast(me, SPELL_DRAIN_ESSENCE_CHANNELING, true);
            evolvedCounter = 0;
            left = IN_PROGRESS;
            right = IN_PROGRESS;
            center = IN_PROGRESS;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (auto p = who->ToPlayer())
                if (!p->isGameMaster() && p->isGMVisible())
                if (me->IsValidAttackTarget(who))
                    if (!me->isInCombat())
                        if (me->GetDistance(who) <= 17.f)
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                AttackStart(who);
                            }
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SetBossState(BOSS_CORLA, IN_PROGRESS);
            _EnterCombat();
            Talk(TALK_AGGRO);
            me->CastStop();
            events.ScheduleEvent(EVENT_DARK_COMMAND, 10000);
            DoCastAOE(SPELL_AURA_OF_ACCELERATION);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                if (auto zealot = ObjectAccessor::GetCreature(*me, *itr))
                    if (zealot->isAlive() && (zealot->GetEntry() == NPC_TWILIGHT_ZEALOT_CORLA))
                        zealot->SetInCombatWithZone();
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (attacker->GetEntry() == NPC_RAZ_THE_CRAZED)
            {
                damage = 0;
                instance->DoOnPlayers([this](Player* player)
                    {
                        me->Kill(player, true, false);
                    });

                me->Kill(attacker, false, false);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (IsHeroic() && evolvedCounter >= 3)
                instance->DoCompleteAchievement(ACHIEVEMENT_ARRESTED_DEVELOPMENT);

            Talk(TALK_DEATH);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _JustDied();
        }

        void KilledUnit(Unit* who)
        {
            if (who->GetTypeId() == TYPEID_PLAYER)
                Talk(TALK_SLAY);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            switch (summon->GetEntry())
            {
                case NPC_TWILIGHT_ZEALOT_CORLA:
                    break;
                default:
                    break;
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_COUNT_EVOLVE)
                evolvedCounter++;

            if (action == 99)
                if (!client_side_bug_refreshed)
            {
                client_side_bug_refreshed = true;
                spawnZealots();
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                for (uint8 i = 0; i < 3; ++i)
                    if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST || spell->Effects[i].Mechanic == MECHANIC_INTERRUPT)
                        if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_DARK_COMMAND)
                            me->InterruptSpell(CURRENT_GENERIC_SPELL, false);
        }

        void SetData(uint32 data, uint32 state)
        {
            switch (data)
            {
                case DATA_SPAWN_LEFT:
                    left = data;
                    break;
                case DATA_SPAWN_RIGHT:
                    right = data;
                    break;
                case DATA_SPAWN_CENTER:
                    center = data;
                    break;
                default:
                    break;
            }
        }
        
        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
                return !(p->isGameMaster() || !p->isGMVisible());

            return false;
        }

        uint32 GetData(uint32 data) const
        {
            switch(data)
            {
                case DATA_SPAWN_LEFT:
                    return left;
                case DATA_SPAWN_RIGHT:
                    return right;
                case DATA_SPAWN_CENTER:
                    return center;
            }
            return 0;
        }

        uint64 GetGUID(int32 type) const
        {
            switch(type)
            {
                case DATA_ZEALOT_LEFT:
                    return ZealotGUID[0];
                case DATA_ZEALOT_RIGHT:
                    return ZealotGUID[1];
                case DATA_ZEALOT_CENTER:
                    return ZealotGUID[2];
            }
            return 0;
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) 
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon);
        }

        void spawnZealots()
        {
            for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                if (auto zealot = ObjectAccessor::GetCreature(*me, *itr))
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, zealot);
                    if (zealot->isAlive() && (zealot->GetEntry() == NPC_TWILIGHT_ZEALOT_CORLA))
                        zealot->AI()->DoAction(ACTION_DESPAWN_ESSENCE);
                }

            summons.DespawnAll();

            for(uint8 i = 0; i <= (IsHeroic() ? 2 : 1); i++)
            {
                ZealotGUID[i] = 0;
                if (auto zealot = me->SummonCreature(NPC_TWILIGHT_ZEALOT_CORLA, positionData[i].position, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    ZealotGUID[i] = zealot->GetGUID();
                    zealot->CastWithDelay(500, zealot, SPELL_KNEELING_IN_SUPPLICATION, true);
                    if (auto trigger_charge_controller = zealot->SummonCreature(NPC_NETHER_ESSENCE_TRIGGER, beam_control_spawns[i], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            switch (i)
                            {
                                case 0:
                                    trigger_charge_controller->AI()->SetData(DATA_CORLA_BEAM_TARGETING, DATA_SPAWN_LEFT);
                                    break;
                                case 1:
                                    trigger_charge_controller->AI()->SetData(DATA_CORLA_BEAM_TARGETING, DATA_SPAWN_RIGHT);
                                    break;
                                case 2:
                                    trigger_charge_controller->AI()->SetData(DATA_CORLA_BEAM_TARGETING, DATA_SPAWN_CENTER);
                                    break;
                                }
                        }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;
			if (auto player = me->FindNearestPlayer(20.00f))
				if(player->HasAura(SPELL_TWILIGHT_EVOLUTION))
					me->SetMaxHealth(me->GetMaxHealth());
				
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_DARK_COMMAND:
                        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true))
                            DoCast(target, SPELL_DARK_COMMAND);
                        events.ScheduleEvent(EVENT_DARK_COMMAND, 25000);
                        break;

                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        bool client_side_bug_refreshed{false};
        uint8 evolvedCounter;
        uint32 left;
        uint32 right;
        uint32 center;
        uint64 ZealotGUID[3];
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_corla_herald_of_twilightAI (creature);
    }
};

class mob_twilight_zealot : public CreatureScript
{
public:
    mob_twilight_zealot() : CreatureScript("mob_twilight_zealot") { }

    struct mob_twilight_zealotAI : public ScriptedAI
    {
        mob_twilight_zealotAI(Creature* creature) : ScriptedAI(creature), summons(creature), Intialized(false)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
            Intialized = false;
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* /*who*/)
        {
            for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                if (auto essence = ObjectAccessor::GetCreature(*me, *itr))
                    if (essence->isAlive() && (essence->GetEntry() == NPC_NETHER_ESSENCE_TRIGGER))
                        essence->AI()->DoAction(ACTION_TRIGGER_START_CHANNELING);

            if (auto corla = Creature::GetCreature(*me, instance->GetData64(DATA_CORLA)))
                corla->SetInCombatWithZone();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            if (summon->GetEntry() == NPC_NETHER_ESSENCE_TRIGGER)
            {
                float dist_spawn[3]
                {
                    me->GetDistance(summonPositions[0]),
                    me->GetDistance(summonPositions[1]),
                    me->GetDistance(summonPositions[2]),
                };

                uint32 i{ 0 };  //what zealot was this thing actually spawned near

                if (dist_spawn[0] < dist_spawn[1] && dist_spawn[0] < dist_spawn[2])
                {
                    i = 0;
                }
                else if (dist_spawn[1] < dist_spawn[0] && dist_spawn[1] < dist_spawn[2])
                {
                    i = 1;
                }
                else if (dist_spawn[2] < dist_spawn[0] && dist_spawn[2] < dist_spawn[1])
                {
                    i = 2;
                }


                if (auto c = summon->SummonCreature(NPC_NETHER_ESSENCE_TRIGGER, NetherEssencePos[i], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    //TC_LOG_ERROR("sql.sql", "summoned trigger with guid %u", c->GetGUID());
                }
            }
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_DESPAWN_ESSENCE)
            {

                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (auto essence = ObjectAccessor::GetCreature(*me, *itr))
                        essence->AI()->DoAction(ACTION_DESPAWN_ESSENCE);

                summons.DespawnAll();
            }
        }

        void UpdateAI(const uint32 Diff)
        {

            if(!Intialized)
            {
                events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
                events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
                events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
                events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);

                Intialized = true;

                me->SetReactState(REACT_AGGRESSIVE);

                me->GetMotionMaster()->MoveChase(GetPlayerAtMinimumRange(1.0f));
            }

            events.Update(Diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_FORCE_BLAST:
                        if (me->isInCombat() && !me->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200, true))
                                DoCast(target, SPELL_FORCE_BLAST);
                        }
                        events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
                        break;
                    case EVENT_GRAVITY_STRIKE:
                        if (me->isInCombat() && !me->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                        {
                            DoCastVictim(SPELL_GRAVITY_STRIKE);
                        }
                        events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
                        break;
                    case EVENT_GRIEVOUS_WHIRL:
                        if (me->isInCombat() && !me->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                        {
                            DoCastAOE(SPELL_GRIEVOUS_WHIRL);
                        }
                        events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
                        break;
                    case EVENT_SHADOW_STRIKE:
                        if (me->isInCombat() && !me->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true))
                            DoCast(target, SPELL_SHADOW_STRIKE);
                        }
                        events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);
                        break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        EventMap events;
        SummonList summons;
        bool Intialized;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_twilight_zealotAI (creature);
    }
};

class mob_corla_netheressence_trigger : public CreatureScript
{
public:
    mob_corla_netheressence_trigger() : CreatureScript("mob_corla_netheressence_trigger") { }
    /*
        This script serves dual roles.

        Corla will spawn the zealots.
        The zealots will spawn one of these and add it to their summmons list.
        These will then spawn a duplicate of themselves on the drake.
        The zealots, upon entering combat, will send ACTION_TRIGGER_START_CHANNELING to the 3 located just behind them. 
        The 3 located behind them will then check all of THEIR summons at the drake and have them cast on the nearby zealot.
    */
    struct mob_corla_netheressence_triggerAI : public ScriptedAI
    {
        mob_corla_netheressence_triggerAI(Creature* creature) : ScriptedAI(creature), summons(creature)
        {
            //me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
            instance = me->GetInstanceScript();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
        }

        void SetData(uint32 data, uint32 state)
        {
            switch (data)
            {
            case DATA_CORLA_BEAM_TARGETING:
                target = state;
            default:
                break;
            }
        }

        uint32 GetData(uint32 data) const
        {
            switch (data)
            {
            case DATA_CORLA_BEAM_TARGETING:
                return target;
            }
            return 0;
        }

        void DoAction(const int32 action)
        {
            switch(action)
            {
                case ACTION_TRIGGER_START_CHANNELING:
                    //TC_LOG_ERROR("sql.sql", "%u recognizing ACTION_TRIGGER_START_CHANNELING", me->GetGUID());
                    for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                        if (auto trigger = ObjectAccessor::GetCreature(*me, *itr))
                            trigger->AddAura(SPELL_NETHERESSENCE_AURA, trigger);
                    events.ScheduleEvent(EVENT_EVOLUTION_CAST, 100);
                    events.ScheduleEvent(EVENT_NETHER_ESSENCE, 500);
                    break;

                case ACTION_DESPAWN_ESSENCE:
                    summons.DespawnAll();
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_EVOLUTION_CAST:
                    {
                        bool abort{ false };
                        bool casted{ false };
                        Creature* zealot{ nullptr };



                        if (auto corla = Creature::GetCreature(*me, instance->GetData64(DATA_CORLA)))
                        {
                            //TC_LOG_ERROR("sql.sql", "DATA_CORLA_BEAM_TARGETING = %u", GetData(DATA_CORLA_BEAM_TARGETING));

                            switch (GetData(DATA_CORLA_BEAM_TARGETING))
                            {
                            case DATA_SPAWN_LEFT:
                                if (auto target = Creature::GetCreature(*me, corla->AI()->GetGUID(DATA_ZEALOT_LEFT)))
                                    zealot = target;
                                break;
                            case DATA_SPAWN_CENTER:
                                if (auto target = Creature::GetCreature(*me, corla->AI()->GetGUID(DATA_ZEALOT_CENTER)))
                                    zealot = target;
                                break;
                            case DATA_SPAWN_RIGHT:
                                if (auto target = Creature::GetCreature(*me, corla->AI()->GetGUID(DATA_ZEALOT_RIGHT)))
                                    zealot = target;
                                break;
                            }

                            if (corla->AI()->GetData(GetData(DATA_CORLA_BEAM_TARGETING)) == DONE)
                                abort = true;
                        }

                        if (!abort)
                        {
                            if (zealot)
                                if (auto radius = (me->GetDistance(zealot) + 0.5f))
                                {
                                    if (auto p = me->FindNearestPlayer(radius))
                                    {
                                        for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                            if (auto trigger = ObjectAccessor::GetCreature(*me, *itr))
                                                if (!casted)
                                                {
                                                    if (!p->HasAura(SPELL_TWILIGHT_EVOLUTION))
                                                    {
                                                        trigger->AI()->DoCast(p, SPELL_EVOLUTION, true);
                                                        trigger->AI()->DoCast(p, SPELL_EVOLUTION, true);
                                                    }
                                                    trigger->AI()->DoCast(p, SPELL_EVOLUTION_COSMETIC_HELIX, false);
                                                    casted = true;
                                                }
                                    }
                                    else
                                    {
                                        for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                            if (auto trigger = ObjectAccessor::GetCreature(*me, *itr))
                                            {
                                                trigger->AI()->DoCast(zealot, SPELL_EVOLUTION, true);
                                                trigger->AI()->DoCast(zealot, SPELL_EVOLUTION, true);
                                                trigger->AI()->DoCast(zealot, SPELL_EVOLUTION_COSMETIC_HELIX, false);
                                            }
                                    }
                                }
                        }
                        events.ScheduleEvent(EVENT_EVOLUTION_CAST, 500);
                    }
                        break;
                    case EVENT_NETHER_ESSENCE:
                        for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                            if (auto trigger = ObjectAccessor::GetCreature(*me, *itr))
                                trigger->AI()->DoCast(trigger, RAND(SPELL_NETHERESSENCE_VISUAL_1, SPELL_NETHERESSENCE_VISUAL_2, SPELL_NETHERESSENCE_VISUAL_3), true);
                        events.ScheduleEvent(EVENT_NETHER_ESSENCE, 3500);
                        break;
                }
        }

    private:
        uint32 target{ 0 };
        SummonList summons;
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_corla_netheressence_triggerAI (creature);
    }
};

class spell_corla_evolution : public SpellScriptLoader
{
public:
    spell_corla_evolution() : SpellScriptLoader("spell_corla_evolution") { }

    class spell_corla_evolution_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corla_evolution_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                if (auto c = GetCaster())
                for (auto itr = targets.begin(); itr != targets.end();)
                    if (auto t = (*itr))
                        if (!t->GetGUID() != c->GetOwnerGUID())
                        {
                            targets.erase(itr++);
                        }
                        else
                        {
                            ++itr;
                        }
        }

        void Register()
        {
            //OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corla_evolution_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corla_evolution_SpellScript();
    }
};

class spell_corla_evolution_aura : public SpellScriptLoader
{
public:
    spell_corla_evolution_aura() : SpellScriptLoader("spell_corla_evolution_aura") { }

    class spell_corla_evolution_aura_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_corla_evolution_aura_SpellScript);

        void HandleEvolution(SpellEffIndex effIndex)
        {
            InstanceScript* instance = GetHitUnit()->GetInstanceScript();
            Unit* target = GetHitUnit();

            if (!instance || !target)
                return;

            if (auto corla = Creature::GetCreature(*GetCaster(), instance->GetData64(DATA_CORLA)))
            {
                if(target->GetAuraCount(SPELL_EVOLUTION_AURA) == 100 || target->GetAuraCount(SPELL_EVOLUTION_AURA_HC) == 100)
                {
                    switch (target->GetTypeId())
                    {
                        case TYPEID_PLAYER:
                            corla->CastSpell(target, SPELL_TWILIGHT_EVOLUTION, true); // a hostile creature needs to cast it...
                            target->RemoveAurasDueToSpell(SPELL_EVOLUTION_AURA);
                            target->RemoveAurasDueToSpell(SPELL_EVOLUTION_AURA_HC);
                            break;
                        case TYPEID_UNIT:
                            target->ToCreature()->AI()->DoAction(ACTION_DESPAWN_ESSENCE);
                            target->RemoveAurasDueToSpell(SPELL_KNEELING_IN_SUPPLICATION);
                            target->RemoveAurasDueToSpell(SPELL_EVOLUTION_AURA);
                            target->RemoveAurasDueToSpell(SPELL_EVOLUTION_AURA_HC);
                            target->SetDisplayId(31705);
                            target->SetMaxHealth(target->GetMaxHealth() / 100 * 300);
                            target->SetHealth(target->GetMaxHealth());
                            if (auto attackTarget = target->ToCreature()->SelectNearestPlayer(200.0f))
                                target->ToCreature()->AI()->AttackStart(attackTarget);

                            if (corla->AI()->GetGUID(DATA_ZEALOT_LEFT) == target->GetGUID())
                                corla->AI()->SetData(DATA_SPAWN_LEFT, DONE);

                            if (corla->AI()->GetGUID(DATA_ZEALOT_RIGHT) == target->GetGUID())
                                corla->AI()->SetData(DATA_SPAWN_RIGHT, DONE);

                            if (corla->AI()->GetGUID(DATA_ZEALOT_CENTER) == target->GetGUID())
                                corla->AI()->SetData(DATA_SPAWN_CENTER, DONE);

                            if (auto c = target->ToCreature())
                                if (auto instance = c->GetInstanceScript())
                                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, target);
                            break;
                        default:
                            break;
                    }

                    if (target->GetMap()->IsHeroic())
                        corla->AI()->DoAction(ACTION_COUNT_EVOLVE);

                    corla->AI()->Talk(TALK_ZEALOT);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_corla_evolution_aura_SpellScript::HandleEvolution, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_corla_evolution_aura_SpellScript();
    }
};

class spell_gravity_strike : public SpellScriptLoader
{
public:
    spell_gravity_strike() : SpellScriptLoader("spell_gravity_strike") { }

    class spell_gravity_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gravity_strike_SpellScript);

        void RecalculateDamage()
        {
            SetHitDamage(GetHitUnit()->GetHealth() * 0.6f);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_gravity_strike_SpellScript::RecalculateDamage);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gravity_strike_SpellScript();
    }
};

class spell_grievous_whirl : public SpellScriptLoader
{
public:
    spell_grievous_whirl() : SpellScriptLoader("spell_grievous_whirl") { }

    class spell_grievous_whirl_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_grievous_whirl_AuraScript);

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            if (GetUnitOwner()->GetHealth() == GetUnitOwner()->GetMaxHealth())
            {
                PreventDefaultAction();
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_grievous_whirl_AuraScript::HandlePeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_grievous_whirl_AuraScript();
    }
};

void AddSC_boss_corla_herald_of_twilight()
{
    new boss_corla_herald_of_twilight();
    new mob_twilight_zealot();
    new mob_corla_netheressence_trigger();
    new spell_corla_evolution();
    new spell_corla_evolution_aura();
    new spell_gravity_strike();
    new spell_grievous_whirl();
}
