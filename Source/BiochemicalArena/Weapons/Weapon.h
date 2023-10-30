#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
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
	virtual void OnRep_Owner() override;

	void SetWeaponState(EWeaponState State);
	void SetAreaSphereCollision();

	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

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

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairsBottom;

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EWeaponCate WeaponCate;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon")
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UAnimationAsset* FireAnimation;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	int32 MagCapacity;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Weapon")
	int32 Ammo;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CarriedAmmo, Category = "Weapon")
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_Ammo();
	UFUNCTION()
	void OnRep_CarriedAmmo();

	void SetHUDAmmo();
	void SetHUDCarriedAmmo();

public:
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EWeaponCate GetWeaponCate() const { return WeaponCate; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }

};
