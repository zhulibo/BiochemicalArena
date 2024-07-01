#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MutantAnimInstance.generated.h"

enum class EMutantCharacterName : uint8;

UCLASS()
class BIOCHEMICALARENA_API UMutantAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EMutantCharacterName MutantCharacterName; // used for idle animation

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class AMutantCharacter* MutantCharacter;

	UPROPERTY(BlueprintReadOnly)
	float Speed;
	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;
	UPROPERTY(BlueprintReadOnly)
	bool bIsDead;
	UPROPERTY(BlueprintReadOnly)
	float AimYaw;
	UPROPERTY(BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(BlueprintReadOnly)
	FRotator Head_Rotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator Neck_Rotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator UpperArm_R_Rotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator UpperArm_L_Rotator;

};
