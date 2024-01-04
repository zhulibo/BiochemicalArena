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

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* AttackMontage;

	void EnableMeshCollision(bool bIsEnabled);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float LightAttackDamage = 100.f;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	float HeavyAttackDamage = 200.f;

	virtual void OnEquipped() override;
	void SetMeshCollision();

	UPROPERTY()
	TArray<AActor*> HitEnemies;
	UFUNCTION()
	virtual void OnMeshOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(Server, Reliable)
	void ServerApplyDamage(AActor* OtherActor, AHumanCharacter* InstigatorHumanCharacter, float DamageToApply);

};
