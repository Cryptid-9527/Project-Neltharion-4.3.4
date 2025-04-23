/*Copyright (C) 2012 SkyMist Project.
*
* - ToDo: Drake respawn at boss reset after wipe.
*
* This file is NOT free software. Third-party users can NOT redistribute it or modify it :).
* If you find it, you are either hacking something, or very lucky (presuming someone else managed to hack it).
*/

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "bastion_of_twilight.h"
#include "GameObjectAI.h"

enum Yells
{
   SAY_AGGRO      = 0,
   SAY_KILL       = 1,
   SAY_KILL_2     = 2,
   SAY_DRAGON     = 0,
};

enum Spells
{
    // Proto-Behemoth
    SPELL_FIREBALL                           = 86058, // Basic spell. 1.5s cast.
    SPELL_FIREBALL_TD                        = 83862, // Time Dilation. 3s cast.
    SPELL_SCORCHING_BREATH                   = 83707,  // Triggers 83855 which needs radius.
    SPELL_FIREBALL_BARRAGE                   = 83706,  // This is channel aura, trigger 83719.
    SPELL_FIREBALL_BARRAGE_DAMAGE            = 83720,  // This with 83719 which has dummy script.
    SPELL_FIREBALL_BARRAGE_DAMAGE_TD         = 83733,  // This with 83719 which has dummy script.
    SPELL_DANCING_FLAMES                     = 84106,  // Activates Fireball Barrage, aura on activating Time Warden.
    SPELL_SUPERHEATED_BREATH                 = 83956,  // Activates Scorching Breath, aura on activating Orphaned Emerald Whelps.

    // Halfus Wyrmbreaker
    SPELL_FURIOUS_ROAR                       = 83710,  // Three times in a row. Needs radius 100 y.
    SPELL_MALEVOLENT_STRIKES                 = 39171,  // Slate Dragon awakes.
    SPELL_FRENZIED_ASSAULT                   = 83693,  // Nether Scion awakes.
    SPELL_SHADOW_WRAPPED                     = 83952,  // Gains aura when Storm Rider awakes.
    SPELL_SHADOW_NOVA                        = 83703,  // Every other 30 - 40 seconds.
    SPELL_BERSERK                            = 26662,  // 6 min enrage timer.
    SPELL_BIND_WILL                          = 83432,  // On drakes at aggro.

    // Dragon debuffs on Halfus
    SPELL_NETHER_BLINDNESS                   = 83611, // From Nether Scion.
    SPELL_CYCLONE_WINDS                      = 84092, // From Storm Rider.
    SPELL_ATROPHIC_POISON                    = 83609, // Eight stacks, from whelps.
    SPELL_TIME_DILATION                      = 83601, // From Time Warden.
    SPELL_STONE_TOUCH_NORMAL                 = 83603,
    SPELL_STONE_TOUCH_HEROIC                 = 84593, // Stone Grip, from Slate Dragon.
    SPELL_PARALYSIS                          = 84030,
    SPELL_PETRIFICATION                      = 84591,
    SPELL_DRAGONS_VENGEANCE                  = 87683, // Debuff on Halfus by dragons when they die.

    //  Dragons
    SPELL_UNRESPONSIVE_DRAGON                = 86003, // Dragons.
    SPELL_UNRESPONSIVE_WHELP                 = 86022,  // Whelps.
    SPELL_FREE_DRAGON                        = 83589
};

enum Events
{
    // Halfus
    EVENT_SHADOW_NOVA = 1,
    EVENT_FURIOUS_ROAR,
    EVENT_FURIOUS_ROAR_CAST,
    EVENT_BERSERK, // 6 min.
    // Behemoth
    EVENT_FIREBALL,
    EVENT_SCORCHING_BREATH,
    EVENT_FIREBALL_BARRAGE
};

enum Creatures
{
    NPC_PROTO_BEHEMOTH                       = 44687,
    NPC_SLATE_DRAGON                         = 44652,
    NPC_NETHER_SCION                         = 44645,
    NPC_STORM_RIDER                          = 44650,
    NPC_TIME_RIDER                           = 44797,
    NPC_ORPHANED_WHELP                       = 44641
};

struct Position const dragonPos [5] =
{
    {-272.912f, -730.280f, 904.511f, 2.377750f},
    {-277.068420f, -677.820618f, 888.086914f, 3.680423f},
    {-278.890198f, -660.204468f,888.090637f, 1.619929f},
    {-319.047f, -721.080f, 888.086f, 2.860756f},
    {-350.260f, -698.793f, 888.100f, 5.531962f}
};

struct Position const welpPos [8] =
{
    {-340.75f, -722.476f, 891.764f, 1.16937f},
    {-350.564f, -721.663f, 891.763f, 1.01229f},
    {-342.002f, -726.528f, 891.764f, 1.16937f},
    {-344.839f, -720.163f, 891.764f, 1.0821f},
    {-350.571f, -725.417f, 891.763f, 1.06465f},
    {-347.578f, -717.302f, 891.763f, 1.0472f},
    {-347.08f, -728.128f, 891.764f, 1.11701f},
    {-346.434f, -724.049f, 891.764f, 1.09956f}
};

#define HALFUS_DRAGON_WORLDSTATE    20070

class blindWillEvent : public BasicEvent
{
public:
    blindWillEvent(Creature* halfus, Creature* dragon) : _halfus(halfus), _dragon(dragon) { }

    bool Execute(uint64 /*execTime*/, uint32 /*diff*/) override
    {
        _halfus->CastSpell(_dragon, SPELL_BIND_WILL, true);
        _dragon->SetReactState(REACT_AGGRESSIVE);
        _dragon->SetInCombatWithZone();
        return true;
    }

private:
    Creature* _halfus;
    Creature* _dragon;
};

class boss_halfus : public CreatureScript
{
    public:
        boss_halfus() : CreatureScript("boss_halfus_wyrmbreaker") {}

        struct boss_halfusAI : public BossAI
        {
            boss_halfusAI(Creature* creature) : BossAI(creature, DATA_HALFUS) {}

            void Reset()
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->RemoveAllAuras();
                welpDieCount = 0;
                RoarCasts = 0;
                roarPhase = false;
                activeDrakes[0] = 0;
                activeDrakes[1] = 0;
                activeDrakes[2] = 0;
                activeDrakes[3] = 0;
                _Reset();
                initDragons();

                std::list<GameObject*> CageList;
                me->GetGameObjectListWithEntryInGrid(CageList, 205087, 100.0f);
                if (!CageList.empty())
                    for (std::list<GameObject*>::const_iterator itr = CageList.begin(); itr != CageList.end(); ++itr)
                    {
                        (*itr)->ResetDoorOrButton();
                        (*itr)->SetGoState(GO_STATE_READY);
                        (*itr)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (auto behemoth = me->FindNearestCreature(NPC_PROTO_BEHEMOTH, 200.0f, true))
                    behemoth->DespawnOrUnsummon();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                _JustDied();
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(RAND(SAY_KILL, SAY_KILL_2));
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            uint32 GetLockOrNormalDrakeEntry(uint32 entry)
            {
                uint32 LockIconDrakeEntry = 0;

                switch (entry)
                {
                case 44645:
                    LockIconDrakeEntry = 44828;
                    break;
                case 44650:
                    LockIconDrakeEntry = 44826;
                    break;
                case 44652:
                    LockIconDrakeEntry = 44829;
                    break;
                case 44797:
                    LockIconDrakeEntry = 44653;
                    break;

                case 44828:
                    LockIconDrakeEntry = 44645;
                    break;
                case 44826:
                    LockIconDrakeEntry = 44650;
                    break;
                case 44829:
                    LockIconDrakeEntry = 44652;
                    break;
                case 44653:
                    LockIconDrakeEntry = 44797;
                    break;
                }

                return LockIconDrakeEntry;
            }

            void EnterCombat(Unit* who)
            {
                Talk(SAY_AGGRO);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                events.ScheduleEvent(EVENT_BERSERK, 6 * MINUTE * IN_MILLISECONDS);
                if (auto behemoth = me->FindNearestCreature(NPC_PROTO_BEHEMOTH, 500.0f, true))
                    behemoth->SetInCombatWith(who);

                std::list<GameObject*> CageList;
                me->GetGameObjectListWithEntryInGrid(CageList, 205087, 100.0f);
                if (!CageList.empty())
                    for (std::list<GameObject*>::const_iterator itr = CageList.begin(); itr != CageList.end(); ++itr)
                        (*itr)->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                for (uint8 i = 0; i < 4; ++i)
                {
                    if (activeDrakes[i] == 0)
                        break;

                    if (auto drake = ObjectAccessor::GetCreature(*me, activeDrakes[i]))
                    {                                     
                        me->SummonCreature(GetLockOrNormalDrakeEntry(drake->GetEntry()), drake->GetPositionX(), drake->GetPositionY(), drake->GetPositionZ(), drake->GetOrientation());
                        drake->DespawnOrUnsummon();
                    }
                }

                if (me->HasAura(SPELL_SHADOW_WRAPPED))
                    events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(7000, 10000));

                _EnterCombat();
            }

            void initDragons()
            {
                me->SummonCreature(NPC_PROTO_BEHEMOTH, dragonPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_SLATE_DRAGON, dragonPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_NETHER_SCION, dragonPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_STORM_RIDER, dragonPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                me->SummonCreature(NPC_TIME_RIDER, dragonPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                for (uint8 i = 0; i < 8; ++i)
                    me->SummonCreature(NPC_ORPHANED_WHELP, welpPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                selectActiveDragons();
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_ORPHANED_WHELP:
                        welpDieCount++;
                        if (welpDieCount > 7)
                            me->AddAura(SPELL_DRAGONS_VENGEANCE, me);
                        break;
                    case NPC_SLATE_DRAGON:
                    case NPC_NETHER_SCION:
                    case NPC_STORM_RIDER:
                    case NPC_TIME_RIDER:
                        me->AddAura(SPELL_DRAGONS_VENGEANCE, me);
                        break;
                    default:
                        break;
                }
            }

            void setWhelpsUnresponsive()
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, NPC_ORPHANED_WHELP, 100.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                {
                    me->AddAura(SPELL_UNRESPONSIVE_WHELP, (*iter));
                    (*iter)->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
            }

            void selectActiveDragons()
            {
                Creature* slateDragon  = me->FindNearestCreature(NPC_SLATE_DRAGON, 100.0f, true);
                Creature* netherScion  = me->FindNearestCreature(NPC_NETHER_SCION, 100.0f, true);
                Creature* stormRider   = me->FindNearestCreature(NPC_STORM_RIDER, 100.0f, true);
                Creature* timeRider    = me->FindNearestCreature(NPC_TIME_RIDER, 100.0f, true);

                if (!IsHeroic())
                {
                    uint8 drakeCombination = sWorld->getWorldState(HALFUS_DRAGON_WORLDSTATE);

                    switch(drakeCombination)
                    {
                        case 1:  // Slate, Storm, Whelps.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, netherScion);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, timeRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = stormRider->GetGUID();
                            break;
                        case 2:  // Slate, Nether, Time.
                            setWhelpsUnresponsive();
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, stormRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = netherScion->GetGUID();
                            activeDrakes[2] = timeRider->GetGUID();
                            break;
                        case 3:  // Slate, Storm, Time.
                            setWhelpsUnresponsive();
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, netherScion);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = stormRider->GetGUID();
                            activeDrakes[2] = timeRider->GetGUID();
                            break;
                        case 4:  // Storm, Nether, Time.
                            setWhelpsUnresponsive();
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, slateDragon);
                            activeDrakes[0] = stormRider->GetGUID();
                            activeDrakes[1] = netherScion->GetGUID();
                            activeDrakes[2] = timeRider->GetGUID();
                            break;
                        case 5:  // Slate, Storm, Nether.
                            setWhelpsUnresponsive();
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, timeRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = stormRider->GetGUID();
                            activeDrakes[2] = netherScion->GetGUID();
                            break;
                        case 6:  // Slate, Whelps, Time.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, netherScion);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, stormRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = timeRider->GetGUID();
                            break;
                        case 7:  // Whelps, Nether, Time.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, slateDragon);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, stormRider);
                            activeDrakes[0] = netherScion->GetGUID();
                            activeDrakes[1] = timeRider->GetGUID();
                            break;
                        case 8:  // Storm, Whelps, Time.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, netherScion);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, slateDragon);
                            activeDrakes[0] = stormRider->GetGUID();
                            activeDrakes[1] = timeRider->GetGUID();
                            break;
                        case 9:  // Storm, Whelps, Nether.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, timeRider);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, slateDragon);
                            activeDrakes[0] = stormRider->GetGUID();
                            activeDrakes[1] = netherScion->GetGUID();
                            break;
                        case 10: // Slate, Whelps, Nether.
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, timeRider);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, stormRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = netherScion->GetGUID();
                            break;
                        default:
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, netherScion);
                            me->AddAura(SPELL_UNRESPONSIVE_DRAGON, timeRider);
                            activeDrakes[0] = slateDragon->GetGUID();
                            activeDrakes[1] = stormRider->GetGUID();
                            break;
                    }
                }
                else
                {
                    activeDrakes[0] = slateDragon->GetGUID();
                    activeDrakes[1] = stormRider->GetGUID();
                    activeDrakes[2] = netherScion->GetGUID();
                    activeDrakes[3] = timeRider->GetGUID();
                }

                if (!slateDragon->HasAura(SPELL_UNRESPONSIVE_DRAGON))
                    me->AddAura(SPELL_MALEVOLENT_STRIKES, me);
                else slateDragon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (!netherScion->HasAura(SPELL_UNRESPONSIVE_DRAGON))
                    me->AddAura(RAID_MODE(83693, 86160, 86161, 86162), me);
                else netherScion->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (!stormRider->HasAura(SPELL_UNRESPONSIVE_DRAGON))
                    me->AddAura(SPELL_SHADOW_WRAPPED, me);
                else stormRider->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (auto behemoth = me->FindNearestCreature(NPC_PROTO_BEHEMOTH, 100.0f, true))
                {
                    if (auto welp = me->FindNearestCreature(NPC_ORPHANED_WHELP, 100.0f, true))
                        if (!welp->HasAura(SPELL_UNRESPONSIVE_WHELP))
                            behemoth->AddAura(SPELL_SUPERHEATED_BREATH, behemoth);

                    if (!timeRider->HasAura(SPELL_UNRESPONSIVE_DRAGON))
                        behemoth->AddAura(SPELL_DANCING_FLAMES, behemoth);
                    else timeRider->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override
            {
                if (HealthBelowPct(51) && !roarPhase)
                {
                    events.CancelEvent(EVENT_SHADOW_NOVA);
                    events.ScheduleEvent(EVENT_FURIOUS_ROAR, 2000);
                    roarPhase = true;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_SHADOW_NOVA:
                            DoCast(me, SPELL_SHADOW_NOVA);
                            if(!roarPhase)
                                events.ScheduleEvent(EVENT_SHADOW_NOVA, 7200);
                            break;
                        case EVENT_FURIOUS_ROAR:
                            RoarCasts = 3;
                            events.ScheduleEvent(EVENT_FURIOUS_ROAR_CAST, 1500);
                            break;
                        case EVENT_FURIOUS_ROAR_CAST:
                            if (RoarCasts > 0)
                            {
                                DoCast(me, SPELL_FURIOUS_ROAR);
                                --RoarCasts;

                                if (RoarCasts == 0)
                                {
                                    if (me->HasAura(SPELL_SHADOW_WRAPPED))
                                        events.ScheduleEvent(EVENT_SHADOW_NOVA, 1000);
                                    events.ScheduleEvent(EVENT_FURIOUS_ROAR, 30000);
                                }
                                else events.ScheduleEvent(EVENT_FURIOUS_ROAR_CAST, 1700);
                                                   
                            }
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                    }
                }

                if(RoarCasts == 0 && (!roarPhase || !events.HasEvent(EVENT_SHADOW_NOVA)))
                    DoMeleeAttackIfReady();
            }
            private:
                bool roarPhase, dragonsPicked;
                uint8 welpDieCount;
                uint8 RoarCasts;
                uint64 activeDrakes[4];
        };
        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_halfusAI(creature);
        }

};

class npc_proto_behemoth : public CreatureScript
{
    public:
        npc_proto_behemoth() : CreatureScript("npc_proto_behemoth") {}

        struct npc_proto_behemothAI : public ScriptedAI
        {
            npc_proto_behemothAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCanFly(true);
                me->SetDisableGravity(true);
            }

            void Reset()
            {
                events.Reset();
                me->RemoveAllAuras();
                me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
            }

            void AttackStart(Unit* target)
            {
                DoStartNoMovement(target);
            }

            void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_FIREBALL, 3000);

                if (!me->HasAura(SPELL_DANCING_FLAMES) && me->HasAura(SPELL_SUPERHEATED_BREATH))
                    events.ScheduleEvent(EVENT_SCORCHING_BREATH, urand(12000, 17000));

                if (me->HasAura(SPELL_DANCING_FLAMES))
                    events.ScheduleEvent(EVENT_FIREBALL_BARRAGE, urand(12000, 17000));
            }

            void UpdateAI(const uint32 diff)
            {
                if (!me->isInCombat())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_FIREBALL:
                            if (auto halfus = Creature::GetCreature(*me, instance->GetData64(NPC_HALFUS_WORMBREAKER))) // innerhit halfu's threat list
                            {
                                if (auto target = halfus->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                {
                                    if (!me->HasAura(SPELL_TIME_DILATION))
                                        DoCast(target, SPELL_FIREBALL);
                                    else
                                        DoCast(target, SPELL_FIREBALL_TD);
                                }
                            }
                            if (!me->HasAura(SPELL_TIME_DILATION))
                                events.ScheduleEvent(EVENT_FIREBALL, 1500);
                            else
                                events.ScheduleEvent(EVENT_FIREBALL, 3000);
                            break;
                        case EVENT_SCORCHING_BREATH:
                            events.DelayEvents(10000); // for some reason "if (me->HasUnitState(UNIT_STATE_CASTING))" check isnt working for this spell...
                            DoCast(me, SPELL_SCORCHING_BREATH, true);
                            if (!me->HasAura(SPELL_DANCING_FLAMES))
                                events.ScheduleEvent(EVENT_SCORCHING_BREATH, 25000);
                            break;
                        case EVENT_FIREBALL_BARRAGE:
                            events.DelayEvents(10000); // for some reason "if (me->HasUnitState(UNIT_STATE_CASTING))" check isnt working for this spell...
                            DoCast(me, SPELL_FIREBALL_BARRAGE, true);
                            if (me->HasAura(SPELL_SUPERHEATED_BREATH))
                                events.ScheduleEvent(EVENT_SCORCHING_BREATH, 10000);
                            events.ScheduleEvent(EVENT_FIREBALL_BARRAGE, 25000);
                            break;
                    }
                }
            }
            private:
                InstanceScript* instance;
                EventMap events;
        };
        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_proto_behemothAI(creature);
        }
};

class npc_halfus_dragon: public CreatureScript
{
public:
    npc_halfus_dragon () : CreatureScript("npc_halfus_dragon") { }

    struct npc_halfus_dragonAI: public ScriptedAI
    {
        npc_halfus_dragonAI (Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->SetReactState(REACT_PASSIVE);
            me->DisableMovementFlagUpdate(true);
            Activate = false;
        }

        void SetData(uint32 /*id*/, uint32 /*value*/)
        {
            Activate = true;
        }

        void UpdateAI (const uint32 /*diff*/) override
        {
            if (Activate)
            {
                if (auto Halfus = me->FindNearestCreature(NPC_HALFUS_WORMBREAKER, 500.0f, true))
                {
                    Talk(SAY_DRAGON);
                    switch (me->GetEntry())
                    {
                    case NPC_SLATE_DRAGON:
                        if (IsHeroic())
                            DoCast(Halfus, SPELL_STONE_TOUCH_HEROIC, true);
                        else
                            DoCast(Halfus, SPELL_STONE_TOUCH_NORMAL, true);
                        break;
                    case NPC_NETHER_SCION:
                        DoCast(Halfus, SPELL_NETHER_BLINDNESS, true);
                        break;
                    case NPC_STORM_RIDER:
                        DoCast(Halfus, SPELL_CYCLONE_WINDS, true);
                        break;
                    case NPC_TIME_RIDER:
                        if (auto behemoth = me->FindNearestCreature(NPC_PROTO_BEHEMOTH, 100.0f, true))
                            DoCast(behemoth, SPELL_TIME_DILATION, true);
                        break;
                    }

                    Halfus->m_Events.AddEvent(new blindWillEvent(Halfus, me), Halfus->m_Events.CalculateTime(1500));
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->SetHover(true);
                }

                Activate = false;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        bool Activate;
    };

    CreatureAI* GetAI (Creature* creature) const
    {
        return new npc_halfus_dragonAI(creature);
    }
};

class npc_halfus_dragon_lock : public CreatureScript
{
public:
    npc_halfus_dragon_lock() : CreatureScript("npc_halfus_dragon_lock") { }

    struct npc_halfus_dragon_lockAI : public ScriptedAI
    {
        npc_halfus_dragon_lockAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->SetReactState(REACT_PASSIVE);
        }

        uint32 GetLockOrNormalDrakeEntry(uint32 entry)
        {
            uint32 LockIconDrakeEntry = 0;

            switch (entry)
            {
            case 44645:
                LockIconDrakeEntry = 44828;
                break;
            case 44650:
                LockIconDrakeEntry = 44826;
                break;
            case 44652:
                LockIconDrakeEntry = 44829;
                break;
            case 44797:
                LockIconDrakeEntry = 44653;
                break;

            case 44828:
                LockIconDrakeEntry = 44645;
                break;
            case 44826:
                LockIconDrakeEntry = 44650;
                break;
            case 44829:
                LockIconDrakeEntry = 44652;
                break;
            case 44653:
                LockIconDrakeEntry = 44797;
                break;
            }

            return LockIconDrakeEntry;
        }

        void SpellHit(Unit* /*target*/, SpellInfo const* spellInfo) override
        {
            if (spellInfo->Id == SPELL_FREE_DRAGON)
            {
                if (auto halfus = Creature::GetCreature(*me, instance->GetData64(NPC_HALFUS_WORMBREAKER)))
                    if (auto drake = halfus->SummonCreature(GetLockOrNormalDrakeEntry(me->GetEntry()), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                        drake->AI()->SetData(1, 1);

                me->DespawnOrUnsummon();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_halfus_dragon_lockAI(creature);
    }
};

class npc_orphaned_whelp : public CreatureScript
{
public:
    npc_orphaned_whelp() : CreatureScript("npc_orphaned_whelp") { }

    struct npc_orphaned_whelpAI : public ScriptedAI
    {
        npc_orphaned_whelpAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset ()
        {
            me->DeleteThreatList();
            me->CombatStop(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (id == 1 && type == POINT_MOTION_TYPE)
            {
                if (auto Halfus = me->FindNearestCreature(NPC_HALFUS_WORMBREAKER, 150.0f, true))
                {
                    if (!me->HasAura(SPELL_UNRESPONSIVE_WHELP))
                    {
                        if (auto behemoth = me->FindNearestCreature(NPC_PROTO_BEHEMOTH, 100.0f, true))
                            DoCast(behemoth, SPELL_ATROPHIC_POISON, true);

                        if (!Halfus->isInCombat())
                            Halfus->AI()->DoZoneInCombat(Halfus, 150.0f);
                        Halfus->CastSpell(me, SPELL_BIND_WILL, true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetInCombatWithZone();
                    }
                }
            }
        }

        void UpdateAI(uint32 const /*diff*/) override
        {
            if (me->HasAura(83432) && !me->getVictim())
                if (auto player = me->FindNearestPlayer(500.0f))
                    AttackStart(player);

            DoMeleeAttackIfReady();
        }

        private:
            InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orphaned_whelpAI(creature);
    }
};

class spell_proto_fireball : public SpellScriptLoader // 86058, 83862
{
public:
    spell_proto_fireball() : SpellScriptLoader("spell_proto_fireball") { }

    class spell_proto_fireballSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_proto_fireballSpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();

            if (caster->HasAura(SPELL_TIME_DILATION))
                caster->CastSpell(GetHitUnit(), SPELL_FIREBALL_BARRAGE_DAMAGE_TD, false);
            else
                caster->CastSpell(GetHitUnit(), SPELL_FIREBALL_BARRAGE_DAMAGE, false);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_proto_fireballSpellScript::HandleDummy,EFFECT_0,SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_proto_fireballSpellScript();
    }
};

class spell_proto_fireball_barrage : public SpellScriptLoader // 83719.
{
public:
    spell_proto_fireball_barrage() : SpellScriptLoader("spell_proto_fireball_barrage") { }

    class spell_proto_fireball_barrageSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_proto_fireball_barrageSpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (auto instance = caster->GetInstanceScript())
                if (auto halfus = Creature::GetCreature(*caster, instance->GetData64(NPC_HALFUS_WORMBREAKER)))
                    if (auto target = halfus->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        caster->CastSpell(target, caster->HasAura(SPELL_TIME_DILATION) ? SPELL_FIREBALL_BARRAGE_DAMAGE_TD : SPELL_FIREBALL_BARRAGE_DAMAGE, true);
        }

        void Register() override
        {
            OnEffectHit += SpellEffectFn(spell_proto_fireball_barrageSpellScript::HandleDummy,EFFECT_0,SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const override
    {
        return new spell_proto_fireball_barrageSpellScript();
    }
};

// 84593
class spell_halfus_stone_touch: public SpellScriptLoader
{
public:
    spell_halfus_stone_touch() : SpellScriptLoader("spell_halfus_stone_touch") { }

    class spell_halfus_stone_touch_AuraScript: public AuraScript
    {
        PrepareAuraScript(spell_halfus_stone_touch_AuraScript)

        void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
        {
            Unit* halfus = GetTarget();
            if (GetId() == SPELL_STONE_TOUCH_NORMAL)
                halfus->CastSpell(halfus, SPELL_PARALYSIS, true);
            else if (GetId() == SPELL_STONE_TOUCH_HEROIC)
                halfus->CastSpell(halfus, SPELL_PETRIFICATION, true);
        }

        void Register() override
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_halfus_stone_touch_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_halfus_stone_touch_AuraScript();
    }
};

class go_halfus_whelp_cage : public GameObjectScript
{
    public:
        go_halfus_whelp_cage() : GameObjectScript("go_halfus_whelp_cage") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, go, NPC_ORPHANED_WHELP, 50.0f);

            if (creatures.empty())
               return false;

            for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                (*iter)->GetMotionMaster()->MovePoint(1, -333.163544f, -707.808777f, 888.096558f);

            go->UseDoorOrButton();
            player->PlayerTalkClass->ClearMenus();

            return true;
        }

        struct go_halfus_whelp_cageAI : public GameObjectAI
        {
            go_halfus_whelp_cageAI(GameObject* gameobject) : GameObjectAI(gameobject) { isHalfus = false; _halfus = nullptr; }

            void UpdateAI(uint32 /*diff*/)
            {
                if (!isHalfus)
                {
                    if (auto halfus = Creature::GetCreature(*go, go->GetInstanceScript()->GetData64(NPC_HALFUS_WORMBREAKER)))
                        _halfus = halfus;
                    isHalfus = true;
                }
                else
                {
                    if (!_halfus->isInCombat())
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }                    
            }

        private:
            bool isHalfus;
            Creature* _halfus;
        };

        GameObjectAI* GetAI(GameObject* gameobject) const
        {
            return new go_halfus_whelp_cageAI(gameobject);
        }
};

class spell_halfus_malevolent_strikes : public SpellScriptLoader
{
public:
    spell_halfus_malevolent_strikes() : SpellScriptLoader("spell_halfus_malevolent_strikes") { }

    class spell_halfus_malevolent_strikes_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_halfus_malevolent_strikes_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetHitMask() & PROC_EX_NORMAL_HIT || eventInfo.GetHitMask() & PROC_EX_CRITICAL_HIT)
                return true;
            return false;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_halfus_malevolent_strikes_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_halfus_malevolent_strikes_AuraScript();
    }
};

enum chogall_halfus_events
{
    EVENT_ANIMS = 1,
    ACTION_RECOGNIZE_MOB_KILL,
    EVENT_SUMMON_ELEMENTAL,
};

enum chogall_wave_paths
{
    PATH_STAIRS_EAST_TOP    = 2284580,
    PATH_STAIRS_EAST_SECOND = 2284581,
    PATH_STAIRS_EAST_THIRD  = 2284582,
    PATH_STAIRS_EAST_BOTTOM = 2284583,

    PATH_STAIRS_CENT_TOP    = 2284584,
    PATH_STAIRS_CENT_SECOND = 2284585,
    PATH_STAIRS_CENT_THIRD  = 2284586,
    PATH_STAIRS_CENT_BOTTOM = 2284587,

    PATH_STAIRS_WEST_TOP    = 2284588,
    PATH_STAIRS_WEST_SECOND = 2284589,
    PATH_STAIRS_WEST_THIRD  = 2284590,
    PATH_STAIRS_WEST_BOTTOM = 2284591,
};

class npc_chogall_after_halfus : public CreatureScript
{
public:
    npc_chogall_after_halfus() : CreatureScript("npc_chogall_after_halfus") { }


    struct npc_chogall_after_halfusAI : public ScriptedAI
    {

        npc_chogall_after_halfusAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
            SetupWaves();
            events.ScheduleEvent(EVENT_ANIMS, 500);
            events.ScheduleEvent(EVENT_SUMMON_ELEMENTAL, 10000);
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            events.ScheduleEvent(EVENT_SUMMON_ELEMENTAL, 5000);
            summons.DespawnAll();
        }

        enum spells_and_events
        {
            EVENT_NEVER_USE = 0,/*BEGIN SPELL EVENTS - EVENT 0 CAN NEVER BE EXECUTED*/
        };


        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() == NPC_CHOSEN_SEER || summon->GetEntry() == NPC_CHOSEN_WARRIOR)
            {
                mob_count++;
                if (mob_count == 12)
                    if (!waves[1])
                    {
                        waves[0] = true;
                        waves[1] = true;
                        SendWave(2);
                    }
            }
        }


        void MoveInLineOfSight(Unit* who)
        {
            if (who->ToPlayer())
                if (me->GetExactDist(who) <= 100.f)
                    if (!waves[0])
                        DoAction(55);
        }

        void DoAction(int32 const act)
        {
            switch (act)
            {
            case 55:
                if (!waves[0])
                {

                    waves[0] = true;
                    Talk(0, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);

                    instance->DoOnPlayers([this](Player* player)
                        {
                            player->PlayDirectSound(22057);
                        });
                    SendWave(1);
                }
                break;
            case 66:
                if (!waves[1])
                    SetupWaves();
                break;
            default:
                break;
            }
        }

        void SetupWaves()
        {
            if (me->isDead())
                return;
            summons.DespawnAll();
            waves[0] = false;
            waves[1] = false;
            waves[2] = false;
            mob_count = 0;

            for (auto i = 0; i <= 22; ++i)
                if (auto c = me->SummonCreature(stairs_waves[i].entry, stairs_waves[i].spawn_pos))
                    stairs_wave_guids[i] = c->GetGUID();
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);
        }

        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 5)
            switch (summon->GetEntry())
            {
            case NPC_TWILIGHT_ELEMENTALIST:
            case NPC_ELEMENTAL_FIRELORD:
            case NPC_CRIMSONBORNE_FIRESTARTER:
            case NPC_AZUREBORNE_DESTROYER:
            case NPC_WIND_BREAKER:
            case NPC_EARTH_RAVAGER:
                //TC_LOG_ERROR("sql.sql", "creature: %u, type: %u, id: %u", summon->GetEntry(), type, id);
                summon->SetFacingTo(0.001f);
                break;
            }
        }

        void SendWave(int8 wave_num)
        {
            switch (wave_num)
            {
            case 1:
                for (uint32 itr = 0; itr <= 11; ++itr)
                    if (auto c = instance->instance->GetCreature(stairs_wave_guids[itr]))
                    {
                        c->SetStandState(UNIT_STAND_STATE_STAND);
                        c->GetMotionMaster()->MovePath(uint32(PATH_STAIRS_EAST_TOP) + itr, false);
                    }
                break;
            case 2:
                me->GetMotionMaster()->MovePoint(1, retreat_to_despawn);
                me->DespawnOrUnsummon(15000);
                Talk(2, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                for (uint32 itr = 12; itr <= 21; ++itr)
                    if (auto c = instance->instance->GetCreature(stairs_wave_guids[itr]))
                    {
                        c->SetStandState(UNIT_STAND_STATE_STAND);
                        c->GetMotionMaster()->MovePoint(5, wave_2_walk_to[itr - 12], true, 5.f);
                    }
                break;
            default:
                TC_LOG_ERROR("sql.sql", "npc_chogall_after_halfus:: sendwave has invalid value %u", wave_num);
                break;
            }
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ANIMS:
                {
                    for (auto itr = 0; itr <= 22; ++itr)
                        if (auto c = instance->instance->GetCreature(stairs_wave_guids[itr]))
                        {
                            c->SetFacingTo(c->GetAngle(me));
                            c->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }

                    //"me" is the npc executing this script. This argument specifies the centerpoint of the circle to check.
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, 47081, 100.0f);
                    if (list_of_npcs.size())
                        for (auto c : list_of_npcs)
                            if (c->GetDBTableGUIDLow())
                                c->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_SPELL_CHANNEL_OMNI);
                    break;
                }
                case EVENT_SUMMON_ELEMENTAL:
                    if (me->isAlive() || me->FindNearestCreature(NPC_TWILIGHT_ELEMENTALIST, 15.f))
                    {
                        me->SummonCreature(RAND(47016, 47017), elemental_spawns[urand(0, 1)]);
                        events.ScheduleEvent(EVENT_SUMMON_ELEMENTAL, 30000);
                    }
                    break;
                default:
                    break;
                }
        }

    private:
         uint64 stairs_wave_guids[22]
        {
             0, 0, 0, 0, 0,
             0, 0, 0, 0, 0,
             0, 0, 0, 0, 0,
             0, 0, 0, 0, 0,
             0, 0,
        };

         struct wave_data
         {
             uint32     entry;
             Position   spawn_pos;
         };

         wave_data stairs_waves[22]
         {
             //wave 1
             {NPC_CHOSEN_WARRIOR,           {-661.294f,    -708.586f,  834.631f, 1.65170f} },    //chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-656.177f,    -697.469f,  834.631f, 2.18977f} },    //Chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-649.062f,    -697.007f,  834.631f, 2.62567f} },    //chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-649.647f,    -690.74f,   834.632f, 2.77898f} },    //chosen warrior
             {NPC_CHOSEN_SEER,              {-649.274f,    -684.731f,  834.631f, 3.16761f} },    //chosen seer
             {NPC_CHOSEN_SEER,              {-642.961f,    -684.779f,  834.632f, 3.15650f} },    //chosen seer
             {NPC_CHOSEN_SEER,              {-661.000f,    -667.928f,  834.631f, 4.63629f} },    //chosen seer
             {NPC_CHOSEN_SEER,              {-666.140f,    -665.689f,  834.631f, 4.94651f} },    //chosen seer
             {NPC_CHOSEN_WARRIOR,           {-649.157f,    -679.232f,  834.631f, 3.55640f} },    //chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-648.863f,    -672.535f,  834.631f, 3.83520f} },    //chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-656.076f,    -671.765f,  834.631f, 4.22398f} },    //chosen warrior
             {NPC_CHOSEN_WARRIOR,           {-663.204f,    -661.826f,  834.631f, 4.72262f} },    //chosen warrior

             //wave 2
             {NPC_TWILIGHT_ELEMENTALIST,    {-665.867f,    -704.287f,  834.631f, 1.59287f} },    //Twilight Elementalist
             {NPC_TWILIGHT_ELEMENTALIST,    {-660.520f,    -702.75f,   834.631f, 1.70282f} },    //twilight elementalist
             {NPC_ELEMENTAL_FIRELORD,       {-640.084f,    -707.964f,  834.632f, 2.32725f} },    //elemental firelord
             {NPC_WIND_BREAKER,             {-645.809f,    -709.924f,  834.633f, 2.24086f} },    //Wind breaker
             {NPC_EARTH_RAVAGER,            {-651.094f,    -714.024f,  834.633f, 2.19766f} },    //Eart Ravager

             {NPC_TWILIGHT_BRUTE,           {-649.047f,    -666.667f,  834.631f, 3.92551f} },    //ogre
             {NPC_TWILIGHT_BRUTE,           {-656.485f,    -660.475f,  834.631f, 4.34569f} },    //ogre
             {NPC_AZUREBORNE_DESTROYER,     {-639.841f,    -661.192f,  834.631f, 3.91374f} },    //lizardman
             {NPC_CRIMSONBORNE_FIRESTARTER, {-644.442f,    -658.074f,  834.631f, 4.04333f} },    //lizardman
             {NPC_AZUREBORNE_DESTROYER,     {-649.583f,    -654.472f,  834.631f, 4.15721f} },    //lizardman
         };

         Position wave_2_walk_to[10]
         {
             {-668.119812f, -686.555420f, 834.631531f, 6.265022f},
             {-667.633606f, -682.578247f, 834.631531f, 6.276796f},
             {-663.365845f, -685.123352f, 834.631531f, 6.241447f},
             {-670.492432f, -680.055542f, 834.634949f, 0.162468f},
             {-672.089600f, -690.703308f, 834.638123f, 0.017161f},

             {-610.886108f, -680.681213f, 834.632690f, 6.190386f},
             {-611.467896f, -686.933167f, 834.632690f, 6.268922f},
             {-605.068848f, -679.810913f, 834.632690f, 0.123191f},
             {-600.607117f, -684.869812f, 834.632690f, 6.253223f},
             {-604.117676f, -689.793457f, 834.632690f, 6.268932f},
         };

         Position elemental_spawns[2]
         {
             {-670.439941f, -676.839294f, 834.634338f,  0.048348f},
             {-669.667114f, -693.536743f, 834.632935f, 0.005151f}
         };

         Position retreat_to_despawn
         {
             -791.135986f, -685.197937f, 831.897400f, 3.166429f
         };

         uint8 mob_count{ 0 };
         bool waves[3]
         {
             false,
             false,
             false
         };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        SummonList summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chogall_after_halfusAI(creature);
    }
};

class npc_BOT_chosen_seer : public CreatureScript
{
public:
    npc_BOT_chosen_seer() : CreatureScript("npc_BOT_chosen_seer") { }


    struct npc_BOT_chosen_seerAI : public ScriptedAI
    {

        npc_BOT_chosen_seerAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void IsSummonedBy(Unit* summoner)
        {
            if (auto c = summoner->ToCreature())
                if (auto ai = c->AI())
                    ai->JustSummoned(me);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == WAYPOINT_MOTION_TYPE)
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H_ALLOW_MOVEMENT);
                me->SetHomePosition(*me);
            }

            if (auto chogall = instance->instance->GetCreature(instance->GetData64(NPC_CHOGALL_HALFUS)))
                if (me->GetExactDist(chogall) > 350.f)
                    chogall->AI()->DoAction(66);

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

            Creature* creature = new Creature();
            if (!creature->LoadCreatureFromDB(228458, me->GetMap(), false))
            {
                delete creature;
            }
            else
            {
                instance->DoOnPlayers([this](Player* player)
                    {
                        if (auto dw = instance->GetCreature(instance->GetData64(NPC_CHOGALL_HALFUS)))
                        {
                            UpdateData transData(player->GetMapId());
                            dw->BuildCreateUpdateBlockForPlayer(&transData, player);
                            WorldPacket packet;
                            transData.BuildPacket(&packet);

                            if (auto s = player->GetSession())
                                s->SendPacket(&packet);
                        }
                    });
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

            EVENT_FLAME_SHOCK,
            EVENT_FIREBOLT,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_FLAME_SHOCK = 87707,
            SPELL_FIREBOLT = 87710,
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

            {EVENT_FLAME_SHOCK,     SPELL_FLAME_SHOCK,      13000,          30000,          true,       true},
            {EVENT_FIREBOLT,        SPELL_FIREBOLT,         5000,           8000,           true,       false},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_FLAME_SHOCK:     Spell_Rotation(eventId);  break;
                case EVENT_FIREBOLT:   Spell_Rotation(eventId);  break;
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
        return new npc_BOT_chosen_seerAI(creature);
    }
};

class npc_BOT_chosen_warr : public CreatureScript
{
public:
    npc_BOT_chosen_warr() : CreatureScript("npc_BOT_chosen_warr") { }


    struct npc_BOT_chosen_warrAI : public ScriptedAI
    {

        npc_BOT_chosen_warrAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void IsSummonedBy(Unit* summoner)
        {
            if (auto c = summoner->ToCreature())
                if (auto ai = c->AI())
                    ai->JustSummoned(me);
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == WAYPOINT_MOTION_TYPE)
            {
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H_ALLOW_MOVEMENT);
                me->SetHomePosition(*me);
            }
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

            EVENT_HAMSTRING,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_HAMSTRING = 87704,
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

            {EVENT_HAMSTRING,     SPELL_HAMSTRING,      5000,          15000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_HAMSTRING:     Spell_Rotation(eventId);  break;
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
        return new npc_BOT_chosen_warrAI(creature);
    }
};

void AddSC_boss_halfus()
{
    new boss_halfus();
    new npc_proto_behemoth();
    new npc_halfus_dragon();
    new npc_halfus_dragon_lock();
    new npc_orphaned_whelp();
    new spell_proto_fireball();
    new spell_proto_fireball_barrage();
    new spell_halfus_stone_touch();
    new go_halfus_whelp_cage();
    new spell_halfus_malevolent_strikes();
    new npc_chogall_after_halfus();
    new npc_BOT_chosen_seer();
    new npc_BOT_chosen_warr();
}
