#include "ServerDetail.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PlayerLineButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/Data/GameModeType.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/UI/Common/CommonComboBox.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "BiochemicalArena/UI/TextChat/TextChat.h"
#include "BiochemicalArena/Utils/LibraryNotify.h"
#include "Components/EditableTextBox.h"

#define LOCTEXT_NAMESPACE "UServerDetail"

void UServerDetail::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ServerNameEditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnServerNameCommitted);
	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);
	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	SwitchTeamButton->OnClicked().AddUObject(this, &ThisClass::OnSwitchTeamButtonClicked);

	ReadyButton->OnClicked().AddUObject(this, &ThisClass::OnReadyButtonClicked);
	JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);
	StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnLobbyMemberJoined.AddUObject(this, &ThisClass::OnLobbyMemberJoined);
		EOSSubsystem->OnLobbyMemberLeft.AddUObject(this, &ThisClass::OnLobbyMemberLeft);
		EOSSubsystem->OnLobbyLeaderChanged.AddUObject(this, &ThisClass::OnLobbyLeaderChanged);
		EOSSubsystem->OnModifyLobbyAttrComplete.AddUObject(this, &ThisClass::OnModifyLobbyAttrComplete);
		EOSSubsystem->OnLobbyAttrChanged.AddUObject(this, &ThisClass::OnLobbyAttrChanged);
		EOSSubsystem->OnModifyLobbyMemberAttrComplete.AddUObject(this, &ThisClass::OnModifyLobbyMemberAttrComplete);
		EOSSubsystem->OnLobbyMemberAttrChanged.AddUObject(this, &ThisClass::OnLobbyMemberAttrChanged);
		EOSSubsystem->OnLobbyLeft.AddUObject(this, &ThisClass::OnLobbyLeft);
		EOSSubsystem->OnLeaveLobbyComplete.AddUObject(this, &ThisClass::OnLeaveLobbyComplete);
	}
}

UWidget* UServerDetail::NativeGetDesiredFocusTarget() const
{
	if (EOSSubsystem)
	{
		if (EOSSubsystem->IsLobbyHost())
		{
			return StartServerButton;
		}
		else
		{
			if (EOSSubsystem->GetLobbyStatus())
			{
				return JoinServerButton;
			}
			else
			{
				return ReadyButton;
			}
		}
	}
	
	return SwitchTeamButton;
}

void UServerDetail::NativeConstruct()
{
	Super::NativeConstruct();
	
	TextChat->MsgContainer->ClearChildren();

	SetUIAttr();
	SetUIButtonState();
	UpdatePlayerList();

	bIsExitingLobby = false;
}

// 设置大厅属性
void UServerDetail::SetUIAttr()
{
	if (EOSSubsystem == nullptr) return;

	ServerNameEditableTextBox->SetText(FText::FromString(EOSSubsystem->GetLobbyServerName()));
	LastServerName = FText::FromString(EOSSubsystem->GetLobbyServerName());

	ModeComboBox->ClearOptions();
	for (int32 i = 0; i < static_cast<int32>(ECoolGameMode::None); ++i)
	{
		FString EnumValue = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(static_cast<ECoolGameMode>(i)));
		ModeComboBox->AddOption(EnumValue);
	}
	ModeComboBox->SetSelectedOption(EOSSubsystem->GetLobbyModeName());

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
		MapComboBox->SetSelectedOption(EOSSubsystem->GetLobbyMapName());
	});
}

// 设置按钮状态
void UServerDetail::SetUIButtonState()
{
	if (EOSSubsystem == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("SetUIButton"));
	if (EOSSubsystem->IsLobbyHost())
	{
		ServerNameEditableTextBox->SetIsReadOnly(false);
		ModeComboBox->SetIsEnabled(true);
		MapComboBox->SetIsEnabled(true);

		StartServerButton->SetIsEnabled(true);
		if (ReadyButton->HasFocusedDescendants() || JoinServerButton->HasFocusedDescendants())
		{
			StartServerButton->SetFocus();
		}

		ReadyButton->SetIsEnabled(false);
		JoinServerButton->SetIsEnabled(false);
	}
	else
	{
		ServerNameEditableTextBox->SetIsReadOnly(true);
		ModeComboBox->SetIsEnabled(false);
		MapComboBox->SetIsEnabled(false);

		JoinServerButton->SetIsEnabled(EOSSubsystem->GetLobbyStatus() != 0);
		ReadyButton->SetIsEnabled(EOSSubsystem->GetLobbyStatus() == 0);
		if (ReadyButton->HasFocusedDescendants() || JoinServerButton->HasFocusedDescendants() || StartServerButton->HasFocusedDescendants())
		{
			if (EOSSubsystem->GetLobbyStatus())
			{
				JoinServerButton->SetFocus();
			}
			else
			{
				ReadyButton->SetFocus();
			}
		}

		StartServerButton->SetIsEnabled(false);
	}
}

// 更新玩家列表
void UServerDetail::UpdatePlayerList()
{
	if (PlayerLineButtonClass == nullptr || Team1Container == nullptr || Team2Container == nullptr
		|| EOSSubsystem == nullptr || EOSSubsystem->CurrentLobby == nullptr
		|| GetWorld()->bIsTearingDown) return;

	// 记录焦点所在位置
	UCommonHierarchicalScrollBox* Container = nullptr;
	int32 FocusWidgetIndex = 0;
	if (Team1Container->HasFocusedDescendants())
	{
		Container = Team1Container;
		FocusWidgetIndex = GetFocusedPlayerIndex(Container);
	}
	else if (Team2Container->HasFocusedDescendants())
	{
		Container = Team2Container;
		FocusWidgetIndex = GetFocusedPlayerIndex(Container);
	}

	// 清空玩家列表
	Team1Container->ClearChildren();
	Team2Container->ClearChildren();

	// TODO Try to use MVVM
	// 重新生成玩家列表
	for (auto& Member : EOSSubsystem->CurrentLobby->Members)
	{
		UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
		if (PlayerLineButton == nullptr) continue;
		
		PlayerLineButton->Member = Member.Value;

		FString PlayerName = EOSSubsystem->GetMemberName(Member.Value);
		PlayerLineButton->PlayerName->SetText(FText::FromString(ULibraryCommon::ObfuscatePlayerName(PlayerName, this)));

		FText Status;
		if (EOSSubsystem->IsLobbyHost(PlayerLineButton->Member))
		{
			Status = LOCTEXT("Host", "Host");
		}
		else
		{
			if (EOSSubsystem->GetMemberReady(Member.Value))
			{
				if (EOSSubsystem->GetLobbyStatus())
				{
					Status = LOCTEXT("Playing", "Playing");
				}
				else
				{
					Status = LOCTEXT("Ready", "Ready");
				}
			}
			else
			{
				Status = FText::FromString("");
			}
		}
		PlayerLineButton->Status->SetText(Status);

		if (EOSSubsystem->GetMemberTeam(Member.Value) == ETeam::Team1)
		{
			Team1Container->AddChild(PlayerLineButton);
		}
		else
		{
			Team2Container->AddChild(PlayerLineButton);
		}
	}

	// 重新设置焦点位置
	if (Container && FocusWidgetIndex != 0)
	{
		int32 MaxIndex = Container->GetChildrenCount() - 1;
		if (FocusWidgetIndex > MaxIndex)
		{
			FocusWidgetIndex = MaxIndex;
		}

		if (UWidget* FocusWidget = Container->GetChildAt(FocusWidgetIndex))
		{
			FocusWidget->SetFocus();
		}
		else
		{
			NativeGetDesiredFocusTarget()->SetFocus();
		}
	}
}

int32 UServerDetail::GetFocusedPlayerIndex(UCommonHierarchicalScrollBox* Container)
{
	for (int i = 0; i < Container->GetChildrenCount(); ++i)
	{
		if (UWidget* FocusWidget = Container->GetChildAt(i))
		{
			if (FocusWidget->HasFocusedDescendants())
			{
				return i;
			}
		}
	}

	return 0;
}

void UServerDetail::InitMapComboBox()
{
	MapComboBox->ClearOptions();

	if (ModeComboBox->GetSelectedOption() == MUTATION)
	{
		for (int32 i = 0; i < static_cast<int32>(EMutationMap::None); ++i)
		{
			FString EnumValue = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(static_cast<EMutationMap>(i)));
			MapComboBox->AddOption(EnumValue);
		}
	}
	else if (ModeComboBox->GetSelectedOption() == MELEE)
	{
		for (int32 i = 0; i < static_cast<int32>(EMeleeMap::None); ++i)
		{
			FString EnumValue = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(static_cast<EMeleeMap>(i)));
			MapComboBox->AddOption(EnumValue);
		}
	}
	else if (ModeComboBox->GetSelectedOption() == TEAM_DEAD_MATCH)
	{
		for (int32 i = 0; i < static_cast<int32>(ETeamDeadMatchMap::None); ++i)
		{
			FString EnumValue = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(static_cast<ETeamDeadMatchMap>(i)));
			MapComboBox->AddOption(EnumValue);
		}
	}
}

// TODO OnLobbyMemberJoined触发两次
// 成员加入大厅事件
void UServerDetail::OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberJoined"));
	UpdatePlayerList();
	
	if (EOSSubsystem)
	{
		TextChat->ShowMsg(
			EMsgType::Join,
			EOSSubsystem->GetMemberTeam(LobbyMemberJoined.Member), 
			EOSSubsystem->GetMemberName(LobbyMemberJoined.Member)
		);
	}
}

void UServerDetail::OnServerNameCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (Text.ToString() == LastServerName.ToString()) return;
	
	LastServerName = Text;

	if (EOSSubsystem && EOSSubsystem->IsLobbyHost())
	{
		EOSSubsystem->ModifyLobbyAttr(TMap<FSchemaAttributeId, FSchemaVariant>{
			{ LOBBY_SERVER_NAME, Text.ToString() },
		});
	}
}

void UServerDetail::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnModeComboBoxChanged, %s"), *SelectedItem);

	InitMapComboBox();

	if (EOSSubsystem && EOSSubsystem->IsLobbyHost())
	{
		// 代码修改Mode，由赋值处处理
		if (SelectionType != ESelectInfo::Direct)
		{
			MapComboBox->SetSelectedIndex(0);
		}
	}
}

void UServerDetail::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMapComboBoxChanged, %s"), *SelectedItem);
	// InitMapComboBox引起OnMapComboBoxChanged时，SelectedItem为空
	if (SelectedItem.IsEmpty()) return;

	if (EOSSubsystem && EOSSubsystem->IsLobbyHost())
	{
		TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes;
		if (ModeComboBox->GetSelectedOption() != EOSSubsystem->GetLobbyModeName())
		{
			UpdatedAttributes.Add(LOBBY_MODE_NAME, ModeComboBox->GetSelectedOption());
		}
		if (SelectedItem != EOSSubsystem->GetLobbyMapName())
		{
			UpdatedAttributes.Add(LOBBY_MAP_NAME, SelectedItem);
		}
		if (UpdatedAttributes.Num() > 0)
		{
			EOSSubsystem->ModifyLobbyAttr(UpdatedAttributes);
		}
	}
}

// 修改大厅属性完成事件
void UServerDetail::OnModifyLobbyAttrComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		NOTIFY(this, C_RED, LOCTEXT("ModifyServerAttrFailed", "Modify server attributes failed!"));
	}
}

// 大厅属性改变事件
void UServerDetail::OnLobbyAttrChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	FString PlayerName = FString();
	ETeam PlayerTeam = ETeam::NoTeam;
	if (EOSSubsystem)
	{
		PlayerName = EOSSubsystem->GetMemberName(EOSSubsystem->GetLobbyHost());
		PlayerTeam = EOSSubsystem->GetMemberTeam(EOSSubsystem->GetLobbyHost());
	}
	
	for (auto& ChangedAttribute : LobbyAttributesChanged.ChangedAttributes)
	{
		if (ChangedAttribute.Key == LOBBY_SERVER_NAME)
		{
			FString ServerName = ChangedAttribute.Value.Value.GetString();
			ServerNameEditableTextBox->SetText(FText::FromString(ServerName));
			LastServerName = FText::FromString(ServerName);
			
			TextChat->ShowMsg(EMsgType::ServerNameChange, PlayerTeam, PlayerName, ServerName);
		}
		else if (ChangedAttribute.Key == LOBBY_MODE_NAME)
		{
			FString ModeName = ChangedAttribute.Value.Value.GetString();
			ModeComboBox->SetSelectedOption(ModeName);

			TextChat->ShowMsg(EMsgType::ModeNameChange, PlayerTeam, PlayerName, ModeName);
		}
		else if (ChangedAttribute.Key == LOBBY_MAP_NAME)
		{
			// mode和map同时改变时，等待InitMapComboBox
			FString MapName = ChangedAttribute.Value.Value.GetString();
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, MapName]() {
				MapComboBox->SetSelectedOption(MapName);
			});
			
			TextChat->ShowMsg(EMsgType::MapNameChange, PlayerTeam, PlayerName, MapName);
		}
		else if (ChangedAttribute.Key == LOBBY_STATUS)
		{
			if (EOSSubsystem && EOSSubsystem->GetLobbyStatus() == 1)
			{
				TextChat->ShowMsg(EMsgType::Start, PlayerTeam, PlayerName);

				SetUIButtonState();
			}

			// TODO
		}
	}
}

void UServerDetail::OnSwitchTeamButtonClicked()
{
	if (EOSSubsystem == nullptr || EOSSubsystem->CurrentLobby == nullptr) return;
	
	for (auto& Member : EOSSubsystem->CurrentLobby->Members)
	{
		if (EOSSubsystem->IsLocalMember(Member.Value))
		{
			int64 Team = EOSSubsystem->GetMemberTeam(Member.Value) == ETeam::Team1 ? 2 : 1;
			EOSSubsystem->ModifyLobbyMemberAttr(TMap<FSchemaAttributeId, FSchemaVariant>{
				{ LOBBY_MEMBER_TEAM, Team }
			});
			break;
		}
	}
}

void UServerDetail::OnReadyButtonClicked()
{
	if (EOSSubsystem == nullptr || EOSSubsystem->CurrentLobby == nullptr) return;

	for (auto& Member : EOSSubsystem->CurrentLobby->Members)
	{
		if (EOSSubsystem->IsLocalMember(Member.Value))
		{
			EOSSubsystem->ModifyLobbyMemberAttr(TMap<FSchemaAttributeId, FSchemaVariant>{
				{ LOBBY_MEMBER_READY, !EOSSubsystem->GetMemberReady(Member.Value)}
			});
			break;
		}
	}
}

// 修改大厅成员属性完成事件
void UServerDetail::OnModifyLobbyMemberAttrComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		NOTIFY(this, C_RED, LOCTEXT("ModifyPlayerAttrFailed", "Modify player attribute failed!"));
	}
}

// 大厅成员属性改变事件
void UServerDetail::OnLobbyMemberAttrChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged)
{
	if (auto LobbyAttribute = LobbyMemberAttributesChanged.ChangedAttributes.Find(LOBBY_MEMBER_MSG))
	{
		if (EOSSubsystem)
		{
			TextChat->ShowMsg(
				EMsgType::Msg,
				EOSSubsystem->GetMemberTeam(LobbyMemberAttributesChanged.Member),
				EOSSubsystem->GetMemberName(LobbyMemberAttributesChanged.Member),
				LobbyAttribute->Value.GetString()
			);
		}
	}
	else
	{
		UpdatePlayerList();
	}
}

void UServerDetail::OnStartServerButtonClicked()
{
	// TODO
	// if (!CanStartServer()) return;

	if (EOSSubsystem && EOSSubsystem->IsLobbyHost())
	{
		EOSSubsystem->ModifyLobbyAttr(TMap<FSchemaAttributeId, FSchemaVariant>{
			{LOBBY_STATUS, static_cast<int64>(1)},
		});
	}

	FString MapPath = "/Game/Maps/Map_" + EOSSubsystem->GetLobbyModeName() + "/" + EOSSubsystem->GetLobbyMapName() + "/" + EOSSubsystem->GetLobbyMapName() + "?listen";

	UE_LOG(LogTemp, Warning, TEXT("ServerTravel ------------------------------------------"));
	GetWorld()->ServerTravel(MapPath, ETravelType::TRAVEL_Absolute);
}

bool UServerDetail::CanStartServer()
{
	if (EOSSubsystem == nullptr || !EOSSubsystem->IsLobbyHost() || EOSSubsystem->CurrentLobby == nullptr) return false;

	if (MapComboBox->GetSelectedOption().IsEmpty())
	{
		NOTIFY(this, C_YELLOW, LOCTEXT("NeedSelectMap", "Please select map!"));
		return false;
	}

	if (ModeComboBox->GetSelectedOption() == MUTATION)
	{
		int32 ReadyPlayerNum = 0;
		for (auto& Member : EOSSubsystem->CurrentLobby->Members)
		{
			if (EOSSubsystem->GetMemberReady(Member.Value) && !EOSSubsystem->IsLobbyHost(Member.Value))
			{
				++ReadyPlayerNum;
			}
		}
		if (ReadyPlayerNum == 0)
		{
			NOTIFY(this, C_YELLOW, LOCTEXT("RequirePlayer", "Require at least 1 player to be ready!"));
			return false;
		}
	}
	else if (ModeComboBox->GetSelectedOption() == TEAM_DEAD_MATCH || ModeComboBox->GetSelectedOption() == MELEE)
	{
		ETeam HostTeam = EOSSubsystem->GetMemberTeam(EOSSubsystem->GetLobbyHost());
		
		if (HostTeam == ETeam::NoTeam)
		{
			NOTIFY(this, C_YELLOW, LOCTEXT("HostTeamNotFound", "Host's team was not found!"));
			return false;
		}

		bool bAnotherTeamHasPlayerReady = false;
		for (auto& Member : EOSSubsystem->CurrentLobby->Members)
		{
			if (EOSSubsystem->GetMemberTeam(Member.Value) != HostTeam && EOSSubsystem->GetMemberReady(Member.Value))
			{
				bAnotherTeamHasPlayerReady = true;
				break;
			}
		}

		if (!bAnotherTeamHasPlayerReady)
		{
			NOTIFY(this, C_YELLOW, FText::Format(LOCTEXT("RequirePlayerAtTeam", "Require at least 1 player to be ready in team {0}!"), FText::AsNumber(HostTeam == ETeam::Team1 ? 1 : 2)));
			return false;
		}
	}

	return true;
}

void UServerDetail::OnJoinServerButtonClicked()
{
	if (EOSSubsystem == nullptr || EOSSubsystem->IsLobbyHost()) return;

	// 如果未准备，把状态置为已准备。
	if (!EOSSubsystem->GetMemberReady(EOSSubsystem->GetLocalMember()))
	{
		OnReadyButtonClicked();
	}

	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		FString Url;
		EOSSubsystem->GetResolvedConnectString(Url);
		UE_LOG(LogTemp, Warning, TEXT("ClientTravel ------------------------------------------"));
		MenuController->ClientTravel(Url, ETravelType::TRAVEL_Absolute);
	}
}

// 大厅房主改变事件
void UServerDetail::OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyLeaderChanged"));
	SetUIButtonState();
	
	UpdatePlayerList();
	
	if (EOSSubsystem)
	{
		TextChat->ShowMsg(
			EMsgType::HostChange,
			EOSSubsystem->GetMemberTeam(LobbyLeaderChanged.Leader),
			EOSSubsystem->GetMemberName(LobbyLeaderChanged.Leader)
		);
	}
}

// 离开大厅
void UServerDetail::OnBackButtonClicked()
{
	if (EOSSubsystem)
	{
		bIsExitingLobby = true;

		EOSSubsystem->LeaveLobby();
	}
}

// 离开大厅完成事件
void UServerDetail::OnLeaveLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
	}
	else
	{
		NOTIFY(this, C_RED, LOCTEXT("LeaveServerFailed", "Leave server failed!"));
	}
}

// 成员离开大厅事件
void UServerDetail::OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft)
{
	UpdatePlayerList();

	if (EOSSubsystem)
	{
		TextChat->ShowMsg(
			EMsgType::Left,
			EOSSubsystem->GetMemberTeam(LobbyMemberLeft.Member),
			EOSSubsystem->GetMemberName(LobbyMemberLeft.Member)
		);
	}
}

// 被踢出大厅事件
void UServerDetail::OnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyLeft"));
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}

	if (!bIsExitingLobby)
	{
		NOTIFY(this, C_WHITE, LOCTEXT("GotKicked", "Got kicked!"));
	}
}

#undef LOCTEXT_NAMESPACE
