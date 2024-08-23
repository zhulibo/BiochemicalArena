#include "TabVideo.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "..\..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/ComboBoxString.h"

void UTabVideo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetUIDefaultValue();

	BrightnessAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnBrightnessChanged);
}

UWidget* UTabVideo::NativeGetDesiredFocusTarget() const
{
	return BrightnessAnalogSlider;
}

void UTabVideo::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		BrightnessAnalogSlider->SetValue(StorageSubsystem->StorageCache->Brightness);
		Brightness->SetText(FText::AsNumber(StorageSubsystem->StorageCache->Brightness));
	}
}

void UTabVideo::OnBrightnessChanged(float Value)
{
	Value = FMath::RoundToFloat(Value * 10) / 10;

	Brightness->SetText(FText::AsNumber(Value));

	GEngine->DisplayGamma = Value;

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->Brightness = Value;
		StorageSubsystem->Save();
	}
}
