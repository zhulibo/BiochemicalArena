#include "EOSSubsystem.h"

#include "BiochemicalArena/BiochemicalArena.h"

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
			OnlineEventDelegateHandles.Add(AuthPtr->OnLoginStatusChanged().Add(this, &ThisClass::BroadcastOnLoginStatusChanged));
		}
		if (LobbyPtr)
		{
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyJoined().Add(this, &ThisClass::BroadcastOnLobbyJoined));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyMemberJoined().Add(this, &ThisClass::BroadcastOnLobbyMemberJoined));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyMemberLeft().Add(this, &ThisClass::BroadcastOnLobbyMemberLeft));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyLeaderChanged().Add(this, &ThisClass::BroadcastOnLobbyLeaderChanged));

			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyInvitationAdded().Add(this, &ThisClass::BroadcastOnLobbyInvitationAdded));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnUILobbyJoinRequested().Add(this, &ThisClass::BroadcastOnUILobbyJoinRequested));

			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyAttributesChanged));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyMemberAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyMemberAttributesChanged));

			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyLeft().Add(this, &ThisClass::BroadcastOnLobbyLeft));
		}
		if (CommercePtr)
		{
			// OnlineEventDelegateHandles.Add(CommercePtr->OnPurchaseCompleted().Add(this, &ThisClass::BroadcastOnPurchaseCompleted));
		}
	}
}

void UEOSSubsystem::Deinitialize()
{
	for (auto& Handle : OnlineEventDelegateHandles)
	{
		Handle.Unbind();
	}

	Super::Deinitialize();
}

// 登录
void UEOSSubsystem::Login(FPlatformUserId TempPlatformUserId, int32 Type)
{
	if (AuthPtr == nullptr || !TempPlatformUserId.IsValid()) return;

	PlatformUserId = TempPlatformUserId;
	AccountInfo = GetAccountInfo(PlatformUserId);

	// 是否已登录
	if (AccountInfo && AuthPtr->IsLoggedIn(AccountInfo->AccountId))
	{
		GetUserInfo();
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MUTANT, TEXT("Login info exists"), false);
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
			AccountInfo = Result.GetOkValue().AccountInfo;
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
	if (AuthPtr == nullptr) return nullptr;

	FAuthGetLocalOnlineUserByPlatformUserId::Params Params = { ID };
	TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> Result = AuthPtr->GetLocalOnlineUserByPlatformUserId(MoveTemp(Params));
	if (Result.IsOk())
	{
		return Result.GetOkValue().AccountInfo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	}
	return nullptr;
}

// 获取用户信息
void UEOSSubsystem::GetUserInfo()
{
	if (UserInfoPtr == nullptr || AccountInfo == nullptr) return;

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
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("GetUserInfo failed!"), false);
	}
}

void UEOSSubsystem::BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	OnLoginStatusChanged.Broadcast(AuthLoginStatusChanged);
}

// 创建大厅
void UEOSSubsystem::CreateLobby()
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId) || LobbyPtr == nullptr)
	{
		OnCreateLobbyComplete.Broadcast(false);
		return;
	}

	FCreateLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LocalName = LocalLobbyName;
	Params.SchemaId = FName(TEXT("GameLobby"));
	Params.MaxMembers = 18;
	Params.JoinPolicy = ELobbyJoinPolicy::PublicAdvertised;
	Params.Attributes.Emplace(LOBBY_LOBBYNAME, FString(TEXT("Default Name")));
	Params.Attributes.Emplace(LOBBY_GAMEMODE, FString(TEXT("Mutation")));
	Params.Attributes.Emplace(LOBBY_MAPNAME, FString(TEXT("DevMutation")));
	Params.Attributes.Emplace(LOBBY_SESSIONID, FString(TEXT("")));
	Params.Attributes.Emplace(LOBBY_STATUS, static_cast<int64>(0));
	Params.UserAttributes.Emplace(LOBBY_MEMBER_TEAM, static_cast<int64>(FMath::RandRange(1, 2)));
	Params.UserAttributes.Emplace(LOBBY_MEMBER_PLAYERNAME, UserInfo->DisplayName);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_BISREADY, false);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_MSG, FString(TEXT("")));

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
void UEOSSubsystem::FindLobby(FString LobbyName, FString GameMode, FString MapName)
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId) || LobbyPtr == nullptr)
	{
		OnFindLobbyComplete.Broadcast(false, TArray<TSharedRef<const FLobby>>());
		return;
	}

	FFindLobbies::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.MaxResults = 100;
	if (LobbyName != FString(TEXT("")))
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_LOBBYNAME,
			ESchemaAttributeComparisonOp::Near,
			LobbyName
		});
	}
	if (GameMode != FString(TEXT("")) && GameMode != FString(TEXT("All")))
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_GAMEMODE,
			ESchemaAttributeComparisonOp::Equals,
			GameMode
		});
	}
	if (MapName != FString(TEXT("")) && MapName != FString(TEXT("All")))
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_MAPNAME,
			ESchemaAttributeComparisonOp::Equals,
			MapName
		});
	}

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
	if (LobbyPtr == nullptr || AccountInfo == nullptr) return;

	FJoinLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LocalName = LocalLobbyName;
	Params.LobbyId = Lobby->LobbyId;
	Params.UserAttributes.Emplace(LOBBY_MEMBER_TEAM, static_cast<int64>(FMath::RandRange(1, 2)));
	Params.UserAttributes.Emplace(LOBBY_MEMBER_PLAYERNAME, UserInfo->DisplayName);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_BISREADY, false);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_MSG, FString(TEXT("")));

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
	CurrentLobby = LobbyMemberJoined.Lobby;
	OnLobbyMemberJoined.Broadcast(LobbyMemberJoined);
}

void UEOSSubsystem::BroadcastOnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLef)
{
	CurrentLobby = LobbyMemberLef.Lobby;
	OnLobbyMemberLeft.Broadcast(LobbyMemberLef);
}

// 房主提升另一名成员为房主
void UEOSSubsystem::PromoteLobbyMember(FAccountId TargetAccountId)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

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
	CurrentLobby = LobbyLeaderChanged.Lobby;
	OnLobbyLeaderChanged.Broadcast(LobbyLeaderChanged);
}

void UEOSSubsystem::KickLobbyMember(FAccountId TargetAccountId)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FKickLobbyMember::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.TargetAccountId = TargetAccountId;

	LobbyPtr->KickLobbyMember(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FKickLobbyMember>& Result)
	{
		if (Result.IsOk())
		{
			OnKickLobbyMemberComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnKickLobbyMemberComplete.Broadcast(false);
		}
	});
}

// 修改大厅属性
void UEOSSubsystem::ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes,
	TSet<FSchemaAttributeId> RemovedAttributes)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

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
	CurrentLobby = LobbyAttributesChanged.Lobby;
	OnLobbyAttributesChanged.Broadcast(LobbyAttributesChanged);
}

// 修改大厅成员属性
void UEOSSubsystem::ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes,
	TSet<FSchemaAttributeId> RemovedAttributes)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

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
	CurrentLobby = LobbyMemberAttributesChanged.Lobby;
	OnLobbyMemberAttributesChanged.Broadcast(LobbyMemberAttributesChanged);
}

void UEOSSubsystem::BroadcastOnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	CurrentLobby = nullptr;
	OnLobbyLeft.Broadcast(LobbyLeft);
}

// 离开大厅
void UEOSSubsystem::LeaveLobby()
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FLeaveLobby::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
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
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnLeaveLobbyComplete.Broadcast(false);
		}
	});
}

// 创建会话
void UEOSSubsystem::CreateSession()
{
	if (SessionPtr == nullptr || AccountInfo == nullptr) return;

	FCreateSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = LocalSessionName;
	Params.SessionIdOverride = FString(TEXT(""));
	Params.SessionSettings.SchemaName = FName(TEXT("SchemaName"));
	Params.SessionSettings.NumMaxConnections = 18;

	SessionPtr->CreateSession(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FCreateSession>& Result)
	{
		if (Result.IsOk())
		{
			OnCreateSessionComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnCreateSessionComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::AddSessionMember()
{
	if (SessionPtr == nullptr || AccountInfo == nullptr) return;

	FAddSessionMember::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = LocalSessionName;

	SessionPtr->AddSessionMember(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FAddSessionMember>& Result)
	{
		if (Result.IsOk())
		{
			OnAddSessionMemberComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnAddSessionMemberComplete.Broadcast(false);
		}
	});
}

// 获取已创建的会话
TSharedPtr<const ISession> UEOSSubsystem::GetSessionByName()
{
	if (SessionPtr == nullptr) return nullptr;

	FGetSessionByName::Params Params;
	Params.LocalName = LocalSessionName;

	TOnlineResult<FGetSessionByName> Result = SessionPtr->GetSessionByName(MoveTemp(Params));
	if (Result.IsOk())
	{
		 return Result.GetOkValue().Session;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	}
	return nullptr;
}

// 查找会话
void UEOSSubsystem::FindSession()
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId)) return;
	if (SessionPtr == nullptr) return;

	FFindSessions::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.MaxResults = 100;

	SessionPtr->FindSessions(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FFindSessions>& Result)
	{
		if (Result.IsOk())
		{
			OnFindSessionComplete.Broadcast(true, Result.GetOkValue().FoundSessionIds);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
			OnFindSessionComplete.Broadcast(false, TArray<FOnlineSessionId>());
		}
	});
}

// 加入会话
void UEOSSubsystem::JoinSession(FString SessionId)
{
	if (SessionPtr == nullptr || AccountInfo == nullptr) return;

	FJoinSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = LocalSessionName;
	Params.SessionId = ToOnlineSessionId(SessionId);

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

FOnlineSessionId UEOSSubsystem::ToOnlineSessionId(FString SessionId)
{
	TArray<uint8> SessionIdData;
	SessionIdData.SetNum(SessionId.Len());
	memcpy(SessionIdData.GetData(), TCHAR_TO_ANSI(*SessionId), SessionId.Len());
	FOnlineIdRegistryRegistry& Registry = FOnlineIdRegistryRegistry::Get();
	return Registry.ToSessionId(EOnlineServices::Epic, SessionIdData);
}

// 离开会话
void UEOSSubsystem::LeaveSession()
{
	if (SessionPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FLeaveSession::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.SessionName = LocalSessionName;
	Params.bDestroySession = CurrentLobby->OwnerAccountId == AccountInfo->AccountId; // TODO

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

FString UEOSSubsystem::GetResolvedConnectString(FString SessionId)
{
	FString URL = FString(TEXT(""));
	if (OnlineServicesPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return URL;

	FGetResolvedConnectString::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.SessionId = ToOnlineSessionId(SessionId);
	Params.PortType = FName(TEXT("")); // TODO

	TOnlineResult<FGetResolvedConnectString> Result = OnlineServicesPtr->GetResolvedConnectString(MoveTemp(Params));
	if (Result.IsOk())
	{
		URL = Result.GetOkValue().ResolvedConnectString;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Error.GetLogString(): %s"), *Result.GetErrorValue().GetLogString());
	}

	return URL;
}

// 缓存用户文件
void UEOSSubsystem::EnumerateFiles()
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId)) return;
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
	if (UserFilePtr == nullptr || AccountInfo == nullptr) return Filenames;

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
	if (UserFilePtr == nullptr || AccountInfo == nullptr) return;

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
	if (UserFilePtr == nullptr || AccountInfo == nullptr) return;

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
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId)) return;
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
	if (CommercePtr == nullptr || AccountInfo == nullptr) return Offers;

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
	if (CommercePtr == nullptr || AccountInfo == nullptr) return;

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
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId)) return;
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
