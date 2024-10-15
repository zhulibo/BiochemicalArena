#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageTypeBase.generated.h"

UENUM(BlueprintType)
enum class EDamageCauserType : uint8
{
	Base UMETA(DisplayName = "Base"),
	Equipment UMETA(DisplayName = "Equipment"),
	Mutant UMETA(DisplayName = "Mutant"),
	Fall UMETA(DisplayName = "Fall"),

	MAX UMETA(Hidden)
};

UCLASS()
class BIOCHEMICALARENA_API UDamageTypeBase : public UDamageType
{
	GENERATED_BODY()

public:
	// Used to determine the type of damage, Only need cast to UDamageTypeBase to get the type
	// avoid cast to UDamageTypeEquipment, UDamageTypeMutant, etc.
	UPROPERTY(EditAnywhere)
	EDamageCauserType DamageType = EDamageCauserType::Base;

	UPROPERTY(EditAnywhere)
	FString CauserName = "";

};
