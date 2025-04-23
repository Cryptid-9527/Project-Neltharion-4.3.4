
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "grimbatol.h"

enum Say
{
    SAY_AGGRO               = 0,
    SAY_EQUIPPING_SHIELD    = 1,
    SAY_EQUIPPING_SWORD     = 2,
    SAY_EQUIPPING_MACE      = 3,
    SAY_SLAY                = 4,
    SAY_DEATH               = 5,
    EMOTE_SWORDS            = 6,
    EMOTE_MACE              = 7,
    EMOTE_SHIELD            = 8,
    EMOTE_IMPALE            = 9
};

enum Spells
{
    // Misc
    SPELL_MIGHTY_STOMP          = 74984, //pietinement puissant
    SPELL_PICK_WEAPON           = 75000, // He switches his weapon (DBM Announce)
    SPELL_CAVE_IN               = 74987,
    SPELL_CAVE_IN_TRIGGER       = 74986,

    // Shield
    SPELL_SHIELD_VISUAL         = 94588,
    SPELL_FLAME_ARROW           = 74946,
    SPELL_PERSONAL_PHALANX_FIX  = 75071,
    SPELL_PERSONAL_PHALANX      = 74908,
    SPELL_FIXATE_TRIGGER_SUMMON = 74926,
    SPELL_FLAMING_SHIELD        = 90819,

    // Swords Phase
    SPELL_DUAL_BLADES_BUFF      = 74981,
    SPELL_BURNING_FLAMES        = 90759,
    SPELL_DISORIENTING_ROAR     = 74976,

    // Mace Phase
    SPELL_ENCUMBERED            = 75007,
    SPELL_LAVAL_MARRE           = 90743,
    SPELL_IMPALING_SLAM         = 75056,

    SPELL_LAVA_PATCH            = 90754,
    SPELL_LAVA_PATCH_VISUAL     = 90752,
};

enum Events
{
    // General
    EVENT_PICK_WEAPON = 1,
    EVENT_STOMP,
    // Shield Phase
    EVENT_PERSONAL_PHALANX ,
    EVENT_PERSONAL_PHALANX_FIX,
    // Swords Phase
    EVENT_DISORIENTING_ROAR,
    // Mace Phase
    EVENT_IMPALING_SLAM,
    EVENT_ARCHER_SHOOT,

    ACTION_FIXATE_PREPARATION,
    ACTION_FIXATE_ORIENTATION,
    ACTION_FIXATE_RECOVERY,
};

enum Weapon
{
    WEAPON_NON          = 0,
    WEAPON_CHOOSING     = 1,
    WEAPON_SHIELD       = 2,
    WEAPON_SWORDS       = 3,
    WEAPON_MACE         = 4,
};

enum Equipment
{
    EQUIPMENT_ID_SWORD  = 64435,  // Not Blizzlike
    EQUIPMENT_ID_MACE   = 49737,  // Not Blizzlike
};

Position const TwilightArcherSummonPos[13] =
{
    {-542.994f, -605.236f, 300.201f, 1.68049f},
    {-543.59f, -605.413f, 283.784f, 1.50377f},
    {-521.237f, -605.435f, 300.76f, 1.63886f},
    {-483.862f, -588.658f, 297.574f, 2.38106f},
    {-482.655f, -588.461f, 280.966f, 2.34571f},
    {-471.266f, -575.324f, 295.906f, 2.30254f},
    {-525.377f, -455.312f, 285.288f, 4.66187f},
    {-544.49f, -454.961f, 295.831f, 4.79539f},
    {-522.164f, -455.31f, 299.791f, 4.77575f},
    {-468.703f, -489.004f, 300.462f, 3.78616f},
    {-470.907f, -484.791f, 282.203f, 3.87255f},
    {-485.052f, -474.621f, 296.525f, 3.92361f},
    {-481.352f, -477.21f, 280.714f, 3.72334f}
};

class boss_forgemaster_throngus: public CreatureScript
{
public:
    boss_forgemaster_throngus() : CreatureScript("boss_forgemaster_throngus") { }

    struct boss_forgemaster_throngusAI : public ScriptedAI
    {
        boss_forgemaster_throngusAI(Creature *c) : ScriptedAI(c), instance(c->GetInstanceScript())
        {
            instance = c->GetInstanceScript();
        }

        void EnterCombat(Unit* /*who*/)
        {
            if (instance)
                instance->SetData(DATA_FORGEMASTER_THRONGUS, IN_PROGRESS);
            Talk(SAY_AGGRO);
            phases[0] = 0;
            phases[1] = 0;
            phases[2] = 0;
            events.ScheduleEvent(EVENT_PICK_WEAPON, 1000);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            Talk(SAY_DEATH);
            DespawnCreatures(NPC_FIRE_PATCH);
            DespawnCreatures(NPC_TWILIGHT_ARCHER);
            if (instance)
                instance->SetData(DATA_FORGEMASTER_THRONGUS, DONE);
            DespawnCreatures(NPC_CAVE_IN_STALKER);
        }

        void KilledUnit(Unit* victim)
        {
            Talk(SAY_SLAY);
        }

        void Reset()
        {
            if (instance)
                instance->SetData(DATA_FORGEMASTER_THRONGUS, NOT_STARTED);
            events.Reset();
            currentWaepon = WEAPON_NON;
            DespawnCreatures(NPC_FIRE_PATCH);
            DespawnCreatures(NPC_TWILIGHT_ARCHER);
            SetEquipmentSlots(false, 0, 0,0);
            DespawnCreatures(NPC_CAVE_IN_STALKER);
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() && me->getThreatManager().isThreatListEmpty())
                return;


            if (me->HasUnitState(UNIT_STATE_CASTING) && currentWaepon != WEAPON_SHIELD)
            {
                events.Update(diff);
                return;
            }

            if (currentWaepon == WEAPON_CHOOSING)
            {
                IntializeWeapon();
                events.ScheduleEvent(EVENT_PICK_WEAPON, 30000);
                return;
            }

            events.Update(diff);

            if (is_disarmed)
                if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED) && !me->HasAuraWithMechanic(MECHANIC_DISARM))
                    HandleDisarm(false);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_PICK_WEAPON:
                    {
                        ResetWeapon();
                        DoCast(SPELL_PICK_WEAPON);
                        currentWaepon = WEAPON_CHOOSING;
                        return;
                    }
                    case EVENT_STOMP:
                    {
                        DoCast(SPELL_MIGHTY_STOMP);
                        events.ScheduleEvent(EVENT_STOMP, 30000);
                        break;
                    }
                    case EVENT_PERSONAL_PHALANX:
                    {
                        DoCast(me, SPELL_SHIELD_VISUAL, true);
                        DoCast(me, SPELL_FLAME_ARROW, true);
                        if (IsHeroic())
                            DoCast(me, SPELL_FLAMING_SHIELD, true);
                        events.ScheduleEvent(EVENT_PERSONAL_PHALANX_FIX, 1);
                        break;
                    }
                    case EVENT_PERSONAL_PHALANX_FIX:
                    {
                        if (Creature *c = me->FindNearestCreature(NPC_FIXATE_STALKER, 100))
                            c->DespawnOrUnsummon();
                        if (auto tempTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 500.0f, true))
                        {
                            tempTarget->CastSpell(tempTarget, 74926, true);
                            me->CastSpell(me, SPELL_PERSONAL_PHALANX_FIX, true);
                        }
                        events.ScheduleEvent(EVENT_PERSONAL_PHALANX_FIX, urand(2000, 10000));
                        break;
                    }
                    case EVENT_IMPALING_SLAM:
                    {
                        if (auto tempTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        {
                            DoCast(tempTarget, SPELL_IMPALING_SLAM);
                            Talk(EMOTE_IMPALE, tempTarget->GetGUID());
                        }
                        events.ScheduleEvent(EVENT_IMPALING_SLAM, 15000);
                        break;
                    }
                    case EVENT_DISORIENTING_ROAR:
                    {
                        DoCast(SPELL_DISORIENTING_ROAR);
                        events.ScheduleEvent(EVENT_DISORIENTING_ROAR, 11000);
                        break;
                    }
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
            switch (summon->GetEntry())
            {
                case NPC_FIRE_PATCH:
                    summon->CastSpell(summon, SPELL_LAVA_PATCH_VISUAL, true);
                    break;
                case NPC_CAVE_IN_STALKER:
                    summon->CastSpell(summon, SPELL_CAVE_IN, true);
                    break;
                default :
                    break;
            }
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->HasAura(SPELL_AURA_MOD_DISARM))
                HandleDisarm(true);
        }

        void DoAction(int32 const actionId) override
        {
            switch (actionId)
            {
            case ACTION_FIXATE_PREPARATION:
            if (!prep)
            {
                prep = true;
                LockOrientation = me->GetAngle(me->getVictim());
                me->AttackStop();
                me->StopMoving();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                me->ClearUnitState(UNIT_STATE_ROTATING);
                me->SetReactState(REACT_PASSIVE);
                me->SetOrientation(LockOrientation);
                me->SetFacingTo(LockOrientation);
            }
                break;
            case ACTION_FIXATE_ORIENTATION:
                me->AttackStop();
                me->StopMoving();
                me->AddUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);
                me->ClearUnitState(UNIT_STATE_ROTATING);
                me->SetReactState(REACT_PASSIVE);
                me->SetOrientation(LockOrientation);
                me->SetFacingTo(LockOrientation);
                break;
            case ACTION_FIXATE_RECOVERY:
                prep = false;
                me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->AddUnitState(UNIT_STATE_ROTATING);
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            default: 
                break;
            }
        }

    private:
        void IntializeWeapon()
        {
            currentWaepon = GetNextPhase();

            switch (currentWaepon)
            {
                case WEAPON_SHIELD:
                    Talk(SAY_EQUIPPING_SHIELD);
                    Talk(EMOTE_SHIELD);
                    DespawnCreatures(NPC_TWILIGHT_ARCHER);
                    for(uint32 i = 0; i <= 12; i++)
                        me->SummonCreature(NPC_TWILIGHT_ARCHER, TwilightArcherSummonPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                    DoCast(me, SPELL_PERSONAL_PHALANX, false);
                    events.ScheduleEvent(EVENT_PERSONAL_PHALANX, 1500);
                    break;
                case WEAPON_SWORDS:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    Talk(SAY_EQUIPPING_SWORD);
                    Talk(EMOTE_SWORDS);
                    DoCast(me, SPELL_DUAL_BLADES_BUFF, true);
                    if (IsHeroic())
                        DoCast(me, SPELL_BURNING_FLAMES, true);
                    SetEquipmentSlots(false, EQUIPMENT_ID_SWORD, EQUIPMENT_ID_SWORD, 0);
                    events.ScheduleEvent(EVENT_DISORIENTING_ROAR, 11000);
                    events.ScheduleEvent(EVENT_STOMP, urand(5000, 9000));
                    break;
                case WEAPON_MACE:
                    me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
                    Talk(SAY_EQUIPPING_MACE);
                    Talk(EMOTE_MACE);
                    DoCast(me, SPELL_ENCUMBERED, true);
                    SetEquipmentSlots(false, EQUIPMENT_ID_MACE, 0,0);
                    events.ScheduleEvent(EVENT_IMPALING_SLAM, 7000);
                    events.ScheduleEvent(EVENT_STOMP, 9000);
                    break;
            }
        }

        void HandleDisarm(bool remove)
        {
            if (remove)
            {
                if (auto shield = me->GetAura(SPELL_SHIELD_VISUAL))
                {
                    is_disarmed = true;
                    disarmed_weapon_aura            = SPELL_SHIELD_VISUAL;
                    disarmed_aura_max_duration      = shield->GetMaxDuration();
                    disarmed_aura_current_duration  = shield->GetDuration();

                    me->RemoveAurasDueToSpell(SPELL_SHIELD_VISUAL);
                    me->RemoveAurasDueToSpell(SPELL_FLAMING_SHIELD);
                }
                else if (auto dualwield = me->GetAura(IsHeroic() ? 90738 : SPELL_DUAL_BLADES_BUFF))
                {
                    is_disarmed                         = true;
                    disarmed_weapon_aura                = (IsHeroic() ? 90738 : SPELL_DUAL_BLADES_BUFF);
                    disarmed_aura_max_duration          = dualwield->GetMaxDuration();
                    disarmed_aura_current_duration      = dualwield->GetDuration();

                    me->RemoveAurasDueToSpell(IsHeroic() ? 90738 : SPELL_DUAL_BLADES_BUFF);
                }
                else if (auto mace = me->GetAura(IsHeroic() ? 90729 : SPELL_ENCUMBERED))
                {
                    is_disarmed                         = true;
                    disarmed_weapon_aura                = (IsHeroic() ? 90729 : SPELL_ENCUMBERED);
                    disarmed_aura_max_duration          = mace->GetMaxDuration();
                    disarmed_aura_current_duration      = mace->GetDuration();

                    me->RemoveAurasDueToSpell(IsHeroic() ? 90729 : SPELL_ENCUMBERED);
                }

                if (is_disarmed)
                    time_of_disarm = getMSTime();
            }
            else
            {
                if (GetMSTimeDiffToNow(time_of_disarm) > disarmed_aura_current_duration)
                {
                    is_disarmed = false;
                    return;
                }
                else
                    disarmed_aura_current_duration -= GetMSTimeDiffToNow(time_of_disarm);

                if (disarmed_aura_current_duration > disarmed_aura_max_duration || disarmed_aura_current_duration < 0)
                {
                    disarmed_aura_current_duration = 0;
                    is_disarmed = false;
                    return;
                }

                if (auto recovered_aura = me->AddAura(disarmed_weapon_aura, me))
                {
                    recovered_aura->SetMaxDuration(disarmed_aura_max_duration);
                    recovered_aura->SetDuration(disarmed_aura_current_duration);
                }

                if (disarmed_weapon_aura == SPELL_SHIELD_VISUAL)
                    if (auto flaming_aura = me->AddAura(SPELL_FLAMING_SHIELD, me))
                    {
                        flaming_aura->SetMaxDuration(disarmed_aura_max_duration);
                        flaming_aura->SetDuration(disarmed_aura_current_duration);
                    }

                is_disarmed = false;
            }
        }
        void ResetWeapon()
        {
            events.Reset();
            if (Creature *c = me->FindNearestCreature(NPC_FIXATE_STALKER, 100))
                c->DespawnOrUnsummon();
            DespawnCreatures(NPC_TWILIGHT_ARCHER);
            me->CastStop();
            me->RemoveAura(SPELL_FLAME_ARROW);
            me->RemoveAura(SPELL_FLAMING_SHIELD);
            me->RemoveAura(SPELL_PERSONAL_PHALANX);
            me->RemoveAura(SPELL_PERSONAL_PHALANX_FIX);
            me->RemoveAura(SPELL_DUAL_BLADES_BUFF);
            me->RemoveAura(SPELL_LAVA_PATCH);
            me->RemoveAura(SPELL_ENCUMBERED);

            is_disarmed = false;
        }

        uint8 GetNextPhase()
        {
            uint8 base[3] = {WEAPON_SHIELD, WEAPON_SWORDS, WEAPON_MACE};
            if (phases[0] == 0 && phases[1] == 0 && phases[2] == 0)
            {
                for(uint8 i = 0; i <= 2; i++)
                {
                    while (phases[i] == 0)
                    {
                        uint8 r = urand(0,2);
                        phases[i] = base[r];
                        base[r] = 0;
                    }
                }
                uint8 v = phases[0];
                phases[0] = 0;
                return v;
            }
            else
            {
                for (uint8 i = 0; i <= 2; i++)
                {
                    if (phases[i] != 0)
                    {
                        uint8 v = phases[i];
                        phases[i] = 0;
                        return v;
                    }
                }
            }
            return urand(WEAPON_SHIELD, WEAPON_MACE);
        }

        void DespawnCreatures(uint32 entry)
        {
            std::list<Creature*> creatures;
            GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);
            if (creatures.empty())
                return;
            for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                (*iter)->DespawnOrUnsummon();
        }

        bool is_disarmed{ false };
        uint32 disarmed_weapon_aura{ 0 };
        uint32 disarmed_aura_max_duration{ 0 };
        uint32 disarmed_aura_current_duration{ 0 };
        uint32 time_of_disarm { 0 };

        bool prep{ false };
        float LockOrientation{ 0.f };
        EventMap events;
        uint32 currentWaepon;
        uint8 phases [3];
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_forgemaster_throngusAI(pCreature);
    }
};

class spell_personal_phalanx : public SpellScriptLoader
{
public:
    spell_personal_phalanx() : SpellScriptLoader("spell_personal_phalanx") { }


    class spell_personal_phalanxSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_personal_phalanxSpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }
        bool Load()
        {
            return true;
        }

        SpellCastResult CheckCast()
        {
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                    cCreature->AI()->DoAction(ACTION_FIXATE_PREPARATION);
            return SPELL_CAST_OK;
        }
        void HandleAfterCast()
        {
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                    cCreature->AI()->DoAction(ACTION_FIXATE_ORIENTATION);
        }
        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_personal_phalanxSpellScript::CheckCast);
            AfterCast += SpellCastFn(spell_personal_phalanxSpellScript::HandleAfterCast);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_personal_phalanxSpellScript();
    }

    class spell_personal_phalanx_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_personal_phalanx_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (auto c = GetCaster())
                if (auto cCreature = c->ToCreature())
                    cCreature->AI()->DoAction(ACTION_FIXATE_RECOVERY);
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_personal_phalanx_AuraScript::HandleEffectRemove,    EFFECT_1, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_personal_phalanx_AuraScript();
    }
};


class spell_throngus_arrow_barage : public SpellScriptLoader
{
    class DistCheck
    {
    public:
        DistCheck(Unit *caster)
        {
            _caster = caster;
        }

        bool operator() (WorldObject* obj)
        {
            if (_caster && obj)
                if (_caster->GetDistance2d(obj) >= 60)
                    return true;
            return false;
        }
    private:
        Unit *_caster;
    };


public:
    spell_throngus_arrow_barage() : SpellScriptLoader("spell_throngus_arrow_barage")
    {
    }

    class script_impl : public SpellScript
    {
        PrepareSpellScript(script_impl);

        bool Load()
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if(DistCheck(GetCaster()));
            if (unitList.empty())
                return;
            std::list<WorldObject*>::const_iterator itr = unitList.begin();
            advance(itr, rand() % unitList.size());
            if (WorldObject * tar = *itr)
            {
                unitList.clear();
                unitList.push_back(tar);
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(script_impl::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new script_impl();
    }
};

class spell_disoriented_roar : public SpellScriptLoader
{
public:
    spell_disoriented_roar() : SpellScriptLoader("spell_disoriented_roar") { }

    class spell_disoriented_roar_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_disoriented_roar_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        bool Load()
        {
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_disoriented_roar_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_MOD_SPEED_SLOW_ALL);
        }

    };

    AuraScript* GetAuraScript() const
    {
        return new spell_disoriented_roar_AuraScript();
    }
};

class spell_burning_blades : public SpellScriptLoader
{
public:
    spell_burning_blades() : SpellScriptLoader("spell_burning_blades") { }

    class spell_burning_blades_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_burning_blades_AuraScript);

        int32 proclimitTimer;

        bool Load()
        {
            proclimitTimer = 0;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (proclimitTimer < 1)
            {
                proclimitTimer = 2000;
                return true;
            }
            else return false;
        }

        void OnUpdate(AuraEffect* aurEff, const uint32 diff)
        {
            proclimitTimer -= diff;
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_burning_blades_AuraScript::CheckProc);
            OnEffectUpdate += AuraEffectUpdateFn(spell_burning_blades_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_burning_blades_AuraScript();
    }
};

class spell_burning_flames_dispel: public SpellScriptLoader
{
public:
    spell_burning_flames_dispel() : SpellScriptLoader("spell_burning_flames_dispel") { }

    class spell_burning_flames_dispel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_burning_flames_dispel_AuraScript);

        void HandleOnDispel(DispelInfo* /*dispelData*/)
        {
            Remove(AURA_REMOVE_BY_DEFAULT);
        }

        void Register()
        {
            OnDispel += AuraDispelFn(spell_burning_flames_dispel_AuraScript::HandleOnDispel);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_burning_flames_dispel_AuraScript();
    }
};

void AddSC_boss_throngus()
{
    new boss_forgemaster_throngus();
    new spell_personal_phalanx();
    new spell_throngus_arrow_barage();
    new spell_disoriented_roar();
    new spell_burning_blades();
    new spell_burning_flames_dispel();
}
