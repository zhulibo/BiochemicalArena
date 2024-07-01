#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "MutantCharacter.generated.h"

UENUM(BlueprintType)
enum class EMutantState : uint8
{
	Ready UMETA(DisplayName = "Ready"),
	LightAttacking UMETA(DisplayName = "LightAttacking"),
	HeavyAttacking UMETA(DisplayName = "HeavyAttacking"),
};

enum class EMutantCharacterName : uint8;

UCLASS()
class BIOCHEMICALARENA_API AMutantCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMutantCharacter();

	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDead();

	virtual void RightHandAttackBegin();
	virtual void RightHandAttackEnd();
	virtual void LeftHandAttackBegin();
	virtual void LeftHandAttackEnd();

	UFUNCTION(Server, Reliable)
	void ServerSelectCharacter(const FString& CharacterName);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void UnPossessed() override;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* RightHandCapsule;
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* LeftHandCapsule;

	UPROPERTY()
	class AMutationMode* MutationMode;
	EMutantState MutantState;

	UPROPERTY()
	class UMutantAnimInstance* MutantAnimInstance;
	UPROPERTY(EditAnywhere)
	UAnimMontage* LightAttackMontage;
	UPROPERTY(EditAnywhere)
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Input | Mutant")
	UInputMappingContext* MutantMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input | Mutant")
	UInputAction* LightAttackAction;
	UPROPERTY(EditAnywhere, Category = "Input | Mutant")
	UInputAction* HeavyAttackAction;

	virtual void LightAttackButtonPressed(const FInputActionValue& Value);
	virtual void LightAttackButtonReleased(const FInputActionValue& Value);
	virtual void HeavyAttackButtonPressed(const FInputActionValue& Value);
	virtual void HeavyAttackButtonReleased(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerLightAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLightAttack();
	void LocalLightAttack();

	UFUNCTION(Server, Reliable)
	void ServerHeavyAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHeavyAttack();
	void LocalHeavyAttack();

	UPROPERTY()
	float LightAttackDamage;
	UPROPERTY()
	float HeavyAttackDamage;

	UPROPERTY()
	TArray<AActor*> RightHandHitEnemies;
	UPROPERTY()
	TArray<AActor*> LeftHandHitEnemies;

	UFUNCTION()
	virtual void OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void MutantReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* AttackerController, AActor* DamageCauser);

};
