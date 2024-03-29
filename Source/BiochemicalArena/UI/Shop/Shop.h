#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
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
	void AddEquipmentButton(FOffer Offer);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGoodsButton> CharacterButtonClass;
	void AddCharacterButton(FOffer Offer);

	void OnGoodsButtonClicked(UGoodsButton* GoodsButton);

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	void OnLoginComplete(bool bWasSuccessful);
	void OnQueryOffersComplete(bool bWasSuccessful);

	void OnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete);
	TArray<FString> Ownership;
	void OnOwnershipComplete(bool bWasSuccessful, const TArray<FString> TemOwnership);

};
