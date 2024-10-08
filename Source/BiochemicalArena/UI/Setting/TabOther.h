#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabOther.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabOther : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	void SetUISavedValue();

};
