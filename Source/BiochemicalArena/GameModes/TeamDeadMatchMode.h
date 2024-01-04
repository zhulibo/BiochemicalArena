#pragma once

#include "CoreMinimal.h"
#include "BaseMode.h"
#include "TeamDeadMatchMode.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchMode : public ABaseMode
{
	GENERATED_BODY()

public:
	virtual void KillPlayer(class AHumanCharacter* KilledCharacter, class AHumanController* KilledController,
		AHumanController* AttackerController, AActor* DamageCauser);
	virtual void Respawn(ACharacter* KilledCharacter, AController* KilledController);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ATeamDeadMatchState* TeamDeadMatchState;
	UPROPERTY()
	UClass* HumanCharacterClass;

	virtual void HandleMatchHasStarted() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	AHumanCharacter* SpawnHumanCharacter(AController* KilledController);
	void AssignPlayerTeam(AHumanController* HumanController);

};
