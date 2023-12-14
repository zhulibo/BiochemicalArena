#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PauseMenu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	class ABaseController* BaseController;

	UFUNCTION()
	void OnBackButtonClicked();
	void OnSettingButtonClicked();
	void OnKickButtonClicked();
	void OnQuitButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	UCommonButton* SettingButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* QuitButton;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class USetting> SettingClass;

};
