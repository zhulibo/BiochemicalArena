#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "StorageButton.generated.h"

enum class EHumanCharacterName : uint8;
enum class EEquipmentName : uint8;
enum class EEquipmentType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UStorageButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UCommonTextBlock* ButtonText;
	UPROPERTY()
	FString ShowName;

	UPROPERTY()
	EHumanCharacterName HumanCharacterName;

	UPROPERTY()
	EEquipmentName EquipmentName;
	UPROPERTY()
	EEquipmentType EquipmentType;

};
