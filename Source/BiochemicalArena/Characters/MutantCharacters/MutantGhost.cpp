#include "MutantGhost.h"

#include "BiochemicalArena/Abilities/AttributeSetBase.h"
#include "BiochemicalArena/Abilities/BAAbilitySystemComponent.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/Characters/MutantAnimInstance.h"
#include "BiochemicalArena/Characters/Components/OverheadWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

AMutantGhost::AMutantGhost()
{
	MutantCharacterName = EMutantCharacterName::Ghost;
}

void AMutantGhost::BeginPlay()
{
	Super::BeginPlay();

	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantAnimInstance->MutantCharacterName = EMutantCharacterName::Ghost;
	}
}

void AMutantGhost::LightAttackButtonPressed(const FInputActionValue& Value)
{
	Super::LightAttackButtonPressed(Value);
}

void AMutantGhost::LightAttackButtonReleased(const FInputActionValue& Value)
{
	Super::LightAttackButtonReleased(Value);
}

void AMutantGhost::HeavyAttackButtonPressed(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonPressed(Value);
}

void AMutantGhost::HeavyAttackButtonReleased(const FInputActionValue& Value)
{
	Super::HeavyAttackButtonReleased(Value);
}

void AMutantGhost::OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnRightHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMutantGhost::OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnLeftHandCapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMutantGhost::OnAbilitySystemComponentInit()
{
	Super::OnAbilitySystemComponentInit();

	if (AbilitySystemComponent && AttributeSetBase)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSetBase->GetJumpZVelocityAttribute()).AddUObject(this, &ThisClass::OnJumpZVelocityChanged);
	}
}

void AMutantGhost::OnJumpZVelocityChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->JumpZVelocity = Data.NewValue;
}

void AMutantGhost::ShowOverheadWidget(bool bIsShow)
{
	if (OverheadWidgetClass)
	{
		OverheadWidgetClass->ShowOverheadWidget(bIsShow);
	}
}
