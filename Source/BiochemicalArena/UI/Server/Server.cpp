#include "Server.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ServerContainer.h"
#include "ServerLineButton.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/EditableTextBox.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Lobby.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/GameModeType.h"
#include "BiochemicalArena/UI/Common/CommonComboBox.h"
#include "Engine/UserInterfaceSettings.h"

void UServer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ServerNameEditableTextBox->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnServerNameChanged);

	ModeComboBox->AddOption(FString("All"));
	for (int32 j = 0; j < static_cast<int32>(EGameMode::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<EGameMode>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		ModeComboBox->AddOption(EnumValue);
	}
	ModeComboBox->SetSelectedIndex(0);
	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);

	MapComboBox->AddOption(FString("All"));
	MapComboBox->SetSelectedIndex(0);
	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	ServerReFreshButton->ButtonText->SetText(FText::FromString("Refresh"));
	ServerReFreshButton->OnClicked().AddUObject(this, &ThisClass::OnServerReFreshButtonClicked);

	ServerResetButton->ButtonText->SetText(FText::FromString("Reset"));
	ServerResetButton->OnClicked().AddUObject(this, &ThisClass::OnServerResetButtonClicked);

	ServerCreateButton->ButtonText->SetText(FText::FromString("Create Server"));
	ServerCreateButton->OnClicked().AddUObject(this, &ThisClass::OnServerCreateButtonClicked);

	PagePrevButton->ButtonText->SetText(FText::FromString("Prev Page"));
	PagePrevButton->OnClicked().AddUObject(this, &ThisClass::OnPagePrevButtonClicked);

	PageNextButton->ButtonText->SetText(FText::FromString("Next Page"));
	PageNextButton->OnClicked().AddUObject(this, &ThisClass::OnPageNextButtonClicked);

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnCreateLobbyComplete.AddUObject(this, &ThisClass::OnCreateLobbyComplete);
		EOSSubsystem->OnFindLobbyComplete.AddUObject(this, &ThisClass::OnFindLobbyComplete);
		EOSSubsystem->OnJoinLobbyComplete.AddUObject(this, &ThisClass::OnJoinLobbyComplete);

		EOSSubsystem->OnLobbyInvitationAdded.AddUObject(this, &ThisClass::OnLobbyInvitationAdded);
		EOSSubsystem->OnUILobbyJoinRequested.AddUObject(this, &ThisClass::OnUILobbyJoinRequested);

		EOSSubsystem->OnLobbyJoined.AddUObject(this, &ThisClass::OnLobbyJoined);
	}
}

void UServer::NativeConstruct()
{
	Super::NativeConstruct();

	OnServerReFreshButtonClicked();
}

UWidget* UServer::NativeGetDesiredFocusTarget() const
{
	return ServerReFreshButton;
}

// 创建大厅
void UServer::OnServerCreateButtonClicked()
{
	if (EOSSubsystem)
	{
		EOSSubsystem->CreateLobby();
	}
}

// 创建大厅完成事件
void UServer::OnCreateLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->AddWidget(LobbyClass);
			ULobby* Lobby = Cast<ULobby>(MenuController->ServerStack->GetActiveWidget());
			if (Lobby)
			{
				Lobby->OnCreateInit();
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Create lobby failed!"), false);
	}
}

void UServer::OnServerNameChanged(const FText& Text)
{
	ServerName = Text;

	UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerName.ToString());
}

void UServer::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedItem: %s"), *SelectedItem);

	MapComboBox->ClearOptions();
	MapComboBox->AddOption(FString("All"));

	if (SelectedItem == "All")
	{
	}
	else if (SelectedItem == "Mutation")
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

void UServer::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectedItem: %s"), *SelectedItem);
}

// 查找大厅
void UServer::OnServerReFreshButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerReFreshButtonClicked"));

	// for (int32 i = 0; i < 100; ++i)
	// {
	// 	UServerLineButton* ServerLineButton = CreateWidget<UServerLineButton>(this, ServerLineButtonClass);
	// 	ServerLineButton->Ping->SetText(FText::FromString(FString::FromInt(i + 1)));
	// 	ServerLineButton->OnClicked().AddUObject(this, &ThisClass::OnServerLineButtonClicked, ServerLineButton);
	// 	ServerLineButtonContainer->AddChild(ServerLineButton);
	// }

	if (EOSSubsystem)
	{
		EOSSubsystem->FindLobby();
	}
}

// 查找大厅完成事件
void UServer::OnFindLobbyComplete(bool bWasSuccessful, const TArray<TSharedRef<const FLobby>>& Lobbies)
{
	if (bWasSuccessful)
	{
		if (Lobbies.Num() > 0)
		{
			if (ServerLineButtonContainer && ServerLineButtonClass)
			{
				for (int32 i = 0; i < Lobbies.Num(); ++i)
				{
					UServerLineButton* ServerLineButton = CreateWidget<UServerLineButton>(this, ServerLineButtonClass);
					if (ServerLineButton)
					{
						ServerLineButton->Lobby = Lobbies[i];

						ServerLineButton->Server->SetText(FText::FromString(Lobbies[i]->Attributes.Find(FName(TEXT("LobbyName")))->GetString()));
						ServerLineButton->Mode->SetText(FText::FromString(Lobbies[i]->Attributes.Find(FName(TEXT("GameMode")))->GetString()));
						ServerLineButton->Map->SetText(FText::FromString(Lobbies[i]->Attributes.Find(FName(TEXT("MapName")))->GetString()));
						ServerLineButton->Player->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Lobbies[i]->Members.Num(), Lobbies[i]->MaxMembers)));
						ServerLineButton->Ping->SetText(FText::FromString(TEXT("-"))); // TODO Lobby don't have ping

						ServerLineButton->OnClicked().AddUObject(this, &ThisClass::OnServerLineButtonClicked, ServerLineButton);
						ServerLineButtonContainer->AddChild(ServerLineButton);
					}
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorMain, TEXT("No lobby found!"), false);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Find lobby failed!"), false);
	}
}

// 重置查询条件
void UServer::OnServerResetButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerResetButtonClicked"));

	ServerNameEditableTextBox->SetText(FText::FromString(""));
	ServerName = FText::FromString("");
	ModeComboBox->SetSelectedIndex(0);
	MapComboBox->SetSelectedIndex(0);
}

void UServer::OnPagePrevButtonClicked()
{
	if (ServerLineButtonContainer && ServerLineButtonContainer->GetChildAt(0))
	{
		float PageHeight = ServerLineButtonContainer->GetCachedGeometry().GetAbsoluteSize().Y;
		float LineButtonHeight = ServerLineButtonContainer->GetChildAt(0) ? ServerLineButtonContainer->GetChildAt(0)->GetCachedGeometry().GetAbsoluteSize().Y : 0.f;
		float JumpOverLineButtonNum = 10.f;
		if (LineButtonHeight > 0.f && PageHeight > 0.f)
		{
			JumpOverLineButtonNum = PageHeight * 0.5f / LineButtonHeight;
		}

		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		const float DPI = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
		int32 TargetLineButtonNum = FMath::CeilToInt(ServerLineButtonContainer->GetScrollOffset() * DPI / LineButtonHeight - JumpOverLineButtonNum + 1);

		UWidget* TargetLineButton = ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1);
		if (TargetLineButton == nullptr)
		{
			TargetLineButtonNum = 1;
			TargetLineButton = ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1);
		}
		if (ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1) && TargetLineButton)
		{
			ServerLineButtonContainer->ScrollWidgetIntoView(TargetLineButton, true, EDescendantScrollDestination::Center);
			ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1)->SetFocus();
		}
	}
}

void UServer::OnPageNextButtonClicked()
{
	if (ServerLineButtonContainer && ServerLineButtonContainer->GetChildAt(0))
	{
		float PageHeight = ServerLineButtonContainer->GetCachedGeometry().GetAbsoluteSize().Y;
		float LineButtonHeight = ServerLineButtonContainer->GetChildAt(0) ? ServerLineButtonContainer->GetChildAt(0)->GetCachedGeometry().GetAbsoluteSize().Y : 0.f;
		float JumpOverLineButtonNum = 10.f;
		if (LineButtonHeight > 0.f && PageHeight > 0.f)
		{
			JumpOverLineButtonNum = PageHeight * 1.5f / LineButtonHeight;
		}

		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		const float DPI = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(ViewportSize.X, ViewportSize.Y));
		int32 TargetLineButtonNum = FMath::FloorToInt(ServerLineButtonContainer->GetScrollOffset() * DPI / LineButtonHeight + JumpOverLineButtonNum);

		UWidget* TargetLineButton = ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1);
		if (TargetLineButton == nullptr)
		{
			TargetLineButtonNum = ServerLineButtonContainer->GetChildrenCount();
			TargetLineButton = ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1);
		}
		if (ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1) && TargetLineButton)
		{
			ServerLineButtonContainer->ScrollWidgetIntoView(TargetLineButton, true, EDescendantScrollDestination::Center);
			ServerLineButtonContainer->GetChildAt(TargetLineButtonNum - 1)->SetFocus();
		}
	}
}

// 加入大厅
void UServer::OnServerLineButtonClicked(UServerLineButton* ServerLineButton)
{
	// if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	// if (MenuController)
	// {
	// 	MenuController->ServerStack->AddWidget(LobbyClass);
	// 	ULobby* Lobby = Cast<ULobby>(MenuController->ServerStack->GetActiveWidget());
	// 	if (Lobby)
	// 	{
	// 		Lobby->OnJoinInit();
	// 	}
	// }

	if (EOSSubsystem && ServerLineButton->Lobby.IsValid())
	{
		EOSSubsystem->JoinLobby(ServerLineButton->Lobby.ToSharedRef());
	}
}

// 加入大厅完成事件
void UServer::OnJoinLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->AddWidget(LobbyClass);
			ULobby* Lobby = Cast<ULobby>(MenuController->ServerStack->GetActiveWidget());
			if (Lobby)
			{
				Lobby->OnJoinInit();
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, ColorHuman, TEXT("Join lobby failed!"), false);
	}
}

// 收到邀请
void UServer::OnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded)
{
	if (EOSSubsystem)
	{
		EOSSubsystem->JoinLobby(LobbyInvitationAdded.Lobby);
	}
}

// 好友请求加入房间
void UServer::OnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested)
{
	// TODO 玩家好像可以在UI界面同意
}

// 加入大厅完成事件
void UServer::OnLobbyJoined(const FLobbyJoined& LobbyJoined)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyJoined"));
}
