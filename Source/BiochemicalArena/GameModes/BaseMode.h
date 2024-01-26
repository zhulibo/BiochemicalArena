#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseMode.generated.h"

namespace MatchState
{
	extern BIOCHEMICALARENA_API const FName Cooldown;
}

UCLASS()
class BIOCHEMICALARENA_API ABaseMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABaseMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnMatchStateSet() override;

	float LevelStartTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 600.f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 5.f;

	float CountdownTime = 0.f;

	AActor* FindRandomPlayerStart(const FName& PlayerStartTag);

public:
	FORCEINLINE float GetLevelStartTime() const { return LevelStartTime; }
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }

};
