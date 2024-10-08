#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "Melee.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMelee : public AEquipment
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

	UFUNCTION()
	virtual void DropBlood(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, float Damage);

};
