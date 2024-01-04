#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PauseMenu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* SettingButton;
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class USetting> SettingClass;
	void OnSettingButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickButton;
	void OnKickButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* QuitButton;
	void OnQuitButtonClicked();

};
