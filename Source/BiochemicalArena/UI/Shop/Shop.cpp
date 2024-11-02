#include "Shop.h"
#include "CommonTextBlock.h"
#include "ItemButton.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "Components/WrapBox.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "BiochemicalArena/Utils/LibraryNotify.h"
#include "Components/WrapBoxSlot.h"

#define LOCTEXT_NAMESPACE "UShop"

void UShop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnQueryOffersComplete.AddUObject(this, &ThisClass::OnQueryOffersComplete);
		EOSSubsystem->OnCheckoutComplete.AddUObject(this, &ThisClass::OnCheckoutComplete);

		EOSSubsystem->QueryOffers();
	}
}

UWidget* UShop::NativeGetDesiredFocusTarget() const
{
	return ItemButtonContainer->GetChildAt(0);
}

// 查询商品列表完成
void UShop::OnQueryOffersComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (EOSSubsystem)
		{
			TArray<FOffer> Offers = EOSSubsystem->GetOffers(); // 获取商品列表

			for (int32 i = 0; i < Offers.Num(); ++i)
			{
				bool bIsEquipment = false;
				for (int32 j = 0; j < static_cast<int32>(EEquipmentName::NONE); ++j)
				{
					FString EnumValue = UEnum::GetValueAsString(static_cast<EEquipmentName>(j));
					EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
					if (EnumValue == Offers[i].Title.ToString())
					{
						AddEquipmentButton(Offers[i]);
						bIsEquipment = true;
						break;
					}
				}

				if (bIsEquipment) continue;

				for (int32 j = 0; j < static_cast<int32>(EHumanCharacterName::NONE); ++j)
				{
					FString EnumValue = UEnum::GetValueAsString(static_cast<EHumanCharacterName>(j));
					EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
					if (EnumValue == Offers[i].Title.ToString())
					{
						AddCharacterButton(Offers[i]);
						break;
					}
				}
			}

			NativeGetDesiredFocusTarget()->SetFocus();
		}
	}
}

// 添加装备按钮
void UShop::AddEquipmentButton(FOffer Offer)
{
	UItemButton* EquipmentButton = CreateWidget<UItemButton>(this, EquipmentButtonClass);
	if (EquipmentButton)
	{
		EquipmentButton->Offer = Offer;
		EquipmentButton->ItemName->SetText(Offer.Title);
		EquipmentButton->Price->SetText(Offer.FormattedPrice);
		EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnItemButtonClicked, EquipmentButton);
		UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(ItemButtonContainer->AddChild(EquipmentButton));
		if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
	}
}

// 添加角色按钮
void UShop::AddCharacterButton(FOffer Offer)
{
	UItemButton* CharacterButton = CreateWidget<UItemButton>(this, CharacterButtonClass);
	if (CharacterButton)
	{
		CharacterButton->Offer = Offer;
		CharacterButton->ItemName->SetText(Offer.Title);
		CharacterButton->Price->SetText(Offer.FormattedPrice);
		CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnItemButtonClicked, CharacterButton);
		UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(ItemButtonContainer->AddChild(CharacterButton));
		if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
	}
}

// 点击购买商品
void UShop::OnItemButtonClicked(UItemButton* ItemButton)
{
	if (EOSSubsystem)
	{
		TArray<FPurchaseOffer> Offers;
		Offers.Add(FPurchaseOffer{ ItemButton->Offer.OfferId, 1 });
		EOSSubsystem->Checkout(Offers);
	}
}

void UShop::OnCheckoutComplete(bool bWasSuccessful, TOptional<FString> String)
{
	if(bWasSuccessful)
	{
		NOTIFY(this, C_RED, LOCTEXT("CheckoutSuccess", "Buy item success!"));
	}
	else
	{
		NOTIFY(this, C_RED, LOCTEXT("CheckoutFailed", "Buy item failed!"));
	}
}

#undef LOCTEXT_NAMESPACE
