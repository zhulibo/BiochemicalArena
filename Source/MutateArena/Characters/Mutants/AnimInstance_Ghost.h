#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Characters/AnimInstance_Mutant.h"
#include "AnimInstance_Ghost.generated.h"

UCLASS()
class MUTATEARENA_API UAnimInstance_Ghost : public UAnimInstance_Mutant
{
	GENERATED_BODY()

public:
	UAnimInstance_Ghost();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
