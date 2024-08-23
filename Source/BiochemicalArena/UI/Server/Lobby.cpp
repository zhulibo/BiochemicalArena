#include "Lobby.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PlayerLineButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/GameModeType.h"
#include "BiochemicalArena/UI/Common/CommonComboBox.h"
#include "Components/EditableTextBox.h"

void ULobby::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ServerNameEditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnServerNameCommitted);

	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);

	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	SwitchTeamButton->ButtonText->SetText(FText::FromString("Switch"));
	SwitchTeamButton->OnClicked().AddUObject(this, &ThisClass::OnSwitchTeamButtonClicked);

	SendMsgButton->ButtonText->SetText(FText::FromString("Send"));
	SendMsgButton->OnClicked().AddUObject(this, &ThisClass::OnSendMsgButtonClicked);
	MsgEditableTextBox->SetIsEnabled(false); // TODO
	// SendMsgButton->SetIsEnabled(false);

	ReadyButton->ButtonText->SetText(FText::FromString("Ready"));
	ReadyButton->OnClicked().AddUObject(this, &ThisClass::OnReadyButtonClicked);

	JoinServerButton->ButtonText->SetText(FText::FromString("Join"));
	JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);

	StartServerButton->ButtonText->SetText(FText::FromString("Start"));
	StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnLobbyMemberJoined.AddUObject(this, &ThisClass::OnLobbyMemberJoined);
		EOSSubsystem->OnLobbyMemberLeft.AddUObject(this, &ThisClass::OnLobbyMemberLeft);
		EOSSubsystem->OnLobbyLeaderChanged.AddUObject(this, &ThisClass::OnLobbyLeaderChanged);

		EOSSubsystem->OnModifyLobbyAttributesComplete.AddUObject(this, &ThisClass::OnModifyLobbyAttributesComplete);
		EOSSubsystem->OnLobbyAttributesChanged.AddUObject(this, &ThisClass::OnLobbyAttributesChanged);

		EOSSubsystem->OnModifyLobbyMemberAttributesComplete.AddUObject(this, &ThisClass::OnModifyLobbyMemberAttributesComplete);
		EOSSubsystem->OnLobbyMemberAttributesChanged.AddUObject(this, &ThisClass::OnLobbyMemberAttributesChanged);

		EOSSubsystem->OnLobbyLeft.AddUObject(this, &ThisClass::OnLobbyLeft);
		EOSSubsystem->OnLeaveLobbyComplete.AddUObject(this, &ThisClass::OnLeaveLobbyComplete);

		EOSSubsystem->OnCreateSessionComplete.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		EOSSubsystem->OnAddSessionMemberComplete.AddUObject(this, &ThisClass::OnAddSessionMemberComplete);
		EOSSubsystem->OnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessionsComplete);
		EOSSubsystem->OnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSessionComplete);

		EOSSubsystem->OnPromoteLobbyMemberComplete.AddUObject(this, &ThisClass::OnPromoteLobbyMemberComplete);
	}

	AddClientMemberButton->ButtonText->SetText(FText::FromString("AddClientMember"));
	ServerTravelButton->ButtonText->SetText(FText::FromString("ServerTravel"));
	ClientTravelButton->ButtonText->SetText(FText::FromString("ClientTravel"));

	AddClientMemberButton->OnClicked().AddUObject(this, &ThisClass::OnAddClientMemberButtonClicked);
	ServerTravelButton->OnClicked().AddUObject(this, &ThisClass::OnServerTravelButtonClicked);
	ClientTravelButton->OnClicked().AddUObject(this, &ThisClass::OnClientTravelButtonClicked);
}

UWidget* ULobby::NativeGetDesiredFocusTarget() const
{
	if (EOSSubsystem)
	{
		if (EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId)
		{
			return StartServerButton;
		}
		else
		{
			if (EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_SESSIONID)->GetString() == FString(""))
			{
				return ReadyButton;
			}
			else
			{
				return JoinServerButton;
			}
		}
	}
	return SwitchTeamButton;
}

void ULobby::NativeConstruct()
{
	Super::NativeConstruct();

	Team1Container->ClearChildren();
	Team2Container->ClearChildren();

	SetLobbyAttribute();

	RefreshLobbyUI();

	bIsStartingServer = false;
	bIsExitingLobby = false;
}

// 设置大厅属性
void ULobby::SetLobbyAttribute()
{
	FString LobbyName;
	FString GameMode;
	FString MapName;
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem && EOSSubsystem->CurrentLobby)
	{
		LobbyName = EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_LOBBYNAME)->GetString();
		GameMode = EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_GAMEMODE)->GetString();
		MapName = EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_MAPNAME)->GetString();
	}

	ServerNameEditableTextBox->SetText(FText::FromString(LobbyName));
	LastServerName = FText::FromString(LobbyName);

	ModeComboBox->ClearOptions();
	for (int32 j = 0; j < static_cast<int32>(EGameMode2::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<EGameMode2>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		ModeComboBox->AddOption(EnumValue);
	}
	ModeComboBox->SetSelectedOption(GameMode);

	RefreshMapComboBoxOption();
	GetWorld()->GetTimerManager().SetTimerForNextTick([this, MapName]() {
		MapComboBox->SetSelectedOption(MapName);
	});
}

// 更新UI
void ULobby::RefreshLobbyUI()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		if (EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId)
		{
			ServerNameEditableTextBox->SetIsReadOnly(false);
			ModeComboBox->SetIsEnabled(true);
			MapComboBox->SetIsEnabled(true);

			ReadyButton->SetIsEnabled(false);
			JoinServerButton->SetIsEnabled(false);
			StartServerButton->SetIsEnabled(true);
		}
		else
		{
			ServerNameEditableTextBox->SetIsReadOnly(true);
			ModeComboBox->SetIsEnabled(false);
			MapComboBox->SetIsEnabled(false);

			ReadyButton->SetIsEnabled(true);
			JoinServerButton->SetIsEnabled(EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_SESSIONID)->GetString() != FString(""));
			StartServerButton->SetIsEnabled(false);
		}

		NativeGetDesiredFocusTarget()->SetFocus();
	}
}

void ULobby::RefreshMapComboBoxOption()
{
	MapComboBox->ClearOptions();
	if (ModeComboBox->GetSelectedOption() == "Mutation")
	{
		for (int32 j = 0; j < static_cast<int32>(EMutationMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<EMutationMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}
	else if (ModeComboBox->GetSelectedOption() == "TeamDeadMatch")
	{
		for (int32 j = 0; j < static_cast<int32>(ETeamDeadMatchMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<ETeamDeadMatchMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}
}

void ULobby::AddToPlayerList(TSharedRef<const FLobbyMember> Member)
{

}

// 成员加入大厅事件
void ULobby::OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	if (PlayerLineButtonClass && Team1Container && Team2Container)
	{
		UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
		if (PlayerLineButton)
		{
			PlayerLineButton->Member = LobbyMemberJoined.Member;

			if (LobbyMemberJoined.Member->Attributes.Num() > 0) // TODO Attributes为空
			{
				PlayerLineButton->PlayerName->SetText(FText::FromString(LobbyMemberJoined.Member->Attributes.Find(LOBBY_MEMBER_PLAYERNAME)->GetString()));

				if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
				if (EOSSubsystem)
				{
					if (PlayerLineButton && PlayerLineButton->Member->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId)
					{
						PlayerLineButton->Status->SetText(FText::FromString("Leader"));
					}
					else
					{
						PlayerLineButton->Status->SetText(FText::FromString(PlayerLineButton->Member->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean() ? "Ready" : ""));
					}
				}

				if (LobbyMemberJoined.Member->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64() == 1)
				{
					Team1Container->AddChild(PlayerLineButton);
				}
				else
				{
					Team2Container->AddChild(PlayerLineButton);
				}
			}
			else
			{
				PlayerLineButton->PlayerName->SetText(FText::FromString("Default Player Name"));
				Team1Container->AddChild(PlayerLineButton);
			}
		}
	}
}

void ULobby::OnServerNameCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (Text.ToString() == LastServerName.ToString()) return;
	LastServerName = Text;

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
			{ LOBBY_LOBBYNAME, Text.ToString() },
		});
	}
}

void ULobby::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct) return;

	RefreshMapComboBoxOption();
	MapComboBox->SetSelectedIndex(0);

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
			{ LOBBY_GAMEMODE, SelectedItem },
			{ LOBBY_MAPNAME, MapComboBox->GetOptionAtIndex(0) }
		});
	}
}

void ULobby::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct) return;
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
			{ LOBBY_MAPNAME, SelectedItem }
		});
	}
}

// 修改大厅属性完成事件
void ULobby::OnModifyLobbyAttributesComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Modify lobby attributes failed!"), false);
	}
}

// 大厅属性改变事件
void ULobby::OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	for (auto& ChangedAttribute : LobbyAttributesChanged.ChangedAttributes)
	{
		if (ChangedAttribute.Key == LOBBY_LOBBYNAME)
		{
			FString LobbyName = ChangedAttribute.Value.Value.GetString();
			ServerNameEditableTextBox->SetText(FText::FromString(LobbyName));
			LastServerName = FText::FromString(LobbyName);
		}
		else if (ChangedAttribute.Key == LOBBY_GAMEMODE)
		{
			ModeComboBox->SetSelectedOption(ChangedAttribute.Value.Value.GetString());
			RefreshMapComboBoxOption();
		}
		else if (ChangedAttribute.Key == LOBBY_MAPNAME)
		{
			FString MapName = ChangedAttribute.Value.Value.GetString();
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, MapName]() {
				MapComboBox->SetSelectedOption(MapName);
			});
		}
		else if (ChangedAttribute.Key == LOBBY_SESSIONID)
		{
			FString SessionId = ChangedAttribute.Value.Value.GetString();
			if (SessionId == FString("")) return;

			if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
			if (EOSSubsystem == nullptr) return;
			if (EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId) return;

			JoinServerButton->SetIsEnabled(true);

			// TODO 取消注释
			// bool bIsReady = false;
			// for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			// {
			// 	if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
			// 	{
			// 		bIsReady = Member.Value->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean();
			// 	}
			// }
			// if (!bIsReady) return;
			//
			// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
			// if (EOSSubsystem)
			// {
			// 	EOSSubsystem->JoinSession(SessionId);
			// }
		}
	}
}

void ULobby::OnSwitchTeamButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		for (auto& Member : EOSSubsystem->CurrentLobby->Members)
		{
			if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
			{
				int64 Team = Member.Value->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64();
				EOSSubsystem->ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
					{ LOBBY_MEMBER_TEAM, Team == 1 ? static_cast<int64>(2) : static_cast<int64>(1) }
				});
				break;
			}
		}
	}
}

void ULobby::OnReadyButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		for (auto& Member : EOSSubsystem->CurrentLobby->Members)
		{
			if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
			{
				int64 bIsReady = Member.Value->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean();
				EOSSubsystem->ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
					{ LOBBY_MEMBER_BISREADY, !bIsReady }
				});
				break;
			}
		}
	}
}

// 修改大厅成员属性完成事件
void ULobby::OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifyLobbyMemberAttributes Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Modify lobby member attributes failed!"), false);
	}
}

// 大厅成员属性改变事件
void ULobby::OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged)
{
	for (auto& ChangedAttribute : LobbyMemberAttributesChanged.ChangedAttributes)
	{
		if (ChangedAttribute.Key == LOBBY_MEMBER_TEAM)
		{
			int64 Team = ChangedAttribute.Value.Value.GetInt64();
			if (Team1Container && Team2Container)
			{
				if (Team == 1)
				{
					for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
					{
						UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
						if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberAttributesChanged.Member->AccountId)
						{
							Team2Container->RemoveChild(PlayerLineButton);
							PlayerLineButton->Member = LobbyMemberAttributesChanged.Member;
							Team1Container->AddChild(PlayerLineButton);
							break;
						}
					}
				}
				else if (Team == 2)
				{
					for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
					{
						UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
						if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberAttributesChanged.Member->AccountId)
						{
							Team1Container->RemoveChild(PlayerLineButton);
							PlayerLineButton->Member = LobbyMemberAttributesChanged.Member;
							Team2Container->AddChild(PlayerLineButton);
							break;
						}
					}
				}
			}
		}
		else if (ChangedAttribute.Key == LOBBY_MEMBER_BISREADY)
		{
			bool bIsReady = ChangedAttribute.Value.Value.GetBoolean();
			int64 Team = LobbyMemberAttributesChanged.Member->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64();

			if (Team1Container && Team2Container)
			{
				if (Team == 1)
				{
					for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
					{
						UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
						if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberAttributesChanged.Member->AccountId)
						{
							PlayerLineButton->Member = LobbyMemberAttributesChanged.Member;
							PlayerLineButton->Status->SetText(FText::FromString(bIsReady ? "Ready" : ""));
							break;
						}
					}
				}
				else if (Team == 2)
				{
					for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
					{
						UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
						if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberAttributesChanged.Member->AccountId)
						{
							PlayerLineButton->Member = LobbyMemberAttributesChanged.Member;
							PlayerLineButton->Status->SetText(FText::FromString(bIsReady ? "Ready" : ""));
							break;
						}
					}
				}
			}
		}
		else if (ChangedAttribute.Key == LOBBY_MEMBER_MSG)
		{
			// TODO
		}
	}
}

void ULobby::OnStartServerButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->CreateSession();
		return;

		if (ModeComboBox->GetSelectedOption() == "Mutation")
		{
			int32 NumberOfPlayerHasReady = 0;
			for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			{
				if (Member.Value->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean()
					&& Member.Value->AccountId != EOSSubsystem->CurrentLobby->OwnerAccountId)
				{
					++NumberOfPlayerHasReady;
				}
			}
			if (NumberOfPlayerHasReady == 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MAIN, TEXT("Requires at least 1 player to be ready"), false);
				return;
			}
		}
		else if (ModeComboBox->GetSelectedOption() == "TeamDeadMatch")
		{
			int64 TeamOfLeader = 0;
			for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			{
				if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
				{
					TeamOfLeader = Member.Value->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64();
					break;
				}
			}
			if (TeamOfLeader == 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MAIN, TEXT("Leader's team was not found!"), false);
				return;
			}

			bool bAnotherTeamHasPlayerReady = false;
			for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			{
				if (Member.Value->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64() == (TeamOfLeader == 1 ? 2 : 1)
					&& Member.Value->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean())
				{
					bAnotherTeamHasPlayerReady = true;
					break;
				}
			}
			if (!bAnotherTeamHasPlayerReady)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MAIN, TeamOfLeader == 1 ?
					TEXT("Requires at least 1 player to be ready in team 2!") :
					TEXT("Requires at least 1 player to be ready in team 1!"), false);
				return;
			}
		}

		EOSSubsystem->CreateSession();
	}

}

void ULobby::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			EOSSubsystem->AddSessionMember(EOSSubsystem->AccountInfo->AccountId);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Create session failed!"), false);
	}
}

void ULobby::OnAddSessionMemberComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnAddSessionMemberComplete-----------------------------------"));
	if (bWasSuccessful)
	{
		if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem == nullptr) return;

		if (bIsAddingClientMember)
		{
			UE_LOG(LogTemp, Warning, TEXT("bIsAddingClientMember-----------------------------------"));
			return;
		}

		if (TSharedPtr<const ISession> Session = EOSSubsystem->GetSessionByName())
		{
			EOSSubsystem->ModifyLobbyAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
				{ LOBBY_SESSIONID, ToString(Session->GetSessionId()) },
			});

			// GetWorld()->ServerTravel("/Game/Maps/Dev_TeamDeadMatch?listen", ETravelType::TRAVEL_Absolute);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("GetSessionByName failed!"), false);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("AddSessionMember failed!"), false);
	}

}

void ULobby::OnJoinServerButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		// TODO Don't know why JoinSession may fail sometimes, maybe there is a problem with ToOnlineSessionId
		// OnlineSessionId = ToOnlineSessionId(EOSSubsystem->CurrentLobby->Attributes.Find(LOBBY_SESSIONID)->GetString());
		// EOSSubsystem->JoinSession(OnlineSessionId);

		EOSSubsystem->FindSessions();
	}
}

FOnlineSessionId ULobby::ToOnlineSessionId(FString SessionId)
{
	TArray<uint8> SessionIdData;
	SessionIdData.SetNum(SessionId.Len());
	memcpy(SessionIdData.GetData(), TCHAR_TO_ANSI(*SessionId), SessionId.Len());
	FOnlineIdRegistryRegistry& Registry = FOnlineIdRegistryRegistry::Get();
	return Registry.ToSessionId(EOnlineServices::Epic, SessionIdData);
}

void ULobby::OnFindSessionsComplete(bool bWasSuccessful, const TArray<FOnlineSessionId>& FoundSessionIds)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete-------------------------------------------------------"));
	if (bWasSuccessful)
	{
		if (FoundSessionIds.Num() != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("FoundSessionIds != 1"));
			return;
		}

		if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			OnlineSessionId = FoundSessionIds[0];
			EOSSubsystem->JoinSession(OnlineSessionId);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Find session failed!"), false);
	}
}

// TODO AddSessionMember
void ULobby::OnJoinSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete-------------------------------------------------------"));
	if (bWasSuccessful)
	{
		// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		// if (EOSSubsystem)
		// {
		// 	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		// 	if (MenuController)
		// 	{
		// 		FString URL = EOSSubsystem->GetResolvedConnectString(OnlineSessionId);
		// 		UE_LOG(LogTemp, Warning, TEXT("GetResolvedConnectString: %s"), *URL);
		// 		MenuController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
		// 	}
		// }
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Join session failed!"), false);
	}
}

// TODO 测试房主掉线 会不会自动转移房主
void ULobby::OnPromoteLobbyMemberComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem && bIsExitingLobby)
		{
			EOSSubsystem->LeaveLobby();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Promote lobby member failed!"), false);
	}
}

// 大厅房主改变事件
void ULobby::OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	RefreshLobbyUI();

	if (Team1Container && Team2Container)
	{
		for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyLeaderChanged.Leader->AccountId)
			{
				PlayerLineButton->Status->SetText(FText::FromString("Leader"));
			}
			else
			{
				// TODO Attributes为空
				if (PlayerLineButton->Member->Attributes.Num() > 0)
				{
					PlayerLineButton->Status->SetText(FText::FromString(PlayerLineButton->Member->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean() ? "Ready" : ""));
				}
				else
				{
					PlayerLineButton->Status->SetText(FText::FromString("-1"));
				}
			}
		}

		for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyLeaderChanged.Leader->AccountId)
			{
				PlayerLineButton->Status->SetText(FText::FromString("Leader"));
			}
			else
			{
				// TODO Attributes为空
				if (PlayerLineButton->Member->Attributes.Num() > 0)
				{
					PlayerLineButton->Status->SetText(FText::FromString(PlayerLineButton->Member->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean() ? "Ready" : ""));
				}
				else
				{
					PlayerLineButton->Status->SetText(FText::FromString("-1"));
				}
			}
		}
	}
}

// 离开大厅
void ULobby::OnBackButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->LeaveSession();
		bIsExitingLobby = true;

		if (EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId
			&& EOSSubsystem->CurrentLobby->Members.Num() > 1)
		{
			for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			{
				if (Member.Value->AccountId != EOSSubsystem->AccountInfo->AccountId)
				{
					EOSSubsystem->PromoteLobbyMember(Member.Value->AccountId);
					return;
				}
			}
		}

		EOSSubsystem->LeaveLobby();
	}
}

// 离开大厅完成事件
void ULobby::OnLeaveLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_HUMAN, TEXT("Leave lobby failed!"), false);
	}
}

// 成员离开大厅事件
void ULobby::OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft)
{
	if (Team1Container && Team2Container)
	{
		if (LobbyMemberLeft.Member->Attributes.Num() > 0) // TODO Attributes为空
		{
			int64 Team = LobbyMemberLeft.Member->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64();
			if (Team == 1)
			{
				for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
				{
					UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
					if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberLeft.Member->AccountId)
					{
						Team1Container->RemoveChild(PlayerLineButton);
						break;
					}
				}
			}
			else if (Team == 2)
			{
				for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
				{
					UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
					if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberLeft.Member->AccountId)
					{
						Team2Container->RemoveChild(PlayerLineButton);
						break;
					}
				}
			}
		}
		else
		{
			for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
			{
				UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
				if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberLeft.Member->AccountId)
				{
					Team1Container->RemoveChild(PlayerLineButton);
					return;
				}
			}
			for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
			{
				UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
				if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberLeft.Member->AccountId)
				{
					Team2Container->RemoveChild(PlayerLineButton);
					return;
				}
			}
		}

	}
}

// 被踢出大厅事件
void ULobby::OnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyLeft"));
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}

	if (!bIsExitingLobby)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, COLOR_MAIN, TEXT("Got kicked!"), false);
	}
}

void ULobby::OnSendMsgButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSendMsgButtonClicked"));

}

void ULobby::OnAddClientMemberButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		for (auto& Member : EOSSubsystem->CurrentLobby->Members)
		{
			if (Member.Value->AccountId != EOSSubsystem->AccountInfo->AccountId)
			{
				UE_LOG(LogTemp, Warning, TEXT("AddSessionMember---------------------------------------------"));
				bIsAddingClientMember = true;
				EOSSubsystem->AddSessionMember(Member.Value->AccountId);

				break;
			}
		}
	}
}

void ULobby::OnServerTravelButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerTravel---------------------------------------------------------"));
	GetWorld()->ServerTravel("/Game/Maps/Dev_TeamDeadMatch?listen", ETravelType::TRAVEL_Absolute);
}

void ULobby::OnClientTravelButtonClicked()
{
	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			FString URL = EOSSubsystem->GetResolvedConnectString(OnlineSessionId);
			UE_LOG(LogTemp, Warning, TEXT("ClientTravel---------------------------------------------------------"));
			UE_LOG(LogTemp, Warning, TEXT("GetResolvedConnectString: %s"), *URL);
			MenuController->ClientTravel(URL, ETravelType::TRAVEL_Absolute);
		}
	}
}
