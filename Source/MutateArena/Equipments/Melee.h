#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Melee.generated.h"

UCLASS()
class MUTATEARENA_API AMelee : public AEquipment
{
	GENERATED_BODY()

public:
	AMelee();

	UPROPERTY(EditAnywhere)
	UAnimMontage* LightAttackMontage_C;
	UPROPERTY(EditAnywhere)
	UAnimMontage* LightAttackMontage_E;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HeavyAttackMontage_C;
	UPROPERTY(EditAnywhere)
	UAnimMontage* HeavyAttackMontage_E;

	virtual void OnEquip() override;

	virtual void OnStartSwapOut() override;

	void SetAttackCollisionEnabled(bool bIsEnabled);

	void ClearHitEnemies();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* AttackCapsule;

	UPROPERTY()
	float LightAttackDamage;
	UPROPERTY()
	float HeavyAttackDamage;

	void SetAttackCapsuleCollision();

	UPROPERTY()
	TArray<AActor*> HitEnemies;
	UFUNCTION()
	virtual void OnAttackCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* OtherActor, AHumanCharacter* InstigatorCharacter, float Damage);
	UFUNCTION()
	virtual void DropBlood(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, float Damage);

};
