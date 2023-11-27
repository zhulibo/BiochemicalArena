#include "TabAudio.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"

void UTabAudio::NativeConstruct()
{
	Super::NativeConstruct();

	SetDefaultValue();

	VolumeController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnVolumeChanged);
}

void UTabAudio::SetDefaultValue()
{
	VolumeValue->SetText(FText::FromString(FString::FromInt(VolumeController->GetValue())));
	// TODO 如有缓存设置缓存值，否则设置默认值
}

void UTabAudio::OnVolumeChanged(float Value)
{
	VolumeValue->SetText(FText::FromString(FString::FromInt(Value)));
}
