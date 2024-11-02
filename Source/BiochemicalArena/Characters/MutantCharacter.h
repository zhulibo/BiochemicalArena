#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Interfaces/InteractableTarget.h"
#include "MutantCharacter.generated.h"


UENUM(BlueprintType)
enum class EMutantState : uint8
{
	Ready,
	LightAttacking,
	HeavyAttacking,
};

enum class EMutantCharacterName : uint8;
enum class ESpawnReason : uint8;

UCLASS()
class BIOCHEMICALARENA_API AMutantCharacter : public ABaseCharacter, public IInteractableTarget
{
	GENERATED_BODY()

public:
	AMutantCharacter();

	virtual bool CanInteract() override;
	virtual void OnInteract(ABaseCharacter* BaseCharacter) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbilityBase> SkillAbility;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> SkillEffect;
	UPROPERTY()
	ESpawnReason SpawnReason;

	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDead(bool bKilledByMelee);

	virtual void RightHandAttackBegin();
	virtual void RightHandAttackEnd();
	virtual void LeftHandAttackBegin();
	virtual void LeftHandAttackEnd();

	UFUNCTION(Server, Reliable)
	void ServerSelectMutant(EMutantCharacterName TempMutantCharacterName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRepel(FVector ImpulseVector);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Destroyed() override;

	virtual void OnAbilitySystemComponentInit() override;
	void OnLocalSkillCooldownTagChanged(FGameplayTag GameplayTag, int32 TagCount);
	void OnLocalCharacterLevelChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* RightHandCapsule;
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* LeftHandCapsule;

	UPROPERTY()
	class AMutationMode* MutationMode;
	EMutantState MutantState;
	UPROPERTY()
	class AMutationController* MutationController;

	UPROPERTY()
	class UMutantAnimInstance* MutantAnimInstance;
	UPROPERTY(EditAnywhere)
	UAnimMontage* LightAttackMontage;
	UPROPERTY(EditAnywhere)
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputMutant> InputMutant;
	virtual void LightAttackButtonPressed(const FInputActionValue& Value);
	virtual void LightAttackButtonReleased(const FInputActionValue& Value);
	virtual void HeavyAttackButtonPressed(const FInputActionValue& Value);
	virtual void HeavyAttackButtonReleased(const FInputActionValue& Value);
	void SkillButtonPressed(const FInputActionValue& Value);

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
	float LightAttackDamage = 0.f;
	UPROPERTY()
	float HeavyAttackDamage = 0.f;

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
	virtual void DropBlood(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, float Damage);

	UFUNCTION()
	void MutantReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* AttackerController, AActor* DamageCauser);

	bool bSuckedDry = false;

};
