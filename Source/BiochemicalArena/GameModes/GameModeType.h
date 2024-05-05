#pragma once

UENUM(BlueprintType)
enum class EGameMode : uint8
{
	Mutation UMETA(DisplayName = "Mutation"),
	TeamDeadMatchMatch UMETA(DisplayName = "TeamDeadMatchMatch"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMutationMap : uint8
{
	Dev UMETA(DisplayName = "Dev"),
	Dev2 UMETA(DisplayName = "Dev2"),

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeamDeadMatchMatchMap : uint8
{
	Dev3 UMETA(DisplayName = "Dev3"),
	Dev4 UMETA(DisplayName = "Dev4"),

	MAX UMETA(Hidden)
};
