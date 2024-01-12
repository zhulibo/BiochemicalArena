#pragma once

#include "Engine/DataTable.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class EHumanCharacterName : uint8
{
	SAS UMETA(DisplayName = "SAS"),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ProductId = "-1";

};
