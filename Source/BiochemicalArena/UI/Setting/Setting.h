#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Setting.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USetting : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	class AMenuController* MenuController;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class USettingTab* SettingTab;
	UPROPERTY(meta = (BindWidget))
	class UTabControl* TabControl;

};
