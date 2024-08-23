#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SystemSound.generated.h"

UCLASS()
class BIOCHEMICALARENA_API USystemSound : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class USoundMix* SoundMix;
	UPROPERTY(EditAnywhere)
	class USoundClass* MasterSound;

};
