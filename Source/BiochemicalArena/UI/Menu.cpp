#include "Menu.h"
#include "MenuContainer.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "Common/CommonButton.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
}

void UMenu::OnSettingButtonClicked()
{
	if (SettingClass)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController && MenuController->MenuContainer)
		{
			MenuController->MenuContainer->MainStack->AddWidget(SettingClass);
		}
	}
}

void UMenu::OnQuitButtonClicked()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}
