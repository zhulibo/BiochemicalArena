#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "MenuTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonActionWidget* LeftTabAction;
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* RightTabAction;

	UPROPERTY(EditInstanceOnly, Category = "UI")
	class UCommonActivatableWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> TabButtonClass;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	TArray<class UCommonActivatableWidget*> TabContent;
	void LinkSwitcher();

};
