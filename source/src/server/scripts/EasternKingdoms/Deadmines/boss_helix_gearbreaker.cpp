
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "deadmines.h"

enum Spells
{
    SPELL_CHEST_BOMB        = 88352,
    SPELL_CHEST_BOMB_EXPLOSION        = 88250,
    SPELL_EXPLODE           = 91566,

    SPELL_OAF_GRAB_TARGETING = 88289, // DUMMY bp SPELL_LUMBERING_CHARGE
    SPELL_LUMBERING_CHARGE            = 88288, // trigger SPELL_LUMBERING_FORCE_RIDE
    SPELL_LUMBERING_FORCE_RIDE        = 88278,

    SPELL_LUMBERING_CHARGE_DEST = 88295,

    SPELL_LUMBERING_CATCH_PLAYER = 88277, // cast by script effec
    SPELL_LUMBERING_CRASH_PLAYER = 88300,

    SPELL_HELIX_LUMBERING_PROTECTION = 90546,

    SPELL_HELIX_RIDE_FACE_AURA = 88351,
    SPELL_HELIX_BOMB_EMOTE = 91572,

    SPELL_NIGHTMARE_AURA = 92566,
    SPELL_HELIX_CANCEL_NIGHTMARE_AURA = 92585,

    // helix part


    SPELL_HELIX_LAUNCH_DUMMY = 88126,
    SPELL_HELIX_SET_VEHICLE = 88337,
    SPELL_HELIX_RIDE_FACING_TARGET = 88349, // dummy SPELL_HELIX_RIDE_VEHICLE
    SPELL_HELIX_RIDE_VEHICLE = 88360,
    SPELL_HELIX_RIDE_FACE_TIMER_AURA = 88351,

    SPELL_HELIX_BOMB_TARGET_SELECTOR = 88268, // dummy bp SPELL_HELIX_LAUNCH_BOMB
    SPELL_HELIX_LAUNCH_BOMB = 88264, // trigger SPELL_HELIC_SUMMON_BOMB
    SPELL_HELIX_SUMMON_BOMB = 88265,

    SPELL_HELIX_TALK_EMOTE = 79506,

    SPELL_HELIX_BOMB_ACTIVATION_VISUAL_YELLOW = 88315,
    SPELL_HELIX_BOMB_ACTIVATION_VISUAL_ORANGE = 88316,
    SPELL_HELIX_BOMB_ACTIVATION_VISUAL_RED    = 88317,
    SPELL_BOMB_ACTIVATE                       = 88319,
    SPELL_BOMB_PERIODIC_TRIGGER               = 88329, // trigger SPELL_HELIX_BOMB_EXPLODE
    SPELL_HELIX_BOMB_EXPLODE                  = 88321,
};

enum Events
{
    EVENT_CHEST_BOMB    = 1,
    EVENT_OAF_SMASH     = 2,
    EVENT_CHARGE_PLAYER        = 3,
    EVENT_HELIX_BOMB,
    EVENT_RIDE_PLAYERS,
    EVENT_RIDE_PLAYERS_END,
    EVENT_HELIX_TEAM,
    EVENT_START_CHARGE,
    EVENT_HELIX_TEAM_P2,
    ACTION_OAF_SMASH_START,
    ACTION_OAF_SMASH_FINISH,
};

enum NPCs
{
    NPC_HELIX_CREW          = 49136,
    NPC_LUMBERING           = 47297,
    NPC_HELIX_BOMB          = 47314,
    NPC_MINE_RAT            = 51462,
    NPC_HELIX_TEAM          = 49139,
};

enum Phases
{
    PHASE_LUMBERING = 1,
    PHASE_HELIX = 2,

    POINT_CHARGE_WITH_PLAYER = 3,
    POINT_START_CHARGE = 4,
    ACTION_RIDE_END = 5,
    ACTION_RESTART_COMBAT = 6,
};

static const Position chargeEvent[2] =
{
    { -289.808f, -527.215f, 49.802f, 1.6231f }, // charge start
    { -289.587f, -489.575f, 49.912f, 4.7123f }  // charge dest

};

static const Position Defias_RP_spawns[2] =
{
    {-306.042236f, -603.958130f, 49.764549f, 4.142948f}, //right
    {-303.725159f, -607.868652f, 49.492847f, 3.632442f}  // left
};

class BombActivationEvent : public BasicEvent
{
public:
    BombActivationEvent(Creature* owner, uint8 count) : _owner(owner), _count(count) { }

    bool Execute(uint64 execTime, uint32 /*diff*/) override
    {
        switch (_count)
        {
            case 0:
            case 1:
                _owner->SetHomePosition(*_owner);
                _owner->CastSpell(_owner, SPELL_HELIX_BOMB_ACTIVATION_VISUAL_YELLOW, true);
                break;
            case 2:
            case 3:
                _owner->CastSpell(_owner, SPELL_HELIX_BOMB_ACTIVATION_VISUAL_ORANGE, true);
                break;
            case 4:
            case 5:
                _owner->CastSpell(_owner, SPELL_HELIX_BOMB_ACTIVATION_VISUAL_RED, true);
                break;
            case 6:
                _owner->SetHomePosition(*_owner);
                _owner->CastSpell(_owner, SPELL_BOMB_ACTIVATE, true);
                _owner->CastSpell(_owner, SPELL_BOMB_PERIODIC_TRIGGER, true);
                break;
            case 19:
                _owner->CastSpell(_owner, SPELL_HELIX_BOMB_EXPLODE, true);
                _owner->DespawnOrUnsummon(1000);
                return true;
            default:
                break;
        }

        ++_count;
        _owner->m_Events.AddEvent(this, execTime + 1000);
        return false;
    }

private:
    Creature* _owner;
    uint8 _count;
};

class TeamBombLaunchEvent : public BasicEvent
{
public:
    TeamBombLaunchEvent(Creature* owner) : _owner(owner) { }

    bool Execute(uint64 execTime, uint32 /*diff*/) override
    {
        if (_owner && _owner->isAlive())
        {
            if (urand(0, 4) == 4)
                _owner->AI()->Talk(0);
            _owner->CastSpell(_owner, SPELL_HELIX_BOMB_TARGET_SELECTOR, true);
            _owner->m_Events.AddEvent(this, execTime + urand(8000, 15000));
        }
        return false;
    }

private:
    Creature* _owner;
};

class boss_helix_gearbreaker : public CreatureScript
{
public:
    boss_helix_gearbreaker() : CreatureScript("boss_helix_gearbreaker") { }

    struct boss_helix_gearbreakerAI : public BossAI
    {
        boss_helix_gearbreakerAI(Creature* creature) : BossAI(creature, BOSS_HELIX_DATA) { }

        void Reset()
        {
            targetGUID = 0;
            isOnPlayer = false;
            _Reset();
            InitVehicle();
        }

        void InitVehicle()
        {
            if (Vehicle *veh = me->GetVehicle())
                if (Unit *baseVeh = veh->GetBase())
                    if (baseVeh->GetTypeId() == TYPEID_UNIT)
                        baseVeh->ToCreature()->DespawnOrUnsummon();
            if (Creature *lumbering = me->SummonCreature(NPC_LUMBERING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                me->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, lumbering, false);
        }

        void EnterCombat(Unit* who)
        {
            Talk(0);
            if (Vehicle *veh = me->GetVehicle())
                if (Unit *baseVeh = veh->GetBase())
                    if (baseVeh->GetTypeId() == TYPEID_UNIT)
                    {
                        baseVeh->ToCreature()->SetReactState(REACT_AGGRESSIVE);
                        baseVeh->ToCreature()->SetInCombatWithZone();
                    }
            events.SetPhase(PHASE_LUMBERING);
            events.ScheduleEvent(EVENT_HELIX_BOMB, 5000, 0, PHASE_LUMBERING);
            events.ScheduleEvent(EVENT_CHARGE_PLAYER, 15000, 0, PHASE_LUMBERING);
            events.ScheduleEvent(EVENT_RIDE_PLAYERS, 25000, 0, PHASE_LUMBERING);
            if (IsHeroic())
                events.ScheduleEvent(EVENT_HELIX_TEAM, 5000);
            _EnterCombat();

            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
            {
                if (auto player = i->getSource())
                    player->ResetAchievementCriteriaByEntry(16209);
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            if (summon->GetEntry() == NPC_LUMBERING)
            {
                Talk(2);
                events.SetPhase(PHASE_HELIX);
                events.ScheduleEvent(EVENT_RIDE_PLAYERS, 5000, 0, PHASE_HELIX);
                events.ScheduleEvent(EVENT_HELIX_TEAM_P2, 1000);
                me->RemoveAurasDueToSpell(VEHICLE_SPELL_RIDE_HARDCODED);
                summon->RemoveAurasDueToSpell(VEHICLE_SPELL_RIDE_HARDCODED);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            me->ExitVehicle();
            Talk(4);
            //instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HELIX_SET_VEHICLE);
            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
            {
                if (auto player = i->getSource())
                {
                    if (player->GetVehicleKit())
                    {
                        player->RemoveVehicleKit();
                        WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, player->GetPackGUID().size() + 4);
                        data.appendPackGUID(player->GetGUID());
                        data << uint32(0);
                        player->SendMessageToSet(&data, true);
                    }
                }
            }
            _JustDied();

            if (auto Defias1 = me->SummonCreature(48421, Defias_RP_spawns[0]))
                Defias1->SetDisplayId(2317);
        }

        void KilledUnit(Unit* victim)
        {

        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (events.IsInPhase(PHASE_LUMBERING))
                damage = 0;
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {

        }

        void DoMeleeAttackIfReady()
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            Unit* victim;
            if (isOnPlayer && me->GetVehicle())
                victim = me->GetVehicle()->GetBase();
            else victim = me->getVictim();
                
            //Make sure our attack is ready and we aren't currently casting before checking distance
            if (me->isAttackReady() && me->IsWithinMeleeRange(victim))
            {
                me->AttackerStateUpdate(victim);
                me->resetAttackTimer();
            }
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_RIDE_END:
                {
                    if (me->GetVehicle())
                        if (me->GetVehicle()->GetBase())
                        {
                            targetGUID = me->GetVehicle()->GetBase()->GetGUID();
                            if (auto player = ObjectAccessor::GetPlayer(*me, targetGUID))
                            {
                                player->RemoveVehicleKit();
                                WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, player->GetPackGUID().size() + 4);
                                data.appendPackGUID(player->GetGUID());
                                data << uint32(0);
                                player->SendMessageToSet(&data, true);
                            }
                        }
                            
                    me->ExitVehicle();
                    if (events.IsInPhase(PHASE_LUMBERING))
                        if (Creature *lumbering = me->FindNearestCreature(NPC_LUMBERING, 200, true))
                            me->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, lumbering, false);
                    if (IsHeroic())
                        events.ScheduleEvent(EVENT_CHEST_BOMB, 100);
                    break;
                }
                case ACTION_OAF_SMASH_START:
                    break;
                case ACTION_OAF_SMASH_FINISH:
                    break;
            }
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_HELIX_BOMB:
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_DISABLE_MOVE);
                    summon->SetReactState(REACT_PASSIVE);
                    break;
                case NPC_HELIX_TEAM:
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_DISABLE_MOVE);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->m_Events.AddEvent(new TeamBombLaunchEvent(summon), summon->m_Events.CalculateTime(urand(10000, 15000)));
                    break;
            }
            BossAI::JustSummoned(summon);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
        }


        void SummonedMovementInform(Creature* summon, uint32 type, uint32 id) 
        {
            if (summon->GetEntry() == NPC_HELIX_BOMB)
            if (type == EFFECT_MOTION_TYPE)
                if (id == 1004)//bomb just landed
                {
                    summon->m_Events.AddEvent(new BombActivationEvent(summon, 0), summon->m_Events.CalculateTime(1));
                    summon->GetMotionMaster()->MoveFall();
                }
        }

        void SetData(uint32 id, uint32 value)
        {
            isOnPlayer = true;
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
                    case EVENT_HELIX_BOMB:
                    {

                        Position pos(*me);
                        pos.m_positionZ += 3.f;

                        Talk(1);
                        DoCast(me, SPELL_HELIX_BOMB_TARGET_SELECTOR, true);
                        if (auto bomb = me->SummonCreature(NPC_HELIX_BOMB, pos))
                        {
                            me->MoveBlink(pos, frand(3, 15), 0.f);
                            bomb->GetMotionMaster()->ForceMoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 7.f, 13.f);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);
                        }
                        events.ScheduleEvent(EVENT_HELIX_BOMB, 10000, 0, PHASE_LUMBERING);
                    }
                        break;
                    case EVENT_CHEST_BOMB:
                    {
                        if (Unit *target = Unit::GetUnit(*me, targetGUID))
                        {
                            Talk(3, targetGUID);
                            me->CastSpell(target, SPELL_CHEST_BOMB, true);
                        }
                        break;
                    }
                    case EVENT_OAF_SMASH:
                        break;
                    case EVENT_CHARGE_PLAYER:
                        DoCastRandom(SPELL_OAF_GRAB_TARGETING, 150.0f, false);
                        events.ScheduleEvent(EVENT_CHARGE_PLAYER, 20000);
                        break;
                    case EVENT_RIDE_PLAYERS:
                        if (auto v = me->getVictim())
                        {
                            DoCastAOE(SPELL_HELIX_SET_VEHICLE); 
                            DoCast(SPELL_HELIX_RIDE_FACING_TARGET);
                        }                   
                        if (events.IsInPhase(PHASE_LUMBERING))
                            events.ScheduleEvent(EVENT_RIDE_PLAYERS, 60000, 0, PHASE_LUMBERING);
                        else
                            events.ScheduleEvent(EVENT_RIDE_PLAYERS, 12000, 0, PHASE_HELIX);
                        events.ScheduleEvent(EVENT_RIDE_PLAYERS_END, 10000);
                        break;
                    case EVENT_RIDE_PLAYERS_END:
                        isOnPlayer = false;
                        break;
                    case EVENT_HELIX_TEAM:
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[0]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[2]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[5]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[7]);
                        break;
                    case EVENT_HELIX_TEAM_P2:
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[1]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[3]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[4]);
                        me->SummonCreature(NPC_HELIX_TEAM, crew_spawn[6]);
                        break;
                }
            }

            if (!events.IsInPhase(PHASE_LUMBERING) || isOnPlayer)
                DoMeleeAttackIfReady();
        }

    private:
        uint64 targetGUID;
        bool isOnPlayer;
        bool oafSmashing{ false };
        const Position crew_spawn[8]
        { 
            {-294.274353f, -502.676239f, 59.833775f, 1.562904f},
            {-291.065887f, -502.671448f, 59.994808f, 1.590406f},
            {-288.212280f, -502.808777f, 60.251312f, 1.574699f},
            {-282.953522f, -502.886871f, 60.365276f, 1.586480f},
            {-293.734833f, -503.986755f, 60.510422f, 4.673071f},
            {-290.442780f, -504.452393f, 60.065411f, 4.673071f},
            {-287.637451f, -504.562744f, 60.191872f, 4.673071f},
            {-283.438202f, -504.727936f, 60.226803f, 4.673071f},
        };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_helix_gearbreakerAI(creature);
    }
};

class boss_helix_lumbering : public CreatureScript
{
public:
    boss_helix_lumbering() : CreatureScript("boss_helix_lumbering") { }

    struct boss_helix_lumberingAI : public ScriptedAI
    {
        boss_helix_lumberingAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset()
        {
            me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            me->SetReactState(REACT_AGGRESSIVE);
            _events.Reset();
        }

        void EnterEvadeMode()
        {
            if (me->GetVehicleKit())
                me->GetVehicleKit()->RemoveAllPassengers();
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;
            if (id == POINT_START_CHARGE)
                _events.ScheduleEvent(EVENT_START_CHARGE, 1);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_RESTART_COMBAT:
                {
                    if (me->getVictim())
                    {
                        DoStartMovement(me->getVictim());
                        for (int i = 0; i < 5; i++)
                            if (auto rat = me->SummonCreature(NPC_MINE_RAT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                                rat->AI()->AttackStart(me->getVictim());
                    }
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            _events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_START_CHARGE:
                        DoCast(SPELL_LUMBERING_CHARGE_DEST);
                        break;
                    default:
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }

    private:
        EventMap _events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_helix_lumberingAI(creature);
    }
};

class BombTargetSelector
{
public:
    explicit BombTargetSelector(Unit* unit) : _oaf(unit){};

    bool operator()(WorldObject* target) const
    {
        if (_oaf && _oaf->getVictim() == target) // phase 1 check
            return true;
        return !target->ToUnit() || target->ToUnit()->HasAura(SPELL_LUMBERING_CATCH_PLAYER);
    }

private:
    Unit const* _oaf;
};

// 88268
class spell_helix_bomb_dummy : public SpellScriptLoader
{
public:
    spell_helix_bomb_dummy() : SpellScriptLoader("spell_helix_bomb_dummy") { }

    class spell_helix_bomb_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_helix_bomb_dummy_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (Unit *caster = GetCaster())
            {
                Unit *oaf = NULL;
                if (Vehicle *veh = caster->GetVehicle())
                    oaf = veh->GetBase();
                targets.remove_if(BombTargetSelector(oaf));
            }
            if (!targets.empty())
                Trinity::Containers::RandomResizeList(targets, 1);
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (auto me = GetCaster())
                if (auto helix = me->FindNearestCreature(NPC_HELIX, 200.f, true))
                {
                    auto list_of_players = helix->GetPlayersInRange(40.f, true);
                    if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                    {
                        Position pos(*me);
                        Position pos2;
                        pos.m_positionZ += 0.3f;
                        if (auto bomb = helix->SummonCreature(NPC_HELIX_BOMB, pos))
                        {
                            bool force_west = ((me->GetOrientation() < M_PI) && (me->GetAngle(random_target) > M_PI));
                            bool force_east = ((me->GetOrientation() > M_PI) && (me->GetAngle(random_target) < M_PI));

                            if (force_west)
                            {
                                //this npc is going to look fugly if it targets someone on the other side.
                                if (auto west_trigger = me->FindNearestCreature(45979, 50.f, true))
                                {
                                    pos2 = (*west_trigger);
                                    west_trigger->MoveBlink(pos2, frand(0.f, 13.f), frand(0.f - M_PI * 0.5f, M_PI * 0.5f));
                                }
                            }
                            else if (force_east)
                            {
                                if (auto west_trigger = me->FindNearestCreature(45979, 50.f, true))
                                {
                                    pos2 = (*west_trigger);
                                    west_trigger->MoveBlink(pos2, frand(17.f, 45.f), frand(0.f - M_PI * 0.2f, M_PI * 0.2f));
                                }
                            }
                            else
                            {
                                pos2 = (*random_target);
                                random_target->MoveBlink(pos2, frand(0.f, 15.f), frand(0.f, M_PI * 2.f));
                            }


                            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_THROWN);
                            bomb->GetMotionMaster()->ForceMoveJump(pos2.GetPositionX(), pos2.GetPositionY(), pos2.GetPositionZ(), 7.f, 15.f);
                        }
                    }
                }

        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_helix_bomb_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_helix_bomb_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_helix_bomb_dummy_SpellScript();
    }
};

// 88349
class spell_helix_ride_face_dummy : public SpellScriptLoader
{
public:
    spell_helix_ride_face_dummy() : SpellScriptLoader("spell_helix_ride_face_dummy") { }

    class spell_helix_ride_face_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_helix_ride_face_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (Unit *caster = GetCaster())
            {
                Unit *oaf = NULL;
                if (Vehicle *veh = caster->GetVehicle())
                    oaf = veh->GetBase();
                targets.remove_if(BombTargetSelector(oaf));
            }
            if (!targets.empty())
                Trinity::Containers::RandomResizeList(targets, 1);
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                {
                    target->CreateVehicleKit(1263, 0);

                    WorldPacket data(SMSG_PLAYER_VEHICLE_DATA, target->GetPackGUID().size() + 4);
                    data.appendPackGUID(target->GetGUID());
                    data << uint32(1263);
                    target->SendMessageToSet(&data, true);
                    target->ToPlayer()->SendOnCancelExpectedVehicleRideAura();

                    caster->CastSpell(target, SPELL_HELIX_RIDE_FACE_TIMER_AURA, true);
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    caster->CastSpell(target, spellId, true);
                    GetCaster()->GetAI()->SetData(0, 0);
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_helix_ride_face_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_helix_ride_face_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_helix_ride_face_dummy_SpellScript();
    }
};

// 88351
class spell_helix_ride_timer : public SpellScriptLoader
{
public:
    spell_helix_ride_timer() : SpellScriptLoader("spell_helix_ride_timer") { }

    class spell_helix_ride_timer_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_helix_ride_timer_AuraScript);

        void Remove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit *caster = GetCaster())
                if (caster->GetTypeId() == TYPEID_UNIT)
                    caster->ToCreature()->AI()->DoAction(ACTION_RIDE_END);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_helix_ride_timer_AuraScript::Remove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_helix_ride_timer_AuraScript();
    }
};

// 88352
class spell_chess_bomb_periodic : public SpellScriptLoader
{
public:
    spell_chess_bomb_periodic() : SpellScriptLoader("spell_chess_bomb_periodic") { }

    class spell_chess_bomb_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_chess_bomb_periodic_AuraScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleEffectPeriodic(AuraEffect const* aurEff)
        {
            if (auto target = GetUnitOwner())
                target->CastSpell(target, SPELL_CHEST_BOMB_EXPLOSION, true);
        }


        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_chess_bomb_periodic_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_chess_bomb_periodic_AuraScript();
    }
};

// 88289
class spell_oaf_grab_targeting : public SpellScriptLoader
{
public:
    spell_oaf_grab_targeting() : SpellScriptLoader("spell_oaf_grab_targeting") { }

    class spell_oaf_grab_targeting_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_oaf_grab_targeting_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (Unit *caster = GetCaster())
            {
                Unit *oaf = NULL;
                if (Vehicle *veh = caster->GetVehicle())
                    oaf = veh->GetBase();
                targets.remove_if(BombTargetSelector(oaf));
            }
            if (!targets.empty())
                Trinity::Containers::RandomResizeList(targets, 1);
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                    if (Vehicle *veh = caster->GetVehicle())
                        if (Unit *oaf = veh->GetBase())
                        {
                            if (oaf->ToCreature())
                                oaf->ToCreature()->AI()->Talk(0);
                            uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                            oaf->CastSpell(target, spellId, true);
                        }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_oaf_grab_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_oaf_grab_targeting_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_oaf_grab_targeting_SpellScript();
    }
};

// 88278
class spell_oaf_force_players_ride : public SpellScriptLoader
{
public:
    spell_oaf_force_players_ride() : SpellScriptLoader("spell_oaf_force_players_ride") { }

    class spell_oaf_force_players_ride_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_oaf_force_players_ride_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                {
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    target->CastSpell(caster, spellId, true);
                    if (Creature *oaf = caster->ToCreature())
                        oaf->GetMotionMaster()->MovePoint(POINT_START_CHARGE, -290.09f, -530.12f, 49.60f);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_oaf_force_players_ride_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_oaf_force_players_ride_SpellScript();
    }
};

// 88300
class spell_oaf_crash_players : public SpellScriptLoader
{
public:
    spell_oaf_crash_players() : SpellScriptLoader("spell_oaf_crash_players") { }

    class spell_oaf_crash_players_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_oaf_crash_players_SpellScript);

        void HandleAfterHit()
        {
            if (Unit *caster = GetCaster())
                if (caster->GetTypeId() == TYPEID_UNIT)
                    caster->ToCreature()->AI()->DoAction(ACTION_RESTART_COMBAT);
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_oaf_crash_players_SpellScript::HandleAfterHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_oaf_crash_players_SpellScript();
    }
};

class spell_oaf_bomb_trigger_aura : public SpellScriptLoader
{
public:
    spell_oaf_bomb_trigger_aura() : SpellScriptLoader("spell_oaf_bomb_trigger_aura") { }

    class spell_oaf_bomb_trigger_aura_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_oaf_bomb_trigger_aura_AuraScript);

        bool Load()
        {
            hit = false;
            return true;
        }

        void HandlePeriodic(AuraEffect const* /*aurEff*/)
        {
            if (Unit *caster = GetCaster())
            {
                if (!caster->FindNearestPlayer(1) || hit)
                    PreventDefaultAction();
                else if (caster->GetTypeId() == TYPEID_UNIT)
                {
                    hit = true;
                    caster->ToCreature()->DespawnOrUnsummon(1000);
                }
            }
        }

    private :
        bool hit;

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_oaf_bomb_trigger_aura_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_oaf_bomb_trigger_aura_AuraScript();
    }
};

// 47314
class npc_helix_bomb : public CreatureScript
{
public:
    npc_helix_bomb() : CreatureScript("npc_helix_bomb") {}

    struct npc_helix_bombAI : public ScriptedAI
    {
        npc_helix_bombAI(Creature * creature) : ScriptedAI(creature)
        {
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == EFFECT_MOTION_TYPE || type == POINT_MOTION_TYPE)
            {
                if (id == 1004)//bomb just landed
                    if (auto helix = me->FindNearestCreature(NPC_HELIX, 200.f, true))
                        helix->AI()->SummonedMovementInform(me, EFFECT_MOTION_TYPE, id);
            }
        }

        void InitializeAI()
        {
            if (instance)
                if (auto helix = me->FindNearestCreature(NPC_HELIX, 200.f, true))
                    helix->AI()->JustSummoned(me);
        }

    private:
        InstanceScript* instance{ instance = me->GetInstanceScript() };
    };

    CreatureAI * GetAI(Creature * creature) const
    {
        return new npc_helix_bombAI(creature);
    }
};

class achievement_rat_pack : public AchievementCriteriaScript
{
public:
    achievement_rat_pack() : AchievementCriteriaScript("achievement_rat_pack") {}

    bool OnCheck(Player* /*player*/, Unit* target)
    {
        InstanceScript* instance = target->GetInstanceScript();
        return target && (target->GetEntry() == NPC_MINE_RAT) && (instance->GetBossState(BOSS_HELIX_DATA) == IN_PROGRESS);
    }
};

enum Defias_OverSeer_RPING
{
    EVENT_LINE_0 = 1,
    EVENT_LINE_1,
    EVENT_LINE_2,
    EVENT_LINE_3,
    EVENT_LINE_4,
    EVENT_LINE_5,
    EVENT_LINE_6,
    EVENT_LINE_7,
    EVENT_LINE_8,
    EVENT_LINE_9,
};

class npc_Defias_OverSeer_RP : public CreatureScript
{
public:
    npc_Defias_OverSeer_RP() : CreatureScript("npc_Defias_OverSeer_RP") { }

    struct npc_Defias_OverSeer_RPAI : public ScriptedAI
    {
        bool isRPLeader{ false };
        bool rpInitiated{ false };
        Creature* Defias2 {nullptr};

        npc_Defias_OverSeer_RPAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
        }

        void IsSummonedBy(Unit* owner)
        {
            if (owner->GetEntry() == NPC_HELIX)
            {
                isRPLeader = true;
                Defias2 = me->SummonCreature(me->GetEntry(), Defias_RP_spawns[1]);
                if (Defias2)
                {
                    Defias2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    Defias2->SetDisplayId(2317);
                    Defias2->SetReactState(REACT_DEFENSIVE);
                }
                me->SetReactState(REACT_DEFENSIVE);
            }
        }
        void MoveInLineOfSight(Unit* who) 
        {
            if (who->ToPlayer())
                if (isRPLeader)
                    if (!rpInitiated)
                    {
                        events.ScheduleEvent(EVENT_LINE_0, 2000);
                        rpInitiated = true;
                    }
        }

        void EnterCombat(Unit* who)
        {
            events.Reset();
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_LINE_0:
                    Talk(0);
                    events.ScheduleEvent(EVENT_LINE_1, 5000);
                    break;

                case EVENT_LINE_1:
                    if (Defias2)
                        Defias2->AI()->Talk(1);
                    events.ScheduleEvent(EVENT_LINE_2, 5000);
                    break;

                case EVENT_LINE_2:
                    Talk(2);
                    events.ScheduleEvent(EVENT_LINE_3, 5000);
                    break;

                case EVENT_LINE_3:
                    if (Defias2)
                        Defias2->AI()->Talk(3);
                    events.ScheduleEvent(EVENT_LINE_4, 5000);
                    break;

                case EVENT_LINE_4:
                    Talk(4);
                    events.ScheduleEvent(EVENT_LINE_5, 5000);
                    break;

                case EVENT_LINE_5:
                    if (Defias2)
                        Defias2->AI()->Talk(5);
                    events.ScheduleEvent(EVENT_LINE_6, 5000);
                    break;

                case EVENT_LINE_6:
                    Talk(6);
                    events.ScheduleEvent(EVENT_LINE_7, 5000);
                    break;

                case EVENT_LINE_7:
                    if (Defias2)
                        Defias2->AI()->Talk(7);
                    events.ScheduleEvent(EVENT_LINE_8, 5000);
                    break;

                case EVENT_LINE_8:
                    Talk(8);
                    events.ScheduleEvent(EVENT_LINE_9, 5000);
                    break;
                case EVENT_LINE_9:
                    if (Defias2)
                    {
                        Defias2->AI()->Talk(9);
                        Defias2->SetReactState(REACT_AGGRESSIVE);
                    }
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
                default:
                    break;
                }
            if (UpdateVictim())
                DoMeleeAttackIfReady();
            }

    private:
        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Defias_OverSeer_RPAI(creature);
    }
};

void AddSC_boss_helix_gearbreaker()
{
    new boss_helix_gearbreaker();
    new boss_helix_lumbering();
    new spell_helix_bomb_dummy();
    new spell_helix_ride_face_dummy();
    new spell_helix_ride_timer();
    new spell_chess_bomb_periodic();
    new spell_oaf_grab_targeting();
    new spell_oaf_force_players_ride();
    new spell_oaf_crash_players();
    new spell_oaf_bomb_trigger_aura();
    new npc_helix_bomb();
    new achievement_rat_pack();
    new npc_Defias_OverSeer_RP();
}
