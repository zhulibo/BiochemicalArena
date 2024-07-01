#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BaseGameState.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABaseGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void AddToTeam(class ABasePlayerState* BasePlayerState, ETeam Team);
	virtual void RemoveFromTeam(ABasePlayerState* BasePlayerState, ETeam Team);
	TArray<ABasePlayerState*> GetTeam(ETeam Team);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAddKillLog(ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY(ReplicatedUsing = OnRep_Team1)
	TArray<ABasePlayerState*> Team1;
	UPROPERTY(ReplicatedUsing = OnRep_Team2)
	TArray<ABasePlayerState*> Team2;
	UFUNCTION()
	virtual void OnRep_Team1();
	UFUNCTION()
	virtual void OnRep_Team2();

};
