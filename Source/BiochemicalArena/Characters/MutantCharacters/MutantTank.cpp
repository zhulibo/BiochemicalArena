#include "MutantTank.h"

#include "BiochemicalArena/Characters/CharacterType.h"
#include "BiochemicalArena/Characters/MutantAnimInstance.h"

AMutantTank::AMutantTank()
{
	MaxHealth = 3000.f;
	Health = MaxHealth;

	LightAttackDamage = 60.f;
	HeavyAttackDamage = 70.f;
}

void AMutantTank::BeginPlay()
{
	Super::BeginPlay();

	MutantCharacterName = EMutantCharacterName::Tank;

	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantAnimInstance->MutantCharacterName = EMutantCharacterName::Tank;
	}
}

void AMutantTank::LightAttackButtonPressed(const FInputActionValue& Value)
{
	Super::LightAttackButtonPressed(Value);
}

void AMutantTank::LightAttackButtonReleased(const FInputActionValue& Value)
{
	Super::LightAttackButtonReleased(Value);
}

void AMutantTank::HeavyAttackButtonPressed(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonPressed(Value);
}

void AMutantTank::HeavyAttackButtonReleased(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonReleased(Value);
}

void AMutantTank::OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnRightHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMutantTank::OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnLeftHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}