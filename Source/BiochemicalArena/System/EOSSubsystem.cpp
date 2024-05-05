#include "EOSSubsystem.h"

#include <string>

#include "BiochemicalArena/BiochemicalArena.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Online/CoreOnline.h"

UEOSSubsystem::UEOSSubsystem()
{
	OnlineServicesPtr = GetServices();
	if (OnlineServicesPtr)
	{
		AuthPtr = OnlineServicesPtr->GetAuthInterface();
		UserInfoPtr = OnlineServicesPtr->GetUserInfoInterface();
		LobbyPtr = OnlineServicesPtr->GetLobbiesInterface();
		SessionPtr = OnlineServicesPtr->GetSessionsInterface();
		UserFilePtr = OnlineServicesPtr->GetUserFileInterface();
		CommercePtr = OnlineServicesPtr->GetCommerceInterface();

		if (AuthPtr)
		{
			(void) AuthPtr->OnLoginStatusChanged().Add(this, &ThisClass::BroadcastOnLoginStatusChanged);
		}
		if (LobbyPtr)
		{
			(void) LobbyPtr->OnLobbyJoined().Add(this, &ThisClass::BroadcastOnLobbyJoined);
			(void) LobbyPtr->OnLobbyMemberJoined().Add(this, &ThisClass::BroadcastOnLobbyMemberJoined);
			(void) LobbyPtr->OnLobbyMemberLeft().Add(this, &ThisClass::BroadcastOnLobbyMemberLeft);
			(void) LobbyPtr->OnLobbyLeaderChanged().Add(this, &ThisClass::BroadcastOnLobbyLeaderChanged);

			(void) LobbyPtr->OnLobbyInvitationAdded().Add(this, &ThisClass::BroadcastOnLobbyInvitationAdded);
			(void) LobbyPtr->OnUILobbyJoinRequested().Add(this, &ThisClass::BroadcastOnUILobbyJoinRequested);

			(void) LobbyPtr->OnLobbyAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyAttributesChanged);
			(void) LobbyPtr->OnLobbyMemberAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyMemberAttributesChanged);

			(void) LobbyPtr->OnLobbyLeft().Add(this, &ThisClass::BroadcastOnLobbyLeft);
		}
		if (CommercePtr)
		{
			(void) CommercePtr->OnPurchaseCompleted().Add(this, &ThisClass::BroadcastOnPurchaseCompleted);
		}
	}
}

// 登录
void UEOSSubsystem::Login(FPlatformUserId TempPlatformUserId, int32 Type)
{
	if (AuthPtr == nullptr || !TempPlatformUserId.IsValid()) return;

	PlatformUserId = TempPlatformUserId;

	// 是否已登录
	GetAccountInfo(PlatformUserId);
	if (AccountInfo && AuthPtr->IsLoggedIn(AccountInfo->AccountId))
	{
		GetUserInfo();
		UE_LOG(LogTemp, Warning, TEXT("Already logged in"));
		OnLoginComplete.Broadcast(true);
		return;
	}

	FAuthLogin::Params Params;
	Params.PlatformUserId = PlatformUserId;

	// Live - Epic Games Launcher
	if (Type == 0)
	{
		Params.CredentialsType = LoginCredentialsType::ExchangeCode;
		FString Token;
		FParse::Value(FCommandLine::Get(), TEXT("AUTH_PASSWORD"), Token);
		Params.CredentialsToken.Emplace<FString>(Token);
	}
	// Dev - DevAuthTool Account 1
	else if(Type == 1)
	{
		Params.CredentialsType = LoginCredentialsType::Developer;
		Params.CredentialsId = FString("127.0.0.1:6000");
		Params.CredentialsToken.Emplace<FString>("dust9923");
	}
	// Dev - DevAuthTool Account 2
	else if(Type == 2)
	{
		Params.CredentialsType = LoginCredentialsType::Developer;
		Params.CredentialsId = FString("127.0.0.1:6000");
		Params.CredentialsToken.Emplace<FString>("dev10001");
	}
	// Dev - DevAuthTool Account 3
	else if(Type == 3)
	{
		Params.CredentialsType = LoginCredentialsType::Developer;
		Params.CredentialsId = FString("127.0.0.1:6000");
		Params.CredentialsToken.Emplace<FString>("dev10002");
	}
	AuthPtr->Login(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FAuthLogin>& Result)
	{
		if(Result.IsOk())
		{
			this->AccountInfo = Result.GetOkValue().AccountInfo;
			GetUserInfo();
			OnLoginComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnLoginComplete.Broadcast(false);
		}
	});
}

// 获取账号信息
TSharedPtr<FAccountInfo> UEOSSubsystem::GetAccountInfo(FPlatformUserId ID)
{
	if (AuthPtr == nullptr) return AccountInfo;

	FAuthGetLocalOnlineUserByPlatformUserId::Params Params = { ID };
	TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> Result = AuthPtr->GetLocalOnlineUserByPlatformUserId(MoveTemp(Params));
	if (Result.IsOk())
	{
		AccountInfo = Result.GetOkValue().AccountInfo;
	}
	return AccountInfo;
}

// 获取用户信息
void UEOSSubsystem::GetUserInfo()
{
	if (UserInfoPtr == nullptr) return;

	FGetUserInfo::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.AccountId = AccountInfo->AccountId;

	TOnlineResult<FGetUserInfo> Result = UserInfoPtr->GetUserInfo(MoveTemp(Params));
	if (Result.IsOk())
	{
		UserInfo = Result.GetOkValue().UserInfo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Get user info failed!"), false);
	}
}

void UEOSSubsystem::BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	OnLoginStatusChanged.Broadcast(AuthLoginStatusChanged);
}

// 创建大厅
void UEOSSubsystem::CreateLobby()
{
	if (LobbyPtr == nullptr) return;

	FCreateLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LocalName = FName(UserInfo->DisplayName + FString(TEXT("'s Lobby LocalName")));
	Params.SchemaId = FName(TEXT("GameLobby"));
	Params.MaxMembers = 18;
	Params.JoinPolicy = ELobbyJoinPolicy::PublicAdvertised;
	Params.Attributes.Emplace(FName(TEXT("LobbyName")), FString(TEXT("TestLobbyName")));
	Params.Attributes.Emplace(FName(TEXT("GameMode")), FString(TEXT("TeamDeadMatch")));
	Params.Attributes.Emplace(FName(TEXT("MapName")), FString(TEXT("Dev")));
	Params.Attributes.Emplace(FName(TEXT("SessionId")), FString(TEXT("")));
	Params.Attributes.Emplace(FName(TEXT("Status")), static_cast<int64>(0));
	Params.UserAttributes.Emplace(FName(TEXT("Team")), static_cast<int64>(FMath::RandRange(1, 2)));
	Params.UserAttributes.Emplace(FName(TEXT("PlayerName")), UserInfo->DisplayName);
	Params.UserAttributes.Emplace(FName(TEXT("bIsReady")), false);

	LobbyPtr->CreateLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCreateLobby>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = Result.GetOkValue().Lobby;
			OnCreateLobbyComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnCreateLobbyComplete.Broadcast(false);
		}
	});
}

// 查找大厅
void UEOSSubsystem::FindLobby()
{
	if (LobbyPtr == nullptr) return;

	FFindLobbies::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.MaxResults = 50;
	// TODO Near不可用
	// Params.Filters.Emplace(FFindLobbySearchFilter{
	// 	FSchemaAttributeId(TEXT("LobbyName")),
	// 	ESchemaAttributeComparisonOp::Equals,
	// 	FString(TEXT(""))
	// });
	Params.Filters.Emplace(FFindLobbySearchFilter{
		FSchemaAttributeId(TEXT("GameMode")),
		ESchemaAttributeComparisonOp::Equals,
		FString(TEXT("TeamDeadMatch"))
	});
	// Params.Filters.Emplace(FFindLobbySearchFilter{
	// 	FSchemaAttributeId(TEXT("MapName")),
	// 	ESchemaAttributeComparisonOp::Equals,
	// 	FString(TEXT(""))
	// });

	LobbyPtr->FindLobbies(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FFindLobbies>& Result)
	{
		if (Result.IsOk())
		{
			OnFindLobbyComplete.Broadcast(true, Result.GetOkValue().Lobbies);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnFindLobbyComplete.Broadcast(false, TArray<TSharedRef<const FLobby>>());
		}
	});
}

// 加入大厅
void UEOSSubsystem::JoinLobby(TSharedRef<const FLobby> Lobby)
{
	if (LobbyPtr == nullptr) return;

	FJoinLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LocalName = FName(UserInfo->DisplayName + FString(TEXT("'s Lobby LocalName")));
	Params.LobbyId = Lobby->LobbyId;
	Params.UserAttributes.Emplace(FName(TEXT("Team")), static_cast<int64>(FMath::RandRange(1, 2)));
	Params.UserAttributes.Emplace(FName(TEXT("PlayerName")), UserInfo->DisplayName);
	Params.UserAttributes.Emplace(FName(TEXT("bIsReady")), false);

	LobbyPtr->JoinLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FJoinLobby>& Result)
	{
		if (Result.IsOk())
		{
			for (auto& Item : Result.GetOkValue().Lobby->Members)
			{
				UE_LOG(LogTemp, Warning, TEXT("Member->Attributes.Num(): %d"), Item.Value->Attributes.Num());
			}

			CurrentLobby = Result.GetOkValue().Lobby;
			OnJoinLobbyComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnJoinLobbyComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded)
{
	OnLobbyInvitationAdded.Broadcast(LobbyInvitationAdded);
}

void UEOSSubsystem::BroadcastOnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested)
{
	OnUILobbyJoinRequested.Broadcast(UILobbyJoinRequested);
}

void UEOSSubsystem::BroadcastOnLobbyJoined(const FLobbyJoined& LobbyJoined)
{
	CurrentLobby = LobbyJoined.Lobby;
	OnLobbyJoined.Broadcast(LobbyJoined);
}

void UEOSSubsystem::BroadcastOnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	OnLobbyMemberJoined.Broadcast(LobbyMemberJoined);
}

void UEOSSubsystem::BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLef)
{
	OnLobbyMemberLeft.Broadcast(LobbyMemberLef);
}

// 房主提升另一名成员为房主
void UEOSSubsystem::PromoteLobbyMember(FAccountId TargetAccountId)
{
	if (LobbyPtr == nullptr) return;

	FPromoteLobbyMember::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.TargetAccountId = TargetAccountId;

	LobbyPtr->PromoteLobbyMember(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FPromoteLobbyMember>& Result)
	{
		if (Result.IsOk())
		{
			OnPromoteLobbyMemberComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnPromoteLobbyMemberComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	OnLobbyLeaderChanged.Broadcast(LobbyLeaderChanged);
}

// 修改大厅属性
void UEOSSubsystem::ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes,
	TSet<FSchemaAttributeId> RemovedAttributes)
{
	if (LobbyPtr == nullptr) return;

	FModifyLobbyAttributes::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.UpdatedAttributes = MoveTemp(UpdatedAttributes);
	Params.RemovedAttributes = MoveTemp(RemovedAttributes);

	LobbyPtr->ModifyLobbyAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyAttributes>& Result)
	{
		if (Result.IsOk())
		{
			OnModifyLobbyAttributesComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnModifyLobbyAttributesComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	OnLobbyAttributesChanged.Broadcast(LobbyAttributesChanged);
}

// 修改大厅成员属性
void UEOSSubsystem::ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes,
	TSet<FSchemaAttributeId> RemovedAttributes)
{
	if (LobbyPtr == nullptr) return;

	FModifyLobbyMemberAttributes::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.UpdatedAttributes = MoveTemp(UpdatedAttributes);
	Params.RemovedAttributes = MoveTemp(RemovedAttributes);

	LobbyPtr->ModifyLobbyMemberAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyMemberAttributes>& Result)
	{
		if (Result.IsOk())
		{
			OnModifyLobbyMemberAttributesComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnModifyLobbyMemberAttributesComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged)
{
	OnLobbyMemberAttributesChanged.Broadcast(LobbyMemberAttributesChanged);
}

void UEOSSubsystem::BroadcastOnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	OnLobbyLeft.Broadcast(LobbyLeft);
}

// 离开大厅
void UEOSSubsystem::LeaveLobby()
{
	if (LobbyPtr == nullptr) return;

	FLeaveLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;

	LobbyPtr->LeaveLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FLeaveLobby>& Result)
	{
		if (Result.IsOk())
		{
			OnLeaveLobbyComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnLeaveLobbyComplete.Broadcast(false);
		}
	});
}

// 创建会话
void UEOSSubsystem::CreateSession()
{
	if (SessionPtr == nullptr) return;

	FCreateSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = FName(TEXT("LobbySession"));
	Params.SessionIdOverride = FString(TEXT(""));
	Params.SessionSettings.SchemaName = FName(TEXT("SchemaName"));
	Params.SessionSettings.NumMaxConnections = 18;

	SessionPtr->CreateSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCreateSession>& Result)
	{
		if (Result.IsOk())
		{
			OnCreateSessionComplete.Broadcast(true);

			// UE_LOG(LogTemp, Warning, TEXT("before convert FOnlineSessionId SessionId: %s"), *ToString(GetPresenceSession()->GetSessionId()));
			// FOnlineIdRegistryRegistry& Registry = FOnlineIdRegistryRegistry::Get();
			// TArray<uint8> SessionIdData = Registry.ToReplicationData(GetPresenceSession()->GetSessionId());
			// const std::string cstr(reinterpret_cast<const char*>(SessionIdData.GetData()), SessionIdData.Num());
			// FString SessionId = cstr.c_str();
			// UE_LOG(LogTemp, Warning, TEXT("before transfer FString SessionId: %s"), *SessionId);

			ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
				{ FName(TEXT("SessionId")), ToString(GetPresenceSession()->GetSessionId()) }
			});
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnCreateSessionComplete.Broadcast(false);
		}
	});
}

// 获取会话
TSharedPtr<const ISession> UEOSSubsystem::GetPresenceSession()
{
	TSharedPtr<const ISession> Session;
	if (SessionPtr == nullptr) return Session;

	FGetPresenceSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	TOnlineResult<FGetPresenceSession> Result = SessionPtr->GetPresenceSession(MoveTemp(Params));
	if (Result.IsOk())
	{
		 Session = Result.GetOkValue().Session;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Get presence session failed!"), false);
	}
	return Session;
}

// 加入会话
void UEOSSubsystem::JoinSession(FString SessionId)
{
	if (SessionPtr == nullptr) return;

	FJoinSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = FName(TEXT("Main"));

	// UE_LOG(LogTemp, Warning, TEXT("after transfer FString SessionId: %s"), *SessionId);
	TArray<uint8> SessionIdData;
	SessionIdData.SetNum(SessionId.Len());
	memcpy(SessionIdData.GetData(), TCHAR_TO_ANSI(*SessionId), SessionId.Len());
	FOnlineIdRegistryRegistry& Registry = FOnlineIdRegistryRegistry::Get();
	Params.SessionId = Registry.ToSessionId(EOnlineServices::Epic, SessionIdData);
	// UE_LOG(LogTemp, Warning, TEXT("after convert FOnlineSessionId SessionId: %s"), *ToString(Params.SessionId));

	SessionPtr->JoinSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FJoinSession>& Result)
	{
		if (Result.IsOk())
		{
			OnJoinSessionComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnJoinSessionComplete.Broadcast(false);
		}
	});
}

// 离开会话
void UEOSSubsystem::LeaveSession()
{
	if (SessionPtr == nullptr) return;

	FLeaveSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = FName(TEXT("Main"));
	Params.bDestroySession = CurrentLobby->OwnerAccountId == AccountInfo->AccountId;

	SessionPtr->LeaveSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FLeaveSession>& Result)
	{
		if (Result.IsOk())
		{
			OnLeaveSessionComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnLeaveSessionComplete.Broadcast(false);
		}
	});
}

// 缓存用户文件
void UEOSSubsystem::EnumerateFiles()
{
	if (UserFilePtr == nullptr) return;

	FUserFileEnumerateFiles::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	UserFilePtr->EnumerateFiles(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FUserFileEnumerateFiles>& Result)
	{
		if (Result.IsOk())
		{
			OnEnumerateFilesComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnEnumerateFilesComplete.Broadcast(false);
		}
	});
}

// 获取用户文件名
TArray<FString> UEOSSubsystem::GetEnumeratedFiles()
{
	TArray<FString> Filenames;
	if (UserFilePtr == nullptr) return Filenames;

	FUserFileGetEnumeratedFiles::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	TOnlineResult<FUserFileGetEnumeratedFiles> Result = UserFilePtr->GetEnumeratedFiles(MoveTemp(Params));
	if (Result.IsOk())
	{
		Filenames = Result.GetOkValue().Filenames;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	}
	return Filenames;
}

// 读取用户文件
void UEOSSubsystem::ReadFile(FString Filename)
{
	if (UserFilePtr == nullptr) return;

	FUserFileReadFile::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.Filename = Filename;

	UserFilePtr->ReadFile(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FUserFileReadFile>& Result)
	{
		if (Result.IsOk())
		{
			OnReadFileComplete.Broadcast(true, Result.GetOkValue().FileContents);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnReadFileComplete.Broadcast(false, FUserFileContentsRef());
		}
	});
}

// 创建或覆盖用户文件
void UEOSSubsystem::WriteFile(FString Filename, FUserFileContents FileContents)
{
	if (UserFilePtr == nullptr) return;

	FUserFileWriteFile::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.Filename = Filename;
	Params.FileContents = MoveTemp(FileContents);

	UserFilePtr->WriteFile(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FUserFileWriteFile>& Result)
	{
		if (Result.IsOk())
		{
			OnWriteFileComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnWriteFileComplete.Broadcast(false);
		}
	});
}

// 缓存商品列表
void UEOSSubsystem::QueryOffers()
{
	if (CommercePtr == nullptr) return;

	FCommerceQueryOffers::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	CommercePtr->QueryOffers(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCommerceQueryOffers>& Result)
	{
		if (Result.IsOk())
		{
			OnQueryOffersComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnQueryOffersComplete.Broadcast(false);
		}
	});
}

// 获取商品列表
TArray<FOffer> UEOSSubsystem::GetOffers()
{
	TArray<FOffer> Offers;
	if (CommercePtr == nullptr) return Offers;

	FCommerceGetOffers::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	TOnlineResult<FCommerceGetOffers> Result = CommercePtr->GetOffers(MoveTemp(Params));
	if (Result.IsOk())
	{
		Offers = Result.GetOkValue().Offers;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	}
	return Offers;
}

// 购买商品
void UEOSSubsystem::Checkout(TArray<FPurchaseOffer> Offers)
{
	if (CommercePtr == nullptr) return;

	FCommerceCheckout::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.Offers = Offers;

	CommercePtr->Checkout(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCommerceCheckout>& Result)
	{
		if (Result.IsOk())
		{
			OnCheckoutComplete.Broadcast(true, Result.GetOkValue().TransactionId);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnCheckoutComplete.Broadcast(false, FString(TEXT("")));
		}
	});
}

void UEOSSubsystem::BroadcastOnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
{
	OnPurchaseCompleted.Broadcast(CommerceOnPurchaseComplete);
}

// 获取已购商品
void UEOSSubsystem::QueryOwnership()
{
	if (CommercePtr == nullptr) return;

	OnQueryOwnershipComplete.Broadcast(true, TArray<FString>());

	// FCommerceQueryOwnership::Params Params;
	// Params.LocalAccountId = AccountInfo->AccountId;
	// Params.bIncludeRedeemed = true;
	//
	// CommercePtr->QueryOwnership(MoveTemp(Params))
	// .OnComplete([this](const TOnlineResult<FCommerceQueryOwnership>& Result)
	// {
	// 	if (Result.IsOk())
	// 	{
	// 		OnQueryOwnershipComplete.Broadcast(true, Result.GetOkValue().Ownership);
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	// 		OnQueryOwnershipComplete.Broadcast(false, TArray<FString>());
	// 	}
	// });
}
