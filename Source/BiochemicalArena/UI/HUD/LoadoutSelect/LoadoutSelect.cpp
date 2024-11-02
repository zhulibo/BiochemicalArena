#include "LoadoutSelect.h"

#include "LoadoutSelectButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/Storage/SaveGameLoadout.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void ULoadoutSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	for (int i = 0; i < LoadoutSelectButtonContainer->GetChildrenCount(); ++i)
	{
		if (ULoadoutSelectButton* LoadoutSelectButton = Cast<ULoadoutSelectButton>(LoadoutSelectButtonContainer->GetChildAt(i)))
		{
			LoadoutSelectButton->OnClicked().AddUObject(this, &ThisClass::OnLoadoutSelectButtonClicked, i);
		}
	}
}

UWidget* ULoadoutSelect::NativeGetDesiredFocusTarget() const
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheLoadout)
	{
		int32 CurLoadoutIndex = StorageSubsystem->CacheLoadout->CurLoadoutIndex;
		if (CurLoadoutIndex > 0 && CurLoadoutIndex < LoadoutSelectButtonContainer->GetChildrenCount())
		{
			return LoadoutSelectButtonContainer->GetChildAt(CurLoadoutIndex);
		}
	}

	return LoadoutSelectButtonContainer->GetChildAt(0);
}

void ULoadoutSelect::OnLoadoutSelectButtonClicked(int32 CurLoadoutIndex)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheLoadout)
	{
		StorageSubsystem->CacheLoadout->CurLoadoutIndex = CurLoadoutIndex;
		StorageSubsystem->SaveLoadout();

		if (AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(GetOwningPlayerPawn()))
		{
			HumanCharacter->TrySwitchLoadout();
		}
	}

	CloseMenu(true);
}

void ULoadoutSelect::CloseMenu(bool bClosePauseMenu)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		DeactivateWidget();
		
		if (bClosePauseMenu)
		{
			BaseController->GameLayout->MenuStack->RemoveWidget(*BaseController->GameLayout->MenuStack->GetActiveWidget());
		}
	}
}
