#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabAudio.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabAudio : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	void SetUIDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* VolumeAnalogSlider;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Volume;
	UFUNCTION()
	void OnVolumeChanged(float Value);

};
