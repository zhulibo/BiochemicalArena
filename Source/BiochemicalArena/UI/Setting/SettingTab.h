#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "SettingTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USettingTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonActionWidget* LeftTabAction;
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* RightTabAction;

	UPROPERTY(EditInstanceOnly, Category = "UI")
	class UCommonActivatableWidgetSwitcher* SettingSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> TabButtonClass;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	TArray<class UCommonActivatableWidget*> TabContent;

	void LinkSwitcher();

};
