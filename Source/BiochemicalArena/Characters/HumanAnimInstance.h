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
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	EEquipmentName EquipmentName; // used for equipment idle animation

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class AHumanCharacter* HumanCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouched;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsAiming;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsKilled;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float AimYaw;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float AimPitch;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator HeadRotator;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator NeckRotator;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator Arm_1_R_Rotator;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator Arm_1_L_Rotator;

};
