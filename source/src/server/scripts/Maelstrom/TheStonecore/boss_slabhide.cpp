#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "the_stonecore.h"
#include "Vehicle.h"
#include "Unit.h"
#include "ScriptedEscortAI.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "ObjectMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "CreatureTextMgr.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "Spell.h"
#include "Player.h"
#include "Map.h"
#include "InstanceScript.h"

Position const StalactitePositions[11] =
{
// Zone 1
{1189.197f, 1110.698f, 264.676f, 0.00f},
{1193.251f, 1100.722f, 264.604f, 0.00f},
{1199.068f, 1099.967f, 265.798f, 0.00f},
{1201.526f, 1104.843f, 265.289f, 0.00f},
{1196.528f, 1107.337f, 264.758f, 0.00f},
{1191.346f, 1110.689f, 264.017f, 0.00f},
{1183.159f, 1113.466f, 262.463f, 0.00f},
{1173.506f, 1113.670f, 263.253f, 0.00f},
{1186.418f, 1122.406f, 261.693f, 0.00f},
{1195.485f, 1119.908f, 263.393f, 0.00f},
{1200.777f, 1114.714f, 264.327f, 0.00f},
};
Position const StalactitePositionsB[15] =
{
//zone 2
{1208.057f, 1119.652f, 264.810f, 0.00f},
{1208.057f, 1119.652f, 264.810f, 0.00f},
{1201.957f, 1128.579f, 262.190f, 0.00f},
{1208.057f, 1119.652f, 264.810f, 0.00f},
{1208.057f, 1119.652f, 264.810f, 0.00f},
{1219.359f, 1138.910f, 261.191f, 0.00f},
{1219.359f, 1138.910f, 261.191f, 0.00f},
{1222.421f, 1126.250f, 263.108f, 0.00f},
{1217.212f, 1122.359f, 263.807f, 0.00f},
{1215.790f, 1119.415f, 264.614f, 0.00f},
{1211.347f, 1126.259f, 262.374f, 0.00f},
{1216.358f, 1130.338f, 262.023f, 0.00f},
{1225.560f, 1135.617f, 262.418f, 0.00f},
{1215.630f, 1123.473f, 263.394f, 0.00f},
{1216.489f, 1114.482f, 265.769f, 0.00f},
};
Position const StalactitePositionsC[11] =
{
//zone 3
{1215.520f, 1135.349f, 261.062f, 0.00f},
{1219.359f, 1138.910f, 261.191f, 0.00f},
{1214.474f, 1142.427f, 258.987f, 0.00f},
{1218.052f, 1141.921f, 259.246f, 0.00f},
{1223.056f, 1138.953f, 260.548f, 0.00f},
{1223.535f, 1135.448f, 262.186f, 0.00f},
{1219.069f, 1134.868f, 261.101f, 0.00f},
{1215.119f, 1146.174f, 257.844f, 0.00f},
{1221.284f, 1146.051f, 258.302f, 0.00f},
{1222.206f, 1149.977f, 257.576f, 0.00f},
{1214.596f, 1150.125f, 256.498f, 0.00f},
};
Position const StalactitePositionsD[13] =
{
//zone 4
{1210.445f, 1155.953f, 255.121f, 0.00f},
{1215.527f, 1157.593f, 255.350f, 0.00f},
{1222.458f, 1160.468f, 255.055f, 0.00f},
{1222.548f, 1167.873f, 253.537f, 0.00f},
{1217.291f, 1168.205f, 254.977f, 0.00f},
{1208.454f, 1160.776f, 254.290f, 0.00f},
{1220.005f, 1155.007f, 256.219f, 0.00f},
{1220.127f, 1163.075f, 254.427f, 0.00f},
{1213.837f, 1160.605f, 254.715f, 0.00f},
{1215.141f, 1165.490f, 253.849f, 0.00f},
{1207.165f, 1166.970f, 253.565f, 0.00f},
{1213.842f, 1171.547f, 252.854f, 0.00f},
{1215.505f, 1155.976f, 255.594f, 0.00f},
};
Position const StalactitePositionsE[9] =
{
//zone 5
{1211.607f, 1169.501f, 253.195f, 0.00f},
{1213.907f, 1176.933f, 252.083f, 0.00f},
{1219.622f, 1172.306f, 252.806f, 0.00f},
{1225.307f, 1167.208f, 253.741f, 0.00f},
{1229.791f, 1173.554f, 252.788f, 0.00f},
{1228.034f, 1180.696f, 251.228f, 0.00f},
{1225.953f, 1188.416f, 249.864f, 0.00f},
{1235.415f, 1189.752f, 248.351f, 0.00f},
{1238.809f, 1181.473f, 249.558f, 0.00f},	
};

//Position const SlabhideIntroPos = { 1292.27f, 1226.16f, 265.573f };
Position const SlabhideIntroLandPos = { 1292.352f, 1226.478f, 247.6368f, 3.630285f };

const G3D::Vector3 IntoPath[]
{
	{ 1197.8999f, 1206.765f, 268.698f },
	{ 1221.3999f, 1249.015f, 267.198f },
	{ 1270.1499f, 1259.515f, 270.448f },
	{ 1292.1499f, 1226.015f, 265.448f }
};

enum Spells
{    
	SPELL_SAND_BLAST = 80807,  //Sandblast spell
	SPELL_LAVA_FISSURE = 80803,  //cast on target - on our server it might already be DB scripted and it works fine. Spell Range needs to be added to damage spells 80800 + 80801 - 5yds.
	SPELL_LAVA_FISSURE_VIS =80798,
    SPELL_CRYSTAL_STORM     = 92265,  //Crystal breath - 2.5 sec cast time + 6 seconds.
	SPELL_CRYSTAL_STORM_REQ = 92306,
    SPELL_STALACTITE  = 80656,  //Trigger Multiple Summon Spell
    SPELL_STALAC_VIS  = 80643,  //Multiple effects - triger spell 80647(DMG)
    SPELL_STALAC_WAR  = 80654,  //Warning for where it falls.
    
    SPELL_ERUPTION    = 80800,
};

enum Events
{
	EVENT_LAND_INTRO			= 1,
    EVENT_STALACTITE,            
    EVENT_STALACTITE_CAST,
    EVENT_STALACTITE_LAND,
    EVENT_STALACTITE_LAND_END,
    EVENT_SAND_BLAST,
    EVENT_LAVA_FISSURE,
    EVENT_CRYSTAL_STORM,
	ACTION_DESPAWN_ROCKS,
};

enum Walls
{
	WALL_ENTRANCE = 0,
	WALL_EXIT,
};

enum Phases
{
    PHASE_NULL,
    PHASE_GROUND,
    PHASE_AIR
};

class boss_slabhide : public CreatureScript
{
    public:
        boss_slabhide() : CreatureScript("boss_slabhide") { }

        struct boss_slabhideAI : public BossAI
        {
            boss_slabhideAI(Creature* creature) : BossAI(creature, DATA_SLABHIDE), summons(me)
            {
                instance = me->GetInstanceScript();
				me->DisableMovementFlagUpdate(true);
				introDone = false;
            }

            InstanceScript* instance;
            Phases phase;
            SummonList summons;
            bool introDone, inCombat;
            EventMap events;
			uint32 SummonsStalactiteTimer;

			void DoAction(int32 const act)
			{
				switch (act)
				{
				case ACTION_DESPAWN_ROCKS:
					//despawn stalactite triggers
					std::list<Creature*> units;
					GetCreatureListWithEntryInGrid(units, me, 43159, 50.0f);
					GetCreatureListWithEntryInGrid(units, me, 43242, 50.0f);
					for (auto itr = units.begin(); itr != units.end(); ++itr)
						if (auto c = (*itr))
						{
							c->DeleteThreatList();
							c->DespawnOrUnsummon();
						}

					//despawn stalactites
					std::list<GameObject*> objs;
					GetGameObjectListWithEntryInGrid(objs, me, 204337, 50.f);
					if (objs.size())
						for (auto itr = objs.begin(); itr != objs.end(); ++itr)
							if (auto rock = (*itr))
								rock->SetLootState(GO_JUST_DEACTIVATED);
					break;
				}
			}

			bool CanSeeAlways(WorldObject const* obj)
			{
				if (auto p = obj->ToPlayer())
					return !(p->isGameMaster() || !p->isGMVisible());

				return false;
			}

            void Reset()
            {

				instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                events.Reset();
				_Reset();
                if (instance)
                    instance->SetBossState(DATA_SLABHIDE, NOT_STARTED);

				if (!introDone)
				{
					phase = PHASE_NULL;
					events.SetPhase(PHASE_NULL);
					inCombat = false;
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetReactState(REACT_PASSIVE);
					me->SetCanFly(true);
					me->SetDisableGravity(true);
					me->SetHover(true);
				}
				else
				{
					phase = PHASE_GROUND;
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetCanFly(false);
					me->SetDisableGravity(false);
					me->SetHover(false);
					me->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
					me->SetReactState(REACT_AGGRESSIVE);
				}

				SetWalls(false, true);
                summons.DespawnAll();
				DoAction(ACTION_DESPAWN_ROCKS);
            }

            void EnterEvadeMode()
            {
                summons.DespawnAll();
				_EnterEvadeMode();
                me->GetMotionMaster()->MoveTargetedHome();
                events.Reset();
                inCombat = false;
                phase = PHASE_NULL;
                events.SetPhase(PHASE_NULL);

                if (instance)
                {
                    instance->SetBossState(DATA_SLABHIDE, FAIL);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                }
            }

			void SetWalls(bool entrance, bool exit)
			{
				//despawn stalactites
				std::list<GameObject*> objs;
				GetGameObjectListWithEntryInGrid(objs, me, 204381, 400.f);
				if (objs.size())
					for (auto itr = objs.begin(); itr != objs.end(); ++itr)
						if (auto rock = (*itr))
							switch (rock->GetDBTableGUIDLow())
							{
							case 116052: //entrance
								rock->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
								rock->SetGoState(entrance ? GO_STATE_READY : GO_STATE_ACTIVE);
								break;
							case 116051:	//exit
								rock->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
								rock->SetGoState(exit ? GO_STATE_READY : GO_STATE_ACTIVE);
								break;
							default:
								break;
							}
			}

            void MoveInLineOfSight(Unit* who)
            {
                if ((introDone && !me->HasReactState(REACT_AGGRESSIVE)) || !me->IsWithinDistInMap(who, 30.0f, false))
                    return;

				if (!introDone)
				if (me->FindNearestPlayer(40.0f))
				{
					introDone = true;
					me->GetMotionMaster()->MoveSmoothPath(IntoPath, 4);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					SetWalls(false, true);
				}

				if (who->ToPlayer())
					if (me->IsValidAttackTarget(who))
					if (!me->isInCombat())
					if (me->GetDistance(who) < 15.f)
						if (introComplete)
						{
							me->SetReactState(REACT_AGGRESSIVE);
							me->AddThreat(who, 10000.f);
							me->CombatStart(who);
						}
            }

			void MovementInform(uint32 type, uint32 id) override
			{
				if (type == SPLINE_MOTION_TYPE && id == 3)
				{
					me->SetFacingTo(SlabhideIntroLandPos.GetOrientation());
					events.ScheduleEvent(EVENT_LAND_INTRO, 200);
				}

				if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
					return;

				switch (id)
				{
					case 1:
					{
						me->SetCanFly(false);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						me->SetDisableGravity(false);
						me->SetHover(false);
						me->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
						me->SetReactState(REACT_AGGRESSIVE);
						me->SetHomePosition(1292.352f, 1226.478f, 247.6368f, 3.630285f);
						me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
						break;
					}
					default:
						break;
				}
			}

            void JustDied(Unit* /*killer*/)
            {
                if (instance)
                {
					instance->SetBossState(DATA_SLABHIDE, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                }
				_JustDied();

				events.Reset();
				summons.DespawnAll();
				me->RemoveAllAuras();
				me->CastStop();
				//despawn stalactite triggers
				std::list<Creature*> units;
				GetCreatureListWithEntryInGrid(units, me, 43159, 50.0f);
				GetCreatureListWithEntryInGrid(units, me, 43242, 50.0f);
				if (units.size())
				for (auto itr = units.begin(); itr != units.end(); ++itr)
					if (auto c = (*itr))
					{
						c->DeleteThreatList();
						c->DespawnOrUnsummon();
					}

				//despawn stalactites
				std::list<GameObject*> objs;
				GetGameObjectListWithEntryInGrid(objs, me, 204337, 50.f);
				if (objs.size())
					for (auto itr = objs.begin(); itr != objs.end(); ++itr)
						if (auto rock = (*itr))
							rock->SetLootState(GO_JUST_DEACTIVATED);

				if (auto gMissile = me->FindNearestCreature(433570, 100.0f))
					gMissile->DespawnOrUnsummon();

				SetWalls(false, false);
            }

            void EnterCombat(Unit* /*who*/)
            {
				if (introComplete)
				{
					_EnterCombat();
					SetWalls(true, true);
					if (instance)
					{
						instance->SetBossState(DATA_SLABHIDE, IN_PROGRESS);
						instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add
					}

					phase = PHASE_GROUND;
					events.SetPhase(PHASE_GROUND);
					inCombat = true;

					events.ScheduleEvent(EVENT_STALACTITE, 20000);
					events.ScheduleEvent(EVENT_SAND_BLAST, urand(5000, 8000));
					events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(10000, 12000));
				}
            }

			void UpdateAI(uint32 diff)
			{
				if ((!UpdateVictim() && inCombat) || me->HasUnitState(UNIT_STATE_CASTING))
					return;

				events.Update(diff);
				if(phase == PHASE_NULL)
					if (events.ExecuteEvent() == EVENT_LAND_INTRO)
					{
						me->GetMotionMaster()->MoveLand(1, SlabhideIntroLandPos);
						introComplete = true;
						me->SetReactState(REACT_AGGRESSIVE);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					}

				if (phase == PHASE_GROUND) // Ground phase
				{
					while (uint32 eventId = events.ExecuteEvent())
					{
						switch (eventId)
						{
							case EVENT_STALACTITE:
								events.DelayEvents(20000);
								events.CancelEventGroup(EVENT_STALACTITE_CAST);
								events.CancelEventGroup(EVENT_STALACTITE_LAND);
								me->SetReactState(REACT_PASSIVE);
								me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
								me->GetMotionMaster()->Clear();
								me->SetCanFly(true);
								me->SetDisableGravity(true);
								me->SetHover(true);
								me->GetMotionMaster()->MovePoint(2, 1275.519f, 1217.723f, me->GetPositionZ() + 15.0f);
								me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
								events.ScheduleEvent(EVENT_STALACTITE_CAST, 3000);
								events.ScheduleEvent(EVENT_STALACTITE_LAND, 8000); 
								break;
							case EVENT_STALACTITE_CAST:
								me->MonsterMoveWithSpeed(x, y, z + 10.0f, 0, false, false);
								me->SetCanFly(true);
								me->SetDisableGravity(true);
								me->SetHover(true);
								if (auto gMissile = me->FindNearestCreature(433570, 100.0f))
									gMissile->AddAura(SPELL_STALACTITE, gMissile);
								else
								{
									Position center = { 1275.970703f, 1217.204224f, 247.107544f };
									if (auto gMissile = me->SummonCreature(433570, center, TEMPSUMMON_TIMED_DESPAWN, 10000))
										gMissile->AddAura(SPELL_STALACTITE, gMissile);
								}
								break;
							case EVENT_STALACTITE_LAND:
								me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
								/*
								
								me->SetCanFly(false);
								me->SetDisableGravity(false);
								me->SetHover(false);
								me->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
								*/

								me->GetMotionMaster()->MoveLand(1, SlabhideIntroLandPos);
								events.ScheduleEvent(EVENT_STALACTITE_LAND_END, IsHeroic() ? 8000 : 1500);
								break;
							case EVENT_STALACTITE_LAND_END:
								me->SetReactState(REACT_AGGRESSIVE);
								if (IsHeroic())
								{
									if (auto victim = me->getVictim())
									{
										me->SetUInt64Value(UNIT_FIELD_TARGET, victim->GetGUID());
										me->CastWithDelay(4000, victim, SPELL_CRYSTAL_STORM, true);
									}
								}
								if (Unit * victim = me->getVictim())
								{
									me->SetUInt64Value(UNIT_FIELD_TARGET, victim->GetGUID());
									DoStartMovement(victim);
								}
								events.ScheduleEvent(EVENT_STALACTITE, urand(25000, 30000));
								break;
							case EVENT_SAND_BLAST:
								DoCast(SPELL_SAND_BLAST);
								events.ScheduleEvent(EVENT_SAND_BLAST, urand(12000, 15000));
								break;
							case EVENT_LAVA_FISSURE:
							{
								DoCastRandom(SPELL_LAVA_FISSURE, 0.0f);
								uint32 cd = IsHeroic() ? urand(5000, 7000) : urand(7000, 10000);
								events.ScheduleEvent(EVENT_LAVA_FISSURE, cd);
							}

							break;
						}
					}

					DoMeleeAttackIfReady();
				}

			}
			private:
				GameObject* Walls[2]{ nullptr, nullptr };
				Position wall_pos[2]
				{
					{1235.72f, 1197.74f, 247.605f, 0.977383f},
					{1324.74f, 1206.68f, 245.773f, 2.86233f }
				};
				bool introComplete{ false };
				float x, y, z;

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_slabhideAI(creature);
        }
};
		
class npc_stalactite_stalker : public CreatureScript
{
public:
	npc_stalactite_stalker() : CreatureScript("npc_stalactite_stalker") {}

	struct npc_stalactite_stalkerAI : public ScriptedAI
	{
		npc_stalactite_stalkerAI(Creature * creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}

		void Reset()
		{
			done = false;
			visual = false;
			visualTimer = 3000;
			summonTimer = 5000;
		}

		void IsSummonedBy(Unit* /*summoner*/)
		{
			me->GetPosition(x, y, z);
			me->GetMotionMaster()->Clear();
			if (me->FindNearestCreature(me->GetEntry(), 1.0f) != NULL) // more visual cleanup
				me->DespawnOrUnsummon();
		}

		void UpdateAI(uint32 const diff)
		{
			if (!visual)
			{
				if (visualTimer <= diff)
				{
					visual = true;
					me->CastSpell(x, y, z, SPELL_STALAC_WAR, true);
				}visualTimer -= diff;
			}

			if (!done)
			{
				if (summonTimer <= diff)
				{
					done = true;
					me->CastSpell(x, y, z, SPELL_STALAC_VIS, true);
					me->DespawnOrUnsummon(10000);
				}
				else summonTimer -= diff;
			}
		}

	private:
		float x, y, z;
		bool done;
		bool visual;
		uint32 visualTimer;
		uint32 summonTimer;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_stalkerAI(creature);
	}
};

class npc_lava_fissure : public CreatureScript
{
public:
    npc_lava_fissure() : CreatureScript("npc_lava_fissure") {}

    struct npc_lava_fissureAI : public ScriptedAI
    {
        npc_lava_fissureAI(Creature* creature) : ScriptedAI(creature) {}

        void Reset()
        {
            active = false;
            activeTimer = IsHeroic() ? 3000 : 5000;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!active)
            {
                if (activeTimer <= diff)
                {
                    active = true;
                    me->RemoveAllAuras();
					DoCast(SPELL_LAVA_FISSURE_VIS);
					me->CastWithDelay(3000, me, SPELL_ERUPTION);
                    me->DespawnOrUnsummon(IsHeroic() ? 30000 : 10000);
                }else activeTimer -= diff;
            }
        }

    private:
        bool active;
        uint32 activeTimer;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lava_fissureAI(creature);
    }
};


class spell_crystal_storm : public SpellScriptLoader
{
public:
    spell_crystal_storm() : SpellScriptLoader("spell_crystal_storm") { }

    class crystal_storm_filter
    {
    public:
        crystal_storm_filter(Unit* caster, std::list<GameObject*> const& stalagmiteList) : _stalagmiteList(stalagmiteList), _caster(caster) { }

        bool operator()(WorldObject* unit)
        {
            for (std::list<GameObject*>::const_iterator itr = _stalagmiteList.begin(); itr != _stalagmiteList.end(); ++itr)
                if ((*itr)->IsInBetween(_caster, unit, 1.0f) && _caster->GetDistance2d((*itr)->GetPositionX(),(*itr)->GetPositionY()) > 1.0f)
                    return true;

            return false;
        }

    private:
        std::list<GameObject*> const& _stalagmiteList;
        WorldObject* _caster;
    };

    class spell_crystal_storm_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_crystal_storm_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            std::list<GameObject*> stalagmites;
            GetGameObjectListWithEntryInGrid(stalagmites, GetCaster(), GO_STALAGMITE, 100.0f);
            unitList.remove_if(crystal_storm_filter(GetCaster(), stalagmites));
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_crystal_storm_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_crystal_storm_SpellScript();
    }
};

class npc_stalactite_summ : public CreatureScript
{
public:
	npc_stalactite_summ() : CreatureScript("npc_stalactite_summ") {}
	struct npc_stalactite_summAI : public ScriptedAI
	{
		npc_stalactite_summAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}
		
		uint32 summctimer = 3000;
		void Reset()
		{
			summctimer = 3000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if(summctimer <= diff)
			{ 
			if(me && me->FindNearestPlayer(14.0f))
				for (int i = 0; i < 5; i++)
						me->SummonCreature(NPC_STALACTITE_TRIGGER, StalactitePositions[urand(0, 11)], TEMPSUMMON_TIMED_DESPAWN, 10000);
			summctimer = 3000;
			}
			else summctimer -= diff;
		}
	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_summAI(creature);
	}
};	

class npc_stalactite_summb : public CreatureScript
{
public:
	npc_stalactite_summb() : CreatureScript("npc_stalactite_summb") {}
	struct npc_stalactite_summbAI : public ScriptedAI
	{
		npc_stalactite_summbAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}
		
		uint32 summdtimer = 3000;
		void Reset()
		{
			summdtimer = 3000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if(summdtimer <= diff)
			{ 
			if(me && me->FindNearestPlayer(14.0f))
				for (int i = 0; i < 5; i++)
						me->SummonCreature(NPC_STALACTITE_TRIGGER, StalactitePositionsB[urand(0, 15)], TEMPSUMMON_TIMED_DESPAWN, 10000);
			summdtimer = 3000;
			}
			else summdtimer -= diff;
		}

	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_summbAI(creature);
	}
};	

class npc_stalactite_summc : public CreatureScript
{
public:
	npc_stalactite_summc() : CreatureScript("npc_stalactite_summc") {}
	struct npc_stalactite_summcAI : public ScriptedAI
	{
		npc_stalactite_summcAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}
		
		uint32 summetimer = 3000;
		void Reset()
		{
			summetimer = 3000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if(summetimer <= diff)
			{ 
			if(me && me->FindNearestPlayer(14.0f))
				for (int i = 0; i < 5; i++)
						me->SummonCreature(NPC_STALACTITE_TRIGGER, StalactitePositionsC[urand(0, 11)], TEMPSUMMON_TIMED_DESPAWN, 10000);
			summetimer = 3000;
			}
			else summetimer -= diff;
		}

	};
	
	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_summcAI(creature);
	}
};		
class npc_stalactite_summd : public CreatureScript
{
public:
	npc_stalactite_summd() : CreatureScript("npc_stalactite_summd") {}
	struct npc_stalactite_summdAI : public ScriptedAI
	{
		npc_stalactite_summdAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}

		uint32 summftimer = 3000;
		void Reset()
		{
			summftimer = 3000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if(summftimer <= diff)
			{ 
			if(me && me->FindNearestPlayer(14.0f))
				for (int i = 0; i < 5; i++)
						me->SummonCreature(NPC_STALACTITE_TRIGGER, StalactitePositionsD[urand(0, 13)], TEMPSUMMON_TIMED_DESPAWN, 10000);
			summftimer = 3000;
			}
			else summftimer -= diff;
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_summdAI(creature);
	}
};

class npc_stalactite_summe : public CreatureScript
{
public:
	npc_stalactite_summe() : CreatureScript("npc_stalactite_summe") {}
	struct npc_stalactite_summeAI : public ScriptedAI
	{
		npc_stalactite_summeAI(Creature* creature) : ScriptedAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}

		uint32 summgtimer = 3000;
		void Reset()
		{
			summgtimer = 3000;
		}
		
		void UpdateAI(uint32 const diff)
		{
			if(summgtimer <= diff)
			{ 
			if(me && me->FindNearestPlayer(8.0f))
				for (int i = 0; i < 5; i++)
						me->SummonCreature(NPC_STALACTITE_TRIGGER, StalactitePositionsE[urand(0, 9)], TEMPSUMMON_TIMED_DESPAWN, 10000);
			summgtimer = 3000;
			}
			else summgtimer -= diff;
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stalactite_summeAI(creature);
	}
};

class spell_crystal_storm_phase_end : public SpellScriptLoader
{
public:
	spell_crystal_storm_phase_end() : SpellScriptLoader("spell_crystal_storm_phase_end") { }	// 92265
	class spell_crystal_storm_phase_end_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_crystal_storm_phase_end_AuraScript);

		void HandleOnEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
		{
			if (auto owner = GetOwner()->ToUnit())
				if (auto me = owner->ToCreature())
					if (auto ai = me->AI())
						ai->DoAction(ACTION_DESPAWN_ROCKS);
		}

		void Register()
		{
			OnEffectRemove += AuraEffectRemoveFn(spell_crystal_storm_phase_end_AuraScript::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
		}
	};
	AuraScript* GetAuraScript() const
	{
		return new spell_crystal_storm_phase_end_AuraScript();
	}
};

void AddSC_boss_slabhide()
{
    new boss_slabhide();
    new npc_stalactite_stalker();
	new spell_crystal_storm();
	new npc_lava_fissure();
	new npc_stalactite_summ();
	new npc_stalactite_summb();
	new npc_stalactite_summc();
	new npc_stalactite_summd();
	new npc_stalactite_summe();
	new spell_crystal_storm_phase_end();
}