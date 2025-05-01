#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EquipmentAnimInstance.generated.h"

enum class EEquipmentName : uint8;

UCLASS()
class BIOCHEMICALARENA_API UEquipmentAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEquipmentAnimInstance();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class AEquipment* Equipment;

};
