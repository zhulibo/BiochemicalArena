#include "PauseMenu.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/UI/HUD/Mutation/HUDMutation.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "BiochemicalArena/UI/HUD/LoadoutSelect/LoadoutSelect.h"
#include "BiochemicalArena/UI/HUD/MutantSelect/MutantSelect.h"

#define LOCTEXT_NAMESPACE "BiochemicalArena"

void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	OnActivated().AddWeakLambda(this, [this]() {
		bWantToBack = true;
	});

	OnDeactivated().AddUObject(this, &ThisClass::OnDeactivatedInternal);

	LoadoutSelectButton->ButtonText->SetText(LOCTEXT("Loadout", "Loadout"));
	LoadoutSelectButton->OnClicked().AddUObject(this, &ThisClass::OnLoadoutSelectClicked);

	MutantSelectButton->ButtonText->SetText(LOCTEXT("Mutant", "Mutant"));
	MutantSelectButton->OnClicked().AddUObject(this, &ThisClass::OnMutantSelectButtonClicked);

	SettingButton->ButtonText->SetText(LOCTEXT("Setting", "Setting"));
	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	KickButton->ButtonText->SetText(LOCTEXT("Kick", "Kick"));
	KickButton->OnClicked().AddUObject(this, &ThisClass::OnKickButtonClicked);

	QuitButton->ButtonText->SetText(LOCTEXT("Quit", "Quit"));
	QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
}

UWidget* UPauseMenu::NativeGetDesiredFocusTarget() const
{
	return LoadoutSelectButton;
}

void UPauseMenu::OnDeactivatedInternal()
{
	// 进入二级菜单时不需要FocusGame
	if (!bWantToBack)
	{
		return;
	}

	// 返回游戏时FocusGame
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		BaseController->FocusGame();
	}
}

void UPauseMenu::OnLoadoutSelectClicked()
{
	if (LoadoutSelectClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->GameLayout)
		{
			bWantToBack = false;
		
			BaseController->GameLayout->MenuStack->AddWidget(LoadoutSelectClass);
		}
	}
}

void UPauseMenu::OnMutantSelectButtonClicked()
{
	if (MutantSelectClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->GameLayout)
		{
			bWantToBack = false;
		
			BaseController->GameLayout->MenuStack->AddWidget(MutantSelectClass);
		}
	}
}

void UPauseMenu::OnSettingButtonClicked()
{
	if (SettingClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->GameLayout)
		{
			bWantToBack = false;
		
			BaseController->GameLayout->MenuStack->AddWidget(SettingClass);
		}
	}
}

void UPauseMenu::OnKickButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnKickButtonClicked"));
}

void UPauseMenu::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnQuitButtonClicked"));
}

#undef LOCTEXT_NAMESPACE
