#include "MeleeContainer.h"

#include "BiochemicalArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "BiochemicalArena/UI/HUD/Scoreboard/Scoreboard.h"

void UMeleeContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}
