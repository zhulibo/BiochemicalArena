#pragma once

#include "Engine/DataTable.h"
#include "EquipmentType.generated.h"

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EEquipmentName : uint8 // 装备名字
{
	AK47 UMETA(DisplayName = "AK47"),
	AK47_Desert UMETA(DisplayName = "AK47_Desert"),
	M870 UMETA(DisplayName = "M870"),

	Glock17 UMETA(DisplayName = "Glock17"),

	Kukri UMETA(DisplayName = "Kukri"),

	Grenade UMETA(DisplayName = "Grenade"),

	NONE UMETA(DisplayName = "NONE")
};

UENUM(BlueprintType)
enum class EEquipmentCate : uint8 // 装备种类（武器自身属性）
{
	Generic UMETA(DisplayName = "Generic"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Melee UMETA(DisplayName = "Melee"),
	Throwing UMETA(DisplayName = "Throwing"),

	NONE UMETA(DisplayName = "NONE")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8 // 装备类型（装备的位置，共4个位置，未来扩充至8个）
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Melee UMETA(DisplayName = "Melee"),
	Throwing UMETA(DisplayName = "Throwing"),

	NONE UMETA(DisplayName = "NONE")
};

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
	Thrown UMETA(DisplayName = "Thrown")
};

USTRUCT(BlueprintType)
struct FEquipmentMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::NONE;

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentParentName = EEquipmentName::NONE;

	UPROPERTY(EditAnywhere)
	EEquipmentCate EquipmentCate = EEquipmentCate::NONE;

	UPROPERTY(EditAnywhere)
	EEquipmentType EquipmentType = EEquipmentType::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEquipment> EquipmentClass;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId;

};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::NONE;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 0;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity = 0;

	UPROPERTY(EditAnywhere)
	float FireRate = 0.f;
	UPROPERTY(EditAnywhere)
	bool bIsAutomatic = true;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DamageCurve;
	UPROPERTY(EditAnywhere)
	float Impulse = 0.f;

	UPROPERTY(EditAnywhere)
	int32 PelletNum = 1;

	UPROPERTY(EditAnywhere)
	float AimingFOVMul = 0.9;
	UPROPERTY(EditAnywhere)
	float AimSpeed = 30.f;

	UPROPERTY(EditAnywhere)
	float MoveSpeedMul = 1.f;

};

USTRUCT(BlueprintType)
struct FMeleeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::NONE;

	UPROPERTY(EditAnywhere)
	float LightAttackDamage = 0.f;
	UPROPERTY(EditAnywhere)
	float HeavyAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float MoveSpeedMul = 1.f;

};

USTRUCT(BlueprintType)
struct FEquipmentRecoil : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::NONE;

	UPROPERTY(EditAnywhere)
	float RecoilMaxVert = 0.f;
	UPROPERTY(EditAnywhere)
	float RecoilMinVert = 0.f;
	UPROPERTY(EditAnywhere)
	float RecoilMaxHor = 0.f;
	UPROPERTY(EditAnywhere)
	float RecoilMinHor = 0.f;

	UPROPERTY(EditAnywhere)
	float FirstShotRecoilMul = 1.f;

	UPROPERTY(EditAnywhere)
	float RecoilIncTime = 0.f;

	UPROPERTY(EditAnywhere)
	float RecoilTotalVertLimit = 0.f;
	UPROPERTY(EditAnywhere)
	float RecoilTotalHorLimit = 0.f;

	UPROPERTY(EditAnywhere)
	float RecoilDecSpeed = 0.f;

	UPROPERTY(EditAnywhere)
	float CenterSpread = 0.f;

};
