/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum WarriorSpells
{
    SPELL_WARRIOR_BLOODTHIRST                       = 23885,
    SPELL_WARRIOR_BLOODTHIRST_DAMAGE                = 23881,
    SPELL_WARRIOR_CHARGE                            = 34846,
    SPELL_WARRIOR_DEEP_WOUNDS_RANK_1                = 12162,
    SPELL_WARRIOR_DEEP_WOUNDS_RANK_2                = 12850,
    SPELL_WARRIOR_DEEP_WOUNDS_RANK_3                = 12868,
    SPELL_WARRIOR_DEEP_WOUNDS_RANK_PERIODIC         = 12721,
    SPELL_WARRIOR_EXECUTE                           = 5308,
    SPELL_WARRIOR_GLYPH_OF_EXECUTION                = 58367,
    SPELL_WARRIOR_GLYPH_OF_VIGILANCE                = 63326,
    SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF        = 65156,
    SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT      = 64976,
    SPELL_WARRIOR_LAST_STAND_TRIGGERED              = 12976,
    SPELL_WARRIOR_SLAM                              = 50782,
    SPELL_WARRIOR_SWEEPING_STRIKES                  = 12328,
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK     = 12723,
    SPELL_WARRIOR_TAUNT                             = 355,
    SPELL_WARRIOR_UNRELENTING_ASSAULT_RANK_1        = 46859,
    SPELL_WARRIOR_UNRELENTING_ASSAULT_RANK_2        = 46860,
    SPELL_WARRIOR_UNRELENTING_ASSAULT_TRIGGER_1     = 64849,
    SPELL_WARRIOR_UNRELENTING_ASSAULT_TRIGGER_2     = 64850,
    SPELL_WARRIOR_VIGILANCE_PROC                    = 50725,
    SPELL_WARRIOR_VIGILANCE_REDIRECT_THREAT         = 59665,
    SPELL_WARRIOR_GLYPH_INTIMIDATING_SHOUT          = 63327,
    SPELL_WARRIOR_INTIMIDATING_SHOUT_ROOT           = 20511,
    SPELL_WARRIOR_BLADESTORM_AURA                   = 46924,
    SPELL_WARRIOR_SHIELD_OF_FURY                    = 105909,
    SPELL_WARRIOR_SHIELD_FORTRESS                   = 105914,

    SPELL_PALADIN_BLESSING_OF_SANCTUARY             = 20911,
    SPELL_PALADIN_GREATER_BLESSING_OF_SANCTUARY     = 25899,
    SPELL_PRIEST_RENEWED_HOPE                       = 63944,
    SPELL_GEN_DAMAGE_REDUCTION_AURA                 = 68066,
    SPELL_GEN_VENGEANCE_PERIODIC                    = 76691,
    SPELL_FIELD_DRESSING_R1                         = 84579,
    SPELL_FIELD_DRESSING_R2                         = 84580,
};

enum WarriorSpellIcons
{
    WARRIOR_ICON_ID_SUDDEN_DEATH                    = 1989,
};

/// Updated 4.3.4
class spell_warr_bloodthirst : public SpellScriptLoader
{
    public:
        spell_warr_bloodthirst() : SpellScriptLoader("spell_warr_bloodthirst") { }

        class spell_warr_bloodthirst_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bloodthirst_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                ApplyPct(damage, GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK));

                if (auto target = GetHitUnit())
                {
                    damage = GetCaster()->SpellDamageBonusDone(target, GetSpellInfo(), uint32(damage), SPELL_DIRECT_DAMAGE);
                    damage = target->SpellDamageBonusTaken(GetSpellInfo(), uint32(damage), SPELL_DIRECT_DAMAGE, 1, GetCaster()->GetGUID());
                }
                SetHitDamage(damage);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                GetCaster()->CastCustomSpell(GetCaster(), SPELL_WARRIOR_BLOODTHIRST, &damage, NULL, NULL, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_bloodthirst_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnEffectHit += SpellEffectFn(spell_warr_bloodthirst_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_bloodthirst_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_bloodthirst_heal : public SpellScriptLoader
{
    public:
        spell_warr_bloodthirst_heal() : SpellScriptLoader("spell_warr_bloodthirst_heal") { }

        class spell_warr_bloodthirst_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_bloodthirst_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARRIOR_BLOODTHIRST_DAMAGE))
                {     
                    SetHitHeal(GetCaster()->CountPctFromMaxHealth(spellInfo->Effects[EFFECT_1].CalcValue(GetCaster())) / 1000);

                if (caster->HasAura(SPELL_FIELD_DRESSING_R1))
                    SetHitHeal(GetCaster()->CountPctFromMaxHealth(spellInfo->Effects[EFFECT_1].CalcValue(GetCaster())) / 883);
                else
                if (caster->HasAura(SPELL_FIELD_DRESSING_R2))
                    SetHitHeal(GetCaster()->CountPctFromMaxHealth(spellInfo->Effects[EFFECT_1].CalcValue(GetCaster())) / 786);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_bloodthirst_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_bloodthirst_heal_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_charge : public SpellScriptLoader
{
    public:
        spell_warr_charge() : SpellScriptLoader("spell_warr_charge") { }

        class spell_warr_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_charge_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT) || !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF) || !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_CHARGE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {               
                int32 chargeBasePoints0 = GetEffectValue();
                Unit* caster = GetCaster();
                caster->CastCustomSpell(caster, SPELL_WARRIOR_CHARGE, &chargeBasePoints0, NULL, NULL, true);

                // Juggernaut crit bonus
                if (caster->HasAura(SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT))
                {
                    caster->CastSpell(caster, SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_BUFF, true);
                    // Intercept shared cooldown
                    int32 cooldown = caster->HasAura(58355) ? 12000 : 13000;
                    WorldPacket data;
                    caster->BuildCooldownPacket(data, SPELL_COOLDOWN_FLAG_NONE, 20252, cooldown);
                    caster->ToPlayer()->SendDirectMessage(&data);
                }
            }

            void HandleChargeStun(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->CastSpell(GetHitUnit(), 7922, true);
                if (GetHitUnit()->HasUnitState(UNIT_STATE_CHARGING) && !GetCaster()->HasAura(7922, GetHitUnit()->GetGUID()) && !GetCaster()->HasAura(20253, GetHitUnit()->GetGUID()))
                    GetHitUnit()->StopMoving();
            }
         
            void HandleBeforeCast()
            {
                if (GetCaster())
                    if (GetCaster()->ToPlayer())
                        TargetBeforeCast = GetCaster()->ToPlayer()->GetSelectedUnit();
            }

            void HandleAfterCast()
            {
                if (GetCaster())
                    if (TargetBeforeCast)
                        if (TargetBeforeCast != GetCaster()->getVictim())
                            GetCaster()->Attack(TargetBeforeCast, true);
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_warr_charge_SpellScript::HandleBeforeCast);
                AfterCast += SpellCastFn(spell_warr_charge_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleChargeStun, EFFECT_0, SPELL_EFFECT_CHARGE);
                OnEffectLaunch += SpellEffectFn(spell_warr_charge_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        private:
            Unit* TargetBeforeCast = NULL;

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_charge_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_intercept : public SpellScriptLoader
{
    public:
        spell_warr_intercept() : SpellScriptLoader("spell_warr_intercept") { }

        class spell_warr_intercept_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_intercept_SpellScript);

            void HandleChargeCooldown(SpellEffIndex /*effIndex*/)
            {
                if (auto caster = GetCaster()->ToPlayer())
                {
                    if (caster->HasAura(SPELL_WARRIOR_JUGGERNAUT_CRIT_BONUS_TALENT))
                    {
                        WorldPacket data;
                        caster->BuildCooldownPacket(data, SPELL_COOLDOWN_FLAG_NONE, 100, 30000);
                        caster->SendDirectMessage(&data);
                    }
                }
            }

            void HandleChargeStun(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->HasUnitState(UNIT_STATE_CHARGING) && !GetCaster()->HasAura(7922, GetHitUnit()->GetGUID()) && !GetCaster()->HasAura(20253, GetHitUnit()->GetGUID()))
                    GetHitUnit()->StopMoving();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_intercept_SpellScript::HandleChargeStun, EFFECT_0, SPELL_EFFECT_CHARGE);
                OnEffectLaunch += SpellEffectFn(spell_warr_intercept_SpellScript::HandleChargeCooldown, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_intercept_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_concussion_blow : public SpellScriptLoader
{
    public:
        spell_warr_concussion_blow() : SpellScriptLoader("spell_warr_concussion_blow") { }

        class spell_warr_concussion_blow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_concussion_blow_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                SetHitDamage(CalculatePct(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), GetEffectValue()));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_concussion_blow_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_concussion_blow_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_deep_wounds : public SpellScriptLoader
{
    public:
        spell_warr_deep_wounds() : SpellScriptLoader("spell_warr_deep_wounds") { }

        class spell_warr_deep_wounds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_deep_wounds_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_DEEP_WOUNDS_RANK_1) || !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_DEEP_WOUNDS_RANK_2) || !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_DEEP_WOUNDS_RANK_3))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (auto target = GetHitUnit())
                {
                    // apply percent damage mods
                    damage = caster->SpellDamageBonusDone(target, GetSpellInfo(), damage, SPELL_DIRECT_DAMAGE);

                    ApplyPct(damage, 16 * sSpellMgr->GetSpellRank(GetSpellInfo()->Id));

                    damage = int32(target->SpellDamageBonusTaken(GetSpellInfo(), damage, SPELL_DIRECT_DAMAGE, 1, caster->GetGUID()));

                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARRIOR_DEEP_WOUNDS_RANK_PERIODIC);
                    uint32 ticks = uint32(spellInfo->GetDuration()) / spellInfo->Effects[EFFECT_0].Amplitude;

                    // Add remaining ticks to damage done
                    if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_WARRIOR_DEEP_WOUNDS_RANK_PERIODIC, EFFECT_0, caster->GetGUID()))
                        damage += aurEff->GetAmount() * int32(ticks - aurEff->GetTickNumber());

                    damage /= int32(ticks);

                    caster->CastCustomSpell(target, SPELL_WARRIOR_DEEP_WOUNDS_RANK_PERIODIC, &damage, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_deep_wounds_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_deep_wounds_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_execute : public SpellScriptLoader
{
    public:
        spell_warr_execute() : SpellScriptLoader("spell_warr_execute") { }

        class spell_warr_execute_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_execute_SpellScript);

            void HandleEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (GetHitUnit())
                {
                    int32 rageUsed = std::min<int32>(200, caster->GetPower(POWER_RAGE));
                    int32 newRage = std::max<int32>(0, caster->GetPower(POWER_RAGE) - rageUsed);

                    // Sudden Death rage save
                    if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_GENERIC, WARRIOR_ICON_ID_SUDDEN_DEATH, EFFECT_0))
                    {
                        int32 ragesave = aurEff->GetSpellInfo()->Effects[EFFECT_0].CalcValue() * 10;
                        newRage = std::max(newRage, ragesave);
                    }

                    caster->SetPower(POWER_RAGE, uint32(newRage));
                    /// Formula taken from the DBC: "${10+$AP*0.437*$m1/100}"
                    int32 baseDamage = GetEffectValue() + int32(10 + caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.437f * GetEffectValue() / 100.0f);
                    /// Formula taken from the DBC: "${$ap*0.874*$m1/100-1} = 20 rage"
                    int32 moreDamage = int32(rageUsed * (caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.874f * GetEffectValue() / 100.0f - 1) / 200);
                    SetEffectDamage(baseDamage + moreDamage);
                }
            }

            void Register()
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_warr_execute_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_execute_SpellScript();
        }
};

// 5246 - Intimidating Shout
class spell_warr_intimidating_shout : public SpellScriptLoader
{
    public:
        spell_warr_intimidating_shout() : SpellScriptLoader("spell_warr_intimidating_shout") { }

        class spell_warr_intimidating_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_intimidating_shout_SpellScript);

            void ApplyGlyph(SpellEffIndex /*effIndex*/)
            {
                if (auto specified_target = GetExplTargetUnit())
                    if (auto hit = GetHitUnit())
                        if (auto cast = GetCaster())
                            if (auto castP = cast->ToPlayer())
                                if (auto shout_aura = hit->GetAura(GetSpellInfo()->Id, cast->GetGUID()))
                                    if (cast->HasAura(SPELL_WARRIOR_GLYPH_INTIMIDATING_SHOUT) || (hit == specified_target))
                                        if (auto glyph_aura = cast->AddAura(SPELL_WARRIOR_INTIMIDATING_SHOUT_ROOT, hit))
                                        {
                                            glyph_aura->SetMaxDuration(shout_aura->GetMaxDuration());
                                            glyph_aura->SetDuration(shout_aura->GetDuration());
                                            shout_aura->Remove();
                                        }
            }

            SpellCastResult CheckRadius()
            {
                if (auto c = GetCaster())
                    if (auto target = GetExplTargetUnit())
                        if (c->GetDistance(target) > 8.f)
                            return SPELL_FAILED_OUT_OF_RANGE;

                return SPELL_CAST_OK;
            }


            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (auto c = GetCaster())
                    if (targets.size())
                        for (auto itr = targets.begin(); itr != targets.end();)
                            if (auto target = (*itr)->ToUnit())
                                if (c->GetDistance(target) > 8.f)
                                    targets.erase(itr++);
                                else
                                    ++itr;
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_warr_intimidating_shout_SpellScript::ApplyGlyph, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
                OnCheckCast += SpellCheckCastFn(spell_warr_intimidating_shout_SpellScript::CheckRadius);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_intimidating_shout_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_last_stand : public SpellScriptLoader
{
    public:
        spell_warr_last_stand() : SpellScriptLoader("spell_warr_last_stand") { }

        class spell_warr_last_stand_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_last_stand_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_LAST_STAND_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (auto caster = GetCaster())
                {
                    int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(GetEffectValue()));
                    caster->CastCustomSpell(caster, SPELL_WARRIOR_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                }
            }

            void Register()
            {
                // add dummy effect spell handler to Last Stand
                OnEffectHit += SpellEffectFn(spell_warr_last_stand_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_last_stand_SpellScript();
        }
};

// 94009 - Rend
class spell_warr_rend : public SpellScriptLoader
{
    public:
        spell_warr_rend() : SpellScriptLoader("spell_warr_rend") { }

        class spell_warr_rend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_rend_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (auto caster = GetCaster())
                {
                    // $0.25 * (($MWB + $mwb) / 2 + $AP / 14 * $MWS) bonus per tick
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 mws = caster->GetAttackTime(BASE_ATTACK);
                    float mwbMin = caster->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE);
                    float mwbMax = caster->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE);
                    float mwb = ((mwbMin + mwbMax) / 2 + ap * mws / 14000) * 0.25f;
                    amount += int32(caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), mwb));
                }
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_rend_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_rend_AuraScript();
        }
};

// Second wind
class spell_warr_second_wind : public SpellScriptLoader
{
    public:
        spell_warr_second_wind() : SpellScriptLoader("spell_warr_second_wind") { }

        class spell_warr_second_wind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_second_wind_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (auto caster = GetCaster())
                {
                    if (auto modOwner = caster->GetSpellModOwner())
                        modOwner->ApplySpellMod(GetId(), SPELLMOD_DOT, amount);

                    amount = caster->CountPctFromMaxHealth(amount) / GetSpellInfo()->GetMaxTicks();
                }
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_second_wind_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_second_wind_AuraScript();
        }
};

// 64380, 65941 - Shattering Throw
class spell_warr_shattering_throw : public SpellScriptLoader
{
    public:
        spell_warr_shattering_throw() : SpellScriptLoader("spell_warr_shattering_throw") { }

        class spell_warr_shattering_throw_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shattering_throw_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                // remove shields, will still display immune to damage part
                if (auto target = GetHitUnit())
                    target->RemoveAurasWithMechanic(1 << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_shattering_throw_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_shattering_throw_SpellScript();
        }
};

/// Updated 4.3.4
class spell_warr_slam : public SpellScriptLoader
{
    public:
        spell_warr_slam() : SpellScriptLoader("spell_warr_slam") { }

        class spell_warr_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_slam_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SLAM))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (auto target = GetHitUnit())
                {
                    Unit* caster = GetCaster();                   
                    // Check for Single-minded fury and Titan's grip
                    if (caster->HasAura(81099) && caster->haveOffhandWeapon() && !caster->ToPlayer()->HasTwoHandWeaponEquipped())
                        caster->CastSpell(target, 81101, true); // for offhand dmg
                        caster->CastSpell(target, SPELL_WARRIOR_SLAM, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_slam_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_slam_SpellScript();
        }
};

class spell_slam_damage_mainhand : public SpellScriptLoader
{
    //INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('50782', 'spell_slam_damage_mainhand');
public:
    spell_slam_damage_mainhand() : SpellScriptLoader("spell_slam_damage_mainhand") { }

    class spell_slam_damage_mainhand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_slam_damage_mainhand_SpellScript);
        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return;
            if (auto target = GetHitUnit())
            {
                Unit* caster = GetCaster();
                if (AuraEffect* aur = GetCaster()->GetAuraEffect(46916, EFFECT_0))//bloodsurge
                {
                    SetHitDamage(GetHitDamage() * 1.2);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_slam_damage_mainhand_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_slam_damage_mainhand_SpellScript();
    }
};
class spell_slam_damage_offhand : public SpellScriptLoader
{
    //INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('81101', 'spell_slam_damage_offhand');
public:
    spell_slam_damage_offhand() : SpellScriptLoader("spell_slam_damage_offhand") { }

    class spell_slam_damage_offhand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_slam_damage_offhand_SpellScript);
        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return;
            if (auto target = GetHitUnit())
            {
                Unit* caster = GetCaster();
                if (AuraEffect* aur = GetCaster()->GetAuraEffect(46916, EFFECT_0))//bloodsurge
                {
                    SetHitDamage(GetHitDamage() * 1.2);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_slam_damage_offhand_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_slam_damage_offhand_SpellScript();
    }
};

// Updated 4.3.4
class spell_warr_victory_rush : public SpellScriptLoader
{
    public:
        spell_warr_victory_rush() : SpellScriptLoader("spell_warr_victory_rush") { }

        class spell_warr_victory_rush_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_victory_rush_SpellScript);

            void HandleAfterCast()
            {
                GetCaster()->RemoveAurasDueToSpell(32216);
            }

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (AuraEffect* aur = GetCaster()->GetAuraEffect(82368, EFFECT_0))
                {
                    SetEffectDamage(aur->GetAmount());
                    aur->GetBase()->Remove();
                }
            }

            void Register()
            {
                BeforeCast += SpellCastFn(spell_warr_victory_rush_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_warr_victory_rush_SpellScript::HandleHeal, EFFECT_2, SPELL_EFFECT_HEAL_PCT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_victory_rush_SpellScript();
        }
};

// 32215 Victorious State
class spell_warr_victorious_state : public SpellScriptLoader
{
public:
    spell_warr_victorious_state() : SpellScriptLoader("spell_warr_victorious_state") { }

    class spell_warr_victorious_state_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_victorious_state_AuraScript);

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            if (auto caster = GetCaster())
                if (auto Pcaster = caster->ToPlayer())
                    if (!Pcaster->HasSpell(34428))
                        PreventDefaultAction();
        }

        void Register()
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_victorious_state_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_victorious_state_AuraScript();
    }
};

/// Updated 4.3.4
class spell_warr_thunder_clap : public SpellScriptLoader
{
    public:
        spell_warr_thunder_clap() : SpellScriptLoader("spell_warr_thunder_clap") { }

        class spell_warr_thunder_clap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_thunder_clap_SpellScript);

            bool Load()
            {
                int32 chance = 0;
                rendTarget = false;
                if (AuraEffect* aur = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_WARRIOR, 5057, EFFECT_0))
                    chance = aur->GetAmount();
                return roll_chance_i(chance);
            }

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                for (std::list<WorldObject*>::const_iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                    if (auto target = (*itr)->ToUnit())
                        if (target->HasAura(94009, GetCaster()->GetGUID()))
                        {
                            rendTarget = true;
                            return;
                        }
            }

            void HandleOnHit()
            {
                if (!rendTarget)
                    return;

                if (auto target = GetHitUnit())
                    if (auto caster = GetCaster())
                        caster->CastSpell(target, 94009, true);
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_thunder_clap_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnHit += SpellHitFn(spell_warr_thunder_clap_SpellScript::HandleOnHit);
            }
            bool rendTarget;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_thunder_clap_SpellScript();
        }
};

// 12328, 18765, 35429 - Sweeping Strikes
class spell_warr_sweeping_strikes : public SpellScriptLoader
{
    public:
        spell_warr_sweeping_strikes() : SpellScriptLoader("spell_warr_sweeping_strikes") { }

        class spell_warr_sweeping_strikes_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_sweeping_strikes_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK))
                    return false;
                return true;
            }

            bool Load()
            {
                _procTarget = NULL;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                _procTarget = eventInfo.GetActor()->SelectNearbyTarget(eventInfo.GetProcTarget());
                bool weaponCheck = eventInfo.GetActor()->ToPlayer()->GetWeaponForAttack(BASE_ATTACK) || eventInfo.GetActor()->ToPlayer()->GetWeaponForAttack(OFF_ATTACK);

                return weaponCheck && _procTarget
                    && !(eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_WARRIOR_EXECUTE && _procTarget->GetHealthPct() >= 20)
                    && eventInfo.GetActionTarget() != eventInfo.GetActor();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                int32 damage = eventInfo.GetDamageInfo()->GetDamage();

                GetTarget()->CastCustomSpell(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK, SPELLVALUE_BASE_POINT0, damage, _procTarget, true, NULL, aurEff);

                if (GetTarget()->HasAura(SPELL_WARRIOR_BLADESTORM_AURA) && !GetTarget()->HasSpellCooldown(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK))
                    GetTarget()->AddSpellCooldown(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK, 0, time(NULL) + 1);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_sweeping_strikes_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_warr_sweeping_strikes_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:
            Unit* _procTarget;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_sweeping_strikes_AuraScript();
        }
};

// 23920 - Spell reflect
class spell_warr_spell_reflect : public SpellScriptLoader
{
    public:
        spell_warr_spell_reflect() : SpellScriptLoader("spell_warr_spell_reflect") { }

        class spell_warr_spell_reflect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_spell_reflect_AuraScript);

            void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
            {
                if (AuraEffect* aur = GetUnitOwner()->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_GENERIC, 1463, EFFECT_1))
                {
                    uint32 triggerSpell = aur->GetSpellInfo()->Effects[EFFECT_1].TriggerSpell;
                    const SpellInfo* triggered = sSpellMgr->GetSpellInfo(triggerSpell);
                    if (!triggered)
                        return;

                    int32 bp0 = triggered->Effects[EFFECT_0].BasePoints * 4;
                    GetUnitOwner()->CastCustomSpell(GetUnitOwner(), triggerSpell, &bp0, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_warr_spell_reflect_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_REFLECT_SPELLS);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_spell_reflect_AuraScript();
        }
};

// 50720 - Vigilance
class spell_warr_vigilance : public SpellScriptLoader
{
    public:
        spell_warr_vigilance() : SpellScriptLoader("spell_warr_vigilance") { }

        class spell_warr_vigilance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_vigilance_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_VIGILANCE_PROC))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_VIGILANCE_REDIRECT_THREAT))
                    return false;
                return true;
            }

            bool Load()
            {
                _procTarget = NULL;
                return true;
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if (!target)
                    return;

                if (auto caster = GetCaster())
                    target->CastSpell(caster, SPELL_WARRIOR_VIGILANCE_REDIRECT_THREAT, true);
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto target = GetTarget())
                    target->ResetRedirectThreat();
            }

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                _procTarget = GetCaster();
                return _procTarget;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                if (auto caster = _procTarget->ToPlayer())
                {
                    if (caster->GetPrimaryTalentTree(caster->GetActiveSpec()) != TALENT_TREE_WARRIOR_PROTECTION)
                    {
                        Remove(AURA_REMOVE_BY_EXPIRE);
                        return;
                    }
                }

                GetTarget()->CastSpell(_procTarget, SPELL_WARRIOR_VIGILANCE_PROC, true, NULL, aurEff);

                // and you gain Vengeance as if 20% of the damage was done to you. 
                if (_procTarget->GetTypeId() == TYPEID_PLAYER && GetTarget()->GetDistance2d(_procTarget) <= 200.0f)
                {
                    float damage = eventInfo.GetDamageInfo()->GetDamage() 
                        + eventInfo.GetDamageInfo()->GetArmorMitigated() 
                        + eventInfo.GetDamageInfo()->GetAbsorb() 
                        + eventInfo.GetDamageInfo()->GetBlock() 
                        + eventInfo.GetDamageInfo()->GetResist();

                    damage = CalculatePct(damage, 20);

                    if (damage <= 0.0f)
                        return;

                    Aura* vengeance = _procTarget->GetAura(SPELL_GEN_VENGEANCE_PERIODIC);
                    bool justApplied = false;

                    if (!vengeance)
                    {
                        vengeance = _procTarget->AddAura(SPELL_GEN_VENGEANCE_PERIODIC, _procTarget);
                        justApplied = true;
                        if (!vengeance)
                            return;
                    }

                    if (AuraEffect* effect = vengeance->GetEffect(EFFECT_2))
                    {
                        uint32 oldAmount = effect->GetAmount();
                        uint32 amount = oldAmount + damage;
                        effect->SetAmount(amount);

                        if (justApplied)
                        {
                            AuraApplication* auraApp = vengeance->GetApplicationOfTarget(_procTarget->GetGUID());
                            if (auraApp)
                                effect->PeriodicTick(auraApp, _procTarget);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_warr_vigilance_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_warr_vigilance_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoCheckProc += AuraCheckProcFn(spell_warr_vigilance_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_warr_vigilance_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }

        private:
            Unit* _procTarget;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_vigilance_AuraScript();
        }
};

// 50725 Vigilance
class spell_warr_vigilance_trigger : public SpellScriptLoader
{
    public:
        spell_warr_vigilance_trigger() : SpellScriptLoader("spell_warr_vigilance_trigger") { }

        class spell_warr_vigilance_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_vigilance_trigger_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                // Remove Taunt cooldown
                if (auto target = GetHitPlayer())
                    target->RemoveSpellCooldown(SPELL_WARRIOR_TAUNT, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_vigilance_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_vigilance_trigger_SpellScript();
        }
};

// Rallying Cry
class spell_warr_rally : public SpellScriptLoader
{
    public:
        spell_warr_rally() : SpellScriptLoader("spell_warr_rally") { }

        class spell_warr_rally_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_rally_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetHitUnit()->CountPctFromMaxHealth(GetEffectValue());
                GetCaster()->CastCustomSpell(GetHitUnit(), 97463, &bp0, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_rally_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_rally_SpellScript();
        }
};

// Colossus smash
class spell_warr_colossus_smash : public SpellScriptLoader
{
    public:
        spell_warr_colossus_smash() : SpellScriptLoader("spell_warr_colossus_smash") { }

        class spell_warr_colossus_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_colossus_smash_SpellScript);

            void HandleGlyph(SpellEffIndex /*effIndex*/)
            {
                if (AuraEffect* glyph = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_WARRIOR, 5288, EFFECT_0))
                    GetCaster()->CastSpell(GetHitUnit(), 58567, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warr_colossus_smash_SpellScript::HandleGlyph, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        class spell_warr_colossus_smash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_colossus_smash_AuraScript);

            void CalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (auto target = GetUnitOwner())
                    if (target->GetTypeId() == TYPEID_PLAYER)
                        amount = 50;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_colossus_smash_AuraScript::CalcAmount, EFFECT_1, SPELL_AURA_BYPASS_ARMOR_FOR_CASTER);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_colossus_smash_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_colossus_smash_SpellScript();
        }
};

// Incite
class spell_warr_incite : public SpellScriptLoader
{
    public:
        spell_warr_incite() : SpellScriptLoader("spell_warr_incite") { }

        class spell_warr_incite_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warr_incite_AuraScript);

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                Unit* caster = GetCaster();
                return caster && !caster->HasAura(86627);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_warr_incite_AuraScript::CheckProc);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warr_incite_AuraScript();
        }
};

// Whirlwind
class spell_warr_whirlwind : public SpellScriptLoader
{
public:
    spell_warr_whirlwind() : SpellScriptLoader("spell_warr_whirlwind") { }

    class spell_warr_whirlwind_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_whirlwind_SpellScript);

        bool load()
        {
            reset = false;
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.size() > 3)
                reset = true;
        }

        void HandleAfterCast()
        {
            if (reset)
            {
                if (auto pl = GetCaster()->ToPlayer())
                {
                    int32 cooldown = GetSpellInfo()->Id;
                    int32 reducedCooldown = 1000 * GetSpellInfo()->Effects[EFFECT_2].BasePoints;
                    if (pl->HasSpellCooldown(cooldown))
                    {
                        uint32 newCooldownDelay = pl->GetSpellCooldownDelay(cooldown);
                        if (newCooldownDelay < uint32(reducedCooldown / 1000))
                            newCooldownDelay = 0;
                        else
                            newCooldownDelay -= uint32(reducedCooldown / 1000);
                        pl->AddSpellCooldown(cooldown, 0, uint32(time(NULL) + newCooldownDelay));

                        WorldPacket data(SMSG_MODIFY_COOLDOWN, 4+8+4);
                        data << uint32(cooldown);               // Spell ID
                        data << uint64(pl->GetGUID());          // Player GUID
                        data << int32(-reducedCooldown);        // Cooldown mod in milliseconds
                        pl->GetSession()->SendPacket(&data);
                    }
                }
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_whirlwind_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            AfterCast += SpellCastFn(spell_warr_whirlwind_SpellScript::HandleAfterCast);
        }

    private:
        bool reset;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_whirlwind_SpellScript();
    }
};

// Heroic leap building 6544
class spell_warr_heroic_leap_build : public SpellScriptLoader
{
public:
    spell_warr_heroic_leap_build() : SpellScriptLoader("spell_warr_heroic_leap_build") { }

    class spell_warr_heroic_leap_build_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_heroic_leap_build_SpellScript)

        SpellCastResult CheckElevation()
        {
            Unit* caster = GetCaster();
            WorldLocation const* const dest = GetExplTargetDest();

            float dZ = (dest->GetPositionZ() - caster->GetPositionZ());     //vertical edge of right triangle
            float travelDistance = caster->GetExactDist2d(dest);            //base edge of right triangle
                /*
                rule 1: any leaps under 5 yards vertical are allowed without questions.
                This is the only rule that existed before rules 2 and 3 were implemented.
                */
            if (dZ > 5.00f)
            {
                /*
                rule 2:
                Any leap higher than 5 yards and 45 degrees are not allowed.
                */
                if ((dZ * 2) > travelDistance)
                    return SPELL_FAILED_NOPATH;
                /*
                rule 3:
                between 45 and 22.5 degrees, there will be a path check that decides if this jump is allowed.
                This should be true for cases of a 45 degree ramp such as twin peaks alliance base,
                This should be false for the gigantic twin peaks rock outside the alliance graveyard.
                */
                if ((dZ * 4) > travelDistance)
                {
                    PathGenerator path(caster);
                    path.SetPathLengthLimit(GetSpellInfo()->GetMaxRange(true) * 1.500f);
                    if (!path.CalculatePath(dest->GetPositionX(), dest->GetPositionY(), dest->GetPositionZ()) || path.GetPathType() != PATHFIND_NORMAL)
                        return SPELL_FAILED_NOPATH;
                }
            }
            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_warr_heroic_leap_build_SpellScript::CheckElevation);
        }
    };

    SpellScript *GetSpellScript() const
    {
        return new spell_warr_heroic_leap_build_SpellScript();
    }
};

//

/// Updated 4.3.4
class spell_warr_shield_slam : public SpellScriptLoader
{
    public:
        spell_warr_shield_slam() : SpellScriptLoader("spell_warr_shield_slam") { }

        class spell_warr_shield_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warr_shield_slam_SpellScript);

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster && GetHitUnit())
                {
                    int32 baseDamage = int32(GetEffectValue() + caster->GetTotalAttackPowerValue(BASE_ATTACK));
                    SetEffectDamage(baseDamage);
                }
            }

            void Register()
            {
                OnEffectLaunchTarget += SpellEffectFn(spell_warr_shield_slam_SpellScript::HandleDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warr_shield_slam_SpellScript();
        }
};

// Glyph of Sunder Armor
class spell_warr_sunder_armor : public SpellScriptLoader
{
public:
    spell_warr_sunder_armor() : SpellScriptLoader("spell_warr_sunder_armor") { }

    class spell_warr_sunder_armor_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_sunder_armor_SpellScript);

        void HandleGlyph(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
                if (auto caster = GetCaster())
                    if (caster->HasAura(58387))
                        if (auto newTarget = caster->SelectNearbyTarget(target))
                            caster->CastSpell(newTarget, 58567, true);
        }


        void Register()
        {
            if (m_scriptSpellId == 7386)
                OnEffectHitTarget += SpellEffectFn(spell_warr_sunder_armor_SpellScript::HandleGlyph, EFFECT_0, SPELL_EFFECT_TRIGGER_SPELL);
            else
                OnEffectHitTarget += SpellEffectFn(spell_warr_sunder_armor_SpellScript::HandleGlyph, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_sunder_armor_SpellScript();
    }
};

class spell_warr_shield_block : public SpellScriptLoader
{
public:
    spell_warr_shield_block() : SpellScriptLoader("spell_warr_shield_block") { }

    class spell_warr_shield_block_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_shield_block_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            // Warrior T12 Protection 4P Bonus
            if (auto caster = GetCaster())
                if (caster->HasAura(99242) && caster->isAlive())
                    caster->CastSpell(caster, 99243, true);
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_warr_shield_block_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_BLOCK_PERCENT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_shield_block_AuraScript();
    }
};

class spell_warr_t12_2p_dd : public SpellScriptLoader
{
public:
    spell_warr_t12_2p_dd() : SpellScriptLoader("spell_warr_t12_2p_dd") { }

    class spell_warr_t12_2p_dd_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t12_2p_dd_AuraScript);

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            // Warrior T12 dd 2P Bonus
            float multiplier = 2.0f;
            if (auto caster = GetCaster())
            {
                if (caster->HasAura(12835)) // Booming Voice R2
                    multiplier = 1.0f;
                else if (caster->HasAura(12321)) // Booming Voice R1
                    multiplier = 1.5f;
            }
            SetMaxDuration(aurEff->GetSpellInfo()->GetMaxDuration() * multiplier);
            SetDuration(aurEff->GetSpellInfo()->GetDuration() * multiplier);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_warr_t12_2p_dd_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_t12_2p_dd_AuraScript();
    }
};

class spell_warr_t13_2p_protection : public SpellScriptLoader
{
public:
    spell_warr_t13_2p_protection() : SpellScriptLoader("spell_warr_t13_2p_protection") { }

    class spell_warr_t13_2p_protection_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t13_2p_protection_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SHIELD_OF_FURY))
                return false;
            return true;
        }

        void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            if (auto caster = eventInfo.GetActor())
            {
                int32 absorb = CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount());
                caster->CastCustomSpell(caster, SPELL_WARRIOR_SHIELD_OF_FURY, &absorb, NULL, NULL, true);
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_t13_2p_protection_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_t13_2p_protection_AuraScript();
    }
};

class spell_warr_shield_wall : public SpellScriptLoader
{
public:
    spell_warr_shield_wall() : SpellScriptLoader("spell_warr_shield_wall") { }

    class spell_warr_shield_wall_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_shield_wall_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SHIELD_FORTRESS))
                return false;
            return true;
        }

        void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto caster = GetCaster())
            {
                // Warrior T13 Protection 4P Bonus (Shield Wall)
                if (AuraEffect* t13_4p = caster->GetAuraEffect(105911, EFFECT_0, GetCasterGUID()))
                {
                    int32 shieldFortressAmount = aurEff->GetAmount() / 2;

                    if (AuraEffect* glyph = caster->GetAuraEffect(63329, EFFECT_1, GetCasterGUID()))
                        shieldFortressAmount = 30;

                    caster->CastCustomSpell(caster, SPELL_WARRIOR_SHIELD_FORTRESS, &shieldFortressAmount, NULL, NULL, true);
                }
            }
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_warr_shield_wall_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_shield_wall_AuraScript();
    }
};

class spell_warr_t13_4p_dd : public SpellScriptLoader
{
public:
    spell_warr_t13_4p_dd() : SpellScriptLoader("spell_warr_t13_4p_dd") { }

    class spell_warr_t13_4p_dd_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_t13_4p_dd_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (!eventInfo.GetDamageInfo() || !eventInfo.GetDamageInfo()->GetSpellInfo())
                return false;

            int32 procChance = 0;
            switch (eventInfo.GetDamageInfo()->GetSpellInfo()->Id)
            {
                case 23881:
                    procChance = 6;
                    break;
                case 12294:
                    procChance = 13;
                    break;
                default:
                    return false;
            }
            return roll_chance_i(procChance);
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_t13_4p_dd_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_t13_4p_dd_AuraScript();
    }
};

class spell_warr_battle_trance : public SpellScriptLoader
{
public:
    spell_warr_battle_trance() : SpellScriptLoader("spell_warr_battle_trance")
    {}

    class spell_warr_battle_trance_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_battle_trance_AuraScript);

        bool HandleProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetSpellInfo()->Id == 1464)
                return false;

            if (GetTarget() && eventInfo.GetSpellInfo()->Id == 50782 && GetTarget()->HasAura(46916))
                return false;

            return true;
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_battle_trance_AuraScript::HandleProc);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_warr_battle_trance_AuraScript();
    }
};

void AddSC_warrior_spell_scripts()
{
    new spell_warr_bloodthirst();
    new spell_warr_bloodthirst_heal();
    new spell_warr_charge();
    new spell_warr_concussion_blow();
    new spell_warr_deep_wounds();
    new spell_warr_execute();
    new spell_warr_intimidating_shout();
    new spell_warr_last_stand();
    new spell_warr_rend();
    new spell_warr_shattering_throw();
    new spell_warr_slam();
    new spell_slam_damage_mainhand();
    new spell_slam_damage_offhand();
    new spell_warr_sweeping_strikes();
    new spell_warr_vigilance();
    new spell_warr_vigilance_trigger();
    new spell_warr_intercept();
    new spell_warr_thunder_clap();
    new spell_warr_spell_reflect();
    new spell_warr_victory_rush();
    new spell_warr_victorious_state();
    new spell_warr_rally();
    new spell_warr_colossus_smash();
    new spell_warr_incite();
    new spell_warr_whirlwind();
    new spell_warr_second_wind();
    new spell_warr_heroic_leap_build();
    new spell_warr_shield_slam();
    new spell_warr_sunder_armor();
    new spell_warr_shield_block();
    new spell_warr_t12_2p_dd();
    new spell_warr_t13_2p_protection();
    new spell_warr_shield_wall();
    new spell_warr_t13_4p_dd();
    new spell_warr_battle_trance();
}
