#include "HumanState.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Net/UnrealNetwork.h"

void AHumanState::BeginPlay()
{
	Super::BeginPlay();
}

void AHumanState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHumanState, Defeat);
}

void AHumanState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetPawn());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->GetController());
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AHumanState::OnRep_Score()
{
	Super::OnRep_Score();

	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetPawn());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->GetController());
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AHumanState::AddToDefeat(int32 DefeatAmount)
{
	Defeat += DefeatAmount;
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetPawn());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller)
		{
			Controller->SetHUDDefeat(Defeat);
		}
	}
}

void AHumanState::OnRep_Defeat()
{
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetPawn());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller)
		{
			Controller->SetHUDDefeat(Defeat);
		}
	}
}
