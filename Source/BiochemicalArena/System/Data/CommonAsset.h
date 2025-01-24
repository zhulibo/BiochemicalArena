#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonAsset.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCommonAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// System Sound
	UPROPERTY(EditAnywhere, Category = "SystemSound")
	class USoundMix* SoundMix;
	UPROPERTY(EditAnywhere, Category = "SystemSound")
	class USoundClass* MasterSound;
	UPROPERTY(EditAnywhere, Category = "SystemSound")
	USoundClass* EffectsSound;
	UPROPERTY(EditAnywhere, Category = "SystemSound")
	USoundClass* MusicSound;
	UPROPERTY(EditAnywhere, Category = "SystemSound")
	USoundClass* DialogueSound;

	// UI
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UNotifyLayout> NotifyLayoutClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UConfirmScreen> ConfirmScreenClass;

	// 倒计时
	UPROPERTY(EditAnywhere, Category = "CountdownSound")
	class UMetaSoundSource* CountdownSound;

};
