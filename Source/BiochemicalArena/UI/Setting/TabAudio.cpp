#include "TabAudio.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "..\..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"

void UTabAudio::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	SetUIDefaultValue();

	VolumeController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnVolumeChanged);
}

void UTabAudio::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		VolumeController->SetValue(StorageSubsystem->StorageCache->Volume * 100.f);
		Volume->SetText(FText::AsNumber(StorageSubsystem->StorageCache->Volume * 100.f));
	}
}

void UTabAudio::OnVolumeChanged(float Value)
{
	Value = FMath::RoundToFloat(Value);
	Volume->SetText(FText::AsNumber(Value));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->SetAudio(Value / 100.f);
		StorageSubsystem->StorageCache->Volume = Value / 100.f;
		StorageSubsystem->SaveToDisk();
	}
}
