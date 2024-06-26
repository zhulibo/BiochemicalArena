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

	virtual void EquipEquipment() override;

	void SetAttackCollisionEnabled(bool bIsEnabled);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* AttackCapsule;

	UPROPERTY(EditAnywhere)
	float LightAttackDamage;
	UPROPERTY(EditAnywhere)
	float HeavyAttackDamage;

	void SetAttackCapsuleCollision();

	UPROPERTY()
	TArray<AActor*> HitEnemies;
	UFUNCTION()
	virtual void OnAttackCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
