#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Common/ConfirmScreen.h"
#include "Menu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY()
	class AMenuController* MenuController;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* SettingButton;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USetting> SettingClass;
	void OnSettingButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UCommonButton* QuitButton;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UConfirmScreen> ConfirmScreenClass;
	void OnQuitButtonClicked();
	void Quit(EMsgResult MsgResult);

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UMenuTab* MenuTab;
	UPROPERTY(meta = (BindWidget))
	class UServerContainer* Server;
	UPROPERTY(meta = (BindWidget))
	class UStorage* Storage;
	UPROPERTY(meta = (BindWidget))
	class UShop* Shop;
	UPROPERTY(meta = (BindWidget))
	class UDev* Dev;

};
