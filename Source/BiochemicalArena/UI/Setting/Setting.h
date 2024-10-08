#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Setting.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USetting : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

};
