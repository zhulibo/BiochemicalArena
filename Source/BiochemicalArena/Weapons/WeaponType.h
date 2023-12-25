#pragma once

#include "Engine/DataTable.h"
#include "WeaponType.generated.h"

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Melee UMETA(DisplayName = "Melee"),
	Throwing UMETA(DisplayName = "Throwing"),

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EWeaponCate : uint8
{
	General UMETA(DisplayName = "General"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Melee UMETA(DisplayName = "Melee"),
};

UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	AK47 UMETA(DisplayName = "AK47"),
	M870 UMETA(DisplayName = "M870"),

	Glock17 UMETA(DisplayName = "Glock17"),

	Kukri UMETA(DisplayName = "Kukri"),

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	InUse UMETA(DisplayName = "InUse"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType = EWeaponType::Primary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponCate WeaponCate = EWeaponCate::General;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponName WeaponName = EWeaponName::AK47;

};
