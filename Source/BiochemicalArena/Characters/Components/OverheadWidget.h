#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "OverheadWidget.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UOverheadWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	friend class AHumanCharacter;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY()
	AHumanCharacter* HumanCharacter;
	UPROPERTY()
	AHumanCharacter* LocalHumanCharacter;

	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;

	void SetPlayerName();
	void RefreshPlayerName();

};
