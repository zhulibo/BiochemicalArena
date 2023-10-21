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
	void AddToDefeats(int32 DefeatsAmount);
	virtual void OnRep_Score() override;

	UFUNCTION()
	virtual void OnRep_Defeats();

private:
	UPROPERTY()
	class AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
