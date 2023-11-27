#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
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

	void OnWeaponButtonClicked(class UGoodsButton* GoodsButton);
	void OnCharacterButtonClicked(UGoodsButton* GoodsButton);

private:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* GoodsButtonContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> WeaponButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> CharacterButtonClass;

};
