#include "ScriptPCH.h"
#include "well_of_eternity.h"

/*
TODO:
- Implement Outro
- Implement coldflame (Frost Magus)
*/

enum Talks
{
    TALK_QUEEN_ENTER_COMBAT             = -55919,
    TALK_QUEEN_ENTER_COMBAT_2           = -53300,
    TALK_QUEEN_TOTAL_OBEDIENCE          = -55920,
    TALK_QUEEN_TOTAL_OBEDIENCE_EMOTE    = -54845,
    TALK_QUEEN_INTERRUPTED              = -53305,
    TALK_QUEEN_COMBAT_END               = -53280,
    TALK_QUEEN_WAVE_2                   = 4,
    TALK_QUEEN_WAVE_3                   = 5,

    TALK_MAGE_WAVE_1_INTRO              = 0,
    TALK_MAGE_WAVE_2_INTRO              = 1,
    TALK_MAGE_WAVE_3_INTRO              = 2,
};

enum Spells
{
    SPELL_SHROUD_OF_LUMINOSITY  = 102915,
    SPELL_SERVANT_OF_THE_QUEEN  = 102334,
    SPELL_TOTAL_OBEDIENCE       = 103241,
    SPELL_TOTAL_OBEDIENCE_STUN  = 110096,
    SPELL_ACTIVATE_WAVE_ONE     = 102681,
    SPELL_ACTIVATE_WAVE_TWO     = 102684,
    SPELL_ACTIVATE_WAVE_THREE   = 102685,
    SPELL_RIDE_VEHICLE_AZSHARA  = 93970,

    // Frost Magus
    SPELL_ICE_FLING             = 102478,
    SPELL_COLDFLAME_PERIODIC    = 102465,
    SPELL_COLDFLAME_AREA        = 102466,
    SPELL_BLADES_OF_ICE         = 102467,
    SPELL_FROST_CHANNELING      = 110492,

    // Fire Magus
    SPELL_FIREBALL              = 102265,
    SPELL_FIREBOMB              = 102482,
    SPELL_BLAST_WAVE            = 102483,
    SPELL_FIRE_CHANNELING       = 110494,

    // Arcane Magus
    SPELL_ARCANE_SHOCK          = 102463,
    SPELL_ARCANE_CHANNELING     = 110495,
    SPELL_HAMMER_OF_RECKONING   = 102453,
    SPELL_HAMMER_OF_RECKONING_2 = 102454,

    // Arcane Bomb
    SPELL_ARCANE_BOMB_VISUAL    = 109122,
    SPELL_ARCANE_BOMB           = 102455,
    SPELL_ARCANE_BOMB_GROUND    = 102460,

    // Hand of the Queen
    SPELL_RIDE_VEHICLE          = 46598,
    SPELL_PUPPET_STRING         = 102319,
    SPELL_PUPPET_STRING_2       = 102318,
    SPELL_PUPPET_STRING_3       = 102315,
    SPELL_PUPPET_STRING_HOVER   = 102312,

    // Wave Helpers
    SPELL_WAVE_1                = 97848,
    SPELL_WAVE_2                = 97849,
    SPELL_WAVE_3                = 97850,

    //RP Dragon soul channeling
    SPELL_DS_ALEX_BEAM          = 107618, //Alex Good Dragon Soul Beam
    SPELL_DS_YSERA_BEAM         = 108615, //Ysera Good Dragon Soul Beam
    SPELL_DS_SORI_BEAM          = 108616, //Sori Good Dragon Soul Beam

    SPELL_DS_EVIL_BEAM          = 107668, //Evil Dragon Soul Beam
    SPELL_DS_EVIL_XPLOD         = 107680, //Evil Dragon Soul Beam Explosion
    SPELL_DS_NELTH_BEAM         = 108759, //Evil Dragon Soul Beam(Deathwing)
    SPELL_DS_OG_BEAM            = 108809, //Evil Dragon Soul Beam(Old God)
    SPELL_DS_OG_XPLOD           = 108826, //Evil Dragon Soul Beam Explosion(Old God)
};

enum Events
{
    EVENT_SERVANT_OF_THE_QUEEN  = 1,
    EVENT_TOTAL_OBEDIENCE       = 2,
    EVENT_ACTIVATE_MAGES        = 3,
    EVENT_PLAYER_ALIVE_CHECK    = 4,
    EVENT_PLAYER_ACTION         = 5,
    EVENT_AZSHARA_FLEE_1,
    EVENT_AZSHARA_FLEE_2,
    EVENT_AZSHARA_FLEE_3,
    EVENT_AZSHARA_FLEE_4,

    // Fire Mage
    EVENT_FIREBALL              = 1,
    EVENT_FIREBOMB              = 2,
    EVENT_BLAST_WAVE            = 3,
    EVENT_ENTER_COMBAT          = 4,

    // Arcane Mage
    EVENT_ARCANE_BOMB           = 1,
    EVENT_ARCANE_SHOCK          = 2,

    // Frost Mage
    EVENT_ICE_FLING             = 1,
    EVENT_COLD_FLAME            = 2,
    EVENT_COLD_FLAME_DUMMY      = 22,
    EVENT_BLADES_OF_ICE         = 3,

    ACTION_INIT_BAT_ESCAPE      = 4,
    PATH_BAT_ESCAPE             = 5,
    PATH_BAT_ESCAPE_2           = 6,
    ACTION_DRAKE_ABORT_RP       = 7,


    EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS,
    EVENT_ALEX_SAY_LETS_PURIFY,
    EVENT_ALL_START_CHANNELING,
    EVENT_DS_CHANNELS_BACK,
    EVENT_NELTHARION_APPEARS_1,
    EVENT_NELTHARION_APPEARS_2,
    EVENT_OG_ENDS_SCENE_1,
    EVENT_OG_ENDS_SCENE_2,
    EVENT_OG_ENDS_SCENE_3,
    EVENT_DUMMY_EVENT,
    ACTION_APPEAR,
    ACTION_RP_SCENE_CLEANUP,
};

static const WellOfEternityEventSpawns mageSpawnPos[6] =
{
    { NPC_FIRE_MAGE,    3453.030f, -5282.740f, 230.031f, 4.4505f },
    { NPC_FROST_MAGE,   3443.540f, -5280.370f, 230.031f, 4.6600f },
    { NPC_ARCANE_MAGE,  3461.139f, -5283.169f, 230.031f, 4.3109f },
    { NPC_FIRE_MAGE,    3435.590f, -5278.350f, 230.031f, 4.5029f },
    { NPC_FROST_MAGE,   3469.729f, -5282.430f, 230.031f, 4.5378f },
    { NPC_ARCANE_MAGE,  3428.429f, -5274.589f, 230.031f, 4.2062f }
};

const Position deathPos = { 3456.799f, -5245.124f, 229.949f, 0.000f };

#define QUEST_THE_VAINGLORIOUS  30100
#define POINT_GROUND    1

uint32 const bat_Path_1_size{ 7 };
const G3D::Vector3 shadowBatRescuePath[bat_Path_1_size] =   //second path, right to mid
{
    {3372.567139f, -5225.144531f, 200.986496f},
    {3363.731934f, -5223.323242f, 250.885880f},
    {3389.708496f, -5197.398438f, 256.368744f},
    {3415.448486f, -5194.395996f, 246.715546f},
    {3414.155029f, -5220.807129f, 246.348389f},
    {3443.492920f, -5228.073730f, 232.006622f},
    {3456.152832f, -5241.364746f, 232.750443f},
};

uint32 const bat_Path_2_size {3};
const G3D::Vector3 shadowBatRescuePath2[bat_Path_2_size] =   //second path, right to mid
{
    {3460.114014f, -5256.793457f, 240.901077f },
    {3483.971436f, -5350.570801f, 253.463120f },
    {3783.862305f, -5501.728516f, 358.443390f },
};

uint32 const handmaiden_Path_1_size{ 3 };
const G3D::Vector3 handmaiden_north[handmaiden_Path_1_size] =   //north path
{
    {3468.290527f, -5225.322754f, 230.575241f},
    {3473.437744f, -5236.002441f, 229.948898f},
    {3468.681152f, -5248.887695f, 229.948898f},
};

const G3D::Vector3 handmaiden_south[handmaiden_Path_1_size] =   //south path
{
    {3453.756348f, -5223.120605f, 230.575073f},
    {3449.925537f, -5231.314941f, 230.575073f},
    {3459.781738f, -5247.622559f, 229.948807f},
};

class HandPositionRelocateHack : public BasicEvent
{
public:
    HandPositionRelocateHack(Creature* owner) : _owner(owner) {}

    bool Execute(uint64 execTime, uint32 /*diff*/) override
    {
        if (_owner->GetVehicleBase())
        {
            Position pos(*_owner->GetVehicleBase());
            _owner->Relocate(pos);
            _owner->m_Events.AddEvent(this, execTime + 100);
            return false;
        }
        return true;
    }

private:
    Creature* _owner;
};

class boss_woe_queen_azshara : public CreatureScript
{
public:
    boss_woe_queen_azshara() : CreatureScript("boss_woe_queen_azshara") {}

    struct boss_woe_queen_azsharaAI : public BossAI
    {
        boss_woe_queen_azsharaAI(Creature* creature) : BossAI(creature, DATA_QUEEN_AZSHARA) 
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetReactState(REACT_PASSIVE);
        }

        void Reset() override
        {
            _Reset();
            waveCounter = 0;
            mageKillCounter = 0;
            summonCount = 0;
            for (int8 i = 0; i < 6; i++)
                me->SummonCreature(mageSpawnPos[i].npcId, mageSpawnPos[i].x, mageSpawnPos[i].y, mageSpawnPos[i].z, mageSpawnPos[i].orientation, TEMPSUMMON_MANUAL_DESPAWN);
            DoCast(me, SPELL_SHROUD_OF_LUMINOSITY, true);

            events.Reset();
        }

        void EnterCombat(Unit* victim) override
        {
            Talk(TALK_QUEEN_ENTER_COMBAT);
            TalkWithDelay(9000, TALK_QUEEN_ENTER_COMBAT_2);
            events.ScheduleEvent(EVENT_ACTIVATE_MAGES, 16000);
            events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
            _EnterCombat();
        }

        void JustDied(Unit* killer) override
        {
            events.Reset();
            _JustDied();
        }

        void JustSummoned(Creature* summon) override
        {
            summonCount++;
            switch (summon->GetEntry())
            {
                case NPC_FIRE_MAGE:
                    if (summonCount == 1)
                        summon->AddAura(SPELL_WAVE_1, summon);
                    else
                        summon->AddAura(SPELL_WAVE_2, summon);
                    summon->CastSpell(summon, SPELL_FIRE_CHANNELING, false);
                    break;
                case NPC_ARCANE_MAGE:
                    if (summonCount == 3)
                        summon->AddAura(SPELL_WAVE_2, summon);
                    else
                        summon->AddAura(SPELL_WAVE_3, summon);
                    summon->CastSpell(summon, SPELL_ARCANE_CHANNELING, false);
                    break;
                case NPC_FROST_MAGE:
                    if (summonCount == 2)
                        summon->AddAura(SPELL_WAVE_1, summon);
                    else
                        summon->AddAura(SPELL_WAVE_3, summon);
                    summon->CastSpell(summon, SPELL_FROST_CHANNELING, false);
                    break;
                default:
                    break;
            }
            BossAI::JustSummoned(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            switch (summon->GetEntry())
            {
                case NPC_HAND_OF_THE_QUEEN:
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PUPPET_STRING, summon->GetGUID());
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PUPPET_STRING_2, summon->GetGUID());
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PUPPET_STRING_3, summon->GetGUID());
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PUPPET_STRING_HOVER, summon->GetGUID());
                    break;
                case NPC_ARCANE_MAGE:
                case NPC_FIRE_MAGE:
                case NPC_FROST_MAGE:
                    if (++mageKillCounter >= 6)
                    {
                        Talk(TALK_QUEEN_COMBAT_END, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                        me->setFaction(35);
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        me->RemoveAurasDueToSpell(SPELL_SHROUD_OF_LUMINOSITY);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SERVANT_OF_THE_QUEEN);
                        if (auto elune = instance->GetGameObject(DATA_LIGHT_OF_ELUNE))
                            elune->SetRespawnTime(elune->GetRespawnDelay());

                        if (auto cache = instance->GetGameObject(DATA_ROYAL_CACHE))
                        {
                            cache->SetRespawnTime(cache->GetRespawnDelay());
                            cache->SetLootRecipient(me->GetLootRecipient());
                        }

                        Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (player->GetQuestStatus(QUEST_THE_VAINGLORIOUS) == QUEST_STATUS_INCOMPLETE)
                                    player->KilledMonsterCredit(me->GetEntry());

                        if (auto nozdormu = instance->GetCreature(DATA_NOZDORMU))
                            nozdormu->NearTeleportTo(NozdormuPositions[1]);

                        //me->DespawnOrUnsummon(10000);
                        _JustDied();
                        if (auto bat = me->SummonCreature(57117, shadowbatSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            bat->AddAura(103760, bat);
                            bat->setFaction(me->getFaction());
                            bat->SetReactState(REACT_PASSIVE);
                            bat->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            bat->AI()->DoAction(ACTION_INIT_BAT_ESCAPE);
                        }
                        events.ScheduleEvent(EVENT_AZSHARA_FLEE_1, 10000);

                        if (auto serve_N = me->SummonCreature(54645/*Royal Handmaiden*/, serve_spawn_N, TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto serve_S = me->SummonCreature(54645/*Royal Handmaiden*/, serve_spawn_S, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            serve_N->DeleteThreatList();
                            serve_N->setFaction(me->getFaction());
                            serve_N->SetReactState(REACT_PASSIVE);
                            serve_N->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            serve_N->AI()->DoAction(EVENT_AZSHARA_FLEE_1);

                            serve_S->DeleteThreatList();
                            serve_S->setFaction(me->getFaction());
                            serve_S->SetReactState(REACT_PASSIVE);
                            serve_S->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            serve_S->AI()->DoAction(EVENT_AZSHARA_FLEE_2);
                        }
                    }
                    else
                    {
                        if (mageKillCounter == 2 || mageKillCounter == 4)
                        {   
                            events.ScheduleEvent(EVENT_ACTIVATE_MAGES, 1);
                        }
                    }
                    break;
                default:
                    break;
            }
        }


        void SpellHit(Unit* attacker, SpellInfo const* spellInfo) override
        {
            if (Spell* spell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                if (spell->GetSpellInfo()->Id == SPELL_TOTAL_OBEDIENCE)
                {
                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    {
                        if (spellInfo->Effects[i].Mechanic == MECHANIC_INTERRUPT || spellInfo->Mechanic == MECHANIC_INTERRUPT)
                        {
                            Talk(TALK_QUEEN_INTERRUPTED);
                            me->InterruptSpell(CURRENT_GENERIC_SPELL);
                            break;
                        }
                    }
                }
            }
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    if (!player->HasAura(SPELL_SERVANT_OF_THE_QUEEN))
                        return false;
            return true;
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_SERVANT_OF_THE_QUEEN:
                    {
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                        {
                            Position pos(*target);
                            if (auto hand = me->SummonCreature(NPC_HAND_OF_THE_QUEEN, pos, TEMPSUMMON_DEAD_DESPAWN))
                            {
                                hand->CastSpell(target, SPELL_SERVANT_OF_THE_QUEEN, true);
                                hand->m_Events.AddEvent(new HandPositionRelocateHack(hand), hand->m_Events.CalculateTime(1000));
                            }
                        }
                        else
                            if (auto target = SelectRandomTarget(false))
                            {
                                Position pos(*target);
                                if (auto hand = me->SummonCreature(NPC_HAND_OF_THE_QUEEN, pos, TEMPSUMMON_DEAD_DESPAWN))
                                {
                                    hand->CastSpell(target, SPELL_SERVANT_OF_THE_QUEEN, true);
                                    hand->m_Events.AddEvent(new HandPositionRelocateHack(hand), hand->m_Events.CalculateTime(1000));
                                }
                            }
                        events.ScheduleEvent(EVENT_SERVANT_OF_THE_QUEEN, 26000);
                    }
                        break;
                    case EVENT_TOTAL_OBEDIENCE:
                        Talk(TALK_QUEEN_TOTAL_OBEDIENCE);
                        Talk(TALK_QUEEN_TOTAL_OBEDIENCE_EMOTE, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                        DoCast(me, SPELL_TOTAL_OBEDIENCE);
                        events.ScheduleEvent(EVENT_TOTAL_OBEDIENCE, 37000);
                        break;
                    case EVENT_ACTIVATE_MAGES:
                        if (waveCounter < 3)
                        {
                            DoCast(me, ++waveCounter == 1 ? SPELL_ACTIVATE_WAVE_ONE : (waveCounter == 2 ? SPELL_ACTIVATE_WAVE_TWO : SPELL_ACTIVATE_WAVE_THREE), true);
                            events.Reset();
                            events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
                            events.ScheduleEvent(EVENT_SERVANT_OF_THE_QUEEN, 24000);
                            events.ScheduleEvent(EVENT_PLAYER_ACTION, 25000);
                            events.ScheduleEvent(EVENT_TOTAL_OBEDIENCE, 36000);
                            events.ScheduleEvent(EVENT_ACTIVATE_MAGES, 90000);
                        }
                        break;
                    case EVENT_PLAYER_ALIVE_CHECK:
                    {
                        if (checkWipe())
                            EnterEvadeMode();
                        else
                            events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
                        break;
                    }
                    case EVENT_PLAYER_ACTION:
                    {
                        Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                        {
                            if (auto player = itr->getSource())
                            {
                                if (player->isCharmed() && !player->HasUnitState(UNIT_STATE_CASTING) && player->isAlive())
                                {
                                    if (auto target = 
                                        player->getVictim() 
                                        ? 
                                        ((player->getVictim()->ToPlayer() && player->getVictim()->isAlive() && !player->isCharmed()) 
                                            ? 
                                            player->getVictim()->ToPlayer() 
                                            : 
                                            SelectRandomTarget(true, player)) 
                                        : 
                                        SelectRandomTarget(true, player))
                                    {
                                        player->GetMotionMaster()->MoveChase(target, 0.0f, 0.0f, 0, MOTION_SLOT_CONTROLLED);
                                        player->SetTarget(target->GetGUID());

                                        switch (player->getClass())
                                        {
                                            case CLASS_DRUID:
                                                if (urand(0, 1))
                                                    player->CastSpell(target, 8921, false);
                                                else
                                                    player->CastSpell(player, 774, false);
                                                break;
                                            case CLASS_HUNTER:
                                                player->CastSpell(target, RAND(2643, 1978), false);
                                                break;
                                            case CLASS_MAGE:
                                                player->CastSpell(target, RAND(44614, 30455), false);
                                                break;
                                            case CLASS_WARLOCK:
                                                player->CastSpell(target, RAND(980, 686), true);
                                                break;
                                            case CLASS_WARRIOR:
                                                player->CastSpell(target, RAND(46924, 845), false);
                                                break;
                                            case CLASS_PALADIN:
                                                if (urand(0, 1))
                                                    player->CastSpell(target, 853, false);
                                                else
                                                    player->CastSpell(player, 20473, false);
                                                break;
                                            case CLASS_PRIEST:
                                                if (urand(0, 1))
                                                    player->CastSpell(target, 34914, false);
                                                else
                                                    player->CastSpell(player, 139, false);
                                                break;
                                            case CLASS_SHAMAN:
                                                if (urand(0, 1))
                                                    player->CastSpell(target, 421, false);
                                                else
                                                    player->CastSpell(player, 61295, false);
                                                break;
                                            case CLASS_ROGUE:
                                                player->CastSpell(target, RAND(16511, 1329), false);
                                                break;
                                            case CLASS_DEATH_KNIGHT:
                                                if (urand(0, 1))
                                                    player->CastSpell(target, 45462, true);
                                                else
                                                    player->CastSpell(target, 49184, true);
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    /*
                                    * what the fuck was this
                                    else
                                        me->Kill(player);
                                    */
                                }
                            }
                        }
                        events.ScheduleEvent(EVENT_PLAYER_ACTION, 1500);
                        break;
                    }
                    case EVENT_AZSHARA_FLEE_1:
                        Talk(2);
                        break;
                    case EVENT_AZSHARA_FLEE_3:
                        Talk(3);
                        events.ScheduleEvent(EVENT_AZSHARA_FLEE_4, 2000);
                        break;
                    case EVENT_AZSHARA_FLEE_4:
                        if (auto bat = me->FindNearestCreature(57117, 20.00f))
                        {
                            me->EnterVehicle(bat);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            bat->AI()->DoAction(EVENT_AZSHARA_FLEE_3);
                        }
                        break;
                    default:
                        break;
                }
            }
        }


        void DoAction(int32 const action) override
        {
            switch (action)
            {
            case EVENT_AZSHARA_FLEE_2:
                events.ScheduleEvent(EVENT_AZSHARA_FLEE_3, 2000);
                break;
            }
        }


    private:
        Position shadowbatSpawn = { 3392.731689f, -5201.51318f, 201.972763f, 4.492863f };

        Position servant_dest_N = { 3468.699463f, -5248.962891f, 229.949753f,  };
        Position servant_dest_S = { 3459.659912f, -5247.402832f, 229.949753f, 4.501460f };
        Position serve_spawn_S = { 3455.017578f, -5223.895996f, 230.575089f,  3.814244f };
        Position serve_spawn_N = { 3463.798096f, -5222.443359f, 230.575241f, 5.648134f };
        uint8 waveCounter;
        uint8 summonCount;
        uint8 mageKillCounter;
        Player* SelectRandomTarget(bool includeTank, Player* exceptPlayer = NULL)
        {
            std::list<HostileReference*> const& threatlist = me->getThreatManager().getThreatList();
            std::list<Player*> tempTargets;

            if (threatlist.empty())
                return NULL;

            for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                if (auto refTarget = (*itr)->getTarget())
                    if (refTarget->GetTypeId() == TYPEID_PLAYER && !refTarget->isCharmed())
                        tempTargets.push_back(refTarget->ToPlayer());

            if (!includeTank)
            {
                tempTargets.remove_if([](WorldObject* target){
                    return target->ToPlayer()->HasTankSpec();
                });
            }

            if (exceptPlayer)
                tempTargets.remove(exceptPlayer);

            if (tempTargets.empty())
                return NULL;

            return Trinity::Containers::SelectRandomContainerElement(tempTargets);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_woe_queen_azsharaAI(creature);
    }
};

class npc_woe_azshara_escape_bat : public CreatureScript
{
public:
    npc_woe_azshara_escape_bat() : CreatureScript("npc_woe_azshara_escape_bat") {}

    struct npc_woe_azshara_escape_batAI : public ScriptedAI
    {
        npc_woe_azshara_escape_batAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);
            
        }
        void InitializeAI()
        {
            
            Position pos(*me);
            pos.m_positionZ += 5.f;
            if (varothen = me->SummonCreature(57118, pos, TEMPSUMMON_MANUAL_DESPAWN))
            {
                varothen->SetDisableGravity(true);
                varothen->SetCanFly(true);
                varothen->SetReactState(REACT_PASSIVE);
                varothen->setFaction(me->getFaction());
                varothen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                events.ScheduleEvent(1, 500);
            }
            
        }

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                    varothen->EnterVehicle(me->ToUnit());
                    break;
                case EVENT_AZSHARA_FLEE_1:  //north servant
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(shadowBatRescuePath, bat_Path_1_size);
                    break;
                case EVENT_AZSHARA_FLEE_3:  //south servant
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveSmoothPath(shadowBatRescuePath2, bat_Path_2_size);
                    events.ScheduleEvent(55, 5000);
                    break;
                case 55:
                    instance->SetData(DATA_RP_SCENE_STATE, 1);
                    if (auto varothen = me->FindNearestCreature(57118, 20.00f))
                        varothen->DespawnOrUnsummon(5000);
                    if (auto azshara = me->FindNearestCreature(NPC_QUEEN_AZSHARA, 20.00f))
                        azshara->DespawnOrUnsummon(5000);
                    me->DespawnOrUnsummon(5000);
                    break;
                }
        }
        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == SPLINE_MOTION_TYPE)
            {
                if (id == bat_Path_1_size - 1)
                {
                    if (currentPath == PATH_BAT_ESCAPE)
                    {
                        if (auto varothen = me->FindNearestCreature(57118, 20.00f))
                        varothen->AI()->Talk(0);

                        if (auto azshara = me->FindNearestCreature(NPC_QUEEN_AZSHARA, 20.00f))
                            azshara->AI()->DoAction(EVENT_AZSHARA_FLEE_2);
                    }
                }
            }
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
            case ACTION_INIT_BAT_ESCAPE:
                events.ScheduleEvent(EVENT_AZSHARA_FLEE_1, 8000);
                currentPath = PATH_BAT_ESCAPE;
                break;
            case EVENT_AZSHARA_FLEE_3:
                events.ScheduleEvent(EVENT_AZSHARA_FLEE_3, 3000);
                currentPath = PATH_BAT_ESCAPE_2;
                break;
            }
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void EnterCombat(Unit* attacker) override
        {
        }
    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        Creature* varothen{ nullptr };
        uint32 currentPath {0};
        EventMap events;

    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_azshara_escape_batAI(creature);
    }
};

class npc_woe_azshara_handmaiden : public CreatureScript
{
public:
    npc_woe_azshara_handmaiden() : CreatureScript("npc_woe_azshara_handmaiden") { }


    struct npc_woe_azshara_handmaidenAI : public ScriptedAI
    {

        npc_woe_azshara_handmaidenAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
        }

        void InitializeAI()
        {
            events.Reset();
            me->SetReactState(REACT_AGGRESSIVE);
            me->AddAura(102415, me);
            me->SetSheath(SHEATH_STATE_UNARMED);
        }
        void Reset()
        {
            events.Reset();

            if (!me->GetDBTableGUIDLow())
                me->SetReactState(REACT_PASSIVE);


            me->AddAura(102415, me);
            me->SetSheath(SHEATH_STATE_UNARMED);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.Reset();
            Start_Rotation();
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);

            me->RemoveAurasDueToSpell(102415);
            me->SetSheath(SHEATH_STATE_MELEE);
            me->SetReactState(REACT_AGGRESSIVE);
            //TC_LOG_ERROR("sql.sql", "Entering combat.");
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

        void DoAction(int32 const action) override
        {
            Position posME = (*me);
            switch (action)
            {
            case EVENT_AZSHARA_FLEE_1:
                currentPath = 1;
                me->AddAura(102415, me);
                me->GetMotionMaster()->Clear();
                me->SetWalk(true);
                me->GetMotionMaster()->MoveSmoothPath(handmaiden_north, handmaiden_Path_1_size, 2.5f);
                break;
            case EVENT_AZSHARA_FLEE_2:
                currentPath = 2;
                me->AddAura(102415, me);
                me->GetMotionMaster()->Clear();
                me->SetWalk(true);
                me->GetMotionMaster()->MoveSmoothPath(handmaiden_south, handmaiden_Path_1_size, 2.5f);
                break;
            default:
                break;
            }
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

        void MovementInform(uint32 type, uint32 id) override
        {
            if (!me->isInCombat())
            if (!me->HasAura(102415))
                DoCast(me, 102415, true);

            if (currentPath > 0)
                if (type == SPLINE_MOTION_TYPE)
                {
                    if (id == handmaiden_Path_1_size - 1)
                    {
                        me->SetOrientation(4.521093f);
                        me->SetFacingTo(me->GetOrientation());
                        me->AddAura(91298, me);
                    }
                }
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

            EVENT_PERFUME,
            EVENT_THORNS,
            EVENT_LULLABY,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_PERFUME = 102233,
            SPELL_PIERCING_THORNS = 102238,
            SPELL_SWEET_LULLABY = 102245,
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

            {EVENT_PERFUME,     SPELL_PERFUME,      5000,          8000,          true,            false},
            {EVENT_THORNS,   SPELL_PIERCING_THORNS,    1000,           8000,           true,       false},
            {EVENT_LULLABY,   SPELL_SWEET_LULLABY,    35000,           45000,           true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_PERFUME:     Spell_Rotation(eventId);  break;
                case EVENT_THORNS:   Spell_Rotation(eventId);  break;
                case EVENT_LULLABY:   Spell_Rotation(eventId);  break;
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
        uint32 currentPath{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_azshara_handmaidenAI(creature);
    }
};

class npc_woe_fire_mage : public CreatureScript
{
public:
    npc_woe_fire_mage() : CreatureScript("npc_woe_fire_mage") {}

    struct npc_woe_fire_mageAI : public ScriptedAI
    {
        npc_woe_fire_mageAI(Creature* creature) : ScriptedAI(creature) 
        {
            isActive = false;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }

        bool CanAIAttack(Unit const* target) const
        {
            return isActive;
        }

        void SpellHit(Unit* attacker, SpellInfo const* spellInfo)
        {
            if (!isActive && spellInfo->SpellIconID == 5611)
            {
                me->RemoveAurasDueToSpell(SPELL_WAVE_1);
                me->RemoveAurasDueToSpell(SPELL_WAVE_2);

                switch (spellInfo->Id)
                {
                case SPELL_ACTIVATE_WAVE_ONE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 8000);
                    break;
                case SPELL_ACTIVATE_WAVE_TWO:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 14000);
                    break;
                case SPELL_ACTIVATE_WAVE_THREE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 13000);
                    break;
                }
            }
        }

        void JustDied(Unit* killer)
        {
            events.Reset();
        }

        void UpdateAI(uint32 const diff) override
        {
            if (isActive && !UpdateVictim())
                return;

            events.Update(diff);

            if (isActive && me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ENTER_COMBAT:
                        isActive = true;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        events.ScheduleEvent(EVENT_FIREBALL, 3000);
                        events.ScheduleEvent(EVENT_FIREBOMB, urand(20000, 30000));
                        events.ScheduleEvent(EVENT_BLAST_WAVE, urand(20000, 30000));
                        if (auto player = me->FindNearestPlayer(200.00f))
                            me->AI()->AttackStart(player);
                        break;
                    case EVENT_FIREBALL:
                        DoCastVictim(SPELL_FIREBALL);
                        events.ScheduleEvent(EVENT_FIREBALL, urand(2500, 3000));
                        break;
                    case EVENT_FIREBOMB:
                        DoCastRandom(SPELL_FIREBOMB, 45.00f);
                        events.ScheduleEvent(EVENT_FIREBOMB, urand(15000, 30000));
                        break;
                    case EVENT_BLAST_WAVE:
                        DoCast(me, SPELL_BLAST_WAVE);
                        events.ScheduleEvent(EVENT_BLAST_WAVE, urand(15000, 30000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
        private:
            EventMap events;
            bool isActive;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_fire_mageAI(creature);
    }
};

class npc_woe_arcane_mage : public CreatureScript
{
public:
    npc_woe_arcane_mage() : CreatureScript("npc_woe_arcane_mage") {}

    struct npc_woe_arcane_mageAI : public ScriptedAI
    {
        npc_woe_arcane_mageAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            isActive = false;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }

        bool CanAIAttack(Unit const* target) const override
        {
            return isActive;
        }

        void SpellHit(Unit* attacker, SpellInfo const* spellInfo)
        {
            if (!isActive && spellInfo->SpellIconID == 5611)
            {
                me->RemoveAurasDueToSpell(SPELL_WAVE_1);
                me->RemoveAurasDueToSpell(SPELL_WAVE_2);

                switch (spellInfo->Id)
                {
                case SPELL_ACTIVATE_WAVE_ONE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 8000);
                    break;
                case SPELL_ACTIVATE_WAVE_TWO:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 14000);
                    break;
                case SPELL_ACTIVATE_WAVE_THREE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 13000);
                    break;
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            switch (summon->GetEntry())
            {
                case NPC_HAMMER_OF_DIVINITY:
                {
                    summon->CastSpell(summon, SPELL_ARCANE_BOMB_VISUAL, true);
                    Position pos(*summon);
                    pos.m_positionZ = 231.9485f;
                    summon->GetMotionMaster()->MovePoint(POINT_GROUND, pos);
                    break;
                }
                case NPC_HAMMER_OF_DIVINITY_2:
                    summon->CastSpell(summon, SPELL_ARCANE_BOMB_GROUND, true);
                    break;
                default:
                    break;
            }
        }

        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || id != POINT_GROUND)
                return;

            if (summon->GetEntry() == NPC_HAMMER_OF_DIVINITY)
            {
                summon->RemoveAurasDueToSpell(SPELL_ARCANE_BOMB_VISUAL);
                if (auto groundVisual = summon->FindNearestCreature(NPC_HAMMER_OF_DIVINITY_2, 20.00f))
                    groundVisual->RemoveAurasDueToSpell(SPELL_ARCANE_BOMB_GROUND);
                summon->CastSpell(summon, SPELL_ARCANE_BOMB, true, NULL, NULL, me->GetGUID());
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (isActive && !UpdateVictim())
                return;

            events.Update(diff);

            if (isActive && me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ENTER_COMBAT:
                        isActive = true;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        events.ScheduleEvent(EVENT_ARCANE_BOMB, urand(5000, 10000));
                        events.ScheduleEvent(EVENT_ARCANE_SHOCK, urand(10000, 15000));
                        if (auto player = me->FindNearestPlayer(200.00f))
                            me->AI()->AttackStart(player);
                        break;
                    case EVENT_ARCANE_BOMB:
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.00f, true))
                        {
                            DoCast(target, SPELL_HAMMER_OF_RECKONING, true);
                            DoCast(target, SPELL_HAMMER_OF_RECKONING_2, true);
                        }
                        events.ScheduleEvent(EVENT_ARCANE_BOMB, urand(15000, 20000));
                        break;
                    case EVENT_ARCANE_SHOCK:
                        DoCast(me, SPELL_ARCANE_SHOCK, false);
                        events.ScheduleEvent(EVENT_ARCANE_SHOCK, urand(20000, 30000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
        private:
            EventMap events;
            SummonList summons;
            bool isActive;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_arcane_mageAI(creature);
    }
};

class npc_woe_frost_mage : public CreatureScript
{
public:
    npc_woe_frost_mage() : CreatureScript("npc_woe_frost_mage") {}

    struct npc_woe_frost_mageAI : public ScriptedAI
    {
        npc_woe_frost_mageAI(Creature* creature) : ScriptedAI(creature) 
        {
            isActive = false;
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
        }

        bool CanAIAttack(Unit const* target) const
        {
            return isActive;
        }

        void SpellHit(Unit* attacker, SpellInfo const* spellInfo)
        {
            if (!isActive && spellInfo->SpellIconID == 5611)
            {
                me->RemoveAurasDueToSpell(SPELL_WAVE_1);
                me->RemoveAurasDueToSpell(SPELL_WAVE_2);

                switch (spellInfo->Id)
                {
                case SPELL_ACTIVATE_WAVE_ONE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 8000);
                    break;
                case SPELL_ACTIVATE_WAVE_TWO:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 14000);
                    break;
                case SPELL_ACTIVATE_WAVE_THREE:
                    events.ScheduleEvent(EVENT_ENTER_COMBAT, 13000);
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (isActive && !UpdateVictim())
                return;

            events.Update(diff);

            if (isActive && me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ENTER_COMBAT:
                        isActive = true;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        events.ScheduleEvent(EVENT_ICE_FLING, urand(5000, 10000));
                        events.ScheduleEvent(EVENT_COLD_FLAME, urand(10000, 15000));
                        events.ScheduleEvent(EVENT_BLADES_OF_ICE, urand(15000, 20000));
                        if (auto player = me->FindNearestPlayer(200.00f))
                            me->AI()->AttackStart(player);
                        break;
                    case EVENT_ICE_FLING:
                        DoCast(me, SPELL_ICE_FLING, false);
                        events.ScheduleEvent(EVENT_ICE_FLING, urand(2500, 5000));
                        break;
                    case EVENT_COLD_FLAME:
                        ColdFlame();
                        events.ScheduleEvent(EVENT_COLD_FLAME, urand(10000, 15000));
                        break;
                    case EVENT_BLADES_OF_ICE:
                        DoCastRandom(SPELL_BLADES_OF_ICE, 40.00f);
                        events.ScheduleEvent(EVENT_BLADES_OF_ICE, urand(20000, 30000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
        void ColdFlame()
        {
            auto list_of_players = me->GetPlayersInRange(30.f, true);

            if (list_of_players.size())
                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                {
                    me->SetInFront(random_target);
                    me->SetFacingTo(me->GetOrientation());
                    
                    Position pos(*me);
                    if (auto c = me->SummonCreature(54635, pos, TEMPSUMMON_TIMED_DESPAWN, 15000))
                        me->Kill(c);
                    
                    DoCast(random_target, SPELL_COLDFLAME_PERIODIC);
                }
        }

    private:
        EventMap events;
        bool isActive;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_frost_mageAI(creature);
    }
};

class npc_woe_frost_mage_dummy : public CreatureScript
{
public:
    npc_woe_frost_mage_dummy() : CreatureScript("npc_woe_frost_mage_dummy") {}

    struct npc_woe_frost_mage_dummyAI : public ScriptedAI
    {
        npc_woe_frost_mage_dummyAI(Creature* creature) : ScriptedAI(creature) {}

        void JustDied(Unit* /*killer*/)
        {
            me->Respawn(true);
            events.ScheduleEvent(EVENT_COLD_FLAME_DUMMY, 1500);
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                case EVENT_COLD_FLAME_DUMMY:
                    ColdFlameDummy();
                    break;
                default:
                    break;
                }
            }
        }
        
        void ColdFlameDummy()
        {
            if (auto fmage = me->FindNearestCreature(54883, 5.f, true))
            {
                Position pos(*me);

                for (auto i = 0; i <= 10; ++i)
                {
                    me->MoveBlink(pos, 2.f * i, 0);
                    if (auto c = me->SummonCreature(54638, pos, TEMPSUMMON_TIMED_DESPAWN, 1500))
                        fmage->CastWithDelay(i * 300, c, SPELL_COLDFLAME_AREA, true);
                }
            }
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_frost_mage_dummyAI(creature);
    }
};

class spell_woe_servant_of_the_queen : public SpellScriptLoader
{
public:
    spell_woe_servant_of_the_queen() : SpellScriptLoader("spell_woe_servant_of_the_queen") { }

    class spell_woe_servant_of_the_queen_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_servant_of_the_queen_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
            if (targets.size() > 1)
                targets.resize(1);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_servant_of_the_queen_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENEMY);
        }


        class spell_woe_servant_of_the_queen_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_woe_servant_of_the_queen_AuraScript);

            void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
            {
                if (auto owner = GetOwner()->ToUnit())
                {
                    owner->StopMoving();
                }
            }
            void Register()
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_woe_servant_of_the_queen_AuraScript::HandleOnEffectRemove, EFFECT_2, SPELL_AURA_DAMAGE_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_woe_servant_of_the_queen_AuraScript();
        }
    };

    class spell_woe_servant_of_the_queen_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_servant_of_the_queen_AuraScript);

        void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto hand = GetCaster())
            {
                if (auto target = GetTarget())
                    if (target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                        target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                
                hand->CastSpell(GetTarget(), SPELL_RIDE_VEHICLE, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_2, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_3, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_HOVER, true);
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_woe_servant_of_the_queen_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_servant_of_the_queen_AuraScript();
    }

    SpellScript* GetSpellScript() const override
    {
        return new spell_woe_servant_of_the_queen_SpellScript();
    }
};

class PlayerDistanceChecker : public BasicEvent
{
public:
    PlayerDistanceChecker(Unit* owner, bool initialMovement) : _owner(owner), _initialMovement(initialMovement) {}

    bool Execute(uint64 execTime, uint32 /*diff*/) override
    {
        if (_initialMovement)
        {
            _owner->GetMotionMaster()->MovePoint(0, deathPos);
            _initialMovement = false;
        }
        else if (_owner->GetDistance2d(deathPos.GetPositionX(), deathPos.GetPositionY()) <= 2.00f)
        {
            _owner->Kill(_owner);
            return true;
        }

        _owner->m_Events.AddEvent(this, execTime + 500);
        return false;
    }

private:
    Unit* _owner;
    bool _initialMovement;
};

class spell_woe_total_obedience : public SpellScriptLoader
{
public:
    spell_woe_total_obedience() : SpellScriptLoader("spell_woe_total_obedience") { }

    class spell_woe_total_obedience_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_total_obedience_AuraScript);

        void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Position pos(*GetTarget());
            if (auto hand = GetTarget()->SummonCreature(NPC_HAND_OF_THE_QUEEN, pos))
            {
                if (auto target = GetTarget())
                    if (target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                        target->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                
                hand->AddUnitTypeMask(UNIT_MASK_ACCESSORY);
                hand->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                hand->CastSpell(GetTarget(), SPELL_RIDE_VEHICLE, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_2, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_3, true);
                hand->CastWithDelay(500, GetTarget(), SPELL_PUPPET_STRING_HOVER, true);
                GetTarget()->m_Events.AddEvent(new PlayerDistanceChecker(GetTarget(), true), GetTarget()->m_Events.CalculateTime(600));
            }
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_woe_total_obedience_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_total_obedience_AuraScript();
    }
};

class spell_woe_mage_wave_selector : public SpellScriptLoader
{
public:
    spell_woe_mage_wave_selector() : SpellScriptLoader("spell_woe_mage_wave_selector") { }

    class spell_woe_mage_wave_selector_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_mage_wave_selector_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (auto c = GetCaster())
                if (auto caster = c->ToCreature())
            switch (GetSpellInfo()->Id)
            {
                case SPELL_ACTIVATE_WAVE_ONE:
                    targets.remove_if([](WorldObject* target){
                        return !target->ToUnit()->HasAura(SPELL_WAVE_1);
                    });

                    if (auto random_target = Trinity::Containers::SelectRandomContainerElement(targets))
                        if (auto mage = random_target->ToCreature())
                            mage->AI()->TalkWithDelay(1000, TALK_MAGE_WAVE_1_INTRO);
                    break;
                case SPELL_ACTIVATE_WAVE_TWO:
                    caster->AI()->Talk(TALK_QUEEN_WAVE_2);
                    targets.remove_if([](WorldObject* target){
                        return !target->ToUnit()->HasAura(SPELL_WAVE_2);
                    });
                    if (auto random_target = Trinity::Containers::SelectRandomContainerElement(targets))
                        if (auto mage = random_target->ToCreature())
                            mage->AI()->TalkWithDelay(7000, TALK_MAGE_WAVE_2_INTRO);
                    break;
                case SPELL_ACTIVATE_WAVE_THREE:
                    caster->AI()->Talk(TALK_QUEEN_WAVE_3);
                    targets.remove_if([](WorldObject* target){
                        return !target->ToUnit()->HasAura(SPELL_WAVE_3);
                    });
                    if (auto random_target = Trinity::Containers::SelectRandomContainerElement(targets))
                        if (auto mage = random_target->ToCreature())
                            mage->AI()->TalkWithDelay(7000, TALK_MAGE_WAVE_3_INTRO);
                    break;
                default:
                    break;
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_mage_wave_selector_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_mage_wave_selector_SpellScript();
    }
};

class spell_woe_ice_fling : public SpellScriptLoader
{
public:
    spell_woe_ice_fling() : SpellScriptLoader("spell_woe_ice_fling") { }

    class spell_woe_ice_fling_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_ice_fling_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (!targets.empty())
            {
                WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);

                targets.remove_if([](WorldObject* target){
                    return target->GetTypeId() == TYPEID_PLAYER && target->ToPlayer()->HasTankSpec();
                });

                if (!targets.empty())
                   target = Trinity::Containers::SelectRandomContainerElement(targets);

                targets.clear();
                targets.push_back(target);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_ice_fling_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_ice_fling_SpellScript();
    }
};

class spell_woe_hammer_of_reckoning : public SpellScriptLoader
{
public:
    spell_woe_hammer_of_reckoning() : SpellScriptLoader("spell_woe_hammer_of_reckoning") { }

    class spell_woe_hammer_of_reckoning_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_hammer_of_reckoning_SpellScript);

        void ChangeSummonPos(SpellEffIndex /*effIndex*/)
        {
            WorldLocation summonPos = *GetExplTargetDest();
            Position offset = { 0.0f, 0.0f, 14.0f, 0.0f };
            summonPos.RelocateOffset(offset);
            SetExplTargetDest(summonPos);
            GetHitDest()->RelocateOffset(offset);
        }

        void ModDestHeight(SpellEffIndex /*effIndex*/)
        {
            Position offset = { 0.0f, 0.0f, 14.0f, 0.0f };
            const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_woe_hammer_of_reckoning_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            OnEffectLaunch += SpellEffectFn(spell_woe_hammer_of_reckoning_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_hammer_of_reckoning_SpellScript();
    }
};

class npc_woe_DS_bronzeDrake : public CreatureScript
{
public:
    npc_woe_DS_bronzeDrake() : CreatureScript("npc_woe_DS_bronzeDrake") { }


    struct npc_woe_DS_bronzeDrakeAI : public ScriptedAI
    {

        npc_woe_DS_bronzeDrakeAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {

            case ACTION_RP_SCENE_CLEANUP:
            {
                scene_over = true;
                if (auto v = me->GetVehicleKit())
                    if (v->GetPassenger(0))
                    {
                        Position flyto{ 3204.064209f, -5548.13916f, 104.712982f, 3.270475f };
                        me->SetHomePosition(flyto);
                        me->GetMotionMaster()->MovePoint(ACTION_DRAKE_ABORT_RP, flyto, true, 30.f);
                    }
                    else
                        me->DespawnOrUnsummon();

                break;
            }

            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (!scene_over)
            {
                if (auto alex = instance->GetCreature(DATA_RP_ALEX))
                {
                    //TC_LOG_ERROR("sql.sql", "Alex found for movement inform.");
                    if (auto ai = alex->AI())
                    {
                        //TC_LOG_ERROR("sql.sql", "Alex AI found for movement inform.");
                        ai->SummonedMovementInform(me, type, id);
                    }
                }

            }
                //TC_LOG_ERROR("sql.sql", "Movement Inform: %u %u", type, id);
                if (type == POINT_MOTION_TYPE)
                {
                    if (id == 55)
                    {
                        //just gone up in the air, need to move to movie seat
                        events.ScheduleEvent(55, 1);
                    }

                    if (id == ACTION_DRAKE_ABORT_RP)
                    {
                        //TC_LOG_ERROR("sql.sql", "Passenger exiting 1");
                        Position Jump_To{ 3082.165771f, -5562.167480f, 19.602331f, 4.167752f };
                        if (auto v = me->GetVehicleKit())
                            if (auto p = v->GetPassenger(0))
                            {
                                p->ExitVehicle();
                                p->GetMotionMaster()->Clear();
                                p->GetMotionMaster()->ForceMoveJump(Jump_To.GetPositionX(), Jump_To.GetPositionY(), Jump_To.GetPositionZ(), 14.f, 14.f);
                            }
                    }
                }
        }


        void InitializeAI()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->setFaction(35);
        }

        void OnSpellClick(Unit* clicker, bool& result)
        {
            if (auto p = clicker->ToPlayer())
                p->EnterVehicle(me);
        }

        void PassengerBoarded(Unit* player, int8 seatId, bool apply)
        {
            //TC_LOG_ERROR("sql.sql", "Passenger %u boarded into seat %u and %u", player->GetGUID(), seatId, apply);

            if (apply)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);

                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    Position up(*me);
                    up.m_positionZ += 45.f;
                    me->GetMotionMaster()->MovePoint(55, up, true, 20.f);
            }
            else
            {
                me->DespawnOrUnsummon(2000);
                //TC_LOG_ERROR("sql.sql", "Passenger exiting 4");
            }
        }

        void SetData(uint32 data_field, uint32 data_value)
        {
            if (data_field == DATA_PLAYER_DRAKE_SLOT)
            {
                my_drake_slot = data_value;
                switch (data_value)
                {
                case 2:
                    TalkWithDelay(2000, 0, NULL, CHAT_MSG_MONSTER_YELL);
                    break;
                case 1:
                    TalkWithDelay(4000, 1, NULL, CHAT_MSG_MONSTER_YELL);
                    break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (auto v = me->GetVehicleKit())
                if (v->GetPassenger(0))
                    //TC_LOG_ERROR("sql.sql", "bronze with pass: ... %u - %u", urand(1, 50), diff);

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 5:
                    break;
                case 55:
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(75, RP_movie_seat[my_drake_slot], true, 13.f);
                    break;
                default:
                    break;
                }
        }
    private:
        bool scene_over{ false };
        uint32 roleplay_state{ 0 };
        uint32 my_drake_slot{ 0 };
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        Position RP_movie_seat[5]
        {
            {3459.753174f, -5350.397949f, 275.360962f, 3.508007f},
            {3449.753174f, -5350.397949f, 282.360962f, 3.508007f},
            {3439.753174f, -5350.397949f, 275.360962f, 3.508007f},
            {3429.753174f, -5350.397949f, 282.360962f, 3.508007f},
            {3419.753174f, -5350.397949f, 275.360962f, 3.508007f},
        };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_bronzeDrakeAI(creature);
    }
};

class npc_woe_DS_Alexstrasza : public CreatureScript
{
public:
    npc_woe_DS_Alexstrasza() : CreatureScript("npc_woe_DS_Alexstrasza") { }



    struct npc_woe_DS_AlexstraszAI : public BossAI
    {
        npc_woe_DS_AlexstraszAI(Creature* creature) : BossAI(creature, DATA_RP_ALEX), summons(me)
        {
        }

        void JustSummoned(Creature* summon) 
        {
            //TC_LOG_ERROR("sql.sql", "Just summoned %u", summon->GetEntry());
            summons.Summon(summon);
        }

        void InitializeAI()
        {
            me->SetVisible(false);
            me->SetReactState(REACT_PASSIVE);
        }

        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
                if (summon->GetEntry() == 144951)
                {
                    switch (id)
                    {
                    case NPC_RP_SCENE_ALEXTRASZA:
                        me->CastStop();
                        summon->RemoveAllAuras();
                        summon->CastSpell(summon, SPELL_DS_EVIL_XPLOD);
                        summon->DespawnOrUnsummon();
                        break;
                    case NPC_RP_SCENE_SORIDORMI:
                        if (auto sori = me->FindNearestCreature(NPC_RP_SCENE_SORIDORMI, 500.f))
                        {
                            sori->CastStop();
                            summon->RemoveAllAuras();
                            summon->CastSpell(summon, SPELL_DS_EVIL_XPLOD);
                            summon->DespawnOrUnsummon();
                        }
                        break;
                    case NPC_RP_SCENE_YSERA:
                        if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                        {
                            ysera->MonsterYell("It is too powerful!", LANG_UNIVERSAL, 0);
                            ysera->PlayDirectSound(26150);
                            ysera->CastStop();
                            summon->RemoveAllAuras();
                            summon->CastSpell(summon, SPELL_DS_EVIL_XPLOD);
                            events.ScheduleEvent(EVENT_NELTHARION_APPEARS_1, 2000);
                            summon->DespawnOrUnsummon();
                        }
                        break;
                    case NPC_RP_SCENE_NELTHARION:
                        if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                        {
                            summon->CastSpell(dw, SPELL_DS_OG_XPLOD);
                            events.ScheduleEvent(EVENT_OG_ENDS_SCENE_2, 500);
                            summon->DespawnOrUnsummon();
                        }
                    }
                }

            if (type == POINT_MOTION_TYPE)
                if (summon->GetEntry() == NPC_RP_SCENE_BRONZE_DRAKE)
                {

                    //TC_LOG_ERROR("sql.sql", "Bronze drake movement inform: %u %u", type, id);
                    if (id == 75)
                        if (!rp_scene_started)
                        {
                            //TC_LOG_ERROR("sql.sql", "SummonedMovementInform bronze drake at point 75.");
                            events.ScheduleEvent(ACTION_APPEAR, 8000);
                            rp_scene_started = true;
                        }

                    if (id == ACTION_DRAKE_ABORT_RP)
                        if (auto v = summon->GetVehicleKit())
                        {
                            v->RemoveAllPassengers();
                            summon->DespawnOrUnsummon(1000);
                        }
                }
        }

        void EnterCombat(Unit* victim) override
        {
            me->DeleteThreatList();
        }

        void UpdateAI(uint32 const diff)
        {
            //TC_LOG_ERROR("sql.sql","AL ... %u - %u", urand(1,50), diff);
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_DUMMY_EVENT:
                    //TC_LOG_ERROR("sql.sql", "ALexstrazsa EVENT_DUMMY_EVENT executing.");
                    break;
                case ACTION_APPEAR:
                {
                    //TC_LOG_ERROR("sql.sql", "Alex recognizes ACTION_APPEAR");
                    me->SetVisible(true);
                    Position Alex{ 3479.371094f, -5542.010254f, 267.882690f, 1.801666f };
                    me->GetMotionMaster()->MovePoint(100, Alex, true, 14.f);
                    if (auto sori = me->FindNearestCreature(NPC_RP_SCENE_SORIDORMI, 500.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing sori movement.");
                        sori->SetVisible(true);
                        Position sori_entrance{ 3265.505615f, -5489.393066f, 279.777161f, 0.103277f };
                        sori->GetMotionMaster()->MovePoint(55, sori_entrance, true, 20.f);
                    }
                    if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "Executing ysera movement.");
                        ysera->SetVisible(true);
                        Position ysera_entrance{ 3358.487061f, -5560.653809f, 276.846710f, 1.026119f };
                        ysera->GetMotionMaster()->MovePoint(55, ysera_entrance, true, 20.f);
                    }
                    if (auto og = me->FindNearestCreature(NPC_RP_SCENE_OLD_GOD, 500.f))
                        og->SetVisible(true);

                    if (!events.HasEvent(EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS))
                    {
                        events.ScheduleEvent(EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS, 2000);
                        //TC_LOG_ERROR("sql.sql", "Scheduled EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS");
                    }
                    else
                    {
                        events.ScheduleEvent(EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS, 2000);
                        //TC_LOG_ERROR("sql.sql", "Re-Scheduled EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS");
                    }

                    break;
                }
                case EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS:
                {
                    //TC_LOG_ERROR("sql.sql", "ALexstrazsa event EVENT_BRONZE_DRAKE_SAY_ITS_THE_ASPECTS executing.");
                    if (auto c = me->FindNearestCreature(NPC_RP_SCENE_BRONZE_DRAKE, 500.f))
                    {
                        if (auto ai = c->AI())
                        {
                            ai->Talk(3, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                        }
                    }

                    events.ScheduleEvent(EVENT_ALEX_SAY_LETS_PURIFY, 3000);
                    //TC_LOG_ERROR("sql.sql", "Alex: Scheduling EVENT_ALEX_SAY_LETS_PURIFY");
                    break;
                }
                case EVENT_ALEX_SAY_LETS_PURIFY:
                    Talk(0);
                    events.ScheduleEvent(EVENT_ALL_START_CHANNELING, 5000);
                    break;
                case EVENT_ALL_START_CHANNELING:
                    if (auto trigger_0 = me->SummonCreature(144951, DS_Coin))
                        trigger_guids[0] = trigger_0->GetGUID();

                    if (auto trigger_1 = me->SummonCreature(144951, DS_Coin))
                        trigger_guids[1] = trigger_1->GetGUID();

                    if (auto trigger_2 = me->SummonCreature(144951, DS_Coin))
                        trigger_guids[2] = trigger_2->GetGUID();

                    if (auto trigger_3 = me->SummonCreature(144951, { DS_Coin.GetPositionX(), DS_Coin.GetPositionY(), DS_Coin.GetPositionZ() - 8.f, DS_Coin.GetOrientation() }))
                        trigger_guids[3] = trigger_3->GetGUID();


                                    if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                                        if (auto sori = me->FindNearestCreature(NPC_RP_SCENE_SORIDORMI, 500.f))
                                        {
                                            me->CastSpell(GetTrigger(0), SPELL_DS_ALEX_BEAM);
                                            ysera->CastSpell(GetTrigger(1), SPELL_DS_YSERA_BEAM);
                                            sori->CastSpell(GetTrigger(2), SPELL_DS_SORI_BEAM);
                                        }
                            
                    events.ScheduleEvent(EVENT_DS_CHANNELS_BACK, 6000);
                    break;
                case EVENT_DS_CHANNELS_BACK:
                {
                    Talk(1);
                    if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                        if (auto sori = me->FindNearestCreature(NPC_RP_SCENE_SORIDORMI, 500.f))
                        {
                            GetTrigger(3)->CastSpell(GetTrigger(0), SPELL_DS_EVIL_BEAM);
                            GetTrigger(3)->CastSpell(GetTrigger(1), SPELL_DS_EVIL_BEAM);
                            GetTrigger(3)->CastSpell(GetTrigger(2), SPELL_DS_EVIL_BEAM);
                            GetTrigger(0)->GetMotionMaster()->MovePoint(me->GetEntry(), *me, true, 13.f);
                            GetTrigger(1)->GetMotionMaster()->MovePoint(NPC_RP_SCENE_YSERA, *ysera, true, 10.f);
                            GetTrigger(2)->GetMotionMaster()->MovePoint(NPC_RP_SCENE_SORIDORMI, *sori, true, 15.f);
                        }
                    break;
                }
                case EVENT_NELTHARION_APPEARS_1:
                {
                    Position deathwing{ 3504.927490f, -5482.624512f, 270.103943f, 3.046453f };
                    if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                    {
                        dw->SetVisible(true);
                        dw->GetMotionMaster()->MovePoint(55, deathwing, true, 10.f);
                        //dw->CastSpell(triggers[3], SPELL_DS_NELTH_BEAM);
                        dw->TalkWithDelay(4000, 0, NULL, CHAT_MSG_MONSTER_YELL);

                        auto triggerCount = 0;
                        if (auto v = dw->GetVehicleKit())
                            for (int i = 0; i < 7; i++)
                                if (v->HasEmptySeat(i))
                                {
                                    //TC_LOG_ERROR("Sql.sql", "Adding Deathwing trigger into seat number %u", i);
                                    if (deathwing_armor_triggers[triggerCount] = me->SummonCreature(144951, *dw))
                                        if (auto c = deathwing_armor_triggers[triggerCount])
                                        {
                                            c->EnterVehicle(dw, i);
                                            //c->CastWithDelay(i * 1000, c, SPELL_DS_NELTH_BEAM);
                                            GetTrigger(3)->CastWithDelay(i * 3500, c, SPELL_DS_NELTH_BEAM);
                                        }
                                    triggerCount++;
                                }

                    }
                    events.ScheduleEvent(EVENT_NELTHARION_APPEARS_2, 8000);
                    break;
                }
                case 99:
                    if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                    {
                        ysera->MonsterYell("He is lost to us, sister. He is Neltharion no longer.", LANG_UNIVERSAL, 0);
                        ysera->PlayDirectSound(26151);
                    }
                    break;
                case EVENT_NELTHARION_APPEARS_2:
                {
                    //"Neltharion what have you done?"
                    Talk(2, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);

                    //"IT IS MINE!"
                    if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                        dw->TalkWithDelay(7000, 1, NULL, CHAT_MSG_MONSTER_YELL);

                    //"He is neltharion no longer."
                    events.ScheduleEvent(99, 9000);//where it's actually done.
                    /*
                    * CHANGED - IS NOW A FUCKIN MONSTER SAY AND PLAYDISTANT SOUND BECAUSE YSERA JUST FUCKIN DECIDES HER DRAGON FORMS CAN"T DISPLAY TEXT CLIENT SIDE.
                    if (auto ys = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                        ys->TalkWithDelay(9000, 1, NULL, CHAT_MSG_MONSTER_SAY);
                    */

                    //"The power is Ripping him apart!"
                    TalkWithDelay(15000, 3, NULL, CHAT_MSG_MONSTER_SAY);

                    //"nooo..... awayyyyy...."
                    if (auto OG = me->FindNearestCreature(NPC_RP_SCENE_OLD_GOD, 500.f))
                    {
                        OG->SetObjectScale(5.f);
                        OG->TalkWithDelay(18000, 0, NULL, CHAT_MSG_MONSTER_YELL);
                    }

                    //"I WILL NOT BE DENIED!"
                    if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                        dw->TalkWithDelay(24000, 2, NULL, CHAT_MSG_MONSTER_YELL);

                    events.ScheduleEvent(EVENT_OG_ENDS_SCENE_1, 24000);
                    break;
                }
                case EVENT_OG_ENDS_SCENE_1:
                {
                    //"AWAY!"
                    if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                    if (auto OG = me->FindNearestCreature(NPC_RP_SCENE_OLD_GOD, 500.f))
                    if (old_god_projectile = me->SummonCreature(144951, *OG))
                    if (auto og_Proj = old_god_projectile->ToCreature())
                    {
                        og_Proj->GetMotionMaster()->MovePoint(NPC_RP_SCENE_NELTHARION, *dw, true, 45.f);
                        OG->TalkWithDelay(2000, 1, NULL, CHAT_MSG_MONSTER_YELL);
                        OG->CastSpell(og_Proj, SPELL_DS_OG_BEAM);
                    }

                    break;
                }
                case EVENT_OG_ENDS_SCENE_2:
                {
                    events.ScheduleEvent(EVENT_OG_ENDS_SCENE_3, 25000);
                    //TC_LOG_ERROR("sql.sql", "EVENT_OG_ENDS_SCENE_2 - removing rp npcs.");


                    //TC_LOG_ERROR("sql.sql", "SCENE_2 step 1");
                    summons.DespawnAll();
                    //TC_LOG_ERROR("sql.sql", "SCENE_2 step 2");
                    me->SetVisible(false);
                    //TC_LOG_ERROR("sql.sql", "SCENE_2 step 3");
                    if (auto dw = me->FindNearestCreature(NPC_RP_SCENE_NELTHARION, 500.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 4");
                        dw->DespawnOrUnsummon(1000);
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 5");
                    }
                    if (auto ysera = me->FindNearestCreature(NPC_RP_SCENE_YSERA, 500.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 6");
                        ysera->SetVisible(false);
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 7");
                        ysera->DespawnOrUnsummon(1000);
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 8");
                    }
                    if (auto sori = me->FindNearestCreature(NPC_RP_SCENE_SORIDORMI, 500.f))
                    {
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 9");
                        sori->SetVisible(false);
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 10");
                        sori->DespawnOrUnsummon(1000);
                        //TC_LOG_ERROR("sql.sql", "SCENE_2 step 11");
                    }
                    //TC_LOG_ERROR("sql.sql", "SCENE_2 step 12");
                    if (auto og = me->FindNearestCreature(NPC_RP_SCENE_OLD_GOD, 500.f))
                        og->DespawnOrUnsummon();


                        bool first_drake{ false };
                        bool second_drake{ false };
                        std::list<Creature*> units2;
                        GetCreatureListWithEntryInGrid(units2, me, NPC_RP_SCENE_BRONZE_DRAKE, 500.0f);
                        if (units2.size())
                        for (auto itr = units2.begin(); itr != units2.end(); ++itr)
                            if (auto c = (*itr))
                            {
                                c->AI()->DoAction(ACTION_RP_SCENE_CLEANUP);

                                if (auto v = c->GetVehicleKit())
                                    if (v->GetPassenger(0))
                                    {
                                        if (!first_drake)
                                        {
                                            first_drake = true;
                                            c->AI()->Talk(4, NULL, CHAT_MSG_MONSTER_YELL);
                                            c->TalkWithDelay(3500, 5, NULL, CHAT_MSG_MONSTER_YELL);
                                        }
                                    }

                            }
                    break;
                }
                case EVENT_OG_ENDS_SCENE_3:
                {
                    //TC_LOG_ERROR("sql.sql", "EVENT_OG_ENDS_SCENE_3 - despawning everything.");
                    //summons.DespawnAll();
                    me->DespawnOrUnsummon();
                    break;
                }
                    default:
                        //TC_LOG_ERROR("sql.sql", "Executing unacknowledged event id %u", eventId);
                        break;
                }
            
        }
        Creature* GetTrigger(int guid)
        {
            std::list<Creature*> units2;
            GetCreatureListWithEntryInGrid(units2, me, 144951, 500.0f);
            if (units2.size())
                for (auto itr = units2.begin(); itr != units2.end(); ++itr)
                    if (auto c = (*itr))
                    {
                        if (c->GetGUID() == trigger_guids[guid])
                            return c->ToCreature();
                    }
            return nullptr;
        }
    
    private:
        Position DS_Coin{ 3409.563965f, -5472.845215f, 244.841629f, 0.197536f };
        bool rp_scene_started{ false };
        SummonList summons;
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
        uint64 trigger_guids[4]{ 0, 0, 0, 0 };
        TempSummon* deathwing_armor_triggers[7]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        TempSummon* old_god_projectile{ nullptr };
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_AlexstraszAI(creature);
    }
};

class npc_woe_DS_Ysera : public CreatureScript
{
public:
    npc_woe_DS_Ysera() : CreatureScript("npc_woe_DS_Ysera") { }


    struct npc_woe_DS_YseraAI : public ScriptedAI
    {

        npc_woe_DS_YseraAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
            me->SetVisible(false);
        }

        void UpdateAI(uint32 const diff) override
        {
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_YseraAI(creature);
    }
};

class npc_woe_DS_Soridormi : public CreatureScript
{
public:
    npc_woe_DS_Soridormi() : CreatureScript("npc_woe_DS_Soridormi") { }


    struct npc_woe_DS_SoridormiAI : public ScriptedAI
    {

        npc_woe_DS_SoridormiAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
            me->SetVisible(false);
        }


        void UpdateAI(uint32 const diff) override
        {
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_SoridormiAI(creature);
    }
};

class npc_woe_DS_Neltharion : public CreatureScript
{
public:
    npc_woe_DS_Neltharion() : CreatureScript("npc_woe_DS_Neltharion") { }


    struct npc_woe_DS_NeltharionAI : public ScriptedAI
    {

        npc_woe_DS_NeltharionAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
            me->SetVisible(false);
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_NeltharionAI(creature);
    }
};

class npc_woe_DS_UnkownOG : public CreatureScript
{
public:
    npc_woe_DS_UnkownOG() : CreatureScript("npc_woe_DS_UnkownOG") { }


    struct npc_woe_DS_UnkownOGAI : public ScriptedAI
    {

        npc_woe_DS_UnkownOGAI(Creature* creature) : ScriptedAI(creature)
        {
        }

        void InitializeAI()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetVisible(false);
            me->SetObjectScale(1.f);
            me->SetReactState(REACT_PASSIVE);
            me->AddAura(37803, me);
        }

        void UpdateAI(uint32 const diff) override
        {
        }
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_DS_UnkownOGAI(creature);
    }
};

class spell_deathwing_blast_from_OG : public SpellScriptLoader // 76717
{
public:
    spell_deathwing_blast_from_OG() : SpellScriptLoader("spell_deathwing_blast_from_OG") {}

    class spell_deathwing_blast_from_OG_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_deathwing_blast_from_OG_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                for (auto itr = targets.begin(); itr != targets.end();)
                    if (auto t = (*itr))
                        if (t->GetEntry() != NPC_RP_SCENE_NELTHARION)
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
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_deathwing_blast_from_OG_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_deathwing_blast_from_OG_SpellScript();
    }
};

class spell_enchanted_reflection : public SpellScriptLoader
{
public:
    spell_enchanted_reflection() : SpellScriptLoader("spell_enchanted_reflection") { }

    class spell_enchanted_reflection_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_enchanted_reflection_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size())
                if (auto random_target = Trinity::Containers::SelectRandomContainerElement(targets))
                    if (auto c = GetCaster())
                        for (auto itr = targets.begin(); itr != targets.end();)
                            if (auto t = (*itr))
                                if (t->GetGUID() == random_target->GetGUID())
                                {
                                    ++itr;
                                }
                                else
                                {
                                    targets.erase(itr++);
                                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_enchanted_reflection_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_enchanted_reflection_SpellScript();
    }
};

void AddSC_boss_queen_azshara()
{
    new boss_woe_queen_azshara();
    new npc_woe_fire_mage();
    new npc_woe_arcane_mage();
    new npc_woe_frost_mage();
    new npc_woe_frost_mage_dummy();
    new npc_woe_azshara_escape_bat();
    new npc_woe_azshara_handmaiden();
    new spell_woe_servant_of_the_queen();
    new spell_woe_total_obedience();
    new spell_woe_mage_wave_selector();
    new spell_woe_ice_fling();
    new spell_woe_hammer_of_reckoning();
    new spell_enchanted_reflection();

    //Dragon Soul Scene
    new npc_woe_DS_bronzeDrake();
    new npc_woe_DS_Ysera();
    new npc_woe_DS_Soridormi();
    new npc_woe_DS_Neltharion();
    new npc_woe_DS_Alexstrasza();
    new npc_woe_DS_UnkownOG();
    new spell_deathwing_blast_from_OG();
}