#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "MutantTank.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMutantTank : public AMutantCharacter
{
	GENERATED_BODY()

public:
	AMutantTank();

protected:
	virtual void BeginPlay() override;

	virtual void LightAttackButtonPressed(const FInputActionValue& Value) override;
	virtual void LightAttackButtonReleased(const FInputActionValue& Value) override;
	virtual void HeavyAttackButtonPressed(const FInputActionValue& Value) override;
	virtual void HeavyAttackButtonReleased(const FInputActionValue& Value) override;

	virtual void OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};
