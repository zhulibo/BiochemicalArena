#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Characters/AnimInstance_Mutant.h"
#include "AnimInstance_Tank.generated.h"

UCLASS()
class MUTATEARENA_API UAnimInstance_Tank : public UAnimInstance_Mutant
{
	GENERATED_BODY()

public:
	UAnimInstance_Tank();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
