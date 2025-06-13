﻿#pragma once

#include "Engine/DataTable.h"
#include "EquipmentType.generated.h"

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EEquipmentName : uint8 // 装备名字
{
	// 主
	AK47,
	AK47_Desert,
	
	M870,
	
	M60,
	
	PKM,
	
	// 副
	Glock17,
	
	DesertEagle,
	
	// 近战
	Kukri,
	
	MilitaryShovel,
	
	FireAxe,
	
	// 投掷
	Grenade,
	Flashbang,
	Smoke,
	FireBottle,
	
	// 补给箱
	AK47_Cyclone,
	MG42,

	None
};

UENUM(BlueprintType)
enum class EEquipmentCate : uint8 // 装备种类（武器自身属性）
{
	Generic,
	Shotgun,
	Melee,
	Throwing,

	None
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8 // 装备类型（装备的位置）
{
	Primary,
	Secondary,
	Melee,
	Throwing,
	// Tool,
	// Tonic,

	None
};

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
	Equipped,
	Dropped,
	Thrown
};

USTRUCT(BlueprintType)
struct FEquipmentMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString ShowName;

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentParentName = EEquipmentName::None;

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::None;

	UPROPERTY(EditAnywhere)
	EEquipmentCate EquipmentCate = EEquipmentCate::None;

	UPROPERTY(EditAnywhere)
	EEquipmentType EquipmentType = EEquipmentType::None;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class AEquipment> EquipmentClass;

	/**
	 * Must be the same as the OfferId in dev portal,
	 * Used to determine the button type in the UShop::OnQueryOffersComplete.
	 */
	UPROPERTY(EditAnywhere)
	FString OfferId;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether player's account owns the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId;

};

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEquipmentName EquipmentName = EEquipmentName::None;

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
	EEquipmentName EquipmentName = EEquipmentName::None;

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
	EEquipmentName EquipmentName = EEquipmentName::None;

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
