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

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	UPROPERTY()
	UGameUserSettings* GameUserSettings;

	void SetUIDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* BrightnessController;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Brightness;
	UFUNCTION()
	void OnBrightnessChanged(float Value);

};
