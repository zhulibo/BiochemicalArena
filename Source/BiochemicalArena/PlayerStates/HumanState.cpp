#include "HumanState.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Net/UnrealNetwork.h"

void AHumanState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHumanState, Defeats);
}

void AHumanState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	if (Character == nullptr)
	{
		Character = Cast<AHumanCharacter>(GetPawn());
	}
	if (Character)
	{
		if (Controller == nullptr)
		{
			Controller = Cast<AHumanController>(Character->GetController());
		}
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AHumanState::OnRep_Score()
{
	Super::OnRep_Score();

	if (Character == nullptr)
	{
		Character = Cast<AHumanCharacter>(GetPawn());
	}
	if (Character)
	{
		if (Controller == nullptr)
		{
			Controller = Cast<AHumanController>(Character->GetController());
		}
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void AHumanState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	if (Character == nullptr)
	{
		Character = Cast<AHumanCharacter>(GetPawn());
	}
	if (Character)
	{
		if (Controller == nullptr)
		{
			// TODO Confirm if Character->Controller may be nullptr
			Controller = Cast<AHumanController>(Character->Controller);
		}
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void AHumanState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<AHumanCharacter>(GetPawn()) : Character;
	if (Character == nullptr)
	{
		Character = Cast<AHumanCharacter>(GetPawn());
	}
	if (Character)
	{
		if (Controller == nullptr)
		{
			Controller = Cast<AHumanController>(Character->Controller);
		}
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
