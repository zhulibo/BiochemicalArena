#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Characters/AnimInstance_Mutant.h"
#include "AnimInstance_Smoker.generated.h"

UCLASS()
class MUTATEARENA_API UAnimInstance_Smoker : public UAnimInstance_Mutant
{
	GENERATED_BODY()

public:
	UAnimInstance_Smoker();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
