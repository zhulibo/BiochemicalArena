#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BagContent.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagContent : public UCommonUserWidget
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
