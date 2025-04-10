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

#include "ThreatManager.h"
#include "Unit.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Map.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "UnitEvents.h"
#include "SpellAuras.h"
#include "SpellMgr.h"

//==============================================================
//================= ThreatCalcHelper ===========================
//==============================================================

// The hatingUnit is not used yet
float ThreatCalcHelper::calcThreat(Unit* hatedUnit, Unit* /*hatingUnit*/, float threat, SpellSchoolMask schoolMask, SpellInfo const* threatSpell)
{
    if (!hatedUnit)
        return 0.f;

    if (threatSpell)
    {
        if (auto  threatEntry = sSpellMgr->GetSpellThreatEntry(threatSpell->Id))
            if (threatEntry->pctMod != 1.0f)
                threat *= threatEntry->pctMod;

        // Energize is not affected by Mods
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; i++)
            if (threatSpell->Effects[i].Effect == SPELL_EFFECT_ENERGIZE || threatSpell->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_ENERGIZE)
                return threat;

        if (Player* modOwner = hatedUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(threatSpell->Id, SPELLMOD_THREAT, threat);
    }

    return hatedUnit->ApplyTotalThreatModifier(threat, schoolMask);
}

bool ThreatCalcHelper::isValidProcess(Unit* hatedUnit, Unit* hatingUnit, SpellInfo const* threatSpell)
{
    //function deals with adding threat and adding players and pets into ThreatList
    //mobs, NPCs, guards have ThreatList and HateOfflineList
    //players and pets have only InHateListOf
    //HateOfflineList is used co contain unattackable victims (in-flight, in-water, GM etc.)

    if (!hatedUnit || !hatingUnit)
        return false;

    // not to self
    if (hatedUnit == hatingUnit)
        return false;

    // not to GM
    if (auto p = hatedUnit->ToPlayer())
        if (p->isGameMaster())
            return false;

    // not to dead and not for dead
    if (!hatedUnit->isAlive() || !hatingUnit->isAlive())
        return false;

    // not in same map or phase
    if (!hatedUnit->IsInMap(hatingUnit) || !hatedUnit->InSamePhase(hatingUnit))
        return false;

    // spell not causing threat
    if (threatSpell && threatSpell->AttributesEx & SPELL_ATTR1_NO_THREAT)
        return false;

    ASSERT(hatingUnit->GetTypeId() == TYPEID_UNIT);

    return true;
}

//============================================================
//================= HostileReference ==========================
//============================================================

HostileReference::HostileReference(Unit* refUnit, ThreatManager* threatManager, float threat)
{
    iThreat             = threat;
    iTempThreatModifier = 0.0f;
    link(refUnit, threatManager);
    iUnitGuid           = refUnit->GetGUID();
    iOnline             = true;
    iAccessible         = true;
}

//============================================================
// Tell our refTo (target) object that we have a link
void HostileReference::targetObjectBuildLink()
{
    if (auto t = getTarget())
        t->addHatedBy(this);
}

//============================================================
// Tell our refTo (taget) object, that the link is cut
void HostileReference::targetObjectDestroyLink()
{
    if (auto t = getTarget())
             t->removeHatedBy(this);
}

//============================================================
// Tell our refFrom (source) object, that the link is cut (Target destroyed)

void HostileReference::sourceObjectDestroyLink()
{
    setOnlineOfflineState(false);
}

//============================================================
// Inform the source, that the status of the reference changed

void HostileReference::fireStatusChanged(ThreatRefStatusChangeEvent& threatRefStatusChangeEvent)
{
    if (auto s = getSource())
            s->processThreatEvent(&threatRefStatusChangeEvent);
}

//============================================================

void HostileReference::addThreat(float modThreat)//Lowest level of adding threat
{
    iThreat += modThreat;
    // the threat is changed. Source and target unit have to be available
    // if the link was cut before relink it again
    if (!isOnline())
        updateOnlineStatus();

    if (modThreat != 0.0f)
    {
        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_THREAT_CHANGE, this, modThreat);
        fireStatusChanged(event);
    }
}

void HostileReference::addThreatPercent(int32 percent)
{
    float tmpThreat = iThreat;
    AddPct(tmpThreat, percent);
    addThreat(tmpThreat - iThreat);
}

//============================================================
// check, if source can reach target and set the status

void HostileReference::updateOnlineStatus()
{
    bool online = false;
    bool accessible = false;

    if (!isValid())
        if (Unit* target = ObjectAccessor::GetUnit(*getSourceUnit(), getUnitGuid()))
            link(target, getSource());

    // only check for online status if
    // ref is valid
    // target is no player or not gamemaster
    // target is not in flight
    if (isValid()
        && (getTarget()->GetTypeId() != TYPEID_PLAYER || !getTarget()->ToPlayer()->isGameMaster())
        && !getTarget()->HasUnitState(UNIT_STATE_IN_FLIGHT)
        && getTarget()->IsInMap(getSourceUnit())
        && getTarget()->InSamePhase(getSourceUnit())
        )
    {
        Creature* creature = getSourceUnit()->ToCreature();
        online = getTarget()->isInAccessiblePlaceFor(creature);
        if (!online)
        {
            if (creature->IsWithinCombatRange(getTarget(), creature->m_CombatDistance))
                online = true;                              // not accessible but stays online
        }
        else
            accessible = true;
    }

    setAccessibleState(accessible);
    setOnlineOfflineState(online);
}

//============================================================
// set the status and fire the event on status change

void HostileReference::setOnlineOfflineState(bool isOnline)
{
    if (iOnline != isOnline)
    {
        iOnline = isOnline;
        if (!iOnline)
            setAccessibleState(false);                      // if not online that not accessable as well

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ONLINE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================

void HostileReference::setAccessibleState(bool isAccessible)
{
    if (iAccessible != isAccessible)
    {
        iAccessible = isAccessible;

        ThreatRefStatusChangeEvent event(UEV_THREAT_REF_ASSECCIBLE_STATUS, this);
        fireStatusChanged(event);
    }
}

//============================================================
// prepare the reference for deleting
// this is called be the target

void HostileReference::removeReference()
{
    invalidate();

    ThreatRefStatusChangeEvent event(UEV_THREAT_REF_REMOVE_FROM_LIST, this);
    fireStatusChanged(event);
}

//============================================================

Unit* HostileReference::getSourceUnit()
{
    if (auto s = getSource())
    return (s->getOwner());
    return NULL;
}

//============================================================
//================ ThreatContainer ===========================
//============================================================

void ThreatContainer::clearReferences()
{
    for (auto i : iThreatList)
    {
        i->unlink();
        delete i;
    }

    iThreatList.clear();
}

//============================================================
// Return the HostileReference of NULL, if not found
HostileReference* ThreatContainer::getReferenceByTarget(Unit* victim) const
{
    if (!victim)
        return NULL;

    uint64 const guid = victim->GetGUID();
    for (auto i : iThreatList)
    {
        if (i)
        if (i->getUnitGuid() == guid)
            return i;
    }

    return NULL;
}

//============================================================
// Add the threat, if we find the reference

HostileReference* ThreatContainer::addThreat(Unit* victim, float threat)//Threat container -> Threat Reference to add threat
{
    if (auto ref = getReferenceByTarget(victim))
    {
        ref->addThreat(threat);
        return ref;
    }
    return NULL;
}

//============================================================

void ThreatContainer::modifyThreatPercent(Unit* victim, int32 percent)
{
    if (auto ref = getReferenceByTarget(victim))
        ref->addThreatPercent(percent);
}

//============================================================
// Check if the list is dirty and sort if necessary

void ThreatContainer::update()
{
    if (iDirty && iThreatList.size() > 1)
        iThreatList.sort(Trinity::ThreatOrderPred());

    iDirty = false;
}

//============================================================
// return the next best victim
// could be the current victim

HostileReference* ThreatContainer::selectNextVictim(Creature* attacker, HostileReference* currentVictim) const
{
    HostileReference* currentRef = NULL;
    bool found = false;
    bool noPriorityTargetFound = false;

    auto lastRef = iThreatList.end();
    --lastRef;

    for (auto iter = iThreatList.begin(); iter != iThreatList.end() && !found;)
    {
        currentRef = (*iter);

        Unit* target = currentRef->getTarget();
        ASSERT(target);                                     // if the ref has status online the target must be there !

        // some units are prefered in comparison to others
        if (!noPriorityTargetFound && (target->IsImmunedToDamage(attacker->GetMeleeDamageSchoolMask()) 
            || target->HasNegativeAuraWithInterruptFlag(AURA_INTERRUPT_FLAG_TAKE_DAMAGE)))
        {
            if (iter != lastRef)
            {
                // current victim is a second choice target, so don't compare threat with it below
                if (currentRef == currentVictim)
                    currentVictim = NULL;
                ++iter;
                continue;
            }
            else
            {
                // if we reached to this point, everyone in the threatlist is a second choice target. In such a situation the target with the highest threat should be attacked.
                noPriorityTargetFound = true;
                iter = iThreatList.begin();
                continue;
            }
        }

        if (attacker->canCreatureAttack(target))           // skip non attackable currently targets
        {
            if (currentVictim)                              // select 1.3/1.1 better target in comparison current target
            {
                // list sorted and and we check current target, then this is best case
                if (currentVictim == currentRef 
                    || currentRef->getThreat() <= 1.1f * currentVictim->getThreat())
                {
                    if (currentVictim != currentRef && attacker->canCreatureAttack(currentVictim->getTarget()))
                        currentRef = currentVictim;            // for second case, if currentvictim is attackable

                    found = true;
                    break;
                }

                if (currentRef->getThreat() > (1.3f * currentVictim->getThreat()+1.f)
                    ||
                    (currentRef->getThreat() > (1.1f * currentVictim->getThreat() + 1.f) && attacker->IsWithinMeleeRange(target)))
                {                                           //implement 110% threat rule for targets in melee range
                    found = true;                           //and 130% rule for targets in ranged distances
                    break;                                  //for selecting alive targets
                }
            }
            else                                            // select any
            {
                found = true;
                break;
            }
        }
        ++iter;
    }
    if (!found)
        currentRef = NULL;

    return currentRef;
}

//============================================================
//=================== ThreatManager ==========================
//============================================================

ThreatManager::ThreatManager(Unit* owner) : iCurrentVictim(NULL), iOwner(owner), iUpdateTimer(THREAT_UPDATE_INTERVAL)
{
}

//============================================================

void ThreatManager::clearReferences()
{
    iThreatContainer.clearReferences();
    iThreatOfflineContainer.clearReferences();
    iCurrentVictim          = NULL;
    iUpdateTimer            = THREAT_UPDATE_INTERVAL;
}

//============================================================

void ThreatManager::addThreat(Unit* victim, float threat, SpellSchoolMask schoolMask, SpellInfo const* threatSpell)
{
    if (!ThreatCalcHelper::isValidProcess(victim, getOwner(), threatSpell))
        return;

    doAddThreat(victim, ThreatCalcHelper::calcThreat(victim, iOwner, threat, schoolMask, threatSpell));
}

void ThreatManager::doAddThreat(Unit* victim, float threat)//Second lowest level of adding threat, only splits threat before applying.
{
    // must check > 0.0f, otherwise dead loop
    if (threat > 0.0f)
    if (auto pct = victim->GetRedirectThreatPercent())
        if (auto redirectTarget = victim->GetRedirectThreatTarget())
        {
            if (!iOwner->IsInCombatWith(redirectTarget))
                iOwner->SetInCombatWith(redirectTarget);

            float redirectThreat = CalculatePct(threat, pct);
            threat -= redirectThreat;
            _addThreat(redirectTarget, redirectThreat);
        }

    _addThreat(victim, threat);
}

void ThreatManager::_addThreat(Unit* victim, float threat)//Threat Manager -> Threat Container to add threat
{
    if (!victim)
    {
        TC_LOG_ERROR("sql.sql", "ThreatManager::_addThreat attempting to add threat to null victim.");
        return;
    }
    ASSERT(victim);

    auto ref = iThreatContainer.addThreat(victim, threat);
    // Ref is not in the online refs, search the offline refs next
    if (!ref)
        ref = iThreatOfflineContainer.addThreat(victim, threat);

    if (!ref) // there was no ref => create a new one
    {
                                                            // threat has to be 0 here
        auto hostileRef         = new HostileReference(victim, this, 0);
        iThreatContainer.addReference(hostileRef);
        if (auto ref_new = iThreatContainer.addThreat(victim, threat))
        {

        }
        else
            TC_LOG_ERROR("sql.sql", "ThreatManager::_addThreat still missing threat reference after manually adding: %u %f", victim->GetGUID(), threat);
        

        if (auto p = victim->ToPlayer())
            if (p->isGameMaster())
                hostileRef->setOnlineOfflineState(false); // GM is always offline
    }
}

//============================================================

void ThreatManager::modifyThreatPercent(Unit* victim, int32 percent)
{
    iThreatContainer.modifyThreatPercent(victim, percent);
}

//============================================================

Unit* ThreatManager::getHostilTarget()
{
    iThreatContainer.update();
    auto v = getCurrentVictim();

        if (auto o = getOwner())
            if (auto oc = o->ToCreature())
                if (auto nextVictim = iThreatContainer.selectNextVictim(oc, v))
                    setCurrentVictim(nextVictim);

        if (v)
            return v->getTarget();

            return NULL;
}

//============================================================

float ThreatManager::getThreat(Unit* victim, bool alsoSearchOfflineList)
{
    if (victim)
    {
        if (auto ref = iThreatContainer.getReferenceByTarget(victim))
            return ref->getThreat();

        if (alsoSearchOfflineList)
            if (auto ref = iThreatOfflineContainer.getReferenceByTarget(victim))
                return ref->getThreat();
    }
 
    return 0.f;
}

//============================================================

void ThreatManager::tauntApply(Unit* taunter)
{
    if (auto ref = iThreatContainer.getReferenceByTarget(taunter))
    if (auto v = getCurrentVictim())
    if (ref->getThreat() < v->getThreat())
    {
        if (ref->getTempThreatModifier() == 0.0f) // Ok, temp threat is unused
            ref->setTempThreat(v->getThreat());
    }
}

//============================================================

void ThreatManager::tauntFadeOut(Unit* taunter)
{
    if (auto ref = iThreatContainer.getReferenceByTarget(taunter))
        ref->resetTempThreat();
}

//============================================================

void ThreatManager::setCurrentVictim(HostileReference* pHostileReference)
{
    if (pHostileReference)
    if (pHostileReference != iCurrentVictim)
        iOwner->SendChangeCurrentVictimOpcode(pHostileReference);

    iCurrentVictim = pHostileReference;
}

//============================================================
// The hated unit is gone, dead or deleted
// return true, if the event is consumed

void ThreatManager::processThreatEvent(ThreatRefStatusChangeEvent* threatRefStatusChangeEvent)
{
    threatRefStatusChangeEvent->setThreatManager(this);     // now we can set the threat manager

    if (auto hostilRef = threatRefStatusChangeEvent->getReference())
    {

        switch (threatRefStatusChangeEvent->getType())
        {
        case UEV_THREAT_REF_THREAT_CHANGE:
            if ((getCurrentVictim() == hostilRef && threatRefStatusChangeEvent->getFValue() <= 0.0f) ||
                (getCurrentVictim() != hostilRef && threatRefStatusChangeEvent->getFValue() > 0.0f))
                setDirty(true);                             // the order in the threat list might have changed
            break;
        case UEV_THREAT_REF_ONLINE_STATUS:
            if (!hostilRef->isOnline())
            {
                if (hostilRef == getCurrentVictim())
                {
                    setCurrentVictim(NULL);
                    setDirty(true);
                }
                iOwner->SendRemoveFromThreatListOpcode(hostilRef);
                iThreatContainer.remove(hostilRef);
                iThreatOfflineContainer.addReference(hostilRef);
            }
            else
            {
                if (getCurrentVictim() && hostilRef->getThreat() > (1.1f * getCurrentVictim()->getThreat()))
                    setDirty(true);
                iThreatContainer.addReference(hostilRef);
                iThreatOfflineContainer.remove(hostilRef);
            }
            break;
        case UEV_THREAT_REF_REMOVE_FROM_LIST:
            if (hostilRef == getCurrentVictim())
            {
                setCurrentVictim(NULL);
                setDirty(true);
            }
            iOwner->SendRemoveFromThreatListOpcode(hostilRef);
            if (hostilRef->isOnline())
                iThreatContainer.remove(hostilRef);
            else
                iThreatOfflineContainer.remove(hostilRef);
            break;
        }
    }



    auto old_v = iCurrentVictim;
    
        iThreatContainer.update();
        if (auto v = getCurrentVictim())
            if (!old_v || old_v->getUnitGuid() != v->getUnitGuid())
                getHostilTarget();
    
}

bool ThreatManager::isNeedUpdateToClient(uint32 time)
{
    if (isThreatListEmpty() && (iThreatContainer.isDirty() == false))
        return false;

    if (time >= iUpdateTimer)
    {
        iUpdateTimer = THREAT_UPDATE_INTERVAL;
        return true;
    }
    iUpdateTimer -= time;
    return false;
}

// Reset all aggro without modifying the threatlist.
void ThreatManager::resetAllAggro()
{

    ThreatContainer::StorageType& threatList = iThreatContainer.iThreatList;
    if (threatList.empty())
        return;

    for (auto itr : threatList)
        itr->setThreat(0);

    setDirty(true);
}
