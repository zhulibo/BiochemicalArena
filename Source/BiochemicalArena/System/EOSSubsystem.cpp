#include "EOSSubsystem.h"
#include "Online/OnlineAsyncOpHandle.h"

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
void UEOSSubsystem::Login(FPlatformUserId ID, int32 Type)
{
	if (AuthPtr == nullptr) return;
	PlatformUserId = ID;

	TSharedPtr<FAccountInfo> AccountInfo = GetAccountInfo(PlatformUserId);
	if (AccountInfo && AccountInfo->AccountId.IsValid() && AuthPtr->IsLoggedIn(AccountInfo->AccountId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Already logged in"));
		OnLoginComplete.Broadcast(true);
		return;
	}

	FAuthLogin::Params Params;
	Params.PlatformUserId = ID;

	// 默认玩家账号密码登录
	if (Type == 0)
	{
		Params.CredentialsType = LoginCredentialsType::Password;
		Params.CredentialsId = FString("");
		Params.CredentialsToken.Emplace<FString>("");
	}
	// 本地开发者登录-账号1
	else if(Type == 1)
	{
		Params.CredentialsType = LoginCredentialsType::Developer;
		Params.CredentialsId = FString("127.0.0.1:6000");
		Params.CredentialsToken.Emplace<FString>("dust");
	}
	// 本地开发者登录-账号2
	else if(Type == 2)
	{
		Params.CredentialsType = LoginCredentialsType::Developer;
		Params.CredentialsId = FString("127.0.0.1:6000");
		Params.CredentialsToken.Emplace<FString>("dev");
	}

	AuthPtr->Login(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FAuthLogin>& Result)
	{
		if(Result.IsOk())
		{
			TSharedRef<FAccountInfo> AccountInfo = Result.GetOkValue().AccountInfo;
			OnLoginComplete.Broadcast(true);
			GetUserInfo();
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnLoginComplete.Broadcast(false);
		}
	});
}

// 获取玩家DisplayName
void UEOSSubsystem::GetUserInfo()
{
	if (UserInfoPtr == nullptr) return;

	FGetUserInfo::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.AccountId = GetAccountInfo(PlatformUserId)->AccountId;

	TOnlineResult<FGetUserInfo> Result = UserInfoPtr->GetUserInfo(MoveTemp(Params));
	if (Result.IsOk())
	{
		UserInfo = Result.GetOkValue().UserInfo;
	}
	else
	{
		UserInfo = MakeShared<FUserInfo>();
		FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Get user info failed!"));
	}
}

// 获取玩家账号信息
TSharedPtr<FAccountInfo> UEOSSubsystem::GetAccountInfo(FPlatformUserId ID)
{
	TSharedPtr<FAccountInfo> AccountInfo;
	if (AuthPtr == nullptr) return AccountInfo;

	FAuthGetLocalOnlineUserByPlatformUserId::Params Params = { ID };
	TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> Result = AuthPtr->GetLocalOnlineUserByPlatformUserId(MoveTemp(Params));
	if (Result.IsOk())
	{
		AccountInfo = Result.GetOkValue().AccountInfo;
	}
	return AccountInfo;
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
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LocalName = FName(TEXT("Test"));
	Params.SchemaId = FName(TEXT("GameLobby"));
	Params.bPresenceEnabled = true;
	Params.MaxMembers = 18;
	Params.JoinPolicy = ELobbyJoinPolicy::PublicAdvertised;
	Params.Attributes.Emplace(FName(TEXT("GameMode")), FString(TEXT("TeamDeadMatch")));
	Params.Attributes.Emplace(FName(TEXT("SessionId")), FString(TEXT("None")));
	Params.Attributes.Emplace(FName(TEXT("MapName")), FString(TEXT("Dev")));

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
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnCreateLobbyComplete.Broadcast(false);
		}
	});
}

// 查找大厅
void UEOSSubsystem::FindLobby()
{
	if (LobbyPtr == nullptr) return;

	FFindLobbies::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.MaxResults = 50;
	// Params.Filters.Emplace(FFindLobbySearchFilter{
	// 	FSchemaAttributeId(TEXT("GameMode")),
	// 	ESchemaAttributeComparisonOp::Near,
	// 	FString(TEXT("TeamDeadMatch"))
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
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnFindLobbyComplete.Broadcast(false, TArray<TSharedRef<const FLobby>>());
		}
	});
}

// 加入大厅
void UEOSSubsystem::JoinLobby(TSharedRef<const FLobby> Lobby)
{
	if (LobbyPtr == nullptr) return;

	FJoinLobby::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LocalName = Lobby->LocalName;
	Params.LobbyId = Lobby->LobbyId;
	Params.bPresenceEnabled = true;
	Params.UserAttributes.Emplace(FName(TEXT("Appearance")), UserInfo->DisplayName);

	LobbyPtr->JoinLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FJoinLobby>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = Result.GetOkValue().Lobby;
			OnJoinLobbyComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
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

void UEOSSubsystem::BroadcastOnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	OnLobbyMemberJoined.Broadcast(LobbyMemberJoined);
}

void UEOSSubsystem::BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLef)
{
	OnLobbyMemberLeft.Broadcast(LobbyMemberLef);
}

// 房主提升另一名成员为房主
void UEOSSubsystem::PromoteLobbyMember()
{
	if (LobbyPtr == nullptr) return;

	FPromoteLobbyMember::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	for (auto& Member : CurrentLobby->Members)
	{
		if (Member.Value->AccountId != GetAccountInfo(PlatformUserId)->AccountId)
		{
			Params.TargetAccountId = Member.Value->AccountId;
			break;
		}
	}

	LobbyPtr->PromoteLobbyMember(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FPromoteLobbyMember>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = nullptr;
			LeaveLobby();
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Promote lobby member failed!"));
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	OnLobbyLeaderChanged.Broadcast(LobbyLeaderChanged);
}

// 修改大厅属性
void UEOSSubsystem::ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes)
{
	if (LobbyPtr == nullptr) return;

	FModifyLobbyAttributes::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.UpdatedAttributes = MoveTemp(UpdatedAttributes);

	LobbyPtr->ModifyLobbyAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyAttributes>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = nullptr;
			OnModifyLobbyAttributesComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnModifyLobbyAttributesComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	OnLobbyAttributesChanged.Broadcast(LobbyAttributesChanged);
}

// 修改大厅成员属性
void UEOSSubsystem::ModifyLobbyMemberAttributes()
{
	if (LobbyPtr == nullptr) return;

	FModifyLobbyMemberAttributes::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	// TODO

	LobbyPtr->ModifyLobbyMemberAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyMemberAttributes>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = nullptr;
			OnModifyLobbyMemberAttributesComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
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

	// 如果是房主离开大厅，则提升另一名成员为房主
	if(CurrentLobby->OwnerAccountId == GetAccountInfo(PlatformUserId)->AccountId && CurrentLobby->Members.Num() > 1)
	{
		PromoteLobbyMember();
		return;
	}

	FLeaveLobby::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;

	LobbyPtr->LeaveLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FLeaveLobby>& Result)
	{
		if (Result.IsOk())
		{
			CurrentLobby = nullptr;
			OnLeaveLobbyComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnLeaveLobbyComplete.Broadcast(false);
		}
	});
}

// 创建会话
void UEOSSubsystem::CreateSession()
{
	if (SessionPtr == nullptr) return;

	FCreateSession::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.SessionName = FName(TEXT("Main"));
	Params.bPresenceEnabled = true;

	SessionPtr->CreateSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCreateSession>& Result)
	{
		if (Result.IsOk())
		{
			OnCreateSessionComplete.Broadcast(true);
			FOnlineSessionId SessionId = GetPresenceSession()->GetSessionId();
			// ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
			// 	{ FName(TEXT("SessionId")), FString(SessionId.ToString()) }
			// });
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
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
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	TOnlineResult<FGetPresenceSession> Result = SessionPtr->GetPresenceSession(MoveTemp(Params));
	if (Result.IsOk())
	{
		 Session = Result.GetOkValue().Session;
	}
	else
	{
		FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Get presence session failed!"));
	}
	return Session;
}

// 加入会话
void UEOSSubsystem::JoinSession(FOnlineSessionId SessionId)
{
	if (SessionPtr == nullptr) return;

	FJoinSession::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.SessionName = FName(TEXT("Main"));
	Params.SessionId = SessionId;
	Params.bPresenceEnabled = true;

	SessionPtr->JoinSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FJoinSession>& Result)
	{
		if (Result.IsOk())
		{
			OnJoinSessionComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnJoinSessionComplete.Broadcast(false);
		}
	});
}

// 离开会话
void UEOSSubsystem::LeaveSession()
{
	if (SessionPtr == nullptr) return;

	FLeaveSession::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.SessionName = FName(TEXT("Main"));
	Params.bDestroySession = CurrentLobby->OwnerAccountId == GetAccountInfo(PlatformUserId)->AccountId;

	SessionPtr->LeaveSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FLeaveSession>& Result)
	{
		if (Result.IsOk())
		{
			OnLeaveSessionComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnLeaveSessionComplete.Broadcast(false);
		}
	});
}

// 缓存用户文件
void UEOSSubsystem::EnumerateFiles()
{
	if (UserFilePtr == nullptr) return;

	FUserFileEnumerateFiles::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	UserFilePtr->EnumerateFiles(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FUserFileEnumerateFiles>& Result)
	{
		if (Result.IsOk())
		{
			OnEnumerateFilesComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
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
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	TOnlineResult<FUserFileGetEnumeratedFiles> Result = UserFilePtr->GetEnumeratedFiles(MoveTemp(Params));
	if (Result.IsOk())
	{
		Filenames = Result.GetOkValue().Filenames;
	}
	else
	{
		FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
	}
	return Filenames;
}

// 读取用户文件
void UEOSSubsystem::ReadFile(FString Filename)
{
	if (UserFilePtr == nullptr) return;

	FUserFileReadFile::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
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
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnReadFileComplete.Broadcast(false, FUserFileContentsRef());
		}
	});
}

// 创建或覆盖用户文件
void UEOSSubsystem::WriteFile(FString Filename, FUserFileContents FileContents)
{
	if (UserFilePtr == nullptr) return;

	FUserFileWriteFile::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
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
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnWriteFileComplete.Broadcast(false);
		}
	});
}

// 缓存商品列表
void UEOSSubsystem::QueryOffers()
{
	if (CommercePtr == nullptr) return;

	FCommerceQueryOffers::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	CommercePtr->QueryOffers(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCommerceQueryOffers>& Result)
	{
		if (Result.IsOk())
		{
			OnQueryOffersComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
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
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	TOnlineResult<FCommerceGetOffers> Result = CommercePtr->GetOffers(MoveTemp(Params));
	if (Result.IsOk())
	{
		Offers = Result.GetOkValue().Offers;
	}
	else
	{
		FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
	}
	return Offers;
}

// 购买商品
void UEOSSubsystem::Checkout(TArray<FPurchaseOffer> Offers)
{
	if (CommercePtr == nullptr) return;

	FCommerceCheckout::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
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
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnCheckoutComplete.Broadcast(false, FString(TEXT("")));
		}
	});
}

void UEOSSubsystem::BroadcastOnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
{
	OnPurchaseCompleted.Broadcast(CommerceOnPurchaseComplete);
}

// 缓存已购商品
void UEOSSubsystem::QueryEntitlements()
{
	if (CommercePtr == nullptr) return;

	FCommerceQueryEntitlements::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;
	Params.bIncludeRedeemed = true;

	CommercePtr->QueryEntitlements(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCommerceQueryEntitlements>& Result)
	{
		if (Result.IsOk())
		{
			OnQueryEntitlementsComplete.Broadcast(true);
		}
		else
		{
			FOnlineError Error = Result.GetErrorValue();
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
			OnQueryEntitlementsComplete.Broadcast(false);
		}
	});
}

// 获取已购商品
TArray<FEntitlement> UEOSSubsystem::GetEntitlements()
{
	TArray<FEntitlement> Entitlements;
	if (CommercePtr == nullptr) return Entitlements;

	FCommerceGetEntitlements::Params Params;
	Params.LocalAccountId = GetAccountInfo(PlatformUserId)->AccountId;

	TOnlineResult<FCommerceGetEntitlements> Result = CommercePtr->GetEntitlements(MoveTemp(Params));
	if (Result.IsOk())
	{
		Entitlements = Result.GetOkValue().Entitlements;
	}
	else
	{
		FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Error.GetLogString());
	}
	return Entitlements;
}
