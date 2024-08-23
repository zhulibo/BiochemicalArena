#include "Setting.h"
#include "CommonTextBlock.h"
#include "TabControl.h"
#include "BiochemicalArena/GameModes/MenuMode.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/MenuContainer.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void USetting::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);
}

void USetting::OnBackButtonClicked()
{
	if (Cast<AMenuMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		// 主菜单设置界面返回
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController && MenuController->MenuContainer)
		{
			MenuController->MenuContainer->MainStack->RemoveWidget(*MenuController->MenuContainer->MainStack->GetActiveWidget());
		}
	}
	else
	{
		// 游戏内设置界面返回
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->HUDContainer)
		{
			BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
		}
	}
}
