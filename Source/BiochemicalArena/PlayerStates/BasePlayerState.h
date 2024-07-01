#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABasePlayerState();

	virtual void SetTeam(ETeam TempTeam);

	UFUNCTION(Server, Reliable)
	void ServerSetHumanCharacterName(const FString& TempHumanCharacterName);

	UFUNCTION()
	void SetMutantCharacterName(const FString& TempMutantCharacterName);

	virtual void AddDamage(float TempDamage);
	void AddDefeat();
	void ResetKillStreak();
	void AddKillStreak();
	void ShowKillStreak();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Team, VisibleAnywhere)
	ETeam Team;
	UFUNCTION()
	virtual void OnRep_Team();
	void SetPlayerNameTeamColor();

	UPROPERTY()
	class ABaseGameState* BaseGameState;
	UPROPERTY()
	class ABaseCharacter* BaseCharacter;
	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	FString HumanCharacterName = "SAS";
	UPROPERTY()
	FString MutantCharacterName = "Tank";

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
	FORCEINLINE FString GetHumanCharacterName() const { return HumanCharacterName; }
	FORCEINLINE FString GetMutantCharacterName() const { return MutantCharacterName; }

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetDefeat() const { return Defeat; }

};
