#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "HUDMutation.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UHUDMutation : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	void OnTeamChange(ETeam Team);
	void OnHUDStateChange(EHUDState HUDState);

	UPROPERTY(meta = (BindWidget))
	class UCommonHUD* CommonHUD;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UDynamicEntryBox> EntryBox_MutationHuman;
	UPROPERTY()
	TObjectPtr<class UMutationHuman> MutationHuman;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> EntryBox_MutationMutant;
	UPROPERTY()
	TObjectPtr<class UMutationMutant> MutationMutant;

	UPROPERTY(meta = (BindWidget))
	class UScoreboard* Scoreboard;

	UPROPERTY(meta = (BindWidget))
	class URadialMenuContainer* RadialMenuContainer;

};
