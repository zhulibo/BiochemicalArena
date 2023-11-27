#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MenuStack;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonActivatableWidget> Menu;

};
