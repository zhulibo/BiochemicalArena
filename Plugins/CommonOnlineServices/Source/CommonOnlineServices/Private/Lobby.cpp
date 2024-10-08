#include "Lobby.h"

#include <eos_sdk.h>
#include <eos_lobby.h>

#include "Connect.h"
#include "ServiceManager.h"

EOS_Lobby_AttributeData FLobbiesAttr::ToAttrData()
{
	EOS_Lobby_AttributeData AttrData;

	AttrData.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
	AttrData.Key = Key.c_str();
	AttrData.ValueType = ValueType;

	switch (ValueType)
	{
	case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
		AttrData.Value.AsBool = AsBool;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
		AttrData.Value.AsDouble = AsDouble;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_INT64:
		AttrData.Value.AsInt64 = AsInt64;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_STRING:
		AttrData.Value.AsUtf8 = AsUtf8.c_str();
		break;
	}

	return AttrData;
}

void FLobbiesAttr::InitFromAttrData(EOS_Lobby_Attribute* Attr)
{
	if (!Attr) return;

	Key = Attr->Data->Key;
	ValueType = Attr->Data->ValueType;

	switch (Attr->Data->ValueType)
	{
	case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
		AsBool = Attr->Data->Value.AsBool;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_INT64:
		AsInt64 = Attr->Data->Value.AsInt64;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
		AsDouble = Attr->Data->Value.AsDouble;
		break;
	case EOS_ELobbyAttributeType::EOS_AT_STRING:
		AsUtf8 = Attr->Data->Value.AsUtf8;
		UE_LOG(LogTemp, Warning, TEXT("Key: %s, AsUtf8: %s, ValueType: %d"), *FString(Attr->Data->Key), *FString(Attr->Data->Value.AsUtf8), Attr->Data->ValueType);
		break;
	}
}

ULobby::ULobby()
{
	UE_LOG(LogTemp, Warning, TEXT("ULobby"));
}

bool ULobby::IsOwner(EOS_ProductUserId ProductUserId)
{
	return ProductUserId == UServiceManager::GetLobby()->CurLobby->LobbyOwnerUserId;
}

void ULobby::AddNotifyLobbyUpdates()
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_AddNotifyLobbyUpdateReceivedOptions LobbyUpdateOptions = {};
	LobbyUpdateOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYUPDATERECEIVED_API_LATEST;

	LobbyUpdateReceivedId = EOS_Lobby_AddNotifyLobbyUpdateReceived(LobbyHandle, &LobbyUpdateOptions, nullptr, LobbyUpdateReceivedCb);

	EOS_Lobby_AddNotifyLobbyMemberUpdateReceivedOptions MemberUpdateOptions = {};
	MemberUpdateOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERUPDATERECEIVED_API_LATEST;
	LobbyMemberUpdateReceivedId = EOS_Lobby_AddNotifyLobbyMemberUpdateReceived(LobbyHandle, &MemberUpdateOptions, nullptr, LobbyMemberUpdateReceivedCb);

	EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions MemberStatusOptions = {};
	MemberStatusOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;
	MemberStatusReceivedId = EOS_Lobby_AddNotifyLobbyMemberStatusReceived(LobbyHandle, &MemberStatusOptions, nullptr, MemberStatusReceivedCb);
}

EOS_HLobbyDetails ULobby::GetLobbyDetailsHandle(std::string LobbyId)
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_CopyLobbyDetailsHandleOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_COPYLOBBYDETAILSHANDLE_API_LATEST;
	Options.LobbyId = LobbyId.c_str();
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;

	EOS_HLobbyDetails LobbyDetailsHandle = nullptr;
	EOS_EResult Result = EOS_Lobby_CopyLobbyDetailsHandle(LobbyHandle, &Options, &LobbyDetailsHandle);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Lobby_CopyLobbyDetailsHandle failed with code %d!"), Result);
	}

	return LobbyDetailsHandle;
}

void ULobby::LobbyUpdateReceivedCb(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo* Data)
{
	EOS_HLobbyDetails LobbyDetailsHandle = GetLobbyDetailsHandle(UServiceManager::GetLobby()->CurLobby->LobbyId);

	UServiceManager::GetLobby()->GetLobbyDetails(LobbyDetailsHandle, UServiceManager::GetLobby()->CurLobby);

	UServiceManager::GetLobby()->OnLobbyUpdateReceived.Broadcast();

	EOS_LobbyDetails_Release(LobbyDetailsHandle);
}

void ULobby::LobbyMemberUpdateReceivedCb(const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo* Data)
{
	EOS_HLobbyDetails LobbyDetailsHandle = GetLobbyDetailsHandle(UServiceManager::GetLobby()->CurLobby->LobbyId);

	UServiceManager::GetLobby()->CurLobby->Members = UServiceManager::GetLobby()->GetLobbyMembers(LobbyDetailsHandle);

	UServiceManager::GetLobby()->OnLobbyMemberUpdateReceived.Broadcast();

	EOS_LobbyDetails_Release(LobbyDetailsHandle);
}

void ULobby::MemberStatusReceivedCb(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data)
{
	EOS_HLobbyDetails LobbyDetailsHandle = GetLobbyDetailsHandle(UServiceManager::GetLobby()->CurLobby->LobbyId);

	UServiceManager::GetLobby()->CurLobby->Members = UServiceManager::GetLobby()->GetLobbyMembers(LobbyDetailsHandle);

	UServiceManager::GetLobby()->OnMemberStatusReceived.Broadcast(Data->TargetUserId, Data->CurrentStatus);

	EOS_LobbyDetails_Release(LobbyDetailsHandle);
}

void ULobby::CreateLobby()
{
	EOS_Lobby_CreateLobbyOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_CREATELOBBY_API_LATEST;
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.MaxLobbyMembers = 18;
	Options.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED;
	Options.bPresenceEnabled = true;
	Options.bAllowInvites = true;
	Options.BucketId = "Cool BucketId";
	Options.bDisableHostMigration = true;
	Options.bEnableRTCRoom = false;
	Options.LocalRTCOptions = nullptr;
	Options.LobbyId = nullptr;
	Options.bEnableJoinById = true;
	Options.bRejoinAfterKickRequiresInvite = true;

	TArray<int32> AllowedPlatformIds;
	AllowedPlatformIds.Add(EOS_OPT_Unknown);
	uint32_t* TempVar = new uint32_t[AllowedPlatformIds.Num()];
	for (int32 i = 0; i < AllowedPlatformIds.Num(); i++)
	{
		TempVar[i] = AllowedPlatformIds[i];
	}

	Options.AllowedPlatformIds = TempVar;
	Options.AllowedPlatformIdsCount = AllowedPlatformIds.Num();
	Options.bCrossplayOptOut = false;

	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_CreateLobby(LobbyHandle, &Options, nullptr, CreateLobbyCb);

	UServiceManager::GetLobby()->CurLobby->LobbyOwnerUserId = UServiceManager::GetConnect()->ProductUserId;
}

void ULobby::CreateLobbyCb(const EOS_Lobby_CreateLobbyCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->CurLobby->LobbyId = Data->LobbyId;

		UServiceManager::GetLobby()->OnCreateLobbyComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnCreateLobbyComplete.Broadcast(false);
	}
}

void ULobby::UpdateLobby(EOS_Lobby_AttributeData& Attr)
{
	EOS_Lobby_UpdateLobbyModificationOptions ModifyOptions = {};
	ModifyOptions.ApiVersion = EOS_LOBBY_UPDATELOBBYMODIFICATION_API_LATEST;
	ModifyOptions.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	ModifyOptions.LobbyId = UServiceManager::GetLobby()->CurLobby->LobbyId.c_str();

	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_HLobbyModification ModificationHandle = nullptr;
	EOS_EResult Result = EOS_Lobby_UpdateLobbyModification(LobbyHandle, &ModifyOptions, &ModificationHandle);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Lobby_UpdateLobbyModification failed with code %d!"), Result);
		return;
	}

	EOS_LobbyModification_AddAttributeOptions AttrOptions = {};
	AttrOptions.ApiVersion = EOS_LOBBYMODIFICATION_ADDATTRIBUTE_API_LATEST;
	AttrOptions.Attribute = &Attr;
	AttrOptions.Visibility = EOS_ELobbyAttributeVisibility::EOS_LAT_PUBLIC;

	Result = EOS_LobbyModification_AddAttribute(ModificationHandle, &AttrOptions);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_LobbyModification_AddAttribute failed with code %d!"), Result);
		return;
	}

	EOS_Lobby_UpdateLobbyOptions UpdateOptions = {};
	UpdateOptions.ApiVersion = EOS_LOBBY_UPDATELOBBY_API_LATEST;
	UpdateOptions.LobbyModificationHandle = ModificationHandle;

	EOS_Lobby_UpdateLobby(LobbyHandle, &UpdateOptions, nullptr, UpdateLobbyCb);
}

void ULobby::UpdateLobbyCb(const EOS_Lobby_UpdateLobbyCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->OnUpdateLobbyComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnUpdateLobbyComplete.Broadcast(true);
	}
}

void ULobby::FindLobbies(TArray<FLobbiesAttr> Attrs)
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_CreateLobbySearchOptions SearchOptions = {};
	SearchOptions.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
	SearchOptions.MaxResults = 100;

	EOS_EResult Result = EOS_Lobby_CreateLobbySearch(LobbyHandle, &SearchOptions, &LobbySearchHandle);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Lobby_CreateLobbySearch failed with code %d!"), Result);
		return;
	}

	EOS_LobbySearch_SetParameterOptions ParamOptions = {};
	ParamOptions.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;

	EOS_Lobby_AttributeData AttrData = {};
	AttrData.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;

	for (auto Attr : Attrs)
	{
		AttrData.Key = Attr.Key.c_str();
		AttrData.ValueType = Attr.ValueType;

		switch (Attr.ValueType)
		{
		case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
			AttrData.Value.AsBool = Attr.AsBool;
			UE_LOG(LogTemp, Warning, TEXT("Key: %s, AsBool: %d, ValueType: %d"), *FString(AttrData.Key), AttrData.Value.AsBool, AttrData.ValueType);
			break;
		case EOS_ELobbyAttributeType::EOS_AT_INT64:
			AttrData.Value.AsInt64 = Attr.AsInt64;
			UE_LOG(LogTemp, Warning, TEXT("Key: %s, AsInt64: %lld, ValueType: %d"), *FString(AttrData.Key), AttrData.Value.AsInt64, AttrData.ValueType);
			break;
		case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
			AttrData.Value.AsDouble = Attr.AsDouble;
			UE_LOG(LogTemp, Warning, TEXT("Key: %s, AsDouble: %f, ValueType: %d"), *FString(AttrData.Key), AttrData.Value.AsDouble, AttrData.ValueType);
			break;
		case EOS_ELobbyAttributeType::EOS_AT_STRING:
			AttrData.Value.AsUtf8 = Attr.AsUtf8.c_str();
			UE_LOG(LogTemp, Warning, TEXT("Key: %s, AsUtf8: %s, ValueType: %d"), *FString(AttrData.Key), *FString(AttrData.Value.AsUtf8), AttrData.ValueType);
			break;
		}

		ParamOptions.Parameter = &AttrData;
		ParamOptions.ComparisonOp = Attr.ComparisonOp;

		Result = EOS_LobbySearch_SetParameter(LobbySearchHandle, &ParamOptions);

		if (Result != EOS_EResult::EOS_Success)
		{
			UE_LOG(LogTemp, Error, TEXT("EOS_LobbySearch_SetParameter failed with code %d!"), Result);
			return;
		}
	}

	EOS_LobbySearch_FindOptions FindOptions = {};
	FindOptions.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
	FindOptions.LocalUserId = UServiceManager::GetConnect()->ProductUserId;

	EOS_LobbySearch_Find(LobbySearchHandle, &FindOptions, nullptr, FindLobbiesCb);
}

void ULobby::FindLobbiesCb(const EOS_LobbySearch_FindCallbackInfo* Data)
{
	for (auto LobbyDetailsHandle : UServiceManager::GetLobby()->LobbyDetailsHandles)
	{
		EOS_LobbyDetails_Release(LobbyDetailsHandle);
	}
	UServiceManager::GetLobby()->LobbyDetailsHandles.Empty();

	TArray<TSharedPtr<FCoolLobby>> FoundLobbies;

	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		EOS_LobbySearch_GetSearchResultCountOptions LobbyCountOptions = {};
		LobbyCountOptions.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
		uint32_t LobbiesNum = EOS_LobbySearch_GetSearchResultCount(UServiceManager::GetLobby()->LobbySearchHandle, &LobbyCountOptions);

		EOS_LobbySearch_CopySearchResultByIndexOptions LobbyIndexOptions = {};
		LobbyIndexOptions.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;

		for (uint32_t Index = 0; Index < LobbiesNum; ++Index)
		{
			TSharedPtr<FCoolLobby> FoundLobby = MakeShared<FCoolLobby>();

			LobbyIndexOptions.LobbyIndex = Index;

			EOS_HLobbyDetails LobbyDetailsHandle = nullptr;
			EOS_EResult Result = EOS_LobbySearch_CopySearchResultByIndex(UServiceManager::GetLobby()->LobbySearchHandle, &LobbyIndexOptions, &LobbyDetailsHandle);

			if (Result == EOS_EResult::EOS_Success && LobbyDetailsHandle)
			{
				UServiceManager::GetLobby()->GetLobbyDetails(LobbyDetailsHandle, FoundLobby);

				FoundLobby->Attrs = UServiceManager::GetLobby()->GetLobbyAttrs(LobbyDetailsHandle);
			}

			FoundLobbies.Add(FoundLobby);

			UServiceManager::GetLobby()->LobbyDetailsHandles.Add(LobbyDetailsHandle);
		}

		UServiceManager::GetLobby()->OnFindLobbiesComplete.Broadcast(true, FoundLobbies);
	}
	else
	{
		UServiceManager::GetLobby()->OnFindLobbiesComplete.Broadcast(false, FoundLobbies);
	}

	EOS_LobbySearch_Release(UServiceManager::GetLobby()->LobbySearchHandle);
}

void ULobby::JoinLobby(TSharedPtr<FCoolLobby> Lobby, EOS_HLobbyDetails LobbyDetailsHandle)
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_JoinLobbyOptions JoinOptions = {};
	JoinOptions.ApiVersion = EOS_LOBBY_JOINLOBBY_API_LATEST;
	JoinOptions.LobbyDetailsHandle = LobbyDetailsHandle;
	JoinOptions.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	JoinOptions.bPresenceEnabled = true;
	JoinOptions.LocalRTCOptions = nullptr;
	JoinOptions.bCrossplayOptOut = false;

	EOS_Lobby_JoinLobby(LobbyHandle, &JoinOptions, nullptr, JoinLobbyCb);

	EOS_LobbyDetails_Release(LobbyDetailsHandle);

	UServiceManager::GetLobby()->CurLobby = Lobby;
}

void ULobby::JoinLobbyCb(const EOS_Lobby_JoinLobbyCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		EOS_HLobbyDetails LobbyDetailsHandle = GetLobbyDetailsHandle(UServiceManager::GetLobby()->CurLobby->LobbyId);
		UServiceManager::GetLobby()->GetLobbyDetails(LobbyDetailsHandle, UServiceManager::GetLobby()->CurLobby);

		UServiceManager::GetLobby()->CurLobby->Attrs = UServiceManager::GetLobby()->GetLobbyAttrs(LobbyDetailsHandle);

		UServiceManager::GetLobby()->CurLobby->Members = UServiceManager::GetLobby()->GetLobbyMembers(LobbyDetailsHandle);

		UServiceManager::GetLobby()->OnJoinLobbyComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnJoinLobbyComplete.Broadcast(false);
	}
}

void ULobby::GetLobbyDetails(EOS_HLobbyDetails LobbyDetailsHandle, TSharedPtr<FCoolLobby> OutLobby)
{
	if (OutLobby == nullptr) return;

	EOS_LobbyDetails_Info* Details = nullptr;

	EOS_LobbyDetails_CopyInfoOptions InfoOptions = {};
	InfoOptions.ApiVersion = EOS_LOBBYDETAILS_COPYINFO_API_LATEST;

	EOS_EResult Result = EOS_LobbyDetails_CopyInfo(LobbyDetailsHandle, &InfoOptions, &Details);

	if (Result == EOS_EResult::EOS_Success && Details)
	{
		OutLobby->LobbyId = Details->LobbyId;
		OutLobby->LobbyOwnerUserId = Details->LobbyOwnerUserId;
		OutLobby->AvailableSlots = Details->AvailableSlots;
		OutLobby->MaxMembers = Details->MaxMembers;
	}

	EOS_LobbyDetails_Info_Release(Details);
}

TArray<FLobbiesAttr> ULobby::GetLobbyAttrs(EOS_HLobbyDetails LobbyDetailsHandle)
{
	TArray<FLobbiesAttr> Attrs;

	EOS_LobbyDetails_GetAttributeCountOptions AttrCountOptions = {};
	AttrCountOptions.ApiVersion = EOS_LOBBYDETAILS_GETATTRIBUTECOUNT_API_LATEST;

	uint32_t AttrCount = EOS_LobbyDetails_GetAttributeCount(LobbyDetailsHandle, &AttrCountOptions);

	for (uint32_t AttrIndex = 0; AttrIndex < AttrCount; ++AttrIndex)
	{
		EOS_LobbyDetails_CopyAttributeByIndexOptions AttrOptions = {};
		AttrOptions.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYINDEX_API_LATEST;
		AttrOptions.AttrIndex = AttrIndex;

		EOS_Lobby_Attribute* OutAttr = nullptr;
		EOS_EResult Result = EOS_LobbyDetails_CopyAttributeByIndex(LobbyDetailsHandle, &AttrOptions, &OutAttr);

		if (Result != EOS_EResult::EOS_Success)
		{
			UE_LOG(LogTemp, Error, TEXT("EOS_LobbyDetails_CopyAttributeByIndex failed with code %d!"), Result);
			continue;
		}

		FLobbiesAttr Attr = {};
		Attr.InitFromAttrData(OutAttr);

		Attrs.Add(Attr);

		EOS_Lobby_Attribute_Release(OutAttr);
	}

	return Attrs;
}

TArray<FCoolLobbyMember> ULobby::GetLobbyMembers(EOS_HLobbyDetails LobbyDetailsHandle)
{
	TArray<FCoolLobbyMember> Members;

	EOS_LobbyDetails_GetMemberCountOptions MemberCountOptions = {};
	MemberCountOptions.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERCOUNT_API_LATEST;
	uint32_t MemberCount = EOS_LobbyDetails_GetMemberCount(LobbyDetailsHandle, &MemberCountOptions);

	for (uint32_t MemberIndex = 0; MemberIndex < MemberCount; ++MemberIndex)
	{
		FCoolLobbyMember Member = {};

		EOS_LobbyDetails_GetMemberByIndexOptions MemberOptions = {};
		MemberOptions.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERBYINDEX_API_LATEST;
		MemberOptions.MemberIndex = MemberIndex;
		EOS_ProductUserId ProductUserId = EOS_LobbyDetails_GetMemberByIndex(LobbyDetailsHandle, &MemberOptions);

		Member.ProductUserId = ProductUserId;

		EOS_LobbyDetails_GetMemberAttributeCountOptions MemberAttrCountOptions = {};
		MemberAttrCountOptions.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERATTRIBUTECOUNT_API_LATEST;
		MemberAttrCountOptions.TargetUserId = ProductUserId;
		uint32_t MemberAttrCount = EOS_LobbyDetails_GetMemberAttributeCount(LobbyDetailsHandle, &MemberAttrCountOptions);

		for (uint32_t MemberAttrIndex = 0; MemberAttrIndex < MemberAttrCount; ++MemberAttrIndex)
		{
			EOS_LobbyDetails_CopyMemberAttributeByIndexOptions MemberAttrCopyOptions = {};
			MemberAttrCopyOptions.ApiVersion = EOS_LOBBYDETAILS_COPYMEMBERATTRIBUTEBYINDEX_API_LATEST;
			MemberAttrCopyOptions.AttrIndex = MemberAttrIndex;
			MemberAttrCopyOptions.TargetUserId = ProductUserId;

			EOS_Lobby_Attribute* OutAttr = nullptr;
			EOS_EResult Result = EOS_LobbyDetails_CopyMemberAttributeByIndex(LobbyDetailsHandle, &MemberAttrCopyOptions, &OutAttr);

			if (Result != EOS_EResult::EOS_Success)
			{
				UE_LOG(LogTemp, Error, TEXT("EOS_LobbyDetails_CopyMemberAttributeByIndex failed with code %d!"), Result);
				continue;
			}

			FLobbiesAttr Attr = {};
			Attr.InitFromAttrData(OutAttr);

			Member.Attrs.Add(Attr);

			EOS_Lobby_Attribute_Release(OutAttr);
		}

		Members.Add(Member);
	}

	return Members;
}

void ULobby::LeaveLobby()
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_LeaveLobbyOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_LEAVELOBBY_API_LATEST;
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.LobbyId = UServiceManager::GetLobby()->CurLobby->LobbyId.c_str();

	EOS_Lobby_LeaveLobby(LobbyHandle, &Options, nullptr, LeaveLobbyCb);
}

void ULobby::LeaveLobbyCb(const EOS_Lobby_LeaveLobbyCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->OnLeaveLobbyComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnLeaveLobbyComplete.Broadcast(false);
	}
}

void ULobby::DestroyLobby()
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_DestroyLobbyOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_LEAVELOBBY_API_LATEST;
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.LobbyId = UServiceManager::GetLobby()->CurLobby->LobbyId.c_str();

	EOS_Lobby_DestroyLobby(LobbyHandle, &Options, nullptr, DestroyLobbyCb);
}

void ULobby::DestroyLobbyCb(const EOS_Lobby_DestroyLobbyCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->OnDestroyLobbyComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnDestroyLobbyComplete.Broadcast(false);
	}
}

void ULobby::PromoteMember(EOS_ProductUserId ProductUserId)
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_PromoteMemberOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_PROMOTEMEMBER_API_LATEST;
	Options.LobbyId = UServiceManager::GetLobby()->CurLobby->LobbyId.c_str();
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.TargetUserId = ProductUserId;

	EOS_Lobby_PromoteMember(LobbyHandle, &Options, nullptr, PromoteMemberCb);
}

void ULobby::PromoteMemberCb(const EOS_Lobby_PromoteMemberCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->OnPromoteMemberComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnPromoteMemberComplete.Broadcast(false);
	}
}

void ULobby::KickMember(EOS_ProductUserId ProductUserId)
{
	EOS_HLobby LobbyHandle = EOS_Platform_GetLobbyInterface(UServiceManager::GetPlatformHandle());

	EOS_Lobby_KickMemberOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_KICKMEMBER_API_LATEST;
	Options.LobbyId = UServiceManager::GetLobby()->CurLobby->LobbyId.c_str();
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.TargetUserId = ProductUserId;

	EOS_Lobby_KickMember(LobbyHandle, &Options, nullptr, KickMemberCb);
}

void ULobby::KickMemberCb(const EOS_Lobby_KickMemberCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetLobby()->OnKickMemberComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetLobby()->OnKickMemberComplete.Broadcast(false);
	}
}
