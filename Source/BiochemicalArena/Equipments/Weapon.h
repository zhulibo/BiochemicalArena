#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Weapon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* FireMontage_C;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* FireMontage_E;
	virtual void Fire(const FVector& HitTarget);

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* ReloadMontage_C;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* ReloadMontage_E;
	virtual void SetAmmo(int32 AmmoNum);
	virtual void SetCarriedAmmo(int32 AmmoNum);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	const USkeletalMeshSocket* MuzzleSocket;

	UPROPERTY(EditAnywhere)
	float AimingFOVFactor = 0.9; // 缩放倍数
	UPROPERTY(EditAnywhere)
	float AimSpeed = 30.f;

	UPROPERTY()
	int32 Ammo; // 当前弹匣子弹数量
	UPROPERTY(EditAnywhere, Category = "Equipment")
	int32 MagCapacity; // 弹匣最大容量
	int32 CarriedAmmo; // 当前携弹量
	UPROPERTY(EditAnywhere, Category = "Equipment")
	int32 MaxCarriedAmmo; // 最大携弹量

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float FireDelay = .1f;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsAutomatic = true;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float ProjectileSpeed = 100000;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	float Damage = 60.f;

	void SpendRound();
	void SetHUDAmmo();
	void SetHUDCarriedAmmo();

private:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<class ACasing> CasingClass;

public:
	FORCEINLINE float GetAimingFOVFactor() const { return AimingFOVFactor; }
	FORCEINLINE float GetAimSpeed() const { return AimSpeed; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	FORCEINLINE int32 GetMaxCarriedAmmo() const { return MaxCarriedAmmo; }

	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }

	FORCEINLINE float GetProjectileSpeed() const { return ProjectileSpeed; }
	FORCEINLINE float GetDamage() const { return Damage; }

	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }

};
