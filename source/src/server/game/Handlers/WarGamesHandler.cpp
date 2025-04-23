#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Object.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "DisableMgr.h"
#include "Chat.h"
#include "Group.h"
#include "BattlegroundMgr.h"

void WorldSession::HandleWarGameCommentatorStart(WorldPacket& recvData)
{

}

void WorldSession::HandleWarGameAccept(WorldPacket& recvData)
{
    ExplodedGuid exGuid(2, "23132712102115261611201714252422", "22262410251116131512232117202714", &recvData);
    bool accept = recvData.ReadBit();
    exGuid.ReadMaskCount(16);
    exGuid.ConditionalRead(16);

    ObjectGuid guid1 = exGuid.GetObjectGuid(1);
    ObjectGuid guid2 = exGuid.GetObjectGuid(0);

    uint32 bgTypeId_;
    //extract from guid
    bgTypeId_ = GUID_LOPART(guid1) & 0x00000FFF;

    if (!accept)
        return;

    if (!sBattlemasterListStore.LookupEntry(bgTypeId_))
    {
        TC_LOG_ERROR("network.opcode", "Battleground: invalid bgtype (%u) received. possible cheater? player guid %u", bgTypeId_, _player->GetGUIDLow());
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeId_, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_BG_DISABLED);
        return;
    }
    BattlegroundTypeId bgTypeId = BattlegroundTypeId(bgTypeId_);

    Player* challenger = ObjectAccessor::FindPlayer(guid2);

    if (!challenger)
        return;

    if (auto challenged = challenger->wargameRequest->opponent)
    {
        if (challenged != GetPlayer())
        {
            TC_LOG_ERROR("network.opcode", "HandleWarGameAccept: Challenger initator doesn't match. possible cheater? player guid %u", _player->GetGUIDLow());
            return;
        }

        if ((time(NULL) - challenger->wargameRequest->startTime) > 93)
            return;
    }
    else
    {
        TC_LOG_ERROR("network.opcode", "HandleWarGameAccept: Challenger initator doesn't match. possible cheater? player guid %u", _player->GetGUIDLow());
        return;
    }

    Battleground* bg_template = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg_template)
        return;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg_template->GetMapId(), GetPlayer()->getLevel());
    if (!bracketEntry)
        return;

    Group* myGroup = GetPlayer()->GetGroup();
    Group* oppGroup = challenger->GetGroup();

    if (myGroup->GetMembersCount() != oppGroup->GetMembersCount())
        return;

    Group* loopGroup;
    for (uint8 i = 0; i < 2; ++i)
    {
        loopGroup = i == 0 ? myGroup : oppGroup;
        for (auto itr = loopGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member)
                return;

            PvPDifficultyEntry const* MemberbracketEntry = GetBattlegroundBracketByLevel(bg_template->GetMapId(), member->getLevel());

            if (MemberbracketEntry->GetBracketId() != bracketEntry->GetBracketId())
                return;
            if (member->InBattlegroundQueue())
                return;
            if (member->isUsingLfg())
                return;
            if (sSoloQueueMgr->IsPlayerInSoloQueue(member))
                return;
        }
    }

    uint8 ArenaType = 0;
    if (bg_template->isArena())
    {
        if (myGroup->GetMembersCount() == 2)
            ArenaType = 2;
        else
        {
            if (myGroup->GetMembersCount() == 3)
                ArenaType = 3;
            else
            {
                if (myGroup->GetMembersCount() == 5)
                    ArenaType = 5;
                else return;
            }
        }
    }

    BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, ArenaType);

    for (uint8 i = 0; i < 2; ++i)
    {
        loopGroup = i == 0 ? myGroup : oppGroup;
        for (auto itr = loopGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member)
                return;
   
            uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);
            member->AddBattlegroundQueueJoinTime(bgTypeId, getMSTime());
            WorldPacket data;
            sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg_template, member, queueSlot, STATUS_WAIT_QUEUE, 0, time(NULL), 0);
            member->GetSession()->SendPacket(&data);

            sBattlegroundMgr->ScheduleQueueUpdate(0, 6, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
        }
    }

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
    bgQueue.AddWarGameGroup(myGroup, oppGroup, bgTypeId, bracketEntry->GetBracketId(), ArenaType);
}

void WorldSession::HandleWarGameStart(WorldPacket& recvData)
{
    ObjectGuid BattlegroundGUID, TargetPlayerGUID;

    TargetPlayerGUID[0] = recvData.ReadBit();
    TargetPlayerGUID[7] = recvData.ReadBit();
    BattlegroundGUID[3] = recvData.ReadBit();
    BattlegroundGUID[7] = recvData.ReadBit();
    BattlegroundGUID[1] = recvData.ReadBit();
    TargetPlayerGUID[5] = recvData.ReadBit();
    TargetPlayerGUID[1] = recvData.ReadBit();
    TargetPlayerGUID[2] = recvData.ReadBit();
    BattlegroundGUID[6] = recvData.ReadBit();
    BattlegroundGUID[5] = recvData.ReadBit();
    BattlegroundGUID[2] = recvData.ReadBit();
    BattlegroundGUID[0] = recvData.ReadBit();
    BattlegroundGUID[4] = recvData.ReadBit();
    TargetPlayerGUID[4] = recvData.ReadBit();
    TargetPlayerGUID[3] = recvData.ReadBit();
    TargetPlayerGUID[6] = recvData.ReadBit();

    recvData.ReadByteSeq(TargetPlayerGUID[6]);
    recvData.ReadByteSeq(BattlegroundGUID[7]);
    recvData.ReadByteSeq(BattlegroundGUID[3]);
    recvData.ReadByteSeq(TargetPlayerGUID[4]);
    recvData.ReadByteSeq(BattlegroundGUID[5]);
    recvData.ReadByteSeq(BattlegroundGUID[2]);
    recvData.ReadByteSeq(TargetPlayerGUID[1]);
    recvData.ReadByteSeq(TargetPlayerGUID[3]);
    recvData.ReadByteSeq(TargetPlayerGUID[5]);
    recvData.ReadByteSeq(BattlegroundGUID[0]);
    recvData.ReadByteSeq(TargetPlayerGUID[2]);
    recvData.ReadByteSeq(TargetPlayerGUID[7]);
    recvData.ReadByteSeq(BattlegroundGUID[6]);
    recvData.ReadByteSeq(TargetPlayerGUID[0]);
    recvData.ReadByteSeq(BattlegroundGUID[1]);
    recvData.ReadByteSeq(BattlegroundGUID[4]);

    uint32 bgTypeId_;
    //extract from guid
    bgTypeId_ = GUID_LOPART(BattlegroundGUID) & 0x00000FFF;

    if (!sBattlemasterListStore.LookupEntry(bgTypeId_))
    {
        TC_LOG_ERROR("network.opcode", "Battleground: invalid bgtype (%u) received. possible cheater? player guid %u", bgTypeId_, _player->GetGUIDLow());
        return;
    }

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeId_, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_BG_DISABLED);
        return;
    }
    BattlegroundTypeId bgTypeId = BattlegroundTypeId(bgTypeId_);

    if (bgTypeId == BATTLEGROUND_RV) // Ring of Valor disabled
        return;

    Player* TargetPlayer = ObjectAccessor::FindPlayer(TargetPlayerGUID);

    if (!TargetPlayer)
        return;

    Group* myGroup = GetPlayer()->GetGroup();
    Group* oppGroup = TargetPlayer->GetGroup();

    if (!myGroup || !oppGroup)
        return;
    if (myGroup->GetLeaderGUID() != GetPlayer()->GetGUID() || oppGroup->GetLeaderGUID() != TargetPlayer->GetGUID())
        return;
    if (myGroup->isLFGGroup() || myGroup->isLFGGroup())
        return;

    Battleground* bg_template = sBattlegroundMgr->GetBattlegroundTemplate(bgTypeId);
    if (!bg_template)
        return;

    uint32 minplayers = 0;
    if (bg_template->isArena())
        minplayers = 2;
    else minplayers = 5;
    uint32 maxplayers = bg_template->GetMaxPlayersPerTeam();

    if (myGroup->GetMembersCount() > maxplayers || myGroup->GetMembersCount() < minplayers
        || oppGroup->GetMembersCount() > maxplayers || oppGroup->GetMembersCount() < minplayers)
        return;

    WarGameRequestInfo* _wargameRequest = new WarGameRequestInfo;
    _wargameRequest->opponent = TargetPlayer;
    _wargameRequest->startTime = time(NULL);
    GetPlayer()->wargameRequest = _wargameRequest;

    WorldPacket CheckEntry(SMSG_WARGAME_CHECK_ENTRY, 16);
    ExplodedGuid requestGuid(2, "21221714241125152716132023122610", "12202224261025271315211714111623", &CheckEntry);
    requestGuid.AddGuid(BattlegroundGUID, 0);
    requestGuid.AddGuid(GetPlayer()->GetGUID(), 1);
    requestGuid.WriteMaskCount(16);
    requestGuid.ConditionalWrite(1);
    CheckEntry << uint32(90 + 3);
    requestGuid.ConditionalWrite(15);

    TargetPlayer->GetSession()->SendPacket(&CheckEntry);

    WorldPacket response(SMSG_WARGAME_REQUEST_SENT);
    uint8 byteMask[] = { 6, 0, 3, 5, 2, 1, 4, 7 };
    uint8 byteBytes[] = { 5, 6, 3, 0, 7, 4, 2, 1 };
    response.WriteGuidMask(BattlegroundGUID, byteMask, 8);
    response.WriteGuidBytes(BattlegroundGUID, byteBytes, 8, 0);
    GetPlayer()->GetSession()->SendPacket(&response);
}
