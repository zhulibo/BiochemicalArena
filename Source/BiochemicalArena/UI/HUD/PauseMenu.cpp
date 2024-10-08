#include "PauseMenu.h"
#include "CommonTextBlock.h"
#include "HUDContainer.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "BiochemicalArena/UI/HUD/LoadoutSelect/LoadoutSelect.h"
#include "BiochemicalArena/UI/HUD/MutantSelect/MutantSelect.h"

void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	OnDeactivated().AddUObject(this, &ThisClass::OnBack);
	OnActivated().AddWeakLambda(this, [this]() {
		bWantToBack = true;
	});

	LoadoutSelectButton->ButtonText->SetText(FText::FromString("Loadout"));
	LoadoutSelectButton->OnClicked().AddUObject(this, &ThisClass::OnLoadoutSelectClicked);

	MutantSelectButton->ButtonText->SetText(FText::FromString("Mutant"));
	MutantSelectButton->OnClicked().AddUObject(this, &ThisClass::OnMutantSelectButtonClicked);

	SettingButton->ButtonText->SetText(FText::FromString("Setting"));
	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	KickButton->ButtonText->SetText(FText::FromString("Kick"));
	KickButton->OnClicked().AddUObject(this, &ThisClass::OnKickButtonClicked);

	QuitButton->ButtonText->SetText(FText::FromString("Quit"));
	QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
}

UWidget* UPauseMenu::NativeGetDesiredFocusTarget() const
{
	return LoadoutSelectButton;
}

void UPauseMenu::OnBack()
{
	if (!bWantToBack) return;

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->HUDContainer)
	{
		FInputModeGameOnly InputModeData;
		BaseController->SetInputMode(InputModeData);
		BaseController->SetShowMouseCursor(false);
	}
}

void UPauseMenu::OnLoadoutSelectClicked()
{
	if (LoadoutSelectClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->HUDContainer)
		{
			bWantToBack = false;

			BaseController->HUDContainer->MainStack->AddWidget(LoadoutSelectClass);
		}
	}
}

void UPauseMenu::OnMutantSelectButtonClicked()
{
	if (MutantSelectClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->HUDContainer)
		{
			bWantToBack = false;

			BaseController->HUDContainer->MainStack->AddWidget(MutantSelectClass);
		}
	}
}

void UPauseMenu::OnSettingButtonClicked()
{
	if (SettingClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->HUDContainer)
		{
			bWantToBack = false;

			BaseController->HUDContainer->MainStack->AddWidget(SettingClass);
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
