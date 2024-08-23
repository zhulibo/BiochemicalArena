#include "BagMenu.h"

#include "BagMenuButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/MutationPlayerState.h"
#include "BiochemicalArena/PlayerStates/TeamType.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UBagMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	for (int i = 0; i < BagMenuButtonContainer->GetChildrenCount(); ++i)
	{
		UBagMenuButton* BagMenuButton = Cast<UBagMenuButton>(BagMenuButtonContainer->GetChildAt(i));
		if (BagMenuButton)
		{
			BagMenuButton->OnClicked().AddUObject(this, &ThisClass::OnBagMenuButtonClicked, i);
		}
	}

	MutationPlayerState = Cast<AMutationPlayerState>(GetOwningPlayerState());
	if (MutationPlayerState)
	{
		MutationPlayerState->OnLocalCharacterTeamChange.AddUObject(this, &ThisClass::OnTeamChange);
	}
}

UWidget* UBagMenu::NativeGetDesiredFocusTarget() const
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		int32 CurBagIndex = StorageSubsystem->StorageCache->CurBagIndex;
		if (CurBagIndex > 0 && CurBagIndex < BagMenuButtonContainer->GetChildrenCount())
		{
			return BagMenuButtonContainer->GetChildAt(CurBagIndex);
		}
	}

	return BagMenuButtonContainer->GetChildAt(0);
}

void UBagMenu::OnBackButtonClicked()
{
	CloseMenu();
}

void UBagMenu::OnBagMenuButtonClicked(int32 CurBagIndex)
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn());
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (HumanCharacter && StorageSubsystem)
	{
		StorageSubsystem->StorageCache->CurBagIndex = CurBagIndex;
		StorageSubsystem->Save();
	}

	CloseMenu();
}

void UBagMenu::CloseMenu()
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

void UBagMenu::OnTeamChange(ETeam TempTeam)
{
	HumanCharacter = nullptr;

	if (TempTeam == ETeam::Team1)
	{
	}
	else if (TempTeam == ETeam::Team2)
	{
		if (IsActivated())
		{
			CloseMenu();
		}
	}
}
