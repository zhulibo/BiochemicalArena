#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TeamDeadMatchMode.generated.h"

namespace MatchState
{
	extern BIOCHEMICALARENA_API const FName Cooldown;
}

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATeamDeadMatchMode();
	virtual void Tick(float DeltaSeconds) override;

	virtual void PlayerEliminated(class AHumanCharacter* ElimmedCharacter, class AHumanController* VictimController,
		AHumanController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	float LevelStartTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 600.f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 5.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }

};
