#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PauseMenu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPauseMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY()
	bool bWantToBack = true;
	UFUNCTION()
	void OnDeactivatedInternal();

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* LoadoutSelectButton;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ULoadoutSelect> LoadoutSelectClass;
	void OnLoadoutSelectClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* MutantSelectButton;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMutantSelect> MutantSelectClass;
	void OnMutantSelectButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* SettingButton;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USetting> SettingClass;
	void OnSettingButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* KickButton;
	void OnKickButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* QuitButton;
	void OnQuitButtonClicked();

};
