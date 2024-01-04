#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BagContent.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagContent : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UStorageButton* PrimaryEquipment;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* SecondaryEquipment;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* MeleeEquipment;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* ThrowingEquipment;

};
