#include "PauseMenu.h"
#include "CommonTextBlock.h"
#include "HUDContainer.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/UI/Setting/Setting.h"

void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	SettingButton->ButtonText->SetText(FText::FromString("Setting"));
	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	KickButton->ButtonText->SetText(FText::FromString("Kick"));
	KickButton->OnClicked().AddUObject(this, &ThisClass::OnKickButtonClicked);

	QuitButton->ButtonText->SetText(FText::FromString("Quit"));
	QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
}

void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		FInputModeUIOnly InputModeData;
		BaseController->SetInputMode(InputModeData);
		BaseController->SetShowMouseCursor(true);
	}
}

UWidget* UPauseMenu::NativeGetDesiredFocusTarget() const
{
	return SettingButton;
}

void UPauseMenu::OnBackButtonClicked()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->HUDContainer)
	{
		BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());

		FInputModeGameOnly InputModeData;
		BaseController->SetInputMode(InputModeData);
		BaseController->SetShowMouseCursor(false);
	}
}

void UPauseMenu::OnSettingButtonClicked()
{
	if (SettingClass)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController && BaseController->HUDContainer)
		{
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
