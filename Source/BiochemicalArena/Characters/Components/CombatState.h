#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Ready UMETA(DisplayName = "Ready"),
	Reloading UMETA(DisplayName = "Reloading"),
	Swapping UMETA(DisplayName = "Swapping"),

	MAX UMETA(Hidden)
};
