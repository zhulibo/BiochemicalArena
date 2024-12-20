#include "EOSSubsystem.h"

#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"

UEOSSubsystem::UEOSSubsystem()
{
	OnlineServicesPtr = GetServices();
	if (OnlineServicesPtr)
	{
		AuthPtr = OnlineServicesPtr->GetAuthInterface();
		UserInfoPtr = OnlineServicesPtr->GetUserInfoInterface();
		LobbyPtr = OnlineServicesPtr->GetLobbiesInterface();
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
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyAttrChanged));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyMemberAttributesChanged().Add(this, &ThisClass::BroadcastOnLobbyMemberAttrChanged));
			OnlineEventDelegateHandles.Add(LobbyPtr->OnLobbyLeft().Add(this, &ThisClass::BroadcastOnLobbyLeft));
		}
		if (CommercePtr)
		{
			OnlineEventDelegateHandles.Add(CommercePtr->OnPurchaseCompleted().Add(this, &ThisClass::BroadcastOnPurchaseCompleted));
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
void UEOSSubsystem::Login(FPlatformUserId TempPlatformUserId, ECoolLoginType LoginType, FString Id, FString Token)
{
	if (AuthPtr == nullptr) return;

	PlatformUserId = TempPlatformUserId;
	GetAccountInfo(PlatformUserId);

	// 已登录
	if (AccountInfo && AuthPtr->IsLoggedIn(AccountInfo->AccountId))
	{
		UE_LOG(LogTemp, Warning, TEXT("Login info exist"));
		GetUserInfo();

		OnLoginComplete.Broadcast(true);
		
		return;
	}

	FAuthLogin::Params Params;
	Params.PlatformUserId = PlatformUserId;
	switch (LoginType)
	{
	case ECoolLoginType::ExchangeCode:
		Params.CredentialsType = LoginCredentialsType::ExchangeCode;
		break;
	case ECoolLoginType::Password:
		Params.CredentialsType = LoginCredentialsType::Password;
		break;
	case ECoolLoginType::DevAuth:
		Params.CredentialsType = LoginCredentialsType::Developer;
		break;
	}
	Params.CredentialsId = Id;
	Params.CredentialsToken.Emplace<FString>(Token);
	
	AuthPtr->Login(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FAuthLogin>& Result)
	{
		if(Result.IsOk())
		{
			AccountInfo = Result.GetOkValue().AccountInfo;

			QueryUserInfo();

			OnLoginComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Login %s"), *Result.GetErrorValue().GetLogString());
			
			OnLoginComplete.Broadcast(false);
		}
	});
}

// 获取账号信息
void UEOSSubsystem::GetAccountInfo(FPlatformUserId Id)
{
	if (AuthPtr == nullptr) return;

	FAuthGetLocalOnlineUserByPlatformUserId::Params Params;
	Params.PlatformUserId = Id;
	TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> Result = AuthPtr->GetLocalOnlineUserByPlatformUserId(MoveTemp(Params));
	if (Result.IsOk())
	{
		AccountInfo = Result.GetOkValue().AccountInfo;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetLocalOnlineUserByPlatformUserId %s"), *Result.GetErrorValue().GetLogString());
	}
}

// 查询用户信息
void UEOSSubsystem::QueryUserInfo()
{
	if (UserInfoPtr == nullptr || AccountInfo == nullptr) return;

	FQueryUserInfo::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.AccountIds.Emplace(AccountInfo->AccountId);

	UserInfoPtr->QueryUserInfo(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FQueryUserInfo>& Result)
	{
		if (Result.IsOk())
		{
			GetUserInfo();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("QueryUserInfo %s"), *Result.GetErrorValue().GetLogString());
		}
	});
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
		UE_LOG(LogTemp, Error, TEXT("GetUserInfo %s"), *Result.GetErrorValue().GetLogString());
	}
}

void UEOSSubsystem::BroadcastOnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged)
{
	OnLoginStatusChanged.Broadcast(AuthLoginStatusChanged);
}

FAccountId UEOSSubsystem::GetAccountId()
{
	if (AccountInfo.IsValid())
	{
		return AccountInfo->AccountId;
	}

	return FAccountId();
}

FString UEOSSubsystem::GetPlayerName()
{
	if (UserInfo == nullptr)
	{
		return FString(TEXT("-1"));
	}

	return UserInfo->DisplayName;
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
	Params.SchemaId = LOBBY_SCHEMA_ID;
	Params.bPresenceEnabled = true;
	Params.MaxMembers = 18;
	Params.JoinPolicy = ELobbyJoinPolicy::PublicAdvertised;
	Params.Attributes.Emplace(LOBBY_SERVER_NAME, FString(TEXT("Default Name")));
	Params.Attributes.Emplace(LOBBY_MODE_NAME, MUTATION);
	Params.Attributes.Emplace(LOBBY_MAP_NAME, FString(TEXT("DevMutation")));
	Params.Attributes.Emplace(LOBBY_STATUS, static_cast<int64>(0));
	Params.UserAttributes.Emplace(LOBBY_MEMBER_NAME, UserInfo->DisplayName);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_TEAM, static_cast<int64>(1));
	Params.UserAttributes.Emplace(LOBBY_MEMBER_READY, false);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_MSG, FString());

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
			UE_LOG(LogTemp, Error, TEXT("CreateLobby %s"), *Result.GetErrorValue().GetLogString());
			OnCreateLobbyComplete.Broadcast(false);
		}
	});
}

// 查找大厅
void UEOSSubsystem::FindLobbies(FString LobbyName, FString GameMode, FString MapName)
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || !AuthPtr->IsLoggedIn(AccountInfo->AccountId) || LobbyPtr == nullptr)
	{
		OnFindLobbiesComplete.Broadcast(false, TArray<TSharedRef<const FLobby>>());
		return;
	}

	FFindLobbies::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.MaxResults = 100;
	if (!LobbyName.IsEmpty())
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_SERVER_NAME,
			ESchemaAttributeComparisonOp::Near,
			LobbyName
		});
	}
	if (GameMode != ALL)
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_MODE_NAME,
			ESchemaAttributeComparisonOp::Equals,
			GameMode
		});
	}
	if (MapName != ALL)
	{
		Params.Filters.Emplace(FFindLobbySearchFilter{
			LOBBY_MAP_NAME,
			ESchemaAttributeComparisonOp::Equals,
			MapName
		});
	}

	LobbyPtr->FindLobbies(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FFindLobbies>& Result)
	{
		if (Result.IsOk())
		{
			OnFindLobbiesComplete.Broadcast(true, Result.GetOkValue().Lobbies);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FindLobbies %s"), *Result.GetErrorValue().GetLogString());
			OnFindLobbiesComplete.Broadcast(false, TArray<TSharedRef<const FLobby>>());
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
	Params.bPresenceEnabled = true;
	Params.UserAttributes.Emplace(LOBBY_MEMBER_TEAM, static_cast<int64>(2)); // TODO 加入人数少的队伍
	Params.UserAttributes.Emplace(LOBBY_MEMBER_NAME, UserInfo->DisplayName);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_READY, false);
	Params.UserAttributes.Emplace(LOBBY_MEMBER_MSG, FString());

	LobbyPtr->JoinLobby(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FJoinLobby>& Result)
	{
		if (Result.IsOk())
		{
			for (auto& Item : Result.GetOkValue().Lobby->Members)
			{
				UE_LOG(LogTemp, Warning, TEXT("JoinLobby Member->Attributes.Num(): %d"), Item.Value->Attributes.Num());
			}

			CurrentLobby = Result.GetOkValue().Lobby;
			OnJoinLobbyComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("JoinLobby %s"), *Result.GetErrorValue().GetLogString());
			OnJoinLobbyComplete.Broadcast(false);
		}
	});
}

bool UEOSSubsystem::GetJoinedLobbies()
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr) return false;

	FGetJoinedLobbies::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	TOnlineResult<FGetJoinedLobbies> Result = LobbyPtr->GetJoinedLobbies(MoveTemp(Params));
	if (Result.IsOk())
	{
		if (Result.GetOkValue().Lobbies.Num() > 0)
		{
			CurrentLobby = Result.GetOkValue().Lobbies[0];
			return true;
		}

		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetJoinedLobbies: %s"), *Result.GetErrorValue().GetLogString());
		return false;
	}
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
			UE_LOG(LogTemp, Error, TEXT("PromoteLobbyMember %s"), *Result.GetErrorValue().GetLogString());
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
			UE_LOG(LogTemp, Error, TEXT("KickLobbyMember %s"), *Result.GetErrorValue().GetLogString());
			OnKickLobbyMemberComplete.Broadcast(false);
		}
	});
}

// 修改大厅属性
void UEOSSubsystem::ModifyLobbyAttr(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttr, TSet<FSchemaAttributeId> RemovedAttr)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FModifyLobbyAttributes::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.UpdatedAttributes = MoveTemp(UpdatedAttr);
	Params.RemovedAttributes = MoveTemp(RemovedAttr);

	LobbyPtr->ModifyLobbyAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyAttributes>& Result)
	{
		if (Result.IsOk())
		{
			OnModifyLobbyAttrComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ModifyLobbyAttr %s"), *Result.GetErrorValue().GetLogString());
			OnModifyLobbyAttrComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyAttrChanged(const FLobbyAttributesChanged& LobbyAttrChanged)
{
	CurrentLobby = LobbyAttrChanged.Lobby;
	OnLobbyAttrChanged.Broadcast(LobbyAttrChanged);
}

// 修改大厅成员属性
void UEOSSubsystem::ModifyLobbyMemberAttr(TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttr, TSet<FSchemaAttributeId> RemovedAttr)
{
	if (LobbyPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FModifyLobbyMemberAttributes::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	Params.UpdatedAttributes = MoveTemp(UpdatedAttr);
	Params.RemovedAttributes = MoveTemp(RemovedAttr);

	LobbyPtr->ModifyLobbyMemberAttributes(MoveTemp(Params))
	.OnComplete([this](const TOnlineResult<FModifyLobbyMemberAttributes>& Result)
	{
		if (Result.IsOk())
		{
			OnModifyLobbyMemberAttrComplete.Broadcast(true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ModifyLobbyMemberAttr %s"), *Result.GetErrorValue().GetLogString());
			OnModifyLobbyMemberAttrComplete.Broadcast(false);
		}
	});
}

void UEOSSubsystem::BroadcastOnLobbyMemberAttrChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttrChanged)
{
	CurrentLobby = LobbyMemberAttrChanged.Lobby;
	OnLobbyMemberAttrChanged.Broadcast(LobbyMemberAttrChanged);
}

void UEOSSubsystem::GetResolvedConnectString(FString& Url)
{
	if (OnlineServicesPtr == nullptr || AccountInfo == nullptr || CurrentLobby == nullptr) return;

	FGetResolvedConnectString::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
	Params.LobbyId = CurrentLobby->LobbyId;
	// Params.SessionId = ;
	Params.PortType = NAME_GamePort;

	TOnlineResult<FGetResolvedConnectString> Result = OnlineServicesPtr->GetResolvedConnectString(MoveTemp(Params));
	if (Result.IsOk())
	{
		Url = Result.GetOkValue().ResolvedConnectString;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetResolvedConnectString %s"), *Result.GetErrorValue().GetLogString());
	}
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
			UE_LOG(LogTemp, Error, TEXT("LeaveLobby %s"), *Result.GetErrorValue().GetLogString());
			OnLeaveLobbyComplete.Broadcast(false);
		}
	});
}

FString UEOSSubsystem::GetLobbyServerName()
{
	if (CurrentLobby == nullptr || CurrentLobby->Attributes.Num() == 0)
	{
		return FString();
	}

	const FSchemaVariant* Attr = CurrentLobby->Attributes.Find(LOBBY_SERVER_NAME);
	if (Attr == nullptr)
	{
		return FString();
	}
	
	return Attr->GetString();
}

FString UEOSSubsystem::GetLobbyModeName()
{
	if (CurrentLobby == nullptr || CurrentLobby->Attributes.Num() == 0)
	{
		return FString();
	}
	
	const FSchemaVariant* Attr = CurrentLobby->Attributes.Find(LOBBY_MODE_NAME);
	if (Attr == nullptr)
	{
		return FString();
	}
	
	return Attr->GetString();
}

FString UEOSSubsystem::GetLobbyMapName()
{
	if (CurrentLobby == nullptr || CurrentLobby->Attributes.Num() == 0)
	{
		return FString();
	}
	
	const FSchemaVariant* Attr = CurrentLobby->Attributes.Find(LOBBY_MAP_NAME);
	if (Attr == nullptr)
	{
		return FString();
	}
	
	return Attr->GetString();
}

int64 UEOSSubsystem::GetLobbyStatus()
{
	if (CurrentLobby == nullptr || CurrentLobby->Attributes.Num() == 0)
	{
		return 0;
	}

	const FSchemaVariant* Attr = CurrentLobby->Attributes.Find(LOBBY_STATUS);
	if (Attr == nullptr)
	{
		return 0;
	}

	return Attr->GetInt64();
}

ETeam UEOSSubsystem::GetMemberTeam(TSharedPtr<const FLobbyMember> Member)
{
	if (Member == nullptr || Member->Attributes.Num() == 0)
	{
		return ETeam::NoTeam;
	}
	
	const FSchemaVariant* Attr = Member->Attributes.Find(LOBBY_MEMBER_TEAM);
	if (Attr == nullptr)
	{
		return ETeam::NoTeam;
	}
	
	return Attr->GetInt64() == 1 ? ETeam::Team1 : ETeam::Team2;
}

bool UEOSSubsystem::GetMemberReady(TSharedPtr<const FLobbyMember> Member)
{
	if (Member == nullptr || Member->Attributes.Num() == 0)
	{
		return false;
	}
	
	const FSchemaVariant* Attr = Member->Attributes.Find(LOBBY_MEMBER_READY);
	if (Attr == nullptr)
	{
		return false;
	}

	return Attr->GetBoolean();
}

FString UEOSSubsystem::GetMemberName(TSharedPtr<const FLobbyMember> Member)
{
	if (Member == nullptr || Member->Attributes.Num() == 0)
	{
		return FString(TEXT("-1"));
	}

	const FSchemaVariant* Attr = Member->Attributes.Find(LOBBY_MEMBER_NAME);
	if (Attr == nullptr)
	{
		return FString(TEXT("-1"));
	}

	return Attr->GetString();
}

TSharedPtr<const FLobbyMember> UEOSSubsystem::GetMember(FAccountId AccountId)
{
	if (CurrentLobby == nullptr || CurrentLobby->Attributes.Num() == 0) return nullptr;
	
	for (const auto& Member : CurrentLobby->Members)
	{
		if (Member.Value->AccountId == AccountId)
		{
			return Member.Value;
		}
	}
	
	return nullptr;
}

FString UEOSSubsystem::GetMemberMsg(TSharedPtr<const FLobbyMember> Member)
{
	if (Member == nullptr || Member->Attributes.Num() == 0)
	{
		return FString();
	}
	
	const FSchemaVariant* Attr = Member->Attributes.Find(LOBBY_MEMBER_MSG);
	if (Attr == nullptr)
	{
		return FString();
	}
	
	return Attr->GetString();
}

bool UEOSSubsystem::IsLobbyHost(TSharedPtr<const FLobbyMember> Member)
{
	if (CurrentLobby == nullptr) return false;

	// 未传Member，判断本地是否为主机
	if (Member == nullptr)
	{
		if (AccountInfo == nullptr) return false;

		return AccountInfo->AccountId == CurrentLobby->OwnerAccountId;
	}

	return Member->AccountId == CurrentLobby->OwnerAccountId;
}

TSharedPtr<const FLobbyMember> UEOSSubsystem::GetLobbyHost()
{
	if (CurrentLobby == nullptr) return nullptr;
	
	for (auto& Member : CurrentLobby->Members)
	{
		if (IsLobbyHost(Member.Value))
		{
			return Member.Value;
		}
	}
	
	return nullptr;
}

bool UEOSSubsystem::IsLocalMember(TSharedPtr<const FLobbyMember> Member)
{
	if (AccountInfo == nullptr) return false;
	
	return Member->AccountId == AccountInfo->AccountId;
}

TSharedPtr<const FLobbyMember> UEOSSubsystem::GetLocalMember()
{
	if (CurrentLobby == nullptr) return nullptr;
	
	for (auto& Member : CurrentLobby->Members)
	{
		if (IsLocalMember(Member.Value))
		{
			return Member.Value;
		}
	}

	return nullptr;
}

// 查询用户文件
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
			UE_LOG(LogTemp, Error, TEXT("EnumerateFiles %s"), *Result.GetErrorValue().GetLogString());
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
		UE_LOG(LogTemp, Error, TEXT("GetEnumeratedFiles %s"), *Result.GetErrorValue().GetLogString());
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
			UE_LOG(LogTemp, Error, TEXT("ReadFile %s"), *Result.GetErrorValue().GetLogString());
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
			UE_LOG(LogTemp, Error, TEXT("WriteFile %s"), *Result.GetErrorValue().GetLogString());
			OnWriteFileComplete.Broadcast(false);
		}
	});
}

// 查询商品列表
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
			UE_LOG(LogTemp, Error, TEXT("QueryOffers %s"), *Result.GetErrorValue().GetLogString());
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
		UE_LOG(LogTemp, Error, TEXT("GetOffers %s"), *Result.GetErrorValue().GetLogString());
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
			UE_LOG(LogTemp, Error, TEXT("Checkout %s"), *Result.GetErrorValue().GetLogString());
			OnCheckoutComplete.Broadcast(false, FString());
		}
	});
}

void UEOSSubsystem::BroadcastOnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
{
	OnPurchaseCompleted.Broadcast(CommerceOnPurchaseComplete);
}

// 查询已购商品
void UEOSSubsystem::QueryEntitlements()
{
	if (AuthPtr == nullptr || AccountInfo == nullptr || CommercePtr == nullptr) return;

	if (!AuthPtr->IsLoggedIn(AccountInfo->AccountId)) return;

	FCommerceQueryEntitlements::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;
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
			UE_LOG(LogTemp, Error, TEXT("QueryEntitlements %s"), *Result.GetErrorValue().GetLogString());
			OnQueryEntitlementsComplete.Broadcast(false);
		}
	});
}

// 获取已购商品
void UEOSSubsystem::GetEntitlements()
{
	if (CommercePtr == nullptr || AccountInfo == nullptr) return;

	FCommerceGetEntitlements::Params Params;
	Params.LocalAccountId = AccountInfo->AccountId;

	TOnlineResult<FCommerceGetEntitlements> Result = CommercePtr->GetEntitlements(MoveTemp(Params));
	if (Result.IsOk())
	{
		// AudienceItemId == ProductId
		Entitlements = Result.GetOkValue().Entitlements;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetEntitlements %s"), *Result.GetErrorValue().GetLogString());
	}
}

// 是否拥有该商品
bool UEOSSubsystem::OwnEntitlement(const FString& Id)
{
	for (int i = 0; i < Entitlements.Num(); ++i)
	{
		if (Entitlements[i].ProductId == Id)
		{
			return true;
		}
	}

	return false;
}
