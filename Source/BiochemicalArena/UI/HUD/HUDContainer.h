#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "HUDContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UHUDContainer : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonHUD* CommonHUD;

	UPROPERTY(meta = (BindWidget))
	class UTeamDeadMatch* TeamDeadMatch;

	UPROPERTY(meta = (BindWidget))
	class UMutation* Mutation;
	UPROPERTY(meta = (BindWidget))
	class UMutationHuman* MutationHuman;
	UPROPERTY(meta = (BindWidget))
	class UMutationMutant* MutationMutant;

	UPROPERTY(meta = (BindWidget))
	class UScoreboard* Scoreboard;

	UPROPERTY(meta = (BindWidget))
	class URadialMenuContainer* RadialMenuContainer;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MainStack;

	void ShowPauseMenu();

protected:
	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UPauseMenu> PauseMenuClass;

};
