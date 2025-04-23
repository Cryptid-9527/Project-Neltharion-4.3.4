/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

 /*
  * SDName: Cape of Stranglethorn
  * SD%Complete: 100% (This CPP + SAI)
  * SDComment: Zone Scripted by Wondi
  */
#include "GameEventMgr.h"
#include "AchievementMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "Player.h"
#include "Vehicle.h"
#include "GameObject.h"
#include "CreatureAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SmartAI.h"
#include "SmartScript.h"

enum CapeOfStranglethornDefaultSpells
{
    SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS = 151235,
    SPELL_RIDE_VEHICLE = 46598,
    SPELL_RIDE_VEHICLE_SEAT_02 = 63313,
    SPELL_EJECT_ALL_PASSENGERS = 50630,
    SPELL_EJECT_PASSENGER_2 = 64630,
    SPELL_DUMMY_PING = 98914,
    SPELL_DUMMY_PING_2 = 98915,
    SPELL_DUMMY_PING_3 = 98916,
    SPELL_DUMMY_PING_4 = 98917,
    SPELL_DUMMY_PING_5 = 98918,
    SPELL_SANCTUARY_NO_COMBAT = 88467,
    SPELL_ROOT = 42716,
    SPELL_DUMMY_SPELLCLICK = 56685,
    SPELL_FLIGHT_SPEED_280 = 50345,
    SPELL_FLIGHT_SPEED_100 = 54421,
    SPELL_PLAYER_UPDATE_AREA = 93425,
};

enum CapeOfStranglethornEvents
{
    EVENT_BACKDOOR_BAT_1 = 1,
    EVENT_BACKDOOR_BAT_2,
    EVENT_SWABBING_1,
    EVENT_SWABBING_2,
    EVENT_SWABBING_3,
    EVENT_SWABBING_4,
    EVENT_SWABBING_5,
    EVENT_SWABBING_6,
    EVENT_SWABBING_7,
    EVENT_SWABBING_8,
    EVENT_SWABBING_9,
    EVENT_SWABBING_10,
    EVENT_SWABBING_11,
    EVENT_SWABBING_12,
    EVENT_SWABBING_13,
    EVENT_SWABBING_14,
    EVENT_OLBLASTY_1,
    EVENT_DRIVEBYPIRACY_1,
    EVENT_DRIVEBYPIRACY_2,
    EVENT_DRIVEBYPIRACY_3,
    EVENT_DRIVEBYPIRACY_4,
    EVENT_DRIVEBYPIRACY_5,
    EVENT_DRIVEBYPIRACY_6,
    EVENT_DRIVEBYPIRACY_7,
    EVENT_FINAL_VOYAGE_1,
    EVENT_UNBAGBWA_1,
    EVENT_UNBAGBWA_2,
    EVENT_UNBAGBWA_3,
    EVENT_ZANZIL_JEKLIK_1,
    EVENT_ZANZIL_JEKLIK_2,
    EVENT_ZANZIL_JEKLIK_3,
    EVENT_ZANZIL_JEKLIK_4,
    EVENT_ZANZIL_JEKLIK_5,
    EVENT_ZANZIL_JEKLIK_6,
    EVENT_ZANZIL_JEKLIK_7,
    EVENT_ZANZIL_JEKLIK_8,
    EVENT_ZANZIL_JEKLIK_9,
    EVENT_ZANZIL_JEKLIK_10,
    EVENT_ZANZIL_JEKLIK_11,
    EVENT_ZANZIL_JEKLIK_12,
    EVENT_ZANZIL_JEKLIK_13,
    EVENT_ZANZIL_JEKLIK_14,
    EVENT_ZANZIL_JEKLIK_15,
    EVENT_ZANZIL_JEKLIK_16,
    EVENT_LIGHTING_BOLT,
    EVENT_ZANZIL_VENOXIS_SUMMON_SPIRIT,
    EVENT_ZANZIL_VENOXIS_1,
    EVENT_ZANZIL_VENOXIS_2,
    EVENT_ZANZIL_VENOXIS_3,
    EVENT_ZANZIL_VENOXIS_4,
    EVENT_ZANZIL_VENOXIS_5,
    EVENT_ZANZIL_VENOXIS_6,
    EVENT_ZANZIL_VENOXIS_7,
    EVENT_ZANZIL_VENOXIS_8,
    EVENT_ZANZIL_VENOXIS_9,
    EVENT_ZANZIL_VENOXIS_10,
    EVENT_ZANZIL_VENOXIS_11,
    EVENT_ZANZIL_VENOXIS_12,
    EVENT_ZANZIL_VENOXIS_13,
    EVENT_VENOXIS_TRANSPORT_1,
    EVENT_VENOXIS_TRANSPORT_2,
    EVENT_VENOXIS_TRANSPORT_3,
    EVENT_SHORT_JOHN_MITHRIL_1,
    EVENT_SHORT_JOHN_MITHRIL_2,
    EVENT_SHORT_JOHN_MITHRIL_3,

    ACTION_ZANZIL_1,
    ACTION_ZANZIL_2,
    ACTION_ZANZIL_3,
    ACTION_ZANZIL_4,
    ACTION_ZANZIL_5,
    ACTION_ZANZIL_6,

    ACTION_TELEPORT_JEKLIK_1,
    ACTION_TELEPORT_JEKLIK_2,
    ACTION_TELEPORT_JEKLIK_3,
    ACTION_TELEPORT_JEKLIK_4,
    ACTION_TELEPORT_JEKLIK_5,
};

// wowhead.com/quest=26550/backdoor-dealings / wowhead.com/quest=26809/backdoor-dealings
enum BackdoorDealingsData
{
    QUEST_BACKDOOR_DEALINGS_H = 26550,
    QUEST_BACKDOOR_DEALINGS_A = 26809,

    SPELL_FLY_STATE_TILTED = 80802,
    SPELL_FLY_STATE_FREEZE_ANIM = 80805,
    SPELL_RIDE_BAT = 80791,
    SPELL_PING_FINISH_EVENT = 151337,
    SPELL_TELEPORT_DELAY = 151338,

    NPC_BACKDOOR_BAT = 43241,
    NPC_BACKDOOR_ZAZIL = 43245,
    NPC_BLACKDOOR_JINDO = 43246,
};

uint32 const BackdoorDealingsBatPathSize = 18;
const G3D::Vector3 BackdoorDealingsBatPath[BackdoorDealingsBatPathSize] =
{
    { -13340.31f, -26.729f, 25.27f, },
    { -13340.24f, -18.698f, 24.59f, },
    { -13343.44f, -1.6720f, 27.33f, },
    { -13369.33f,  4.8911f, 32.04f, },
    { -13399.54f,  20.904f, 52.12f, },
    { -13426.96f,  61.460f, 52.29f, },
    { -13456.12f,  65.959f, 50.63f, },
    { -13480.54f,  31.568f, 51.16f, },
    { -13517.44f, -44.229f, 53.05f, },
    { -13541.66f, -119.50f, 56.32f, },
    { -13571.73f, -209.64f, 46.47f, },
    { -13616.98f, -266.71f, 40.54f, },
    { -13659.10f, -291.87f, 37.40f, },
    { -13700.94f, -333.40f, 27.99f, },
    { -13669.57f, -357.52f, 26.94f, },
    { -13646.66f, -351.07f, 25.56f, },
    { -13643.47f, -328.60f, 21.70f, },
    { -13649.05f, -319.87f, 14.32f, }, // 17
};

class npc_backdoor_dealings_bat : public CreatureScript
{
public:
    npc_backdoor_dealings_bat() : CreatureScript("npc_backdoor_dealings_bat") { }

    struct npc_backdoor_dealings_batAI : public VehicleAI
    {
        npc_backdoor_dealings_batAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->setActive(true);

            if (auto player = summoner->ToPlayer())
            {
                me->SetPhaseMask(4, false);
                _playerGUID = player->GetGUID();
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                player->CastSpell(me, SPELL_RIDE_BAT);
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                _events.ScheduleEvent(EVENT_BACKDOOR_BAT_1, 1000);
            }
            else if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    _summons.DespawnAll();
                    me->DespawnOrUnsummon(1000);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    DoCast(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->ExitVehicle();
                    player->RemoveAura(SPELL_RIDE_BAT);
                    player->CastSpell(player, SPELL_TELEPORT_DELAY);
                }
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_PING_FINISH_EVENT)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    _summons.DespawnAll();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    DoCast(me, SPELL_EJECT_ALL_PASSENGERS);
                    player->ExitVehicle();
                    player->KilledMonsterCredit(NPC_BACKDOOR_ZAZIL);
                    player->CastSpell(player, SPELL_TELEPORT_DELAY);
                    player->RemoveAura(SPELL_RIDE_BAT);
                    me->DespawnOrUnsummon(500);
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 17:
                me->RemoveAura(SPELL_FLY_STATE_TILTED);
                DoCast(me, SPELL_FLY_STATE_FREEZE_ANIM);

                if (me->FindNearestCreature(NPC_BACKDOOR_BAT, 10.0f))
                    return;
                else
                {
                    if (auto zazil = me->SummonCreature(NPC_BACKDOOR_ZAZIL, -13657.547f, -306.09f, 8.174f, 3.227307f, TEMPSUMMON_TIMED_DESPAWN, 120000))
                        if (auto hexxer = me->SummonCreature(NPC_BLACKDOOR_JINDO, -13683.792f, -309.109f, 7.749f, 0.1202f, TEMPSUMMON_TIMED_DESPAWN, 120000))
                        {
                            zazil->setActive(true);
                            hexxer->setActive(true);
                            _summons.Summon(zazil);
                            _summons.Summon(hexxer);
                            _zazilGUID = zazil->GetGUID();
                            _hexxerGUID = hexxer->GetGUID();
                            zazil->SetPhaseMask(4, false);
                            hexxer->SetPhaseMask(4, false);

                            hexxer->SetWalk(true);
                            hexxer->GetMotionMaster()->MovePoint(1, -13665.002f, -306.83f, 8.17f);
                            _events.ScheduleEvent(EVENT_BACKDOOR_BAT_2, 1000);
                        }
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (me->GetDistance(player) >= 100.f)
                    {
                        me->DespawnOrUnsummon();
                        _summons.DespawnAll();
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_BACKDOOR_BAT_1:
                    me->GetMotionMaster()->MoveSmoothPath(BackdoorDealingsBatPath, BackdoorDealingsBatPathSize);
                    DoCast(me, SPELL_FLY_STATE_TILTED);
                    break;
                case EVENT_BACKDOOR_BAT_2:
                    if (auto zazil = Unit::GetCreature(*me, _zazilGUID))
                        if (auto hexxer = Unit::GetCreature(*me, _hexxerGUID))
                        {
                            zazil->AI()->Talk(0);
                            zazil->AI()->TalkWithDelay(4000, 1);
                            hexxer->AI()->TalkWithDelay(12000, 0);
                            hexxer->AI()->TalkWithDelay(18000, 1);
                            zazil->AI()->TalkWithDelay(25500, 2);
                            hexxer->AI()->TalkWithDelay(32000, 2);
                            hexxer->AI()->TalkWithDelay(39000, 3);
                            zazil->AI()->TalkWithDelay(45500, 3);
                            zazil->AI()->TalkWithDelay(52000, 4);
                            me->CastWithDelay(58000, me, SPELL_PING_FINISH_EVENT);
                        }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        uint64 _zazilGUID = 0;
        uint64 _hexxerGUID = 0;
        uint32 _checkTimer = 1000;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_backdoor_dealings_batAI(creature);
    }
};

enum QuestSwabbingDuty
{
    QUEST_SWABBING_DUTY = 26633,

    SPELL_SWABBING_THE_DECKS_AURA = 81349,
    SPELL_ANIM_MOP_STAIN = 81353,
    SPELL_SWABBING_DUTY_CREDIT = 81357,
    SPELL_WARNING_MARK = 81355,

    NPC_STAIN = 43511,
    NPC_DUNCAN = 2545,
};

const Position StainLocations[] =
{
    { -14951.9f, 356.646f, 12.7822f, 4.95952f, },
    { -14953.4f, 355.756f, 12.7371f, 4.97523f, },
    { -14952.3f, 351.727f, 12.8784f, 4.97523f, },
    { -14952.7f, 353.171f, 12.829f, 4.9713f,   },
    { -14951.6f, 349.434f, 12.9357f, 5.02235f, },
    { -14950.9f, 347.097f, 12.8522f, 5.02628f, },
    { -14950.0f, 344.381f, 12.7921f, 5.02628f, },
    { -14950.9f, 342.232f, 12.7326f, 3.21593f, },
    { -14953.3f, 341.26f, 12.7074f, 3.49868f,  },
    { -14954.6f, 340.827f, 12.7203f, 3.45155f, },
    { -14957.9f, 339.76f, 12.7517f, 3.45155f,  },
    { -14961.3f, 338.692f, 12.8048f, 3.45155f, },
    { -14964.6f, 337.633f, 12.8577f, 3.4437f,  },
    { -14969.9f, 335.996f, 12.9439f, 3.42799f, },
    { -14973.3f, 335.007f, 13.2123f, 3.42799f, },
    { -14974.2f, 337.194f, 13.1447f, 1.90039f, },
    { -14975.3f, 340.523f, 13.2324f, 1.88468f, },
    { -14976.4f, 343.852f, 13.2806f, 1.88076f, },
    { -14976.4f, 343.852f, 13.2806f, 1.88076f, },
    { -14977.4f, 347.186f, 13.2029f, 1.88076f, },
    { -14980.1f, 347.406f, 13.2921f, 2.66223f, },
    { -14977.7f, 349.998f, 13.114f, 2.70542f,  },
    { -14976.4f, 351.646f, 13.0127f, 0.270687f,},
    { -14973.5f, 352.426f, 12.9295f, 0.243198f,},
    { -14973.5f, 352.426f, 12.9295f, 0.243198f,},
    { -14966.8f, 354.191f, 12.8195f, 0.262833f,},
    { -14963.4f, 355.1f, 12.7649f, 0.262833f,  },
    { -14960.0f, 356.009f, 12.7151f, 0.262833f,  },
    { -14955.6f, 357.009f, 12.6919f, 0.184293f,},
    { -14953.9f, 357.325f, 12.7398f, 6.22793f, },
    { -14955.5f, 355.174f, 12.746f, 3.40836f,  },
    { -14958.0f, 354.521f, 12.7684f, 3.35731f,   },
    { -14959.5f, 354.2f, 12.776f, 3.34553f,    },
    { -14966.2f, 352.616f, 12.8569f, 3.43585f, },
    { -14968.0f, 352.092f, 12.8877f, 3.40444f,   },
    { -14971.4f, 351.128f, 12.9468f, 3.38087f, },
    { -14970.2f, 353.604f, 12.8659f, 3.34946f, },
    { -14963.8f, 352.61f, 12.8368f, 3.32982f,  },
    { -14961.3f, 353.556f, 12.7972f, 3.80892f, },
    { -14954.6f, 352.816f, 12.837f, 5.01058f,  },
    { -14953.8f, 350.18f, 12.9296f, 4.99094f,  },
    { -14953.8f, 350.18f, 12.9296f, 4.99094f,  },
    { -14954.2f, 343.486f, 12.795f, 3.56152f,  },
    { -14952.3f, 344.937f, 12.8097f, 4.99094f, },
    { -14954.7f, 345.588f, 12.8702f, 1.85328f, },
    { -14953.5f, 347.369f, 12.9031f, 1.49592f, },
    { -14955.6f, 348.453f, 12.9691f, 1.86899f, },
    { -14956.5f, 351.327f, 12.8851f, 1.85328f, },
    { -14958.8f, 351.827f, 12.8607f, 3.267f,   },
    { -14963.0f, 350.717f, 12.8894f, 3.45549f,   },
    { -14966.2f, 349.814f, 12.9336f, 3.35339f, },
    { -14968.5f, 349.33f, 12.9676f, 3.34946f,  },
    { -14971.5f, 348.683f, 13.0132f, 3.37695f, },
    { -14973.5f, 348.197f, 13.0446f, 3.3848f,  },
    { -14975.3f, 346.124f, 13.1489f, 4.98308f, },
    { -14974.3f, 342.933f, 13.2014f, 5.12838f, },
    { -14973.2f, 339.815f, 13.1211f, 5.04591f, },
    { -14972.2f, 337.248f, 13.0559f, 5.12053f, },
    { -14968.3f, 338.739f, 12.9626f, 0.298177f,},
    { -14967.3f, 336.667f, 12.9014f, 0.29425f, },
    { -14965.5f, 339.685f, 12.9172f, 0.290323f,},
    { -14962.4f, 340.573f, 12.8683f, 0.278542f,},
    { -14959.5f, 341.403f, 12.8208f, 0.278542f,},
    { -14957.2f, 342.078f, 12.8024f, 0.278542f,},
    { -14957.0f, 345.069f, 12.8868f, 2.07475f, },
    { -14957.7f, 347.425f, 12.9704f, 2.00171f, },
    { -14960.4f, 349.737f, 12.9294f, 3.34474f, },
    { -14960.3f, 347.246f, 13.0079f, 3.3683f,  },
    { -14959.7f, 344.415f, 12.9136f, 3.3683f,  },
    { -14962.3f, 343.683f, 12.9317f, 3.50574f, },
    { -14964.5f, 342.505f, 12.95f, 3.58821f,   },
    { -14967.7f, 341.61f, 13.0044f, 3.35259f,  },
    { -14970.6f, 340.648f, 13.0482f, 3.4704f,  },
    { -14971.7f, 343.531f, 13.1284f, 1.87997f, },
    { -14972.3f, 345.465f, 13.1124f, 1.83284f, },
    { -14969.6f, 346.292f, 13.0632f, 0.328807f,},
    { -14968.8f, 343.976f, 13.0752f, 0.305245f,},
    { -14966.9f, 344.584f, 13.0469f, 0.305245f,},
    { -14967.7f, 347.077f, 13.0256f, 0.301318f,},
    { -14965.6f, 347.721f, 12.9893f, 0.301318f,},
    { -14964.7f, 344.82f, 13.004f, 0.301318f,  },
    { -14962.9f, 345.967f, 13.0103f, 0.301318f,},
    { -14963.1f, 348.275f, 12.9741f, 0.23456f, },
    { -14969.8f, 342.346f, 13.0651f, 4.95087f, },
    { -14976.9f, 338.643f, 13.304f, 2.97953f,  },
    { -14977.6f, 341.995f, 13.3752f, 1.92317f, },
    { -14978.8f, 344.864f, 13.3445f, 1.986f,   },
    { -14957.7f, 349.605f, 12.9422f, 0.411277f,},
};

class npc_swabbing_duty_controller : public CreatureScript
{
public:
    npc_swabbing_duty_controller() : CreatureScript("npc_swabbing_duty_controller") { }

    struct npc_swabbing_duty_controllerAI : public ScriptedAI
    {
        npc_swabbing_duty_controllerAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                me->setActive(true);
                me->SetPhaseMask(8, false);
                _playerGUID = player->GetGUID();
                summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                DoCast(me, SPELL_ROOT);
                me->NearTeleportTo(-14964.16f, 346.31f, 13.040f, 0.262f);
                _events.ScheduleEvent(EVENT_SWABBING_1, 500);
                _started = true;
            }
            else
                me->DespawnOrUnsummon();
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                Talk(4, _playerGUID);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_2)
            {
                Talk(5, _playerGUID);
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_3)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (auto duncan = me->FindNearestCreature(NPC_DUNCAN, 40.0f))
                    {
                        duncan->AI()->Talk(1, _playerGUID);
                    }

                    _started = false;
                    _events.Reset();
                    Talk(6, _playerGUID);
                    player->RemoveAura(SPELL_SWABBING_THE_DECKS_AURA);
                    _summons.DespawnAll();
                    me->DespawnOrUnsummon();
                }
            }

            if (spellInfo->Id == SPELL_DUMMY_PING_4)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (_step1)
                    {
                        if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                _summons.Summon(stain1);
                                _summons.Summon(stain2);
                                Talk(2, _playerGUID);
                                _step1 = false;
                                _events.ScheduleEvent(EVENT_SWABBING_6, 7000);
                            }
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            if (_step2)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (!_summons.empty())
                    {
                        _summons.RemoveNotExisting();
                    }
                    else
                    {
                        if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        {
                            player->RemoveAura(SPELL_SWABBING_THE_DECKS_AURA);
                            DoCast(me, SPELL_SWABBING_DUTY_CREDIT);
                            _started = false;
                            _events.Reset();
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon();
                            _step2 = false;
                        }
                    }
                }
                else _checkTimer -= diff;
            }

            if (_started)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive())
                    {
                        _started = false;
                        player->RemoveAura(SPELL_SWABBING_THE_DECKS_AURA);
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                        _events.Reset();
                    }

                    if (me->GetDistance(player) >= 20.f || !player->GetAura(SPELL_SWABBING_THE_DECKS_AURA))
                    {
                        Talk(7, _playerGUID);
                        _started = false;
                        player->RemoveAura(SPELL_SWABBING_THE_DECKS_AURA);
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                        _events.Reset();
                    }

                    if (player->GetQuestStatus(QUEST_SWABBING_DUTY) == QUEST_STATUS_NONE)
                    {
                        _started = false;
                        player->RemoveAura(SPELL_SWABBING_THE_DECKS_AURA);
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                        _events.Reset();
                    }
                }
            }

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SWABBING_1:
                    Talk(0, _playerGUID);
                    TalkWithDelay(5000, 1, _playerGUID);
                    _events.ScheduleEvent(EVENT_SWABBING_2, 5000);
                    break;
                case EVENT_SWABBING_2:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        _step1 = true;
                        _summons.Summon(stain1);
                    }
                    break;
                case EVENT_SWABBING_6:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                _summons.Summon(stain1);
                                _summons.Summon(stain2);
                                _summons.Summon(stain3);
                                _events.ScheduleEvent(EVENT_SWABBING_7, 8000);
                            }
                    break;
                case EVENT_SWABBING_7:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                _summons.Summon(stain1);
                                _summons.Summon(stain2);
                                _summons.Summon(stain3);
                                _events.ScheduleEvent(EVENT_SWABBING_8, 8000);
                            }
                    break;
                case EVENT_SWABBING_8:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                _summons.Summon(stain1);
                                _summons.Summon(stain2);
                                _summons.Summon(stain3);
                                TalkWithDelay(8000, 3, _playerGUID);
                                _events.ScheduleEvent(EVENT_SWABBING_9, 10000);
                            }
                    break;
                case EVENT_SWABBING_9:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto stain5 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto stain6 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        {
                                            _summons.Summon(stain1);
                                            _summons.Summon(stain2);
                                            _summons.Summon(stain3);
                                            _summons.Summon(stain4);
                                            _summons.Summon(stain5);
                                            _summons.Summon(stain6);
                                            _events.ScheduleEvent(EVENT_SWABBING_10, 9000);
                                        }
                    break;
                case EVENT_SWABBING_10:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto stain5 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto stain6 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        {
                                            _summons.Summon(stain1);
                                            _summons.Summon(stain2);
                                            _summons.Summon(stain3);
                                            _summons.Summon(stain4);
                                            _summons.Summon(stain5);
                                            _summons.Summon(stain6);
                                            _events.ScheduleEvent(EVENT_SWABBING_11, 6000);
                                        }
                    break;
                case EVENT_SWABBING_11:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    _summons.Summon(stain1);
                                    _summons.Summon(stain2);
                                    _summons.Summon(stain3);
                                    _summons.Summon(stain4);
                                    _events.ScheduleEvent(EVENT_SWABBING_12, 10000);
                                }
                    break;
                case EVENT_SWABBING_12:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto stain5 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto stain6 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                            if (auto stain7 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                                if (auto stain8 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                                {
                                                    _summons.Summon(stain1);
                                                    _summons.Summon(stain2);
                                                    _summons.Summon(stain3);
                                                    _summons.Summon(stain4);
                                                    _summons.Summon(stain5);
                                                    _summons.Summon(stain6);
                                                    _summons.Summon(stain7);
                                                    _summons.Summon(stain8);
                                                    _events.ScheduleEvent(EVENT_SWABBING_13, 7000);
                                                }
                    break;
                case EVENT_SWABBING_13:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto stain5 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        _summons.Summon(stain1);
                                        _summons.Summon(stain2);
                                        _summons.Summon(stain3);
                                        _summons.Summon(stain4);
                                        _summons.Summon(stain5);
                                        _events.ScheduleEvent(EVENT_SWABBING_14, 8000);
                                    }
                    break;
                case EVENT_SWABBING_14:
                    if (auto stain1 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                        if (auto stain2 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                            if (auto stain3 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                if (auto stain4 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (auto stain5 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        if (auto stain6 = me->SummonCreature(NPC_STAIN, StainLocations[urand(0, 87)], TEMPSUMMON_MANUAL_DESPAWN))
                                        {
                                            _summons.Summon(stain1);
                                            _summons.Summon(stain2);
                                            _summons.Summon(stain3);
                                            _summons.Summon(stain4);
                                            _summons.Summon(stain5);
                                            _summons.Summon(stain6);
                                            _step2 = true;
                                        }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _step2 = false;
        bool _step1 = false;
        bool _started = false;
        EventMap _events;
        SummonList _summons;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 100;
        uint16 _checkPlayerInRangeTimer = 2000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swabbing_duty_controllerAI(creature);
    }
};

class npc_swabbing_duty_stain : public CreatureScript
{
public:
    npc_swabbing_duty_stain() : CreatureScript("npc_swabbing_duty_stain") { }

    struct npc_swabbing_duty_stainAI : public ScriptedAI
    {
        npc_swabbing_duty_stainAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto controller = summoner->ToCreature())
            {
                _controllerGUID = controller->GetGUID();
                _events.ScheduleEvent(EVENT_SWABBING_3, 8500);
            }
        }

        void OnSpellClick(Unit* player, bool& result)
        {
            if (auto controller = Unit::GetCreature(*me, _controllerGUID))
            {
                player->CastSpell(me, SPELL_ANIM_MOP_STAIN);
                _events.Reset();
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                controller->CastWithDelay(2000, controller, SPELL_DUMMY_PING_4);
                me->RemoveAura(SPELL_WARNING_MARK);
                me->DespawnOrUnsummon();
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SWABBING_3:
                    DoCast(me, SPELL_WARNING_MARK);
                    _events.ScheduleEvent(EVENT_SWABBING_4, 5000);

                    if (auto controller = Unit::GetCreature(*me, _controllerGUID))
                        DoCast(controller, SPELL_DUMMY_PING);
                    break;
                case EVENT_SWABBING_4:
                    _events.ScheduleEvent(EVENT_SWABBING_5, 5000);

                    if (auto controller = Unit::GetCreature(*me, _controllerGUID))
                        DoCast(controller, SPELL_DUMMY_PING_2);
                    break;
                case EVENT_SWABBING_5:
                    if (auto controller = Unit::GetCreature(*me, _controllerGUID))
                        DoCast(controller, SPELL_DUMMY_PING_3);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        EventMap _events;
        uint64 _controllerGUID = 0;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_swabbing_duty_stainAI(creature);
    }
};

enum OlBlasty
{
    QUEST_OL_BLASTY = 26647,

    NPC_SMILLIN_TIMMY_STICKS = 43560,
    NPC_BLOODSAIL_BOAT = 43561,

    SPELL_FIRE_EFFECT_CANNON = 81507,
};

class npc_ol_blasty_cannon : public CreatureScript
{
public:
    npc_ol_blasty_cannon() : CreatureScript("npc_ol_blasty_cannon") { }

    struct npc_ol_blasty_cannonAI : public VehicleAI
    {
        npc_ol_blasty_cannonAI(Creature* creature) : VehicleAI(creature) {}

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = true;
                me->SetRooted(true);

                if (auto player = passenger->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _onUse = false;
                _playerGUID = 0;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->KilledMonsterCredit(NPC_SMILLIN_TIMMY_STICKS);

                    if (who->isAttackingPlayer())
                    {
                        who->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                        player->CastSpell(me, SPELL_SANCTUARY_NO_COMBAT);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff && _onUse)
            {
                _checkTimer = 2000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (player->GetQuestStatus(QUEST_OL_BLASTY) == QUEST_STATUS_NONE)
                    {
                        player->ExitVehicle();
                        DoCast(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _onUse = false;
                    }

                    if (player->GetQuestStatus(QUEST_OL_BLASTY) == QUEST_STATUS_COMPLETE)
                    {
                        player->ExitVehicle();
                        DoCast(me, SPELL_EJECT_ALL_PASSENGERS);
                        _playerGUID = NULL;
                        _onUse = false;
                    }
                }
            }
            else
                _checkTimer -= diff;
        }
    private:
        bool _onUse = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ol_blasty_cannonAI(creature);
    }
};

uint32 const BlastyBoatPathSize = 9;
const G3D::Vector3 BlastyBoatPath[BlastyBoatPathSize] =
{
    { -14955.000f, 209.414f - 14, 0.25f },
    { -14955.000f, 209.414f - 14, 0.25f },
    { -14965.700f, 231.363f - 14, 0.25f },
    { -14988.599f, 239.190f - 14, 0.25f },
    { -15011.799f, 227.520f - 14, 0.25f },
    { -15021.400f, 205.671f - 14, 0.25f },
    { -15008.200f, 182.796f - 14, 0.25f },
    { -14984.700f, 176.388f - 14, 0.25f },
    { -14960.700f, 186.082f - 14, 0.25f }, // 8
};

class npc_ol_blasty_boat : public CreatureScript
{
public:
    npc_ol_blasty_boat() : CreatureScript("npc_ol_blasty_boat") { }

    struct npc_ol_blasty_boatAI : public VehicleAI
    {
        npc_ol_blasty_boatAI(Creature* creature) : VehicleAI(creature) {}

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            me->setActive(true);

            if (auto* target = me->SummonCreature(NPC_SMILLIN_TIMMY_STICKS, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
            {
                target->setActive(true);
                target->CastWithDelay(200, me, SPELL_RIDE_VEHICLE);
                _events.ScheduleEvent(EVENT_OLBLASTY_1, 1000, 0);
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 8:
                me->GetMotionMaster()->MoveSmoothPath(BlastyBoatPath, BlastyBoatPathSize);
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_FIRE_EFFECT_CANNON)
            {
                DoCast(who, SPELL_DUMMY_PING);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_OLBLASTY_1:
                    me->GetMotionMaster()->MoveSmoothPath(BlastyBoatPath, BlastyBoatPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ol_blasty_boatAI(creature);
    }
};

enum DriveByPiracy
{
    QUEST_DRIVE_BY_PIRACY = 26649,

    NPC_SUMMONED_OARMAN = 43587,
    NPC_DRIVE_BY_PIRACY_CANNON = 43595,
    NPC_DRIVE_BY_PIRACY_BOAT = 43588,

    SPELL_HOLDING_OAR = 81534,
    SPELL_FLYING_200 = 55034,
    SPELL_FLYING_300 = 54422,
};

uint32 const DriveByPiracyBoatPathSize = 10;
const G3D::Vector3 DriveByPiracyBoatPath[DriveByPiracyBoatPathSize] =
{
    { -14854.085f,  273.234f, -0.5 },
    { -14785.864f,  209.144f, -0.5 },
    { -14738.429f,  125.326f, -0.5 },
    { -14671.703f,   90.570f, -0.5 },
    { -14568.262f,    8.814f, -0.5 },
    { -14494.771f,  -28.118f, -0.5 },
    { -14409.625f,  -22.309f, -0.5 },
    { -14307.977f,  -31.471f, -0.5 },
    { -14273.926f,  -82.725f, -0.5 },
    { -14318.404f, -145.247f, -0.5 }, // 9
};

uint32 const DriveByPiracyBoatPathSize2 = 5;
const G3D::Vector3 DriveByPiracyBoatPath2[DriveByPiracyBoatPathSize2] =
{
    { -14332.752f, -177.057f, -0.5f },
    { -14337.797f, -239.233f, -0.5f },
    { -14298.810f, -270.964f, -0.5f },
    { -14237.944f, -276.631f, -0.5f },
    { -14164.298f, -289.438f, -0.5f }, // 4
};

uint32 const DriveByPiracyBoatPathSize3 = 4;
const G3D::Vector3 DriveByPiracyBoatPath3[DriveByPiracyBoatPathSize3] =
{
    { -14124.786f, -301.703f, -0.5f },
    { -14068.258f, -308.003f, -0.5f },
    { -13997.853f, -324.582f, -0.5f },
    { -13967.435f, -314.371f, -0.5f }, // 3
};

uint32 const DriveByPiracyBoatPathSize4 = 4;
const G3D::Vector3 DriveByPiracyBoatPath4[DriveByPiracyBoatPathSize4] =
{
    { -13938.195f, -301.02f, -0.5f },
    { -13892.570f, -298.10f, -0.5f },
    { -13848.581f, -356.41f, -0.5f },
    { -13874.649f, -418.81f, -0.5f },
};

uint32 const DriveByPiracyBoatPathSize5 = 14;
const G3D::Vector3 DriveByPiracyBoatPath5[DriveByPiracyBoatPathSize5] =
{
    { -13908.16f, -427.00f, -0.5f },
    { -14028.51f, -435.88f, -0.5f },
    { -14102.56f, -438.35f, -0.5f },
    { -14184.78f, -406.67f, -0.5f },
    { -14272.71f, -320.30f, -0.5f },
    { -14351.83f, -214.26f, -0.5f },
    { -14414.26f, -131.25f, -0.5f },
    { -14500.49f,  -84.95f, -0.5f },
    { -14613.36f,  -43.21f, -0.5f },
    { -14687.40f,   27.94f, -0.5f },
    { -14771.53f,  123.56f, -0.5f },
    { -14825.09f,  197.03f, -0.5f },
    { -14860.21f,  271.89f, -0.5f },
    { -14867.42f,  292.52f, -0.5f }, // 13
};

uint32 const DriveByPiracyBoatPathSize6 = 10;
const G3D::Vector3 DriveByPiracyBoatPath6[DriveByPiracyBoatPathSize6] =
{
    { -14059.35f, -331.84f, -0.5f },
    { -14052.07f, -458.60f, -0.5f },
    { -14256.13f, -463.82f, -0.5f },
    { -14338.04f, -305.02f, -0.5f },
    { -14376.91f, -187.53f, -0.5f },
    { -14446.06f,  -93.92f, -0.5f },
    { -14612.91f,  -11.97f, -0.5f },
    { -14738.40f,   87.90f, -0.5f },
    { -14833.28f,  216.65f, -0.5f },
    { -14867.88f,  290.03f, -0.5f }, // 9
};

class npc_drive_by_piracy_boat : public CreatureScript
{
public:
    npc_drive_by_piracy_boat() : CreatureScript("npc_drive_by_piracy_boat") { }

    struct npc_drive_by_piracy_boatAI : public VehicleAI
    {
        npc_drive_by_piracy_boatAI(Creature* creature) : VehicleAI(creature), _summons(me) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetPhaseMask(16, false);
            DoCast(me, SPELL_FLYING_200);
            _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_1, 500, 0);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (_pauseCounter)
            {
            case 1:
                if (point == 7)
                {
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        oarman->AI()->TalkWithDelay(1000, 4, _playerGUID);
                        _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_4, 21000, 0);
                    }
                }
                break;
            case 2:
                if (point == 4)
                {
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        oarman->AI()->TalkWithDelay(1000, 6, _playerGUID);
                        _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_5, 15000, 0);
                    }
                }
                break;
            case 3:
                if (point == 3)
                {
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        oarman->AI()->TalkWithDelay(1000, 8, _playerGUID);
                        _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_6, 21000, 0);
                    }
                }
                break;
            case 4:
                if (point == 3)
                {
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        oarman->AI()->TalkWithDelay(1000, 10, _playerGUID);
                        _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_7, 15000, 0);
                    }
                }
                break;
            case 5: // return not completed
                if (point == 13)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon();
                        onBoat = false;
                    }
                }
                break;
            case 10: // return with completed q
                if (point == 9)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(500);
                        onBoat = false;
                    }
                }
                break;
            default:
                break;
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                _summons.DespawnAll();
                me->DespawnOrUnsummon();
                onBoat = false;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (onBoat)
                    {
                        if (!player->isAlive())
                        {
                            player->ExitVehicle();
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon();
                            onBoat = false;
                        }

                        if (player->GetQuestStatus(QUEST_DRIVE_BY_PIRACY) == QUEST_STATUS_NONE)
                        {
                            player->ExitVehicle();
                            _summons.DespawnAll();
                            me->DespawnOrUnsummon();
                            onBoat = false;
                        }
                    }

                    if (!_complete)
                    {
                        if (player->GetQuestStatus(QUEST_DRIVE_BY_PIRACY) == QUEST_STATUS_COMPLETE)
                        {
                            if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                            {
                                _complete = true;
                                me->GetMotionMaster()->Clear();
                                DoCast(me, SPELL_FLYING_300);
                                _pauseCounter = 10;
                                me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath6, DriveByPiracyBoatPathSize6);
                                oarman->AI()->Talk(3, _playerGUID);
                                _events.Reset();
                            }
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_DRIVEBYPIRACY_1:
                    if (auto oarman = me->SummonCreature(NPC_SUMMONED_OARMAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto cannon = me->SummonCreature(NPC_DRIVE_BY_PIRACY_CANNON, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _oarmanGUID = oarman->GetGUID();
                            _cannonGUID = cannon->GetGUID();
                            _summons.Summon(oarman);
                            _summons.Summon(cannon);
                            oarman->CastSpell(oarman, SPELL_HOLDING_OAR);
                            oarman->EnterVehicle(me, 0);
                            cannon->EnterVehicle(me, 1);
                            _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_2, 500, 0);
                        }
                    break;
                case EVENT_DRIVEBYPIRACY_2:
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto cannon = Unit::GetCreature(*me, _cannonGUID))
                        {
                            onBoat = true;
                            player->CastSpell(cannon, SPELL_RIDE_VEHICLE);
                            _events.ScheduleEvent(EVENT_DRIVEBYPIRACY_3, 1000, 0);
                        }
                    break;
                case EVENT_DRIVEBYPIRACY_3:
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        _pauseCounter = 1;
                        me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath, DriveByPiracyBoatPathSize);
                        oarman->AI()->TalkWithDelay(2000, 0, _playerGUID);
                        oarman->AI()->TalkWithDelay(9000, 1, _playerGUID);
                        oarman->AI()->TalkWithDelay(16000, 2, _playerGUID);
                    }
                    break;
                case EVENT_DRIVEBYPIRACY_4:
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        _pauseCounter++;
                        oarman->AI()->TalkWithDelay(500, 5, _playerGUID);
                        me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath2, DriveByPiracyBoatPathSize2);

                    }
                    break;
                case EVENT_DRIVEBYPIRACY_5:
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        _pauseCounter++;
                        oarman->AI()->TalkWithDelay(500, 7, _playerGUID);
                        me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath3, DriveByPiracyBoatPathSize3);

                    }
                    break;
                case EVENT_DRIVEBYPIRACY_6:
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        _pauseCounter++;
                        oarman->AI()->TalkWithDelay(500, 9, _playerGUID);
                        me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath4, DriveByPiracyBoatPathSize4);
                    }
                    break;
                case EVENT_DRIVEBYPIRACY_7:
                    if (auto oarman = Unit::GetCreature(*me, _oarmanGUID))
                    {
                        _pauseCounter++;
                        me->GetMotionMaster()->MoveSmoothPath(DriveByPiracyBoatPath5, DriveByPiracyBoatPathSize5);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint16 _pauseCounter = 0;
        bool onBoat = false;
        bool _complete = false;
        uint16 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        uint64 _oarmanGUID = 0;
        uint64 _cannonGUID = 0;
        EventMap _events;
        SummonList _summons;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drive_by_piracy_boatAI(creature);
    }
};

class npc_drive_by_piracy_cannon : public CreatureScript
{
public:
    npc_drive_by_piracy_cannon() : CreatureScript("npc_drive_by_piracy_cannon") { }

    struct npc_drive_by_piracy_cannonAI : public VehicleAI
    {
        npc_drive_by_piracy_cannonAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto boat = summoner->ToCreature())
                _cannonGUID = boat->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId*/, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _playerGUID = passenger->GetGUID();
                _onBoard = true;
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                if (_onBoard)
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                        if (auto cannon = Unit::GetCreature(*me, _cannonGUID))
                        {
                            if (player->GetQuestStatus(QUEST_DRIVE_BY_PIRACY) == QUEST_STATUS_COMPLETE)
                            {
                                DoCast(cannon, SPELL_DUMMY_PING);
                                _onBoard = false;
                            }
                        }
            }
        }
    private:
        bool _onBoard = false;
        uint16 _checkTimer = 1000;
        uint64 _cannonGUID = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drive_by_piracy_cannonAI(creature);
    }
};

enum QuestOurMortalEnemies
{
    QUEST_OUR_MORTAL_ENEMIES = 26648,

    NPC_DREDERICK_MCGUMM = 43557,

    SPELL_DETECT_NINJAS = 81471,
    SPELL_DETECT_NINJAS_COMBAT = 81464,
    // ID 81498  OUR_MORTAL_ENEMIES  DEAD - EYE_AREA_AURA
    // ID 81501  OUR_MORTAL_ENEMIES  CANCEL_DEAD - EYE_AREA_AURA
};

class npc_drederick_mcgumm : public CreatureScript
{
public:
    npc_drederick_mcgumm() : CreatureScript("npc_drederick_mcgumm") { }

    struct npc_drederick_mcgummAI : public ScriptedAI
    {
        npc_drederick_mcgummAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            me->SetReactState(REACT_PASSIVE);
            me->CastWithDelay(2000, me, SPELL_DETECT_NINJAS);
            TalkWithDelay(2000, 0, _playerGUID);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void UpdateAI(uint32 const diff) override
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    if (!player->isAlive() || me->GetAreaId() != 297)
                        me->DespawnOrUnsummon();

                    if (player->GetQuestStatus(QUEST_OUR_MORTAL_ENEMIES) == QUEST_STATUS_NONE)
                        me->DespawnOrUnsummon();

                    if (!_rewarded)
                    {
                        if (player->GetQuestStatus(QUEST_OUR_MORTAL_ENEMIES) == QUEST_STATUS_REWARDED)
                        {
                            player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                            DoCast(me, SPELL_ROOT);
                            me->SetFacingToObject(player);
                            TalkWithDelay(1000, 1, _playerGUID);
                            me->DespawnOrUnsummon(5000);
                            _rewarded = true;
                        }
                    }

                    if (!player->getVictim() && !player->isInCombat())
                    {
                        me->GetMotionMaster()->MoveFollow(player, 1.0f, 0.6f * M_PI);
                    }
                }
            }
            else _checkTimer -= diff;
        }
    private:
        bool _rewarded = false;
        uint64 _playerGUID = 0;
        uint16 _checkTimer = 1000;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drederick_mcgummAI(creature);
    }
};

uint32 const WindRiderPathSize = 3;
const G3D::Vector3 WindRiderPath[WindRiderPathSize] =
{
    { -14439.057f, 515.131f, 33.08f},
    { -14405.340f, 549.378f, 38.49f},
    { -14295.020f, 610.124f, 28.60f}, // 2
};

uint32 const WindRiderPathSize2 = 24;
const G3D::Vector3 WindRiderPath2[WindRiderPathSize2] =
{
    { -14271.61f, 627.91f, 42.0f },
    { -14227.39f, 612.40f, 42.0f },
    { -14173.01f, 612.95f, 42.0f },
    { -14130.41f, 604.39f, 42.0f },
    { -14094.05f, 553.88f, 42.0f },
    { -14056.62f, 545.61f, 42.0f },
    { -14038.72f, 581.42f, 42.0f },
    { -14049.41f, 607.34f, 42.0f },
    { -14084.70f, 666.15f, 42.0f },
    { -14063.00f, 736.60f, 42.0f },
    { -14000.06f, 755.79f, 42.0f },
    { -13936.99f, 758.05f, 42.0f },
    { -13903.43f, 776.50f, 42.0f },
    { -13909.46f, 820.23f, 42.0f },
    { -13953.98f, 852.66f, 42.0f },
    { -14018.47f, 851.23f, 42.0f },
    { -14081.75f, 821.86f, 42.0f },
    { -14161.73f, 808.18f, 42.0f },
    { -14273.56f, 822.46f, 42.0f },
    { -14388.80f, 827.87f, 42.0f },
    { -14432.20f, 783.60f, 42.0f },
    { -14422.70f, 728.99f, 42.0f },
    { -14348.54f, 656.98f, 42.0f },
    { -14301.90f, 640.15f, 42.0f }, // 23
};

uint32 const WindRiderPathSize3 = 8;
const G3D::Vector3 WindRiderPath3[WindRiderPathSize3] =
{
    { -14178.89f, 653.06f, 57.90f },
    { -14156.73f, 690.77f, 55.34f },
    { -14175.09f, 738.03f, 61.92f },
    { -14231.02f, 718.68f, 62.25f },
    { -14280.53f, 636.42f, 59.79f },
    { -14347.41f, 577.13f, 51.03f },
    { -14403.83f, 541.14f, 41.93f },
    { -14442.44f, 515.71f, 29.05f }, // 7
};

enum FinalVoyage
{
    QUEST_FINAL_VOYAGE = 26697,
    SPELL_BACK_TO_BOOTYBAAAYYYY = 81887,
};

class npc_final_voyage_wind_rider : public CreatureScript
{
public:
    npc_final_voyage_wind_rider() : CreatureScript("npc_final_voyage_wind_rider") { }

    struct npc_final_voyage_wind_riderAI : public VehicleAI
    {
        npc_final_voyage_wind_riderAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto player = summoner->ToPlayer())
            {
                player->CastSpell(player, SPELL_SANCTUARY_NO_COMBAT);
                player->CastSpell(player, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);
                _playerGUID = player->GetGUID();
            }
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                _events.ScheduleEvent(EVENT_FINAL_VOYAGE_1, 500);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_BACK_TO_BOOTYBAAAYYYY && !_activated)
            {
                _phase = 2;
                _activated = true;
                me->GetMotionMaster()->Clear();
                DoCast(me, SPELL_FLYING_300);
                me->GetMotionMaster()->MoveSmoothPath(WindRiderPath3, WindRiderPathSize3);
                _events.Reset();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (point == 2 && _phase == 0)
            {
                me->GetMotionMaster()->MoveSmoothPath(WindRiderPath2, WindRiderPathSize2);
                _phase = 1; // enable spell back to bootybay
            }

            if (_phase == 1 && point == 23)
            {
                _events.Reset();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveSmoothPath(WindRiderPath2, WindRiderPathSize2);
            }

            if (_phase == 2 && point == 7)
            {
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    _events.Reset();
                    me->GetMotionMaster()->Clear();
                    me->DespawnOrUnsummon();
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (!_complete)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (player->GetQuestStatus(26697) == QUEST_STATUS_COMPLETE)
                        {
                            _complete = true;
                            me->GetMotionMaster()->Clear();
                            DoCast(me, SPELL_FLYING_300);
                            me->GetMotionMaster()->MoveSmoothPath(WindRiderPath3, WindRiderPathSize3);
                            _events.Reset();
                            _phase = 2;
                        }
                    }
                }
            }
            else _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_FINAL_VOYAGE_1:
                    DoCast(me, SPELL_FLYING_200);
                    me->GetMotionMaster()->MoveSmoothPath(WindRiderPath, WindRiderPathSize);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _activated = false;
        bool _complete = false;
        uint8 _phase = 0;
        uint32 _checkTimer = 1000;
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_final_voyage_wind_riderAI(creature);
    }
};

enum TheDamselBadLuck
{
    NPC_ROPE_FROM_DOCK = 43721,
    NPC_ROPE_TO_DOCK = 43722,
};

uint32 const DamselRopePathSize = 7;
const G3D::Vector3 DamselRopePath[DamselRopePathSize] =
{
    { -14435.54f, 473.79f, 15.76f },
    { -14430.80f, 480.16f, 14.71f },
    { -14416.35f, 496.34f, 12.50f },
    { -14405.98f, 501.35f, 11.95f },
    { -14394.69f, 502.93f, 16.23f },
    { -14384.89f, 500.24f, 20.60f },
    { -14375.57f, 488.98f, 21.42f }, // 6
};

uint32 const DamselRopePathSize1 = 6;
const G3D::Vector3 DamselRopePath1[DamselRopePathSize1] =
{
    { -14380.04f, 490.804f, 16.51f },
    { -14383.93f, 497.327f, 18.34f },
    { -14395.74f, 504.600f, 19.98f },
    { -14411.04f, 499.101f, 19.19f },
    { -14424.25f, 488.789f, 17.73f },
    { -14439.12f, 473.267f, 15.76f }, // 5
};

class npc_damsel_rope_go_in_out : public CreatureScript
{
public:
    npc_damsel_rope_go_in_out() : CreatureScript("npc_damsel_rope_go_in_out") { }

    struct npc_damsel_rope_go_in_outAI : public VehicleAI
    {
        npc_damsel_rope_go_in_outAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            summoner->CastSpell(summoner, SPELL_SANCTUARY_NO_COMBAT);
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();

            me->SetReactState(REACT_PASSIVE);
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (me->GetEntry() == NPC_ROPE_FROM_DOCK)
            {
                if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    me->GetMotionMaster()->MoveSmoothPath(DamselRopePath, DamselRopePathSize);
                }

                if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon();
                }
            }

            if (me->GetEntry() == NPC_ROPE_TO_DOCK)
            {
                if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    me->GetMotionMaster()->MoveSmoothPath(DamselRopePath1, DamselRopePathSize1);
                }

                if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon();
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 5:
                if (me->GetEntry() == NPC_ROPE_TO_DOCK)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->GetMotionMaster()->Clear();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon();
                    }
                }
                break;
            case 6:
                if (me->GetEntry() == NPC_ROPE_FROM_DOCK)
                {
                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        player->ExitVehicle();
                        me->GetMotionMaster()->Clear();
                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        me->DespawnOrUnsummon();
                    }
                }
                break;
            default:
                break;
            }
        }

    private:
        uint64 _playerGUID = 0;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_damsel_rope_go_in_outAI(creature);
    }
};

uint32 const WindRiderBloodsailsEndPathSize = 15;
const G3D::Vector3 WindRiderBloodsailsEndPath[WindRiderBloodsailsEndPathSize] =
{
    { -14437.29f, 517.82f, 30.02f },
    { -14418.94f, 537.36f, 30.38f },
    { -14356.71f, 634.65f, 26.32f },
    { -14368.68f, 673.91f, 31.89f },
    { -14422.50f, 698.17f, 31.89f },
    { -14477.17f, 696.52f, 30.54f },
    { -14506.69f, 705.69f, 24.46f },
    { -14501.63f, 720.74f, 22.72f },
    { -14468.32f, 731.25f, 20.43f },
    { -14434.08f, 738.22f, 20.07f },
    { -14385.54f, 735.53f, 17.64f },
    { -14348.57f, 717.34f, 15.71f },
    { -14319.56f, 679.01f, 22.78f },
    { -14290.84f, 660.38f, 21.95f },
    { -14250.79f, 672.51f, 13.49f }, // 14
};

class npc_bloodsails_end_wind_rider : public CreatureScript
{
public:
    npc_bloodsails_end_wind_rider() : CreatureScript("npc_bloodsails_end_wind_rider") { }

    struct npc_bloodsails_end_wind_riderAI : public VehicleAI
    {
        npc_bloodsails_end_wind_riderAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            summoner->CastSpell(summoner, SPELL_SANCTUARY_NO_COMBAT);
            summoner->CastSpell(summoner, SPELL_DISMOUNT_AND_CANCEL_ALL_SHAPESHIFTS);

            if (auto player = summoner->ToPlayer())
                _playerGUID = player->GetGUID();
        }

        void PassengerBoarded(Unit* passenger, int8 /* seatId */, bool apply) override
        {
            if (apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(me, SPELL_FLYING_300);
                me->GetMotionMaster()->MoveSmoothPath(WindRiderBloodsailsEndPath, WindRiderBloodsailsEndPathSize);
            }

            if (!apply && passenger->GetTypeId() == TYPEID_PLAYER)
            {
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->DespawnOrUnsummon();
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 14:
                if (auto player = Unit::GetPlayer(*me, _playerGUID))
                {
                    player->ExitVehicle();
                    me->GetMotionMaster()->Clear();
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->DespawnOrUnsummon();
                }
                break;
            default:
                break;
            }
        }

    private:
        uint64 _playerGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsails_end_wind_riderAI(creature);
    }
};

enum BloodsailsEnd
{
    QUEST_BLOODSAILS_END = 26703,
    QUEST_BLOODSAILS_END_TRACKER = 30180,

    SPELL_BLOODSAILS_END_SLEEPING = 82054,
    SPELL_BLOODSAILS_END_STUN_SELF = 81981,
    SPELL_BLOODSAILS_END_SUMMON_YELL = 82026,
    SPELL_BLOODSAILS_END_TELEPORT = 81991,
    SPELL_BLOODSAILS_END_SPACER_AURA = 81986,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_SELF = 81963,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_01 = 81909,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_02 = 81916,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_03 = 81918,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_04 = 81920,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_05 = 81921,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_06 = 81922,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_07 = 81923,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_08 = 81924,
    SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_09 = 81925,
    SPELL_BLOODSAILS_END_REMOVE_QUEST_AURAS = 151340,
    SPELL_CHECK_AURA = 151100,
    SPELL_BLACKOUT = 89315,
};

class npc_bloodsails_end_scene_controller : public CreatureScript
{
public:
    npc_bloodsails_end_scene_controller() : CreatureScript("npc_bloodsails_end_scene_controller") { }

    struct npc_bloodsails_end_scene_controllerAI : public ScriptedAI
    {
        npc_bloodsails_end_scene_controllerAI(Creature* creature) : ScriptedAI(creature) {}

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                auto player = me->GetPlayersInRange(50, true);
                for (auto it : player)
                    if (!it->GetAura(SPELL_BLOODSAILS_END_SPACER_AURA))
                    {
                        it->CastSpell(it, SPELL_BLOODSAILS_END_SPACER_AURA);
                        it->CastWithDelay(600, it, SPELL_BLOODSAILS_END_SUMMON_YELL);
                        it->CastWithDelay(6300, it, SPELL_BLOODSAILS_END_SLEEPING);
                        it->CastWithDelay(6300, it, SPELL_BLOODSAILS_END_STUN_SELF);
                        it->CastWithDelay(6000, it, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_SELF);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_01);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_02);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_03);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_04);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_05);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_06);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_07);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_08);
                        me->CastWithDelay(6000, me, SPELL_BLOODSAILS_END_SUMMON_EXPLOSION_09);
                        it->CastWithDelay(9950, it, SPELL_BLACKOUT);
                        it->CastWithDelay(11800, it, SPELL_BLOODSAILS_END_TELEPORT);
                        it->CastWithDelay(11800, it, SPELL_BLOODSAILS_END_REMOVE_QUEST_AURAS);
                    }
            }
        }
    private:
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodsails_end_scene_controllerAI(creature);
    }
};

// quest=26598/the-heart-of-mokk
enum HeartOfMokk
{
    NPC_SILVERBACK_GORILLA = 1511,
    NPC_KONDA = 1516,
    NPC_MOKK = 1514,

    ITEM_HEART_OF_MOKK = 2797,

    QUEST_HEART_OF_MOKK = 26598,
    QUEST_STRANGLETHORN_FEVER = 26597,
};

const Position HeartOfMokkSpawnPosition[] =
{
    {-13787.22f, 6.2627f, 35.53f, 5.88f},
};

class npc_unbagbwa_stranglethorn_fever : public CreatureScript
{
public:
    npc_unbagbwa_stranglethorn_fever() : CreatureScript("npc_unbagbwa_stranglethorn_fever") { }

    bool OnQuestReward(Player* player, Creature* creature, Quest const* _Quest, uint32)
    {
        if (_Quest->GetQuestId() == QUEST_HEART_OF_MOKK)
            player->CastSpell(creature, SPELL_DUMMY_PING_2);

        return true;
    }

    struct npc_unbagbwa_stranglethorn_feverAI : public ScriptedAI
    {
        npc_unbagbwa_stranglethorn_feverAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void JustDied(Unit* /*killer*/)
        {
            if (_started)
            {
                _summons.DespawnAll();
                _started = false;
                _killCount = 0;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                _events.Reset();
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING_2 && !_started)
            {
                Talk(0);
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                _started = true;
                _killCount = 0;
                _events.ScheduleEvent(EVENT_UNBAGBWA_1, 3000);

                if (auto player = who->ToPlayer())
                    _playerGUID = player->GetGUID();
            }

            if (spellInfo->Id == SPELL_DUMMY_PING && _started)
            {
                ++_killCount;

                switch (_killCount)
                {
                case 1:
                    _events.ScheduleEvent(EVENT_UNBAGBWA_2, 6000);
                    break;
                case 2:
                    _events.ScheduleEvent(EVENT_UNBAGBWA_3, 6000);
                    break;
                case 3:
                {
                    EnterEvadeMode();
                    _started = false;
                    _killCount = 0;
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    _events.Reset();
                    Talk(1);

                    auto character = me->GetPlayersInRange(40, true);
                    for (auto it : character)
                        if (it->GetQuestStatus(QUEST_STRANGLETHORN_FEVER) == QUEST_STATUS_INCOMPLETE)
                            it->AddItem(ITEM_HEART_OF_MOKK, 1);
                }
                break;
                default:
                    break;
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            if (_started)
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (auto player = Unit::GetPlayer(*me, _playerGUID))
                    {
                        if (me->GetDistance(player) > 38.f)
                        {
                            EnterEvadeMode();
                            _summons.DespawnAll();
                            _started = false;
                            _killCount = 0;
                            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            _events.Reset();
                        }
                    }
                }
            _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_UNBAGBWA_1:
                    if (auto enemy1 = me->SummonCreature(NPC_SILVERBACK_GORILLA, HeartOfMokkSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy1);
                        enemy1->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                    }
                    break;
                case EVENT_UNBAGBWA_2:
                    if (auto enemy2 = me->SummonCreature(NPC_KONDA, HeartOfMokkSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy2);
                        enemy2->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                    }
                    break;
                case EVENT_UNBAGBWA_3:
                    if (auto enemy3 = me->SummonCreature(NPC_MOKK, HeartOfMokkSpawnPosition[0], TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        _summons.Summon(enemy3);
                        enemy3->SetHomePosition(me->GetPositionX() + 2, me->GetPositionY() + 2, me->GetPositionZ(), me->GetOrientation());
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _started = false;
        uint16 _checkTimer = 2000;
        uint8 _killCount = 0;
        uint64 _playerGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbagbwa_stranglethorn_feverAI(creature);
    }
};

enum ThroughTheTrollHole
{
    QUEST_THROUGH_THE_TROLL_HOLE_A = 26811,
    QUEST_THROUGH_THE_TROLL_HOLE_H = 26552,

    QUEST_PLUNGING_INTO_ZULGURUB_A = 26813,
    QUEST_PLUNGING_INTO_ZULGURUB_H = 26554,

    NPC_THROUGH_THE_TROLL_HOLE_CREDIT = 43253,
    NPC_PLUNGING_INTO_ZG_CREDIT = 43321,
};

class spell_teleport_to_zg_through : public SpellScriptLoader
{
public:
    spell_teleport_to_zg_through() : SpellScriptLoader("spell_teleport_to_zg_through") { }

    class spell_teleport_to_zg_through_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_teleport_to_zg_through_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetHitUnit();
            if (auto player = caster->ToPlayer())
            {
                if (player->GetAreaId() == 311)
                {
                    if (player->GetQuestStatus(QUEST_THROUGH_THE_TROLL_HOLE_A) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_THROUGH_THE_TROLL_HOLE_H) == QUEST_STATUS_INCOMPLETE)
                        player->KilledMonsterCredit(NPC_THROUGH_THE_TROLL_HOLE_CREDIT);
                }

                if (player->GetAreaId() == 19)
                {
                    if (player->GetQuestStatus(QUEST_PLUNGING_INTO_ZULGURUB_A) == QUEST_STATUS_INCOMPLETE ||
                        player->GetQuestStatus(QUEST_PLUNGING_INTO_ZULGURUB_H) == QUEST_STATUS_INCOMPLETE)
                        player->KilledMonsterCredit(NPC_PLUNGING_INTO_ZG_CREDIT);
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_teleport_to_zg_through_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_teleport_to_zg_through_SpellScript();
    }
};

// quest=26553/HIGH-PRIESTESS-JEKLIK H & quest=26812/high-priestess-jeklik A
enum HighPriestessJeklik
{
    NPC_ZANZIL_THE_OUTCAST_1 = 43255,
    NPC_HIGH_PRIESTESS_JEKLIK = 43257,

    QUEST_HIGH_PRIESTESS_JEKLIK_H = 26553,
    QUEST_HIGH_PRIESTESS_JEKLIK_A = 26812,

    SPELL_HIGH_PRIESTESS_JEKLIK_JEKLIK_DEAD_VISUAL = 80837,
    SPELL_HIGH_PRIESTESS_JEKLIK_JEKLIK_HEALING_VISUAL = 80838,
    SPELL_HIGH_PRIESTESS_JEKLIK_AOE_CREDIT = 80841,
    SPELL_HIGH_PRIESTESS_JEKLIK_TROLL_EXPLOSION = 80844,
    SPELL_HIGH_PRIESTESS_JEKLIK_BONE = 80845,
    SPELL_HIGH_PRIESTESS_JEKLIK_MEAT = 80846,
    SPELL_HIGH_PRIESTESS_JEKLIK_BLOODY_MEAT = 80847,
    SPELL_HIGH_PRIESTESS_JEKLIK_TELEPORT_JEKLIK = 80854,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_THREAT = 80880,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_TELEPORT_05 = 80853,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_TELEPORT_04 = 80882,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_TELEPORT_03 = 80883,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_TELEPORT_02 = 80884,
    SPELL_HIGH_PRIESTESS_JEKLIK_ZANZIL_TELEPORT_01 = 80885,
    SPELL_HIGH_PRIESTESS_JEKLIK_FACE = 80888,

    SPELL_INVISIBILITY_MISC_17  = 84886,
    SPELL_DETECT_MISC_17        = 94570,
    SPELL_LIGHTING_BOLT         = 9532,
    SPELL_ZANZIL_HEAL           = 80839,
    SPELL_DROP_ZANZIL_ELIXIR    = 81078,
    SPELL_COSMETIC_TELEPORT     = 52096,

    SPELL_COSMETIC_GREEN_BEAM   = 40227,
    SPELL_VOODOO_MOVE           = 81944,
};

const Position ZazilTeleportPosition[] =
{
    { -12285.92f,  -1454.14f,  130.60f, 6.16f},
    { -12278.473f, -1417.28f, 130.589f, 4.28f},
    { -12237.342f, -1422.990f, 130.62f, 3.08f},
    { -12217.467f, -1438.207f, 130.60f, 2.69f},
    { -12220.522f, -1470.460f, 130.60f, 1.56f},
};

uint32 const ZanzilEscapePathSize = 3;
const G3D::Vector3 ZanzilEscapePath[ZanzilEscapePathSize] =
{
    { -12217.89f, -1455.85f, 130.59f }, // 0
    { -12228.25f, -1451.32f, 130.75f }, // 1
    { -12228.25f, -1451.32f, 130.75f }, // 2
};


class npc_zanzil_the_outcast_jeklik : public CreatureScript
{
public:
    npc_zanzil_the_outcast_jeklik() : CreatureScript("npc_zanzil_the_outcast_jeklik") { }

    struct npc_zanzil_the_outcast_jeklikAI : public ScriptedAI
    {
        npc_zanzil_the_outcast_jeklikAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            _place = 1;
            me->setActive(true);
            _isKilled = false;
            _started = false;

            if (auto jeklik = me->SummonCreature(NPC_HIGH_PRIESTESS_JEKLIK, -12219.59f, -1464.739f, 131.691f, 4.59f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
            {
                _summons.Summon(jeklik);
                _jeklikGUID = jeklik->GetGUID();
                jeklik->SetReactState(REACT_PASSIVE);
                jeklik->setRegeneratingHealth(false);
                jeklik->SetHealth(jeklik->GetMaxHealth() * 0.6f);
                jeklik->CastSpell(jeklik, SPELL_HIGH_PRIESTESS_JEKLIK_JEKLIK_DEAD_VISUAL);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                _fightInProgress = true;
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;

            if ((me->GetHealthPct() <= 10) && !_isKilled)
            {
                me->setRegeneratingHealth(false);
                _isKilled = true;
                
                switch (_place)
                {
                case 1:
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_1, 200);
                    break;
                case 2:
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_5, 200);
                    break;
                case 3:
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_8, 200);
                    break;
                case 4:
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_11, 200);
                    break;
                case 5:
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_14, 200);
                    break;
                case 6:
                    _place = 1;
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_1, 200);
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_place == 0)
            {
                if (point == 2 && _fightInProgress)
                {
                    _fightInProgress = false;
                    DoCast(me, SPELL_COSMETIC_TELEPORT);
                    me->DespawnOrUnsummon(1000);
                }
            }
        }

        void DoAction(int32 const actionId)
        {
            switch (actionId)
            {
            case ACTION_ZANZIL_1:
                _events.Reset();
                me->SetVisible(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->RemoveAllAttackers();
                TalkWithDelay(1000, 2);
                TalkWithDelay(4000, 3);
                TalkWithDelay(8000, 4);
                _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_3, 8000);
                break;
            case ACTION_ZANZIL_2:
                _events.Reset();
                me->RemoveAllAttackers();
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastStop();
                me->RemoveAura(SPELL_ZANZIL_HEAL);

                TalkWithDelay(2000, 3);
                TalkWithDelay(6000, 4);
                _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_3, 6000);
                break;
            default:
                break;
            }
        }
        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(1000, 2000));

            if (!_started)
            {
                Talk(0);
                _started = true;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            //if (me->HasUnitState(UNIT_STATE_CASTING))
                //return;

            if (me->isInCombat() && !me->GetAura(SPELL_ZANZIL_HEAL) && !_isKilled)
                DoCast(me, SPELL_ZANZIL_HEAL);

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_LIGHTING_BOLT:
                    if (auto v = me->getVictim())
                        DoCast(v, SPELL_LIGHTING_BOLT);
                    _events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(5000, 7000));
                    break;
                case EVENT_ZANZIL_JEKLIK_3: // finish
                    _place = 0;
                    me->GetMotionMaster()->MoveSmoothPath(ZanzilEscapePath, ZanzilEscapePathSize);
                    break;
                case EVENT_ZANZIL_JEKLIK_1:
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                        me->SetFacingToObject(jeklik);

                    me->RemoveAllAttackers();
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    me->RemoveAura(SPELL_ZANZIL_HEAL);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_2, 1000);
                    break;
                case EVENT_ZANZIL_JEKLIK_2:
                    _isKilled = false;
                    me->SetVisible(false);
                    me->SetFullHealth();
                    me->setRegeneratingHealth(true);
                    me->NearTeleportTo(ZazilTeleportPosition[0]);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_4, 8000);
                    break;
                case EVENT_ZANZIL_JEKLIK_4: // first tp place
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                    {
                        _place++;
                        DoCast(me, SPELL_ZANZIL_HEAL);
                        me->setRegeneratingHealth(false);
                        me->SetVisible(true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        jeklik->AI()->TalkWithDelay(1000, 0);
                        DoCast(jeklik, SPELL_COSMETIC_GREEN_BEAM, false);
                        jeklik->AI()->DoAction(ACTION_TELEPORT_JEKLIK_1);
                    }
                    break;
                case EVENT_ZANZIL_JEKLIK_5:
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                        me->SetFacingToObject(jeklik);

                    me->RemoveAllAttackers();
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    me->RemoveAura(SPELL_ZANZIL_HEAL);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_6, 1000);
                    break;
                case EVENT_ZANZIL_JEKLIK_6:
                    _isKilled = false;
                    me->SetFullHealth();
                    me->SetVisible(false);
                    me->setRegeneratingHealth(true);
                    me->NearTeleportTo(ZazilTeleportPosition[1]);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_7, 8000);
                    break;
                case EVENT_ZANZIL_JEKLIK_7: // second tp place
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                    {
                        _place++;
                        DoCast(me, SPELL_ZANZIL_HEAL);
                        me->setRegeneratingHealth(false);
                        me->SetVisible(true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        jeklik->AI()->TalkWithDelay(1000, 0);
                        DoCast(jeklik, SPELL_COSMETIC_GREEN_BEAM, false);
                        jeklik->AI()->DoAction(ACTION_TELEPORT_JEKLIK_2);
                    }
                    break;
                case EVENT_ZANZIL_JEKLIK_8:
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                        me->SetFacingToObject(jeklik);

                    me->RemoveAllAttackers();
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    me->RemoveAura(SPELL_ZANZIL_HEAL);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_9, 1000);
                    break;
                case EVENT_ZANZIL_JEKLIK_9:
                    _isKilled = false;
                    me->SetFullHealth();
                    me->SetVisible(false);
                    me->setRegeneratingHealth(true);
                    me->NearTeleportTo(ZazilTeleportPosition[2]);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_10, 8000);
                    break;
                case EVENT_ZANZIL_JEKLIK_10: // 3rd tp place
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                    {
                        _place++;
                        me->setRegeneratingHealth(false);
                        me->SetVisible(true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        jeklik->AI()->TalkWithDelay(1000, 0);
                        DoCast(jeklik, SPELL_COSMETIC_GREEN_BEAM, false);
                        jeklik->AI()->DoAction(ACTION_TELEPORT_JEKLIK_3);
                    }
                    break;
                case EVENT_ZANZIL_JEKLIK_11:
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                        me->SetFacingToObject(jeklik);

                    me->RemoveAllAttackers();
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    me->RemoveAura(SPELL_ZANZIL_HEAL);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_12, 1000);
                    break;
                case EVENT_ZANZIL_JEKLIK_12:
                    _isKilled = false;
                    me->SetFullHealth();
                    me->SetVisible(false);
                    me->setRegeneratingHealth(true);
                    me->NearTeleportTo(ZazilTeleportPosition[3]);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_13, 8000);
                    break;
                case EVENT_ZANZIL_JEKLIK_13: // 4th tp place
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                    {
                        _place++;
                        DoCast(me, SPELL_ZANZIL_HEAL);
                        me->setRegeneratingHealth(false);
                        me->SetVisible(true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        jeklik->AI()->TalkWithDelay(1000, 0);
                        DoCast(jeklik, SPELL_COSMETIC_GREEN_BEAM, false);
                        jeklik->AI()->DoAction(ACTION_TELEPORT_JEKLIK_4);
                    }
                    break;
                case EVENT_ZANZIL_JEKLIK_14:
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                        me->SetFacingToObject(jeklik);

                    me->RemoveAllAttackers();
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    me->RemoveAura(SPELL_ZANZIL_HEAL);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_15, 1000);
                    break;
                case EVENT_ZANZIL_JEKLIK_15:
                    _isKilled = false;
                    me->SetFullHealth();
                    me->SetVisible(false);
                    me->setRegeneratingHealth(true);
                    me->NearTeleportTo(ZazilTeleportPosition[4]);
                    _events.ScheduleEvent(EVENT_ZANZIL_JEKLIK_16, 8000);
                    break;
                case EVENT_ZANZIL_JEKLIK_16: // 5th tp place
                    if (auto jeklik = Unit::GetCreature(*me, _jeklikGUID))
                    {
                        _place++;
                        DoCast(me, SPELL_ZANZIL_HEAL);
                        me->setRegeneratingHealth(false);
                        me->SetVisible(true);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        jeklik->AI()->TalkWithDelay(1000, 0);
                        DoCast(jeklik, SPELL_COSMETIC_GREEN_BEAM, false);
                        jeklik->AI()->DoAction(ACTION_TELEPORT_JEKLIK_5);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _started = false;
        bool _isKilled = false;
        bool _fightInProgress = false;
        uint16 _place = 1;
        uint64 _jeklikGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_the_outcast_jeklikAI(creature);
    }
};

uint32 const JeklikTeleportPathSize = 7;
const G3D::Vector3 JeklikTeleportPath[JeklikTeleportPathSize] =
{
    { -12219.455f, -1464.718f, 133.93f },
    { -12219.455f, -1464.718f, 134.69f },
    { -12220.619f, -1464.579f, 136.19f },
    { -12268.374f, -1459.409f, 136.55f },
    { -12277.010f, -1459.861f, 136.70f },
    { -12279.214f, -1459.586f, 135.35f },
    { -12282.360f, -1458.841f, 131.69f }, // 6
};

uint32 const JeklikTeleportPathSize2 = 5;
const G3D::Vector3 JeklikTeleportPath2[JeklikTeleportPathSize2] =
{
    { -12282.2636f, -1457.8214f, 134.04531f },
    { -12282.2636f, -1457.8214f, 136.39732f },
    { -12282.1025f, -1436.1066f, 135.84899f },
    { -12281.8320f, -1418.7833f, 134.94517f },
    { -12281.5927f, -1416.7280f, 131.69209f }, // 4
};
uint32 const JeklikTeleportPathSize3 = 5;
const G3D::Vector3 JeklikTeleportPath3[JeklikTeleportPathSize3] =
{
    { -12281.37890f, -1416.78576f, 133.04570f },
    { -12279.64355f, -1417.24829f, 134.45283f },
    { -12252.67871f, -1417.68798f, 135.25997f },
    { -12244.46289f, -1416.66308f, 135.06764f },
    { -12242.87890f, -1416.28942f, 131.69192f }, // 4
};

uint32 const JeklikTeleportPathSize4 = 5;
const G3D::Vector3 JeklikTeleportPath4[JeklikTeleportPathSize4] =
{
    { -12242.55566f, -1416.18713f, 133.95594f },
    { -12240.36816f, -1417.61010f, 136.64692f },
    { -12229.25585f, -1424.81811f, 136.70475f },
    { -12218.37890f, -1431.49499f, 135.50058f },
    { -12216.59082f, -1432.70764f, 131.69210f }, // 4
};

uint32 const JeklikTeleportPathSize5 = 4;
const G3D::Vector3 JeklikTeleportPath5[JeklikTeleportPathSize5] =
{
    { -12216.18554f, -1433.64819f, 133.77363f },
    { -12216.20117f, -1435.24536f, 136.31060f },
    { -12218.97949f, -1456.56250f, 134.85781f },
    { -12219.43554f, -1464.76245f, 131.69201f }, // 3
};

class npc_high_priestess_jeklik : public CreatureScript
{
public:
    npc_high_priestess_jeklik() : CreatureScript("npc_high_priestess_jeklik") { }

    struct npc_high_priestess_jeklikAI : public ScriptedAI
    {
        npc_high_priestess_jeklikAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto zanzil = summoner->ToCreature())
                _zanzilGUID = zanzil->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            DoCast(me, SPELL_ROOT);
        }

        void DoAction(int32 const actionId)
        {
            if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
            {
                switch (actionId)
                {
                case ACTION_TELEPORT_JEKLIK_1:
                    _place = 1;
                    DoCast(me, SPELL_VOODOO_MOVE);
                    me->GetMotionMaster()->MoveSmoothPath(JeklikTeleportPath, JeklikTeleportPathSize);
                    break;
                case ACTION_TELEPORT_JEKLIK_2:
                    _place = 2;
                    DoCast(me, SPELL_VOODOO_MOVE);
                    me->GetMotionMaster()->MoveSmoothPath(JeklikTeleportPath2, JeklikTeleportPathSize2);
                    break;
                case ACTION_TELEPORT_JEKLIK_3:
                    _place = 3;
                    DoCast(me, SPELL_VOODOO_MOVE);
                    me->GetMotionMaster()->MoveSmoothPath(JeklikTeleportPath3, JeklikTeleportPathSize3);
                    break;
                case ACTION_TELEPORT_JEKLIK_4:
                    _place = 4;
                    DoCast(me, SPELL_VOODOO_MOVE);
                    me->GetMotionMaster()->MoveSmoothPath(JeklikTeleportPath4, JeklikTeleportPathSize4);
                    break;
                case ACTION_TELEPORT_JEKLIK_5:
                    _place = 5;
                    DoCast(me, SPELL_VOODOO_MOVE);
                    me->GetMotionMaster()->MoveSmoothPath(JeklikTeleportPath5, JeklikTeleportPathSize5);
                    break;
                default:
                    break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            switch (point)
            {
            case 3:
                if (_place == 5)
                    me->RemoveAura(SPELL_VOODOO_MOVE);
                break;
            case 4:
                if (_place == 2 || _place == 3 || _place == 2)
                    me->RemoveAura(SPELL_VOODOO_MOVE);
                break;
            case 6:
                if (_place == 1)
                    me->RemoveAura(SPELL_VOODOO_MOVE);
                break;
            default:
                break;
            }
        }

        void JustDied(Unit* killer)
        {
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_AOE_CREDIT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BONE);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BLOODY_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BONE);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BLOODY_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BONE);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BONE);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BLOODY_MEAT);
            DoCast(me, SPELL_HIGH_PRIESTESS_JEKLIK_BLOODY_MEAT);

            if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
            {
                if (zanzil->IsVisible())
                    zanzil->AI()->DoAction(ACTION_ZANZIL_2);
                else
                    zanzil->AI()->DoAction(ACTION_ZANZIL_1);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
                {
                    if (!zanzil->isAlive())
                        me->DespawnOrUnsummon();
                }
            }
            _checkTimer -= diff;
        }

    private:
        uint8  _place = 1;
        uint32 _checkTimer = 1000;
        uint64 _zanzilGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_priestess_jeklikAI(creature);
    }
};

// quest=26555/high-priest-venoxis# H & quest=26814/high-priest-venoxis A
enum HighPriestessVenoxis
{
    NPC_ZANZIL_THE_OUTCAST_2 = 43322,
    NPC_HIGH_PRIESTESS_VENOXIS = 43323,
    NPC_SHADE_OF_THE_HEXXER = 43332,
    NPC_SPIRIT_OF_HEXXER = 43839,
    NPC_ZANZIL_ELIKSIR_SUMMON_TARGET = 74931,

    QUEST_HIGH_PRIESTESS_VENOXIS_H = 26555,
    QUEST_HIGH_PRIESTESS_VENOXIS_A = 26814,

    SPELL_HIGH_PRIEST_VENOXIS_FORCE_FACE_VENOXIS = 80923,
    SPELL_HIGH_PRIEST_VENOXIS_VENOXIS_DEAD_VISUAL = 80931,
    SPELL_HIGH_PRIEST_VENOXIS_VENOXIS_HEALING_VISUAL = 80932,
    SPELL_HIGH_PRIEST_VENOXIS_ZANZIL_TELEPORT_01 = 80934,
    SPELL_HIGH_PRIEST_VENOXIS_ZANZIL_TELEPORT_02 = 80935,
    SPELL_HIGH_PRIEST_VENOXIS_ZANZIL_TELEPORT_03 = 80936,
    SPELL_HIGH_PRIEST_VENOXIS_ZANZIL_TELEPORT_04 = 80937,
    SPELL_HIGH_PRIEST_VENOXIS_ZANZIL_TELEPORT_05 = 80938,
    SPELL_HIGH_PRIEST_VENOXIS_TELEPORT_VENOXIS = 80939,
    SPELL_HIGH_PRIEST_VENOXIS_TELEPORT_OUT_H = 80955,
    SPELL_HIGH_PRIEST_VENOXIS_TELEPORT_OUT_A = 82607,
    SPELL_HIGH_PRIEST_VENOXIS_SUMMON_HEXXER = 80961, // npc id 43332
    SPELL_HIGH_PRIEST_VENOXIS_JIN_DO_S_TELEPORT = 81012,
    SPELL_JIN_DO_S_GRASP = 81011,
    SPELL_JIN_DO_S_RAGE = 81002,
    SPELL_HIGH_PRIEST_VENOXIS_FORCECAST_SUMMON_RESCUE_VEHICLE = 80960,
    SPELL_HIGH_PRIEST_VENOXIS_SUMMON_RESCUE_VEHICLE = 80956,
    SPELL_HIGH_PRIEST_VENOXIS_SUMMON_BALLOON_H = 82605, // npc id 43335
    SPELL_HIGH_PRIEST_VENOXIS_SUMMON_PLANE_A = 82606, // npc id 44156
    SPELL_HIGH_PRIEST_VENOXIS_SUMMON_SPIRIT = 80924,

    SPELL_AURA_CHECK = 151100,
    SPELL_HEAL_5SEC = 83849,
    // SPELL_LIGHTING_BOLT = 9532,
    SPELL_ZANZIL_ELIXIR = 80924,
};

const Position HexxerLocations[] =
{
    { -11993.96f, -1665.33f, 32.32f, 4.5f }, // hexxer summon position
    { -11996.22f, -1679.64f, 32.28f, 4.5f }, // move point
    { -12071.77f, -1578.063f, 102.55f, 5.14f }, // ballon/plane
};

const Position ZanzilTeleportLocations[] =
{
    { -12031.742f, -1693.030f, 39.297f, 3.7f },
    { -12019.013f, -1714.621f, 39.313f, 3.7f },
    { -11975.220f, -1700.724f, 32.285f, 3.7f },
    { -11997.398f, -1703.773f, 32.284f, 3.7f },
    { -11980.344f, -1680.873f, 32.282f, 3.7f },
    { -12003.882f, -1686.914f, 32.282f, 3.7f },
    { -12002.629f, -1669.510f, 32.282f, 3.7f },
};

const Position SpiritLocations[] =
{
    { -12027.68f, -1710.41f, 39.312f },
    { -12025.25f, -1708.74f, 39.314f },
    { -12023.68f, -1706.63f, 39.314f },
    { -12024.06f, -1703.90f, 39.314f },
    { -12026.39f, -1701.78f, 39.311f },
    { -12029.63f, -1700.77f, 39.314f },
    { -12032.55f, -1702.24f, 39.352f },
    { -12034.12f, -1704.90f, 39.609f },
};

const Position PlayerLocations[] =
{
    { -11991.43f, -1678.93f, 32.27f, 3.0f },
    { -11992.78f, -1682.09f, 32.27f, 3.0f },
    { -11996.63f, -1682.94f, 32.27f, 3.0f },
    { -12000.08f, -1681.88f, 32.27f, 3.0f },
    { -12001.16f, -1679.91f, 32.27f, 3.0f },
    { -12001.20f, -1676.75f, 32.27f, 3.0f },
    { -11997.72f, -1675.53f, 32.27f, 3.0f },
};

class npc_zanzil_the_outcast_venoxis : public CreatureScript
{
public:
    npc_zanzil_the_outcast_venoxis() : CreatureScript("npc_zanzil_the_outcast_venoxis") { }

    struct npc_zanzil_the_outcast_venoxisAI : public ScriptedAI
    {
        npc_zanzil_the_outcast_venoxisAI(Creature* creature) : ScriptedAI(creature), _summons(me) {}

        void Reset()
        {
            me->setActive(true);
            _isKilled = false;
            _fightInProgress = true;
            _sayStartText = true;

            if (auto venoxis = me->SummonCreature(NPC_HIGH_PRIESTESS_VENOXIS, -12028.5f, -1705.92f, 41.97f, 5.218f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000))
            {
                _summons.Summon(venoxis);
                _venoxisGUID = venoxis->GetGUID();
                venoxis->SetReactState(REACT_PASSIVE);
                venoxis->setRegeneratingHealth(false);
                venoxis->SetHealth(venoxis->GetMaxHealth() / 2);
                venoxis->CastSpell(venoxis, SPELL_HIGH_PRIEST_VENOXIS_VENOXIS_DEAD_VISUAL);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                _fightInProgress = true;
            }
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;

            if ((me->GetHealthPct() <= 10) && !_isKilled)
            {
                _isKilled = true;
                _events.CancelEvent(EVENT_ZANZIL_VENOXIS_SUMMON_SPIRIT);
                _events.CancelEvent(EVENT_LIGHTING_BOLT);
                _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_1, 200);
            }
        }

        void DoAction(int32 const actionId)
        {
            switch (actionId)
            {
            case ACTION_ZANZIL_3:
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastStop();
                TalkWithDelay(3000, 4);
                _events.Reset();

                if (auto hexxer = me->SummonCreature(NPC_SHADE_OF_THE_HEXXER, HexxerLocations[0], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _summons.Summon(hexxer);
                    _hexxerGUID = hexxer->GetGUID();
                    hexxer->SetWalk(true);
                    hexxer->GetMotionMaster()->MovePoint(1, HexxerLocations[1]);
                    hexxer->AI()->TalkWithDelay(1000, 0);
                    me->SetFacingToObject(hexxer);
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_5, 5000);
                }
                break;
            case ACTION_ZANZIL_4:
                me->SetVisible(true);
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastStop();
                TalkWithDelay(1000, 4);
                _events.Reset();

                if (auto hexxer = me->SummonCreature(NPC_SHADE_OF_THE_HEXXER, HexxerLocations[0], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _summons.Summon(hexxer);
                    _hexxerGUID = hexxer->GetGUID();
                    hexxer->SetWalk(true);
                    hexxer->GetMotionMaster()->MovePoint(1, HexxerLocations[1]);
                    hexxer->AI()->TalkWithDelay(3000, 0);
                    me->SetFacingToObject(hexxer);
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_5, 5000);
                }
                break;
            case ACTION_ZANZIL_5:
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastStop();
                TalkWithDelay(1000, 6);
                _events.Reset();

                if (auto hexxer = me->SummonCreature(NPC_SHADE_OF_THE_HEXXER, HexxerLocations[0], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _summons.Summon(hexxer);
                    _hexxerGUID = hexxer->GetGUID();
                    hexxer->SetWalk(true);
                    hexxer->GetMotionMaster()->MovePoint(1, HexxerLocations[1]);
                    hexxer->AI()->TalkWithDelay(3000, 0);
                    me->SetFacingToObject(hexxer);
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_5, 5000);
                }
                break;
            case ACTION_ZANZIL_6:
                me->SetVisible(true);
                DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->CastStop();
                TalkWithDelay(1000, 6);
                _events.Reset();

                if (auto hexxer = me->SummonCreature(NPC_SHADE_OF_THE_HEXXER, HexxerLocations[0], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    _summons.Summon(hexxer);
                    _hexxerGUID = hexxer->GetGUID();
                    hexxer->SetWalk(true);
                    hexxer->GetMotionMaster()->MovePoint(1, HexxerLocations[1]);
                    hexxer->AI()->TalkWithDelay(3000, 0);
                    me->SetFacingToObject(hexxer);
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_5, 5000);
                }
                break;
            default:
                break;
            }
        }
        void EnterCombat(Unit* /*who*/)
        {
            _events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(3000, 8000));
            _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_SUMMON_SPIRIT, urand(2000, 4000));

            if (_sayStartText)
            {
                Talk(0);
                _sayStartText = false;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            DoMeleeAttackIfReady();

            //if (me->HasUnitState(UNIT_STATE_CASTING))
            //    return;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ZANZIL_VENOXIS_SUMMON_SPIRIT:
                {
                    me->CastStop();

                    if (auto target = me->FindNearestCreature(NPC_ZANZIL_ELIKSIR_SUMMON_TARGET, 39.9f))
                        DoCast(target, SPELL_HIGH_PRIEST_VENOXIS_SUMMON_SPIRIT);

                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_SUMMON_SPIRIT, urand(11000, 12000));
                    break;
                }
                case EVENT_LIGHTING_BOLT:
                    if (auto victim = me->getVictim())
                        DoCast(victim, SPELL_LIGHTING_BOLT);

                    _events.ScheduleEvent(EVENT_LIGHTING_BOLT, urand(5000, 8000));
                    break;
                case EVENT_ZANZIL_VENOXIS_1:
                    DoCast(me, SPELL_SANCTUARY_NO_COMBAT);
                    DoCast(me, SPELL_DROP_ZANZIL_ELIXIR);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    Talk(1);
                    me->CastStop();
                    _events.Reset();
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_2, 1000);
                    break;
                case EVENT_ZANZIL_VENOXIS_2:
                    me->SetVisible(false);
                    me->SetFullHealth();

                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_3, 15000);
                    break;
                case EVENT_ZANZIL_VENOXIS_3:
                    me->NearTeleportTo(ZanzilTeleportLocations[urand(0, 6)]);
                    _isKilled = false;
                    me->SetVisible(true);
                    TalkWithDelay(1000, 2);
                    Talk(3);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);

                    if (auto player = me->FindNearestPlayer(70.0f))
                        me->AI()->AttackStart(player);
                    break;
                case EVENT_ZANZIL_VENOXIS_5:
                    if (auto hexxer = Unit::GetCreature(*me, _hexxerGUID))
                    {
                        hexxer->AI()->Talk(1);
                        TalkWithDelay(4000, 5);
                        hexxer->AI()->TalkWithDelay(9000, 2);
                        hexxer->CastWithDelay(9000, hexxer, SPELL_JIN_DO_S_GRASP);
                        _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_9, 9000);
                    }
                    break;
                case EVENT_ZANZIL_VENOXIS_9:
                {
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_10, 5000);

                    auto player = me->GetPlayersInRange(100, true);
                    for (auto it : player)
                    {
                        if (it->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_H) == QUEST_STATUS_INCOMPLETE ||
                            it->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_H) == QUEST_STATUS_COMPLETE ||
                            it->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_A) == QUEST_STATUS_INCOMPLETE ||
                            it->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_A) == QUEST_STATUS_COMPLETE)
                        {
                            it->NearTeleportTo(PlayerLocations[urand(0, 6)]);
                        }
                    }
                    break;
                }
                case EVENT_ZANZIL_VENOXIS_10:

                    if (auto hexxer = Unit::GetCreature(*me, _hexxerGUID))
                    {
                        if (hexxer->GetPhaseMask() == 1024) // ALLIANCE PHASE
                        {
                            hexxer->AI()->Talk(3);
                            hexxer->AI()->TalkWithDelay(5000, 4);
                            hexxer->AI()->TalkWithDelay(9000, 5);
                            hexxer->CastWithDelay(9000, hexxer, SPELL_JIN_DO_S_RAGE);
                            _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_11, 7000);
                        }

                        if (hexxer->GetPhaseMask() == 2048) // HORDE PHASE
                        {
                            hexxer->AI()->Talk(3);
                            hexxer->AI()->TalkWithDelay(5000, 4);
                            hexxer->AI()->TalkWithDelay(9000, 5);
                            hexxer->CastWithDelay(9000, hexxer, SPELL_JIN_DO_S_RAGE);
                            _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_12, 7000);
                        }
                    }
                    break;
                case EVENT_ZANZIL_VENOXIS_11:
                    if (auto plane = me->SummonCreature(44156, HexxerLocations[2], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        _transportGUID = plane->GetGUID();
                        _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_13, 22500);
                    }
                    break;
                case EVENT_ZANZIL_VENOXIS_12:
                    if (auto ballon = me->SummonCreature(43335, HexxerLocations[2], TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        _transportGUID = ballon->GetGUID();
                        _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_13, 22500);
                    }
                    break;
                case EVENT_ZANZIL_VENOXIS_13:
                    if (auto transport = Unit::GetCreature(*me, _transportGUID))
                    {
                        DoCast(transport, SPELL_DUMMY_PING);
                        _summons.DespawnAll();
                        me->DespawnOrUnsummon(100);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _sayStartText = true;
        bool _isKilled = false;
        bool _fightInProgress = false;
        uint64 _venoxisGUID = 0;
        uint64 _hexxerGUID = 0;
        uint64 _transportGUID = 0;
        EventMap _events;
        SummonList _summons;
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zanzil_the_outcast_venoxisAI(creature);
    }
};

class npc_high_priest_venoxis_quest : public CreatureScript
{
public:
    npc_high_priest_venoxis_quest() : CreatureScript("npc_high_priest_venoxis_quest") { }

    struct npc_high_priest_venoxis_questAI : public ScriptedAI
    {
        npc_high_priest_venoxis_questAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto zanzil = summoner->ToCreature())
                _zanzilGUID = zanzil->GetGUID();

            me->SetReactState(REACT_PASSIVE);
            DoCast(me, SPELL_ROOT);
        }

        void DamageTaken(Unit* attacker, uint32& damage)
        {
            if (damage >= me->GetHealth())
                damage = 0;

            if ((me->GetHealthPct() <= 4) && !_almostDead)
            {
                me->setRegeneratingHealth(false);
                _almostDead = true;
                _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_4, 0);
            }

            if ((me->GetHealthPct() >= 92) && !_almostHealed)
            {
                _almostHealed = true;
                Talk(0);
                _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_6, 2000);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (_checkTimer <= diff)
            {
                _checkTimer = 1000;

                if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
                {
                    if (!zanzil->isAlive())
                        me->DespawnOrUnsummon();
                }

                if (me->FindNearestCreature(NPC_SPIRIT_OF_HEXXER, 5.0f))
                {
                    std::list<Unit*> spirit = me->SelectNearbyUnits(NPC_SPIRIT_OF_HEXXER, 5.0f);
                    for (std::list<Unit*>::iterator itr = spirit.begin(); itr != spirit.end(); ++itr)
                        if (auto spirit = (*itr))
                        {
                            if (!spirit->GetAura(SPELL_AURA_CHECK))
                            {
                                spirit->CastSpell(spirit, SPELL_AURA_CHECK);
                                spirit->GetMotionMaster()->MovePoint(1, SpiritLocations[urand(0, 7)]);
                            }
                        }
                }
            }
            _checkTimer -= diff;

            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ZANZIL_VENOXIS_4:
                    if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
                    {
                        if (zanzil->IsVisible())
                            zanzil->AI()->DoAction(ACTION_ZANZIL_3);
                        else
                            zanzil->AI()->DoAction(ACTION_ZANZIL_4);
                    }
                    break;
                case EVENT_ZANZIL_VENOXIS_6:
                    if (auto zanzil = Unit::GetCreature(*me, _zanzilGUID))
                    {
                        if (zanzil->IsVisible())
                            zanzil->AI()->DoAction(ACTION_ZANZIL_5);
                        else
                            zanzil->AI()->DoAction(ACTION_ZANZIL_6);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    private:
        bool _almostHealed = false;
        bool _almostDead = false;
        uint32 _checkTimer = 1000;
        uint64 _zanzilGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_priest_venoxis_questAI(creature);
    }
};

class npc_spirit_of_venoxis_summoned : public CreatureScript
{
public:
    npc_spirit_of_venoxis_summoned() : CreatureScript("npc_spirit_of_venoxis_summoned") { }

    struct npc_spirit_of_venoxis_summonedAI : public ScriptedAI
    {
        npc_spirit_of_venoxis_summonedAI(Creature* creature) : ScriptedAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (auto zanzil = summoner->ToCreature())
                _zanzilGUID = zanzil->GetGUID();

            me->SetReactState(REACT_PASSIVE);

            _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_7, 1000);
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (point)
            {
            case 1:
                if (auto venoxis = me->FindNearestCreature(NPC_HIGH_PRIESTESS_VENOXIS, 20.0f))
                {
                    DoCast(me, SPELL_ROOT);
                    DoCast(venoxis, SPELL_HEAL_5SEC);
                    _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_8, 8000);
                    me->SetFacingToObject(venoxis);
                }
                break;
            default:
                break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_ZANZIL_VENOXIS_7:
                    if (auto venoxis = me->FindNearestCreature(NPC_HIGH_PRIESTESS_VENOXIS, 50.0f))
                        me->GetMotionMaster()->MoveChase(venoxis, 1.0f, frand(0.0, 6.0));
                    break;
                case EVENT_ZANZIL_VENOXIS_8:
                    if (auto venoxis = me->FindNearestCreature(NPC_HIGH_PRIESTESS_VENOXIS, 50.0f))
                    {
                        DoCast(venoxis, SPELL_HEAL_5SEC);
                        _events.ScheduleEvent(EVENT_ZANZIL_VENOXIS_8, 8000);
                    }
                    break;
                }
            }
        }

    private:
        uint32 _checkTimer = 1000;
        uint64 _zanzilGUID = 0;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spirit_of_venoxis_summonedAI(creature);
    }
};

enum TransportFromZulGurub
{
    NPC_EXPLORERS_LEAGUE_BIPLANE = 44156,
    NPC_BRONWYN_HEWSTRIKE = 44155,
    NPC_ROPE_LADDER_A = 44157,

    NPC_HARDWRENCH_TOUR_BALLOON = 43335,
    NPC_SASSY_HARDWRENCH = 43333,
    NPC_ROPE_LADDER_H = 43334,

};

uint32 const TransportPathSize = 4;
const G3D::Vector3 TransportPath[TransportPathSize] =
{
    { -12036.611f, -1596.66f, 89.94f, },
    { -12044.361f, -1623.15f, 91.58f, },
    { -12027.459f, -1649.44f, 85.35f, },
    { -11996.42f, -1681.34f, 46.0f, }, // 3
};

class npc_venoxis_quest_rescue_transport : public CreatureScript
{
public:
    npc_venoxis_quest_rescue_transport() : CreatureScript("npc_venoxis_quest_rescue_transport") { }

    struct npc_venoxis_quest_rescue_transportAI : public VehicleAI
    {
        npc_venoxis_quest_rescue_transportAI(Creature* creature) : VehicleAI(creature) {}

        void IsSummonedBy(Unit* summoner)
        {
            if (me->GetEntry() == NPC_EXPLORERS_LEAGUE_BIPLANE)
            {
                _events.ScheduleEvent(EVENT_VENOXIS_TRANSPORT_1, 500, 0);
            }

            if (me->GetEntry() == NPC_HARDWRENCH_TOUR_BALLOON)
            {
                _events.ScheduleEvent(EVENT_VENOXIS_TRANSPORT_2, 500, 0);
            }
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (auto actor = Unit::GetCreature(*me, _actorGUID))
                    if (auto ladder = Unit::GetCreature(*me, _ladderGUID))
                    {
                        actor->DespawnOrUnsummon();
                        ladder->DespawnOrUnsummon();
                        me->DespawnOrUnsummon(100);
                    }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_VENOXIS_TRANSPORT_1:
                    if (auto hewstrike = me->SummonCreature(NPC_BRONWYN_HEWSTRIKE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto ladder = me->SummonCreature(NPC_ROPE_LADDER_A, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _actorGUID = hewstrike->GetGUID();
                            _ladderGUID = ladder->GetGUID();
                            hewstrike->EnterVehicle(me, 1);
                            ladder->EnterVehicle(me, 0);
                            _events.ScheduleEvent(EVENT_VENOXIS_TRANSPORT_3, 500, 0);
                        }
                    break;
                case EVENT_VENOXIS_TRANSPORT_2:
                    if (auto sassy = me->SummonCreature(NPC_SASSY_HARDWRENCH, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        if (auto ladder = me->SummonCreature(NPC_ROPE_LADDER_H, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            _actorGUID = sassy->GetGUID();
                            _ladderGUID = ladder->GetGUID();
                            sassy->EnterVehicle(me, 1);
                            ladder->EnterVehicle(me, 0);
                            _events.ScheduleEvent(EVENT_VENOXIS_TRANSPORT_3, 500, 0);
                        }
                    break;
                case EVENT_VENOXIS_TRANSPORT_3:
                    me->GetMotionMaster()->MoveSmoothPath(TransportPath, TransportPathSize);

                    if (auto actor = Unit::GetCreature(*me, _actorGUID))
                    {
                        actor->AI()->TalkWithDelay(2000, 0);
                        actor->AI()->TalkWithDelay(5000, 1);
                    }
                    break;
                default:
                    break;
                }
            }
        }
    private:
        uint64 _actorGUID = 0;
        uint64 _ladderGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venoxis_quest_rescue_transportAI(creature);
    }
};

// ID - 80956 High Priest Venoxis : Summon Rescue Vehicle
class spell_venoxis_summon_rescue_vehicle : public SpellScriptLoader
{
public:
    spell_venoxis_summon_rescue_vehicle() : SpellScriptLoader("spell_venoxis_summon_rescue_vehicle") { }

    class spell_venoxis_summon_rescue_vehicle_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_venoxis_summon_rescue_vehicle_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetHitUnit();
            if (auto player = caster->ToPlayer())
            {
                if (player->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_H) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_H) == QUEST_STATUS_COMPLETE)
                    player->CastSpell(player, SPELL_HIGH_PRIEST_VENOXIS_SUMMON_BALLOON_H);

                if (player->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_A) == QUEST_STATUS_INCOMPLETE ||
                    player->GetQuestStatus(QUEST_HIGH_PRIESTESS_VENOXIS_A) == QUEST_STATUS_INCOMPLETE)
                    player->CastSpell(player, SPELL_HIGH_PRIEST_VENOXIS_SUMMON_PLANE_A);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_venoxis_summon_rescue_vehicle_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_venoxis_summon_rescue_vehicle_SpellScript();
    }
};

enum GurubashiEvent
{
    EVENT_GURUBASHI_ARENA = 16,
    NPC_SHORT_JOHN_MITHRIL = 14508,
    OBJECT_ARENA_TREASURE_CHEST = 179697,
};

class npc_gurubashi_challenge_controller : public CreatureScript
{
public:
    npc_gurubashi_challenge_controller() : CreatureScript("npc_gurubashi_challenge_controller") { }

    struct npc_gurubashi_challenge_controllerAI : public ScriptedAI
    {
        npc_gurubashi_challenge_controllerAI(Creature* creature) : ScriptedAI(creature) {}
       
        void Reset()
        {
            me->setActive(true);
            _activated = true;
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                sGameEventMgr->StopEvent(EVENT_GURUBASHI_ARENA, true);
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            if (IsEventActive(EVENT_GURUBASHI_ARENA))
                _activated = true;
            else
                _activated = false;

            if (_activated)
            {
                if (_checkTimer <= diff)
                {
                    _checkTimer = 1000;

                    if (auto shortjohn = me->FindNearestCreature(NPC_SHORT_JOHN_MITHRIL, 200.f))
                        DoCast(shortjohn, SPELL_DUMMY_PING);
                }
                else _checkTimer -= diff;
            }
        }

    private:
        bool _activated = false;
        uint32 _checkTimer = 1000;
        uint64 _shortJohnGUID = 78002;

        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gurubashi_challenge_controllerAI(creature);
    }
};

uint32 const JohnMithrilPathSize = 9;
const G3D::Vector3 JohnMithrilPath[JohnMithrilPathSize] =
{
    { -13190.060547f, 320.417145f, 33.240601f }, // -13197.732f, 322.98f, 33.2406f OLD
    { -13214.074f, 322.53f, 33.2411f },
    { -13234.189f, 314.48f, 33.2215f },
    { -13248.335f, 300.20f, 33.2403f },
    { -13254.748f, 285.30f, 33.2411f },
    { -13255.523f, 280.82f, 33.2434f },
    { -13255.680f, 264.86f, 33.2434f },
    { -13248.581f, 246.36f, 33.2295f },
    { -13239.662f, 234.339f, 33.2401f }, // 8
};

uint32 const JohnMithrilPathSize2 = 10;
const G3D::Vector3 JohnMithrilPath2[JohnMithrilPathSize2] =
{
    { -13235.492f, 232.10f, 33.2394f },
    { -13231.726f, 234.27f, 33.2435f },
    { -13235.412f, 240.90f, 33.2511f },
    { -13238.229f, 244.50f, 29.1848f },
    { -13240.829f, 248.65f, 29.2694f },
    { -13244.611f, 257.70f, 21.8586f }, // Bottom of the ramp
    { -13237.583f, 267.03f, 21.8586f }, // .go -13237.583, 267.03, 21.8586
    { -13226.898438f, 269.247070f, 21.858601f },
    { -13215.320312f, 271.813202f, 21.858601f },
    { -13206.067383f, 273.700073f, 21.858601f }, // 9
};

uint32 const JohnMithrilPathSize3 = 7;
const G3D::Vector3 JohnMithrilPath3[JohnMithrilPathSize3] =
{
    { -13226.470f, 267.02f, 21.8575f },
    { -13243.116f, 259.90f, 21.8575f },
    { -13244.231f, 256.61f, 22.5426f },
    { -13240.719f, 248.60f, 29.2844f },
    { -13235.998f, 240.31f, 33.3178f }, // 4
    { -13231.726f, 234.27f, 33.2435f },
    { -13235.492f, 232.10f, 33.2394f }, // 6
};

uint32 const JohnMithrilPathSize4 = 10;
const G3D::Vector3 JohnMithrilPath4[JohnMithrilPathSize4] =
{
    { -13239.662f, 234.339f, 33.2401f },
    { -13248.581f, 246.36f, 33.2295f },
    { -13255.680f, 264.86f, 33.2434f },
    { -13255.523f, 280.82f, 33.2434f },
    { -13254.748f, 285.30f, 33.2411f },
    { -13248.335f, 300.20f, 33.2403f },
    { -13234.189f, 314.48f, 33.2215f },
    { -13214.074f, 322.53f, 33.2411f },
    { -13197.732f, 322.98f, 33.2406f },
    { -13184.5f, 332.671f, 40.3299f }, // 9
};

class npc_short_john_mithril : public CreatureScript
{
public:
    npc_short_john_mithril() : CreatureScript("npc_short_john_mithril") { }

    struct npc_short_john_mithrilAI : public ScriptedAI
    {
        npc_short_john_mithrilAI(Creature* creature) : ScriptedAI(creature) {}

        void Initialize()
        {
            Reset();
        }

        void Reset()
        {
            me->setActive(true);
            me->SetReactState(REACT_PASSIVE);
            _isEventInProgress = false;
            //_pathStep = 0; // default
            //me->SetWalk(true);
            //me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath, JohnMithrilPathSize);
        }

        void SpellHit(Unit* who, SpellInfo const* spellInfo)
        {
            if (spellInfo->Id == SPELL_DUMMY_PING)
            {
                if (!_isEventInProgress)
                {
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    _isEventInProgress = true;
                    Talk(0);
                    _pathStep = 0; // default
                    me->SetWalk(true);
                    me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath, JohnMithrilPathSize);
                    DoCast(who, SPELL_DUMMY_PING); // ping back to controller
                }
            }
        }

        void MovementInform(uint32 type, uint32 point)
        {
            if (type != SPLINE_MOTION_TYPE)
                return;

            if (_pathStep == 0)
            {
                if (point == 8)
                {
                    if (_isEventInProgress)
                    {
                        _pathStep = 1;
                        me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath2, JohnMithrilPathSize2);

                        if (auto treasure = GameObject::GetGameObject(*me, _treasureGUID))
                        {
                            treasure->Delete();
                            _treasureGUID = 0;
                        }
                    }
                    /*else
                        me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath4, JohnMithrilPathSize4);
                        _events.ScheduleEvent(EVENT_SHORT_JOHN_MITHRIL_4, 1000); */
                }
            }

            if (_pathStep == 1)
            {
                if (point == 9)
                {
                    me->SetFacingTo(0.223419f);
                    _events.ScheduleEvent(EVENT_SHORT_JOHN_MITHRIL_1, 1000);
                }
            }

            if (_pathStep == 2)
            {
                if (point == 6)
                {
                    //me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                    _isEventInProgress = false;
                    _pathStep = 3;
                    //me->SetWalk(true);
                    me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath4, JohnMithrilPathSize4);
                }
            }

            if (_pathStep == 3)
            {
                if (point == 9)
                {
                    _pathStep = 0;
                    me->SetFacingTo(4.41568f);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_SHORT_JOHN_MITHRIL_1:
                    //me->SetByteFlag(UNIT_FIELD_BYTES_1, 0, 8);
                    _events.ScheduleEvent(EVENT_SHORT_JOHN_MITHRIL_2, 2500);
                    break;
                case EVENT_SHORT_JOHN_MITHRIL_2:
                    //me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 0, 8);

                    if (auto treasure = me->SummonGameObject(OBJECT_ARENA_TREASURE_CHEST, -13203.682f, 274.122f, 21.8601f, 4.208324f, 0, 0, 0, 0, 0))
                    {
                        treasure->setActive(true);
                        _treasureGUID = treasure->GetGUID();
                    }

                    _events.ScheduleEvent(EVENT_SHORT_JOHN_MITHRIL_3, 1000);
                    break;
                case EVENT_SHORT_JOHN_MITHRIL_3:
                    _pathStep = 2;
                    me->SetWalk(false);
                    me->GetMotionMaster()->MoveSmoothPath(JohnMithrilPath3, JohnMithrilPathSize3);
                    Talk(1);
                    break;
                default:
                    break;
                }
            }
        }
    private:
        bool _isEventInProgress = false;
        uint16 _pathStep = 0;
        uint64 _treasureGUID = 0;
        EventMap _events;
    };
    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_short_john_mithrilAI(creature);
    }
};

void AddSC_cape_of_stranglethorn()
{
    new npc_backdoor_dealings_bat();
    new npc_swabbing_duty_controller();
    new npc_swabbing_duty_stain();
    new npc_ol_blasty_cannon();
    new npc_ol_blasty_boat();
    new npc_drive_by_piracy_boat();
    new npc_drive_by_piracy_cannon();
    new npc_drederick_mcgumm();
    new npc_final_voyage_wind_rider();
    new npc_damsel_rope_go_in_out();
    new npc_bloodsails_end_wind_rider();
    new npc_bloodsails_end_scene_controller();
    new npc_unbagbwa_stranglethorn_fever();
    new spell_teleport_to_zg_through();
    new npc_zanzil_the_outcast_jeklik();
    new npc_high_priestess_jeklik();
    new spell_venoxis_summon_rescue_vehicle();
    new npc_zanzil_the_outcast_venoxis();
    new npc_high_priest_venoxis_quest();
    new npc_spirit_of_venoxis_summoned();
    new npc_venoxis_quest_rescue_transport();
    new npc_gurubashi_challenge_controller();
    new npc_short_john_mithril();
}