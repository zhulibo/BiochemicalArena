#include "TabOther.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"

void UTabOther::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUISavedValue();
}

UWidget* UTabOther::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

void UTabOther::SetUISavedValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
	}
}
