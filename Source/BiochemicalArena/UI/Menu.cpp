#include "Menu.h"
#include "CommonTextBlock.h"
#include "MenuContainer.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "Common/CommonButton.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "Dev/Dev.h"
#include "Server/Server.h"
#include "Server/ServerContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MenuController = Cast<AMenuController>(GetOwningPlayer());

	SettingButton->ButtonText->SetText(FText::FromString("Setting"));
	SettingButton->SetPadding(FMargin(0, 0, 20, 0));
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
