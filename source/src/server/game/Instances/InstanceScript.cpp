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

#include "Creature.h"
#include "CreatureAI.h"
#include "PassiveAI.h"
#include "DatabaseEnv.h"
#include "GameObject.h"
#include "Group.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "Pet.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Guild.h"
#include "GridNotifiers.h"
#include "ScriptedCreature.h"
#include "Chat.h"

#define CAST_AI(a, b)   (dynamic_cast<a*>(b))

void InstanceScript::SaveToDB()
{
    std::string data = GetSaveData();
    if (data.empty())
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_DATA);
    stmt->setUInt32(0, GetCompletedEncounterMask());
    stmt->setString(1, data);
    stmt->setUInt8(2, GetHardModeDifficulty());
    stmt->setUInt32(3, instance->GetInstanceId());
    CharacterDatabase.Execute(stmt);
}

bool InstanceScript::IsEncounterInProgress() const
{
    for (std::vector<BossInfo>::const_iterator itr = bosses.begin(); itr != bosses.end(); ++itr)
        if (itr->state == IN_PROGRESS)
            return true;

    return false;
}

bool InstanceScript::ItemDrop(Object* /*source*/, LootItem& /*loot*/)
{
    return true;
}

void InstanceScript::OnCreatureCreate(Creature* creature)
{

    AddObject(creature, true);
    AddMinion(creature, true);

    if (IsChallengeModeStarted())
    {
        creature->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, (creature->GetMaxHealth() * sChallengeModeMgr->GetHealthMultiplier(GetChallengeModeLevel())) - creature->GetMaxHealth(), true);

        sChallengeModeMgr->ApplyAffixes(creature);

        if (creature->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER))
            creature->SetFullHealth();
    }

    _creatures.insert(creature->GetGUID());
}

void InstanceScript::OnCreatureRemove(Creature* creature)
{
    AddObject(creature, false);
    AddMinion(creature, false);

    _creatures.erase(creature->GetGUID());
}

void InstanceScript::OnGameObjectCreate(GameObject* go)
{
    AddObject(go, true);
    AddDoor(go, true);
}

void InstanceScript::OnGameObjectRemove(GameObject* go)
{
    AddObject(go, false);
    AddDoor(go, false);
}

uint64 InstanceScript::GetObjectGuid(uint32 type) const
{
    ObjectGuidMap::const_iterator i = _objectGuids.find(type);
    if (i != _objectGuids.end())
        return i->second;
    return 0;
}

uint64 InstanceScript::GetData64(uint32 type) const
{
    return GetObjectGuid(type);
}

void InstanceScript::OnPlayerEnter(Player* player)
{
    if (auto g = player->GetGroup())
        if (g->isLFGGroup())
            if (auto aggra = player->FindNearestCreature(90000, 100.f))
                aggra->SetVisible(false);

    if (IsChallengeModeStarted())
    {
        if (GetChallengeModeRemainingTime() > 0)
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_ID))
            {
                player->RemoveTimedQuest(CHALLENGE_MODE_QUEST_ID);
                player->RemoveActiveQuest(CHALLENGE_MODE_QUEST_ID);
                player->SetQuestSlot(player->FindQuestSlot(CHALLENGE_MODE_QUEST_ID), 0);
                player->AddQuest(quest, nullptr);
            }

            if (Quest const* quest2 = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_KILL_COUNT))
            {
                player->RemoveActiveQuest(CHALLENGE_MODE_QUEST_KILL_COUNT);
                player->SetQuestSlot(player->FindQuestSlot(CHALLENGE_MODE_QUEST_KILL_COUNT), 0);
                player->AddQuest(quest2, nullptr);

                int16 count = ((int)(_challengeModeProgress * 100)) - player->GetReqKillOrCastCurrentCount(90001, 2442);
                if (count > 0)
                    player->KilledMonsterCredit(2442, 0, count);
            }
        }

        sChallengeModeMgr->ApplyAffixes(player);
    }
}

void InstanceScript::OnPlayerLeave(Player* player)
{
    if (Quest const* quest = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_ID))
        if (player->GetQuestStatus(CHALLENGE_MODE_QUEST_ID) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(CHALLENGE_MODE_QUEST_ID) == QUEST_STATUS_FAILED)
        {
            player->RemoveTimedQuest(CHALLENGE_MODE_QUEST_ID);
            player->RemoveActiveQuest(CHALLENGE_MODE_QUEST_ID);
            player->SetQuestSlot(player->FindQuestSlot(CHALLENGE_MODE_QUEST_ID), 0);
        }

    if (Quest const* quest2 = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_KILL_COUNT))
        if (player->GetQuestStatus(CHALLENGE_MODE_QUEST_KILL_COUNT) == QUEST_STATUS_INCOMPLETE ||
            player->GetQuestStatus(CHALLENGE_MODE_QUEST_KILL_COUNT) == QUEST_STATUS_FAILED)
        {
            player->RemoveActiveQuest(CHALLENGE_MODE_QUEST_KILL_COUNT);
            player->SetQuestSlot(player->FindQuestSlot(CHALLENGE_MODE_QUEST_KILL_COUNT), 0);
        }

    Map::PlayerList const& lPlayers = instance->GetPlayers();
    if (lPlayers.getSize() <= 1)
    {
        for (auto i = 0; i != bosses.size(); ++i)
            if (GetBossState(i) == IN_PROGRESS)
            {
                SetBossState(i, NOT_STARTED);
                if (auto boss_guid = GetData64(i))
                    if (auto c = instance->GetCreature(boss_guid))
                    {
                        if (c->AI())
                            c->AI()->Reset();

                        c->Respawn(true);
                    }
            }

        DoOnCreatures([](Creature* c)
            {
                if (c->isInCombat())
                {
                    if (c->GetDBTableGUIDLow())
                    {
                        c->Respawn(true);
                    }
                    else
                        c->DespawnOrUnsummon();
                }
            });
    }
}

void InstanceScript::OnPlayerDeath(Player* /*player*/)
{
    if (IsChallengeModeStarted())
    {
        ++_challengeModeDeathCount;

        if (GetChallengeModeRemainingTime() > 0)
        {
            if (Quest const* qInfo = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_ID))
            {
                DoOnPlayers([qInfo, this](Player* player)
                {
                    player->RemoveTimedQuest(CHALLENGE_MODE_QUEST_ID);
                    player->RemoveActiveQuest(CHALLENGE_MODE_QUEST_ID);
                    player->SetQuestSlot(player->FindQuestSlot(CHALLENGE_MODE_QUEST_ID), 0);
                    player->AddQuest(qInfo, nullptr);
                });
            }
        }
    }
}

void InstanceScript::OnUnitDeath(Unit* unit)
{
    // When any non-boss enemy dies, its death cry empowers nearby allies, increasing their maximum health and damage by 20%.
    if (HasAffix(AFFIX_BOLSTERING))
    {
        if (auto dyingCreature = unit->ToCreature())
        {
            if (dyingCreature->CanBeSubjectOfAffix(AFFIX_BOLSTERING) && !dyingCreature->IsDungeonBoss() && (dyingCreature->GetEntry() != 45932))
            {
                std::list<Creature*> templist;
                Trinity::AllFriendlyCreaturesInGrid check(dyingCreature);
                Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(dyingCreature, templist, check);
                dyingCreature->VisitNearbyGridObject(30, searcher);

                for (Creature* creature : templist)
                {
                    if (dyingCreature->GetDistance(creature) > 30.0f)
                        continue;

                    if (!creature->isInCombat() || !creature->CanBeSubjectOfAffix(AFFIX_BOLSTERING) || creature->IsDungeonBoss())
                        continue;

                    CustomSpellValues damageValues;
                    damageValues.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                    creature->CastCustomSpell(AFFIX_SPELL_ENRAGE_20, damageValues, creature, TRIGGERED_FULL_MASK);

                    float currenthprate = float(creature->GetHealth()) / float(creature->GetMaxHealth());
                    creature->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, (creature->GetMaxHealth() * 0.2), true);
                    creature->SetHealth(creature->GetMaxHealth() * currenthprate);

                    /*if (Aura* healthAura = creature->GetAura(AFFIX_SPELL_RALLYING_CRY_HEALTH))
                    {
                        healthAura->ModStackAmount(1);
                        healthAura->SetMaxDuration(-1);
                        healthAura->SetDuration(-1);
                    }
                    else
                    {
                        CustomSpellValues healthValues;
                        uint32 value = creature->CountPctFromMaxHealth(20);
                        healthValues.AddSpellMod(SPELLVALUE_BASE_POINT0, value);
                        healthValues.AddSpellMod(SPELLVALUE_DURATION, 7 * DAY * IN_MILLISECONDS);
                        creature->CastCustomSpell(AFFIX_SPELL_RALLYING_CRY_HEALTH, healthValues, creature, TRIGGERED_FULL_MASK);
                    }*/
                }
            }
        }
    }

    //Whenever any mob dies, it will summon a Spiteful Shade creature nearby that will fixate a random player.
    if (HasAffix(AFFIX_SPITEFUL) && isAffixEnabled(AFFIX_SPITEFUL))
    {
        if (auto creature = unit->ToCreature())
            if (!creature->IsDungeonBoss())
                if (creature->CanBeSubjectOfAffix(AFFIX_SPITEFUL))
                    switch (creature->GetEntry())
                    {
                    case AFFIX_NPC_SPITEFUL:
                    case AFFIX_NPC_VOLCANIC:
                    case AFFIX_NPC_EXPLOSIVE:
                    case AFFIX_NPC_SANGUINE:
                    case AFFIX_NPC_PRIDEFUL:
                    case AFFIX_NPC_ENTANGLING:
                    case AFFIX_NPC_AFFLICTED:
                    case AFFIX_NPC_INCORPOREAL:
                    case AFFIX_NPC_SHIELDING:
                        break;
                    default:
                        if (auto c = instance->SummonCreature(AFFIX_NPC_SPITEFUL, *unit))
                            c->ToCreature()->DespawnOrUnsummon(15000);

                        break;
                    }
    }

    // When slain, non-boss enemies leave behind a lingering pool of ichor that heals their allies and damages players.
    if (HasAffix(AFFIX_SANGUINE) && isAffixEnabled(AFFIX_SANGUINE))
        if (auto dyingCreature = unit->ToCreature())
            if (dyingCreature->CanBeSubjectOfAffix(AFFIX_SANGUINE) && !dyingCreature->IsDungeonBoss())
            {
                Position summonPos;
                dyingCreature->GetPosition(&summonPos);

                AddTimedDelayedOperation(2000, [this, summonPos]()
                {
                    instance->SummonCreature(AFFIX_NPC_SANGUINE, summonPos);
                });
            }

    // When slain, non-boss enemies explode, causing all players to suffer 14% of their max health in damage over 4 sec. This effect stacks.
    if (HasAffix(AFFIX_BURSTING) && isAffixEnabled(AFFIX_BURSTING))
        if (auto dyingCreature = unit->ToCreature())
            if (dyingCreature->CanBeSubjectOfAffix(AFFIX_BURSTING) && !dyingCreature->IsDungeonBoss())
            {
                CustomSpellValues damageValues;
                damageValues.AddSpellMod(SPELLVALUE_BASE_POINT0, 0);
                dyingCreature->CastCustomSpell(AFFIX_SPELL_BURSTING_VISUAL, damageValues, dyingCreature, TRIGGERED_FULL_MASK);

                auto players = dyingCreature->GetPlayersInRange(10.f, true);

                for (Player* player : players)
                    if (Aura* burstingAura = player->GetAura(AFFIX_SPELL_BURSTING_DAMAGE))
                    {
                        burstingAura->ModStackAmount(1);
                    }
                    else
                    {
                        float damageMultiplier = sChallengeModeMgr->GetDamageMultiplier(GetChallengeModeLevel());

                        CustomSpellValues healthValues;
                        uint32 value = player->CountPctFromMaxHealth(14) / damageMultiplier / 4;
                        healthValues.AddSpellMod(SPELLVALUE_BASE_POINT0, value);
                        healthValues.AddSpellMod(SPELLVALUE_DURATION, 4 * IN_MILLISECONDS);
                        dyingCreature->CastCustomSpell(AFFIX_SPELL_BURSTING_DAMAGE, healthValues, player, TRIGGERED_FULL_MASK);
                    }
            }

    for (auto it = _challengeModeMobsToKill.begin(); it != _challengeModeMobsToKill.end(); ++it)
        if (unit->GetEntry() == it->first && (_challengeModeMobsStatus[it->first] > 0))
        {
            //TC_LOG_ERROR("sql.sql", "killed %u", unit->GetEntry());
            _challengeModeProgress += (1.f / (float)it->second) / (float)_challengeModeMobsToKill.size();
            _challengeModeMobsStatus[it->first] -= 1;

            if (_challengeModeMobsStatus[it->first] == 0)
            {
                    DoOnPlayers([unit, this](Player* player)
                        {
                            WorldPacket data(SMSG_QUESTUPDATE_ADD_KILL, (4 * 4 + 8));
                            data << uint32(CHALLENGE_MODE_QUEST_ID);
                            data << uint32(unit->GetEntry());
                            data << uint32(100);
                            data << uint32(100);
                            data << uint64(unit->GetGUID());
                                if (auto s = player->GetSession())
                                    s->SendPacket(&data);
                        });
            }
        }

    DoOnPlayers([this, unit](Player* player)
    {
        int16 count = ((int)(_challengeModeProgress * 100)) - player->GetReqKillOrCastCurrentCount(90001, 2442);
        if (count > 0)
        {

            //TC_LOG_ERROR("sql.sql", "updating progress for %u", player->GetGUID());
            player->KilledMonsterCredit(2442, 0, count);
            SetChallengeKillPct(GetChallengeKillPct() + count);

            if (!player->isGameMaster() && player->isGMVisible())
            if (auto g = player->GetGroup())
            if (g->IsLeader(player->GetGUID()))
                if (HasAffix(AFFIX_PRIDEFUL) && isAffixEnabled(AFFIX_PRIDEFUL))
                {
                    for (int i = 0; i < 5; ++i)
                        if (auto threshold = ((i * 20) + 10))
                            if (GetChallengeKillPct() >= threshold && !GetPridefulSpawn(i))
                            {
                                Position spawn(*player);
                                player->MoveBlink(spawn, 5.f, 0.f);

                                if (auto prideful = instance->SummonCreature(AFFIX_NPC_PRIDEFUL, spawn))
                                {
                                    //TC_LOG_ERROR("sql.sql", "Spawning prideful %u on player %u", prideful->GetGUID(), player->GetGUID());
                                    prideful->AI()->DoAction(501);
                                    SetPridefulSpawn(i, true);
                                }
                            }
                            //else TC_LOG_ERROR("sql.sql", "Threshold: %u, count: %u, spawn number: %u, spawn status: %u", threshold, GetChallengeKillPct(), i, GetPridefulSpawn(i));

                }
        }
    });

    if (G3D::fuzzyGe(_challengeModeProgress, 1.f))
        CompleteChallengeMode();
}

void InstanceScript::OnDamageTaken(Unit* invoker, Unit* unit, uint32 damage, DamageEffectType damagetype)
{
    // Non - boss enemies enrage at 30 % health remaining, dealing 100 % increased damage until defeated.
    if (HasAffix(AFFIX_RAGING))
        if (auto creature = unit->ToCreature())
            if (creature->CanBeSubjectOfAffix(AFFIX_RAGING))
                if (!creature->IsDungeonBoss())
                    if (creature->HealthBelowPctDamaged(30, damage))
                        if (!creature->HasAura(AFFIX_SPELL_ENRAGE_100))
                        {
                            CustomSpellValues values;
                            values.AddSpellMod(SPELLVALUE_BASE_POINT1, 0);
                            values.AddSpellMod(SPELLVALUE_DURATION, 7 * DAY * IN_MILLISECONDS);
                            creature->CastCustomSpell(AFFIX_SPELL_ENRAGE_100, values, creature, TRIGGERED_FULL_MASK);
                        }

    // When injured below 90 % health, players will suffer increasing damage over time until healed above 90 % health.
        if (HasAffix(AFFIX_GRIEVOUS))
            if (isAffixEnabled(AFFIX_GRIEVOUS))
            if (auto player = unit->ToPlayer())
                if (player->HealthBelowPctDamaged(90, 0))
                if (damagetype == DIRECT_DAMAGE || damagetype == SPELL_DIRECT_DAMAGE || damagetype == DOT || damagetype == SELF_DAMAGE)
                if (invoker)
                if (!player->HasAura(AFFIX_SPELL_GRIEVOUS))
                    player->AddAura(AFFIX_SPELL_GRIEVOUS, player);

    if (HasAffix(AFFIX_NECROTIC))
        if (isAffixEnabled(AFFIX_NECROTIC))
        if (damagetype == DIRECT_DAMAGE)
            if (auto player = unit->ToPlayer())
                if (auto creature = invoker->ToCreature())
                    if (creature->CanBeSubjectOfAffix(AFFIX_NECROTIC))
                        if (!creature->IsDungeonBoss())
                            creature->CastSpell(player, 60626, true);
}

void InstanceScript::OnHealReceived(Unit* invoker, Unit* unit, uint32 heal)
{
    if (auto player = unit->ToPlayer())
    {
        /*
            Handle Overflowing
        */
        if(HasAffix(AFFIX_OVERFLOWING) && isAffixEnabled(AFFIX_OVERFLOWING))
            if ((heal + player->GetHealth()) > player->GetMaxHealth())
            {
                uint32 absorb = heal - (player->GetMaxHealth() - player->GetHealth());
                if (auto caster = unit)
                if (auto overflowing_debuff = caster->GetAuraEffect(107801, EFFECT_1, caster->GetGUID()))
                {
                    absorb += (overflowing_debuff->GetAmount());
                    overflowing_debuff->SetAmount(int32(absorb));
                    if (auto aura = overflowing_debuff->GetBase())
                    {
                        aura->SetMaxDuration(60000);
                        aura->SetDuration(60000);
                    }
                }
                else
                {
                    if (auto caster = unit)
                        if (auto aura = caster->AddAura(107801, caster))
                            if (auto overflowing_debuff = caster->GetAuraEffect(107801, EFFECT_1, caster->GetGUID()))
                            {
                                overflowing_debuff->SetAmount(int32(absorb));
                                aura->SetMaxDuration(60000);
                                aura->SetDuration(60000);
                            }
                }
            }

        /*
            Remove Grievous
        */
        if (HasAffix(AFFIX_GRIEVOUS))
        if ((heal + player->GetHealth()) >= (player->GetMaxHealth() * 0.9f))
            if (auto aura = player->GetAura(AFFIX_SPELL_GRIEVOUS, player->GetGUID()))
                aura->Remove(AURA_REMOVE_BY_DEFAULT);
    }
}

Unit* SelectTarget(Creature* me, SelectAggroTarget targetType, uint32 position = 0, float dist = 0.f, bool playerOnly = false, int32 aura = 0)
{
    ThreatContainer::StorageType const& threatlist = me->getThreatManager().getThreatList();
    if (position >= threatlist.size())
        return nullptr;

    auto predicate = DefaultTargetSelector(me, dist, playerOnly, aura);

    std::list<Unit*> targetList;
    for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
        if (predicate((*itr)->getTarget()))
            targetList.push_back((*itr)->getTarget());

    if (position >= targetList.size())
        return nullptr;

    if (targetType == SELECT_TARGET_NEAREST || targetType == SELECT_TARGET_FARTHEST)
        targetList.sort(Trinity::ObjectDistanceOrderPred(me));

    switch (targetType)
    {
        case SELECT_TARGET_NEAREST:
        case SELECT_TARGET_TOPAGGRO:
        {
            std::list<Unit*>::iterator itr = targetList.begin();
            std::advance(itr, position);
            return *itr;
        }
        case SELECT_TARGET_FARTHEST:
        case SELECT_TARGET_BOTTOMAGGRO:
        {
            std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
            std::advance(ritr, position);
            return *ritr;
        }
        case SELECT_TARGET_RANDOM:
        {
            std::list<Unit*>::iterator itr = targetList.begin();
            std::advance(itr, urand(position, targetList.size() - 1));
            return *itr;
        }
        default:
            break;
    }

    return nullptr;
}

void InstanceScript::OnCreatureEnterCombat(Creature* creature)
{
    if (creature)
    if (creature->GetCreatureTemplate())
    if (creature->GetCreatureTemplate()->type == CREATURE_TYPE_CRITTER)
    {
        creature->setRegeneratingHealth(false);
        creature->DeleteThreatList();
        creature->CombatStopWithPets();
        return;
    }

    if (!IsChallengeModeInProgress())
    if (!GetMythicTamperState())
    {
        if (auto unit = creature->getVictim())
        {
            if (unit->ToPlayer())
                SetMythicTamperState(true);
            if (auto owner = unit->GetOwner())
            if (owner->ToPlayer())
                SetMythicTamperState(true);
        }
    }

    // While in combat, enemies periodically summon Explosive Orbs that will detonate if not destroyed.
    if (HasAffix(AFFIX_EXPLOSIVE) && creature->CanBeSubjectOfAffix(AFFIX_EXPLOSIVE) && isAffixEnabled(AFFIX_EXPLOSIVE))
    {
        creature->GetScheduler().Schedule(10s, 30s, AFFIX_NPC_EXPLOSIVE, [creature](TaskContext context)
        {
            if (!creature->isAlive() || !creature->isInCombat())
                return;

            if (urand(0, 2))
            {
                context.Repeat(10s, 30s);
                return;
            }

            if (auto playerTarget = SelectTarget(creature, SELECT_TARGET_RANDOM, 0, 0.f, false))
            {
                // Random pos can go out of LOS, we try 5 times
                for (uint8 i = 0; i < 2; ++i)
                {
                    Position randomPosition;
                    GetRandPosFromCenterInDist(playerTarget, frand(5.f, 7.f), randomPosition);
                    randomPosition.m_positionZ += 10.f;
                    playerTarget->UpdateGroundPositionZ(randomPosition.GetPositionX(), randomPosition.GetPositionY(), randomPosition.m_positionZ);
                    randomPosition.m_positionZ += 1.f;

                    if (playerTarget->IsWithinLOS(randomPosition.GetPositionX(), randomPosition.GetPositionY(), randomPosition.GetPositionZ()))
                    if (!(playerTarget->ToPlayer() && playerTarget->ToPlayer()->isGameMaster()))
                    {
                        creature->SummonCreature(AFFIX_NPC_EXPLOSIVE, randomPosition, TEMPSUMMON_CORPSE_DESPAWN);
                        break;
                    }
                }
            }

            context.Repeat(10s, 30s);
        });
    }

    // While in combat, enemies periodically cause gouts of flame to erupt beneath the feet of distant players.
    if (HasAffix(AFFIX_VOLCANIC) && creature->CanBeSubjectOfAffix(AFFIX_VOLCANIC) && isAffixEnabled(AFFIX_VOLCANIC))
    {
        creature->GetScheduler().Schedule(10s, 30s, AFFIX_NPC_VOLCANIC, [creature](TaskContext context)
        {
            if (!creature->isInCombat())
                return;

            if (urand(0, 2))
            {
                context.Repeat(10s, 30s);
                return;
            }

            if (auto target = SelectTarget(creature, SELECT_TARGET_RANDOM, 0, 10.f, true))
            {
                creature->CastSpell(target, AFFIX_SPELL_VOLCANIC_VISUAL, true);
                creature->SummonCreature(AFFIX_NPC_VOLCANIC, *target);
            }

            context.Repeat(10s, 30s);
        });
    }

    if (HasAffix(AFFIX_STORMING) && creature->CanBeSubjectOfAffix(AFFIX_STORMING) && isAffixEnabled(AFFIX_STORMING))
    {
        creature->GetScheduler().Schedule(10s, 30s, AFFIX_NPC_STORMING, [creature](TaskContext context)
            {
                if (!creature->isInCombat())
                    return;

                //TC_LOG_ERROR("sql.sql", "rolling to summon storming npc");
                if (urand(0, 2))
                {
                    context.Repeat(10s, 30s);
                    return;
                }
                Position pos(*creature);
                auto p = creature->SelectNearestPlayer(50.f);
                creature->MovePosition(pos, 2.f, p ? creature->GetAngle(p) : frand(0.f, M_PI * 2.f));
                creature->SummonCreature(AFFIX_NPC_STORMING, pos);
                context.Repeat(10s, 30s);
            });
    }
}

void InstanceScript::OnCreatureReset(Creature* creature)
{
    if (IsChallengeModeStarted())
    {
        sChallengeModeMgr->ApplyAffixes(creature, false);

        uint8 hpaddcount = creature->GetAuraCount(AFFIX_SPELL_ENRAGE_20);
        creature->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, ((creature->GetMaxHealth() * 0.2) * hpaddcount), false);
        creature->RemoveAurasDueToSpell(AFFIX_SPELL_ENRAGE_20);

        if (creature->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER))
            creature->SetFullHealth();

        creature->GetScheduler().CancelGroup(AFFIX_NPC_EXPLOSIVE);
        creature->GetScheduler().CancelGroup(AFFIX_NPC_VOLCANIC);
    }
}

void InstanceScript::OnAddThreat(Unit* /*owner*/, Unit* victim, float& threat)
{
    // Enemies pay far less attention to threat generated by tanks.
    if (HasAffix(AFFIX_SKITTISH))
        if (auto playerVictim = victim->ToPlayer())
            if (playerVictim->HasTankSpec())
                threat *= 0.2f;
}

void InstanceScript::SetHeaders(std::string const& dataHeaders)
{
    for (char header : dataHeaders)
        if (isalpha(header))
            headers.push_back(header);
}

void InstanceScript::LoadMinionData(const MinionData* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            minions.insert(std::make_pair(data->entry, MinionInfo(&bosses[data->bossId])));

        ++data;
    }
    TC_LOG_DEBUG("scripts", "InstanceScript::LoadMinionData: " UI64FMTD " minions loaded.", uint64(minions.size()));
}

void InstanceScript::LoadDoorData(const DoorData* data)
{
    while (data->entry)
    {
        if (data->bossId < bosses.size())
            doors.insert(std::make_pair(data->entry, DoorInfo(&bosses[data->bossId], data->type, BoundaryType(data->boundary))));

        ++data;
    }
    TC_LOG_DEBUG("scripts", "InstanceScript::LoadDoorData: " UI64FMTD " doors loaded.", uint64(doors.size()));
}

void InstanceScript::LoadObjectData(ObjectData const* creatureData, ObjectData const* gameObjectData)
{
    if (creatureData)
        LoadObjectData(creatureData, _creatureInfo);

    if (gameObjectData)
        LoadObjectData(gameObjectData, _gameObjectInfo);

    TC_LOG_DEBUG("scripts", "InstanceScript::LoadObjectData: " SZFMTD " objects loaded.", _creatureInfo.size() + _gameObjectInfo.size());
}

void InstanceScript::LoadObjectData(ObjectData const* data, ObjectInfoMap& objectInfo)
{
    while (data->entry)
    {
        ASSERT(objectInfo.find(data->entry) == objectInfo.end());
        objectInfo[data->entry] = data->type;
        ++data;
    }
}

void InstanceScript::UpdateMinionState(Creature* minion, EncounterState state)
{
    switch (state)
    {
        case NOT_STARTED:
            if (!minion->isAlive())
                minion->Respawn();
            else if (minion->isInCombat())
                minion->AI()->EnterEvadeMode();
            break;
        case IN_PROGRESS:
            if (!minion->isAlive())
                minion->Respawn();
            else if (!minion->getVictim())
                minion->AI()->DoZoneInCombat();
            break;
        default:
            break;
    }
}

void InstanceScript::UpdateDoorState(GameObject* door)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    bool open = true;
    for (; range.first != range.second && open; ++range.first)
    {
        DoorInfo const& info = range.first->second;
        switch (info.type)
        {
            case DOOR_TYPE_ROOM:
                open = (info.bossInfo->state != IN_PROGRESS);
                break;
            case DOOR_TYPE_PASSAGE:
                open = ((info.bossInfo->state == DONE) || (info.bossInfo->state == DONE_HM));
                break;
            case DOOR_TYPE_SPAWN_HOLE:
                open = (info.bossInfo->state == IN_PROGRESS);
                break;
            default:
                break;
        }
    }
    door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

void InstanceScript::AddObject(Creature* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _creatureInfo.find(obj->GetEntry());
    if (j != _creatureInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(GameObject* obj, bool add)
{
    ObjectInfoMap::const_iterator j = _gameObjectInfo.find(obj->GetEntry());
    if (j != _gameObjectInfo.end())
        AddObject(obj, j->second, add);
}

void InstanceScript::AddObject(WorldObject* obj, uint32 type, bool add)
{
    if (add)
        _objectGuids[type] = obj->GetGUID();
    else
    {
        ObjectGuidMap::iterator i = _objectGuids.find(type);
        if (i != _objectGuids.end() && i->second == obj->GetGUID())
            _objectGuids.erase(i);
    }
}

void InstanceScript::AddDoor(GameObject* door, bool add)
{
    DoorInfoMapBounds range = doors.equal_range(door->GetEntry());
    if (range.first == range.second)
        return;

    for (; range.first != range.second; ++range.first)
    {
        DoorInfo const& data = range.first->second;

        if (add)
        {
            data.bossInfo->door[data.type].insert(door);
            switch (data.boundary)
            {
                default:
                case BOUNDARY_NONE:
                    break;
                case BOUNDARY_N:
                case BOUNDARY_S:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX();
                    break;
                case BOUNDARY_E:
                case BOUNDARY_W:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionY();
                    break;
                case BOUNDARY_NW:
                case BOUNDARY_SE:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() + door->GetPositionY();
                    break;
                case BOUNDARY_NE:
                case BOUNDARY_SW:
                    data.bossInfo->boundary[data.boundary] = door->GetPositionX() - door->GetPositionY();
                    break;
            }
        }
        else
            data.bossInfo->door[data.type].erase(door);
    }

    if (add)
        UpdateDoorState(door);
}

void InstanceScript::AddMinion(Creature* minion, bool add)
{
    MinionInfoMap::iterator itr = minions.find(minion->GetEntry());
    if (itr == minions.end())
        return;

    if (add)
        itr->second.bossInfo->minion.insert(minion);
    else
        itr->second.bossInfo->minion.erase(minion);
}

bool InstanceScript::SetBossState(uint32 id, EncounterState state)
{
    if (id < bosses.size())
    {
        BossInfo* bossInfo = &bosses[id];
        if (bossInfo->state == TO_BE_DECIDED) // loading
        {
            bossInfo->state = state;
            //TC_LOG_ERROR("misc", "Inialize boss %u state as %u.", id, (uint32)state);
            return false;
        }
        else
        {
            if (bossInfo->state == state)
                return false;

            if (state == DONE || state == DONE_HM)
                for (MinionSet::iterator i = bossInfo->minion.begin(); i != bossInfo->minion.end(); ++i)
                    if ((*i)->isWorldBoss() && (*i)->isAlive())
                        return false;

            if (bossInfo->state == IN_PROGRESS)
                ResetBattleResurrections();

            bossInfo->state = state;
            SaveToDB();
        }

        for (uint32 type = 0; type < MAX_DOOR_TYPES; ++type)
            for (DoorSet::iterator i = bossInfo->door[type].begin(); i != bossInfo->door[type].end(); ++i)
                UpdateDoorState(*i);

        for (MinionSet::iterator i = bossInfo->minion.begin(); i != bossInfo->minion.end(); ++i)
            UpdateMinionState(*i, state);

        UpdateTeleportations();

        if (state == IN_PROGRESS && instance->IsRaid())
            SendEncounterUnit(ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT, nullptr, _battleResurrectionsCounter);

        return true;
    }
    else
        TC_LOG_ERROR("sql.sql", "InstanceScript::SetBossState boss ID %u is out of range: %u", id, bosses.size());
    return false;
}

void InstanceScript::Load(char const* data)
{
    if (!data)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(data);

    std::istringstream loadStream(data);

    if (ReadSaveDataHeaders(loadStream))
    {
        ReadSaveDataBossStates(loadStream);
        ReadSaveDataMore(loadStream);
    }
    else
        OUT_LOAD_INST_DATA_FAIL;

    OUT_LOAD_INST_DATA_COMPLETE;
}

bool InstanceScript::ReadSaveDataHeaders(std::istringstream& data)
{
    for (char header : headers)
    {
        char buff;
        data >> buff;

        if (header != buff)
            return false;
    }

    return true;
}

void InstanceScript::ReadSaveDataBossStates(std::istringstream& data)
{
    uint32 bossId = 0;
    for (std::vector<BossInfo>::iterator i = bosses.begin(); i != bosses.end(); ++i, ++bossId)
    {
        uint32 buff;
        data >> buff;
        if (buff == IN_PROGRESS || buff == SPECIAL)
            buff = NOT_STARTED;

        if (buff < TO_BE_DECIDED)
            SetBossState(bossId, EncounterState(buff));
    }
}

std::string InstanceScript::GetSaveData()
{
    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;

    WriteSaveDataHeaders(saveStream);
    WriteSaveDataBossStates(saveStream);
    WriteSaveDataMore(saveStream);

    OUT_SAVE_INST_DATA_COMPLETE;

    return saveStream.str();
}

void InstanceScript::WriteSaveDataHeaders(std::ostringstream& data)
{
    for (char header : headers)
        data << header << ' ';
}

void InstanceScript::WriteSaveDataBossStates(std::ostringstream& data)
{
    for (BossInfo const& bossInfo : bosses)
        data << uint32(bossInfo.state) << ' ';
}

void InstanceScript::HandleGameObject(uint64 guid, bool open, GameObject* go /*= nullptr*/)
{
    if (!go)
        go = instance->GetGameObject(guid);
    if (go)
        go->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: HandleGameObject failed");
}

void InstanceScript::UpdateOperations(uint32 const diff)
{
    for (auto itr = timedDelayedOperations.begin(); itr != timedDelayedOperations.end(); itr++)
    {
        itr->first -= diff;

        if (itr->first < 0)
        {
            itr->second();
            itr->second = nullptr;
        }
    }

    uint32 timedDelayedOperationCountToRemove = std::count_if(std::begin(timedDelayedOperations), std::end(timedDelayedOperations), [](const std::pair<int32, std::function<void()>>& pair) -> bool
    {
        return pair.second == nullptr;
    });

    for (uint32 i = 0; i < timedDelayedOperationCountToRemove; i++)
    {
        auto itr = std::find_if(std::begin(timedDelayedOperations), std::end(timedDelayedOperations), [](const std::pair<int32, std::function<void()>>& p_Pair) -> bool
            {
                return p_Pair.second == nullptr;
            });

        if (itr != std::end(timedDelayedOperations))
            timedDelayedOperations.erase(itr);
    }
}

void InstanceScript::DoUseDoorOrButton(uint64 guid, uint32 withRestoreTime /*= 0*/, bool useAlternativeState /*= false*/)
{
    if (!guid)
        return;

    if (auto go = instance->GetGameObject(guid))
    {
        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR || go->GetGoType() == GAMEOBJECT_TYPE_BUTTON || go->GetGoType() == GAMEOBJECT_TYPE_TRAPDOOR)
        {
            if (go->getLootState() == GO_READY)
                go->UseDoorOrButton(withRestoreTime, useAlternativeState);
            else if (go->getLootState() == GO_ACTIVATED)
                go->ResetDoorOrButton();
        }
        else
            TC_LOG_ERROR("scripts", "InstanceScript: DoUseDoorOrButton can't use gameobject entry %u, because type is %u.", go->GetEntry(), go->GetGoType());
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: HandleGameObject failed");
}

void InstanceScript::DoRespawnGameObject(uint64 guid, uint32 timeToDespawn /*= MINUTE*/)
{
    if (auto go = instance->GetGameObject(guid))
    {
        switch (go->GetGoType())
        {
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_BUTTON:
            case GAMEOBJECT_TYPE_TRAP:
            case GAMEOBJECT_TYPE_FISHINGNODE:
            case GAMEOBJECT_TYPE_TRAPDOOR:
                // not expect any of these should ever be handled
                TC_LOG_ERROR("scripts", "InstanceScript: DoRespawnGameObject can't respawn gameobject entry %u, because type is %u.", go->GetEntry(), go->GetGoType());
                return;
            default:
                break;
        }

        if (go->isSpawned())
            return;

        go->SetRespawnTime(timeToDespawn);
    }
    else
        TC_LOG_DEBUG("scripts", "InstanceScript: DoRespawnGameObject failed");
}

void InstanceScript::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for (auto itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            if (auto player = itr->getSource())
                player->SendUpdateWorldState(uiStateId, uiStateData);
    }
    else
        TC_LOG_DEBUG("scripts", "DoUpdateWorldState attempt send data but no players in map.");
}

// Send Notify to all players in instance
void InstanceScript::DoSendNotifyToInstance(char const* format, ...)
{
    InstanceMap::PlayerList const& players = instance->GetPlayers();

    if (!players.isEmpty())
    {
        va_list ap;
        va_start(ap, format);
        char buff[1024];
        vsnprintf(buff, 1024, format, ap);
        va_end(ap);
        for (auto i = players.begin(); i != players.end(); ++i)
            if (auto player = i->getSource())
                if (WorldSession* session = player->GetSession())
                    session->SendNotification("%s", buff);
    }
}

// Complete Achievement for all players in instance
void InstanceScript::DoCompleteAchievement(uint32 achievement)
{
    AchievementEntry const* pAE = sAchievementStore.LookupEntry(achievement);
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!pAE)
    {
        TC_LOG_ERROR("misc", "DoCompleteAchievement called for not existing achievement %u", achievement);
        return;
    }

    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player *player = i->getSource())
                player->CompletedAchievement(pAE);
}

// Complete Criteria for all players in instance
void InstanceScript::DoCompleteCriteria(uint32 criteria)
{
    AchievementCriteriaEntry const* achievement = sAchievementCriteriaStore.LookupEntry(criteria);
    if (!achievement)
    {
        TC_LOG_ERROR("entities.unit", "TSCR: DoCompleteCriteria called for not existing criteria %u", criteria);
        return;
    }

    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player *player = i->getSource())
                player->CompletedCriteria(achievement);
}

// Update Achievement Criteria for all players in instance
void InstanceScript::DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 /*= 0*/, uint32 miscValue2 /*= 0*/, Unit* unit /*= NULL*/)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (auto player = i->getSource())
                player->UpdateAchievementCriteria(type, miscValue1, miscValue2, 0, unit);
}

// Start timed achievement for all players in instance
void InstanceScript::DoStartTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (auto player = i->getSource())
                player->StartTimedAchievement(type, entry);
}

// Stop timed achievement for all players in instance
void InstanceScript::DoStopTimedAchievement(AchievementCriteriaTimedTypes type, uint32 entry)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (auto player = i->getSource())
                player->RemoveTimedAchievement(type, entry);
}

// Remove Auras due to Spell on all players in instance
void InstanceScript::DoRemoveAurasDueToSpellOnPlayers(uint32 spell, uint64 casterGUID, AuraRemoveMode removeMode)
{
    spell = sSpellMgr->GetSpellIdForDifficulty(spell, instance->GetSpawnMode());

    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            if (auto player = itr->getSource())
            {
                if (casterGUID && !player->HasAura(spell, casterGUID))
                    continue;
                player->RemoveAurasDueToSpell(spell, casterGUID, 0, removeMode);
                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(spell, casterGUID, 0, removeMode);
            }
        }
    }
}

// Remove Aura stack on all players in instance
void InstanceScript::DoRemoveAuraStackOnPlayers(uint32 spell, uint64 casterGUID, AuraRemoveMode removeMode)
{
    spell = sSpellMgr->GetSpellIdForDifficulty(spell, instance->GetSpawnMode());

    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
    {
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            if (auto player = itr->getSource())
            {
                if (casterGUID && !player->HasAura(spell, casterGUID))
                    continue;

                player->RemoveAuraFromStack(spell, casterGUID, removeMode);
                if (Pet* pet = player->GetPet())
                    pet->RemoveAuraFromStack(spell, casterGUID, removeMode);
            }
        }
    }
}

// Cast spell on all players in instance
void InstanceScript::DoCastSpellOnPlayers(uint32 spell)
{
    Map::PlayerList const &PlayerList = instance->GetPlayers();

    if (!PlayerList.isEmpty())
        for (auto i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (auto player = i->getSource())
                player->CastSpell(player, spell, true);
}

bool InstanceScript::CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/ /*= NULL*/, uint32 /*miscvalue1*/ /*= 0*/)
{
    TC_LOG_ERROR("misc", "Achievement system call InstanceScript::CheckAchievementCriteriaMeet but instance script for map %u not have implementation for achievement criteria %u",
        instance->GetId(), criteria_id);
    return false;
}

void InstanceScript::SendEncounterUnit(uint32 type, Unit* unit /*= NULL*/, uint8 param1 /*= 0*/, uint8 param2 /*= 0*/)
{
    if (!this)
        return;
    // size of this packet is at most 15 (usually less)
    WorldPacket data(SMSG_UPDATE_INSTANCE_ENCOUNTER_UNIT, 15);
    data << uint32(type);

    switch (type)
    {
        case ENCOUNTER_FRAME_ENGAGE:
        case ENCOUNTER_FRAME_DISENGAGE:
        case ENCOUNTER_FRAME_UPDATE_PRIORITY:
            if (!unit)
                return;
            data.append(unit->GetPackGUID());
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_ADD_TIMER:
        case ENCOUNTER_FRAME_ENABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_DISABLE_OBJECTIVE:
        case ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT:
            data << uint8(param1);
            break;
        case ENCOUNTER_FRAME_UPDATE_OBJECTIVE:
            data << uint8(param1);
            data << uint8(param2);
            break;
        case ENCOUNTER_FRAME_UNK7:
        case ENCOUNTER_FRAME_ADD_COMBAT_RES_LIMIT:
        case ENCOUNTER_FRAME_RESET_COMBAT_RES_LIMIT:
        default:
            break;
    }

    instance->SendToPlayers(&data);
}

void InstanceScript::UpdateEncounterState(EncounterCreditType type, uint32 creditEntry, Unit* /*source*/)
{
    DungeonEncounterList const* encounters = sObjectMgr->GetDungeonEncounterList(instance->GetId(), instance->GetDifficulty());
    if (!encounters)
        return;

    uint32 dungeonId = 0;
    uint32 encounterId = 0;
    for (DungeonEncounterList::const_iterator itr = encounters->begin(); itr != encounters->end(); ++itr)
    {
        DungeonEncounter const* encounter = *itr;
        if (encounter->creditType == type && encounter->creditEntry == creditEntry)
        {
            completedEncounters |= 1 << encounter->dbcEntry->encounterIndex;
            encounterId = encounter->dbcEntry->id;
            if (encounter->lastEncounterDungeon)
            {
                dungeonId = sLFGMgr->GetLfgMapDungeonId(instance->GetId(), instance->GetDifficulty());
                TC_LOG_DEBUG("lfg", "UpdateEncounterState: Instance %s (instanceId %u) completed encounter %s. Credit Dungeon: %u", instance->GetMapName(), instance->GetInstanceId(), encounter->dbcEntry->encounterName, dungeonId);
                break;
            }
        }
    }

    /*if (dungeonId)
        CompleteChallengeMode();*/

    bool LFGRewarded = false;
    std::vector<uint32> guildList;
    Map::PlayerList const& players = instance->GetPlayers();
    for (auto i = players.begin(); i != players.end(); ++i)
    {
        if (auto player = i->getSource())
        {
            /*if (dungeonId)
                if (Quest const* quest = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_ID))
                    if (player->GetQuestStatus(CHALLENGE_MODE_QUEST_ID) == QUEST_STATUS_FAILED)
                        CompleteChallengeMode();*/
            
            if (Group* grp = player->GetGroup())
            {
                bool guildAlreadyUpdate = false;
                for (auto guildItr = guildList.begin(); guildItr != guildList.end(); guildItr++)
                    if (*guildItr == player->GetGuildId())
                        guildAlreadyUpdate = true;

                if (!guildAlreadyUpdate)
                {
                    if (Guild *guild = player->GetGuild())
                    {
                        if (grp->IsGuildGroupFor(player))
                        {
                            if (lfg::LFGDungeonData const* dungeon = sLFGMgr->GetLFGDungeon(dungeonId))
                            {
                                if (grp->MemberLevelIsInRange(dungeon->minlevel, dungeon->maxlevel))
                                {
                                    if (dungeonId)
                                        guild->CompleteChallenge(instance->IsNonRaidDungeon() ? GUILD_CHALLENGE_DUNGEON : GUILD_CHALLENGE_RAID, player);
                                    else if (instance->IsRaid())
                                        guild->CompleteChallenge(GUILD_CHALLENGE_RAID, player);
                                }
                            }

                            guild->AddGuildNews(GUILD_NEWS_DUNGEON_ENCOUNTER, player->GetGUID(), 0, encounterId);
                            guildList.push_back(player->GetGuildId());
                        }
                    }
                }

                if (grp->isLFGGroup() && !LFGRewarded && dungeonId)
                {
                    sLFGMgr->FinishDungeon(grp->GetGUID(), dungeonId);
                    LFGRewarded = true;
                }
            }
        }
    }
}

void InstanceScript::UpdatePhasing()
{
    PhaseUpdateData phaseUdateData;
    phaseUdateData.AddConditionType(CONDITION_INSTANCE_INFO);

    Map::PlayerList const& players = instance->GetPlayers();
    for (auto itr = players.begin(); itr != players.end(); ++itr)
        if (auto player = itr->getSource())
            player->GetPhaseMgr().NotifyConditionChanged(phaseUdateData);
}

void InstanceScript::SendCinematicStartToPlayers(uint32 cinematicId)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (auto player = itr->getSource())
            {

                player->StopMoving();
                if (Aura* stun = player->AddAura(85379, player))
                    {
                        stun->SetMaxDuration(200);
                        stun->SetDuration(200);
                    }
                player->NearTeleportTo(*player);
                player->SetUnitMovementFlags(MOVEMENTFLAG_NONE);
                player->SendCinematicStart(cinematicId);
            }
}

void InstanceScript::CompleteGuildCriteriaForGuildGroup(uint32 criteriaId)
{
    if (AchievementCriteriaEntry const* entry = sAchievementCriteriaStore.LookupEntry(criteriaId))
    {
        Map::PlayerList const& players = instance->GetPlayers();
        for (auto i = players.begin(); i != players.end(); ++i)
            if (auto player = i->getSource())
                if (Group* grp = player->GetGroup())
                    if (grp->IsGuildGroupFor(player))
                        if (auto guild = player->GetGuild())
                        {
                            guild->CompletedCriteria(entry, player);
                            break;
                        }
    }
}

void InstanceScript::ResetBattleResurrections()
{
    if (instance->IsRaid())
    {
        SendEncounterUnit(ENCOUNTER_FRAME_RESET_COMBAT_RES_LIMIT);
        _battleResurrectionsCounter = 0;
    }
}

bool InstanceScript::IsBattleResurrectionAvailable() const
{
    if (!instance->IsRaid() || !IsEncounterInProgress())
        return true;

    return _battleResurrectionsCounter < (instance->Is25ManRaid() ? 3 : 1);
}

void InstanceScript::UseBattleResurrection()
{
    if (instance->IsRaid() && IsEncounterInProgress())
    {
        ++_battleResurrectionsCounter;

        if (_battleResurrectionsCounter >= (instance->Is25ManRaid() ? 3 : 1))
            SendEncounterUnit(ENCOUNTER_FRAME_ADD_COMBAT_RES_LIMIT);
        else
            SendEncounterUnit(ENCOUNTER_FRAME_SET_COMBAT_RES_LIMIT, nullptr, _battleResurrectionsCounter);
    }
}

void InstanceScript::FinishLfgDungeon(Creature* boss)
{
    Map::PlayerList const& players = boss->GetMap()->GetPlayers();
    if (!players.isEmpty())
    {
        if (Group* group = players.begin()->getSource()->GetGroup())
        {
            if (group->isLFGGroup())
            {
                uint32 dungeonId = sLFGMgr->GetDungeon(group->GetGUID());
                if (dungeonId)
                    sLFGMgr->FinishDungeon(group->GetGUID(), dungeonId);
            }
        }
    }
}

void InstanceScript::UpdateTeleportations()
{
    for (std::map<uint32, std::list<uint32 > >::iterator itr = _teleportationReq.begin(); itr != _teleportationReq.end(); itr++)
    {
        bool allowTeleport = true;
        for (auto iter = itr->second.begin(); iter != itr->second.end(); iter++)
            if (!IsDone(*iter))
            {
                allowTeleport = false;
                break;
            }
        for (unsigned int i = 0; i < _teleporters.size(); i++)
            if (Creature *teleporter = instance->GetCreature(_teleporters[i]))
                teleporter->AI()->SetData((*itr).first, allowTeleport ? 1 : 0);
    }
}

void InstanceScript::LoadTeleportationData(const TeleportationData* data)
{
    _teleportationReq.clear();
    while (data->teleportationId)
    {
        if (data->bossId < bosses.size())
            _teleportationReq[data->teleportationId].push_back(data->bossId);
        ++data;
    }
}

void InstanceScript::AddTeleporter(uint64 guid)
{
    _teleporters.push_back(guid);
    UpdateTeleportations();
}

void InstanceScript::DoOnPlayers(std::function<void(Player*)>&& function)
{
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (auto player = itr->getSource())
                function(player);
}

void InstanceScript::DoOnCreatures(std::function<void(Creature*)>&& function)
{
    for (uint64 guid : _creatures)
        if (auto cre = instance->GetCreature(guid))
            function(cre);
}

class ChallengeModeWorker
{
public:
    ChallengeModeWorker(InstanceScript* instance) : _instance(instance) { }

    void Visit(std::unordered_map<ObjectGuid, Creature*>& creatureMap)
    {
        for (auto const& p : creatureMap)
        {
            if (p.second->IsInWorld())
            {
                if (!p.second->isAlive())
                    p.second->Respawn();
            }
        }
    }

    template<class T>
    void Visit(std::unordered_map<ObjectGuid, T*>&) { }

private:
    InstanceScript* _instance;
};


std::unordered_map<uint8, std::string> AffixNamesList_2 =
{
    { AFFIX_NONE,           "|TInterface/ICONS/trade_engineering:15|t|r None"							},

    { AFFIX_FORTIFIED,      "|TInterface/ICONS/ability_toughness:15|t|r Fortified"						},
    { AFFIX_TYRANNICAL,     "|TInterface/ICONS/spell_shadow_charm:15|t|r Tyrannical"					},

    { AFFIX_TEEMING,        "|TInterface/ICONS/spell_nature_massteleport:15|t|r Teeming"				},
    { AFFIX_RAGING,         "|TInterface/ICONS/ability_warrior_endlessrage:15|t|r Raging"				},
    { AFFIX_BOLSTERING,     "|TInterface/ICONS/ability_warrior_battleshout:15|t|r Bolstering"			},
    { AFFIX_SANGUINE,       "|TInterface/ICONS/ability_deathwing_bloodcorruption_death:15|t|r Sanguine"	},
    { AFFIX_BURSTING,       "|TInterface/ICONS/inv_elemental_mote_fire01:15|t|r Bursting"				},

    { AFFIX_NECROTIC,       "|TInterface/ICONS/Ability_Creature_Disease_02:15|t|r Necrotic"				},
    { AFFIX_EXPLOSIVE,      "|TInterface/ICONS/spell_mage_flameorb:15|t|r Explosive"					},
    { AFFIX_VOLCANIC,       "|TInterface/ICONS/spell_shaman_lavasurge:15|t|r Volcanic"					},
    { AFFIX_SKITTISH,       "|TInterface/ICONS/spell_magic_lesserinvisibilty:15|t|r Skittish"			},
    { AFFIX_GRIEVOUS,       "|TInterface/ICONS/ability_backstab:15|t|r Grievous"						},
    { AFFIX_QUAKING,        "|TInterface/ICONS/spell_nature_earthquake:15|t|r Quaking"					},
    { AFFIX_OVERFLOWING,    "|TInterface/ICONS/inv_misc_volatilewater:15|t|r Overflowing"				},
    { AFFIX_RELENTLESS,     "|TInterface/ICONS/ability_warrior_bloodsurge:15|t|r Relentless"			},
    { AFFIX_SPITEFUL,       "|TInterface/ICONS/spell_holy_prayerofshadowprotection:15|t|r Spiteful"		},
    { AFFIX_STORMING,       "|TInterface/ICONS/spell_nature_cyclone:15|t|r Storming"		            },
    { AFFIX_PRIDEFUL,       "|TInterface/ICONS/ability_warlock_baneofhavoc:15|t|r Prideful"		        },
    { AFFIX_THUNDERING,     "|TInterface/ICONS/spell_shaman_thunderstorm:15|t|r Thundering"		        },
};

bool InstanceScript::MeetsChallengeRequirements()
{
    Map::PlayerList const& instancePlayers = instance->GetPlayers();
    if (!instancePlayers.isEmpty())
        for (auto itr = instancePlayers.begin(); itr != instancePlayers.end(); ++itr)
            if (auto iPlayer = itr->getSource())
                if (!iPlayer->isGameMaster())
                    if (iPlayer->GetAverageItemLevel() < 370)
                        return false;
    return true;
};

void InstanceScript::StartChallengeMode(Player* player)
{
    if (IsChallengeModeStarted() || IsChallengeModeFinished())
        return;

    if (GetCompletedEncounterMask() != 0)
        return;

    AreaTriggerStruct const* areaTrigger = sObjectMgr->GetMapEntranceTrigger(instance->GetId());
    if (!areaTrigger)
        return;

    Keystone keystone = player->GetKeystone();
    uint8 level = keystone.level;

    if (keystone.mapId != instance->GetId())
        level = 1;
    else
    {
        for (uint8 affix : keystone.affixes)
            if (affix != 0)
                _challengeModeAffixes.insert(ChallengeModeAffix(affix));
    }

    _challengeModeStarted = true;
    _challengeModeLevel = level;

    float healthMultiplier = sChallengeModeMgr->GetHealthMultiplier(level);

    for (uint64 guid : _creatures)
    {
        if (auto cre = instance->GetCreature(guid))
        {
            if (!cre->isAlive())
                cre->Respawn(true);

            float previousHealthPct = cre->GetHealthPct();
            cre->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, (cre->GetMaxHealth() * healthMultiplier) - cre->GetMaxHealth(), true);
            //cre->SetHealth(cre->CountPctFromMaxHealth(previousHealthPct));
            cre->SetHealth(cre->GetMaxHealth());

            sChallengeModeMgr->ApplyAffixes(cre);
        }
    }

    switch (instance->GetId())
    {
        case 657:
            _challengeModeMobsToKill =
            {
                {45915, 2},
                {45917, 2},
                {43878, 1},
                {45922, 6},
                {45919, 2},
                {45935, 3},
                {45932, 8},
                {43875, 1},
                {43873, 1}
            };
            break;
        case 645:
            _challengeModeMobsToKill =
            {
              {39708, 4}, // twilight flamecaller
              {39978, 2}, // twilight torturer
              {39665, 1}, // romogg bonecrusher
              {39987, 4}, // evolved twilight zealot
              {39679, 1}, // corla
              {39990, 9}, // twilight zealot
              {39994, 2}, // conflagration
              {39698, 1}, // karsh steelbender
              {40019, 6}, // twilight obsidian borer
              {40023, 4}, // defiled earth rager
              {39700, 1}, // beauty
              {40021, 2}, // incendiary spark
              {40017, 2}, // Twilight element warden
              {39705, 1} // ascendant lord obsidius
            };
            break;
        case 36:
            _challengeModeMobsToKill =
            {
                {48262, 2}, // ogre bodyguard
                {47162, 1}, // glubtok
                {48338, 1}, // mine bunny
                {48279, 2}, // goblin overseer
                {48445, 2}, // oaf lackey
                {47296, 1}, // helix
                {48419, 6}, // defias miner
                {47403, 2}, // defias reaper
                {47404, 2}, // defias watcher
                {43778, 1}, // foe reaper
                {48417, 3}, // blood wizard
                {48502, 3}, // enforcer
                {48505, 2}, // shadowguard
                {48447, 3}, // monstrous parrot
                {48521, 2}, // squallshaper
                {48522, 5}, // pirate
                {47626, 1}, // admiral
                {47739, 1}, // cookie
                {49670, 1}, // nightmare glubtok
                {49674, 1}, // nightmare helix
                {49681, 1}, // nightmare reaper
                {49532, 3}, // enraged worgen
                {49541, 1}  // vanessa
            };
            break;
        case 670:
            _challengeModeMobsToKill =
            {
                {39854, 4}, // azureborn guardian
                {39381, 2}, // crimsonborne guardian
                {39405, 4}, // crimsonborne seer
                {39961, 2}, // enslaved water spirit
                {39962, 2}, // twilight stormbreaker
                {41073, 1}, // twilight armsmaster
                {39900, 2}, // enslaved rock elemental
                {39890, 2}, // twilight earthshaper
                {39956, 4}, // twilight enforcer
                {39414, 1}, // ascended windwalker
                {39415, 1}, // ascended flameseeker
                {39625, 1}, // general umbriss
                {39450, 12}, // trogg dweller
                {40166, 1}, // enslaved gronn brute
                {40167, 3}, // twilight beguiler
                {40177, 1}, // forgemaster throngus
                {39909, 4}, // azureborn warlord
                {40270, 1}, // twilight thundercaller
                {40269, 1}, // enslaved thunder spirit
                {39870, 1}, // twilight firecatcher
                {39892, 1}, // enslaved burning ember
                {39873, 1}, // wyrmcaller
                {39626, 3}, // crimsonborne warlord
                {40268, 1}, // twilight warmage
                {40319, 1}, // drahga shadowburner
                {40272, 1}, // ascended rockbreaker
                {40273, 1}, // ascended waterlasher
                {39392, 2}, // faceless corruptor
                {40484, 1}  // erudax
            };
            break;
        case 644:
            _challengeModeMobsToKill =
            {
                {48143, 3},
                {39425, 1},
                {40251, 3},
                {39788, 1},
                {39428, 1},
                {39732, 1},
                {40668, 2},
                {39731, 1},
                {40033, 2},
                {39587, 1},
                {39378, 1}
            };
            break;

        case 755:
            _challengeModeMobsToKill =
            {
                {45122, 1}, // Oathsworn Captain
                {44981, 1}, // Oathsworn Skinner
                {44922, 1}, // Oathsworn Axemaster
                {44577, 1}, // General Husam
                {44897, 3}, // Pygmy Scout
                {44898, 2}, // Pygmy Firebreather
                {44896, 2}, // Pygmy Brute      
                {43614, 1}, // Lockmaw
                {49045, 1}, // Augh
                {44981, 1}, // Oathsworn Skinner
                {44982, 1}, // Neferset Darkcaster
                {44976, 1}, // Neferset Plaguebringer
                {45065, 4}, // Tolvir Merchant
                {43612, 1}, // High Prophet Barim
                {45259, 3}, // Servant of Siamat
                {44819, 1}  // Siamat
            };
            break;
        case 33:
            _challengeModeMobsToKill =
            {
                {47134, 3},
                {46962, 1},
                {47232, 4},
                {47231, 5},
                {3887, 1},
                {4278, 1},
                {47145, 2},
                {47132, 8},
                {46963, 1},
                {47141, 1},
                {46964, 1}
            };
            break;
        case 725:
            _challengeModeMobsToKill =
            {
                {42808, 7}, // flayer
                {43537, 2}, // earthshaper
                {43438, 1}, // corborus
                {42810, 2}, // crystal giant
                {43214, 1}, // slabhide
                {42695, 3}, // sentry
                {42692, 2}, // bruiser
                {42188, 1}, // ozruk
                {42333, 1}  // azil
            };
            break;
        case 643:
            _challengeModeMobsToKill =
            {
                {40584, 2},
                {41139, 2},
                {40577, 2},
                {40586, 1},
                {40765, 1},
                {40935, 4},
                {40943, 4},
                {40936, 3},
                {40788, 1},
                {40655, 1},
                {40925, 2}
            };
            break;
        case 938:
        {
            std::map<uint32, uint16> baine_kc
            {
                {54431, 1},//baine
                {54543, 3},//drake
                {54552, 3},//breaker
                {54553, 3},//seer
            };
            std::map<uint32, uint16> jaina_kc
            {
                {54445, 1},//jaina
                {54687, 6},//Time twisted footman, 10 in db
                {54690, 2},//Time twisted priest, 3 in db
                {54691, 2},//Time twisted sorceress, 5 in db
                {54693, 2},//Time twisted riflemen, 5 in db
            };
            std::map<uint32, uint16> sylvanas_kc
            {
                {54123, 1},//sylvanas
                {54952, 3},//brittle ghoul
                {54507, 2},//scourge beast
                {54511, 10}, //geists
                {54517, 2},//skeletal crow
            };
            std::map<uint32, uint16> tyrande_kc
            {
                {54544, 1},//tyrande
                {54688, 5},//Nightsaber 1, 35 in db
                {54699, 4},//Nightsaber 2, 28 in db
                {54700, 3},//Nightsaber 3, 22 in db
                {54701, 1},//huntress, 6 in db
                {54512, 3},//sentinel, 13 in db
            };
            std::map<uint32, uint16> murozond_kc
            {
                {54432, 1},//murozond
                {54923, 4},//warden
                {54920, 4},//suppressor
            };

            switch (GetData(5003))
            {
            case 0: //Sylvanas  -   Jaina       - Murozond
                _challengeModeMobsToKill = sylvanas_kc;
                for (auto& it : jaina_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 1: //Sylvanas  -   Tyrande     - Murozond
                _challengeModeMobsToKill = sylvanas_kc;
                for (auto& it : tyrande_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 2: //Sylvanas  -   Baine       - Murozond
                _challengeModeMobsToKill = sylvanas_kc;
                for (auto& it : baine_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 3: //Jaina     -   Sylvanas    - Murozond
                _challengeModeMobsToKill = jaina_kc;
                for (auto& it : sylvanas_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 4: //Jaina     -   Tyrande     - Murozond
                _challengeModeMobsToKill = jaina_kc;
                for (auto& it : tyrande_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 5: //Jaina     -   Baine       - Murozond
                _challengeModeMobsToKill = jaina_kc;
                for (auto& it : baine_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 6: //Tyrande   -   Sylvanas    - Murozond
                _challengeModeMobsToKill = tyrande_kc;
                for (auto& it : sylvanas_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 7: //Tyrande   -   Jaina       - Murozond
                _challengeModeMobsToKill = tyrande_kc;
                for (auto& it : jaina_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                break;
            case 8: //Tyrande   -   Baine       - Murozond
                _challengeModeMobsToKill = tyrande_kc;
                for (auto& it : baine_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
            case 9: //Baine     -   Sylvanas    - Murozond
                _challengeModeMobsToKill = baine_kc;
                for (auto& it : sylvanas_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
            case 10://Baine     -   Jaina       - Murozond
                _challengeModeMobsToKill = baine_kc;
                for (auto& it : jaina_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
            case 11://Baine     -   Tyrande     - Murozond
                _challengeModeMobsToKill = baine_kc;
                for (auto& it : tyrande_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
                for (auto& it : murozond_kc)
                    _challengeModeMobsToKill[it.first] = it.second;
            }
            break;
        }

        case 939://well of eternity
            _challengeModeMobsToKill =
            {
                {55085, 1},//Perotharn
                {55419, 1},//Varothen
                {54969, 1},//Mannoroth
                //Trash between bosses
                {55503, 2},//Firewall Demon
                {55654, 3},//Corrupted Arcanist
                {55656, 6},//Dreadlord defender
                {54612, 4},//Eternal Champion
                {54747, 3},//Eyes
                {54589, 2},//Mage 1
                {56579, 2},//Mage 2
                {54645, 1},//Mage 2

                {54883,2},//Azshara: mage 1
                {54884,2},//Azshara: mage 2
                {54882,2},//Azshara: mage 3
                {55519,14},//Hoard of doomguards that you use moonwell for
                {55510, 1},//big fiery fuck

                {55543, 2},//Varothen: two shadowbats
                {55426, 2},//Varothen: two elites
            };
            break;
        case 940://Hour of Twilight

            _challengeModeMobsToKill =
            {
                //Bosses
                {54590, 1},
                {54968, 1},
                {54938, 1},
                //Arcurion Trash
                {54555, 5},
                {55559, 1},
                {55563, 5},
                //Asira Trash
                {55106, 4},
                {55107, 5},
                {55109, 4},
                {55111, 4},
                {55112, 6},
                //Benedictus trash
                {54633, 3},
                {54632, 3},
                {54686, 3},
                {54646, 3},
            };
            break;
        case 568://Zul Aman
            _challengeModeMobsToKill =
            {   
                //Bosses
                {23578, 1},//Janalai
                {23863, 1},
                {24239, 1},
                {23577, 1},
                {23576, 1},
                {23574, 1},

                {24549, 1},//AKilzon: Tempest caller
                {24175, 1},//AKilzon: snitch lookout
                {24179, 3},//AKilzon: windwalker
                {24180, 3},//AKilzon: protector

                {23542, 4}, //Nalorakk: Axe thrower
                {23580, 2}, //Nalorakk: Warbringer
                {23581, 1}, //Nalorakk: Medicine Man

                {23586, 3},//Janali: lookouts
                {23596, 4},//Janali: Firecasters
                {23774, 3}, //Janali: Trainers
                {23834, 12}, //Janali: Dragonhawks

                {24065, 3},//Halazzi: Handler
                {24064, 5},//Halazzi: cub
                {24059, 1},//Halazzi: Beast Tamer
                {24138, 5},//Halazzi: Tamed croc
                {24047, 2},//Halazzi: Wild croc
                {24043, 6},//Halazzi: Lynx

                {24374,2},//Hexlord: Berserkers

                //all over:
                {23597, 7}, //Amanashi guardian
                {23889, 20}, //Amanashi savages
                {52962, 3}, //Zandalari Archon
                {52958, 3}, //Zandalari Hierophant
                {52956, 3}, //Zandalari Juggernaut
            };
            break;
        case 859://Zul Gurub
            _challengeModeMobsToKill =
            {
                //bosses
                {52155, 1},
                {52151, 1},
                {52059, 1},
                {52053, 1},
                {52148, 1},

                {52311, 2},//Venoxis: Venomguard, two posted outside venoxis

                {52606, 3},//Mandokir: Warmonger
                {52079, 2},//Mandokir: Bloodrager (on raptors)

                {52362, 1},//Kilnara: Tiki lord
                {52345, 4},//Kilnara: outside version of "pride of bethekk"
                {52327, 2},//Kilnara: Shadow hunters
                {52339, 2}, //Kilnara: Shadow priest
                {52061, 16},//Kilnara: boss fight pride of bethekk

                {52082, 1},//Zanzil: Green cauldron mixer
                {52085, 5}, //Zanzil: snakes after cauldron
                {52086, 2},//Zanzil: Witch doctors
                {52081, 1},//Zanzil: Red cauldron mixer
                {52087, 15},//Zanzil: Mindslave
                {52077, 2},//Zanzil: Berserksers

                {52167, 2},//Jindo: Spirit warriors/berserkers
                {52430, 3},//Jindo: Chains of Hakkar

                //Misc trash:
                {52325, 3},//Blood Drinker
                {52438, 1},//Mortaxx
                {52076, 1},//Frost cauldron npc
                {52380, 1},//Venomancer, first one you see when you enter
                {52340, 1},//First tiki lord
                {52381, 1},//Venomancer, second one you see, just outside venoxis
                {52348, 1},//Boulder roller
                {52322, 1},//The one npc that shields all the snake morphers
                {52323, 11},//All the snake morphers

                {52962, 3}, //Zandalari Archon
                {52958, 3}, //Zandalari Hierophant
                {52956, 3}, //Zandalari Juggernaut
            };
            break;
    }

    _challengeModeMobsStatus = _challengeModeMobsToKill;

    /*
        SENDING TIMER
    */
    /*
    Battlegrounds version:
        WorldPacket data(SMSG_START_TIMER, 4+4+4);
        data << uint32(0); // unk
        data << uint32(countdownMaxForBGType - (GetElapsedTime() / 1000));
        data << uint32(countdownMaxForBGType);
    */
    DoOnPlayers([](Player* player)
        {
            if (player->GetOTeam() == ALLIANCE)
                player->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, player->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
        });

    WorldPacket data(SMSG_START_TIMER, 4+4+4);//must be between 0 and 12. higher makes it not appear, lower crashes server.
    data << uint32(0); //this must exist with a zero value for anything to appear. higher or lower makes it not appear, not existing makes it not appear.
    data << uint32(11); //current time in seconds
    data << uint32(11); //full time in seconds
    instance->SendToPlayers(&data);
    /*
        Other misc timer experimentation
    */
    /*
        WorldPacket data3(SMSG_START_MIRROR_TIMER, (21));
        data3 << (uint32)BREATH_TIMER;                                  //bar color
        data3 << 10000;                                                 //Currenet Time
        data3 << 20000;                                                 //Max time value
        data3 << 0;                                                     // COUNT DOWN = -1, STAY STATIC = 0, INCREASE = 1
        data3 << (uint8)0;                                              // positive = stop, negative = hide, 0 = no effect
        data3 << (uint32)91422;                                         // spell id, client side spell name will be overlayed on the bar.
        instance->SendToPlayers(&data3);

        WorldPacket data4(SMSG_START_MIRROR_TIMER, (21));
        data4 << (uint32)FIRE_TIMER;                                    //bar color
        data4 << 10000;                                                 //Currenet Time
        data4 << 20000;                                                 //Max time value
        data4 << 0;                                                     // COUNT DOWN = -1, STAY STATIC = 0, INCREASE = 1
        data4 << (uint8)0;                                              // positive = stop, negative = hide, 0 = no effect
        data4 << (uint32)67502;                                         // spell id, client side spell name will be overlayed on the bar.
        instance->SendToPlayers(&data4);

        //Second occurance of a bar slot (color, in this case, BREATH_TIMER) will overwrite any previously existing one.
        WorldPacket data5(SMSG_START_MIRROR_TIMER, (21));
        data5 << (uint32)BREATH_TIMER;                                  //bar color
        data5 << 10000;                                                 //Currenet Time
        data5 << 20000;                                                 //Max time value
        data5 << 0;                                                     // COUNT DOWN = -1, STAY STATIC = 0, INCREASE = 1
        data5 << (uint8)0;                                              // positive = stop, negative = hide, 0 = no effect
        data5 << (uint32)57685;                                         // spell id, client side spell name will be overlayed on the bar.
        instance->SendToPlayers(&data5);
    */

    int8 startPosNumber = 0;
    Position centerPos = { areaTrigger->target_X, areaTrigger->target_Y, areaTrigger->target_Z, areaTrigger->target_Orientation };
    Position movePosition;
    Player* leader;

    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (auto player = itr->getSource())
            if (!player->isGameMaster())
            {
                player->GetRole(true);

                sChallengeModeMgr->ApplyAffixes(player);
                /*
                void GetPositionWithDistInOrientation(Position* fromPos, float dist, float orientation, Position& movePosition)
                */
                switch (startPosNumber)
                {
                case 0:
                    player->NearTeleportTo(areaTrigger->target_X, areaTrigger->target_Y, areaTrigger->target_Z, areaTrigger->target_Orientation);
                    player->ToUnit()->UpdateOrientation(areaTrigger->target_Orientation);
                    leader = player;
                    break;
                case 1://far left of leader
                    GetPositionWithDistInOrientation(&centerPos, 3.f, (leader->GetOrientation() + (M_PI / 2)), movePosition);
                    player->NearTeleportTo((&movePosition)->GetPositionX(), (&movePosition)->GetPositionY(), (&movePosition)->GetPositionZ() + 0.5f, leader->GetOrientation());
                    break;
                case 2://left of leader
                    GetPositionWithDistInOrientation(&centerPos, 1.5f, (leader->GetOrientation() + (M_PI / 2)), movePosition);
                    player->NearTeleportTo((&movePosition)->GetPositionX(), (&movePosition)->GetPositionY(), (&movePosition)->GetPositionZ() + 0.5f, leader->GetOrientation());
                    break;
                case 3://right of leader
                    GetPositionWithDistInOrientation(&centerPos, 1.5f, (leader->GetOrientation() - (M_PI / 2)), movePosition);
                    player->NearTeleportTo((&movePosition)->GetPositionX(), (&movePosition)->GetPositionY(), (&movePosition)->GetPositionZ() + 0.5f, leader->GetOrientation());
                    break;
                case 4://far right of leader
                    GetPositionWithDistInOrientation(&centerPos, 3.f, (leader->GetOrientation() - (M_PI / 2)), movePosition);
                    player->NearTeleportTo((&movePosition)->GetPositionX(), (&movePosition)->GetPositionY(), (&movePosition)->GetPositionZ() + 0.5f, leader->GetOrientation());
                    break;
                default:
                    // should never happen.
                    player->NearTeleportTo(areaTrigger->target_X, areaTrigger->target_Y, areaTrigger->target_Z, areaTrigger->target_Orientation);
                    break;
                }
                //set pets in front of owner
                if (Pet* playerPet = player->GetPet())
                if (auto playerPetUnit = playerPet->ToUnit())
                {
                    playerPetUnit->CastCustomSpell(68442, SPELLVALUE_DURATION, 15000, playerPetUnit);
                    GetPositionWithDistInOrientation(&movePosition, 1.5f, leader->GetOrientation(), movePosition);
                    playerPetUnit->NearTeleportTo((&movePosition)->GetPositionX(), (&movePosition)->GetPositionY(), (&movePosition)->GetPositionZ(), player->GetOrientation());
                }
                startPosNumber++;
            }

    DoOnPlayers([](Player* player)
        {
            /*
                We'll add a custom duration to the spell in case there is a crash before it's removed. it'll fade after 15 seconds if not automatically after 11 seconds.
            */
            player->CastCustomSpell(68442, SPELLVALUE_DURATION, 15000, player);
            if (Pet* playerPet = player->GetPet())
                if (auto playerPetUnit = playerPet->ToUnit())
                    playerPetUnit->CastCustomSpell(68442, SPELLVALUE_DURATION, 15000, player);
        });

    AddTimedDelayedOperation(11000, [this]()
    {
        _challengeModeStartTime = getMSTime();

        if (Quest const* qInfo = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_ID))
        {
            DoOnPlayers([qInfo](Player* player)
            {
                player->AddQuest(qInfo, nullptr);
                player->RemoveAurasDueToSpell(68442);

                if (Pet* playerPet = player->GetPet())
                    if (auto playerPetUnit = playerPet->ToUnit())
                        playerPetUnit->RemoveAurasDueToSpell(68442);
            });
        }

        if (Quest const* qInfo2 = sObjectMgr->GetQuestTemplate(CHALLENGE_MODE_QUEST_KILL_COUNT))
        {
            DoOnPlayers([qInfo2](Player* player)
            {
                player->AddQuest(qInfo2, nullptr);
                player->RemoveAurasDueToSpell(9454);
            });
        }
    });

    if (HasAffix(AFFIX_QUAKING))
    {
        GetScheduler().Schedule(Milliseconds(11000 + AFFIX_QUAKING_TIMER), [this](TaskContext context)
        {
            if (urand(0, 2) == 0)
                DoOnPlayers([this](Player* player)
                {
                    if (isAffixEnabled(AFFIX_QUAKING))
                    if (!player->isGameMaster())
                    if (player->isAlive())
                    {
                        if (auto c = instance->SummonCreature(AFFIX_NPC_QUAKING, *player))
                        {
                            c->AddAura(AFFIX_SPELL_QUAKING_WARNING, c);
                            c->DespawnOrUnsummon(5000);

                            c->SetReactState(REACT_PASSIVE);
                            c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                            c->DisableSpline();
                            c->m_movementInfo.t_guid = player->GetGUID();
                            c->m_movementInfo.t_pos.Relocate(0.f, 0.f, 0.f, 0.f);//left
                            c->DisableSpline();
                            c->SendTeleportPacket(c->m_movementInfo.t_pos);
                        }

                        player->GetScheduler().Schedule(2500ms, [player, this](TaskContext context)
                            {
                                player->CastSpell(player, AFFIX_SPELL_QUAKING, true);
                                player->CastSpell(player, 85543, true);

                                auto players = player->GetPlayersInRange(5.f, true, false);
                                if (players.size())
                                for (Player* victim : players)
                                {
                                    player->SpellNonMeleeDamageLog(victim, AFFIX_SPELL_QUAKING, victim->CountPctFromMaxHealth(20));
                                    victim->InterruptNonMeleeSpells(true);
                                }

                                std::list<Creature*> list_of_npcs;
                                GetCreatureListWithEntryInGrid(list_of_npcs, player, AFFIX_NPC_QUAKING, 10.0f);
                                if (list_of_npcs.size())
                                    for (auto c : list_of_npcs)
                                    {
                                        c->RemoveAurasDueToSpell(AFFIX_SPELL_QUAKING_WARNING);
                                        c->DeleteThreatList();
                                        c->DespawnOrUnsummon();
                                    }
                            });
                    }
                });

            context.Repeat(Milliseconds(AFFIX_QUAKING_TIMER));
        });
    }


    // While in combat, enemies periodically spawn exhumed spirits
    if (HasAffix(AFFIX_INCORPOREAL) && isAffixEnabled(AFFIX_INCORPOREAL))
    {
        GetScheduler().Schedule(Milliseconds(AFFIX_INCORPOREAL_TIMER), [this](TaskContext context)
            {
                if (HasAffix(AFFIX_INCORPOREAL) && isAffixEnabled(AFFIX_INCORPOREAL))
                if (urand(0, 1) == 0)
                DoOnPlayers([this](Player* playerTarget)
                    {
                        if (urand(0, 1) == 0)
                        {
                            // Random pos can go out of LOS, we try 5 times
                            Position randomPosition;
                            playerTarget->MoveBlink(randomPosition, frand(10.f, 20.f), frand(float(-M_PI / 2), float(M_PI / 2)));

                            if (!playerTarget->isGameMaster())
                                if (auto c = instance->SummonCreature(AFFIX_NPC_INCORPOREAL, randomPosition))
                                {
                                    c->DespawnOrUnsummon(30000);
                                }
                        }

                    });


        context.Repeat(Milliseconds(AFFIX_INCORPOREAL_TIMER));
            });
    }

    // While in combat, enemies periodically spawn afflicted spirits
    if (HasAffix(AFFIX_AFFLICTED) && isAffixEnabled(AFFIX_AFFLICTED))
    {
        GetScheduler().Schedule(Milliseconds(35000), [this](TaskContext context)
            {
                if (HasAffix(AFFIX_AFFLICTED) && isAffixEnabled(AFFIX_AFFLICTED))
                if (urand(0, 1) == 0)
                DoOnPlayers([this](Player* playerTarget)
                    {

                        if (urand(0, 2) > 0)
                        {
                            Position randomPosition;
                            playerTarget->MoveBlink(randomPosition, frand(10.f, 20.f), frand(float(-M_PI / 2), float(M_PI / 2)));
                            if (!playerTarget->isGameMaster())
                                if (auto c = instance->SummonCreature(AFFIX_NPC_AFFLICTED, randomPosition))
                                {
                                    c->ToUnit()->setFaction(35);
                                    c->GetMotionMaster()->MoveFollow(playerTarget, 10.f, playerTarget->GetFollowAngle());
                                    c->DespawnOrUnsummon(20000);
                                }

                        }
                    });

        context.Repeat(Milliseconds(30000));
            });
    }


    // While in combat, enemies periodically spawn afflicted spirits
    if (HasAffix(AFFIX_ENTANGLING) && isAffixEnabled(AFFIX_ENTANGLING))
    {
        GetScheduler().Schedule(Milliseconds(35000), [this](TaskContext context)
            {
                if (HasAffix(AFFIX_ENTANGLING) && isAffixEnabled(AFFIX_ENTANGLING))
                if (urand(0, 1) == 0)//50% chance to occur
                DoOnPlayers([this](Player* playerTarget)
                    {
                        if (urand(0, 2) > 0)//66% chance to spawn per player
                        {
                            // Random pos can go out of LOS, we try 5 times
                            Position randomPosition;
                            playerTarget->MoveBlink(randomPosition, frand(2.f, 8.f), frand(0, M_PI));

                            if (!playerTarget->isGameMaster())
                                if (auto c = instance->SummonCreature(AFFIX_NPC_ENTANGLING, randomPosition))
                                {

                                    c->DespawnOrUnsummon(20000);
                                }
                        }
                    });

                context.Repeat(Milliseconds(15000));
            });
    }

    if (HasAffix(AFFIX_SHIELDING) && isAffixEnabled(AFFIX_SHIELDING))
    {
        GetScheduler().Schedule(Milliseconds(25000), [this](TaskContext context)
            {
                if (HasAffix(AFFIX_SHIELDING) && isAffixEnabled(AFFIX_SHIELDING))
                if (urand(0, 1) == 0)
                {

                    Map::PlayerList const& PlayerList = instance->GetPlayers();

                    bool selected{ false };

                    if (!PlayerList.isEmpty())
                        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (!selected)
                            if (auto playerTarget = itr->getSource())
                                if (playerTarget->isInCombat())
                                {
                                    selected = true;
                                    Position randomPosition;
                                    playerTarget->MoveBlink(randomPosition, frand(5.f, 8.f), frand(0, M_PI));
                                    randomPosition.m_positionZ = playerTarget->GetPositionZ() + 3.5f;

                                    if (!playerTarget->isGameMaster())
                                        if (auto c = instance->SummonCreature(AFFIX_NPC_SHIELDING, randomPosition))
                                        {
                                            c->SetMaxHealth(100000);
                                            c->SetHealth(100000);

                                            if (auto a = c->AddAura(17, c))
                                            {
                                                a->SetMaxDuration(45000);
                                                a->SetDuration(45000);
                                                if (auto eff = a->GetEffect(0))
                                                    eff->SetAmount(250000);
                                            }

                                            c->SetReactState(REACT_PASSIVE);
                                            c->DespawnOrUnsummon(45000);
                                            //MoveAroundPoint(Position const& centerPos, float radius, bool clockwise, uint8 numberOfPoints, float z, bool smooth)
                                            c->GetMotionMaster()->MoveAroundPoint(*playerTarget, c->GetExactDist2d(playerTarget), true, 10, c->GetPositionZ(), true);
                                        }
                                }


                }

        context.Repeat(Milliseconds(45000 + urand(10000, 15000)));
            });
    }

    if (HasAffix(AFFIX_RELENTLESS))
    {
        GetScheduler().Schedule(Milliseconds((75 - (4 * level)) * IN_MILLISECONDS), [this](TaskContext context)
        {
            for (uint64 guid : _creatures)
            {
                if (auto cre = instance->GetCreature(guid))
                    if (cre->isInCombat())
                    if (cre->isAlive() && cre->CanBeSubjectOfAffix(AFFIX_RELENTLESS) && !cre->IsDungeonBoss())
                    {
                        CustomSpellValues values;
                        values.AddSpellMod(SPELLVALUE_DURATION, 8000);
                        cre->CastCustomSpell(6615, values, cre, TRIGGERED_FULL_MASK);
                    }
            }
            context.Repeat();
        });
    }
    //player->RemoveKeystone(); // we shouldnt remove keystone with new system

    if (HasAffix(AFFIX_THUNDERING))
        if (isAffixEnabled(AFFIX_THUNDERING))
        {
            GetScheduler().Schedule(Milliseconds(11000 + AFFIX_THUNDERING_TIMER), [this](TaskContext context)
                {
                    uint64 guid{ 0 };
                    Map::PlayerList const& PlayerList = instance->GetPlayers();

                    if (!PlayerList.isEmpty())
                        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (!guid || urand(0, 1))
                                    guid = player->GetGUID();


                    if (!PlayerList.isEmpty())
                        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                            if (auto player = itr->getSource())
                                if (player->GetGUID() == guid)
                                {
                                    /*
                                        We now have the final decided player to center the storm over.
                                    */
                                    Position thundering_starts[7]
                                    {
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                        {0.f, 0.f, 0.f, 0.f},
                                    };
                                    player->MoveBlink(thundering_starts[0], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[1], 15.f, (NORTH - player->GetOrientation()), false);//NORTH 1
                                    player->MovePositionToFirstCollision(thundering_starts[1], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[2], 30.f, (NORTH - player->GetOrientation()), false);//NORTH 2
                                    player->MovePositionToFirstCollision(thundering_starts[2], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[3], 45.f, (NORTH - player->GetOrientation()), false);//NORTH 3
                                    player->MovePositionToFirstCollision(thundering_starts[3], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[4], 15.f, (SOUTH - player->GetOrientation()), false);//SOUTH 1
                                    player->MovePositionToFirstCollision(thundering_starts[4], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[5], 30.f, (SOUTH - player->GetOrientation()), false);//SOUTH 2
                                    player->MovePositionToFirstCollision(thundering_starts[5], 60.f, (EAST - player->GetOrientation()));

                                    player->MovePosition(thundering_starts[6], 45.f, (SOUTH - player->GetOrientation()), false);//SOUTH 3
                                    player->MovePositionToFirstCollision(thundering_starts[6], 60.f, (EAST - player->GetOrientation()));

                                    for (auto i = 0; i <= 6; ++i)
                                        instance->SummonCreature(AFFIX_NPC_THUNDERING_SPAWNER, thundering_starts[i]);

                                    instance->SummonCreature(AFFIX_NPC_THUNDERING_DESPAWNER, *player);
                                }

                    context.Repeat(Milliseconds(AFFIX_THUNDERING_TIMER));


                });
        }


    MapEntry const* mapEntry = sMapStore.LookupEntry(player->GetKeystone().mapId);
    Keystone this_keystone = player->GetKeystone();

    SetInstanceKeystone(this_keystone, mapEntry);


    Keystone instance_keystone = GetKeystone();
    std::ostringstream ostream;
    std::ostringstream ostream2;

    ostream << "Keystone Started:\nDifficulty level: +" << uint32(instance_keystone.level) << "\nDungeon: " << mapEntry->name;

    bool has_affixes{ false };

        for (uint8 i = 0; i < MAX_KEYSTONE_AFFIXES; ++i)
            if (instance_keystone.affixes[i] != 0)
            {
                has_affixes = true;

                auto affix_data = sWorld->GetAffixDetails(ChallengeModeAffix(keystone.affixes[i]));

                std::string menu_option{ ChallengeModeMgr::GenerateAfficon(20, affix_data->affix_icon) + affix_data->affix_name };

                ostream2 << "\nAffix " << i + 1 << ": " << menu_option;
            }

    std::string fullmsg = ostream.str();

    if (auto me = player->FindNearestCreature(90000, 100.f))
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (auto player_nearby = itr->getSource())
                if (auto session = player_nearby->GetSession())
                {
                    auto loc_idx                    { session->GetSessionDbLocaleIndex() };
                    WorldPacket data                (SMSG_MESSAGECHAT, 200);
                    std::stringstream Affix_report  { "" };
                    Affix_report                    << "Keystone Affixes:";
                    Affix_report                    << ((has_affixes) ? ostream2.str().c_str() : "\nnone");
                    auto text_pre1                  { Affix_report.str() };
                    auto text                       { text_pre1.c_str() };
                    me->BuildMonsterChat(&data, CHAT_MSG_RAID_BOSS_EMOTE, text, LANG_UNIVERSAL, me->GetNameForLocaleIdx(loc_idx), player_nearby->GetGUID());
                    ChatHandler(player_nearby->GetSession()).PSendSysMessage("%s", fullmsg.c_str());
                    session->SendPacket(&data);
                }
}

uint32 InstanceScript::GetDeadBossTotal(bool SequenceOnly)
{
    //TC_LOG_ERROR("sql.sql", "InstanceScript::GetDeadBossTotal initiated");
    uint32 totalDead = 0;
    if (bosses.size())
    {
        //TC_LOG_ERROR("sql.sql", "bosses has at least one member.");
        for (std::vector<BossInfo>::const_iterator itr = bosses.begin(); itr != bosses.end(); ++itr)
        {
            //TC_LOG_ERROR("sql.sql", "InstanceScript::GetDeadBossTotal iteration");
            if (itr->state == DONE || itr->state == DONE_HM)
            {
                //TC_LOG_ERROR("sql.sql", "Boss states: %u, %u, %u, %u", int32(GetBossState(0)), int32(GetBossState(1)), int32(GetBossState(2)), int32(GetBossState(3)));
                totalDead++;
            }
            else if (SequenceOnly)
            {
                return totalDead;
            }
            //TC_LOG_ERROR("sql.sql", "Boss states: %u, %u, %u, %u", int32(GetBossState(0)), int32(GetBossState(1)), int32(GetBossState(2)), int32(GetBossState(3)));
        }

    }
        return totalDead;
}
bool InstanceScript::isAffixEnabled(ChallengeModeAffix affix)
{
    if (!IsChallengeModeInProgress())
        return false;

    switch (affix)
    {
    case AFFIX_ENTANGLING:
    case AFFIX_QUAKING:
    case AFFIX_GRIEVOUS:
    case AFFIX_EXPLOSIVE:
    case AFFIX_VOLCANIC:
    case AFFIX_THUNDERING:
    case AFFIX_AFFLICTED:
    case AFFIX_INCORPOREAL:
        if ((instance->GetId() == 33) && (GetBossState(0) == IN_PROGRESS))     //Baron ashbury in progress
            return false;
        if (!IsChallengeModeStarted())
            return false;
        return true;
        break;
    case AFFIX_SANGUINE:
    case AFFIX_NECROTIC:
    case AFFIX_OVERFLOWING:
    case AFFIX_BURSTING:
    default:
        if (!IsChallengeModeStarted())
            return false;
        return true;
        break;
    }
    return true;
}

void InstanceScript::CompleteChallengeMode()
{
    if (!IsChallengeModeStarted())
        return;

    SaveMythicCompletionToDB();

    bool alerted{ false };
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (!alerted)
            if (auto player = itr->getSource())
            {
                player->GetSession()->SendAreaTriggerMessage("Mythic Challenge Completed!");
                alerted = true;
                break;
            }

    DoOnPlayers([this](Player* player)
    {
        sChallengeModeMgr->Reward(player, GetKeystone().level);
    });
    _challengeModeFinished = true;
    _challengeModeStarted = false;

    DoOnCreatures([this](Creature* c) 
    {
        c->DespawnOrUnsummon(urand(2000, 10000));
    });

}

uint32 InstanceScript::GetChallengeModeRemainingTime() const
{
    uint32 timeDiff = GetMSTimeDiffToNow(_challengeModeStartTime) / 1000;

    return sChallengeModeMgr->GetTimerForInstance(instance->GetId(), GetKeystone().level) - timeDiff - (5 * _challengeModeDeathCount);
}

void InstanceScript::SendChallengeModeHint(Player* player)
{
    if (player)
        if (auto s = player->GetSession())
        {
            std::ostringstream ostream;
            ostream << "Challenge Hint:";
            uint32 entry{ 0 };
            Creature* target{ nullptr };
            for (auto it = _challengeModeMobsToKill.begin(); it != _challengeModeMobsToKill.end(); ++it)
                if (_challengeModeMobsStatus[it->first] > 0)
                {
                    if (target)
                    {
                        if (auto c = player->FindNearestCreature(it->first, 1000.f, true))
                        {
                            if (player->GetExactDist(c) < player->GetExactDist(target))
                            {
                                entry = it->first;
                                target = c;
                            }
                        }
                    }
                    else
                    {
                        entry = it->first;
                        if (auto c = player->FindNearestCreature(it->first, 1000.f, true))
                            target = c;
                    }
                }

            if (entry > 0)
            {
                if (auto t = sObjectMgr->GetCreatureTemplate(entry))
                {
                    ostream << "\n" << t->Name << " is the closest living credit target.";
                    std::string fullmsg = ostream.str();
                    ChatHandler(s).PSendSysMessage("%s", fullmsg.c_str());
                }

                if (auto c = target)
                if (auto poi = sObjectMgr->GetPointOfInterest(19))
                if (auto t = c->GetCreatureTemplate())
                {
                    std::string iconText = c->GetName();
                    WorldPacket data(SMSG_GOSSIP_POI, 4 + 4 + 4 + 4 + 4 + 10);  // guess size
                    data << uint32(poi->flags);
                    data << float(c->GetPositionX());
                    data << float(c->GetPositionY());
                    data << uint32(poi->icon);
                    data << uint32(poi->data);
                    data << iconText;
                    s->SendPacket(&data);


                    WorldPacket data2(MSG_MINIMAP_PING, (8 + 4 + 4));
                    data2 << uint64(player->GetGUID());
                    data2 << float(c->GetPositionX());
                    data2 << float(c->GetPositionY());
                    s->SendPacket(&data2);
                }
            }
        }
}

void InstanceScript::SaveMythicCompletionToDB()
{

    uint32 const completion_time = GetMSTimeDiffToNow(_challengeModeStartTime);
    Keystone instance_keystone = GetKeystone();
    PreparedStatement* stmt = nullptr;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MYTHIC_HISCORES);

    //stmt->setUInt64(X, DATEANDTIME);                                                                  //date and time set automatically
    stmt->setUInt32(0, instance->GetId());                                                              //instance ID
    stmt->setUInt32(1, completion_time);                                                                //time elapsed, milliseconds
    stmt->setUInt8(2, GetChallengeModeLevel());                                                         //mythic keystone level

    int8 query_index{ 3 };//party member index 0
        for (uint8 i = 0; i < MAX_KEYSTONE_AFFIXES; ++i)
        {
            stmt->setUInt8(query_index, instance_keystone.affixes[i]);                                  //mythic keystone affixes
            query_index++;
        }
 
        for (uint32 i = query_index; i < 22; ++i)
            stmt->setUInt32(i, 0);

    query_index = 7;//party member index 0
    Map::PlayerList const& PlayerList = instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (auto itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (auto player = itr->getSource())
            if (!player->isGameMaster())
            {
                stmt->setUInt64(query_index, player->GetGUID());                                        //Party member GUIDs
                query_index++;

                if (player->GetRole(false) == lfg::PLAYER_ROLE_TANK 
                && player->GetPrimaryTalentTree(player->GetActiveSpec()) == TALENT_TREE_DRUID_FERAL_COMBAT)
                    stmt->setUInt32(query_index, player->GetPrimaryTalentTree(player->GetActiveSpec())+1);    //BEAR TANK EXCLUSIVE SET TO 751
                else
                    stmt->setUInt32(query_index, player->GetPrimaryTalentTree(player->GetActiveSpec()));    //Party member spec

                query_index++;
                stmt->setUInt64(query_index, player->GetRole(false));                                   //Party member role
                query_index++;
            }

    CharacterDatabase.Execute(stmt);
}