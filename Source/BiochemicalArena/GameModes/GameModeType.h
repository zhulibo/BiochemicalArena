#pragma once

UENUM(BlueprintType)
enum class ECoolGameMode : uint8
{
	Mutation UMETA(DisplayName = "Mutation"),
	TeamDeadMatch UMETA(DisplayName = "TeamDeadMatch"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMutationMap : uint8
{
	DevMutation UMETA(DisplayName = "Dev_Mutation"),
	DevMutation2 UMETA(DisplayName = "Dev_Mutation2"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeamDeadMatchMap : uint8
{
	DevTeamDeadMatch UMETA(DisplayName = "Dev_TeamDeadMatch"),
	DevTeamDeadMatch2 UMETA(DisplayName = "Dev_TeamDeadMatch2"),

	MAX UMETA(Hidden)
};
