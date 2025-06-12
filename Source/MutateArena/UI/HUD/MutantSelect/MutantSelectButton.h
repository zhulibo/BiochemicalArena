#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MutantSelectButton.generated.h"

enum class EMutantCharacterName : uint8;

UCLASS()
class MUTATEARENA_API UMutantSelectButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* NameText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* DescText;
	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

};
