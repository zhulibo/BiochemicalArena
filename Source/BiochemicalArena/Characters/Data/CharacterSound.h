#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSound.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCharacterSound : public UDataAsset
{
	GENERATED_BODY()

public:
	// 捡到装备的声音
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;
	// 无子弹时扣动扳机的声音
	UPROPERTY(EditAnywhere)
	USoundCue* ClickSound;

	// 脚步声
	UPROPERTY(EditAnywhere)
	USoundCue* MetalSound;
	UPROPERTY(EditAnywhere)
	USoundCue* WaterSound;
	UPROPERTY(EditAnywhere)
	USoundCue* GrassSound;
	UPROPERTY(EditAnywhere)
	USoundCue* MudSound;
	UPROPERTY(EditAnywhere)
	USoundCue* CommonSound;

};
