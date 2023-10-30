#pragma once

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_MainWeapon UMETA(DisplayName = "MainWeapon"),
	EWT_SecondaryWeapon UMETA(DisplayName = "SecondaryWeapon"),
	EWT_MeleeWeapon UMETA(DisplayName = "MeleeWeapon"),
	EWT_ThrowingWeapon UMETA(DisplayName = "ThrowingWeapon"),

	EWT_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponCate : uint8
{
	EWT_GeneralWeapon UMETA(DisplayName = "GeneralWeapon"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),

	EWT_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_InUse UMETA(DisplayName = "InUse"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_Max UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EBodySocket : uint8
{
	EBS_RightShoulder UMETA(DisplayName = "RightShoulder"),
	EBS_RightCrotch UMETA(DisplayName = "RightCrotch"),
	EBS_LeftShoulder UMETA(DisplayName = "LeftShoulder"),
	EBS_LeftCrotch UMETA(DisplayName = "LeftCrotch"),

	EBS_Max UMETA(DisplayName = "DefaultMax")
};