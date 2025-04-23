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

#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "the_lost_city_of_tol_vir.h"

 // Minion Of Siamat 44704
 // Servant Of Siamat 45269
 // Cloud Burst 44541

enum Texts
{
    SAY_INTRO = 0,
    SAY_AGGRO = 1,
    SAY_EVENT = 2,
    SAY_KILL = 3,
    SAY_DEATH = 4
};

enum Spells
{
    // Siamat
    SPELL_DEFLECTING_WINDS       = 84589, // Initial shield
    SPELL_STATIC_SHOCK           = 84546,
    SPELL_CALL_OF_SKY            = 84956,
    SPELL_STORM_BOLT_PHASE_1     = 73564,
    SPELL_STORM_BOLT_PHASE_2     = 91853,
    SPELL_ABSORB_STORMS          = 83151,
    SPELL_CLOUD_BURST_SIAMAT     = 83048, // unused
    SPELL_CLOUD_BURST_SUMMON     = 83790,
    SPELL_WAILING_WINDS          = 83094,
    SPELL_WAILING_WINDS_AURA     = 83066,
    SPELL_WAILING_WINDS_TRIGGER  = 83089,
    SPELL_TEMPEST_STORM_ROOT     = 84616,
    SPELL_GATHERED_STORM         = 84982,

    // Minion Of Siamat
    SPELL_TEMPEST_STORM_VISUAL   = 84512,
    SPELL_TEMPEST_STORM_SUMMON   = 83414,

    SPELL_DEPLETION = 84550, // Aura
    SPELL_CHAIN_LIGHTNING        = 83455,

    // tempest storm
    SPELL_TEMPEST_STORM_FORM     = 83170,
    SPELL_TEMPEST_STORM_AURA     = 83406,

    // Servant Of Siamat
    SPELL_THUNDER_CRASH = 84521,
    SPELL_LIGHTNING_NOVA = 84544,
    SPELL_LIGHTNING_CHARGE = 91872,
    SPELL_LIGHTNING_CHARGE_AURA = 93959,

    // Cloud
    SPELL_CLOUD_BURST_VISUAL = 83048, // Aura Cloud
    SPELL_CLOUD_BURST = 83051,

    // Wind Tunnel
    SPELL_SLIPSTREAM = 85016,
    SPELL_SLIPSTREAM_IMMEDIATE = 85017,
    SPELL_EJECT_ALL_PASSENGERS = 79737,

    //tamer 
    SPELL_FERAL_SPIRIT_LINK = 84230,
    SPELL_FERAL_SPIRIT_LINK_TRIGGER = 84231
};

enum Events
{
    // Siamat Event
    EVENT_DEFLECTING_WINDS              = 1,
    EVENT_STORM_BOLT_RANDOM             = 2,
    EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT  = 3,
    EVENT_SUMMON_NPC_MINION_OF_SIAMAT   = 4,
    EVENT_ABSORB_STORMS                 = 5,
    EVENT_CLOUD_BURST_SUMMON            = 6,
    EVENT_WAILING_WINDS                 = 7,

    // Servant Of Siamat Event
    EVENT_THUNDER_CRASH                 = 8,
    EVENT_LIGHTNING_NOVA                = 9,

    // Cloud Event
    EVENT_CLOUD_BURST                   = 10,
    EVENT_DESPAWN_CLOUD_BURST           = 11,

    // tempest storm
    EVENT_UPDATE_SCALE                  = 12,

    // wind tunnel
    EVENT_EJECT_PASSANGERS              = 13,

    //tamer
    EVENT_FERAL_SPIRIT_LINK             = 14,
    EVENT_DELAYED_DEATH                 = 15,
    EVENT_RAIN_CHECK,
    EVENT_SERVANT_Z_CHECK,
};

enum Actions
{
    ACTION_ENTER_TRANSITION = 1,
    ACTION_FINISH_TRANSITION = 2,
};

enum Phases
{
    PHASE_ALL = 0,
    PHASE_ONE = 1,
    PHASE_TWO = 2
};

#define ACHIEVEMENT_HEADED_SOUTH    5292
#define    FLOR_COORD_Z    36.0f

class boss_siamat : public CreatureScript
{
public:
    boss_siamat() : CreatureScript("boss_siamat") {}

    struct boss_siamatAI : public BossAI
    {
        boss_siamatAI(Creature* creature) : BossAI(creature, DATA_SIAMAT), summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        Position Home = {-10948.834961f, -1400.076172f, 35.894650f, 0.336093f};
        void Reset()
        {
            _Reset();
            me->SetHomePosition(Home);
            killCount = 0;
            events.Reset();
            summons.DespawnAll();

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            instance->SetData(DATA_SIAMAT_EVENT, NOT_STARTED);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE); // idk why siamat is moving idle WITHOUT movement id and movetype...
            SendRain(false);
            me->NearTeleportTo(Home);
        }

        void EnterCombat(Unit* /*victim*/)
        {
            _EnterCombat();
            me->SetHomePosition(Home);

            Talk(SAY_AGGRO);
            DoCast(me, SPELL_TEMPEST_STORM_ROOT);
            SendRain(true);
            DoZoneInCombat();

            events.SetPhase(PHASE_ONE);
            events.ScheduleEvent(EVENT_DEFLECTING_WINDS, 1500, 0, PHASE_ONE);
            events.ScheduleEvent(EVENT_STORM_BOLT_RANDOM, urand(3000, 8000), 0, PHASE_ONE);
            events.ScheduleEvent(EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT, 3000, 0, PHASE_ONE);
            events.ScheduleEvent(EVENT_CLOUD_BURST_SUMMON, urand(10000, 15000), 0, PHASE_ALL);
            events.ScheduleEvent(EVENT_SUMMON_NPC_MINION_OF_SIAMAT, 5000, 0, PHASE_ALL);
            events.ScheduleEvent(EVENT_RAIN_CHECK, 20000, 0, PHASE_ALL);
            instance->SetData(DATA_SIAMAT_EVENT, IN_PROGRESS);

            std::list<Creature*> list_of_npcs;
            GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_WIND_TUNNEL, 200.0f);
            if (list_of_npcs.size())
                for (auto c : list_of_npcs)
                {
                    c->SetVisible(true);
                    c->AI()->DoAction(ACTION_ENTER_TRANSITION);
                }
        }


        void SendRain(bool apply) const
        {
            WorldPacket data(SMSG_WEATHER, 9);
            data << uint32(apply ? WEATHER_STATE_HEAVY_RAIN : WEATHER_STATE_FINE);
            data << float(0.9f);
            data << uint8(0);

            auto players = me->GetPlayersInRange(40.f, true);
            if (players.size())
                for (Player* player : players)
                    //if (player->isInCombat())
                {
                    player->GetSession()->SendPacket(&data);
                    if (apply)
                    {
                        if (!player->HasAura(82651))
                            me->AddAura(82651, player); //This type of aura is checked for by the weather management system before it updates players' weather.
                    }
                    else
                        player->RemoveAura(82651);
                }
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            SendRain(false);
            Talk(SAY_DEATH);
            summons.DespawnAll();

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            instance->SetData(DATA_SIAMAT_EVENT, DONE);

            if (IsHeroic())
            {
                AchievementEntry const* headedSouth = sAchievementStore.LookupEntry(ACHIEVEMENT_HEADED_SOUTH);
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                {
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (auto player = itr->getSource())
                            if (player->GetAuraCount(SPELL_LIGHTNING_CHARGE_AURA) == 3)
                                player->CompletedAchievement(headedSouth);
                    }
                }
                instance->FinishLfgDungeon(me);
            }
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_KILL);
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            summon->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            if (summon->GetEntry() == NPC_SERVANT_OF_SIAMAT)
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon);
                Position pos;
                summon->GetPosition(&pos);
                pos.m_positionZ = me->GetPositionZ() + 1.f;
                summon->NearTeleportTo(pos);
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            Position summonPos = { summon->GetPositionX(), summon->GetPositionY(), me->GetPositionZ(), summon->GetOrientation() };
            switch (summon->GetEntry())
            {
            case NPC_SERVANT_OF_SIAMAT:
            case NPC_SERVANT_OF_SIAMAT_HC:
                summon->DisableMovementFlagUpdate(true);
                summon->NearTeleportTo(summonPos);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, summon);
                killCount++;
                if (killCount > 2 && events.IsInPhase(PHASE_ONE))
                {
                    DoAction(ACTION_ENTER_TRANSITION);
                }
                break;
            default:
                break;
            }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
            case ACTION_INTRO:
                Talk(SAY_INTRO);
                break;
            case ACTION_ENTER_TRANSITION:
                //me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
                events.Reset();
                me->CastStop();
                me->ClearUnitState(UNIT_STATE_CASTING);
                me->CastSpell(me, SPELL_WAILING_WINDS_AURA, false);
                //events.ScheduleEvent(EVENT_WAILING_WINDS, 1000, 0, PHASE_TWO);
                break;
            case ACTION_FINISH_TRANSITION:
                //me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);

                me->RemoveAurasDueToSpell(SPELL_DEFLECTING_WINDS);
                me->RemoveAurasDueToSpell(SPELL_TEMPEST_STORM_ROOT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.SetPhase(PHASE_TWO);
                events.ScheduleEvent(EVENT_STORM_BOLT_RANDOM, 1);
                events.ScheduleEvent(EVENT_ABSORB_STORMS, 4000, 0, PHASE_TWO);
                events.ScheduleEvent(EVENT_CLOUD_BURST_SUMMON, urand(10000, 25000), 0, PHASE_ALL);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() && events.IsInPhase(PHASE_TWO))
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DEFLECTING_WINDS:
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 468);
                    DoCast(me, SPELL_DEFLECTING_WINDS, true);
                    //me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    break;
                case EVENT_STORM_BOLT_RANDOM:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        auto list_of_players = me->GetPlayersInRange(140.f, true);
                        if (list_of_players.size())
                            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            {
                                DoCast(random_target, (events.IsInPhase(PHASE_ONE) ? SPELL_STORM_BOLT_PHASE_1 : SPELL_STORM_BOLT_PHASE_2), false);
                                me->SetFacingTo(me->GetAngle(random_target));
                            }
                        events.ScheduleEvent(EVENT_STORM_BOLT_RANDOM, 2500, 0, PHASE_ALL);
                    }
                    break;
                case EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        Talk(SAY_EVENT);
                        auto flare_dest = servant_spawns[urand(0, 14)];
                        me->CastSpell(
                            flare_dest.GetPositionX(),
                            flare_dest.GetPositionY(),
                            me->GetPositionZ() + 2.f, SPELL_STATIC_SHOCK, false, NULL, NULL, NULL);
                        events.ScheduleEvent(EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT, 45000, 0, PHASE_ONE);
                    }
                    break;
                case EVENT_SUMMON_NPC_MINION_OF_SIAMAT:
                {
                    Talk(SAY_EVENT);
                    auto flare_dest = servant_spawns[urand(0, 14)];
                    me->CastSpell(
                        flare_dest.GetPositionX(),
                        flare_dest.GetPositionY(),
                        me->GetPositionZ() + 2.f, SPELL_CALL_OF_SKY, false, NULL, NULL, NULL);
                    events.ScheduleEvent(EVENT_SUMMON_NPC_MINION_OF_SIAMAT, 30000, 0, PHASE_ONE);
                    break;
                }
                case EVENT_ABSORB_STORMS:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        std::list<Creature*> list_of_npcs;
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_MINION_OF_SIAMAT_STORM, 150.0f);
                        if (list_of_npcs.size())
                        {
                            list_of_npcs.sort(Trinity::ObjectDistanceOrderPred(me));
                            if (auto target = list_of_npcs.front())
                            {
                                DoCast(target, SPELL_ABSORB_STORMS);
                                me->SetFacingTo(me->GetAngle(target));
                            }
                        }

                        events.ScheduleEvent(EVENT_ABSORB_STORMS, 30000, 0, PHASE_TWO);
                    }
                    break;
                case EVENT_CLOUD_BURST_SUMMON:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCastRandom(SPELL_CLOUD_BURST_SUMMON, 100.0f, true);
                        auto list_of_players = me->GetPlayersInRange(140.f, true);
                        if (list_of_players.size())
                            if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                                DoCast(random_target, SPELL_CLOUD_BURST_SUMMON, true);

                        events.ScheduleEvent(EVENT_CLOUD_BURST_SUMMON, urand(15000, 20000), 0, PHASE_ALL);
                    }
                    break;
                case EVENT_RAIN_CHECK:
                    if (!me->isDead())
                        if (me->isInCombat())
                        {
                            if (events.IsInPhase(PHASE_ONE))
                                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 468);
                            SendRain(true);
                            events.ScheduleEvent(EVENT_RAIN_CHECK, 20000, 0, PHASE_ALL);
                        }
                    break;
                default:
                    break;
                }
            }

            if (events.IsInPhase(PHASE_TWO))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        EventMap events;
        SummonList summons;
        uint8 killCount;

        Position servant_spawns[15]
        {
            {-10985.553711f, -1413.457397f, 35.533749f, 0.355741f},//outside
            {-10971.304688f, -1414.716187f, 35.529911f, 0.367522f},
            {-10976.868164f, -1404.350342f, 35.528629f, 0.343960f},
            {-10976.151367f, -1386.586670f, 35.532127f, 5.645405f},
            {-10967.264648f, -1374.885864f, 35.532295f, 5.590430f},
            {-10948.993164f, -1370.923096f, 35.529846f, 4.604769f},
            {-10938.730469f, -1373.228516f, 35.529846f, 4.553720f},
            {-10941.781250f, -1361.676270f, 35.532654f, 4.581209f},
            {-10922.639648f, -1383.131714f, 35.533466f, 3.548413f},
            {-10916.950195f, -1396.615723f, 35.532406f, 3.505216f},
            {-10924.274414f, -1415.249512f, 35.529285f, 2.421366f},
            {-10929.549805f, -1421.255249f, 35.529285f, 2.433147f},
            {-10919.377930f, -1424.764160f, 35.532944f, 2.472417f},
            {-10945.981445f, -1430.399780f, 35.532860f, 1.384640f},
            {-10960.633789f, -1427.640259f, 35.531265f, 1.384640f},
        };

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_siamatAI(creature);
    }
};

class npc_minion_of_siamat : public CreatureScript
{
public:
    npc_minion_of_siamat() : CreatureScript("npc_minion_of_siamat") {}

    struct npc_minion_of_siamatAI : public Scripted_NoMovementAI
    {
        npc_minion_of_siamatAI(Creature* creature) : Scripted_NoMovementAI(creature)
        {
            instance = me->GetInstanceScript();
            me->AddAura(SPELL_DEPLETION, me);
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->SetHover(false);
        }

        void JustSummoned(Creature* summon)
        {
            if (auto siamat = Creature::GetCreature(*me, instance->GetData64(DATA_SIAMAT)))
                siamat->AI()->JustSummoned(summon);

            if (summon->GetEntry() == NPC_SERVANT_OF_SIAMAT || summon->GetEntry() == NPC_MINION_OF_SIAMAT)
            {
                Position pos(*summon);
                pos.m_positionZ = 37.f;
                summon->NearTeleportTo(pos);
            }
        }

        void DamageTaken(Unit* damageDealer, uint32& damage)
        {
            if (effect_used)
            {
                damage = 0;
            }
            else
                if (me->HealthBelowPctDamaged(5, damage))
                {
                    effect_used = true;
                    me->CastSpell(me, SPELL_TEMPEST_STORM_SUMMON, true);
                }
        }

        void UpdateAI(const uint32 diff)
        {
            if (UpdateVictim())
                DoSpellAttackIfReady(SPELL_CHAIN_LIGHTNING);

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_DELAYED_DEATH:
                    me->CastStop();
                    (killer ? killer : me)->Kill(me, false, (killer ? true : false));
                    break;
                default:
                    break;
            }

        }

    private:
        InstanceScript* instance;
        bool effect_used{ false };
        Unit* killer{ nullptr };

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_minion_of_siamatAI(creature);
    }
};


class npc_servant_of_siamat : public CreatureScript
{
public:
    npc_servant_of_siamat() : CreatureScript("npc_servant_of_siamat") {}

    struct npc_servant_of_siamatAI : public ScriptedAI
    {
        npc_servant_of_siamatAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void InitializeAI()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_SERVANT_Z_CHECK, 5000);


        }


        void JustDied(Unit* killer) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void EnterCombat(Unit* /*attacker*/)
        {
            events.Reset();
            me->SetDisableGravity(false);
            me->SetCanFly(false);
            me->SetHover(false);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            events.ScheduleEvent(EVENT_LIGHTNING_NOVA, urand(10000, 15000));
            events.ScheduleEvent(EVENT_THUNDER_CRASH, urand(15000, 20000));
        }

        void DamageTaken(Unit* damageDealer, uint32& damage)
        {
            if (effect_used)
            {
                damage = 0;
            }
            else
                if (me->HealthBelowPctDamaged(4, damage))
                {
                    effect_used = true;
                    killer = damageDealer;
                    damage = 0;
                    me->SetHealth(uint32(float(me->GetMaxHealth()) * 0.04f));
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    me->CastSpell(me, SPELL_LIGHTNING_CHARGE, false, NULL, NULL, instance->GetData64(DATA_SIAMAT));
                    events.ScheduleEvent(EVENT_DELAYED_DEATH, 2800);
                }
        }

        void UpdateAI(const uint32 diff)
        {

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_THUNDER_CRASH:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCastVictim(SPELL_THUNDER_CRASH);
                        events.ScheduleEvent(EVENT_THUNDER_CRASH, urand(15000, 20000));
                    }
                    break;
                case EVENT_LIGHTNING_NOVA:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(me, SPELL_LIGHTNING_NOVA);
                        events.ScheduleEvent(EVENT_LIGHTNING_NOVA, urand(10000, 15000));
                    }
                    break;
                case EVENT_DELAYED_DEATH:
                    me->CastStop();
                    (killer ? killer : me)->Kill(me, false, (killer ? true : false));
                    break;
                default:
                    break;
                }
            }

            if (me->HasReactState(REACT_AGGRESSIVE))
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    if (UpdateVictim())
                        DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        EventMap events;
        Unit* killer{ nullptr };
        bool effect_used{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_servant_of_siamatAI(creature);
    }
};

class npc_cloud_burst : public CreatureScript
{
public:
    npc_cloud_burst() : CreatureScript("npc_cloud_burst") {}

    struct npc_cloud_burstAI : public Scripted_NoMovementAI
    {
        npc_cloud_burstAI(Creature* creature) : Scripted_NoMovementAI(creature) {}

        void IsSummonedBy(Unit* /*summoner*/)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_ROOT);
            me->AddUnitState(UNIT_STATE_CANNOT_AUTOATTACK);

            DoCast(me, SPELL_CLOUD_BURST_VISUAL, true);
            me->CastWithDelay(3000, me, SPELL_CLOUD_BURST);
            me->CastWithDelay(4000, me, SPELL_CLOUD_BURST);
            me->CastWithDelay(5000, me, SPELL_CLOUD_BURST);
            me->DespawnOrUnsummon(7500);
        }

        /*
        void UpdateAI(const uint32 diff) override
        {
            //do nothing, this is here to prevent melee attacks.
        }
        */


    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cloud_burstAI(creature);
    }
};

class jumpDest : public BasicEvent
{
public:
    jumpDest(Unit* caster) : _caster(caster) {}

    bool Execute(uint64 execTime, uint32 /*diff*/)
    {
        if (_caster)
            _caster->CastSpell(_caster, SPELL_SLIPSTREAM);
        return true;
    }

private:
    Unit* _caster;
};

class npc_wind_tunnel : public CreatureScript
{
public:
    npc_wind_tunnel() : CreatureScript("npc_wind_tunnel") { }

    struct npc_wind_tunnelAI : public ScriptedAI
    {
        npc_wind_tunnelAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);

            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {

        //X: -10939.769531 Y : -1353.072266 Z : 36.621571 Orientation : 4.502650

            if (!apply)
                return;

            who->ExitVehicle();
            if (auto c = me->FindNearestCreature(48097, 100))
            {
                who->GetMotionMaster()->MoveJump(*c, 40.f, 35.f);
                if (auto a = who->AddAura(55883, who))
                    if (auto b = who->AddAura(85063, who))
                {
                    a->SetDuration(2000);
                    a->SetMaxDuration(2000);
                    b->SetDuration(2000);
                    b->SetMaxDuration(2000);
                }
            }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_ENTER_TRANSITION:
                    forced = true;
                    break;
            }
        }
        bool ShouldBeVisible()
        {
            if (forced)
                return true;

            if (instance->IsDone(DATA_GENERAL_HUSAM_EVENT))
                if (instance->IsDone(DATA_HIGH_PROPHET_BARIM_EVENT))
                    if (instance->IsDone(DATA_LOCKMAW_EVENT))
                        if (instance->IsDone(DATA_AUGH_EVENT) || !IsHeroic())
                            return true;
            return false;
        }
        void UpdateAI(const uint32 diff)
        {
            if (me->IsVisible() != ShouldBeVisible())
                    me->SetVisible(ShouldBeVisible());
        }
    private:
        bool forced{ false };
        InstanceScript* instance{ me->GetInstanceScript() };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wind_tunnelAI(creature);
    }
};

class npc_wind_tunnel_landing_zone : public CreatureScript
{
public:
    npc_wind_tunnel_landing_zone() : CreatureScript("npc_wind_tunnel_landing_zone") { }

    struct npc_wind_tunnel_landing_zoneAI : public PassiveAI
    {
        npc_wind_tunnel_landing_zoneAI(Creature* creature) : PassiveAI(creature) 
        {
            me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_UNK1);
        }

        void PassengerBoarded(Unit* who, int8 seatId, bool apply)
        {
            if (!apply)
                return;

            events.ScheduleEvent(EVENT_EJECT_PASSANGERS, 50);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                if (eventId == EVENT_EJECT_PASSANGERS)
                    if (me->GetVehicleKit())
                        me->GetVehicleKit()->RemoveAllPassengers();
            }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wind_tunnel_landing_zoneAI(creature);
    }
};

class npc_tempest_storm : public CreatureScript
{
public:
    npc_tempest_storm() : CreatureScript("npc_tempest_storm") { }

    struct npc_tempest_stormAI : public ScriptedAI
    {
        npc_tempest_stormAI(Creature* creature) : ScriptedAI(creature) {}

        void AttackStart(Unit* /*who*/) {} // just for disable attacks/aggro
        void EnterEvadeMode() {}           // just for blocking evade mode...

        void IsSummonedBy(Unit* summoner)
        {
            summoner->ToCreature()->DespawnOrUnsummon();
            me->AddAura(SPELL_TEMPEST_STORM_FORM, me);
            me->AddAura(SPELL_TEMPEST_STORM_AURA, me);
            me->GetMotionMaster()->MoveRandom(10.0f);
        }

        void SpellHit(Unit* /*who*/, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_ABSORB_STORMS)
            {
                me->DespawnOrUnsummon(3000);
                events.ScheduleEvent(EVENT_UPDATE_SCALE, 2000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_UPDATE_SCALE:
                    me->SetObjectScale(0.1f);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap events;

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_stormAI(creature);
    }
};


class npc_oathsworn_tamer : public CreatureScript
{
public:
    npc_oathsworn_tamer() : CreatureScript("npc_oathsworn_tamer") { }

    struct npc_oathsworn_tamerAI : public ScriptedAI
    {
        npc_oathsworn_tamerAI(Creature* creature) : ScriptedAI(creature) {}

        void EnterCombat(Unit* /*attacker*/)
        {
            events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 5000);
            events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 25000);
        }

        void EnterEvadeMode()
        {
            if (!me->isAlive())
                return;

            me->DeleteThreatList();
            me->CombatStop(true);
            me->GetMotionMaster()->MoveTargetedHome();

            Reset();
        }
        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (auto axemaster = me->FindNearestCreature(NPC_OATHSWORN_AXEMASTER, 5.0f))
                if (axemaster->GetHealth() > me->GetHealth())
                    axemaster->SetHealth(axemaster->GetHealth() - damage);
            if (auto captain = me->FindNearestCreature(NPC_OATHSWORN_CAPTAIN, 5.0f))
                if (captain->GetHealth() > me->GetHealth())
                    captain->SetHealth(captain->GetHealth() - damage);
            if (auto myrmidon = me->FindNearestCreature(NPC_OATHSWORN_MYRMIDON, 5.0f))
                if (myrmidon->GetHealth() > me->GetHealth())
                    myrmidon->SetHealth(myrmidon->GetHealth() - damage);
            if (auto pathfinder = me->FindNearestCreature(NPC_OATHSWORN_PATHFINDER, 5.0f))
                if (pathfinder->GetHealth() > me->GetHealth())
                    pathfinder->SetHealth(pathfinder->GetHealth() - damage);
            if (auto scorpidkeeper = me->FindNearestCreature(NPC_OATHSWORN_SCORPID_KEEPER, 5.0f))
                if (scorpidkeeper->GetHealth() > me->GetHealth())
                    scorpidkeeper->SetHealth(scorpidkeeper->GetHealth() - damage);
            if (auto skinner = me->FindNearestCreature(NPC_OATHSWORN_SKINNER, 5.0f))
                if (skinner->GetHealth() > me->GetHealth())
                    skinner->SetHealth(skinner->GetHealth() - damage);
            if (auto darkcaster = me->FindNearestCreature(NPC_NEFERSET_DARKCASTER, 5.0f))
                if (darkcaster->GetHealth() > me->GetHealth())
                    darkcaster->SetHealth(darkcaster->GetHealth() - damage);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FERAL_SPIRIT_LINK:
                    me->CastSpell(me, SPELL_FERAL_SPIRIT_LINK, true);
                    me->RemoveAura(84231);
                    events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 25000);
                    break;
                default:
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
        return new npc_oathsworn_tamerAI(creature);
    }
};

class npc_oathsworn_tamer_wall : public CreatureScript
{
public:
    npc_oathsworn_tamer_wall() : CreatureScript("npc_oathsworn_tamer_wall") { }

    struct npc_oathsworn_tamer_wallAI : public ScriptedAI
    {
        npc_oathsworn_tamer_wallAI(Creature* creature) : ScriptedAI(creature) {}

        void EnterCombat(Unit* /*attacker*/)
        {
            events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 5000);
            events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 25000);
        }

        void EnterEvadeMode()
        {
            if (!me->isAlive())
                return;

            me->DeleteThreatList();
            me->CombatStop(true);
            me->GetMotionMaster()->MoveTargetedHome();

            Reset();
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FERAL_SPIRIT_LINK:
                    me->CastSpell(me, SPELL_FERAL_SPIRIT_LINK, true);
                    me->RemoveAura(84231);
                    events.ScheduleEvent(EVENT_FERAL_SPIRIT_LINK, 25000);
                    break;
                default:
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
        return new npc_oathsworn_tamer_wallAI(creature);
    }
};

class spell_wailing_winds : public SpellScriptLoader
{
public:
    spell_wailing_winds() : SpellScriptLoader("spell_wailing_winds") { }

    class spell_wailing_winds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_wailing_winds_SpellScript);

        void RandomJump(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            Unit* target = GetHitUnit();
            Unit* caster = GetCaster()->ToUnit();
            if (!target)
                return;

            uint8 roll = urand(0, 1);
            float angle = frand(0, M_PI);
            float SpeedXY = frand(10.0f, 30.0f);
            float SpeedZ = frand(10.0f, 15.0f);
            float x, y;

            if (auto player = target->ToPlayer())
            {
                player->SetUnderACKmountAdvanced();
                player->SetSkipOnePacketForASH(true);
            }

            bool correction{ false };

            if (auto aura = caster->GetAura(SPELL_WAILING_WINDS_AURA))
                if (aura->GetDuration() <= 500 || target->GetAreaId() != caster->GetAreaId())
                    correction = true;

            if (correction)
            {
                auto c = target->FindNearestCreature(NPC_WIND_TUNNEL, 500.f, true);
                if (target->GetExactDist2d(caster) > target->GetExactDist2d(c))
                {
                    target->SetOrientation(target->GetAngle(c));
                    target->JumpTo(20.f, 20.f, true, true);
                }
                return;
            }

            if (roll)
            {
                target->GetPosition(x, y);
                target->SetOrientation(angle);
                target->KnockbackFrom(x, y, SpeedXY, SpeedZ);
                return;
            }

                target->SetOrientation(target->GetAngle(caster));
                target->JumpTo(15.f, 15.f, true, true);
            
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_wailing_winds_SpellScript::RandomJump, EFFECT_2, SPELL_EFFECT_DUMMY);
        }
    };


    SpellScript* GetSpellScript() const
    {
        return new spell_wailing_winds_SpellScript();
    }
};

class spell_wailing_winds_aura : public SpellScriptLoader
{
public:
    spell_wailing_winds_aura() : SpellScriptLoader("spell_wailing_winds_aura") { }

    class spell_wailing_winds_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_wailing_winds_aura_AuraScript);

        bool Load()
        {
            _instance = GetCaster()->GetInstanceScript();
            return _instance != NULL;
        }

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            if (!GetCaster())
                return;

            GetCaster()->CastSpell((Unit*)NULL, SPELL_WAILING_WINDS_TRIGGER, true);
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "float script activated by remove");
            if (auto caster = GetCaster())
                if (auto me = caster->ToCreature())
                    if (auto ai = me->AI())
                    {
                            ai->DoAction(ACTION_FINISH_TRANSITION);
                    }
        }
        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_wailing_winds_aura_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            OnEffectRemove += AuraEffectRemoveFn(spell_wailing_winds_aura_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }

        InstanceScript* _instance;
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_wailing_winds_aura_AuraScript();
    }
};

class spelL_siamat_absorb_storm : public SpellScriptLoader
{
public:
    spelL_siamat_absorb_storm() : SpellScriptLoader("spelL_siamat_absorb_storm") { }

    class spelL_siamat_absorb_storm_AuraScript : public AuraScript
    {
        PrepareAuraScript(spelL_siamat_absorb_storm_AuraScript);

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "float script activated by remove");
            if (auto caster = GetCaster())
                if (auto t = GetOwner())
                    if (auto target = t->ToCreature())
                    {
                        target->DespawnOrUnsummon();
                        caster->CastSpell(caster, SPELL_GATHERED_STORM, true);
                    }
        }
        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spelL_siamat_absorb_storm_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };


    AuraScript* GetAuraScript() const
    {
        return new spelL_siamat_absorb_storm_AuraScript();
    }
};

void AddSC_boss_siamat()
{
    new boss_siamat();
    new npc_minion_of_siamat();
    new npc_servant_of_siamat();
    new npc_cloud_burst();
    new npc_wind_tunnel();
    new npc_wind_tunnel_landing_zone();
    new npc_tempest_storm();
    new spell_wailing_winds();
    new spell_wailing_winds_aura();
    new npc_oathsworn_tamer();
    new npc_oathsworn_tamer_wall();
    new spelL_siamat_absorb_storm();
}