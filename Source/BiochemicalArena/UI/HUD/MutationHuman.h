#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MutationHuman.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutationHuman : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamageMul;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ammo;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CarriedAmmo;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamageIcon;

};
