#pragma once

#include "Engine/DataTable.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class EHumanCharacterName : uint8
{
	SAS UMETA(DisplayName = "SAS"),
	PLA UMETA(DisplayName = "PLA"),
	SEAL UMETA(DisplayName = "SEAL"),

	NONE UMETA(DisplayName = "NONE")
};

UENUM(BlueprintType)
enum class EMutantCharacterName : uint8
{
	Tank UMETA(DisplayName = "Tank", Desc = "Tank Desc"),
	Ghost UMETA(DisplayName = "Ghost", Desc = "Ghost Desc"),
	Smoker UMETA(DisplayName = "Smoker", Desc = "Smoker Desc"),

	NONE UMETA(DisplayName = "NONE")
};

USTRUCT(BlueprintType)
struct FHumanCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHumanCharacter> HumanCharacterClass;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId;

};

USTRUCT(BlueprintType)
struct FMutantCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMutantCharacter> MutantCharacterClass;

};
