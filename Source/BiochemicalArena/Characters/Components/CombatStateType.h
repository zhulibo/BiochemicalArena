#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	Ready,
	Reloading,
	Swapping,
	LightAttacking,
	HeavyAttacking,
	Throwing,

	MAX UMETA(Hidden)
};
