#include "Shop.h"
#include "CommonTextBlock.h"
#include "GoodsButton.h"
#include "Components/WrapBox.h"

void UShop::NativeConstruct()
{
	Super::NativeConstruct();

	AddEquipmentButton();
	AddCharacterButton();
}

void UShop::AddEquipmentButton()
{
	UGoodsButton* EquipmentButton = CreateWidget<UGoodsButton>(this, EquipmentButtonClass);
	if (EquipmentButton)
	{
		EquipmentButton->GoodsName->SetText(FText::FromString("Ak47 / one year"));
		EquipmentButton->Price->SetText(FText::FromString("5$"));
		EquipmentButton->SetPadding(FMargin(0, 0, 20, 20));
		EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentButtonClicked, EquipmentButton);
		GoodsButtonContainer->AddChild(EquipmentButton);
	}
}

void UShop::AddCharacterButton()
{
	UGoodsButton* CharacterButton = CreateWidget<UGoodsButton>(this, CharacterButtonClass);
	if (CharacterButton)
	{
		CharacterButton->GoodsName->SetText(FText::FromString("JK / one year"));
		CharacterButton->Price->SetText(FText::FromString("20$"));
		CharacterButton->SetPadding(FMargin(0, 0, 20, 20));
		CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterButtonClicked, CharacterButton);
		GoodsButtonContainer->AddChild(CharacterButton);
	}
}

void UShop::OnEquipmentButtonClicked(UGoodsButton* GoodsButton)
{
}

void UShop::OnCharacterButtonClicked(UGoodsButton* GoodsButton)
{
}
