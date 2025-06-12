#include "TeamDeadMatchContainer.h"

#include "MutateArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "MutateArena/UI/HUD/Scoreboard/Scoreboard.h"

void UTeamDeadMatchContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}
