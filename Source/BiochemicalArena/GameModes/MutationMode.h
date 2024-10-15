#pragma once

#include "CoreMinimal.h"
#include "BaseMode.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "MutationMode.generated.h"

namespace MatchState
{
	extern BIOCHEMICALARENA_API const FName PostRound;
}

UCLASS()
class BIOCHEMICALARENA_API AMutationMode : public ABaseMode
{
	GENERATED_BODY()

public:
	virtual bool IsMatchInProgress() const override;

	bool bWatchRoundState = false;
	virtual void StartMatch() override;
	void EndRound();
	void StartNextRound();

	bool bWatchMatchState = false;
	virtual void EndMatch() override;

	virtual void HumanReceiveDamage(AHumanCharacter* DamagedCharacter, ABaseController* DamagedController,
		float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser) override;

	void GetInfect(AHumanCharacter* DamagedCharacter, ABaseController* DamagedController,
		AMutantCharacter* AttackerCharacter, ABaseController* AttackerController, EMutantState MutantState);

	virtual void MutantReceiveDamage(AMutantCharacter* DamagedCharacter, ABaseController* DamagedController,
		float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser) override;

	void SelectMutant(ACharacter* Character, AController* Controller);
	void MutantRespawn(ACharacter* Character, AController* Controller);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class AMutationGameState* MutationGameState;

	UPROPERTY(EditAnywhere)
	int32 TotalRound = 12;
	int32 CurrentRound = 1;

	float RoundStartTime = 0.f;
	float RoundEndTime = 0.f;

	UPROPERTY(EditAnywhere)
	float WarmupTime = 5.f;
	UPROPERTY(EditAnywhere)
	float RoundTime = 180.f;
	UPROPERTY(EditAnywhere)
	float MutateTime = 25.f;
	UPROPERTY(EditAnywhere)
	float PostRoundTime = 5.f;
	UPROPERTY(EditAnywhere)
	float CooldownTime = 5.f;

	float CountdownTime = 0.f;

	bool bIsEndingMatch = false;

	virtual void OnMatchStateSet() override;
	virtual void HandleMatchHasStarted() override;
	virtual void OnPostLogin(AController* NewPlayer) override;

	void RandomMutate();
	void Mutate(ACharacter* Character, AController* Controller);
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> ChangeMutantEffect;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;
	FTimerHandle SpawnPickupTimerHandle;
	TArray<class APlayerStart*> PickupStartPoints;
	void SpawnPickups();

public:
	FORCEINLINE float GetTotalRound() const { return TotalRound; }
	FORCEINLINE int32 GetCurrentRound() const { return CurrentRound; }
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetRoundTime() const { return RoundTime; }
	FORCEINLINE float GetMutateTime() const { return MutateTime; }
	FORCEINLINE float GetPostRoundTime() const { return PostRoundTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }

};
