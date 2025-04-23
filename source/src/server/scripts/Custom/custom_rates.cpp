#include "ScriptMgr.h"
#include "Player.h"

class CustomRates
{
private:
	static int32 GetRateFromDB(const Player* player, CharacterDatabaseStatements statement)
	{
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(statement);
		stmt->setUInt32(0, player->GetGUID());
		PreparedQueryResult result = CharacterDatabase.Query(stmt);

		if (result)
			return static_cast<int32>((*result)[0].GetUInt32());

		return -1;
	}

	static void SaveRateToDB(const Player* player, uint32 rate, bool update, CharacterDatabaseStatements uStmt, CharacterDatabaseStatements iStmt)
	{
		if (update)
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(uStmt);
			stmt->setUInt32(0, rate);
			stmt->setUInt32(1, player->GetGUID());
			CharacterDatabase.Execute(stmt);
		}
		else
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(iStmt);
			stmt->setUInt32(0, player->GetGUID());
			stmt->setUInt32(1, rate);
			CharacterDatabase.Execute(stmt);
		}
	}
public:
	static void DeleteRateFromDB(uint64 guid, CharacterDatabaseStatements statement)
	{
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(statement);
		stmt->setUInt32(0, guid);
		CharacterDatabase.Execute(stmt);
	}

	static int32 GetXpRateFromDB(const Player* player)
	{
		return GetRateFromDB(player, CHAR_SEL_INDIVIDUAL_XP_RATE);
	}

	static void SaveXpRateToDB(const Player* player, uint32 rate, bool update)
	{
		SaveRateToDB(player, rate, update, CHAR_UPD_INDIVIDUAL_XP_RATE, CHAR_INS_INDIVIDUAL_XP_RATE);
	}
};

class add_del_rates : public PlayerScript
{
public:
	add_del_rates() : PlayerScript("add_del_rates") { }

	void OnDelete(uint64 guid)
	{
		CustomRates::DeleteRateFromDB(guid, CHAR_DEL_INDIVIDUAL_XP_RATE);
	}

	void OnLogin(Player* player)
	{
		int32 rate = CustomRates::GetXpRateFromDB(player);

		if (rate < 0 || rate > 7)
			rate = 7;

		uint32 uRate = static_cast<uint32>(rate);
		player->SetCustomXpRate(uRate);
		
		if (player->getLevel() != sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
			ChatHandler(player->GetSession()).PSendSysMessage("|cffADFF2FYour current XP rate is set to|r|cffFFC125 x%u|r|cffADFF2F. Use command|r |cffFFC125.xp rate 1-7|r |cffADFF2Fto change your rate.|r", uRate);
	}
};

#define GOSSIP_TEXT_EXP         1
#define GOSSIP_XP_1				"Set my experience gain to x1"
#define GOSSIP_XP_2				"Set my experience gain to x2"
#define GOSSIP_XP_3				"Set my experience gain to x3"
#define GOSSIP_XP_4				"Set my experience gain to x4"
#define GOSSIP_XP_5				"Set my experience gain to x5"
#define GOSSIP_XP_6				"Set my experience gain to x6"
#define GOSSIP_XP_7				"Set my experience gain to x7"

class npc_xp_rates : public CreatureScript
{
public:
	npc_xp_rates() : CreatureScript("npc_xp_rates") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->getLevel() == 85)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You are already level 85", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
		}
		else
		{
			if (!player->GetCustomXpRate())
				player->ADD_GOSSIP_ITEM(-1, "|TInterface\\icons\\spell_shaman_improvedreincarnation:20:20:-10:0|tYour current experience rate is x7", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			else
			{
				std::stringstream ss;
				ss << "|TInterface\\icons\\spell_shaman_improvedreincarnation:20:20:-10:0|tYour current experience rate is x" << player->GetCustomXpRate();
				std::string post = ss.str();
				player->ADD_GOSSIP_ITEM(-1, post.c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			}
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, GOSSIP_XP_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_EXP, creature->GetGUID());
		}
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		uint32 uRate = 0;

		switch (action)
		{
		case GOSSIP_ACTION_INFO_DEF + 8:
		{
			OnGossipHello(player, creature);
			return true;
		}
		break;
		case GOSSIP_ACTION_INFO_DEF + 1:
			uRate = static_cast<uint32>(1);
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:
			uRate = static_cast<uint32>(2);
			break;
		case GOSSIP_ACTION_INFO_DEF + 3:
			uRate = static_cast<uint32>(3);
			break;
		case GOSSIP_ACTION_INFO_DEF + 4:
			uRate = static_cast<uint32>(4);
			break;
		case GOSSIP_ACTION_INFO_DEF + 5:
			uRate = static_cast<uint32>(5);
			break;
		case GOSSIP_ACTION_INFO_DEF + 6:
			uRate = static_cast<uint32>(6);
			break;
		case GOSSIP_ACTION_INFO_DEF + 7:
			uRate = static_cast<uint32>(7);
			break;
		}

		if (player->GetCustomXpRate() == uRate)
		{
			player->PlayerTalkClass->SendCloseGossip();
			creature->MonsterWhisper("You already have this experience rate set!", player->GetGUID(), false);
			return false;
		}
		else
		{
			player->PlayerTalkClass->SendCloseGossip();
			player->SetCustomXpRate(uRate);
			int32 rateFromDB = CustomRates::GetXpRateFromDB(player);
			if (rateFromDB == -1)
				CustomRates::SaveXpRateToDB(player, uRate, false);
			else
				CustomRates::SaveXpRateToDB(player, uRate, true);

			std::stringstream ss;
			ss << "Experience gain changed to x" << uRate;
			std::string post = ss.str();
			creature->MonsterWhisper(post.c_str(), player->GetGUID(), true);
			return true;
		}
		return false;
	}
};

class xp_rate_commands : public CommandScript
{
public:
	xp_rate_commands() : CommandScript("xp_rate_commands") { }

	std::vector<ChatCommand> GetCommands() const override
	{
		static std::vector<ChatCommand> xpCommandTable =
		{
			{ "rate",		SEC_CONSOLE,		false,	HandleRateCommand,		"" },
			{ "setRate",	SEC_CONSOLE,	false,	HandleSetRateCommand,	"" },
		};
		static std::vector<ChatCommand> commandTable =
		{
			{ "xp",			SEC_CONSOLE,		true,	NULL,	"", xpCommandTable		}
		};
		return commandTable;
	}

	static bool HandleRateCommand(ChatHandler* handler, char const* args)
	{
		if (auto player = handler->GetSession()->GetPlayer())
		{
			if (!*args)
			{
				if (player->GetSession()->GetSecurity() > SEC_CONSOLE)
				{
					if (auto target = handler->getSelectedPlayer())
					{
						if (target != player)
						{
							if (target->GetCustomXpRate())
								handler->PSendSysMessage("Current XP rate of player %s is x%u.", handler->GetNameLink(target).c_str(), target->GetCustomXpRate());
							else
								handler->PSendSysMessage("Current XP rate of player %s is x7.", handler->GetNameLink(target).c_str());
							handler->SetSentErrorMessage(true);
							return false;
						}
						else
						{
							if (player->GetCustomXpRate())
								handler->PSendSysMessage("|cffADFF2FYour current XP rate is set to|r|cffFFC125 x%u|r|cffADFF2F.|r", player->GetCustomXpRate());
							else
								handler->PSendSysMessage("|cffADFF2FYour current XP rate is set to|r|cffFFC125 x7|r|cffADFF2F.|r");
							handler->SetSentErrorMessage(true);
							return false;
						}
					}
					else
					{
						handler->PSendSysMessage("Invalid target / no target selected.");
						handler->SetSentErrorMessage(true);
						return false;
					}
				}
				else
				{
					if (player->GetCustomXpRate())
						handler->PSendSysMessage("|cffADFF2FYour current XP rate is set to|r|cffFFC125 x%u|r|cffADFF2F.|r", player->GetCustomXpRate());
					else
						handler->PSendSysMessage("|cffADFF2FYour current XP rate is set to|r|cffFFC125 x7|r|cffADFF2F.|r");
					handler->SetSentErrorMessage(true);
					return false;
				}
			}

			if (player->getLevel() == sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
			{
				handler->SendSysMessage("|cffADFF2FYou are already|r|cffFFC125 maximum level|r|cffADFF2F.|r");
				handler->SetSentErrorMessage(true);
				return false;
			}

			int rate = atoi((char*)args);
			int maxRate = 7;

			if (rate < 1 || rate > maxRate)
			{
				handler->PSendSysMessage("|cffADFF2FInvalid rate specified, must be between|r|cffFFC125 1|r|cffADFF2F and|r |cffFFC125%i|r|cffADFF2F.|r", maxRate);
				handler->SetSentErrorMessage(true);
				return false;
			}

			uint32 uRate = static_cast<uint32>(rate);
			player->SetCustomXpRate(uRate);
			int32 rateFromDB = CustomRates::GetXpRateFromDB(player);
			if (rateFromDB == -1)
				CustomRates::SaveXpRateToDB(player, uRate, false);
			else
				CustomRates::SaveXpRateToDB(player, uRate, true);

			handler->PSendSysMessage("|cffADFF2FYou have set your XP rate to|r |cffFFC125x%u|r|cffADFF2F.|r", uRate);
			return true;
		}

		return false;
	}

	static bool HandleSetRateCommand(ChatHandler* handler, char const* args)
	{
		if (auto player = handler->GetSession()->GetPlayer())
		{
			if (auto target = handler->getSelectedPlayer())
			{
				int rate = atoi((char*)args);
				int maxRate = 7;

				if (!*args)
				{
					handler->PSendSysMessage("Modify xp rates of selected player between 1 and %i.", maxRate);
					handler->SetSentErrorMessage(true);
					return false;
				}

				if (target->getLevel() == sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
				{
					handler->PSendSysMessage("Target is already at maximum level.", maxRate);
					handler->SetSentErrorMessage(true);
					return false;
				}

				if (rate < 1 || rate > maxRate)
				{
					handler->PSendSysMessage("Invalid rate specified, must be between 1 and %i.", maxRate);
					handler->SetSentErrorMessage(true);
					return false;
				}

				uint32 uRate = static_cast<uint32>(rate);
				target->SetCustomXpRate(uRate);
				int32 rateFromDB = CustomRates::GetXpRateFromDB(target);
				if (rateFromDB == -1)
					CustomRates::SaveXpRateToDB(target, uRate, false);
				else
					CustomRates::SaveXpRateToDB(target, uRate, true);

				handler->PSendSysMessage("You have set %s's XP rate to %u.", handler->GetNameLink(target).c_str(), uRate);
				ChatHandler(target->GetSession()).PSendSysMessage("%s |cffADFF2Fhas set your XP rate to|r |cffFFC125x%u|r|cffADFF2F.|r", handler->GetNameLink().c_str(), uRate);
				return true;
			}
			else
			{
				handler->PSendSysMessage("No target selected.");
				handler->SetSentErrorMessage(true);
				return false;
			}
		}

		return false;
	}
};

void Add_SC_Custom_Rates()
{
	new add_del_rates();
	new npc_xp_rates();
	new xp_rate_commands();
}