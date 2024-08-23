#pragma once

#include "CoreMinimal.h"
#include "Online/OnlineServices.h"
#include "Online/Auth.h"
#include "Online/UserInfo.h"
#include "Online/Lobbies.h"
#include "Online/Sessions.h"
#include "Online/UserFile.h"
#include "Online/Commerce.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EOSSubsystem.generated.h"

using namespace UE::Online;

// Login
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoginStatusChanged, const FAuthLoginStatusChanged& AuthLoginStatusChanged);

// Lobby
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindLobbiesComplete, bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyInvitationAdded, const FLobbyInvitationAdded& LobbyInvitationAdded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUILobbyJoinRequested, const FUILobbyJoinRequested& UILobbyJoinRequested);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyJoined, const FLobbyJoined& LobbyJoined);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberJoined, const FLobbyMemberJoined& LobbyMemberJoined);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberLeft, const FLobbyMemberLeft& LobbyMemberLeft);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPromoteLobbyMemberComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeaderChanged, const FLobbyLeaderChanged& LobbyLeaderChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKickLobbyMemberComplete, bool bWasSuccessful);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyAttributesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyAttributesChanged, const FLobbyAttributesChanged& LobbyAttributesChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyLobbyMemberAttributesComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyMemberAttributesChanged, const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLobbyLeft, const FLobbyLeft& LobbyLeft);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveLobbyComplete, bool bWasSuccessful);

// Session
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateSessionComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddSessionMemberComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsComplete, bool bWasSuccessful, const TArray<FOnlineSessionId>& FoundSessionIds);
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
	virtual void Deinitialize() override;

	TArray<FOnlineEventDelegateHandle> OnlineEventDelegateHandles;

	// Login
	void Login(FPlatformUserId ID, int32 Type);
	FOnLoginComplete OnLoginComplete;
	TSharedPtr<FAccountInfo> AccountInfo;
	TSharedPtr<FAccountInfo> GetAccountInfo(FPlatformUserId ID);
	TSharedPtr<FUserInfo> UserInfo;
	void GetUserInfo();
	FOnLoginStatusChanged OnLoginStatusChanged;
	void BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);

	// Lobby
	TSharedPtr<const FLobby> CurrentLobby;

	FName LocalLobbyName = FName(TEXT("LocalLobbyName"));
	void CreateLobby();
	FOnCreateLobbyComplete OnCreateLobbyComplete;
	void FindLobbies(FString LobbyName, FString GameMode, FString MapName);
	FOnFindLobbiesComplete OnFindLobbiesComplete;
	void JoinLobby(TSharedRef<const FLobby> Lobby);
	FOnJoinLobbyComplete OnJoinLobbyComplete;

	FOnLobbyInvitationAdded OnLobbyInvitationAdded;
	void BroadcastOnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded);
	FOnUILobbyJoinRequested OnUILobbyJoinRequested;
	void BroadcastOnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested);

	FOnLobbyJoined OnLobbyJoined;
	void BroadcastOnLobbyJoined(const FLobbyJoined& LobbyJoined);
	FOnLobbyMemberJoined OnLobbyMemberJoined;
	void BroadcastOnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined);
	FOnLobbyMemberLeft OnLobbyMemberLeft;
	void BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft);

	void PromoteLobbyMember(FAccountId TargetAccountId);
	FOnPromoteLobbyMemberComplete OnPromoteLobbyMemberComplete;
	FOnLobbyLeaderChanged OnLobbyLeaderChanged;
	void BroadcastOnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged);
	void KickLobbyMember(FAccountId TargetAccountId);
	FOnKickLobbyMemberComplete OnKickLobbyMemberComplete;

	void ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes = TMap<FSchemaAttributeId, FSchemaVariant>(),
		TSet<FSchemaAttributeId> RemovedAttributes = TSet<FSchemaAttributeId>());
	FOnModifyLobbyAttributesComplete OnModifyLobbyAttributesComplete;
	FOnLobbyAttributesChanged OnLobbyAttributesChanged;
	void BroadcastOnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged);
	void ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes = TMap<FSchemaAttributeId, FSchemaVariant>(),
		TSet<FSchemaAttributeId> RemovedAttributes = TSet<FSchemaAttributeId>());
	FOnModifyLobbyMemberAttributesComplete OnModifyLobbyMemberAttributesComplete;
	FOnLobbyMemberAttributesChanged OnLobbyMemberAttributesChanged;
	void BroadcastOnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged);

	FOnLobbyLeft OnLobbyLeft;
	void BroadcastOnLobbyLeft(const FLobbyLeft& LobbyLeft);
	void LeaveLobby();
	FOnLeaveLobbyComplete OnLeaveLobbyComplete;

	// TODO GetJoinedLobbies

	// Session
	FName LocalSessionName = FName(TEXT("LocalSessionName"));
	void CreateSession();
	FOnCreateSessionComplete OnCreateSessionComplete;
	void AddSessionMember(FAccountId AccountId);
	FOnAddSessionMemberComplete OnAddSessionMemberComplete;
	TSharedPtr<const ISession> GetSessionByName();
	void FindSessions();
	FOnFindSessionsComplete OnFindSessionsComplete;
	void JoinSession(FOnlineSessionId OnlineSessionId);
	FOnJoinSessionComplete OnJoinSessionComplete;
	FString GetResolvedConnectString(FOnlineSessionId OnlineSessionId);
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
