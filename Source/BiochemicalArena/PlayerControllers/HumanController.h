#pragma once

#include "CoreMinimal.h"
#include "BaseController.h"
#include "HumanController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AHumanController : public ABaseController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDTeamScore(float Score, int32 Team);

protected:
	virtual void BeginPlay() override;

	void InitDefaultHUD();

private:
	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class ATeamDeadMatchState* TeamDeadMatchState;
	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AHumanState* HumanState;

	bool bHasInitDefaultHUD = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeat;

};
