#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabVideo.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabVideo : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void SetDefaultValue();
	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnBrightnessChanged(float Value);
	UFUNCTION()
	void OnFieldOfViewChanged(float Value);
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnVerticalSyncChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFrameRateLimitChanged(float Value);

private:
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* WindowModeController;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* BrightnessValue;
	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* BrightnessController;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FieldOfViewValue;
	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* FieldOfViewController;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ResolutionController;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* VerticalSyncController;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* FrameRateLimitValue;
	UPROPERTY(meta = (BindWidget))
	UAnalogSlider* FrameRateLimitController;

};
