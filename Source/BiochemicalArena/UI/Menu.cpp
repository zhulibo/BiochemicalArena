#include "Menu.h"
#include "CommonTextBlock.h"
#include "MenuContainer.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "Common/CommonButton.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SettingButton->ButtonText->SetText(FText::FromString("Setting"));
	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	QuitButton->ButtonText->SetText(FText::FromString("Quit"));
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
	UE_LOG(LogTemp, Warning, TEXT("OnQuitButtonClicked"));
}
