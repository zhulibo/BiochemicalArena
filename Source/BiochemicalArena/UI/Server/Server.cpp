#include "Server.h"

#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "ServerLineButton.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/EditableTextBox.h"

void UServer::NativeConstruct()
{
	Super::NativeConstruct();

	AddServerListItem();
	ServerReFreshButton->ButtonText->SetText(FText::FromString("Refresh"));
	ServerResetButton->ButtonText->SetText(FText::FromString("Reset"));
	ServerReFreshButton->OnClicked().AddUObject(this, &ThisClass::OnServerReFreshButtonClicked);
	ServerResetButton->OnClicked().AddUObject(this, &ThisClass::OnServerResetButtonClicked);
	ServerNameInput->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnServerNameInputTextChanged);
}

void UServer::AddServerListItem()
{
	if (ServerLineButtonContainer && ServerLineButtonClass)
	{
		for (int i = 0; i < 30; ++i)
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

void UServer::OnServerLineButtonClicked(UServerLineButton* ServerLineButton)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerLineButton: %s"), *ServerLineButton->Server->GetText().ToString());
}

void UServer::OnServerReFreshButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnServerReFreshButtonClicked"));
}

void UServer::OnServerResetButtonClicked()
{
	ServerNameInput->SetText(FText::FromString(""));
	ServerName = FText::FromString("");
}

void UServer::OnServerNameInputTextChanged(const FText& Text)
{
	ServerName = Text;
	UE_LOG(LogTemp, Warning, TEXT("ServerName: %s"), *ServerName.ToString());
}
