#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GoodsButton.h"
#include "Components/WrapBox.h"
#include "Shop.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UShop : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void AddWeaponButton();
	void AddCharacterButton();

	void OnWeaponButtonClicked(UGoodsButton* GoodsButton);
	void OnCharacterButtonClicked(UGoodsButton* GoodsButton);

private:
	UPROPERTY(meta = (BindWidget))
	UWrapBox* GoodsButtonContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> WeaponButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> CharacterButtonClass;

};
