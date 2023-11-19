#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuTab.h"
#include "MenuSwitcher.h"
#include "TabWidget.h"
#include "MainStack.h"
#include "MenuContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UMenuTab* MenuTab;
	UPROPERTY(meta = (BindWidget))
	UMenuSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	UTabWidget* TabStorage;
	UPROPERTY(meta = (BindWidget))
	UTabWidget* TabServer;
	UPROPERTY(meta = (BindWidget))
	UTabWidget* TabShop;
	UPROPERTY(meta = (BindWidget))
	UMainStack* MainStack;

};
