#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "MutationPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLocalCharacterTeamChange, ETeam TempTeam);

UCLASS()
class BIOCHEMICALARENA_API AMutationPlayerState : public ABasePlayerState
{
	GENERATED_BODY()

public:
	virtual void SetTeam(ETeam TempTeam) override;
	FOnLocalCharacterTeamChange OnLocalCharacterTeamChange;

	virtual void AddDamage(float TempDamage) override;

	void SetRage(float TempRage);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	virtual void InitData() override;

	UPROPERTY()
	class AMutationController* MutationController;

	virtual void OnRep_Team() override;

	virtual void OnRep_Damage() override;
	float BaseDamage = 0;
	void Show1000DamageUI();

	UPROPERTY(ReplicatedUsing = OnRep_Rage)
	float Rage = 0.f;
	UFUNCTION()
	void OnRep_Rage();

public:
	FORCEINLINE float GetRage() const { return Rage; }

};
