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
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginStatusChanged, const FAuthLoginStatusChanged& AuthLoginStatusChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindLobbyComplete, bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyInvitationAdded, const FLobbyInvitationAdded& LobbyInvitationAdded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUILobbyJoinRequested, const FUILobbyJoinRequested& UILobbyJoinRequested);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberJoined, const FLobbyMemberJoined& LobbyMemberJoined);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberLeft, const FLobbyMemberLeft& LobbyMemberLeft);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeaderChanged, const FLobbyLeaderChanged& LobbyLeaderChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyAttributesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyAttributesChanged, const FLobbyAttributesChanged& LobbyAttributesChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyMemberAttributesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberAttributesChanged, const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeft, const FLobbyLeft& LobbyLeft);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveLobbyComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveSessionComplete, bool bWasSuccessful);

UCLASS()
class BIOCHEMICALARENA_API UEOS : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UEOS();

	void Login(FPlatformUserId ID, int32 Type = 0);
	FOnLoginComplete OnLoginComplete;
	TSharedPtr<FUserInfo> UserInfo;
	void GetUserInfo();
	TSharedPtr<FAccountInfo> GetAccountInfo(FPlatformUserId ID);
	FOnLoginStatusChanged OnLoginStatusChanged;
	void BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);

	TSharedPtr<const FLobby> CurrentLobby;

	void CreateLobby();
	FOnCreateLobbyComplete OnCreateLobbyComplete;
	void FindLobby();
	FOnFindLobbyComplete OnFindLobbyComplete;
	void JoinLobby(TSharedRef<const FLobby> Lobby);
	FOnJoinLobbyComplete OnJoinLobbyComplete;

	FOnLobbyInvitationAdded OnLobbyInvitationAdded;
	void BroadcastOnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	FOnUILobbyJoinRequested OnUILobbyJoinRequested;
	void BroadcastOnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);

	FOnLobbyMemberJoined OnLobbyMemberJoined;
	void BroadcastOnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);
	FOnLobbyMemberLeft OnLobbyMemberLeft;
	void BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);
	void PromoteLobbyMember();
	FOnLobbyLeaderChanged OnLobbyLeaderChanged;
	void BroadcastOnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);

	void ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes);
	FOnModifyLobbyAttributesComplete OnModifyLobbyAttributesComplete;
	FOnLobbyAttributesChanged OnLobbyAttributesChanged;
	void BroadcastOnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);
	void ModifyLobbyMemberAttributes();
	FOnModifyLobbyMemberAttributesComplete OnModifyLobbyMemberAttributesComplete;
	FOnLobbyMemberAttributesChanged OnLobbyMemberAttributesChanged;
	void BroadcastOnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

	FOnLobbyLeft OnLobbyLeft;
	void BroadcastOnLobbyLeft(const FLobbyLeft& LobbyLeft);
	void LeaveLobby();
	FOnLeaveLobbyComplete OnLeaveLobbyComplete;

	void CreateSession();
	FOnCreateSessionComplete OnCreateSessionComplete;
	TSharedPtr<const ISession> GetPresenceSession();
	void JoinSession(FOnlineSessionId SessionId);
	FOnJoinSessionComplete OnJoinSessionComplete;
	void LeaveSession();
	FOnLeaveSessionComplete OnLeaveSessionComplete;

protected:
	FPlatformUserId PlatformUserId;

	IOnlineServicesPtr OnlineServicesPtr;
	IAuthPtr AuthPtr;
	IUserInfoPtr UserInfoPtr;
	ILobbiesPtr LobbyPtr;
	ISessionsPtr SessionPtr;

};
