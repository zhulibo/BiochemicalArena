#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabAudio.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabAudio : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	void SetUIDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* VolumeController;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Volume;
	UFUNCTION()
	void OnVolumeChanged(float Value);

};
