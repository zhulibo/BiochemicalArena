#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/Weapons/WeaponType.h"
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
	void AddEquipmentTypeButton();
	void OnEquipmentTypeButtonClicked(class UCommonButton* CommonButton);
	TArray<FText> FilterWeapon(FString WeaponTypeToFilter);

	void AddWeaponButton(TArray<FText> WeaponNames);
	void AddCharacterButton();

	void OnWeaponButtonClicked(class UEquipmentButton* CommonButton);
	void OnCharacterButtonClicked(UEquipmentButton* CommonButton);

	void SetBagContent(EWeaponType& WeaponType, FString& WeaponName);
	void SaveBag();

private:
	UPROPERTY()
	UDataTable* WeaponDataTable;
	TArray<FWeaponData*> WeaponDataRows;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCommonButton> EquipmentTypeButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipmentButton> WeaponButtonClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEquipmentButton> CharacterButtonClass;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* EquipmentTypeButtonContainer;
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* EquipmentButtonContainer;
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	UPROPERTY(meta = (BindWidget))
	UEquipmentButton* Character;

};
