#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "CharacterMenuButton.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCharacterMenuButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* CharacterNameText;
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* CharacterDescText;

};
