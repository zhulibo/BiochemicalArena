#pragma once

#define TRACE_LENGTH 100000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_GeneralWeapon UMETA(DisplayName = "GeneralWeapon"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),

	EWT_Max UMETA(DisplayName = "DefaultMax")
};
