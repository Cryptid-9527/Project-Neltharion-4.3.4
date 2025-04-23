#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "well_of_eternity.h"

enum Talks
{
    TALK_AGGRO                          = -54778,
    TALK_ILLIDAN_AGGRO_BOSS             = -54829,
    TALK_DRAIN_ESSENCE                  = -54546,
    TALK_ILLIDAN_ABSORB                 = -54775,
    TALK_INTO_SHADOWS                   = -54816,
    TALK_INTO_SHADOWS_EMOTE             = -55983,
    TALK_ILLIDAN_RETURN                 = -54831,
    TALK_EYE_EVENT                      = -54817,
    TALK_EYE_EVENT_WARNING              = -55984,
    TALK_YOU_HIDE_WELL                  = -54766,
    TALK_ILLIDAN_STRENGTH_RETURN        = -54832,
    TALK_FRENZY                         = -54812,
    TALK_DEATH                          = -56991,
    TALK_COWER_IN_HIDING                = -54819,
    TALK_PORTAL_SHUTDOWN                = -54833,
    TALK_ILLIDAN_MOVE_IN_LINE_OF_SIGHT  = -54274,
    TALK_ILLIDAN_WAIT_FOR_EVENT_START   = -54277,
    TALK_ILLIDAN_HIDE_IN_SHADOWS        = -54705,
    TALK_ILLIDAN_START_MOVEMENT         = -54284,
    TALK_ILLIDAN_MANY_DEMONS            = -54288,
    TALK_ILLIDAN_SINGLE_GUARDIAN_TALK   = -54296,
    TALK_ILLIDAN_HOLD_THEM_BACK         = -54297,
    TALK_ILLIDAN_MAGIC_IS_FADING        = -54299,
    TALK_ILLIDAN_ATTACK_CRYSTALL        = -54304,
    TALK_ILLIDAN_DESTROY_THE_CRYSTALL   = -54709,
    TALK_ILLIDAN_LEAVING                = -54511,
    TALK_ILLIDAN_THEY_COME_ENDLESSLY    = -54510,
    TALK_ILLIDAN_ATTACKING              = -54444,
    TALK_ILLIDAN_THE_STENCH_OF_SULFUR   = -54512,
    TALK_ILLIDAN_CUT_THIS_ONE_DOWN      = -54514,
    TALK_ILLIDAN_SHUT_DOWN_LAST_PORTAL  = -54736,
    TALK_ILLIDAN_DEMONS_LEAVING         = -54750,
    TALK_ILLIDAN_DEMONS_NOT_ON_PLACE    = -54572,
    TALK_ILLIDAN_TOO_EASY               = -54573,
    TALK_ILLIDAN_ANOTHER_DEMON          = -54574,
    TALK_ILLIDAN_PATIENCE               = -54309,
    TALK_ILLIDAN_AGGRO                  = -54444,
    TALK_ILLIDAN_PEROTHARN_DEATH        = -54838,
    TALK_ILLIDAN_GOOD_BYE               = -54839,
    TALK_NOZDORMU_INTRO                 = -55271,
    TALK_NOZDORMU_INTRO_TWO             = -55304,
    TALK_NOZDORMU_INTRO_THREE           = -55305,
    TALK_PEROTHARN_INTRO                = -54683,
    TALK_PEROTHARN_INTRO_TWO            = -54685,
    TALK_PEROTHARN_INTRO_THREE          = -54687,
    TALK_PEROTHARN_PORTAL_SHUTDOWN      = -54833,
};

enum Spells
{
    // Perotharn
    SPELL_CORRUPTING_TOUCH                      = 104939,
    SPELL_50_PCT_INCREASED_SCALE                = 105014,
    SPELL_CAMOUFLAGE                            = 105341,
    SPELL_FEL_FLAMES                            = 108141,
    SPELL_FEL_DECAY                             = 105544,
    SPELL_FEL_SURGE                             = 108128,
    SPELL_DRAIN_ESSENCE                         = 104905,
    SPELL_FEL_ADDLED                            = 105545,
    SPELL_ENDLESS_FRENZY                        = 105521,
    SPELL_CHARGE_PLAYER                         = 105496,
    SPELL_ASPHYXIATE                            = 93422,
    SPELL_ENFEEBLED                             = 105442,

    // Fel Flames
    SPELL_FEL_FLAMES_PERIODIC                   = 108214,

    // Hunting Circle
    SPELL_SUMMON_CIRCLE_SPAWN                   = 105462,

    // Hunting Stalker
    SPELL_PUNISHING_FLAMES                      = 107532,
    SPELL_FEL_FIREBALL_AGGRO                    = 105491,
    SPELL_CANCEL_FEL_FIREBALL_AGGRO             = 105492,
    SPELL_HUNTING_CHANNEL                       = 105353, // The Eyes of Peroth'arn are looking for you.
    SPELL_SUMMON_CIRCLE_TRIGGER                 = 105463, // unused
    SPELL_END_EXHAUSTED                         = 105548, // target illidan
    
    // Eye of Perotharn
    SPELL_EASY_PREY                             = 105493,
    SPELL_EASY_PREY_SUMMON                      = 105761,
    SPELL_INVISIBILITY_DETECTION                = 93105,

    // Illidan event
    SPELL_ABSORB_FEL_ENERGY                     = 105543,
    SPELL_ILLIDAN_WALL_OF_SHADOWS               = 104400,
    SPELL_ILLIDAN_SHADOWCLOAK                   = 105915,
    SPELL_ILLIDAN_STALKER_SHADOWCLOAK_COSMETIC  = 105924,
    SPELL_RIDE_VEHICLE_HARDCODED                = 46598,
    SPELL_PLAYER_SHADOWCLOAK_COSMETIC           = 102951,
    SPELL_PLAYER_SHADOW_AMBUSHER                = 103018,
    SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH        = 103420,
    SPELL_DISTRACT_DEMONS_STATIONARY            = 110082,
    SPELL_DEMON_RUSH                            = 104205,
    SPELL_DEMONIC_SIGHT                         = 104746,
    SPELL_REGENERATION                          = 105547,
    // Legion demon
    SPELL_SUMMON_FIREWALL                       = 105243,
    SPELL_SUMMON_FIREWALL_PERIODIC              = 105247,
    SPELL_SUMMON_FIREWALL_PULSE                 = 105250,
    SPELL_STRIKE_FEAR                           = 103913,
    // Dreadlord Defender
    SPELL_DEMONIC_WARDING                       = 107900,
    SPELL_CARRION_SWARM                         = 107840,
    // Corrupted Arcanist
    SPELL_CORRUPTED_NIGHT_ELF_EYES              = 110048,
    SPELL_DISCIPLINED_CASTER                    = 107877,
    SPELL_ARCANE_ANNIHILATION                   = 107865,
    SPELL_INFINITE_MANA                         = 107880,
    // Fel Crystal
    SPELL_FEL_CRYSTAL_MELTDOWN                  = 105074,
    SPELL_FEL_CRYSTAL_DESTRUCTION               = 105079,
    SPELL_FEL_CRYSTAL_DESTROYED                 = 105119,
    SPELL_FEL_CRYSTAL_SWITCH_GLOW               = 105046,
    SPELL_SHATTER_FEL_CRYSTAL                   = 105004,
    SPELL_ARCANE_EXPLOSION_VISUAL               = 98122,
    // Guardian Demon
    SPELL_DEMONIC_GRIP                          = 102561,
    SPELL_DEMONIC_GRIP_PULL                     = 103649,
    SPELL_DEMONIC_GRIP_ROOT                     = 102937,
    // Teleporter
    SPELL_TELEPORT_AZSHARAS_PALACE              = 107690,
    SPELL_TELEPORT_WELL_OF_ETERNITY             = 107691,
    SPELL_TELEPORT_AZSHARAS_PALACE_OVERLOOK     = 107979,
    SPELL_TELEPORT_COURTYARD_ENTRANCE           = 107934,

    SPELL_CONNECTOR_DEAD                        = 105201,
};

enum Events
{
    EVENT_SUMMON_DEMONS                     = 1,
    EVENT_SUMMON_DEMON_WAVES_PORTAL_ONE_TWO = 2,
    EVENT_FEL_FLAMES                        = 3,
    EVENT_FEL_DECAY                         = 4,
    EVENT_DRAIN_ESSENCE                     = 5,
    EVENT_DRAIN_ESSENCE_ILLIDAN             = 6,
    EVENT_MOVE_TO_COMBAT_AREA               = 7,
    EVENT_TELEPORT_TO_HOME_POS              = 8,
    EVENT_CAMOUFLAGE                        = 9,
    EVENT_START_EYE_EVENT                   = 10,
    EVENT_END_EYE_EVENT                     = 11,
    EVENT_UPDATE_DEMON_PATHING              = 12,
    EVENT_HALFWAY_EYE_EVENT                 = 13,
    EVENT_SET_FIGHT_ACTIVE                  = 14,
    EVENT_SAY_PORTAL_SHUTDOWN               = 15,
    EVENT_LURK_CENTER_COURTYARD             = 16,
    EVENT_TALK_PEROTHARN_INTRO_TWO          = 17,
    EVENT_TALK_PEROTHARN_INTRO_THREE        = 18,
    // Legion Demon
    EVENT_MOVE_TO_PORTAL                    = 12,
    // Illidan
    EVENT_RESTART_ESCORT                    = 1,
    EVENT_SHADOWCLOAK_CHECK                 = 2,
    EVENT_MOVE_TO_NEXT_PORTAL               = 3,
    EVENT_START_BOSS_FIGHT                  = 4,
    EVENT_PLAYER_IN_COMBAT_CHECK            = 5,
    EVENT_PLAYER_ALIVE_CHECK                = 6,
    EVENT_LOCK_TRAFFIC_1,
    EVENT_LOCK_TRAFFIC_2,
    EVENT_LOCK_TRAFFIC_3,
    EVENT_LOCK_TRAFFIC_4,
    EVENT_LOCK_TRAFFIC_5,
    EVENT_LOCK_TRAFFIC_6,
    EVENT_LOCK_TRAFFIC_7,
    EVENT_UNLOCK_TRAFFIC,
    // Instance pre event
    EVENT_START_PEROTHARN_INTRO             = 1,
    EVENT_START_DEMON_MOVEMENT              = 2,
    EVENT_REMOVE_FLAGS                      = 3,
    // Legion Demon
    EVENT_STRIKE_FEAR                       = 1,
    // Dreadlord Defender
    EVENT_DEMONIC_WARDING                   = 1,
    EVENT_CARRION_SWARM                     = 2,
    EVENT_START_NEXT_WAYPOINT               = 3,
    EVENT_LAST_WAYPOINT                     = 4,
    // Corrupted Arcanist
    EVENT_ARCANE_ANNIHILATION               = 1,
    EVENT_CHECK_MANA_POOL                   = 2
};

enum Points
{
    POINT_COMBAT_AREA       = 1,
    POINT_EVENT_WALK        = 2,
    POINT_PORTAL            = 3,
    POINT_EYE_PHASE         = 4,
    POINT_SUMMON_FIREWALL   = 5,
    POINT_EVENT_WALK_TWO    = 7
};

enum Phases
{
    EVENT_PHASE_COMBAT                       = 1,
    EVENT_PHASE_INTRO                        = 2,
};

enum summonGroups
{
    SUMMON_GROUP_DEMON_WAVE_PORTAL_THREE    = 1,
    SUMMON_GROUP_DEMON_WAVE_PORTAL_ONE      = 2,
    SUMMON_GROUP_DEMON_WAVE_PORTAL_TWO      = 3,
    SUMMON_GROUP_EVENT_INTRO                = 1,
};

enum AnimKits
{
    ANIM_ONE_SHOT_CIRCLE    = 1688,
};

enum actions
{
    ACTION_START_EVENT              = 1,
    ACTION_SET_FIRST_FIREWALL_DEMON = 2,
    ACTION_SALUTE_EMOTE             = 3,
    ACTION_DEMON_KILLED             = 4,
    ACTION_MOVE_NEXT_PORTAL         = 5,
    ACTION_PLAYER_FOUND             = 6,
    ACTION_DEMON_MOVE_SPEED         = 7,
    ACTION_CLEAN_COMBAT             = 8,
    ACTION_APPEAR_MID               = 9,
    ACTION_ILLIDAN_REAPPLY          = 10,
    // Illidan ACtions
    ACTION_ILLIDAN_START_EVENT      = 1,
    ACTION_ILLIDAN_FINISH_EVENT     = 2,
    ACTION_ILLIDAN_PORTAL_CLOSED    = 3,
    ACTION_ILLIDAN_COMBAT_FINISHED  = 4
};

enum Factions
{
    ILLIDAN_FACTION_ATTACKER    = 2385,
    ILLIDAN_FACTION_ESCORT      = 2386
};

enum QuestData
{
    QUEST_IN_UNENDING_NUMBERS   = 30099,
    QUEST_CREDIT_PORTAL_ONE     = 58239,
    QUEST_CREDIT_PORTAL_TWO     = 58240,
    QUEST_CREDIT_PORTAL_THREE   = 58241,
};

enum npcs
{
    NPC_ILLIDAN_SHADOWCLOAK_STALKER = 56389,
    NPC_PLAYER_SHADOWCLOAK_STALKER  = 55154,
    NPC_PEROTHARN_INTRO             = 55085
};

const Position portalPositions[3] =
{
    { 3256.285f, -4942.128f, 188.679f, 0.000f },    //LEFT_TURN     portal
    { 3287.178f, -4816.500f, 188.471f, 0.000f },    //STRAIGHT      portal
    { 3414.551f, -4842.202f, 188.704f, 0.000f }      //RIGHT_TURN    portal
};

enum DemonPoints
{
    DEMON_SPAWN_LEADER          = 0,
    DEMON_SPAWN_FWD_RIGHT,
    DEMON_SPAWN_AFT_RIGHT,
    DEMON_SPAWN_AFT_LEFT,

    DEMON_SPAWN_FAKE_RIGHT,
    DEMON_SPAWN_FAKE_LEFT,

    POINT_CENTER_FROM_RIGHTPATH  = 1,
    POINT_CENTER_FROM_LEFTPATH,

    POINT_RIGHTPORTAL_FROM_RIGHTPATH,
    POINT_RIGHTPORTAL_FROM_LEFTPATH,

    POINT_LEFTPORTAL_FROM_RIGHTPATH,
    POINT_LEFTPORTAL_FROM_LEFTPATH,

    POINT_MIDPORTAL_FROM_RIGHTPATH,
    POINT_MIDPORTAL_FROM_LEFTPATH,

    POINT_FAKE_LEFT_FINAL,
    POINT_FAKE_RIGHT_FINAL,
};

const Position DemonSpawns[6]
{
    { 3392.963f, -4975.567f, 197.3622f, 2.102f },   //FWD "LEFT PATH"
    { 3388.211f, -4979.317f, 196.7823f, 2.102f },   //FWD "RIGHT PATH"
    { 3388.211f, -4979.317f, 196.7823f, 2.102f },   //AFT "RIGHT PATH"
    { 3392.963f, -4975.567f, 197.3622f, 2.102f },   //AFT "LEFT PATH"

    //Fake spawns, used as a cosmetic extension near the citadel
    { 3428.311035f, -5034.955566f, 196.782669f, 2.137467f },
    { 3423.635986f, -5037.802734f, 196.783066f, 2.117831f },
};

uint32 const demonPathSize{ 3 };

const G3D::Vector3 demonLeftPath_fake[demonPathSize] =   //Intiial path, left if the stairs are fwd
{
    { 3428.311035f, -5034.955566f, 196.782669f },
    { 3404.174561f, -5005.493652f, 196.783066f },
    { 3392.800293f, -4987.514648f, 196.782669f },
};
const G3D::Vector3 demonRightPath_fake[demonPathSize] =   //Intiial path, left if the stairs are fwd
{
    { 3423.635986f, -5037.802734f, 196.783066f },
    { 3409.695801f, -5001.699707f, 196.782669f },
    { 3398.437988f, -4983.896484f, 196.782669f },   //solid
};

const G3D::Vector3 demonLeftPath_to_middle[demonPathSize] =   //Intiial path, left if the stairs are fwd
{
    { 3392.963f, -4975.567f, 197.3622f },
    { 3380.213f, -4954.817f, 181.8622f },
    { 3345.230713f, -4901.197754f, 181.077896f},   //solid
};
const G3D::Vector3 demonRightPath_to_middle[demonPathSize] =  //Initial path, right if the stairs are fwd
{
    { 3396.493f, -4992.185f, 197.3622f },
    {3373.626221f, -4958.200195f, 181.8622f },
    { 3340.493f, -4904.435f, 181.3785f },
};

const G3D::Vector3 demonLeftPath_to_left[demonPathSize] =   //second path, left to left
{ 
    {3341.405029f, -4893.106934f, 181.077560f},
    {3346.359375f, -4889.054199f, 181.077560f},
    {3414.013916f, -4846.238770f, 181.705017f},
};
const G3D::Vector3 demonRightPath_to_left[demonPathSize] =   //second path, right to left
{
    {3337.777832f, -4893.294434f, 181.077271f},
    {3343.374268f, -4881.779785f, 181.077271f},
    {3408.614258f, -4841.757812f, 181.705734f},
};

const G3D::Vector3 demonLeftPath_to_bottom[demonPathSize] =   //second path, left to mid
{
    {3334.832520f, -4883.278809f, 181.077240f},
    {3311.460693f, -4846.777832f, 181.077240f},
    {3292.374268f, -4818.967285f, 181.471054f},
};
const G3D::Vector3 demonRightPath_to_bottom[demonPathSize] =   //second path, right to mid
{ 
    {3332.553223f, -4894.373047f, 181.078278f},
    {3310.552246f, -4860.894531f, 181.078278f},
    {3286.972412f, -4822.000000f, 181.471802f},
};

const G3D::Vector3 demonLeftPath_to_right[demonPathSize] =   //second path, left to mid
{
    {3335.596680f, -4893.947754f, 181.076889f},
    {3320.793945f, -4895.947266f, 181.076889f},
    {3258.529053f, -4937.021484f, 181.680038f},
};
const G3D::Vector3 demonRightPath_to_right[demonPathSize] =   //second path, right to mid
{ 
    {3333.456787f, -4900.165527f, 181.077942f},
    {3326.677979f, -4902.268555f, 181.077942f},
    {3261.236572f, -4943.220703f, 181.679153f},
};


const Position huntingCirclePos[8] =
{
    { 3338.792f, -4898.623f, 181.2851f, 0.000f },
    { 3331.352f, -4884.458f, 181.2851f, 0.000f },
    { 3332.695f, -4899.179f, 181.2851f, 0.000f },
    { 3337.449f, -4883.901f, 181.2851f, 0.000f },
    { 3327.990f, -4895.260f, 181.2851f, 0.000f },
    { 3342.154f, -4887.820f, 181.2851f, 0.000f },
    { 3327.433f, -4889.163f, 181.2851f, 0.000f },
    { 3342.711f, -4893.917f, 181.2851f, 0.000f }
};

const Position firewallPeriodicPositions[7] =
{
    { 3204.615f, -4935.365f, 194.4112f, 0.000f },
    { 3182.099f, -4933.120f, 194.4112f, 0.000f },
    { 3201.065f, -4937.566f, 194.4112f, 0.000f },
    { 3185.144f, -4935.979f, 194.4112f, 0.000f },
    { 3197.066f, -4938.774f, 194.4112f, 0.000f },
    { 3188.825f, -4937.952f, 194.4112f, 0.000f },
    { 3192.892f, -4938.905f, 194.4112f, 0.000f }
};

const Position DemonStalkerPos[4] =
{
    { 3324.963f, -4898.221f, 181.243f, 0.000f },
    { 3321.393f, -4900.539f, 181.285f, 0.000f },
    { 3317.850f, -4902.840f, 181.284f, 0.000f },
    { 3314.297f, -4905.146f, 181.284f, 0.000f }
};

const Position demonEndPos = { 3193.616f, -4940.210f, 189.525f, 5.119f };
const Position huntingStalkerPos = { 3335.072f, -4891.54f, 181.1601f, 0.000f };
const Position eyeSummonPos = { 3335.223f, -4891.458f, 181.076f, 0.000f };


enum Demon_actions
{
    ACTION_INTIALIZE_LEADER_MOVEMENT = 1,
    ACTION_INITIALIZE_FWD_RIGHT_FOLLOW,
    ACTION_INITIALIZE_AFT_RIGHT_FOLLOW,
    ACTION_INITIALIZE_AFT_LEFT_FOLLOW,
    ACTION_INITIALIZE_FAKE_LEFT,
    ACTION_INITIALIZE_FAKE_RIGHT,
};

class DelayedEyeSummonEvent : public BasicEvent
{
public:
    DelayedEyeSummonEvent(Unit* trigger) : _trigger(trigger)
    {
    }

    bool Execute(uint64 /*time*/, uint32 /*diff*/)
    {
        _trigger->CastSpell(_trigger, SPELL_SUMMON_CIRCLE_SPAWN, true);
        return true;
    }

private:
    Unit* _trigger;
};

class boss_woe_perotharn : public CreatureScript
{
public:
    boss_woe_perotharn() : CreatureScript("boss_woe_perotharn") {}

    struct boss_woe_perotharnAI : public BossAI
    {
        boss_woe_perotharnAI(Creature* creature) : BossAI(creature, DATA_PEROTHARN) {}

        void InitializeAI() override
        {
            demonWaves = 0;
            demonWavesPortalOneAndTwo = 0;
            eyeCount = 0;
            eyeTimer = 1000;
            skipUpdateVictim = true;
            drainEssenceTriggered = false;
            lazyEyeAchievementDone = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
            BossAI::InitializeAI();
            events.ScheduleEvent(EVENT_SUMMON_DEMONS, 1000);
            events.ScheduleEvent(EVENT_UPDATE_DEMON_PATHING, 8000);

            for (auto i = 0; i <= 2; ++i)
            if (auto c = me->SummonCreature(56087, portalPositions[i]))
            {
                //moved to it's own script
            }
        }

        void Reset() override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveAllAuras();
            DoCast(me, SPELL_CORRUPTING_TOUCH, true);
            drainEssenceTriggered = false;
            lazyEyeAchievementDone = true;
            eyeTimer = 1000;
            eyeCount = 0;
            _Reset();

            std::list<GameObject*> objs;
            GetGameObjectListWithEntryInGrid(objs, me, 210130, 100.f);
            if (objs.size())
                for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                    if (auto rock = (*itr))
                        switch (rock->GetDBTableGUIDLow())
                        {
                        case 124070: //entrance
                            rock->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            rock->SetGoState(GO_STATE_READY);
                            break;
                        default:
                            break;
                        }
            if (Ready_to_Fight)
                me->SetFacingTo(2.17f);
        }

        void DamageTaken(Unit* attacker, uint32 &damage) override
        {
            if (!drainEssenceTriggered && me->HealthBelowPctDamaged(60, damage))
            {
                me->DespawnCreaturesInArea(NPC_FEL_FLAMES_TRIGGER, 200.f);
                drainEssenceTriggered = true;
                skipUpdateVictim = true;
                Talk(TALK_DRAIN_ESSENCE);
                me->SetReactState(REACT_PASSIVE);
                me->AttackStop();
                me->StopMoving();
                events.Reset();
                DoCast(SPELL_DRAIN_ESSENCE);
                events.ScheduleEvent(EVENT_DRAIN_ESSENCE_ILLIDAN, 4000);
            }
            else if (me->HealthBelowPctDamaged(20, damage) && !me->HasAura(SPELL_ENDLESS_FRENZY))
            {
                Talk(TALK_FRENZY);
                DoCast(me, SPELL_ENDLESS_FRENZY, true);
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* killer) override
        {
            if (summon->GetEntry() == NPC_EYE_OF_THE_PEROTHARN)
                if (auto p = killer->ToPlayer())
                    player_guid_found = p->GetGUID();
            Talk(0);
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_DESPAWN_PORTAL_ONE:
                case ACTION_DESPAWN_PORTAL_TWO:
                case ACTION_DESPAWN_PORTAL_THREE:
                    // Here we use doaction to doaction because with a grid searcher we need ~500m search radius... here we have a summonlist...
                    for (uint64 summonGUID : summons)
                        if (auto summon = ObjectAccessor::GetCreature(*me, summonGUID))
                            summon->AI()->DoAction(action);
                    if (action == ACTION_DESPAWN_PORTAL_THREE)
                    {
                        me->SetVisible(true);
                        me->SetFacingTo(2.1f);
                        events.ScheduleEvent(EVENT_SAY_PORTAL_SHUTDOWN, 15000);
                    }
                    break;
                case ACTION_START_COMBAT:
                {
                    DoCast(me, SPELL_CAMOUFLAGE, true);
                    //DoCast(me, 37803, true);
                    me->SetVisible(true);
                    me->RemoveAurasDueToSpell(SPELL_CORRUPTING_TOUCH);
                    DoNotSummonDemons = true;
                    Ready_to_Fight = true;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    //me->DespawnCreaturesInArea(NPC_LEGION_DEMON_MASSES, 300.00f);//DEV PURPOSES ONLY
                    me->DespawnCreaturesInArea(NPC_GUARDIAN_DEMON, 300.00f);
                    me->DespawnCreaturesInArea(56087, 300.f);
                    //despawn stalactites

                    std::list<GameObject*> objs;
                    GetGameObjectListWithEntryInGrid(objs, me, 210130, 100.f);
                    if (objs.size())
                        for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                            if (auto rock = (*itr))
                                switch (rock->GetDBTableGUIDLow())
                                {
                                case 124070: //entrance
                                    rock->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                                    rock->SetGoState(GO_STATE_READY);
                                    break;
                                default:
                                    break;
                                }
                    break;
                }
                case ACTION_PLAYER_FOUND:
                    me->DespawnCreaturesInArea(144951);

                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                        me->CastWithDelay(7000, illidan, SPELL_END_EXHAUSTED, true);

                    lazyEyeAchievementDone = false;
                    events.CancelEvent(EVENT_END_EYE_EVENT);
                    events.CancelEvent(EVENT_HALFWAY_EYE_EVENT);
                    DoCastAOE(SPELL_CANCEL_FEL_FIREBALL_AGGRO);
                    //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_WALK);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    summons.DespawnEntry(NPC_HUNTING_CIRCLE);
                    summons.DespawnEntry(NPC_HUNTING_STALKER);
                    summons.DespawnEntry(NPC_EYE_OF_THE_PEROTHARN);
                    skipUpdateVictim = false;
                    events.ScheduleEvent(EVENT_FEL_FLAMES, 15000);
                    events.ScheduleEvent(EVENT_FEL_DECAY, 18000);
                    if (player_guid_found)
                        if (auto player = ObjectAccessor::FindPlayer(player_guid_found))
                        {
                            DoResetThreat();
                            me->GetMotionMaster()->Clear();
                            me->StopMoving();
                            me->AddThreat(player, 5000.f);
                            me->CastWithDelay(1000, player, SPELL_CHARGE_PLAYER);
                        }
                    break;
                case ACTION_APPEAR_MID:
                    if (Ready_to_Fight)
                    {
                        me->RemoveAurasDueToSpell(SPELL_CAMOUFLAGE);
                        me->RemoveAurasDueToSpell(37803);
                        events.ScheduleEvent(EVENT_SET_FIGHT_ACTIVE, 8000);
                        DoCast(me, SPELL_CORRUPTING_TOUCH, true);
                        Talk(5);
                        if (auto c = me->SummonCreature(144951, *me, TEMPSUMMON_TIMED_DESPAWN, 6000))
                        {
                            c->SetVisible(true);
                            c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            c->setFaction(35);
                            c->SetObjectScale(10.f);
                            c->CastWithDelay(500, c, 74620, true);
                            c->CastWithDelay(1000, c, 74620, true);
                            c->CastWithDelay(1500, c, 74620, true);
                        }
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    break;
                default:
                    break;
            }
        }

        void EnterCombat(Unit* victim) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            TalkWithDelay(2000, TALK_AGGRO);
            if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
            {
                illidan->AI()->TalkWithDelay(500, TALK_ILLIDAN_AGGRO_BOSS);
                illidan->AI()->AttackStart(me);
            }
            skipUpdateVictim = false;
            events.ScheduleEvent(EVENT_FEL_FLAMES, 5000);
            events.ScheduleEvent(EVENT_FEL_DECAY, 8000);
            _EnterCombat();

            if (!me->HasAura(SPELL_CORRUPTING_TOUCH))
                DoCast(me, SPELL_CORRUPTING_TOUCH, true);
        }

        uint32 GetData(uint32 data) const override
        {
            if (data == DATA_LAZY_EYE_ACHIEVEMENT)
                return lazyEyeAchievementDone ? 1 : 0;

            return 0;
        }

        void JustDied(Unit* killer) override
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            Talk(TALK_DEATH);
            _JustDied();
            if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                illidan->AI()->DoAction(ACTION_COMBAT_FINISHED);
            if (auto nozdormu = instance->GetCreature(DATA_NOZDORMU))
                nozdormu->NearTeleportTo(NozdormuPositions[0]);
            summons.DespawnAll();

            me->DespawnCreaturesInArea(NPC_FEL_FLAMES_TRIGGER, 200.f);
            me->DespawnCreaturesInArea(57329);
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
                case NPC_FEL_FLAMES_TRIGGER:
                    summon->CastSpell(summon, SPELL_FEL_FLAMES_PERIODIC, true);
                    break;
                case NPC_HUNTING_CIRCLE:
                    if (++eyeCount > 2)
                    {
                        eyeTimer += 1000;
                        eyeCount = 1;
                    }
                    summon->m_Events.AddEvent(new DelayedEyeSummonEvent(summon), summon->m_Events.CalculateTime(eyeTimer));
                    break;
                case NPC_HUNTING_STALKER:
                    summon->CastSpell(summon, SPELL_PUNISHING_FLAMES, true);
                    summon->CastSpell((Unit*)NULL, SPELL_FEL_FIREBALL_AGGRO, true);
                    summon->CastSpell(summon, SPELL_HUNTING_CHANNEL, true);
                    summon->AI()->Talk(TALK_EYE_EVENT_WARNING, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    break;
                case NPC_EYE_OF_THE_PEROTHARN:
                    summon->CastSpell(summon, SPELL_INVISIBILITY_DETECTION, true);
                    summon->SetFacingTo(me->GetAngle(summon));
                    summon->SetSpeed(MOVE_WALK, 4);
                    summon->SetSpeed(MOVE_RUN, 4);
                    summon->SetWalk(true);
                    //Scripted movement in it's own script.
                    break;
                default:
                    break;
            }
        }

        void KilledUnit(Unit* victim) override
        {

        }
        void SpellHitTarget(Unit* target, SpellInfo const* spell) 
        {
            if (spell)
                if (spell->Id == 41421)
                {
                    if (auto aura = target->GetAura(spell->Id))
                    {
                        aura->SetMaxDuration(5000);
                        aura->SetDuration(5000);
                    }
                }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                    case POINT_EVENT_WALK:
                        events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 100);
                        break;
                    case POINT_COMBAT_AREA:
                    {
                        Position newHomePos(*me);
                        me->SetHomePosition(newHomePos);
                        me->UpdateObjectVisibility(true);
                        me->SetVisible(false);
                        me->SetFacingTo(2.17f);
                        //DoAction(ACTION_START_COMBAT);//DEV PURPOSES ONLY
                        //events.ScheduleEvent(EVENT_SAY_PORTAL_SHUTDOWN, 2000);//DEV PURPOSES ONLY
                        break;
                    }
                    case 1003:
                        me->RemoveAurasDueToSpell(SPELL_CAMOUFLAGE);
                        //easy prey charge has completed
                        Talk(1);
                        break;
                    default:
                        break;
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!skipUpdateVictim && !UpdateVictim())
                return;

            events.Update(diff);
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                case EVENT_SET_FIGHT_ACTIVE:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    break;
                case EVENT_MOVE_TO_COMBAT_AREA:
                    me->SetHomePosition(eventWaypoints[3]);
                    me->GetMotionMaster()->MovePoint(POINT_COMBAT_AREA, eventWaypoints[3]);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                    break;
                case EVENT_SUMMON_DEMONS:
                    if (DoNotSummonDemons)
                    {
                        me->DespawnCreaturesInArea(NPC_LEGION_DEMON_MASSES, 45.f);
                    }
                    else
                    {
                        if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_LEADER]))
                            c->AI()->DoAction(ACTION_INTIALIZE_LEADER_MOVEMENT);
                        events.ScheduleEvent(EVENT_SUMMON_DEMONS, 1600);
                    }
                    break;
                case EVENT_FEL_FLAMES:
                    DoCastRandom(SPELL_FEL_FLAMES, 150.00f);
                    events.ScheduleEvent(EVENT_FEL_FLAMES, 8400);
                    break;
                case EVENT_FEL_DECAY:
                    DoCastRandom(SPELL_FEL_DECAY, 150.00f, false, -SPELL_FEL_DECAY);
                    events.ScheduleEvent(EVENT_FEL_DECAY, 17000);
                    break;
                case EVENT_DRAIN_ESSENCE_ILLIDAN:
                    DoCastAOE(SPELL_FEL_ADDLED);
                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                    {
                        illidan->AI()->Talk(TALK_ILLIDAN_ABSORB);
                        illidan->CastSpell(me, SPELL_ABSORB_FEL_ENERGY, false);
                    }
                    events.ScheduleEvent(EVENT_CAMOUFLAGE, 5000);
                    break;
                case EVENT_CAMOUFLAGE:
                    DoResetThreat();
                    me->AddAura(108214, me);
                    Talk(TALK_INTO_SHADOWS);
                    Talk(TALK_INTO_SHADOWS_EMOTE, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveAllAuras();
                    DoCast(me, SPELL_CORRUPTING_TOUCH, true);
                    DoCast(me, SPELL_CAMOUFLAGE, true);
                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                    {
                        illidan->CastSpell(illidan, SPELL_REGENERATION, true);
                        illidan->TalkWithDelay(1000, TALK_ILLIDAN_RETURN, NULL, CHAT_MSG_MONSTER_SAY);
                    }
                    events.ScheduleEvent(EVENT_START_EYE_EVENT, 3000);
                    break;
                case EVENT_START_EYE_EVENT:
                    Talk(TALK_EYE_EVENT);
                    me->NearTeleportTo(eyeSummonPos);
                    for (int8 i = 0; i < 9; i++)
                        me->SummonCreature(NPC_HUNTING_CIRCLE, huntingCirclePos[i], TEMPSUMMON_TIMED_DESPAWN, 40000);
                    me->SummonCreature(NPC_HUNTING_STALKER, huntingStalkerPos, TEMPSUMMON_TIMED_DESPAWN, 40000);

                    if (auto c = me->SummonCreature(144951, eyeSummonPos, TEMPSUMMON_TIMED_DESPAWN, 40000))
                    {
                        c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        c->AddAura(108214, c);
                    }

                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                        illidan->AI()->DoAction(ACTION_CLEAN_COMBAT);

                    events.ScheduleEvent(EVENT_END_EYE_EVENT, 40000);
                    events.ScheduleEvent(EVENT_HALFWAY_EYE_EVENT, 20000);
                    break;
                case EVENT_HALFWAY_EYE_EVENT:
                    Talk(2);
                    break;
                case EVENT_END_EYE_EVENT:
                    me->DespawnCreaturesInArea(144951);

                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                        illidan->AI()->Talk(TALK_ILLIDAN_STRENGTH_RETURN);
                    TalkWithDelay(3000, TALK_YOU_HIDE_WELL);
                    DoCastAOE(SPELL_CANCEL_FEL_FIREBALL_AGGRO);
                    //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_WALK);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    instance->DoRemoveAuraStackOnPlayers(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveAurasDueToSpell(SPELL_CAMOUFLAGE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    summons.DespawnEntry(NPC_EYE_OF_THE_PEROTHARN);
                    if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                        DoCast(illidan, SPELL_END_EXHAUSTED, true);
                    DoCast(me, SPELL_ENFEEBLED, true);
                    skipUpdateVictim = false;
                    events.ScheduleEvent(EVENT_FEL_FLAMES, 5000);
                    events.ScheduleEvent(EVENT_FEL_DECAY, 8000);
                    break;
                case EVENT_UPDATE_DEMON_PATHING:
                    break;
                case EVENT_SAY_PORTAL_SHUTDOWN:
                    me->DespawnCreaturesInArea(NPC_LEGION_DEMON_MASSES, 45.f);
                    DoNotSummonDemons = true;
                    me->SetVisible(true);
                    me->SetFacingTo(2.1f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Talk(TALK_PORTAL_SHUTDOWN, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                    events.ScheduleEvent(EVENT_LURK_CENTER_COURTYARD, 9000);
                    break;
                case EVENT_LURK_CENTER_COURTYARD:
                    DoAction(ACTION_START_COMBAT);
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(8888, eventWaypoints[2], true, 35.f);
                    me->SetHomePosition(eventWaypoints[2]);
                    break;
                default:
                    break;
                }

                if (UpdateVictim())
                DoMeleeAttackIfReady();
        }
        private:
        bool Ready_to_Fight{ false };
        bool DoNotSummonDemons { false };
        bool drainEssenceTriggered;
        bool skipUpdateVictim;
        bool lazyEyeAchievementDone;
        uint8 demonWaves;
        uint8 demonWavesPortalOneAndTwo;
        uint32 eyeTimer;
        uint32 eyeCount;
        uint64 player_guid_found{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_woe_perotharnAI(creature);
    }
};

class npc_woe_legion_demon : public CreatureScript
{
public:
    npc_woe_legion_demon() : CreatureScript("npc_woe_legion_demon") {}

    struct npc_woe_legion_demonAI : public ScriptedAI
    {
        npc_woe_legion_demonAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
        }
        void InitializeAI() override
        {
            me->SetReactState(REACT_PASSIVE);
        }
        
        int32 formationPosition{ 0 };
        
        void Reset()
        {
            if (enteredCombat)
                me->DespawnOrUnsummon();
        }

        void EnterCombat(Unit* target) override
        {
            enteredCombat = true;
            me->SetReactState(REACT_AGGRESSIVE);
        }
        void DamageTaken(Unit* attacker, uint32& damage)
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->CallForHelp(20.f);
        }

        void SetData(uint32 data_field, uint32 data_value)
        {
            /*
                This is how our demon will judge where it needs to go.
                Every Demon will receieve info on what the most recent destination of a demon formation position was.
            */
            previous_traffic_pulse[data_field] = data_value;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell) 
        {
            if (spell->Id == 105721)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->AddThreat(caster, 10000.f);
                me->CallForHelp(20.f);
            }
        }

        uint32 get_decider_position()
        {
            switch(formationPosition)
            {
            case DEMON_SPAWN_LEADER:    //FWD LEFT
                return DEMON_SPAWN_AFT_LEFT;
                break;
            case DEMON_SPAWN_AFT_LEFT:  //BACK LEFT
                return DEMON_SPAWN_LEADER;
                break;
            case DEMON_SPAWN_AFT_RIGHT: //BACK RIGHT
                return DEMON_SPAWN_FWD_RIGHT;
                break;
            case DEMON_SPAWN_FWD_RIGHT: //FWD RIGHT
                return DEMON_SPAWN_AFT_RIGHT;
                break;
            }
            TC_LOG_ERROR("sql.sql", "OUT OF RANGE VALUE FOR get_decider_position: %u", formationPosition);
            return DEMON_SPAWN_LEADER;
        };
        uint32 get_next_path()
        {
            switch (previous_traffic_pulse[get_decider_position()])
            {
            case 0://first demons
                switch (formationPosition)
                {
                case DEMON_SPAWN_LEADER:    //FWD LEFT
                    return check_alternate_required(POINT_MIDPORTAL_FROM_LEFTPATH);
                    break;
                case DEMON_SPAWN_AFT_LEFT:  //BACK LEFT
                    return check_alternate_required(POINT_LEFTPORTAL_FROM_LEFTPATH);
                    break;
                case DEMON_SPAWN_AFT_RIGHT: //BACK RIGHT
                    return check_alternate_required(POINT_LEFTPORTAL_FROM_RIGHTPATH);
                    break;
                case DEMON_SPAWN_FWD_RIGHT: //FWD RIGHT
                    return check_alternate_required(POINT_MIDPORTAL_FROM_RIGHTPATH);
                    break;
                }
                return check_alternate_required(POINT_MIDPORTAL_FROM_LEFTPATH);
                break;
            case POINT_RIGHTPORTAL_FROM_RIGHTPATH:
                return check_alternate_required(POINT_MIDPORTAL_FROM_RIGHTPATH);
                break;
            case POINT_RIGHTPORTAL_FROM_LEFTPATH:
                return check_alternate_required(POINT_MIDPORTAL_FROM_LEFTPATH);
                break;
            case POINT_LEFTPORTAL_FROM_RIGHTPATH:
                return check_alternate_required(POINT_RIGHTPORTAL_FROM_RIGHTPATH);
                break;
            case POINT_LEFTPORTAL_FROM_LEFTPATH:
                return check_alternate_required(POINT_RIGHTPORTAL_FROM_LEFTPATH);
                break;
            case POINT_MIDPORTAL_FROM_RIGHTPATH:
                return check_alternate_required(POINT_LEFTPORTAL_FROM_RIGHTPATH);
                break;
            case POINT_MIDPORTAL_FROM_LEFTPATH:
                return check_alternate_required(POINT_LEFTPORTAL_FROM_LEFTPATH);
                break;
            }
            TC_LOG_ERROR("sql.sql", "TRAFFIC DEMON: OUT OF RANGE VALUE FOR get_next_path: %u", previous_traffic_pulse[get_decider_position()]);
            return check_alternate_required(POINT_MIDPORTAL_FROM_LEFTPATH);
        }
        uint32 check_alternate_required(uint32 desired_demon_path)
        {
            switch (desired_demon_path)
            {
                case POINT_RIGHTPORTAL_FROM_RIGHTPATH:
                    return (instance->GetData(DATA_LEGION_MOVEMENT) < 1 ? desired_demon_path : POINT_MIDPORTAL_FROM_RIGHTPATH);
                    break;
                case POINT_RIGHTPORTAL_FROM_LEFTPATH:
                    return (instance->GetData(DATA_LEGION_MOVEMENT) < 1 ? desired_demon_path : POINT_MIDPORTAL_FROM_LEFTPATH);
                    break;
                case POINT_LEFTPORTAL_FROM_RIGHTPATH:
                    return (instance->GetData(DATA_LEGION_MOVEMENT) < 2 ? desired_demon_path : POINT_MIDPORTAL_FROM_RIGHTPATH);
                    break;
                case POINT_LEFTPORTAL_FROM_LEFTPATH:
                    return (instance->GetData(DATA_LEGION_MOVEMENT) < 2 ? desired_demon_path : POINT_MIDPORTAL_FROM_LEFTPATH);
                    break;
                case POINT_MIDPORTAL_FROM_RIGHTPATH:
                    return POINT_MIDPORTAL_FROM_RIGHTPATH;
                    break;
                case POINT_MIDPORTAL_FROM_LEFTPATH:
                    return POINT_MIDPORTAL_FROM_LEFTPATH;
                    break;
            }
            return POINT_LEFTPORTAL_FROM_LEFTPATH;
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == SPLINE_MOTION_TYPE)
                if (id == 2)
                {
                    if (currentPath == POINT_CENTER_FROM_LEFTPATH || currentPath == POINT_CENTER_FROM_RIGHTPATH)
                    {
                        switch (get_next_path())
                        {
                        case POINT_RIGHTPORTAL_FROM_RIGHTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonRightPath_to_right, demonPathSize);
                            break;
                        case POINT_RIGHTPORTAL_FROM_LEFTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonLeftPath_to_right, demonPathSize);
                            break;
                        case POINT_LEFTPORTAL_FROM_RIGHTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonRightPath_to_left, demonPathSize);
                            break;
                        case POINT_LEFTPORTAL_FROM_LEFTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonLeftPath_to_left, demonPathSize);
                            break;
                        case POINT_MIDPORTAL_FROM_RIGHTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonRightPath_to_bottom, demonPathSize);
                            break;
                        case POINT_MIDPORTAL_FROM_LEFTPATH:
                            me->GetMotionMaster()->MoveSmoothPath(demonLeftPath_to_bottom, demonPathSize);
                            break;
                        default:
                            break;
                        }
                        currentPath = get_next_path();
                        DoAction(ACTION_PULSE_TRAFFIC_UPDATE);
                    }
                    else if (currentPath == POINT_FAKE_LEFT_FINAL || currentPath == POINT_FAKE_RIGHT_FINAL)
                    {
                        me->DespawnOrUnsummon();
                    }
                    else
                        me->DespawnOrUnsummon();
                }
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_INTIALIZE_LEADER_MOVEMENT:
                    formationPosition = DEMON_SPAWN_LEADER;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 200);
                    if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_FWD_RIGHT]))
                        c->AI()->DoAction(ACTION_INITIALIZE_FWD_RIGHT_FOLLOW);
                    if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_AFT_RIGHT]))
                        c->AI()->DoAction(ACTION_INITIALIZE_AFT_RIGHT_FOLLOW);
                    if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_AFT_LEFT]))
                        c->AI()->DoAction(ACTION_INITIALIZE_AFT_LEFT_FOLLOW);

                    //two will be spawned for cosmetic purposes to appear as though a line is coming from the palace
                    if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_FAKE_LEFT]))
                        c->AI()->DoAction(ACTION_INITIALIZE_FAKE_LEFT);
                    if (auto c = me->SummonCreature(NPC_LEGION_DEMON_MASSES, DemonSpawns[DEMON_SPAWN_FAKE_RIGHT]))
                        c->AI()->DoAction(ACTION_INITIALIZE_FAKE_RIGHT);
                    break;

                case ACTION_INITIALIZE_FWD_RIGHT_FOLLOW:
                    formationPosition = DEMON_SPAWN_FWD_RIGHT;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 200);
                    break;
                case ACTION_INITIALIZE_AFT_RIGHT_FOLLOW:
                    formationPosition = DEMON_SPAWN_AFT_RIGHT;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 1100);
                    break;

                case ACTION_INITIALIZE_AFT_LEFT_FOLLOW:
                    formationPosition = DEMON_SPAWN_AFT_LEFT;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 1100);
                    break;

                case ACTION_INITIALIZE_FAKE_LEFT:
                    formationPosition = DEMON_SPAWN_FAKE_LEFT;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 2000);
                    break;
                case ACTION_INITIALIZE_FAKE_RIGHT:
                    formationPosition = DEMON_SPAWN_FAKE_RIGHT;
                    events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 2000);
                    break;
                case ACTION_DESPAWN_PORTAL_ONE:
                    break;
                case ACTION_DESPAWN_PORTAL_TWO:
                    break;
                case ACTION_DESPAWN_PORTAL_THREE:
                    break;
                case ACTION_PULSE_TRAFFIC_UPDATE:
                {
                    //despawn stalactite triggers
                    std::list<Creature*> units;
                    GetCreatureListWithEntryInGrid(units, me, me->GetEntry(), 250.0f);
                    for (auto itr = units.begin(); itr != units.end(); ++itr)
                        if (auto c = (*itr))
                        {
                            if (auto demon = c->ToCreature())
                                demon->AI()->SetData(formationPosition, currentPath);
                        }

                    break;
                }
                case ACTION_RECOGNIZE_TRAFFIC_UPDATE:
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                case EVENT_TELEPORT_TO_HOME_POS:
                    break;
                case EVENT_MOVE_TO_COMBAT_AREA:
                    //TC_LOG_ERROR("sql.sql", "DEMON: UpdateAI EVENT_MOVE_TO_COMBAT_AREA");
                    switch (formationPosition)
                    {
                        case DEMON_SPAWN_LEADER:
                        case DEMON_SPAWN_AFT_LEFT:
                            me->GetMotionMaster()->MoveSmoothPath(demonLeftPath_to_middle, demonPathSize);
                            currentPath = POINT_CENTER_FROM_LEFTPATH;
                            break;
                        case DEMON_SPAWN_FWD_RIGHT:
                        case DEMON_SPAWN_AFT_RIGHT:
                            me->GetMotionMaster()->MoveSmoothPath(demonRightPath_to_middle, demonPathSize);
                            currentPath = POINT_CENTER_FROM_RIGHTPATH;
                            break;
                        case DEMON_SPAWN_FAKE_LEFT:
                            me->GetMotionMaster()->MoveSmoothPath(demonLeftPath_fake, demonPathSize);
                            currentPath = POINT_FAKE_LEFT_FINAL;
                            break;
                        case DEMON_SPAWN_FAKE_RIGHT:
                            me->GetMotionMaster()->MoveSmoothPath(demonRightPath_fake, demonPathSize);
                            currentPath = POINT_FAKE_RIGHT_FINAL;
                            break;
                        default:
                            TC_LOG_ERROR("sql.sql", "DEMON: formationPosition:%u", formationPosition);
                            break;
                    }
                    break;
                case EVENT_MOVE_TO_PORTAL:
                    break;
                }
            }

            if (!me->HasReactState(REACT_PASSIVE))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
        private:
            uint32 previous_traffic_pulse[4]{ 0, 0, 0, 0 };
            EventMap events;
            InstanceScript* instance;
            Position portalDest;
            uint32 currentPath{ 0 };
            bool enteredCombat{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_legion_demonAI(creature);
    }
};

#define GOSSIP_TEXT_ONE_READY "I am ready to be hidden by your shadowcloak."
#define GOSSIP_TEXT_ONE_GO    "Let's go!"

class npc_woe_illidan_part_one : public CreatureScript
{
public:
    npc_woe_illidan_part_one() : CreatureScript("npc_woe_illidan_part_one") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        InstanceScript* instance = creature->GetInstanceScript();
        switch (action)
        {
        case 1:
        {
            if (creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            {
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                creature->AI()->DoAction(ACTION_START_EVENT);
            }
            player->CLOSE_GOSSIP_MENU();
            break;
        }
        case 2:
        {
            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_ONE_GO, GOSSIP_SENDER_MAIN, 1);
            player->SEND_GOSSIP_MENU(18552, creature->GetGUID());
                break;
            }
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->isQuestGiver())
        {
            player->PrepareQuestMenu(creature->GetGUID());
            player->SendPreparedQuest(creature->GetGUID());
        }
        if (auto instance = creature->GetInstanceScript())
            if (!instance->IsDone(DATA_PEROTHARN))
                //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "start event", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_ONE_READY, GOSSIP_SENDER_MAIN, 2);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_woe_illidan_part_oneAI : public npc_escortAI
    {
        npc_woe_illidan_part_oneAI(Creature* creature) : npc_escortAI(creature)
        {
            me->GetPosition(&spawnPosition);
            instance = me->GetInstanceScript();
        }

        void InitializeAI() override
        {
            me->SetVisible(false);
            isInitialStart = true;
            EscortStarted = false;
            npc_escortAI::InitializeAI();
        }

        void Reset()
        {
            //DoCast(me, SPELL_DEMONIC_SIGHT, true);
            DoCast(me, SPELL_ILLIDAN_SHADOWCLOAK, true);
            //Position pos(*me);
            //me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, pos, TEMPSUMMON_MANUAL_DESPAWN);
        }

        void EnterCombat(Unit* target) override
        {
            Talk(TALK_ILLIDAN_AGGRO);
        }

        void EnterEvadeMode() override
        {
            if (!me->HasAura(SPELL_REGENERATION))
            {
                me->DeleteThreatList();
                me->CombatStop(true);
                DoCast(me, SPELL_DEMONIC_SIGHT, true);
            }
        }

        void ResetCombat(bool fullReset)
        {
            me->RemoveAurasDueToSpell(SPELL_REGENERATION);
            me->DeleteThreatList();
            me->CombatStop(true);
            me->setFaction(ILLIDAN_FACTION_ESCORT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            SetCanAttack(false);
            me->GetMotionMaster()->MoveTargetedHome();
            events.Reset();
            if (fullReset)
            {
                me->NearTeleportTo(spawnPosition);
                me->Kill(me);
                me->Respawn();
                EscortStarted = false;
            }
            else
            {
                events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 1000);
                events.ScheduleEvent(EVENT_SHADOWCLOAK_CHECK, 1000);
            }
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_START_EVENT:
                {
                    instance->SetData(DATA_LEGION_MOVEMENT, 0);
                    if (isInitialStart)
                    {
                        isInitialStart = false;
                        demonKillCounter = 0;
                        currentPortalAction = ACTION_DESPAWN_PORTAL_ONE;
                    }
                    Talk(TALK_ILLIDAN_HIDE_IN_SHADOWS, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                    TalkWithDelay(4000, TALK_ILLIDAN_START_MOVEMENT, NULL, CHAT_MSG_MONSTER_SAY);
                    events.ScheduleEvent(EVENT_SHADOWCLOAK_CHECK, 100);
                    DoCast(me, SPELL_ILLIDAN_SHADOWCLOAK, true);
                    Position pos(*me);
                    me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, pos, TEMPSUMMON_MANUAL_DESPAWN);
                    events.ScheduleEvent(EVENT_RESTART_ESCORT, 8000);
                    break;
                }
                case ACTION_DEMON_KILLED:
                {
                    if (++demonKillCounter == 3 || demonKillCounter == 6 || demonKillCounter == 7 || demonKillCounter == 10)
                    {
                        switch (demonKillCounter)
                        {
                            case 3:
                                if (auto energyFocus = instance->GetGameObject(DATA_ENERGY_FOCUS_ONE))
                                {
                                    Talk(TALK_ILLIDAN_DESTROY_THE_CRYSTALL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                                    energyFocus->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                                    energyFocus->SetFlag(GAMEOBJECT_DYNAMIC, 4294901760);
                                }
                                break;
                            case 6:
                                if (auto energyFocus = instance->GetGameObject(DATA_ENERGY_FOCUS_TWO))
                                {
                                    Talk(TALK_ILLIDAN_DESTROY_THE_CRYSTALL);
                                    energyFocus->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                                    energyFocus->SetFlag(GAMEOBJECT_DYNAMIC, 4294901760);
                                }
                                break;
                            case 7:
                                Talk(TALK_ILLIDAN_SHUT_DOWN_LAST_PORTAL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                                SetEscortPaused(false);
                                break;
                            case 10:
                                if (auto energyFocus = instance->GetGameObject(DATA_ENERGY_FOCUS_THREE))
                                {
                                    Talk(TALK_ILLIDAN_DESTROY_THE_CRYSTALL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                                    energyFocus->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                                    energyFocus->SetFlag(GAMEOBJECT_DYNAMIC, 4294901760);
                                }
                                break;
                            default:
                                break;
                        }
                        if (auto visual = me->FindNearestCreature(NPC_FEL_CRYSTAL_STALKER, 50.00f))
                            visual->CastSpell(visual, SPELL_ARCANE_EXPLOSION_VISUAL, true);
                        events.CancelEvent(EVENT_PLAYER_IN_COMBAT_CHECK); 
                        events.CancelEvent(EVENT_PLAYER_ALIVE_CHECK);
                        SetCanAttack(false);
                        me->setFaction(ILLIDAN_FACTION_ESCORT);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    }
                    break;
                }
                case ACTION_MOVE_NEXT_PORTAL:
                {
                    instance->SetData(DATA_LEGION_PORTAL_STATE, instance->GetData(DATA_LEGION_PORTAL_STATE) + 1);
                    instance->SetData(DATA_LEGION_MOVEMENT, instance->GetData(DATA_LEGION_PORTAL_STATE));
                    if (!me->HasAura(SPELL_ILLIDAN_SHADOWCLOAK))
                    {
                        DoCast(me, SPELL_ILLIDAN_SHADOWCLOAK, true);
                        Position pos(*me);
                        me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, pos, TEMPSUMMON_MANUAL_DESPAWN);
                    }
                    TalkWithDelay(5000, TALK_ILLIDAN_LEAVING);
                    if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                        perotharn->AI()->DoAction(currentPortalAction);

                    std::list<Creature*> list_of_npcs;
                    me->GetCreatureListWithEntryInGrid(list_of_npcs, 55541, 500.0f);
                    me->GetCreatureListWithEntryInGrid(list_of_npcs, 55542, 500.0f);
                    me->GetCreatureListWithEntryInGrid(list_of_npcs, 55543, 500.0f);
                    if (list_of_npcs.size())
                        for (auto c : list_of_npcs)
                        {
                            c->AI()->DoAction(currentPortalAction);
                        }
                    Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                        if (auto player = itr->getSource())
                            if (player->GetQuestStatus(QUEST_IN_UNENDING_NUMBERS) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(currentPortalAction == 10 ? QUEST_CREDIT_PORTAL_ONE : (currentPortalAction == 11 ? QUEST_CREDIT_PORTAL_TWO : QUEST_CREDIT_PORTAL_THREE));

                    events.ScheduleEvent(EVENT_RESTART_ESCORT, 6000);
                    currentPortalAction++;
                    break;
                }
                case ACTION_COMBAT_FINISHED:
                    events.Reset();
                    TalkWithDelay(3000, TALK_ILLIDAN_PEROTHARN_DEATH, NULL, CHAT_MSG_MONSTER_SAY);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOWCLOAK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    events.ScheduleEvent(EVENT_RESTART_ESCORT, 6000);
                    break;
                case ACTION_CLEAN_COMBAT:
                {
                    auto list_of_players = me->GetPlayersInRange(240.f, true);
                    for (auto itr = list_of_players.begin(); itr != list_of_players.end(); ++itr)
                        if (auto player = (*itr))
                            if (player->isInCombat() && !player->HasAura(SPELL_DEMONIC_GRIP_ROOT) && player->isAlive())
                                player->CombatStop();
                    break;
                }
                case ACTION_ILLIDAN_REAPPLY:
                    if (!me->HasAura(SPELL_ILLIDAN_SHADOWCLOAK))
                    {
                        DoCast(me, SPELL_ILLIDAN_SHADOWCLOAK, true);
                        Position pos(*me);
                        me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, pos, TEMPSUMMON_MANUAL_DESPAWN);
                    }
                    break;
                default:
                    break;
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (me->HealthBelowPctDamaged(10, damage))
                me->SetHealth(me->GetMaxHealth());
        }

        void JustSummoned(Creature* summon) override
        {
            if (summon->GetEntry() == NPC_ILLIDAN_SHADOWCLOAK_STALKER)
            {
                summon->CastSpell(summon, SPELL_ILLIDAN_STALKER_SHADOWCLOAK_COSMETIC, true);
                summon->CastSpell(me, SPELL_RIDE_VEHICLE_HARDCODED, true);
                summon->AddUnitTypeMask(UNIT_MASK_ACCESSORY);
            }
            npc_escortAI::JustSummoned(summon);
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!EscortStarted && who->GetTypeId() == TYPEID_PLAYER)
            {
                if (!instance->IsDone(DATA_PEROTHARN))
                {
                    if (me->GetDistance2d(who) <= 50.00f)
                    {
                        me->SetVisible(true);
                        me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, *me, TEMPSUMMON_MANUAL_DESPAWN);
                        if (auto c = me->SummonCreature(NPC_ILLIDAN_SHADOWCLOAK_STALKER, *me, TEMPSUMMON_MANUAL_DESPAWN))
                            c->DespawnOrUnsummon(4500);
                        EscortStarted = true;
                        Start(false, false, who->GetGUID());
                        SetMaxPlayerDistance(500.0f);
                        SetDespawnAtEnd(false);
                        SetDespawnAtFar(false);
                    }
                }
            }
            npc_escortAI::MoveInLineOfSight(who);
        }

        void WaypointReached(uint32 waypointId) override
        {
            /*std::stringstream ss;
            ss << "Waypoint: " << waypointId << " reached!";
            std::string post = ss.str();
            sWorld->SendServerMessage(SERVER_MSG_STRING, post.c_str());*/
            
            currentWaypoint = waypointId;
            switch (waypointId)
            {
                case 1:
                    Talk(TALK_ILLIDAN_MOVE_IN_LINE_OF_SIGHT, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 5:
                    Talk(TALK_ILLIDAN_WAIT_FOR_EVENT_START, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_SHADOWCLOAK);
                    SetEscortPaused(true);
                    break;
                case 9:
                    Talk(TALK_ILLIDAN_MANY_DEMONS, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 10:
                    Talk(TALK_ILLIDAN_SINGLE_GUARDIAN_TALK, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 15:
                    me->CastWithDelay(5000, me, SPELL_ILLIDAN_WALL_OF_SHADOWS);
                    Talk(TALK_ILLIDAN_HOLD_THEM_BACK, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    TalkWithDelay(18000, TALK_ILLIDAN_MAGIC_IS_FADING, NULL, CHAT_MSG_MONSTER_SAY);
                    events.ScheduleEvent(EVENT_LOCK_TRAFFIC_1, 2000);
                    break;
                case 16:
                    instance->SetData(DATA_LEGION_MOVEMENT, 0);
                    break;
                case 26:
                {
                    if (currentPortalAction == 10)
                    {
                        Talk(TALK_ILLIDAN_ATTACK_CRYSTALL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                        SetEscortPaused(true);
                        Position newHomePos(*me);
                        me->SetHomePosition(newHomePos);
                        events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 100);
                    }
                    break;
                }
                case 32:
                    Talk(TALK_ILLIDAN_THEY_COME_ENDLESSLY, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 46:
                {
                    Talk(TALK_ILLIDAN_ATTACK_CRYSTALL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    SetEscortPaused(true);
                    Position newHomePos(*me);
                    me->SetHomePosition(newHomePos);
                    events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 100);
                    break;
                }
                case 51:
                    Talk(TALK_ILLIDAN_THE_STENCH_OF_SULFUR, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    instance->SetData(DATA_LEGION_MOVEMENT, 3);
                    events.ScheduleEvent(EVENT_LOCK_TRAFFIC_2, 15000);
                    break;
                case 72:
                {
                    Talk(TALK_ILLIDAN_CUT_THIS_ONE_DOWN, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    if (auto demon = me->FindNearestCreature(NPC_LEGION_DEMON, 20.00f))
                        demon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    SetEscortPaused(true);
                    Position newHomePos(*me);
                    me->SetHomePosition(newHomePos);
                    events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 100);
                    break;
                }
                case 74:
                    DoAction(ACTION_ILLIDAN_REAPPLY);
                    break;
                case 81:
                {
                    Talk(TALK_ILLIDAN_ATTACK_CRYSTALL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    SetEscortPaused(true);
                    Position newHomePos(*me);
                    me->SetHomePosition(newHomePos);
                    events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 100);
                    break;
                }
                case 89:
                    Talk(TALK_ILLIDAN_DEMONS_LEAVING, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    //events.ScheduleEvent(EVENT_START_BOSS_FIGHT, 1);
                    me->DespawnCreaturesInArea(NPC_GUARDIAN_DEMON, 300.00f);
                    break;
                case 99:
                    Talk(TALK_ILLIDAN_DEMONS_NOT_ON_PLACE, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 103:
                    Talk(TALK_ILLIDAN_TOO_EASY, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 104: // Appear boss
                    if (auto boss = instance->GetCreature(DATA_PEROTHARN))
                        boss->AI()->DoAction(ACTION_APPEAR_MID);
                    break;
                case 109:
                {
                    Position newHomePos(*me);
                    me->SetHomePosition(newHomePos);
                    Talk(TALK_ILLIDAN_ANOTHER_DEMON, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    TalkWithDelay(8000, TALK_ILLIDAN_PATIENCE, NULL, CHAT_MSG_MONSTER_SAY);
                    SetEscortPaused(true);
                    events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 10000);
                    break;
                }
                case 110:
                    Talk(TALK_ILLIDAN_GOOD_BYE, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                    break;
                case 119:
                {
                    Position jump = { 3400.101807f, -5131.78076f, 181.078064f, 4.590448f };
                    me->GetMotionMaster()->MoveJump(jump, 12.f, 18.f);
                    me->DespawnOrUnsummon(10000);
                    break;
                }
                case 123:
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            npc_escortAI::UpdateAI(diff);
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_RESTART_ESCORT:
                        SetEscortPaused(false);
                        break;
                    case EVENT_LOCK_TRAFFIC_1:
                        instance->SetData(DATA_LEGION_MOVEMENT, 1);
                        events.ScheduleEvent(EVENT_UNLOCK_TRAFFIC, 18000);
                        break;
                    case EVENT_LOCK_TRAFFIC_2:
                        instance->SetData(DATA_LEGION_MOVEMENT, 2);
                        break;
                    case EVENT_UNLOCK_TRAFFIC:
                        break;
                    case EVENT_SHADOWCLOAK_CHECK:
                    {
                        Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (!player->HasAura(SPELL_PLAYER_SHADOWCLOAK) && !player->HasAura(SPELL_DEMONIC_GRIP_ROOT) && player->isAlive())
                                    if (player->GetAreaId() == 5993)
                                        player->CastSpell(player, SPELL_PLAYER_SHADOWCLOAK, true);
                        events.ScheduleEvent(EVENT_SHADOWCLOAK_CHECK, 1000);
                        break;
                    }
                    case EVENT_MOVE_TO_NEXT_PORTAL:
                        Talk(TALK_ILLIDAN_LEAVING, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_ZONE);
                        SetEscortPaused(false);
                        break;
                    case EVENT_START_BOSS_FIGHT:
                        if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                            perotharn->AI()->DoAction(ACTION_START_COMBAT);
                        break;
                    case EVENT_PLAYER_IN_COMBAT_CHECK:
                    {
                        bool needsNewCheck = true;
                        Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (player->GetDistance2d(me) <= 40.00f && player->isInCombat())
                                {
                                    SetCanAttack(true);
                                    me->setFaction(ILLIDAN_FACTION_ATTACKER);
                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                                    me->RemoveAurasDueToSpell(SPELL_ILLIDAN_SHADOWCLOAK);
                                    needsNewCheck = false;
                                    events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
                                    break;
                                }

                        if (needsNewCheck)
                            events.ScheduleEvent(EVENT_PLAYER_IN_COMBAT_CHECK, 1000);
                        break;
                    }
                    case EVENT_PLAYER_ALIVE_CHECK:
                    {
                        bool alivePlayerFound = false;
                        Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (!player->isGameMaster() && player->isAlive())
                                {
                                    alivePlayerFound = true;
                                    break;
                                }

                        if (!alivePlayerFound)
                            ResetCombat(currentWaypoint < 27);
                        else
                            events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }
            if (!me->HasAura(SPELL_REGENERATION) && !UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
        bool EscortStarted;
        bool isInitialStart;
        uint8 demonKillCounter;
        uint8 currentPortalAction;
        uint8 currentWaypoint;
        Position spawnPosition;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_woe_illidan_part_oneAI(creature);
    }
};

class npc_woe_nozdormu_part_one : public CreatureScript
{
public:
    npc_woe_nozdormu_part_one() : CreatureScript("npc_woe_nozdormu_part_one") {}

    struct npc_woe_nozdormu_part_oneAI : public ScriptedAI
    {
        npc_woe_nozdormu_part_oneAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            instance = me->GetInstanceScript();
            introStarted = false;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!introStarted)
            {
                if (!instance->IsDone(DATA_PEROTHARN))
                {
                    if (who->GetTypeId() == TYPEID_PLAYER)
                    {
                        introStarted = true;
                        me->SummonCreatureGroup(SUMMON_GROUP_EVENT_INTRO);
                        Talk(TALK_NOZDORMU_INTRO, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                        TalkWithDelay(10000, TALK_NOZDORMU_INTRO_TWO, NULL, CHAT_MSG_MONSTER_SAY);
                        TalkWithDelay(16000, TALK_NOZDORMU_INTRO_THREE, NULL, CHAT_MSG_MONSTER_SAY);
                        events.ScheduleEvent(EVENT_START_PEROTHARN_INTRO, 28000);
                    }
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            if (summon->GetEntry() == NPC_LEGION_DEMON)
                summon->AI()->DoAction(ACTION_SET_FIRST_FIREWALL_DEMON);
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_START_PEROTHARN_INTRO:
                        if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                        {
                            // perotharn->AI()->Talk(TALK_PEROTHARN_INTRO);
                            // perotharn->AI()->TalkWithDelay(6000, TALK_PEROTHARN_INTRO_TWO);
                            // perotharn->AI()->TalkWithDelay(9000, TALK_PEROTHARN_INTRO_THREE);
                            perotharn->AI()->Talk(TALK_PEROTHARN_INTRO, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                            perotharn->AI()->TalkWithDelay(6000, TALK_PEROTHARN_INTRO_TWO, NULL, CHAT_MSG_MONSTER_SAY);
                            perotharn->AI()->TalkWithDelay(9000, TALK_PEROTHARN_INTRO_THREE, NULL, CHAT_MSG_MONSTER_SAY);
                        }
                        for (uint64 summonGuid : summons)
                            if (auto summon = ObjectAccessor::GetCreature(*me, summonGuid))
                                if (summon->GetEntry() == NPC_LEGION_DEMON)
                                    summon->EmoteWithDelay(6000, EMOTE_ONESHOT_SALUTE_NO_SHEATH);
                        events.ScheduleEvent(EVENT_START_DEMON_MOVEMENT, 12000);
                        break;
                    case EVENT_START_DEMON_MOVEMENT:
                        for (uint64 summonGuid : summons)
                        {
                            if (auto summon = ObjectAccessor::GetCreature(*me, summonGuid))
                            {
                                switch (summon->GetEntry())
                                {
                                    case NPC_LEGION_DEMON:
                                        summon->GetMotionMaster()->MovePoint(POINT_SUMMON_FIREWALL, demonEndPos);
                                        break;
                                    default:
                                        summon->GetMotionMaster()->MovePoint(POINT_EVENT_WALK, eventWaypoints[0]);
                                        break;
                                }
                            }
                        }

                        if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                            perotharn->GetMotionMaster()->MovePoint(POINT_EVENT_WALK, eventWaypoints[1]);
                        break;
                    case EVENT_TALK_PEROTHARN_INTRO_TWO:
                        //if (auto perotharn = instance->GetCreature(DATA_PEROTHARN)) 
                        if (auto c = me->FindNearestCreature(NPC_PEROTHARN, 500.f))
                        {
                            c->AI()->Talk(TALK_PEROTHARN_INTRO_TWO, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                        }

                        break;
                    case EVENT_TALK_PEROTHARN_INTRO_THREE:
                        //if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                        if (auto c = me->FindNearestCreature(NPC_PEROTHARN, 500.f))
                        {
                            c->AI()->Talk(TALK_PEROTHARN_INTRO_THREE, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    private:
        EventMap events;
        InstanceScript* instance;
        SummonList summons;
        bool introStarted{ false };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_nozdormu_part_oneAI(creature);
    }
};

class npc_woe_legion_demon_firewall : public CreatureScript
{
public:
    npc_woe_legion_demon_firewall() : CreatureScript("npc_woe_legion_demon_firewall") {}

    struct npc_woe_legion_demon_firewallAI : public ScriptedAI
    {
        npc_woe_legion_demon_firewallAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
            isSecFirewallDemon = true;
        }

        void InitializeAI() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            ScriptedAI::InitializeAI();
        }

        void Reset()
        {
            events.Reset();
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_SET_FIRST_FIREWALL_DEMON)
            {
                events.SetPhase(EVENT_PHASE_INTRO);
                isSecFirewallDemon = false;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type != POINT_MOTION_TYPE || id != POINT_SUMMON_FIREWALL)
                return;

            Position pos(*me);
            me->SetHomePosition(pos);

            me->SetFacingTo(5.150090f);
            me->AddAura(SPELL_SUMMON_FIREWALL_PERIODIC, me);
            DoCast(me, SPELL_SUMMON_FIREWALL);
            events.ScheduleEvent(EVENT_REMOVE_FLAGS, 2000);
        }

        void EnterCombat(Unit* attacker) override
        {
            events.RemovePhase(EVENT_PHASE_INTRO);
            events.ScheduleEvent(EVENT_STRIKE_FEAR, 5000);
        }

        void JustDied(Unit* killer) override
        {
            if (!isSecFirewallDemon)
            {
                if (auto go = instance->GetGameObject(DATA_COURTYYARD_DOOR_ONE))
                    go->SetGoState(GO_STATE_ACTIVE);

                if (auto go = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_FIREWALL_ONE)))
                    go->SetGoState(GO_STATE_ACTIVE);
            }
            else
            {
                if (auto go = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_FIREWALL_TWO)))
                    go->SetGoState(GO_STATE_ACTIVE);
                if (auto go = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_FIREWALL_THREE)))
                    go->SetGoState(GO_STATE_ACTIVE);
                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                    illidan->AI()->DoAction(ACTION_DEMON_KILLED);
            }

        }

        void UpdateAI(uint32 const diff) override
        {
            if (!events.IsInPhase(EVENT_PHASE_INTRO) && !UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_REMOVE_FLAGS:
                        if (auto go = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_FIREWALL_ONE)))
                            go->SetGoState(GO_STATE_READY);

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        break;
                    case EVENT_STRIKE_FEAR:
                        DoCastAOE(SPELL_STRIKE_FEAR);
                        events.ScheduleEvent(EVENT_STRIKE_FEAR, 7000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        InstanceScript* instance;
        bool isSecFirewallDemon;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_legion_demon_firewallAI(creature);
    }
};

class npc_woe_dreadlord_defender : public CreatureScript
{
public:
    npc_woe_dreadlord_defender() : CreatureScript("npc_woe_dreadlord_defender") {}

    struct npc_woe_dreadlord_defenderAI : public ScriptedAI
    {
        npc_woe_dreadlord_defenderAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                    case POINT_EVENT_WALK:
                        events.SetPhase(EVENT_PHASE_INTRO);
                        events.ScheduleEvent(EVENT_START_NEXT_WAYPOINT, 100);
                        break;
                    case POINT_EVENT_WALK_TWO:
                        events.ScheduleEvent(EVENT_LAST_WAYPOINT, 100);
                        break;
                    default:
                        break;
                }
            }
        }

        void EnterCombat(Unit* attacker) override
        {
            events.RemovePhase(EVENT_PHASE_INTRO);
            events.ScheduleEvent(EVENT_DEMONIC_WARDING, urand(1000,10000));
            events.ScheduleEvent(EVENT_CARRION_SWARM, 5000);
        }

        void JustDied(Unit* killer) override
        {
            if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                illidan->AI()->DoAction(ACTION_DEMON_KILLED);
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                    case EVENT_DEMONIC_WARDING:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventid, 250);
                        else
                        {
                            if (auto arcanist = me->FindNearestCreature(NPC_CORRUPTED_ARCANIST, 40.00f))
                                if (!arcanist->HasAura(SPELL_DEMONIC_WARDING))
                                    DoCast(arcanist, SPELL_DEMONIC_WARDING);
                            events.ScheduleEvent(EVENT_DEMONIC_WARDING, urand(8000, 16000));
                        }
                        break;
                    case EVENT_CARRION_SWARM:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventid, 250);
                        else
                        {
                            DoCastVictim(SPELL_CARRION_SWARM);
                            events.ScheduleEvent(EVENT_CARRION_SWARM, 5500);
                        }
                        break;
                    case EVENT_START_NEXT_WAYPOINT:
                            me->GetMotionMaster()->MovePoint(POINT_EVENT_WALK_TWO, eventWaypoints[1]);
                        break;
                    case EVENT_LAST_WAYPOINT:
                            me->GetMotionMaster()->MovePoint(POINT_EVENT_WALK_TWO, eventWaypoints[2]);
                        break;
                    default:
                        break;
                }

            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }
        private:
            EventMap events;
            InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_dreadlord_defenderAI(creature);
    }
};

class npc_woe_corrupted_arcanist : public CreatureScript
{
public:
    npc_woe_corrupted_arcanist() : CreatureScript("npc_woe_corrupted_arcanist") {}

    struct npc_woe_corrupted_arcanistAI : public ScriptedAI
    {
        npc_woe_corrupted_arcanistAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            if (!me->HasAura(SPELL_CORRUPTED_NIGHT_ELF_EYES))
                DoCast(me, SPELL_CORRUPTED_NIGHT_ELF_EYES, true);
            if (!me->HasAura(SPELL_DISCIPLINED_CASTER))
                DoCast(me, SPELL_DISCIPLINED_CASTER, true);
            events.Reset();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                    case POINT_EVENT_WALK:
                        events.SetPhase(EVENT_PHASE_INTRO);
                        events.ScheduleEvent(EVENT_START_NEXT_WAYPOINT, 100);
                        break;
                    case POINT_EVENT_WALK_TWO:
                        events.ScheduleEvent(EVENT_LAST_WAYPOINT, 100);
                        break;
                    default:
                        break;
                }
            }
        }

        void EnterCombat(Unit* attacker) override
        {
            events.RemovePhase(EVENT_PHASE_INTRO);
            events.ScheduleEvent(EVENT_ARCANE_ANNIHILATION, 1000);
            events.ScheduleEvent(EVENT_CHECK_MANA_POOL, 1000);
        }

        void JustDied(Unit* killer) override
        {
            if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                illidan->AI()->DoAction(ACTION_DEMON_KILLED);
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                    case EVENT_START_NEXT_WAYPOINT:
                        me->GetMotionMaster()->MovePoint(POINT_EVENT_WALK_TWO, eventWaypoints[1]);
                        break;
                    case EVENT_LAST_WAYPOINT:
                        me->GetMotionMaster()->MovePoint(POINT_EVENT_WALK_TWO, eventWaypoints[2]);
                        break;
                    case EVENT_CHECK_MANA_POOL:
                        if (me->GetPowerPct(POWER_MANA) <= 11)
                        {
                            DoCast(me, SPELL_INFINITE_MANA);
                            events.DelayEvents(6000);
                            events.ScheduleEvent(EVENT_CHECK_MANA_POOL, 6000);
                        }
                        events.ScheduleEvent(EVENT_CHECK_MANA_POOL, 1000);
                        break;
                    case EVENT_ARCANE_ANNIHILATION:
                        DoCastVictim(SPELL_ARCANE_ANNIHILATION);
                        events.ScheduleEvent(EVENT_ARCANE_ANNIHILATION, 3100);
                        break;
                default:
                    break;
                }

            if (UpdateVictim())
                DoMeleeAttackIfReady();
        }
    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_corrupted_arcanistAI(creature);
    }
};

class npc_woe_guardian_demon : public CreatureScript
{
public:
    npc_woe_guardian_demon() : CreatureScript("npc_woe_guardian_demon") {}

    struct npc_woe_guardian_demonAI : public ScriptedAI
    {
        npc_woe_guardian_demonAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_ROOT);
        }

        void CastOrWait(uint32 eventId, uint32 spell, uint32 min_repeat, uint32 max_repeat, bool wait_castnext)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING) && me->isInCombat() && UpdateVictim())
            {
                //TC_LOG_ERROR("sql.sql", "casting demonic grip.");
                DoCast(me->getVictim(), SPELL_DEMONIC_GRIP_ROOT, true);
                DoCast(spell);
                events.ScheduleEvent(eventId, urand(min_repeat, max_repeat));
            }
            else
                events.ScheduleEvent(eventId, (wait_castnext ? 250 : urand(min_repeat, max_repeat)));
        }

        void KilledUnit(Unit* victim) override
        {
            Talk(2);
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
                case SPELL_DEMONIC_GRIP:
                    spell = SPELL_DEMONIC_GRIP;         //Spell ID
                    min_repeat = 3000;                 //Minimum Delay
                    max_repeat = 3000;                //Maximum Delay
                    wait_castnext = true;                 //Should this spell be queued next (true) or rescheduled (false) if a spell is already in progress?
                    CastOrWait(eventId, spell, min_repeat, max_repeat, wait_castnext);
                    break;
                case 200:
                {
                    Position portal = me->GetHomePosition();
                    portal.m_positionZ += 3.f;
                    me->ClearUnitState(UNIT_STATE_ROOT);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->SetHover(true);
                    me->GetMotionMaster()->MovePoint(200, portal, true, 7.f);
                    Talk(0, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                    TalkWithDelay(5000, 1, NULL, CHAT_MSG_MONSTER_YELL);
                    break;
                }
                default:
                    break;
                }
        }
        void EnterEvadeMode()
        {
            if (me->HasUnitState(UNIT_STATE_CASTING) || me->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) != 0)
                return;

            ScriptedAI::EnterEvadeMode();
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_ROOT);
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
            case 200:
                events.ScheduleEvent(200, 1500);
                break;
            }
        }

        void EnterCombat(Unit* attacker) override
        {
            me->SetReactState(REACT_AGGRESSIVE);
            attacker->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK);
            attacker->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK_VISUAL);
            attacker->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH);
            events.ScheduleEvent(SPELL_DEMONIC_GRIP, 1);
        }
    private:
        EventMap events;

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_guardian_demonAI(creature);
    }
};

class go_woe_portal_energy_focus : public GameObjectScript
{
public:
    go_woe_portal_energy_focus() : GameObjectScript("go_woe_portal_energy_focus") 
    { 
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (!go)
            return false;
        instance = go->GetInstanceScript();
        if (!go->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE))
        {
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            if (auto stalker = go->FindNearestCreature(NPC_FEL_CRYSTAL_STALKER, 5.0f))
            {
                stalker->RemoveAurasDueToSpell(SPELL_FEL_CRYSTAL_SWITCH_GLOW);
                stalker->CastSpell(stalker, SPELL_FEL_CRYSTAL_MELTDOWN, false);
            }

            if (auto instance = go->GetInstanceScript())
                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_ONE))
                    illidan->AI()->DoAction(ACTION_MOVE_NEXT_PORTAL);


            if (auto portal = go->FindNearestCreature(56087, 65.0f))
            {
                portal->SetObjectScale(0.03f);
                portal->DespawnOrUnsummon(15000);
                if (auto guardian = portal->FindNearestCreature(NPC_GUARDIAN_DEMON, 100.0f))
                {
                    guardian->SetHomePosition(*portal);
                    guardian->AI()->DoAction(200);
                    guardian->DespawnOrUnsummon(15000);
                }
            }
            if (instance)
                instance->SetData(DATA_LEGION_PORTAL_STATE, instance->GetData(DATA_LEGION_PORTAL_STATE) + 1);
        }

        return true;
    }
private:
    InstanceScript* instance{ nullptr };
};

class go_woe_time_transit_device : public GameObjectScript
{
public:
    go_woe_time_transit_device() : GameObjectScript("go_woe_time_transit_device") {}

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->isInCombat())
            return true;
        if (InstanceScript *instance = go->GetInstanceScript())
        {
            if (go->GetEntry() != 209998)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to the garden entrance", GOSSIP_SENDER_MAIN, SPELL_TELEPORT_COURTYARD_ENTRANCE);
            if (go->GetEntry() != 209997 && instance->IsDone(DATA_PEROTHARN))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Azshara's Palace.", GOSSIP_SENDER_MAIN, SPELL_TELEPORT_AZSHARAS_PALACE);
            if (instance->IsDone(DATA_PEROTHARN) && instance->IsDone(DATA_QUEEN_AZSHARA))
            {
                if (go->GetEntry() != 210000)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Azshara's Palace Overlook.", GOSSIP_SENDER_MAIN, SPELL_TELEPORT_AZSHARAS_PALACE_OVERLOOK);
                if (go->GetEntry() != 209999)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to The Well of Eternity.", GOSSIP_SENDER_MAIN, SPELL_TELEPORT_WELL_OF_ETERNITY);
            }
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action)
    {
        InstanceScript* instance    { go->GetInstanceScript() };
        player->PlayerTalkClass->ClearMenus();
        if (player->isInCombat())
            return true;
        player->CastSpell(player, action, true);
        return true;
    }
private:
    uint64 players_teleported[5]{ 0, 0, 0, 0, 0 };
};

class spell_woe_fel_decay_heal : public SpellScriptLoader
{
public:
    spell_woe_fel_decay_heal() : SpellScriptLoader("spell_woe_fel_decay_heal") { }

    class spell_woe_fel_decay_heal_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_fel_decay_heal_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetHealInfo() && eventInfo.GetHealInfo()->GetSpellInfo())
                return true;
            return false;
        }

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            if (auto instance = GetTarget()->GetInstanceScript())
            {
                if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                {
                    int32 damage = eventInfo.GetHealInfo()->GetHeal();
                    perotharn->CastCustomSpell(eventInfo.GetActor(), SPELL_FEL_SURGE, &damage, NULL, NULL, true);
                }
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_woe_fel_decay_heal_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_woe_fel_decay_heal_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_fel_decay_heal_AuraScript();
    }
};

class spell_woe_drain_essence : public SpellScriptLoader
{
public:
    spell_woe_drain_essence() : SpellScriptLoader("spell_woe_drain_essence") { }

private:
    class spell_woe_drain_essence_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_drain_essence_SpellScript)

        void TriggerSpell(SpellEffIndex effIndex)
        {
        }

        void Register()
        {
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_woe_drain_essence_SpellScript();
    }
    class spell_woe_drain_essence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_drain_essence_AuraScript);

        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
                if (auto t = GetOwner())
                    if (auto target = t->ToUnit())
                    {
                        c->CastWithDelay(1100, target, 41421, true);
                        target->KnockbackFrom(c->GetPositionX(), c->GetPositionY(), 10.f, 15.f);
                    }
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
                if (auto t = GetOwner())
                    if (auto target = t->ToUnit())
                    {
                        target->RemoveAurasDueToSpell(41421);
                    }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_woe_drain_essence_AuraScript::HandleOnEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_woe_drain_essence_AuraScript::HandleOnEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_woe_drain_essence_AuraScript();
    }

};

class spell_woe_summon_circle_spawn : public SpellScriptLoader
{
public:
    spell_woe_summon_circle_spawn() : SpellScriptLoader("spell_woe_summon_circle_spawn") { }

    class spell_woe_summon_circle_spawn_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_summon_circle_spawn_AuraScript);

        void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32& amplitude)
        {
            isPeriodic = true;
            amplitude = 4000;
        }

        void OnPeriodicTick(AuraEffect const* /*aurEff*/)
        {
            if (auto instance = GetTarget()->GetInstanceScript())
                if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                {
                    Position summonPos(*GetTarget());
                    GetTarget()->PlayOneShotAnimKit(ANIM_ONE_SHOT_CIRCLE);
                    if (auto eye = perotharn->SummonCreature(NPC_EYE_OF_THE_PEROTHARN, summonPos))
                    {

                    }

                }
        }

        void Register()
        {
            DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_woe_summon_circle_spawn_AuraScript::CalcPeriodic, EFFECT_0, SPELL_AURA_DUMMY);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_woe_summon_circle_spawn_AuraScript::OnPeriodicTick, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_summon_circle_spawn_AuraScript();
    }
};

class spell_woe_punishing_flames : public SpellScriptLoader
{
public:
    spell_woe_punishing_flames() : SpellScriptLoader("spell_woe_punishing_flames") { }

    class spell_woe_punishing_flames_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_punishing_flames_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            Unit* caster = GetCaster();
            targets.remove_if([caster](WorldObject* target)
            {
                return caster->GetDistance2d(target) < 70.00f;
            });
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_punishing_flames_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_punishing_flames_SpellScript();
    }
};

class spell_woe_fel_fireball_aggro : public SpellScriptLoader
{
public:
    spell_woe_fel_fireball_aggro() : SpellScriptLoader("spell_woe_fel_fireball_aggro") { }

    class spell_woe_fel_fireball_aggro_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_fel_fireball_aggro_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (auto caster = GetCaster())
                if (auto player = caster->ToPlayer())
                    if (auto eye = GetHitUnit()->ToCreature())
                    if (auto instance = eye->GetInstanceScript())
                        if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                        {
                            perotharn->AI()->SetGUID(GetHitUnit()->GetGUID(), ACTION_PLAYER_FOUND);
                            GetCaster()->CastSpell(GetCaster(), SPELL_EASY_PREY, true, NULL, NULL, perotharn->GetGUID());
                            GetCaster()->CastSpell(GetCaster(), SPELL_EASY_PREY_SUMMON, true);


                            perotharn->AI()->SummonedCreatureDies(eye, player);
                            perotharn->AI()->DoAction(ACTION_PLAYER_FOUND);

                        } 
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_woe_fel_fireball_aggro_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript *GetSpellScript() const override
    {
        return new spell_woe_fel_fireball_aggro_SpellScript();
    }
};

class spell_woe_tracked_lock_on_player : public SpellScriptLoader
{
public:
    spell_woe_tracked_lock_on_player() : SpellScriptLoader("spell_woe_tracked_lock_on_player") { }

    class spell_woe_tracked_lock_on_player_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_tracked_lock_on_player_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_EASY_PREY));
            // should never happen
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_tracked_lock_on_player_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_tracked_lock_on_player_SpellScript();
    }
};

class spell_woe_drain_essence_damage : public SpellScriptLoader
{
public:
    spell_woe_drain_essence_damage() : SpellScriptLoader("spell_woe_drain_essence_damage") { }

    class spell_woe_drain_essence_damage_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_drain_essence_damage_SpellScript);

        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
            {
                SetHitDamage(target->CountPctFromMaxHealth(5));
            }


        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_woe_drain_essence_damage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_drain_essence_damage_SpellScript();
    }
};

class spell_woe_drain_essence_knockback : public SpellScriptLoader
{
public:
    spell_woe_drain_essence_knockback() : SpellScriptLoader("spell_woe_drain_essence_knockback") { }

    class spell_woe_drain_essence_knockback_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_drain_essence_knockback_SpellScript);

        void ApplyStun(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
                target->CastSpell(target, SPELL_ASPHYXIATE, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_woe_drain_essence_knockback_SpellScript::ApplyStun, EFFECT_0, SPELL_EFFECT_KNOCK_BACK);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_drain_essence_knockback_SpellScript();
    }
};

class spell_woe_shadowcloak_periodic : public SpellScriptLoader
{
public:
    spell_woe_shadowcloak_periodic() : SpellScriptLoader("spell_woe_shadowcloak_periodic") { }

    class spell_woe_shadowcloak_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_shadowcloak_periodic_AuraScript);

        void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            if (auto target = GetTarget())
                if (auto aura = target->GetAura(SPELL_PLAYER_SHADOW_WALK_VISUAL))
                {
                    if (auto stalkerBuff = target->GetAura(SPELL_PLAYER_SHADOW_AMBUSHER))
                    {
                        if (uint32(stalkerBuff->GetDuration()) < aura->GetStackAmount() * 1000)
                        {
                            stalkerBuff->SetMaxDuration(int32(aura->GetStackAmount() * 1000));
                            stalkerBuff->SetDuration(int32(aura->GetStackAmount() * 1000));
                        }
                    }
                    else if (auto stalkerBuff = target->AddAura(SPELL_PLAYER_SHADOW_AMBUSHER, target))
                    {
                        stalkerBuff->SetMaxDuration(int32(aura->GetStackAmount() * 1000));
                        stalkerBuff->SetDuration(int32(aura->GetStackAmount() * 1000));
                    }
                    target->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK);
                    target->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                    target->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH);
                }
        }

        void onPeriodicTick(AuraEffect const* aurEff)
        {
            if (!GetTarget()->isInCombat())
            {
                if (!GetTarget()->HasAura(SPELL_PLAYER_SHADOW_WALK))
                    GetTarget()->CastSpell(GetTarget(), SPELL_PLAYER_SHADOW_WALK, true);

                if (!GetTarget()->HasAura(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH))
                    GetTarget()->CastSpell(GetTarget(), SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH, true);

                GetTarget()->CastSpell(GetTarget(), SPELL_PLAYER_SHADOW_WALK_VISUAL, true);
            }
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_woe_shadowcloak_periodic_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_woe_shadowcloak_periodic_AuraScript::onPeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_shadowcloak_periodic_AuraScript();
    }
};

class spell_woe_shadow_walk : public SpellScriptLoader
{
public:
    spell_woe_shadow_walk() : SpellScriptLoader("spell_woe_shadow_walk") { }

    class spell_woe_shadow_walk_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_shadow_walk_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto u = GetTarget())
                if (u->HasAuraType(SPELL_AURA_MOUNTED))
                    u->RemoveAurasByType(SPELL_AURA_MOUNTED);
        }
        
        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Position pos(*GetTarget());
            if (auto creature = GetTarget()->SummonCreature(NPC_PLAYER_SHADOWCLOAK_STALKER, pos, TEMPSUMMON_MANUAL_DESPAWN))
            {
                creature->CastSpell(creature, SPELL_PLAYER_SHADOWCLOAK_COSMETIC, true);
                creature->CastSpell(GetTarget(), SPELL_RIDE_VEHICLE_HARDCODED, true);
                creature->AddUnitTypeMask(UNIT_MASK_ACCESSORY);
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_woe_shadow_walk_AuraScript::OnApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
            AfterEffectApply += AuraEffectApplyFn(spell_woe_shadow_walk_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_shadow_walk_AuraScript();
    }
};

class spell_woe_firewall_periodic : public SpellScriptLoader
{
public:
    spell_woe_firewall_periodic() : SpellScriptLoader("spell_woe_firewall_periodic") { }

    class spell_woe_firewall_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_firewall_periodic_AuraScript);

        void onPeriodicTick(AuraEffect const* aurEff)
        {
            uint8 position = aurEff->GetTickNumber();
            if (auto target = GetTarget()->SummonCreature(56096, firewallPeriodicPositions[position]))
                target->CastSpell(target, SPELL_SUMMON_FIREWALL_PULSE, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_woe_firewall_periodic_AuraScript::onPeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_firewall_periodic_AuraScript();
    }
};

class spell_woe_wall_of_shadow_periodic : public SpellScriptLoader
{
public:
    spell_woe_wall_of_shadow_periodic() : SpellScriptLoader("spell_woe_wall_of_shadow_periodic") { }

    class spell_woe_wall_of_shadow_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_wall_of_shadow_periodic_AuraScript);

        void onPeriodicTick(AuraEffect const* aurEff)
        {
            if (aurEff->GetTickNumber() > 3)
                return;

            if (auto visual = GetTarget()->SummonCreature(NPC_DISTRACT_STALKER, DemonStalkerPos[aurEff->GetTickNumber()], TEMPSUMMON_TIMED_DESPAWN, 15000))
                visual->CastSpell(visual, SPELL_DISTRACT_DEMONS_STATIONARY, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_woe_wall_of_shadow_periodic_AuraScript::onPeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_wall_of_shadow_periodic_AuraScript();
    }
};

class DelayedCrystalDeathEvent : public BasicEvent
{
public:
    DelayedCrystalDeathEvent(Unit* trigger, uint64 targetGUID) : _trigger(trigger), _targetGUID(targetGUID)
    {
    }

    bool Execute(uint64 /*time*/, uint32 /*diff*/)
    {
        if (auto crystal = ObjectAccessor::GetCreature(*_trigger, _targetGUID))
            _trigger->Kill(crystal);
        return true;
    }

private:
    Unit* _trigger;
    uint64 _targetGUID;
};

class spell_woe_fel_crystal_meltdown : public SpellScriptLoader
{
public:
    spell_woe_fel_crystal_meltdown() : SpellScriptLoader("spell_woe_fel_crystal_meltdown") { }

    class spell_woe_fel_crystal_meltdown_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_fel_crystal_meltdown_AuraScript);

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            if (auto caster = GetCaster())
            {
                caster->CastSpell(GetUnitOwner(), SPELL_SHATTER_FEL_CRYSTAL, true);
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_FEL_CRYSTAL_DESTRUCTION, true);

                if (!caster->HasAura(SPELL_FEL_CRYSTAL_DESTROYED))
                    caster->CastSpell(caster, SPELL_FEL_CRYSTAL_DESTROYED, true);

                if (auto focus = caster->FindNearestGameObject(GO_PORTAL_ENERGY_FOCUS_ONE, 5.00f))
                    focus->RemoveFromWorld();
                else if (auto focus = caster->FindNearestGameObject(GO_PORTAL_ENERGY_FOCUS_TWO, 5.00f))
                    focus->RemoveFromWorld();
                else if (auto focus = caster->FindNearestGameObject(GO_PORTAL_ENERGY_FOCUS_THREE, 5.00f))
                    focus->RemoveFromWorld();

                if (GetTarget() != caster)
                    caster->m_Events.AddEvent(new DelayedCrystalDeathEvent(caster, GetTarget()->GetGUID()), caster->m_Events.CalculateTime(500));
            }
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_woe_fel_crystal_meltdown_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_fel_crystal_meltdown_AuraScript();
    }
};

class spell_woe_sheen_of_elune : public SpellScriptLoader
{
public:
    spell_woe_sheen_of_elune() : SpellScriptLoader("spell_woe_sheen_of_elune") { }

    class spell_woe_sheen_of_elune_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_sheen_of_elune_AuraScript);

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetDamageInfo() && eventInfo.GetDamageInfo()->GetDamage() > 0)
            {
                int32 damage = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), 60);
                eventInfo.GetActionTarget()->CastCustomSpell(eventInfo.GetActor(), 102258, &damage, NULL, NULL, TRIGGERED_FULL_MASK);
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_woe_sheen_of_elune_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_sheen_of_elune_AuraScript();
    }
};

class spell_woe_shadowcloak_aggro_check_stealth : public SpellScriptLoader
{
public:
    spell_woe_shadowcloak_aggro_check_stealth() : SpellScriptLoader("spell_woe_shadowcloak_aggro_check_stealth") { }

    class spell_woe_shadowcloak_aggro_check_stealth_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_shadowcloak_aggro_check_stealth_SpellScript);

        void CastDemonicPull(SpellEffIndex effIndex)
        {
            if (auto player = GetCaster())
                if (auto npc = GetHitUnit())
                    if (npc->IsHostileTo(player))
                        if (npc->GetDistance(player) <= 5.f)
                        {
                            player->CastSpell(player, SPELL_DEMONIC_GRIP_ROOT, true);
                            player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK);
                            player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_WALK_VISUAL);
                            player->RemoveAurasDueToSpell(SPELL_PLAYER_SHADOW_AMBUSHER_STEALTH);

                            if (auto guardian = GetCaster()->FindNearestCreature(NPC_GUARDIAN_DEMON, 100.00f))
                                if (guardian->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT) == 0)
                                {
                                    guardian->SetTarget(player->GetGUID());
                                    guardian->SetFacingTo(guardian->GetAngle(player));
                                    guardian->SetTarget(GetCaster()->GetGUID());
                                    guardian->SetReactState(REACT_AGGRESSIVE);
                                    guardian->AddThreat(GetCaster()->ToUnit(), 10000.f);
                                    guardian->CastWithDelay(1000, GetCaster(), 92111);
                                }
                        }
        }
        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if (auto h = GetExplTargetUnit())
                    if (c->GetDistance(h) > 5.f)
                        return SPELL_FAILED_DONT_REPORT;

            return SPELL_CAST_OK;
        }
        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_woe_shadowcloak_aggro_check_stealth_SpellScript::CastDemonicPull, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            OnCheckCast += SpellCheckCastFn(spell_woe_shadowcloak_aggro_check_stealth_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_shadowcloak_aggro_check_stealth_SpellScript();
    }
};

class spell_woe_demon_grip_pull : public SpellScriptLoader
{
public:
    spell_woe_demon_grip_pull() : SpellScriptLoader("spell_woe_demon_grip_pull") { }

    class spell_woe_demon_grip_pull_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_demon_grip_pull_SpellScript);

        void ModDestHeight(SpellEffIndex /*effIndex*/)
        {
            if (auto guardian = GetCaster()->FindNearestCreature(NPC_GUARDIAN_DEMON, 100.00f))
            {
                Position pos;
                guardian->GetNearPosition(pos, 5.0f, 0.0f);
                GetHitDest()->Relocate(pos);
            }
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_woe_demon_grip_pull_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_woe_demon_grip_pull_SpellScript();
    }
};

class spell_woe_demon_grip_periodic : public SpellScriptLoader
{
public:
    spell_woe_demon_grip_periodic() : SpellScriptLoader("spell_woe_demon_grip_periodic") { }

    class spell_woe_demon_grip_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_demon_grip_periodic_AuraScript);

        void onPeriodicTick(AuraEffect const* aurEff)
        {
            PreventDefaultAction();
            if (auto caster = GetCaster())
                if (auto o = GetOwner())
                if (auto target = o->ToUnit())
                    if (auto aura = aurEff->GetBase())
                    {
                        caster->CastSpell(target, aura->GetSpellInfo()->Effects[EFFECT_2].TriggerSpell, true);
                    }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_woe_demon_grip_periodic_AuraScript::onPeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_demon_grip_periodic_AuraScript();
    }
};

class achievement_woe_lazy_eye : public AchievementCriteriaScript
{
public:
    achievement_woe_lazy_eye() : AchievementCriteriaScript("achievement_woe_lazy_eye") { }

    bool OnCheck(Player* /*player*/, Unit* target)
    {
        return target && target->GetAI()->GetData(DATA_LAZY_EYE_ACHIEVEMENT) == 1;
    }
};

class spell_guardian_demon_stun : public SpellScriptLoader
{
    /*
        Spell difficulty Ids:
        Normal 10: 93423
        Normal 25: 93710
    */
public:
    spell_guardian_demon_stun() : SpellScriptLoader("spell_guardian_demon_stun") { };
    class spell_guardian_demon_stunAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_guardian_demon_stunAuraScript);

        bool Load()
        {
            return true;
        }

        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "float script activated by apply.");
            if (auto caster = GetCaster())
                if (auto target = GetTarget())
                {
                    Position dest(*caster);
                    caster->MoveBlink(dest, 5.f, 0.f);
                    float o = dest.GetOrientation();
                    float z = dest.GetPositionZ() + 21.0f;
                    float x = dest.GetPositionX();
                    float y = dest.GetPositionY();
                    target->GetMotionMaster()->ForceMoveJump(x, y, z, 4.f, 4.f);
                    target->AddUnitState(UNIT_STATE_STUNNED);
                    //target->AddUnitState(UNIT_STATE_NOT_MOVE);
                }
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            //TC_LOG_ERROR("sql.sql", "float script activated by remove");
            if (auto caster = GetCaster())
                if (auto target = GetTarget())
                    if (target->ToPlayer())
                    {
                        target->ClearUnitState(UNIT_STATE_STUNNED);
                        //target->ClearUnitState(UNIT_STATE_NOT_MOVE);
                        target->GetMotionMaster()->MoveFall();
                    }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_guardian_demon_stunAuraScript::HandleOnEffectApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_guardian_demon_stunAuraScript::HandleOnEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_guardian_demon_stunAuraScript();
    }
};

class npc_perotharn_searching_eye : public CreatureScript
{
public:
    npc_perotharn_searching_eye() : CreatureScript("npc_perotharn_searching_eye") { }


    struct npc_perotharn_searching_eyeAI : public ScriptedAI
    {

        npc_perotharn_searching_eyeAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
            if (!events.HasEvent(EVENT_MOVE_TO_COMBAT_AREA))
                events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 2000);
        }

        void InitializeAI()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 2000);
        }

        void JustDied(Unit* who)
        {
            me->DeleteThreatList();
            events.Reset();
            summons.DespawnAll();
        }


        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if (id == EVENT_MOVE_TO_COMBAT_AREA)
                    if (!events.HasEvent(EVENT_MOVE_TO_COMBAT_AREA))
                        events.ScheduleEvent(EVENT_MOVE_TO_COMBAT_AREA, 1);
        }
        void UpdateAI(const uint32 diff) override
        {
            if (auto p = me->FindNearestPlayer(5.f))
            {
                me->SetSpeed(MOVE_RUN, 1.5f);
                me->SetSpeed(MOVE_WALK, 1.5f);
                AttackStart(p);
            }
            else
            {
                me->SetSpeed(MOVE_RUN, walk_speed);
                me->SetSpeed(MOVE_WALK, run_speed);
            }

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_MOVE_TO_COMBAT_AREA:
                    if (me->isMoving()) events.ScheduleEvent(eventId, 250);
                    else
                    {
                        if (auto v = me->getVictim())
                        {
                        }
                        else
                        if (auto pero = me->FindNearestCreature(NPC_PEROTHARN, 200.f))
                        {
                            if (me->isInCombat())
                            {
                                me->DeleteThreatList();
                                me->CombatStop();
                            }
                            me->SetSpeed(MOVE_RUN, 4.f);
                            me->SetSpeed(MOVE_WALK, 4.f);
                            Position dest(*me);
                            me->MoveBlink(dest, frand(5.f, 15.f), frand(0 - (M_PI * 0.35f), (M_PI * 0.35f)));

                            if (pero->GetDistance(dest) > 65.f)
                            {
                                dest = (*pero);
                                pero->MoveBlink(dest, 20.f, frand(0, 2.f * M_PI));
                            }

                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(EVENT_MOVE_TO_COMBAT_AREA, dest, true);
                        }
                    }
                    break;
                default:
                    break;
                }
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        float walk_speed{ me->GetSpeed(MOVE_WALK) };
        float run_speed{ me->GetSpeed(MOVE_RUN) };
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_perotharn_searching_eyeAI(creature);
    }
};

enum beam
{
    EVENT_CAST_BEAM_ONE = 1,

    SPELL_CAST_BEAM = 104034

};
enum beam_npc
{
    BEAM_NPC_ONE = 55541,
    BEAM_NPC_TWO = 55542,
    BEAM_NPC_THREE = 55543
};

class npc_beam_connectors : public CreatureScript
{
public:
    npc_beam_connectors() : CreatureScript("npc_beam_connectors") { }
    // For Entry 55541

    struct npc_beam_connectorsAI : public ScriptedAI
    {

        npc_beam_connectorsAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void Reset()
        {
            events.Reset();
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
            case ACTION_DESPAWN_PORTAL_ONE:
                if (me->GetDBTableGUIDLow() == 255878
                    || me->GetDBTableGUIDLow() == 255866
                    || me->GetDBTableGUIDLow() == 255888
                    || me->GetDBTableGUIDLow() == 255867
                    )
                {
                    TC_LOG_ERROR("sql.sql", "connector %u db guid %u received despawn portal 1", me->GetEntry(), me->GetDBTableGUIDLow());
                    //me->RemoveAurasDueToSpell(104034);
                    me->CastStop();
                    me->CastSpell(me, SPELL_CONNECTOR_DEAD, true);
                    events.Reset();
                }
                break;
            case ACTION_DESPAWN_PORTAL_TWO:
                if (me->GetDBTableGUIDLow() == 255863
                    || me->GetDBTableGUIDLow() == 255876
                    || me->GetDBTableGUIDLow() == 255886
                    || me->GetDBTableGUIDLow() == 255864
                    )
                {
                    TC_LOG_ERROR("sql.sql", "connector %u db guid %u received despawn portal 1", me->GetEntry(), me->GetDBTableGUIDLow());
                    //me->RemoveAurasDueToSpell(104034);
                    me->CastStop();
                    me->CastSpell(me, SPELL_CONNECTOR_DEAD, true);
                    events.Reset();
                }
                break;
            case ACTION_DESPAWN_PORTAL_THREE:
                me->CastStop();
                me->CastSpell(me, SPELL_CONNECTOR_DEAD, true);
                events.Reset();
                break;
            }
        }
        void InitializeAI()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_CAST_BEAM_ONE, 3000);
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_CAST_BEAM_ONE:
                    if (auto target = me->FindNearestCreature(GetTarget(), 28.f))
                    {
                        me->CastSpell(target, SPELL_CAST_BEAM);
                        //events.ScheduleEvent(EVENT_CAST_BEAM_ONE, 3000);
                    }

                    if (auto perotharn = instance->GetCreature(DATA_PEROTHARN))
                        perotharn->AI()->JustSummoned(me);
                    break;
                default:
                    break;
                }
        }
        uint32 GetTarget()
        {
            switch (me->GetEntry())
            {
            case BEAM_NPC_ONE:
                return BEAM_NPC_TWO;
                break;
            case BEAM_NPC_TWO:
                return BEAM_NPC_THREE;
                break;
            case BEAM_NPC_THREE:
                return BEAM_NPC_ONE;
                break;
            }

            return BEAM_NPC_ONE;
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_beam_connectorsAI(creature);
    }
};

void AddSC_boss_perotharn()
{
    new boss_woe_perotharn();
    new npc_woe_legion_demon();
    new npc_woe_illidan_part_one();
    new npc_woe_nozdormu_part_one();
    new npc_woe_legion_demon_firewall();
    new npc_woe_dreadlord_defender();
    new npc_woe_corrupted_arcanist();
    new npc_woe_guardian_demon();
    new go_woe_portal_energy_focus();
    new go_woe_time_transit_device();
    new spell_woe_fel_decay_heal();
    new spell_woe_drain_essence();
    new spell_woe_summon_circle_spawn();
    new spell_woe_punishing_flames();
    new spell_woe_fel_fireball_aggro();
    new spell_woe_tracked_lock_on_player();
    new spell_woe_drain_essence_damage();
    new spell_woe_drain_essence_knockback();
    new spell_woe_shadowcloak_periodic();
    new spell_woe_shadow_walk();
    new spell_woe_firewall_periodic();
    new spell_woe_wall_of_shadow_periodic();
    new spell_woe_fel_crystal_meltdown();
    new spell_woe_sheen_of_elune();
    new spell_woe_shadowcloak_aggro_check_stealth();
    new spell_woe_demon_grip_pull();
    new spell_woe_demon_grip_periodic();
    new achievement_woe_lazy_eye(); 
    new spell_guardian_demon_stun();
    new npc_perotharn_searching_eye();
    new npc_beam_connectors();
}