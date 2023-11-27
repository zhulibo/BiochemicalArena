#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BiochemicalArena/Interfaces/CrosshairInterface.h"
#include "BiochemicalArena/Weapons/WeaponType.h"
#include "Components/CombatState.h"
#include "HumanCharacter.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanCharacter : public ABaseCharacter, public ICrosshairInterface
{
	GENERATED_BODY()

public:
	AHumanCharacter();
	virtual void Tick(float DeltaSeconds) override;

	template <typename EnumType>
	FName GetMetaData(EnumType enum_value, FString MetaDataName); // 获取枚举类型的元数据

	void EquipOverlappingWeapon(class AWeapon* Weapon);

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlaySwapMontage(EWeaponName NewWeaponName);

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	void SetDefaultWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Landed(const FHitResult& Hit) override;
	void PollInit();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComponent;

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void DropButtonPressed(const FInputActionValue& Value);
	void SwapPrimaryWeaponButtonPressed(const FInputActionValue& Value);
	void SwapSecondaryWeaponButtonPressed(const FInputActionValue& Value);
	void SwapMeleeWeaponButtonPressed(const FInputActionValue& Value);
	void SwapThrowingWeaponButtonPressed(const FInputActionValue& Value);
	void SwapLastWeaponButtonPressed(const FInputActionValue& Value);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();

private:
	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultPrimaryWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultSecondaryWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultMeleeWeaponClass;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> DefaultThrowingWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	UPROPERTY(VisibleAnywhere)
	class UPickupComponent* Pickup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* WeaponMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapPrimaryWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapSecondaryWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapMeleeWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapThrowingWeaponAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Weapon", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapLastWeaponAction;

	void DetectOverlappingWeapon();

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SwapMontage;

	UPROPERTY(EditAnywhere, Category = "Player")
	float MaxHealth = 200.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player")
	float Health = 200.f;
	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	class AHumanController* HumanController;
	UPROPERTY()
	class AHumanState* HumanState;

	bool bElimmed = false;
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	void ElimTimerFinished();

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return CameraComponent; }
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

template <typename EnumType>
FName AHumanCharacter::GetMetaData(EnumType enum_value, FString MetaDataName)
{
	FString EnumName = UEnum::GetValueAsString(enum_value);
	EnumName = EnumName.Left(EnumName.Find("::"));
	UEnum* Enum = FindObject<UEnum>(nullptr, *FString::Printf(TEXT("/Script/BIOCHEMICALARENA.%s"), *EnumName));
	return FName(Enum->GetMetaData(*MetaDataName, static_cast<int32>(enum_value)));
}
