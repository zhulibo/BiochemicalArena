#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabControl.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabControl : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void SetDefaultValue();
	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);
	UFUNCTION()
	void OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnControllerSensitivityChanged(float Value);
	UFUNCTION()
	void OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MouseSensitivityValue;
	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* MouseSensitivityController;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* MouseAimAssistSteeringController;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* MouseAimAssistSlowdownController;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ControllerSensitivityValue;
	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* ControllerSensitivityController;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSteeringController;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ControllerAimAssistSlowdownController;

};
