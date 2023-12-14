#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "InfectHuman.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInfectHuman : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamagePercent;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* WeaponAmmoText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CarriedAmmoText;

};
