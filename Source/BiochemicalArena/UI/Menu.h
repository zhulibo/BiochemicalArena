#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Menu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void OnSettingButtonClicked();
	void OnQuitButtonClicked();

private:
	UPROPERTY()
	class AMenuController* MenuController;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SettingButton;
	UPROPERTY(meta = (BindWidget))
	UCommonButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UMenuTab* MenuTab;
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Storage;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Server;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Shop;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Dev;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class USetting> SettingClass;

};
