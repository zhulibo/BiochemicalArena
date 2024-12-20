#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MutationHuman.generated.h"

enum class ETeam : uint8;

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

protected:
	virtual void NativeOnInitialized() override;

	void OnHumanHealthChange(float TempHealth);
	void OnAmmoChange(int32 TempAmmo);
	void OnCarriedAmmoChange(int32 TempCarriedAmmo);
	void OnCause1000Damage();
	FTimerHandle DamageIconTimerHandle;
	void ClearDamageIcon();
	void OnDamageMulChange(float TempDamageMul);
	void OnBeImmune();

};
