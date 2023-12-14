#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Infect.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInfect : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MatchCountdownText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* HumanNumberText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* ZombieNumberText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* RoundText;

};
