#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabAudio.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabAudio : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void SetDefaultValue();

	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* VolumeController;
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* VolumeValue;
	UFUNCTION()
	void OnVolumeChanged(float Value);

};
