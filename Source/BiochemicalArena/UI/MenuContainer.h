#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MenuContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMenuContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* MainStack;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMenu> MenuClass;

};
