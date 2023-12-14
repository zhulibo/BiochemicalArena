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

void UServer::NativeConstruct()
{
	Super::NativeConstruct();

	MenuController = Cast<AMenuController>(GetOwningPlayer());

	ServerNameInput->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnServerNameInputTextChanged);

	ServerReFreshButton->ButtonText->SetText(FText::FromString("Refresh"));
	ServerReFreshButton->OnClicked().AddUObject(this, &ThisClass::OnServerReFreshButtonClicked);

	ServerResetButton->ButtonText->SetText(FText::FromString("Reset"));
	ServerResetButton->OnClicked().AddUObject(this, &ThisClass::OnServerResetButtonClicked);

	ServerCreateButton->ButtonText->SetText(FText::FromString("Create Server"));
	ServerCreateButton->OnClicked().AddUObject(this, &ThisClass::OnServerCreateButtonClicked);

	Login1Button->ButtonText->SetText(FText::FromString("Login1"));
	Login1Button->OnClicked().AddUObject(this, &ThisClass::OnLogin1ButtonClicked);

	Login2Button->ButtonText->SetText(FText::FromString("Login2"));
	Login2Button->OnClicked().AddUObject(this, &ThisClass::OnLogin2ButtonClicked);

	EOS = GetGameInstance()->GetSubsystem<UEOS>();
	// 绑定委托
	if (EOS) EOS->OnCreateLobbyComplete.AddUObject(this, &ThisClass::OnCreateLobbyComplete);
	if (EOS) EOS->OnFindLobbyComplete.AddUObject(this, &ThisClass::OnFindLobbyComplete);
	if (EOS) EOS->OnJoinLobbyComplete.AddUObject(this, &ThisClass::OnJoinLobbyComplete);

	if (EOS) EOS->OnLobbyInvitationAdded.AddUObject(this, &ThisClass::OnLobbyInvitationAdded);
	if (EOS) EOS->OnUILobbyJoinRequested.AddUObject(this, &ThisClass::OnUILobbyJoinRequested);

}

void UServer::OnServerNameInputTextChanged(const FText& Text)
{
	ServerName = Text;
	UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerName.ToString());
}

void UServer::OnServerReFreshButtonClicked()
{
	if (EOS == nullptr) EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		EOS->FindLobby();
	}
}

void UServer::OnServerResetButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerResetButtonClicked"));
	ServerNameInput->SetText(FText::FromString(""));
	ServerName = FText::FromString("");
}

void UServer::OnServerCreateButtonClicked()
{
	if (EOS == nullptr) EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		EOS->CreateLobby();
	}
}

void UServer::OnLogin1ButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->Login1();
	}
}

void UServer::OnLogin2ButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->Login2();
	}
}

void UServer::OnCreateLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->AddWidget(LobbyClass);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CreateLobby Failed!"));
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
				for (int i = 0; i < Lobbies.Num(); ++i)
				{
					UServerLineButton* ServerLineButton = CreateWidget<UServerLineButton>(this, ServerLineButtonClass);
					if (ServerLineButton)
					{
						ServerLineButton->Lobby = Lobbies[i];
						ServerLineButton->Server->SetText(FText::FromName(Lobbies[i]->LocalName));
						ServerLineButton->Map->SetText(FText::FromString(Lobbies[i]->Attributes.Find(FName(TEXT("MapName")))->GetString()));
						int32 MaxMembers = Lobbies[i]->MaxMembers;
						int32 CurrentMembers = Lobbies[i]->Members.Num();
						ServerLineButton->Player->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), CurrentMembers, MaxMembers)));
						ServerLineButton->Ping->SetText(FText::FromString(TEXT("-"))); // TODO Lobby don't have ping
						ServerLineButton->OnClicked().AddUObject(this, &ThisClass::OnServerLineButtonClicked, ServerLineButton);
						ServerLineButtonContainer->AddChild(ServerLineButton);
					}
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Found 0 lobby!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FindLobby Failed!"));
	}
}

void UServer::OnServerLineButtonClicked(UServerLineButton* ServerLineButton)
{
	if (EOS == nullptr) EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS && ServerLineButton->Lobby.IsValid())
	{
		EOS->JoinLobby(ServerLineButton->Lobby.ToSharedRef());
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
			ULobby* Lobby = Cast<ULobby>(MenuController->ServerStack->GetActiveWidget());
			Lobby->InitPlayerList();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("JoinLobby Failed!"));
	}
}

// 收到邀请事件
void UServer::OnLobbyInvitationAdded(const FLobbyInvitationAdded& LobbyInvitationAdded)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("OnLobbyInvitationAdded"));
	if (EOS == nullptr) EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		EOS->JoinLobby(LobbyInvitationAdded.Lobby);
	}
}

// 好友请求加入房间
void UServer::OnUILobbyJoinRequested(const FUILobbyJoinRequested& UILobbyJoinRequested)
{
	// TODO 玩家好像可以在UI界面同意
}
