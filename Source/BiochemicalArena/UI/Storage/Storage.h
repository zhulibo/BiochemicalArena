#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "DataRegistryId.h"
// #include "BiochemicalArena/System/EOSSubsystem.h"
#include "Storage.generated.h"

#define STORAGETYPE_ALL FString("All")
#define STORAGETYPE_CHARACTER FString("Character")

enum class EEquipmentType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UStorage : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	// UPROPERTY()
	// UEOSSubsystem* EOSSubsystem;
	void OnEnumerateFilesComplete(bool bWasSuccessful);

	TArray<FString> Ownership;
	void OnQueryOwnershipComplete(bool bWasSuccessful, const TArray<FString> TempOwnership);

	// void OnReadFileComplete(bool bWasSuccessful, const FUserFileContentsRef& FileContents);
	void InitPlayerConfig(class USaveGameSetting* SaveGameSetting);
	bool HasEquipment(FString EquipmentName);
	bool HasHumanCharacter(FString HumanCharacterName);

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	TMap<FDataRegistryId, const uint8*> HumanCharacterMains;
	TMap<FDataRegistryId, const uint8*> EquipmentMains;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* StorageTypeButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCommonButton> StorageTypeButtonClass;
	void AddStorageTypeButton();

	void OnStorageTypeButtonClicked(UCommonButton* CommonButton);
	TArray<FText> FilterEquipment(FString EquipmentType);
	TArray<FText> FilterHumanCharacter();

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* StorageButtonContainer;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStorageButton> EquipmentButtonClass;
	void AddEquipmentButton(TArray<FText> EquipmentNames);
	UPROPERTY(EditAnywhere)
	TSubclassOf<UStorageButton> CharacterButtonClass;
	void AddCharacterButton(TArray<FText> EquipmentNames);

	void OnEquipmentButtonClicked(class UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonActivatableWidgetSwitcher* BagSwitcher;
	void SaveBag(EEquipmentType EquipmentType, FString EquipmentName);
	void SaveBagToStorage();

	void OnCharacterButtonClicked(UStorageButton* CommonButton);
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* Character;

};
