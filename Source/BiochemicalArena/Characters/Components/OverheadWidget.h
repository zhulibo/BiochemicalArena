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
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* PlayerName;
	void SetPlayerName();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void RefreshPlayerName();

private:
	UPROPERTY()
	AHumanCharacter* HumanCharacter;
	UPROPERTY()
	AHumanCharacter* LocalHumanCharacter;

};
