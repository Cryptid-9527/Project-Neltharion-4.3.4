#include "ScriptMgr.h"
#include "DatabaseEnv.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Player.h"
#include "Formulas.h"

/* test
std::vector<std::pair<std::string, uint32>> SQLTasks =
{
	{ 
	"REPLACE INTO `auth`.`faction_ratio` (`id`, `totalliance`, `tothorde`, `totonline`) VALUES (1, (SELECT count(*) FROM `characters`.`characters` where race in ('1', '3', '4', '7', '11', '22') AND level > 10 AND online = 1), (SELECT count(*) FROM `characters`.`characters` where race in ('2', '5', '6', '8', '9', '10') AND level > 10 AND online = 1), (SELECT count(*) FROM `characters`.`characters` where online = 1));", 
	180000// every 3 minute
	}, 
	{ 
	"REPLACE INTO `auth`.`faction_ratio` (`id`, `totalliance`, `tothorde`, `totonline`) VALUES (2, (SELECT count(*) FROM `characters`.`characters` where race in ('1', '3', '4', '7', '11', '22') AND level > 10 ), (SELECT count(*) FROM `characters`.`characters` where race in ('2', '5', '6', '8', '9', '10') AND level > 10), (SELECT count(*) FROM `characters`.`characters` where level = 85));", 
	180000// every 3 minute
}, 
};
*/


class BackgroundSQLTask : public WorldScript
{
public:
    BackgroundSQLTask() : WorldScript("BackgroundSQLTask") { }

    void OnStartup() override
    {
		if (sWorld->getRealmID() == 1)
		events.ScheduleEvent(1, 45000);
    }

    void OnUpdate(uint32 diff) override
    {
        events.Update(diff);
        while (uint32 eventId = events.ExecuteEvent())
			switch (eventId)
			{
			case 1:
				UpdateRealmPopulationStatistics();
				events.ScheduleEvent(1, 1800000);
				break;
			default:
				break;
			}
    }

	enum misc
	{
		SORTING_CLASS	= 0,
		SORTING_ROLE,
		SORTING_GENDER,
		SORTING_RACE,
		SORTING_PROFS,
	};

	struct table_entry_per_level
	{
		uint32 count_a = 0;
		uint32 count_h = 0;
	};

	void UpdateRealmPopulationStatistics()
	{


															//3 dimensions
		std::map<uint8,									//1
			std::map <uint8,								//2
				std::map<uint32, table_entry_per_level>		//3
			>
		> data_map;

		for (uint8 a = 1; a <= 85; ++a)//for every level
			for (uint8 b = 0; b <= 3; ++b)//for every sorting
				for (uint32 c = 0; c <= SKILL_INSCRIPTION; ++c)//for every index
					{
						data_map[a][b][c] = { uint32(0), uint32(0) };//initialize
						data_map[a][b][c].count_a += 0;
						data_map[a][b][c].count_h += 0;
					}

			if (auto result = CharacterDatabase.PQuery("SELECT guid, LEVEL, race, class, gender, CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(talentTree, ' ',  activeSpec+1), ' ', -1) AS INT) AS 'spec' FROM characters;"))
			{

				do
				{
					if (auto fields2 = result->Fetch())
					{
						uint64 guid		{ fields2[0].GetUInt64() };
						uint8 level		{ fields2[1].GetUInt8()  };
						uint8 race		{ fields2[2].GetUInt8()  };
						uint8 pclass	{ fields2[3].GetUInt8()  };
						uint8 gender	{ fields2[4].GetUInt8()  };
						uint32 spec		{ fields2[5].GetUInt32()  };
						uint8 role		= GetRole(spec);
						//TC_LOG_ERROR("sql.sql", "found role %u on guid %u from spec %u", role, guid, spec);

						for (uint8 sorting = 0; sorting <= 3; ++sorting)//for every sorting
							switch (sorting)
							{
								case SORTING_CLASS:
								{
									if (isAlliance(race))
										data_map[level][sorting][pclass].count_a += 1;
									else
										data_map[level][sorting][pclass].count_h += 1;
									break;
								}
								case SORTING_RACE:
								{
									if (isAlliance(race))
										data_map[level][sorting][race].count_a += 1;
									else
										data_map[level][sorting][race].count_h += 1;
									break;
								}
								case SORTING_ROLE:
								{
									if (isAlliance(race))
										data_map[level][sorting][role].count_a += 1;
									else
										data_map[level][sorting][role].count_h += 1;
									break;
								}
								case SORTING_GENDER:
								{
									if (isAlliance(race))
										data_map[level][sorting][gender].count_a += 1;
									else
										data_map[level][sorting][gender].count_h += 1;
									break;
								}
							}
					}

				} while (result->NextRow());
			}

			if (auto result = CharacterDatabase.PQuery("SELECT ch.guid, ch.level, ch.race, cs.skill FROM characters ch LEFT JOIN character_skills cs ON ch.guid = cs.guid WHERE cs.value >= (100 + (RAND() * 425)) AND(cs.skill IN(164, 171, 182, 186, 197, 202, 333, 393, 755, 773)); "))
			{
				do
				{
					if (auto fields2 = result->Fetch())
					{
						uint64 guid{ fields2[0].GetUInt64() };
						uint8 level{ fields2[1].GetUInt8() };
						uint8 race{ fields2[2].GetUInt8() };
						uint32 skill{ fields2[3].GetUInt32() };


						if (isAlliance(race))
							data_map[level][SORTING_PROFS][skill].count_a += 1;
						else
							data_map[level][SORTING_PROFS][skill].count_h += 1;

					}

				} while (result->NextRow());
			}

			SQLTransaction trans = LoginDatabase.BeginTransaction();

			for (uint8 a = 1; a <= 85; ++a)//for every level
				for (uint8 b = SORTING_CLASS; b <= SORTING_PROFS; ++b)//for every sorting
					for (uint32 c = 0; c <= SKILL_INSCRIPTION; ++c)//for every index
						if (data_map.find(a) != data_map.end())
						if (data_map[a].find(b) != data_map[a].end())
						if (data_map[a][b].find(c) != data_map[a][b].end())
						if (isValidIndex(b, c))
						if (auto stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_FACTION_RATIO)) //REPLACE INTO `faction_ratio` (`sort`, `index`, `level`, `value_0`, `value_1`) VALUES
						{
							auto value_h = data_map[a][b][c].count_h;
							auto value_a = data_map[a][b][c].count_a;
							stmt->setUInt8(0, b);
							stmt->setUInt32(1, c);
							stmt->setUInt8(2, a);
							stmt->setUInt32(3, value_h);
							stmt->setUInt32(4, value_a);
							trans->Append(stmt);
						}

			LoginDatabase.CommitTransaction(trans);
			return;
	};


	enum LfgRoles
	{
		PLAYER_ROLE_NONE = 0x00,
		PLAYER_ROLE_LEADER = 0x01,
		PLAYER_ROLE_TANK = 0x02,
		PLAYER_ROLE_HEALER = 0x04,
		PLAYER_ROLE_DAMAGE = 0x08
	};

	uint8 GetRole(uint32 spec)
	{
		uint8 role{ PLAYER_ROLE_DAMAGE };

		switch (spec)
		{
		case TALENT_TREE_WARRIOR_FURY:          role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_WARRIOR_ARMS:          role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_WARRIOR_PROTECTION:    role = (PLAYER_ROLE_TANK);   break;

		case TALENT_TREE_PALADIN_HOLY:          role = (PLAYER_ROLE_HEALER); break;
		case TALENT_TREE_PALADIN_PROTECTION:    role = (PLAYER_ROLE_TANK);   break;
		case TALENT_TREE_PALADIN_RETRIBUTION:   role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_HUNTER_BEAST_MASTERY:  role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_HUNTER_MARKSMANSHIP:   role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_HUNTER_SURVIVAL:       role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_ROGUE_ASSASSINATION:   role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_ROGUE_COMBAT:          role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_ROGUE_SUBTLETY:        role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_PRIEST_DISCIPLINE:     role = (PLAYER_ROLE_HEALER); break;
		case TALENT_TREE_PRIEST_HOLY:           role = (PLAYER_ROLE_HEALER); break;
		case TALENT_TREE_PRIEST_SHADOW:         role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_DEATH_KNIGHT_BLOOD:    role = (PLAYER_ROLE_TANK);   break;
		case TALENT_TREE_DEATH_KNIGHT_FROST:    role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_DEATH_KNIGHT_UNHOLY:   role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_SHAMAN_ELEMENTAL:      role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_SHAMAN_ENHANCEMENT:    role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_SHAMAN_RESTORATION:    role = (PLAYER_ROLE_HEALER); break;

		case TALENT_TREE_MAGE_ARCANE:           role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_MAGE_FIRE:             role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_MAGE_FROST:            role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_WARLOCK_AFFLICTION:    role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_WARLOCK_DEMONOLOGY:    role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_WARLOCK_DESTRUCTION:   role = (PLAYER_ROLE_DAMAGE); break;

		case TALENT_TREE_DRUID_BALANCE:         role = (PLAYER_ROLE_DAMAGE); break;
		case TALENT_TREE_DRUID_RESTORATION:     role = (PLAYER_ROLE_HEALER); break;
		case TALENT_TREE_DRUID_FERAL_COMBAT:	role = RAND(PLAYER_ROLE_DAMAGE, PLAYER_ROLE_DAMAGE, PLAYER_ROLE_TANK); break;
		default:
			break;
		}
		//TC_LOG_ERROR("sql.sql", "returning role %u with input %u", role, spec);
		return role;
	};

	bool isAlliance(uint8 race)
	{
		if (RACEMASK_ALLIANCE & (1 << (race-1)))
		{
			//TC_LOG_ERROR("sql.sql", "race %u is alliance.", race);
			return true;
		}
		else
		{
			//TC_LOG_ERROR("sql.sql", "race %u is NOT alliance.", race);
		}
		return false;
	};

	bool isValidIndex(uint8 sorting, uint32 index)
	{
		switch (sorting)
		{
		case SORTING_CLASS:
			if (index <= CLASS_DRUID && index != 10)
				return true;
			else
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_CLASS %u is invalid", index);
				return false;
			}
			break;
		case SORTING_RACE:
			if ((index > 0 && index <= RACE_DRAENEI) || index == RACE_WORGEN)
				return true;
			else
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_RACE %u is invalid", index);
				return false;
			}
			break;
		case SORTING_ROLE:
			if (index == PLAYER_ROLE_TANK || index == PLAYER_ROLE_HEALER || index == PLAYER_ROLE_DAMAGE)
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_ROLE %u is valid", index);
				return true;
			}
			else
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_ROLE %u is invalid", index);
				return false;
			}
			break;
		case SORTING_GENDER:
			if (index <= GENDER_FEMALE)
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_GENDER %u is valid", index);
				return true;
			}
			else
			{
				//TC_LOG_ERROR("sql.sql", "SORTING_GENDER %u is invalid", index);
				return false;
			}
		case SORTING_PROFS:
			return IsPrimaryProfessionSkill(index);
			break;
		}
		//TC_LOG_ERROR("sql.sql", "BackgroundScript Population balance update: bool isValidIndex(uint8 sorting, uint8 index) something fucked up");

		return false;
	}

private:
    EventMap events;
};

	
enum CharItemIds
{
	CITEM_RENAME		 = 100000,
	CITEM_CUSTOMIZE		 = 100001,
	CITEM_RACE_CHANGE	 = 100002,
	CITEM_FACTION_CHANGE = 100003,
	CITEM_LEVEL_UP		 = 100004,
	CITEM_ACC_UPGRADE	 = 100018,
	CITEM_GUILD_LEVEL	 = 100019,
	CITEM_GUILD_LEVEL2	 = 100020,
	CITEM_LEVEL_UP2		 = 100021,
};

class item_char_rename : public ItemScript
{
public:
	item_char_rename() : ItemScript("item_char_rename") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
	{
		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		if (player->HasAtLoginFlag(AT_LOGIN_RENAME) || player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your character is already flagged for one of the character services.");
			return false;
		}

		else
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
			stmt->setUInt16(0, uint16(AT_LOGIN_RENAME));
			ChatHandler(player->GetSession()).PSendSysMessage("Relog to change your name.");
			player->SetAtLoginFlag(AT_LOGIN_RENAME);
			stmt->setUInt32(1, player->GetGUIDLow());
			CharacterDatabase.Execute(stmt);
			player->DestroyItemCount(CITEM_RENAME, 1, true);
			return true;
		}
	}
};


class item_account_upgrade : public ItemScript
{
public:
	item_account_upgrade() : ItemScript("item_account_upgrade") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
	{
		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		if (player->HasItemCount(CITEM_ACC_UPGRADE, 1, true))
			if (auto s = player->GetSession())
			{
				if (s->premium_HasPerk(ACCOUNT_PERK_50_CHARACTERS))
					if (s->premium_HasPerk(ACCOUNT_PERK_FULL_STATUS))
					{
						ChatHandler(s).PSendSysMessage("Your account already has full status. Please contact support if you have any issues related to these perks.");
						return false;
					}

				if (auto stmt3 = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_PREMIUM))
					if (auto stmt4 = LoginDatabase.GetPreparedStatement(LOGIN_REP_ACCOUNT_PREMIUM))
				{
					/*
					
					PrepareStatement(LOGIN_REP_ACCOUNT_PREMIUM, 
					"REPLACE INTO `account_premium` 
					(`account_id`,				0
					`realm_id`,					1
					`perk_type`,				2
					`perk_value1`,				3
					`perk_value2`,				4
					`perk_value3`,				5
					`StartTime`,				skip
					`EndTime`,					6
					`perk_active`,				7
					`comment`)					8
					VALUES (?, ?, ?, ?, ?, ?, NOW(), ?, ?, ?);", CONNECTION_SYNCH);
					*/
					stmt3->setUInt32(0, s->GetAccountId());
					stmt3->setUInt32(1, sWorld->getRealmID());
					stmt3->setUInt32(2, 0);
					stmt3->setFloat(3, 0.f);
					stmt3->setFloat(4, 0.f);
					stmt3->setFloat(5, 0.f);
					stmt3->setUInt8(7, 1);
					stmt3->setString(8, "Added via Account Upgrade Item");
					LoginDatabase.AsyncQuery(stmt3);

					stmt4->setUInt32(0, s->GetAccountId());
					stmt4->setUInt32(1, sWorld->getRealmID());
					stmt4->setUInt32(2, 1);
					stmt4->setFloat(3, 0.f);
					stmt4->setFloat(4, 0.f);
					stmt4->setFloat(5, 0.f);
					stmt4->setUInt8(7, 1);
					stmt4->setString(8, "Added via Account Upgrade Item");
					LoginDatabase.AsyncQuery(stmt4);

					player->DestroyItemCount(CITEM_ACC_UPGRADE, 1, true);
					ChatHandler(s).PSendSysMessage("Your account has been upgraded to full status. Over time, benefits and rewards exclusive to this status will be made available to you.");
					ChatHandler(s).PSendSysMessage("Your account has been upgraded to hold 50 characters. Please speak to the custom features guard to add more characters.");

					WorldSession::PerkDetails perk_0
					{
						uint8(0),
						0.f,
						0.f,
						0.f
					};
					WorldSession::PerkDetails perk_1
					{
						uint8(1),
						0.f,
						0.f,
						0.f
					};


					s->premium_AddPerk(0, perk_0);
					s->premium_AddPerk(1, perk_1);
					player->CastSpell(player, 47292, true);
					/*
						TODO:
						- Mail a letter to the player with instructions on making new characters
					*/

				}
			}
		return true;
	}
};


class item_char_customize : public ItemScript
{
public:
	item_char_customize() : ItemScript("item_char_customize") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
	{
		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		if (player->HasAtLoginFlag(AT_LOGIN_RENAME) || player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your character is already flagged for one of the character services.");
			return false;
		}

		else
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
			stmt->setUInt16(0, uint16(AT_LOGIN_CUSTOMIZE));
			ChatHandler(player->GetSession()).PSendSysMessage("Relog to customize your character.");
			player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
			stmt->setUInt32(1, player->GetGUIDLow());
			CharacterDatabase.Execute(stmt);
			player->DestroyItemCount(CITEM_CUSTOMIZE, 1, true);
			return true;
		}
	}
};

class item_char_racechange : public ItemScript
{
public:
	item_char_racechange() : ItemScript("item_char_racechange") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
	{
		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		if (player->HasAtLoginFlag(AT_LOGIN_RENAME) || player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your character is already flagged for one of the character services.");
			return false;
		}

		else
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
			stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_RACE));
			ChatHandler(player->GetSession()).PSendSysMessage("Relog to change your race.");
			player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
			stmt->setUInt32(1, player->GetGUIDLow());
			CharacterDatabase.Execute(stmt);
			player->DestroyItemCount(CITEM_RACE_CHANGE, 1, true);
			return true;
		}
	}
};

class item_char_factionchange : public ItemScript
{
public:
	item_char_factionchange() : ItemScript("item_char_facitonchange") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets & /*targets*/)
	{
		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		if (player->HasAtLoginFlag(AT_LOGIN_RENAME) || player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your character is already flagged for one of the character services.");
			return false;
		}

		else
		{
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ADD_AT_LOGIN_FLAG);
			stmt->setUInt16(0, uint16(AT_LOGIN_CHANGE_FACTION));
			ChatHandler(player->GetSession()).PSendSysMessage("Relog to change your faction.");
			player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
			stmt->setUInt32(1, player->GetGUIDLow());
			CharacterDatabase.Execute(stmt);
			player->DestroyItemCount(CITEM_FACTION_CHANGE, 1, true);
			return true;
		}
	}
};

class item_char_levelup : public ItemScript
{
public:
	item_char_levelup() : ItemScript("item_char_levelup") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
	{
		if (!player)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong!");
			return false;
		}

		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			return false;
		}

		player->PlayerTalkClass->ClearMenus();

		if (player->getLevel() == 85)
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "Already 85", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Your character is already level 85!\nWould you like to convert level-up to\nStarter Items Pack?", 0, false);
		else
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "Level-up 85", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, "Are you sure you want to level-up to 85?\nThis action cannot be reversed.", 0, false);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		return false;
	}

	void OnGossipSelect(Player* player, Item* item, uint32 /*sender*/, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
		case GOSSIP_ACTION_INFO_DEF + 1:
		{
			player->DestroyItemCount(item->GetEntry(), 1, true);
			player->AddItem(100022, 1); // Starter items
			ChatHandler(player->GetSession()).PSendSysMessage("Your [Character Level-up 85] has been converted into [Starter Item Pack].");
			player->CLOSE_GOSSIP_MENU();
		}
		break;
		case GOSSIP_ACTION_INFO_DEF + 2:
		{
			player->GiveLevel(85);
			player->InitTalentForLevel();
			player->SetUInt32Value(PLAYER_XP, 0);
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LEVEL);
			stmt->setUInt8(0, uint8(85));
			stmt->setUInt32(1, GUID_LOPART(player->GetGUID()));
			CharacterDatabase.Execute(stmt);
			if (player->getClass() == CLASS_DEATH_KNIGHT)
			{
				if (!player->HasSpell(50977))
					player->learnSpell(50977, false);
				if (!player->HasSpell(53428))
					player->learnSpell(53428, false);
			}
			if (player->getRace() == RACE_WORGEN)
			{
				if (!player->HasSpell(68996))
					player->learnSpell(68996, false);
				if (!player->HasSpell(68992))
					player->learnSpell(68992, false);
			}
			if (!player->HasSpell(33388)) // Apprentice Riding
				player->learnSpell(33388, false);
			if (!player->HasSpell(33391)) // Journeyman Riding
				player->learnSpell(33391, false);
			if (!player->HasSpell(34090)) // Expert Riding
				player->learnSpell(34090, false);
			if (!player->HasSpell(54197)) // Cold Weather Flying
				player->learnSpell(54197, false);
			if (!player->HasSpell(90267)) // Flight Master's License
				player->learnSpell(90267, false);
			if (player->GetTeam() == ALLIANCE && !player->HasSpell(32235)) // Golden Gryphon
				player->learnSpell(32235, false);
			else if (player->GetTeam() == HORDE && !player->HasSpell(32243)) // Tawny Wind Rider
				player->learnSpell(32243, false);
			player->DestroyItemCount(CITEM_LEVEL_UP, 1, true);
			player->ModifyMoney(5000000, false);
			player->AddItem(100022, 1); // Starter items
			player->AddItem(100023, 1); // Teleport rune
			player->SaveToDB();
			ChatHandler(player->GetSession()).PSendSysMessage("Use the [Starter Item Pack] to select your desired specialization.");
		}
		break;
		default:
			break;
		}
	}
};

class item_char_levelup2 : public ItemScript
{
public:
	item_char_levelup2() : ItemScript("item_char_levelup2") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
	{
		if (!player)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong!");
			return false;
		}

		if (player->HasSpellCooldown(90065))
			return false;

		if (player->isInCombat() || player->InArena() || (player->InBattleground()))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You cannot use that item here, go to a safe location!");
			player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 3);
			return false;
		}

		if (player->getLevel() >= 80)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your character is already above level 80!");
			player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 3);
			return false;
		}

		else
		{
			uint32 playerLevel = player->getLevel();
			uint32 newLevel = playerLevel + 1;

			player->GiveLevel(newLevel);
			player->InitTalentForLevel();
			player->SetUInt32Value(PLAYER_XP, 0);
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LEVEL);
			stmt->setUInt8(0, uint8(85));
			stmt->setUInt32(1, GUID_LOPART(player->GetGUID()));
			CharacterDatabase.Execute(stmt);
			player->SaveToDB();
			player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 3);
			player->DestroyItemCount(CITEM_LEVEL_UP2, 1, true);
			return true;
		}
	}
};

class item_guild_levelup : public ItemScript
{
public:
	item_guild_levelup() : ItemScript("item_guild_levelup") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
	{
		if (!player)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong!");
			return false;
		}

		if (auto guild = player->GetGuild())
		{
			if (!guild->GetId())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong, try again later.");
				return false;
			}
			
			if (guild->GetLeaderGUID() != player->GetGUID())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("You are not the Guild Master of your guild!");
				return false;
			}

			if (guild->GetLevel() == 25)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Your guild is already level 25!");
				return false;
			}
			else
			{
				uint32 oldLevel = guild->GetLevel();
				for (int i = guild->GetLevel(); i < 25; ++i)
					guild->LevelUp(oldLevel, player);
				
				ChatHandler(player->GetSession()).PSendSysMessage("%s is now a level 25 Guild", guild->GetName());
				player->DestroyItemCount(CITEM_GUILD_LEVEL, 1, true);
				return true;
			}
		}
		else
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You are not in a guild!");
			return false;
		}
	}
};

class item_guild_levelup2 : public ItemScript
{
public:
	item_guild_levelup2() : ItemScript("item_guild_levelup2") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets& /*targets*/)
	{
		if (!player)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong!");
			return false;
		}

		if (player->HasSpellCooldown(90065))
			return false;

		if (auto guild = player->GetGuild())
		{
			if (!guild->GetId())
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Something went wrong, try again later.");
				return false;
			}

			if (guild->GetLevel() == 25)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Your guild is already level 25!");
				player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 5);
				return false;
			}
			else
			{
				uint32 oldLevel = guild->GetLevel();
				uint32 newLevel = oldLevel + 1;
				guild->LevelUp(oldLevel, player);

				ChatHandler(player->GetSession()).PSendSysMessage("%s is now a level %u Guild", guild->GetName(), newLevel);
				player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 5);
				player->DestroyItemCount(CITEM_GUILD_LEVEL2, 1, true);
				return true;
			}
		}
		else
		{
			ChatHandler(player->GetSession()).PSendSysMessage("You are not in a guild!");
			player->AddSpellCooldown(90065, item->GetEntry(), time(NULL) + 5);
			return false;
		}
	}
};

class npc_starting_options : public CreatureScript
{
public:
	npc_starting_options() : CreatureScript("npc_starting_options") { }

	bool OnGossipHello(Player* pPlayer, Creature* pCreature)
	{
		if (pPlayer->getLevel() < 85)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "I would like to start at level 85.", GOSSIP_SENDER_MAIN, 1);
		if (pPlayer->GetTeam() == ALLIANCE)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I would like a teleport to Stormwind.", GOSSIP_SENDER_MAIN, 2);
		if (pPlayer->GetTeam() == HORDE)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "I would like a teleport to Orgrimmar.", GOSSIP_SENDER_MAIN, 3);
		if (pPlayer->GetMoney() < 50000000 && pPlayer->getLevel() == 85)
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "I'm almost out of gold, give me more!", GOSSIP_SENDER_MAIN, 4);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, "I would like to browse your goods.", GOSSIP_SENDER_MAIN, 5);
		if (!pPlayer->GetGuild())
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Invite me to the Beta Guild", GOSSIP_SENDER_MAIN, 6);
		pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
		case 1:
		{
			if (player->getClass() == CLASS_DEATH_KNIGHT)
			{
				player->learnSpell(50977, false);
				player->learnSpell(53428, false);
			}

			if (player->getRace() == RACE_WORGEN)
			{
				player->learnSpell(68996, false);
				player->learnSpell(68992, false);
			}

			player->PlayerTalkClass->SendCloseGossip();
			player->GiveLevel(85);
			player->InitTalentForLevel();
			player->SetUInt32Value(PLAYER_XP, 0);
			player->ModifyMoney(300000000, false);
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_LEVEL);
			stmt->setUInt8(0, uint8(85));
			stmt->setUInt32(1, GUID_LOPART(player->GetGUID()));
			CharacterDatabase.Execute(stmt);
			player->SaveToDB();
			break;
		}
		case 2:
		{
			player->PlayerTalkClass->SendCloseGossip();
			player->TeleportTo(0, -8853.607f, 657.064f, 98.225f, 6.225f);
			break;
		}
		case 3:
		{
			player->PlayerTalkClass->SendCloseGossip();
			player->TeleportTo(1, 1595.626f, -4413.430f, 16.462f, 5.163f);
			break;
		}
		case 4:
		{
			player->PlayerTalkClass->SendCloseGossip();
			player->ModifyMoney(250000000, false);
			break;
		}
		case 5:
		{
			player->GetSession()->SendListInventory(creature->GetGUID());
			break;
		}
		case 6:
			Guild* guild = sGuildMgr->GetGuildById(1);
			if (!player->GetGuild() && !player->GetGuildIdInvited())
			{
				guild->AddMember(player->GetGUID());
				creature->MonsterWhisper("Welcome to Project Nelth Beta guild!", player->GetGUID(), true);
				player->PlayerTalkClass->SendCloseGossip();
			}
			else
			{
				creature->MonsterWhisper("You are already in a guild, or have been invited to one.", player->GetGUID(), false);
				player->PlayerTalkClass->SendCloseGossip();
			}
			break;
		}
		return true;
	}
};

// Instead of adding a hacky way into Player::Create, we use existing hooks to cast these spells on first character login
class CharacterCreationProcedures : public PlayerScript
{
public:
	CharacterCreationProcedures() : PlayerScript("CharacterCreationProcedures") {}

	void OnLogin(Player* player)
	{
		if (player->HasAtLoginFlag(AT_LOGIN_FIRST))
		{
			if (player->GetSession()->GetSecurity() >= 2)
			{
				for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
					player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

				player->SetGameMaster(true);
				player->SetGMVisible(true);
				player->AddAura(44816, player);
				player->UpdateObjectVisibility();
				player->EquipNewItem(EQUIPMENT_SLOT_CHEST, 2586, true);
				player->EquipNewItem(EQUIPMENT_SLOT_FEET, 11508, true);
				player->EquipNewItem(EQUIPMENT_SLOT_HEAD, 12064, true);
				player->EquipNewItem(EQUIPMENT_SLOT_HANDS, /*81429*/55737, true);//custom items won't be visible to players on the first viewing


				if (player->getClass() == CLASS_DEATH_KNIGHT)
					for (uint8 slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; slot++)
						player->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);

				for (uint8 slot = INVENTORY_SLOT_BAG_START; slot < INVENTORY_SLOT_BAG_END; slot++)
					player->EquipNewItem(slot, 23162, true);
			}
			
			switch (player->getClass())
			{
			case CLASS_WARRIOR:
				player->CastSpell(player, 2457, true);
				return;
			case CLASS_DEATH_KNIGHT:
				player->CastSpell(player, 48266, true);
				return;
			default:
				return;
			}
		}

		// Glyph of Seal of Truth update on login
		if (player->getClass() == CLASS_PALADIN && player->HasAura(31801) && player->HasAura(56416))
		{
			if (AuraApplication* seal = player->GetAuraApplication(31801))
			{
				int32 duration = seal->GetBase()->GetDuration();
				player->RemoveAurasDueToSpell(31801);
				if (Aura* newSeal = player->AddAura(31801, player))
					newSeal->SetDuration(duration);
			}
		}
	}
};

class ArenaDampening : public PlayerScript
{
public:
	ArenaDampening() : PlayerScript("ArenaDampening") {}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)
	{
		if (auto bg = player->GetBattleground())
			if (bg->isArena())
				return;

		// Remove Dampening spell on update zone
		if (player->HasAura(110310))
			player->RemoveAura(110310);
	}

	void OnLogin(Player* player)
	{
		if (auto bg = player->GetBattleground())
			if (bg->isArena())
				return;

		// Remove Dampening spell on update zone
		if (player->HasAura(110310))
			player->RemoveAura(110310);
	}
};

class SaveNewCap : public PlayerScript
{
public:
	SaveNewCap() : PlayerScript("SaveNewCap") { }

	void OnLogout(Player* player)
	{
		if (player->GetCurrency(CURRENCY_TYPE_CONQUEST_POINTS, true))
		{
			PreparedStatement* stmt = NULL;
			uint32 capArena = Trinity::Currency::ConquestRatingCalculator(player->GetMaxPersonalArenaRatingRequirement(0)) * CURRENCY_PRECISION;
			uint32 capRBG = Trinity::Currency::BgConquestRatingCalculator(player->GetRbgOrSoloQueueRatingForCapCalculation()) * CURRENCY_PRECISION;
			uint32 cap = std::max(capArena, capRBG);
			stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_CURRENCY_NEW_CAP_OVERALL);
			stmt->setUInt32(0, cap);
			stmt->setUInt64(1, player->GetGUIDLow());
			CharacterDatabase.Execute(stmt);

			if (player->GetCurrency(CURRENCY_TYPE_CONQUEST_META_ARENA, true))
			{
				stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_CURRENCY_NEW_CAP_ARENA);
				stmt->setUInt32(0, capArena);
				stmt->setUInt64(1, player->GetGUIDLow());
				CharacterDatabase.Execute(stmt);
			}

			if (player->GetCurrency(CURRENCY_TYPE_CONQUEST_META_RBG, true))
			{
				stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_CURRENCY_NEW_CAP_RBG);
				stmt->setUInt32(0, capRBG);
				stmt->setUInt64(1, player->GetGUIDLow());
				CharacterDatabase.Execute(stmt);
			}
		}
	}
};

class npc_soloq_guard : public CreatureScript
{
public:
	npc_soloq_guard() : CreatureScript("npc_soloq_guard") { }

	enum univeral_npc
	{
		NO_MENU			= 0,
		GOSSIP_SENDER_MULTI_SELECTION = 9191919,
		SOLO_QUEUE		= 123456,
		SPECTATE		= 1758,
		MORPHER			= 600064,
		XP_RATE			= 1337016,
		ACCOUNT_MGR		= 1337030,
		TELEPORTER		= 26367
	};


	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (!player)
			return true;

		SetGUIDSelection(player->GetGUID(), NO_MENU);

		if (player->getLevel() >= 85)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,	"Solo queue options", GOSSIP_SENDER_MULTI_SELECTION,		SOLO_QUEUE);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,		"Spectating options", GOSSIP_SENDER_MULTI_SELECTION,		SPECTATE);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,		"Morphing options", GOSSIP_SENDER_MULTI_SELECTION,		MORPHER);

		if (player->getLevel() < 85)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,		"XP Rate options", GOSSIP_SENDER_MULTI_SELECTION,		XP_RATE);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,		"Account Manager options", GOSSIP_SENDER_MULTI_SELECTION,		ACCOUNT_MGR);

		if (player->getLevel() >= 85)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,		"Teleporter options", GOSSIP_SENDER_MULTI_SELECTION,		TELEPORTER);

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool isValidmenu(univeral_npc id)
	{
		switch (id)
		{
		case SOLO_QUEUE:
		case SPECTATE:
		case MORPHER:
		case XP_RATE:
		case ACCOUNT_MGR:
		case TELEPORTER:
			return true;
			break;
		}
		//TC_LOG_ERROR("sql.sql", "isValidmenu = false for %u", uint32(id));
		return false;
	}


	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		if (!player)
			return true;


		if (sender == GOSSIP_SENDER_MULTI_SELECTION)
		{
			if (isValidmenu(univeral_npc(action)))
			{
				SetGUIDSelection(player->GetGUID(), univeral_npc(action));
				if (auto select = GetGUIDSelection(player->GetGUID()))
					if (auto t = sObjectMgr->GetCreatureTemplate(select))
						sScriptMgr->OnGossipHello(player, creature, t->ScriptID);
			}
			else
			{
				creature->MonsterWhisper("Menu selection error 4", player->GetGUID(), false);
				player->CLOSE_GOSSIP_MENU();
				return true;
			}

		}
		else
		{
			if (auto select = GetGUIDSelection(player->GetGUID()))
			{
				if (auto t = sObjectMgr->GetCreatureTemplate(select))
				{
					sScriptMgr->OnGossipSelect(player, creature, sender, action, t->ScriptID);
					//TC_LOG_ERROR("sql.sql", "sender: %u, action: %u, to saved selection: %u", sender, action, GetGUIDSelection(player->GetGUID()));
				}
				else
				{
					//TC_LOG_ERROR("sql.sql", "sender: %u, action: %u, saved selection: %u", sender, action, GetGUIDSelection(player->GetGUID()));

					creature->MonsterWhisper("Menu selection error 8", player->GetGUID(), false);
					player->CLOSE_GOSSIP_MENU();
				}
			}
			else
			{
				//TC_LOG_ERROR("sql.sql", "sender: %u, action: %u, saved selection: %u", sender, action, GetGUIDSelection(player->GetGUID()));

				creature->MonsterWhisper("Menu selection error 5", player->GetGUID(), false);
				player->CLOSE_GOSSIP_MENU();
			}
		}

		return true;
	}

	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
	{
		if (!player)
			return true;

		if (!univeral_npc(action))
			return true;

		if (sender == GOSSIP_SENDER_MULTI_SELECTION)
		{
			creature->MonsterWhisper("Menu selection error 7", player->GetGUID(), false);
		}
		else
		{
			if (auto select = GetGUIDSelection(player->GetGUID()))
			{
				if (auto t = sObjectMgr->GetCreatureTemplate(select))
					sScriptMgr->OnGossipSelectCode(player, creature, sender, action, code, t->ScriptID);
			}
			else
			{
				creature->MonsterWhisper("Menu selection error 6", player->GetGUID(), false);
				player->CLOSE_GOSSIP_MENU();
			}
		}

		return true;
	}

	/*
		We can't use a single selection for all guids. we need to know where everyone is in the menus.
	*/
	uint32 GetGUIDSelection(uint64 guid)
	{
		if (pdata.find(guid) != pdata.end())
			return pdata[guid];

		return NULL;
	}

	void SetGUIDSelection(uint64 guid, univeral_npc selection)
	{
		if (pdata.find(guid) == pdata.end())
			pdata[guid] = {uint32(NO_MENU)};
		
		pdata[guid] = uint32(selection);
		//TC_LOG_ERROR("sql.sql", "pdata[%u]: %u", guid, pdata[guid]);
	}

private:
	std::map<uint64, uint32> pdata{ {NULL, NULL} };

};

uint32 starter_cloth_dps[] = {
	57864, // Helm
	56110, // Neck
	55876, // Shoulder
	56126, // Back
	55998, // Chest
	55866, // Wrist
	67126, // Hands
	63921, // Waist
	55993, // Legs
	67237, // Feet
	55859, // Ring #1
	56106, // Ring #2
	55889, // Trinket #1
	56138, // Trinket #2
	56137, // Main-Hand
	56122, // Ranged
};

uint32 starter_pri_holy[] = {
	56133, // Helm
	55864, // Neck
	55876, // Shoulder
	56107, // Back
	57860, // Chest
	56125, // Wrist
	67126, // Hands
	55878, // Waist
	56218, // Legs
	56105, // Feet
	55869, // Ring #1
	56000, // Ring #2
	55995, // Trinket #1
	56136, // Trinket #2
	56097, // Main-Hand
	56122, // Ranged
};

uint32 starter_rogue[] = {
	66975, // Helm
	56095, // Neck
	66905, // Shoulder
	55858, // Back
	57863, // Chest
	55886, // Wrist
	66970, // Hands
	56098, // Waist
	63707, // Legs
	55871, // Feet
	56120, // Ring #1
	55994, // Ring #2
	55874, // Trinket #1
	56102, // Trinket #2
	56127, // Main-Hand
	55870, // Off-Hand
	56001, // Ranged
};

uint32 starter_dru_feral[] = {
	66936, // Helm
	56095, // Neck
	66905, // Shoulder
	55858, // Back
	56128, // Chest
	55886, // Wrist
	66970, // Hands
	56098, // Waist
	63707, // Legs
	55871, // Feet
	56120, // Ring #1
	55994, // Ring #2
	55874, // Trinket #1
	56102, // Trinket #2
	55860, // Main-Hand
	63776, // Ranged
};

uint32 starter_dru_balance[] = {
	66921, // Helm
	56110, // Neck
	57858, // Shoulder
	56126, // Back
	56113, // Chest
	67127, // Wrist
	66934, // Hands
	56094, // Waist
	55877, // Legs
	55890, // Feet
	55859, // Ring #1
	56106, // Ring #2
	55889, // Trinket #1
	56138, // Trinket #2
	56137, // Main-Hand
	63772, // Ranged
};

uint32 starter_dru_resto[] = {
	66921, // Helm
	55864, // Neck
	57858, // Shoulder
	56107, // Back
	56113, // Chest
	55882, // Wrist
	66934, // Hands
	56094, // Waist
	55877, // Legs
	63920, // Feet
	55869, // Ring #1
	56000, // Ring #2
	55995, // Trinket #1
	56138, // Trinket #2
	56097, // Main-Hand
	63772, // Ranged
};

uint32 starter_hunter[] = {
	56112, // Helm
	66974, // Neck
	55997, // Shoulder
	55858, // Back
	56515, // Chest
	66935, // Wrist
	66915, // Hands
	63758, // Waist
	55861, // Legs
	57857, // Feet
	56120, // Ring #1
	55994, // Ring #2
	55874, // Trinket #1
	56115, // Trinket #2
	55860, // Main-Hand
	66973, // Ranged
};

uint32 starter_sham_enha[] = {
	56112, // Helm
	63762, // Neck
	55997, // Shoulder
	55858, // Back
	56515, // Chest
	66935, // Wrist
	66915, // Hands
	63758, // Waist
	55861, // Legs
	57857, // Feet
	56120, // Ring #1
	55994, // Ring #2
	55874, // Trinket #1
	56102, // Trinket #2
	55875, // Main-Hand
	55875, // Off-Hand
	63776, // Ranged
};

uint32 starter_sham_ele[] = {
	67123, // Helm
	56110, // Neck
	66938, // Shoulder
	56126, // Back
	63914, // Chest
	66906, // Wrist
	63806, // Hands
	55887, // Waist
	57862, // Legs
	56119, // Feet
	55859, // Ring #1
	56106, // Ring #2
	55889, // Trinket #1
	55879, // Trinket #2
	56116, // Main-Hand
	55880, // Off-Hand
	63772, // Ranged
};

uint32 starter_sham_resto[] = {
	67123, // Helm
	55864, // Neck
	66938, // Shoulder
	56107, // Back
	63914, // Chest
	66906, // Wrist
	55857, // Hands
	55887, // Waist
	57862, // Legs
	56119, // Feet
	55869, // Ring #1
	56000, // Ring #2
	55995, // Trinket #1
	56138, // Trinket #2
	57865, // Main-Hand
	55880, // Off-Hand
	63772, // Ranged
};

uint32 starter_dk_dps[] = {
	55031, // Helm
	55999, // Neck
	56124, // Shoulder
	55872, // Back
	56093, // Chest
	57855, // Wrist
	56099, // Hands
	56118, // Waist
	66911, // Legs
	55856, // Feet
	55996, // Ring #1
	56129, // Ring #2
	55868, // Trinket #1
	56100, // Trinket #2
	56131, // Main-Hand
	55865, // One-Hand
	56101, // Off-Hand
	63888, // Ranged
};

uint32 starter_dk_tank[] = {
	57861, // Helm
	55888, // Neck
	56124, // Shoulder
	56219, // Back
	66933, // Chest
	55992, // Wrist
	56099, // Hands
	67067, // Waist
	66911, // Legs
	66909, // Feet
	55873, // Ring #1
	56114, // Ring #2
	56121, // Trinket #1
	56100, // Trinket #2
	56131, // Main-Hand
	67059, // Ranged
};

uint32 starter_war_dps[] = {
	55031, // Helm
	55999, // Neck
	56124, // Shoulder
	55872, // Back
	56093, // Chest
	57855, // Wrist
	56099, // Hands
	56118, // Waist
	66911, // Legs
	55856, // Feet
	55996, // Ring #1
	55863, // Ring #2
	55868, // Trinket #1
	56100, // Trinket #2
	56131, // Main-Hand
	56131, // Off-Hand
	55865, // One-Hand
	56101, // One-Hand-Off-Hand
	65813, // Ranged
};

uint32 starter_war_tank[] = {
	57861, // Helm
	55888, // Neck
	56124, // Shoulder
	56219, // Back
	66933, // Chest
	55992, // Wrist
	56099, // Hands
	67067, // Waist
	66911, // Legs
	66909, // Feet
	55873, // Ring #1
	56114, // Ring #2
	56121, // Trinket #1
	56100, // Trinket #2
	56130, // Main-Hand
	56096, // Off-Hand
	65813, // Ranged
};

uint32 starter_pala_ret[] = {
	55031, // Helm
	55999, // Neck
	56124, // Shoulder
	55872, // Back
	56093, // Chest
	57855, // Wrist
	56099, // Hands
	56118, // Waist
	66911, // Legs
	55856, // Feet
	55996, // Ring #1
	55863, // Ring #2
	55868, // Trinket #1
	56100, // Trinket #2
	56131, // Main-Hand
	63888, // Ranged
};

uint32 starter_pala_prot[] = {
	57861, // Helm
	55888, // Neck
	56124, // Shoulder
	56219, // Back
	66933, // Chest
	55992, // Wrist
	56099, // Hands
	67067, // Waist
	66911, // Legs
	66909, // Feet
	55873, // Ring #1
	56114, // Ring #2
	56121, // Trinket #1
	56100, // Trinket #2
	56130, // Main-Hand
	56096, // Off-Hand
	67059, // Ranged
};

uint32 starter_pala_holy[] = {
	55039, // Helm
	55864, // Neck
	66907, // Shoulder
	56107, // Back
	57856, // Chest
	56135, // Wrist
	66913, // Hands
	63805, // Waist
	56104, // Legs
	55862, // Feet
	55869, // Ring #1
	56000, // Ring #2
	55995, // Trinket #1
	56136, // Trinket #2
	57865, // Main-Hand
	55880, // Off-Hand
	63772, // Ranged
};

class starter_items : public ItemScript
{
public:
	starter_items() : ItemScript("starter_items") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
	{
		player->PlayerTalkClass->ClearMenus();
		switch (player->getClass())
		{
		case CLASS_MAGE:
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_fire_flamebolt:20:20:-10:0|tFire", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to select Fire?\nThis action cannot be reversed.", 0, false);
			break;
		case CLASS_WARLOCK:
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_shadow_deathcoil:20:20:-10:0|tAffliction", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2, "Are you sure you want to select Affliction?\nThis action cannot be reversed.", 0, false);
			break;
		case CLASS_PRIEST:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_holy_holybolt:20:20:-10:0|tHoly", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3, "Are you sure you want to select Holy?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_shadow_shadowwordpain:20:20:-10:0|tShadow", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4, "Are you sure you want to select Shadow?\nThis action cannot be reversed.", 0, false);
			break;
		}
		case CLASS_ROGUE:
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\ability_rogue_eviscerate:20:20:-10:0|tAssassination", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5, "Are you sure you want to select Assassination?\nThis action cannot be reversed.", 0, false);
			break;
		case CLASS_DRUID:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_nature_starfall:20:20:-10:0|tBalance", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6, "Are you sure you want to select Balance?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\ability_racial_bearform:20:20:-10:0|tFeral", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7, "Are you sure you want to select Feral?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_nature_healingtouch:20:20:-10:0|tRestoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8, "Are you sure you want to select Restoration?\nThis action cannot be reversed.", 0, false);
			break;
		}
		case CLASS_HUNTER:
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\ability_Hunter_swiftstrike:20:20:-10:0|tSurvival", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9, "Are you sure you want to select Survival?\nThis action cannot be reversed.", 0, false);
			break;
		case CLASS_SHAMAN:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_nature_lightning:20:20:-10:0|tElemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10, "Are you sure you want to select Elemental?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_nature_lightningshield:20:20:-10:0|tEnhancement", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11, "Are you sure you want to select Enhancement?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_nature_magicimmunity:20:20:-10:0|tRestoration", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12, "Are you sure you want to select Restoration?\nThis action cannot be reversed.", 0, false);
			break;
		}
		case CLASS_DEATH_KNIGHT:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_deathknight_bloodpresence:20:20:-10:0|tBlood", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13, "Are you sure you want to select Blood?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_deathknight_unholypresence:20:20:-10:0|tUnholy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14, "Are you sure you want to select Unholy?\nThis action cannot be reversed.", 0, false);
			break;
		}
		case CLASS_WARRIOR:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\ability_warrior_innerrage:20:20:-10:0|tFury", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15, "Are you sure you want to select Fury?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\ability_warrior_defensivestance:20:20:-10:0|tProtection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16, "Are you sure you want to select Protection?\nThis action cannot be reversed.", 0, false);
			break;
		}
		case CLASS_PALADIN:
		{
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_holy_holybolt:20:20:-10:0|tHoly", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17, "Are you sure you want to select Holy?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_holy_devotionaura:20:20:-10:0|tProtection", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18, "Are you sure you want to select Protection?\nThis action cannot be reversed.", 0, false);
			player->ADD_GOSSIP_ITEM_EXTENDED(-1, "|TInterface\\icons\\spell_holy_auraoflight:20:20:-10:0|tRetribution", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19, "Are you sure you want to select Retribution?\nThis action cannot be reversed.", 0, false);
			break;
		}
		}
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Close menu", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
		player->SEND_GOSSIP_MENU(930000, item->GetGUID());
		return false;
	}

	void OnGossipSelect(Player* player, Item* item, uint32 /*sender*/, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();

		std::list<uint32> starterItems; //(std::begin(starter_cloth_dps), std::end(starter_cloth_dps));

		switch (action)
		{
		case GOSSIP_ACTION_INFO_DEF + 1: // Mage
		case GOSSIP_ACTION_INFO_DEF + 2: // Warlock
		case GOSSIP_ACTION_INFO_DEF + 4: // Shadow Priest
			for (auto i = std::begin(starter_cloth_dps); i != std::end(starter_cloth_dps); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 3: // Holy Priest
			for (auto i = std::begin(starter_pri_holy); i != std::end(starter_pri_holy); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 5: // Rogue
			for (auto i = std::begin(starter_rogue); i != std::end(starter_rogue); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 6: // Balance Druid
			for (auto i = std::begin(starter_dru_balance); i != std::end(starter_dru_balance); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 7: // Feral Druid
			for (auto i = std::begin(starter_dru_feral); i != std::end(starter_dru_feral); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 8: // Restoration Druid
			for (auto i = std::begin(starter_dru_resto); i != std::end(starter_dru_resto); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 9: // Hunter
			for (auto i = std::begin(starter_hunter); i != std::end(starter_hunter); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 10: // Elemental Shaman
			for (auto i = std::begin(starter_sham_ele); i != std::end(starter_sham_ele); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 11: // Enhancement Shaman
			for (auto i = std::begin(starter_sham_enha); i != std::end(starter_sham_enha); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 12: // Restoration Shaman
			for (auto i = std::begin(starter_sham_resto); i != std::end(starter_sham_resto); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 13: // Blood Death Knight
			for (auto i = std::begin(starter_dk_tank); i != std::end(starter_dk_tank); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 14: // Unholy/Frost Death Knight
			for (auto i = std::begin(starter_dk_dps); i != std::end(starter_dk_dps); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 15: // Fury/Arms Warrior
			for (auto i = std::begin(starter_war_dps); i != std::end(starter_war_dps); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 16: // Protection Warrior
			for (auto i = std::begin(starter_war_tank); i != std::end(starter_war_tank); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 17: // Holy Paladin
			for (auto i = std::begin(starter_pala_holy); i != std::end(starter_pala_holy); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 18: // Protection Paladin
			for (auto i = std::begin(starter_pala_prot); i != std::end(starter_pala_prot); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 19: // Retribution Paladin
			for (auto i = std::begin(starter_pala_ret); i != std::end(starter_pala_ret); ++i)
				starterItems.push_back(*i);
			break;
		case GOSSIP_ACTION_INFO_DEF + 20: // Close
			player->CLOSE_GOSSIP_MENU();
			return;
			break;
		default:
			return;
		}

		while (!starterItems.empty())
		{
			MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);
			MailDraft draft("Starter Items", "Thank you for your support,\n\nplease see the included item(s) for your starter items.\n\nProject Nelth Team");

			SQLTransaction trans = CharacterDatabase.BeginTransaction();

			for (uint8 i = 0; !starterItems.empty() && i < MAX_MAIL_ITEMS; ++i)
			{
				if (Item* item = Item::CreateItem(starterItems.front(), 1))
				{
					item->SaveToDB(trans);
					draft.AddItem(item);
					starterItems.pop_front();
				}
			}
			draft.SendMailTo(trans, MailReceiver(player, GUID_LOPART(player->GetGUID())), sender);
			CharacterDatabase.CommitTransaction(trans);
		}

		player->CLOSE_GOSSIP_MENU();
		player->DestroyItemCount(item->GetEntry(), 1, true);
		ChatHandler(player->GetSession()).PSendSysMessage("Your selected items has been sent to your mailbox.");
	}
};

class teleport_rune : public ItemScript
{
public:
	teleport_rune() : ItemScript("teleport_rune") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
	{
		if (player->isInCombat())
		{
			player->GetSession()->SendAreaTriggerMessage("|cffff0000You are in combat!|r");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}
		
		player->PlayerTalkClass->ClearMenus();
		player->ADD_GOSSIP_ITEM_EXTENDED(-1, "Teleport", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "Are you sure you want to teleport now?\nTeleportation Rune can only be used once!", 0, false);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());
		return false;
	}

	void OnGossipSelect(Player* player, Item* item, uint32 /*sender*/, uint32 action) override
	{
		if (player->isInCombat())
		{
			player->GetSession()->SendAreaTriggerMessage("|cffff0000You are in combat!|r");
			player->CLOSE_GOSSIP_MENU();
			return;
		}

		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
		case GOSSIP_ACTION_INFO_DEF + 1:
			if (player->GetTeam() == ALLIANCE)
				player->TeleportTo(0, -8853.607f, 657.064f, 98.225f, 6.225f); // Sw
			else
				player->TeleportTo(1, 1595.626f, -4413.430f, 16.462f, 5.163f); // Org
			break;
		default:
			break;
		}

		player->CLOSE_GOSSIP_MENU();
		player->DestroyItemCount(item->GetEntry(), 1, true);
	}
};

void AddSC_BackgroundSQLTask()
{
    new BackgroundSQLTask();
	new item_char_rename();
	new item_char_customize();
	new item_char_racechange();
	new item_char_factionchange();
	new item_char_levelup();
	new item_char_levelup2();
	new item_guild_levelup();
	new item_guild_levelup2();
	//new npc_starting_options();
	new CharacterCreationProcedures();
	new ArenaDampening();
	new SaveNewCap();
	new npc_soloq_guard();
	new item_account_upgrade();
	new starter_items();
	new teleport_rune();
}