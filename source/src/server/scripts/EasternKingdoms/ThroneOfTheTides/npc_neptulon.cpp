
#include "GridNotifiers.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "throne_of_the_tides.h"

#define GOSSIP_READY "We are ready!"
#define SAY_1 "As I purify these waters, the servants of filth will surely be stirred. Beware!"
#define SAY_2 "Patience guests. The waters are nearly cleansed."
#define SAY_3 "The beast has returned! It must not pollute my waters!"
#define SAY_DEATH "Your kind... cannot be... trusted..."
#define SAY_CLEANSED "My waters are cleansed! Drink in their power!"

enum Phases
{
    PHASE_NULL  = 0,
    PHASE_FILTHY_INVADERS,
    PHASE_THE_BEAST_RETURN,
    PHASE_TIDAL_SURGE,
};
enum Spells
{
    // Neptulon
    SPELL_PURIFY                            = 76952, // Phase 1
    SPELL_AURA_OF_DEAD                      = 83970, // Phase 2
    SPELL_OZUMAT_GLOPE_IMPACT               = 83120,
    SPELL_ENTANGLING_GRASP                  = 83463,
    SPELL_BLIGHT_OF_OZUMAT                  = 83506,
    SPELL_BLIGHT_OF_OZUMAT_ROOM             = 83672, // Phase 3
    SPELL_TIDAL_SURGE                       = 76133,
    SPELL_BLIGHT_OF_OZUMAT_ROOM_TRIGGER     = 83607,
    //2 spells that apply, one falls off.
    SPELL_BLIGHT_OF_OZUMAT_AURA             = 83608,
    SPELL_BLIGHT_OF_OZUMAT_DISTANCE         = 83561,

    // Vicious Mindlasher
    SPELL_BRAIN_SPIKE                       = 83915,
    SPELL_SHADOW_BOLT                       = 83914,
    SPELL_VEIL_OF_SHADOW                    = 83926,

    // Unyielding Behemoth
    SPELL_BLIGHT_SPRAY                      = 83985,
    SPELL_SHADOW_BLAST                      = 83931,

    SPELL_SUMMON_BLIGHT_OF_OZUMAT_ROOM      = 83606,
    SPELL_SUMMON_KITE_ADD_EFFECT            = 83647,
    SPELL_JUMP_TO_GROUND                    = 83652,
    SPELL_CHARGE_TO_WINDOW                  = 83653,
    SPELL_KTE_ADD_VISUAL                    = 83665,

    SPELL_PURE_WATER                        = 84037,//casted at ozumat after he is free
    //Outro
    SPELL_RIDE_OZUMAT                       = 83119,
    SPELL_OZUMAT_GRAB_NEPTULON_PERIODIC     = 94170,
    SPELL_OZUMAT_GRAB_NEPTULON_TRIGGERED    = 94171,
};
enum Events
{
    EVENT_NONE,
    // Neptulon
    EVENT_PHASE_ONE_START,      // Phase One
    EVENT_SUMMON_MURLOCS,
    EVENT_SUMMON_MINDLASHER,
    EVENT_SUMMON_BEHEMOTH,
    EVENT_PHASE_TWO_START,      // Phase Two
    EVENT_SUMMON_SAPPERS,
    EVENT_SAPPERS_CAST,
    EVENT_BLIGHT_OF_OZUMAT,
    EVENT_SUMMON_BEAST,
    EVENT_PHASE_THREE_INITIAL,  // Phase Three
    EVENT_CHECK_OZUMAT,
    EVENT_BLIGHT_TICK,
    EVENT_PURE_WATER_CAST,

    // Vicious Mindlasher
    EVENT_SHADOW_BOLT,
    EVENT_BRAIN_SPIKE,
    EVENT_VEIL_OF_SHADOW,

    // Unyielding Behemoth
    EVENT_BLIGHT_SPRAY,
    EVENT_SHADOW_BLAST,
    EVENT_LIFTOFF,
};
enum eActions
{
    ACTION_PHASE_1          = 1002,
    ACTION_GOSSIP_START,
    ACTION_CHANGE_PHASE,
    ACTION_KILLED_PLAYER,

    ACTION_OZUMAT_SUMMON,
    ACTION_OZUMAT_SHOCKED,
    ACTION_OZUMAT_FLEE,
};

const Position outside_holes_ccw_from_5_oclock[] =
{
    {-190.524979f, 940.189453f, 233.058792f, 0.775605f},
    {-163.949127f, 919.256104f, 234.890808f, 1.341092f},

    {-99.515816f, 917.353333f, 230.245789f, 2.208958f},
    {-90.731491f, 922.935181f, 229.426285f, 2.281525f},

    {-75.367043f, 964.552063f, 227.312042f, 2.919744f},
    {-70.816338f, 983.769653f, 226.327789f, 3.151436f},

    {-89.741096f, 1036.377319f, 225.846985f, 3.917196f},
    {-120.807915f, 1061.054321f, 231.150269f, 4.423773f},

    {-192.202652f, 1055.116455f, 233.711609f, 5.275907f},
    {-188.130814f, 1033.986084f, 228.781372f, 5.236636f},
};

const Position inside_holes_ccw_from_5_oclock[] =
{
    {-174.193039f, 956.315674f, 230.017029f, 0.846244f},
    {-155.173035f, 955.332825f, 230.239105f, 1.227145f},
    {-124.036903f, 950.504395f, 231.450104f, 2.157849f},
    {-122.724571f, 957.854736f, 231.043076f, 2.352216f},

    {-110.404289f, 983.521729f, 229.911118f, 3.080691f},
    {-110.404289f, 983.521729f, 229.911118f, 3.080691f},

    {-118.001587f, 1009.154907f, 229.999680f, 3.901441f},
    {-135.362915f, 1019.878662f, 230.990494f, 4.462996f},
    {-164.593689f, 1009.259460f, 231.114426f, 5.122732f},
    {-170.144394f, 1007.727844f, 230.793503f, 5.346580f},
};

const Position spawns[] =
{
    // Phase 1
    { -114.42f, 940.497f, 280.9348f, 2.3315f }, // Summon POS Right
    { -112.48f, 1028.09f, 274.8811f, 3.5025f }, // Summon POS Left
    { -146.51f, 949.896f, 230.4939f, 1.3065f }, // Jump POS Right
    { -148.16f, 1016.96f, 231.0417f, 3.4593f }, // Jump POS Left
    // Phase 2
    { -142.29f, 790.119f, 321.0964f, 1.9886f }, // Ozumat Summon POS
    { -130.44f, 919.619f, 271.1572f, 1.7648f }, // Ozumat Point POS
    { -166.50f, 962.370f, 305.5118f, 0.6807f }, // First Sapper Summon POS
    { -170.60f, 1001.17f, 302.4476f, 6.1455f }, // Second Sapper Summon POS
    { -121.92f, 1001.26f, 304.3308f, 3.6872f }, // Third Sapper Summon POS
    { -166.50f, 962.370f, 230.2042f, 0.6807f }, // First Sapper Jump POS
    { -170.60f, 1001.17f, 230.6976f, 6.1455f }, // Second Sapper Jump POS
    { -121.92f, 1001.26f, 230.2709f, 3.6872f }, // Third Sapper Jump POS
    // Phase 3
    { -92.541f, 768.062f, 357.7690f, 1.9086f }, // Ozumat Run POS
};

/*
    Neptulon - 40792(N) & 49094(H)
*/
class npc_neptulon : public CreatureScript
{
public:
    npc_neptulon() : CreatureScript("npc_neptulon") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_neptulonAI (creature);
    }
    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (auto instance = creature->GetInstanceScript())
        {
            if (player)
            {
                if (instance->GetData(DATA_COMMANDER_ULTHOK_EVENT) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_READY, GOSSIP_SENDER_MAIN, ACTION_GOSSIP_START);
                player->SEND_GOSSIP_MENU(15997, creature->GetGUID());
            }
        }
        return true;
    }
    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == ACTION_GOSSIP_START)
            creature->AI()->DoAction(ACTION_PHASE_1);
        player->PlayerTalkClass->SendCloseGossip();
        return true;
    }
    struct npc_neptulonAI : public ScriptedAI
    {
        npc_neptulonAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            instance = creature->GetInstanceScript();
            coralGarden = false;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void Reset()
        {
            jumped = false;
            sappers = 0;
            checkPlayer = 3000;
            talk25Pct = talk50Pct = checkSapper = false;
            me->SetReactState(REACT_PASSIVE);
            events.Reset();
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
        void DoAction(int const action)
        {
            switch (action)
            {
            case ACTION_NEPTULON_INTRO:
                if (!coralGarden)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    Talk(0);
                    TalkWithDelay(5000, 1);
                    coralGarden = true;
                }
                break;
            case ACTION_PHASE_1:
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);

                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                Phase = PHASE_FILTHY_INVADERS;
                DoZoneInCombat();
                TalkWithDelay(3000, 2);
                me->setRegeneratingHealth(false);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                if (instance)
                    instance->SetData(DATA_OZUMAT, IN_PROGRESS);
                events.ScheduleEvent(EVENT_PHASE_ONE_START, 3000);
                me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.1664f);
                    instance->HandleGameObject(instance->GetData64(GO_ABYSSAL_MAW_04), false, NULL);
                break;
            case ACTION_KILLED_PLAYER:
                Talk(11);
                break;
            case 1995:
                //TC_LOG_ERROR("sql.sql", "EVENT_CHECK_OZUMAT detected under 15 pct health");
                jumped = true;
                me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT);
                me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT_SUMMONER);
                me->CastStop();
                events.Reset();
                if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 300.f, true))
                {
                    ozu->RemoveAllAuras();
                    ozu->CastSpell(me, SPELL_OZUMAT_GRAB_NEPTULON_PERIODIC);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, ozu);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    ozu->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                }
                events.ScheduleEvent(5000, 3000);
                break;
            default:
                break;
            }
        }
        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
            case NPC_DEEP_MURLOC_INVADER:
                break;
            case NPC_VICIOUS_MINDLASHER:
                summon->AI()->AttackStart(me);
                break;
            case NPC_UNYIELDING_BEHEMOTH:
                summon->AI()->AttackStart(me);
                break;
            case NPC_FACELESS_SAPPER:
                summon->SetInCombatWithZone();
                summon->SetReactState(REACT_PASSIVE);
                summon->SetDisableGravity(false);
                break;
            case BOSS_OZUMAT:
                summon->AI()->DoCast(SPELL_OZUMAT_GLOPE_IMPACT);
                summon->AI()->DoAction(ACTION_OZUMAT_SUMMON);
                break;
            case NPC_BLIGHT_BEAST:
                if (IsHeroic())
                    summon->CastSpell(summon, SPELL_AURA_OF_DEAD, true);
                summon->AI()->AttackStart(me);
                summon->SetInCombatWithZone();
                break;
            default:
                summon->SetInCombatWithZone();
                break;
            }
            summons.Summon(summon);
        }
        void JustDied(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            Talk(10);
            _Evade();
            me->Respawn(true);
            if (auto go = me->FindNearestGameObject(GO_ABYSSAL_MAW_04, 100.0f))
            {
                instance->HandleGameObject(instance->GetData64(GO_ABYSSAL_MAW_04), true, NULL);
            }
        }
        void DamageTaken(Unit* attacker, uint32& damage)
        {

            if (!talk50Pct && me->GetHealthPct() <= 50.0f)
            {
                talk50Pct = true;
                Talk(8);
            }
            if (!talk25Pct && me->GetHealthPct() <= 25.0f)
            {
                talk25Pct = true;
                Talk(9);
            }
        }
        void _Evade()
        {
            //TC_LOG_ERROR("sql.sql", "executing _evade");
            jumped = false;
            Phase = PHASE_NULL;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->RemoveAura(SPELL_PURIFY);
            me->CombatStop();
            me->setRegeneratingHealth(true);
            me->SetFacingTo(3.2825f);
            me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT);
            me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT_SUMMONER);
            if (auto go = me->FindNearestGameObject(GO_ABYSSAL_MAW_04, 100.0f))
            {
                //TC_LOG_ERROR("sql.sql", "opening door - evade");
                instance->HandleGameObject(instance->GetData64(GO_ABYSSAL_MAW_04), true, NULL);
            }
            if (instance)
            {
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TIDAL_SURGE);
                instance->DoRemoveAurasDueToSpellOnPlayers(76155);
                instance->SetData(DATA_OZUMAT, NOT_STARTED);
            }
            summons.DespawnAll();
            events.Reset();
            me->SetFacingTo(3.27f);
            Reset();
        }

        void BlightTickPunish()
        {
            auto players = me->GetPlayersInRange(70.f, true);
            if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 100.f))
                if (auto meUnit = me->ToUnit())
                    if (players.size())
                        for (Player* player : players)
                        {
                            if (Phase == PHASE_TIDAL_SURGE)
                            {
                                if (auto blight = player->GetAura(SPELL_BLIGHT_OF_OZUMAT_ROOM))
                                {
                                    if (blight->GetStackAmount() < 200)
                                        blight->SetStackAmount(blight->GetStackAmount() + 1);

                                    blight->SetDuration(blight->GetMaxDuration());
                                }
                                else
                                    ozu->CastSpell(player, SPELL_BLIGHT_OF_OZUMAT_ROOM, true);
                            }
                            else
                            {
                                if (auto c = player->FindNearestCreature(NPC_BLIGHT_OF_OZUMAT_SUMMONER, 50.f, true))
                                {
                                    if (player->GetExactDist(c) <= 10.f)
                                    {
                                        if (c->HasAura(83525))
                                        {
                                            if (auto blight = player->GetAura(SPELL_BLIGHT_OF_OZUMAT_DISTANCE))
                                            {
                                                if (blight->GetStackAmount() < 200)
                                                    blight->SetStackAmount(blight->GetStackAmount() + 1);

                                                blight->SetDuration(blight->GetMaxDuration());
                                            }
                                            else
                                                c->AddAura(SPELL_BLIGHT_OF_OZUMAT_DISTANCE, player);
                                        }
                                    }
                                }
                            }
                        }
        }


        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id) 
        {
            if (summon->GetEntry() == NPC_BLIGHT_BEAST)
                if (type == POINT_MOTION_TYPE)
                    if (id == 553)
                    {
                        summon->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                        summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                        summon->SetReactState(REACT_AGGRESSIVE);
                        summon->SetInCombatWithZone();
                        if (auto v = summon->getVictim())
                            if (!v->ToPlayer())
                                if (auto p = summon->FindNearestPlayer(100.f))
                                {
                                    summon->AI()->AttackStart(p);
                                    summon->AddThreat(p, 10000.f);
                                }

                    }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            switch (summon->GetEntry())
            {
            case NPC_FACELESS_SAPPER:
                summon->DespawnOrUnsummon();
                sappers -= 1;
                break;
            default:
                break;
            }
        }


        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase == PHASE_NULL)
                return;

            if (checkPlayer <= diff)
            {
                if (checkWipe())
                    _Evade();
                    checkPlayer = 3000;
            }
            checkPlayer -= diff;

            events.Update(diff);

            switch (Phase)
            {
            case PHASE_FILTHY_INVADERS:
            {
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_PHASE_ONE_START:
                        TalkWithDelay(45000, 3);
                        DoCast(SPELL_PURIFY);
                        events.ScheduleEvent(EVENT_PHASE_TWO_START, 60000);
                        events.ScheduleEvent(EVENT_SUMMON_MURLOCS, 5000);
                        events.ScheduleEvent(EVENT_SUMMON_MINDLASHER, 5000);
                        events.ScheduleEvent(EVENT_SUMMON_BEHEMOTH, 15000);
                        break;
                    case EVENT_SUMMON_MURLOCS:
                    {
                        for (int i = 0; i < 6; ++i)
                        {
                            int j = urand(0,9);
                            if (auto murloc = me->SummonCreature(NPC_DEEP_MURLOC_INVADER, outside_holes_ccw_from_5_oclock[j], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                            {
                                murloc->GetMotionMaster()->MoveJump(inside_holes_ccw_from_5_oclock[j], 15.0f, 15.0f, 1001);
                                murloc->SetReactState(REACT_PASSIVE);
                            }
                        }
                        events.RescheduleEvent(EVENT_SUMMON_MURLOCS, 23000);
                        break;
                    }
                    case EVENT_SUMMON_MINDLASHER:
                    {
                        if (auto j = urand(0, 1))
                        if (auto mindlasher = me->SummonCreature(NPC_VICIOUS_MINDLASHER, spawns[j], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                        {
                            /*
                            mindlasher->SetHover(false);
                            mindlasher->DisableMovementFlagUpdate(true);
                            mindlasher->SetCanFly(false);
                            mindlasher->SetDisableGravity(true);
                            mindlasher->GetMotionMaster()->Clear();
                            */
                            mindlasher->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                            mindlasher->SetReactState(REACT_PASSIVE);
                            mindlasher->GetMotionMaster()->MoveJump(spawns[j + 2], 15.0f, 15.0f, 553);
                        }
                        events.RescheduleEvent(EVENT_SUMMON_MINDLASHER, 23000);
                        break;
                    }
                    case EVENT_SUMMON_BEHEMOTH:
                    {
                        int j = urand(0, 1);
                        if (auto behemoth = me->SummonCreature(NPC_UNYIELDING_BEHEMOTH, spawns[j], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        {
                            behemoth->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                            behemoth->SetReactState(REACT_PASSIVE);
                            behemoth->GetMotionMaster()->MoveJump(spawns[j + 2], 15.0f, 15.0f, 553);
                        }
                        break;
                    }
                    case EVENT_PHASE_TWO_START:
                        events.ScheduleEvent(EVENT_BLIGHT_TICK, 8000);
                        me->RemoveAura(SPELL_PURIFY);
                        Talk(4);
                        TalkWithDelay(10000, 5);
                        Ozumat = me->SummonCreature(BOSS_OZUMAT, spawns[5]);
                        blightOfOzumatSummoner = me->SummonCreature(NPC_BLIGHT_OF_OZUMAT_SUMMONER, spawns[5]);
                        events.ScheduleEvent(EVENT_SUMMON_SAPPERS, 4000);
                        events.ScheduleEvent(EVENT_SUMMON_BEAST, 5000);
                        events.ScheduleEvent(EVENT_BLIGHT_OF_OZUMAT, 14000);
                        Phase = PHASE_THE_BEAST_RETURN;
                        break;
                    default:
                        break;
                    }
                }
                break;
            }
            case PHASE_THE_BEAST_RETURN:
            {
                if (checkSapper)
                    if (sappers == 0)
                    {
                        Phase = PHASE_TIDAL_SURGE;
                        Talk(6);
                        TalkWithDelay(5000, 7);
                        events.ScheduleEvent(EVENT_PHASE_THREE_INITIAL, 4000);
                    }
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_BLIGHT_TICK:
                        BlightTickPunish();
                        events.ScheduleEvent(EVENT_BLIGHT_TICK, 500);
                        break;
                    case EVENT_SUMMON_SAPPERS:
                    {
                        sappers = 3;
                        for (int i = 6; i < 9; ++i)
                            if (auto sapper = me->SummonCreature(NPC_FACELESS_SAPPER, spawns[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
                            {
                                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, sapper);
                                sapper->GetMotionMaster()->MoveJump(spawns[i + 3], 15.0f, 15.0f, 1001);
                                sapper->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                                sapper->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                            }
                        checkSapper = true;
                        events.ScheduleEvent(EVENT_SAPPERS_CAST, 6000);
                        break;
                    }
                    case EVENT_SAPPERS_CAST:
                    {
                        std::list<Creature*> units;
                        GetCreatureListWithEntryInGrid(units, me, NPC_FACELESS_SAPPER, 250.0f);
                        if (units.size())
                        for (auto itr = units.begin(); itr != units.end(); ++itr)
                            if (auto sapper = (*itr))
                                if (sapper->isAlive())
                                {
                                    sapper->CastSpell(me, SPELL_ENTANGLING_GRASP, true);
                                }
                        break;
                    }
                    case EVENT_SUMMON_BEAST:
                    {
                        int j = urand(0, 1);
                        if (auto beast = me->SummonCreature(NPC_BLIGHT_BEAST, spawns[j], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
                        {
                            summons.Summon(beast);
                            beast->AddAura(SPELL_WATER_JUMP_VISUAL, beast);
                            beast->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                            beast->SetReactState(REACT_PASSIVE);
                            beast->GetMotionMaster()->MoveJump(spawns[j + 2], 15.0f, 15.0f, 553);
                        }
                        events.RescheduleEvent(EVENT_SUMMON_BEAST, 15000);
                        break;
                    }
                    case EVENT_BLIGHT_OF_OZUMAT:
                    {
                        if(blightOfOzumatSummoner && blightOfOzumatSummoner->isAlive())
                            if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                blightOfOzumatSummoner->CastSpell(victim, SPELL_BLIGHT_OF_OZUMAT, true);
                        events.RescheduleEvent(EVENT_BLIGHT_OF_OZUMAT, 10000);
                        break;
                    }
                    default:
                        break;
                    }
                }
                break;
            }
            case PHASE_TIDAL_SURGE:
            {
                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_BLIGHT_TICK:
                        BlightTickPunish();
                        events.ScheduleEvent(EVENT_BLIGHT_TICK, 500);
                        break;
                    case EVENT_PURE_WATER_CAST:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.RescheduleEvent(eventId, 250);
                        else
                        {
                            if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 500.f))
                            {
                                DoCast(ozu, SPELL_PURE_WATER);
                            }
                            events.ScheduleEvent(EVENT_PURE_WATER_CAST, 3000);
                        }
                        break;
                    case EVENT_PHASE_THREE_INITIAL:
                    {
                        DoCast(SPELL_TIDAL_SURGE);
                        if (blightOfOzumatSummoner && blightOfOzumatSummoner->isAlive())
                            blightOfOzumatSummoner->CastSpell(me, SPELL_BLIGHT_OF_OZUMAT_ROOM);
                        if (Ozumat && Ozumat->isAlive())
                        {
                            Ozumat->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, Ozumat);
                        }
                        Position pos;
                        me->GetPosition(&pos);
                        if (auto blight = me->SummonCreature(NPC_BLIGHT_OF_OZUMAT_SUMMONER, pos))
                        {
                            blight->RemoveAllAuras();
                            blight->CastSpell(blight, SPELL_BLIGHT_OF_OZUMAT_ROOM_TRIGGER);
                        }
                        events.ScheduleEvent(EVENT_CHECK_OZUMAT, 500);
                        events.ScheduleEvent(EVENT_PURE_WATER_CAST, 1000);


                        if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 500.f))
                        {
                            me->SetReactState(REACT_PASSIVE);
                            me->SetTarget(ozu->GetGUID());
                            me->SetFacingTo(me->GetAngle(ozu));
                            me->AddUnitState(UNIT_STATE_ROOT);
                            me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                        }

                        break;
                    }
                    case EVENT_CHECK_OZUMAT:
                    {
                        break;
                    }
                    case 5000:
                        me->ClearUnitState(UNIT_STATE_ROOT);
                        if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 500.f))
                        {
                            me->GetMotionMaster()->ForceMoveJump(ozu->GetPositionX(), ozu->GetPositionY(), ozu->GetPositionZ(), 35.f, 21.f);
                            ozu->CastStop();
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            default:
                break;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            //TC_LOG_ERROR("sql.sql", "MovementInform %u %u", type, id);
            if (type == 17)
                if (id == 1)
                {
                    //TC_LOG_ERROR("sql.sql", "MovementInform meets requirements");
                    if (auto ozu = me->FindNearestCreature(BOSS_OZUMAT, 50.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "Ozu found in movement inform function.");
                        Position fuck_off{ 51.376114f, 609.145325f, 572.117065f, 5.268106f };
                        ozu->GetMotionMaster()->MovePoint(1, fuck_off, false, 35.f);
                        ozu->DespawnOrUnsummon(5000);

                        if (auto instance = me->GetInstanceScript())
                        {
                            //TC_LOG_ERROR("sql.sql", "Instance script found.");
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                            instance->OnUnitDeath(ozu->ToUnit());
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TIDAL_SURGE);
                            instance->SetData(DATA_OZUMAT, DONE);

                            me->DespawnCreaturesInArea(NPC_DEEP_MURLOC_DRUDGE, 500.f);
                            me->DespawnCreaturesInArea(NPC_DEEP_MURLOC_INVADER, 500.f);
                            me->DespawnCreaturesInArea(NPC_BLIGHT_BEAST, 500.f);
                            me->DespawnCreaturesInArea(NPC_VICIOUS_MINDLASHER, 500.f);
                            me->DespawnCreaturesInArea(NPC_UNYIELDING_BEHEMOTH, 500.f);
                            me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT, 500.f);
                            me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT_ROOM, 500.f);
                            me->DespawnCreaturesInArea(NPC_BLIGHT_OF_OZUMAT_SUMMONER, 500.f);

                            if (auto go = me->FindNearestGameObject(GO_ABYSSAL_MAW_04, 100.0f))
                            {
                                instance->HandleGameObject(go->GetGUID(), true, NULL);
                            }

                        }

                    }
                    me->DespawnOrUnsummon(1000);
                }
        }

    private:
        bool coralGarden, talk25Pct, talk50Pct, checkSapper;
        bool jumped{ false };
        int Phase;
        uint32 sappers{ 0 };
        uint32 checkPlayer = 3000;
        Creature* Ozumat;
        Creature* blightOfOzumatSummoner;
        SummonList summons;
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };
};

/*
    Ozumat - 40655(N) & 49096(H)
*/
class boss_ozumat : public CreatureScript
{
public:
    boss_ozumat() : CreatureScript("boss_ozumat") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ozumatAI (creature);
    }

    struct boss_ozumatAI : public ScriptedAI
    {
        boss_ozumatAI(Creature* creature) : ScriptedAI(creature) 
        {
        }
        void InitializeAI()
        {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 417);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
        }
        
        void Reset() { }
        void DoAction(int const action)
        {
            if (action == ACTION_OZUMAT_FLEE)
            {
                Position move_to{ 520.473816f, 843.327942f, 520.379883f, 0.007537f };
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                me->GetMotionMaster()->MovePoint(200, move_to, true, 70.f);
                me->SetObjectScale(2.f);
                me->DespawnOrUnsummon(10000);
            }

            if (action == ACTION_OZUMAT_SHOCKED)
            {
                Position ozu_rp{ -130.894f, 819.306f, 365.f, 3.27308f };
                me->NearTeleportTo(ozu_rp);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 430);
            }

            if (action == ACTION_OZUMAT_SUMMON)
            {
                //me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);
                me->HandleEmoteCommand(EMOTE_STATE_CUSTOM_SPELL_01);

                if (auto neptulon = me->FindNearestCreature(BOSS_NEPTULON, 300.f, true))
                    neptulon->SetFacingTo(neptulon->GetAngle(me));
            }
        }
        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (me->GetHealthPct() <= 15.0f)
            {
                damage = 0;
                me->SetHealth(me->GetMaxHealth() * 0.15);

                if (!finished)
                if (auto neptulon = me->FindNearestCreature(BOSS_NEPTULON, 300.f, true))
                {
                    finished = true;
                    neptulon->AI()->DoAction(1995);
                }
            }
        }
    private:
        bool finished{ false };
    };
};

/*
    Vicious Mindlasher - 44658(N) & 49068(H) 
*/
class npc_vicious_mindlasher : public CreatureScript
{
public:
    npc_vicious_mindlasher() : CreatureScript("npc_vicious_mindlasher") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vicious_mindlasherAI(creature);
    }
    struct npc_vicious_mindlasherAI : public ScriptedAI
    {
        npc_vicious_mindlasherAI(Creature* creature) : ScriptedAI(creature) 
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_SHADOW_BOLT, 1000);
            events.ScheduleEvent(EVENT_BRAIN_SPIKE, urand(12000, 15000));
            if (IsHeroic())
                events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, urand(7000, 13000));
        }
        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                if (auto neptulon = me->FindNearestCreature(BOSS_NEPTULON, 100.0f, true))
                    neptulon->AI()->DoAction(ACTION_KILLED_PLAYER);
        }

        void MovementInform()
        {
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == EFFECT_MOTION_TYPE)
                if (id == 553)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetHover(false);
                    me->DisableMovementFlagUpdate(true);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveFall();

                    me->RemoveAura(SPELL_WATER_JUMP_VISUAL);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    UpdateVictim();
                }
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason reason)
        {
            if (reason == SPELL_FINISHED_CANCELED)
            {
                events.DelayEvent(EVENT_VEIL_OF_SHADOW, 6000);
                events.DelayEvent(EVENT_BRAIN_SPIKE, 6000);
                events.DelayEvent(EVENT_BRAIN_SPIKE, 6000);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SHADOW_BOLT:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(SPELL_SHADOW_BOLT);
                        events.RescheduleEvent(EVENT_SHADOW_BOLT, urand(4000, 5000));
                    }
                    break;
                case EVENT_BRAIN_SPIKE:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        DoCast(SPELL_BRAIN_SPIKE);
                        events.RescheduleEvent(EVENT_BRAIN_SPIKE, urand(15000, 25000));
                    }
                    break;
                case EVENT_VEIL_OF_SHADOW:
                    if (me->HasUnitState(UNIT_STATE_CASTING))   events.ScheduleEvent(eventId, 250);
                    else
                    {
                        if (IsHeroic())
                        {
                            if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                                DoCast(target, SPELL_VEIL_OF_SHADOW);
                            events.RescheduleEvent(EVENT_VEIL_OF_SHADOW, urand(10000, 15000));
                        }
                    }
                    break;
                default:
                    break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
    private:
        InstanceScript* instance;
        EventMap events;
    };
};

/*
    Unyielding Behemoth - 44648(N) & 49104(H)
*/
class npc_unyielding_behemoth : public CreatureScript
{
public:
    npc_unyielding_behemoth() : CreatureScript("npc_unyielding_behemoth") {}
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unyielding_behemothAI (creature);
    }
    struct npc_unyielding_behemothAI : public ScriptedAI
    {
        npc_unyielding_behemothAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
        }
        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                if (auto neptulon = me->FindNearestCreature(BOSS_NEPTULON, 100.0f, true))
                    neptulon->AI()->DoAction(ACTION_KILLED_PLAYER);
        }
        void EnterCombat(Unit* /*attacker*/)
        {
            events.ScheduleEvent(EVENT_BLIGHT_SPRAY, 8000);
            events.ScheduleEvent(EVENT_LIFTOFF, urand(10000, 15000));
        }

        void MovementInform(uint32 type, uint32 id)
        {
            //TC_LOG_ERROR("sql.sql", "behemoth movements: %u %u", type, id);
            if (type == EFFECT_MOTION_TYPE)
            {
                if (id == 553)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetInCombatWithZone();
                    UpdateVictim();
                }
                if (id == 2000)
                {
                    DoCastAOE(SPELL_SHADOW_BLAST, true);
                    me->SetReactState(REACT_AGGRESSIVE);
                    UpdateVictim();
                    summons.DespawnEntry(44949);
                }
            }
        }

        void JustDied(Unit* killer)
        {
            if (instance)
            if (killer->HasAura(SPELL_TIDAL_SURGE) && IsHeroic())
                instance->DoCompleteAchievement(HEROIC_DEFEAT_BEHEMOT_WITH_SURGE_EFFECT);
        }
        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_BLIGHT_SPRAY:
                    if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetVehicle()) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        float lock_o{ me->GetOrientation() };
                        DoCastAOE(SPELL_BLIGHT_SPRAY);
                        me->SetFacingTo(lock_o);
                        events.RescheduleEvent(EVENT_BLIGHT_SPRAY, urand(5000, 7000));
                    }
                    break;
                case EVENT_LIFTOFF:
                    if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        events.DelayEvent(EVENT_BLIGHT_SPRAY, 12000);
                        events.ScheduleEvent(EVENT_SHADOW_BLAST, 3000);

                        Position vehiclePos(*me);
                        Position attackPos(*me);
                        vehiclePos.m_positionZ += 7.f;

                        me->SetReactState(REACT_PASSIVE);
                        me->StopMoving();
                        me->AttackStop();
                        //44949
                        if (auto c = me->SummonCreature(44949, vehiclePos, TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto c3 = me->SummonCreature(44949, vehiclePos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            me->MoveBlink(shadow_blast_at, 0.1f, 0.f);
                            c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            if (auto v = c->GetVehicleKit())
                                me->EnterVehicle(c);


                                    c3->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    if (auto aura = c3->AddAura(83977, c3))
                                    {
                                        aura->SetMaxDuration(4000);
                                        aura->SetDuration(4000);
                                    }
                                    c3->AddAura(83975, me);
                        }
                    }
                    break;
                case EVENT_SHADOW_BLAST:
                {
                    events.ScheduleEvent(EVENT_LIFTOFF, 16000);
                    if (auto v = me->GetVehicleBase())
                        v->RemoveAllAuras();

                    me->GetMotionMaster()->MoveJump(shadow_blast_at, 10.f, 10.f, 2000);
                }
                    break;
                default:
                    break;
                }

            if (!me->HasReactState(REACT_PASSIVE))
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
    private:
        Position shadow_blast_at    { *me };
        InstanceScript* instance;
        EventMap events;
    };
};

// 76133
class spell_tidal_surge : public SpellScriptLoader
{
public:
    spell_tidal_surge() : SpellScriptLoader("spell_tidal_surge") { }

    class spell_tidal_surge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tidal_surge_SpellScript);

        void HandleEffect(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
            {
                uint32 maxHealth = target->GetMaxHealth();
                target->CastSpell(target, 76155, true);
                if (target->ToTotem())
                {
                    target->SetMaxHealth(maxHealth);
                    target->SetHealth(maxHealth);
                    target->SetObjectScale(0.2f);
                }
            }

        }

        void FilterTargets(std::list<WorldObject*>& unitList)   //Force-add player totems to the list to be buffed.
        {
            Unit* caster = GetCaster();
            auto players = caster->GetPlayersInRange(100.f, true);
            if (players.size())
            {
                for (Player* player : players)
                    for (uint8 i = 0; i < MAX_SUMMON_SLOT; ++i)
                        if (player->m_SummonSlot[i])
                            if (auto summon = caster->GetMap()->GetCreature(player->m_SummonSlot[i]))
                                if (summon->ToTotem())
                                    unitList.push_back(summon);
            }


            unitList.remove_if([](WorldObject* target)
                {
                    auto myArray =
                    {
                        BOSS_NEPTULON,
                        BOSS_LADY_NAZJAR,
                    };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray); //DOES INCLUDE, would be == end(myArray) to be "not includes".
                });
        }

        void FilterTargets_no_Totems(std::list<WorldObject*>& unitList)   //Force-add player totems to the list to be buffed.
        {
            Unit* caster = GetCaster();
            unitList.remove_if([](WorldObject* target)
                {
                    auto myArray =
                    {
                        BOSS_NEPTULON,
                        BOSS_LADY_NAZJAR,
                    };
                    return std::find(begin(myArray), end(myArray), target->GetEntry()) != end(myArray); //DOES INCLUDE, would be == end(myArray) to be "not includes".
                });
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_tidal_surge_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tidal_surge_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tidal_surge_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tidal_surge_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_tidal_surge_SpellScript();
    }
};

enum neptulon_murloc
{
    // Gilgoblin Hunter
    EVENT_ATTACK_NEPTULON = 5,
};

class npc_neptulon_murloc : public CreatureScript
{
public:
    npc_neptulon_murloc() : CreatureScript("npc_neptulon_murloc") { }


    struct npc_neptulon_murlocAI : public ScriptedAI
    {

        npc_neptulon_murlocAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
        }

        void MovementInform(uint32 type, uint32 id) 
        {
            if (type == EFFECT_MOTION_TYPE)
                if (id == 1001)
                {
                    me->SetReactState(REACT_AGGRESSIVE);
                    if (auto c = me->FindNearestCreature(BOSS_NEPTULON, 200.f, true))
                    {
                        AttackStart(c);
                        me->AddThreat(c, 500.f);
                    }
                }
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            events.ScheduleEvent(5, urand(5000, 10000));
        }

        void InitializeAI()
        {
            me->SetInCombatWithZone();
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }

        void UpdateAI(const uint32 diff) override
        {
            if (UpdateVictim())
                DoMeleeAttackIfReady();

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_ATTACK_NEPTULON:
                    break;
                default:
                    break;
                }
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_neptulon_murlocAI(creature);
    }
};

class spell_neptulon_purify_cosmetic : public SpellScriptLoader
{
public:
    spell_neptulon_purify_cosmetic() : SpellScriptLoader("spell_neptulon_purify_cosmetic") { }

    class spell_neptulon_purify_cosmetic_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_neptulon_purify_cosmetic_SpellScript);


        void FilterTargets(std::list<WorldObject*>& unitList)
        {

            unitList.remove_if([](WorldObject* target)
                {
                    return (target->GetEntry() != 144950);
                });
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_neptulon_purify_cosmetic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_neptulon_purify_cosmetic_SpellScript();
    }
};


void AddSC_neptulon()
{
    // ST Scripts
    new npc_neptulon();
    new boss_ozumat();
    new npc_vicious_mindlasher();
    new npc_unyielding_behemoth();
    new spell_tidal_surge();          //Disabled for now, will need testing.
    new npc_neptulon_murloc();
    new spell_neptulon_purify_cosmetic();
}
