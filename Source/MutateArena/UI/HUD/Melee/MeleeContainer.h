#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MeleeContainer.generated.h"

UCLASS()
class MUTATEARENA_API UMeleeContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	// HUD
	UPROPERTY(meta = (BindWidget))
	class UCommonHUD* CommonHUD;

	UPROPERTY(meta = (BindWidget))
	class UMeleeHUD* MeleeHUD;

	// 计分板
	UPROPERTY(meta = (BindWidget))
	class UScoreboard* Scoreboard;

	// 轮盘选择器
	UPROPERTY(meta = (BindWidget))
	class URadialMenuContainer* RadialMenuContainer;

};
