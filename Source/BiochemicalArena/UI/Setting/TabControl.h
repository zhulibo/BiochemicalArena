#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabControl.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabControl : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	void SetUIDefaultValue();
	float MapSensitivity(float Value);
	float InverseMapSensitivity(float Value);

	// 键鼠
	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* MouseSensitivityAnalogSlider;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MouseSensitivity;
	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* MouseAimAssistSteeringComboBox;
	UFUNCTION()
	void OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* MouseAimAssistSlowdownComboBox;
	UFUNCTION()
	void OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// 手柄
	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* ControllerSensitivityAnalogSlider;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ControllerSensitivity;
	UFUNCTION()
	void OnControllerSensitivityChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSteeringComboBox;
	UFUNCTION()
	void OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSlowdownComboBox;
	UFUNCTION()
	void OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

};
