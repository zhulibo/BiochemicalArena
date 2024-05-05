#include "TabOther.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabOther::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUIDefaultValue();
}

UWidget* UTabOther::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

void UTabOther::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
	}
}
