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

UENUM(BlueprintType)
enum class EMutantCharacterName : uint8
{
	Tank UMETA(DisplayName = "Tank"),
	Ghost UMETA(DisplayName = "Ghost"),
	Octopus UMETA(DisplayName = "Octopus"),

	MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FHumanCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::SAS;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId = "-1";

};
