#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "KillLogLine.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UKillLogLine : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* AttackerPlayer;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CauserName;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DamagedPlayer;

};
