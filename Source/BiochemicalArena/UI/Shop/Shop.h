#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
// #include "BiochemicalArena/System/EOSSubsystem.h"
#include "Shop.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UShop : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* GoodsButtonContainer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGoodsButton> EquipmentButtonClass;
	// void AddEquipmentButton(FOffer Offer);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoodsButton> CharacterButtonClass;
	// void AddCharacterButton(FOffer Offer);

	void OnGoodsButtonClicked(UGoodsButton* GoodsButton);

	// UPROPERTY()
	// UEOSSubsystem* EOSSubsystem;
	void OnQueryOffersComplete(bool bWasSuccessful);

	// void OnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete);
	TArray<FString> Ownership;
	void OnOwnershipComplete(bool bWasSuccessful, const TArray<FString> TempOwnership);

};
