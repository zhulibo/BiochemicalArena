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
	Tank UMETA(DisplayName = "Tank"),
	Ghost UMETA(DisplayName = "Ghost"),
	Smoker UMETA(DisplayName = "Smoker"),

	NONE UMETA(DisplayName = "NONE")
};

USTRUCT(BlueprintType)
struct FHumanCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere)
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMutantCharacter> MutantCharacterClass;

	UPROPERTY(EditAnywhere)
	float LightAttackDamage = 0.f;
	UPROPERTY(EditAnywhere)
	float HeavyAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	FString Desc = "";

};
