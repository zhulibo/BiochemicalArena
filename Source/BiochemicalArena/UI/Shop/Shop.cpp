#include "Shop.h"
#include "CommonTextBlock.h"
#include "GoodsButton.h"
#include "Components/WrapBox.h"

void UShop::NativeConstruct()
{
	Super::NativeConstruct();

	AddWeaponButton();
	AddCharacterButton();
}

void UShop::AddWeaponButton()
{
	UGoodsButton* WeaponButton = CreateWidget<UGoodsButton>(this, WeaponButtonClass);
	if (WeaponButton)
	{
		WeaponButton->GoodsNameText->SetText(FText::FromString("Ak47 / one year"));
		WeaponButton->PriceText->SetText(FText::FromString("5$"));
		WeaponButton->SetPadding(FMargin(0, 0, 20, 20));
		WeaponButton->OnClicked().AddUObject(this, &ThisClass::OnWeaponButtonClicked, WeaponButton);
		GoodsButtonContainer->AddChild(WeaponButton);
	}
}

void UShop::AddCharacterButton()
{
	UGoodsButton* CharacterButton = CreateWidget<UGoodsButton>(this, CharacterButtonClass);
	if (CharacterButton)
	{
		CharacterButton->GoodsNameText->SetText(FText::FromString("JK / one year"));
		CharacterButton->PriceText->SetText(FText::FromString("20$"));
		CharacterButton->SetPadding(FMargin(0, 0, 20, 20));
		CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterButtonClicked, CharacterButton);
		GoodsButtonContainer->AddChild(CharacterButton);
	}
}

void UShop::OnWeaponButtonClicked(UGoodsButton* GoodsButton)
{
}

void UShop::OnCharacterButtonClicked(UGoodsButton* GoodsButton)
{
}
