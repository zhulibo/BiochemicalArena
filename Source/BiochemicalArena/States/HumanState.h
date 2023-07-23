#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HumanState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanState : public APlayerState
{
	GENERATED_BODY()

public:

private:
	UPROPERTY()
	class AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;

};
