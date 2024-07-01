#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "BaseDamageType.generated.h"

UENUM(BlueprintType)
enum class EDamageType2 : uint8
{
	Base UMETA(DisplayName = "Base"),
	Equipment UMETA(DisplayName = "Equipment"),
	Mutant UMETA(DisplayName = "Mutant"),
	Fall UMETA(DisplayName = "Fall"),

	MAX UMETA(Hidden)
};

UCLASS()
class BIOCHEMICALARENA_API UBaseDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	// Used to determine the type of damage, Only need cast to UBaseDamageType to get the type
	// avoid cast to UEquipmentDamageType, UMutantDamageType, etc.
	UPROPERTY(EditAnywhere)
	EDamageType2 DamageType = EDamageType2::Base;

	UPROPERTY(EditAnywhere)
	FString CauserName = "";

	UPROPERTY(EditAnywhere)
	float Impulse = 0.f;

};
