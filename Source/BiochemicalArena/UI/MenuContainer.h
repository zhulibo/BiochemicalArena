#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuTab.h"
#include "MenuSwitcher.h"
#include "Common/MainStack.h"
#include "Input/CommonBoundActionBar.h"
#include "MenuContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void AddSettingActionButton();

private:
	UPROPERTY(meta = (BindWidget))
	UMenuTab* MenuTab;
	UPROPERTY(meta = (BindWidget))
	UMenuSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Storage;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Server;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Shop;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidget* Dev;

	UPROPERTY(meta = (BindWidget))
	UCommonBoundActionBar* ActionBar;
	UPROPERTY(meta = (BindWidget))
	UMainStack* MainStack;

};
