#include "TabAudio.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "..\..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabAudio::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUIDefaultValue();

	VolumeAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnVolumeChanged);
}

UWidget* UTabAudio::NativeGetDesiredFocusTarget() const
{
	return VolumeAnalogSlider;
}

void UTabAudio::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		VolumeAnalogSlider->SetValue(StorageSubsystem->StorageCache->Volume * 100.f);
		Volume->SetText(FText::AsNumber(StorageSubsystem->StorageCache->Volume * 100.f));
	}
}

void UTabAudio::OnVolumeChanged(float Value)
{
	Value = FMath::RoundToFloat(Value);
	Volume->SetText(FText::AsNumber(Value));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->SetAudio(Value / 100.f);
		StorageSubsystem->StorageCache->Volume = Value / 100.f;
		StorageSubsystem->Save();
	}
}
