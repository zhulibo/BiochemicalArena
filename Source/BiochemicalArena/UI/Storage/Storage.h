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

	TArray<FString> Ownership;
	void OnQueryOwnershipComplete(bool bWasSuccessful, const TArray<FString> TempOwnership);

	void OnReadFileComplete(bool bWasSuccessful, const FUserFileContentsRef& FileContents);
	void InitPlayerConfig(class UStorageSaveGame* StorageSaveGame);
	bool HasEquipment(FString EquipmentName);
	bool HasHumanCharacter(FString HumanCharacterName);

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;
	UPROPERTY()
	class UAssetSubsystem* AssetSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* StorageTypeButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UCommonButton> StorageTypeButtonClass;
	void AddStorageTypeButton();

	void OnStorageTypeButtonClicked(UCommonButton* CommonButton);
	TArray<FText> FilterEquipment(FString EquipmentType);
	TArray<FText> FilterHumanCharacter();

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* StorageButtonContainer;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UStorageButton> EquipmentButtonClass;
	void AddEquipmentButton(TArray<FText> EquipmentNames);
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStorageButton> CharacterButtonClass;
	void AddCharacterButton(TArray<FText> EquipmentNames);

	void OnEquipmentButtonClicked(class UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	void SaveBag(EEquipmentType& EquipmentType, FString& EquipmentName);
	void SaveBagToStorage();

	void OnCharacterButtonClicked(UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Character;

};
