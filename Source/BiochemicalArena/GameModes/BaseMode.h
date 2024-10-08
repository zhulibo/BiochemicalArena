#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseMode.generated.h"

enum class ETeam : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABaseMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABaseMode();

	float LevelStartTime = 0.f;

	virtual void HumanReceiveDamage(class AHumanCharacter* DamagedCharacter, class ABaseController* DamagedController,
		float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser) {}
	virtual void MutantReceiveDamage(class AMutantCharacter* DamagedCharacter, ABaseController* DamagedController,
	float Damage, const UDamageType* DamageType, AController* AttackerController, AActor* DamageCauser) {}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class ABaseGameState* BaseGameState;

	void SpawnHumanCharacter(AController* Controller);
	void SpawnMutantCharacter(AController* Controller, bool bSpawnByInfectOrChosen = false,
		FVector Location = FVector::ZeroVector, FRotator ActorRotation = FRotator::ZeroRotator, FRotator ViewRotation = FRotator::ZeroRotator);

	TArray<class APlayerStart*> Team1PlayerStarts;
	TArray<APlayerStart*> Team2PlayerStarts;
	AActor* FindCharacterPlayerStart(ETeam Team);
	void AssignTeam(AController* Controller, ETeam Team);

	void AddKillLog(class ABasePlayerState* AttackerState, AActor* DamageCauser, const UDamageType* DamageType, ABasePlayerState* DamagedState);

};
