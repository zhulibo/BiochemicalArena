#pragma once

#include "CoreMinimal.h"
#include "MutateArena/Characters/AnimInstance_Mutant.h"
#include "AnimInstance_Cutter.generated.h"

UCLASS()
class MUTATEARENA_API UAnimInstance_Cutter : public UAnimInstance_Mutant
{
	GENERATED_BODY()

public:
	UAnimInstance_Cutter();

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
