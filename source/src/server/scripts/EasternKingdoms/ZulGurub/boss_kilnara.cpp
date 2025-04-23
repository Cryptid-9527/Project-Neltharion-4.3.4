/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "zulgurub.h"

enum eSpells
{
    // NPC_HIGHPRIEST_KILNARA
    SPELL_AURA_KILNARA_HOVER            = 97459, // 22:28:18.000
    SPELL_AURA_KILNARA_HOVER_2          = 97428, // 22:28:18.000
    SPELL_WAVE_OF_AGONY                 = 98270, // dummy SPELL_WAVE_OF_AGONY_SUMMON
    SPELL_WAVE_OF_AGONY_SUMMON          = 96461, // summon NPC_WAVE_OF_AGONY_STALKER
    SPELL_TEARS_OF_BLOOD                = 96435,
    SPELL_TEARS_OF_BLOOD_COSM_EXTRA     = 96438,
    SPELL_WAIL_OF_SORROW                = 96948,
    SPELL_SHADOW_BOLT                   = 96956,
    SPELL_LASH_OF_ANGUISH               = 96423, // 96958
    SPELL_CAVE_IN                       = 97380,
    SPELL_PRIME_OF_BLESSING             = 96559,
    SPELL_RAGE_OF_THE_ANCIENTS          = 96896,
    SPELL_PRIMAL_AWAKING                = 96530,
    SPELL_CAMOUFLAGE                    = 96594,
    SPELL_RAVAGE                        = 96592,
    SPELL_VENGEFULL_SMASH               = 96954, // 96593
    SPELL_SHADOWSTEP                    = 96683,

    // NPC_PRIDE_OF_BETHEKK
    SPELL_DARK_SLUMBER                  = 96446,
    SPELL_GAPING_WOUND                  = 97355,

    // NPC_WAVE_OF_AGONY_STALKER
    SPELL_WAVE_OF_AGONY_PERIODIC        = 96457,
    SPELL_WAVE_OF_AGONY_EFF             = 96542,
    SPELL_WAVE_OF_AGONY_DAMAGE          = 96460,//actual damage source

    // NPC_WAVE_OF_AGONY
    SPELL_WAVE_OF_AGONY_AURA = 96532,

    // NPC_CAVE_IN_STALKER
    SPELL_CAVE_IN_AURA = 96935,

    SPELL_FELIN_JUMP = 97355,
};

enum eNpcs
{
    NPC_HIGHPRIEST_KILNARA              = 52059,
    NPC_PRIDE_OF_BETHEKK                = 52061,
    NPC_WAVE_OF_AGONY_STALKER           = 52160,
    NPC_WAVE_OF_AGONY                   = 52147,
    NPC_CAVE_IN_STALKER                 = 52387,
};

enum eEvents
{
    EVENT_WAVE_OF_AGONY         = 1,
    EVENT_RETURN_TO_FIGHT       = 2,
    EVENT_TEARS_OF_BLOOD        = 3,
    EVENT_WAIL_OF_SORROW        = 4,
    EVENT_SHADOW_BOLT           = 5,
    EVENT_LASH_OF_ANGUISH       = 6,
    EVENT_MIGHT_OF_BETHEKK      = 7,
    EVENT_PRIMAL_BLESSING       = 8,
    EVENT_PRIMAL_AWAKENING      = 9,
    EVENT_CAMOUFLAGE            = 10,
    EVENT_CAMOUFLAGE_HUNTING    = 11,
    EVENT_CAMOUFLAGE_REMOVE     = 12,
    EVENT_RAVAGE                = 13,
    EVENT_VENGEFUL_SMASH        = 14,
    EVENT_CHECK_ROOM_POSITION   = 15,
    EVENT_FLOAT_AGAIN,

    EVENT_GAPING_WOUND          = 1,
    EVENT_AWAKE                 = 2,
};

enum eTexts
{
    SAY_AGGRO                   = 0,
    SAY_WAVE_OF_AGONY           = 1,
    SAY_TRANSFROM_1             = 2,
    SAY_TRANSFROM_2             = 3,
    SAY_DEATH                   = 4,
    SAY_PLAYER_KILL             = 5,
    SAY_PLAYER_KILL_FERAL       = 6,
};

enum eMiscs
{
    ACTION_WAVE_OF_AGONY        = 1,
    TYPE_WAVE_OF_AGONY,
};

const Position CaveInSP[13]=
{
    {-11531.5f, -1605.48f, 44.4849f, 0.0f},
    {-11538.2f, -1645.08f, 44.4849f, 0.0f},
    {-11508.8f, -1604.29f, 44.4849f, 0.0f},
    {-11495.3f, -1621.67f, 44.4849f, 0.0f},
    {-11560.4f, -1627.20f, 44.4849f, 0.0f},
    {-11539.6f, -1635.53f, 44.4849f, 0.0f},
    {-11510.0f, -1619.33f, 44.4849f, 0.0f},
    {-11529.0f, -1626.86f, 44.4849f, 0.0f},
    {-11502.0f, -1632.88f, 44.4849f, 0.0f},
    {-11491.3f, -1634.09f, 44.4849f, 0.0f},
    {-11519.9f, -1640.14f, 44.4849f, 0.0f},
    {-11528.5f, -1648.30f, 44.4849f, 0.0f},
    {-11539.6f, -1617.73f, 44.4849f, 0.0f},
};

const Position PrideofBethekkSP[16]=
{
    {-11519.2f, -1605.37f, 44.4849f, 3.56047f},
    {-11518.1f, -1651.48f, 44.4849f, 2.26893f},
    {-11505.6f, -1607.56f, 44.4849f, 2.26893f},
    {-11521.8f, -1651.58f, 44.4849f, 0.97738f},
    {-11504.3f, -1645.56f, 44.4849f, 4.66003f},
    {-11517.2f, -1646.82f, 44.4849f, 3.87463f},
    {-11504.6f, -1603.33f, 44.4849f, 3.87463f},
    {-11506.6f, -1651.04f, 44.4849f, 1.60570f},
    {-11507.0f, -1644.55f, 44.4849f, 4.78220f},
    {-11519.7f, -1609.00f, 44.4849f, 2.26893f},
    {-11523.2f, -1605.96f, 44.4849f, 5.41052f},
    {-11523.2f, -1609.31f, 44.4849f, 0.97738f},
    {-11504.1f, -1650.26f, 44.4849f, 2.26893f},
    {-11520.6f, -1646.00f, 44.4849f, 4.95674f},
    {-11508.3f, -1607.37f, 44.4849f, 0.97738f},
    {-11508.7f, -1603.38f, 44.4849f, 5.41052f},
};

const float RoomCenter[2] = {-11518.8f, -1627.16f};

class boss_kilnara : public CreatureScript
{
    public:
        boss_kilnara() : CreatureScript("boss_kilnara") { }

    private:
        struct boss_kilnaraAI : public BossAI
        {
            boss_kilnaraAI(Creature* creature) : BossAI(creature, DATA_KILNARA) 
            {
                events.ScheduleEvent(EVENT_FLOAT_AGAIN, 2000);
            }

            uint64 WaveofAgonyTargetGUID;
            bool MightofBethekk;

            void Reset()
            {
                _Reset();
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 43219);
                //  me->SetCurrentEquipmentId(43219);
                me->SetReactState(REACT_AGGRESSIVE);
                WaveofAgonyTargetGUID = 0;
                MightofBethekk = false;
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                for (int i = 0; i < 16; ++i)
                    me->SummonCreature(NPC_PRIDE_OF_BETHEKK, PrideofBethekkSP[i]);

                events.Reset();
                events.ScheduleEvent(EVENT_FLOAT_AGAIN, 2000);
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    /*
                    * //Never Occurs, scripted otherwise
                    case NPC_WAVE_OF_AGONY_STALKER:
                    {
                        me->CastSpell(summon, SPELL_WAVE_OF_AGONY_PERIODIC, false);
                        WaveofAgonyTargetGUID = summon->GetGUID();
                        break;
                    }
                    */
                    case NPC_WAVE_OF_AGONY:
                    {
                        summon->setFaction(me->getFaction());
                        summon->SetReactState(REACT_PASSIVE);
                        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        summon->CastSpell(summon, SPELL_WAVE_OF_AGONY_AURA, true);
                        break;
                    }
                    case NPC_CAVE_IN_STALKER:
                        summon->CastSpell(summon, SPELL_CAVE_IN_AURA, false);
                        return;
                    case NPC_PRIDE_OF_BETHEKK:
                        //TC_LOG_ERROR("sql.sql", "kilnara just summoned NPC_PRIDE_OF_BETHEKK");
                        summon->SetReactState(REACT_PASSIVE);
                        break;
                }

                summons.Summon(summon);
            }

            void DoAction(int32 const action)
            {
                /*
                if (action == ACTION_WAVE_OF_AGONY)
                {
                    if (auto target = ObjectAccessor::GetCreature(*me, WaveofAgonyTargetGUID))
                    {
                        WaveofAgonyTargetGUID = 0;
                        target->DespawnOrUnsummon(500);
                        if (Creature *wave = me->FindNearestCreature(NPC_WAVE_OF_AGONY, 50.0f))
                            wave->DespawnOrUnsummon(500);
                    }
                    events.ScheduleEvent(EVENT_RETURN_TO_FIGHT, 1000);
                }
                */
            }

            uint64 GetGUID(int32 type) const
            {
                if (type == TYPE_WAVE_OF_AGONY)
                    return WaveofAgonyTargetGUID;
                return 0;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (!MightofBethekk && me->GetHealthPct() <= 50.0f)
                {
                    MightofBethekk = true;
                    events.Reset();
                    events.ScheduleEvent(EVENT_CHECK_ROOM_POSITION, 1000);
                    events.ScheduleEvent(EVENT_MIGHT_OF_BETHEKK, 250);
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                Talk(SAY_AGGRO);
                me->SetHover(false);
                me->RemoveAurasDueToSpell(SPELL_AURA_KILNARA_HOVER);
                me->RemoveAurasDueToSpell(SPELL_AURA_KILNARA_HOVER_2);
                events.ScheduleEvent(EVENT_TEARS_OF_BLOOD, urand(5000, 10000));
                events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 7000));
                events.ScheduleEvent(EVENT_LASH_OF_ANGUISH, urand(10000, 15000));
                events.ScheduleEvent(EVENT_CHECK_ROOM_POSITION, 1000);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(MightofBethekk ? SAY_PLAYER_KILL_FERAL : SAY_PLAYER_KILL);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->RemoveAllAuras();
            }

            void GetInvisibleTeleportPosition(float &x, float &y, float &z)
            {

            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                if (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_FLOAT_AGAIN:
                        {
                            if (me->GetDistance(me->GetHomePosition()) > 1.f || me->isMoving() || me->isInCombat())
                                events.ScheduleEvent(EVENT_FLOAT_AGAIN, 2000);
                            else
                            {
                                //me->monsterYell("EVENT_FLOAT_AGAIN", LANG_UNIVERSAL, 0);
                                me->SetHover(true);
                                me->AddAura(SPELL_AURA_KILNARA_HOVER, me);
                                me->AddAura(SPELL_AURA_KILNARA_HOVER_2, me);
                            }
                            break;
                        }
                        case EVENT_CHECK_ROOM_POSITION:
                        {
                            if (me->GetExactDist2dSq(RoomCenter[0], RoomCenter[1]) > 1500.0f)
                            {
                                ScriptedAI::EnterEvadeMode();
                                return;
                            }
                            events.ScheduleEvent(EVENT_CHECK_ROOM_POSITION, 1000);
                            break;
                        }
                        case EVENT_WAVE_OF_AGONY:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_WAVE_OF_AGONY", LANG_UNIVERSAL, 0);
                                //me->SetReactState(REACT_PASSIVE);
                                //me->AttackStop();
                                //me->StopMoving();
                                //me->NearTeleportTo(me->GetHomePosition());
                                Position pos_summon(*me);
                                Position pos_moveTo(*me);
                                me->MoveBlink(pos_moveTo, 100.f, 0.f);
                                if (auto c = me->SummonCreature(NPC_WAVE_OF_AGONY, pos_summon))
                                {
                                    c->AddAura(96457, c);
                                    c->GetMotionMaster()->MovePoint(1, pos_moveTo, true, 4.f);
                                    //c->DespawnOrUnsummon(18000);
                                }

                                Talk(SAY_WAVE_OF_AGONY);
                                //DoCastRandom(SPELL_WAVE_OF_AGONY_SUMMON, 40.0f, true);
                                events.ScheduleEvent(EVENT_TEARS_OF_BLOOD, urand(15000, 25000));
                            }
                            break;
                        }
                        case EVENT_TEARS_OF_BLOOD:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_TEARS_OF_BLOOD", LANG_UNIVERSAL, 0);
                                if (!me->HasReactState(REACT_PASSIVE))
                                    DoCast(SPELL_TEARS_OF_BLOOD);
                                events.ScheduleEvent(EVENT_WAIL_OF_SORROW, 9000);
                            }
                            break;
                        case EVENT_WAIL_OF_SORROW:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_WAIL_OF_SORROW", LANG_UNIVERSAL, 0);
                                if (!me->HasReactState(REACT_PASSIVE))
                                    DoCast(SPELL_WAIL_OF_SORROW);
                                events.ScheduleEvent(EVENT_WAVE_OF_AGONY, urand(10000, 20000));
                            }
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_SHADOW_BOLT", LANG_UNIVERSAL, 0);
                                if (!me->HasReactState(REACT_PASSIVE))
                                    DoCastVictim(SPELL_SHADOW_BOLT);
                                events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(8000, 12000));
                            }
                            break;
                        case EVENT_LASH_OF_ANGUISH:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_LASH_OF_ANGUISH", LANG_UNIVERSAL, 0);
                                if (!me->HasReactState(REACT_PASSIVE))
                                    DoCastRandom(SPELL_LASH_OF_ANGUISH, 50.0f, true, -96958);
                                events.ScheduleEvent(EVENT_LASH_OF_ANGUISH, 15000);
                            }
                            break;
                        case EVENT_MIGHT_OF_BETHEKK:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_MIGHT_OF_BETHEKK", LANG_UNIVERSAL, 0);
                                for (int i = 0; i < 13; ++i)
                                    me->SummonCreature(NPC_CAVE_IN_STALKER, CaveInSP[i], TEMPSUMMON_TIMED_DESPAWN, 6000);

                                Talk(SAY_TRANSFROM_1);
                                me->SetReactState(REACT_AGGRESSIVE);
                                DoCast(SPELL_CAVE_IN);
                                events.ScheduleEvent(EVENT_PRIMAL_BLESSING, 6000);
                            }
                            break;
                        }
                        case EVENT_PRIMAL_BLESSING:
                            //me->monsterYell("EVENT_PRIMAL_BLESSING", LANG_UNIVERSAL, 0);
                            DoCast(SPELL_PRIME_OF_BLESSING);
                            me->LoadEquipment(0, true);
                            me->SetSheath(SHEATH_STATE_UNARMED);
                            events.ScheduleEvent(EVENT_PRIMAL_AWAKENING, 2000);
                            break;
                        case EVENT_PRIMAL_AWAKENING:
                            //me->monsterYell("EVENT_PRIMAL_AWAKENING", LANG_UNIVERSAL, 0);
                            Talk(SAY_TRANSFROM_2);
                            DoCast(SPELL_RAGE_OF_THE_ANCIENTS);
                            DoCast(SPELL_PRIMAL_AWAKING);
                            events.ScheduleEvent(EVENT_CAMOUFLAGE, urand(15000, 30000));
                            events.ScheduleEvent(EVENT_RAVAGE, urand(5000, 10000));
                            events.ScheduleEvent(EVENT_VENGEFUL_SMASH, urand(10000, 20000));
                            break;
                        case EVENT_CAMOUFLAGE:
                        {
                            //me->monsterYell("EVENT_CAMOUFLAGE", LANG_UNIVERSAL, 0);
                            DoCast(SPELL_CAMOUFLAGE);
                            events.Reset();
                            events.ScheduleEvent(EVENT_CHECK_ROOM_POSITION, 1000);
                            events.ScheduleEvent(EVENT_CAMOUFLAGE_HUNTING, 2500);
                            events.ScheduleEvent(EVENT_CAMOUFLAGE_REMOVE, 24000);
                            break;
                        }
                        case EVENT_CAMOUFLAGE_HUNTING:
                        {
                            //me->monsterYell("EVENT_CAMOUFLAGE_HUNTING", LANG_UNIVERSAL, 0);
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            {
                                me->CastSpell(target, SPELL_SHADOWSTEP, true);
                                me->RemoveAura(SPELL_VENGEFULL_SMASH);
                                me->RemoveAura(96594);
                                DoCast(target, SPELL_RAVAGE);
                                events.RescheduleEvent(EVENT_CAMOUFLAGE_REMOVE, 500);
                            }

                            break;
                        }
                        case EVENT_CAMOUFLAGE_REMOVE:
                            //me->monsterYell("EVENT_CAMOUFLAGE_REMOVE", LANG_UNIVERSAL, 0);
                            events.Reset();
                            events.ScheduleEvent(EVENT_CHECK_ROOM_POSITION, 1000);
                            events.ScheduleEvent(EVENT_CAMOUFLAGE, urand(15000, 30000));
                            events.ScheduleEvent(EVENT_RAVAGE, urand(5000, 10000));
                            events.ScheduleEvent(EVENT_VENGEFUL_SMASH, urand(10000, 20000));
                            break;
                        case EVENT_RAVAGE:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_RAVAGE", LANG_UNIVERSAL, 0);
                                DoCastVictim(SPELL_RAVAGE);
                                events.ScheduleEvent(EVENT_RAVAGE, urand(10000, 20000));
                            }
                            break;
                        case EVENT_VENGEFUL_SMASH:
                            if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                            else
                            {
                                //me->monsterYell("EVENT_VENGEFUL_SMASH", LANG_UNIVERSAL, 0);
                                DoCast(SPELL_VENGEFULL_SMASH);
                                events.ScheduleEvent(EVENT_RAVAGE, urand(20000, 40000));
                            }
                            break;
                        case EVENT_RETURN_TO_FIGHT:
                            me->SetReactState(REACT_AGGRESSIVE);
                            UpdateVictim();
                            break;
                    }

                if (!me->HasReactState(REACT_PASSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_kilnaraAI(creature);
        }
};

class npc_pride_of_bethekk : public CreatureScript
{
    public:
        npc_pride_of_bethekk() : CreatureScript("npc_pride_of_bethekk") { }

    private:
        struct npc_pride_of_bethekkAI : public ScriptedAI
        {
            npc_pride_of_bethekkAI(Creature* creature) : ScriptedAI(creature) 
            {
                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                {
                    //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : setting REACT_PASSIVE");
                    me->SetReactState(REACT_PASSIVE);
                    Sleep = true;
                }
                else
                {
                    Sleep = false;
                    me->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void InitializeAI()
            {
            }

            void EnterCombat(Unit* who)//entering combat means nothing for the boss mobs
            {
                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                {
                    if (me->HasReactState(REACT_PASSIVE))
                    {
                        //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : EnterCombat with REACT_PASSIVE");
                    }
                }
                else
                {
                    //TC_LOG_ERROR("sql.sql", "pre-bethekk cat : EnterCombat");
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (!events.HasEvent(EVENT_GAPING_WOUND))
                    events.ScheduleEvent(EVENT_GAPING_WOUND, 500);
                    //UpdateVictim();
                }
            }


            void Reset()
            {
                events.Reset();
                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                {
                    Sleep = true;
                    enrage = false;
                    awoken = false;
                    me->SetReactState(REACT_PASSIVE);
                    if (!me->HasAura(SPELL_DARK_SLUMBER))
                        me->AddAura(SPELL_DARK_SLUMBER, me);
                }
                else
                {

                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                {
                    if (Sleep)
                    {
                        //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : damagetaken with sleep");
                        events.CancelEvent(EVENT_AWAKE);
                        events.ScheduleEvent(EVENT_AWAKE, 1);
                    }
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : damagetaken without sleep");
                    }
                }
                else
                {
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                {
                    if (spell->Id == SPELL_PRIMAL_AWAKING)
                    {
                        //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : hit by SPELL_PRIMAL_AWAKING");
                        enrage = true;
                        if (Sleep)
                        {
                            //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : hit by SPELL_PRIMAL_AWAKING with sleep");
                            events.ScheduleEvent(EVENT_AWAKE, 1000);
                        }
                        else
                        {
                            //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : hit by SPELL_PRIMAL_AWAKING without sleep");
                        }
                    }
                    else
                    {
                        //TC_LOG_ERROR("sql.sql", "NPC_PRIDE_OF_BETHEKK : hit by spell %u", spell->Id);
                    }
                }
                else
                {

                }
            }

            void Pounce()
            {
                //TC_LOG_ERROR("sql.sql", "cat with entry %u Executing pounce", me->GetEntry());
                me->ClearUnitState(UNIT_STATE_ROOT);
                auto players = me->GetPlayersInRange(40.f, true);
                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(players))
                {
                    DoCast(random_target, SPELL_FELIN_JUMP, true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_AWAKE:
                            //TC_LOG_ERROR("sql.sql", "cat with entry %u Executing EVENT_AWAKE", me->GetEntry());
                            if (Sleep)
                            {
                                //TC_LOG_ERROR("sql.sql", "cat with entry %u Executing EVENT_AWAKE, sleep is true", me->GetEntry());
                                Sleep = false;
                                if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                                {
                                    me->SetInCombatWithZone();
                                    me->RemoveAura(SPELL_DARK_SLUMBER);
                                    me->SetReactState(REACT_AGGRESSIVE);
                                    events.CancelEvent(EVENT_GAPING_WOUND);
                                    events.ScheduleEvent(EVENT_GAPING_WOUND, enrage ? urand(5000, 15000) : urand(25000, 35000));
                                    //TC_LOG_ERROR("sql.sql", "cat with entry %u scheduling EVENT_GAPING_WOUND", me->GetEntry());
                                }
                            }
                            break;

                        case EVENT_GAPING_WOUND:
                            //TC_LOG_ERROR("sql.sql", "cat with entry %u  and guid %u executing EVENT_GAPING_WOUND", me->GetEntry(), me->GetDBTableGUIDLow());
                            Pounce();
                            if (me->GetEntry() == NPC_PRIDE_OF_BETHEKK)
                            events.ScheduleEvent(EVENT_GAPING_WOUND, enrage ? urand(5000, 15000) : urand(25000, 35000));
                            break;
                    }

                if (me->HasReactState(REACT_AGGRESSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
            }

        private:
            bool awoken{ false };
            bool enrage{ false };
            bool Sleep{ (me->GetEntry() == NPC_PRIDE_OF_BETHEKK) };
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pride_of_bethekkAI(creature);
        }
};

class spell_wave_of_agony_selector : public SpellScriptLoader
{
    public:
        spell_wave_of_agony_selector() : SpellScriptLoader("spell_wave_of_agony_selector") { }

    private:
        class spell_wave_of_agony_selector_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wave_of_agony_selector_SpellScript)

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                if (!unitList.empty())
                    Trinity::Containers::RandomResizeList(unitList, 1);
            }

            void TriggerSpell(SpellEffIndex effIndex)
            {
                GetCaster()->CastSpell(GetHitUnit(), GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_wave_of_agony_selector_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_wave_of_agony_selector_SpellScript::TriggerSpell, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript *GetSpellScript() const
        {
            return new spell_wave_of_agony_selector_SpellScript();
        }
};

class spell_wave_of_agony_periodic : public SpellScriptLoader
{
    public:
        spell_wave_of_agony_periodic() : SpellScriptLoader("spell_wave_of_agony_periodic") { }

    private:
        class spell_wave_of_agony_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_wave_of_agony_periodic_AuraScript)

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                    if (auto owner = GetOwner())
                        if (auto caster = owner->ToUnit())
                        {
                            caster->CastSpell(caster, SPELL_WAVE_OF_AGONY_EFF, true);
                            //TC_LOG_ERROR("sql.sql", "casting SPELL_WAVE_OF_AGONY_EFF");
                        }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_wave_of_agony_periodic_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_wave_of_agony_periodic_AuraScript();
        }
};

class spell_wave_of_agony_periodic_2 : public SpellScriptLoader
{
    public:
        spell_wave_of_agony_periodic_2() : SpellScriptLoader("spell_wave_of_agony_periodic_2") { }

    private:
        class spell_wave_of_agony_periodic_2_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_wave_of_agony_periodic_2_AuraScript)

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_wave_of_agony_periodic_2_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_wave_of_agony_periodic_2_AuraScript();
        }
};

class spell_tears_of_blood : public SpellScriptLoader
{
    public:
        spell_tears_of_blood() : SpellScriptLoader("spell_tears_of_blood") { }

    private:
        class spell_tears_of_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tears_of_blood_SpellScript)

            void Trigger(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[effIndex].TriggerSpell, false);
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_tears_of_blood_SpellScript::Trigger, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript *GetSpellScript() const
        {
            return new spell_tears_of_blood_SpellScript();
        }
};

class spell_tears_damage_channel : public SpellScriptLoader
{
public:
    spell_tears_damage_channel() : SpellScriptLoader("spell_tears_damage_channel") { }
private:
    class spell_tears_damage_channel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_tears_damage_channel_AuraScript)

        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "HandleOnEffectApply");
            if (auto cast = GetCaster())
                cast->AddAura(SPELL_TEARS_OF_BLOOD_COSM_EXTRA, cast);
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "HandleOnEffectRemove");
            if (auto cast = GetCaster())
                cast->RemoveAurasDueToSpell(SPELL_TEARS_OF_BLOOD_COSM_EXTRA);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_tears_damage_channel_AuraScript::HandleOnEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_tears_damage_channel_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_tears_damage_channel_AuraScript();
    }
};


class spell_blooddrinker_blood_leech : public SpellScriptLoader
{
public:
    spell_blooddrinker_blood_leech() : SpellScriptLoader("spell_blooddrinker_blood_leech") { }
    /*
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('96762', 'spell_blooddrinker_blood_leech');
        REPLACE INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('96953', 'spell_blooddrinker_blood_leech');
    */
    class spell_blooddrinker_blood_leech_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_blooddrinker_blood_leech_SpellScript);

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if ((c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED)) || c->HasAuraWithMechanic(MECHANIC_DISARM))
                {
                    c->RemoveAurasDueToSpell(96764);
                    c->RemoveAurasDueToSpell(96952);
                    return SPELL_FAILED_DONT_REPORT;
                }
            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_blooddrinker_blood_leech_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_blooddrinker_blood_leech_SpellScript();
    }
};

class npc_wave_of_agony : public CreatureScript
{
public:
    npc_wave_of_agony() : CreatureScript("npc_wave_of_agony") { }

private:
    struct npc_wave_of_agonyAI : public ScriptedAI
    {
        npc_wave_of_agonyAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            damage = 0;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if (id == 1)
                    me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wave_of_agonyAI(creature);
    }
};

void AddSC_boss_kilnara()
{
    new boss_kilnara();
    new npc_pride_of_bethekk();
    new npc_wave_of_agony();
    new spell_wave_of_agony_selector();
    new spell_wave_of_agony_periodic();
    new spell_wave_of_agony_periodic_2();
    new spell_tears_of_blood();
    new spell_tears_damage_channel();
    new spell_blooddrinker_blood_leech();
}
