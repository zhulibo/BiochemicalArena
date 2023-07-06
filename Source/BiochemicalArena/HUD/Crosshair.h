#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "Crosshair.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ACrosshair : public AHUD
{
	GENERATED_BODY()

public:
	// HUD绘制的主要调用
	virtual void DrawHUD() override;

protected:
	// 将被绘制在屏幕中心
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;

};
