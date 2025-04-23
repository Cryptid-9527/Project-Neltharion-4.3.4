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
    SPELL_DRAIN_SPIRIT_ESSENCE = 97321,
    SPELL_DEAD_ZONE = 97170,
    SPELL_SHADOW_OF_HAKKAR = 97172,
    SPELL_ADD_PLAYERS_TO_THREAT_LIST = 100401,
    SPELL_SPIRIT_WORLD = 98861,
    SPELL_VANISH = 97002,
    SPELL_TRANSFORM = 96716,

    // NPC_JINDO_THE_GODBREAKER
    SPELL_SHADOW_SPIKE = 97158,
    SPELL_SHADOW_SPIKE_RIGHT_ARM = 97160,
    SPELL_SHADOW_SPIKE_LEFT_ARM = 97208,
    SPELL_CALL_SPIRIT = 97152,
    SPELL_SUMMON_SPIRIT = 97123,

    // NPC_SPIRIT_OF_HAKKAR
    SPELL_HAKKAR_BREAKS_FREE = 97209,

    // NPC_HAKKAR_CHAINS
    SPELL_HAKKAR_CHAINS = 97022,
    SPELL_HAKKAR_CHAINS_VISUAL = 97091,
    SPELL_BRITTLE_BARRIER = 97417,

    // NPC_GURUBASHI_SPIRIT_WARRIOR
    SPELL_BODY_SLAM = 97252,
    SPELL_GAZE_LEAP_TARGETING = 97597,
    SPELL_FRENZY = 97088,

};

enum eNpcs
{
    NPC_GURUBASHI_SPIRIT_WARRIOR = 52167,
    //    NPC_SHADOW_OF_HAKKAR = 52650,
    NPC_HAKKAR_CHAINS = 52430,
    NPC_GURUBASHI_SPIRIT = 52730,
    NPC_GURUBASHI_SHADOW = 52732,
    //  NPC_SPIRIT_OF_HAKKAR = 52222,
    NPC_JINDO_THE_GODBREAKER = 52150,
    NPC_TWISTED_SPIRIT = 52624,
    NPC_SPIRIT_PORTAL = 52532,

    NPC_SUNDERED_RIFT = 52400,
    NPC_BROKEN_GROUND = 52407,

};

enum eEvents
{
    EVENT_DRAIN_SPIRIT_ESSENCE_COSMETIC_INTRO       = 1,
    EVENT_DEADZONE                                  = 2,
    EVENT_SHADOWS_OF_HAKKAR                         = 3,
    EVENT_SET_FACING                                = 4,
    EVENT_SPIRIT_WORLD                              = 5,
    EVENT_SPIRIT_WORLD_SUMMON                       = 6,
    EVENT_HAKKAR_SET_FACING                         = 7,
    EVENT_HAKKAR_KILL_JINDO                         = 8,
    EVENT_HAKKAR_YELL_BYE                           = 9,
    EVENT_JINDO_KILL_SELF                           = 10,
    EVENT_RESPAWN_WAR_SPIRIT                        = 11,
    // Spirit world
    EVENT_SHADOW_SPIKE                              = 1,
    EVENT_CALL_SPIRIT                               = 2,
    // Gurubashi troll
    EVENT_BODY_SLAM                                 = 1,
    EVENT_FRENZY                                    = 2,
    EVENT_CHECK_WIPE = 50,
};

enum eTexts
{
    JINDO_YELL_START                                = 0,
    JINDO_EMOTE_SHADOWS_OF_HAKKAR                   = 1,
    JINDO_YELL_SPIRIT_REALM                         = 2,
    JINDO_KILL_PLAYER                               = 3,
    JINDO_SPIRIT_YELL_KILL_PLAYER                   = 0,
    JINDO_SPIRIT_YELL_DIED                          = 1,
    HAKKAR_YELL_SPIT                                = 0,
    HAKKAR_YELL_OVERSTEPPED                         = 1,
    HAKKAR_SPIRIT_YELL_INSECTS                      = 0,
};


enum eDefault
{
    POINT_SPIRIT_WORLD                              = 1,

    ACTION_HAKKAR_FREE                              = 1,
    ACTION_DESPAWN                                  = 2,
    ACTION_CHAIN_DIED                               = 3,
    ACTION_BERSERK_SPIRIT_ENTER                     = 4,
    ACTION_BERSERK_SPIRIT_LEAVE                     = 5,

    TYPE_SPAWN_POSITION_ID                          = 1,
    TYPE_KILLED_TROLL_ID                            = 1,
    DATA_ARM,
};

const Position HakkarSP = {-11786.5f, -1690.92f, 53.01943f, 1.605703f};

const Position JindoSpiritSP = {-11785.81f, -1707.089f, 58.22891f, 1.466077f};

const Position HakkarChainSP[2][3]=
{
    {
        {-11814.56f, -1652.120f, 52.96632f, 0.0f},
        {-11778.60f, -1659.418f, 53.06332f, 0.0f},
        {-11801.36f, -1678.391f, 53.04712f, 0.0f},
    },
    {
        {-11761.55f, -1649.866f, 52.96572f, 0.0f},
        {-11772.65f, -1676.898f, 53.04771f, 0.0f},
        {-11796.30f, -1659.990f, 53.09364f, 0.0f},
    }
};

const Position SpiritPortalSP[15]=
{
    {-11811.90f, -1637.995f, 52.99569f, 0.0f},
    {-11793.88f, -1646.389f, 54.62890f, 0.0f},
    {-11782.06f, -1646.380f, 54.56368f, 0.0f},
    {-11769.54f, -1663.590f, 53.06438f, 0.0f},
    {-11788.62f, -1628.262f, 54.86021f, 0.0f},
    {-11751.88f, -1662.833f, 53.08055f, 0.0f},
    {-11764.74f, -1636.479f, 52.98969f, 0.0f},
    {-11752.42f, -1645.153f, 52.87562f, 0.0f},
    {-11756.94f, -1680.793f, 53.07720f, 0.0f},
    {-11749.95f, -1631.240f, 52.89489f, 0.0f},
    {-11823.27f, -1665.200f, 53.08056f, 0.0f},
    {-11823.57f, -1647.462f, 52.87725f, 0.0f},
    {-11816.96f, -1682.873f, 53.07767f, 0.0f},
    {-11805.72f, -1664.719f, 53.06411f, 0.0f},
    {-11827.62f, -1634.120f, 52.93477f, 0.0f},
};

const Position SpiritWarriorSP[4]=
{
    {-11837.85f, -1698.944f, 40.88818f, 0.0f},
    {-11735.52f, -1695.576f, 40.88869f, 0.0f},
    {-11737.58f, -1592.231f, 40.88970f, 0.0f},
    {-11842.00f, -1595.804f, 40.88970f, 0.0f},
};

class boss_jindo_the_godbreaker : public CreatureScript
{
    public:
        boss_jindo_the_godbreaker() : CreatureScript("boss_jindo_the_godbreaker") { }

        struct boss_jindo_the_godbreakerAI : public BossAI
        {
            boss_jindo_the_godbreakerAI(Creature* creature) : BossAI(creature, DATA_JINDO) { }

            void Reset()
            {
                EntryCheckPredicate pred(NPC_HAKKAR_CHAINS);
                summons.DoAction(ACTION_DESPAWN, pred);

                _Reset();

                SpiritTrollSpawMask = 0;

                me->SetReactState(REACT_AGGRESSIVE);
                events.ScheduleEvent(EVENT_DRAIN_SPIRIT_ESSENCE_COSMETIC_INTRO, urand(5000, 15000));

                HakkarGUID = 0;
                JindoSpiritGUID = 0;
                HakkarSpiritGUID = 0;
                ChainCount = 0;
                SpiritWorld = false;
                _currentSpawnWarrior = -1;
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                set_spirit_world(false);
            }

            void InitializeAI()
            {
            }

            void ResetBerserkers()
            {
            }


            bool checkWipe()
            {
                auto players = me->GetPlayersInRange(200.f, true);
                for (Player* player : players)
                    if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                        return false;
                return true;
            }

            void set_spirit_world(bool is_spirit_world)
            {
                std::list<Creature*> units;
                GetCreatureListWithEntryInGrid(units, me, NPC_GURUBASHI_SPIRIT_WARRIOR, 500.0f);
                GetCreatureListWithEntryInGrid(units, me, NPC_GURUBASHI_SPIRIT, 500.0f);
                if (units.size())
                for (auto itr = units.begin(); itr != units.end(); ++itr)
                if (auto c = (*itr))
                {
                    if (c->GetEntry() == NPC_GURUBASHI_SPIRIT_WARRIOR)
                        c->SetVisible(!is_spirit_world);

                    if (c->GetEntry() == NPC_GURUBASHI_SPIRIT)
                        c->SetVisible(is_spirit_world);
                }

                if (auto c = me->FindNearestCreature(NPC_SHADOW_OF_HAKKAR, 100.f, true))
                    c->SetVisible(!is_spirit_world);
                else if (auto c = me->SummonCreature(NPC_SHADOW_OF_HAKKAR, HakkarSP))
                    c->SetVisible(!is_spirit_world);

                me->SetVisible(!is_spirit_world);
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == TYPE_KILLED_TROLL_ID)
                {
                    /*
                    events.CancelEvent(EVENT_RESPAWN_WAR_SPIRIT);
                    if (_currentSpawnWarrior >= 0)
                        if (Creature *warrior = me->SummonCreature(NPC_GURUBASHI_SPIRIT, SpiritWarriorSP[data]))
                            warrior->AI()->SetData(TYPE_SPAWN_POSITION_ID, data);
                    _currentSpawnWarrior = data;
                    events.ScheduleEvent(EVENT_RESPAWN_WAR_SPIRIT, 25000);
                    */
                }
            }

            void JustSummoned(Creature* summoned)
            {
                switch (summoned->GetEntry())
                {
                    case NPC_SHADOW_OF_HAKKAR:
                        HakkarSpiritGUID = summoned->GetGUID();
                        summoned->SetPhaseMask(1, false);
                        break;
                    case NPC_SPIRIT_OF_HAKKAR:
                        HakkarGUID = summoned->GetGUID();
                        summoned->SetReactState(REACT_PASSIVE);
                        summoned->SetPhaseMask(174, true);           //phasemask 16 is ID 174 I guess.
                        break;
                    case NPC_GURUBASHI_SPIRIT_WARRIOR:
                        summoned->SetPhaseMask(1, false);
                        break;
                    case NPC_GURUBASHI_SPIRIT:
                    case NPC_GURUBASHI_SHADOW:
                        summoned->SetPhaseMask(174, true);
                        //summons.Summon(summoned);
                        break;
                    case NPC_JINDO_THE_GODBREAKER:
                        JindoSpiritGUID = summoned->GetGUID();
                        summoned->SetPhaseMask(174, true);
                        break;
                    case NPC_HAKKAR_CHAINS:
                        summoned->SetPhaseMask(174, true);
                        summoned->CastSpell(summoned, SPELL_HAKKAR_CHAINS, false);
                        break;
                    case NPC_SPIRIT_PORTAL:
                        summoned->SetPhaseMask(174, true);
                        if (outro)
                            summoned->DespawnOrUnsummon();
                        break;
                    case NPC_TWISTED_SPIRIT:
                        if (outro)
                            summoned->DespawnOrUnsummon();
                        break;
                }
                BossAI::JustSummoned(summoned);
            }


            void SummonedCreatureDies(Creature* summon, Unit* killer) 
            {
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_CHAIN_DIED)
                {
                    ++ChainCount;

                    if (ChainCount == 3)
                    {
                        outro = true;
                        summons.DespawnEntry(NPC_TWISTED_SPIRIT);
                        summons.DespawnEntry(NPC_SPIRIT_PORTAL);

                        if (auto jindo = ObjectAccessor::GetCreature(*me, JindoSpiritGUID))
                            jindo->AI()->DoAction(ACTION_HAKKAR_FREE);

                        if (auto hakkar = ObjectAccessor::GetCreature(*me, HakkarGUID))
                        {
                            hakkar->AI()->Talk(HAKKAR_YELL_OVERSTEPPED);
                            hakkar->CastSpell(hakkar, SPELL_HAKKAR_BREAKS_FREE, true);
                            events.ScheduleEvent(EVENT_HAKKAR_SET_FACING, 1000);
                        }

                        events.CancelEvent(EVENT_DEADZONE);
                        events.CancelEvent(EVENT_SHADOWS_OF_HAKKAR);
                        
                        std::list<Creature*> units;
                        GetCreatureListWithEntryInGrid(units, me, NPC_GURUBASHI_SPIRIT, 500.0f);
                        GetCreatureListWithEntryInGrid(units, me, NPC_TWISTED_SPIRIT, 500.0f);
                        if (units.size())
                            for (auto itr = units.begin(); itr != units.end(); ++itr)
                                if (auto c = (*itr))
                                    c->Kill(c, false, false);
                    }
                }
            }

            void DamageTaken(Unit* /*done_by*/, uint32 & damage)
            {
                if (me->HealthBelowPctDamaged(70, damage))
                if (!SpiritWorld)
                {
                    SpiritWorld = true;
                    Talk(JINDO_YELL_SPIRIT_REALM);

                    me->SetReactState(REACT_PASSIVE);
                    me->CastStop();
                    me->DeleteThreatList();
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SendClearTarget();
                    me->RemoveAllAuras();

                    events.Reset();
                    events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                    events.ScheduleEvent(EVENT_SET_FACING, 5000);
                    me->GetMotionMaster()->MovePoint(0, -11787.0f, -1695.73f, 52.98806f);
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                _EnterCombat();
                Talk(JINDO_YELL_START);
                me->CastStop();
                events.Reset();
                events.ScheduleEvent(EVENT_DEADZONE, urand(5000, 15000));
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(JINDO_KILL_PLAYER);
            }

            void JustDied(Unit* /*killer*/)
            {
                EntryCheckPredicate pred(NPC_HAKKAR_CHAINS);
                summons.DoAction(ACTION_DESPAWN, pred);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _JustDied();
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);


                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
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
                        case EVENT_DEADZONE:
                            me->CastSpell(me, SPELL_DEAD_ZONE, false);
                            events.ScheduleEvent(EVENT_DEADZONE, 21000);
                            events.ScheduleEvent(EVENT_SHADOWS_OF_HAKKAR, urand(5000, 10000));
                            break;
                        case EVENT_SHADOWS_OF_HAKKAR:
                        {
                            Talk(JINDO_EMOTE_SHADOWS_OF_HAKKAR);
                            float lock_o{ me->GetOrientation() };
                            auto v{ me->getVictim() };
                            me->CastSpell(me, SPELL_SHADOW_OF_HAKKAR, false);
                            me->SetTarget(v->GetGUID());
                            me->SetFacingTo(lock_o);
                            break;
                        }
                        case EVENT_SET_FACING:
                            me->SetFacingTo(1.570796f);
                            events.ScheduleEvent(EVENT_SPIRIT_WORLD, 500);
                            break;
                        case EVENT_SPIRIT_WORLD:
                            me->CastSpell(me, SPELL_ADD_PLAYERS_TO_THREAT_LIST, true);
                            me->CastSpell(me, SPELL_VANISH, true);
                            events.ScheduleEvent(EVENT_SPIRIT_WORLD_SUMMON, 3000);
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                            if (auto c = me->SummonCreature(54638, me->GetPositionX(), me->GetPositionY() + 5.f, me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                c->setFaction(35);
                                c->CastSpell(c, SPELL_SPIRIT_WORLD, true);
                            }
                            if (auto c = me->FindNearestCreature(NPC_SHADOW_OF_HAKKAR, 100.f, true))
                                c->SetVisible(false);
                            break;
                        case EVENT_SPIRIT_WORLD_SUMMON:
                        {
                            me->SummonCreature(NPC_JINDO_THE_GODBREAKER, JindoSpiritSP);
                            uint8 id = urand(0, 1);
                            for (int i = 0; i < 3; ++i)
                                me->SummonCreature(NPC_HAKKAR_CHAINS, HakkarChainSP[id][i]);
                            for (int i = 0; i < 15; ++i)
                                me->SummonCreature(NPC_SPIRIT_PORTAL, SpiritPortalSP[i]);
                            /*
                            for (int i = 0; i < 4; ++i)
                                me->SummonCreature(SpiritTrollSpawMask & 1 << i ? NPC_GURUBASHI_SPIRIT : NPC_GURUBASHI_SHADOW, SpiritWarriorSP[i]);
                            */
                            if (auto hakkar = me->SummonCreature(NPC_SPIRIT_OF_HAKKAR, HakkarSP))
                                hakkar->AI()->Talk(HAKKAR_YELL_SPIT);

                            set_spirit_world(true);
                            break;
                        }
                        case EVENT_HAKKAR_SET_FACING:
                        {
                            if (auto hakkar = ObjectAccessor::GetCreature(*me, HakkarGUID))
                                hakkar->SetFacingTo(4.712389f);
                            events.ScheduleEvent(EVENT_HAKKAR_KILL_JINDO, 17000);
                            break;
                        }
                        case EVENT_HAKKAR_KILL_JINDO:
                        {
                            if (auto hakkar = ObjectAccessor::GetCreature(*me, HakkarGUID))
                                hakkar->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
                            me->RemoveAura(SPELL_SPIRIT_WORLD);
                            events.ScheduleEvent(EVENT_HAKKAR_YELL_BYE, 2500);
                            break;
                        }
                        case EVENT_HAKKAR_YELL_BYE:
                        {
                            if (auto hakkar = ObjectAccessor::GetCreature(*me, HakkarSpiritGUID))
                            {
                                hakkar->AI()->Talk(HAKKAR_SPIRIT_YELL_INSECTS);
                                hakkar->DespawnOrUnsummon(5000);
                            }
                            events.ScheduleEvent(EVENT_JINDO_KILL_SELF, 5000);
                            break;
                        }
                        case EVENT_JINDO_KILL_SELF:
                        {
                            me->RemoveAllAuras();
                            me->CastSpell(me, SPELL_TRANSFORM, false);
                            if (!me->getThreatManager().getThreatList().empty())
                                if (auto killer = ObjectAccessor::GetUnit(*me, (*me->getThreatManager().getThreatList().begin())->getUnitGuid()))
                                    killer->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                            break;
                        }
                        case EVENT_RESPAWN_WAR_SPIRIT:
                        {
                            if (Creature *warrior = me->SummonCreature(NPC_GURUBASHI_SPIRIT, SpiritWarriorSP[_currentSpawnWarrior]))
                                warrior->AI()->SetData(TYPE_SPAWN_POSITION_ID, _currentSpawnWarrior);
                            _currentSpawnWarrior = -1;
                            break;
                        }
                    }
                }

                if (!SpiritWorld)
                if (!me->HasReactState(REACT_PASSIVE))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }
        private:
            //Creature* berserker_spirit_warriors[4]{ nullptr, nullptr, nullptr, nullptr };
            //Creature* berserker_spirits[4]{ nullptr, nullptr, nullptr, nullptr };
            uint64 HakkarGUID           { 0 };
            uint64 JindoSpiritGUID      { 0 };
            uint64 HakkarSpiritGUID     { 0 };
            uint8 ChainCount            { 0 };
            uint8 SpiritTrollSpawMask   { 0 };
            bool SpiritWorld            {false};
            int32 _currentSpawnWarrior  { 0 };
            bool outro                  { false };
        };

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jindo_the_godbreakerAI(creature);
        }
};

class boss_jindo_the_godbreaker_spirit_world : public CreatureScript
{
    public:
        boss_jindo_the_godbreaker_spirit_world() : CreatureScript("boss_jindo_the_godbreaker_spirit_world") { }

        struct boss_jindo_the_godbreaker_spirit_worldAI : public ScriptedAI
        {
            boss_jindo_the_godbreaker_spirit_worldAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                RightArm = true;
                me->SetReactState(REACT_PASSIVE);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(JINDO_SPIRIT_YELL_KILL_PLAYER);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_HAKKAR_FREE)
                {
                    Talk(JINDO_SPIRIT_YELL_DIED);
                    events.Reset();
                    outro = true;
                }
            }

            uint32 GetData(uint32 type) const
            {
                if (type == DATA_ARM)
                    return (uint32)RightArm;
                return 0;
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_SHADOW_SPIKE, 1000);
                events.ScheduleEvent(EVENT_CALL_SPIRIT, 10000);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_SPIKE:
                        {
                            if (!outro)
                            {
                                RightArm = !RightArm;
                                DoCast(SPELL_SHADOW_SPIKE);
                                events.ScheduleEvent(EVENT_SHADOW_SPIKE, 3100);
                            }
                            break;
                        }
                        case EVENT_CALL_SPIRIT:
                        {
                            me->CastSpell(me, SPELL_CALL_SPIRIT, true);
                            events.ScheduleEvent(EVENT_CALL_SPIRIT, 4000);
                            break;
                        }
                    }
                }
            }

        private:
            EventMap events;
            bool RightArm;
            bool outro{ false };
        };

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jindo_the_godbreaker_spirit_worldAI(creature);
        }
};

/*
    GURU SPIRIT WARRIOR AND SPIRIT
*/
class npc_gurubashi_spirit_warrior : public CreatureScript
{
public:
    npc_gurubashi_spirit_warrior() : CreatureScript("npc_gurubashi_spirit_warrior") { }


    struct npc_gurubashi_spirit_warriorAI : public ScriptedAI
    {
        npc_gurubashi_spirit_warriorAI(Creature* creature) : ScriptedAI(creature) {}

        void InitializeAI()
        {
            if (me->isDead())
                if (summons.empty())
                    me->Respawn(true);
        }

        void JustReachedHome()
        {
            me->GetMotionMaster()->MoveRandom(3.0f);
        }

        void JustSummoned(Creature* summoned)
        {
            switch (summoned->GetEntry())
            {
            case NPC_SUNDERED_RIFT:
            case NPC_BROKEN_GROUND:
            case NPC_TWISTED_SPIRIT:
            case NPC_SPIRIT_PORTAL:
                if (me->isDead())
                {
                    summoned->DespawnOrUnsummon();
                }
                else
                summoned->DespawnOrUnsummon(20000);
                break;
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* killer)
        {
            if (summon->GetEntry() == NPC_GURUBASHI_SPIRIT)
            {
                summon->DespawnOrUnsummon();
                me->Respawn(true);
                CorrectVisibility(me);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (me->GetEntry() == NPC_GURUBASHI_SPIRIT_WARRIOR)
            {
                if (auto c = me->SummonCreature(NPC_GURUBASHI_SPIRIT, me->GetHomePosition()))
                {
                    c->SetPhaseMask(174, true);
                    c->DeleteThreatList();
                    CorrectVisibility(c);
                }
            }
            else
            {
                summons.DespawnAll();
                me->DeleteThreatList();
            }
        }

        void CorrectVisibility(Creature* npc)
        {
            if (auto trigger = npc->FindNearestCreature(54638, 200.f, true))
            {
                if (trigger->HasAura(SPELL_SPIRIT_WORLD))
                    npc->SetVisible(npc->GetEntry() == NPC_GURUBASHI_SPIRIT);
                else
                    npc->SetVisible(npc->GetEntry() == NPC_GURUBASHI_SPIRIT_WARRIOR);
            }
            else
                npc->SetVisible(npc->GetEntry() == NPC_GURUBASHI_SPIRIT_WARRIOR);
        }

        void DoAction(int32 const action)
        {
        }

        void SpellHitTarget(Unit* victim, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BODY_SLAM)
                if (victim->GetEntry() == NPC_HAKKAR_CHAINS)
                {
                    victim->RemoveAurasDueToSpell(SPELL_BRITTLE_BARRIER);
                }

            if (spell->Id == SPELL_GAZE_LEAP_TARGETING)
                AttackStart(victim);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_BODY_SLAM, urand(7000, 14000));
            events.ScheduleEvent(EVENT_FRENZY, urand(7000, 14000));
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
        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            switch (spell->Id)
            {
            case SPELL_BODY_SLAM:
                me->SetReactState(REACT_AGGRESSIVE);
                UpdateVictim();
                break;
            }
        }
        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BODY_SLAM:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 30.f, true))
                        DoCast(target, SPELL_GAZE_LEAP_TARGETING, false);
                    events.ScheduleEvent(EVENT_BODY_SLAM, 20000);
                    break;
                case EVENT_FRENZY:
                    me->AddAura(SPELL_FRENZY, me);
                    events.ScheduleEvent(EVENT_FRENZY, 13000);
                    break;
                case EVENT_SPIRIT_WORLD:
                    break;
                default:
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gurubashi_spirit_warriorAI(creature);
    }
};

class npc_hakkars_chains : public CreatureScript
{
    public:
        npc_hakkars_chains() : CreatureScript("npc_hakkars_chains") { }

        struct npc_hakkars_chainsAI : public ScriptedAI
        {
            npc_hakkars_chainsAI(Creature* creature) : ScriptedAI(creature), instance(me->GetInstanceScript()) { }

            void InitializeAI()
            {

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_BIND_SIGHT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DURABILITY_DAMAGE_PCT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_FORCE_CAST_2, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INSTAKILL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INEBRIATE, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_LEAP, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_BURN, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_POWER_DRAIN, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT_ALL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_TAMECREATURE, true);

                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);

                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_STUNNED);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);
            }
            void Reset()
            {
                ASSERT(instance);
                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_HAKKAR_CHAINS_VISUAL, me);
                me->AddAura(SPELL_BRITTLE_BARRIER, me);
                me->CastSpell(me, SPELL_HAKKAR_CHAINS, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);

            }

            void EnterCombat(Unit* /*who*/)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_DESPAWN)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (auto jindo = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_JINDO)))
                    jindo->AI()->DoAction(ACTION_CHAIN_DIED);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

        private:
            InstanceScript* instance;
        };

    private:
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hakkars_chainsAI (creature);
        }
};

class npc_shadow_spirit_summon : public CreatureScript
{
public:
    npc_shadow_spirit_summon() : CreatureScript("npc_shadow_spirit_summon") {}

    struct npc_shadow_spirit_summonAI : public ScriptedAI
    {
        npc_shadow_spirit_summonAI(Creature * creature) : ScriptedAI(creature), instance(me->GetInstanceScript())
        {
        }

        void IsSummonedBy(Unit *)
        {
            if (auto jindo = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_JINDO)))
                jindo->AI()->JustSummoned(me);
        }

        void UpdateAI(const uint32 diff) override
        {
            if (me->isInCombat())
            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private:
        InstanceScript *instance;
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_shadow_spirit_summonAI(creature);
    }
};

class spell_sunder_rift_visual : public SpellScriptLoader
{
    public:
        spell_sunder_rift_visual() : SpellScriptLoader("spell_sunder_rift_visual") { }

    private:
        class spell_sunder_rift_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sunder_rift_visual_SpellScript);

            void ModDestHeight(SpellEffIndex /*effIndex*/)
            {
                Position offset = {0.0f, 0.0f, 10.0f, 0.0f};
                const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
                GetHitDest()->RelocateOffset(offset);
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_sunder_rift_visual_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sunder_rift_visual_SpellScript();
        }
};

class spell_spirit_warriors_gaze : public SpellScriptLoader
{
    public:
        spell_spirit_warriors_gaze() : SpellScriptLoader("spell_spirit_warriors_gaze") { }

    private:
        class spell_spirit_warriors_gazeAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spirit_warriors_gazeAuraScript)

            void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
            {
                if (auto c = GetCaster())
                    if (auto o = GetOwner())
                        if (auto oUnit = o->ToUnit())
                            if (auto cCreature = c->ToCreature())
                            {
                                cCreature->SetReactState(REACT_PASSIVE);
                                c->AttackStop();
                                c->CastSpell(oUnit, GetSpellInfo()->Effects[EFFECT_0].BasePoints, false);
                            }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_spirit_warriors_gazeAuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript *GetAuraScript() const
        {
            return new spell_spirit_warriors_gazeAuraScript();
        }
};

// 97160
class spell_shadow_spike : public SpellScriptLoader
{
    public:
        spell_shadow_spike() : SpellScriptLoader("spell_shadow_spike") { }

    private:
        class spell_shadow_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shadow_spike_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                if (Creature *jindo = GetCaster()->ToCreature())
                    GetCaster()->CastSpell(GetHitUnit(), jindo->AI()->GetData(DATA_ARM) ? GetSpellInfo()->Effects[effIndex].BasePoints : SPELL_SHADOW_SPIKE_LEFT_ARM, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_shadow_spike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_shadow_spike_SpellScript();
        }
};

// 97152
class spell_call_spirit : public SpellScriptLoader
{
    public:
        spell_call_spirit() : SpellScriptLoader("spell_call_spirit") { }

    private:
        class spell_call_spirit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_call_spirit_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SUMMON_SPIRIT, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_call_spirit_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_call_spirit_SpellScript();
        }
};
enum Hierophant
{
    EVENT_ANCIENT_POWER = 1,
    EVENT_HEAL_1,
    EVENT_HEAL_2,
    EVENT_SANCTIFY,
    EVENT_ANC_CALL,
    EVENT_HYPOTHERMIA,
    EVENT_TRANSFORM,
    EVENT_TRANSFORM_RECOVER,

    SPELL_HIERO_ANCIENT_POWER       = 23979,//Ancient Power, damage spell
    SPELL_HIERO_BLESSING_ZANDALAR   = 97977,//Blessing of Zandalar, aura of immunity while casting glory of zandalari
    SPELL_HIERO_GLORY               = 97969,//Glory of the Zandalari, transform spell at 50%?
    SPELL_JUGGER_GLORY              = 97970,
    SPELL_BLOODSLAYER_GLORY         = 97971,
    SPELL_ARCHON_GLORY              = 97972,
    SPELL_HIERO_SANCTIFY_1          = 97973,//Sanctify
    SPELL_HIERO_SANCTIFY_2          = 97974,//Sanctify
    SPELL_HIERO_GUARDIAN            = 97978,//Ancient guardian, healing ally spell

    SPELL_JUGGERNAUT_ANC_CALL       = 98000,
    SPELL_JUGGERNAUT_HYPOTHERMIA    = 96332,
};

class npc_Zandalari_Hierophant : public CreatureScript
{
public:
    npc_Zandalari_Hierophant() : CreatureScript("npc_Zandalari_Hierophant") { }

    struct npc_Zandalari_HierophantAI : public ScriptedAI
    {
        npc_Zandalari_HierophantAI(Creature* creature) : ScriptedAI(creature) {}

        bool transformed = false;
        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_ANCIENT_POWER, urand(5000, 10000));
            events.ScheduleEvent(EVENT_HEAL_1, urand(10000, 20000));
        }
        void Reset()
        {
            events.Reset();
            me->RemoveAurasDueToSpell(SPELL_HIERO_GLORY);
            transformed = false;
        }
        void JustDied(Unit* who)
        {
        }

        bool CanSeeAlways(WorldObject const* obj)
        {
            if (auto p = obj->ToPlayer())
            {
                if (me->IsInCombatWith(obj->ToUnit()))
                    return true;

                return !(p->isGameMaster() || !p->isGMVisible());
            }

            return false;
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

        void DamageTaken(Unit* damageDealer, uint32& damage)
        {

            if (!transformed && me->GetHealthPct() <= 60)
            {
                transformed = true;
                events.ScheduleEvent(EVENT_TRANSFORM, 0);
            }

            if (me->HasAura(SPELL_HIERO_GUARDIAN))
            if (damage > (me->GetHealth() - 1))
            {
                damage = (me->GetHealth() - 1);                 //cancel the possibility of dying from this hit.
                me->SetHealth(me->GetMaxHealth() * 0.50f);
                me->RemoveAurasDueToSpell(SPELL_HIERO_GUARDIAN);
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
                case EVENT_ANCIENT_POWER:
                    spell = SPELL_HIERO_ANCIENT_POWER;         //Spell ID
                    min_repeat = 5000;                 //Minimum Delay
                    max_repeat = 10000;                //Maximum Delay
                    wait_castnext = false;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_HEAL_1:
                    spell = (SPELL_HIERO_SANCTIFY_1);           //Spell ID
                    min_repeat = 8000;                     //Minimum delay
                    max_repeat = 15000;                     //Maximum delay
                    wait_castnext = true;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_HEAL_2:
                    spell = (SPELL_HIERO_GUARDIAN);           //Spell ID
                    min_repeat = 15000;                     //Minimum delay
                    max_repeat = 30000;                     //Maximum delay
                    wait_castnext = true;                  //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;

                case EVENT_TRANSFORM:
                    events.Reset();
                    events.ScheduleEvent(EVENT_TRANSFORM_RECOVER, 5000);
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    me->SetReactState(REACT_PASSIVE);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    DoCast(SPELL_HIERO_GLORY);
                    break;

                case EVENT_TRANSFORM_RECOVER:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.ScheduleEvent(EVENT_ANCIENT_POWER, urand(5000, 10000));
                    events.ScheduleEvent(EVENT_HEAL_1, urand(10000, 20000));
                    events.ScheduleEvent(EVENT_HEAL_2, urand(5000, 10000));
                    break;
                default:
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Zandalari_HierophantAI(creature);
    }
};

class npc_zandalari_juggernaut : public CreatureScript
{
public:
    npc_zandalari_juggernaut() : CreatureScript("npc_zandalari_juggernaut") { }

    struct npc_zandalari_juggernautAI : public ScriptedAI
    {
        npc_zandalari_juggernautAI(Creature* creature) : ScriptedAI(creature) {}

        bool transformed = false;
        void InitializeAI()
        {
        }

        void EnterCombat(Unit* /*who*/) {
            me->ClearUnitState(UNIT_STATE_CASTING);
            events.ScheduleEvent(EVENT_HEAL_1, urand(10000, 20000));
        }
        void Reset()
        {
            events.Reset();
            me->RemoveAurasDueToSpell(SPELL_HIERO_GLORY);
            transformed = false;
        }
        void JustDied(Unit* who)
        {
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

        void DamageTaken(Unit* damageDealer, uint32& damage)
        {

            if (!transformed && me->GetHealthPct() <= 60)
            {
                transformed = true;
                events.ScheduleEvent(EVENT_TRANSFORM, 0);
            }

            if (me->HasAura(SPELL_HIERO_GUARDIAN))
                if (damage > (me->GetHealth() - 1))
                {
                    damage = (me->GetHealth() - 1);                 //cancel the possibility of dying from this hit.
                    me->SetHealth(me->GetMaxHealth() * 0.50f);
                    me->RemoveAurasDueToSpell(SPELL_HIERO_GUARDIAN);
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
                case EVENT_HYPOTHERMIA:
                    spell = SPELL_JUGGERNAUT_HYPOTHERMIA;         //Spell ID
                    min_repeat = 25000;                 //Minimum Delay
                    max_repeat = 35000;                //Maximum Delay
                    wait_castnext = false;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(spell);
                    break;

                case EVENT_ANC_CALL:
                    spell = SPELL_JUGGERNAUT_ANC_CALL;         //Spell ID
                    min_repeat = 25000;                 //Minimum Delay
                    max_repeat = 35000;                //Maximum Delay
                    wait_castnext = false;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    DoCast(spell);
                    events.ScheduleEvent(EVENT_ANC_CALL, urand(10000, 20000));
                    break;

                case EVENT_TRANSFORM:
                    events.Reset();
                    events.ScheduleEvent(EVENT_TRANSFORM_RECOVER, 5000);
                    me->ClearUnitState(UNIT_STATE_CASTING);
                    me->SetReactState(REACT_PASSIVE);
                    me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                    DoCast(SPELL_JUGGER_GLORY);
                    break;

                case EVENT_TRANSFORM_RECOVER:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    me->SetReactState(REACT_AGGRESSIVE);
                    events.ScheduleEvent(EVENT_HYPOTHERMIA, urand(5000, 10000));
                    events.ScheduleEvent(EVENT_ANC_CALL, urand(10000, 20000));
                    break;
                default:
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
    };
private:
    EventMap events;
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zandalari_juggernautAI(creature);
    }
};


class spell_zg_sanctify : public SpellScriptLoader
{
    /*
    *   Need to add players to the spell aura list
    */
public:
    spell_zg_sanctify() : SpellScriptLoader("spell_zg_sanctify") { }

    class spell_zg_sanctify_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_zg_sanctify_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            Unit* caster = GetCaster();
            std::list<Unit*> targets;
            Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(caster, caster, 1.0f);
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targets, u_check);
            caster->VisitNearbyObject(1.0f, searcher);
            if (targets.size())
                for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end();)
                {
                    Unit* temp = (*itr);
                    itr++;
                    if (temp != NULL)
                        unitList.push_back(temp);
                }
        }

        void Register()
        {
            //OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zg_sanctify_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_zg_sanctify_SpellScript();
    }
};

class spell_zg_berserker_void_rift : public SpellScriptLoader
{
    /*
        INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('97320', 'spell_zg_berserker_void_rift');
    */
public:
    spell_zg_berserker_void_rift() : SpellScriptLoader("spell_zg_berserker_void_rift") { }

    class spell_zg_berserker_void_rift_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_zg_berserker_void_rift_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (unitList.size())
            for (auto itr = unitList.begin(); itr != unitList.end();)
            {
                if ((*itr)->GetTypeId() == TYPEID_PLAYER)
                    unitList.erase(itr++);
                else
                    ++itr;
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zg_berserker_void_rift_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        };
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_zg_berserker_void_rift_SpellScript();
    }
};

class spelL_tectonic_plating : public SpellScriptLoader
{
public:
    spelL_tectonic_plating() : SpellScriptLoader("spelL_tectonic_plating") { }

    class spelL_tectonic_plating_AuraScript : public AuraScript
    {
        PrepareAuraScript(spelL_tectonic_plating_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (auto o = GetOwner())
            if (auto caster = o->ToUnit())
                if (auto dyn = caster->GetDynObject(97988))
                    return false;
            
            return true;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spelL_tectonic_plating_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spelL_tectonic_plating_AuraScript();
    }
};

void AddSC_boss_jindo_the_godbreaker()
{
    new npc_Zandalari_Hierophant();
    new npc_zandalari_juggernaut();
    new boss_jindo_the_godbreaker();
    new boss_jindo_the_godbreaker_spirit_world();
    new npc_gurubashi_spirit_warrior();
    new npc_hakkars_chains();
    new spell_sunder_rift_visual();
    new spell_spirit_warriors_gaze();
    new spell_shadow_spike();
    new spell_call_spirit();
    new npc_shadow_spirit_summon();
    new spell_zg_sanctify();
    new spell_zg_berserker_void_rift();
    new spelL_tectonic_plating();
}
