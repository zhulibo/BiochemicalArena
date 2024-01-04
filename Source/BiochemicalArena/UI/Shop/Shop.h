#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Shop.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UShop : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* GoodsButtonContainer;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGoodsButton> EquipmentButtonClass;
	void AddEquipmentButton();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> CharacterButtonClass;
	void AddCharacterButton();

	void OnEquipmentButtonClicked(UGoodsButton* GoodsButton);
	void OnCharacterButtonClicked(UGoodsButton* GoodsButton);

};
