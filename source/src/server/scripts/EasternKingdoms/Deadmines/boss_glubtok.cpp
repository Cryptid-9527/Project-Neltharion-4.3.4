
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "deadmines.h"

enum Spells
{
    SPELL_FIRE_BLOSSOM          = 88129,
    SPELL_FIRE_BLOSSOM_VISUAL   = 88164,
    SPELL_FIRE_BLOSSOM_SUMMON   = 91301,
    SPELL_FROST_BLOSSOM         = 88169,
    SPELL_FROST_BLOSSOM_VISUAL  = 88165,
    SPELL_FROST_BLOSSOM_SUMMON  = 91302,
    SPELL_ARCANE_POWER          = 88009,
    SPELL_FIST_OF_FLAME         = 87859,
    SPELL_FIST_OF_FROST         = 87861,
    SPELL_BLINK                 = 87925,
    SPELL_MASS_TELEPORT         = 87999,
    SPELL_DEEP_FREEZE           = 88000,

    SPELL_ARCANE_FIRE_RAY       = 88072, // 47242
    SPELL_ARCANE_FROST_RAY      = 88093, // 472421

    SPELL_FIREWALL_DAMAGE       = 91398,


    SPELL_TRANSITION_INVIS = 90424,
    SPELL_ARCANE_OVERCHARGE = 88183,
    SPELL_ARCANE_OVERCHARGE_2 = 90520,
    SPELL_ARCANE_DEAD = 88185,
};

enum Texts
{
    SAY_AGGRO           = 0,
    SAY_FISTS_OF_FLAME,
    SAY_FISTS_OF_FROST,
    SAY_READY,
    SAY_LETS_DO_IT,
    SAY_ARCANE_POWER,
    EMOTE_FLAME_WALL,
    SAY_TOO_MUCH_POWER,
    SAY_KILL,
};

enum npcs
{
    NPC_FIREWALL_CENTER = 48974,
    NPC_FIREWALL_BUNNY_1 = 49040,//the only one we will use
    NPC_FIREWALL_BUNNY_2 = 49042,
    NPC_FIREWALL_BUNNY_3 = 48975,
    NPC_FIREWALL_BUNNY_4 = 48976,
    NPC_FIREWALL_BUNNY_5 = 49039,
    NPC_FIREWALL_BUNNY_6 = 49041,

    NPC_FIRE_BUNNY = 47242,
    NPC_FROST_BUNNY = 472421,
    NPC_FIRE_ELEMENTAL = 48957,
    NPC_FROST_ELEMENTAL = 48958,
};

enum Events
{
    EVENT_FISTS_OF_FLAME   = 1, // phase 1
    EVENT_FISTS_OF_FROST   = 2, // phase 1
    EVENT_BLINK            = 3, // phase 1
    EVENT_FROST_BLOSSOM    = 4, // phase 2
    EVENT_FIRE_BLOSSOM     = 5, // phase 2 hc only?
    EVENT_ARCANE_POWER     = 6,
    EVENT_SUMMON_FIRE_WALL = 7,

    EVENT_OUTRO,
    EVENT_OUTRO1,
    EVENT_PUNISH_FIREWALL,
    EVENT_CHECK_WIPE,
};

enum misc
{
    PHASE_ONE           = 1,
    PHASE_50_PERCENT    = 2,
    PHASE_OUTRO         = 3,
    POINT_CENTER        = 4
};

enum actions
{
    ACTION_BLINK = 42,
};

class RotateEvent : public BasicEvent
{
public:
    RotateEvent(Creature* owner) : _owner(owner) { }

    bool Execute(uint64 execTime, uint32 /*diff*/) override
    {
        _owner->GetMotionMaster()->MoveRotate(20000, ROTATE_DIRECTION_RIGHT);
        _owner->m_Events.AddEvent(this, execTime + 20000);
        return false;
    }
private :
    Creature* _owner;
};

class boss_glubtok : public CreatureScript
{
public:
    boss_glubtok() : CreatureScript("boss_glubtok") { }

    struct boss_glubtokAI : public BossAI
    {
        boss_glubtokAI(Creature* creature) : BossAI(creature, BOSS_GLUBTOK_DATA) { }

        void Reset()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            me->ClearUnitState(UNIT_STATE_CANNOT_TURN);
            _Reset();
            instance->SetData(DATA_FIREWALL, 1);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            me->SetHover(false);
            die = false;
            cnt = 0;
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            instance->DoRemoveAurasDueToSpellOnPlayers(109326);
        }

        bool checkWipe()
        {
            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                if (!player->HasAuraType(SPELL_AURA_FEIGN_DEATH) && !player->HasAuraType(SPELL_AURA_MOD_STEALTH) && !player->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                return false;
            return true;
        }

        void EnterCombat(Unit* /*who*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            _EnterCombat();
            Talk(SAY_AGGRO);
            events.SetPhase(PHASE_ONE);
            events.ScheduleEvent(urand(EVENT_FISTS_OF_FLAME, EVENT_FISTS_OF_FROST), 3000, 0, PHASE_ONE);
            events.ScheduleEvent(EVENT_BLINK, urand(5000, 10000), 0, PHASE_ONE);
            events.ScheduleEvent(EVENT_PUNISH_FIREWALL, 1000);
            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);

            auto players = me->GetPlayersInRange(200.f, true);
            for (Player* player : players)
                me->AddAura(109326, player);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            _JustDied();
            if (auto map = me->GetMap())
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
                    for (auto itr = players.begin(); itr != players.end(); ++itr)
                        if (auto player = itr->getSource())
                            if (auto instanceMap = map->ToInstanceMap())
                                instanceMap->PermBindAllPlayers(player);
            }
            instance->DoRemoveAurasDueToSpellOnPlayers(109326);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPEID_PLAYER)
                Talk(SAY_KILL);
        }

        void DoAction(int32 const act)
        {
            if (act == ACTION_BLINK)
            {
                if (events.IsInPhase(PHASE_50_PERCENT))
                    return;
                DoCast(SPELL_BLINK);
                if (IsHeroic())
                    DoResetThreat();
            }
        }

        void Punish_Firewall()
        {
            if (!events.IsInPhase(PHASE_50_PERCENT))
                return;

            auto players = me->GetPlayersInRange(100.f, true);
            if (players.size())
                for (Player* player : players)
                {
                    if (auto gas = player->FindNearestCreature(NPC_FIREWALL_BUNNY_1, 2.f, true))
                    {
                        if (gas->HasAura(SPELL_FIREWALL_DAMAGE))
                            gas->AI()->DoCast(player, 91397, true);//91397
                    }
                }
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage)
        {
            if (die)
            {
                damage = 0;
                return;
            }
            if (me->HealthBelowPctDamaged(50, damage) && events.IsInPhase(PHASE_ONE))
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CHECK_WIPE, 1000);
                events.SetPhase(PHASE_50_PERCENT);

                me->RemoveAurasDueToSpell(SPELL_FIST_OF_FLAME);
                me->RemoveAurasDueToSpell(SPELL_FIST_OF_FROST);
                //me->RemoveAllAuras();
                Position pos = { -193.460892f, -436.837738f, 53.325890f, 0.981683f };
                me->SetReactState(REACT_PASSIVE);
                me->StopMoving();
                me->AttackStop();
                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), true);
                me->AddUnitState(UNIT_STATE_CANNOT_TURN);

                me->TalkWithDelay(1000, SAY_READY, NULL, CHAT_MSG_MONSTER_YELL);
                me->TalkWithDelay(2500, SAY_LETS_DO_IT, NULL, CHAT_MSG_MONSTER_YELL);
                events.ScheduleEvent(EVENT_ARCANE_POWER, 5000, 0, PHASE_50_PERCENT);
            }

            if (damage > me->GetHealth())
            {
                die = true;
                damage = 0;
                events.Reset();
                events.SetPhase(PHASE_OUTRO);
                me->SetHover(false);
                me->CastSpell(me, SPELL_ARCANE_OVERCHARGE, true);
                instance->SetBossState(BOSS_GLUBTOK_DATA, DONE);
                events.ScheduleEvent(EVENT_OUTRO, 1000);
            }
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_FIRE_BLOSSOM_BUNNY:
                    summon->CastSpell(summon, SPELL_FIRE_BLOSSOM_VISUAL, true);
                    me->CastSpell(summon, SPELL_FIRE_BLOSSOM, false);
                    break;
                case NPC_FROST_BLOSSOM_BUNNY:
                    summon->CastSpell(summon, SPELL_FROST_BLOSSOM_VISUAL, true);
                    me->CastSpell(summon, SPELL_FROST_BLOSSOM, false);
                    break;
                case 49042:
                case NPC_FIREWALL_BUNNY_1:
                case 48975:
                case 48976:
                case 49039:
                case 49041:
                {
                    if (auto vehicle = me->FindNearestCreature(NPC_FIREWALL_CENTER, 100.0f, true))
                    {
                        cnt++;
                        summon->CastCustomSpell(VEHICLE_SPELL_RIDE_HARDCODED, SPELLVALUE_BASE_POINT0, cnt, vehicle, false);
                    }
                    summon->AddAura(SPELL_FIREWALL_DAMAGE, summon);
                    break;
                }
                case NPC_FIREWALL_CENTER:
                {
                    summon->m_Events.AddEvent(new RotateEvent(summon), summon->m_Events.CalculateTime(5000));
                    summon->SetDisableGravity(true);
                    Position pos = me->GetHomePosition();
                    summon->NearTeleportTo(summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ() + 3.5f, summon->GetOrientation(), false);
                    for (auto bunny_count = 0; bunny_count < 8; bunny_count++)
                    {
                        me->SummonCreature(NPC_FIREWALL_BUNNY_1, pos, TEMPSUMMON_MANUAL_DESPAWN);
                    }

                    break;
                }
                case NPC_FROST_ELEMENTAL:
                case NPC_FIRE_ELEMENTAL:
                    summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_DISABLE_MOVE);
                    summon->SetReactState(REACT_AGGRESSIVE);
                    summon->SetInCombatWithZone();
                    break;
                default:
                    break;
            }
            BossAI::JustSummoned(summon);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                    case EVENT_CHECK_WIPE:
                        //TC_LOG_ERROR("sql.sql", "Executing check wipe");
                        if (checkWipe())
                        {
                            //TC_LOG_ERROR("sql.sql", "Wipe recognized at glubtok.");
                            me->DeleteThreatList();
                            me->CombatStop(true);
                            Reset();
                            me->Respawn(true);
                            me->DeleteThreatList();
                        }
                        else
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 5000);
                        break;
                    case EVENT_PUNISH_FIREWALL:
                        Punish_Firewall();
                        events.ScheduleEvent(EVENT_PUNISH_FIREWALL, 1000);
                        break;
                    case EVENT_OUTRO:
                        me->CastSpell(me, SPELL_TRANSITION_INVIS, true);
                        me->CastSpell(me, SPELL_ARCANE_OVERCHARGE_2, true);
                        events.ScheduleEvent(EVENT_OUTRO1, 2300);
                        break;
                    case EVENT_OUTRO1:
                        die = false;
                        me->CastSpell(me, SPELL_ARCANE_DEAD, true);
                        break;
                    case EVENT_FISTS_OF_FLAME:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(SAY_FISTS_OF_FLAME);
                            DoCast(me, SPELL_FIST_OF_FLAME, false);
                            events.ScheduleEvent(EVENT_FISTS_OF_FROST, 12100, 0, PHASE_ONE);
                        }
                        break;
                    case EVENT_FISTS_OF_FROST:
                        if (me->HasUnitState(UNIT_STATE_CASTING)) events.ScheduleEvent(eventId, 250);
                        else
                        {
                            Talk(SAY_FISTS_OF_FROST);
                            DoCast(me, SPELL_FIST_OF_FROST, false);
                            events.ScheduleEvent(EVENT_FISTS_OF_FLAME, 12100, 0, PHASE_ONE);
                        }
                        break;
                    case EVENT_FROST_BLOSSOM:
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            me->SummonCreature(NPC_FROST_BLOSSOM_BUNNY, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(EVENT_FIRE_BLOSSOM, 3000, 0, PHASE_50_PERCENT);
                        break;
                    case EVENT_FIRE_BLOSSOM:
                        if (auto target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            me->SummonCreature(NPC_FIRE_BLOSSOM_BUNNY, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5000);
                        events.ScheduleEvent(EVENT_FROST_BLOSSOM, 3000, 0, PHASE_50_PERCENT);
                        break;
                    case EVENT_ARCANE_POWER:
                    {
                        Talk(SAY_ARCANE_POWER);
                        DoCast(me, SPELL_ARCANE_POWER, true);
                        me->SetHover(true);
                        me->SetControlled(true, UNIT_STATE_ROOT);
                        std::list<Creature*> stalker;
                        me->GetCreatureListWithEntryInGrid(stalker, NPC_FIRE_BUNNY, 200.0f);
                        for (std::list<Creature *>::iterator itr = stalker.begin(); itr != stalker.end(); itr++)
                            if (Creature *c = *itr)
                            {
                                c->SetObjectScale(1.4f);
                                c->GetMotionMaster()->MoveRandom(5.f);
                                c->CastSpell(me, SPELL_ARCANE_FIRE_RAY, true);
                            }
                        stalker.clear();
                        me->GetCreatureListWithEntryInGrid(stalker, NPC_FROST_BUNNY, 200.0f);
                        for (std::list<Creature *>::iterator itr = stalker.begin(); itr != stalker.end(); itr++)
                            if (Creature *c = *itr)
                            {
                                c->SetObjectScale(1.4f);
                                c->GetMotionMaster()->MoveRandom(5.f);
                                c->CastSpell(me, SPELL_ARCANE_FROST_RAY, true);
                            }
                        events.ScheduleEvent(urand(EVENT_FROST_BLOSSOM, EVENT_FIRE_BLOSSOM), 10000, 0, PHASE_50_PERCENT);
                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_SUMMON_FIRE_WALL, 6000, 0, PHASE_50_PERCENT);
                        break;
                    }
                    case EVENT_SUMMON_FIRE_WALL:
                        Talk(EMOTE_FLAME_WALL);
                        me->SummonCreature(NPC_FIREWALL_CENTER, me->GetPositionX(), me->GetPositionY(), 53.325890f, M_PI / 2.0f, TEMPSUMMON_MANUAL_DESPAWN);

                        std::list<Creature*> stalker;
                        me->GetCreatureListWithEntryInGrid(stalker, NPC_FIRE_BUNNY, 200.0f);
                        for (std::list<Creature*>::iterator itr = stalker.begin(); itr != stalker.end(); itr++)
                            if (auto c = *itr)
                            {
                                c->SetObjectScale(1.4f);
                                c->GetMotionMaster()->MoveRandom(5.f);
                                c->CastSpell(me, SPELL_ARCANE_FIRE_RAY, true);
                            }
                        stalker.clear();
                        me->GetCreatureListWithEntryInGrid(stalker, NPC_FROST_BUNNY, 200.0f);
                        for (std::list<Creature*>::iterator itr = stalker.begin(); itr != stalker.end(); itr++)
                            if (auto c = *itr)
                            {
                                c->SetObjectScale(1.4f);
                                c->GetMotionMaster()->MoveRandom(5.f);
                                c->CastSpell(me, SPELL_ARCANE_FROST_RAY, true);
                            }
                        break;
                }

            if (!(events.IsInPhase(PHASE_50_PERCENT) || me->HasReactState(REACT_PASSIVE)))
                if (UpdateVictim())
                DoMeleeAttackIfReady();
        }

    private :
        int cnt{ 0 };
        bool die;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_glubtokAI(creature);
    }
};

// 87925
class spell_glubtok_blink : public SpellScriptLoader
{
public:
    spell_glubtok_blink() : SpellScriptLoader("spell_glubtok_blink") { }

    class NonPlayer
    {
    public:
        explicit NonPlayer() {};

        bool operator()(WorldObject* target) const
        {
            return target->GetTypeId() != TYPEID_PLAYER;
        }
    };


    class spell_glubtok_blink_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_glubtok_blink_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove_if(NonPlayer());
        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_glubtok_blink_SpellScript::FilterTargets, EFFECT_0, TARGET_DEST_TARGET_RANDOM);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_glubtok_blink_SpellScript();
    }
};

// 87859 87861
class spell_glubtok_fist : public SpellScriptLoader
{
public:
    spell_glubtok_fist() : SpellScriptLoader("spell_glubtok_fist") { }

    class spell_glubtok_fist_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_glubtok_fist_AuraScript);

        void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (auto caster = GetCaster())
                if (Creature *glubtok = caster->ToCreature())
                    glubtok->AI()->DoAction(ACTION_BLINK);
        }

        void Register()
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_glubtok_fist_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_glubtok_fist_AuraScript();
    }
};

class spell_blottom_fire : public SpellScriptLoader
{
public:
    spell_blottom_fire() : SpellScriptLoader("spell_blottom_fire") { }

    class spell_blottom_fire_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_blottom_fire_SpellScript);

        void HitTargetBunny(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit *caster = GetCaster())
                if (Unit *trigger = caster->FindNearestCreature(NPC_FIRE_BLOSSOM_BUNNY, 200.0f))
                    caster->CastSpell(trigger, 88173, true);
        }

        void HeroicSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit *caster = GetCaster())
                if (Unit *trigger = caster->FindNearestCreature(NPC_FIRE_BLOSSOM_BUNNY, 200.0f))
                    caster->CastSpell(trigger, 91301, true);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_blottom_fire_SpellScript::HitTargetBunny, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            if (m_scriptSpellId == 91286)
                OnEffectHit += SpellEffectFn(spell_blottom_fire_SpellScript::HeroicSummon, EFFECT_1, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_blottom_fire_SpellScript();
    }
};

class spell_blottom_frost : public SpellScriptLoader
{
public:
    spell_blottom_frost() : SpellScriptLoader("spell_blottom_frost") { }

    class spell_blottom_frost_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_blottom_frost_SpellScript);

        void HitTargetBunny(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit *caster = GetCaster())
                if (Unit *trigger = caster->FindNearestCreature(NPC_FROST_BLOSSOM_BUNNY, 200.0f))
                    caster->CastSpell(trigger, 88177, true);
        }

        void HeroicSummon(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            if (Unit *caster = GetCaster())
                if (Unit *trigger = caster->FindNearestCreature(NPC_FROST_BLOSSOM_BUNNY, 200.0f))
                    caster->CastSpell(trigger, 91302, true);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_blottom_frost_SpellScript::HitTargetBunny, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            if (m_scriptSpellId == 91287)
                OnEffectHit += SpellEffectFn(spell_blottom_frost_SpellScript::HeroicSummon, EFFECT_1, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_blottom_frost_SpellScript();
    }
};

// 16184
class achievement_ready_for_raid : public AchievementCriteriaScript
{
public:
    achievement_ready_for_raid() : AchievementCriteriaScript("achievement_ready_for_raid") { }

    bool OnCheck(Player* source, Unit* target)
    {
        if (!target)
            return false;

        /*
        if (InstanceScript *instance = target->GetInstanceScript())
            return instance->GetData(DATA_FIREWALL);
        */
        if (source->HasAura(109326, target->GetGUID()))
            return true;

        return false;
    }
};

// 91397
class spell_gl_firewall : public SpellScriptLoader
{
public:
    spell_gl_firewall() : SpellScriptLoader("spell_gl_firewall") { }

    class spell_gl_firewall_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gl_firewall_SpellScript);

        void CalculateDamage(SpellEffIndex /*effIndex*/)
        {
            if (auto t = GetHitUnit())
                if (auto glubtok = t->FindNearestCreature(NPC_GLUBTOK, 150.f))
                    if (auto aura = t->GetAura(109326, glubtok->GetGUID()))
                        aura->Remove();
                /*
                if (InstanceScript *instance = caster->GetInstanceScript())
                    instance->SetData(DATA_FIREWALL, 0);
                */
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gl_firewall_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gl_firewall_SpellScript();
    }
};


class spell_gl_firewall_b : public SpellScriptLoader
{
public:
    spell_gl_firewall_b() : SpellScriptLoader("spell_gl_firewall_b") { }

    class spell_gl_firewall_b_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gl_firewall_b_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            return true;
        }

        bool Load()
        {
            return true;
        }

        void HandleProc(AuraEffect const* aurEff)
        {
            PreventDefaultAction();
            if (Unit *target = GetCaster())
            {
                Position pos;
                target->GetPosition(&pos);
                float z = target->GetMap()->GetHeight(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                target->CastSpell(pos.m_positionX, pos.m_positionY, z, 91397, true);
            }
        }


        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_gl_firewall_b_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gl_firewall_b_AuraScript();
    }
};

void AddSC_boss_glubtok()
{
    new boss_glubtok();
    new spell_glubtok_blink();
    new spell_glubtok_fist();
    new spell_blottom_fire();
    new spell_blottom_frost();

    new achievement_ready_for_raid();
    new spell_gl_firewall();
    new spell_gl_firewall_b();
}
