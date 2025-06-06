﻿#pragma once

UENUM(BlueprintType)
enum class ECoolGameMode : uint8
{
	Mutation,
	Melee,
	TeamDeadMatch,

	None
};

UENUM(BlueprintType)
enum class EMutationMap : uint8
{
	Colosseum,
	CrossStreet,
	Dust,
	Farm,

	None
};

UENUM(BlueprintType)
enum class EMeleeMap : uint8
{
	Arena,

	None
};

UENUM(BlueprintType)
enum class ETeamDeadMatchMap : uint8
{
	TrainingField,

	None
};
