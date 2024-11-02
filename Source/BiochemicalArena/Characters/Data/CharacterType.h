#pragma once

#include "Engine/DataTable.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class EHumanCharacterName : uint8
{
	SAS,
	PLA,
	SEAL,
	Test,

	NONE
};

UENUM(BlueprintType)
enum class EMutantCharacterName : uint8
{
	Tank,
	Ghost,
	Smoker,

	NONE
};

USTRUCT(BlueprintType)
struct FHumanCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHumanCharacter> HumanCharacterClass;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether account own the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId;

};

USTRUCT(BlueprintType)
struct FMutantCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::NONE;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMutantCharacter> MutantCharacterClass;

	UPROPERTY(EditAnywhere)
	float LightAttackDamage = 0.f;
	UPROPERTY(EditAnywhere)
	float HeavyAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	FString Desc = "";

};

UENUM(BlueprintType)
enum class ESpawnReason : uint8
{
	// 开局随机突变
	RoundStart,
	// 被感染
	Infect,
	// 被突变体造成伤害
	MutantDamage,
	// 通过菜单选择突变体
	SelectMutant,
	// 突变体死亡重生
	Respawn,
};
