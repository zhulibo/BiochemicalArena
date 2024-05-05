#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Dev.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UDev : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

};
