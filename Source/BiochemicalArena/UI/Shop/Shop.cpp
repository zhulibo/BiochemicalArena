#include "Shop.h"
#include "CommonTextBlock.h"
#include "GoodsButton.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "Components/WrapBox.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "Components/WrapBoxSlot.h"

void UShop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	EOSSubsystem->OnQueryOffersComplete.AddUObject(this, &ThisClass::OnQueryOffersComplete);
	// 	EOSSubsystem->OnPurchaseCompleted.AddUObject(this, &ThisClass::OnPurchaseCompleted);
	// 	EOSSubsystem->OnQueryOwnershipComplete.AddUObject(this, &ThisClass::OnOwnershipComplete);
	// 	// EOSSubsystem->QueryOffers(); // TODO
	// }
}

UWidget* UShop::NativeGetDesiredFocusTarget() const
{
	return GoodsButtonContainer->GetChildAt(0);
}

// 缓存商品列表
void UShop::OnQueryOffersComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
		// if (EOSSubsystem)
		// {
		// 	TArray<FOffer> Offers = EOSSubsystem->GetOffers(); // 获取商品列表
		//
		// 	for (int32 i = 0; i < Offers.Num(); ++i)
		// 	{
		// 		for (int32 j = 0; j < static_cast<int32>(EEquipmentName::NONE); ++j)
		// 		{
		// 			FString EnumValue = UEnum::GetValueAsString(static_cast<EEquipmentName>(j));
		// 			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		// 			if (EnumValue == Offers[i].Title.ToString())
		// 			{
		// 				AddEquipmentButton(Offers[i]);
		// 				break;
		// 			}
		// 		}
		//
		// 		// TODO continue
		//
		// 		for (int32 j = 0; j < static_cast<int32>(EHumanCharacterName::NONE); ++j)
		// 		{
		// 			FString EnumValue = UEnum::GetValueAsString(static_cast<EHumanCharacterName>(j));
		// 			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		// 			if (EnumValue == Offers[i].Title.ToString())
		// 			{
		// 				AddCharacterButton(Offers[i]);
		// 				break;
		// 			}
		// 		}
		// 	}
		// }
	}
}

// 添加装备按钮
// void UShop::AddEquipmentButton(FOffer Offer)
// {
// 	UGoodsButton* EquipmentButton = CreateWidget<UGoodsButton>(this, EquipmentButtonClass);
// 	if (EquipmentButton)
// 	{
// 		EquipmentButton->Offer = Offer;
// 		EquipmentButton->GoodsName->SetText(Offer.Title);
// 		EquipmentButton->Price->SetText(FText::Format(FText::FromString(TEXT("{0} / 1 year")), Offer.FormattedPrice));
// 		EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnGoodsButtonClicked, EquipmentButton);
// 		UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(GoodsButtonContainer->AddChild(EquipmentButton));
// 		if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
// 	}
// }

// 添加角色按钮
// void UShop::AddCharacterButton(FOffer Offer)
// {
// 	UGoodsButton* CharacterButton = CreateWidget<UGoodsButton>(this, CharacterButtonClass);
// 	if (CharacterButton)
// 	{
// 		CharacterButton->Offer = Offer;
// 		CharacterButton->GoodsName->SetText(Offer.Title);
// 		CharacterButton->Price->SetText(FText::Format(FText::FromString(TEXT("{0} / 1 year")), Offer.FormattedPrice));
// 		CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnGoodsButtonClicked, CharacterButton);
// 		UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(GoodsButtonContainer->AddChild(CharacterButton));
// 		if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
// 	}
// }

// 点击商品
void UShop::OnGoodsButtonClicked(UGoodsButton* GoodsButton)
{
	// 商品已拥有直接退出
	// if (Ownership.Contains(GoodsButton->Offer.OfferId))
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 10.f, C_YELLOW, TEXT("Already own this goods!"), false);
	// 	return;
	// }

	// 结账
	// if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	// if (EOSSubsystem)
	// {
	// 	TArray<FPurchaseOffer> Offers;
	// 	Offers.Add(FPurchaseOffer{ GoodsButton->Offer.OfferId, 1 });
	// 	EOSSubsystem->Checkout(Offers);
	// }
}

// 购买完成
// void UShop::OnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
// {
// 	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
// 	if (EOSSubsystem)
// 	{
// 		EOSSubsystem->QueryOwnership(); // 获取已购商品
// 	}
// }

// 获取已购商品完成
void UShop::OnOwnershipComplete(bool bWasSuccessful, const TArray<FString> TempOwnership)
{
	if(!bWasSuccessful) return;
	Ownership = TempOwnership;
}
