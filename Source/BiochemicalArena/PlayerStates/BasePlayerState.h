#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "BasePlayerState.generated.h"

enum class ETeam : uint8;
enum class EHumanCharacterName: uint8;
enum class EMutantCharacterName: uint8;

UCLASS()
class BIOCHEMICALARENA_API ABasePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABasePlayerState();

	virtual void Reset() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSetBase* GetAttributeSetBase();
	float GetMaxHealth();
	float GetHealth();
	float GetDamageReceivedMul();
	float GetRepelReceivedMul();
	float GetCharacterLevel();
	float GetJumpZVelocity();

	virtual void SetTeam(ETeam TempTeam);

	UFUNCTION(Server, Reliable)
	void ServerSetHumanCharacterName(EHumanCharacterName TempHumanCharacterName);
	UFUNCTION(Server, Reliable)
	void ServerSetMutantCharacterName(EMutantCharacterName TempMutantCharacterName);

	UFUNCTION()
	void SetMutantCharacterName(EMutantCharacterName TempMutantCharacterName);

	virtual void AddDamage(float TempDamage);
	void AddDefeat();
	void ResetKillStreak();
	void AddKillStreak();
	void ShowKillStreak();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual void InitData();

	UPROPERTY()
	class UBAAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UAttributeSetBase* AttributeSetBase;

	UPROPERTY()
	class ABaseCharacter* BaseCharacter;
	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	EHumanCharacterName HumanCharacterName;
	UPROPERTY()
	EMutantCharacterName MutantCharacterName;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere)
	ETeam Team;
	UFUNCTION()
	virtual void OnRep_Team();
	void InitOverheadWidget();

	UPROPERTY(ReplicatedUsing = OnRep_Damage)
	float Damage;
	UFUNCTION()
	virtual void OnRep_Damage();

	UPROPERTY(ReplicatedUsing = OnRep_Defeat)
	int32 Defeat;
	UFUNCTION()
	void OnRep_Defeat();

	UPROPERTY(ReplicatedUsing = OnRep_KillStreak)
	int32 KillStreak = 0;
	UFUNCTION()
	void OnRep_KillStreak();
	FTimerHandle ResetKillStreakTimerHandle;
	FTimerHandle ClearKillStreakTimerHandle;
	void HiddenKillStreak();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE EHumanCharacterName GetHumanCharacterName() const { return HumanCharacterName; }
	FORCEINLINE EMutantCharacterName GetMutantCharacterName() const { return MutantCharacterName; }

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
