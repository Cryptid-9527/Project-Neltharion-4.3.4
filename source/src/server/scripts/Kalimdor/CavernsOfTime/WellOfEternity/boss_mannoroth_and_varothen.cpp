#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "well_of_eternity.h"
#include "LFGMgr.h"

enum Talks
{
    TALK_ILLIDAN_CLOSE_PORTAL               = -54103,
    TALK_ILLIDAN_VERY_WELL = -54104,
    TALK_ILLIDAN_WEAK_PITIFUL = -54111,
    TALK_ILLIDAN_THIS_WILL_BE_FUN = -54112,
    TALK_ILLIDAN_WAIT_I_HAVE_AN_IDEA = -54113,
    TALK_ILLIDAN_LET_THEM_COME = -54105,
    TALK_ILLIDAN_WHAT_OUR_PEOPLE = -54116,
    TALK_ILLIDAN_EMOTE_DRINK = -54155,
    TALK_ILLIDAN_YES_YES_I_CAN_FEEL = -54117,
    TALK_ILLIDAN_THE_SWORD_HAS_PIERCED = -54254,
    TALK_ILLIDAN_HE_IS_STILL_CONNECTED = -54257,
    TALK_ILLIDAN_YOU_ARE_NOT_THE_SOLE = -54261,
    TALK_ILLIDAN_I_WILL_BE_THE_SAVIOR = -54264,
    TALK_ILLIDAN_DEMONIC = -54266,
    TALK_TYRANDE_COMBAT_DONE = -54270,

    TALK_MALFURION_IT_IS_MAINTAINED         = -54106,
    TALK_MALFURION_CANNOT_BREAK             = -54107,

    TALK_TYRANDE_HE_KNOWS_WHAT              = -54108,
    TALK_TYRANDE_MOTHER_MOON                = -54110,
    TALK_TYRANDE_ILLIDAN_WHAT_IS_IN         = -54114,
    TALK_TYRANDE_EMOTE_BLESSING             = -54153,
    TALK_TYRANDE_EMOTE_BLESSING_BOSS        = -54233,
    TALK_TYRANDE_LIGHT_OF_ELUNE_SAVE = -54229,
    TALK_TYRANDE_I_WILL_HANDLE_DEMON = -54227,
    TALK_TYRANDE_EMOTE_CAN_HOLD_HER = -54236,
    TALK_TYRANDE_HOLD_THEM_BACK_AGAIN = -54231,
    TALK_TYRANDE_I_AM_OUT_OF_ARROWS = -54258,
    TALK_TYRANDE_EMOTE_HAND_OF_ELUNE = -54268,
    TALK_TYRANDE_THERE_ARE_TOO_MANY = -54609,
    TALK_TYRANDE_EMOTE_COLLAPSE = -54263,
    TALK_TYRANDE_ILLIDAN_YOU_MUSNT = -54262,
    TALK_TYRANDE_NO_ILLIDAN = -54265,

    TALK_CAPTAIN_INTRO = -54119,
    TALK_CAPTAIN_AGGRO = -54225,
    TALK_CAPTAIN_DEATH = -54253,
    TALK_EMOTE_SWORD_FALLS_TO_THE_GROUND = -54255,

    TALK_MANNOROTH_INTRO = -54118,
    TALK_MONNOROTH_AGGRO = -54223,
    TALK_MANNOROTH_EMOTE_FIRESTORM = -48107,
    TALK_MONNOROTH_RRRAAAGHHH = -54555,
    TALK_MONNOROTH_NETHER_TEAR = -54256,
    TALK_MANNOROTH_INFERNO = -54259,
    TALK_MANNOROTH_EMOTE_INFERNAL = -54608,
    TALK_EMOTE_PORTAL_OPEN = -54260,
    TALK_EMOTE_FELHOUND_SUMMON = -54605,
    TALK_EMOTE_FELGUARD_SUMMON = -54606,
    TALK_EMOTE_DOOMGUARD_SUMMON = -54607,
};

enum Spells
{
    // Mannoroth
    SPELL_PORTAL_TO_THE_NETHER      = 102920,
    SPELL_FEL_FIRESTORM             = 103888,
    SPELL_FEL_FIRESTORM_PERIODIC    = 103894,
    SPELL_FELBLADE                  = 103966,
    SPELL_FEL_DRAIN                 = 104961,
    SPELL_NETHER_PORTAL             = 104625,
    SPELL_NETHER_PORTAL_TRIGGERED   = 104648,
    SPELL_NETHER_TEAR               = 105041,
    SPELL_SUMMON_INFERNO            = 105141,
    SPELL_EMBEDDED_BLADE            = 104820,
    SPELL_ACHIEVEMENT_SPELL         = 105576,
    SPELL_FINALE_ANIMKIT            = 105422,
    SPELL_PORTAL_PULL               = 105334,
    SPELL_FEL_FIRE_NOVA             = 105093,
    SPELL_MAGISTRIKE_ARC_DAMAGE     = 105523,
    SPELL_MAGISTRIKE_ARC_VISUAL     = 104822,

    // Illidan
    SPELL_WATERS_OF_ETERNITY        = 103952,
    SPELL_DEMON_RUSH                = 104205,
    SPELL_DEMONIC_SIGHT             = 104746,
    SPELL_DARKLANCE                 = 104394,
    SPELL_AURA_OF_IMMOLATION        = 104379,
    SPELL_GIFT_OF_SARGERAS          = 104998,
    SPELL_GIFT_OF_SARGERAS_PLAYER   = 105009,
    SPELL_DEMONIC_TAUNT             = 104461,

    // Tyrande Whisperwind
    SPELL_LUNAR_SHOT_DOOMBRINGER    = 104214,
    SPELL_LUNAR_SHOT_INSTANT        = 104313,
    SPELL_BLESSING_OF_ELUNE         = 103917,
    SPELL_ELUNES_WRATH              = 103919,
    SPELL_HAND_OF_ELUNE             = 105072,
    SPELL_COLLAPSE                  = 96733,  // correct spell missing

    // Abyssal Doombringer
    SPELL_ABYSSAL_FLAMETHROWER      = 105218, // target 55091
    SPELL_ABYSSAL_FLAMES            = 103992,

    // Shadowbat
    SPELL_RIDE_VEHICLE_HARDCODED    = 46598,
    SPELL_DISPLACEMENT              = 103763,

    // Captain
    SPELL_MAGISTRIKE                = 103669,
    SPEL_VAROTHENS_MAGICAL_BLADE    = 104815,

    // Dreadlord Debilitator
    SPELL_DEBILITATING_FLAY         = 104678,
    SPELL_COSMETIC_PURPLE_STATE     = 83237,
    SPELL_COSMETIC_SUMMON_EFFECT    = 104672,

    // Fel Flames
    SPELL_FEL_FLAMES_PERIODIC       = 103892,

    // Blade
    SPELL_BLADE_VISUAL              = 104823,

    // Spellclick Blade
    SPELL_MAGICAL_BLADE_TRIGGER     = 104817,
    SPELL_MAGICAL_BLADE_VISUAL      = 104819,

    // Tear Portal
    SPELL_SUMMON_FELHOUND_PERIODIC  = 105053,
    SPELL_SUMMON_FELGUARD_PERIODIC  = 105057,
    SPELL_SUMMON_DOOMGUARD_PERIODIC = 105061,

    //Dragon soul visual
    SPELL_DRAGON_SOUL_BUNNY_COSMETIC = 102845,
};

enum Events
{
    // Mannoroth
    EVENT_FEL_FIRESTORM                 = 1,
    EVENT_FELBLADE                      = 2,
    EVENT_NETHER_PORTAL                 = 3,
    EVENT_NETHER_TEAR                   = 4,
    EVENT_NETHER_POTAL                  = 5,
    EVENT_FEL_FIRE_NOVA                 = 6,
    EVENT_SUMMON_DOOMGUARD              = 7,
    EVENT_SUMMON_DOOMGUARD_BOSSEVENT    = 8,
    EVENT_PULL_DEMONS_INTO_PORTAL       = 9,

    // Illidan
    EVENT_CHECK_DEMONS          = 1,
    EVENT_DRINK                 = 2,
    EVENT_ATTACK_DOOMBRINGER    = 3,
    EVENT_PLAYER_ALIVE_CHECK    = 4,
    EVENT_AURA_OF_IMMOLATION    = 5,
    EVENT_TAUNT                 = 6,

    // Tyramde Whisperwind
    EVENT_START_ESCORT          = 1,
    EVENT_BLESSING_OF_ELUNE     = 2,
    EVENT_SEND_EMOTE            = 4,
    EVENT_HAND_OF_ELUNE         = 5,
    EVENT_COLLAPSE              = 6,
    EVENT_ATTACK_DOOMGUARD      = 7,
    EVENT_ATTACK_ASSIST_PLAYERS = 8,

    // Abyssal Doombringer
    EVENT_ABYSSAL_FLAMES        = 1,
    EVENT_ABYSSAL_FLAMETHROWER  = 2,

    // Captain Verothen
    EVENT_INTRO                 = 1,
    EVENT_MAGISTRIKE            = 2,
    //EVENT_BDSM_TYRANDE          = 3,
};

enum Actions
{
    ACTION_DONT_CALL_FOR_HELP       = 1,
    ACTION_START_ESCORT_EVENT       = 2,
    ACTION_RESTART_ESCORT           = 3,
    ACTION_ATTACK_DOOMBRINGER       = 4,
    ACTION_CAST_BLESSING_OF_ELUNE   = 5,
    ACTION_HIGHGUARD_DIED           = 6,
    ACTION_START_INTRO              = 7,
    ACTION_LIGHT_OF_ELUNE           = 8,
    ACTION_RESTART_COMBAT           = 9,
    ACTION_RESET_BOSS_FIGHT         = 10,
    ACTION_HAND_OF_ELUNE            = 11,
    ACTION_COLLAPSE                 = 12,
    ACTION_ILLIDAN_START_PHASE_3    = 13,
    ACTION_DISABLE_DAMAGE_COUNTER   = 14,
    ACTION_FIGHT_FINISHED           = 15,
    ACTION_PULL_INTO_PORTAL         = 16,
    ACTION_ATTACK_ASSIST_PLAYERS    = 17,
    ACTION_RESET_EVENTS             = 18,
};

const Position highguardElitePos[2] = 
{
    { 3298.469f, -5721.270f, 18.899f, 2.3561f },
    { 3310.100f, -5694.490f, 18.070f, 2.9321f }
};

const Position doomguardDevastatorPos = { 3426.192f, -5462.206f, 26.174f, 5.5580f };
const Position doomguardDevastatorLandingPos[5] =
{
    { 3288.266f, -5669.797f, 11.758f, 4.5082f },
    {3533.025391f, -5847.861816f, 68.247459f, 5.341068f},
    {3792.412598f, -5441.447266f, 60.116974f, 0.742560f},
    {2859.843262f, -5310.005371f, 261.054749f, 2.847429f},
    {3845.879883f, -5051.450195f, 161.030899f, 0.777907f},
};

#define POINT_LANDING    1
#define QUEST_THE_PATH_TO_THE_DRAGON_SOUL 30101
#define DATA_THATS_NOT_CANON 1
#define CRITERIA_GUILD_RUN_WELL_OF_ETERNITY 18476


class boss_woe_mannoroth : public CreatureScript
{
public:
    boss_woe_mannoroth() : CreatureScript("boss_woe_mannoroth") {}

    struct boss_woe_mannorothAI : public BossAI
    {
        boss_woe_mannorothAI(Creature* creature) : BossAI(creature, DATA_MANNOROTH)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void Reset() override
        {
            DoCast(me, SPELL_PORTAL_TO_THE_NETHER, false);
            RemoveEncounterFrame();
            aliveDemons = 0;
            felstormCounter = 0;
            damageReceived = 0;
            canDamageCount = true;
            phase85PctTriggered = false;
            phase70PctTriggered = false;
            phase60PctTriggered = false;
            phase50PctTriggered = false;
            _Reset();
        }

        void EnterEvadeMode()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            _EnterEvadeMode();
            BossAI::EnterEvadeMode();
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (canDamageCount)
            {
                damageReceived += damage;
                if (damageReceived >= 10000000)
                {
                    canDamageCount = false;
                    if (auto captain = instance->GetCreature(DATA_CAPTAIN_VAROTHEN))
                        if (captain->isAlive())
                            DoCast(captain, SPELL_FEL_DRAIN, true);
                }
            }

            if (!phase85PctTriggered && me->HealthBelowPctDamaged(86, damage))
            {
                phase85PctTriggered = true;
                Talk(TALK_MONNOROTH_NETHER_TEAR);
                DoCast(me, SPELL_NETHER_TEAR, false);

                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    tyrande->AI()->DoAction(ACTION_HAND_OF_ELUNE);

                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                    illidan->AI()->TalkWithDelay(16000, TALK_ILLIDAN_HE_IS_STILL_CONNECTED);
            }
            else if (!phase70PctTriggered && me->HealthBelowPctDamaged(71, damage))
            {
                phase70PctTriggered = true;
                Talk(TALK_EMOTE_FELGUARD_SUMMON, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                Talk(TALK_MANNOROTH_EMOTE_FIRESTORM, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                if (auto portal = instance->GetCreature(DATA_PORTAL))
                    portal->AddAura(SPELL_SUMMON_FELGUARD_PERIODIC, portal);
            }
            else if (!phase60PctTriggered && me->HealthBelowPctDamaged(61, damage))
            {
                phase60PctTriggered = true;
                Talk(TALK_EMOTE_DOOMGUARD_SUMMON, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                if (auto portal = instance->GetCreature(DATA_PORTAL))
                    portal->AddAura(SPELL_SUMMON_DOOMGUARD_PERIODIC, portal);
            }
            else if (!phase50PctTriggered && me->HealthBelowPctDamaged(51, damage))
            {
                phase50PctTriggered = true;
                Talk(TALK_MANNOROTH_INFERNO);
                TalkWithDelay(14000, TALK_MANNOROTH_EMOTE_INFERNAL, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                DoCast(me, SPELL_SUMMON_INFERNO, false);

                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                {
                    tyrande->AI()->DoAction(ACTION_COLLAPSE);
                    tyrande->AI()->TalkWithDelay(20000, TALK_TYRANDE_THERE_ARE_TOO_MANY);
                    tyrande->AI()->TalkWithDelay(22000, TALK_TYRANDE_EMOTE_COLLAPSE, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    tyrande->AI()->TalkWithDelay(32000, TALK_TYRANDE_ILLIDAN_YOU_MUSNT);
                    tyrande->AI()->TalkWithDelay(44000, TALK_TYRANDE_NO_ILLIDAN);
                }

                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                {
                    illidan->CastWithDelay(27000, illidan, SPELL_GIFT_OF_SARGERAS, false);
                    illidan->CastWithDelay(57000, illidan, SPELL_GIFT_OF_SARGERAS_PLAYER, true);
                    illidan->AI()->TalkWithDelay(27000, TALK_ILLIDAN_YOU_ARE_NOT_THE_SOLE);
                    illidan->AI()->TalkWithDelay(35000, TALK_ILLIDAN_I_WILL_BE_THE_SAVIOR);
                    illidan->AI()->TalkWithDelay(46000, TALK_ILLIDAN_DEMONIC);
                }
            }

            if (me->HealthBelowPctDamaged(26, damage))
            {
                me->setRegeneratingHealth(false);
                damage = 0;
                float health{ float(me->GetMaxHealth()) * 0.05f };
                me->SetHealth(uint32(health));
                instance->OnUnitDeath(me->ToUnit());
                instance->SetBossState(DATA_MANNOROTH, DONE);
                DoCastAOE(SPELL_ACHIEVEMENT_SPELL, true);
                instance->CompleteGuildCriteriaForGuildGroup(CRITERIA_GUILD_RUN_WELL_OF_ETERNITY);

                /*
                    Force-end fight
                */
                events.Reset();
                me->RemoveAllAuras();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                me->DeleteThreatList();
                me->CombatStop();
                if (auto portal = instance->GetCreature(DATA_PORTAL))
                    portal->RemoveAllAuras();

                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                    illidan->AI()->DoAction(ACTION_FIGHT_FINISHED);

                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    tyrande->AI()->DoAction(ACTION_FIGHT_FINISHED);

                if (auto chromie = instance->GetCreature(DATA_CHROMIE))
                    chromie->SetVisible(true);

                if (auto cache = instance->GetGameObject(DATA_MANNOROTH_CACHE))
                    cache->SetRespawnTime(cache->GetRespawnDelay());

                Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                {
                    if (auto player = itr->getSource())
                    {
                        if (player->GetQuestStatus(QUEST_THE_PATH_TO_THE_DRAGON_SOUL) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(me->GetEntry());

                        sLFGMgr->InitializeLockedDungeons(player);
                        player->GetSession()->SendLfgPlayerLockInfo();
                        player->RemoveAura(SPELL_GIFT_OF_SARGERAS_PLAYER);
                    }
                }

                //me->Kill(me);
                //me->DespawnOrUnsummon(5000);
                instance->FinishLfgDungeon(me);
                RemoveEncounterFrame();
                _JustDied();
                DoAction(ACTION_PULL_INTO_PORTAL);


                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                {
                    tyrande->DeleteThreatList();
                    tyrande->CombatStop();
                }
            }
        }


        void DoAction(int32 const action)
        {
            if (action == ACTION_DISABLE_DAMAGE_COUNTER)
            {
                //events.ScheduleEvent(EVENT_NETHER_POTAL, 5000);
                canDamageCount = false;
            }

            if (action == ACTION_PULL_INTO_PORTAL)
            {
                DoCast(me, 105422, true);//finale animkit
                if (auto c = me->FindNearestCreature(56087, 400.f, true))
                {
                    c->AI()->DoAction(ACTION_PULL_INTO_PORTAL);
                }
            }
        }

        void EnterCombat(Unit* victim) override
        {
            if (instance->GetBossState(DATA_MANNOROTH) != DONE)
            {
                AddEncounterFrame();
                me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                Talk(TALK_MONNOROTH_AGGRO);
                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    tyrande->AI()->TalkWithDelay(12000, TALK_TYRANDE_I_WILL_HANDLE_DEMON);
                events.ScheduleEvent(EVENT_FEL_FIRESTORM, 15000);
                events.ScheduleEvent(EVENT_FELBLADE, 30000);
                events.ScheduleEvent(EVENT_NETHER_POTAL, 71000);
                events.ScheduleEvent(EVENT_SUMMON_DOOMGUARD, 1000);
                me->CastWithDelay(2000, me, SPELL_FELBLADE);
                _EnterCombat();
            }
        }

        uint32 GetData(uint32 data) const override
        {
            if (data == DATA_THATS_NOT_CANON)
                return damageReceived;
            return 0;
        }

        void JustSummoned(Creature* summon) override
        {
            switch (summon->GetEntry())
            {
                case NPC_FEL_FLAME:
                    summon->CastWithDelay(750, summon, SPELL_FEL_FLAMES_PERIODIC, true);
                    break;
                case NPC_DREADLORD_DEBILITATOR:
                    aliveDemons++;
                    summon->SetReactState(REACT_PASSIVE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    summon->CastSpell(summon, SPELL_COSMETIC_PURPLE_STATE, true);
                    summon->CastSpell(summon, SPELL_COSMETIC_SUMMON_EFFECT, true);
                    if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    {
                        summon->SetFacingToObject(tyrande);
                        tyrande->AI()->DoAction(ACTION_LIGHT_OF_ELUNE);
                        summon->CastSpell(tyrande, SPELL_DEBILITATING_FLAY, false);
                    }
                    break;
                case NPC_EMBEDDED_BLADE:
                    summon->CastSpell(me, SPELL_RIDE_VEHICLE_HARDCODED, true);
                    break;
                default:
                    break;
            }
            BossAI::JustSummoned(summon);
        }

        void SpellHit(Unit* /*attacker*/, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_MAGICAL_BLADE_TRIGGER)
            {
                Talk(TALK_MONNOROTH_RRRAAAGHHH);
                DoCast(me, SPELL_EMBEDDED_BLADE, true);
                events.CancelEvent(EVENT_FEL_FIRESTORM);
                events.CancelEvent(EVENT_FELBLADE);
                events.ScheduleEvent(EVENT_FEL_FIRE_NOVA, 5000);
                Position pos(*me);
                me->SummonCreature(NPC_EMBEDDED_BLADE, pos);
                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                {
                    illidan->AddAura(73411, illidan);
                    illidan->AI()->TalkWithDelay(5000, TALK_ILLIDAN_THE_SWORD_HAS_PIERCED);
                }
                if (auto portal = instance->GetCreature(DATA_DOOMGUARD_PORTAL))
                    portal->m_Events.KillAllEvents(false);
            }
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_NETHER_TEAR)
            {
                Talk(TALK_EMOTE_PORTAL_OPEN, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                Talk(TALK_EMOTE_FELHOUND_SUMMON, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                target->AddAura(SPELL_SUMMON_FELHOUND_PERIODIC, target);
            }
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_DREADLORD_DEBILITATOR:
                    if (--aliveDemons <= 0)
                        if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                            tyrande->AI()->DoAction(ACTION_RESTART_COMBAT);
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_FEL_FIRESTORM:
                        Talk(TALK_MANNOROTH_EMOTE_FIRESTORM, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                        DoCast(me, SPELL_FEL_FIRESTORM, false);
                        me->CastWithDelay(3000, me, SPELL_FEL_FIRESTORM_PERIODIC, true);
                        if (++felstormCounter <= 2)
                            events.ScheduleEvent(EVENT_FEL_FIRESTORM, 39000);
                        break;
                    case EVENT_FELBLADE:
                        DoCast(me, SPELL_FELBLADE, false);
                        events.ScheduleEvent(EVENT_FELBLADE, 30000);
                        break;
                    case EVENT_NETHER_POTAL:
                        DoCast(me, SPELL_NETHER_PORTAL, false);
                        break;
                    case EVENT_FEL_FIRE_NOVA:
                        DoCastAOE(SPELL_FEL_FIRE_NOVA);
                        events.ScheduleEvent(EVENT_FEL_FIRE_NOVA, urand(10000, 15000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }
    private:
        int8 aliveDemons;
        uint8 felstormCounter;
        uint32 damageReceived;
        bool phase85PctTriggered;
        bool phase70PctTriggered;
        bool phase60PctTriggered;
        bool phase50PctTriggered;
        bool canDamageCount;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_woe_mannorothAI(creature);
    }
};

class npc_woe_illidan_part_two : public CreatureScript
{
public:
    npc_woe_illidan_part_two() : CreatureScript("npc_woe_illidan_part_two") { }

    struct npc_woe_illidan_part_twoAI : public npc_escortAI
    {
        npc_woe_illidan_part_twoAI(Creature* creature) : npc_escortAI(creature)
        {
            isEventActive = false;
            restartEscortAtHome = true;
            instance = me->GetInstanceScript();
        }

        bool CanAIAttack(Unit const* target) const override
        {
            return target->GetEntry() != NPC_CAPTAIN_VAROTHEN;
        }

        void ResetCombat()
        {
            checkWipe = true;

            if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
            {
                mannoroth->SetReactState(REACT_PASSIVE);
                mannoroth->CombatStop(true);
                mannoroth->AI()->EnterEvadeMode();
            }
            if (auto captain = instance->GetCreature(DATA_CAPTAIN_VAROTHEN))
            {
                captain->SetReactState(REACT_PASSIVE);
                if (!captain->isAlive())
                    captain->Respawn(true);
                else
                    captain->CombatStop(true);
            }
            if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                tyrande->AI()->DoAction(ACTION_RESET_BOSS_FIGHT);

            me->AI()->DoAction(ACTION_RESET_BOSS_FIGHT);

            if (auto portal = instance->GetCreature(DATA_PORTAL))
                portal->RemoveAllAuras();

            if (auto portal = instance->GetCreature(DATA_DOOMGUARD_PORTAL))
                portal->m_Events.KillAllEvents(false);

            me->DespawnCreaturesInArea(NPC_DOOMGUARD_DEVASTATOR, 500.00f);
            me->DespawnCreaturesInArea(NPC_EMBEDDED_BLADE, 500.00f);
            me->DespawnCreaturesInArea(NPC_FELHOUND, 500.00f);
            me->DespawnCreaturesInArea(NPC_FELGUARD, 500.00f);
            me->DespawnCreaturesInArea(NPC_DOOMGUARD, 500.00f);
            me->DespawnCreaturesInArea(NPC_EMBEDDED_BLADE_CAPTAIN, 500.00f);
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_RESTART_ESCORT:
                    me->CombatStop(true);
                    me->GetMotionMaster()->MoveTargetedHome();
                    break;
                case ACTION_FIGHT_FINISHED:
                    TalkWithDelay(15000, 1, NULL, CHAT_MSG_MONSTER_SAY);
                    me->CombatStop(true);
                    me->RemoveAllAuras();
                    events.Reset();
                    SetEscortPaused(false);
                    break;
                case ACTION_RESET_BOSS_FIGHT:
                    me->SetReactState(REACT_PASSIVE);
                    events.Reset();
                    restartEscortAtHome = false;
                    me->CombatStop(true);
                    me->RemoveAllAuras();
                    me->m_Events.KillAllEvents(false);
                    SetNextWaypoint(14, false, false);
                    me->SetHomePosition(resetPosition);
                    me->GetMotionMaster()->MoveTargetedHome();
                    break;
                case ACTION_RESET_EVENTS:
                    events.Reset();
                    break;
                default:
                    break;
            }
        }


        void DamageTaken(Unit* attacker, uint32& damage)
        {
            float numerator{ float(me->GetHealthPct()) };
            float denominator{ 100.f };
            float dmg = float(damage) * (numerator / denominator);
            damage = (me->HealthBelowPctDamaged(5, damage) ? 0 : uint32(dmg));
        }

        void EnterCombat(Unit* attacker)
        {
            events.ScheduleEvent(EVENT_AURA_OF_IMMOLATION, 5000);
            events.ScheduleEvent(EVENT_TAUNT, 2000);
        }

        void EnterEvadeMode() override {}

        void JustReachedHome() override
        {
            me->SetWalk(false);
            if (restartEscortAtHome)
                SetEscortPaused(false);
            restartEscortAtHome = true;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            if (!instance->IsDone(DATA_PEROTHARN) || !instance->IsDone(DATA_QUEEN_AZSHARA))
                return;
            
            if (instance->IsDone(DATA_MANNOROTH))
                return;

            if (checkWipe && who->GetTypeId() == TYPEID_PLAYER)
                if (me->GetDistance(who) <= 70.f)
                {
                    checkWipe = false;
                }

            if (!isEventActive && who->GetTypeId() == TYPEID_PLAYER)
            {
                if (me->GetDistance(who) <= 20.00f)
                {
                    isEventActive = true;
                    SetMaxPlayerDistance(500.0f);
                    SetDespawnAtEnd(false);
                    SetDespawnAtFar(false);
                    events.ScheduleEvent(EVENT_CHECK_DEMONS, 1000);
                    Talk(TALK_ILLIDAN_CLOSE_PORTAL, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                    TalkWithDelay(15000, TALK_ILLIDAN_VERY_WELL, NULL, CHAT_MSG_MONSTER_SAY);
                    TalkWithDelay(26000, TALK_ILLIDAN_LET_THEM_COME, NULL, CHAT_MSG_MONSTER_SAY);
                    TalkWithDelay(36000, TALK_ILLIDAN_WEAK_PITIFUL, NULL, CHAT_MSG_MONSTER_SAY);

                    if (auto malfurion = instance->GetCreature(DATA_MALFURION_STORMRAGE))
                    {
                        malfurion->AI()->TalkWithDelay(3000, TALK_MALFURION_IT_IS_MAINTAINED, NULL, CHAT_MSG_MONSTER_SAY);
                        malfurion->AI()->TalkWithDelay(10000, TALK_MALFURION_CANNOT_BREAK, NULL, CHAT_MSG_MONSTER_SAY);
                    }

                    if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    {
                        tyrande->SetReactState(REACT_PASSIVE);
                        tyrande->AI()->TalkWithDelay(20000, TALK_TYRANDE_HE_KNOWS_WHAT, NULL, CHAT_MSG_MONSTER_SAY);
                        tyrande->AI()->TalkWithDelay(30000, TALK_TYRANDE_MOTHER_MOON, NULL, CHAT_MSG_MONSTER_SAY);
                        tyrande->AI()->DoAction(ACTION_CAST_BLESSING_OF_ELUNE);
                    }
                }
            }
            npc_escortAI::MoveInLineOfSight(who);
        }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
                case 10:
                    me->GetPosition(&resetPosition);
                    me->SetHomePosition(resetPosition);
                    SetEscortPaused(true);
                    TalkWithDelay(4000, TALK_ILLIDAN_THIS_WILL_BE_FUN, NULL, CHAT_MSG_MONSTER_SAY);
                    TalkWithDelay(7000, TALK_ILLIDAN_WAIT_I_HAVE_AN_IDEA, NULL, CHAT_MSG_MONSTER_SAY);
                    TalkWithDelay(17000, TALK_ILLIDAN_WHAT_OUR_PEOPLE, NULL, CHAT_MSG_MONSTER_SAY);
                    events.ScheduleEvent(EVENT_DRINK, 21000);
                    TalkWithDelay(23000, TALK_ILLIDAN_YES_YES_I_CAN_FEEL);

                    if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    {
                        tyrande->DeleteThreatList();
                        tyrande->AI()->TalkWithDelay(10000, TALK_TYRANDE_ILLIDAN_WHAT_IS_IN, NULL, CHAT_MSG_MONSTER_SAY);
                    }
                    break;
                case 12:
                    me->SetReactState(REACT_PASSIVE);
                    me->GetPosition(&resetPosition);
                    me->SetHomePosition(resetPosition);
                    me->RemoveAurasDueToSpell(SPELL_WATERS_OF_ETERNITY);
                    SetEscortPaused(true);
                    if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
                        mannoroth->AI()->Talk(TALK_MANNOROTH_INTRO);

                    if (auto captain = instance->GetCreature(DATA_CAPTAIN_VAROTHEN))
                        captain->AI()->DoAction(ACTION_START_INTRO);
                    break;
                case 13:
                    me->GetPosition(&resetPosition);
                    me->SetHomePosition(resetPosition);
                    if (auto captain = instance->GetCreature(DATA_CAPTAIN_VAROTHEN))
                        if (!captain->isInCombat())
                            SetEscortPaused(true);
                    break;
                case 14:
                    SetEscortPaused(true);
                    events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 5000);
                    if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
                    {
                        me->SetReactState(REACT_AGGRESSIVE);
                        mannoroth->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                        AttackStart(mannoroth);
                        mannoroth->AI()->AttackStart(me);
                        mannoroth->getThreatManager().addThreat(me, 100000.00f);
                        DoCast(mannoroth, SPELL_DEMON_RUSH, false);
                    }
                    break;
                case 17:
                    SetEscortPaused(true);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            npc_escortAI::UpdateAI(diff);
            events.Update(diff);

            if (!me->HasAura(SPELL_DEMONIC_SIGHT))
                DoCast(me, SPELL_DEMONIC_SIGHT, true);

            /*if (log_timer <= diff)
            {
                if (HasEscortState(STATE_ESCORT_PAUSED))
                    TC_LOG_ERROR("sql, sql", "HasEscortState(STATE_ESCORT_PAUSED)");

                log_timer = 3000;
            }
            else log_timer -= diff;*/

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_DEMONS:
                    {
                        bool aliveGuardFound = false;
                        std::list<Creature*> doomguards;
                        me->GetCreatureListWithEntryInGrid(doomguards, NPC_DOOMGUARD_ANNIHILATOR, 300.0f);
                        for (std::list<Creature*>::iterator itr = doomguards.begin(); itr != doomguards.end(); ++itr)
                        {
                            if ((*itr)->isAlive())
                            {
                                aliveGuardFound = true;
                                break;
                            }
                        }

                        if (!aliveGuardFound || doomguards.empty())
                        {
                            Start(false, true);
                            if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                                tyrande->AI()->DoAction(ACTION_START_ESCORT_EVENT);

                        }
                        else
                            events.ScheduleEvent(EVENT_CHECK_DEMONS, 1000);
                        break;
                    }
                    case EVENT_DRINK:
                        Talk(TALK_ILLIDAN_EMOTE_DRINK, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                        DoCast(me, SPELL_WATERS_OF_ETERNITY, false);
                        events.ScheduleEvent(EVENT_ATTACK_DOOMBRINGER, 2500);
                        break;
                    case EVENT_ATTACK_DOOMBRINGER:
                        if (auto doombringer = instance->GetCreature(DATA_DOOMBRINGER))
                        {
                            doombringer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC);
                            doombringer->SetInCombatWithZone();
                            AttackStart(doombringer);
                        }
                        if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                            tyrande->AI()->DoAction(ACTION_ATTACK_DOOMBRINGER);
                        break;
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
                            ResetCombat();
                        else
                            events.ScheduleEvent(EVENT_PLAYER_ALIVE_CHECK, 1000);
                        break;
                    }
                    case EVENT_AURA_OF_IMMOLATION:
                        DoCast(me, SPELL_AURA_OF_IMMOLATION, false);
                        events.ScheduleEvent(EVENT_AURA_OF_IMMOLATION, 15000);
                        break;
                    case EVENT_TAUNT:
                        DoCastVictim(SPELL_DEMONIC_TAUNT);
                        events.ScheduleEvent(EVENT_TAUNT, urand(5000, 10000));
                        break;
                }
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
        Position resetPosition;
        bool isEventActive;
        bool restartEscortAtHome;
        bool checkWipe{ false };
        //uint32 log_timer = 3000;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_woe_illidan_part_twoAI(creature);
    }
};

class npc_woe_tyrande_whisperwind : public CreatureScript
{
public:
    npc_woe_tyrande_whisperwind() : CreatureScript("npc_woe_tyrande_whisperwind") { }

    struct npc_woe_tyrande_whisperwindAI : public npc_escortAI
    {
        npc_woe_tyrande_whisperwindAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            restartEscortAtHome = true;
            outOfArrows = false;
        }

        bool CanAIAttack(Unit const* target) const override
        {
            return target->GetEntry() != NPC_CAPTAIN_VAROTHEN;
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            float numerator{ float(me->GetHealthPct()) };
            float denominator{ 100.f };
            float dmg = float(damage) * (numerator / denominator);
            damage = (me->HealthBelowPctDamaged(5, damage) ? 0 : uint32(dmg));
        }

        void EnterEvadeMode() override {}

        void JustReachedHome() override
        {
            me->SetWalk(false);
            if (restartEscortAtHome)
                SetEscortPaused(false);
            restartEscortAtHome = true;
        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_START_ESCORT_EVENT:
                    events.CancelEvent(EVENT_BLESSING_OF_ELUNE);
                    events.CancelEventGroup(EVENT_ATTACK_ASSIST_PLAYERS);
                    me->CombatStop(true);
                    me->RemoveAurasDueToSpell(SPELL_BLESSING_OF_ELUNE);
                    SetMaxPlayerDistance(500.0f);
                    SetDespawnAtEnd(false);
                    SetDespawnAtFar(false);
                    Start(false, true);
                    break;
                case ACTION_RESTART_ESCORT:
                    me->GetMotionMaster()->MoveTargetedHome();
                    break;
                case ACTION_ATTACK_DOOMBRINGER:
                    events.ScheduleEvent(EVENT_ATTACK_DOOMBRINGER, 1000);
                    break;
                case ACTION_ATTACK_ASSIST_PLAYERS:
                    events.ScheduleEvent(EVENT_ATTACK_ASSIST_PLAYERS, 1000);
                    break;
                case ACTION_CAST_BLESSING_OF_ELUNE:
                    events.CancelEventGroup(EVENT_ATTACK_ASSIST_PLAYERS);
                    events.ScheduleEvent(EVENT_BLESSING_OF_ELUNE, 30000);
                    break;
                case ACTION_LIGHT_OF_ELUNE:
                    if (!actionStarted)
                    {
                        actionStarted = true;
                        Talk(TALK_TYRANDE_LIGHT_OF_ELUNE_SAVE);
                        DoCast(me, SPELL_BLESSING_OF_ELUNE, false);
                        events.ScheduleEvent(EVENT_SEND_EMOTE, 3000);
                        events.CancelEvent(EVENT_ATTACK_DOOMGUARD);
                    }
                    break;
                case ACTION_RESTART_COMBAT:
                    events.CancelEvent(EVENT_ATTACK_ASSIST_PLAYERS);
                    events.ScheduleEvent(EVENT_ATTACK_DOOMGUARD, 500);
                    me->RemoveAurasDueToSpell(SPELL_BLESSING_OF_ELUNE);
                    Talk(TALK_TYRANDE_HOLD_THEM_BACK_AGAIN, NULL, CHAT_MSG_MONSTER_SAY);
                    Talk(TALK_TYRANDE_EMOTE_CAN_HOLD_HER, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    break;
                case ACTION_RESET_BOSS_FIGHT:
                    events.Reset();
                    restartEscortAtHome = false;
                    me->CombatStop(true);
                    me->RemoveAllAuras();
                    me->m_Events.KillAllEvents(false);
                    SetNextWaypoint(15, false, false);
                    me->SetHomePosition(resetPosition);
                    me->GetMotionMaster()->MoveTargetedHome();
                    outOfArrows = false;
                    break;
                case ACTION_HAND_OF_ELUNE:
                    events.ScheduleEvent(EVENT_HAND_OF_ELUNE, 26000);
                    break;
                case ACTION_COLLAPSE:
                    events.ScheduleEvent(EVENT_COLLAPSE, 22000);
                    break;
                case ACTION_FIGHT_FINISHED:
                    Talk(TALK_TYRANDE_COMBAT_DONE);
                    me->CombatStop(true);
                    me->RemoveAllAuras();
                    events.Reset();
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    break;
                default:
                    break;
            }
        }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
                case 11:
                case 13:
                    me->GetPosition(&resetPosition);
                    me->SetHomePosition(resetPosition);
                    SetEscortPaused(true);
                    break;
                case 17:
                    events.ScheduleEvent(EVENT_ATTACK_DOOMGUARD, 1000);
                    actionStarted = false;
                    SetEscortPaused(true);
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
                case EVENT_ATTACK_DOOMBRINGER:
                    if (auto doombringer = instance->GetCreature(DATA_DOOMBRINGER))
                    {
                        if (doombringer->isAlive())
                        {
                            DoCast(doombringer, SPELL_LUNAR_SHOT_DOOMBRINGER, false);
                            events.ScheduleEvent(EVENT_ATTACK_DOOMBRINGER, 2000);
                        }
                    }
                    break;
                case EVENT_ATTACK_ASSIST_PLAYERS:
                {
                    if (auto target = me->FindNearestCreature(NPC_SHADOWBAT, 40.f, true))
                    {
                        me->SetFacingToObject(target);
                        DoCast(target, SPELL_LUNAR_SHOT_DOOMBRINGER);
                    }
                    else
                        if (auto target = me->FindNearestCreature(NPC_HIGHGUARD_ELITE, 40.f, true))
                        {
                            me->SetFacingToObject(target);
                            DoCast(target, SPELL_LUNAR_SHOT_DOOMBRINGER);
                        }
                    events.ScheduleEvent(EVENT_ATTACK_ASSIST_PLAYERS, 4000);
                    break;
                }
                case EVENT_BLESSING_OF_ELUNE:
                    Talk(TALK_TYRANDE_EMOTE_BLESSING, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    DoCast(me, SPELL_BLESSING_OF_ELUNE, false);
                    break;
                case EVENT_SEND_EMOTE:
                    Talk(TALK_TYRANDE_EMOTE_BLESSING_BOSS, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    break;
                case EVENT_HAND_OF_ELUNE:
                    outOfArrows = true;
                    Talk(TALK_TYRANDE_I_AM_OUT_OF_ARROWS, NULL, CHAT_MSG_MONSTER_SAY);
                    Talk(TALK_TYRANDE_EMOTE_HAND_OF_ELUNE, 0, CHAT_MSG_RAID_BOSS_EMOTE);
                    if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
                        me->SetFacingToObject(mannoroth);
                    DoCast(me, SPELL_HAND_OF_ELUNE, false);
                    break;
                case EVENT_COLLAPSE:
                    me->RemoveAllAuras();
                    me->RemoveAurasDueToSpell(SPELL_HAND_OF_ELUNE);
                    DoCast(me, SPELL_COLLAPSE, true);
                    events.CancelEventGroup(EVENT_ATTACK_ASSIST_PLAYERS);
                    events.CancelEventGroup(EVENT_ATTACK_DOOMBRINGER);
                    events.CancelEventGroup(EVENT_ATTACK_DOOMGUARD);
                    events.Reset();
                    break;
                case EVENT_ATTACK_DOOMGUARD:
                    if (auto doomguard = me->FindNearestCreature(NPC_DOOMGUARD_DEVASTATOR, 45.00f))
                    {
                        if (!outOfArrows)
                        {
                            me->SetFacingToObject(doomguard);
                            DoCast(doomguard, SPELL_LUNAR_SHOT_INSTANT, true);
                        }

                        std::list<Creature*> list_of_npcs;
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_DOOMGUARD_DEVASTATOR, 65.0f);
                        for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end(); ++itr)
                            if (auto c = (*itr))
                            {
                                if (!c->isInCombat() && !c->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                                    if (auto p = c->FindNearestPlayer(65.f))
                                    {
                                        c->AI()->AttackStart(p);
                                        c->AddThreat(p, 100.f);
                                    }
                            }
                    }
                    events.ScheduleEvent(EVENT_ATTACK_DOOMGUARD, 1000);
                    break;
                default:
                    break;
                }
            }
        }

    private:
        EventMap events;
        InstanceScript* instance;
        Position resetPosition;
        bool actionStarted;
        bool restartEscortAtHome;
        bool outOfArrows;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_woe_tyrande_whisperwindAI(creature);
    }
};

class npc_woe_doomguard_annihilator : public CreatureScript
{
public:
    npc_woe_doomguard_annihilator() : CreatureScript("npc_woe_doomguard_annihilator") {}

    struct npc_woe_doomguard_annihilatorAI : public ScriptedAI
    {
        npc_woe_doomguard_annihilatorAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset() override
        {
            me->GetMotionMaster()->MoveRandom(10.f);
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_DONT_CALL_FOR_HELP)
                canCallForHelp = false;
        }

        void EnterCombat(Unit* attacker) override
        {
            if (canCallForHelp)
            {
                std::list<Creature*> doomguards;
                me->GetCreatureListWithEntryInGrid(doomguards, NPC_DOOMGUARD_ANNIHILATOR, 300.0f);
                for (auto itr = doomguards.begin(); itr != doomguards.end(); ++itr)
                {
                    if ((*itr)->isAlive())
                    {
                        (*itr)->AI()->DoAction(ACTION_DONT_CALL_FOR_HELP);
                        (*itr)->AI()->AttackStart(attacker);
                    }
                }
            }
        }

        private:
            InstanceScript* instance;
            bool canCallForHelp{ true };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_doomguard_annihilatorAI(creature);
    }
};

class npc_woe_abyssal_doombringer : public CreatureScript
{
public:
    npc_woe_abyssal_doombringer() : CreatureScript("npc_woe_abyssal_doombringer") {}

    struct npc_woe_abyssal_doombringerAI : public ScriptedAI
    {
        npc_woe_abyssal_doombringerAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_ABYSSAL_FLAMETHROWER, 1000);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void EnterCombat(Unit* attacker) override
        {
            events.CancelEvent(EVENT_ABYSSAL_FLAMETHROWER);
            me->InterruptSpell(CURRENT_CHANNELED_SPELL);
            events.ScheduleEvent(EVENT_ABYSSAL_FLAMES, 2000);
        }

        void JustDied(Unit* killer) override
        {
            if (me->GetEntry() == NPC_ABYSSAL_DOOMBRINGER)
            {
                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                {
                    illidan->AI()->DoAction(ACTION_RESET_EVENTS);
                    illidan->AI()->DoAction(ACTION_RESTART_ESCORT);
                }

                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    tyrande->AI()->DoAction(ACTION_RESTART_ESCORT);

                me->DespawnOrUnsummon(1000);
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_ABYSSAL_FLAMES:
                        DoCast(me, SPELL_ABYSSAL_FLAMES, true);
                        events.ScheduleEvent(EVENT_ABYSSAL_FLAMES, 2000);
                        break;
                    case EVENT_ABYSSAL_FLAMETHROWER:
                        if (auto flameTarget = me->FindNearestCreature(NPC_FLAME_TARGET, 50.00f))
                            DoCast(flameTarget, SPELL_ABYSSAL_FLAMETHROWER, false);
                        events.ScheduleEvent(EVENT_ABYSSAL_FLAMETHROWER, 10000);
                        break;
                    default:
                        break;
                }
            }
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        private:
            EventMap events;
            InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_abyssal_doombringerAI(creature);
    }
};

class npc_woe_captain_varothen : public CreatureScript
{
public:
    npc_woe_captain_varothen() : CreatureScript("npc_woe_captain_varothen") {}

    struct npc_woe_captain_varothenAI : public ScriptedAI
    {
        npc_woe_captain_varothenAI(Creature* creature) : ScriptedAI(creature), summons(me)
        {
            instance = me->GetInstanceScript();
            aliveGuards = 0;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }

        void InitializeAI() override
        {
            for (int8 i = 0; i < 2; i++)
                me->SummonCreature(NPC_HIGHGUARD_ELITE, highguardElitePos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
            ScriptedAI::InitializeAI();
        }

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            events.Reset();
        }

        bool CanAIAttack(Unit const* target) const override
        {
            return target->GetEntry() != NPC_TYRANDE_WHISPERWIND && target->GetEntry() != NPC_ILLIDAN_PART_TWO;
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_START_INTRO)
                events.ScheduleEvent(EVENT_INTRO, 7000);
        }

        void EnterCombat(Unit* attacker) override
        {
            if (instance->GetBossState(DATA_MANNOROTH) != IN_PROGRESS)
            {
                if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                    illidan->AI()->DoAction(ACTION_RESTART_ESCORT);
                if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                    tyrande->AI()->DoAction(ACTION_RESTART_ESCORT);
            }

            Talk(TALK_CAPTAIN_AGGRO);
            me->SetReactState(REACT_AGGRESSIVE);
            events.ScheduleEvent(EVENT_MAGISTRIKE, 10000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            //events.ScheduleEvent(EVENT_BDSM_TYRANDE, 120000);
        }

        void EnterEvadeMode()
        {
            _EnterEvadeMode();
            ScriptedAI::EnterEvadeMode();
        }

        void JustDied(Unit* killer) override
        {
            Talk(TALK_CAPTAIN_DEATH);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            // WORKAROUND - we are out of time... - maybe some know the real issue... 
            // if a unfriedly npc summons the blade the spellclick doesnt work.. we use temp. a friendly trigger npc until the bug is found...
            Position pos(*me);
            if (auto trigger = me->SummonCreature(55091, pos, TEMPSUMMON_TIMED_DESPAWN, 30000))
                trigger->CastSpell(trigger, SPEL_VAROTHENS_MAGICAL_BLADE, true);
        }
        bool CanRespawn() 
        { 
            return respawn;
        }

        void JustSummoned(Creature* summon) override
        {
            summons.Summon(summon);
            switch (summon->GetEntry())
            {
                case NPC_HIGHGUARD_ELITE:
                {
                    aliveGuards++;
                    Position pos(*summon);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    if (auto shadowbat = me->SummonCreature(NPC_SHADOWBAT, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                    {
                        shadowbat->setFaction(me->getFaction());
                        shadowbat->AddAura(103714, shadowbat);
                        shadowbat->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                        summon->CastSpell(shadowbat, SPELL_RIDE_VEHICLE_HARDCODED, true);
                        shadowbat->CastSpell(shadowbat, SPELL_DISPLACEMENT, true);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* killer) override
        {
            switch (summon->GetEntry())
            {
                case NPC_HIGHGUARD_ELITE:
                    if (--aliveGuards <= 0)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                            illidan->AI()->DoAction(ACTION_RESTART_ESCORT);

                        if (auto tyrande = me->FindNearestCreature(NPC_TYRANDE_WHISPERWIND, 100.f, true))
                        {
                            tyrande->DeleteThreatList();
                            tyrande->CombatStop();
                        }
                    }
                    break;
                case NPC_SHADOWBAT:
                    for (uint64 summonGUID : summons)
                    {
                        if (auto summon = ObjectAccessor::GetCreature(*me, summonGUID))
                        {
                            if (summon->GetEntry() == NPC_HIGHGUARD_ELITE)
                            {
                                if (!summon->GetVehicleBase() || (summon->GetVehicleBase() && !summon->GetVehicleBase()->isAlive()))
                                {
                                    summon->SetReactState(REACT_AGGRESSIVE);
                                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                                    summon->SetStandState(UNIT_STAND_STATE_STAND);
                                    if (auto p = me->FindNearestPlayer(100.f))
                                        summon->AI()->AttackStart(p);
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            ScriptedAI::SummonedCreatureDies(summon, killer);
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);

            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                    case EVENT_INTRO:
                        Talk(TALK_CAPTAIN_INTRO);

                        /*
                            Tutorial Phrases
                        */
                        if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                            if (auto illidan = instance->GetCreature(DATA_ILLIDAN_PART_TWO))
                            {
                                tyrande->AI()->TalkWithDelay(8000, 0, NULL, CHAT_MSG_MONSTER_SAY);
                                illidan->TalkWithDelay(11000, 0, NULL, CHAT_MSG_MONSTER_SAY);

                                tyrande->AI()->DoAction(ACTION_ATTACK_ASSIST_PLAYERS);
                            }



                        if (auto p = me->FindNearestPlayer(100.f))
                        for (uint64 summonGUID : summons)
                            if (auto summon = ObjectAccessor::GetCreature(*me, summonGUID))
                                if (summon->GetEntry() == NPC_SHADOWBAT)
                                {
                                    summon->setFaction(me->getFaction());
                                    summon->SetReactState(REACT_AGGRESSIVE);
                                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC);
                                    summon->AI()->AttackStart(p);
                                }
                        break;
                    case EVENT_MAGISTRIKE:
                        DoCastVictim(SPELL_MAGISTRIKE);
                        events.ScheduleEvent(EVENT_MAGISTRIKE, 10000);
                        break;
                    /*case EVENT_BDSM_TYRANDE:
                        if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
                        {
                            mannoroth->CastStop();
                            mannoroth->CastSpell(mannoroth, SPELL_NETHER_PORTAL, true);
                        }
                        events.ScheduleEvent(EVENT_BDSM_TYRANDE, 60000);
                        break;*/
                }
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

    private:
        bool respawn{false};
        EventMap events;
        InstanceScript* instance;
        SummonList summons;
        int8 aliveGuards;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_captain_varothenAI(creature);
    }
};

class npc_woe_fel_flames : public CreatureScript
{
public:
    npc_woe_fel_flames() : CreatureScript("npc_woe_fel_flames") { }

    struct npc_woe_fel_flamesAI : public ScriptedAI
    {
        npc_woe_fel_flamesAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* owner)
        {
            if (auto instance = me->GetInstanceScript())
                if (auto mannoroth = instance->GetCreature(DATA_MANNOROTH))
                    if (owner != mannoroth)
                        mannoroth->AI()->JustSummoned(me);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_fel_flamesAI(creature);
    }
};

class npc_woe_varothens_magical_blade : public CreatureScript
{
public:
    npc_woe_varothens_magical_blade() : CreatureScript("npc_woe_varothens_magical_blade") { }

    struct npc_woe_varothens_magical_bladeAI : public ScriptedAI
    {
        npc_woe_varothens_magical_bladeAI(Creature* creature) : ScriptedAI(creature) 
        {
        }

        void IsSummonedBy(Unit* owner)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            Talk(TALK_EMOTE_SWORD_FALLS_TO_THE_GROUND, 0, CHAT_MSG_RAID_BOSS_EMOTE);
            DoCast(me, SPELL_MAGICAL_BLADE_VISUAL, true);
        }

        void OnSpellClick(Unit* /*clicker*/, bool& /*result*/)
        {
            me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_varothens_magical_bladeAI(creature);
    }
};

class npc_woe_mixed_creatures_ai : public CreatureScript
{
public:
    npc_woe_mixed_creatures_ai() : CreatureScript("npc_woe_mixed_creatures_ai") { }

    struct npc_woe_mixed_creatures_aiAI : public ScriptedAI
    {
        npc_woe_mixed_creatures_aiAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* owner) override
        {
            if (me->GetEntry() == NPC_FELHOUND || me->GetEntry() == NPC_DOOMGUARD || me->GetEntry() == NPC_FELGUARD || me->GetEntry() == NPC_INFERNO)
                DoZoneInCombat();
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (me->GetEntry() == NPC_DOOMGUARD_DEVASTATOR)
            {
                if (type == POINT_MOTION_TYPE && (id == POINT_LANDING || id == EVENT_SUMMON_DOOMGUARD_BOSSEVENT))
                {
                    me->SetDisableGravity(false);
                    me->SetCanFly(false);
                    DoZoneInCombat();
                }


                if (type == POINT_MOTION_TYPE)
                    if (id == EVENT_SUMMON_DOOMGUARD)
                        me->DespawnOrUnsummon();
            }

        }

        void DoAction(int32 const action) override
        {
            switch (action)
            {
                case ACTION_ATTACK_DOOMBRINGER:
                    currentPath = ACTION_ATTACK_DOOMBRINGER;
                    break;
            }
        }
    private:
        uint32 currentPath{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_mixed_creatures_aiAI(creature);
    }
};

class npc_woe_twisting_nether_portal : public CreatureScript
{
public:
    npc_woe_twisting_nether_portal() : CreatureScript("npc_woe_twisting_nether_portal") {}

    struct npc_woe_twisting_nether_portalAI : public ScriptedAI
    {
        npc_woe_twisting_nether_portalAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        void InitializeAI()
        {
            demonSpawn.m_positionZ = demonSpawn.m_positionZ - 5.f;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (me->GetDBTableGUIDLow())
                isCosmetic = false;

            if (!isCosmetic)
            {
                me->SetObjectScale(1.f);
                if (dragon_soul[0] = me->SummonCreature(47242, me->GetPositionX(), me->GetPositionY(), 230.f, 1.38f, TEMPSUMMON_MANUAL_DESPAWN))
                if (dragon_soul[1] = me->SummonCreature(47242, me->GetPositionX(), me->GetPositionY(), 246.07f, 1.38f, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    dragon_soul[0]->AddAura(SPELL_DRAGON_SOUL_BUNNY_COSMETIC, dragon_soul[0]);
                    dragon_soul[0]->SetObjectScale(2.f);
                    me->AddAura(102919, dragon_soul[1]);//cosmetic beam from portal to dragon soul
                    /*
                        dragon_soul[1] will be the center of our cosmetic, whereas[0] will be the actual holder of the aura.
                    */
                }
            }
            if (!isCosmetic)
            {
                events.ScheduleEvent(EVENT_SUMMON_DOOMGUARD, 2000);
                events.ScheduleEvent(EVENT_SUMMON_DOOMGUARD_BOSSEVENT, 1);
                
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            Position Destination { doomguardDevastatorLandingPos[portalChoice] };
            Position Destination2 { doomguardDevastatorLandingPos[0] };
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
            {
                switch (eventid)
                {
                case EVENT_SUMMON_DOOMGUARD:
                    if (CanSpawn)
                    {
                        if (uint32 count = urand(3, (portalChoice == 0 ? 2 : 12)))
                            for (uint32 i = 0; i <= count; ++i)
                                if (portalChoice != 0 || (instance->GetBossState(DATA_MANNOROTH) == IN_PROGRESS))
                                    if (auto demon_x = me->SummonCreature(NPC_DOOMGUARD_DEVASTATOR,
                                        demonSpawn.GetPositionX() + frand(-20.f, 20.f),
                                        demonSpawn.GetPositionY() + frand(-20.f, 20.f),
                                        demonSpawn.GetPositionZ() + frand(-20.f, 5.f),
                                        demonSpawn.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        if (portalChoice == 0)
                                        {
                                            demon_x->GetMotionMaster()->
                                                MovePoint(POINT_LANDING,
                                                    Destination.GetPositionX() + frand(-5.f, 5.f),
                                                    Destination.GetPositionY() + frand(-5.f, 5.f),
                                                    Destination.GetPositionZ() + 2.f,
                                                    Destination.GetOrientation()
                                                );
                                        }
                                        else
                                        {
                                            demon_x->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                            demon_x->SetReactState(REACT_PASSIVE);
                                            demon_x->GetMotionMaster()->
                                                MovePoint(EVENT_SUMMON_DOOMGUARD,
                                                    Destination.GetPositionX() + frand(-10.f, 10.f),
                                                    Destination.GetPositionY() + frand(-10.f, 10.f),
                                                    Destination.GetPositionZ() + frand(-10.f, 10.f),
                                                    ((float(i) / float(count)) * (M_PI * 2.f))
                                                );
                                        }
                                    }

                        portalChoice++;
                        if (portalChoice > 4)
                            portalChoice = 0;

                        //TC_LOG_ERROR("sql.sql", "portal choice: %u", portalChoice);
                        events.ScheduleEvent(EVENT_SUMMON_DOOMGUARD, 3000);
                    }
                    break;
                case EVENT_SUMMON_DOOMGUARD_BOSSEVENT:
                    if (auto tyrande = instance->GetCreature(DATA_TYRANDE_WHISPERWIND))
                        if (tyrande->HasAura(SPELL_HAND_OF_ELUNE))
                            midPortal = false;
                    if (midPortal)
                    {
                        if (auto captain = instance->GetCreature(DATA_CAPTAIN_VAROTHEN))
                            if (auto manno = instance->GetCreature(DATA_MANNOROTH))
                                if (captain->isInCombat() || manno->isInCombat())
                                {
                                    for (uint32 i = 0; i <= 5; ++i)
                                        if (auto demon_x = me->SummonCreature(NPC_DOOMGUARD_DEVASTATOR,
                                            demonSpawn.GetPositionX() + frand(-20.f, 20.f),
                                            demonSpawn.GetPositionY() + frand(-20.f, 20.f),
                                            demonSpawn.GetPositionZ() + frand(0.f, 5.f),
                                            demonSpawn.GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                                        {
                                            demon_x->GetMotionMaster()->MovePoint(EVENT_SUMMON_DOOMGUARD_BOSSEVENT,
                                                Destination2.GetPositionX() + frand(-10.f, 10.f),
                                                Destination2.GetPositionY() + frand(-10.f, 10.f),
                                                Destination2.GetPositionZ(), false);
                                        }
                                }
                        events.ScheduleEvent(EVENT_SUMMON_DOOMGUARD_BOSSEVENT, 15000);
                    }
                    break;
                case EVENT_PULL_DEMONS_INTO_PORTAL:
                {
                    CanSpawn = false;
                    if (auto mannoroth = me->FindNearestCreature(NPC_MANNOROTH, 400.f, true))
                    {
                        Position mannoroth_dest{ 3489.762207f, -5543.303711f, 15.243201f, 4.141446f };
                        if (!pulled_Mannoroth)
                        {
                            mannoroth->AI()->Talk(0);
                            pulled_Mannoroth = true;
                            Position portal(*me);
                            mannoroth->StopMoving();
                            mannoroth->GetMotionMaster()->Clear();

                            if (auto c = mannoroth->SummonCreature(44949, *mannoroth, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                                if (auto v = c->GetVehicleKit())
                                    mannoroth->EnterVehicle(c);
                                c->GetMotionMaster()->ForceMoveJump(mannoroth_dest.m_positionX, mannoroth_dest.m_positionY, mannoroth_dest.m_positionZ, 2.5f, 0.f);
                                c->DespawnOrUnsummon(25000);
                                mannoroth->DespawnOrUnsummon(25000);
                            }


                            //Get rid of 
                            std::list<Creature*> units;
                            GetCreatureListWithEntryInGrid(units, me, NPC_DOOMGUARD_ANNIHILATOR,    500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_DOOMGUARD_DEVASTATOR,     500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_ABYSSAL_DOOMBRINGER,      500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_SHADOWBAT,                500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_DOOMGUARD,                500.0f);
                            GetCreatureListWithEntryInGrid(units, me, 57458,                        500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_DREADLORD_DEBILITATOR,    500.0f);
                            GetCreatureListWithEntryInGrid(units, me, 56002,                        500.0f);
                            GetCreatureListWithEntryInGrid(units, me, NPC_INFERNO,                  500.0f);
                            GetCreatureListWithEntryInGrid(units, me, 56078,                        500.0f);
                            GetCreatureListWithEntryInGrid(units, me, 56073,                        500.0f);
                            GetCreatureListWithEntryInGrid(units, me, 56001,                        500.0f);

                            for (auto itr = units.begin(); itr != units.end(); ++itr)
                                if (auto c = (*itr))
                                {
                                    c->DeleteThreatList();
                                    c->CombatStop();
                                    me->CastWithDelay(0, c, 105335, true);
                                    me->CastWithDelay(urand(250, 650), c, 105335, true);
                                    me->CastWithDelay(urand(1250, 1750), c, 105335, true);
                                    me->CastWithDelay(urand(2250, 2750), c, 105335, true);
                                    me->CastWithDelay(urand(3250, 3750), c, 105335, true);
                                    me->CastWithDelay(urand(3250, 3900), c, 105335, true);
                                    c->GetMotionMaster()->ForceMoveJump(portal.m_positionX, portal.m_positionY, portal.m_positionZ, c->GetDistance(portal.m_positionX, portal.m_positionY, portal.m_positionZ) / 10.f, 20.f);
                                    c->GetMotionMaster()->Clear();
                                    c->DespawnOrUnsummon(4000);
                                }
                        }

                        for (auto i = 0; i <= 5; ++i)
                        {
                            Position trigger = { 0.f, 0.f, 0.f, 0.f };
                            trigger.m_positionX += frand(-3.f, 3.f);
                            trigger.m_positionY += frand(-3.f, 3.f);
                            trigger.m_positionZ += frand(-0.f, 15.f);
                            if (auto bunny = mannoroth->SummonCreature(144951, trigger, TEMPSUMMON_TIMED_DESPAWN, 1000))
                            {
                                bunny->m_movementInfo.t_pos = trigger;
                                bunny->m_movementInfo.flags = (MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_CAN_FLY);
                                bunny->m_movementInfo.t_guid = mannoroth->GetGUID();
                                bunny->SendTeleportPacket(trigger);
                                //bunny->EnterVehicle(mannoroth);
                                //bunny->AddAura(34873, bunny);
                                me->CastWithDelay(urand(0, 650), bunny, 105335, true);
                            }
                        }
                        mannoroth->SetFacingTo(2.68f);
                        mannoroth->AddUnitState(UNIT_STATE_CANNOT_TURN);
                        mannoroth->SetFacingTo(2.68f);
                    }
                    events.ScheduleEvent(EVENT_PULL_DEMONS_INTO_PORTAL, 500);
                }
                    break;
                }
            }
        }

        void Reset() override
        {
        }

        void DoAction(int32 const action) override
        {
            if (action == ACTION_PULL_INTO_PORTAL)
                events.ScheduleEvent(EVENT_PULL_DEMONS_INTO_PORTAL, 1);
        }

        void EnterCombat(Unit* attacker) override
        {
        }

    private:
        bool pulled_Mannoroth{ false };
        uint32 portalChoice{ 0 };
        Position demonSpawn{ (*me) };
        EventMap events;
        Creature* dragon_soul[2]{ nullptr, nullptr };
        InstanceScript* instance;
        bool isCosmetic{ true };
        bool CanSpawn{ true };
        bool midPortal{ true };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_woe_twisting_nether_portalAI(creature);
    }
};

class spell_woe_nether_portal : public SpellScriptLoader
{
public:
    spell_woe_nether_portal() : SpellScriptLoader("spell_woe_nether_portal") { }

    class spell_woe_nether_portal_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_nether_portal_AuraScript);

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto caster = GetCaster())
            {
                Position pos(*GetTarget());
                caster->SummonCreature(NPC_DREADLORD_DEBILITATOR, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000);
            }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_woe_nether_portal_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_woe_nether_portal_AuraScript();
    }
};

class spell_woe_wrath_of_elune : public SpellScriptLoader
{
public:
    spell_woe_wrath_of_elune() : SpellScriptLoader("spell_woe_wrath_of_elune") {}

    class spell_woe_wrath_of_elune_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_wrath_of_elune_SpellScript)

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* target){
                return (target->GetEntry() != NPC_FELGUARD && target->GetEntry() != NPC_FELHOUND && target->GetEntry() != NPC_DOOMGUARD);
            });
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_wrath_of_elune_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_wrath_of_elune_SpellScript();
    }
};

class spell_woe_embedded_blade : public SpellScriptLoader
{
public:
    spell_woe_embedded_blade() : SpellScriptLoader("spell_woe_embedded_blade")
    {}

    class spell_woe_embedded_blade_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_embedded_blade_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (auto caster = eventInfo.GetActor())
                if (auto target = GetTarget())
                {
                    if (target->HasSpellCooldown(GetId()))
                        return false;

                    /*
                        Don't allow illidan to too-quickly solo mannoroth.
                        His procs will taper off at low health.

                        ALSO

                        Youtube recorded time to go between procs start and 26% health outro is 210 seconds.
                        100m health - 26m health = 74 procs
                        74 * 3 seconds = 222 seconds.

                        This will require players to stay engaged or their kill time will increase dramatically, long enough to outlast the demon buff.
                    */

                    if (caster->GetEntry() == NPC_ILLIDAN_PART_TWO)
                        return roll_chance_f((target->GetHealthPct() - 20.f));

                    return true;
                }

            return false;
        }

        void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            if (auto caster = eventInfo.GetActor())
                if (auto target = GetTarget())
                {
                    eventInfo.GetActor()->CastSpell(target, SPELL_MAGISTRIKE_ARC_DAMAGE, true);
                    target->AddSpellCooldown(GetId(), 0, time(NULL) + 3);
                }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_woe_embedded_blade_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_woe_embedded_blade_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_embedded_blade_AuraScript();
    }
};

class spell_woe_magistrike_arc : public SpellScriptLoader
{
public:
    spell_woe_magistrike_arc() : SpellScriptLoader("spell_woe_magistrike_arc") { }

    class spell_woe_magistrike_arc_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_magistrike_arc_SpellScript);

        void DisableDamageEffect(SpellEffIndex effIndex)
        {
            // implemented at spell_woe_embedded_blade
            PreventHitDefaultEffect(effIndex);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_woe_magistrike_arc_SpellScript::DisableDamageEffect, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_magistrike_arc_SpellScript();
    }
};

class spell_woe_aura_of_immolation : public SpellScriptLoader
{
public:
    spell_woe_aura_of_immolation() : SpellScriptLoader("spell_woe_aura_of_immolation") {}

    class spell_woe_aura_of_immolation_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_aura_of_immolation_SpellScript)

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* target)
            {
                if (target->GetEntry() == NPC_FELGUARD || target->GetEntry() == NPC_FELHOUND || target->GetEntry() == NPC_DOOMGUARD
                    || target->GetEntry() == NPC_MANNOROTH || target->GetEntry() == NPC_ABYSSAL_DOOMBRINGER)
                    return false;
                return true;
            });
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_woe_aura_of_immolation_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_aura_of_immolation_SpellScript();
    }
};

class spell_woe_blessing_of_elune : public SpellScriptLoader
{
public:
    spell_woe_blessing_of_elune() : SpellScriptLoader("spell_woe_blessing_of_elune") { }

    class spell_woe_blessing_of_elune_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_blessing_of_elune_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            return !eventInfo.GetActor()->HasSpellCooldown(GetId());
        }

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            if (auto c = eventInfo.GetActionTarget())
            {
                switch (c->GetEntry())
                {
                case NPC_CAPTAIN_VAROTHEN:
                case NPC_MANNOROTH:
                    break;
                default:
                    eventInfo.GetActor()->AddSpellCooldown(GetId(), 0, time(NULL) + 2);
                    eventInfo.GetActor()->CastSpell(eventInfo.GetActionTarget(), SPELL_ELUNES_WRATH, true);
                    break;
                }
            }
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_woe_blessing_of_elune_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_woe_blessing_of_elune_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_woe_blessing_of_elune_AuraScript();
    }
};

class spell_woe_inferno : public SpellScriptLoader
{
public:
    spell_woe_inferno() : SpellScriptLoader("spell_woe_inferno") { }

    class spell_woe_inferno_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_woe_inferno_SpellScript);

        void CheckTarget(SpellEffIndex effIndex)
        {
            if (!GetHitDest())
                return;

            float x, y, z;
            GetHitDest()->GetPosition(x, y, z);
            GetCaster()->SummonCreature(NPC_INFERNO, x, y, z);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_woe_inferno_SpellScript::CheckTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_woe_inferno_SpellScript();
    }
};

class achievement_woe_thats_not_canon : public AchievementCriteriaScript
{
public:
    achievement_woe_thats_not_canon() : AchievementCriteriaScript("achievement_woe_thats_not_canon") { }

    bool OnCheck(Player* /*player*/, Unit* target)
    {
        return target && (target->GetAI()->GetData(DATA_THATS_NOT_CANON) >= 10000000);
    }
};

class npc_WoE_Chromie : public CreatureScript
{
public:
    npc_WoE_Chromie() : CreatureScript("npc_WoE_Chromie") { }

    struct npc_WoE_ChromieAI : public ScriptedAI
    {
        npc_WoE_ChromieAI(Creature* creature) : ScriptedAI(creature)
        {
        }

    };


    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_WoE_ChromieAI(creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        //TC_LOG_ERROR("sql.sql", "player: %u, creature: %u, sender: %u, action: %u");
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();
        if (action == 1)
        {
            player->TeleportTo(1, -8292.610352f, -4573.350098f, -224.837006f, 4.860810f);
        }
        return true;
    }
};

class npc_highguard_shadowbat : public CreatureScript
{
public:
    npc_highguard_shadowbat() : CreatureScript("npc_highguard_shadowbat") { }


    struct npc_highguard_shadowbatAI : public ScriptedAI
    {

        npc_highguard_shadowbatAI(Creature* creature) : ScriptedAI(creature), summons(me)
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
            DoCast(me, SPELL_DISPLACEMENT, true);
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

            EVENT_DISPLACEMENT,

            SPELL_EVENT_COUNT,/*END SPELL EVENTS    - THIS EVENT CAN NEVER BE EXECUTED*/
            EVENT_WIPE_CHECK,

            SPELL_DISPLACEMENT = 103763,
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
            /*EVENT_ID               SPELL_ID                 MIN_REPEAT      MAX_REPEAT  inter_delayed  wait_castnext*/
            {EVENT_DISPLACEMENT,     SPELL_DISPLACEMENT,      30000,          50000,          true,       true},
        };

        void UpdateAI(const uint32 diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_DISPLACEMENT:     Spell_Rotation(eventId);  break;
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
            if (use_melee)
                if (me->isInCombat())
                    if (me->HasReactState(REACT_AGGRESSIVE))
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                            if (UpdateVictim())
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
        return new npc_highguard_shadowbatAI(creature);
    }
};

class spell_woe_gift_of_sargeras_players : public SpellScriptLoader
{
public:
    spell_woe_gift_of_sargeras_players() : SpellScriptLoader("spell_woe_gift_of_sargeras_players") {}

    class spell_woe_gift_of_sargeras_players_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_woe_gift_of_sargeras_players_AuraScript);

        void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto o = GetOwner())
                if (auto t = o->ToUnit())
                    if (t->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
                        PreventDefaultAction();
        }


        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_woe_gift_of_sargeras_players_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_woe_gift_of_sargeras_players_AuraScript();
    }

};

void AddSC_boss_mannoroth_and_varothen()
{
    new boss_woe_mannoroth();
    new npc_woe_illidan_part_two();
    new npc_woe_tyrande_whisperwind();
    new npc_woe_doomguard_annihilator();
    new npc_woe_abyssal_doombringer();
    new npc_woe_captain_varothen();
    new npc_woe_fel_flames();
    new npc_woe_varothens_magical_blade();
    new npc_woe_mixed_creatures_ai();
    new npc_woe_twisting_nether_portal();
    new npc_WoE_Chromie();
    new npc_highguard_shadowbat();
    new spell_woe_nether_portal();
    new spell_woe_wrath_of_elune();
    new spell_woe_embedded_blade();
    new spell_woe_magistrike_arc();
    new spell_woe_aura_of_immolation();
    new spell_woe_blessing_of_elune();
    new spell_woe_inferno();
    new achievement_woe_thats_not_canon();
    new spell_woe_gift_of_sargeras_players();
}
