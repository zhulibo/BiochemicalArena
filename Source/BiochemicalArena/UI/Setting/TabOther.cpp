#include "TabOther.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabOther::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	SetDefaultValue();
}

void UTabOther::SetDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
	}
}
