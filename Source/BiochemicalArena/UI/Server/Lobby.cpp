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

	ServerNameEditableTextBox->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnServerNameChanged);

	for (int32 j = 0; j < static_cast<int32>(EGameMode::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<EGameMode>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		ModeComboBox->AddOption(EnumValue);
	}
	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);

	for (int32 j = 0; j < static_cast<int32>(EMutationMap::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<EMutationMap>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		MapComboBox->AddOption(EnumValue);
	}
	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	SwitchTeamButton->ButtonText->SetText(FText::FromString("Switch"));
	SwitchTeamButton->OnClicked().AddUObject(this, &ThisClass::OnSwitchTeamButtonClicked);

	SendMsgButton->ButtonText->SetText(FText::FromString("Send"));
	SendMsgButton->OnClicked().AddUObject(this, &ThisClass::OnSendMsgButtonClicked);

	ReadyButton->ButtonText->SetText(FText::FromString("Ready"));
	ReadyButton->OnClicked().AddUObject(this, &ThisClass::OnReadyButtonClicked);

	JoinServerButton->ButtonText->SetText(FText::FromString("Join"));
	JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);

	StartServerButton->ButtonText->SetText(FText::FromString("Start"));
	StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
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

		EOSSubsystem->OnPromoteLobbyMemberComplete.AddUObject(this, &ThisClass::OnPromoteLobbyMemberComplete);
	}
}

UWidget* ULobby::NativeGetDesiredFocusTarget() const
{
	return SwitchTeamButton; // TODO
}

void ULobby::OnCreateInit()
{
	ServerNameEditableTextBox->SetText(FText::FromString("Default Server Name"));
	ModeComboBox->SetSelectedIndex(0);
	MapComboBox->SetSelectedIndex(0);

	InitPlayerList();
}

void ULobby::OnJoinInit()
{
	// TODO
	ServerNameEditableTextBox->SetText(FText::FromString("Default Server Name"));
	ModeComboBox->SetSelectedIndex(0);
	MapComboBox->SetSelectedIndex(0);

	ServerNameEditableTextBox->SetIsReadOnly(true);
	ModeComboBox->SetIsEnabled(false);
	MapComboBox->SetIsEnabled(false);

	InitPlayerList();
}

// 初始化玩家列表
void ULobby::InitPlayerList()
{
	Team1Container->ClearChildren();
	Team2Container->ClearChildren();

	// for (int32 i = 0; i < 9; ++i)
	// {
	// 	UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
	// 	if (PlayerLineButton) Team1Container->AddChild(PlayerLineButton);
	// }
	//
	// for (int32 i = 0; i < 9; ++i)
	// {
	// 	UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
	// 	if (PlayerLineButton) Team2Container->AddChild(PlayerLineButton);
	// }

	if (EOSSubsystem && EOSSubsystem->CurrentLobby)
	{
		for (auto& Item : EOSSubsystem->CurrentLobby->Members)
		{
			AddToPlayerList(Item.Value);
		}
	}
}

void ULobby::AddToPlayerList(TSharedRef<const FLobbyMember> Member)
{
	if (PlayerLineButtonClass && Team1Container && Team2Container)
	{
		UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
		if (PlayerLineButton)
		{
			PlayerLineButton->Member = Member;

			if (Member->Attributes.Num() > 0)
			{
				PlayerLineButton->PlayerName->SetText(FText::FromString(Member->Attributes.Find(FName(TEXT("PlayerName")))->GetString()));

				if (Member->Attributes.Find(FName(TEXT("Team")))->GetInt64() == 1)
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
				UE_LOG(LogTemp, Warning, TEXT("AddToPlayerList Member->Attributes is empty"));

				PlayerLineButton->PlayerName->SetText(FText::FromString("Default Player Name"));
				Team1Container->AddChild(PlayerLineButton);
			}
		}
	}
}

// 成员加入大厅事件
void ULobby::OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	// TODO OnLobbyMemberJoined不可用
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberJoined"));

	AddToPlayerList(LobbyMemberJoined.Member);
}

void ULobby::OnServerNameChanged(const FText& Text)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *Text.ToString());

	// ModifyLobbyAttributes("MapName", Text.ToString());
}

void ULobby::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedItem: %s"), *SelectedItem);

	// ModifyLobbyAttributes("GameMode", Text.ToString());

	MapComboBox->ClearOptions();
	if (SelectedItem == "Mutation")
	{
		for (int32 j = 0; j < static_cast<int32>(EMutationMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<EMutationMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}
	else if (SelectedItem == "TeamDeadMatchMatch")
	{
		for (int32 j = 0; j < static_cast<int32>(ETeamDeadMatchMatchMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<ETeamDeadMatchMatchMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}
	MapComboBox->SetSelectedIndex(0);
}

void ULobby::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedItem: %s"), *SelectedItem);

	// ModifyLobbyAttributes("MapName", SelectedItem);
}

void ULobby::ModifyLobbyAttributes(const FString& Key, const FString& Value)
{
	if (EOSSubsystem)
	{
		TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes;
		UpdatedAttributes.Add(FSchemaAttributeId(Key), FSchemaVariant(Value));
		EOSSubsystem->ModifyLobbyAttributes(UpdatedAttributes);
	}
}

// 修改大厅属性完成事件
void ULobby::OnModifyLobbyAttributesComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifyLobbyAttributes Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Modify lobby attributes failed!"), false);
	}
}

// 大厅属性改变事件
void ULobby::OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyAttributesChanged"));

	for (auto& ChangedAttribute : LobbyAttributesChanged.ChangedAttributes)
	{
		if (ChangedAttribute.Key == FName(TEXT("SessionId")))
		{
			UE_LOG(LogTemp, Warning, TEXT("New SessionId: %s"), *ChangedAttribute.Value.Value.GetString());

			if (EOSSubsystem)
			{
				EOSSubsystem->JoinSession(ChangedAttribute.Value.Value.GetString());
			}
		}
	}
}

void ULobby::OnJoinServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinServerButtonClicked"));
}

void ULobby::OnSwitchTeamButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSwitchTeamButtonClicked"));

	// ModifyLobbyMemberAttributes();
}

void ULobby::OnReadyButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnReadyButtonClicked"));

	// ModifyLobbyMemberAttributes();
}

void ULobby::ModifyLobbyMemberAttributes(const FString& Key, const FString& Value)
{
	if (EOSSubsystem)
	{
		TMap<FSchemaAttributeId, FSchemaVariant> UpdatedAttributes;
		UpdatedAttributes.Add(FSchemaAttributeId(Key), FSchemaVariant(Value));
		EOSSubsystem->ModifyLobbyMemberAttributes(UpdatedAttributes);
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
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Modify lobby member attributes failed!"), false);
	}
}

// 大厅成员属性改变事件
void ULobby::OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberAttributesChanged"));
}

void ULobby::OnStartServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartServerButtonClicked"));

	// if (EOSSubsystem)
	// {
	// 	EOSSubsystem->CreateSession();
	// }
}

// 离开大厅
void ULobby::OnBackButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}

	if (EOSSubsystem && EOSSubsystem->AccountInfo->AccountId == EOSSubsystem->CurrentLobby->OwnerAccountId)
	{
		if (1) // TODO 成员大于1
		{
			for (auto& Member : EOSSubsystem->CurrentLobby->Members)
			{
				if (Member.Value->AccountId != EOSSubsystem->AccountInfo->AccountId)
				{
					EOSSubsystem->PromoteLobbyMember(Member.Value->AccountId);
					break;
				}
			}
		}
		else
		{
			EOSSubsystem->LeaveLobby();
		}
	}
}

// 离开大厅完成事件
void ULobby::OnLeaveLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Leave lobby failed!"), false);
	}
}

// 成员离开大厅事件
void ULobby::OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberLeft"));

	if (Team1Container && Team2Container)
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

// 被踢出大厅事件
void ULobby::OnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}

	GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorMain, TEXT("Got kicked!"), false);
}

void ULobby::OnPromoteLobbyMemberComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (EOSSubsystem)
		{
			EOSSubsystem->LeaveLobby();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Promote lobby member failed!"), false);
	}
}

// 大厅房主改变事件
void ULobby::OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyLeaderChanged"));

	if (Team1Container && Team2Container)
	{
		for (int32 i = 0; i < Team1Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyLeaderChanged.Leader->AccountId)
			{
				// TODO 添加房主图标
			}
			else
			{
				// TODO 移除房主图标
			}
		}

		for (int32 i = 0; i < Team2Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team2Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyLeaderChanged.Leader->AccountId)
			{
			}
			else
			{
			}
		}
	}
}

void ULobby::OnSendMsgButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSendMsgButtonClicked"));
}
