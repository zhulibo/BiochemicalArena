#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Dev.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UDev : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

};
