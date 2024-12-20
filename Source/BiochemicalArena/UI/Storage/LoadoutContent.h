#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "LoadoutContent.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ULoadoutContent : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UStorageButton* Primary;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* Secondary;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* Melee;
	UPROPERTY(meta = (BindWidget))
	UStorageButton* Throwing;

};
