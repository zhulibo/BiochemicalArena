#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HumanState.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddToScore(float ScoreAmount);
	void AddToDefeat(int32 DefeatAmount);
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeat();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;

public:
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
