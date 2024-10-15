#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseController.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FChangeCrosshairSpread, float Spread);
DECLARE_MULTICAST_DELEGATE_OneParam(FChangeAnnouncement, FText Text);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchCountdownChange, int32 CountdownTime);
DECLARE_MULTICAST_DELEGATE_OneParam(FShowScoreboard, bool bIsShow);
DECLARE_MULTICAST_DELEGATE(FShowPauseMenu);
DECLARE_MULTICAST_DELEGATE_OneParam(FShowRadialMenu, bool bIsShow);
DECLARE_MULTICAST_DELEGATE(FChangeRadialMenu);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSelectRadialMenu, double X, double Y);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKillStreakChange, int32 KillStreak);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHumanHealthChange, float Health);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMutantHealthChange, float Health);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChange, int32 Ammo);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCarriedAmmoChange, int32 CarriedAmmo);

UCLASS()
class BIOCHEMICALARENA_API ABaseController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UGameLayout* GameLayout;

	void FocusGame();
	void FocusUI();

	FChangeCrosshairSpread ChangeCrosshairSpread;
	FChangeAnnouncement ChangeAnnouncement;
	FOnMatchCountdownChange OnMatchCountdownChange;
	FShowScoreboard ShowScoreboard;
	FShowPauseMenu ShowPauseMenu;
	FShowRadialMenu ShowRadialMenu;
	FChangeRadialMenu ChangeRadialMenu;
	FSelectRadialMenu SelectRadialMenu;
	FOnKillStreakChange OnKillStreakChange;
	FOnHumanHealthChange OnHumanHealthChange;
	FOnMutantHealthChange OnMutantHealthChange;
	FOnAmmoChange OnAmmoChange;
	FOnCarriedAmmoChange OnCarriedAmmoChange;

	virtual void ManualReset();

	virtual void SetHUDHealth(float Health) {}

	virtual void SetHUDAmmo(int32 Ammo);
	virtual void SetHUDCarriedAmmo(int32 CarriedAmmo);

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
	TSubclassOf<UGameLayout> GameLayoutClass;
	void AddGameLayout();
	void SetHUDWarmupCountdown(int32 CountdownTime);

	virtual void HandleMatchHasStarted();
	virtual void HandleMatchHasEnded();

	virtual void InitHUD() {}
	virtual void SetHUDTime() {}
	
};
