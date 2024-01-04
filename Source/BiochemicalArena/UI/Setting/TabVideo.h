#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabVideo.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabVideo : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	void SetDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* WindowModeController;
	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* BrightnessController;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* BrightnessValue;
	UFUNCTION()
	void OnBrightnessChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* FieldOfViewController;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FieldOfViewValue;
	UFUNCTION()
	void OnFieldOfViewChanged(float Value);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ResolutionController;
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* VerticalSyncController;
	UFUNCTION()
	void OnVerticalSyncChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* FrameRateLimitController;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FrameRateLimitValue;
	UFUNCTION()
	void OnFrameRateLimitChanged(float Value);

};
