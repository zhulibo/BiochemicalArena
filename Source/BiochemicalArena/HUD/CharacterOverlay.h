#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPValue;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* CSBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CSValue;

};
