#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MutationHuman.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutationHuman : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamagePercent;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ammo;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CarriedAmmo;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamageIcon;

};
