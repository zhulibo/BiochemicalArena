#include "Server.h"

#include <string>

#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ServerContainer.h"
#include "ServerLineButton.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/EditableTextBox.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "ServerDetail.h"
#include "Lobby.h"
#include "P2P.h"
#include "ServiceManager.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/GameModes/GameModeType.h"
#include "BiochemicalArena/UI/Common/CommonComboBox.h"
#include "Engine/UserInterfaceSettings.h"

void UServer::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	for (int32 j = 0; j < static_cast<int32>(ECoolGameMode::MAX); ++j)
	{
		FString EnumValue = UEnum::GetValueAsString(static_cast<ECoolGameMode>(j));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		ModeComboBox->AddOption(EnumValue);
	}
	ModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnModeComboBoxChanged);
	ModeComboBox->SetSelectedIndex(0);

	MapComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMapComboBoxChanged);

	RefreshServerButton->OnClicked().AddUObject(this, &ThisClass::OnRefreshServerButtonClicked);
	ResetServerButton->OnClicked().AddUObject(this, &ThisClass::OnResetServerButtonClicked);
	CreateServerButton->OnClicked().AddUObject(this, &ThisClass::OnCreateServerButtonClicked);
	PagePrevButton->OnClicked().AddUObject(this, &ThisClass::OnPagePrevButtonClicked);
	PageNextButton->OnClicked().AddUObject(this, &ThisClass::OnPageNextButtonClicked);

	ServiceManager = UServiceManager::GetServiceManager();
	if (ServiceManager)
	{
		Lobby = ServiceManager->GetLobby();
		if (Lobby)
		{
			if (!Lobby->OnCreateLobbyComplete.IsBoundToObject(this)) Lobby->OnCreateLobbyComplete.AddUObject(this, &ThisClass::OnCreateLobbyComplete);
			if (!Lobby->OnFindLobbiesComplete.IsBoundToObject(this)) Lobby->OnFindLobbiesComplete.AddUObject(this, &ThisClass::OnFindLobbiesComplete);
			if (!Lobby->OnJoinLobbyComplete.IsBoundToObject(this)) Lobby->OnJoinLobbyComplete.AddUObject(this, &ThisClass::OnJoinLobbyComplete);
		}

		P2P = ServiceManager->GetP2P();
	}
}

void UServer::NativeConstruct()
{
	Super::NativeConstruct();

	// OnRefreshServerButtonClicked();
}

UWidget* UServer::NativeGetDesiredFocusTarget() const
{
	return RefreshServerButton;
}

// 创建大厅
void UServer::OnCreateServerButtonClicked()
{
	if (Lobby)
	{
		CreateServerButton->SetIsEnabled(false);
	
		Lobby->CreateLobby();
	}
}

// 创建大厅完成事件
void UServer::OnCreateLobbyComplete(bool bWasSuccessful)
{
	CreateServerButton->SetIsEnabled(true);

	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->AddWidget(ServerDetailClass);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Create lobby failed!"), false);
	}
}

void UServer::OnModeComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnModeComboBoxChanged: %s %d"), *SelectedItem, SelectionType);

	MapComboBox->ClearOptions();
	MapComboBox->AddOption(FString(TEXT("All")));

	if (SelectedItem == "Mutation")
	{
		for (int32 j = 0; j < static_cast<int32>(EMutationMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<EMutationMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}
	else if (SelectedItem == "TeamDeadMatch")
	{
		for (int32 j = 0; j < static_cast<int32>(ETeamDeadMatchMap::MAX); ++j)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<ETeamDeadMatchMap>(j));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			MapComboBox->AddOption(EnumValue);
		}
	}

	MapComboBox->SetSelectedIndex(0);
}

void UServer::OnMapComboBoxChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMapComboBoxChanged: %s %d"), *SelectedItem, SelectionType);
}

// 查找大厅
void UServer::OnRefreshServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRefreshServerButtonClicked"));

	if (Lobby)
	{
		RefreshServerButton->SetIsEnabled(false);

		TArray<FLobbiesAttr> Attrs;

		FString ServerName = ServerNameEditableTextBox->GetText().ToString();
		if (!ServerName.IsEmpty())
		{
			FLobbiesAttr Attr = {};
			std::string TempKey = TCHAR_TO_UTF8(*LOBBY_SERVERNAME.ToString());
			std::string TempValue = TCHAR_TO_UTF8(*ServerName);
			Attr.Key = TempKey;
			Attr.AsUtf8 = TempValue;
			Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
			Attr.ComparisonOp = EOS_EComparisonOp::EOS_CO_CONTAINS;
			Attrs.Add(Attr);
		}

		FString ModeName = ModeComboBox->GetSelectedOption();
		{
			FLobbiesAttr Attr = {};
			std::string TempKey = TCHAR_TO_UTF8(*LOBBY_MODENAME.ToString());
			std::string TempValue = TCHAR_TO_UTF8(*ModeName);
			Attr.Key = TempKey;
			Attr.AsUtf8 = TempValue;
			Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
			Attr.ComparisonOp = EOS_EComparisonOp::EOS_CO_EQUAL;
			Attrs.Add(Attr);
		}

		FString MapName = MapComboBox->GetSelectedOption();
		if (!MapName.IsEmpty() && MapName != "All")
		{
			FLobbiesAttr Attr = {};
			std::string TempKey = TCHAR_TO_UTF8(*LOBBY_MAPNAME.ToString());
			std::string TempValue = TCHAR_TO_UTF8(*MapName);
			Attr.Key = TempKey;
			Attr.AsUtf8 = TempValue;
			Attr.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
			Attr.ComparisonOp = EOS_EComparisonOp::EOS_CO_EQUAL;
			Attrs.Add(Attr);
		}

		Lobby->FindLobbies(Attrs);
	}
}

// 查找大厅完成事件
void UServer::OnFindLobbiesComplete(bool bWasSuccessful, TArray<TSharedPtr<FCoolLobby>> FoundLobbies)
{
	RefreshServerButton->SetIsEnabled(true);

	if (bWasSuccessful)
	{
		ServerLineButtonContainer->ClearChildren();

		if (FoundLobbies.Num() > 0)
		{
			if (ServerLineButtonContainer && ServerLineButtonClass)
			{
				for (int32 i = 0; i < FoundLobbies.Num(); ++i)
				{
					UServerLineButton* ServerLineButton = CreateWidget<UServerLineButton>(this, ServerLineButtonClass);
					if (ServerLineButton)
					{
						ServerLineButton->Lobby = FoundLobbies[i];

						ServerLineButton->Server->SetText(FText::FromString(FoundLobbies[i]->GetStringAttr(LOBBY_SERVERNAME)));
						ServerLineButton->Mode->SetText(FText::FromString(FoundLobbies[i]->GetStringAttr(LOBBY_MODENAME)));
						ServerLineButton->Map->SetText(FText::FromString(FoundLobbies[i]->GetStringAttr(LOBBY_MAPNAME)));
						ServerLineButton->Player->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"),
							FoundLobbies[i]->MaxMembers - FoundLobbies[i]->AvailableSlots, FoundLobbies[i]->MaxMembers)));
						ServerLineButton->Ping->SetText(FText::FromString(TEXT("-"))); // TODO Lobby don't have ping

						ServerLineButton->OnClicked().AddUObject(this, &ThisClass::OnServerLineButtonClicked, ServerLineButton, i);
						ServerLineButtonContainer->AddChild(ServerLineButton);
					}
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("No lobby found!"), false);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Find lobby failed!"), false);
	}
}

// 重置查询条件
void UServer::OnResetServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnResetServerButtonClicked"));

	ServerNameEditableTextBox->SetText(FText::FromString(TEXT("")));
	
	ModeComboBox->SetSelectedIndex(0);
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
void UServer::OnServerLineButtonClicked(UServerLineButton* ServerLineButton, int32 Index)
{
	if (Lobby)
	{
		Lobby->JoinLobby(ServerLineButton->Lobby, UServiceManager::GetLobby()->LobbyDetailsHandles[Index]);
	}
}

// 加入大厅完成事件
void UServer::OnJoinLobbyComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinLobbyComplete %d"), bWasSuccessful);
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->AddWidget(ServerDetailClass);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_RED, TEXT("Join lobby failed!"), false);
	}
}

// 收到邀请
void UServer::OnLobbyInvitationAdded()
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyInvitationAdded"));
}

// 好友请求加入房间
void UServer::OnUILobbyJoinRequested()
{
	UE_LOG(LogTemp, Warning, TEXT("OnUILobbyJoinRequested"));
}
