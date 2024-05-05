#pragma once

#include "Engine/DataTable.h"
#include "EquipmentType.generated.h"

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EEquipmentType : uint8 // 装备类型（装备的位置，共4个位置，未来扩充至8个）
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Melee UMETA(DisplayName = "Melee"),
	Throwing UMETA(DisplayName = "Throwing"),

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EEquipmentCate : uint8 // 装备种类（武器自身属性）
{
	General UMETA(DisplayName = "General"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Melee UMETA(DisplayName = "Melee"),
	Throwing UMETA(DisplayName = "Throwing"),
};

UENUM(BlueprintType)
enum class EEquipmentName : uint8 // 装备名字
{
	AK47 UMETA(DisplayName = "AK47"),
	AK47_Desert UMETA(DisplayName = "AK47_Desert"),
	M870 UMETA(DisplayName = "M870"),

	Glock17 UMETA(DisplayName = "Glock17"),

	Kukri UMETA(DisplayName = "Kukri"),

	Grenade UMETA(DisplayName = "Grenade"),

	MAX UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
	Thrown UMETA(DisplayName = "Thrown"),
};

USTRUCT(BlueprintType)
struct FEquipmentData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentType EquipmentType = EEquipmentType::Primary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentCate EquipmentCate = EEquipmentCate::General;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentName EquipmentName = EEquipmentName::AK47;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AudienceItemId = "-1";

};
