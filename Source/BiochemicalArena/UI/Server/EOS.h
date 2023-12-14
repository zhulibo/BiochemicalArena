#pragma once

#include "CoreMinimal.h"
#include "Online/Auth.h"
#include "Online/UserInfo.h"
#include "Online/Lobbies.h"
#include "Online/Sessions.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Online/OnlineServices.h"
#include "EOS.generated.h"

using namespace UE::Online;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindLobbyComplete, bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyAttributesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyMemberAttributesComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginStatusChanged, const FAuthLoginStatusChanged& AuthLoginStatusChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberJoined, const FLobbyMemberJoined& LobbyMemberJoined);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberLeft, const FLobbyMemberLeft& LobbyMemberLeft);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeft, const FLobbyLeft& LobbyLeft);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyAttributesChanged, const FLobbyAttributesChanged& LobbyAttributesChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberAttributesChanged, const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyInvitationAdded, const FLobbyInvitationAdded& LobbyInvitationAdded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUILobbyJoinRequested, const FUILobbyJoinRequested& UILobbyJoinRequested);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeaderChanged, const FLobbyLeaderChanged& LobbyLeaderChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveSessionComplete, bool bWasSuccessful);

UCLASS()
class BIOCHEMICALARENA_API UEOS : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UEOS();

	// 委托
	FOnLoginComplete OnLoginComplete;
	FOnCreateLobbyComplete OnCreateLobbyComplete;
	FOnFindLobbyComplete OnFindLobbyComplete;
	FOnJoinLobbyComplete OnJoinLobbyComplete;
	FOnLeaveLobbyComplete OnLeaveLobbyComplete;
	FOnModifyLobbyAttributesComplete OnModifyLobbyAttributesComplete;
	FOnModifyLobbyMemberAttributesComplete OnModifyLobbyMemberAttributesComplete;

	FOnLoginStatusChanged OnLoginStatusChanged;
	FOnLobbyMemberJoined OnLobbyMemberJoined;
	FOnLobbyMemberLeft OnLobbyMemberLeft;
	FOnLobbyLeft OnLobbyLeft;
	FOnLobbyAttributesChanged OnLobbyAttributesChanged;
	FOnLobbyMemberAttributesChanged OnLobbyMemberAttributesChanged;
	FOnLobbyInvitationAdded OnLobbyInvitationAdded;
	FOnUILobbyJoinRequested OnUILobbyJoinRequested;
	FOnLobbyLeaderChanged OnLobbyLeaderChanged;

	FOnCreateSessionComplete OnCreateSessionComplete;
	FOnJoinSessionComplete OnJoinSessionComplete;
	FOnLeaveSessionComplete OnLeaveSessionComplete;

	void Login(FPlatformUserId ID, int Type = 0);
	void CreateLobby();
	void FindLobby();
	void JoinLobby(TSharedRef<const FLobby> Lobby);
	void LeaveLobby();
	void PromoteLobbyMember();
	void ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes);
	void ModifyLobbyMemberAttributes();

	void BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);
	void BroadcastOnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);
	void BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);
	void BroadcastOnLobbyLeft(const FLobbyLeft& LobbyLeft);
	void BroadcastOnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);
	void BroadcastOnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);
	void BroadcastOnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	void BroadcastOnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);
	void BroadcastOnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

	TSharedPtr<const FLobby> CurrentLobby;

	void CreateSession();
	TSharedPtr<const ISession> GetPresenceSession();
	void JoinSession(FOnlineSessionId SessionId);
	void LeaveSession();

private:
	FPlatformUserId PlatformUserId;

	IOnlineServicesPtr OnlineServicesPtr;
	IAuthPtr AuthPtr;
	IUserInfoPtr UserInfoPtr;
	ILobbiesPtr LobbyPtr;
	ISessionsPtr SessionPtr;

	TSharedPtr<FAccountInfo> GetAccountInfo(FPlatformUserId ID);
	void GetUserInfo();
	TSharedPtr<FUserInfo> UserInfo;

};
