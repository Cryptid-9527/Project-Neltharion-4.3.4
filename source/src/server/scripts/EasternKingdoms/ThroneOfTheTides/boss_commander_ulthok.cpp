
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "throne_of_the_tides.h"

enum Spells
{
    SPELL_ENRAGE                        = 76100,
    SPELL_DARK_FISSURE                  = 76047,
    SPELL_DARK_FISSURE_AURA_NM          = 76066,
    SPELL_DARK_FISSURE_AURA_HC          = 91371,
    SPELL_DARK_FISSURE_DMG_NM           = 76085,
    SPELL_DARK_FISSURE_DMG_HC           = 91375,
    SPELL_SQUEEZE                       = 76026,
    SPELL_SQUEEZE_VEHICLE               = 76038,
    SPELL_CURSE_OF_FATIGUE              = 76094,
    SPELL_PULL_TARGET                   = 67357,

    // Intro
    SPELL_INTRO_VISUAL_STATE            = 76017,
    SPELL_INTRO_VISUAL_CEILING_IMPACT   = 76021,
    SPELL_INTRO_JUMP                    = 82960,
    SPELL_RIDE_VEHICLE_HARDCODED = 46598,  // caster 53879 "Deathwing" - target 56161
};
enum Events
{
    EVENT_NONE                          = 0,
    EVENT_DARK_FISSURE,
    EVENT_ENRAGE,
    EVENT_SQUEEZE_START,
    EVENT_SQUEEZE_END,
    EVENT_CURSE,
};
enum Yells
{
    SAY_AGGRO                           = 0,
    SAY_AGGRO_WHISP,
    SAY_DEATH,
    SAY_DEATH_WHISP,
    SAY_KILLED_PLAYER,
    SAY_KILLED_PLAYER_WHISP,
};

/*
    Commander Ulthok - 40765(N) & 49064(H)
*/
class boss_commander_ulthok : public CreatureScript
{
public:
    boss_commander_ulthok() : CreatureScript("boss_commander_ulthok") {}

    struct boss_commander_ulthokAI : public BossAI
    {
        boss_commander_ulthokAI(Creature* creature) : BossAI(creature, DATA_COMMANDER_ULTHOK), summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        void Reset()
        {
            events.Reset();
            start_event = false;
            mui_start_event = 500;
            if (Vehicle *veh = me->GetVehicleKit())
                veh->RemoveAllPassengers();
            if (instance)
                instance->SetData(DATA_COMMANDER_ULTHOK, NOT_STARTED);
            me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            me->SetHomePosition(50.57f, 802.39f, 805.7309f, 0.0f);
            me->DeleteThreatList();
            _Reset();
        }
        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
            Talk(SAY_AGGRO);
            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
            if (!PlayerList.isEmpty())
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    Talk(SAY_AGGRO_WHISP, i->getSource()->GetGUID());
            events.ScheduleEvent(EVENT_SQUEEZE_START, 15000);
            if (instance)
                instance->SetData(DATA_COMMANDER_ULTHOK, IN_PROGRESS);
        }


        void DoAction(int32 const action)
        {
            switch (action)
            {
            case 42 :
                start_event = true;
                break;
            default :
                break;
            }
        }
        void MovementInform(uint32 Type, uint32 Id)
        {
            if(Id == 1001)
            {
                me->RemoveAllAuras();
                if (auto corail = ObjectAccessor::GetGameObject(*me, instance->GetData64(GO_CORAIL)))
                {
                    corail->Use(me);
                    corail->Delete();
                }
                me->CastSpell(me, SPELL_INTRO_JUMP, true);
            }
        }
        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
            {
                Talk(SAY_KILLED_PLAYER);
                Talk(SAY_KILLED_PLAYER_WHISP, victim->GetGUID());
            }
        }
        void UpdateAI(const uint32 diff)
        {
            if (start_event)
            {
                if (start_event && mui_start_event <= diff)
                {
                    if (auto trigger = me->FindNearestCreature(NPC_CINEMATIC_TRIGGER, 20.0f))
                        trigger->AI()->DoCast(SPELL_INTRO_VISUAL_CEILING_IMPACT);
                    me->CastSpell(me, SPELL_INTRO_VISUAL_STATE, true);
                    Position pos = { 50.57f, 802.39f, 805.7308f, 0.0f };
                    me->GetMotionMaster()->MoveJump(pos, 15.0f, 15.0f, 1001);
                    start_event = false;
                    mui_start_event = 2000;
                }
                else
                    mui_start_event -= diff;
                return;
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SQUEEZE_START:
                {
                    lock_orientation = me->GetOrientation();
                    if (auto victim = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                        me->CastSpell(victim, SPELL_SQUEEZE);
                    me->SetFacingTo(lock_orientation);
                    if (events.HasEvent(EVENT_DARK_FISSURE))
                        events.DelayEvent(EVENT_DARK_FISSURE, 9000);
                    else
                        events.RescheduleEvent(EVENT_DARK_FISSURE, 9000);

                    if (events.HasEvent(EVENT_ENRAGE))
                        events.DelayEvent(EVENT_ENRAGE, 9000);
                    else
                        events.RescheduleEvent(EVENT_ENRAGE, 11000);

                    events.CancelEvent(EVENT_SQUEEZE_START);
                    events.ScheduleEvent(EVENT_SQUEEZE_END, 6000);
                    break;
                }
                case EVENT_SQUEEZE_END:
                {
                    me->SetFacingTo(lock_orientation);
                    if (Vehicle* vehicle = me->GetVehicleKit())
                        vehicle->RemoveAllPassengers();
                    events.CancelEvent(EVENT_SQUEEZE_END);
                    events.RescheduleEvent(EVENT_SQUEEZE_START, 20000);
                    events.RescheduleEvent(EVENT_CURSE, 2000);
                    break;
                }
                case EVENT_DARK_FISSURE:
                {
                    Position pos(*me);
                    me->MoveBlink(pos, 5.f, 0.f);
                    if (auto dark = me->SummonCreature(NPC_DARK_FISSURE, pos))
                        dark->CastWithDelay(2000, dark, IsHeroic() ? SPELL_DARK_FISSURE_AURA_HC : SPELL_DARK_FISSURE_AURA_NM);
                    DoCast(me, SPELL_DARK_FISSURE);
                    events.CancelEvent(EVENT_DARK_FISSURE);
                }
                    break;
                case EVENT_ENRAGE:
                    DoCast(SPELL_ENRAGE);
                    events.CancelEvent(EVENT_ENRAGE);
                    break;
                case EVENT_CURSE:
                    if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        me->CastSpell(target, SPELL_CURSE_OF_FATIGUE);
                    events.CancelEvent(EVENT_CURSE);
                    break;
                default:
                    break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) 
        {
            if (auto p = passenger)
                    if (!apply)
                    {
                        //TC_LOG_ERROR("sql.sql", "booting passenger back.");
                        Position pos(*me);
                        me->MoveBlink(pos, 8, 0.f);
                        pos.m_positionZ += 7.f;
                        p->NearTeleportTo(pos);
                        p->CastWithDelay(200, p, 66033, true);
                    }
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();
            Talk(SAY_DEATH);
            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
            if (!PlayerList.isEmpty())
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    Talk(SAY_DEATH_WHISP, i->getSource()->GetGUID());
            if (instance)
                instance->SetData(DATA_COMMANDER_ULTHOK_EVENT, DONE);

            me->DeleteThreatList();
            summons.DespawnAll();
        }
    private:
        float lock_orientation{ 0.f };
        SummonList summons;
        EventMap events;
        InstanceScript *instance;
        uint32 mui_start_event;
        bool start_event = false;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_commander_ulthokAI(creature);
    }
};

/*
    Dark Fissure - 40784(N) & 49065(H)
*/
class npc_dark_fissure : public CreatureScript
{
public:
    npc_dark_fissure() : CreatureScript("npc_dark_fissure") { }
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_fissureAI(creature);
    }
    struct npc_dark_fissureAI : public ScriptedAI
    {
        npc_dark_fissureAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
        }
        void Reset()
        {
            mui_despawn = 1000;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
        }
        void UpdateAI(const uint32 diff)
        {
            if (mui_despawn <= diff)
            {
                if (instance->GetData(DATA_COMMANDER_ULTHOK_EVENT) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
                mui_despawn = 1000;
            }
            else mui_despawn -= diff;
        }
    private :
        InstanceScript* instance;
        uint32 mui_despawn;

    };
};

/*
	Squeeze - 76026(N) & 91484(H)
*/

class spell_ulthok_squeeze_channel : public SpellScriptLoader
{
public:
    spell_ulthok_squeeze_channel() : SpellScriptLoader("spell_ulthok_squeeze_channel") { };
    class spell_ulthok_squeeze_channelAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ulthok_squeeze_channelAuraScript);

        bool Load()
        {
            return true;
        }

        void HandleOnEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
                if (auto o = GetOwner()->ToUnit())
                    o->AddAura(SPELL_SQUEEZE_VEHICLE, c);
        }

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
            {
                c->RemoveAurasDueToSpell(SPELL_SQUEEZE_VEHICLE);
                if (auto v = c->GetVehicleKit())
                    v->RemoveAllPassengers();
            }
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_ulthok_squeeze_channelAuraScript::HandleOnEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            OnEffectRemove += AuraEffectRemoveFn(spell_ulthok_squeeze_channelAuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_ulthok_squeeze_channelAuraScript();
    }
};

class spell_ulthok_squeeze_triggered : public SpellScriptLoader
{
public:
    spell_ulthok_squeeze_triggered() : SpellScriptLoader("spell_ulthok_squeeze_triggered") { };
    class spell_ulthok_squeeze_triggeredAuraScript : public AuraScript
    {
        PrepareAuraScript(spell_ulthok_squeeze_triggeredAuraScript);

        bool Load()
        {
            return true;
        }

        void HandleOnEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
        {
            if (auto c = GetCaster())
                if (auto t = GetOwner())
                    if (auto b = c->GetVehicleBase())
                        if (b->GetGUID() == t->GetGUID())
                        {
                            //TC_LOG_ERROR("sql.sql", "removing %u due to %u", aurEff->GetEffIndex(), int32(mode));
                            c->ExitVehicle();
                            c->RemoveAurasDueToSpell(SPELL_SQUEEZE);
                        }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_ulthok_squeeze_triggeredAuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_ulthok_squeeze_triggeredAuraScript();
    }
};


/*
    Dark Fissure - 91375(HC)
*/
class spell_dark_fissure_hc_ulthok : public SpellScriptLoader
{
    class DistCheck
    {
    public:
        DistCheck(Unit* caster)
        {
            _caster = caster;
        }

        bool operator() (WorldObject* obj)
        {
            if (_caster && obj)
                if (_caster->GetDistance2d(obj) >= 5 + ((0.25) * _caster->GetAuraCount(SPELL_DARK_FISSURE_DMG_HC)))
                    return true;
            return false;
        }
    private:
        Unit* _caster;
    };

public:
    spell_dark_fissure_hc_ulthok() : SpellScriptLoader("spell_dark_fissure_hc_ulthok") { }

    class spell_dark_fissure_hc_ulthokSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dark_fissure_hc_ulthokSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }
        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if(DistCheck(GetCaster()));
        }
        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dark_fissure_hc_ulthokSpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_dark_fissure_hc_ulthokSpellScript();
    }
};

void AddSC_boss_commander_ulthok()
{
    new boss_commander_ulthok();
    new npc_dark_fissure();

    new spell_ulthok_squeeze_channel();
    new spell_ulthok_squeeze_triggered();
    new spell_dark_fissure_hc_ulthok();
}
