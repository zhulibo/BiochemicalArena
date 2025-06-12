#include "PauseMenu.h"
#include "CommonTextBlock.h"
#include "MutateArena/MutateArena.h"
#include "MutateArena/UI/HUD/Mutation/MutationContainer.h"
#include "MutateArena/PlayerControllers/BaseController.h"
#include "MutateArena/System/AssetSubsystem.h"
#include "MutateArena/System/Data/CommonAsset.h"
#include "MutateArena/UI/GameLayout.h"
#include "MutateArena/UI/Common/CommonButton.h"
#include "MutateArena/UI/Common/ConfirmScreen.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "MutateArena/UI/Setting/Setting.h"
#include "MutateArena/UI/HUD/LoadoutSelect/LoadoutSelect.h"
#include "MutateArena/UI/HUD/MutantSelect/MutantSelect.h"

#define LOCTEXT_NAMESPACE "UPauseMenu"

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

	VoteButton->ButtonText->SetText(LOCTEXT("Vote", "Vote"));
	VoteButton->OnClicked().AddUObject(this, &ThisClass::OnVoteButtonClicked);
	VoteButton->SetIsEnabled(false);

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
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->GameLayout)
	{
		bWantToBack = false;

		BaseController->GameLayout->MenuStack->AddWidget(LoadoutSelectClass);
	}
}

void UPauseMenu::OnMutantSelectButtonClicked()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->GameLayout)
	{
		bWantToBack = false;
		
		BaseController->GameLayout->MenuStack->AddWidget(MutantSelectClass);
	}
}

void UPauseMenu::OnSettingButtonClicked()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->GameLayout)
	{
		bWantToBack = false;
		
		BaseController->GameLayout->MenuStack->AddWidget(SettingClass);
	}
}

void UPauseMenu::OnVoteButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnVoteButtonClicked"));
}

void UPauseMenu::OnQuitButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnQuitButtonClicked -------------------------------------------"));
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	UAssetSubsystem* AssetSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	
	if (BaseController && BaseController->GameLayout && AssetSubsystem && AssetSubsystem->CommonAsset)
	{
		FConfirmScreenComplete ResultCallback = FConfirmScreenComplete::CreateUObject(this, &ThisClass::Quit);
		BaseController->GameLayout->ModalStack->AddWidget<UConfirmScreen>(
			AssetSubsystem->CommonAsset->ConfirmScreenClass,
			[ResultCallback](UConfirmScreen& Dialog) {
				Dialog.Setup(LOCTEXT("SureToQuit", "Sure to quit?"), ResultCallback);
			}
		);
	}
}

void UPauseMenu::Quit(EMsgResult MsgResult)
{
	if (MsgResult == EMsgResult::Confirm)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
		if (BaseController)
		{
			BaseController->ClientTravel(MAP_MENU, ETravelType::TRAVEL_Absolute);
		}
	}
}

#undef LOCTEXT_NAMESPACE
