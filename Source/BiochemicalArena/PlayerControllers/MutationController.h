#pragma once

#include "CoreMinimal.h"
#include "BaseController.h"
#include "MutationController.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API AMutationController : public ABaseController
{
	GENERATED_BODY()

public:
	void OnTeamChange(ETeam TempTeam);

	void OnMatchStateSet(FName TempMatchState, int32 TempCurrentRound);

	virtual void SetHUDHealth(float Health) override;
	virtual void SetHUDAmmo(int32 Ammo) override;
	virtual void SetHUDCarriedAmmo(int32 Ammo) override;
	void SetHUDTeamNum(int32 TeamNum, ETeam Team);

	void Show1000DamageUI();

	void ShowSkillUI(bool bIsShow);

	void UpdateRageUI(float UpdateRageUI);

	void SetHUDDamageMul(float DamageMul);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY()
	class AMutationMode* MutationMode;
	UPROPERTY()
	class AMutationGameState* MutationGameState;
	UPROPERTY()
	class AMutationPlayerState* MutationPlayerState;

	UFUNCTION(Server, Reliable)
	void RequestServerMatchInfo();
	UFUNCTION(Client, Reliable)
	void ReturnServerMatchInfo(
		FName TempMatchState,
		int32 TempTotalRound,
		float TempLevelStartTime,
		float TempWarmupTime,
		float TempRoundTime,
		float TempMutateTime,
		float TempPostRoundTime,
		float TempCooldownTime
	);

	int32 TotalRound = 0;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentRound)
	int32 CurrentRound = 1;

	float RoundStartTime = 0.f;
	float RoundEndTime = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	float LevelStartTime = 0.f;
	float WarmupTime = 0.f;
	float RoundTime = 0.f;
	float MutateTime = 0.f;
	float PostRoundTime = 0.f;
	float CooldownTime = 0.f;

	int32 CountdownSeconds = 0;

	UFUNCTION()
	void OnRep_MatchState();
	void HandleMatchStateChange();

	UFUNCTION()
	void OnRep_CurrentRound();

	virtual void SetHUDTime() override;
	void SetHUDMatchCountdown(int32 CountdownTime);

	virtual void InitHUD() override;
	void InitHumanHUD();
	void InitMutantHUD();

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeat;

	void HandleRoundHasEnded();

	void SetHUDCurrentRound();
	void SetHUDTotalRound();

	FTimerHandle DamageIconTimerHandle;
	void Clear1000DamageUI();

};
