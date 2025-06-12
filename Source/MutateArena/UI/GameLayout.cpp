#include "GameLayout.h"

#include "MutateArena/PlayerControllers/BaseController.h"
#include "MutateArena/UI/HUD/PauseMenu.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UGameLayout::NativeOnInitialized()
{
	BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		BaseController->ShowPauseMenu.AddUObject(this, &ThisClass::ShowPauseMenu);
	}
}

void UGameLayout::ShowPauseMenu()
{
	BaseController->FocusUI();
	
	MenuStack->AddWidget(PauseMenuClass);
}
