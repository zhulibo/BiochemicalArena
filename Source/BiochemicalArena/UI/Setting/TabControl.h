#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabControl.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabControl : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	void SetDefaultValue();

	// 键鼠
	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* MouseSensitivityController;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MouseSensitivity;
	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* MouseAimAssistSteeringController;
	UFUNCTION()
	void OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* MouseAimAssistSlowdownController;
	UFUNCTION()
	void OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// 手柄
	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* ControllerSensitivityController;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ControllerSensitivity;
	UFUNCTION()
	void OnControllerSensitivityChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSteeringController;
	UFUNCTION()
	void OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSlowdownController;
	UFUNCTION()
	void OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

};
