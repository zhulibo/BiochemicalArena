#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ShowScoreboard(bool bIsShow);
	void ShowPauseMenu();
	void AddKillLog(class ABasePlayerState* AttackerState, const FString& WeaponName, ABasePlayerState* KilledState);
	void ShowRadialMenu();
	void CloseRadialMenu();
	void ChangeRadialMenu();
	void SelectRadialMenu(double X, double Y);

	void OnMatchStateSet(FName State);

	UPROPERTY()
	class UHUDContainer* HUDContainer;

protected:
	virtual void BeginPlay() override;

	float GetServerTime();

	void HandleMatchHasStarted();
	void HandleCooldown();
	void SetHUDTime();
	void SetHUDWarmupCountdown(uint32 CountdownTime);
	void SetHUDMatchCountdown(uint32 CountdownTime);

	float ClientServerDelta = 0.f; // difference between client and server time(not network delay)
	UPROPERTY(EditAnywhere, Category = Time)
	float RefreshFrequency = 5.f;
	void HandleClientServerDelta();
	UFUNCTION(Server, Reliable)
	void RequestServerTime(float TimeClientRequest);
	UFUNCTION(Client, Reliable)
	void ReturnServerTime(float TimeClientRequest, float TimeServerReceived);

	// Client join midgame, get match state from server.
	UFUNCTION(Server, Reliable)
	void RequestServerMatchState();
	UFUNCTION(Client, Reliable)
	void ReturnServerMatchState(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	void AddHUDContainer();

private:
	UPROPERTY()
	class ABaseMode* BaseMode;

	float LevelStartTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownSeconds = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

};
