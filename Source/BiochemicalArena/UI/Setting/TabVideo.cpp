#include "TabVideo.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/System/PlayerStorage.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/ComboBoxString.h"

void UTabVideo::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	// GameUserSettings = GEngine->GetGameUserSettings();

	SetDefaultValue();

	BrightnessController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnBrightnessChanged);
}

void UTabVideo::SetDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		BrightnessController->SetValue(StorageSubsystem->PlayerStorageCache->Brightness);
		Brightness->SetText(FText::AsNumber(StorageSubsystem->PlayerStorageCache->Brightness));
	}
}

void UTabVideo::OnBrightnessChanged(float Value)
{
	Brightness->SetText(FText::AsNumber(Value));

	GEngine->DisplayGamma = Value;

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->Brightness = Value;
		StorageSubsystem->Save();
	}
}
