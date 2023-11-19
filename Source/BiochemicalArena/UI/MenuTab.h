#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "CommonActionWidget.h"
#include "MenuSwitcher.h"
#include "TabWidget.h"
#include "TabButton.h"
#include "Components/HorizontalBox.h"
#include "MenuTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void LinkSwitcher();

private:
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* LeftTabAction;
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* RightTabAction;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* ButtonContainer;

	UPROPERTY(EditInstanceOnly, Category = "UI")
	UMenuSwitcher* MenuSwitcher;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	UTabWidget* TabStorage;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	UTabWidget* TabServer;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	UTabWidget* TabShop;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTabButton> TabButtonClass;

};
