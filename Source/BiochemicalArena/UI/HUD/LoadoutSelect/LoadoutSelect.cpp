#include "LoadoutSelect.h"

#include "LoadoutSelectButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void ULoadoutSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	for (int i = 0; i < LoadoutSelectButtonContainer->GetChildrenCount(); ++i)
	{
		ULoadoutSelectButton* LoadoutSelectButton = Cast<ULoadoutSelectButton>(LoadoutSelectButtonContainer->GetChildAt(i));
		if (LoadoutSelectButton)
		{
			LoadoutSelectButton->OnClicked().AddUObject(this, &ThisClass::OnLoadoutSelectButtonClicked, i);
		}
	}
}

UWidget* ULoadoutSelect::NativeGetDesiredFocusTarget() const
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		int32 CurLoadoutIndex = StorageSubsystem->CacheSetting->CurLoadoutIndex;
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
	if (StorageSubsystem)
	{
		StorageSubsystem->CacheSetting->CurLoadoutIndex = CurLoadoutIndex;
		StorageSubsystem->Save();
	}

	CloseMenu(true);
}

void ULoadoutSelect::CloseMenu(bool bCloseAll)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->HUDContainer)
	{
		BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
		if (bCloseAll)
		{
			BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
			FInputModeGameOnly InputModeData;
			BaseController->SetInputMode(InputModeData);
			BaseController->SetShowMouseCursor(false);
		}
	}
}
