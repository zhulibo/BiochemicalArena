#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "SettingTab.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USettingTab : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void LinkSwitcher();

private:
	UPROPERTY(EditInstanceOnly, Category = "UI")
	class UCommonActivatableWidgetSwitcher* SettingSwitcher;
	UPROPERTY(EditInstanceOnly, Category = "UI")
	TArray<class UCommonActivatableWidget*> TabContent;

	UPROPERTY(meta = (BindWidget))
	class UCommonActionWidget* LeftTabAction;
	UPROPERTY(meta = (BindWidget))
	UCommonActionWidget* RightTabAction;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* TabButtonContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> TabButtonClass;

};
