#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GeneralWeapon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGeneralWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AGeneralWeapon();
	virtual void Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ProjectileClass;

};
