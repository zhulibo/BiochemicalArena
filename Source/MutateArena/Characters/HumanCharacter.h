#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "MutantCharacter.h"
#include "Interfaces/InteractableTarget.h"
#include "HumanCharacter.generated.h"

enum class EEquipmentName : uint8;
enum class EEquipmentType : uint8;
enum class ECombatState : uint8;

UCLASS()
class MUTATEARENA_API AHumanCharacter : public ABaseCharacter, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AHumanCharacter();

	void EquipOverlappingEquipment(class AEquipment* Equipment);

	UFUNCTION(Server, Reliable)
	void ServerGivePickupEquipment(class APickupEquipment* PickupEquipment);

	void SwapPrimaryEquipmentButtonPressed();
	void SwapSecondaryEquipmentButtonPressed();
	void SwapMeleeEquipmentButtonPressed();
	void SwapThrowingEquipmentButtonPressed();

	void TrySwitchLoadout();
	UPROPERTY()
	bool bCanSwitchLoadout = true;

	void OnServerDropEquipment();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastMutationDead(bool bNeedSpawn);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMeleeDead();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTeamDeadMatchDead();

	UPROPERTY(ReplicatedUsing = OnRep_bIsImmune)
	bool bIsImmune = false;
	UFUNCTION()
	virtual void OnInteractMutantSuccess(class AMutantCharacter* MutantCharacter) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void UnPossessed() override;
	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	class URecoilComponent* RecoilComponent;
	UPROPERTY(VisibleAnywhere)
	class UCrosshairComponent* CrosshairComponent;

	UPROPERTY()
	class AMutationMode* MutationMode;
	UPROPERTY()
	class AMeleeMode* MeleeMode;
	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class AMeleeGameState* MeleeGameState;

	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
public:
	void FireButtonPressed(const FInputActionValue& Value);
protected:
	void FireButtonReleased(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void DropButtonPressed(const FInputActionValue& Value);
	void SwapLastEquipmentButtonPressed(const FInputActionValue& Value);


	virtual void OnControllerReady() override;

	void ApplyLoadout();
	UFUNCTION(Server, Reliable)
	void ServerSpawnEquipments(EEquipmentName Primary, EEquipmentName Secondary, EEquipmentName Melee, EEquipmentName Throwing);
	EEquipmentName GetEquipmentName(int32 LoadoutIndex, EEquipmentType EquipmentType);

	UPROPERTY(ReplicatedUsing = OnRep_DefaultPrimary)
	class AWeapon* DefaultPrimary;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultSecondary)
	AWeapon* DefaultSecondary;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultMelee)
	class AMelee* DefaultMelee;
	UPROPERTY(ReplicatedUsing = OnRep_DefaultThrowing)
	class AThrowing* DefaultThrowing;
	UFUNCTION()
	void OnRep_DefaultPrimary();
	UFUNCTION()
	void OnRep_DefaultSecondary();
	UFUNCTION()
	void OnRep_DefaultMelee();
	UFUNCTION()
	void OnRep_DefaultThrowing();

	UFUNCTION()
	void HumanReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* AttackerController, AActor* DamageCauser);
	void HandleDead();
	
	UFUNCTION(Server, Reliable)
	void ServerOnImmune(AMutantCharacter* MutantCharacter);
	UFUNCTION()
	void OnRep_bIsImmune();

public:
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE URecoilComponent* GetRecoilComponent() const { return RecoilComponent; }
	FORCEINLINE UCrosshairComponent* GetCrosshairComponent() const { return CrosshairComponent; }
	FVector GetHitTarget() const;
	FORCEINLINE bool IsImmune() const { return bIsImmune; }

};
