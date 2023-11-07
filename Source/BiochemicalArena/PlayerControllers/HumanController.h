#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HumanController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime();

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	void SetHUDWarmupCountdown(float CountdownTime);
	void SetHUDMatchCountdown(float CountdownTime);

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);

protected:
	virtual void BeginPlay() override;

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

	void InitDefaultHUD();
	void SetHUDTime();

private:
	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class ATeamDeadMatchState* TeamDeadMatchState;
	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AHumanState* HumanState;
	UPROPERTY()
	class AHumanHUD* HumanHUD;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	UFUNCTION()
	void OnRep_MatchState();

	float LevelStartTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownSeconds = 0;

	bool bHasInitDefaultHUD = false;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;

};
