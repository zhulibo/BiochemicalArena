#include "ServerDetail.h"

#include <string>

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PlayerLineButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "Connect.h"
#include "Lobby.h"
#include "ServiceManager.h"
#include "Session.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/GameModeType.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/Common/CommonComboBox.h"
#include "Components/EditableTextBox.h"

void UServerDetail::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ServerNameEditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnServerNameCommitted);
	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);
	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	SwitchTeamButton->OnClicked().AddUObject(this, &ThisClass::OnSwitchTeamButtonClicked);

	SendMsgButton->ButtonText->SetText(FText::FromString(TEXT("Send")));
	SendMsgButton->OnClicked().AddUObject(this, &ThisClass::OnSendMsgButtonClicked);

	ReadyButton->OnClicked().AddUObject(this, &ThisClass::OnReadyButtonClicked);
	JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);
	StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);

	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	// EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	EOSSubsystem->OnModifyLobbyMemberAttributesComplete.AddUObject(this, &ThisClass::OnModifyLobbyMemberAttributesComplete);
	//
	// 	EOSSubsystem->OnLobbyLeft.AddUObject(this, &ThisClass::OnLobbyLeft);
	//
	// 	EOSSubsystem->OnPromoteLobbyMemberComplete.AddUObject(this, &ThisClass::OnPromoteLobbyMemberComplete);
	// }

	ServiceManager = UServiceManager::GetServiceManager();
	if (ServiceManager)
	{
		Auth = ServiceManager->GetAuth();
	
		Connect = ServiceManager->GetConnect();
	
		Lobby = ServiceManager->GetLobby();
		if (Lobby)
		{
			if (!Lobby->OnUpdateLobbyComplete.IsBoundToObject(this)) Lobby->OnUpdateLobbyComplete.AddUObject(this, &ThisClass::OnUpdateLobbyComplete);
			if (!Lobby->OnLobbyUpdateReceived.IsBoundToObject(this)) Lobby->OnLobbyUpdateReceived.AddUObject(this, &ThisClass::OnLobbyUpdateReceived);
			if (!Lobby->OnMemberStatusReceived.IsBoundToObject(this)) Lobby->OnMemberStatusReceived.AddUObject(this, &ThisClass::OnMemberStatusReceived);
			if (!Lobby->OnLeaveLobbyComplete.IsBoundToObject(this)) Lobby->OnLeaveLobbyComplete.AddUObject(this, &ThisClass::OnLeaveLobbyComplete);
			if (!Lobby->OnDestroyLobbyComplete.IsBoundToObject(this)) Lobby->OnDestroyLobbyComplete.AddUObject(this, &ThisClass::OnDestroyLobbyComplete);
		}
		
		Session = ServiceManager->GetSession();
		if (Session)
		{
			if (!Session->OnCreateSessionComplete.IsBoundToObject(this)) Session->OnCreateSessionComplete.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		}
	}
}

UWidget* UServerDetail::NativeGetDesiredFocusTarget() const
{
	if (Lobby && Connect)
	{
		if (Lobby->CurLobby->LobbyOwnerUserId == Connect->ProductUserId)
		{
			return StartServerButton;
		}

		if (Lobby->CurLobby->GetStringAttr(LOBBY_SESSIONID).IsEmpty())
		{
			return ReadyButton;
		}

		return JoinServerButton;
	}

	return SwitchTeamButton;
}

void UServerDetail::NativeConstruct()
{
	Super::NativeConstruct();

	SetLobbyUIAttr();
	
	SetLobbyUIButton();
	
	RefreshPlayerList();
}

void UServerDetail::SetLobbyUIAttr()
{
	if (Lobby == nullptr || Lobby->CurLobby == nullptr) return;

	FString ServerName = Lobby->CurLobby->GetStringAttr(LOBBY_SERVERNAME);
	ServerNameEditableTextBox->SetText(FText::FromString(ServerName));
	LastServerName = FText::FromString(ServerName);

	FString ModeName = Lobby->CurLobby->GetStringAttr(LOBBY_MODENAME);
	ModeComboBox->ClearOptions();
	for (int32 j = 0; j < static_cast<int32>(ECoolGameMode::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<ECoolGameMode>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		ModeComboBox->AddOption(EnumValue);
	}
	if (!ModeName.IsEmpty())
	{
		ModeComboBox->SetSelectedOption(ModeName);
	}

	InitMapComboBox();

	FString MapName = Lobby->CurLobby->GetStringAttr(LOBBY_MAPNAME);
	if (!MapName.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, MapName]() {
			MapComboBox->SetSelectedOption(MapName);
		});
	}
}

void UServerDetail::InitMapComboBox()
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

void UServerDetail::SetLobbyUIButton()
{
	if (Lobby && Connect)
	{
		if (Lobby->CurLobby->LobbyOwnerUserId == Connect->ProductUserId)
		{
			ServerNameEditableTextBox->SetIsReadOnly(false);
			ModeComboBox->SetIsEnabled(true);
			MapComboBox->SetIsEnabled(true);

			ReadyButton->SetIsEnabled(false);
			// JoinServerButton->SetIsEnabled(false);
			StartServerButton->SetIsEnabled(true);
		}
		else
		{
			ServerNameEditableTextBox->SetIsReadOnly(true);
			ModeComboBox->SetIsEnabled(false);
			MapComboBox->SetIsEnabled(false);

			ReadyButton->SetIsEnabled(true);
			// JoinServerButton->SetIsEnabled(!Lobby->CurLobby->GetStringAttr(LOBBY_SESSIONID).IsEmpty());
			StartServerButton->SetIsEnabled(false);
		}

		NativeGetDesiredFocusTarget()->SetFocus();
	}
}

void UServerDetail::RefreshPlayerList()
{
	if (PlayerLineButtonClass == nullptr) return;

	Team1Container->ClearChildren();
	Team2Container->ClearChildren();

	for (auto Member : Lobby->CurLobby->Members)
	{
		UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
		if (PlayerLineButton)
		{
			PlayerLineButton->Member = Member;

			if (UServiceManager::GetLobby()->IsOwner())
			{
				PlayerLineButton->Status->SetText(FText::FromString(TEXT("Leader")));
			}
			else
			{
				PlayerLineButton->Status->SetText(FText::FromString(UServiceManager::GetLobby()->CurLobby->GetBoolAttr(LOBBY_MEMBER_BISREADY) ? TEXT("Ready") : TEXT("")));
			}

			Team1Container->AddChild(PlayerLineButton);
		}
	}
}

void UServerDetail::OnMemberStatusReceived(EOS_ProductUserId TargetUserId, EOS_ELobbyMemberStatus CurrentStatus)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMemberStatusReceived: %d"), CurrentStatus);

	if (Connect == nullptr) return;

	if (TargetUserId == Connect->ProductUserId)
	{
		if (CurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_PROMOTED)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_GREEN, TEXT("You Have been promoted to the leader!"), false);

			RefreshPlayerList();
		}
		else if (CurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_KICKED)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Got kick!"), false);

			DeactivateWidget();
		}
		else if (CurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_CLOSED)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Lobby has been destroyed!"), false);

			DeactivateWidget();
		}
		else if (CurrentStatus == EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Disconnected with lobby!"), false);

			DeactivateWidget();
		}
	}
	else
	{
		RefreshPlayerList();
	}
}

void UServerDetail::OnServerNameCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (Text.ToString() == LastServerName.ToString()) return;

	LastServerName = Text;

	if (Lobby)
	{
		std::string TempKey = TCHAR_TO_UTF8(*LOBBY_SERVERNAME.ToString());
		std::string TempValue = TCHAR_TO_UTF8(*Text.ToString());

		EOS_Lobby_AttributeData Attr;
		Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
		Attr.Key = TempKey.c_str();
		Attr.Value.AsUtf8 = TempValue.c_str();
		Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;

		Lobby->UpdateLobby(Attr);
	}
}

void UServerDetail::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnModeComboBoxChanged: %s %d"), *SelectedItem, SelectionType);

	if (SelectionType == ESelectInfo::Direct) return;

	InitMapComboBox();

	if (Lobby)
	{
		std::string TempKey = TCHAR_TO_UTF8(*LOBBY_MODENAME.ToString());
		std::string TempValue = TCHAR_TO_UTF8(*SelectedItem);

		EOS_Lobby_AttributeData Attr;
		Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
		Attr.Key = TempKey.c_str();
		Attr.Value.AsUtf8 = TempValue.c_str();
		Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;

		Lobby->UpdateLobby(Attr);
	}
}

void UServerDetail::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMapComboBoxChanged: %s %d"), *SelectedItem, SelectionType);

	if (SelectionType == ESelectInfo::Direct) return;

	if (Lobby)
	{
		std::string TempKey = TCHAR_TO_UTF8(*LOBBY_MAPNAME.ToString());
		std::string TempValue = TCHAR_TO_UTF8(*SelectedItem);

		EOS_Lobby_AttributeData Attr;
		Attr.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
		Attr.Key = TempKey.c_str();
		Attr.Value.AsUtf8 = TempValue.c_str();
		Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;

		Lobby->UpdateLobby(Attr);
	}
}

// 修改大厅属性完成事件
void UServerDetail::OnUpdateLobbyComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnUpdateLobbyComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Modify lobby attributes failed!"), false);
	}
}

// 大厅属性改变事件
void UServerDetail::OnLobbyUpdateReceived()
{
	if (Lobby == nullptr) return;

	FString ServerName = Lobby->CurLobby->GetStringAttr(LOBBY_SERVERNAME);
	FString ModeName = Lobby->CurLobby->GetStringAttr(LOBBY_MODENAME);
	FString MapName = Lobby->CurLobby->GetStringAttr(LOBBY_MAPNAME);

	ServerNameEditableTextBox->SetText(FText::FromString(ServerName));
	LastServerName = FText::FromString(ServerName);

	ModeComboBox->SetSelectedOption(ModeName);
	InitMapComboBox();

	GetWorld()->GetTimerManager().SetTimerForNextTick([this, MapName]() {
		MapComboBox->SetSelectedOption(MapName);
	});
}

void UServerDetail::OnSwitchTeamButtonClicked()
{
	// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	for (auto& Member : EOSSubsystem->CurrentLobby->Members)
	// 	{
	// 		if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
	// 		{
	// 			int64 Team = Member.Value->Attributes.Find(LOBBY_MEMBER_TEAM)->GetInt64();
	// 			EOSSubsystem->ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
	// 				{ LOBBY_MEMBER_TEAM, Team == 1 ? static_cast<int64>(2) : static_cast<int64>(1) }
	// 			});
	// 			break;
	// 		}
	// 	}
	// }
	
	if (Session)
	{
		Session->CreateSession();
	}
}

void UServerDetail::OnReadyButtonClicked()
{
	// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	for (auto& Member : EOSSubsystem->CurrentLobby->Members)
	// 	{
	// 		if (Member.Value->AccountId == EOSSubsystem->AccountInfo->AccountId)
	// 		{
	// 			int64 bIsReady = Member.Value->Attributes.Find(LOBBY_MEMBER_BISREADY)->GetBoolean();
	// 			EOSSubsystem->ModifyLobbyMemberAttributes(TMap<FSchemaAttributeId, FSchemaVariant>{
	// 				{ LOBBY_MEMBER_BISREADY, !bIsReady }
	// 			});
	// 			break;
	// 		}
	// 	}
	// }

	if (Session)
	{
		Session->GetSessionDetail();
	}
}

void UServerDetail::OnCreateSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Create session failed!"), false);
	}
}

void UServerDetail::OnStartServerButtonClicked()
{
	GetWorld()->ServerTravel("/Game/Maps/Dev_TeamDeadMatch?listen", ETravelType::TRAVEL_Absolute);

	// if (CanStartServer())
	// {
	// 	GetWorld()->ServerTravel("/Game/Maps/Dev_TeamDeadMatch?listen", ETravelType::TRAVEL_Absolute);
	// }
}

bool UServerDetail::CanStartServer()
{
	if (Lobby == nullptr) return false;

	if (ModeComboBox->GetSelectedOption() == "")
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Please select Mode!"), false);

		return false;
	}
	else if (ModeComboBox->GetSelectedOption() == "Mutation")
	{
		int32 ReadyPlayerNum = 0;

		for (auto& Member : Lobby->CurLobby->Members)
		{
			if (Member.GetBoolAttr(LOBBY_MEMBER_BISREADY) && Lobby->IsOwner(Member.ProductUserId))
			{
				++ReadyPlayerNum;
			}
		}

		if (ReadyPlayerNum == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Requires at least 1 player to be ready!"), false);

			return false;
		}
	}
	else if (ModeComboBox->GetSelectedOption() == "TeamDeadMatch")
	{
		ETeam TeamOfLeader = ETeam::NoTeam;

		for (auto& Member : Lobby->CurLobby->Members)
		{
			if (Lobby->IsOwner(Member.ProductUserId))
			{
				TeamOfLeader = Member.GetIntAttr(LOBBY_MEMBER_TEAM) == 1 ? ETeam::Team1 : ETeam::Team2;
				break;
			}
		}

		if (TeamOfLeader == ETeam::NoTeam)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Leader's team was not found!"), false);

			return false;
		}

		bool bAnotherTeamHasPlayerReady = false;

		int32 AnotherTeamNum = TeamOfLeader == ETeam::Team1 ? 1 : 2;
		for (auto& Member : Lobby->CurLobby->Members)
		{
			if (Member.GetIntAttr(LOBBY_MEMBER_TEAM) == AnotherTeamNum && Member.GetBoolAttr(LOBBY_MEMBER_BISREADY))
			{
				bAnotherTeamHasPlayerReady = true;
				break;
			}
		}

		if (!bAnotherTeamHasPlayerReady)
		{
			FString Msg = FString::Printf(TEXT("Requires at least 1 player to be ready in team %s!"), AnotherTeamNum == 1 ? TEXT("1") : TEXT("2"));
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, Msg, false);

			return false;
		}
	}

	if (MapComboBox->GetSelectedOption() == "")
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Please select Map!"), false);

		return false;
	}

	return true;
}

void UServerDetail::OnJoinServerButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClientTravel---------------------------------------------------------"));

		// TODO Need implement p2p, see SocketSubsystemEOS plugin
		MenuController->ClientTravel("EOS:0002facb7b3f43a58ea43cf91b88d1ad:GameNetDriver:26", ETravelType::TRAVEL_Absolute);
	}
}

// 离开大厅
void UServerDetail::OnBackButtonClicked()
{
	if (Lobby == nullptr) return;

	if (UServiceManager::GetLobby()->IsOwner() && UServiceManager::GetLobby()->CurLobby->Members.Num() <= 1)
	{
		Lobby->DestroyLobby();
	}
	else
	{
		Lobby->LeaveLobby();
	}
}

// 离开大厅完成事件
void UServerDetail::OnLeaveLobbyComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLeaveLobbyComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
		DeactivateWidget();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Leave lobby failed!"), false);
	}
}

// 销毁大厅完成事件
void UServerDetail::OnDestroyLobbyComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDestroyLobbyComplete %d"), bWasSuccessful);

	if (bWasSuccessful)
	{
		DeactivateWidget();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Destroy lobby failed!"), false);
	}
}

void UServerDetail::OnSendMsgButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSendMsgButtonClicked"));
}
