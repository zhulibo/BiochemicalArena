#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponShotgun.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AWeaponShotgun : public AWeapon
{
	GENERATED_BODY()

public:
	AWeaponShotgun();

	virtual void Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ProjectileClass;

};
