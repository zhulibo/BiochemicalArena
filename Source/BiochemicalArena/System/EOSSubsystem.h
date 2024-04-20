#pragma once

#include "CoreMinimal.h"
#include "Online/OnlineServices.h"
#include "Online/Auth.h"
#include "Online/UserInfo.h"
#include "Online/Lobbies.h"
#include "Online/Sessions.h"
#include "Online/UserFile.h"
#include "Online/Commerce.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EOSSubsystem.generated.h"

using namespace UE::Online;

// Login
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginStatusChanged, const FAuthLoginStatusChanged& AuthLoginStatusChanged);

// Lobby
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

// Session
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveSessionComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnumerateFilesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnReadFileComplete, bool bWasSuccessful, const FUserFileContentsRef& FileContents);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWriteFileComplete, bool bWasSuccessful);

// Commerce
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQueryOffersComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCheckoutComplete, bool bWasSuccessful, TOptional<FString> TransactionId);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPurchaseCompleted, const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQueryOwnershipComplete, bool bWasSuccessful, const TArray<FString>);

UCLASS()
class BIOCHEMICALARENA_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UEOSSubsystem();

	// Login
	void Login(FPlatformUserId ID, int32 Type = 0);
	FOnLoginComplete OnLoginComplete;
	TSharedPtr<FUserInfo> UserInfo;
	void GetUserInfo();
	TSharedPtr<FAccountInfo> GetAccountInfo(FPlatformUserId ID);
	FOnLoginStatusChanged OnLoginStatusChanged;
	void BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);
	// TODO LoginStatus != ELoginStatus::LoggedIn 禁用一切操作

	// Lobby
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

	// Session
	void CreateSession();
	FOnCreateSessionComplete OnCreateSessionComplete;
	TSharedPtr<const ISession> GetPresenceSession();
	void JoinSession(FOnlineSessionId SessionId);
	FOnJoinSessionComplete OnJoinSessionComplete;
	void LeaveSession();
	FOnLeaveSessionComplete OnLeaveSessionComplete;

	// UserFile
	void EnumerateFiles();
	FOnEnumerateFilesComplete OnEnumerateFilesComplete;
	TArray<FString> GetEnumeratedFiles();
	FOnReadFileComplete OnReadFileComplete;
	void ReadFile(FString Filename);
	FOnWriteFileComplete OnWriteFileComplete;
	void WriteFile(FString Filename, FUserFileContents FileContents);

	// Commerce
	void QueryOffers();
	FOnQueryOffersComplete OnQueryOffersComplete;
	TArray<FOffer> GetOffers();
	void Checkout(TArray<FPurchaseOffer> Offers);
	FOnCheckoutComplete OnCheckoutComplete;
	void BroadcastOnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete);
	FOnPurchaseCompleted OnPurchaseCompleted;
	void QueryOwnership(); // TODO Waiting for EOS to encapsulate EOS_Ecom_QueryOwnership
	FOnQueryOwnershipComplete OnQueryOwnershipComplete;

protected:
	FPlatformUserId PlatformUserId;

	IOnlineServicesPtr OnlineServicesPtr;
	IAuthPtr AuthPtr;
	IUserInfoPtr UserInfoPtr;
	ILobbiesPtr LobbyPtr;
	ISessionsPtr SessionPtr;
	IUserFilePtr UserFilePtr;
	ICommercePtr CommercePtr;

};
