#include "HUDContainer.h"
#include "Mutation.h"
#include "MutationHuman.h"
#include "MutationMutant.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PauseMenu.h"
#include "RadialMenuContainer.h"
#include "Scoreboard.h"
#include "TeamDeadMatch.h"

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
