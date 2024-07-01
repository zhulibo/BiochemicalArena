#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Shotgun.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AShotgun : public AWeapon
{
	GENERATED_BODY()

public:
	AShotgun();

	virtual void Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ProjectileClass;

	UPROPERTY(EditAnywhere)
	int32 PelletNum;

};
