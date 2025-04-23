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
 * Scripts for spells with SPELLFAMILY_GENERIC which cannot be included in AI script file
 * of creature using it or can't be bound to any player class.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_gen_"
 */

#include "ScriptMgr.h"
#include "Battleground.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Pet.h"
#include "ReputationMgr.h"
#include "SkillDiscovery.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"

class spell_gen_absorb0_hitlimit1 : public SpellScriptLoader
{
    public:
        spell_gen_absorb0_hitlimit1() : SpellScriptLoader("spell_gen_absorb0_hitlimit1") { }

        class spell_gen_absorb0_hitlimit1_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_absorb0_hitlimit1_AuraScript);

            uint32 limit;

            bool Load()
            {
                // Max absorb stored in 1 dummy effect
                limit = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
            {
                absorbAmount = std::min(limit, absorbAmount);
            }

            void Register()
            {
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb0_hitlimit1_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_absorb0_hitlimit1_AuraScript();
        }
};

// 28764 - Adaptive Warding (Frostfire Regalia Set)
enum AdaptiveWarding
{
    SPELL_GEN_ADAPTIVE_WARDING_FIRE     = 28765,
    SPELL_GEN_ADAPTIVE_WARDING_NATURE   = 28768,
    SPELL_GEN_ADAPTIVE_WARDING_FROST    = 28766,
    SPELL_GEN_ADAPTIVE_WARDING_SHADOW   = 28769,
    SPELL_GEN_ADAPTIVE_WARDING_ARCANE   = 28770
};

class spell_gen_adaptive_warding : public SpellScriptLoader
{
    public:
        spell_gen_adaptive_warding() : SpellScriptLoader("spell_gen_adaptive_warding") { }

        class spell_gen_adaptive_warding_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_adaptive_warding_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_FIRE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_NATURE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_FROST) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_SHADOW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_ADAPTIVE_WARDING_ARCANE))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                if (eventInfo.GetDamageInfo()->GetSpellInfo()) // eventInfo.GetSpellInfo()
                    return false;

                // find Mage Armor
                if (!GetTarget()->GetAuraEffect(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT, SPELLFAMILY_MAGE, 0x10000000, 0x0, 0x0))
                    return false;

                switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
                {
                    case SPELL_SCHOOL_NORMAL:
                    case SPELL_SCHOOL_HOLY:
                        return false;
                    default:
                        break;
                }
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                uint32 spellId = 0;
                switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
                {
                    case SPELL_SCHOOL_FIRE:
                        spellId = SPELL_GEN_ADAPTIVE_WARDING_FIRE;
                        break;
                    case SPELL_SCHOOL_NATURE:
                        spellId = SPELL_GEN_ADAPTIVE_WARDING_NATURE;
                        break;
                    case SPELL_SCHOOL_FROST:
                        spellId = SPELL_GEN_ADAPTIVE_WARDING_FROST;
                        break;
                    case SPELL_SCHOOL_SHADOW:
                        spellId = SPELL_GEN_ADAPTIVE_WARDING_SHADOW;
                        break;
                    case SPELL_SCHOOL_ARCANE:
                        spellId = SPELL_GEN_ADAPTIVE_WARDING_ARCANE;
                        break;
                    default:
                        return;
                }
                GetTarget()->CastSpell(GetTarget(), spellId, true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_gen_adaptive_warding_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_gen_adaptive_warding_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_adaptive_warding_AuraScript();
        }
};

// 41337 Aura of Anger
class spell_gen_aura_of_anger : public SpellScriptLoader
{
    public:
        spell_gen_aura_of_anger() : SpellScriptLoader("spell_gen_aura_of_anger") { }

        class spell_gen_aura_of_anger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_aura_of_anger_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
                    aurEff1->ChangeAmount(aurEff1->GetAmount() + 5);
                aurEff->SetAmount(100 * aurEff->GetTickNumber());
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_aura_of_anger_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_aura_of_anger_AuraScript();
        }
};

class spell_gen_av_drekthar_presence : public SpellScriptLoader
{
    public:
        spell_gen_av_drekthar_presence() : SpellScriptLoader("spell_gen_av_drekthar_presence") { }

        class spell_gen_av_drekthar_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_av_drekthar_presence_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                switch (target->GetEntry())
                {
                    // alliance
                    case 14762: // Dun Baldar North Marshal
                    case 14763: // Dun Baldar South Marshal
                    case 14764: // Icewing Marshal
                    case 14765: // Stonehearth Marshal
                    case 11948: // Vandar Stormspike
                    // horde
                    case 14772: // East Frostwolf Warmaster
                    case 14776: // Tower Point Warmaster
                    case 14773: // Iceblood Warmaster
                    case 14777: // West Frostwolf Warmaster
                    case 11946: // Drek'thar
                        return true;
                    default:
                        return false;
                        break;
                }
            }
            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_drekthar_presence_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_av_drekthar_presence_AuraScript();
        }
};

// 46394 Brutallus Burn
class spell_gen_burn_brutallus : public SpellScriptLoader
{
    public:
        spell_gen_burn_brutallus() : SpellScriptLoader("spell_gen_burn_brutallus") { }

        class spell_gen_burn_brutallus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_burn_brutallus_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (aurEff->GetTickNumber() % 11 == 0)
                    aurEff->SetAmount(aurEff->GetAmount() * 2);
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_burn_brutallus_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_burn_brutallus_AuraScript();
        }
};

enum CannibalizeSpells
{
    SPELL_CANNIBALIZE_TRIGGERED = 20578,
};

class spell_gen_cannibalize : public SpellScriptLoader
{
    public:
        spell_gen_cannibalize() : SpellScriptLoader("spell_gen_cannibalize") { }

        class spell_gen_cannibalize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cannibalize_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CANNIBALIZE_TRIGGERED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, SPELL_CANNIBALIZE_TRIGGERED, false);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cannibalize_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_gen_cannibalize_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cannibalize_SpellScript();
        }
};

class spell_gen_flag_of_ownership : public SpellScriptLoader
{//INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('51640', 'spell_flag_of_ownership');
public:
    spell_gen_flag_of_ownership() : SpellScriptLoader("spell_gen_flag_of_ownership") { }

    class spell_gen_flag_of_ownership_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_flag_of_ownership_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(51640))
                return false;
            return true;
        }
        Player* closestCorpsePlayer = NULL;
        Unit* corpseTarget()
        {
            Unit* caster = GetCaster();

            Unit* closestCorpse = NULL;
            std::list<Unit*> targets;
            Trinity::AnyUnfriendlyDeadUnitInObjectRangeCheck u_check(caster, caster, 5.0f);
            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyDeadUnitInObjectRangeCheck> searcher(caster, targets, u_check);
            caster->VisitNearbyObject(5.0f, searcher);

            if (targets.size())
                for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end();)
                {
                    Unit* temp = (*itr);
                    itr++;
                    if (temp != NULL)
                        if ((temp)->ToPlayer() && (temp->GetGUID() != caster->GetGUID()))
                            if (!closestCorpse || caster->GetDistance(closestCorpse) > caster->GetDistance(temp))
                            {
                                closestCorpse = temp;
                                closestCorpsePlayer = temp->ToPlayer();
                            }
                }
            return closestCorpse;
        }

        SpellCastResult CheckIfCorpseNear()
        {
            if (corpseTarget())
            {
                SetExplTargetUnit(corpseTarget());
                return SPELL_CAST_OK;
            }

            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NO_NEARBY_CORPSES);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        void doEmote()
        {
            if (GetCaster())
                if (auto casterPlayer = GetCaster()->ToPlayer())
                    if (closestCorpsePlayer)
                    {
                        std::string _text = "has placed the Flag of Ownership in " + closestCorpsePlayer->GetName() + "'s corpse.";
                        sScriptMgr->OnPlayerChat(casterPlayer, CHAT_MSG_EMOTE, LANG_UNIVERSAL, _text);
                        WorldPacket data(SMSG_MESSAGECHAT, 200);
                        casterPlayer->BuildPlayerChat(&data, CHAT_MSG_EMOTE, _text, LANG_UNIVERSAL, "");
                        casterPlayer->SendMessageToSetInRange(&data, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), true, false);
                    }
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_flag_of_ownership_SpellScript::CheckIfCorpseNear);
            AfterCast += SpellCastFn(spell_gen_flag_of_ownership_SpellScript::doEmote);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_flag_of_ownership_SpellScript();
    }
};

// 63845 - Create Lance
enum CreateLanceSpells
{
    SPELL_CREATE_LANCE_ALLIANCE = 63914,
    SPELL_CREATE_LANCE_HORDE    = 63919
};

class spell_gen_create_lance : public SpellScriptLoader
{
    public:
        spell_gen_create_lance() : SpellScriptLoader("spell_gen_create_lance") { }

        class spell_gen_create_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_create_lance_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_LANCE_ALLIANCE) || !sSpellMgr->GetSpellInfo(SPELL_CREATE_LANCE_HORDE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (auto target = GetHitPlayer())
                {
                    if (target->GetTeam() == ALLIANCE)
                        GetCaster()->CastSpell(target, SPELL_CREATE_LANCE_ALLIANCE, true);
                    else
                        GetCaster()->CastSpell(target, SPELL_CREATE_LANCE_HORDE, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_create_lance_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_create_lance_SpellScript();
        }
};

// 28702 - Netherbloom
enum Netherbloom
{
    SPELL_NETHERBLOOM_POLLEN_1 = 28703
};

class spell_gen_netherbloom : public SpellScriptLoader
{
    public:
        spell_gen_netherbloom() : SpellScriptLoader("spell_gen_netherbloom") { }

        class spell_gen_netherbloom_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_netherbloom_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                for (uint8 i = 0; i < 5; ++i)
                    if (!sSpellMgr->GetSpellInfo(SPELL_NETHERBLOOM_POLLEN_1 + i))
                        return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (auto target = GetHitUnit())
                {
                    // 25% chance of casting a random buff
                    if (roll_chance_i(75))
                        return;

                    // triggered spells are 28703 to 28707
                    // Note: some sources say, that there was the possibility of
                    //       receiving a debuff. However, this seems to be removed by a patch.

                    // don't overwrite an existing aura
                    for (uint8 i = 0; i < 5; ++i)
                        if (target->HasAura(SPELL_NETHERBLOOM_POLLEN_1 + i))
                            return;

                    target->CastSpell(target, SPELL_NETHERBLOOM_POLLEN_1 + urand(0, 4), true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_netherbloom_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_netherbloom_SpellScript();
        }
};

// 28720 - Nightmare Vine
enum NightmareVine
{
    SPELL_NIGHTMARE_POLLEN  = 28721
};

class spell_gen_nightmare_vine : public SpellScriptLoader
{
    public:
        spell_gen_nightmare_vine() : SpellScriptLoader("spell_gen_nightmare_vine") { }

        class spell_gen_nightmare_vine_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_nightmare_vine_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_NIGHTMARE_POLLEN))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (auto target = GetHitUnit())
                {
                    // 25% chance of casting Nightmare Pollen
                    if (roll_chance_i(25))
                        target->CastSpell(target, SPELL_NIGHTMARE_POLLEN, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_nightmare_vine_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_nightmare_vine_SpellScript();
        }
};

// 27539 - Obsidian Armor
enum ObsidianArmor
{
    SPELL_GEN_OBSIDIAN_ARMOR_HOLY       = 27536,
    SPELL_GEN_OBSIDIAN_ARMOR_FIRE       = 27533,
    SPELL_GEN_OBSIDIAN_ARMOR_NATURE     = 27538,
    SPELL_GEN_OBSIDIAN_ARMOR_FROST      = 27534,
    SPELL_GEN_OBSIDIAN_ARMOR_SHADOW     = 27535,
    SPELL_GEN_OBSIDIAN_ARMOR_ARCANE     = 27540
};

class spell_gen_obsidian_armor : public SpellScriptLoader
{
    public:
        spell_gen_obsidian_armor() : SpellScriptLoader("spell_gen_obsidian_armor") { }

        class spell_gen_obsidian_armor_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_obsidian_armor_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_HOLY) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_FIRE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_NATURE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_FROST) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_SHADOW) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_OBSIDIAN_ARMOR_ARCANE))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                if (eventInfo.GetDamageInfo()->GetSpellInfo()) // eventInfo.GetSpellInfo()
                    return false;

                if (GetFirstSchoolInMask(eventInfo.GetSchoolMask()) == SPELL_SCHOOL_NORMAL)
                    return false;

                return true;
            }

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                uint32 spellId = 0;
                switch (GetFirstSchoolInMask(eventInfo.GetSchoolMask()))
                {
                    case SPELL_SCHOOL_HOLY:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_HOLY;
                        break;
                    case SPELL_SCHOOL_FIRE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_FIRE;
                        break;
                    case SPELL_SCHOOL_NATURE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_NATURE;
                        break;
                    case SPELL_SCHOOL_FROST:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_FROST;
                        break;
                    case SPELL_SCHOOL_SHADOW:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_SHADOW;
                        break;
                    case SPELL_SCHOOL_ARCANE:
                        spellId = SPELL_GEN_OBSIDIAN_ARMOR_ARCANE;
                        break;
                    default:
                        return;
                }
                GetTarget()->CastSpell(GetTarget(), spellId, true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_gen_obsidian_armor_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_gen_obsidian_armor_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_obsidian_armor_AuraScript();
        }
};

// 45472 Parachute
enum ParachuteSpells
{
    SPELL_PARACHUTE         = 45472,
    SPELL_PARACHUTE_BUFF    = 44795,
};

class spell_gen_parachute : public SpellScriptLoader
{
    public:
        spell_gen_parachute() : SpellScriptLoader("spell_gen_parachute") { }

        class spell_gen_parachute_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PARACHUTE) || !sSpellMgr->GetSpellInfo(SPELL_PARACHUTE_BUFF))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (auto target = GetTarget()->ToPlayer())
                    if (target->IsFalling())
                    {
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
                        target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
                    }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_AuraScript();
        }
};

enum NPCEntries
{
    NPC_DOOMGUARD   = 11859,
    NPC_INFERNAL    = 89,
    NPC_IMP         = 416,
};

class spell_gen_pet_summoned : public SpellScriptLoader
{
    public:
        spell_gen_pet_summoned() : SpellScriptLoader("spell_gen_pet_summoned") { }

        class spell_gen_pet_summoned_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pet_summoned_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetCaster()->ToPlayer();
                PetData* t_pet;
                if (player->GetLastPetId())
                {
                    PetType newPetType = (player->getClass() == CLASS_HUNTER) ? HUNTER_PET : SUMMON_PET;
                    if (Pet* newPet = new Pet(player, newPetType))
                    {
                        if (player->getClass() == CLASS_HUNTER)
                            t_pet = player->GetPetDatabySlot(player->GetLastPetSlot());
                        else
                            t_pet = player->GetPetDatabySpell(player->GetLastPetNumber());
                        if (newPet->LoadPet(player, t_pet, true))
                        {
                            // revive the pet if it is dead
                            if (newPet->getDeathState() == DEAD)
                                newPet->setDeathState(ALIVE);

                            newPet->SetFullHealth();
                            newPet->SetPower(newPet->getPowerType(), newPet->GetMaxPower(newPet->getPowerType()));

                            if (newPet->GetEntry() == NPC_INFERNAL)
                                newPet->SetEntry(NPC_IMP);
                        }
                        else
                            delete newPet;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_pet_summoned_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pet_summoned_SpellScript();
        }
};

class spell_gen_remove_flight_auras : public SpellScriptLoader
{
    public:
        spell_gen_remove_flight_auras() : SpellScriptLoader("spell_gen_remove_flight_auras") {}

        class spell_gen_remove_flight_auras_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_remove_flight_auras_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (auto target = GetHitUnit())
                {
                    target->RemoveAurasByType(SPELL_AURA_FLY);
                    target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_remove_flight_auras_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_remove_flight_auras_SpellScript();
        }
};

enum EluneCandle
{
    NPC_OMEN                       = 15467,

    SPELL_ELUNE_CANDLE_OMEN_HEAD   = 26622,
    SPELL_ELUNE_CANDLE_OMEN_CHEST  = 26624,
    SPELL_ELUNE_CANDLE_OMEN_HAND_R = 26625,
    SPELL_ELUNE_CANDLE_OMEN_HAND_L = 26649,
    SPELL_ELUNE_CANDLE_NORMAL      = 26636,
};

class spell_gen_elune_candle : public SpellScriptLoader
{
    public:
        spell_gen_elune_candle() : SpellScriptLoader("spell_gen_elune_candle") {}

        class spell_gen_elune_candle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_elune_candle_SpellScript);
            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HEAD))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_CHEST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_R))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_L))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_NORMAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = 0;

                if (GetHitUnit()->GetEntry() == NPC_OMEN)
                {
                    switch (urand(0, 3))
                    {
                        case 0: spellId = SPELL_ELUNE_CANDLE_OMEN_HEAD; break;
                        case 1: spellId = SPELL_ELUNE_CANDLE_OMEN_CHEST; break;
                        case 2: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_R; break;
                        case 3: spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_L; break;
                    }
                }
                else
                    spellId = SPELL_ELUNE_CANDLE_NORMAL;

                GetCaster()->CastSpell(GetHitUnit(), spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_elune_candle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_elune_candle_SpellScript();
        }
};

class spell_idle_BgRemoval : public SpellScriptLoader
{
public:
    spell_idle_BgRemoval() : SpellScriptLoader("spell_idle_BgRemoval") { }
    class spell_idle_BgRemoval_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_idle_BgRemoval_AuraScript);
        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (auto target = GetTarget()->ToPlayer()) {
                target->AddAura(43681, target); // prevents falling damage
            }
        }
        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (!GetAura()->IsExpired())
                return;
            if (auto target = GetTarget()) {
                if (target->ToPlayer()) {
                    target->ToPlayer()->ToggleAFK();
                }
                else {
                    TC_LOG_ERROR("entities.unit", "Unit::RemoveOwnedAura: unit with fading inactivity aura was not a player.");
                }

            }
            else {
                TC_LOG_ERROR("entities.unit", "no target.");
            }
        }
        void Register()
        {
            OnEffectApply += AuraEffectRemoveFn(spell_idle_BgRemoval_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_CURRENCY_GAIN, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_idle_BgRemoval_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_MOD_CURRENCY_GAIN, AURA_EFFECT_HANDLE_REAL);
        }
    };
    AuraScript* GetAuraScript() const
    {
        return new spell_idle_BgRemoval_AuraScript();
    }
};

class spell_creature_permanent_feign_death : public SpellScriptLoader
{
    public:
        spell_creature_permanent_feign_death() : SpellScriptLoader("spell_creature_permanent_feign_death") { }

        class spell_creature_permanent_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_creature_permanent_feign_death_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto target = GetTarget())
                {
                    target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->ToCreature()->SetReactState(REACT_PASSIVE);
                }
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto target = GetTarget())
                {
                    target->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->ToCreature()->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectApplyFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_creature_permanent_feign_death_AuraScript();
        }
};

enum PvPTrinketTriggeredSpells
{
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER         = 72752,
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF    = 72757,
};

class spell_pvp_trinket_wotf_shared_cd : public SpellScriptLoader
{
    public:
        spell_pvp_trinket_wotf_shared_cd() : SpellScriptLoader("spell_pvp_trinket_wotf_shared_cd") {}

        class spell_pvp_trinket_wotf_shared_cd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pvp_trinket_wotf_shared_cd_SpellScript);

            void HandleCooldown()
            {
                Unit* caster = GetCaster();
                switch (GetSpellInfo()->Id)
                {
                    // PvP Trinket
                    case 42292:
                        caster->CastSpell(caster, SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER, false);
                        break;
                    // PvP Trinket
                    case 7744:
                        caster->CastSpell(caster, SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF, false);
                        break;
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_pvp_trinket_wotf_shared_cd_SpellScript::HandleCooldown);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pvp_trinket_wotf_shared_cd_SpellScript();
        }
};

enum AnimalBloodPoolSpell
{
    SPELL_ANIMAL_BLOOD      = 46221,
    SPELL_SPAWN_BLOOD_POOL  = 63471,
};

class spell_gen_animal_blood : public SpellScriptLoader
{
    public:
        spell_gen_animal_blood() : SpellScriptLoader("spell_gen_animal_blood") { }

        class spell_gen_animal_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_animal_blood_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SPAWN_BLOOD_POOL))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Remove all auras with spell id 46221, except the one currently being applied
                while (Aura* aur = GetUnitOwner()->GetOwnedAura(SPELL_ANIMAL_BLOOD, 0, 0, 0, GetAura()))
                    GetUnitOwner()->RemoveOwnedAura(aur);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto owner = GetUnitOwner())
                    if (owner->IsInWater())
                        owner->CastSpell(owner, SPELL_SPAWN_BLOOD_POOL, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_animal_blood_AuraScript();
        }
};

enum DivineStormSpell
{
    SPELL_DIVINE_STORM  = 53385,
};

// 70769 Divine Storm!
class spell_gen_divine_storm_cd_reset : public SpellScriptLoader
{
    public:
        spell_gen_divine_storm_cd_reset() : SpellScriptLoader("spell_gen_divine_storm_cd_reset") {}

        class spell_gen_divine_storm_cd_reset_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_divine_storm_cd_reset_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DIVINE_STORM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (caster->HasSpellCooldown(SPELL_DIVINE_STORM))
                    caster->RemoveSpellCooldown(SPELL_DIVINE_STORM, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_divine_storm_cd_reset_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_divine_storm_cd_reset_SpellScript();
        }
};

class spell_gen_gunship_portal : public SpellScriptLoader
{
    public:
        spell_gen_gunship_portal() : SpellScriptLoader("spell_gen_gunship_portal") { }

        class spell_gen_gunship_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gunship_portal_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleScript()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (auto bg = caster->GetBattleground())
                    if (bg->GetTypeID(true) == BATTLEGROUND_IC)
                        bg->DoAction(1, caster->GetGUID());
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_gen_gunship_portal_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gunship_portal_SpellScript();
        }
};

enum parachuteIC
{
    SPELL_PARACHUTE_IC = 66657,
};

class spell_gen_parachute_ic : public SpellScriptLoader
{
    public:
        spell_gen_parachute_ic() : SpellScriptLoader("spell_gen_parachute_ic") { }

        class spell_gen_parachute_ic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_ic_AuraScript)

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (auto target = GetTarget()->ToPlayer())
                    if (target->m_movementInfo.fallTime > 2000)
                        target->CastSpell(target, SPELL_PARACHUTE_IC, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_ic_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_ic_AuraScript();
        }
};

class spell_gen_dungeon_credit : public SpellScriptLoader
{
    public:
        spell_gen_dungeon_credit() : SpellScriptLoader("spell_gen_dungeon_credit") { }

        class spell_gen_dungeon_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dungeon_credit_SpellScript);

            bool Load()
            {
                _handled = false;
                return GetCaster()->GetTypeId() == TYPEID_UNIT;
            }

            void CreditEncounter()
            {
                // This hook is executed for every target, make sure we only credit instance once
                if (_handled)
                    return;

                _handled = true;
                Unit* caster = GetCaster();
                if (auto instance = caster->GetInstanceScript())
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, GetSpellInfo()->Id, caster);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_gen_dungeon_credit_SpellScript::CreditEncounter);
            }

            bool _handled;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dungeon_credit_SpellScript();
        }
};

class spell_gen_profession_research : public SpellScriptLoader
{
    public:
        spell_gen_profession_research() : SpellScriptLoader("spell_gen_profession_research") {}

        class spell_gen_profession_research_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_profession_research_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_TO_DISCOVER);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                uint32 spellId = GetSpellInfo()->Id;

                // learn random explicit discovery recipe (if any)
                if (uint32 discoveredSpellId = GetExplicitDiscoverySpell(spellId, caster))
                    caster->learnSpell(discoveredSpellId, false);

                caster->UpdateCraftSkill(spellId);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_profession_research_SpellScript::CheckRequirement);
                OnEffectHitTarget += SpellEffectFn(spell_gen_profession_research_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_profession_research_SpellScript();
        }
};

enum CloneWeaponSpells
{
    SPELL_COPY_WEAPON_AURA       = 41054,
    SPELL_COPY_WEAPON_2_AURA     = 63418,
    SPELL_COPY_WEAPON_3_AURA     = 69893,

    SPELL_COPY_OFFHAND_AURA      = 45205,
    SPELL_COPY_OFFHAND_2_AURA    = 69896,

    SPELL_COPY_RANGED_AURA       = 57594
};

class spell_generic_clone_weapon : public SpellScriptLoader
{
    public:
        spell_generic_clone_weapon() : SpellScriptLoader("spell_generic_clone_weapon") { }

        class spell_generic_clone_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_generic_clone_weapon_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();

                if (auto target = GetHitUnit())
                {

                    uint32 spellId = uint32(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
                    caster->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_generic_clone_weapon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_generic_clone_weapon_SpellScript();
        }
};

class spell_gen_clone_weapon_aura : public SpellScriptLoader
{
    public:
        spell_gen_clone_weapon_aura() : SpellScriptLoader("spell_gen_clone_weapon_aura") { }

        class spell_gen_clone_weapon_auraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_clone_weapon_auraScript);

            uint32 prevItem;

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_2_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_3_AURA)
                    || !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_2_AURA) || !sSpellMgr->GetSpellInfo(SPELL_COPY_RANGED_AURA))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();

                if (!caster)
                    return;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID);

                        if (auto player = caster->ToPlayer())
                        {
                            if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, mainItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID));
                        break;
                    }
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) + 1;

                        if (auto player = caster->ToPlayer())
                        {
                            if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, offItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1));
                        break;
                    }
                    case SPELL_COPY_RANGED_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) + 2;

                        if (auto player = caster->ToPlayer())
                        {
                            if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, rangedItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2));
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                    {
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, prevItem);
                        break;
                    }
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                    {
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, prevItem);
                        break;
                    }
                    case SPELL_COPY_RANGED_AURA:
                    {
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, prevItem);
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_clone_weapon_auraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_clone_weapon_auraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_clone_weapon_auraScript();
        }
};

enum FriendOrFowl
{
    SPELL_TURKEY_VENGEANCE  = 25285,
};

class spell_gen_turkey_marker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_marker() : SpellScriptLoader("spell_gen_turkey_marker") { }

        class spell_gen_turkey_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_turkey_marker_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // store stack apply times, so we can pop them while they expire
                _applyTimes.push_back(getMSTime());
                Unit* target = GetTarget();

                // on stack 15 cast the achievement crediting spell
                if (GetStackAmount() >= 15)
                    target->CastSpell(target, SPELL_TURKEY_VENGEANCE, true, NULL, aurEff, GetCasterGUID());
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (_applyTimes.empty())
                    return;

                // pop stack if it expired for us
                if (_applyTimes.front() + GetMaxDuration() < getMSTime())
                    ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

            std::list<uint32> _applyTimes;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_turkey_marker_AuraScript();
        }
};

class spell_gen_lifeblood : public SpellScriptLoader
{
    public:
        spell_gen_lifeblood() : SpellScriptLoader("spell_gen_lifeblood") { }

        class spell_gen_lifeblood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifeblood_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (auto owner = GetUnitOwner())
                    amount += int32(CalculatePct(owner->GetMaxHealth(), 1.5f / aurEff->GetTotalTicks()));
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_lifeblood_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_lifeblood_AuraScript();
        }
};

class spell_gen_angerforge : public SpellScriptLoader
{
    public:
        spell_gen_angerforge() : SpellScriptLoader("spell_gen_angerforge") { }

        class spell_gen_angerforge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_angerforge_SpellScript);

            SpellCastResult CheckRequirement()
            {
                Aura* rawFury = GetCaster()->GetAura(91832);
                if (!rawFury || rawFury->GetStackAmount() != rawFury->GetSpellInfo()->StackAmount)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void HandleOnHit()
            {
                GetHitUnit()->RemoveAurasDueToSpell(91832);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_angerforge_SpellScript::HandleOnHit);
                OnCheckCast += SpellCheckCastFn(spell_gen_angerforge_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_angerforge_SpellScript();
        }
};

class spell_gen_ignacious : public SpellScriptLoader
{
    public:
        spell_gen_ignacious() : SpellScriptLoader("spell_gen_ignacious") { }

        class spell_gen_ignacious_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_ignacious_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (auto caster = GetCaster())
                {
                    switch (GetId())
                    {
                        case 91041:
                            if (Aura*stacks = caster->GetAura(91027))
                                amount = amount * stacks->GetStackAmount();
                            caster->RemoveAurasDueToSpell(91027);
                            break;
                        case 92328:
                            if (Aura*stacks = caster->GetAura(92325))
                                amount = amount * stacks->GetStackAmount();
                            caster->RemoveAurasDueToSpell(92325);
                            break;
                    }
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_ignacious_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_RATING);
            }
        };

        class spell_gen_ignacious_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ignacious_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCaster()->HasAura(91027) && !GetCaster()->HasAura(92325))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_ignacious_SpellScript::CheckRequirement);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_ignacious_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_ignacious_SpellScript();
        }
};

class spell_gen_scales_of_life : public SpellScriptLoader
{
    public:
        spell_gen_scales_of_life() : SpellScriptLoader("spell_gen_scales_of_life") { }

        class spell_gen_scales_of_life_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_scales_of_life_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCaster()->HasAura(96881))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                if (AuraEffect* tracker = GetCaster()->GetAuraEffect(96881, EFFECT_0))
                    SetHitHeal(tracker->GetAmount());

                GetHitUnit()->RemoveAurasDueToSpell(96881);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_scales_of_life_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_HEAL);
                OnCheckCast += SpellCheckCastFn(spell_gen_scales_of_life_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_scales_of_life_SpellScript();
        }
};

class spell_gen_apparatus_of_khaz : public SpellScriptLoader
{
    public:
        spell_gen_apparatus_of_khaz() : SpellScriptLoader("spell_gen_apparatus_of_khaz") { }

        class spell_gen_apparatus_of_khaz_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_apparatus_of_khaz_SpellScript);

            void HandleOnHit(SpellEffIndex /*effIndex*/)
            {
                if (Aura* tracker = GetCaster()->GetAura(96923))
                {
                    int32 bp0 = tracker->GetStackAmount() * GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                    if (auto caster = GetCaster()->ToPlayer())
                    {
                        uint32 mastery = caster->GetRatingBonusValue(CR_MASTERY);
                        uint32 haste = caster->GetRatingBonusValue(CR_HASTE_MELEE);
                        uint32 crit = caster->GetRatingBonusValue(CR_CRIT_MELEE);
                        if (mastery > haste && mastery > crit)
                            caster->CastCustomSpell(caster, 96929, &bp0, NULL, NULL, true);
                        else if (haste > mastery && haste > crit)
                            caster->CastCustomSpell(caster, 96927, &bp0, NULL, NULL, true);
                        else if (crit > haste && crit > mastery)
                            caster->CastCustomSpell(caster, 96928, &bp0, NULL, NULL, true);
                    }
                    GetCaster()->RemoveAurasDueToSpell(96923);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_apparatus_of_khaz_SpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_apparatus_of_khaz_SpellScript();
        }
};

class spell_gen_allow_cast_from_item_only : public SpellScriptLoader
{
    public:
        spell_gen_allow_cast_from_item_only() : SpellScriptLoader("spell_gen_allow_cast_from_item_only") { }

        class spell_gen_allow_cast_from_item_only_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_allow_cast_from_item_only_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCastItem())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_allow_cast_from_item_only_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_allow_cast_from_item_only_SpellScript();
        }
};

enum Launch
{
    SPELL_LAUNCH_NO_FALLING_DAMAGE = 66251
};

class spell_gen_launch : public SpellScriptLoader
{
    public:
        spell_gen_launch() : SpellScriptLoader("spell_gen_launch") {}

        class spell_gen_launch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_launch_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (auto player = GetHitPlayer())
                    player->AddAura(SPELL_LAUNCH_NO_FALLING_DAMAGE, player); // prevents falling damage
            }

            void Launch()
            {
                WorldLocation const* const position = GetExplTargetDest();

                if (auto player = GetHitPlayer())
                {
                    // A better research is needed
                    // There is no spell for this, the following calculation was based on void Spell::CalculateJumpSpeeds

                    float speedZ = 10.0f;
                    float dist = position->GetExactDist2d(player->GetPositionX(), player->GetPositionY());
                    float speedXY = dist;

                    player->JumpWithDelay(1500, position->GetPositionX(), position->GetPositionY(), position->GetPositionZ(), speedXY, speedZ, 0);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_launch_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
                AfterHit += SpellHitFn(spell_gen_launch_SpellScript::Launch);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_launch_SpellScript();
        }
};

enum VehicleScaling
{
    SPELL_GEAR_SCALING      = 66668,
    SPELL_GEAR_SCALING_2    = 65636,
};

class spell_gen_vehicle_scaling : public SpellScriptLoader
{
    public:
        spell_gen_vehicle_scaling() : SpellScriptLoader("spell_gen_vehicle_scaling") { }

        class spell_gen_vehicle_scaling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vehicle_scaling_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                float factor;
                uint16 baseItemLevel;

                // TODO: Research coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                    case SPELL_GEAR_SCALING_2:
                    default:
                        factor = 0.8f;
                        baseItemLevel = 352;
                        break;
                }

                float avgILvl = caster->ToPlayer()->GetAverageItemLevel();
                if (avgILvl < baseItemLevel)
                    return;                     // TODO: Research possibility of scaling down

                amount = uint16((avgILvl - baseItemLevel) * factor);
            }

            void CalculateDamagePctAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                float factor;
                uint16 baseItemLevel;

                // TODO: Research coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                        factor = 1.0f;
                        baseItemLevel = 348;
                        break;
                    case SPELL_GEAR_SCALING_2:
                        factor = 7.0f;
                        baseItemLevel = 289;
                        break;
                    default:
                        factor = 1.0f;
                        baseItemLevel = 289;
                        break;
                }

                float avgILvl = caster->ToPlayer()->GetAverageItemLevel();
                if (avgILvl < baseItemLevel)
                    return;                     // TODO: Research possibility of scaling down

                amount = uint16((avgILvl - baseItemLevel) * factor);
            }

            void Register()
            {
                if (m_scriptSpellId != 91401 && m_scriptSpellId != 91405)
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
                else
                    DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_DONE_PERCENT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateDamagePctAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_vehicle_scaling_AuraScript();
        }
};


class spell_gen_oracle_wolvar_reputation : public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 factionId = GetSpellInfo()->Effects[effIndex].CalcValue();
                int32  repChange =  GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                FactionEntry const* factionEntry = sFactionStore.LookupEntry(factionId);

                if (!factionEntry)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(factionEntry) < repChange)
                    player->GetReputationMgr().SetReputation(factionEntry, repChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};

enum DummyTrigger
{
    SPELL_PERSISTANT_SHIELD_TRIGGERED       = 26470,
    SPELL_PERSISTANT_SHIELD                 = 26467,
};

class spell_gen_dummy_trigger : public SpellScriptLoader
{
    public:
        spell_gen_dummy_trigger() : SpellScriptLoader("spell_gen_dummy_trigger") { }

        class spell_gen_dummy_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dummy_trigger_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (auto target = GetHitUnit())
                    if (SpellInfo const* triggeredByAuraSpell = GetTriggeringSpell())
                        if (triggeredByAuraSpell->Id == SPELL_PERSISTANT_SHIELD_TRIGGERED)
                            caster->CastCustomSpell(target, SPELL_PERSISTANT_SHIELD_TRIGGERED, &damage, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dummy_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dummy_trigger_SpellScript();
        }

};

class spell_gen_spirit_healer_res : public SpellScriptLoader
{
    public:
        spell_gen_spirit_healer_res(): SpellScriptLoader("spell_gen_spirit_healer_res") { }

        class spell_gen_spirit_healer_res_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spirit_healer_res_SpellScript);

            bool Load()
            {
                return GetOriginalCaster() && GetOriginalCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Player* originalCaster = GetOriginalCaster()->ToPlayer();
                if (auto target = GetHitUnit())
                {
                    WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                    data << uint64(target->GetGUID());
                    originalCaster->GetSession()->SendPacket(&data);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spirit_healer_res_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_spirit_healer_res_SpellScript();
        }
};

enum TransporterBackfires
{
    SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH     = 23444,
    SPELL_TRANSPORTER_EVIL_TWIN                 = 23445,
    SPELL_TRANSPORTER_MALFUNCTION_MISS          = 36902,
};

class spell_gen_gadgetzan_transporter_backfire : public SpellScriptLoader
{
    public:
        spell_gen_gadgetzan_transporter_backfire() : SpellScriptLoader("spell_gen_gadgetzan_transporter_backfire") { }

        class spell_gen_gadgetzan_transporter_backfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gadgetzan_transporter_backfire_SpellScript)

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_EVIL_TWIN)
                    || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_MISS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                int32 r = irand(0, 119);
                if (r < 20)                           // Transporter Malfunction - 1/6 polymorph
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH, true);
                else if (r < 100)                     // Evil Twin               - 4/6 evil twin
                    caster->CastSpell(caster, SPELL_TRANSPORTER_EVIL_TWIN, true);
                else                                    // Transporter Malfunction - 1/6 miss the target
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_MISS, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gadgetzan_transporter_backfire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gadgetzan_transporter_backfire_SpellScript();
        }
};

enum GnomishTransporter
{
    SPELL_TRANSPORTER_SUCCESS                   = 23441,
    SPELL_TRANSPORTER_FAILURE                   = 23446,
};

class spell_gen_gnomish_transporter : public SpellScriptLoader
{
    public:
        spell_gen_gnomish_transporter() : SpellScriptLoader("spell_gen_gnomish_transporter") { }

        class spell_gen_gnomish_transporter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gnomish_transporter_SpellScript)

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_FAILURE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(50) ? SPELL_TRANSPORTER_SUCCESS : SPELL_TRANSPORTER_FAILURE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gnomish_transporter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gnomish_transporter_SpellScript();
        }
};

enum DalaranDisguiseSpells
{
    SPELL_SUNREAVER_DISGUISE_TRIGGER       = 69672,
    SPELL_SUNREAVER_DISGUISE_FEMALE        = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE          = 70974,

    SPELL_SILVER_COVENANT_DISGUISE_TRIGGER = 69673,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE  = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE    = 70972,
};

class spell_gen_dalaran_disguise : public SpellScriptLoader
{
    public:
        spell_gen_dalaran_disguise(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_dalaran_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dalaran_disguise_SpellScript);
            bool Validate(SpellInfo const* spellEntry)
            {
                switch (spellEntry->Id)
                {
                    case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_MALE))
                            return false;
                        break;
                    case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                            return false;
                        break;
                }
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (auto player = GetHitPlayer())
                {
                    uint8 gender = player->getGender();

                    uint32 spellId = GetSpellInfo()->Id;

                    switch (spellId)
                    {
                        case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SUNREAVER_DISGUISE_FEMALE : SPELL_SUNREAVER_DISGUISE_MALE;
                            break;
                        case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SILVER_COVENANT_DISGUISE_FEMALE : SPELL_SILVER_COVENANT_DISGUISE_MALE;
                            break;
                        default:
                            break;
                    }
                    GetCaster()->CastSpell(player, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dalaran_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dalaran_disguise_SpellScript();
        }
};

/* DOCUMENTATION: Break-Shield spells
    Break-Shield spells can be classified in three groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: NONE
            + EFFECT_2: NONE
        - Spells casted by players triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: FORCE_CAST
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE

    In the following script we handle the SCRIPT_EFFECT for effIndex EFFECT_0 and EFFECT_1.
        - When handling EFFECT_0 we're in the "Spells on vehicle bar used by players" case
          and we'll trigger "Spells casted by players triggered by script"
        - When handling EFFECT_1 we're in the "Spells casted by players triggered by script"
          or "Spells casted by NPCs on players" so we'll search for the first defend layer and drop it.
*/

enum BreakShieldSpells
{
    SPELL_BREAK_SHIELD_DAMAGE_2K                 = 62626,
    SPELL_BREAK_SHIELD_DAMAGE_10K                = 64590,

    SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS    = 62575, // Also on ToC5 mounts
    SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE = 64595,
    SPELL_BREAK_SHIELD_TRIGGER_UNK               = 66480,
};

class spell_gen_break_shield: public SpellScriptLoader
{
    public:
        spell_gen_break_shield(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_break_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_break_shield_SpellScript)

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_BREAK_SHIELD_TRIGGER_UNK:
                            case SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_10K;
                                break;
                            case SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_2K;
                                break;
                            default:
                                return;
                        }

                        if (auto rider = GetCaster()->GetCharmer())
                            rider->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (auto rider = target->GetCharmer())
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_break_shield_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_break_shield_SpellScript();
        }
};

/* DOCUMENTATION: Charge spells
    Charge spells can be classified in four groups:

        - Spells on vehicle bar used by players:
            + EFFECT_0: SCRIPT_EFFECT
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: NONE
        - Spells casted by player's mounts triggered by script:
            + EFFECT_0: CHARGE
            + EFFECT_1: TRIGGER_SPELL
            + EFFECT_2: APPLY_AURA
        - Spells casted by players on the target triggered by script:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: SCRIPT_EFFECT
            + EFFECT_2: NONE
        - Spells casted by NPCs on players:
            + EFFECT_0: SCHOOL_DAMAGE
            + EFFECT_1: CHARGE
            + EFFECT_2: SCRIPT_EFFECT

    In the following script we handle the SCRIPT_EFFECT and CHARGE
        - When handling SCRIPT_EFFECT:
            + EFFECT_0: Corresponds to "Spells on vehicle bar used by players" and we make player's mount cast
              the charge effect on the current target ("Spells casted by player's mounts triggered by script").
            + EFFECT_1 and EFFECT_2: Triggered when "Spells casted by player's mounts triggered by script" hits target,
              corresponding to "Spells casted by players on the target triggered by script" and "Spells casted by
              NPCs on players" and we check Defend layers and drop a charge of the first found.
        - When handling CHARGE:
            + Only launched for "Spells casted by player's mounts triggered by script", makes the player cast the
              damaging spell on target with a small chance of failing it.
*/

enum ChargeSpells
{
    SPELL_CHARGE_DAMAGE_8K5             = 62874,
    SPELL_CHARGE_DAMAGE_20K             = 68498,
    SPELL_CHARGE_DAMAGE_45K             = 64591,

    SPELL_CHARGE_CHARGING_EFFECT_8K5    = 63661,
    SPELL_CHARGE_CHARGING_EFFECT_20K_1  = 68284,
    SPELL_CHARGE_CHARGING_EFFECT_20K_2  = 68501,
    SPELL_CHARGE_CHARGING_EFFECT_45K_1  = 62563,
    SPELL_CHARGE_CHARGING_EFFECT_45K_2  = 66481,

    SPELL_CHARGE_TRIGGER_FACTION_MOUNTS = 62960,
    SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION = 68282,

    SPELL_CHARGE_MISS_EFFECT            = 62977,
};

class spell_gen_mounted_charge: public SpellScriptLoader
{
    public:
        spell_gen_mounted_charge() : SpellScriptLoader("spell_gen_mounted_charge") { }

        class spell_gen_mounted_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mounted_charge_SpellScript)

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_20K_1;
                                break;
                            case SPELL_CHARGE_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_8K5;
                                break;
                            default:
                                return;
                        }

                        // If target isn't a training dummy there's a chance of failing the charge
                        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) && roll_chance_f(12.5f))
                            spellId = SPELL_CHARGE_MISS_EFFECT;

                        if (auto vehicle = GetCaster()->GetVehicleBase())
                            vehicle->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    case EFFECT_2:
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (auto rider = target->GetCharmer())
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CHARGE_CHARGING_EFFECT_8K5:
                        spellId = SPELL_CHARGE_DAMAGE_8K5;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_2:
                        spellId = SPELL_CHARGE_DAMAGE_20K;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_2:
                        spellId = SPELL_CHARGE_DAMAGE_45K;
                        break;
                    default:
                        return;
                }

                if (auto rider = GetCaster()->GetCharmer())
                    rider->CastSpell(GetHitUnit(), spellId, false);
                else
                    GetCaster()->CastSpell(GetHitUnit(), spellId, false);
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                if (spell->HasEffect(SPELL_EFFECT_SCRIPT_EFFECT))
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);

                if (spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_CHARGE)
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_mounted_charge_SpellScript();
        }
};

enum DefendVisuals
{
    SPELL_VISUAL_SHIELD_1 = 63130,
    SPELL_VISUAL_SHIELD_2 = 63131,
    SPELL_VISUAL_SHIELD_3 = 63132,
};

class spell_gen_defend : public SpellScriptLoader
{
    public:
        spell_gen_defend() : SpellScriptLoader("spell_gen_defend") { }

        class spell_gen_defend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_defend_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_2))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_3))
                    return false;
                return true;
            }

            void RefreshVisualShields(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                {
                    Unit* target = GetTarget();

                    for (uint32 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                        target->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);

                    target->CastSpell(target, SPELL_VISUAL_SHIELD_1 + GetAura()->GetStackAmount() - 1, true, NULL, aurEff);
                }
                else
                    GetTarget()->RemoveAurasDueToSpell(GetId());
            }

            void RemoveVisualShields(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                for (uint32 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                    GetTarget()->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
            }

            void RemoveDummyFromDriver(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto caster = GetCaster())
                    if (TempSummon* vehicle = caster->ToTempSummon())
                        if (auto rider = vehicle->GetSummoner())
                            rider->RemoveAurasDueToSpell(GetId());
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                // Defend spells casted by NPCs (add visuals)
                if (spell->Effects[EFFECT_0].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }

                // Remove Defend spell from player when he dismounts
                if (spell->Effects[EFFECT_2].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveDummyFromDriver, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);

                // Defend spells casted by players (add/remove visuals)
                if (spell->Effects[EFFECT_1].ApplyAuraName == SPELL_AURA_DUMMY)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_defend_AuraScript();
        }
};

enum MountedDuelSpells
{
    SPELL_ON_TOURNAMENT_MOUNT = 63034,
    SPELL_MOUNTED_DUEL        = 62875,
};

class spell_gen_tournament_duel : public SpellScriptLoader
{
    public:
        spell_gen_tournament_duel() : SpellScriptLoader("spell_gen_tournament_duel") { }

        class spell_gen_tournament_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tournament_duel_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ON_TOURNAMENT_MOUNT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNTED_DUEL))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (auto rider = GetCaster()->GetCharmer())
                {
                    if (auto plrTarget = GetHitPlayer())
                    {
                        if (plrTarget->HasAura(SPELL_ON_TOURNAMENT_MOUNT) && plrTarget->GetVehicleBase())
                            rider->CastSpell(plrTarget, SPELL_MOUNTED_DUEL, true);
                    }
                    else if (auto unitTarget = GetHitUnit())
                    {
                        if (unitTarget->GetCharmer() && unitTarget->GetCharmer()->GetTypeId() == TYPEID_PLAYER && unitTarget->GetCharmer()->HasAura(SPELL_ON_TOURNAMENT_MOUNT))
                            rider->CastSpell(unitTarget->GetCharmer(), SPELL_MOUNTED_DUEL, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tournament_duel_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_tournament_duel_SpellScript();
        }
};

enum TournamentMountsSpells
{
    SPELL_LANCE_EQUIPPED = 62853,
};

class spell_gen_summon_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_summon_tournament_mount() : SpellScriptLoader("spell_gen_summon_tournament_mount") { }

        class spell_gen_summon_tournament_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_summon_tournament_mount_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LANCE_EQUIPPED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfLanceEquiped()
            {
                if (GetCaster()->IsInDisallowedMountForm())
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (!GetCaster()->HasAura(SPELL_LANCE_EQUIPPED))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_summon_tournament_mount_SpellScript::CheckIfLanceEquiped);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_summon_tournament_mount_SpellScript();
        }
};

enum TournamentPennantSpells
{
    SPELL_PENNANT_STORMWIND_ASPIRANT      = 62595,
    SPELL_PENNANT_STORMWIND_VALIANT       = 62596,
    SPELL_PENNANT_STORMWIND_CHAMPION      = 62594,
    SPELL_PENNANT_GNOMEREGAN_ASPIRANT     = 63394,
    SPELL_PENNANT_GNOMEREGAN_VALIANT      = 63395,
    SPELL_PENNANT_GNOMEREGAN_CHAMPION     = 63396,
    SPELL_PENNANT_SEN_JIN_ASPIRANT        = 63397,
    SPELL_PENNANT_SEN_JIN_VALIANT         = 63398,
    SPELL_PENNANT_SEN_JIN_CHAMPION        = 63399,
    SPELL_PENNANT_SILVERMOON_ASPIRANT     = 63401,
    SPELL_PENNANT_SILVERMOON_VALIANT      = 63402,
    SPELL_PENNANT_SILVERMOON_CHAMPION     = 63403,
    SPELL_PENNANT_DARNASSUS_ASPIRANT      = 63404,
    SPELL_PENNANT_DARNASSUS_VALIANT       = 63405,
    SPELL_PENNANT_DARNASSUS_CHAMPION      = 63406,
    SPELL_PENNANT_EXODAR_ASPIRANT         = 63421,
    SPELL_PENNANT_EXODAR_VALIANT          = 63422,
    SPELL_PENNANT_EXODAR_CHAMPION         = 63423,
    SPELL_PENNANT_IRONFORGE_ASPIRANT      = 63425,
    SPELL_PENNANT_IRONFORGE_VALIANT       = 63426,
    SPELL_PENNANT_IRONFORGE_CHAMPION      = 63427,
    SPELL_PENNANT_UNDERCITY_ASPIRANT      = 63428,
    SPELL_PENNANT_UNDERCITY_VALIANT       = 63429,
    SPELL_PENNANT_UNDERCITY_CHAMPION      = 63430,
    SPELL_PENNANT_ORGRIMMAR_ASPIRANT      = 63431,
    SPELL_PENNANT_ORGRIMMAR_VALIANT       = 63432,
    SPELL_PENNANT_ORGRIMMAR_CHAMPION      = 63433,
    SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT  = 63434,
    SPELL_PENNANT_THUNDER_BLUFF_VALIANT   = 63435,
    SPELL_PENNANT_THUNDER_BLUFF_CHAMPION  = 63436,
    SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT = 63606,
    SPELL_PENNANT_ARGENT_CRUSADE_VALIANT  = 63500,
    SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION = 63501,
    SPELL_PENNANT_EBON_BLADE_ASPIRANT     = 63607,
    SPELL_PENNANT_EBON_BLADE_VALIANT      = 63608,
    SPELL_PENNANT_EBON_BLADE_CHAMPION     = 63609,
};

enum TournamentMounts
{
    NPC_STORMWIND_STEED             = 33217,
    NPC_IRONFORGE_RAM               = 33316,
    NPC_GNOMEREGAN_MECHANOSTRIDER   = 33317,
    NPC_EXODAR_ELEKK                = 33318,
    NPC_DARNASSIAN_NIGHTSABER       = 33319,
    NPC_ORGRIMMAR_WOLF              = 33320,
    NPC_DARK_SPEAR_RAPTOR           = 33321,
    NPC_THUNDER_BLUFF_KODO          = 33322,
    NPC_SILVERMOON_HAWKSTRIDER      = 33323,
    NPC_FORSAKEN_WARHORSE           = 33324,
    NPC_ARGENT_WARHORSE             = 33782,
    NPC_ARGENT_STEED_ASPIRANT       = 33845,
    NPC_ARGENT_HAWKSTRIDER_ASPIRANT = 33844,
};

enum TournamentQuestsAchievements
{
    ACHIEVEMENT_CHAMPION_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMPION_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMPION_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMPION_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMPION_THE_EXODAR    = 2778,
    ACHIEVEMENT_CHAMPION_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMPION_SEN_JIN       = 2784,
    ACHIEVEMENT_CHAMPION_THUNDER_BLUFF = 2786,
    ACHIEVEMENT_CHAMPION_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMPION_SILVERMOON    = 2785,
    ACHIEVEMENT_ARGENT_VALOR           = 2758,
    ACHIEVEMENT_CHAMPION_ALLIANCE      = 2782,
    ACHIEVEMENT_CHAMPION_HORDE         = 2788,

    QUEST_VALIANT_OF_STORMWIND         = 13593,
    QUEST_A_VALIANT_OF_STORMWIND       = 13684,
    QUEST_VALIANT_OF_DARNASSUS         = 13706,
    QUEST_A_VALIANT_OF_DARNASSUS       = 13689,
    QUEST_VALIANT_OF_IRONFORGE         = 13703,
    QUEST_A_VALIANT_OF_IRONFORGE       = 13685,
    QUEST_VALIANT_OF_GNOMEREGAN        = 13704,
    QUEST_A_VALIANT_OF_GNOMEREGAN      = 13688,
    QUEST_VALIANT_OF_THE_EXODAR        = 13705,
    QUEST_A_VALIANT_OF_THE_EXODAR      = 13690,
    QUEST_VALIANT_OF_ORGRIMMAR         = 13707,
    QUEST_A_VALIANT_OF_ORGRIMMAR       = 13691,
    QUEST_VALIANT_OF_SEN_JIN           = 13708,
    QUEST_A_VALIANT_OF_SEN_JIN         = 13693,
    QUEST_VALIANT_OF_THUNDER_BLUFF     = 13709,
    QUEST_A_VALIANT_OF_THUNDER_BLUFF   = 13694,
    QUEST_VALIANT_OF_UNDERCITY         = 13710,
    QUEST_A_VALIANT_OF_UNDERCITY       = 13695,
    QUEST_VALIANT_OF_SILVERMOON        = 13711,
    QUEST_A_VALIANT_OF_SILVERMOON      = 13696,
};

class spell_gen_on_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_on_tournament_mount() : SpellScriptLoader("spell_gen_on_tournament_mount") { }

        class spell_gen_on_tournament_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_on_tournament_mount_AuraScript);

            uint32 _pennantSpellId;

            bool Load()
            {
                _pennantSpellId = 0;
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto caster = GetCaster())
                {
                    if (auto vehicle = caster->GetVehicleBase())
                    {
                        _pennantSpellId = GetPennatSpellId(caster->ToPlayer(), vehicle);
                        caster->CastSpell(caster, _pennantSpellId, true);
                    }
                }
            }

            void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto caster = GetCaster())
                    caster->RemoveAurasDueToSpell(_pennantSpellId);
            }

            uint32 GetPennatSpellId(Player* player, Unit* mount)
            {
                switch (mount->GetEntry())
                {
                    case NPC_ARGENT_STEED_ASPIRANT:
                    case NPC_STORMWIND_STEED:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_STORMWIND) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_VALIANT;
                        else
                            return SPELL_PENNANT_STORMWIND_ASPIRANT;
                    }
                    case NPC_GNOMEREGAN_MECHANOSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_GNOMEREGAN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_VALIANT;
                        else
                            return SPELL_PENNANT_GNOMEREGAN_ASPIRANT;
                    }
                    case NPC_DARK_SPEAR_RAPTOR:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SEN_JIN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_VALIANT;
                        else
                            return SPELL_PENNANT_SEN_JIN_ASPIRANT;
                    }
                    case NPC_ARGENT_HAWKSTRIDER_ASPIRANT:
                    case NPC_SILVERMOON_HAWKSTRIDER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SILVERMOON) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_VALIANT;
                        else
                            return SPELL_PENNANT_SILVERMOON_ASPIRANT;
                    }
                    case NPC_DARNASSIAN_NIGHTSABER:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_DARNASSUS) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_VALIANT;
                        else
                            return SPELL_PENNANT_DARNASSUS_ASPIRANT;
                    }
                    case NPC_EXODAR_ELEKK:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THE_EXODAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_VALIANT;
                        else
                            return SPELL_PENNANT_EXODAR_ASPIRANT;
                    }
                    case NPC_IRONFORGE_RAM:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_IRONFORGE) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_VALIANT;
                        else
                            return SPELL_PENNANT_IRONFORGE_ASPIRANT;
                    }
                    case NPC_FORSAKEN_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_UNDERCITY) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_VALIANT;
                        else
                            return SPELL_PENNANT_UNDERCITY_ASPIRANT;
                    }
                    case NPC_ORGRIMMAR_WOLF:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_ORGRIMMAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_VALIANT;
                        else
                            return SPELL_PENNANT_ORGRIMMAR_ASPIRANT;
                    }
                    case NPC_THUNDER_BLUFF_KODO:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THUNDER_BLUFF) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_VALIANT;
                        else
                            return SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT;
                    }
                    case NPC_ARGENT_WARHORSE:
                    {
                        if (player->HasAchieved(ACHIEVEMENT_CHAMPION_ALLIANCE) || player->HasAchieved(ACHIEVEMENT_CHAMPION_HORDE))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_CHAMPION : SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION;
                        else if (player->HasAchieved(ACHIEVEMENT_ARGENT_VALOR))
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_VALIANT : SPELL_PENNANT_ARGENT_CRUSADE_VALIANT;
                        else
                            return player->getClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_ASPIRANT : SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT;
                    }
                    default:
                        return 0;
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_on_tournament_mount_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_on_tournament_mount_AuraScript::HandleRemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_on_tournament_mount_AuraScript();
        }
};

class spell_gen_tournament_pennant : public SpellScriptLoader
{
    public:
        spell_gen_tournament_pennant() : SpellScriptLoader("spell_gen_tournament_pennant") { }

        class spell_gen_tournament_pennant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_tournament_pennant_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto caster = GetCaster())
                    if (!caster->GetVehicleBase())
                        caster->RemoveAurasDueToSpell(GetId());
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_tournament_pennant_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_tournament_pennant_AuraScript();
        }
};

enum ChaosBlast
{
    SPELL_CHAOS_BLAST   = 37675,
};

class spell_gen_chaos_blast : public SpellScriptLoader
{
    public:
        spell_gen_chaos_blast() : SpellScriptLoader("spell_gen_chaos_blast") { }

        class spell_gen_chaos_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_chaos_blast_SpellScript)

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHAOS_BLAST))
                    return false;
                return true;
            }
            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                int32 basepoints0 = 100;
                Unit* caster = GetCaster();
                if (auto target = GetHitUnit())
                    caster->CastCustomSpell(target, SPELL_CHAOS_BLAST, &basepoints0, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_chaos_blast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_chaos_blast_SpellScript();
        }

};

class spell_gen_wg_water : public SpellScriptLoader
{
    public:
        spell_gen_wg_water() : SpellScriptLoader("spell_gen_wg_water") {}

        class spell_gen_wg_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wg_water_SpellScript);

            SpellCastResult CheckCast()
            {
                if (!GetSpellInfo()->CheckTargetCreatureType(GetCaster()))
                    return SPELL_FAILED_DONT_REPORT;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wg_water_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_wg_water_SpellScript();
        }
};

class spell_gen_count_pct_from_max_hp : public SpellScriptLoader
{
    public:
        spell_gen_count_pct_from_max_hp(char const* name, int32 damagePct = 0) : SpellScriptLoader(name), _damagePct(damagePct) { }

        class spell_gen_count_pct_from_max_hp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_count_pct_from_max_hp_SpellScript)

        public:
            spell_gen_count_pct_from_max_hp_SpellScript(int32 damagePct) : SpellScript(), _damagePct(damagePct) { }

            void RecalculateDamage()
            {
                if (!_damagePct)
                    _damagePct = GetHitDamage();

                SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(_damagePct));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_count_pct_from_max_hp_SpellScript::RecalculateDamage);
            }

        private:
            int32 _damagePct;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_count_pct_from_max_hp_SpellScript(_damagePct);
        }

    private:
        int32 _damagePct;
};

class spell_gen_despawn_self : public SpellScriptLoader
{
public:
    spell_gen_despawn_self() : SpellScriptLoader("spell_gen_despawn_self") { }

    class spell_gen_despawn_self_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_despawn_self_SpellScript);

        bool Load()
        {
            return GetCaster()->GetTypeId() == TYPEID_UNIT;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
            if (GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_DUMMY || GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_SCRIPT_EFFECT)
                GetCaster()->ToCreature()->DespawnOrUnsummon();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_despawn_self_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_despawn_self_SpellScript();
    }
};

class spell_gen_touch_the_nightmare : public SpellScriptLoader
{
public:
    spell_gen_touch_the_nightmare() : SpellScriptLoader("spell_gen_touch_the_nightmare") { }

    class spell_gen_touch_the_nightmare_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_touch_the_nightmare_SpellScript);

        void HandleDamageCalc(SpellEffIndex /*effIndex*/)
        {
            uint32 bp = GetCaster()->GetMaxHealth() * 0.3f;
            SetHitDamage(bp);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_touch_the_nightmare_SpellScript::HandleDamageCalc, EFFECT_2, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_touch_the_nightmare_SpellScript();
    }
};

class spell_gen_dream_funnel: public SpellScriptLoader
{
public:
    spell_gen_dream_funnel() : SpellScriptLoader("spell_gen_dream_funnel") { }

    class spell_gen_dream_funnel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_dream_funnel_AuraScript);

        void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
        {
            if (GetCaster())
                amount = GetCaster()->GetMaxHealth() * 0.05f;

            canBeRecalculated = false;
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_dream_funnel_AuraScript::HandleEffectCalcAmount, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_dream_funnel_AuraScript();
    }
};

enum GenericBandage
{
    SPELL_RECENTLY_BANDAGED = 11196,
};

class spell_gen_bandage : public SpellScriptLoader
{
    public:
        spell_gen_bandage() : SpellScriptLoader("spell_gen_bandage") { }

        class spell_gen_bandage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bandage_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RECENTLY_BANDAGED))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (auto target = GetExplTargetUnit())
                {
                    if (target->HasAura(SPELL_RECENTLY_BANDAGED))
                        return SPELL_FAILED_TARGET_AURASTATE;
                }
                return SPELL_CAST_OK;
            }

            void HandleScript()
            {
                if (auto target = GetHitUnit())
                    GetCaster()->CastSpell(target, SPELL_RECENTLY_BANDAGED, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_bandage_SpellScript::CheckCast);
                AfterHit += SpellHitFn(spell_gen_bandage_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bandage_SpellScript();
        }
};

enum GenericLifebloom
{
    SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL        = 43422,
    SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL              = 52552,
    SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL           = 53692,
    SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL           = 57763,
    SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL    = 66094,
};

class spell_gen_lifebloom : public SpellScriptLoader
{
    public:
        spell_gen_lifebloom(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifebloom_AuraScript);

        public:
            spell_gen_lifebloom_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                // final heal
                GetTarget()->CastSpell(GetTarget(), _spellId, true, NULL, aurEff, GetCasterGUID());
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_lifebloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_lifebloom_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum SummonElemental
{
    SPELL_SUMMON_FIRE_ELEMENTAL  = 8985,
    SPELL_SUMMON_EARTH_ELEMENTAL = 19704
};

class spell_gen_summon_elemental : public SpellScriptLoader
{
    public:
        spell_gen_summon_elemental(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_summon_elemental_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_summon_elemental_AuraScript);

        public:
            spell_gen_summon_elemental_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (auto owner = GetCaster()->GetOwner())
                        owner->CastSpell(owner, _spellId, true);
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (auto owner = GetCaster()->GetOwner())
                        if (owner->GetTypeId() == TYPEID_PLAYER) // todo: this check is maybe wrong
                            owner->ToPlayer()->RemovePet(NULL, PET_SLOT_ACTUAL_PET_SLOT, true);
            }

            void Register()
            {
                 AfterEffectApply += AuraEffectApplyFn(spell_gen_summon_elemental_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                 AfterEffectRemove += AuraEffectRemoveFn(spell_gen_summon_elemental_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_summon_elemental_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum FoamSword
{
    ITEM_FOAM_SWORD_GREEN   = 45061,
    ITEM_FOAM_SWORD_PINK    = 45176,
    ITEM_FOAM_SWORD_BLUE    = 45177,
    ITEM_FOAM_SWORD_RED     = 45178,
    ITEM_FOAM_SWORD_YELLOW  = 45179,

    SPELL_BONKED            = 62991,
    SPELL_FOAM_SWORD_DEFEAT = 62994,
    SPELL_ON_GUARD          = 62972,
};

class spell_gen_upper_deck_create_foam_sword : public SpellScriptLoader
{
    public:
        spell_gen_upper_deck_create_foam_sword() : SpellScriptLoader("spell_gen_upper_deck_create_foam_sword") { }

        class spell_gen_upper_deck_create_foam_sword_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_upper_deck_create_foam_sword_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (auto player = GetHitPlayer())
                {
                    static uint32 const itemId[5] = { ITEM_FOAM_SWORD_GREEN, ITEM_FOAM_SWORD_PINK, ITEM_FOAM_SWORD_BLUE, ITEM_FOAM_SWORD_RED, ITEM_FOAM_SWORD_YELLOW };
                    // player can only have one of these items
                    for (uint8 i = 0; i < 5; ++i)
                    {
                        if (player->HasItemCount(itemId[i], 1, true))
                            return;
                    }

                    CreateItem(effIndex, itemId[urand(0, 4)]);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_upper_deck_create_foam_sword_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_upper_deck_create_foam_sword_SpellScript();
        }
};

class spell_gen_bonked : public SpellScriptLoader
{
    public:
        spell_gen_bonked() : SpellScriptLoader("spell_gen_bonked") { }

        class spell_gen_bonked_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bonked_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (auto target = GetHitPlayer())
                {
                    Aura const* aura = GetHitAura();
                    if (!(aura && aura->GetStackAmount() == 3))
                        return;

                    target->CastSpell(target, SPELL_FOAM_SWORD_DEFEAT, true);
                    target->RemoveAurasDueToSpell(SPELL_BONKED);

                    if (Aura const* aura = target->GetAura(SPELL_ON_GUARD))
                    {
                        if (Item* item = target->GetItemByGuid(aura->GetCastItemGUID()))
                            target->DestroyItemCount(item->GetEntry(), 1, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_bonked_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bonked_SpellScript();
        }
};

class spell_gen_increase_stats_buff : public SpellScriptLoader
{
    public:
        spell_gen_increase_stats_buff(char const* scriptName) : SpellScriptLoader(scriptName) { }

        class spell_gen_increase_stats_buff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_increase_stats_buff_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->IsInRaidWith(GetCaster()))
                {
                    uint32 newValue = GetEffectValue() == 79039 ? GetEffectValue() - 1 : GetEffectValue() + 1;
                    GetCaster()->CastSpell(GetCaster(), newValue, true); // raid buff
                }
                else
                    GetCaster()->CastSpell(GetHitUnit(), GetEffectValue(), true); // single-target buff
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_increase_stats_buff_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_increase_stats_buff_SpellScript();
        }
};

class spell_gen_replenishment : public SpellScriptLoader
{
    public:
        spell_gen_replenishment() : SpellScriptLoader("spell_gen_replenishment") { }

        class spell_gen_replenishment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_replenishment_AuraScript);

            bool Load()
            {
                return GetUnitOwner()->GetPower(POWER_MANA);
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                amount = GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.001f;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_replenishment_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_replenishment_AuraScript();
        }
};

enum RunningWildMountIds
{
    RUNNING_WILD_MODEL_MALE     = 29422,
    RUNNING_WILD_MODEL_FEMALE   = 29423,
    SPELL_ALTERED_FORM          = 97709,
};

class spell_gen_running_wild : public SpellScriptLoader
{
    public:
        spell_gen_running_wild() : SpellScriptLoader("spell_gen_running_wild") { }

        class spell_gen_running_wild_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_running_wild_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sCreatureDisplayInfoStore.LookupEntry(RUNNING_WILD_MODEL_MALE))
                    return false;
                if (!sCreatureDisplayInfoStore.LookupEntry(RUNNING_WILD_MODEL_FEMALE))
                    return false;
                return true;
            }

            void HandleMount(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                PreventDefaultAction();


                if (auto p = target->ToPlayer())
                    if (auto bg = p->GetBattleground())
                        if (!p->InArena())
                            if (!p->IsPlayingNative())
                            {
                                target->RemoveAura(GetSpellInfo()->Id);
                                target->AddAura(63635, target);
                                return;
                            }

                target->Mount(target->getGender() == GENDER_FEMALE ? RUNNING_WILD_MODEL_FEMALE : RUNNING_WILD_MODEL_MALE, 0, 0);

                // cast speed aura
                if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(aurEff->GetAmount()))
                    target->CastSpell(target, mountCapability->SpeedModSpell, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_running_wild_AuraScript::HandleMount, EFFECT_1, SPELL_AURA_MOUNTED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_gen_running_wild_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_running_wild_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ALTERED_FORM))
                    return false;
                return true;
            }

            bool Load()
            {
                // Definitely not a good thing, but currently the only way to do something at cast start
                // Should be replaced as soon as possible with a new hook: BeforeCastStart
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
                return false;
            }

            void Register()
            {
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_running_wild_AuraScript();
        }

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_running_wild_SpellScript();
        }
};

enum nokaled_Ids
{
    NOKALED_HC_PROC = 109873,
    NOKALED_HC_ITEM = 78472,

    NOKALED_NM_PROC = 107786,
    NOKALED_NM_ITEM = 77188,

    NOKALED_LFR_PROC = 109866,
    NOKALED_LFR_ITEM = 78481
};

class spell_gen_nokaled : public SpellScriptLoader
{
    /*
    INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('109873', 'spell_gen_nokaled'); -- HEROIC
    INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('107786', 'spell_gen_nokaled'); -- NORMAL
    INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('109866', 'spell_gen_nokaled'); -- LFRAID
    */
public:
    spell_gen_nokaled() : SpellScriptLoader("spell_gen_nokaled") { }

    class spell_gen_nokaled_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_nokaled_AuraScript);

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (!GetCaster()->ToPlayer())
                return false;

            Item* weapons[2];
            weapons[0] = GetCaster()->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            weapons[1] = GetCaster()->ToPlayer()->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);

            if (eventInfo.GetSpellInfo())                                       //null check, avoid crashes
                if (eventInfo.GetSpellInfo()->Id)                                   //null check, avoid crashes
                    if (eventInfo.GetSpellInfo()->Id == 86392)                          //exclude main gauche entirely
                        return false;

            if (eventInfo.GetDamageInfo())                                      //null check, avoid crashes
                if (eventInfo.GetDamageInfo()->GetAttackType())                     //null check, avoid crashes
                    if (eventInfo.GetDamageInfo()->GetAttackType() == RANGED_ATTACK)    //exclude ranged attacks completely
                        return false;

            if (eventInfo.GetDamageInfo())                                      //null check, avoid crashes
                if (eventInfo.GetDamageInfo()->GetAttackType())                     //null check, avoid crashes
                    if (eventInfo.GetDamageInfo()->GetAttackType() == OFF_ATTACK)       //if this proc came from an offhand attack, we need to check that the offhand weapon matches the aura trying to proc.
                        switch (GetId())
                        {
                        case NOKALED_HC_PROC:   if (weapons[1]->GetEntry() != NOKALED_HC_ITEM)     return false;
                            break;
                        case NOKALED_NM_PROC:   if (weapons[1]->GetEntry() != NOKALED_NM_ITEM)     return false;
                            break;
                        case NOKALED_LFR_PROC:  if (weapons[1]->GetEntry() != NOKALED_LFR_ITEM)    return false;
                            break;

                        default:
                            return false;//should never happen, this script should only be applied to the 3 invisible proc auras for no'kaled.
                            break;
                        }

            if (eventInfo.GetDamageInfo())                                      //null check, avoid crashes
                if (eventInfo.GetDamageInfo()->GetAttackType())                     //null check, avoid crashes
                    if (eventInfo.GetDamageInfo()->GetAttackType() == BASE_ATTACK)      //if this proc came from a mainhand attack, we need to check that the mainhand weapon matches the aura trying to proc.
                        switch (GetId())
                        {
                        case NOKALED_HC_PROC:   if (weapons[0]->GetEntry() != NOKALED_HC_ITEM)     return false;
                            break;
                        case NOKALED_NM_PROC:   if (weapons[0]->GetEntry() != NOKALED_NM_ITEM)     return false;
                            break;
                        case NOKALED_LFR_PROC:  if (weapons[0]->GetEntry() != NOKALED_LFR_ITEM)    return false;
                            break;

                        default:
                            return false;//should never happen, this script should only be applied to the 3 invisible proc auras for no'kaled.
                            break;
                        }
            //default case outside of spells:
            return !(eventInfo.GetHitMask() & (PROC_EX_IMMUNE | PROC_EX_MISS | PROC_EX_RESIST));
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_gen_nokaled_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_nokaled_AuraScript();
    }
};

class spell_gen_DragonSoul_crossproc : public SpellScriptLoader
{

public:
    spell_gen_DragonSoul_crossproc() : SpellScriptLoader("spell_gen_DragonSoul_crossproc") { }

    class spell_gen_DragonSoul_crossproc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_DragonSoul_crossproc_AuraScript);

        uint32 DragonSoul_Proc_Spells[37] =
        {
            /*Gurthalak*/
            52586,
            /*Vishanka*/
            107821,
            109856,
            109858,
            /*Bonelink Fetish*/
            109754,
            107997,
            109752,
            /*Vial of Shadows*/
            109724,
            107994,
            109721,
            /*Rathrak*/
            107831,
            109851,
            109854,
            /*Cunning*/
            108005,
            109798,
            109800,
            /*Nokaled*/
            109868,
            109872,
            109870,
            109871,
            109869,
            109867,
            107787,
            107785,
            107789,
            /*Souldrinker*/
            108022,
            109828,
            109831,
            /*Ti'tahk the steps of time*/
            107804,
            109842,
            109844,
            /*Maw of the dragonlord*/
            107835,
            109847,
            109849,
            /*Windward heart*/
            108000,
            109822,
            109825
        };

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (!GetCaster()->ToPlayer())
                return false;

            for (uint8 i = 0; i < 37; ++i)
            {
                if (eventInfo.GetSpellInfo())                                       //null check, avoid crashes
                    if (eventInfo.GetSpellInfo()->Id)                                   //null check, avoid crashes
                        if (eventInfo.GetSpellInfo()->Id == DragonSoul_Proc_Spells[i])
                        {
                            //TC_LOG_ERROR("spells", "Spell %u would have been procced by %u, cancelled by crossproc script.", GetId(), eventInfo.GetSpellInfo()->Id);
                            return false;
                        }
            }
            //default case outside of spells:
            return !(eventInfo.GetHitMask() & (PROC_EX_IMMUNE | PROC_EX_MISS | PROC_EX_RESIST));
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_gen_DragonSoul_crossproc_AuraScript::CheckProc);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_DragonSoul_crossproc_AuraScript();
    }
};

class spell_gen_two_forms : public SpellScriptLoader
{
    public:
        spell_gen_two_forms() : SpellScriptLoader("spell_gen_two_forms") { }

        class spell_gen_two_forms_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_two_forms_SpellScript);

            SpellCastResult CheckCast()
            {
                if (GetCaster()->ToPlayer()->GetBattleground())
                {
                    if (!GetCaster()->ToPlayer()->InArena())
                    {
                        if (!GetCaster()->ToPlayer()->IsPlayingNative())
                        {
                            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                            return SPELL_FAILED_CUSTOM_ERROR;
                        }
                    }
                }

                if (GetCaster()->isInCombat())
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                // Player cannot transform to human form if he is forced to be worgen for some reason (Darkflight)
                if (GetCaster()->GetAuraEffectsByType(SPELL_AURA_WORGEN_ALTERED_FORM).size() > 1)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TRANSFORM);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleTransform(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();
                PreventHitDefaultEffect(effIndex);
                if (target->HasAuraType(SPELL_AURA_WORGEN_ALTERED_FORM))
                    target->RemoveAurasByType(SPELL_AURA_WORGEN_ALTERED_FORM);
                else    // Basepoints 1 for this aura control whether to trigger transform transition animation or not.
                    target->CastCustomSpell(SPELL_ALTERED_FORM, SPELLVALUE_BASE_POINT0, 1, target, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_two_forms_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_gen_two_forms_SpellScript::HandleTransform, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_two_forms_SpellScript();
        }
};

class spell_gen_darkflight : public SpellScriptLoader
{
    public:
        spell_gen_darkflight() : SpellScriptLoader("spell_gen_darkflight") { }

        class spell_gen_darkflight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_darkflight_SpellScript);

            void TriggerTransform()
            {
                GetCaster()->CastSpell(GetCaster(), SPELL_ALTERED_FORM, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_gen_darkflight_SpellScript::TriggerTransform);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_darkflight_SpellScript();
        }
};

class spell_gen_pvp_trinket : public SpellScriptLoader
{
    public:
        spell_gen_pvp_trinket() : SpellScriptLoader("spell_gen_pvp_trinket") { }

        class spell_gen_pvp_trinket_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pvp_trinket_SpellScript);

            void TriggerAnimation()
            {
                Player* caster = GetCaster()->ToPlayer();
                uint8 currentRace;
                currentRace = (caster->GetBattleground() ? ((caster->InArena()) ? (caster->getORace()) : ((caster->IsPlayingNative()) ? (caster->getORace()) : (caster->getFRace()))) : (caster->getORace()));
                switch (currentRace)
                {
                case RACE_HUMAN:
                    caster->CastSpell(caster, 97979, TRIGGERED_FULL_MASK);//human trinket
                    break;
                case RACE_NIGHTELF:
                case RACE_DWARF:
                case RACE_GNOME:
                case RACE_WORGEN:
                case RACE_DRAENEI:
                    caster->CastSpell(caster, 97403, TRIGGERED_FULL_MASK);//alliance trinket
                        break;
                case RACE_TAUREN:
                case RACE_BLOODELF:
                case RACE_ORC:
                case RACE_GOBLIN:
                case RACE_UNDEAD_PLAYER:
                case RACE_TROLL:
                    caster->CastSpell(caster, 97404, TRIGGERED_FULL_MASK);//horde trinket
                    break;
                default:
                    caster->CastSpell(caster, 97979, TRIGGERED_FULL_MASK);//human trinket
                        break;
                }
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_gen_pvp_trinket_SpellScript::TriggerAnimation);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pvp_trinket_SpellScript();
        }
};

enum crossfaction_mounts
{
    //paladin
    SPELL_PALADIN_MOUNT_DRAENEI_1 = 73629,
    SPELL_PALADIN_MOUNT_DRAENEI_2 = 73630,
    SPELL_PALADIN_MOUNT_HUMAN_DWARF_1 = 13819,
    SPELL_PALADIN_MOUNT_HUMAN_DWARF_2 = 23214,
    SPELL_PALADIN_MOUNT_BLOOD_ELF_1 = 34769,
    SPELL_PALADIN_MOUNT_BLOOD_ELF_2 = 34767,
    SPELL_PALADIN_MOUNT_TAUREN_1 = 69820,
    SPELL_PALADIN_MOUNT_TAUREN_2 = 69826,
    //ally flying mounts
    SPELL_ALLI_GRYPHON_EBON = 32239,
    SPELL_ALLI_GRYPHON_GOLDEN = 32235,
    SPELL_ALLI_GRYPHON_SNOWY = 32240,
    //faster
    SPELL_ALLI_GRYPHON_ARMORED_SNOWY = 61229,
    SPELL_ALLI_GRYPHON_SPECTRAL = 107516,
    SPELL_ALLI_GRYPHON_SWIFT_BLUE = 32242,
    SPELL_ALLI_GRYPHON_SWIFT_RED = 32289,
    SPELL_ALLI_GRYPHON_SWIFT_GREEN = 32290,
    SPELL_ALLI_GRYPHON_SWIFT_PURPLE = 32292,
    //horde flying mounts
    SPELL_HORDE_WRIDER_TAWNY = 32243,
    SPELL_HORDE_WRIDER_BLUE = 32244,
    SPELL_HORDE_WRIDER_GREEN = 32245,
    //faster
    SPELL_HORDE_WRIDER_ARMORED_BLUE = 61230,
    SPELL_HORDE_WRIDER_SPECTRAL = 107517,
    //special
    SPELL_HORDE_WRIDER_SWIFT_GREEN = 32295,
    SPELL_HORDE_WRIDER_SWIFT_PURPLE = 32297,
    SPELL_HORDE_WRIDER_SWIFT_RED = 32246,
    SPELL_HORDE_WRIDER_SWIFT_YELLOW = 32296,
    //guild
    SPELL_HORDE_GUILD_MOUNT = 93644,
    SPELL_ALLI_GUILD_MOUNT = 90621,
    //rated BG
    SPELL_VICIOUS_WAR_STEED = 100332,
    SPELL_VICIOUS_WAR_WOLF = 100333,
    //tol barad spectral
    SPELL_SPECTRAL_STEED = 92231,
    SPELL_SPECTRAL_WOLF = 92232,
    //undead
    SPELL_SKELETAL_HORSE_BLACK = 64977,
    SPELL_SKELETAL_HORSE_RED = 17462,
    SPELL_SKELETAL_HORSE_BLUE = 17463,
    SPELL_SKELETAL_HORSE_BROWN = 17464,
    SPELL_SKELETAL_HORSE_WAR_PURPLE = 23246,
    SPELL_SKELETAL_HORSE_WAR_OCHRE = 66846,
    SPELL_SKELETAL_HORSE_WAR_RED_BG = 22722,
    SPELL_SKELETAL_HORSE_FORSAKEN = 63643,
    //human
    SPELL_HUMAN_HORSE_BLACK = 470,
    SPELL_HUMAN_HORSE_CHESTNUT = 6648,
    SPELL_HUMAN_HORSE_BROWN = 458,
    SPELL_HUMAN_HORSE_PINTO = 472,
    SPELL_HUMAN_SWIFT_PALOMINO = 23227,
    SPELL_HUMAN_SWIFT_WHITE_STEED = 23228,
    SPELL_HUMAN_SWIFT_BROWN_STEED = 23229,//done
    SPELL_HUMAN_WAR_STEED_BLACK_BG = 22717,//done
    SPELL_HUMAN_STORMWIND_STEED = 63232,
    //worgen 6
    SPELL_WORGEN_MOUNTAIN_HORSE = 103195,//done
    SPELL_WORGEN_MOUNTAIN_HORSE_SWIFT = 103196,//done
    //goblin
    SPELL_GOBLIN_TRIKE_KEY = 87090,
    SPELL_GOBLIN_TURBO_TRIKE_KEY = 87091,//done
    //engineering
    SPELL_MECHANO_HOG = 55531,
    SPELL_MEKGINEER_CHOPPER = 60424,//done

    //gnome 2
    SPELL_GNOME_MECHANO_BLUE = 10969,
    SPELL_GNOME_MECHANO_GREEN = 17453,
    SPELL_GNOME_MECHANO_RED = 10873,
    SPELL_GNOME_MECHANO_UNPAINTED = 17454,
    SPELL_GNOME_MECHANO_SWIFT_GREEN = 23225,
    SPELL_GNOME_MECHANO_SWIFT_WHITE = 23223,
    SPELL_GNOME_MECHANO_SWIFT_YELLOW = 23222,
    SPELL_GNOME_MECHANO_BLACK_WAR_BG = 22719,
    SPELL_GNOME_MECHANO_GNOMEREGAN = 63638,
    //blood elf
    SPELL_BLOODELF_HSTRIDER_RED = 34795,
    SPELL_BLOODELF_HSTRIDER_PURPLE = 35018,
    SPELL_BLOODELF_HSTRIDER_BLUE = 35020,
    SPELL_BLOODELF_HSTRIDER_BLACK = 35022,
    SPELL_BLOODELF_HSTRIDER_SWIFT_PINK = 33660,
    SPELL_BLOODELF_HSTRIDER_SWIFT_GREEN = 35025,
    SPELL_BLOODELF_HSTRIDER_SWIFT_PURPLE = 35027,
    SPELL_BLOODELF_HSTRIDER_SWIFT_WHITE = 46628,
    SPELL_BLOODELF_HSTRIDER_SWIFT_RED = 66091,
    SPELL_BLOODELF_HSTRIDER_BLACK_WAR_BG = 35028,
    SPELL_BLOODELF_HSTRIDER_SILVERMOON = 63642,
    SPELL_BLOODELF_HSTRIDER_SUNREAVER = 65639,//done
    //night elf 3
    SPELL_NIGHTELF_SABER_SPOTTED_FROST = 10789,
    SPELL_NIGHTELF_SABER_STRIPED_DAWN = 66847,
    SPELL_NIGHTELF_SABER_STRIPED_NIGHT = 10793,
    SPELL_NIGHTELF_SABER_STRIPED_FROST = 8394,
    SPELL_NIGHTELF_SABER_SWIFT_FROST = 23221,
    SPELL_NIGHTELF_SABER_SWIFT_MIST = 23219,
    SPELL_NIGHTELF_SABER_SWIFT_STORM = 23338,
    SPELL_NIGHTELF_SABER_BLACK_WAR = 22723,
    SPELL_NIGHTELF_SABER_DARNASSUS = 63637,
    //troll
    SPELL_TROLL_RAPTOR_EMERALD = 8395,
    SPELL_TROLL_RAPTOR_TURQUOISE = 10796,
    SPELL_TROLL_RAPTOR_VIOLET = 10799,
    SPELL_TROLL_RAPTOR_SWIFT_BLUE = 23241,
    SPELL_TROLL_RAPTOR_SWIFT_OLIVE = 23242,
    SPELL_TROLL_RAPTOR_SWIFT_ORANGE = 23243,
    SPELL_TROLL_RAPTOR_BLACK_WAR = 22721,
    SPELL_TROLL_RAPTOR_DARKSPEAR = 63635,//done

    //tauren
    SPELL_TAUREN_KODO_BROWN = 18990,
    SPELL_TAUREN_KODO_GRAY = 18989,
    SPELL_TAUREN_KODO_WHITE = 64657,
    SPELL_TAUREN_KODO_GREAT_WHITE = 23247,
    SPELL_TAUREN_KODO_GREAT_GRAY = 23248,
    SPELL_TAUREN_KODO_GREAT_BROWN = 23249,
    SPELL_TAUREN_KODO_BLACK_WAR_BG = 22718,
    SPELL_TAUREN_KODO_THUNDERBLUFF = 63641,
    //draenei 4
    SPELL_DRAENEI_ELEKK_BROWN = 34406,
    SPELL_DRAENEI_ELEKK_GREAT_ELITE = 34407,
    SPELL_DRAENEI_ELEKK_GRAY = 35710,
    SPELL_DRAENEI_ELEKK_PURPLE = 35711,
    SPELL_DRAENEI_ELEKK_GREAT_GREEN = 35712,
    SPELL_DRAENEI_ELEKK_GREAT_PURPLE = 35714,
    SPELL_DRAENEI_ELEKK_GREAT_RED = 65637,
    SPELL_DRAENEI_ELEKK_BLACK_WAR = 48027,//same visual
    SPELL_DRAENEI_ELEKK_SWIFT_WAR = 47037,//same visual, unused?
    SPELL_DRAENEI_ELEKK_EXODAR = 63639,//done
    //orc
    SPELL_ORC_WOLF_BLACK = 64658,
    SPELL_ORC_WOLF_BROWN = 6654,
    SPELL_ORC_WOLF_DIRE = 6653,
    SPELL_ORC_WOLF_TIMBER = 580,
    SPELL_ORC_WOLF_SWIFT_BROWN = 23250,
    SPELL_ORC_WOLF_SWIFT_GRAY = 23252,
    SPELL_ORC_WOLF_SWIFT_TIMBER = 23251,
    SPELL_ORC_WOLF_BLACK_WAR_BG = 22724,
    SPELL_ORC_WOLF_ORGRIMMAR = 63640,
    //dwarf 5
    SPELL_DWARF_RAM_BROWN = 6899,
    SPELL_DWARF_RAM_GRAY = 6777,
    SPELL_DWARF_RAM_WHITE = 6898,
    SPELL_DWARF_RAM_SWIFT_BROWN = 23238,
    SPELL_DWARF_RAM_SWIFT_GRAY = 23239,
    SPELL_DWARF_RAM_SWIFT_WHITE = 23240,
    SPELL_DWARF_RAM_BLACK_WAR_BG = 22720,
    SPELL_DWARF_RAM_IRONFORGE = 63636,

    //mammoths
    SPELL_WOOLY_MAMMOTH_ALLIANCE                = 59791,
    SPELL_WOOLY_MAMMOTH_HORDE                   = 59793,
    SPELL_BLACK_WAR_MAMMOTH_ALLIANCE            = 59785,
    SPELL_BLACK_WAR_MAMMOTH_HORDE               = 59788,
    SPELL_ICE_MAMMOTH_HORDE                     = 59797,
    SPELL_ICE_MAMMOTH_ALLIANCE                  = 59799,
    SPELL_ICE_MAMMOTH_GRAND_HORDE               = 59802,
    SPELL_ICE_MAMMOTH_GRAND_ALLIANCE            = 59804,
    SPELL_ICE_MAMMOTH_GRAND_HORDE_ID2           = 61469,
    SPELL_ICE_MAMMOTH_GRAND_ALLIANCE_ID2        = 61470,
    SPELL_GRAND_CARAVAN_MAMMOTH_ALLIANCE        = 60136,
    SPELL_GRAND_CARAVAN_MAMMOTH_HORDE           = 60140,
    SPELL_TRAVELER_TUNDRA_MAMMOTH_ALLIANCE      = 61425,
    SPELL_TRAVELER_TUNDRA_MAMMOTH_HORDE         = 61447,
    SPELL_GRAND_BLACK_WAR_MAMMOTH_ALLIANCE      = 61465,
    SPELL_GRAND_BLACK_WAR_MAMMOTH_HORDE         = 61467,
};


class spell_gen_mount_crossfaction : public SpellScriptLoader
{
public:
    spell_gen_mount_crossfaction() : SpellScriptLoader("spell_gen_mount_crossfaction") { }

    class spell_gen_mount_crossfaction_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_mount_crossfaction_SpellScript);
        uint32 GetCrossFactionEquivelant(uint32 input)
        {
            switch (input)
            {
                //paladin mounts
            case SPELL_PALADIN_MOUNT_DRAENEI_1:         return  SPELL_PALADIN_MOUNT_TAUREN_1;           break;
            case SPELL_PALADIN_MOUNT_TAUREN_1:          return  SPELL_PALADIN_MOUNT_DRAENEI_1;          break;
            case SPELL_PALADIN_MOUNT_DRAENEI_2:         return  SPELL_PALADIN_MOUNT_TAUREN_2;           break;
            case SPELL_PALADIN_MOUNT_TAUREN_2:          return  SPELL_PALADIN_MOUNT_DRAENEI_2;          break;
            case SPELL_PALADIN_MOUNT_HUMAN_DWARF_1:     return  SPELL_PALADIN_MOUNT_BLOOD_ELF_1;        break;
            case SPELL_PALADIN_MOUNT_BLOOD_ELF_1:       return  SPELL_PALADIN_MOUNT_HUMAN_DWARF_1;      break;
            case SPELL_PALADIN_MOUNT_HUMAN_DWARF_2:     return  SPELL_PALADIN_MOUNT_BLOOD_ELF_2;        break;
            case SPELL_PALADIN_MOUNT_BLOOD_ELF_2:       return  SPELL_PALADIN_MOUNT_HUMAN_DWARF_2;      break;
                //faction slow flying mounts
            case SPELL_ALLI_GRYPHON_EBON:               return  SPELL_HORDE_WRIDER_TAWNY;               break;
            case SPELL_HORDE_WRIDER_TAWNY:              return  SPELL_ALLI_GRYPHON_EBON;                break;
            case SPELL_ALLI_GRYPHON_GOLDEN:             return  SPELL_HORDE_WRIDER_BLUE;                break;
            case SPELL_HORDE_WRIDER_BLUE:               return  SPELL_ALLI_GRYPHON_GOLDEN;              break;
            case SPELL_ALLI_GRYPHON_SNOWY:              return  SPELL_HORDE_WRIDER_GREEN;               break;
            case SPELL_HORDE_WRIDER_GREEN:              return  SPELL_ALLI_GRYPHON_SNOWY;               break;
                //faction special flying mounts
            case SPELL_ALLI_GRYPHON_ARMORED_SNOWY:      return  SPELL_HORDE_WRIDER_ARMORED_BLUE;        break;
            case SPELL_HORDE_WRIDER_ARMORED_BLUE:       return  SPELL_ALLI_GRYPHON_ARMORED_SNOWY;       break;
            case SPELL_ALLI_GRYPHON_SWIFT_BLUE:         return  SPELL_HORDE_WRIDER_SWIFT_YELLOW;        break;
            case SPELL_HORDE_WRIDER_SWIFT_YELLOW:       return  SPELL_ALLI_GRYPHON_SWIFT_BLUE;          break;
            case SPELL_ALLI_GRYPHON_SPECTRAL:           return  SPELL_HORDE_WRIDER_SPECTRAL;            break;
            case SPELL_HORDE_WRIDER_SPECTRAL:           return  SPELL_ALLI_GRYPHON_SPECTRAL;            break;
                //faction fast flying mounts
            case SPELL_ALLI_GRYPHON_SWIFT_RED:          return  SPELL_HORDE_WRIDER_SWIFT_RED;           break;
            case SPELL_HORDE_WRIDER_SWIFT_RED:          return  SPELL_ALLI_GRYPHON_SWIFT_RED;           break;
            case SPELL_ALLI_GRYPHON_SWIFT_GREEN:        return  SPELL_HORDE_WRIDER_SWIFT_GREEN;         break;
            case SPELL_HORDE_WRIDER_SWIFT_GREEN:        return  SPELL_ALLI_GRYPHON_SWIFT_GREEN;         break;
            case SPELL_ALLI_GRYPHON_SWIFT_PURPLE:       return  SPELL_HORDE_WRIDER_SWIFT_PURPLE;        break;
            case SPELL_HORDE_WRIDER_SWIFT_PURPLE:       return  SPELL_ALLI_GRYPHON_SWIFT_PURPLE;        break;
                //guild mounts
            case SPELL_HORDE_GUILD_MOUNT:               return  SPELL_ALLI_GUILD_MOUNT;                 break;
            case SPELL_ALLI_GUILD_MOUNT:                return  SPELL_HORDE_GUILD_MOUNT;                break;
                //Tol Barad Spectrals
            case SPELL_SPECTRAL_WOLF:                   return SPELL_SPECTRAL_STEED;                    break;
            case SPELL_SPECTRAL_STEED:                  return SPELL_SPECTRAL_WOLF;                     break;
                //Rated BG mounts
            case SPELL_VICIOUS_WAR_WOLF:                return SPELL_VICIOUS_WAR_STEED;                 break;
            case SPELL_VICIOUS_WAR_STEED:               return SPELL_VICIOUS_WAR_WOLF;                  break;
                /*Undead to human mount conversion*/
            case SPELL_SKELETAL_HORSE_BLACK:
            case SPELL_SKELETAL_HORSE_RED:
            case SPELL_SKELETAL_HORSE_BLUE:
            case SPELL_SKELETAL_HORSE_BROWN:            return  SPELL_HUMAN_HORSE_BLACK;                break;
            case SPELL_SKELETAL_HORSE_WAR_PURPLE:
            case SPELL_SKELETAL_HORSE_WAR_OCHRE:        return SPELL_HUMAN_SWIFT_WHITE_STEED;           break;
            case SPELL_SKELETAL_HORSE_WAR_RED_BG:       return SPELL_HUMAN_WAR_STEED_BLACK_BG;          break;
            case SPELL_SKELETAL_HORSE_FORSAKEN:         return SPELL_HUMAN_STORMWIND_STEED;             break;
                /*Human to undead mount conversion*/
            case SPELL_HUMAN_HORSE_BLACK:
            case SPELL_HUMAN_HORSE_CHESTNUT:
            case SPELL_HUMAN_HORSE_BROWN:
            case SPELL_WORGEN_MOUNTAIN_HORSE:
            case SPELL_HUMAN_HORSE_PINTO:               return SPELL_SKELETAL_HORSE_BLACK;              break;
            case SPELL_HUMAN_SWIFT_PALOMINO:
            case SPELL_HUMAN_SWIFT_WHITE_STEED:
            case SPELL_WORGEN_MOUNTAIN_HORSE_SWIFT:
            case SPELL_HUMAN_SWIFT_BROWN_STEED:         return SPELL_SKELETAL_HORSE_WAR_PURPLE;         break;
            case SPELL_HUMAN_WAR_STEED_BLACK_BG:        return SPELL_SKELETAL_HORSE_WAR_RED_BG;         break;
            case SPELL_HUMAN_STORMWIND_STEED:           return SPELL_SKELETAL_HORSE_FORSAKEN;           break;
                /*Engineering and goblin chopper conversions*/
            case SPELL_GOBLIN_TRIKE_KEY:
            case SPELL_GOBLIN_TURBO_TRIKE_KEY:
            case SPELL_MECHANO_HOG:                     return SPELL_MEKGINEER_CHOPPER;                 break;
            case SPELL_MEKGINEER_CHOPPER:               return SPELL_MECHANO_HOG;                       break;
                //gnome 2
            case SPELL_GNOME_MECHANO_BLUE:
            case SPELL_GNOME_MECHANO_GREEN:
            case SPELL_GNOME_MECHANO_RED:
            case SPELL_GNOME_MECHANO_UNPAINTED:         return SPELL_BLOODELF_HSTRIDER_RED;             break;
            case SPELL_GNOME_MECHANO_SWIFT_GREEN:
            case SPELL_GNOME_MECHANO_SWIFT_WHITE:
            case SPELL_GNOME_MECHANO_SWIFT_YELLOW:      return SPELL_BLOODELF_HSTRIDER_SWIFT_GREEN;     break;
            case SPELL_GNOME_MECHANO_BLACK_WAR_BG:      return SPELL_BLOODELF_HSTRIDER_BLACK_WAR_BG;    break;
            case SPELL_GNOME_MECHANO_GNOMEREGAN:        return SPELL_BLOODELF_HSTRIDER_SUNREAVER;       break;
                //blood elf
            case SPELL_BLOODELF_HSTRIDER_RED:
            case SPELL_BLOODELF_HSTRIDER_PURPLE:
            case SPELL_BLOODELF_HSTRIDER_BLUE:
            case SPELL_BLOODELF_HSTRIDER_BLACK:         return SPELL_GNOME_MECHANO_RED;                 break;
            case SPELL_BLOODELF_HSTRIDER_SWIFT_PINK:
            case SPELL_BLOODELF_HSTRIDER_SWIFT_GREEN:
            case SPELL_BLOODELF_HSTRIDER_SWIFT_PURPLE:
            case SPELL_BLOODELF_HSTRIDER_SWIFT_WHITE:
            case SPELL_BLOODELF_HSTRIDER_SWIFT_RED:     return SPELL_GNOME_MECHANO_SWIFT_GREEN;         break;
            case SPELL_BLOODELF_HSTRIDER_SILVERMOON:
            case SPELL_BLOODELF_HSTRIDER_SUNREAVER:     return SPELL_GNOME_MECHANO_GNOMEREGAN;          break;
            case SPELL_BLOODELF_HSTRIDER_BLACK_WAR_BG:  return SPELL_GNOME_MECHANO_BLACK_WAR_BG;        break;
                //night elf cat to troll raptors
            case SPELL_NIGHTELF_SABER_SPOTTED_FROST:
            case SPELL_NIGHTELF_SABER_STRIPED_DAWN:
            case SPELL_NIGHTELF_SABER_STRIPED_NIGHT:
            case SPELL_NIGHTELF_SABER_STRIPED_FROST:    return SPELL_TROLL_RAPTOR_EMERALD;              break;
            case SPELL_NIGHTELF_SABER_SWIFT_FROST:
            case SPELL_NIGHTELF_SABER_SWIFT_MIST:
            case SPELL_NIGHTELF_SABER_SWIFT_STORM:      return SPELL_TROLL_RAPTOR_SWIFT_BLUE;           break;
            case SPELL_NIGHTELF_SABER_BLACK_WAR:        return SPELL_TROLL_RAPTOR_BLACK_WAR;            break;
            case SPELL_NIGHTELF_SABER_DARNASSUS:        return SPELL_TROLL_RAPTOR_DARKSPEAR;            break;
                //troll
            case SPELL_TROLL_RAPTOR_EMERALD:
            case SPELL_TROLL_RAPTOR_TURQUOISE:
            case SPELL_TROLL_RAPTOR_VIOLET:             return SPELL_NIGHTELF_SABER_STRIPED_NIGHT;      break;
            case SPELL_TROLL_RAPTOR_SWIFT_BLUE:
            case SPELL_TROLL_RAPTOR_SWIFT_OLIVE:
            case SPELL_TROLL_RAPTOR_SWIFT_ORANGE:       return SPELL_NIGHTELF_SABER_SWIFT_STORM;        break;
            case SPELL_TROLL_RAPTOR_BLACK_WAR:          return SPELL_NIGHTELF_SABER_BLACK_WAR;          break;
            case SPELL_TROLL_RAPTOR_DARKSPEAR:          return SPELL_NIGHTELF_SABER_DARNASSUS;          break;
                //tauren
            case SPELL_TAUREN_KODO_BROWN:
            case SPELL_TAUREN_KODO_GRAY:
            case SPELL_TAUREN_KODO_WHITE:               return SPELL_DRAENEI_ELEKK_GRAY;                break;
            case SPELL_TAUREN_KODO_GREAT_WHITE:
            case SPELL_TAUREN_KODO_GREAT_GRAY:
            case SPELL_TAUREN_KODO_GREAT_BROWN:         return SPELL_DRAENEI_ELEKK_GREAT_ELITE;         break;
            case SPELL_TAUREN_KODO_BLACK_WAR_BG:        return SPELL_DRAENEI_ELEKK_BLACK_WAR;           break;
            case SPELL_TAUREN_KODO_THUNDERBLUFF:        return SPELL_DRAENEI_ELEKK_EXODAR;              break;
                //draenei 4
            case SPELL_DRAENEI_ELEKK_BROWN:
            case SPELL_DRAENEI_ELEKK_GRAY:
            case SPELL_DRAENEI_ELEKK_PURPLE:            return SPELL_TAUREN_KODO_GRAY;                  break;
            case SPELL_DRAENEI_ELEKK_GREAT_GREEN:
            case SPELL_DRAENEI_ELEKK_GREAT_PURPLE:
            case SPELL_DRAENEI_ELEKK_GREAT_ELITE:
            case SPELL_DRAENEI_ELEKK_GREAT_RED:         return SPELL_TAUREN_KODO_GREAT_GRAY;            break;
            case SPELL_DRAENEI_ELEKK_BLACK_WAR:
            case SPELL_DRAENEI_ELEKK_SWIFT_WAR:         return SPELL_TAUREN_KODO_BLACK_WAR_BG;          break;
            case SPELL_DRAENEI_ELEKK_EXODAR:            return SPELL_TAUREN_KODO_THUNDERBLUFF;          break;
                //orcs to dwarves
            case SPELL_ORC_WOLF_BLACK:
            case SPELL_ORC_WOLF_BROWN:
            case SPELL_ORC_WOLF_DIRE:
            case SPELL_ORC_WOLF_TIMBER:                 return SPELL_DWARF_RAM_GRAY;                    break;
            case SPELL_ORC_WOLF_SWIFT_BROWN:
            case SPELL_ORC_WOLF_SWIFT_GRAY:
            case SPELL_ORC_WOLF_SWIFT_TIMBER:           return SPELL_DWARF_RAM_SWIFT_GRAY;              break;
            case SPELL_ORC_WOLF_BLACK_WAR_BG:           return SPELL_DWARF_RAM_BLACK_WAR_BG;            break;
            case SPELL_ORC_WOLF_ORGRIMMAR:              return SPELL_DWARF_RAM_IRONFORGE;               break;
                //dwarf 5
            case SPELL_DWARF_RAM_BROWN:
            case SPELL_DWARF_RAM_GRAY:
            case SPELL_DWARF_RAM_WHITE:                 return SPELL_ORC_WOLF_BLACK;                    break;
            case SPELL_DWARF_RAM_SWIFT_BROWN:
            case SPELL_DWARF_RAM_SWIFT_GRAY:
            case SPELL_DWARF_RAM_SWIFT_WHITE:           return SPELL_ORC_WOLF_SWIFT_GRAY;               break;
            case SPELL_DWARF_RAM_BLACK_WAR_BG:          return SPELL_ORC_WOLF_BLACK_WAR_BG;             break;
            case SPELL_DWARF_RAM_IRONFORGE:             return SPELL_ORC_WOLF_ORGRIMMAR;                break;

            case SPELL_WOOLY_MAMMOTH_ALLIANCE:          return SPELL_WOOLY_MAMMOTH_HORDE;               break;
            case SPELL_WOOLY_MAMMOTH_HORDE:             return SPELL_WOOLY_MAMMOTH_ALLIANCE;            break;
            case SPELL_BLACK_WAR_MAMMOTH_ALLIANCE:      return SPELL_BLACK_WAR_MAMMOTH_HORDE;           break;
            case SPELL_BLACK_WAR_MAMMOTH_HORDE:         return SPELL_BLACK_WAR_MAMMOTH_ALLIANCE;        break;
            case SPELL_ICE_MAMMOTH_HORDE:               return SPELL_ICE_MAMMOTH_ALLIANCE;              break;
            case SPELL_ICE_MAMMOTH_ALLIANCE:            return SPELL_ICE_MAMMOTH_HORDE;                 break;
            case SPELL_ICE_MAMMOTH_GRAND_HORDE:         return SPELL_ICE_MAMMOTH_GRAND_ALLIANCE;        break;
            case SPELL_ICE_MAMMOTH_GRAND_ALLIANCE:      return SPELL_ICE_MAMMOTH_GRAND_HORDE;           break;
            case SPELL_ICE_MAMMOTH_GRAND_HORDE_ID2:     return SPELL_ICE_MAMMOTH_GRAND_ALLIANCE_ID2;    break;
            case SPELL_ICE_MAMMOTH_GRAND_ALLIANCE_ID2:  return SPELL_ICE_MAMMOTH_GRAND_HORDE_ID2;       break;
            case SPELL_GRAND_CARAVAN_MAMMOTH_ALLIANCE:  return SPELL_GRAND_CARAVAN_MAMMOTH_HORDE;       break;
            case SPELL_GRAND_CARAVAN_MAMMOTH_HORDE:     return SPELL_GRAND_CARAVAN_MAMMOTH_ALLIANCE;    break;
            case SPELL_TRAVELER_TUNDRA_MAMMOTH_ALLIANCE:return SPELL_TRAVELER_TUNDRA_MAMMOTH_HORDE;     break;
            case SPELL_TRAVELER_TUNDRA_MAMMOTH_HORDE:   return SPELL_TRAVELER_TUNDRA_MAMMOTH_ALLIANCE;  break;
            case SPELL_GRAND_BLACK_WAR_MAMMOTH_ALLIANCE:return SPELL_GRAND_BLACK_WAR_MAMMOTH_HORDE;     break;
            case SPELL_GRAND_BLACK_WAR_MAMMOTH_HORDE:   return SPELL_GRAND_BLACK_WAR_MAMMOTH_ALLIANCE;  break;
            default:
                return GetSpellInfo()->Id;
                break;
            }
            return GetSpellInfo()->Id;//should never happen
        }

        void HandleMount()
        {
            if (auto mountCaster = GetCaster()->ToPlayer())
            {
                if (mountCaster->GetBattleground()) //the player is currently in a battleground
                    if (!mountCaster->GetBattleground()->isArena() && !mountCaster->GetBattleground()->isRatedBattleground())   //don't morph mounts in arena :pepeweird:
                    if (!mountCaster->IsPlayingNative()) //the player is NOT on their home faction in the battleground
                        if (GetCrossFactionEquivelant(GetSpellInfo()->Id) != GetSpellInfo()->Id) //there is a valid and different mount ID mentioned in the function above for this mount.
                            if (mountCaster->HasAura(GetSpellInfo()->Id))
                            {
                                mountCaster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                                mountCaster->AddAura(GetCrossFactionEquivelant(GetSpellInfo()->Id), mountCaster); // crossfaction mount
                            }
            }
        }


        void Register()
        {
            AfterCast += SpellCastFn(spell_gen_mount_crossfaction_SpellScript::HandleMount);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_mount_crossfaction_SpellScript();
    }
};

class spell_gen_gift_of_naaru : public SpellScriptLoader
{
    public:
        spell_gen_gift_of_naaru() : SpellScriptLoader("spell_gen_gift_of_naaru") { }

        class spell_gen_gift_of_naaru_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gift_of_naaru_AuraScript);

            void CalculateBonus(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
            {
                canBeRecalculated = false;
                if (auto caster = GetCaster())
                    amount = CalculatePct(caster->GetMaxHealth(), GetSpellInfo()->Effects[EFFECT_0].CalcValue(caster));
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_gift_of_naaru_AuraScript::CalculateBonus, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_gift_of_naaru_AuraScript();
        }
};

enum RaidHasteSpellsBuffsAndDebuffs
{
    SPELL_BUFF_BLOODLUST                = 2825,
    SPELL_DEBUFF_SATED                  = 57724,

    SPELL_BUFF_HEROISM                  = 32182,
    SPELL_DEBUFF_EXHAUSTION             = 57723,

    SPELL_BUFF_TIME_WARP                = 80353,
    SPELL_DEBUFF_TEMPORAL_DISPLACEMENT  = 80354,

    SPELL_BUFF_ANCIENT_HYSTERIA         = 90355,
    SPELL_DEBUFF_INSANITY               = 95809,
};

// Filter targets for Ancient Hysteria, Bloodlust, Heroism, and Time Warp.
class spell_gen_raid_haste : public SpellScriptLoader
{
public:
    spell_gen_raid_haste(char const* scriptName) : SpellScriptLoader(scriptName) { }

    class spell_gen_raid_SPELL_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_raid_SPELL_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DEBUFF_SATED) ||
                !sSpellMgr->GetSpellInfo(SPELL_DEBUFF_EXHAUSTION) ||
                !sSpellMgr->GetSpellInfo(SPELL_DEBUFF_TEMPORAL_DISPLACEMENT) ||
                !sSpellMgr->GetSpellInfo(SPELL_DEBUFF_INSANITY))
                return false;
            return true;
        }

        void RemoveInvalidTargets(std::list<WorldObject*>& targets)
        {
            targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_DEBUFF_SATED));
            targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_DEBUFF_EXHAUSTION));
            targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_DEBUFF_TEMPORAL_DISPLACEMENT));
            targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_DEBUFF_INSANITY));
        }

        void ApplyDebuff()
        {
            switch (GetSpellInfo()->Id)
            {
                case SPELL_BUFF_BLOODLUST:
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DEBUFF_SATED, true);
                    break;
                case SPELL_BUFF_HEROISM:
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DEBUFF_EXHAUSTION, true);
                    break;
                case SPELL_BUFF_TIME_WARP:
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DEBUFF_TEMPORAL_DISPLACEMENT, true);
                    break;
                case SPELL_BUFF_ANCIENT_HYSTERIA:
                    GetHitUnit()->CastSpell(GetHitUnit(), SPELL_DEBUFF_INSANITY, true);
                    break;
            }
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_raid_SPELL_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_raid_SPELL_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_CASTER_AREA_RAID);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gen_raid_SPELL_SpellScript::RemoveInvalidTargets, EFFECT_2, TARGET_UNIT_CASTER_AREA_RAID);
            AfterHit += SpellHitFn(spell_gen_raid_SPELL_SpellScript::ApplyDebuff);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_raid_SPELL_SpellScript();
    }
};

class spell_gen_armor_specialization : public SpellScriptLoader
{
    public:
        spell_gen_armor_specialization(char const* scriptName) : SpellScriptLoader(scriptName) { }

        class spell_gen_armor_specialization_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_armor_specialization_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetHitPlayer();
                if (!player)
                    return;

                uint32 spellId = 0;

                switch(player->GetPrimaryTalentTree(player->GetActiveSpec()))
                {
                    case TALENT_TREE_WARRIOR_ARMS:
                        spellId = 86110;
                        break;
                    case TALENT_TREE_WARRIOR_FURY:
                        spellId = 86101;
                        break;
                    case TALENT_TREE_WARRIOR_PROTECTION:
                        spellId = 86535;
                        break;
                    case TALENT_TREE_PALADIN_HOLY:
                        spellId = 86103;
                        break;
                    case TALENT_TREE_PALADIN_PROTECTION:
                        spellId = 86102;
                        break;
                    case TALENT_TREE_PALADIN_RETRIBUTION:
                        spellId = 86539;
                        break;
                    case TALENT_TREE_HUNTER_BEAST_MASTERY:
                    case TALENT_TREE_HUNTER_MARKSMANSHIP:
                    case TALENT_TREE_HUNTER_SURVIVAL:
                        spellId = 86538;
                        break;
                    case TALENT_TREE_ROGUE_ASSASSINATION:
                    case TALENT_TREE_ROGUE_COMBAT:
                    case TALENT_TREE_ROGUE_SUBTLETY:
                        spellId = 86092;
                        break;
                    case TALENT_TREE_DEATH_KNIGHT_BLOOD:
                        spellId = 86537;
                        break;
                    case TALENT_TREE_DEATH_KNIGHT_FROST:
                        spellId = 86536;
                        break;
                    case TALENT_TREE_DEATH_KNIGHT_UNHOLY:
                        spellId = 86113;
                        break;
                    case TALENT_TREE_SHAMAN_ELEMENTAL:
                        spellId = 86100;
                        break;
                    case TALENT_TREE_SHAMAN_ENHANCEMENT:
                        spellId = 86099;
                        break;
                    case TALENT_TREE_SHAMAN_RESTORATION:
                        spellId = 86108;
                        break;
                    case TALENT_TREE_DRUID_BALANCE:
                        spellId = 86093;
                        break;
                    case TALENT_TREE_DRUID_FERAL_COMBAT:
                    {
                        if (player->GetShapeshiftForm() == FORM_BEAR)
                            spellId = 86096;
                        else
                            spellId = 86097;
                        break;
                    }
                    case TALENT_TREE_DRUID_RESTORATION:
                        spellId = 86104;
                        break;
                    default:
                        return;
                }

                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
                if (spellInfo && player->HasAllItemsToFitToSpellRequirements(spellInfo))
                    player->CastSpell(player, spellId, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_armor_specialization_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_armor_specialization_SpellScript();
        }
};

class spell_select_random_target : public SpellScriptLoader
{
public:
    spell_select_random_target() : SpellScriptLoader("spell_select_random_target") { }

    class spell_select_random_target_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_select_random_target_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
                return;

            WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
            targets.clear();
            if (auto player = target->ToPlayer())
                if (player->isAlive())
                    player->CastSpell(GetCaster(), uint32(GetSpellInfo()->Effects[EFFECT_0].BasePoints), true);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_select_random_target_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_select_random_target_SpellScript();
    }
};

enum Vengeance
{
    SPELL_VENGEANCE_PERIODIC    = 76691,
    DURATION_VENGEANCE          = 20000
};
class spell_vengeance : public SpellScriptLoader
{
public:
    spell_vengeance() : SpellScriptLoader("spell_vengeance") { }

    class spell_vengeance_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_vengeance_AuraScript);

        void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            Unit* attacker = eventInfo.GetActor();
            if (attacker && (attacker->GetTypeId() == TYPEID_PLAYER || (attacker->GetOwner() ? attacker->GetOwner()->GetTypeId() == TYPEID_PLAYER : false)))
                return;

            Unit* target = eventInfo.GetActionTarget();
            if (!target || target->GetTypeId() != TYPEID_PLAYER)
                return;

            float damage = eventInfo.GetDamageInfo()->GetDamage() + eventInfo.GetDamageInfo()->GetArmorMitigated() + eventInfo.GetDamageInfo()->GetAbsorb() + eventInfo.GetDamageInfo()->GetBlock() + eventInfo.GetDamageInfo()->GetResist();
            if (damage <= 0.0f)
                return;

            Aura* aura = target->GetAura(SPELL_VENGEANCE_PERIODIC);
            bool justApplied = false;
            if (!aura)
            {
                aura = target->AddAura(SPELL_VENGEANCE_PERIODIC, target);
                justApplied = true;
                if (!aura)
                    return;
            }
            AuraEffect* effect = aura->GetEffect(EFFECT_2);
            if (!effect)
                return;
            uint32 oldAmount = effect->GetAmount();
            uint32 amount = oldAmount + damage;
            effect->SetAmount(amount);
            if (justApplied)
            {
                AuraApplication* auraApp = aura->GetApplicationOfTarget(target->GetGUID());
                if (auraApp)
                    effect->PeriodicTick(auraApp, target);
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_vengeance_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_vengeance_AuraScript();
    }
};

class spell_vengeance_periodic : public SpellScriptLoader
{
public:
    spell_vengeance_periodic() : SpellScriptLoader("spell_vengeance_periodic") { }

    class spell_vengeance_periodic_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_vengeance_periodic_AuraScript);

        virtual bool Load()
        {
            amountLastHit = 0;
            return true;
        }

        void HandlePeriodicTick(AuraEffect const* aurEff)
        {
            PreventDefaultAction();

            if (aurEff->GetAmount() == 0)
            {
                AuraEffect* effect0 = GetEffect(EFFECT_0);
                AuraEffect* effect1 = GetEffect(EFFECT_1);
                AuraEffect* effect2 = GetEffect(EFFECT_2);
                if (!effect0 || !effect1 || !effect2)
                    return;
                uint32 amount = amountLastHit * GetAura()->GetDuration() / DURATION_VENGEANCE;
                effect0->ChangeAmount(amount);
                effect1->ChangeAmount(amount);
                return;
            }

            Player* owner = GetOwner() ? GetOwner()->ToPlayer() : NULL;
            if (!owner)
                return;
            // http://us.battle.net/wow/en/forum/topic/1827666726?page=3#47
            uint32 maxApBonus = owner->GetStat(STAT_STAMINA) + 0.1f * owner->GetCreateHealth();
            uint32 apBonus = 0.05f * aurEff->GetAmount();
            // http://www.wowwiki.com/Vengeance_(druid_ability)
            uint32 minApBonus = 0.33f * aurEff->GetAmount();
            AuraEffect* effect0 = GetEffect(EFFECT_0);
            AuraEffect* effect1 = GetEffect(EFFECT_1);
            AuraEffect* effect2 = GetEffect(EFFECT_2);
            if (!effect0 || !effect1 || !effect2)
                return;
            uint32 oldAmount = amountLastHit * GetAura()->GetDuration() / DURATION_VENGEANCE;
            uint32 amount = oldAmount + apBonus;
            amount = std::max(amount, minApBonus);
            amount = std::min(amount, maxApBonus);
            amountLastHit = amount;
            effect0->ChangeAmount(amount);
            effect1->ChangeAmount(amount);
            effect2->SetAmount(0);
            GetAura()->SetDuration(DURATION_VENGEANCE);
        }

    private:
        uint32 amountLastHit;

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_vengeance_periodic_AuraScript::HandlePeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_vengeance_periodic_AuraScript();
    }
};

enum SynapseSpringsProc
{
    SPELL_SYNAPSE_STR_PROC  = 96229,
    SPELL_SYNAPSE_AGI_PROC  = 96228,
    SPELL_SYNAPSE_INT_PROC  = 96230
};

class spell_gen_synapse_springs : public SpellScriptLoader
{
public:
    spell_gen_synapse_springs() : SpellScriptLoader("spell_gen_synapse_springs") { }

    class spell_gen_synapse_springs_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_synapse_springs_SpellScript);

        void HandleDummy(SpellEffIndex /* effIndex */)
        {
            Unit* caster = GetCaster();
            float stat = caster->GetStat(STAT_STRENGTH);
            uint32 id_proc = SPELL_SYNAPSE_STR_PROC;
            // agility
            if (caster->GetStat(STAT_AGILITY) > stat)
            {
                stat = caster->GetStat(STAT_AGILITY);
                id_proc = SPELL_SYNAPSE_AGI_PROC;
            }
            // intellect
            if (caster->GetStat(STAT_INTELLECT) > stat)
                id_proc = SPELL_SYNAPSE_INT_PROC;
            caster->CastSpell(caster, id_proc, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_synapse_springs_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_synapse_springs_SpellScript();
    }
};

enum PlasmaShieldProc
{
    SPELL_PLASMA_SHIELD_ABSORB  = 82627,
    SPELL_PLASMA_PAINFUL_SHOCK  = 82407,
    SPELL_PLASMA_SHIELD_MAGNETIZED = 82403,
    SPELL_PLASMA_SHIELD_REVERSE_SHIELD = 82406,
};

class spell_gen_plasma_shield : public SpellScriptLoader
{
public:
    spell_gen_plasma_shield() : SpellScriptLoader("spell_gen_plasma_shield") { }

    class spell_gen_plasma_shield_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_plasma_shield_SpellScript);

        void HandleDummy(SpellEffIndex /* effIndex */)
        {
            Unit *caster = GetCaster();
            if (!caster || caster->GetTypeId() != TYPEID_PLAYER || caster->ToPlayer()->InArena())
                return;

            if (roll_chance_i(3))
                caster->CastSpell(caster, SPELL_PLASMA_SHIELD_REVERSE_SHIELD, true);
            else
            {
                if (roll_chance_i(3))
                {
                    switch (rand() % 1)
                    {
                    case 0:
                        caster->CastSpell(caster, SPELL_PLASMA_SHIELD_MAGNETIZED, true);
                        break;
                    case 1:
                        caster->CastSpell(caster, SPELL_PLASMA_PAINFUL_SHOCK, true);
                        break;
                    }
                }
                caster->CastSpell(caster, SPELL_PLASMA_SHIELD_ABSORB, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_plasma_shield_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_plasma_shield_SpellScript();
    }
};

enum InvisibilityField
{
    SPELL_INVISIBILITY_FIELD = 82820,
    SPELL_INVISIBILITY_FIELD_DISRUPTION = 82854,
};

class InvisInterruptEvent : public BasicEvent
{
public:
    InvisInterruptEvent(Unit* player) : _player(player) { }

    bool Execute(uint64 /*execTime*/, uint32 /*diff*/)
    {
        if (_player && _player->IsInWorld())
        {
            _player->RemoveAurasDueToSpell(SPELL_INVISIBILITY_FIELD);
            _player->CastSpell(_player, SPELL_INVISIBILITY_FIELD_DISRUPTION, true);
        }
        return false;
    }

private:
    Unit* _player;
};

class spell_gen_invisibility_field : public SpellScriptLoader // 84348
{
public:
    spell_gen_invisibility_field() : SpellScriptLoader("spell_gen_invisibility_field") { }

    class spell_gen_invisibility_field_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_invisibility_field_SpellScript);

        void HandleDummy(SpellEffIndex /* effIndex */)
        {
            Unit* caster = GetCaster();
            if (caster->GetTypeId() != TYPEID_PLAYER || caster->ToPlayer()->InArena())
                return;

            if (roll_chance_i(10))
                caster->m_Events.AddEvent(new InvisInterruptEvent(caster), caster->m_Events.CalculateTime(urand(3000, 12000)));

            caster->CastSpell(caster, SPELL_INVISIBILITY_FIELD, true);
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_invisibility_field_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_invisibility_field_SpellScript();
    }
};

enum ParabolMindSpirit
{
    SPELL_DULLARD = 67809,
    SPELL_MIND_BATTLE = 67810,
    SPELL_PARABOL_CONTROL = 13181,
};

class spell_gen_parabol_mind_spirit : public SpellScriptLoader // 67799
{
public:
    spell_gen_parabol_mind_spirit() : SpellScriptLoader("spell_gen_parabol_mind_spirit") { }

    class spell_gen_parabol_mind_spirit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_parabol_mind_spirit_SpellScript);

        void HandleDummy(SpellEffIndex /* effIndex */)
        {
            Unit *caster = GetCaster();
            if (!caster || caster->GetTypeId() != TYPEID_PLAYER || caster->ToPlayer()->InArena())
                return;
            uint64 sel_guid = caster->ToPlayer()->GetSelection();
            Unit *target = Unit::GetUnit(*caster, sel_guid);
            if (!target || target->GetGUID() == caster->GetGUID())
                return;
            uint32 result = rand() % 100;
            if (result <= 65) // succes
            {
                if (roll_chance_i(3))
                    caster->CastSpell(target, SPELL_MIND_BATTLE, true);
                else
                    caster->CastSpell(target, SPELL_PARABOL_CONTROL, true);
            }
            else if (result <= 95)
                caster->CastSpell(caster, SPELL_DULLARD, true);
            else
                target->CastSpell(caster, SPELL_PARABOL_CONTROL, true);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_parabol_mind_spirit_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_parabol_mind_spirit_SpellScript();
    }
};


enum
{
    SPELL_ITEM_COLLECTING_MANA = 92272,
    SPELL_DETONATE_MANA        = 92601
};

class spell_gen_item_collecting_mana : public SpellScriptLoader // 92272
{
public:
    spell_gen_item_collecting_mana() : SpellScriptLoader("spell_gen_item_collecting_mana") { }

    class spell_gen_item_collecting_mana_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_item_collecting_mana_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DETONATE_MANA))
                return false;
            return true;
        }

        bool Load() override
        {
            maxAmount = 4200;
            return true;
        }

        void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            if (auto caster = GetCaster())
            {
                int32 mana = eventInfo.GetDamageInfo()->GetSpellInfo()->CalcPowerCost(caster, eventInfo.GetDamageInfo()->GetSchoolMask());
                if (AuraEffect* auraEffect1 = GetEffect(EFFECT_1))
                {
                    int32 manaStored = auraEffect1->GetAmount();
                    manaStored += mana * 0.2;
                    manaStored = std::min(manaStored, maxAmount);
                    auraEffect1->SetAmount(manaStored);
                }
            }
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_gen_item_collecting_mana_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
        }

    private:
        int32 maxAmount;
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_item_collecting_mana_AuraScript();
    }
};

enum DismountAndCancelAllShapeshifts
{
    SPELL_DISMISS_PET = 36562,
    SPELL_DISMOUNT = 61286,
};

class spell_gen_dismount_cancel_all_shapeshifts : public SpellScriptLoader
{
public:
    spell_gen_dismount_cancel_all_shapeshifts() : SpellScriptLoader("spell_gen_dismount_cancel_all_shapeshifts") { }

    class spell_gen_dismount_cancel_all_shapeshifts_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_dismount_cancel_all_shapeshifts_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();

            if (!caster)
                return;

            if (auto player = caster->ToPlayer())
            {
                player->Dismount();

                if (player->getClass() == CLASS_DRUID)
                    player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (player->GetPet())
                    player->CastSpell(player, SPELL_DISMISS_PET);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gen_dismount_cancel_all_shapeshifts_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_dismount_cancel_all_shapeshifts_SpellScript();
    }
};

class spell_gen_detonate_mana : public SpellScriptLoader // 92601
{
public:
    spell_gen_detonate_mana() : SpellScriptLoader("spell_gen_detonate_mana") { }

    class spell_gen_detonate_mana_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_detonate_mana_SpellScript);

        void HandleDamage(SpellEffIndex /* effIndex */)
        {
            if (GetCaster())
                if (Aura* aura = GetCaster()->GetAura(SPELL_ITEM_COLLECTING_MANA))
                    if (AuraEffect* effect = aura->GetEffect(EFFECT_1))
                        SetHitDamage(effect->GetAmount());
        }

        void HandleEnergize(SpellEffIndex /* effIndex */)
        {
            if (GetCaster())
                if (Aura* aura = GetCaster()->GetAura(SPELL_ITEM_COLLECTING_MANA))
                    if (AuraEffect* effect = aura->GetEffect(EFFECT_1))
                    {
                        SetEffectDamage(effect->GetAmount());
                        effect->SetAmount(0);
                    }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_detonate_mana_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnEffectHitTarget += SpellEffectFn(spell_gen_detonate_mana_SpellScript::HandleEnergize, EFFECT_1, SPELL_EFFECT_ENERGIZE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_detonate_mana_SpellScript();
    }
};

class spell_gen_eject_passenger : public SpellScriptLoader
{
public:
    spell_gen_eject_passenger() : SpellScriptLoader("spell_gen_eject_passenger") { }

    class spell_gen_eject_passenger_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_eject_passenger_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            if (Unit *caster = GetCaster())
            {
                if (Vehicle *veh = caster->GetVehicleKit())
                {
                    if (GetEffectValue() > 0)
                        veh->EjectPassenger(GetEffectValue() - 1);
                    else
                        veh->RemoveAllPassengers();
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_eject_passenger_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_eject_passenger_SpellScript();
    }
};

class spell_gen_t12_energize_pct : public SpellScriptLoader
{
public:
    spell_gen_t12_energize_pct() : SpellScriptLoader("spell_gen_t12_energize_pct") { }

    class spell_gen_t12_energize_pct_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_t12_energize_pct_SpellScript);

        void HandleEnergize(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
            {
                uint32 basehp = 0, basemana = 0;
                sObjectMgr->GetPlayerClassLevelInfo(target->getClass(), target->getLevel(), basehp, basemana);
                SetEffectDamage(CalculatePct(basemana, GetEffectValue()));
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_t12_energize_pct_SpellScript::HandleEnergize, EFFECT_0, SPELL_EFFECT_ENERGIZE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_t12_energize_pct_SpellScript();
    }
};

class spell_sota_carrying_seaforium : public SpellScriptLoader
{
public:
    spell_sota_carrying_seaforium() : SpellScriptLoader("spell_sota_carrying_seaforium")
    {}

    class spell_sota_carrying_seaforium_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_sota_carrying_seaforium_AuraScript);

        void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            PreventDefaultAction();
        }
        
        void HandleAfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH || GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_CANCEL)
            {
                int32 triggerSpell = aurEff->GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
                GetTarget()->CastSpell(GetTarget(), triggerSpell);
            }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_sota_carrying_seaforium_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_sota_carrying_seaforium_AuraScript::HandleAfterRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_sota_carrying_seaforium_AuraScript();
    }
};

class spell_gen_rocket_barrage_SpellScript : public SpellScript
{
PrepareSpellScript(spell_gen_rocket_barrage_SpellScript);

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        SetEffectDamage(int32(1 + 
            (0.25f * GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK)) +
            (0.429f * GetCaster()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE)) +
            (GetCaster()->getLevel() * 2)));
    }

    void Register()
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_gen_rocket_barrage_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

enum naliceEnrageData
{
    SPELL_ROGUE_SHIV    = 5940,
    SPELL_SATED        = 109891
};

class spell_gen_nalice_enrage : public SpellScriptLoader
{
public:
    spell_gen_nalice_enrage() : SpellScriptLoader("spell_gen_nalice_enrage") { }

    class spell_gen_nalice_enrage_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_nalice_enrage_AuraScript);

        void HandleOnDispel(DispelInfo* dispelData)
        {
            if (dispelData->GetDispellerSpellId() == SPELL_ROGUE_SHIV)
                GetUnitOwner()->CastSpell(GetUnitOwner(), SPELL_SATED, true);
        }

        void Register() override
        {
            OnDispel += AuraDispelFn(spell_gen_nalice_enrage_AuraScript::HandleOnDispel);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_gen_nalice_enrage_AuraScript();
    }
};

enum JournalSpells
{
    SPELL_TERRAIN_EXPERT        = 86972,
    SPELL_LEARNING_BY_EXAMPLE   = 86963,
    SPELL_LIVING_FOREVER        = 86988,
    SPELL_KNEECAPPER            = 86977,
    SPELL_THE_GREAT_HUNT        = 86992,
    SPELL_DEADLY_DRIVE          = 86980,
    SPELL_DRAGONSBANE           = 86975,
    SPELL_28_CHEAPTERS_LATER    = 86983,
    SPELL_FIRE_AND_WATER        = 86976,
    SPELL_MECHANICAL_APTITUDE   = 86982
};

class spell_gen_read_journal : public SpellScriptLoader
{
public:
    spell_gen_read_journal() : SpellScriptLoader("spell_gen_read_journal") { }

    class spell_gen_read_journal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_read_journal_SpellScript);

        SpellCastResult CheckCasterLevel()
        {
            if (GetCaster()->getLevel() >= 85)
            {
                SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_TOO_WISE_AND_POWERFUL);
                return SPELL_FAILED_CUSTOM_ERROR;
            }
            return SPELL_CAST_OK;
        }

        void HandleScript(SpellEffIndex effIndex)
        {
            GetCaster()->CastSpell(GetCaster(), RAND(SPELL_TERRAIN_EXPERT, SPELL_LEARNING_BY_EXAMPLE, SPELL_LIVING_FOREVER, SPELL_KNEECAPPER, SPELL_THE_GREAT_HUNT, SPELL_DEADLY_DRIVE, SPELL_DRAGONSBANE, SPELL_28_CHEAPTERS_LATER, SPELL_FIRE_AND_WATER, SPELL_MECHANICAL_APTITUDE), true);
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_read_journal_SpellScript::CheckCasterLevel);
            OnEffectHitTarget += SpellEffectFn(spell_gen_read_journal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_read_journal_SpellScript();
    }
};


// Achievement: The Turkinator
enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL       = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX   = 62021,
};

#define THE_THUKINATOR_10           "Turkey Hunter!"
#define THE_THUKINATOR_20           "Turkey Domination!"
#define THE_THUKINATOR_30           "Turkey Slaughter!"
#define THE_THUKINATOR_40           "TURKEY TRIUMPH!"

class spell_gen_turkey_tracker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_tracker() : SpellScriptLoader("spell_gen_turkey_tracker") { }

        class spell_gen_turkey_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_turkey_tracker_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return;

                Player* target = GetHitPlayer();
                if (!target)
                    return;

                if (Aura const* aura = GetCaster()->ToPlayer()->GetAura(GetSpellInfo()->Id))
                {
                    switch (aura->GetStackAmount())
                    {
                    case 10:
                        target->MonsterTextEmote(THE_THUKINATOR_10, 0, true);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 20:
                        target->MonsterTextEmote(THE_THUKINATOR_20, 0, true);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 30:
                        target->MonsterTextEmote(THE_THUKINATOR_30, 0, true);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 40:
                        target->MonsterTextEmote(THE_THUKINATOR_40, 0, true);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL_MAX); // Achievement Credit
                        break;
                    default:
                        break;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_turkey_tracker_SpellScript();
        }
};

class spell_gen_feast_on : public SpellScriptLoader
{
    public:
        spell_gen_feast_on() : SpellScriptLoader("spell_gen_feast_on") { }

        class spell_gen_feast_on_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_feast_on_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                    if (auto player = caster->GetVehicleKit()->GetPassenger(0))
                        caster->CastSpell(player, bp0, true, NULL, NULL, player->ToPlayer()->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_feast_on_SpellScript();
        }
};

enum WellFedPilgrimsBounty
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY           = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES      = 61804,
    SPELL_A_SERVING_OF_STUFFING         = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES   = 61808,
    SPELL_A_SERVING_OF_PIE              = 61805,

    // Well Fed
    SPELL_WELL_FED_AP                   = 65414,
    SPELL_WELL_FED_ZM                   = 65412,
    SPELL_WELL_FED_HIT                  = 65416,
    SPELL_WELL_FED_HASTE                = 65410,
    SPELL_WELL_FED_SPIRIT               = 65415,

    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING         = 61849,
};

class spell_gen_well_fed_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bounty_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_well_fed_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBounty
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,
};

class spell_gen_on_plate_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, NULL, NULL, player->ToPlayer()->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bounty_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_on_plate_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK          = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD           = 66478,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT    = 66622,
};

class spell_gen_bountiful_feast : public SpellScriptLoader
{
    public:
        spell_gen_bountiful_feast() : SpellScriptLoader("spell_gen_bountiful_feast") { }

        class spell_gen_bountiful_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bountiful_feast_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bountiful_feast_SpellScript();
        }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415,
};

class spell_pilgrims_bounty_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bounty_buff_food(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bounty_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bounty_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load()
            {
                _handled = false;
                return true;
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                _handled = true;
                caster->CastSpell(caster, _triggeredSpellId, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bounty_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pilgrims_bounty_buff_food_AuraScript(_triggeredSpellId);
        }
};

class spell_depth_charge_countdown_pulse : public SpellScriptLoader
{
public:
    spell_depth_charge_countdown_pulse() : SpellScriptLoader("spell_depth_charge_countdown_pulse") {}

    class spell_depth_charge_countdown_pulse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_depth_charge_countdown_pulse_AuraScript);

        void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (GetTarget()->GetTypeId() != TYPEID_UNIT)
                return;

            GetTarget()->CastSpell(GetTarget(), GetSpellInfo()->Effects[EFFECT_2].BasePoints, true);
            GetTarget()->ToCreature()->DespawnOrUnsummon(750);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_depth_charge_countdown_pulse_AuraScript::HandleOnRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_depth_charge_countdown_pulse_AuraScript();
    }
};


enum QuestRideTheLightning
{
    AURA_REPOLARZIED_SPHERE = 37830,
    QUEST_KILLCREDIT = 21910,
};

class spell_gen_lightning_strike : public SpellScriptLoader
{
public:
    spell_gen_lightning_strike() : SpellScriptLoader("spell_gen_lightning_strike") { }

    class spell_gen_lightning_strikeSpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_lightning_strikeSpellScript);

        void HandleOnHit(SpellEffIndex /*effIndex*/)
        {
            Player* target = GetHitPlayer();
            if (!target)
                return;

            if (target->HasAura(AURA_REPOLARZIED_SPHERE))
                target->KilledMonsterCredit(QUEST_KILLCREDIT);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_lightning_strikeSpellScript::HandleOnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_lightning_strikeSpellScript();
    }
};

class spell_gen_TolBarad_Teleport : public SpellScriptLoader
{
    /*
    INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('88339', 'spell_gen_TolBarad_Teleport');
    INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES ('88341', 'spell_gen_TolBarad_Teleport');
    */
public:
    spell_gen_TolBarad_Teleport() : SpellScriptLoader("spell_gen_TolBarad_Teleport") { }

    class spell_gen_TolBarad_Teleport_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_TolBarad_Teleport_SpellScript);

        void removeAuras()
        {
            if (auto victim = GetCaster())
                if (auto player = victim->ToPlayer())
                    if (player->getClass() == CLASS_DRUID)
                    {
                        player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                        //TC_LOG_ERROR("entities.unit", "removing druid shapeshift auras.");
                    }
        }

        void Register() override
        {
            AfterCast += SpellCastFn(spell_gen_TolBarad_Teleport_SpellScript::removeAuras);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_gen_TolBarad_Teleport_SpellScript();
    }
};

class spell_gen_placement_pvp_detection_script : public SpellScriptLoader
{
public:
    spell_gen_placement_pvp_detection_script() : SpellScriptLoader("spell_gen_placement_pvp_detection_script") { }

    class spell_gen_placement_pvp_detection : public SpellScript
    {
        PrepareSpellScript(spell_gen_placement_pvp_detection);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            //   if (GetExplTargetDest())
            //   TC_LOG_ERROR("sql.sql", "1, GUID: %u", GetCaster()->GetGUID());
            if (auto caster = GetCaster())
                if (Position* landTarget = GetHitDest())
                {
                    if (!caster->ToPlayer())
                        if (caster->ToTempSummon() || caster->ToPet())
                            if (caster->GetOwner())
                                if (caster->GetOwner()->ToPlayer())
                                {
                                    //change "caster" to the player if done by a pet.
                                    caster = caster->GetOwner();
                                }
                    /*
                        Grab all enemy units within 80 yards of the caster
                    */
                    std::list<Unit*> targets;
                    Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(caster, caster, 80.0f);
                    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targets, u_check);
                    caster->VisitNearbyObject(80.0f, searcher);

                    int32 nearestEnemyPlayer_dist = NULL;
                    Player* nearestEnemyPlayer = NULL;
                    /*
                        Iterate through all gathered units.
                        If the current iteration (temp) is closer than the current closest, temp becomes the closest.
                    */

                    if (targets.size())
                        for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end();)
                        {
                            Unit* temp = (*itr);
                            itr++;
                            if (temp != NULL)
                                if ((temp)->ToPlayer())
                                    if (!temp->ToPlayer()->isGameMaster())
                                        if (temp->GetGUID() != caster->GetGUID())
                                            if (!nearestEnemyPlayer ||
                                                (temp->GetExactDist(landTarget->GetPositionX(), landTarget->GetPositionY(), landTarget->GetPositionZ()) <
                                                    nearestEnemyPlayer->GetExactDist(landTarget->GetPositionX(), landTarget->GetPositionY(), landTarget->GetPositionZ())))
                                            {
                                                nearestEnemyPlayer = temp->ToPlayer();
                                                nearestEnemyPlayer_dist = int32(10000.0f * nearestEnemyPlayer->GetExactDist(landTarget->GetPositionX(), landTarget->GetPositionY(), landTarget->GetPositionZ()));
                                            }
                        }

                    if (nearestEnemyPlayer)
                        if (auto caster = GetCaster())
                        if (auto casterPlayer = caster->ToPlayer())
                        if (auto thisBG = casterPlayer->GetBattleground())
                            if (thisBG->isArena())
                                if (uint32 interrupterTeamID = casterPlayer->GetArenaTeamId(thisBG->GetArenaSlot()))
                                    if (int32 caster_targetLoc_dist = int32(GetCaster()->GetExactDist(landTarget->GetPositionX(), landTarget->GetPositionY(), landTarget->GetPositionZ())))
                                        if (PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PVP_DETECTION_DATA))
                                        {
                                            stmt->setUInt64(0, caster->GetGUID());                                                              //playerGUID
                                            stmt->setUInt32(1, PVP_DETECTION_EVENT_SPELL_PLACEMENT);                                            //eventType
                                                                                                                                                //DateAndTime filled already
                                            stmt->setUInt32(2, uint64(nearestEnemyPlayer->GetGUID()));                                          //data1 = guid of nearest player
                                            stmt->setUInt32(3, (thisBG->isArena() ? thisBG->GetArenaType() : thisBG->GetMaxPlayersPerTeam()));  //data2 = bracket size
                                            stmt->setUInt32(4, (thisBG->isArena() ? interrupterTeamID : 0));                                    //data3 = team of player who just interrupted a spell
                                            stmt->setUInt32(5, GetSpellInfo()->Id);                                                             //data4 = spell being placed
                                            stmt->setUInt32(6, (caster->ToPlayer()->GetSession()->GetLatency()));                               //data5 = Casting player's latency
                                            stmt->setUInt32(7, nearestEnemyPlayer_dist);                                                        //data6 = Distance (x10,000) between cast center and nearest player center.
                                            stmt->setUInt32(8, caster_targetLoc_dist);                                                          //data7 = Distance (x1) between caster and selected location. This will indicate
                                            CharacterDatabase.Execute(stmt);
                                        }
                }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SUMMON);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DISPEL);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_ADD_FARSIGHT);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_PERSISTENT_AREA_AURA);
            OnEffectLaunch += SpellEffectFn(spell_gen_placement_pvp_detection::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DISTRACT);

        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_placement_pvp_detection();
    }
};

enum Transmute_LivingElements {
    VOLATILE_FIRE_ID = 52325,
    VOLATILE_WATER_ID = 52326,
    VOLATILE_EARTH_ID = 52327,
    VOLATILE_AIR_ID = 52328,
};

class spell_transmute_livinge_lements : public SpellScriptLoader
{
public:
    spell_transmute_livinge_lements() : SpellScriptLoader("spell_transmute_livinge_lements") { }

    class spell_transmute_livinge_lements_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_transmute_livinge_lements_SpellScript);

        void HandleDummy()
        {
            Unit* caster = GetCaster();
            if (auto player = GetCaster()->ToPlayer())
            {
                int zone = player->GetZoneId();
                TC_LOG_ERROR("misc", "ZONE: %d CD: %d", zone, GetSpellInfo()->GetRecoveryTime());
                if (zone == 5034) { // ULDUM
                    player->AddItem(VOLATILE_AIR_ID, urand(14, 16));
                }
                else if (zone == 5042) { // DEEPHOLM
                    player->AddItem(VOLATILE_EARTH_ID, urand(14, 16));
                }
                else if (zone == 5146) { // VASHJIR
                    player->AddItem(VOLATILE_WATER_ID, urand(14, 16));
                }
                else if (zone == 616) { // MOUNT HYJAL
                    player->AddItem(VOLATILE_FIRE_ID, urand(14, 16));
                }
                else {
                    int Random = urand(1, 100);
                    if (Random < 25) {
                        player->AddItem(VOLATILE_FIRE_ID, urand(14, 16));
                    }
                    else if (Random < 50) {
                        player->AddItem(VOLATILE_WATER_ID, urand(14, 16));
                    }
                    else if (Random < 75) {
                        player->AddItem(VOLATILE_EARTH_ID, urand(14, 16));
                    }
                    else {
                        player->AddItem(VOLATILE_AIR_ID, urand(14, 16));
                    }
                }
            }
        }
        void Register()
        {
            OnCast += SpellCastFn(spell_transmute_livinge_lements_SpellScript::HandleDummy);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_transmute_livinge_lements_SpellScript();
    }
};

class spell_dragonwrath_whisper_trigger : public SpellScriptLoader
{
public:
    spell_dragonwrath_whisper_trigger() : SpellScriptLoader("spell_dragonwrath_whisper_trigger") { }

    class spell_dragonwrath_whisper_trigger_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dragonwrath_whisper_trigger_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (auto target = GetTarget()->ToPlayer())
                if (!target->HasAura(100874))
                {
                    target->AddAura(100874, target);
                }
        }
        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (auto target = GetTarget()->ToPlayer())
            {
                target->RemoveAura(100874);
            }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_dragonwrath_whisper_trigger_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectApply += AuraEffectRemoveFn(spell_dragonwrath_whisper_trigger_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dragonwrath_whisper_trigger_AuraScript();
    }
};

class spell_dragonwrath_whisper : public SpellScriptLoader
{
public:
    spell_dragonwrath_whisper() : SpellScriptLoader("spell_dragonwrath_whisper") { }

    class spell_dragonwrath_whisper_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dragonwrath_whisper_AuraScript);

        void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
        {
            if (!GetAura()->IsExpired())
                return;

            if (auto owner = GetOwner())
                if (auto target = owner->ToUnit())
                    if (auto p = target->ToPlayer())
                    {

                        target->AddAura(100874, target); //Renew timer
                        if (urand(0, 2) == 2)
                        {
                            Position pos(*target);
                            auto trigger = target->SummonCreature(52832, pos);
                            trigger->SetVisible(false);
                            trigger->SetReactState(REACT_PASSIVE);
                            trigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                            //target->AddAura(100874, target); //Renew timer
                            trigger->TalkWithDelay(2000, 0, p->GetGUID(), CHAT_MSG_MONSTER_WHISPER);
                            trigger->DespawnOrUnsummon(5000);
                            if (auto session = p->GetSession())
                            {
                                WorldPacket data2(SMSG_MESSAGECHAT, 200);
                                std::stringstream warning_to_send2{ "" };
                                warning_to_send2 << "The warm embrace of Tarecgosa's presence encircles you.";
                                auto text_pre2{ warning_to_send2.str() };
                                auto text2{ text_pre2.c_str() };

                                auto loc_idx{ session->GetSessionDbLocaleIndex() };
                                trigger->BuildMonsterChat(&data2, CHAT_MSG_MONSTER_EMOTE, text2, LANG_UNIVERSAL, trigger->GetNameForLocaleIdx(loc_idx), p->GetGUID());
                                session->SendPacket(&data2);
                            }
                        }
                    }
        }
        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_dragonwrath_whisper_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dragonwrath_whisper_AuraScript();
    }
};



class spell_gen_persistant_aoe_los_check : public SpellScriptLoader
{
public:
    spell_gen_persistant_aoe_los_check() : SpellScriptLoader("spell_gen_persistant_aoe_los_check") { }

    class spell_gen_persistant_aoe_los_check_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_persistant_aoe_los_check_AuraScript);

        bool CheckAreaTarget(Unit* target)
        {
            if (target)
                if (auto s = GetOwner())
                    if (auto info = GetSpellInfo())
                        if (!(info->AttributesEx2 & SPELL_ATTR2_CAN_TARGET_NOT_IN_LOS))
                            if (!target->IsWithinLOS(s->GetPositionX(), s->GetPositionY(), s->GetPositionZ() + 1.f))
                                return false;

            return true;
        }
        void Register()
        {
            DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_persistant_aoe_los_check_AuraScript::CheckAreaTarget);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_persistant_aoe_los_check_AuraScript();
    }
};

class spell_gen_double_dip : public SpellScriptLoader
{
public:
    spell_gen_double_dip() : SpellScriptLoader("spell_gen_double_dip") { }

    class spell_gen_double_dip_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_double_dip_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            if (auto c = GetCaster())
                if (auto p = c->ToPlayer())
                    if (p->GetPrimaryStat() != STAT_STRENGTH)
                        if (auto damage = GetHitDamage())
                            if (auto target = GetHitUnit())
                                if (auto p_target = target->ToPlayer())
                                {
                                    c->ApplyResilience(target, &damage);
                                    SetHitDamage(damage);
                                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_double_dip_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_double_dip_SpellScript();
    }
};

void AddSC_generic_spell_scripts()
{
    new spell_gen_double_dip();
    new spell_gen_persistant_aoe_los_check();
    new spell_gen_placement_pvp_detection_script();
    new spell_gen_DragonSoul_crossproc();
    new spell_gen_TolBarad_Teleport();
    new spell_gen_flag_of_ownership();
    new spell_gen_absorb0_hitlimit1();
    new spell_gen_adaptive_warding();
    new spell_gen_aura_of_anger();
    new spell_gen_av_drekthar_presence();
    new spell_gen_burn_brutallus();
    new spell_gen_cannibalize();
    new spell_gen_create_lance();
    new spell_gen_netherbloom();
    new spell_gen_nightmare_vine();
    new spell_gen_obsidian_armor();
    new spell_gen_parachute();
    new spell_gen_pet_summoned();
    new spell_gen_remove_flight_auras();
    new spell_creature_permanent_feign_death();
    new spell_pvp_trinket_wotf_shared_cd();
    new spell_gen_animal_blood();
    new spell_gen_divine_storm_cd_reset();
    new spell_gen_parachute_ic();
    new spell_gen_gunship_portal();
    new spell_gen_dungeon_credit();
    new spell_gen_profession_research();
    new spell_generic_clone_weapon();
    new spell_gen_clone_weapon_aura();
    new spell_gen_turkey_marker();
    new spell_gen_lifeblood();
    new spell_gen_allow_cast_from_item_only();
    new spell_gen_launch();
    new spell_gen_vehicle_scaling();
    new spell_gen_oracle_wolvar_reputation();
    new spell_gen_dummy_trigger();
    new spell_gen_spirit_healer_res();
    new spell_gen_gadgetzan_transporter_backfire();
    new spell_gen_gnomish_transporter();
    new spell_gen_dalaran_disguise("spell_gen_sunreaver_disguise");
    new spell_gen_dalaran_disguise("spell_gen_silver_covenant_disguise");
    new spell_gen_elune_candle();
    new spell_idle_BgRemoval();
    new spell_gen_break_shield("spell_gen_break_shield");
    new spell_gen_break_shield("spell_gen_tournament_counterattack");
    new spell_gen_mounted_charge();
    new spell_gen_defend();
    new spell_gen_tournament_duel();
    new spell_gen_summon_tournament_mount();
    new spell_gen_on_tournament_mount();
    new spell_gen_tournament_pennant();
    new spell_gen_chaos_blast();
    new spell_gen_wg_water();
    new spell_gen_count_pct_from_max_hp("spell_gen_default_count_pct_from_max_hp");
    new spell_gen_count_pct_from_max_hp("spell_gen_50pct_count_pct_from_max_hp", 50);
    new spell_gen_despawn_self();
    new spell_gen_touch_the_nightmare();
    new spell_gen_dream_funnel();
    new spell_gen_bandage();
    new spell_gen_lifebloom("spell_hexlord_lifebloom", SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_tur_ragepaw_lifebloom", SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_cenarion_scout_lifebloom", SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_twisted_visage_lifebloom", SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_faction_champion_dru_lifebloom", SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_summon_elemental("spell_gen_summon_fire_elemental", SPELL_SUMMON_FIRE_ELEMENTAL);
    new spell_gen_summon_elemental("spell_gen_summon_earth_elemental", SPELL_SUMMON_EARTH_ELEMENTAL);
    new spell_gen_upper_deck_create_foam_sword();
    new spell_gen_bonked();
    new spell_gen_increase_stats_buff("spell_pal_blessing_of_kings");
    new spell_gen_increase_stats_buff("spell_pal_blessing_of_might");
    new spell_gen_increase_stats_buff("spell_dru_mark_of_the_wild");
    new spell_gen_increase_stats_buff("spell_pri_power_word_fortitude");
    new spell_gen_increase_stats_buff("spell_pri_shadow_protection");
    new spell_gen_increase_stats_buff("spell_mage_arcane_brilliance");
    new spell_gen_increase_stats_buff("spell_mage_dalaran_brilliance");
    new spell_gen_replenishment();
    new spell_gen_nokaled();
    new spell_gen_running_wild();
    new spell_gen_two_forms();
    new spell_gen_darkflight();
    new spell_gen_pvp_trinket();
    new spell_gen_mount_crossfaction();
    new spell_gen_gift_of_naaru();
    new spell_gen_raid_haste("spell_bloodlust");
    new spell_gen_raid_haste("spell_heroism");
    new spell_gen_raid_haste("spell_time_warp");
    new spell_gen_raid_haste("spell_ancient_hysteria");
    new spell_gen_armor_specialization("spell_dru_armor_specialization");
    new spell_gen_armor_specialization("spell_rog_armor_specialization");
    new spell_gen_armor_specialization("spell_sha_armor_specialization");
    new spell_gen_armor_specialization("spell_hun_armor_specialization");
    new spell_gen_armor_specialization("spell_pal_armor_specialization");
    new spell_gen_armor_specialization("spell_dk_armor_specialization");
    new spell_gen_armor_specialization("spell_war_armor_specialization");
    new spell_gen_ignacious();
    new spell_gen_angerforge();
    new spell_select_random_target();
    new spell_gen_scales_of_life();
    new spell_gen_apparatus_of_khaz();
    new spell_vengeance();
    new spell_vengeance_periodic();
    new spell_gen_synapse_springs();
    new spell_gen_plasma_shield();
    new spell_gen_invisibility_field();
    new spell_gen_parabol_mind_spirit();
    new spell_gen_item_collecting_mana();
    new spell_gen_detonate_mana();
    new spell_gen_eject_passenger();
    new spell_gen_t12_energize_pct();
    new spell_sota_carrying_seaforium();
    new SpellScriptLoaderEx<spell_gen_rocket_barrage_SpellScript>("spell_gen_rocket_barrage");
    new spell_gen_nalice_enrage();
    new spell_gen_read_journal();
    new spell_gen_turkey_tracker();
    new spell_gen_feast_on();
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_gen_bountiful_feast();
    new spell_pilgrims_bounty_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
    new spell_depth_charge_countdown_pulse();
    new spell_gen_lightning_strike();
    new spell_transmute_livinge_lements();
    new spell_dragonwrath_whisper_trigger();
    new spell_dragonwrath_whisper();
    new spell_gen_dismount_cancel_all_shapeshifts();
}
