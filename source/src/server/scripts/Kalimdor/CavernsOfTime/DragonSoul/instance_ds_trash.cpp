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
#include "dragonsoul.h"

enum Spells
{
    SPELL_BOULDER_SMASH = 107597,
    SPELL_DUST_STORM = 107682,
    SPELL_SHADOW_BOLT = 95440,

    SPELL_DRENCHED = 107801,
    SPELL_FLOOD = 107797,

    SPELL_TWILIGHT_SUBMISSION = 108183,
    SPELL_TWILIGHT_VOLLEY = 108172,

    SPELL_TWILIGHT_CORRUPTION = 107852,
    SPELL_TWILIGHT_RAGE = 107872,
    
    SPELL_SHADOW_GAZE = 109391,

    SPELL_SLUDGE_SPEW = 110102,
    SPELL_WILD_FLAIL = 109199,
};

enum Events
{
    EVENT_BOULDER_SMASH         = 1,
    EVENT_DUST_STORM            = 2,
    EVENT_SHADOW_BOLT           = 3,

    EVENT_DRENCHED              = 4,
    EVENT_FLOOD                 = 5,

    EVENT_TWILIGHT_SUBMISSION   = 6,
    EVENT_TWILIGHT_VOLLEY       = 7,

    EVENT_TWILIGHT_CORRUPTION   = 8,
    EVENT_TWILIGHT_RAGE         = 9,

    EVENT_SHADOW_GAZE           = 10,

    EVENT_SLUDGE_SPEW           = 11,
    EVENT_WILD_FLAIL            = 12,

};

class npc_eiendormi : public CreatureScript
{
public:
	npc_eiendormi() : CreatureScript("npc_eiendormi") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (auto instance = creature->GetInstanceScript())
		{
			if (player->isInCombat())
			{
				creature->MonsterWhisper("You are in combat!", player->GetGUID());
				return false;
			}

            if (instance->GetBossState(DATA_MORCHOK) == DONE)
            {
                creature->CastSpell(creature, SPELL_WARDEN_RING_YELLOW);

                if (auto t = player->SummonCreature(57474, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
                    player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                    player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, true);
            }
        }

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}
};

class npc_valeera_tele : public CreatureScript
{
public:
	npc_valeera_tele() : CreatureScript("npc_valeera_tele") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (auto instance = creature->GetInstanceScript())
		{

			if (player->isInCombat())
			{
				creature->MonsterWhisper("You are in combat!", player->GetGUID());
				return false;
			}

            if (instance->GetBossState(DATA_MORCHOK) == DONE)
            {
                creature->CastSpell(creature, SPELL_WARDEN_RING_GREEN);

                if (auto t = player->SummonCreature(57475, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
                    player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                    player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, true);
            }
		}

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}
};

class npc_nethestrasz : public CreatureScript
{
public:
	npc_nethestrasz() : CreatureScript("npc_nethestrasz") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (auto instance = creature->GetInstanceScript())
		{

			if (player->isInCombat())
			{
				creature->MonsterWhisper("You are in combat!", player->GetGUID());
				return false;
			}

            if (instance->GetBossState(DATA_YORSAHJ) == DONE && instance->GetBossState(DATA_WARLORD_ZONOZZ == DONE))
            {
                if (auto t = player->SummonCreature(57473, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(),
                    player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                    player->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, 1, t, true);
            }
		}

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}
};

// 57158
class npc_ds_earthen_destroyer : public CreatureScript
{
public:
    npc_ds_earthen_destroyer() : CreatureScript("npc_ds_earthen_destroyer") {}

    struct npc_ds_earthen_destroyerAI : public ScriptedAI
    {
        npc_ds_earthen_destroyerAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_BOULDER_SMASH, 5000);
            events.ScheduleEvent(EVENT_DUST_STORM, 15000);
            events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BOULDER_SMASH:
                        DoCastRandom(SPELL_BOULDER_SMASH, 90.0f);
                        events.ScheduleEvent(EVENT_BOULDER_SMASH, urand(10000, 12000));
                        break;
                    case EVENT_DUST_STORM:
                        DoCast(SPELL_DUST_STORM);
                        events.ScheduleEvent(EVENT_DUST_STORM, urand(10000, 15000));
                        break;
                    case EVENT_SHADOW_BOLT:
                        DoCastRandom(SPELL_SHADOW_BOLT, 90.0f);
                        events.ScheduleEvent(EVENT_BOULDER_SMASH, urand(5000, 10000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_earthen_destroyerAI(creature);
    }
};

// 57160
class npc_ds_ancient_water_lord : public CreatureScript
{
public:
    npc_ds_ancient_water_lord() : CreatureScript("npc_ds_ancient_water_lord") {}

    struct npc_ds_ancient_water_lordAI : public ScriptedAI
    {
        npc_ds_ancient_water_lordAI(Creature* creature) : ScriptedAI(creature) { }

        void JustDied(Unit* killer) override
        {
            DoCast(me, 107780, true);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == 57206)
                summon->CastWithDelay(1250, summon, 107751, true, true);
        }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_DRENCHED, 5000);
            events.ScheduleEvent(EVENT_FLOOD, 15000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_DRENCHED:
                    {
                        auto list_of_players = me->GetPlayersInRange(90.f, true);

                        if (list_of_players.size())
                        if (auto random_target = Trinity::Containers::SelectRandomContainerElement(list_of_players))
                            me->CastSpell(random_target, SPELL_DRENCHED);
                        events.ScheduleEvent(EVENT_DRENCHED, urand(5000, 8000));
                    }
                        break;
                    case EVENT_FLOOD:
                        DoCastRandom(SPELL_FLOOD, 90.0f);
                        events.ScheduleEvent(EVENT_FLOOD, urand(10000, 15000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_ancient_water_lordAI(creature);
    }
};

// 57280
class npc_ds_twilight_siege_captain : public CreatureScript
{
public:
    npc_ds_twilight_siege_captain() : CreatureScript("npc_ds_twilight_siege_captain") {}

    struct npc_ds_twilight_siege_captainAI : public ScriptedAI
    {
        npc_ds_twilight_siege_captainAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_TWILIGHT_SUBMISSION, 2000);
            events.ScheduleEvent(EVENT_TWILIGHT_VOLLEY, 5000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_TWILIGHT_SUBMISSION:
                        DoCastRandom(SPELL_TWILIGHT_SUBMISSION, 100.0f);
                        events.ScheduleEvent(EVENT_TWILIGHT_SUBMISSION, urand(6000, 8000));
                        break;
                    case EVENT_TWILIGHT_VOLLEY:
                        DoCastRandom(SPELL_TWILIGHT_VOLLEY, 35.0f);
                        events.ScheduleEvent(EVENT_TWILIGHT_VOLLEY, urand(10000, 12000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_twilight_siege_captainAI(creature);
    }
};

// UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` IN(57231,57295);

// 57159
class npc_ds_earthen_solider : public CreatureScript
{
public:
    npc_ds_earthen_solider() : CreatureScript("npc_ds_earthen_solider") {}

    struct npc_ds_earthen_soliderAI : public ScriptedAI
    {
        npc_ds_earthen_soliderAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_SHADOW_BOLT, 5000);
            events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 8000);
            events.ScheduleEvent(EVENT_TWILIGHT_RAGE, 25000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOW_BOLT:
                        DoCastRandom(SPELL_SHADOW_BOLT, 90.0f);
                        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(3000, 5000));
                        break;
                    case EVENT_TWILIGHT_CORRUPTION:
                        DoCast(SPELL_TWILIGHT_CORRUPTION);
                        events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, urand(6000, 8000));
                        break;
                    case EVENT_TWILIGHT_RAGE:
                        DoCast(SPELL_TWILIGHT_RAGE);
                        events.ScheduleEvent(EVENT_TWILIGHT_RAGE, 24000);
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_earthen_soliderAI(creature);
    }
};

// 57875
class npc_ds_eye_of_gorath_trash : public CreatureScript
{
public:
    npc_ds_eye_of_gorath_trash() : CreatureScript("npc_ds_eye_of_gorath_trash") {}

    struct npc_ds_eye_of_gorath_trashAI : public Scripted_NoMovementAI
    {
        npc_ds_eye_of_gorath_trashAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_SHADOW_GAZE, 3000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHADOW_GAZE:
                        DoCastRandom(SPELL_SHADOW_GAZE, 100.0f);
                        events.ScheduleEvent(EVENT_SHADOW_GAZE, urand(5000, 8000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_eye_of_gorath_trashAI(creature);
    }
};

// 57877
class npc_ds_flail_of_gorath_trash : public CreatureScript
{
public:
    npc_ds_flail_of_gorath_trash() : CreatureScript("npc_ds_flail_of_gorath_trash") {}

    struct npc_ds_flail_of_gorath_trashAI : public Scripted_NoMovementAI
    {
        npc_ds_flail_of_gorath_trashAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        void EnterCombat(Unit* victim) override
        {
            events.ScheduleEvent(EVENT_SLUDGE_SPEW, 2000);
            events.ScheduleEvent(EVENT_WILD_FLAIL, 5000);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SLUDGE_SPEW:
                        DoCastRandom(SPELL_SLUDGE_SPEW, 200.0f);
                        events.ScheduleEvent(EVENT_SLUDGE_SPEW, urand(10000, 12000));
                        break;
                    case EVENT_WILD_FLAIL:
                        DoCast(SPELL_WILD_FLAIL);
                        events.ScheduleEvent(EVENT_WILD_FLAIL, urand(6000, 8000));
                        break;
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_flail_of_gorath_trashAI(creature);
    }
};

enum Globules
{
    ACID                    = 57333,
    SHADOWED                = 57388,
    COBALT                  = 57384,
    GLOWING                 = 57387,
    CRIMSON                 = 57386,
    DARK                    = 57382,

    SPELL_MANA_DIFFUSION    = 108228,
    SPELL_GLOWING_BLOOD     = 108221,

    EVENT_1                 = 1,
    EVENT_2                 = 2,
};

// 57333 57388 57384 57387 57386 57382
class npc_ds_globule_trash : public CreatureScript
{
public:
    npc_ds_globule_trash() : CreatureScript("npc_ds_globule_trash") {}

    struct npc_ds_globule_trashAI : public ScriptedAI
    {
        npc_ds_globule_trashAI(Creature* creature) : ScriptedAI(creature) { }

        void EnterCombat(Unit* victim) override
        {

            switch (me->GetEntry())
            {
                case COBALT:
                //case GLOWING: temp disabled because the effect triggers from npc to npc -> every creature at this map get this buff it is impossible to remove this aura
                    DoCast(me->GetEntry() == COBALT ? 108223 : 108221);
                    break;
                case DARK:
                    events.ScheduleEvent(EVENT_1, 2000);
                    break;
                case ACID:
                case SHADOWED:
                case CRIMSON:
                    events.ScheduleEvent(EVENT_2, urand(2000, 6000));
                    break;
                default:
                    break;
            }
        }

        void JustDied(Unit* killer) override
        {
            if (me->GetEntry() == COBALT)
                DoCast(SPELL_MANA_DIFFUSION);
        }

        void UpdateAI(uint32 const diff) override
        {
            if (!me->IsVisible() || !UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_1:
                        DoCastRandom(108922, 20.0f);
                        if (me->HasAura(SPELL_GLOWING_BLOOD))
                            events.ScheduleEvent(EVENT_1, urand(1500, 3000));
                        else
                            events.ScheduleEvent(EVENT_1, urand(3000, 6000));
                        break;
                    case EVENT_2:
                    {
                        uint32 spellId = 0;
                        switch (me->GetEntry())
                        {
                            case ACID:
                                spellId = 108419;
                                break;
                            case SHADOWED:
                                spellId = 108220;
                                break;
                            case CRIMSON:
                                spellId = 108218;
                            default:
                                break;
                        }
                        DoCastRandom(spellId, 40.0f);
                        if (me->HasAura(SPELL_GLOWING_BLOOD))
                            events.ScheduleEvent(EVENT_2, urand(5000, 7500));
                        else
                            events.ScheduleEvent(EVENT_2, urand(10000, 15000));
                        break;
                    }
                    default:
                        break;
                }
            }
            DoMeleeAttackIfReady();
        }

    private:
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_globule_trashAI(creature);
    }
};

enum LordMisc
{
    TALK_WE_WILL_SEE_CLASS  = -56587,
    MENU_ID_RAID_NERF       = 13357,
    TALK_DEFENSE            = 0,
    TALK_ADVANCING          = 1,
    TALK_SET_UP_INSIDE      = 2,
};

class npc_ds_lord_afrasastrasz : public CreatureScript
{
public:
    npc_ds_lord_afrasastrasz() : CreatureScript("npc_ds_lord_afrasastrasz") { }


    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->getClass() == CLASS_ROGUE)
        {
            player->PrepareQuestMenu(creature->GetGUID()); 
            //TC_LOG_ERROR("sql.sql", "rogue is true, sending quest menu.");
        }

        if (canGossip(creature))
        {
            //TC_LOG_ERROR("sql.sql", "cangossip is true, sending menu and options.");
            auto text_line{ "I humbly request that the Dragon Aspects provide no aid to myself and my fellow adventurers. They should worry about their own problems, and we will worry about ours." };
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, text_line, GOSSIP_SENDER_MAIN, MENU_ID_RAID_NERF);
        }
        //else
            //TC_LOG_ERROR("sql.sql", "cangossip is false, bypassing.");

        
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool canGossip(Creature* creature)
    {
        if (auto instance{ creature->GetInstanceScript() })
        {
            //TC_LOG_ERROR("sql.sql", "DATA_RAID_NERF = %u", instance->GetData(DATA_RAID_NERF));
            return (instance->GetData(DATA_RAID_NERF) > 0);
        }
        else
        {
            //TC_LOG_ERROR("sql.sql", "cangossip returning false, instance not found.");
            return false;
        }
    }

    struct npc_ds_lord_afrasastraszAI : public ScriptedAI
    {
        npc_ds_lord_afrasastraszAI(Creature* creature) : ScriptedAI(creature) { }



        void sGossipSelect(Player* player, uint32 menuId, uint32 /*gossipId*/) override
        {
            if (menuId == MENU_ID_RAID_NERF)
            {
                Talk(TALK_WE_WILL_SEE_CLASS, player->GetGUID());
                if (auto instance = me->GetInstanceScript())
                    instance->SetData(DATA_RAID_NERF, RAID_NERF_INACTIVE);
                player->PlayerTalkClass->SendCloseGossip();
            }
        }


        void JustSummoned(Creature* summon) override
        {
            switch (summon->GetEntry())
            {
            case NPC_TYRYGOSA_IMAGE:
                summon->SetFacingTo(summon->GetAngle(me));
                summon->AddAura(108841, summon);
                summon->CastSpell(summon, 51347, true);
                events.ScheduleEvent(100, 2000);
                events.ScheduleEvent(101, 15000);
                break;
            default:
                break;
            }
        }

        void MoveInLineOfSight(Unit* mover) override
        {
            if (me->GetDBTableGUIDLow() != 255819)
                return;
            if (instance->IsDone(DATA_MORCHOK))
                return;

            if (!tyry_summoned)
            if (me->GetDistance(mover) < 50.f)
                if ((mover->GetTypeId() == TYPEID_PLAYER))
                    {
                            me->TalkWithDelay(6000, TALK_DEFENSE, NULL, CHAT_MSG_MONSTER_YELL);
                            tyry_summoned = true;
                            me->CastWithDelay(13000, me, 108924, true);
                    }
        }
        void DoAction(int32 const action)
        {
            if (action == NPC_MORCHOK)
            {
                me->TalkWithDelay(5000, TALK_ADVANCING, NULL, CHAT_MSG_MONSTER_YELL);
                me->TalkWithDelay(22000, TALK_SET_UP_INSIDE, NULL, CHAT_MSG_MONSTER_YELL);
                events.ScheduleEvent(500, 30000);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {

            if (type == POINT_MOTION_TYPE)
                if (id == 100)
                {
                    events.ScheduleEvent(105, 200);
                }
            /*
            Position waypoint{ *me };
            waypoint.m_positionZ += 1.f;
            me->NearTeleportTo(waypoint);
            */
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventid = events.ExecuteEvent())
                switch (eventid)
                {
                case 100:
                {
                    if (auto c = me->FindNearestCreature(NPC_TYRYGOSA_IMAGE, 25.f, true))
                    {
                        /*
                            NPC empty-talk bug, must bee done via monsteryell or the text appears empty if called from the DB.
                        */
                        c->MonsterYell("They have broken our defenses... the very earth turns against us in Deathwing's name.", LANG_UNIVERSAL, 0);
                        c->PlayDirectSound(26305);
                    }
                    break;
                }
                case 101:
                {
                    if (auto c = me->FindNearestCreature(NPC_TYRYGOSA_IMAGE, 25.f, true))
                    {
                        /*
                            NPC empty-talk bug, must bee done via monsteryell or the text appears empty if called from the DB.
                        */
                        c->MonsterYell("You must hurry... Wyrmrest falls as we speak... all... is lost...", LANG_UNIVERSAL, 0);
                        c->PlayDirectSound(26306);
                    }
                    break;
                }
                case 105:
                {
                    Position next{ -1913.145874f, -2402.645264f, 54.249599f, 0.f };
                    me->GetMotionMaster()->MovePoint(101, next, true, 10.f);
                    break;
                }
                case 500:
                    instance->SetData(DATA_AFRASABI_DELAYED_PHASING, 1);
                    break;
                }
        }

    private:
        bool tyry_summoned{ false };
        EventMap events;
        InstanceScript* instance{ me->GetInstanceScript() };
    };


    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_ds_lord_afrasastraszAI(creature);
    }
};

class npc_ds_twilight_portal : public CreatureScript
{
public:
    npc_ds_twilight_portal() : CreatureScript("npc_ds_twilight_portal") {}

    struct npc_ds_twilight_portalAI : public ScriptedAI
    {
        npc_ds_twilight_portalAI(Creature* creature) : ScriptedAI(creature) 
        { 
            me->SetVisible(true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetReactState(REACT_PASSIVE);
            me->AddUnitState(UNIT_STATE_ROOT);
            me->AddUnitState(UNIT_STATE_CANNOT_TURN);
            events.ScheduleEvent(5, 1000);
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            damage = 0;
        }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 5:
                {
                    Position pos(*me);
                    pos.m_positionZ += 8.f;
                    auto trigger = me->FindNearestCreature(144951, 10.f, true);
                    if (!trigger)
                        trigger = me->SummonCreature(144951, pos);

                    if (trigger)
                    {
                        //"me" is the npc executing this script. This argument specifies the centerpoint of the circle to check.
                        std::list<Creature*> list_of_npcs;
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_TWILIGHT_SIEGE_CAPTAIN, 25.f);
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_ANCIENT_WATER_LORD, 25.f);
                        GetCreatureListWithEntryInGrid(list_of_npcs, me, NPC_EARTHEN_DESTROYER, 25.f);

                        if (list_of_npcs.size())
                            for (auto itr = list_of_npcs.begin(); itr != list_of_npcs.end();)
                                if (auto t = (*itr))
                                    if (t->isDead())//filter condition
                                    {
                                        list_of_npcs.erase(itr++);
                                    }
                                    else
                                    {
                                        ++itr;
                                    }

                        if (list_of_npcs.size())
                        {
                            if (!me->IsVisible())
                                me->SetVisible(true);

                            for (auto c : list_of_npcs)
                                if (!c->isInCombat())
                                {
                                    if (c->GetEntry() == NPC_TWILIGHT_SIEGE_CAPTAIN)
                                    {
                                        if (!c->HasUnitState(UNIT_STATE_CASTING))
                                            c->CastSpell(trigger, 108096);
                                    }
                                    else
                                    {
                                        if (!c->isMoving())
                                            c->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
                                    }
                                }
                        }
                        else
                            me->SetVisible(false);
                    }

                    events.ScheduleEvent(5, 5000);
                    break;
                }
                default:
                    break;
                }
        }
    private:
        EventMap events;
        InstanceScript* instance;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_twilight_portalAI(creature);
    }
};

class spell_waterlord_pure_water : public SpellScriptLoader
{
    /*
    * spell 107770 
    */
public:
    spell_waterlord_pure_water() : SpellScriptLoader("spell_waterlord_pure_water") { }

    class spell_waterlord_pure_water_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_waterlord_pure_water_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (auto caster = GetCaster())
                if (targets.size())
                    for (auto itr = targets.begin(); itr != targets.end(); ++itr)
                        if (auto this_itr = (*itr))
                            if (auto target = this_itr->ToUnit())
                                if (target == caster)
                                    targets.erase(itr);
        }

        void HandleHit(SpellEffIndex /*effIndex*/)
        {
            if (auto target = GetHitUnit())
                target->RemoveAurasDueToSpell(107677);
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_waterlord_pure_water_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_waterlord_pure_water_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            OnEffectHitTarget += SpellEffectFn(spell_waterlord_pure_water_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_waterlord_pure_water_SpellScript();
    }
};

const G3D::Vector3 DS_Drake_Gank_DeathPath[]
{
    {-2264.21f,	-2455.09f,	100.512f},
    {-2279.1f,	-2463.52f,	90.6844f},
    {-2291.12f,	-2470.7f,	85.3421f},
    {-2300.19f,	-2476.31f,	80.998f},
    {-2308.97f,	-2481.77f,	81.0592f},
    {-2317.87f,	-2487.32f,	80.8834f},
    {-2324.46f,	-2491.42f,	80.2523f},
    {-2330.41f,	-2495.13f,	79.7195f},
};
const G3D::Vector3 DS_Drake_StartingChase_path[]
{
{- 2421.19f, 	-2415.13f, 	97.3102f},
{- 2341.55f, 	-2382.08f, 	92.2915f},
{- 2274.19f, 	-2378.83f, 	91.3103f},
{- 2241.89f, 	-2363.87f, 	107.41f},
{- 2212.36f, 	-2366.36f, 	129.984f},
{- 2194.31f, 	-2381.65f, 	141.205f},
{- 2195.94f, 	-2404.29f, 	134.374f},
{- 2211.84f, 	-2411.55f, 	123.83f},
{- 2236.83f, 	-2422.98f, 	109.037f},
{- 2266.24f, 	-2447.41f, 	92.8972f},
{- 2291.47f, 	-2471.31f, 	94.9706f},
{- 2303.5f, 	-2499.69f, 	96.5083f},
{- 2286.33f, 	-2529.43f, 	96.3469f},
{- 2240.79f, 	-2549.47f, 	112.103f},
{- 2202.67f, 	-2538.81f, 	111.798f},
{- 2184.59f, 	-2518.91f, 	110.368f},
{- 2156.06f, 	-2467.17f, 	107.436f},
{- 2136.25f, 	-2423.44f, 	110.028f},
{- 2115.77f, 	-2372.21f, 	112.974f},
{- 2119.22f, 	-2344.04f, 	118.166f},
{- 2140.79f, 	-2320.55f, 	126.146f},
{- 2163.83f, 	-2316.04f, 	126.071f},
{- 2185.08f, 	-2331.63f, 	116.608f},
{- 2190.56f, 	-2358.24f, 	108.434f},
{- 2178.17f, 	-2384.37f, 	102.457f},
{- 2155.9f, 	-2402.58f, 	101.258f},
{- 2124.94f, 	-2419.95f, 	102.611f},
{- 2111.22f, 	-2426.59f, 	104.779f},
{- 2071.74f, 	-2445.91f, 	121.076f},
{- 2040.12f, 	-2459.47f, 	148.229f},
{- 2014.95f, 	-2457.58f, 	174.357f},
{- 2007.93f, 	-2435.01f, 	154.328f},
{- 2012.75f, 	-2423.16f, 	135.37f},
{- 2021.82f, 	-2390.83f, 	100.942f},
{- 2027.91f, 	-2358.08f, 	95.7101f},
{- 2023.49f, 	-2337.74f, 	90.6706f},
{- 2005.87f, 	-2327.33f, 	99.7973f},
{- 1984.99f, 	-2339.11f, 	99.8581f},
{- 1964.81f, 	-2364.05f, 	91.3294f},
{- 1945.48f, 	-2392.16f, 	91.567f},
{- 1914.35f, 	-2411.04f, 	94.5497f},
{- 1893.8f, 	-2406.75f, 	85.1332f},
{- 1878.38f, 	-2403.59f, 	87.5018f},
{- 1616.63f, 	-2380.19f, 	114.053f},
{- 1584.12f, 	-2377.72f, 	135.603f},
{- 1529.08f, 	-2366.11f, 	165.649f},
{- 1491.42f, 	-2426.52f, 	158.61f},
{- 1536.59f, 	-2438.84f, 	137.27f},
{- 1559.86f, 	-2432.77f, 	120.84f},
{- 1590.56f, 	-2417.85f, 	99.7372f},
{- 1616.24f, 	-2403.66f, 	88.0564f},
{- 1651.96f, 	-2394.31f, 	73.4329f},
{- 1692.f, 	    -2387.98f, 	68.8751f},
{- 1902.17f, 	-2404.89f, 	92.7494f},
{- 2092.82f, 	-2410.55f, 	160.966f},
{- 2232.68f, 	-2396.79f, 	170.141f},
{- 2284.22f, 	-2389.8f, 	167.327f},
{- 2428.79f, 	-2369.22f, 	109.783f},
};

class DS_drake_rp_trash : public CreatureScript
{
public:
    DS_drake_rp_trash() : CreatureScript("DS_drake_rp_trash") {}

    struct DS_drake_rp_trashAI : public ScriptedAI
    {
        DS_drake_rp_trashAI(Creature* creature) : ScriptedAI(creature)
        {
            me->SetReactState(REACT_DEFENSIVE);
            //DoAction(999);
            events.ScheduleEvent(404, 1000);
        }

        void EnterCombat(Unit* victim) override
        {

            if (victim->ToPlayer())
            {
                me->DeleteThreatList();
                EnterEvadeMode();
                return;
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            }

        }


        void DamageTaken(Unit* attacker, uint32& damage)
        {
            damage = 0;
            me->SetHealth(me->GetMaxHealth() * 0.9);
        }
        

        void MovementInform(uint32 type, uint32 id)
        {
            if (me->GetDBTableGUIDLow() == 256021)
                if (type == EFFECT_MOTION_TYPE)
                    if (id == 100)
                    if (acting)
                    {
                        acting = false;
                        events.ScheduleEvent(256022, 100);
                        events.ScheduleEvent(256023, 4000);
                    }
        }

        void Check_To_Crush_drakes()
        {
            if (!dying)
            if (me->GetDBTableGUIDLow() == 256021)
            {
                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 56144, 15.0f);
                if (list_of_npcs.size() == 2)
                {
                    dying = true;
                    for (auto c : list_of_npcs)
                        if (c->GetDBTableGUIDLow() == 256244 || c->GetDBTableGUIDLow() == 256245)
                            me->Kill(c, false, false);
                }
            }
        }

        void StartMovements()
        {
            switch (me->GetDBTableGUIDLow())
            {
                /*
                    Team Thrall
                */
                //list of drakes sitting on tower
            case 256219:
            case 256182:
            case 256016:
            case 256215:
            case 256201:
            case 256031:
            case 256194://end first floor
            case 256192://begin second floor
            case 256193:
            case 256029:
            case 256199:
            case 256200:
            case 256181:
            case 256180:
            case 256023:
            case 256214:
            {
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->SetHover(false);
                me->GetMotionMaster()->MoveFall();
                Position my_center{ -1786.390015f, -2393.530029f, me->GetPositionZ(), 0.f };
                me->SetFacingTo(me->GetAngle(&my_center) + M_PI);
                me->NearTeleportTo(*me);
                me->CastSpell(me, 87458, true);
                break;
            }
            case 256216:
                if (auto c = me->FindNearestCreature(55967, 75.f, true))
                {
                    me->GetMotionMaster()->MoveAroundPoint(*c, me->GetExactDist2d(c), urand(0, 1), 15, me->GetPositionZ(), true);
                    me->CastWithDelay(2000, c, 77508, true, true);
                }
                break;
            case 256032:
            {
                if (auto c = me->FindNearestCreature(55967, 100.f, true))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                }
                break;
            }
            case 256021:
            {
                break;
            }
            case 256027:
                break;
            case 256195:
                break;
            case 256205:
                break;
            case 256018:
            {
                if (auto c = me->FindNearestCreature(55967, 100.f, true))
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                    /*
                    Position crush_jump{ -1955.514648f, -2437.536621f, 111.827965f, 1.589635f };

                    //me->DisableMovementFlagUpdate(true);
                    me->SetHover(false);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
                    me->GetMotionMaster()->Clear();
                    */
                }
                break;
            }
            case 256197:
                if (auto c = me->FindNearestCreature(56143, 20.f, true))
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                    /*
                    Position crush_jump{ -1955.514648f, -2437.536621f, 111.827965f, 1.589635f };

                    //me->DisableMovementFlagUpdate(true);
                    me->SetHover(false);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
                    me->GetMotionMaster()->Clear();
                    */
                }
                break;
            case 256028:
                break;
            case 256017:
                if (auto c = me->FindNearestCreature(55969, 30.f, true))
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                    /*
                    Position crush_jump{ -1955.514648f, -2437.536621f, 111.827965f, 1.589635f };

                    //me->DisableMovementFlagUpdate(true);
                    me->SetHover(false);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
                    me->GetMotionMaster()->Clear();
                    */
                    break;
                }
                /*
                    Fly in a circle around the tower
                */
            case 256202:
            case 256185:
            {
                Position my_center{ -1786.390015f, -2393.530029f, me->GetPositionZ(), 0.f };
                if (me->GetExactDist(&my_center) > 135.f)
                {
                    me->SetSpeed(MOVE_FLIGHT, frand(3.f, 7.f));
                    me->GetMotionMaster()->MoveAroundPoint(my_center, me->GetExactDist(&my_center), bool(urand(0, 1)), 100, me->GetPositionZ(), true);

                    me->CastWithDelay(urand(5000, 12000), me, 101837, true, true);
                }
                break;
            }
            case 256022://blue drake vs harbinger of destruction
                if (auto c = me->FindNearestCreature(55967, 50.f, true))
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                    /*
                    Position crush_jump{ -1955.514648f, -2437.536621f, 111.827965f, 1.589635f };

                    //me->DisableMovementFlagUpdate(true);
                    me->SetHover(false);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
                    me->GetMotionMaster()->Clear();
                    */
                }
                break;
            case 256204:
                break;
            case 256203:
                if (auto c = me->FindNearestCreature(56143, 65.f, true))
                {
                    me->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                    me->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);
                    c->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    c->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->CastWithDelay(10000, c, 105468, false, true);
                    c->CastWithDelay(14000, me, 105468, false, true);
                    c->AddThreat(me, 100.f);
                    c->AI()->AttackStart(me);
                    /*
                    Position crush_jump{ -1955.514648f, -2437.536621f, 111.827965f, 1.589635f };

                    //me->DisableMovementFlagUpdate(true);
                    me->SetHover(false);
                    me->SetCanFly(false);
                    me->SetDisableGravity(false);
                    me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_NO_JUMPING);
                    me->GetMotionMaster()->Clear();
                    */
                }
                break;
            case 256184:
                break;
            case 256196:
                break;
            case 256183:
                break;
            case 256217:
            {
                me->SetReactState(REACT_PASSIVE);
                std::list<Creature*> list_of_npcs;
                GetCreatureListWithEntryInGrid(list_of_npcs, me, 56144, 200.0f);
                if (list_of_npcs.size())
                    for (auto c : list_of_npcs)
                    {
                        if (c->GetDBTableGUIDLow() == 256244 || c->GetDBTableGUIDLow() == 256245)
                        {
                            //TC_LOG_ERROR("sql.sql", "found evil drake to do action.");
                            c->AI()->DoAction(256244);
                        }
                    }


            }
            break;
            /*
                Team Deathwing
            */
            case 256233:
                break;
            case 256227://waypoint duo
            case 256231:
                me->CastWithDelay(urand(7000, 12000), me, 101837, true, true);
                break;
            case 256051:
                break;
            case 256245:
                break;
            case 256244:
                break;
            case 256049:
                break;
            case 256226:
                break;
            case 256225:
                break;
            case 256243:
                break;
            case 256067:
                break;
            case 256068:
                break;
                /*
                    Fly in a circle around the tower
                */
            case 256053:
            case 256065:
            case 256070:
            {
                Position my_center{ -1786.390015f, -2393.530029f, me->GetPositionZ(), 0.f };
                if (me->GetExactDist(&my_center) > 135.f)
                {
                    me->SetSpeed(MOVE_FLIGHT, frand(3.f, 7.f));
                    me->GetMotionMaster()->MoveAroundPoint(my_center, me->GetExactDist(&my_center), bool(urand(0, 1)), 100, me->GetPositionZ(), true);
                }
                break;
            }
            case 256054:
                break;
            case 256249:
                break;
                /*
                    FLy in circles over morchok
                */

            case 256246:
            case 256066:
            case 256228:
            case 256232:
            case 256248:
            {
                Position my_center{ -2150.005127f, -2404.018311f, me->GetPositionZ(), 6.062236f };
                me->SetSpeed(MOVE_FLIGHT, frand(3.f, 7.f));
                me->GetMotionMaster()->MoveAroundPoint(my_center, me->GetExactDist(&my_center), bool(urand(0, 1)), 50, me->GetPositionZ(), true);

                break;
            }
            case 256230:
                break;
            case 256229:
                break;
            case 256069:
                break;
            case 256247:
                break;
            case 256052:
                break;


            case 256218://little drakee being chased in opener
                events.ScheduleEvent(420, 200);
                break;
            case 256050://big drake chasing little at start
                events.ScheduleEvent(420, 1000);
                break;

            default:
                break;
            }
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == 104230)
                if (!dying)
                {
                    me->CastSpell(me, 29266, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->AddAura(83237, me);
                    me->GetMotionMaster()->MoveSmoothPath(DS_Drake_Gank_DeathPath, 8, 30.f);
                    dying = true;
                }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (acting)
                if (!dying)
                Check_To_Crush_drakes();

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 101:
                    break;
                case 404:
                    StartMovements();
                    break;
                case 420:
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetReactState(REACT_PASSIVE);
                    me->AddAura(101511, me);
                    me->CastWithDelay(4000, me, 101837, true, true);
                    me->GetMotionMaster()->MoveSmoothPath(DS_Drake_StartingChase_path, 59, 60.f);
                    break;
                case 256244:
                {
                    //TC_LOG_ERROR("sql.sql", "%u doing action 256244", me->GetDBTableGUIDLow());
                    std::list<Creature*> list_of_npcs;
                    GetCreatureListWithEntryInGrid(list_of_npcs, me, 56142, 200.0f);
                    if (list_of_npcs.size())
                        for (auto c : list_of_npcs)
                            if (c->GetDBTableGUIDLow() == 256217)
                            {
                                me->SetFacingTo(me->GetAngle(c));
                                me->CastWithDelay(500,  c, 104230, true, false);
                                me->CastWithDelay(1000, c, 104230, true, false);
                                me->CastWithDelay(3000, c, 104230, true, false);
                                me->CastWithDelay(5000, c, 104230, true, false);
                                me->CastWithDelay(7000, c, 104230, true, false);
                                me->CastWithDelay(9000, c, 104230, true, false);
                                if (auto c = me->FindNearestCreature(55912, 100.f, true))
                                    c->AI()->DoAction(256021);
                            }

                    break;
                }
                case 256021:
                {
                    Position land_at{ -2149.023926f, -2376.466553f, 135.200119f, 3.231117f };
                    me->SetSpeed(MOVE_FLIGHT, 3.f);
                    me->GetMotionMaster()->MoveLand(100, land_at);
                    break;
                }
                case 256022:
                {
                    Position take_off_to{ -2203.449951f, -2392.32690f, 165.605103f, 3.436104f };
                    me->GetMotionMaster()->MoveTakeoff(100, take_off_to);
                        break;
                }
                case 256023:
                {
                    me->GetMotionMaster()->Clear();
                    Position move_around{ -2075.396240f, -2424.924072f, 144.028046f, 6.165361f };
                    me->GetMotionMaster()->MoveAroundPoint(move_around, me->GetExactDist(&move_around), false, 25, me->GetPositionZ(), true);
                    break;
                }
                default:
                    break;
                }

            if (me->isInCombat())
                if (!me->HasReactState(REACT_PASSIVE))
                if (UpdateVictim())
                    DoMeleeAttackIfReady();
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
            case 256244:
                events.ScheduleEvent(256244, 25000);
                break;
            case 256021:
                if (!acting)
                {
                    acting = true;
                    events.ScheduleEvent(256021, 12000);
                }
                break;
            default:
                break;
            }
        }
    private:
        bool acting{ false };
        bool dying{ false };

        InstanceScript* instance;
        EventMap events;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new DS_drake_rp_trashAI(creature);
    }
};

class AreaTrigger_ds_players_at_wyrmrest_bottomFloor : public AreaTriggerScript
{
public:
    AreaTrigger_ds_players_at_wyrmrest_bottomFloor() : AreaTriggerScript("ds_players_at_wyrmrest_bottomFloor") {}

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
    {
        if (player->isGameMaster() || !player->isGMVisible())
            return false;

        if (auto instance = player->GetInstanceScript())
        {
            if (auto tyrygosa = instance->GetCreature(NPC_TYRYGOSA))
            {
                tyrygosa->AI()->DoAction(1);
            }

        }

        return false;
    }
};


class npc_ds_tyrygosa_center : public CreatureScript
{
public:
    npc_ds_tyrygosa_center() : CreatureScript("npc_ds_tyrygosa_center") {}

    struct npc_ds_tyrygosa_centerAI : public Scripted_NoMovementAI
    {
        npc_ds_tyrygosa_centerAI(Creature* creature) : Scripted_NoMovementAI(creature) { }

        void UpdateAI(uint32 const diff) override
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                case 5:
                    me->SetFacingTo(0.f);
                    Talk(6, NULL, CHAT_MSG_MONSTER_SAY);
                    me->EmoteWithDelay(2000, EMOTE_ONESHOT_POINT);
                    if (auto drake = me->FindNearestCreature(NPC_NETHESTRASZ, 150.f))
                        if (auto click_me = drake->AddAura(SPELL_WARDEN_RING_ORANGE, drake))
                        {
                            click_me->SetMaxDuration(86400000);
                            click_me->SetDuration(86400000);
                        }
                    break;
                default:
                    break;
                }
        }

        void DoAction(int32 const action)
        {
            if (action == 1)//areatrigger from players
            {
                if (me->GetPhaseMask() != 1)
                    return;

                if (rp_state >= 3)
                    return;

                uint8 rp_check{ 0 };


                bool morchok    { instance->IsDone(DATA_MORCHOK)        };
                bool zonozz     { instance->IsDone(DATA_WARLORD_ZONOZZ) };
                bool yorsahj    { instance->IsDone(DATA_YORSAHJ)        };
                bool hagara     { instance->IsDone(DATA_HAGARA)         };

                //boss status logic should make sense before we have the NPC start talking
                if (morchok)
                {
                    rp_check++;

                    if (zonozz)
                        rp_check++;
                    if (yorsahj)
                        rp_check++;

                    if (zonozz && yorsahj)
                        if (hagara)
                            rp_check++;
                }


                if (rp_check > rp_state)
                {
                    rp_state = rp_check;
                    switch (rp_state)
                    {
                    case 2:
                        me->TalkWithDelay(1000, 4, NULL, CHAT_MSG_MONSTER_SAY);
                        break;
                    case 3:
                    case 4:
                    case 5:
                        me->TalkWithDelay(1000, 5, NULL, CHAT_MSG_MONSTER_SAY);
                        events.ScheduleEvent(5, 9000);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

    private:
        InstanceScript* instance{ me->GetInstanceScript() };
        EventMap        events;
        uint8           rp_state{ 0 };
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ds_tyrygosa_centerAI(creature);
    }
};


void AddSC_instance_ds_trash()
{
	new npc_eiendormi();
	new npc_valeera_tele();
	new npc_nethestrasz();
	new npc_ds_earthen_destroyer();
    new npc_ds_ancient_water_lord();
    new npc_ds_twilight_siege_captain();
    new npc_ds_earthen_solider();
    new npc_ds_eye_of_gorath_trash();
    new npc_ds_flail_of_gorath_trash();
    new npc_ds_globule_trash();
    new npc_ds_lord_afrasastrasz();
    new npc_ds_twilight_portal();
    new spell_waterlord_pure_water();
    new DS_drake_rp_trash();
    new AreaTrigger_ds_players_at_wyrmrest_bottomFloor();
    new npc_ds_tyrygosa_center();
}