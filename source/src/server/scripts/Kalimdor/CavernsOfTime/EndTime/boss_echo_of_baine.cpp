#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "SpellScript.h"
#include "MoveSplineInit.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "endTime.h"

enum Texts
{
    TALK_ENTER_COMBAT = 0,
    TALK_INTRO        = 1, // What dark horrors have you wrought in this place? By my ancestors
    TALK_PLAYER_DIE   = 2,
    TALK_LAUNCH_TOTEM = 4,
    TALK_PULVERIZE    = 5,
    TALK_UNK2         = 6, // My wrath knows no bounds
    TALK_UNK3         = 7, // You wish to train? YOU!?
    TALK_DEATH        = 8,
};

enum Spells
{
    SPELL_LAUNCH_TOTEM_DUMMY         = 101613,
    SPELL_LAUNCH_TOTEM               = 101615,
    SPELL_SUMMON_LAUNCHED_TOTEM      = 101614,
    SPELL_MOLTEN_AXE_PERIODIC_DUMMY  = 101834,
    SPELL_MOLTEN_AXE                 = 101836,
    SPELL_MOLTEN_BLAST               = 101840,
    SPELL_MOLTEN_FISTS_PERIODIC      = 101865,
    SPELL_MOLTEN_FISTS_PROC_AURA     = 101866,
    SPELL_PULVERIZE_DUMMY            = 101625,
    SPELL_PULVERIZE                  = 101626,
    SPELL_PULVERIZE_1                = 101627,
    SPELL_PULVERIZE_DESTROY_PLATFORM = 101815,
    AURA_IN_LAVA                     = 101619,
    // NPC_BAINE_TOTEM
    SPELL_BAINE_TOTAM_AURA           = 101594,
    // players ?
    SPELL_TOTEM_BACK_PERIODIC        = 107837,
    SPELL_TOTEM_BACK                 = 101601,
};

enum Events
{
    EVENT_LAUNCH_TOTEM = 1,
    EVENT_PULVERIZE,
    EVENT_MOLTEN_AXE,
    EVENT_WIPE_CHECK,
    EVENT_ENABLE_COMBAT,
};

enum Npcs
{
    NPC_BAINE_TOTEM   = 54434,
    NPC_BAINE_TOTEM_2 = 54433, // wtf is this for ? cast veh hardcoded on NPC_BAINE_TOTEM
    NPC_ROCK_ISLAND   = 54496,
};

enum Gobs
{
    PLATFORM_EAST   = 209670,//EAST
    PLATFORM_SOUTH  = 209695,//SOUTH
    PLATFORM_WEST   = 209694,//WEST
    PLATFORM_NORTH  = 209693,//NORTH
};

class boss_echo_of_baine : public CreatureScript
{
public:
    boss_echo_of_baine() : CreatureScript("boss_echo_of_baine") { }
    struct boss_echo_of_baineAI : public BossAI
    {
        boss_echo_of_baineAI(Creature* creature) : BossAI(creature, BOSS_ECHO_OF_BAINE)
        {
            introDone = false;
            instance = me->GetInstanceScript();
            creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
            creature->SetReactState(REACT_PASSIVE);
        }

        InstanceScript* instance;

        void Reset()
        {
            SetWalls(false);
            me->RemoveAurasDueToSpell(SPELL_MOLTEN_AXE_PERIODIC_DUMMY);
            events.Reset();

            if (instance) // Open the doors.
            {
                instance->SetBossState(BOSS_ECHO_OF_BAINE, NOT_STARTED);
            }
            RebuiltPlatforms();
            me->DespawnCreaturesInArea(NPC_BAINE_TOTEM);
            if (introDone)
                me->SetReactState(REACT_AGGRESSIVE);
        }

        void EnterEvadeMode() override
        {
            me->GetMotionMaster()->MoveTargetedHome();
            RebuiltPlatforms();
            BossAI::EnterEvadeMode();
            me->SetHealth(me->GetMaxHealth());
            Reset();
            if (instance)
            {
                instance->SetBossState(BOSS_ECHO_OF_BAINE, FAIL);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
            }
        }

        void SetWalls(bool active)
        {
                //despawn stalactites
                std::list<GameObject*> objs;
                GetGameObjectListWithEntryInGrid(objs, me, 209990, 200.f);
                if (objs.size())
                    for (auto itr = objs.begin(); itr != objs.end(); ++itr)
                        if (auto rock = (*itr))
                        {
                            rock->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            rock->SetGoState(active ? GO_STATE_READY : GO_STATE_ACTIVE);
                        }
        }


        void MoveInLineOfSight(Unit* who)
        {
            if (!who->ToPlayer())
                return;

            if (introDone)
            {
                if (me->isAlive())
                    if (!me->isInCombat())
                        if (me->IsWithinDistInMap(who, 15.0f, false))
                            AttackStart(who);
            }
            else
            {
                if (me->isAlive())
                    if (!me->isInCombat())
                        if (me->IsWithinDistInMap(who, 40.0f, false))
                        {
                            if (!events.HasEvent(EVENT_ENABLE_COMBAT) && !introDone)
                            {
                                Talk(TALK_INTRO);
                                events.ScheduleEvent(EVENT_ENABLE_COMBAT, 7000);
                            }
                        }
            }
        }

        void JustDied(Unit* killer)
        {
            SetWalls(false);
            auto players = me->GetPlayersInRange(100.f, true);
            if (players.size())
                for (auto p : players)
                {
                    if (p->GetDistance(me) <= 100.f)
                    {
                        p->CastSpell(p, 109289, true);
                        p->CastSpell(me, 109291, true);
                    }
                };

            
            Talk(TALK_DEATH);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOLTEN_FISTS_PERIODIC);
            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MOLTEN_FISTS_PROC_AURA);
            _JustDied();
            me->DespawnCreaturesInArea(NPC_BAINE_TOTEM);
        }

        void EnterCombat(Unit* who)
        {
            SetWalls(true);
            Talk(TALK_ENTER_COMBAT);
            events.ScheduleEvent(EVENT_LAUNCH_TOTEM, 10000);
            events.ScheduleEvent(EVENT_PULVERIZE, 30000);
            events.ScheduleEvent(EVENT_MOLTEN_AXE, 3000);
            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);
            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                if (auto player = itr->getSource())
                    if (player->GetAreaId() == 5792)
                        player->AddAura(SPELL_MOLTEN_FISTS_PERIODIC, player);

            if (!me->HasAura(SPELL_MOLTEN_AXE_PERIODIC_DUMMY))
                me->AddAura(SPELL_MOLTEN_AXE_PERIODIC_DUMMY, me);
            _EnterCombat();
        }

        void OnSpellCastFinished(SpellInfo const* spell, SpellFinishReason /*reason*/) 
        { 
            if (spell->Id == SPELL_PULVERIZE_DESTROY_PLATFORM)
            {
                if (auto east    = me->FindNearestGameObject(PLATFORM_EAST, 200.0f))
                    UpdatePlatformTriggers(east->GetEntry(), east->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED);
                if (auto south   = me->FindNearestGameObject(PLATFORM_SOUTH, 200.0f))
                    UpdatePlatformTriggers(south->GetEntry(), south->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED);
                if (auto west    = me->FindNearestGameObject(PLATFORM_WEST, 200.0f))
                    UpdatePlatformTriggers(west->GetEntry(), west->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED);
                if (auto north   = me->FindNearestGameObject(PLATFORM_NORTH, 200.0f))
                    UpdatePlatformTriggers(north->GetEntry(), north->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED);
            }
        }

        void RebuiltPlatforms()
        {
            if (auto platform = me->FindNearestGameObject(PLATFORM_EAST, 100.0f))
                platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
            if (auto platform = me->FindNearestGameObject(PLATFORM_SOUTH, 100.0f))
                platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
            if (auto platform = me->FindNearestGameObject(PLATFORM_WEST, 100.0f))
                platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
            if (auto platform = me->FindNearestGameObject(PLATFORM_NORTH, 100.0f))
                platform->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING);
            std::list<Creature*> trigList;
            me->GetCreatureListWithEntryInGrid(trigList, NPC_ROCK_ISLAND, 100.0f);
            for (auto itr = trigList.begin(); itr != trigList.end(); itr++)
                (*itr)->Respawn();
        }

        void UpdatePlatformTriggers(uint32 platform_entry, bool intact)
        {
            std::list<Creature*> list_of_npcs;
            GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_ROCK_ISLAND, 400.0f);

            uint64 first_trigger    { 0 };
            uint64 second_trigger   { 0 };

            switch (platform_entry)
            {
                case PLATFORM_EAST:
                    first_trigger   = 242076;
                    second_trigger  = 242072;
                    break;
                case PLATFORM_WEST:
                    first_trigger   = 242079;
                    second_trigger  = 242074;
                    break;
                case PLATFORM_NORTH:
                    first_trigger   = 242078;
                    second_trigger  = 242073;
                    break;
                case PLATFORM_SOUTH:
                    first_trigger   = 242077;
                    second_trigger  = 242075;
                    break;
            }

            if (list_of_npcs.size())
                for (auto c : list_of_npcs)
                    if (c->GetDBTableGUIDLow() == first_trigger || c->GetDBTableGUIDLow() == second_trigger)
                    {
                        if (intact)
                        {
                            if (c->isDead())
                                c->Respawn(true);
                        }
                        else
                        {
                            if (c->isAlive())
                                c->Kill(c, false, false);
                        }
                        c->NearTeleportTo(c->GetHomePosition());
                    }
        }

        void JustSummoned(Creature *summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_BAINE_TOTEM:
                    summon->SetReactState(REACT_PASSIVE);
                    summon->CastSpell(summon, SPELL_BAINE_TOTAM_AURA, true);
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == EFFECT_MOTION_TYPE && id == EVENT_JUMP)
                me->CastSpell(me, SPELL_PULVERIZE_1, true);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(100.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                    return false;
            return true;
        }

        bool CheckOutOfBounds()
        {
            if (auto c = me->FindNearestCreature(229999, 100.0f))
                if (me->GetExactDist(c) <= 2.f)
                    return true;

            return false;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_ENABLE_COMBAT:
                        introDone = true;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H_ALLOW_MOVEMENT);
                        me->SetReactState(REACT_AGGRESSIVE);
                    break;
                    case EVENT_WIPE_CHECK:
                        if (checkWipe() || CheckOutOfBounds())
                        {
                            me->DeleteThreatList();
                            me->CombatStop(true);
                            EnterEvadeMode();
                        }
                        else
                            events.ScheduleEvent(EVENT_WIPE_CHECK, 500);
                    break;
                    case EVENT_LAUNCH_TOTEM:
                        Talk(TALK_LAUNCH_TOTEM);
                        DoCast(SPELL_LAUNCH_TOTEM_DUMMY);
                        events.ScheduleEvent(EVENT_LAUNCH_TOTEM, 30000);
                        break;
                    case EVENT_PULVERIZE:
                        DoCast(SPELL_PULVERIZE_DUMMY);
                        events.ScheduleEvent(EVENT_PULVERIZE, 45000);
                        break;
                    case EVENT_MOLTEN_AXE:
                        me->CastSpell(me, SPELL_MOLTEN_AXE_PERIODIC_DUMMY, true);
                        break;
                    default:
                        break;
                }


            if (!me->HasUnitState(UNIT_STATE_CASTING))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

    private:
        bool introDone  {false};
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_echo_of_baineAI(creature);
    }
};

class npc_baine_totem : public CreatureScript
{
public:
    npc_baine_totem() : CreatureScript("npc_baine_totem") { }

    struct npc_baine_totemAI : public ScriptedAI
    {
        npc_baine_totemAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
        }

        void OnSpellClick(Unit* player, bool& result) override
        {
            me->DespawnOrUnsummon();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_baine_totemAI(creature);
    }
};

class spell_baine_totem_launch : public SpellScriptLoader
{
public:
    spell_baine_totem_launch() : SpellScriptLoader("spell_baine_totem_launch") { }

    class spell_baine_totem_launch_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_baine_totem_launch_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if([](WorldObject* target)
                              {
                                  if (target->GetTypeId() != TYPEID_PLAYER)
                                      return true;
                                  return false;
                              });


            if (targets.size())
            Trinity::Containers::RandomResizeList(targets, 1);
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                {
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    caster->CastSpell(target, spellId, true);
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_baine_totem_launch_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_baine_totem_launch_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_baine_totem_launch_SpellScript();
    }
};

class spell_baine_pulverize : public SpellScriptLoader
{
public:
    spell_baine_pulverize() : SpellScriptLoader("spell_baine_pulverize") { }

    class spell_baine_pulverize_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_baine_pulverize_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            targets.clear();
            if (auto c = GetCaster())
            {
                std::list<Creature*> trigList;
                GetCaster()->GetCreatureListWithEntryInGrid(trigList, NPC_ROCK_ISLAND, 100.0f);
                for (auto itr = trigList.begin(); itr != trigList.end(); itr++)
                    if ((*itr)->isAlive())
                        targets.push_back(*itr);

                targets.sort(Trinity::ObjectDistanceOrderPred(c));
                if (auto t = targets.front())
                    for (auto itr = targets.begin(); itr != targets.end(); itr++)
                        if (auto tar = (*itr))
                            if (t->GetGUID() != tar->GetGUID())
                                targets.erase(itr);

                if (targets.size())
                    Trinity::Containers::RandomResizeList(targets, 1);
                else
                {
                    auto list_of_players = c->GetPlayersInRange(140.f, true);

                    for (auto itr = list_of_players.begin(); itr != list_of_players.end(); itr++)
                        if ((*itr)->isAlive())
                            targets.push_back(*itr);

                    if (targets.size())
                        Trinity::Containers::RandomResizeList(targets, 1);
                }
            }
        }


        void HandleDummy(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
                if (Unit *target = GetHitUnit())
                {
                    if (auto baine = caster->ToCreature())
                        if (auto script = baine->AI())
                        {
                            script->Talk(TALK_PULVERIZE, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                        }
                    uint32 spellId = GetSpellInfo()->Effects[effIndex].CalcValue();
                    caster->CastSpell(target, spellId, true);
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->ToCreature()->DespawnOrUnsummon(1000);
                    else
                        if (Creature *trig = target->FindNearestCreature(NPC_ROCK_ISLAND, 15.0f))
                            trig->DespawnOrUnsummon(1000);
                }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_baine_pulverize_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
           OnEffectHitTarget += SpellEffectFn(spell_baine_pulverize_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_baine_pulverize_SpellScript();
    }
};

class spell_molten_axe_periodic_dummy : public SpellScriptLoader
{
public:
    spell_molten_axe_periodic_dummy() : SpellScriptLoader("spell_molten_axe_periodic_dummy") { }

    class spell_molten_axe_periodic_dummy_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_molten_axe_periodic_dummy_AuraScript);

        void OnPeriodic(AuraEffect const* aurEff)
        {
            if (GetSpellInfo()->Id == SPELL_MOLTEN_AXE_PERIODIC_DUMMY)
                if (auto target = GetTarget())
                    if (target->IsInWater())
                        target->CastSpell(target, SPELL_MOLTEN_AXE, true);

            if (GetSpellInfo()->Id == SPELL_MOLTEN_FISTS_PERIODIC)
                if (auto target = GetTarget())
                    if (auto baine = target->FindNearestCreature(NPC_ECHO_OF_BAINE, 250.f, true))
                        if (baine->HasAura(SPELL_MOLTEN_AXE))
                            if (target->IsInWater())
                                target->CastSpell(target, SPELL_MOLTEN_FISTS_PROC_AURA, true);
                    
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_molten_axe_periodic_dummy_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_molten_axe_periodic_dummy_AuraScript();
    }
};

class spell_molten_axe_proc : public SpellScriptLoader
{
public:
    spell_molten_axe_proc() : SpellScriptLoader("spell_molten_axe_proc") { }

    class spell_molten_axe_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_molten_axe_proc_AuraScript);

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetActionTarget(), aurEff->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_molten_axe_proc_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_molten_axe_proc_AuraScript();
    }
};

class spell_player_launch_totem : public SpellScriptLoader
{
public:
    spell_player_launch_totem() : SpellScriptLoader("spell_player_launch_totem") { }

    class spell_player_launch_totem_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_player_launch_totem_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Unit *caster = GetCaster())
            {
                caster->RemoveAurasDueToSpell(SPELL_TOTEM_BACK_PERIODIC);
                caster->RemoveAurasDueToSpell(SPELL_TOTEM_BACK);
            }
        }

        void Register() override
        {
            OnEffectLaunch += SpellEffectFn(spell_player_launch_totem_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_player_launch_totem_SpellScript();
    }
};

/*
    101815
*/
class spell_baine_leap_object_destruction : public SpellScriptLoader
{
public:
    spell_baine_leap_object_destruction() : SpellScriptLoader("spell_baine_leap_object_destruction") { }


    class spell_baine_leap_object_destructionSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_baine_leap_object_destructionSpellScript);

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if (c->IsInWater())
                    return SPELL_FAILED_DONT_REPORT;

                return SPELL_CAST_OK;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (auto c = GetCaster())
            {
                targets.sort(Trinity::ObjectDistanceOrderPred(c));
                if (auto t = targets.front())
                    for (auto itr = targets.begin(); itr != targets.end(); itr++)
                        if (auto tar = (*itr))
                            if (t->GetGUID() != tar->GetGUID())
                                targets.erase(itr);

                if (targets.size())
                    Trinity::Containers::RandomResizeList(targets, 1);
            }
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_baine_leap_object_destructionSpellScript::CheckCast);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_baine_leap_object_destructionSpellScript::FilterTargets, EFFECT_0, TARGET_GAMEOBJECT_DEST_AREA);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_baine_leap_object_destructionSpellScript();
    }
};

class npc_baine_volcano : public CreatureScript
{
public:
    npc_baine_volcano() : CreatureScript("npc_baine_volcano") { }

    struct npc_baine_volcanoAI : public ScriptedAI
    {
        npc_baine_volcanoAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            events.ScheduleEvent(1, urand(0, 120000));
            me->AddAura(98250, me);
            me->AddAura(97699, me);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            if (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 1:
                {
                    events.ScheduleEvent(1, urand(45000, 180000));
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, 54550, 150.0f);
                    if (list_of_npcs.size())
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_npcs))
                        {
                            Position erupt_to(*random_target);
                            random_target->MoveBlink(erupt_to, frand(0.f, 10.f), frand(0.f, M_PI * 2.f));
                            if (auto eruption = me->SummonCreature(144951, erupt_to, TEMPSUMMON_TIMED_DESPAWN, 15000))
                                me->CastSpell(eruption, 98402, true);
                        }
                }
                default:
                    break;
                }
        }
    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_baine_volcanoAI(creature);
    }
};

void AddSC_boss_echo_of_baine()
{
    new boss_echo_of_baine();
    new npc_baine_totem();
    new spell_baine_pulverize();
    new spell_baine_totem_launch();
    new spell_molten_axe_periodic_dummy();
    new spell_molten_axe_proc();
    new spell_player_launch_totem();
    new spell_baine_leap_object_destruction();
    new npc_baine_volcano();
}
