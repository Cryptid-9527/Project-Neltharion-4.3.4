#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundQueue.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "BattlegroundCrossFaction.h"
#include "lfg.h"

/*####################################################################################
###############################CROSSFACTION BATTLEGROUNDS#############################
####################################################################################*/

uint8 Unit::getRace(bool forceoriginal) const
{
    if (GetTypeId() == TYPEID_PLAYER)
    {
        Player const* player = ToPlayer();

        if (forceoriginal)
            return player->getORace();

        if (player->InArena())
            return GetByteValue(UNIT_FIELD_BYTES_0, 0);

        if (!player->IsPlayingNative())
            return player->getFRace();
    }

    return GetByteValue(UNIT_FIELD_BYTES_0, 0);
 }

bool Player::SendRealNameQuery()
{
    if (IsPlayingNative())
        return false;

    WorldPacket data(SMSG_NAME_QUERY_RESPONSE, (8 + 1 + 1 + 1 + 1 + 1 + 10));
    data.appendPackGUID(GetGUID()); // player guid
    data << uint8(0); // added in 3.1; if > 1, then end of packet
    data << GetName(); // played nam
    data << uint8(0); // realm name for cross realm BG usage
    data << uint8(getORace());
    data << uint8(getGender());
    data << uint8(getClass());
    data << uint8(0); // is not declined

    return true;
}

void Player::SetFakeRace()
{
    switch (getORace()) {
    case RACE_BLOODELF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        case CLASS_PALADIN:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_TAUREN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_DRAENEI; /*Draeni*/
            break;
        case CLASS_PALADIN:
            m_FakeRace = RACE_DRAENEI; /*Draeni*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_DRAENEI; /*Draeni*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_DRAENEI; /*Draeni*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_DRAENEI; /*Draeni*/
            break;
        case CLASS_DRUID:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_GOBLIN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_GNOME; /*Gnome*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_GNOME; /*Gnome*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_GNOME; /*Gnome*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_GNOME; /*Gnome*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_GNOME; /*Gnome*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_UNDEAD_PLAYER:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_HUMAN; /*Human*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_ORC:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_TROLL:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_DWARF; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_WORGEN; /*Worgen*/
            break;
        case CLASS_DRUID:
            m_FakeRace = RACE_NIGHTELF; /*Night Elf*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;

    case RACE_HUMAN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        case CLASS_PALADIN:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_UNDEAD_PLAYER; /*Forsaken*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_GNOME:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_GOBLIN; /*Goblin*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_GOBLIN; /*Goblin*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_GOBLIN; /*Goblin*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_GOBLIN; /*Goblin*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_DWARF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_PALADIN:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_GOBLIN; /*Goblin*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_WORGEN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_WARLOCK:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        case CLASS_DRUID:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_NIGHTELF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_ROGUE:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_BLOODELF; /*Blood Elf*/
            break;
        case CLASS_DRUID:
            m_FakeRace = RACE_TROLL; /*Troll*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_DRAENEI:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        case CLASS_PALADIN:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        case CLASS_SHAMAN:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        case CLASS_HUNTER:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        case CLASS_MAGE:
            m_FakeRace = RACE_ORC; /*Orc*/
            break;
        case CLASS_PRIEST:
            m_FakeRace = RACE_TAUREN; /*Tauren*/
            break;
        default:
            m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.
        }
        break;
    default: m_FakeRace = RACE_HUMAN; //This should never occur, but it will be noticeable if it does.    
    }
}

bool Player::SendBattleGroundChat(uint32 msgtype, std::string message)
{
    // Select distance to broadcast to.
    float distance = msgtype == CHAT_MSG_SAY ? sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY) : sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL);

    if (auto pBattleGround = GetBattleground())
    {
        if (pBattleGround->isArena()) // Only fake chat in BG's. CFBG should not interfere with arenas.
            return false;

        for (Battleground::BattlegroundPlayerMap::const_iterator itr = pBattleGround->GetPlayers().begin(); itr != pBattleGround->GetPlayers().end(); ++itr)
        {
            if (auto pPlayer = ObjectAccessor::FindPlayer(itr->first))
            {
                {
                    if (GetDistance2d(pPlayer->GetPositionX(), pPlayer->GetPositionY()) <= distance)
                    {
                        WorldPacket data(SMSG_MESSAGECHAT, 200);

                        int language = 0;

                        language = (pPlayer->GetTeam() == ALLIANCE ? LANG_ORCISH : LANG_COMMON);
                        if (IsPlayingNative())
                            if (!pPlayer->IsPlayingNative())
                                if (GetTeam() == pPlayer->GetOTeam())
                                {
                                    /*
                                        This is a special case. The player is playing crossfaction and is receiving a language that their client is set to understand from the other side.
                                        We need to change it over to another language to completely seal off communication between the sides.
                                    */
                                    language = LANG_TITAN;
                                }

                        if (GetTeam() == pPlayer->GetTeam())
                            BuildPlayerChat(&data, msgtype, message, LANG_UNIVERSAL, "");
                        else if (msgtype != CHAT_MSG_EMOTE)
                            BuildPlayerChat(&data, msgtype, message, language, "");

                        pPlayer->GetSession()->SendPacket(&data);
                    }
                }

            }
        }
        return true;
    }
    else
        return false;
}

void Player::MorphFit(bool value)
{
    if (!IsPlayingNative() && value)
    {
        //The player needs a new model
        SetDisplayId(get_Ideal_Crossfaction_Model());
        SetNativeDisplayId(get_Ideal_Crossfaction_Model());
        //The player needs a new model
    }
    else
        InitDisplayIds();
}

uint16 Player::get_Ideal_Crossfaction_Model() {
    switch (getORace()) {
    case RACE_BLOODELF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        case CLASS_PALADIN:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_TAUREN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37916 : 20323; /*Draenei*/
            break;
        case CLASS_PALADIN:
            return getGender() == GENDER_MALE ? 37916 : 20323; /*Draenei*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 37916 : 20323; /*Draenei*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 37916 : 20323; /*Draenei*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37916 : 20323; /*Draenei*/
            break;
        case CLASS_DRUID:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_GOBLIN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37913 : 20320; /*Gnome*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37913 : 20320; /*Gnome*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 37913 : 20320; /*Gnome*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 37913 : 20320; /*Gnome*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37913 : 20320; /*Gnome*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_UNDEAD_PLAYER:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 19723 : 19724; /*Human*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_ORC:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_TROLL:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 20317 : 37918; /*Dwarf*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37915 : 37914; /*Worgen*/
            break;
        case CLASS_DRUID:
            return getGender() == GENDER_MALE ? 39690 : 37919; /*Night Elf*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;

    case RACE_HUMAN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        case CLASS_PALADIN:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37923 : 37924; /*Forsaken*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_GNOME:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37927 : 37928; /*Goblin*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37927 : 37928; /*Goblin*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 37927 : 37928; /*Goblin*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37927 : 37928; /*Goblin*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_DWARF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_PALADIN:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 37927 : 37928; /*Goblin*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_WORGEN:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_WARLOCK:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        case CLASS_DRUID:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_NIGHTELF:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_ROGUE:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 20578 : 20579; /*Blood Elf*/
            break;
        case CLASS_DRUID:
            return getGender() == GENDER_MALE ? 20321 : 37922; /*Troll*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;
    case RACE_DRAENEI:
        switch (getClass()) {
        case CLASS_DEATH_KNIGHT:
        case CLASS_WARRIOR:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        case CLASS_PALADIN:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        case CLASS_SHAMAN:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        case CLASS_HUNTER:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        case CLASS_MAGE:
            return getGender() == GENDER_MALE ? 37920 : 20316; /*Orc*/
            break;
        case CLASS_PRIEST:
            return getGender() == GENDER_MALE ? 20585 : 20584; /*Tauren*/
            break;
        default:
            return 29344; //This should never occur, but it will be noticeable if it does.
        }
        break;

    default: return 29344; //This should never occur, but it will be noticeable if it does.    
    }
}

void Player::FitPlayerInTeam(bool action, Battleground* pBattleGround)
{
    if (!pBattleGround)
        pBattleGround = GetBattleground();

    if (!pBattleGround || (pBattleGround->isArena() && action))
        return;

    if (pBattleGround->isWarGame())
    {
        if (!IsPlayingNative() && action)
        {
            if (GetTeam() == TEAM_HORDE)
                setFaction(1735);
            else setFaction(1732);
        }
        else
            setFactionForRace(getORace());

        if (action)
            SetForgetBGPlayers(true);
        else
            SetForgetInListPlayers(true);

        if (!IsPlayingNative())
            ForceBGFactions(true);

        return;
    }

    if (!pBattleGround->isRatedBattleground())
    {
        if (!IsPlayingNative() && action)
            setFactionForRace(getRace());
        else
            setFactionForRace(getORace());
    }

    if (action)
        SetForgetBGPlayers(true);
    else
        SetForgetInListPlayers(true);

    if (!pBattleGround->isRatedBattleground())
        MorphFit(action);
    else
        RemoveArenaAuras();

    if (!IsPlayingNative())
        ForceBGFactions(true);

    if (pBattleGround && action)
        SendChatMessage("%sYou are playing for the %s%s in this %s", MSG_COLOR_WHITE, GetTeam() == ALLIANCE ? MSG_COLOR_DARKBLUE"alliance" : MSG_COLOR_RED"horde", MSG_COLOR_WHITE, pBattleGround->GetName());
}

void Player::ForceBGFactions(bool apply)
{
    GetReputationMgr().ApplyForceReaction(1216, (GetTeam() == ALLIANCE ? REP_FRIENDLY : REP_HOSTILE), apply);
    GetReputationMgr().ApplyForceReaction(469, (GetTeam() == ALLIANCE ? REP_FRIENDLY : REP_HOSTILE), apply);
    GetReputationMgr().ApplyForceReaction(1214, (GetTeam() == ALLIANCE ? REP_HOSTILE : REP_FRIENDLY), apply);
    GetReputationMgr().ApplyForceReaction(67, (GetTeam() == ALLIANCE ? REP_HOSTILE : REP_FRIENDLY), apply);
    GetReputationMgr().ApplyForceReaction(1534, (GetTeam() == ALLIANCE ? REP_FRIENDLY : REP_HOSTILE), apply);
}

void Player::DoForgetPlayersInList()
{
    // m_FakePlayers is filled from a vector within the battleground
    // they were in previously so all players that have been in that BG will be invalidated.
    for (FakePlayers::const_iterator itr = m_FakePlayers.begin(); itr != m_FakePlayers.end(); ++itr)
    {
        WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
        data << *itr;
        GetSession()->SendPacket(&data);
        if (auto pPlayer = ObjectAccessor::FindPlayer(*itr))
            GetSession()->SendNameQueryOpcode(pPlayer->GetGUID());
    }
    m_FakePlayers.clear();
}

void Player::DoForgetPlayersInBG(Battleground* pBattleGround)
{
    for (Battleground::BattlegroundPlayerMap::const_iterator itr = pBattleGround->GetPlayers().begin(); itr != pBattleGround->GetPlayers().end(); ++itr)
    {
        // Here we invalidate players in the bg to the added player
        WorldPacket data1(SMSG_INVALIDATE_PLAYER, 8);
        data1 << itr->first;
        GetSession()->SendPacket(&data1);

        if (auto pPlayer = ObjectAccessor::FindPlayer(itr->first))
        {
            GetSession()->SendNameQueryOpcode(pPlayer->GetGUID()); // Send namequery answer instantly if player is available
            // Here we invalidate the player added to players in the bg
            WorldPacket data2(SMSG_INVALIDATE_PLAYER, 8);
            data2 << GetGUID();
            pPlayer->GetSession()->SendPacket(&data2);
            pPlayer->GetSession()->SendNameQueryOpcode(GetGUID());
        }
    }
}

bool BattlegroundQueue::CheckCrossFactionMatch(BattlegroundBracketId bracket_id, Battleground* bg, uint8 minPlayers, uint8 maxPlayers)
{
    if (!sWorld->getBoolConfig(BATTLEGROUND_CROSSFACTION_ENABLED) || bg->isArena())
        return false; // Only do this if crossbg's are enabled.

    // Here we will add all players to selectionpool, later we check if there are enough and launch a bg.
    FillXPlayersToBG(bracket_id, bg, true, minPlayers, maxPlayers);

    if (sBattlegroundMgr->isTesting() && (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() || m_SelectionPools[TEAM_HORDE].GetPlayerCount()))
        return true;

    if (m_SelectionPools[TEAM_ALLIANCE].GetPlayerCount() < minPlayers || m_SelectionPools[TEAM_HORDE].GetPlayerCount() < minPlayers)
        return false;

    return true;
}

// This function will invite players in the least populated faction, which makes battleground queues much faster.
// This function will return true if cross faction battlegrounds are enabled, otherwise return false,
// which is useful in FillPlayersToBG. Because then we can interrupt the regular invitation if cross faction bg's are enabled.
bool BattlegroundQueue::FillXPlayersToBG(BattlegroundBracketId bracket_id, Battleground* bg, bool start, uint8 minPlayers, uint8 maxPlayers)
{
    uint8 queuedPeople = 0;
    for (auto itr = m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].end(); ++itr)
        if (!(*itr)->IsInvitedToBGInstanceGUID)
            queuedPeople += (*itr)->Players.size();

    if (sWorld->getBoolConfig(BATTLEGROUND_CROSSFACTION_ENABLED) && (sBattlegroundMgr->isTesting() || queuedPeople >= (minPlayers * 2) || !start))
    {
        int32 aliFree = start ? maxPlayers : bg->GetFreeSlotsForTeam(ALLIANCE);
        int32 hordeFree = start ? maxPlayers : bg->GetFreeSlotsForTeam(HORDE);
        // Empty selection pools. They will be refilled from queued groups.
        m_SelectionPools[TEAM_ALLIANCE].Init();
        m_SelectionPools[TEAM_HORDE].Init();
        int32 valiFree = aliFree;
        int32 vhordeFree = hordeFree;
        int32 diff = 0;


        // Add teams to their own factions as far as possible.
        if (start)
        {
            QueuedGroupMap m_PreGroupMap_a, m_PreGroupMap_h;
            int32 m_SmallestOfTeams = 0;
            int32 queuedAlliance = 0;
            int32 queuedHorde = 0;


            int32 healersAlliance = 0;
            int32 healersHorde = 0;

            for (auto itr = m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].end(); ++itr)
                if (auto info = (*itr))
                {

                    auto players_in_queue_group = info->Players;
                    for (auto itr = players_in_queue_group.begin(); itr != players_in_queue_group.end(); ++itr)
                        if (auto p_guid = itr->first)
                            if (auto player = sObjectMgr->GetPlayerByLowGUID(p_guid))
                                if (player->GetRole(false) == lfg::PLAYER_ROLE_HEALER)
                                    (info->Team == ALLIANCE ? healersAlliance : healersHorde) += 1;

                }

            for (auto itr = m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].end(); ++itr)
            {
                if ((*itr)->IsInvitedToBGInstanceGUID)
                    continue;

                bool alliance = (*itr)->OTeam == ALLIANCE;

                if (alliance)
                {
                    m_PreGroupMap_a.insert(std::make_pair((*itr)->Players.size(), *itr));
                    queuedAlliance += (*itr)->Players.size();
                }
                else
                {
                    m_PreGroupMap_h.insert(std::make_pair((*itr)->Players.size(), *itr));
                    queuedHorde += (*itr)->Players.size();
                }
            }

            m_SmallestOfTeams = std::min(std::min(aliFree, queuedAlliance), std::min(hordeFree, queuedHorde));

            valiFree -= PreAddPlayers(m_PreGroupMap_a, m_SmallestOfTeams, aliFree);
            vhordeFree -= PreAddPlayers(m_PreGroupMap_h, m_SmallestOfTeams, hordeFree);
        }

        QueuedGroupMap m_QueuedGroupMap;

        for (auto itr = m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_MIXED].end(); ++itr)
            m_QueuedGroupMap.insert(std::make_pair((*itr)->Players.size(), *itr));

        //Count up the queued players in hypothetical teams
        for (auto itr = m_QueuedGroupMap.rbegin(); itr != m_QueuedGroupMap.rend(); ++itr)
        {
            GroupsQueueType allypool = m_SelectionPools[TEAM_ALLIANCE].SelectedGroups;
            GroupsQueueType hordepool = m_SelectionPools[TEAM_HORDE].SelectedGroups;

            GroupQueueInfo* ginfo = itr->second;

            // If player already was invited via pre adding (add to own team first) or he was already invited to a bg, skip.
            if (ginfo->IsInvitedToBGInstanceGUID ||
                std::find(allypool.begin(), allypool.end(), ginfo) != allypool.end() ||
                std::find(hordepool.begin(), hordepool.end(), ginfo) != hordepool.end() ||
                (valiFree == 0 && vhordeFree == 0))
                continue;

            diff = abs(valiFree - vhordeFree);
            bool moreAli = valiFree < vhordeFree;

            if (diff > 0)
                ginfo->Team = moreAli ? HORDE : ALLIANCE;

            //DO NOT TOUCH HERE, THIS IS DECIDED ABOVE
            bool alliance = ginfo->Team == ALLIANCE;
            //DO NOT TOUCH HERE, THIS IS DECIDED ABOVE

            //add a player/group to a team
            if (m_SelectionPools[alliance ? TEAM_ALLIANCE : TEAM_HORDE].AddGroup(ginfo, alliance ? aliFree : hordeFree))
            {
                // Mark the added players spots as taken.
                alliance ? valiFree -= ginfo->Players.size() : vhordeFree -= ginfo->Players.size();
            }
        }

        return true;
    }
    return false;
}

int32 BattlegroundQueue::PreAddPlayers(QueuedGroupMap m_PreGroupMap, int32 MaxAdd, uint32 MaxInTeam)
{
    int32 LeftToAdd = MaxAdd;
    uint32 Added = 0;

    for (auto itr = m_PreGroupMap.rbegin(); itr != m_PreGroupMap.rend(); ++itr)
    {
        int32 PlayerSize = itr->first;
        bool alliance = itr->second->OTeam == ALLIANCE;

        if (PlayerSize <= LeftToAdd && m_SelectionPools[alliance ? TEAM_ALLIANCE : TEAM_HORDE].AddGroup(itr->second, MaxInTeam))
            LeftToAdd -= PlayerSize, Added -= PlayerSize;
    }

    return LeftToAdd;
}

void Player::SendChatMessage(const char *format, ...)
{
    if (!IsInWorld())
        return;

    if (format)
    {
        va_list ap;
        char str [2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);

        ChatHandler(GetSession()).SendSysMessage(str);
    }
}
