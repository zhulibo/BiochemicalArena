#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UHUDContainer* HUDContainer;

	virtual void ManualReset();
	virtual void SetHUDHealth(float Health) {}

	virtual void SetHUDAmmo(int32 Ammo) {}
	virtual void SetHUDCarriedAmmo(int32 Ammo) {}

	void ShowScoreboard(bool bIsShow);
	void ShowPauseMenu();
	void AddKillLog(class ABasePlayerState* AttackerState, const FString& CauserName, ABasePlayerState* DamagedState);
	void ShowRadialMenu();
	void SelectRadialMenu(double X, double Y);
	void ChangeRadialMenu();
	void CloseRadialMenu();

	void SetHUDCrosshair(float CrosshairSpread);

	void ShowKillStreak(int32 KillStreak);
	void HiddenKillStreak();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ABaseMode* BaseMode;
	UPROPERTY()
	class ABaseCharacter* BaseCharacter;

	float ServerClientDeltaTime = 0.f; // Difference between client and server time(not network delay)
	void HandleServerClientDeltaTime();
	UFUNCTION(Server, Reliable)
	void RequestServerTime(float ClientTime);
	UFUNCTION(Client, Reliable)
	void ReturnServerTime(float ClientTime, float ServerTime);
	float GetServerTime();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UHUDContainer> HUDContainerClass;
	void AddHUDContainer();
	void SetHUDWarmupCountdown(int32 CountdownTime);
	virtual void HandleMatchHasStarted();
	virtual void HandleMatchHasEnded();

	bool bNeedInitHUD = false;
	virtual void InitHUD() {}
	virtual void SetHUDTime() {}

};
