#include "TeamDeadMatchContainer.h"

#include "BiochemicalArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "BiochemicalArena/UI/HUD/Scoreboard/Scoreboard.h"

void UTeamDeadMatchContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}
