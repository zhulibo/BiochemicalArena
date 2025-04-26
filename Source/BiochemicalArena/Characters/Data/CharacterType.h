#pragma once

#include "Engine/DataTable.h"
#include "CharacterType.generated.h"

UENUM(BlueprintType)
enum class EHumanCharacterName : uint8
{
	SAS,
	SEAL,
	PLA,
	Test,

	None
};

UENUM(BlueprintType)
enum class EMutantCharacterName : uint8
{
	Tank,
	Ghost,
	Smoker,
	Terminator,

	None
};

USTRUCT(BlueprintType)
struct FHumanCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EHumanCharacterName HumanCharacterName = EHumanCharacterName::None;

	UPROPERTY(EditAnywhere)
	FString ShowName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHumanCharacter> HumanCharacterClass;

	/**
	 * Must be the same as the OfferId in dev portal,
	 * Used to determine the button type in the UShop::OnQueryOffersComplete.
	 */
	UPROPERTY(EditAnywhere)
	FString OfferId;

	/**
	 * Must be same as the audience item ID in dev portal,
	 * Used to determine whether player's account owns the product.
	 */
	UPROPERTY(EditAnywhere)
	FString AudienceItemId;

};

USTRUCT(BlueprintType)
struct FMutantCharacterMain : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMutantCharacterName MutantCharacterName = EMutantCharacterName::None;

	UPROPERTY(EditAnywhere)
	FString ShowName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMutantCharacter> MutantCharacterClass; // TODO TSoftClassPtr

	UPROPERTY(EditAnywhere)
	float LightAttackDamage = 0.f;
	UPROPERTY(EditAnywhere)
	float HeavyAttackDamage = 0.f;

	UPROPERTY(EditAnywhere)
	FString Desc = "";

};

UENUM(BlueprintType)
enum class ESpawnMutantReason : uint8
{
	// 开局随机突变
	RoundStart,
	// 被感染
	MutantInfect,
	// 被突变体造成伤害
	MutantDamage,
	// 通过菜单选择突变体
	SelectMutant,
	// 突变体死亡重生
	Respawn,
};
