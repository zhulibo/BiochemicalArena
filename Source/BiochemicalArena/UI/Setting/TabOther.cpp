#include "TabOther.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabOther::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	SetUIDefaultValue();
}

void UTabOther::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
	}
}
