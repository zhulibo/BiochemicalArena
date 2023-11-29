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

	AddServerListItem();
}

void UServer::AddServerListItem()
{
	if (ServerLineButtonContainer && ServerLineButtonClass)
	{
		for (int i = 0; i < 30; i++)
		{
			UServerLineButton* ServerLineButton = CreateWidget<UServerLineButton>(this, ServerLineButtonClass);
			if (ServerLineButton)
			{
				ServerLineButton->SetPadding(FMargin(0, 0, 0, 2));
				ServerLineButton->OnClicked().AddUObject(this, &ThisClass::OnServerLineButtonClicked, ServerLineButton);
				ServerLineButtonContainer->AddChild(ServerLineButton);
			}
		}
	}
}

void UServer::OnServerNameInputTextChanged(const FText& Text)
{
	ServerName = Text;
	UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerName.ToString());
}

void UServer::OnServerReFreshButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerReFreshButtonClicked"));
}

void UServer::OnServerResetButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerResetButtonClicked"));
	ServerNameInput->SetText(FText::FromString(""));
	ServerName = FText::FromString("");
}

void UServer::OnServerCreateButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerCreateButtonClicked"));
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->AddWidget(LobbyClass);
	}
}

void UServer::OnServerLineButtonClicked(UServerLineButton* ServerLineButton)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerLineButton: %s"), *ServerLineButton->Server->GetText().ToString());
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->AddWidget(LobbyClass);
	}
}
