#include "Menu.h"
#include "MenuLayout.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/Data/CommonAsset.h"
#include "Common/CommonButton.h"
#include "BiochemicalArena/UI/Setting/Setting.h"
#include "BiochemicalArena/UI/Common/ConfirmScreen.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#define LOCTEXT_NAMESPACE "UMenu"

void UMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SettingButton->OnClicked().AddUObject(this, &ThisClass::OnSettingButtonClicked);

	QuitButton->OnClicked().AddUObject(this, &ThisClass::OnQuitButtonClicked);
}

// TODO 禁止鼠标点击MenuStack层
void UMenu::OnSettingButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController && MenuController->MenuLayout && SettingClass)
	{
		// 本来应该添加到MenuStack, 但是为了显示Menu背景，添加到了ModalStack
		MenuController->MenuLayout->ModalStack->AddWidget(SettingClass);
	}
}

void UMenu::OnQuitButtonClicked()
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	UAssetSubsystem* AssetSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAssetSubsystem>();

	if (MenuController && MenuController->MenuLayout && AssetSubsystem && AssetSubsystem->CommonAsset)
	{
		FConfirmScreenComplete ResultCallback = FConfirmScreenComplete::CreateUObject(this, &ThisClass::Quit);
		MenuController->MenuLayout->ModalStack->AddWidget<UConfirmScreen>(
			AssetSubsystem->CommonAsset->ConfirmScreenClass,
			[ResultCallback](UConfirmScreen& Dialog) {
				Dialog.Setup(LOCTEXT("SureToQuit", "Sure to quit?"), ResultCallback);
			}
		);
	}
}

void UMenu::Quit(EMsgResult MsgResult)
{
	if (MsgResult == EMsgResult::Confirm)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
	}
}

#undef LOCTEXT_NAMESPACE
