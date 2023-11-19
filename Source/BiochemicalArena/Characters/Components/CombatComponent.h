#pragma once

#include "CoreMinimal.h"
#include "CombatState.h"
#include "Components/ActorComponent.h"
#include "BiochemicalArena/HUD/HumanHUD.h"
#include "BiochemicalArena/Weapons/WeaponType.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIOCHEMICALARENA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class AHumanCharacter;
	virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void SwapWeapon(EWeaponType NewWeaponType);
	void FinishSwapAttach(EWeaponType NewWeaponType);
	void FinishSwap();
	void FireHandle(bool bPressed);

	void Reload();
	void ShellReload();
	void JumpToShotgunEnd();
	void FinishReloading();
	void DropWeapon();

	AWeapon* GetCurrentWeapon();
	AWeapon* GetWeaponByType(EWeaponType WeaponType);
	bool HasEquippedThisTypeWeapon(EWeaponType WeaponType);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshair(float DeltaSeconds);

	void SetAiming(bool bIsAiming);

	void Fire();

private:
	UPROPERTY()
	AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;
	UPROPERTY()
	AHumanHUD* HUD;
	UPROPERTY()
	class USoundCue* ClickSound;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;
	bool bAimButtonPressed = false;
	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(Replicated)
	AWeapon* PrimaryWeapon;
	UPROPERTY(Replicated)
	AWeapon* SecondaryWeapon;
	UPROPERTY(Replicated)
	AWeapon* MeleeWeapon;
	UPROPERTY(Replicated)
	AWeapon* ThrowingWeapon;
	UPROPERTY(Replicated)
	EWeaponType CurrentWeaponType = EWeaponType::MAX;
	UPROPERTY(Replicated)
	EWeaponType LastWeaponType = EWeaponType::MAX;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FVector HitTarget;
	FHUDPackage HUDPackage;

	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;
	void InterpFOV(float DeltaSeconds);

	bool bFireButtonPressed;
	FTimerHandle FireTimer;
	bool bCanFire = true;
	void LoadNewBulletFinished();
	bool CanFire();

	UPROPERTY(Replicated)
	ECombatState CombatState = ECombatState::MAX;

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerEquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(AWeapon* WeaponToEquip);
	void LocalEquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(Server, Reliable)
	void ServerSwapWeapon(EWeaponType NewWeaponType);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwapWeapon(EWeaponType NewWeaponType);
	void LocalSwapWeapon(EWeaponType NewWeaponType);
	void UseWeapon(AWeapon* WeaponToUse);
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBodySocket(AWeapon* ActorToAttach);
	void PlayUseWeaponSound();
	UFUNCTION(Server, Reliable)
	void ServerDropWeapon();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDropWeapon();
	void LocalDropWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReload();

public:
	FORCEINLINE AWeapon* GetPrimaryWeapon() const { return PrimaryWeapon; }

};
