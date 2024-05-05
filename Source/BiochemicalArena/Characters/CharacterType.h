#pragma once

#include "Engine/DataTable.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class EHumanCharacterName : uint8
{
	SAS UMETA(DisplayName = "SAS"),
	PLA UMETA(DisplayName = "PLA"),
	SEAL UMETA(DisplayName = "SEAL"),

	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FHumanCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::SAS;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AudienceItemId = "-1";

};
