#include "MutantSmoker.h"

#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/Characters/MutantAnimInstance.h"

AMutantSmoker::AMutantSmoker()
{
	MutantCharacterName = EMutantCharacterName::Smoker;
}

void AMutantSmoker::BeginPlay()
{
	Super::BeginPlay();

	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantAnimInstance->MutantCharacterName = EMutantCharacterName::Smoker;
	}
}

void AMutantSmoker::LightAttackButtonPressed(const FInputActionValue& Value)
{
	Super::LightAttackButtonPressed(Value);
}

void AMutantSmoker::LightAttackButtonReleased(const FInputActionValue& Value)
{
	Super::LightAttackButtonReleased(Value);
}

void AMutantSmoker::HeavyAttackButtonPressed(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonPressed(Value);
}

void AMutantSmoker::HeavyAttackButtonReleased(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonReleased(Value);
}

void AMutantSmoker::OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnRightHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMutantSmoker::OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnLeftHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
