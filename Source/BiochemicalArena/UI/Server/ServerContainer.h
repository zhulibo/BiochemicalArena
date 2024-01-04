#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ServerContainer.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UServerContainer : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetStack* ServerStack;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UServer> ServerClass;
	UPROPERTY()
	UServer* Server;

protected:
	virtual void NativeConstruct() override;

};
