#include "Session.h"

#include <eos_sdk.h>
#include <eos_sessions.h>
#include <eos_sessions_types.h>

#include "Connect.h"
#include "ServiceManager.h"

USession::USession()
{
}

void USession::CreateSession()
{
	EOS_HSessions SessionsHandle = EOS_Platform_GetSessionsInterface(UServiceManager::GetPlatformHandle());

	EOS_Sessions_CreateSessionModificationOptions Options = {};
	Options.ApiVersion = EOS_SESSIONS_CREATESESSIONMODIFICATION_API_LATEST;
	Options.SessionName = "SessionName";
	Options.BucketId = "BucketId";
	Options.MaxPlayers = 18;
	Options.LocalUserId = UServiceManager::GetConnect()->ProductUserId;
	Options.bPresenceEnabled = true;
	Options.bSanctionsEnabled = false;

	TArray<int32> AllowedPlatformIds;
	AllowedPlatformIds.Add(EOS_OPT_Unknown);
	uint32_t* Ids = new uint32_t[AllowedPlatformIds.Num()];
	for (int32 i = 0; i < AllowedPlatformIds.Num(); i++)
	{
		Ids[i] = AllowedPlatformIds[i];
	}

	Options.AllowedPlatformIds = Ids;
	Options.AllowedPlatformIdsCount = AllowedPlatformIds.Num();

	EOS_HSessionModification ModificationHandle = nullptr;
	EOS_EResult Result = EOS_Sessions_CreateSessionModification(SessionsHandle, &Options, &ModificationHandle);
	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Sessions_CreateSessionModification failed with code %d!"), Result);
		return;
	}

	EOS_SessionModification_SetPermissionLevelOptions PermOptions = {};
	PermOptions.ApiVersion = EOS_SESSIONMODIFICATION_SETPERMISSIONLEVEL_API_LATEST;
	PermOptions.PermissionLevel = EOS_EOnlineSessionPermissionLevel::EOS_OSPF_PublicAdvertised;
	const EOS_EResult PermResult = EOS_SessionModification_SetPermissionLevel(ModificationHandle, &PermOptions);
	if (PermResult != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_SessionModification_SetPermissionLevel failed with code %d!"), Result);
		EOS_SessionModification_Release(ModificationHandle);
		return;
	}

	EOS_SessionModification_SetJoinInProgressAllowedOptions JoinOptions = {};
	JoinOptions.ApiVersion = EOS_SESSIONMODIFICATION_SETJOININPROGRESSALLOWED_API_LATEST;
	JoinOptions.bAllowJoinInProgress = true;
	const EOS_EResult JoinResult = EOS_SessionModification_SetJoinInProgressAllowed(ModificationHandle, &JoinOptions);
	if (JoinResult != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_SessionModification_SetJoinInProgressAllowed failed with code %d!"), Result);
		EOS_SessionModification_Release(ModificationHandle);
		return;
	}

	EOS_Sessions_UpdateSessionOptions UpdateOptions = {};
	UpdateOptions.ApiVersion = EOS_SESSIONS_UPDATESESSION_API_LATEST;
	UpdateOptions.SessionModificationHandle = ModificationHandle;
	EOS_Sessions_UpdateSession(SessionsHandle, &UpdateOptions, nullptr, CreateSessionCb);

	EOS_SessionModification_Release(ModificationHandle);
}

void USession::CreateSessionCb(const EOS_Sessions_UpdateSessionCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetSession()->OnCreateSessionComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetSession()->OnCreateSessionComplete.Broadcast(true);
	}
}

void USession::DestroySession()
{
	EOS_HSessions SessionsHandle = EOS_Platform_GetSessionsInterface(UServiceManager::GetPlatformHandle());

	EOS_Sessions_DestroySessionOptions Options = {};
	Options.ApiVersion = EOS_SESSIONS_DESTROYSESSION_API_LATEST;
	Options.SessionName = "SessionName";

	EOS_Sessions_DestroySession(SessionsHandle, &Options, nullptr, DestroySessionCb);
}

void USession::DestroySessionCb(const EOS_Sessions_DestroySessionCallbackInfo* Data)
{
	if (Data && Data->ResultCode == EOS_EResult::EOS_Success)
	{
		UServiceManager::GetSession()->OnDestroySessionComplete.Broadcast(true);
	}
	else
	{
		UServiceManager::GetSession()->OnDestroySessionComplete.Broadcast(true);
	}
}

void USession::GetSessionDetail()
{
	EOS_HSessions SessionsHandle = EOS_Platform_GetSessionsInterface(UServiceManager::GetPlatformHandle());

	EOS_Sessions_CopyActiveSessionHandleOptions Options = {};
	Options.ApiVersion = EOS_SESSIONS_COPYACTIVESESSIONHANDLE_API_LATEST;
	Options.SessionName = "SessionName";

	EOS_HActiveSession ActiveSessionHandle;
	EOS_EResult Result = EOS_Sessions_CopyActiveSessionHandle(SessionsHandle, &Options, &ActiveSessionHandle);

	if (Result != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Sessions_CopyActiveSessionHandle failed with code %d!"), Result);
		EOS_ActiveSession_Release(ActiveSessionHandle);
		return;
	}

	EOS_ActiveSession_CopyInfoOptions CopyInfoOptions = {};
	CopyInfoOptions.ApiVersion = EOS_ACTIVESESSION_COPYINFO_API_LATEST;
	EOS_ActiveSession_Info* ActiveSessionInfo = nullptr;
	EOS_EResult CopyInfoResult = EOS_ActiveSession_CopyInfo(ActiveSessionHandle, &CopyInfoOptions, &ActiveSessionInfo);

	if (CopyInfoResult != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_ActiveSession_CopyInfo failed with code %d!"), CopyInfoResult);
		EOS_ActiveSession_Info_Release(ActiveSessionInfo);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("HostAddress: %s"), *FString(ActiveSessionInfo->SessionDetails->HostAddress));

	EOS_ActiveSession_Release(ActiveSessionHandle);
	EOS_ActiveSession_Info_Release(ActiveSessionInfo);
}
