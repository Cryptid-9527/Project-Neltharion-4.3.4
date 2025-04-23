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

};

enum Spells
{
    SPELL_BLINK                       = 101812,
    SPELL_FLARECORE_MISSILE           = 101927,
    SPELL_FROSTBOLT_VOLLEY            = 101810,
    SPELL_PYROBLAST                   = 101809,
    SPELL_FROST_BLADES                = 101339, // Summon Frost Blades spell. Use with JustSummoned to send them away in a straight line, otherwise npc works ok.

    // NPC_FLARECORE
    SPELL_FLARECORE_TARGET_SELECTOR   = 101588, // dummy should cast SPELL_UNSTABLE_FLARE on hit, on expire timer cast SPELL_TIME_EXPIRE_FLARE
    SPELL_UNSTABLE_FLARE              = 101980,
    SPELL_TIME_EXPIRE_FLARE           = 101587,
    SPELL_FLARE_UP                    = 101589,

    // NPC_FROST_BLADE
    SPELL_FROST_BLADES_AURA           = 101338, // trigger SPELL_FROST_BLADES_STUN
    SPELL_FROST_BLADES_STUN           = 101337,
};

enum Events
{
    EVENT_FLARECORE = 1,
    EVENT_FROSTBOLT,
    EVENT_PYROBLAST,
    EVENT_PRE_FROSTBLADES,
    EVENT_FROSTBLADES,
    EVENT_INTRO_SAY,
    EVENT_INTRO_APPEAR_SAY,
};

enum Npcs
{
    NPC_FROST_BLADE  = 54494,
    NPC_FLARECORE    = 54446,
    NPC_BLINK_TARGET = 54542,

};

enum Jaina_Says
{
    JAINA_SAY_INTRO = 0,
    JAINA_SAY_AGGRO,
    JAINA_SAY_KILLED_PLAYER,
    JAINA_SAY_SPELL_FROST_BLADES,
    JAINA_SAY_JUST_DIED = 4,
};

class boss_echo_of_jaina : public CreatureScript
{
public:
    boss_echo_of_jaina() : CreatureScript("boss_echo_of_jaina") { }
    struct boss_echo_of_jainaAI : public BossAI
    {
        boss_echo_of_jainaAI(Creature* creature) : BossAI(creature, BOSS_ECHO_OF_JAINA) 
        {
            creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            intro = false;
        }

        void Reset()
        {
            _orientation = 0.0f;
            RespawnBlinkTrigger();
            _Reset();
            me->DespawnCreaturesInArea(NPC_FROST_BLADE, 250.f);
            me->DespawnCreaturesInArea(NPC_FLARECORE, 250.f);
        }

        void RespawnBlinkTrigger()
        {
            std::list<Creature *> trigList;
            me->GetCreatureListWithEntryInGrid(trigList, NPC_BLINK_TARGET, 1000.0f);
            for (auto itr = trigList.begin(); itr != trigList.end(); itr++)
                (*itr)->Respawn();
        }

        void JustDied(Unit* killer)
        {
            Talk(JAINA_SAY_JUST_DIED, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
            // fixes combat bug - we currently dont know what the players hold in combat
            Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                if (auto player = i->getSource())
                    if (player->isAlive())
                        player->CombatStop(false);

            _JustDied();
            me->DespawnCreaturesInArea(NPC_FROST_BLADE, 250.f);
            me->DespawnCreaturesInArea(NPC_FLARECORE, 250.f);
        }

        void KilledUnit(Unit* victim) override
        {
            Talk(JAINA_SAY_KILLED_PLAYER);
        }

        void EnterCombat(Unit* who)
        {
            Talk(JAINA_SAY_AGGRO, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
            me->SetReactState(REACT_AGGRESSIVE);
            events.ScheduleEvent(EVENT_PYROBLAST, 500);
            events.ScheduleEvent(EVENT_PRE_FROSTBLADES, 20000);
            events.ScheduleEvent(EVENT_FLARECORE, 14000);
            _EnterCombat();
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetReactState(REACT_PASSIVE);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            switch (summon->GetEntry())
            {
                case NPC_FLARECORE:
                    summon->CastSpell(summon, SPELL_FLARECORE_TARGET_SELECTOR, true);
                    summon->CastWithDelay(1000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(2000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(3000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(4000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(5000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(6000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(7000, me, SPELL_FLARE_UP);
                    summon->CastWithDelay(8000, me, SPELL_FLARE_UP);
                    break;
                case NPC_FROST_BLADE:
                {
                    summon->CastSpell(summon, SPELL_FROST_BLADES_AURA, true);
                    Position pos;
                    me->GetPosition(&pos);
                    me->NormalizeOrientation(_orientation);
                    me->GetNearPosition(pos, 100.0f, _orientation);
                    summon->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                    summon->GetMotionMaster()->MovePoint(0, pos, false);
                    summon->DespawnOrUnsummon(20000);
                    _orientation += (M_PI / 5.3f);
                    break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {            

            events.Update(diff);

            if (!intro)
            if (me->IsVisible())
            if (me->FindNearestPlayer(40.f))
            if (me->isAlive())
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC))
            {
                intro = true;
                events.ScheduleEvent(EVENT_INTRO_APPEAR_SAY, 2000);
                events.ScheduleEvent(EVENT_INTRO_SAY, 10000);
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_PYROBLAST:
                        if (!UpdateVictim())
                            return;
                        DoCastVictim(SPELL_PYROBLAST);
                        events.ScheduleEvent(EVENT_PYROBLAST, 3500);
                        break;
                    case EVENT_FROSTBOLT:
                        if (!UpdateVictim())
                            return;
                        DoCast(SPELL_FROSTBOLT_VOLLEY);
                        events.ScheduleEvent(EVENT_FROSTBOLT, 25000);
                        break;
                    case EVENT_FLARECORE:
                        if (!UpdateVictim())
                            return;
                        DoCastRandom(SPELL_FLARECORE_MISSILE, 0.0f);
                        events.ScheduleEvent(EVENT_FLARECORE, 20000);
                        break;
                    case EVENT_PRE_FROSTBLADES:
                        if (!UpdateVictim())
                            return;
                        DoCast(SPELL_BLINK);
                        events.DelayEvents(2500);
                        events.ScheduleEvent(EVENT_PRE_FROSTBLADES, 25000);
                        events.ScheduleEvent(EVENT_FROSTBLADES, 1000);
                        break;
                    case EVENT_FROSTBLADES:
                        if (!UpdateVictim())
                            return;
                        Talk(JAINA_SAY_SPELL_FROST_BLADES);
                        if (Creature *blink = me->FindNearestCreature(NPC_BLINK_TARGET, 10.0f))
                            blink->DespawnOrUnsummon();
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            _orientation = me->GetAngle(target) - M_PI / 12.0f - me->GetOrientation();
                        for (uint8 i = 0; i < 3; i++)
                            me->CastSpell(me, SPELL_FROST_BLADES, true);
                        if (!events.HasEvent(EVENT_FROSTBOLT))
                            events.ScheduleEvent(EVENT_FROSTBOLT, 0); // always first spell after frostblades
                        else
                        {
                            events.RescheduleEvent(EVENT_FROSTBOLT, 0);
                        }
                        break;
                    case EVENT_INTRO_APPEAR_SAY:
                        Talk(0, NULL, CHAT_MSG_MONSTER_SAY, TEXT_RANGE_MAP);
                        break;
                    case EVENT_INTRO_SAY:
                        intro = true;
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                        break;
                    default:
                        break;
                }
            }
        }

    private:
        bool intro;
        float _orientation;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_echo_of_jainaAI(creature);
    }
};

// 101616
class spell_jaina_flarecore_dummy : public SpellScriptLoader
{
public:
    spell_jaina_flarecore_dummy() : SpellScriptLoader("spell_jaina_flarecore_dummy") { }

    class spell_jaina_flarecore_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jaina_flarecore_dummy_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit *caster = GetCaster())
                if (auto target = GetHitUnit())
                {
                    //caster->CastSpell(caster, SPELL_UNSTABLE_FLARE, true);
                    int32 basepoints0 = 5000 * caster->GetObjectSize();
                    caster->CastCustomSpell(caster, SPELL_UNSTABLE_FLARE, &basepoints0, NULL, NULL, true);

                    if (caster->GetTypeId() == TYPEID_UNIT)
                        caster->ToCreature()->DespawnOrUnsummon(1000);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_jaina_flarecore_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_jaina_flarecore_dummy_SpellScript();
    }
};

// 101588
class spell_jaina_flamecore_periodic : public SpellScriptLoader
{
public:
    spell_jaina_flamecore_periodic() : SpellScriptLoader("spell_jaina_flamecore_periodic") { }

    class spell_jaina_flamecore_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_jaina_flamecore_periodic_AuraScript);

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
            if (removeMode != AURA_REMOVE_BY_EXPIRE)
                return;
            if (Unit *caster = GetCaster())
            {
                caster->CastSpell(caster, SPELL_TIME_EXPIRE_FLARE, true);
                if (caster->GetTypeId() == TYPEID_UNIT)
                    caster->ToCreature()->DespawnOrUnsummon(1000);
            }
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_jaina_flamecore_periodic_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_jaina_flamecore_periodic_AuraScript();
    }
};

class go_jaina_staff_fragment : public GameObjectScript
{
public:
    go_jaina_staff_fragment() : GameObjectScript("go_jaina_staff_fragment") 
    {
    }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE))
            return true;
        
        if (auto g = go->FindNearestCreature(54638, 5.f))
            g->DespawnOrUnsummon();
        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);

        if (auto instance = go->GetInstanceScript())
            instance->SetData(DATA_FRAGMENTS, 0);
        player->PlayerTalkClass->ClearMenus();
        go->Delete(1000);
        return true;
    }
    private:
        InstanceScript* instance;
};

enum nozdormu_events
{
    EVENT_SPARKLE_FRAGMENTS         = 1,
    EVENT_TALK_INTRO_JAINA,
    EVENT_TALK_INTRO_TYRANDE,
    EVENT_TALK_INTRO_SYLVANAS,
    EVENT_TALK_INTRO_BAINE,
    EVENT_INTRO_DESPAWN,
};

enum nozdormu_intros
{
    INTRO_AZURE = 0,
    INTRO_EMERALD,
    INTRO_RUBY,
    INTRO_OBSIDIAN,
};

class npc_Image_Nozdormu : public CreatureScript
{
public:
    npc_Image_Nozdormu() : CreatureScript("npc_Image_Nozdormu") { }

    struct npc_Image_NozdormuAI : public ScriptedAI
    {
        npc_Image_NozdormuAI(Creature* creature) : ScriptedAI(creature) { }

        void InitializeAI()
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void SparkleFragments()
        {
            //TC_LOG_ERROR("sql.sql", "executing sparkle fragments.");
            std::list<GameObject*> Fragments;
            me->GetGameObjectListWithEntryInGrid(Fragments, 209318, 800.0f);
            if (!Fragments.empty())
            {
                //TC_LOG_ERROR("sql.sql", "fragments size: %u", Fragments.size());

                for (std::list<GameObject*>::const_iterator itr = Fragments.begin(); itr != Fragments.end(); ++itr)
                {
                    if (!(*itr)->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE))
                    if (!(*itr)->FindNearestCreature(54638, 5.f))
                    if (auto g = (*itr)->SummonCreature(54638, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                        g->AddAura(92406, g);
                }
            }
        }


        void DoAction(int32 const action) override
        {
            switch (action)
            {
            case 7://murozond enter combat
                Talk(5, NULL, CHAT_MSG_MONSTER_YELL, TEXT_RANGE_MAP);
                break;
            case 8://murozond died
            {
                Position moruzond_just_died { 4129.305664f, -422.857574f, 122.765884f, 0.096477f };
                me->NearTeleportTo(moruzond_just_died);
                me->TalkWithDelay(4000, 6, NULL, CHAT_MSG_MONSTER_SAY);
                me->TalkWithDelay(19000, 7, NULL, CHAT_MSG_MONSTER_SAY);
                me->TalkWithDelay(36000, 8, NULL, CHAT_MSG_MONSTER_SAY);
                break;
            }
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->ToPlayer())
            {
                //TC_LOG_ERROR("sql.sql", "recognized player movement.");
                if (me->GetDistance(who) <= 50.f)
                    switch (me->GetAreaId())
                    {
                    case 5790:
                        if (intros[INTRO_RUBY] == false)
                        {
                            intros[INTRO_RUBY] = true;
                            events.ScheduleEvent(EVENT_TALK_INTRO_SYLVANAS, 3000);
                        }
                        break;
                    case 5792:
                        if (intros[INTRO_OBSIDIAN] == false)
                        {
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            intros[INTRO_OBSIDIAN] = true;
                            events.ScheduleEvent(EVENT_TALK_INTRO_BAINE, 5000);
                        }
                        break;
                    case 5793:
                        if (intros[INTRO_AZURE] == false)
                        {
                            events.ScheduleEvent(EVENT_SPARKLE_FRAGMENTS, 10000);
                            events.ScheduleEvent(EVENT_TALK_INTRO_JAINA, 5000);
                            intros[INTRO_AZURE] = true;
                        }
                        break;
                    case 5794:
                        if (intros[INTRO_EMERALD] == false)
                        {
                            events.ScheduleEvent(EVENT_TALK_INTRO_TYRANDE, 5000);
                            intros[INTRO_EMERALD] = true;
                        }
                    default:
                        break;
                    }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            //TC_LOG_ERROR("sql.sql", "UpdateAI");
            events.Update(diff);
                while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case EVENT_SPARKLE_FRAGMENTS:
                    //TC_LOG_ERROR("sql.sql", "Event sparkle fragments.");
                    SparkleFragments();
                    events.ScheduleEvent(EVENT_SPARKLE_FRAGMENTS, 5000);
                    if (auto instance = me->GetInstanceScript())
                        instance->SetData(DATA_FRAGMENTS, 1);
                    break;
                case EVENT_TALK_INTRO_JAINA:
                    Talk(0);
                    events.ScheduleEvent(EVENT_INTRO_DESPAWN, 25000);
                    break;
                case EVENT_TALK_INTRO_TYRANDE:
                    Talk(1);
                    break;
                case EVENT_INTRO_DESPAWN:
                    me->SetVisible(false);
                    me->DespawnOrUnsummon(2000);
                    break;
                case EVENT_TALK_INTRO_SYLVANAS:
                    Talk(4);
                    break;
                case EVENT_TALK_INTRO_BAINE:
                    Talk(2);
                    me->TalkWithDelay(15000, 3, NULL, CHAT_MSG_MONSTER_SAY);
                    break;
                default:
                    break;
                }
        }

        void Reset() override
        {
        }
    private:
        bool intros[4] { false, false, false, false };
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Image_NozdormuAI(creature);
    }
};

class spell_jaina_flarecore_damage : public SpellScriptLoader
{
public:
    spell_jaina_flarecore_damage() : SpellScriptLoader("spell_jaina_flarecore_damage") { }

    class spell_jaina_flarecore_damageSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_jaina_flarecore_damageSpellScript);

        void HandleDamage(SpellEffIndex /*effIndex*/)
        {
            if (auto c = GetCaster())
            {
                float scale         { c->GetFloatValue(OBJECT_FIELD_SCALE_X)    };
                float damage        { 5000.f                                    };
                float new_damage    { damage * scale                            };
                if (new_damage > 50000.f)
                    new_damage = 50000.f;

                //TC_LOG_ERROR("sql.sql", "caster scale: %u base damage:%u multiplied damage: %u", int32(double(scale)), int32(double(damage)), int32(double(new_damage)));
                SetHitDamage(int32(new_damage));
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_jaina_flarecore_damageSpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_jaina_flarecore_damageSpellScript();
    }
};

void AddSC_boss_echo_of_jaina()
{
    new npc_Image_Nozdormu();
    new boss_echo_of_jaina();
    new spell_jaina_flarecore_dummy();
    new spell_jaina_flamecore_periodic();
    new go_jaina_staff_fragment();
    new spell_jaina_flarecore_damage();
}
