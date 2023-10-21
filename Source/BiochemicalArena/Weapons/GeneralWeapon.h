#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GeneralWeapon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AGeneralWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

};
