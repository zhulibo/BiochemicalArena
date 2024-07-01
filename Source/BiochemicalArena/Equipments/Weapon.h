#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Weapon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AWeapon : public AEquipment
{
	GENERATED_BODY()

public:
	AWeapon();

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireMontage_C;
	UPROPERTY(EditAnywhere)
	UAnimMontage* FireMontage_E;
	virtual void Fire(const FVector& HitTarget, float RecoilVert, float RecoilHor);

	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage_C;
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage_E;
	virtual void SetAmmo(int32 AmmoNum);
	virtual void SetCarriedAmmo(int32 AmmoNum);

	// 后坐力范围
	UPROPERTY(EditAnywhere)
	float RecoilMaxVert;
	UPROPERTY(EditAnywhere)
	float RecoilMinVert;
	UPROPERTY(EditAnywhere)
	float RecoilMaxHor;
	UPROPERTY(EditAnywhere)
	float RecoilMinHor;

	// 首发后坐力倍率
	UPROPERTY(EditAnywhere)
	float FirstShotRecoilMul;

	// 应用后坐力需要的时间
	UPROPERTY(EditAnywhere)
	float RecoilIncTime;

	// 总后坐力上限
	UPROPERTY(EditAnywhere)
	float RecoilTotalVertLimit;
	UPROPERTY(EditAnywhere)
	float RecoilTotalHorLimit;

	// 后坐力回复速度
	UPROPERTY(EditAnywhere)
	float RecoilDecSpeed;

	// 子弹散布，固定值，不受其他因素影响，固定存在
	UPROPERTY(EditAnywhere)
	float CenterSpread;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFullAmmo();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	const USkeletalMeshSocket* MuzzleSocket;

	UPROPERTY()
	float AimingFOVFactor = 0.9; // 缩放倍数
	UPROPERTY()
	float AimSpeed = 30.f;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo; // 最大携弹量
	UPROPERTY()
	int32 CarriedAmmo; // 当前携弹量

	UPROPERTY(EditAnywhere)
	int32 MagCapacity; // 弹匣容量
	UPROPERTY()
	int32 Ammo; // 当前弹匣子弹数量

	UPROPERTY(EditAnywhere)
	float FireRate;
	UPROPERTY(EditAnywhere)
	bool bIsAutomatic = true;

	UPROPERTY(EditAnywhere)
	float Damage;
	UPROPERTY(EditAnywhere)
	float Impulse;

	void SpendRound();
	void SetHUDAmmo();
	void SetHUDCarriedAmmo();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AShell> ShellClass;

public:
	FORCEINLINE float GetAimingFOVFactor() const { return AimingFOVFactor; }
	FORCEINLINE float GetAimSpeed() const { return AimSpeed; }

	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	FORCEINLINE int32 GetMaxCarriedAmmo() const { return MaxCarriedAmmo; }

	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; }
	FORCEINLINE bool IsFull() const { return Ammo == MagCapacity; }

	FORCEINLINE float GetDamage() const { return Damage; }

	FORCEINLINE float GetFireDelay() const { return 60 / FireRate; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }

};
