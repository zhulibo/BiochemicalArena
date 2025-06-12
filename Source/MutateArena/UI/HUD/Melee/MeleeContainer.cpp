#include "MeleeContainer.h"

#include "MutateArena/UI/HUD/RadialMenu/RadialMenuContainer.h"
#include "MutateArena/UI/HUD/Scoreboard/Scoreboard.h"

void UMeleeContainer::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Scoreboard->SetVisibility(ESlateVisibility::Hidden);
	RadialMenuContainer->SetVisibility(ESlateVisibility::Hidden);
}
