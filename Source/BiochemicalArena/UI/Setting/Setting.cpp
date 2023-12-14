#include "Setting.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/Menu.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void USetting::NativeConstruct()
{
	Super::NativeConstruct();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	if (!BackButton->OnClicked().IsBoundToObject(this)) BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);
}

void USetting::OnBackButtonClicked()
{
	// 菜单退出
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController && MenuController->MenuContainer)
	{
		MenuController->MenuContainer->MainStack->RemoveWidget(*MenuController->MenuContainer->MainStack->GetActiveWidget());
	}

	// 游戏内退出
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->HUDContainer)
	{
		BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
	}
}
