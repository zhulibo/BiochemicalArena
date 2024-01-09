#pragma once

#include "CoreMinimal.h"
#include "BaseController.h"
#include "HumanController.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API AHumanController : public ABaseController
{
	GENERATED_BODY()

public:
	void SetHasInitDefaultHUD(bool bHasInit);

	void SetHUDHealth(float Health);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDTeamScore(float Score, ETeam Team);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ATeamDeadMatchMode* TeamDeadMatchMode;
	UPROPERTY()
	class ATeamDeadMatchState* TeamDeadMatchState;
	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AHumanState* HumanState;

	bool bHasInitDefaultHUD = false;
	void InitDefaultHUD();

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeat;

};
