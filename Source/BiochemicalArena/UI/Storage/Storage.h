#pragma once

#include "CoreMinimal.h"
#include "BagContent.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonHierarchicalScrollBox.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "EquipmentButton.h"
#include "Components/WrapBox.h"
#include "Storage.generated.h"

USTRUCT(BlueprintType)
struct FBag
{
	GENERATED_BODY()

public:
	FString Primary;
	FString Secondary;
	FString Melee;
	FString Throwing;

};

UCLASS()
class BIOCHEMICALARENA_API UStorage : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	UEnum* WeaponNameEnum;

	void AddEquipmentTypeButton();
	void OnEquipmentTypeButtonClicked(UCommonButton* CommonButton);
	TArray<FText> FilterWeapon(FString WeaponTypeToFilter);

	void AddWeaponButton(TArray<FText> WeaponNames);
	void AddCharacterButton();

	void OnWeaponButtonClicked(UEquipmentButton* CommonButton);
	void OnCharacterButtonClicked(UEquipmentButton* CommonButton);

	void SetBagContent(FString& WeaponType, FString& WeaponName);
	void SaveBag();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonButton> EquipmentTypeButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipmentButton> WeaponButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipmentButton> CharacterButtonClass;

	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* EquipmentTypeButtonContainer;
	UPROPERTY(meta = (BindWidget))
	UWrapBox* EquipmentButtonContainer;
	UPROPERTY(meta = (BindWidget))
	UCommonActivatableWidgetSwitcher* BagSwitcher;
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Character;

};
