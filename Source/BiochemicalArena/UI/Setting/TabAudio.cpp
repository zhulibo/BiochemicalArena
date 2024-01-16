#include "TabAudio.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/System/PlayerStorage.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabAudio::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	// GameUserSettings = GEngine->GetGameUserSettings();

	SetDefaultValue();

	VolumeController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnVolumeChanged);
}

void UTabAudio::SetDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		VolumeController->SetValue(StorageSubsystem->PlayerStorageCache->Volume);
		Volume->SetText(FText::AsNumber(StorageSubsystem->PlayerStorageCache->Volume));
	}
}

void UTabAudio::OnVolumeChanged(float Value)
{
	Volume->SetText(FText::AsNumber(Value));

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->SetAudio(Value);

		StorageSubsystem->PlayerStorageCache->Volume = Value;
		StorageSubsystem->Save();
	}
}
