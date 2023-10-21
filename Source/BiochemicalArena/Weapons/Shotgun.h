#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Shotgun.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AShotgun : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	uint32 PelletNum = 8;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float SpreadDegrees = 10.f;

};
