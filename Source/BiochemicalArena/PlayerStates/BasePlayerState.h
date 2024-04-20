#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasePlayerState();

	void SetTeam(ETeam TempTeam);

	UFUNCTION(Server, Reliable)
	void ServerSetSpawnCharacterName(const FString& TempSpawnCharacterName);

	void AddScore(float ScoreAmount);
	void AddDefeat(int32 DefeatAmount);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere, Category = "Player")
	ETeam Team;
	UFUNCTION()
	void OnRep_Team();

	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	FString SpawnCharacterName; // 角色名字，开局生成角色时使用

	virtual void OnRep_Score() override;

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;
	UFUNCTION()
	void OnRep_Defeat();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE FString GetSpawnCharacterName() const { return SpawnCharacterName; }
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
