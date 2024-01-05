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

	virtual void Fire(const FVector& HitTarget) override;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	int32 PelletNum = 8;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	float SpreadDegrees = 10.f;

};
