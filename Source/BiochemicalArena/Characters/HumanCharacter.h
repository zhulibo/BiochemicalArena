#pragma once

#include "CoreMinimal.h"
#include "CommonCharacter.h"
#include "CharacterComponents/CombatComponent.h"
#include "BiochemicalArena/Interfaces/CrosshairsInterface.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponents/CombatState.h"
#include "HumanCharacter.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanCharacter : public ACommonCharacter, public ICrosshairsInterface
{
	GENERATED_BODY()

public:
	AHumanCharacter();
	virtual void Tick(float DeltaTime) override;

	void EquipOverlappingWeapon(AWeapon* Weapon);

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	void SetDefaultWeapon();

protected:
	virtual void BeginPlay() override;
	void PollInit();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void DropButtonPressed(const FInputActionValue& Value);
	void SwapMainWeaponButtonPressed(const FInputActionValue& Value);
	void SwapSecondaryWeaponButtonPressed(const FInputActionValue& Value);
	void SwapMeleeWeaponButtonPressed(const FInputActionValue& Value);
	void SwapThrowingWeaponButtonPressed(const FInputActionValue& Value);
	void SwapLastWeaponButtonPressed(const FInputActionValue& Value);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	void PlayHitReactMontage();

private:
	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultMainWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultSecondaryWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultMeleeWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultThrowingWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* Combat;
	UPROPERTY(VisibleAnywhere)
	class UPickupComponent* Pickup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponsMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapMainWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapSecondaryWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapMeleeWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapThrowingWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapLastWeaponAction;

	UFUNCTION(Server, Reliable)
	void ServerDetectOverlappingWeapon();
	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponHandle(AWeapon* Weapon);
	UFUNCTION(Server, Reliable)
	void ServerDropButtonPressed();
	UFUNCTION(Server, Reliable)
	void ServerSwapWeaponHandle(EWeaponType NewWeaponType);

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Player")
	float MaxHealth = 200.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player")
	float Health = 200.f;
	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	AHumanController* HumanController;
	UPROPERTY()
	class AHumanState* HumanState;

	bool bElimmed = false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return CameraComponent; }
	bool IsWeaponEquipped();
	bool IsAiming();
	AWeapon* GetCurrentWeapon();
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FVector GetHitTarget() const;
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }

};
