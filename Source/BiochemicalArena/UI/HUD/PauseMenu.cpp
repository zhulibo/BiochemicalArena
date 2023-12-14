#include "PauseMenu.h"
#include "CommonTextBlock.h"
#include "HUDContainer.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/UI/Setting/Setting.h"

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

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	if (!BackButton->OnClicked().IsBoundToObject(this)) BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	SettingButton->ButtonText->SetText(FText::FromString("Setting"));
	SettingButton->SetPadding(FMargin(0, 0, 0, 10));
	if (!SettingButton->OnClicked().IsBoundToObject(this)) SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	KickButton->ButtonText->SetText(FText::FromString("Kick"));
	KickButton->SetPadding(FMargin(0, 0, 0, 10));
	if (!KickButton->OnClicked().IsBoundToObject(this)) KickButton->OnClicked().AddUObject(this, &ThisClass::OnKickButtonClicked);

	QuitButton->ButtonText->SetText(FText::FromString("Quit"));
	if (!QuitButton->OnClicked().IsBoundToObject(this)) QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
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
