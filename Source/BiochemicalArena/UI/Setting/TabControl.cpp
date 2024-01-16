#include "TabControl.h"
#include "AnalogSlider.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/System/PlayerStorage.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/ComboBoxString.h"

void UTabControl::NativeConstruct()
{
	Super::NativeConstruct();

	StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

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
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		MouseSensitivityController->SetValue(StorageSubsystem->PlayerStorageCache->MouseSensitivity);
		MouseSensitivity->SetText(FText::AsNumber(StorageSubsystem->PlayerStorageCache->MouseSensitivity));
		MouseAimAssistSteeringController->SetSelectedOption(StorageSubsystem->PlayerStorageCache->MouseAimAssistSteering);
		MouseAimAssistSlowdownController->SetSelectedOption(StorageSubsystem->PlayerStorageCache->MouseAimAssistSlowdown);

		ControllerSensitivityController->SetValue(StorageSubsystem->PlayerStorageCache->ControllerSensitivity);
		ControllerSensitivity->SetText(FText::AsNumber(StorageSubsystem->PlayerStorageCache->ControllerSensitivity));
		ControllerAimAssistSteeringController->SetSelectedOption(StorageSubsystem->PlayerStorageCache->ControllerAimAssistSteering);
		ControllerAimAssistSlowdownController->SetSelectedOption(StorageSubsystem->PlayerStorageCache->ControllerAimAssistSlowdown);
	}
}

void UTabControl::OnMouseSensitivityChanged(float Value)
{
	MouseSensitivity->SetText(FText::AsNumber(Value));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->MouseSensitivity = Value;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}

void UTabControl::OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMouseAimAssistSteeringChanged: %s"), *SelectedItem);
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->MouseAimAssistSteering = SelectedItem;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}
void UTabControl::OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMouseAimAssistSlowdownChanged: %s"), *SelectedItem);
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->MouseAimAssistSlowdown = SelectedItem;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}

void UTabControl::OnControllerSensitivityChanged(float Value)
{
	ControllerSensitivity->SetText(FText::AsNumber(Value));
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->ControllerSensitivity = Value;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}

void UTabControl::OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnControllerAimAssistSteeringChanged: %s"), *SelectedItem);
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->ControllerAimAssistSteering = SelectedItem;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}
void UTabControl::OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnControllerAimAssistSlowdownChanged: %s"), *SelectedItem);
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->PlayerStorageCache->ControllerAimAssistSlowdown = SelectedItem;
		StorageSubsystem->SetCharacterControlVariable();
		StorageSubsystem->Save();
	}
}
