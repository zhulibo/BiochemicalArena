#include "Lobby.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PlayerLine.h"
#include "CommonHierarchicalScrollBox.h"

void ULobby::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	if (!BackButton->OnClicked().IsBoundToObject(this)) BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	SendMsgButton->ButtonText->SetText(FText::FromString("SendMsg"));
	if (!SendMsgButton->OnClicked().IsBoundToObject(this)) SendMsgButton->OnClicked().AddUObject(this, &ThisClass::OnSendMsgButtonClicked);

	JoinServerButton->ButtonText->SetText(FText::FromString("Join"));
	if (!JoinServerButton->OnClicked().IsBoundToObject(this)) JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);

	StartServerButton->ButtonText->SetText(FText::FromString("Start"));
	if (!StartServerButton->OnClicked().IsBoundToObject(this)) StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);

	Team1Button->ButtonText->SetText(FText::FromString("Join Team1"));
	if (!Team1Button->OnClicked().IsBoundToObject(this)) Team1Button->OnClicked().AddUObject(this, &ThisClass::OnTeam1ButtonClicked);

	Team2Button->ButtonText->SetText(FText::FromString("Join Team2"));
	if (!Team2Button->OnClicked().IsBoundToObject(this)) Team2Button->OnClicked().AddUObject(this, &ThisClass::OnTeam2ButtonClicked);

	AddPlayer();
}

void ULobby::OnBackButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}
}

void ULobby::OnSendMsgButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSendMsgButtonClicked"));
}

void ULobby::OnJoinServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinServerButtonClicked"));
}

void ULobby::OnStartServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartServerButtonClicked"));
}

void ULobby::OnTeam1ButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTeam1ButtonClicked"));
}

void ULobby::OnTeam2ButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTeam2ButtonClicked"));
}

void ULobby::AddPlayer()
{
	if (Team1Container && PlayerLineButtonClass)
	{
		Team1Container->ClearChildren();
		for (int i = 0; i < 9; ++i)
		{
			UPlayerLine* PlayerLineButton = CreateWidget<UPlayerLine>(this, PlayerLineButtonClass);
			if (PlayerLineButton)
			{
				PlayerLineButton->SetPadding(FMargin(0, 0, 0, 10));
				Team1Container->AddChild(PlayerLineButton);
			}
		}
	}
}
