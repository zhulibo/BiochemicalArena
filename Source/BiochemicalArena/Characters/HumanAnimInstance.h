#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HumanAnimInstance.generated.h"

enum class EEquipmentName : uint8;

UCLASS()
class BIOCHEMICALARENA_API UHumanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UHumanAnimInstance();

	UPROPERTY(BlueprintReadOnly)
	EEquipmentName EquipmentName; // used for equipment idle animation

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class AHumanCharacter* HumanCharacter;

	UPROPERTY(BlueprintReadOnly)
	float Speed;
	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;
	UPROPERTY(BlueprintReadOnly)
	float AimYaw;
	UPROPERTY(BlueprintReadOnly)
	float AimPitch;
	UPROPERTY(BlueprintReadOnly)
	FRotator Spine_01_Rotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator Spine_02_Rotator;
	UPROPERTY(BlueprintReadOnly)
	FRotator Spine_03_Rotator;

};
