#include "HUDContainer.h"
#include "Infect.h"
#include "InfectHuman.h"
#include "InfectZombie.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PauseMenu.h"
#include "TeamDeadMatch.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"

void UHUDContainer::NativeConstruct()
{
	Super::NativeConstruct();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		FInputModeGameOnly InputModeData;
		BaseController->SetInputMode(InputModeData);
		BaseController->SetShowMouseCursor(false);
	}

	TeamDeadMatch->SetVisibility(ESlateVisibility::Hidden);

	Infect->SetVisibility(ESlateVisibility::Hidden);
	InfectHuman->SetVisibility(ESlateVisibility::Hidden);
	InfectZombie->SetVisibility(ESlateVisibility::Hidden);

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDContainer::ShowPauseMenu()
{
	if (PauseMenuClass) MainStack->AddWidget(PauseMenuClass);
}
