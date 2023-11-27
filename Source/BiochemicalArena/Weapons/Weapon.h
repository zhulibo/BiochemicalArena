#pragma once

#include "CoreMinimal.h"
#include "WeaponType.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& HitTarget);
	void DropWeapon();

	void SetWeaponState(EWeaponState State);
	void SetAreaSphereCollision();

	UPROPERTY(EditAnywhere)
	class USoundCue* UseWeaponSound;

	void SetAmmo(int32 AmmoNum);
	void SetCarriedAmmo(int32 AmmoNum);

	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireDelay = .1f;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool bAutomatic = true;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	EWeaponCate WeaponCate;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponName WeaponName;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon")
	EWeaponState WeaponState;

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float Damage = 60.f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class USphereComponent* AreaSphere;

	UPROPERTY()
	class AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int32 Ammo;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	int32 MagCapacity;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	int32 CarriedAmmo;
	int32 MaxCarriedAmmo;

	void SpendRound();
	void SetHUDAmmo();
	void SetHUDCarriedAmmo();

public:
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EWeaponCate GetWeaponCate() const { return WeaponCate; }
	FORCEINLINE EWeaponName GetWeaponName() const { return WeaponName; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	FORCEINLINE int32 GetMaxCarriedAmmo() const { return MaxCarriedAmmo; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }

};
