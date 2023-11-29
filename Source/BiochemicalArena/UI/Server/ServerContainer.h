#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ServerContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServerContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* ServerStack;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UServer> ServerClass;

};
