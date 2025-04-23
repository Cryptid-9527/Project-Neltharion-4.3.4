#include "arena_rewards.h"

void ArenaRewardsData::InitPlayerData(Player* plr)
{
	// Distribute titles on login -- PlayerScript("ArenaTitlesHelper")
	if (plr->GetPendingArenaReward().titleId > 1)
	{
		uint32 questid = 0;
		uint32 titleid = plr->GetPendingArenaReward().titleId;

		switch (titleid)
		{
		case GLADIATOR_TITLE:	questid = GLADIATOR_QUEST;		break;
		case DUELIST_TITLE:		questid = DUELIST_QUEST;		break;
		case RIVAL_TITLE:		questid = RIVAL_QUEST;			break;
		case CHALLENGR_TITLE:	questid = CHALLENGR_QUEST;		break;
		case MERCILESS_GLAD:	questid = MERCILESS_QUEST;		break;
		case VENGEFUL_GLAD:		questid = VENGEFUL_QUEST;		break;
		case BRUTAL_GLAD:		questid = BRUTAL_QUEST;			break;
		case DEADLY_GLAD:		questid = DEADLY_QUEST;			break;
		case FURIOUS_GLAD:		questid = FURIOUS_QUEST;		break;
		case RELENTLESS_GLAD:	questid = RELENTLESS_QUEST;		break;
		case WRATHFUL_GLAD:		questid = WRATHFUL_QUEST;		break;
		case VICIOUS_GLAD:		questid = VICIOUS_QUEST;		break;
		case RUTHLESS_GLAD:		questid = RUTHLESS_QUEST;		break;
		case CATACLYSMIC_GLAD:	questid = CATACLYSMIC_QUEST;	break;
		}

		if (!questid)
			return;

		if (Quest const* qInfo = sObjectMgr->GetQuestTemplate(questid))
		{
			plr->AddQuest(qInfo, nullptr);
			plr->CompleteQuest(questid);

			if (qInfo->GetQuestId() >= MERCILESS_QUEST)
			{
				std::stringstream ss;
				std::string name = plr->GetName();
				std::string title = qInfo->GetTitle();
				ss << "|cffADFF2FCongratulations to|r |cffFFC125" << name << "|r |cffADFF2Ffor obtaining the|r |cffFFC125" << title << "|r |cffADFF2Ftitle!|r";
				std::string post = ss.str();
				sWorld->SendServerMessage(SERVER_MSG_STRING, post.c_str());
			}
		}
		plr->RemovePendingArenaReward();
	}
}

void ArenaRewardsData::DistributeArenaTitles()
{
	m_distributeTitlesForArenaSlot[0] = true;
	m_distributeTitlesForArenaSlot[1] = true;
	m_distributeTitlesForArenaSlot[2] = false;
	
	sObjectAccessor->SaveAllPlayers();
	sWorld->SendServerMessage(SERVER_MSG_STRING, "Flushing Arena Titles from previous season...");
	PreparedStatement* stmt1 = CharacterDatabase.GetPreparedStatement(CHAR_CLEAN_ARENA_REWARDS);
	CharacterDatabase.Query(stmt1); // Clear data from previous season
	PreparedStatement* stmt2 = CharacterDatabase.GetPreparedStatement(CHAR_CLEAN_ARENA_TITLES);
	CharacterDatabase.Query(stmt2); // Clear titles from previous season
	PreparedStatement* stmt3 = CharacterDatabase.GetPreparedStatement(CHAR_UNCLAIMED_TITLE_CLEAN);
	CharacterDatabase.Query(stmt3); // Clear unclaimed titles from pending_arena_titles

	sWorld->SendServerMessage(SERVER_MSG_STRING, "Loading current Arena Teams...");
	ArenaRatingSorter* teams = new ArenaRatingSorter();
	teams->GenerateArenaTeamRankings();

	sWorld->SendServerMessage(SERVER_MSG_STRING, "Initializing cutoffs data...");
	uint32 teamcount[3] = { 0, 0, 0 };
	for (vector<ArenaTeamSortingData*>::iterator itr = teams->sorted.begin(); itr != teams->sorted.end(); itr++)
		if (ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById((*itr)->id))
			teamcount[at->GetSlot()]++;

	// r1 Gladiator is reserved for the top 0.1% of teams, Gladiator for 0.5%, Duelist for 3%, Rival for 10% and Challenger for 35%.
	
	uint32 twoOne		= ceil(teamcount[0] * 0.001f);
	uint32 twoTwo		= ceil(teamcount[0] * 0.005f);
	uint32 twoThree		= ceil(teamcount[0] * 0.03f);
	uint32 twoFour		= ceil(teamcount[0] * 0.10f);
	uint32 twoFive		= ceil(teamcount[0] * 0.35f);
	uint32 threeOne		= ceil(teamcount[1] * 0.001f);
	uint32 threeTwo		= ceil(teamcount[1] * 0.005f);
	uint32 threeThree	= ceil(teamcount[1] * 0.03f);
	uint32 threeFour	= ceil(teamcount[1] * 0.10f);
	uint32 threeFive	= ceil(teamcount[1] * 0.35f);
	uint32 fiveOne		= ceil(teamcount[2] * 0.001f);
	uint32 fiveTwo		= ceil(teamcount[2] * 0.005f);
	uint32 fiveThree	= ceil(teamcount[2] * 0.03f);
	uint32 fiveFour		= ceil(teamcount[2] * 0.10f);
	uint32 fiveFive		= ceil(teamcount[2] * 0.35f);

	uint32 teamdata[3][5]; // TeamData[Bracket][ArenaTitleRank] = slots
	
	// 2v2 reward counter
	teamdata[0][0] = twoOne;
	teamdata[0][1] = twoTwo;
	teamdata[0][2] = twoThree;
	teamdata[0][3] = twoFour;
	teamdata[0][4] = twoFive;;

	// 3v3 reward counter
	teamdata[1][0] = threeOne;
	teamdata[1][1] = threeTwo;
	teamdata[1][2] = threeThree;
	teamdata[1][3] = threeFour;
	teamdata[1][4] = threeFive;

	// 5v5 rewards counter
	teamdata[2][0] = fiveOne;
	teamdata[2][1] = fiveTwo;
	teamdata[2][2] = fiveThree;
	teamdata[2][3] = fiveFour;
	teamdata[2][4] = fiveFive;

	// Force team counter slots if lower than they should be (not enough teams)
	teamdata[0][0] == 0 ? teamdata[0][0] = 1 : teamdata[0][0] = teamdata[0][0];
	teamdata[0][1] < 2	? teamdata[0][1] = 2 : teamdata[0][1] = teamdata[0][1];
	teamdata[0][2] < 3	? teamdata[0][2] = 3 : teamdata[0][2] = teamdata[0][2];
	teamdata[0][3] < 4	? teamdata[0][3] = 4 : teamdata[0][3] = teamdata[0][3];
	teamdata[0][4] < 5	? teamdata[0][4] = 5 : teamdata[0][4] = teamdata[0][4];

	teamdata[1][0] == 0 ? teamdata[1][0] = 1 : teamdata[1][0] = teamdata[1][0];
	teamdata[1][1] < 2	? teamdata[1][1] = 2 : teamdata[1][1] = teamdata[1][1];
	teamdata[1][2] < 3	? teamdata[1][2] = 3 : teamdata[1][2] = teamdata[1][2];
	teamdata[1][3] < 4	? teamdata[1][3] = 4 : teamdata[1][3] = teamdata[1][3];
	teamdata[1][4] < 5	? teamdata[1][4] = 5 : teamdata[1][4] = teamdata[1][4];

	teamdata[2][0] == 0 ? teamdata[2][0] = 1 : teamdata[2][0] = teamdata[2][0];
	teamdata[2][1] < 2	? teamdata[2][1] = 2 : teamdata[2][1] = teamdata[2][1];
	teamdata[2][2] < 3	? teamdata[2][2] = 3 : teamdata[2][2] = teamdata[2][2];
	teamdata[2][3] < 4	? teamdata[2][3] = 4 : teamdata[2][3] = teamdata[2][3];
	teamdata[2][4] < 5	? teamdata[2][4] = 5 : teamdata[2][4] = teamdata[2][4];

	uint32 rankcount[3] = { 0, 0, 0 }; // 2s, 3s and 5s reward counter
	uint32 ranktype[3] = { R1_GLAD, R1_GLAD, R1_GLAD }; // 2s, 3s, and 5s title counter
	map<uint64, vector<uint32>> titleList;

	sWorld->SendServerMessage(SERVER_MSG_STRING, "Checking eligibility for new rewards...");

	for (vector<ArenaTeamSortingData*>::iterator itr = teams->sorted.begin(); itr != teams->sorted.end(); itr++)
	{
		ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById((*itr)->id);
		if (!at)
			continue;
		if (!TitleDistributionForSlotEnabled(at->GetSlot()))
			continue;
		if (at->GetRating() < 1000)
			continue; // Dont bother with rewards below 1k rating

		rankcount[at->GetSlot()]++;
		if (rankcount[at->GetSlot()] <= teamdata[at->GetSlot()][ranktype[at->GetSlot()]])
		{
			std::list<ArenaTeamMember> membersList = at->GetMembers();
			for (std::list<ArenaTeamMember>::iterator itr = membersList.begin(); itr != membersList.end(); ++itr)
			{
				if (float(itr->SeasonGames) / float(at->GetStats().SeasonGames) > 0.70f) // 70% games played minimum
				{
					if (itr->PersonalRating >= (at->GetRating() - 150)) // is within 150 rating of the team
					{
						uint64 rc = itr->Guid;

						uint32 titleID = 0;

						switch (ranktype[at->GetSlot()]) {
						case GLADIATOR:		titleID = GLADIATOR_TITLE;	break;
						case DUELIST:		titleID = DUELIST_TITLE;	break;
						case RIVAL:			titleID = RIVAL_TITLE;		break;
						case CHALLENGER:	titleID = CHALLENGR_TITLE;	break;

						case R1_GLAD:
						{
							if (at->GetType() == 3) // Only give r1 titles in 3v3
							{
								titleID = MERCILESS_GLAD; // what comes next is not optimal way to do this roflmao
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, MERCILESS_QUEST))
									titleID = VENGEFUL_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, VENGEFUL_QUEST))
									titleID = BRUTAL_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, BRUTAL_QUEST))
									titleID = DEADLY_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, DEADLY_QUEST))
									titleID = FURIOUS_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, FURIOUS_QUEST))
									titleID = RELENTLESS_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, RELENTLESS_QUEST))
									titleID = WRATHFUL_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, WRATHFUL_QUEST))
									titleID = CATACLYSMIC_GLAD;
								if (QueryResult result = CharacterDatabase.PQuery("SELECT * FROM `character_queststatus_rewarded` WHERE `guid` = '%u' AND `quest` = '%u';", rc, CATACLYSMIC_QUEST))
									titleID = GLADIATOR_TITLE; // End of r1 titles, start giving regular glad instead
							}
							else // if 2v2 or 5v5
								titleID = GLADIATOR_TITLE;
						} break;
						}

						if (itr->PersonalRating < 2000)
							if (titleID == GLADIATOR_TITLE || titleID >= MERCILESS_GLAD)
								titleID = DUELIST_TITLE; // dont give gladiator titles below 2000 rating

						CharTitlesEntry const* titleInfo = sCharTitlesStore.LookupEntry(titleID);

						if (titleInfo)
							titleList[rc].push_back(titleID);

						std::string titleName;
						switch (titleID)
						{
						case GLADIATOR_TITLE:	titleName = "Gladiator";				break;
						case DUELIST_TITLE:		titleName = "Duelist";					break;
						case RIVAL_TITLE:		titleName = "Rival";					break;
						case CHALLENGR_TITLE:	titleName = "Challenger";				break;
						case MERCILESS_GLAD:	titleName = "Merciless Gladiator";		break;
						case VENGEFUL_GLAD:		titleName = "Vengeful Gladiator";		break;
						case BRUTAL_GLAD:		titleName = "Brutal Gladiator";			break;
						case DEADLY_GLAD:		titleName = "Deadly Gladiator";			break;
						case FURIOUS_GLAD:		titleName = "Furious Gladiator";		break;
						case RELENTLESS_GLAD:	titleName = "Relentless Gladiator";		break;
						case WRATHFUL_GLAD:		titleName = "Wrathful Gladiator";		break;
						case VICIOUS_GLAD:		titleName = "Vicious Gladiator";		break;
						case RUTHLESS_GLAD:		titleName = "Ruthless Gladiator";		break;
						case CATACLYSMIC_GLAD:	titleName = "Cataclysmic Gladiator";	break;
						}

						MailSender sender(MAIL_NORMAL, 1); // Sender is Radioactive (MAIL_NORMAL, 0, MAIL_STATIONERY_GM)
						char subject[255];
						char message[255];
						sprintf(subject, "Rank %u Arena Team!", rankcount[at->GetSlot()]);
						if (titleID >= 62) // if r1 title
							sprintf(message, "Congratulations!\n\nArena season has concluded and you were part of the rank %u arena team in the %uv%u bracket!\n\nYou have earned yourself the permanent title '%s' !", rankcount[at->GetSlot()], at->GetType(), at->GetType(), titleName.c_str());
						else // if seasonal titles
							sprintf(message, "Congratulations!\n\nArena season has concluded and you were part of the rank %u arena team in the %uv%u bracket!\n\nYou have earned yourself the title '%s' in the preceding arena season!", rankcount[at->GetSlot()], at->GetType(), at->GetType(), titleName.c_str());
						string sub = subject;
						string mes = message;
						MailDraft draft(sub, mes);

						SQLTransaction trans = CharacterDatabase.BeginTransaction();

						if (ranktype[at->GetSlot()] == R1_GLAD || ranktype[at->GetSlot()] == GLADIATOR)
						{
							if (titleID != DUELIST_TITLE)
							{
								uint32 mountID = 0;
								uint32 flymountID = 0;

								switch (titleID)
								{
								case GLADIATOR_TITLE:	flymountID = 30609;		break;
								case MERCILESS_GLAD:	flymountID = 34092;		break;
								case VENGEFUL_GLAD:		flymountID = 37676;		break;
								case BRUTAL_GLAD:		flymountID = 43516;		break;
								case DEADLY_GLAD:		flymountID = 46708;		break;
								case FURIOUS_GLAD:		flymountID = 46171;		break;
								case RELENTLESS_GLAD:	flymountID = 47840;		break;
								case WRATHFUL_GLAD:		flymountID = 50435;		break;
								case VICIOUS_GLAD:		flymountID = 71339;		break;
								case RUTHLESS_GLAD:		flymountID = 71954;		break;
								case CATACLYSMIC_GLAD:	flymountID = 77956;		break;
								}

								if (Item* mailItem1 = Item::CreateItem(flymountID, 1))
								{
									mailItem1->SaveToDB(trans);
									draft.AddItem(mailItem1);
								}

								if (titleID == GLADIATOR_TITLE)
									mountID = 33809; // Amani War bear
								if (titleID >= MERCILESS_GLAD)
									mountID = 49290; // Chicken mount

								if (Item* mailItem2 = Item::CreateItem(mountID, 1))
								{
									mailItem2->SaveToDB(trans);
									draft.AddItem(mailItem2);
								}
							}
						}

						draft.SendMailTo(trans, MailReceiver(/*receive, */GUID_LOPART(rc)), sender);
						CharacterDatabase.CommitTransaction(trans);
					}
				}
			}
		}

		if (rankcount[at->GetSlot()] == teamdata[at->GetSlot()][ranktype[at->GetSlot()]])
		{
			if (ranktype[at->GetSlot()] < CHALLENGER)
				ranktype[at->GetSlot()]++;

			else
				ToggleTitleDistributionForSlot(at->GetSlot(), false);
		}
	}

	sWorld->SendServerMessage(SERVER_MSG_STRING, "Distributing new arena rewards to players...");

	// Clean titles for people who have more than one.
	for (map<uint64, vector<uint32>>::iterator itr = titleList.begin(); itr != titleList.end(); ++itr)
	{
		uint32 finalTitle = 0;
		for (vector<uint32>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
		{
			if (!finalTitle)			// No title yet, set to first one.
				finalTitle = (*itr2);
			else if (finalTitle < MERCILESS_GLAD && finalTitle > (*itr2))
				finalTitle = (*itr2);	// If title is greater than another one, set it to that as it is a better rank
		}
		PreparedStatement* stmt1 = CharacterDatabase.GetPreparedStatement(CHAR_REWARD_ARENA);
		stmt1->setUInt32(0, GUID_LOPART(itr->first));
		stmt1->setUInt16(1, finalTitle);
		CharacterDatabase.Query(stmt1);

		PreparedStatement* stmt2 = CharacterDatabase.GetPreparedStatement(CHAR_ARENA_REWARDS);
		stmt2->setUInt32(0, GUID_LOPART(itr->first));
		stmt2->setUInt16(1, finalTitle);
		CharacterDatabase.Query(stmt2);

		if (finalTitle >= MERCILESS_GLAD)
		{
			if (QueryResult result = CharacterDatabase.PQuery("SELECT `count` FROM `cataclysmic_gladiators` WHERE `guid` = '%u';", GUID_LOPART(itr->first)))
			{
				if (result->Fetch()[0].GetUInt32() < 6)
					CharacterDatabase.PQuery("UPDATE `cataclysmic_gladiators` SET `count` = count+1 WHERE `guid` = '%u';", GUID_LOPART(itr->first));
				else
					CharacterDatabase.PQuery("UPDATE `cataclysmic_gladiators` SET `count` = '7' WHERE `guid` = '%u';", GUID_LOPART(itr->first));
			}
			else
				CharacterDatabase.PQuery("INSERT INTO `cataclysmic_gladiators` VALUES ('%u', 1);", GUID_LOPART(itr->first));
		}
	}
	sWorld->SendServerMessage(SERVER_MSG_STRING, "All players saved, server restarting shortly to start a new season!");
}

void ArenaRatingSorter::GenerateArenaTeamRankings()
{
	for (ArenaTeamMgr::ArenaTeamContainer::iterator team_itr = sArenaTeamMgr->GetArenaTeamMapBegin(); team_itr != sArenaTeamMgr->GetArenaTeamMapEnd(); ++team_itr)
	{
		ArenaTeam* at = team_itr->second;
		if (!at)
			continue;

		if (at->GetRating() < 50) // dont include teams below this rating
			continue;

		ArenaTeamSortingData* n = new ArenaTeamSortingData();
		n->id = at->GetId();
		n->rating = at->GetRating();
		n->slot = at->GetSlot();
		unsorted.push_back(n);
	}
	Sort();
}

void ArenaRatingSorter::FindHighest(uint32 pos)
{
	bool highest = true;
	for (uint32 x = 0; x < temp.size(); x++)
		if (unsorted.at(pos)->rating < temp.at(x)->rating)
		{
			highest = false;
			break;
		}

	if (highest)
	{
		ArenaTeamSortingData* n = new ArenaTeamSortingData();
		n->id = unsorted.at(pos)->id;
		n->rating = unsorted.at(pos)->rating;
		n->slot = unsorted.at(pos)->slot;
		sorted.push_back(n);
		temp.erase(temp.begin() + pos);
		unsorted.erase(unsorted.begin() + pos);
	}
}

void ArenaRatingSorter::Sort()
{
	while (!unsorted.empty())
	{
		temp = unsorted;
		for (uint32 x = 0; x < unsorted.size(); x++)
			FindHighest(x);
	}
}

class arenaflush_commands : public CommandScript
{
public:
	arenaflush_commands() : CommandScript("arenaflush_commands") { }

	std::vector<ChatCommand> GetCommands() const override
	{
		static std::vector<ChatCommand> arenaCommandTable =
		{
			{ "distributeTitles",	SEC_CONSOLE,				false, HandleFlushArenaCommand,				"" },
			//{ "dummyteams",			SEC_CONSOLE,				false, HandleDummyTeams3Command,			"" },
		};
		static std::vector<ChatCommand> commandTable =
		{
			{ "arenaSeason",        SEC_CONSOLE,				true, NULL,				"", arenaCommandTable  }
		};
		return commandTable;
	}

	static bool HandleFlushArenaCommand(ChatHandler* handler, char const* /*args*/)
	{
		if (auto player = handler->GetSession()->GetPlayer())
		{
			if (player->GetGUID() == 1)
			{
				ArenaRewardsData season;
				season.DistributeArenaTitles();
				return true;
			}
			else
				return false;
		}

		return false;
	}

	//static bool HandleDummyTeams3Command(ChatHandler* handler, char const* /*args*/)
	/*{
		if (auto player = handler->GetSession()->GetPlayer())
		{
			if (player->GetGUID() == 1)
			{
				handler->PSendSysMessage("Creating 3v3 arena teams...");

				uint32 TeamID = 2;
				std::string Name = "ArenaTeam3v3";
				uint32 TeamNumber = 2;
				uint32 captain = 2;
				uint32 rating = 0;
				for (uint32 i = 0; i < 3600; ++i)
				{
					CharacterDatabase.PQuery("INSERT INTO `arena_team` (`arenaTeamId`, `name`, `captainGuid`, `type`, `rating`, `seasonGames`, `seasonWins`, `backgroundColor`, `emblemColor`, `borderColor`, `emblemStyle`, `borderStyle`) VALUES ('%u', '%s %u', '%u', '3', '%u', '100', '100', '4283617661', '4287414197', '4283100688', '63', '2');", TeamID, Name, TeamNumber, captain, rating);
					TeamID += 1;
					TeamNumber += 1;
					captain += 1;
					rating += 1;
				}
				handler->PSendSysMessage("Completed creating 3v3 arena teams");

				handler->PSendSysMessage("Filling 3v3 team members...");

				TeamID = 2;
				uint32 apguid = 2;
				rating = 0;
				for (uint32 i = 0; i < 3600; ++i)
				{
					CharacterDatabase.PQuery("INSERT INTO `arena_team_member` (`arenaTeamId`, `guid`, `type`, `seasonGames`, `seasonWins`, `personalRating`) VALUES ('%u', '%u', '3', '100', '100', '%u');", TeamID, apguid, rating);
					TeamID += 1;
					apguid += 1;
					rating += 1;
				}
				handler->PSendSysMessage("Completed filling team members");
				return true;
			}
			else
				return false;
		}

		return false;
	}*/
};

class ArenaTitlesHelper : public PlayerScript
{
public:
	ArenaTitlesHelper() : PlayerScript("ArenaTitlesHelper") {}

	void OnLogin(Player* player)
	{
		ArenaRewardsData toPlayer;
		toPlayer.InitPlayerData(player);
	}

};

void AddSC_ArenaRewards()
{
	new arenaflush_commands();
	new ArenaTitlesHelper();
}