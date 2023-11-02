#pragma once

#include "CoreMinimal.h"
#include "CombatState.h"
#include "Components/ActorComponent.h"
#include "BiochemicalArena/HUD/HumanHUD.h"
#include "BiochemicalArena/Weapons/WeaponTypes.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIOCHEMICALARENA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class AHumanCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	void UseWeapon(AWeapon* WeaponToEquip);
	void SwapWeapon(EWeaponType NewWeaponType);
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBodySocket(AWeapon* ActorToAttach);
	void FireHandle(bool bPressed);

	void Reload();
	UFUNCTION(BlueprintCallable)
	void ShellReload();
	void JumpToShotgunEnd();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	AWeapon* GetCurrentWeapon();
	bool HasEquippedThisTypeWeapon(EWeaponType WeaponType);
	void SetCombatWeaponPointNull(EWeaponType WeaponType);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TraceUnderCrosshair(FHitResult& TraceHitResult);
	void SetHUDCrosshair(float DeltaTime);

	void SetAiming(bool bIsAiming);

	void Fire();

private:
	UPROPERTY()
	AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;
	UPROPERTY()
	AHumanHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;
	bool bAimButtonPressed = false;
	UFUNCTION()
	void OnRep_Aiming();

	bool bLocallyReloading = false;

	UPROPERTY(Replicated)
	AWeapon* MainWeapon;
	UPROPERTY(Replicated)
	AWeapon* SecondaryWeapon;
	UPROPERTY(Replicated)
	AWeapon* MeleeWeapon;
	UPROPERTY(Replicated)
	AWeapon* ThrowingWeapon;
	UPROPERTY(Replicated)
	EWeaponType CurrentWeaponType;
	UPROPERTY(Replicated)
	EWeaponType LastWeaponType;

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
	void InterpFOV(float DeltaTime);

	bool bFireButtonPressed;
	FTimerHandle FireTimer;
	bool bCanFire = true;
	void LoadNewBulletFinished();
	bool CanFire();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(AWeapon* WeaponToEquip);
	void LocalEquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUseWeapon(AWeapon* WeaponToUse);
	void LocalUseWeapon(AWeapon* WeaponToUse);
	void PlayEquipWeaponSound();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();

public:
	FORCEINLINE AWeapon* GetMainWeapon() const { return MainWeapon; }

};
