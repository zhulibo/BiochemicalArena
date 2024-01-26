#include "Shop.h"
#include "CommonTextBlock.h"
#include "GoodsButton.h"
#include "BiochemicalArena/Characters/CharacterType.h"
#include "Components/WrapBox.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"

void UShop::NativeConstruct()
{
	Super::NativeConstruct();

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnLoginComplete.AddUObject(this, &ThisClass::OnLoginComplete);
		EOSSubsystem->OnQueryOffersComplete.AddUObject(this, &ThisClass::OnQueryOffersComplete);
		EOSSubsystem->OnPurchaseCompleted.AddUObject(this, &ThisClass::OnPurchaseCompleted);
		EOSSubsystem->OnQueryOwnershipComplete.AddUObject(this, &ThisClass::OnOwnershipComplete);
	}
}

// 登录完成
void UShop::OnLoginComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if(EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			EOSSubsystem->QueryOffers();
		}
	}
}

// 缓存商品列表
void UShop::OnQueryOffersComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if(EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		if (EOSSubsystem)
		{
			TArray<FOffer> Offers = EOSSubsystem->GetOffers(); // 获取商品列表

			for (int i = 0; i < Offers.Num(); ++i)
			{
				for (int32 j = 0; j < static_cast<int32>(EEquipmentName::MAX); ++j)
				{
					FString EnumValue = UEnum::GetValueAsString(static_cast<EEquipmentName>(j));
					EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
					if (EnumValue == Offers[i].Title.ToString())
					{
						AddEquipmentButton(Offers[i]);
						break;
					}
				}

				// TODO continue

				for (int32 j = 0; j < static_cast<int32>(EHumanCharacterName::MAX); ++j)
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
		}
	}
}

// 添加装备按钮
void UShop::AddEquipmentButton(FOffer Offer)
{
	UGoodsButton* EquipmentButton = CreateWidget<UGoodsButton>(this, EquipmentButtonClass);
	if (EquipmentButton)
	{
		EquipmentButton->Offer = Offer;
		EquipmentButton->GoodsName->SetText(Offer.Title);
		EquipmentButton->Price->SetText(FText::Format(FText::FromString("{0} / 1 year"), Offer.FormattedPrice));
		EquipmentButton->SetPadding(FMargin(0, 0, 20, 20));
		EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnGoodsButtonClicked, EquipmentButton);
		GoodsButtonContainer->AddChild(EquipmentButton);
	}
}

// 添加角色按钮
void UShop::AddCharacterButton(FOffer Offer)
{
	UGoodsButton* CharacterButton = CreateWidget<UGoodsButton>(this, CharacterButtonClass);
	if (CharacterButton)
	{
		CharacterButton->Offer = Offer;
		CharacterButton->GoodsName->SetText(Offer.Title);
		CharacterButton->Price->SetText(FText::Format(FText::FromString("{0} / 1 year"), Offer.FormattedPrice));
		CharacterButton->SetPadding(FMargin(0, 0, 20, 20));
		CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnGoodsButtonClicked, CharacterButton);
		GoodsButtonContainer->AddChild(CharacterButton);
	}
}

// 点击商品
void UShop::OnGoodsButtonClicked(UGoodsButton* GoodsButton)
{
	// GetWorld()->ServerTravel("/Game/Maps/Dev?listen");
	// return;

	// 商品已拥有直接退出
	if (Ownership.Contains(GoodsButton->Offer.OfferId))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Already own this goods!"));
		return;
	}

	// 结账
	if(EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		TArray<FPurchaseOffer> Offers;
		Offers.Add(FPurchaseOffer{ GoodsButton->Offer.OfferId, 1 });
		EOSSubsystem->Checkout(Offers);
	}
}

// 购买完成
void UShop::OnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
{
	if(EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->QueryOwnership(); // 获取已购商品
	}
}

// 获取已购商品完成
void UShop::OnOwnershipComplete(bool bWasSuccessful, const TArray<FString> TemOwnership)
{
	if(!bWasSuccessful) return;
	Ownership = TemOwnership;
}
