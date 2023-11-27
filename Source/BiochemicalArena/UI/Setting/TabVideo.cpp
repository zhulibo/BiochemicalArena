#include "TabVideo.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "Components/ComboBoxString.h"

void UTabVideo::NativeConstruct()
{
	Super::NativeConstruct();

	SetDefaultValue();

	WindowModeController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnWindowModeChanged);
	BrightnessController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnBrightnessChanged);
	FieldOfViewController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnFieldOfViewChanged);
	ResolutionController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnResolutionChanged);
	VerticalSyncController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnVerticalSyncChanged);
	FrameRateLimitController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnFrameRateLimitChanged);
}

void UTabVideo::SetDefaultValue()
{
	BrightnessValue->SetText(FText::FromString(FString::FromInt(BrightnessController->GetValue())));
	FieldOfViewValue->SetText(FText::FromString(FString::FromInt(FieldOfViewController->GetValue())));
	FrameRateLimitValue->SetText(FText::FromString(FString::FromInt(FrameRateLimitController->GetValue())));
	// TODO 如有缓存设置缓存值，否则设置默认值
}

void UTabVideo::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnWindowModeChanged: %s"), *SelectedItem);
}

void UTabVideo::OnBrightnessChanged(float Value)
{
	BrightnessValue->SetText(FText::FromString(FString::FromInt(Value)));
}

void UTabVideo::OnFieldOfViewChanged(float Value)
{
	FieldOfViewValue->SetText(FText::FromString(FString::FromInt(Value)));
}

void UTabVideo::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnResolutionChanged: %s"), *SelectedItem);
}

void UTabVideo::OnVerticalSyncChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnVerticalSyncChanged: %s"), *SelectedItem);
}

void UTabVideo::OnFrameRateLimitChanged(float Value)
{
	FrameRateLimitValue->SetText(FText::FromString(FString::FromInt(Value)));
}
