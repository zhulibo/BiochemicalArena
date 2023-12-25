#include "HUDContainer.h"
#include "Mutation.h"
#include "MutationHuman.h"
#include "MutationMutant.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PauseMenu.h"
#include "RadialMenuContainer.h"
#include "Scoreboard.h"
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

	Mutation->SetVisibility(ESlateVisibility::Hidden);
	MutationHuman->SetVisibility(ESlateVisibility::Hidden);
	MutationMutant->SetVisibility(ESlateVisibility::Hidden);

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDContainer::ShowPauseMenu()
{
	if (PauseMenuClass) MainStack->AddWidget(PauseMenuClass);
}
