#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabVideo.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabVideo : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	// UPROPERTY()
	// UGameUserSettings* GameUserSettings;

	void SetUIDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* BrightnessAnalogSlider;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Brightness;
	UFUNCTION()
	void OnBrightnessChanged(float Value);

};
