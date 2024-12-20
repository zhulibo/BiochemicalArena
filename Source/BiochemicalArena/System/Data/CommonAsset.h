#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonAsset.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UNotifyLayout> NotifyLayoutClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UConfirmScreen> ConfirmScreenClass;

};
