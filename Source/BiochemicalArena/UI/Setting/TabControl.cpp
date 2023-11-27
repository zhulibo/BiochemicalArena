#include "TabControl.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "Components/ComboBoxString.h"

void UTabControl::NativeConstruct()
{
	Super::NativeConstruct();

	SetDefaultValue();

	MouseSensitivityController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnMouseSensitivityChanged);
	MouseAimAssistSteeringController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSteeringChanged);
	MouseAimAssistSlowdownController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSlowdownChanged);
	ControllerSensitivityController->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnControllerSensitivityChanged);
	ControllerAimAssistSteeringController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSteeringChanged);
	ControllerAimAssistSlowdownController->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSlowdownChanged);
}

void UTabControl::SetDefaultValue()
{
	MouseSensitivityValue->SetText(FText::FromString(FString::FromInt(MouseSensitivityController->GetValue())));
	ControllerSensitivityValue->SetText(FText::FromString(FString::FromInt(ControllerSensitivityController->GetValue())));
	// TODO 如有缓存设置缓存值，否则设置默认值
}

void UTabControl::OnMouseSensitivityChanged(float Value)
{
	MouseSensitivityValue->SetText(FText::FromString(FString::FromInt(Value)));
}

void UTabControl::OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMouseAimAssistSteeringChanged: %s"), *SelectedItem);
}
void UTabControl::OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMouseAimAssistSlowdownChanged: %s"), *SelectedItem);
}

void UTabControl::OnControllerSensitivityChanged(float Value)
{
	ControllerSensitivityValue->SetText(FText::FromString(FString::FromInt(Value)));
}

void UTabControl::OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnControllerAimAssistSteeringChanged: %s"), *SelectedItem);
}
void UTabControl::OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnControllerAimAssistSlowdownChanged: %s"), *SelectedItem);
}
