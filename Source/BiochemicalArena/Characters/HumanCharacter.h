#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BiochemicalArena/Interfaces/CrosshairInterface.h"
#include "HumanCharacter.generated.h"

enum class EEquipmentType : uint8;
enum class ECombatState : uint8;

UCLASS()
class BIOCHEMICALARENA_API AHumanCharacter : public ABaseCharacter, public ICrosshairInterface
{
	GENERATED_BODY()

public:
	AHumanCharacter();

	void EquipOverlappingEquipment(class AEquipment* Equipment);

	void SwapPrimaryEquipmentButtonPressed();
	void SwapSecondaryEquipmentButtonPressed();
	void SwapMeleeEquipmentButtonPressed();
	void SwapThrowingEquipmentButtonPressed();

	void Kill();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	void PollInit();

	UFUNCTION(Server, Reliable)
	void ServerSetDefaultEquipment(const FString& PrimaryEquipmentName,
		const FString& SecondaryEquipmentName, const FString& MeleeEquipmentName, const FString& ThrowingEquipmentName);
	FString GetEquipmentName(int32 BagIndex, EEquipmentType EquipmentType);
	FString GetEquipmentClassPath(FString EquipmentName);
	UPROPERTY(ReplicatedUsing = OnRep_DefaultPrimaryEquipment)
	class AWeapon* DefaultPrimaryEquipment;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultSecondaryEquipment)
	AWeapon* DefaultSecondaryEquipment;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultMeleeEquipment)
	class AMelee* DefaultMeleeEquipment;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultThrowingEquipment)
	class AThrowing* DefaultThrowingEquipment;
	UFUNCTION()
	void OnRep_DefaultPrimaryEquipment();
	UFUNCTION()
	void OnRep_DefaultSecondaryEquipment();
	UFUNCTION()
	void OnRep_DefaultMeleeEquipment();
	UFUNCTION()
	void OnRep_DefaultThrowingEquipment();

	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class AHumanController* HumanController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* EquipmentMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapPrimaryEquipmentAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapSecondaryEquipmentAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapMeleeEquipmentAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapThrowingEquipmentAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Equipment", meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapLastEquipmentAction;

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void DropButtonPressed(const FInputActionValue& Value);
	void SwapLastEquipmentButtonPressed(const FInputActionValue& Value);

	void DetectOverlappingEquipment();

	UPROPERTY(EditAnywhere, Category = "Player")
	float MaxHealth = 200.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player")
	float Health = MaxHealth;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	UFUNCTION()
	void OnRep_Health();

	bool bIsKilled = false;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastKill();
	FTimerHandle KillTimer;
	UPROPERTY(EditDefaultsOnly)
	float KillDelay = 3.f;
	void KillTimerFinished();

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	ECombatState GetCombatState() const;

	bool IsAiming();
	AEquipment* GetCurrentEquipment();
	FVector GetHitTarget() const;

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool IsKilled() const { return bIsKilled; }

};
