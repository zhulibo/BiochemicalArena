#include "HUDContainer.h"
#include "Mutation.h"
#include "MutationHuman.h"
#include "MutationMutant.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PauseMenu.h"
#include "TeamDeadMatch.h"
#include "RadialMenu/RadialMenuContainer.h"
#include "Scoreboard/Scoreboard.h"

void UHUDContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TeamDeadMatch->SetVisibility(ESlateVisibility::Collapsed);

	Mutation->SetVisibility(ESlateVisibility::Collapsed);
	MutationHuman->SetVisibility(ESlateVisibility::Collapsed);
	MutationMutant->SetVisibility(ESlateVisibility::Collapsed);

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDContainer::ShowPauseMenu()
{
	if (PauseMenuClass) MainStack->AddWidget(PauseMenuClass);
}
