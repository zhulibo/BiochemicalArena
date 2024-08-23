#include "TabControl.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "..\..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/ComboBoxString.h"

void UTabControl::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MouseAimAssistSteeringComboBox->AddOption("on");
	MouseAimAssistSteeringComboBox->AddOption("off");
	MouseAimAssistSlowdownComboBox->AddOption("on");
	MouseAimAssistSlowdownComboBox->AddOption("off");
	MouseAimAssistSteeringComboBox->SetIsEnabled(false); // TODO
	MouseAimAssistSlowdownComboBox->SetIsEnabled(false);

	ControllerAimAssistSteeringComboBox->AddOption("on");
	ControllerAimAssistSteeringComboBox->AddOption("off");
	ControllerAimAssistSlowdownComboBox->AddOption("on");
	ControllerAimAssistSlowdownComboBox->AddOption("off");
	ControllerAimAssistSteeringComboBox->SetIsEnabled(false); // TODO
	ControllerAimAssistSlowdownComboBox->SetIsEnabled(false);

	SetUIDefaultValue();

	MouseSensitivityAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnMouseSensitivityChanged);
	MouseAimAssistSteeringComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSteeringChanged);
	MouseAimAssistSlowdownComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSlowdownChanged);
	ControllerSensitivityAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnControllerSensitivityChanged);
	ControllerAimAssistSteeringComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSteeringChanged);
	ControllerAimAssistSlowdownComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSlowdownChanged);
}

UWidget* UTabControl::NativeGetDesiredFocusTarget() const
{
	return MouseSensitivityAnalogSlider;
}

void UTabControl::SetUIDefaultValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		MouseSensitivityAnalogSlider->SetValue(InverseMapSensitivity(StorageSubsystem->StorageCache->MouseSensitivity));
		MouseSensitivity->SetText(FText::AsNumber(InverseMapSensitivity(StorageSubsystem->StorageCache->MouseSensitivity)));
		MouseAimAssistSteeringComboBox->SetSelectedOption(StorageSubsystem->StorageCache->MouseAimAssistSteering ? "on" : "off");
		MouseAimAssistSlowdownComboBox->SetSelectedOption(StorageSubsystem->StorageCache->MouseAimAssistSlowdown ? "on" : "off");

		ControllerSensitivityAnalogSlider->SetValue(InverseMapSensitivity(StorageSubsystem->StorageCache->ControllerSensitivity));
		ControllerSensitivity->SetText(FText::AsNumber(InverseMapSensitivity(StorageSubsystem->StorageCache->ControllerSensitivity)));
		ControllerAimAssistSteeringComboBox->SetSelectedOption(StorageSubsystem->StorageCache->ControllerAimAssistSteering ? "on" : "off");
		ControllerAimAssistSlowdownComboBox->SetSelectedOption(StorageSubsystem->StorageCache->ControllerAimAssistSlowdown ? "on" : "off");
	}
}

float UTabControl::MapSensitivity(float Value)
{
	if (Value < 50.f)
	{
		FVector2D InRange(1.f, 50.f);
		FVector2D OutRange(0.25f, 1.f);
		Value =  FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else if (Value > 50.f)
	{
		FVector2D InRange(50.f, 100.f);
		FVector2D OutRange(1.f, 4.f);
		Value =  FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else
	{
		Value =  1.f;
	}

	return FMath::RoundToFloat(Value * 10.f) / 10.f;
}

float UTabControl::InverseMapSensitivity(float Value)
{
	if (Value < 1.f)
	{
		FVector2D InRange(0.25f, 1.f);
		FVector2D OutRange(1.f, 50.f);
		Value = FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else if (Value > 1.f)
	{
		FVector2D InRange(1.f, 4.f);
		FVector2D OutRange(50.f, 100.f);
		Value = FMath::GetMappedRangeValueClamped(InRange, OutRange, Value);
	}
	else
	{
		Value = 50.f;
	}

	return FMath::RoundToFloat(Value);
}

void UTabControl::OnMouseSensitivityChanged(float Value)
{
	MouseSensitivity->SetText(FText::AsNumber(FMath::RoundToFloat(Value)));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->MouseSensitivity = MapSensitivity(Value);
		StorageSubsystem->Save();
	}
}

void UTabControl::OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->MouseAimAssistSteering = SelectedItem == "on";
		StorageSubsystem->Save();
	}
}
void UTabControl::OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->MouseAimAssistSlowdown = SelectedItem == "on";
		StorageSubsystem->Save();
	}
}

void UTabControl::OnControllerSensitivityChanged(float Value)
{
	ControllerSensitivity->SetText(FText::AsNumber(FMath::RoundToFloat(Value)));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->ControllerSensitivity = MapSensitivity(Value);
		StorageSubsystem->Save();
	}
}

void UTabControl::OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->ControllerAimAssistSteering = SelectedItem == "on";
		StorageSubsystem->Save();
	}
}
void UTabControl::OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->ControllerAimAssistSlowdown = SelectedItem == "on";
		StorageSubsystem->Save();
	}
}
