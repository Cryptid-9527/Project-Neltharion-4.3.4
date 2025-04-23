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

#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "blackrock_caverns.h"

enum Texts
{
    TALK_AGGRO   = 0,
    TALK_SHADOW  = 1,
    TALK_SLAY    = 2,
    TALK_DEATH   = 3,
    EMOTE_SHADOW = 4
};

enum Spells
{
    SPELL_THUNDERCLAP             = 76186,
    SPELL_TWILIGHT_CORRUPTION     = 76188,
    SPELL_STONE_BLOW              = 76185,
    SPELL_TRANSFORMATION          = 76196,
    SPELL_TRANSFORMATION_TELEPORT = 76211,
    SPELL_STOP_HEART              = 82393, // unused - intro related
    SPELL_CREPUSCULAR_VEIL        = 76190,
    SPELL_CREPUSCULAR_AURA        = 76189
};

enum Events
{
    EVENT_THUNDERCLAP             = 1,
    EVENT_TWILIGHT_CORRUPTION     = 2,
    EVENT_STONE_BLOW              = 3,
    EVENT_SHADOW_SWITCH           = 4,
    EVENT_UPDATE_AGGRO            = 5,
    EVENT_MAKE_SELECTABLE         = 6
};

#define ACTION_ACHIEVEMENT_FAIL 0
#define ACHIEVEMENT_ASCENDANT_DESCENDING 5284

Position const SummonPositions[4] =
{
    {327.089569f, 557.183044f, 66.007217f, 3.12599f},
    {334.520294f, 565.762573f, 66.007217f, 3.12599f},
    {332.597778f, 547.403442f, 66.007217f, 3.12599f},
    {334.290f, 556.181f, 66.0060f, 3.23686f}
};

class boss_ascendant_lord_obsidius : public CreatureScript
{
    public:
        boss_ascendant_lord_obsidius() : CreatureScript("boss_ascendant_lord_obsidius") { }

        struct boss_ascendant_lord_obsidiusAI : public BossAI
        {
            boss_ascendant_lord_obsidiusAI(Creature* creature) : BossAI(creature, BOSS_ASCENDANT_LORD_OBSIDIUS) {}

            void Reset()
            {
                _Reset();

                first = true;
                sec = true;
                AscendantDescending = true;
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                for (uint8 i = 0; i <= (IsHeroic() ? 2 : 1); ++i)
                    me->SummonCreature(NPC_SHADOW_OF_OBSIDIUS, SummonPositions[i], TEMPSUMMON_MANUAL_DESPAWN);
            }

            void EnterCombat(Unit* who)
            {
                _EnterCombat();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                Talk(TALK_AGGRO);
                events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 10000);
                events.ScheduleEvent(EVENT_STONE_BLOW, 13000);

                if(IsHeroic())
                    events.ScheduleEvent(EVENT_THUNDERCLAP, 7000);

                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    if (auto shadow = ObjectAccessor::GetCreature(*me, *itr))
                        if (shadow->isAlive() && (shadow->GetEntry() == NPC_SHADOW_OF_OBSIDIUS))
                            shadow->AI()->AttackStart(who);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (IsHeroic() && AscendantDescending)
                    instance->DoCompleteAchievement(ACHIEVEMENT_ASCENDANT_DESCENDING);

                Talk(TALK_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->SummonCreature(NPC_FINKLE_EINHORN, SummonPositions[3], TEMPSUMMON_MANUAL_DESPAWN);
                _JustDied();

                if (IsHeroic())
                    instance->FinishLfgDungeon(me);
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
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
                    return;
                }

                if (HealthBelowPct(69) && first || HealthBelowPct(34) && sec)
                {
                    first ? first = false : sec = false;
                    Talk(EMOTE_SHADOW);
                    DoCastAOE(SPELL_TRANSFORMATION);
                    events.ScheduleEvent(EVENT_SHADOW_SWITCH, 2000);
                }
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_SHADOW_OF_OBSIDIUS)
                    summon->CastSpell(summon, SPELL_CREPUSCULAR_VEIL);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_ACHIEVEMENT_FAIL)
                    AscendantDescending = false;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THUNDERCLAP:
                            DoCastAOE(SPELL_THUNDERCLAP);
                            events.ScheduleEvent(EVENT_THUNDERCLAP, 7000);
                            break;
                        case EVENT_TWILIGHT_CORRUPTION:
                            if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target,SPELL_TWILIGHT_CORRUPTION);
                            events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 10000);
                            break;
                        case EVENT_STONE_BLOW:
                            DoCastVictim(SPELL_STONE_BLOW);
                            events.ScheduleEvent(EVENT_STONE_BLOW, 13000);
                            break;
                        case EVENT_SHADOW_SWITCH:
                            {
                                Talk(TALK_SHADOW);
                                std::list<Creature*> temp;
                                for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                    if (auto shadow = ObjectAccessor::GetCreature(*me, *itr))
                                        if (shadow->isAlive() && (shadow->GetEntry() == NPC_SHADOW_OF_OBSIDIUS))
                                        {
                                            temp.push_back(shadow);
                                            shadow->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                        }
                                            
                                if (temp.empty())
                                    break;

                                if (auto shadow = Trinity::Containers::SelectRandomContainerElement(temp))
                                {
                                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    events.ScheduleEvent(EVENT_MAKE_SELECTABLE, 1000);
                                    Position telePos; // this part doesn't work until the teleport opcodes are fixed
                                    me->GetPosition(&telePos);
                                    me->NearTeleportTo(shadow->GetPositionX(),shadow->GetPositionY(),shadow->GetPositionZ(), shadow->GetOrientation());
                                    shadow->NearTeleportTo(telePos.GetPositionX(),telePos.GetPositionY(),telePos.GetPositionZ(),telePos.GetOrientation());
                                    me->getThreatManager().resetAllAggro();
                                }
                            }
                            break;
                        case EVENT_MAKE_SELECTABLE:
                        {
                            for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                if (auto shadow = ObjectAccessor::GetCreature(*me, *itr))
                                    if (shadow->GetEntry() == NPC_SHADOW_OF_OBSIDIUS)
                                        shadow->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        }
                        break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        private:
            bool first;
            bool sec;
            bool AscendantDescending;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ascendant_lord_obsidiusAI(creature);
        }
};

class npc_shadow_of_obsidius : public CreatureScript
{
public:
    npc_shadow_of_obsidius() : CreatureScript("npc_shadow_of_obsidius") { }

    struct npc_shadow_of_obsidiusAI : public ScriptedAI
    {
        npc_shadow_of_obsidiusAI(Creature* creature) : ScriptedAI(creature) {}

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            damage = 0;
            me->SetHealth(me->GetMaxHealth());
        }

        void EnterCombat(Unit* attacker)
        {
            events.ScheduleEvent(EVENT_UPDATE_AGGRO, 2000);
            if (auto boss = Creature::GetCreature(*me, instance->GetData64(DATA_ASCENDANT_LORD_OBSIDIUS)))
                if (!boss->isInCombat())
                    boss->AI()->AttackStart(attacker);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_UPDATE_AGGRO:
                        if (auto target = SelectTarget(SELECT_TARGET_BOTTOMAGGRO))
                        {
                            ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
                            for (auto itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                                if (auto unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                                    me->getThreatManager().modifyThreatPercent(unit, -100);

                            me->getThreatManager().modifyThreatPercent(target, +100);
                        }
                        events.ScheduleEvent(EVENT_UPDATE_AGGRO, 5000);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadow_of_obsidiusAI(creature);
    }
};

class spell_lord_obsidius_transform : public SpellScriptLoader
{
public:
    spell_lord_obsidius_transform() : SpellScriptLoader("spell_lord_obsidius_transform") { }

    class spell_lord_obsidius_transform_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_lord_obsidius_transform_SpellScript);

        void HandleDummy(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (
                (!GetHitUnit()->ToCreature()) 
                || 
                ((GetHitUnit()->GetEntry() != NPC_SHADOW_OF_OBSIDIUS) && (GetHitUnit()->GetEntry() != NPC_ASCENDANT_LORD_OBSIDIUS))
                )
                return;

            for (uint8 i = 0; i < 3; ++i)
                GetCaster()->CastSpell(GetHitUnit(), GetSpellValue()->EffectBasePoints[EFFECT_0 + i], true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_lord_obsidius_transform_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_lord_obsidius_transform_SpellScript();
    }
};

class spell_crepuscular_veil : public SpellScriptLoader
{
public:
    spell_crepuscular_veil() : SpellScriptLoader("spell_crepuscular_veil") { }

    class spell_crepuscular_veil_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_crepuscular_veil_SpellScript);

        void ExtraEffect(SpellEffIndex /*effIndex*/)
        {
            if (GetHitUnit()->GetAuraCount(SPELL_CREPUSCULAR_AURA) == 4)
                if (auto instance = GetCaster()->GetInstanceScript())
                    if (auto obsidius = Creature::GetCreature(*GetCaster(), instance->GetData64(DATA_ASCENDANT_LORD_OBSIDIUS)))
                        obsidius->AI()->DoAction(ACTION_ACHIEVEMENT_FAIL);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_crepuscular_veil_SpellScript::ExtraEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_crepuscular_veil_SpellScript();
    }
};
void AddSC_boss_ascendant_lord_obsidius()
{
    new boss_ascendant_lord_obsidius();
    new npc_shadow_of_obsidius();
    new spell_lord_obsidius_transform();
    new spell_crepuscular_veil();
}
