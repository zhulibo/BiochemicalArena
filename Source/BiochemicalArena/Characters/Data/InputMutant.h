#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputMutant.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInputMutant : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* MutantMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* LightAttackAction;
	UPROPERTY(EditAnywhere)
	UInputAction* HeavyAttackAction;
	UPROPERTY(EditAnywhere)
	UInputAction* SkillAction;

};
