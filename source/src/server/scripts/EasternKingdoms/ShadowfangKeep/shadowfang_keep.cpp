 /*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Shadowfang_Keep
SD%Complete: 0%
SDComment:
SDCategory: Shadowfang Keep
EndScriptData */

/* ContentData
npc_shadowfang_prisoner
EndContentData */

#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "shadowfang_keep.h"

class npc_haunted_stable_hand : public CreatureScript
{
public:
    npc_haunted_stable_hand() : CreatureScript("npc_haunted_stable_hand") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 Sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (Sender != GOSSIP_SENDER_MAIN)
            return true;
        if (!player->getAttackers().empty())
            return true;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->TeleportTo(33, -225.70f, 2269.67f, 74.999f, 2.76f);
                player->CLOSE_GOSSIP_MENU();
            break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->TeleportTo(33, -260.66f, 2246.97f, 100.89f, 2.43f);
                player->CLOSE_GOSSIP_MENU();
            break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->TeleportTo(33, -171.28f, 2182.020f, 129.255f, 5.92f);
                player->CLOSE_GOSSIP_MENU();
            break;
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        InstanceScript* instance = creature->GetInstanceScript();

        if (instance && instance->GetData(DATA_BARON_SILVERLAINE_EVENT)==DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleportiert mich zu Baron Silberlein", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        if (instance && instance->GetData(DATA_COMMANDER_SPRINGVALE_EVENT)==DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleportiert mich zu Kommandant Gruental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        if (instance && instance->GetData(DATA_LORD_WALDEN_EVENT)==DONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleportiert mich zu Lord Walden", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

        player->SEND_GOSSIP_MENU(2475, creature->GetGUID());
        return true;
    }
};

// Spitebone Guardian
#define SPELL_SHIELD_BASH       11972
#define SPELL_STRIKE            87081
#define SPELL_SHIELD_OF_BONES   91629

class mob_spitebone_guardian : public CreatureScript
{
public:
    mob_spitebone_guardian() : CreatureScript("mob_spitebone_guardian") {}
 
       struct mob_spitebone_guardianAI : public ScriptedAI
       {
           mob_spitebone_guardianAI(Creature *c): ScriptedAI(c) {}
	   
	   uint32 ShieldBash_Timer;
	   uint32 Strike_Timer;

           bool Bones;

           void Reset()
           {
               ShieldBash_Timer = 9000;
	       Strike_Timer = 6000;
	   
	       Bones = false;
	   }

           void UpdateAI(const uint32 diff)
           {
               if (!UpdateVictim())
               return;
	       
	       if(ShieldBash_Timer<= diff)
               {
                     DoCast(me->getVictim(), SPELL_SHIELD_BASH);
                     ShieldBash_Timer = 10000;
	       }else ShieldBash_Timer -= diff;
	       
	       if(Strike_Timer<= diff)
               {
                     DoCast(me->getVictim(), SPELL_STRIKE);
                     Strike_Timer = 5000;
	       }else Strike_Timer -= diff;
	       
	       if (!Bones && !HealthAbovePct(30))
	       {
	          if (IsHeroic())
                  {
                     DoCast(me, SPELL_SHIELD_OF_BONES);
					 
		     Bones = true;
			 if (!me->HasAura(SPELL_SHIELD_OF_BONES))
				 DoCastAOE(91631);
		  }
		  
	        }
		  
	      DoMeleeAttackIfReady();
           }
       };
	
CreatureAI* GetAI(Creature *pCreature) const
{
        return new mob_spitebone_guardianAI(pCreature);
}
};

// Fetid Ghoul
#define SPELL_DISEASE_CLOUD     91561
#define SPELL_FETID_CLOUD       91554

class mob_fetid_ghoul : public CreatureScript
{
public:
    mob_fetid_ghoul() : CreatureScript("mob_fetid_ghoul") {}

    struct mob_fetid_ghoulAI : public ScriptedAI
    {
        mob_fetid_ghoulAI(Creature* c) : ScriptedAI(c) {}

        void JustDied(Unit* /*Killer*/)
        {
            if (IsHeroic())
            {
                DoCast(SPELL_FETID_CLOUD);
            }
        }

        void Reset()
        {
            DoCast(me, SPELL_DISEASE_CLOUD);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();

        }

    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new mob_fetid_ghoulAI(pCreature);
    }
};

class npc_horde_shadowfang : public CreatureScript
{
public:
    npc_horde_shadowfang() : CreatureScript("npc_horde_shadowfang") {}

    struct npc_horde_shadowfangAI : public ScriptedAI
    {
        npc_horde_shadowfangAI(Creature* c) : ScriptedAI(c)
        {
            me->SetVisible(false);

        }

        void UpdateAI(const uint32 diff)
        {

            if (auto player = me->FindNearestPlayer(100.0f))
                if (player->GetTeam() == HORDE)
                    me->SetVisible(true);

        }

    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_horde_shadowfangAI(pCreature);
    }
};

class npc_alliance_shadowfang : public CreatureScript
{
public:
    npc_alliance_shadowfang() : CreatureScript("npc_alliance_shadowfang") {}

    struct npc_alliance_shadowfangAI : public ScriptedAI
    {
        npc_alliance_shadowfangAI(Creature* c) : ScriptedAI(c)
        {
            me->SetVisible(false);

        }

        void UpdateAI(const uint32 diff)
        {

            if (auto player = me->FindNearestPlayer(100.0f))
                if (player->GetTeam() == ALLIANCE)
                    me->SetVisible(true);

        }

    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_alliance_shadowfangAI(pCreature);
    }
};

class spell_haunting_spirits : public SpellScriptLoader
{
public:
    spell_haunting_spirits() : SpellScriptLoader("spell_haunting_spirits") { }

    class spell_haunting_spirits_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_haunting_spirits_AuraScript);

        bool Load()
        {
            events.ScheduleEvent(1, 0);
            return true;
        }

        void OnUpdate(AuraEffect* aurEff, const uint32 diff)
        {
            events.Update(diff);
            
            if (events.ExecuteEvent() == 1)
            {
                if (urand(1, 100) <= 30)
                {
                    if (auto servitor = GetCaster())
                        if (WorldObject* owner = GetOwner())
                        {
                            Position pos;
                            owner->GetPosition(&pos);
                            servitor->SummonCreature(4958, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        }
                }
                events.ScheduleEvent(1, 5000);
            }      
        }

        void Register()
        {
            OnEffectUpdate += AuraEffectUpdateFn(spell_haunting_spirits_AuraScript::OnUpdate, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_haunting_spirits_AuraScript();
    }
};




class spell_mindless_horror_fear : public SpellScriptLoader
{
public:
    spell_mindless_horror_fear() : SpellScriptLoader("spell_mindless_horror_fear") { }

    class spell_mindless_horror_fear_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_mindless_horror_fear_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            for (auto itr = targets.begin(); itr != targets.end();)
                if (auto target = (*itr))
                    if (auto unit = target->ToUnit())
                        if (auto aura = unit->GetAura(6346))
                        {
                            aura->Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                            targets.erase(itr++);
                        }
                        else
                            ++itr;
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mindless_horror_fear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mindless_horror_fear_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mindless_horror_fear_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };
    SpellScript* GetSpellScript() const
    {
        return new spell_mindless_horror_fear_SpellScript();
    }
};

void AddSC_shadowfang_keep()
{
   new npc_haunted_stable_hand();
   new mob_spitebone_guardian();
   new mob_fetid_ghoul();
   new npc_horde_shadowfang();
   new npc_alliance_shadowfang();
   new spell_haunting_spirits();
   new spell_mindless_horror_fear();
}