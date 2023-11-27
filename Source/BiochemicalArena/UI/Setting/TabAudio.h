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
	UFUNCTION()
	void OnVolumeChanged(float Value);

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* VolumeValue;
	UPROPERTY(meta = (BindWidget))
	class UAnalogSlider* VolumeController;

};
