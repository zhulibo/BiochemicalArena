#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputSpectator.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInputSpectator : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* SpectatorMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* ViewNextAction;
	UPROPERTY(EditAnywhere)
	UInputAction* ViewPrevAction;

};
