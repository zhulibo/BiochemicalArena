#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InfectMode.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AInfectMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;

public:
	virtual void PlayerEliminated(class AHumanCharacter* ElimmedCharacter, class AHumanController* VictimController, AHumanController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

};
