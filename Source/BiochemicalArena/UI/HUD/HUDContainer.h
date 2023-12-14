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

	void ShowPauseMenu();

	UPROPERTY(meta = (BindWidget))
	class UCommonHUD* CommonHUD;

	UPROPERTY(meta = (BindWidget))
	class UTeamDeadMatch* TeamDeadMatch;

	UPROPERTY(meta = (BindWidget))
	class UInfect* Infect;
	UPROPERTY(meta = (BindWidget))
	class UInfectHuman* InfectHuman;
	UPROPERTY(meta = (BindWidget))
	class UInfectZombie* InfectZombie;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidget* Scoreboard;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MainStack;

protected:
	UPROPERTY()
	class ABaseController* BaseController;

private:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UPauseMenu> PauseMenuClass;

};
