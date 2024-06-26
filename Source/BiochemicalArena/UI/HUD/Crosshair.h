#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Crosshair.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCrosshair : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonBorder* Crosshair_L;
	UPROPERTY(meta = (BindWidget))
	UCommonBorder* Crosshair_R;
	UPROPERTY(meta = (BindWidget))
	UCommonBorder* Crosshair_T;
	UPROPERTY(meta = (BindWidget))
	UCommonBorder* Crosshair_B;

	void SetCrosshairSpread(float CrosshairSpread);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UCanvasPanelSlot* Crosshair_L_Slot;
	UPROPERTY()
	UCanvasPanelSlot* Crosshair_R_Slot;
	UPROPERTY()
	UCanvasPanelSlot* Crosshair_T_Slot;
	UPROPERTY()
	UCanvasPanelSlot* Crosshair_B_Slot;

};
