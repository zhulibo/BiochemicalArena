#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "MutantCharacter.h"
#include "HumanCharacter.generated.h"

enum class EEquipmentType : uint8;
enum class ECombatState : uint8;

UCLASS()
class BIOCHEMICALARENA_API AHumanCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AHumanCharacter();

	virtual void OnControllerReady() override;

	void EquipOverlappingEquipment(class AEquipment* Equipment);

	UFUNCTION(Server, Reliable)
	void ServerGivePickupEquipment(class APickupEquipment* PickupEquipment);

	void SwapPrimaryEquipmentButtonPressed();
	void SwapSecondaryEquipmentButtonPressed();
	void SwapMeleeEquipmentButtonPressed();
	void SwapThrowingEquipmentButtonPressed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTeamDeadMatchDead();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMutationDead();

	void GetInfect(AMutantCharacter* AttackerCharacter, ABaseController* AttackerController, EMutantState MutantState);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void UnPossessed() override;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere)
	class URecoilComponent* RecoilComponent;
	UPROPERTY(VisibleAnywhere)
	class UCrosshairComponent* CrosshairComponent;

	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class AMutationMode* MutationMode;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputHuman> InputHuman;
	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void ReloadButtonPressed(const FInputActionValue& Value);
	void DropButtonPressed(const FInputActionValue& Value);
	void SwapLastEquipmentButtonPressed(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerDetectOverlappingEquipment();

	UFUNCTION(Server, Reliable)
	void ServerSetDefaultEquipment(const FString& PrimaryName,
		const FString& SecondaryName, const FString& MeleeName, const FString& ThrowingName);
	FString GetEquipmentName(int32 CurLoadoutIndex, EEquipmentType EquipmentType);

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

	UFUNCTION()
	void HumanReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* AttackerController, AActor* DamageCauser);

public:
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE URecoilComponent* GetRecoilComponent() const { return RecoilComponent; }
	FORCEINLINE UCrosshairComponent* GetCrosshairComponent() const { return CrosshairComponent; }
	AEquipment* GetCurrentEquipment();
	FVector GetHitTarget() const;

};
