#pragma once

#include "CoreMinimal.h"
#include "BaseMode.h"
#include "TeamDeadMatchMode.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ATeamDeadMatchMode : public ABaseMode
{
	GENERATED_BODY()

public:
	bool bWatchMatchState = false;
	virtual void EndMatch() override;

	virtual void HumanReceiveDamage(AHumanCharacter* DamagedCharacter, ABaseController* DamagedController,
		float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser) override;

	void HumanRespawn(ACharacter* Character, AController* Controller);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ATeamDeadMatchGameState* TeamDeadMatchGameState;

	UPROPERTY(EditAnywhere)
	float WarmupTime = 5.f;
	UPROPERTY(EditAnywhere)
	float MatchTime = 600.f;
	UPROPERTY(EditAnywhere)
	float CooldownTime = 5.f;

	float CountdownTime = 0.f;

	bool bIsEndingMatch = false;

	virtual void OnMatchStateSet() override;
	virtual void HandleMatchHasStarted() override;
	virtual void OnPostLogin(AController* NewPlayer) override;

public:
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }

};
