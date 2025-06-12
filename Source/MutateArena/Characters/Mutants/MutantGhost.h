#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Characters/MutantCharacter.h"
#include "MutantGhost.generated.h"

UCLASS()
class MUTATEARENA_API AMutantGhost : public AMutantCharacter
{
	GENERATED_BODY()

public:
	AMutantGhost();
	
	void ShowOverheadWidget(bool bIsShow);

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

	virtual void OnAbilitySystemComponentInit() override;
	void OnJumpZVelocityChanged(const FOnAttributeChangeData& Data);
	
};
