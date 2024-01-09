#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Storage.generated.h"

enum class EEquipmentType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UStorage : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	void OnLoginComplete(bool bWasSuccessful);
	void OnEnumerateFilesComplete(bool bWasSuccessful);
	void UseLocalPlayerStorage();
	void OnReadFileComplete(bool bWasSuccessful, const FUserFileContentsRef& FileContents);
	void InitPlayerConfig(class UPlayerStorage* PlayerStorage);

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	UPROPERTY()
	UDataTable* EquipmentDataTable;
	TArray<struct FEquipmentData*> EquipmentDataRows;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* StorageTypeButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> StorageTypeButtonClass;
	void AddStorageTypeButton();

	void OnStorageTypeButtonClicked(UCommonButton* CommonButton);
	TArray<FText> FilterEquipment(FString EquipmentType);

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* StorageButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStorageButton> EquipmentButtonClass;
	void AddEquipmentButton(TArray<FText> EquipmentNames);
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStorageButton> CharacterButtonClass;
	void AddCharacterButton();

	void OnEquipmentButtonClicked(class UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	void SetBagContent(EEquipmentType& EquipmentType, FString& EquipmentName);
	void SaveBag();

	void OnCharacterButtonClicked(UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Character;

};
