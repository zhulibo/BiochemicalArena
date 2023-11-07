#pragma once

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Primary UMETA(DisplayName = "Primary", BodySocketName = "RightShoulderSocket"),
	Secondary UMETA(DisplayName = "Secondary", BodySocketName = "RightCrotchSocket"),
	Melee UMETA(DisplayName = "Melee", BodySocketName = "LeftShoulderSocket"),
	Throwing UMETA(DisplayName = "Throwing", BodySocketName = "LeftCrotchSocket"),

	MAX UMETA(Hidden)
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
	AK47 UMETA(DisplayName = "AK47", MontageSectionName = "AK47", WeaponType = "Primary", WeaponCate = "General"),
	M870 UMETA(DisplayName = "M870", MontageSectionName = "M870", WeaponType = "Melee", WeaponCate = "Shotgun"),

	Glock17 UMETA(DisplayName = "Glock17", MontageSectionName = "Glock17", WeaponType = "Secondary", WeaponCate = "Shotgun"),

	Kukri UMETA(DisplayName = "Kukri", MontageSectionName = "Kukri", WeaponType = "Melee", WeaponCate = "Melee"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	InUse UMETA(DisplayName = "InUse"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};
