#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	void OnMatchStateSet(FName State);

	UPROPERTY()
	class UHUDContainer* HUDContainer;

	void ShowScoreboard(bool bIsShow);
	void ShowPauseMenu();
	void AddKillLog(class ABasePlayerState* AttackerState, const FString& EquipmentName, ABasePlayerState* KilledState);
	void ShowRadialMenu();
	void SelectRadialMenu(double X, double Y);
	void ChangeRadialMenu();
	void CloseRadialMenu();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ABaseMode* BaseMode;

	float ClientServerDelta = 0.f; // Difference between client and server time(not network delay)
	UPROPERTY(EditAnywhere, Category = Time)
	float RefreshFrequency = 5.f;
	void HandleClientServerDelta();
	UFUNCTION(Server, Reliable)
	void RequestServerTime(float TimeClientRequest);
	UFUNCTION(Client, Reliable)
	void ReturnServerTime(float TimeClientRequest, float TimeServerReceived);
	float GetServerTime();

	// Client join midgame, get match state from server.
	UFUNCTION(Server, Reliable)
	void RequestServerMatchState();
	UFUNCTION(Client, Reliable)
	void ReturnServerMatchState(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	float LevelStartTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	int32 CountdownSeconds = 0;
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	void AddHUDContainer();
	void SetHUDTime();
	void SetHUDWarmupCountdown(int32 CountdownTime);
	void SetHUDMatchCountdown(int32 CountdownTime);
	void HandleMatchHasStarted();
	void HandleCooldown();

};
