#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabControl.generated.h"

UCLASS()
class MUTATEARENA_API UTabControl : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* SettingLeft;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* SettingRight;
	void OnTabButtonHovered(int Index);

	void SetUISavedValue();

	FVector2D Range_1_50 = FVector2D(1.f, 50.f);
	FVector2D Range_50_100 = FVector2D(50.f, 100.f);

	float SensitivityMouseMin = 0.2f;
	float SensitivityMouseMiddle = 0.8f; // 需同步修改USaveGameSetting中的默认值
	float SensitivityMouseMax = 3.2f;
	float MapSensitivityMouse(float Value);
	float InverseMapSensitivityMouse(float Value);

	float SensitivityControllerMin = 1.f;
	float SensitivityControllerMiddle = 3.f; // 需同步修改USaveGameSetting中的默认值
	float SensitivityControllerMax = 9.f;
	float MapSensitivityController(float Value);
	float InverseMapSensitivityController(float Value);

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

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle SetDefaultData;
	FUIActionBindingHandle SetDefaultHandle;
	void SetDefault();

};
