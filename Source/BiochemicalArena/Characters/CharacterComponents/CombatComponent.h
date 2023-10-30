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
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void ShellReload();
	void JumpToShotgunEnd();

	AWeapon* GetCurrentWeapon();
	bool HasEquippedThisTypeWeapon(EWeaponType WeaponType);
	void SetCombatWeaponPointNull(EWeaponType WeaponType);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHUDCrosshairs(float DeltaTime);

	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(AWeapon* WeaponToEquip);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUseWeapon(AWeapon* WeaponToUse);
	void PlayEquipWeaponSound();

	void Fire();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();
	void HandleReload();

private:
	UPROPERTY()
	AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;
	UPROPERTY()
	AHumanHUD* HUD;

	UPROPERTY(Replicated)
	bool bAiming;

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

};
