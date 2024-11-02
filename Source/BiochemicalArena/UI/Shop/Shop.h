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
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* ItemButtonContainer;

	void OnQueryOffersComplete(bool bWasSuccessful);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UItemButton> EquipmentButtonClass;
	void AddEquipmentButton(FOffer Offer);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UItemButton> CharacterButtonClass;
	void AddCharacterButton(FOffer Offer);

	void OnItemButtonClicked(UItemButton* ItemButton);
	void OnCheckoutComplete(bool bWasSuccessful, TOptional<FString> String);

};
