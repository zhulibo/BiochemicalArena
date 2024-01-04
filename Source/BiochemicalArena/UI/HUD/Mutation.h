#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Mutation.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMutation : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* MatchCountdown;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* HumanNumber;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* MutantNumber;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Round;

};
