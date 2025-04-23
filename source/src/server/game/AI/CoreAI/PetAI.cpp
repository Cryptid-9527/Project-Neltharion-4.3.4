/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "PetAI.h"
#include "Errors.h"
#include "Pet.h"
#include "Player.h"
#include "DBCStores.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "SpellMgr.h"
#include "Creature.h"
#include "World.h"
#include "Util.h"
#include "Group.h"
#include "SpellInfo.h"

int PetAI::Permissible(const Creature* creature)
{
    if (creature->isPet())
        return PERMIT_BASE_SPECIAL;

    return PERMIT_BASE_NO;
}

PetAI::PetAI(Creature* c) : CreatureAI(c), i_tracker(TIME_INTERVAL_LOOK)
{
    m_AllySet.clear();
    UpdateAllies();
}

bool PetAI::_needToStop()
{
    // This is needed for charmed creatures, as once their target was reset other effects can trigger threat
    if (auto v = me->getVictim())
        {
            if (me->isCharmed())
                if (v == me->GetCharmer())
                    return true;

            return !me->IsValidAttackTarget(v);
        } 

    return true;
}

void PetAI::_stopAttack()
{
    if (!me->isAlive())
    {
        TC_LOG_DEBUG("misc", "Creature stoped attacking cuz his dead [guid=%u]", me->GetGUIDLow());
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->CombatStop();
        me->getHostileRefManager().deleteReferences();

        return;
    }

    me->AttackStop();
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop(); // Should stop pet's attack button from flashing
    me->GetCharmInfo()->SetIsCommandAttack(false);
    ClearCharmInfoFlags();
    HandleReturnMovement();
}

void PetAI::AttemptBasicAttack()
{
    if (!me)
        return;

    if (auto v = me->getVictim())
    {
        if (v->isAlive())
        {
            // is only necessary to stop casting, the pet must not exit combat
            if (v->HasBreakableByDamageCrowdControlAura(me))
            {
                me->InterruptNegativeNonMeleeSpells(false);
                return;
            }

            if (_needToStop())
            {
                TC_LOG_DEBUG("misc", "Pet AI stopped attacking [guid=%u]", me->GetGUIDLow());
                _stopAttack();
                return;
            }

            // Check before attacking to prevent pets from leaving stay position
            if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
            {
                if (me->GetCharmInfo()->IsCommandAttack() || (me->GetCharmInfo()->IsAtStay() && me->IsWithinMeleeRange(v)))
                    if (me->GetEntry() != 416 && me->GetEntry() != 1863 && me->GetEntry() != 510)
                        DoMeleeAttackIfReady();
            }
            else if (me->GetEntry() != 416 && me->GetEntry() != 1863 && me->GetEntry() != 510)
                DoMeleeAttackIfReady();
        }
        else
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (me->HasReactState(REACT_AGGRESSIVE) || me->GetCharmInfo()->IsAtStay())
                {

                    UpdateVictim();
                    // Every update we need to check targets only in certain cases
                    // Aggressive - Allow auto select if owner or pet don't have a target
                    // Stay - Only pick from pet or owner targets / attackers so targets won't run by
                    // while chasing our owner. Don't do auto select.
                    // All other cases (ie: defensive) - Targets are assigned by AttackedBy(), OwnerAttackedBy(), OwnerAttacked(), etc.
                    Unit* nextTarget = SelectNextTarget(me->HasReactState(REACT_AGGRESSIVE));

                    if (nextTarget)
                    {
                        return;
                    }
                    else
                        HandleReturnMovement();
                }
                else
                    HandleReturnMovement();
            }
        }
    }
        else
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                // Every update we need to check targets only in certain cases
                // Aggressive - Allow auto select if owner or pet don't have a target
                // Stay - Only pick from pet or owner targets / attackers so targets won't run by
                // while chasing our owner. Don't do auto select.
                // All other cases (ie: defensive) - Targets are assigned by AttackedBy(), OwnerAttackedBy(), OwnerAttacked(), etc.

                if (auto v = me->getVictim())
                {
                    if (UpdateVictim())
                    {
                        AttackStart(v);
                    }
                    else if (auto nextTarget = SelectNextTarget(me->HasReactState(REACT_AGGRESSIVE)))
                    {
                        AttackStart(nextTarget);
                    }
                    else HandleReturnMovement();
                }
                else
                    HandleReturnMovement();
            }
        }
}

//Generic Searcher
Unit* PetAI::GetAutoCastTarget(bool friendly, float dist)
{

    if (friendly)
    {
        auto friendlies = me->GetFriendlyUnitsInRange(dist, me);
        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(friendlies))
            return random_target;
        else
            return me;
    }
    else
    {
        if (auto v = me->getVictim())
            return v;
        else
        {
            if (auto charm_info = me->GetCharmInfo())
            if (charm_info->HasCommandState(COMMAND_ATTACK))
            {

                if (auto nextTarget = SelectNextTarget(true))
                    return nextTarget;
            }
                else return nullptr;
            else return nullptr;
        }
    }

    return nullptr;
}

Unit* PetAI::AutoCast_FindTarget(uint32 spell_id)
{
    auto info = sSpellMgr->GetSpellInfo(spell_id);

    /*
        Logic to decide on searching for a friendly or opponent.
    */
    bool assist{ false };
    switch (spell_id)
    {
    case 90361://spirit beast heal
    {
        Unit* cast_on{ nullptr };
        auto friendlies = me->GetFriendlyUnitsInRange(25.f, me);
        if (friendlies.size())
        {
            float min_pct{ 100.f };

            for (auto target : friendlies)
                if (auto pct = target->GetHealth()/target->GetMaxHealth())
                    if (pct < 100.f)
                    if (pct < min_pct)
                    {
                        cast_on = target;
                        min_pct = pct;
                    }
            if (cast_on)
                return cast_on;
        }
        else
        {
            return me->GetOwner();
        }

        return me->GetOwner();
        break;
    }
        //self casts
    case 24604://wolf howl - attack power
    case 90355://ancient hysteria - heroism
    case 97229://bellowing roar - crit chance
    case 50256://demoralizing roar - aoe bear
    case 24423://screech - aoe dmg reduction
    case 90339:
    case 90309:
    case 54424://warlock pet int buff
    case 6307: //warlock imp stam buff
    case 93435:
    case 24450://prowl
    case 61684://sprint
    case 53490://bull headed
        return me->ToUnit();
        break;
    default:
            return GetAutoCastTarget(false, 20.f);
        break;
    }
    return (Unit*)NULL;
}

bool PetAI::AutoCast_ExtraChecks(Unit* target, uint32 spell_id)
{
    if (!target)
        return false;

    switch (spell_id)
    {
    case 6307: //warlock imp stam buff
    case 54424://warlock pet int buff
        if (auto o = me->GetOwner())
        {
            if (o->HasAura(spell_id, me->GetGUID()))
                return false;
            return true;
        }
        break;

    case 24450://prowl
        if (me->HasAura(spell_id, me->GetGUID()))
            return false;
        return true;
        break;
    default:
        // is only necessary to stop casting, the pet must not exit combat
        if (me->IsValidAttackTarget(target))
        if (target->HasBreakableByDamageCrowdControlAura(me))
        {
            me->InterruptNegativeNonMeleeSpells(false);
            return false;
        }
        return true;
        break;
    }
    //something is fucked if we make it here. don't go through with a cast.
    return false;
}

void PetAI::AttemptAutoSpells()
{
    /*
        The plan employed here is to iterate through all of the action bar spells set to autocast. 
    */

    /*
    typedef std::vector<std::pair<Unit*, Spell*> > TargetSpellList;
    TargetSpellList targetSpellStore;
    */


    bool casted{ false };

    if (auto pet = me->ToPet())
        if (!pet->hasCrowdControlPreventingActions())
    if (auto owner = me->GetCharmerOrOwner())
        if (auto charm_info = me->GetCharmInfo())
            for (uint8 i = 0; i < ACTION_BAR_INDEX_END; ++i)
                if (!casted)//ONLY CAST ONCE PER TICK
                    if (auto button = charm_info->GetActionBarEntry(i))
                        if (auto action = button->GetAction())
                            if (button->GetType() == ACTIONBAR_STATE_AURA_ACTIVE_OR_SPELL_AUTOCAST)
                                if (auto spell_info = sSpellMgr->GetSpellInfo(action))
                                    if (!me->HasSpellCooldown(action))
                                        if (!charm_info->GetGlobalCooldownMgr().HasGlobalCooldown(spell_info))
                                            if (auto t = AutoCast_FindTarget(spell_info->Id))
                                            {
                                                //TC_LOG_ERROR("sql.sql", "inside block for spell %u", action);
                                                SpellCastResult result{ SPELL_FAILED_NOT_HERE };

                                                Spell* pre_spell = new Spell(me, spell_info, TRIGGERED_NONE, 0);
                                                result = pre_spell->CheckPetCast(t);
                                                if (result == SPELL_CAST_OK)
                                                if (AutoCast_ExtraChecks(t, spell_info->Id))
                                                {
                                                    //TC_LOG_ERROR("sql.sql", "casting spell %u on %u", action, t->GetGUID());
                                                    me->CastSpell(t, spell_info->Id);
                                                    casted = true;
                                                    me->SendUpdateToPlayer(owner->ToPlayer());
                                                }
                                                /*
                                                else TC_LOG_ERROR("sql.sql", "extra checks failed for spell %u", action);
                                                else TC_LOG_ERROR("sql.sql", "spell cast checks failed for spell %u, results: %u", action, result);
                                                */

                                                delete pre_spell;
                                            }
    /*
                                            else TC_LOG_ERROR("sql.sql", "could not find target for spell %u", action);
                                        else TC_LOG_ERROR("sql.sql", "has cat cooldown for spell %u", action);
                                    else TC_LOG_ERROR("sql.sql", "has spell cooldown for spell %u", action);
                                else TC_LOG_ERROR("sql.sql", "could not find spell info for spell %u", action);
                            else TC_LOG_ERROR("sql.sql", "button for spell %u is in bad state: %u", action, button->GetType());
                        else TC_LOG_ERROR("sql.sql", "could not find button data for slot %u", i);
    */
    /*
        if (false)
    {
        uint32 spellID = me->GetPetAutoSpellOnPos(i);
        if (!spellID)
            continue;

        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellID);
        if (!spellInfo)
            continue;

        // Ghoul energy should never go below 30 when autocasting
        if (me->getPowerType() == POWER_ENERGY && me->GetPower(me->getPowerType()) <= 70
            && me->GetOwner()->getClass() == CLASS_DEATH_KNIGHT)
            continue;

        if (me->GetCharmInfo() && me->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
            continue;

        if (spellInfo->IsPositive())
        {
            if (spellInfo->CanBeUsedInCombat())
            {
            }

            Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, 0);
            bool spellUsed = false;

            // Some spells can target enemy or friendly (DK Ghoul's Leap)
            // Check for enemy first (pet then owner)
            Unit* target = me->getAttackerForHelper();
            if (!target && owner)
                target = owner->getAttackerForHelper();

            if (target)
            {
                if (CanAttack(target) && spell->CanAutoCast(target))
                {
                    targetSpellStore.push_back(std::make_pair(target, spell));
                    spellUsed = true;
                }
            }

            if (spellInfo->HasEffect(SPELL_EFFECT_JUMP_DEST))
            {
                if (!spellUsed)
                    delete spell;

                continue; // Pets must only jump to target
            }

            // No enemy, check friendly
            if (!spellUsed)
            {
                for (auto tar = m_AllySet.begin(); tar != m_AllySet.end(); ++tar)
                {
                    Unit* ally = ObjectAccessor::GetUnit(*me, *tar);

                    //only buff targets that are in combat, unless the spell can only be cast while out of combat
                    if (!ally)
                        continue;

                    if (spell->CanAutoCast(ally) && ally->HasAura(spellInfo->Id))
                    {
                        targetSpellStore.push_back(std::make_pair(ally, spell));
                        spellUsed = true;
                        break;
                    }
                }
            }

            // No valid targets at all
            if (!spellUsed)
                delete spell;
        }
        else if (me->getVictim() && CanAttack(me->getVictim()) && spellInfo->CanBeUsedInCombat())
        {
            Spell* spell = new Spell(me, spellInfo, TRIGGERED_NONE, 0);
            if (spell->CanAutoCast(me->getVictim()))
                targetSpellStore.push_back(std::make_pair(me->getVictim(), spell));
            else
                delete spell;
        }
    }

    //found units to cast on to
    if (!targetSpellStore.empty())
    {
        uint32 index = urand(0, targetSpellStore.size() - 1);

        Spell* spell = targetSpellStore[index].second;
        Unit* target = targetSpellStore[index].first;

        targetSpellStore.erase(targetSpellStore.begin() + index);

        SpellCastTargets targets;
        targets.SetUnitTarget(target);

        if (!me->HasInArc(M_PI, target))
        {
            me->SetInFront(target);
            if (target && target->GetTypeId() == TYPEID_PLAYER)
                me->SendUpdateToPlayer(target->ToPlayer());

            if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                me->SendUpdateToPlayer(owner->ToPlayer());
        }
        spell->prepare(&targets);
    }

    // deleted cached Spell objects
    for (auto itr = targetSpellStore.begin(); itr != targetSpellStore.end(); ++itr)
        delete itr->second;

    targetSpellStore.clear();

        */
}

void PetAI::UpdateAI(const uint32 diff)
{
    if (!me->isAlive() || !me->GetCharmInfo())
        return;

    if (m_updateAlliesTimer <= diff)
        // UpdateAllies self set update timer
        UpdateAllies();
    else
        m_updateAlliesTimer -= diff;

    if (me->HasBreakableByDamageCrowdControlAura())
        return;

    AttemptBasicAttack();

    // Autocast (casted only in combat or persistent spells in any state)
    if (!me->HasUnitState(UNIT_STATE_CASTING))
        AttemptAutoSpells();
}

void PetAI::UpdateAllies()
{
    Unit* owner = me->GetCharmerOrOwner();
    Group* group = NULL;

    m_updateAlliesTimer = 10*IN_MILLISECONDS;                //update friendly targets every 10 seconds, lesser checks increase performance

    if (!owner)
        return;
    else if (owner->GetTypeId() == TYPEID_PLAYER)
        group = owner->ToPlayer()->GetGroup();

    //only pet and owner/not in group->ok
    if (m_AllySet.size() == 2 && !group)
        return;

    //owner is in group; group members filled in already (no raid -> subgroupcount = whole count)
    if (group && !group->isRaidGroup() && m_AllySet.size() == (group->GetMembersCount() + 2))
        return;

    m_AllySet.clear();
    m_AllySet.insert(me->GetGUID());
    if (group)                                              //add group
    {
        for (auto itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();
            if (!Target || !group->SameSubGroup((Player*)owner, Target))
                continue;

            if (Target->GetGUID() == owner->GetGUID())
                continue;

            m_AllySet.insert(Target->GetGUID());
        }
    }
    else                                                    //remove group
        m_AllySet.insert(owner->GetGUID());
}

void PetAI::KilledUnit(Unit* victim)
{
    // Called from Unit::Kill() in case where pet or owner kills something
    // if owner killed this victim, pet may still be attacking something else
    if (auto v = me->getVictim())
    if (v != victim)
        return;

    // Clear target just in case. May help problem where health / focus / mana
    // regen gets stuck. Also resets attack command.
    // Can't use _stopAttack() because that activates movement handlers and ignores
    // next target selection
    me->AttackStop(victim);
    me->InterruptNonMeleeSpells(false);
    me->SendMeleeAttackStop();  // Stops the pet's 'Attack' button from flashing

    // Before returning to owner, see if there are more things to attack
    if (auto nextTarget = SelectNextTarget(false))
        AttackStart(nextTarget);
    else
        HandleReturnMovement(); // Return
}

void PetAI::AttackStart(Unit* target)
{
    if (!target)
        return;

    if (auto o = me->GetOwner())
        if (!o->IsValidAttackTarget(target))
            return;
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!CanAttack(target))
        return;

    // Only chase if not commanded to stay or if stay but commanded to attack
    DoAttack(target, ((!me->GetCharmInfo()->HasCommandState(COMMAND_STAY) && !me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
        || me->GetCharmInfo()->IsCommandAttack()));
}

void PetAI::AttackStart(Unit* target, uint32 spellId)
{
    if (!target)
        return;

    if (auto o = me->GetOwner())
        if (!o->IsValidAttackTarget(target))
            return;
    // Overrides Unit::AttackStart to correctly evaluate Pet states

    // Check all pet states to decide if we can attack this target
    if (!CanAttack(target))
        return;

    // Only chase if not commanded to stay or if stay but commanded to attack
    DoAttack(target, ((!me->GetCharmInfo()->HasCommandState(COMMAND_STAY) && !me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
        || me->GetCharmInfo()->IsCommandAttack()), spellId);
}


void PetAI::JustAttacked(Unit* attacker)
{
    if (auto aura = me->GetAura(51755))
        aura->Remove(AURA_REMOVE_BY_DAMAGE);

    if (auto o = me->GetOwner())
        if (auto aura = o->GetAura(51755))
            aura->Remove(AURA_REMOVE_BY_DAMAGE);
}

void PetAI::OwnerAttackedBy(Unit* attacker)
{
    // Called when owner takes damage. This function helps keep pets from running off
    //  simply due to owner gaining aggro.

    if (!attacker)
        return;

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->getVictim() && me->getVictim()->isAlive())
        return;

    // Continue to evaluate and attack if necessary
    if (me->HasReactState(REACT_DEFENSIVE))
    {
        if (auto v = me->getVictim())
        {

        }
        else
        {
            AttackStart(attacker);
        }
    }
}

void PetAI::OwnerAttacked(Unit* target)
{
    // Called when owner attacks something. Allows defensive pets to know
    //  that they need to assist

    // Target might be NULL if called from spell with invalid cast targets
    if (!target)
        return;

    // Passive pets don't do anything
    if (!me->HasReactState(REACT_ASSIST))
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(target);
}

Unit* PetAI::SelectNextTarget(bool allowAutoSelect) const
{
    if (!me)
        return NULL;

    // Provides next target selection after current target death.
    // This function should only be called internally by the AI
    // Targets are not evaluated here for being valid targets, that is done in _CanAttack()
    // The parameter: allowAutoSelect lets us disable aggressive pet auto targeting for certain situations

    // Passive pets don't do next target selection
    if (me->HasReactState(REACT_PASSIVE))
        return NULL;

    // Check pet attackers first so we don't drag a bunch of targets to the owner
    if (auto myAttacker = me->getAttackerForHelper())
        if (!myAttacker->HasBreakableByDamageCrowdControlAura())
            return myAttacker;

    // Not sure why we wouldn't have an owner but just in case...
    if (!me->GetCharmerOrOwner())
        return NULL;

    // Check owner attackers
    if (auto o = me->GetCharmerOrOwner())
    if (auto ownerAttacker = o->getAttackerForHelper())
        if (!ownerAttacker->HasBreakableByDamageCrowdControlAura())
            return ownerAttacker;

    // Check owner victim
    // 3.0.2 - Pets now start attacking their owners victim in defensive mode as soon as the hunter does
    if (auto o = me->GetCharmerOrOwner())
    if (auto ownerVictim = o->getVictim())
            return ownerVictim;

    // Neither pet or owner had a target and aggressive pets can pick any target
    // To prevent aggressive pets from chain selecting targets and running off, we
    //  only select a random target if certain conditions are met.
    if (me->HasReactState(REACT_AGGRESSIVE) && allowAutoSelect)
    {
        if (auto charmInfo = me->GetCharmInfo())
        if (!charmInfo->IsReturning()
            || charmInfo->IsFollowing()
            || charmInfo->IsAtStay())
            if (auto me_cr = me->ToCreature())
            if (auto nearTarget = me_cr->SelectNearestHostileUnitInAggroRange(true))
                return nearTarget;
    }

    // Default - no valid targets
    return NULL;
}

void PetAI::HandleReturnMovement()
{
    // Handles moving the pet back to stay or owner

    // Prevent activating movement when under control of spells
    // such as "Eyes of the Beast"
    if (me->isCharmed() || me->HasUnitState(UNIT_STATE_CHARGING))
        return;

    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
    {
        if (!me->GetCharmInfo()->IsAtStay() && !me->GetCharmInfo()->IsReturning())
        {
            // Return to previous position where stay was clicked
            float x, y, z;

            me->GetCharmInfo()->GetStayPosition(x, y, z);
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(me->GetGUIDLow(), x, y, z);
        }
    }
    else // COMMAND_FOLLOW
    {
        if (!me->GetCharmInfo()->IsFollowing() && !me->GetCharmInfo()->IsReturning())
        {
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsReturning(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveFollow(me->GetCharmerOrOwner(), PET_FOLLOW_DIST, me->GetFollowAngle());
        }
    }
}

void PetAI::DoAttack(Unit* target, bool chase, uint32 spellId)
{
    // Handles attack with or without chase and also resets flags
    // for next update / creature kill

    if (me->Attack(target, true))
    {
        // Play sound to let the player know the pet is attacking something it picked on its own
        if (me->HasReactState(REACT_AGGRESSIVE) && !me->GetCharmInfo()->IsCommandAttack())
            me->SendPetAIReaction(me->GetGUID());

        if (!me->getThreatManager().getThreat(target, false))
            me->AddThreat(target, 1.0f);

        if (chase)
        {
            /*
            float Unit::GetMeleeRange(const Unit* obj) const
            float Unit::GetMeleeReEnagementDistance(const Unit* obj) const
            */
            float distance_excluding_owner_combat_reach
            {
                (
                    me->GetMeleeReEnagementDistance(target)//me->GetMeleeReEnagementDistance(me)
                - me->GetMeleeRange(target)
                - target->GetBoundaryRadius()
                )
            };
            bool oldCmdAttack = me->GetCharmInfo()->IsCommandAttack(); // This needs to be reset after other flags are cleared
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsCommandAttack(oldCmdAttack); // For passive pets commanded to attack so they will use spells
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveChase(target, distance_excluding_owner_combat_reach, 0.0f, spellId);
        }
        else // (Stay && ((Aggressive || Defensive) && In Melee Range)))
        {
            ClearCharmInfoFlags();
            me->GetCharmInfo()->SetIsAtStay(true);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
        }
    }
}

void PetAI::MovementInform(uint32 moveType, uint32 data)
{
    // Receives notification when pet reaches stay or follow owner
    switch (moveType)
    {
        case POINT_MOTION_TYPE:
        {
            // Pet is returning to where stay was clicked. data should be
            // pet's GUIDLow since we set that as the waypoint ID
            if (data == me->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsAtStay(true);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();
            }
            break;
        }
        case FOLLOW_MOTION_TYPE:
        {
            // If data is owner's GUIDLow then we've reached follow point,
            // otherwise we're probably chasing a creature
            if (me->GetCharmerOrOwner() && me->GetCharmInfo() && data == me->GetCharmerOrOwner()->GetGUIDLow() && me->GetCharmInfo()->IsReturning())
            {
                ClearCharmInfoFlags();
                me->GetCharmInfo()->SetIsFollowing(true);
            }
            break;
        }
        default:
            break;
    }
}

bool PetAI::CanAttack(Unit* target)
{
    // Evaluates wether a pet can attack a specific target based on CommandState, ReactState and other flags
    // IMPORTANT: The order in which things are checked is important, be careful if you add or remove checks

    // Hmmm...
    if (!target || !me->GetCharmInfo())
        return false;

    if (!target->isAlive())
    {
        // Clear target to prevent getting stuck on dead targets
        me->AttackStop();
        me->InterruptNonMeleeSpells(false);
        me->SendMeleeAttackStop();
        return false;
    }

    // Passive - passive pets can attack if told to
    if (me->HasReactState(REACT_PASSIVE))
        return me->GetCharmInfo()->IsCommandAttack();

    // CC - mobs under crowd control can be attacked if owner commanded
    if (target->HasBreakableByDamageCrowdControlAura())
        return me->GetCharmInfo()->IsCommandAttack();

    // Returning - pets ignore attacks only if owner clicked follow
    if (me->GetCharmInfo()->IsReturning())
        return !me->GetCharmInfo()->IsCommandFollow();

    // Stay - can attack if target is within range or commanded to
    if (me->GetCharmInfo()->HasCommandState(COMMAND_STAY) || me->GetCharmInfo()->HasCommandState(COMMAND_MOVE_TO))
        return (me->IsWithinMeleeRange(target) || me->GetCharmInfo()->IsCommandAttack());

    //  Pets attacking something (or chasing) should only switch targets if owner tells them to
    if (me->getVictim() && me->getVictim() != target)
    {
        // Check if our owner selected this target and clicked "attack"
        Unit* ownerTarget = NULL;
        if (auto owner = me->GetCharmerOrOwner()->ToPlayer())
            ownerTarget = owner->GetSelectedUnit();
        else
            ownerTarget = me->GetCharmerOrOwner()->getVictim();

        if (ownerTarget && me->GetCharmInfo()->IsCommandAttack())
            return (target->GetGUID() == ownerTarget->GetGUID());
    }

    // Follow
    if (me->GetCharmInfo()->HasCommandState(COMMAND_FOLLOW))
        return !me->GetCharmInfo()->IsReturning();

    // default, though we shouldn't ever get here
    return false;
}

void PetAI::ReceiveEmote(Player* player, uint32 emote)
{
    if (me->GetOwnerGUID() && me->GetOwnerGUID() == player->GetGUID())
        switch (emote)
        {
            case TEXT_EMOTE_COWER:
                if (me->isPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(/*EMOTE_ONESHOT_ROAR*/EMOTE_ONESHOT_OMNICAST_GHOUL);
                break;
            case TEXT_EMOTE_ANGRY:
                if (me->isPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(/*EMOTE_ONESHOT_COWER*/EMOTE_STATE_STUN);
                break;
            case TEXT_EMOTE_GLARE:
                if (me->isPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(EMOTE_STATE_STUN);
                break;
            case TEXT_EMOTE_SOOTHE:
                if (me->isPet() && me->ToPet()->IsPetGhoul())
                    me->HandleEmoteCommand(EMOTE_ONESHOT_OMNICAST_GHOUL);
                break;
        }
}

void PetAI::ClearCharmInfoFlags()
{
    // Quick access to set all flags to FALSE

    CharmInfo* ci = me->GetCharmInfo();

    if (ci)
    {
        ci->SetIsAtStay(false);
        ci->SetIsCommandAttack(false);
        ci->SetIsCommandFollow(false);
        ci->SetIsFollowing(false);
        ci->SetIsReturning(false);
    }
}

void PetAI::AttackedBy(Unit* attacker)
{
    // Called when pet takes damage. This function helps keep pets from running off
    //  simply due to gaining aggro.

    if (!attacker)
        return;

    // Passive pets don't do anything
    if (me->HasReactState(REACT_PASSIVE))
        return;

    // Prevent pet from disengaging from current target
    if (me->getVictim() && me->getVictim()->isAlive())
        return;

    // Continue to evaluate and attack if necessary
    AttackStart(attacker);
}
