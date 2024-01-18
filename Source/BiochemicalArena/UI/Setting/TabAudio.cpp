#include "TabAudio.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "..\..\System\StorageSaveGame.h"
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
		VolumeController->SetValue(StorageSubsystem->StorageSaveGameCache->Volume);
		Volume->SetText(FText::AsNumber(StorageSubsystem->StorageSaveGameCache->Volume));
	}
}

void UTabAudio::OnVolumeChanged(float Value)
{
	Volume->SetText(FText::AsNumber(Value));

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->SetAudio(Value);

		StorageSubsystem->StorageSaveGameCache->Volume = Value;
		StorageSubsystem->Save();
	}
}
