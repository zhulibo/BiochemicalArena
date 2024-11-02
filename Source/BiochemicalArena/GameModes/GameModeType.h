#pragma once

UENUM(BlueprintType)
enum class ECoolGameMode : uint8
{
	Mutation,
	TeamDeadMatch,

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMutationMap : uint8
{
	DevMutation,
	DevMutation2,

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeamDeadMatchMap : uint8
{
	DevTeamDeadMatch,
	DevTeamDeadMatch2,

	MAX UMETA(Hidden)
};
