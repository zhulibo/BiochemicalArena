#include "CrosshairComponent.h"

#include "CombatComponent.h"
#include "RecoilComponent.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "TimerManager.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "GameFramework/CharacterMovementComponent.h"

UCrosshairComponent::UCrosshairComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCrosshairComponent::BeginPlay()
{
	Super::BeginPlay();

	// if (HumanCharacter)
	// {
	// 	WalkSpeedRange = FVector2D(0.f, HumanCharacter->GetCharacterMovement()->MaxWalkSpeed);
	// }
}

void UCrosshairComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		SetHUDCrosshair(DeltaSeconds);
	}
}


void UCrosshairComponent::SetHUDCrosshair(float DeltaSeconds)
{
	if (HumanCharacter == nullptr || HumanCharacter->GetCombatComponent() == nullptr) return;
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(HumanCharacter->Controller);
	if (BaseController == nullptr) return;

	AWeapon* Weapon = HumanCharacter->GetCombatComponent()->GetCurShotEquipment();
	if (Weapon == nullptr) return;

	// 水平速度
	// FVector Velocity = HumanCharacter->GetVelocity();
	// Velocity.Z = 0.f;
	// VelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityFactorRange, Velocity.Size());

	// 跳跃
	// if (HumanCharacter->GetCharacterMovement()->IsFalling())
	// {
	// 	JumpFactor = FMath::FInterpTo(JumpFactor, 1.f, DeltaSeconds, 10.f);
	// }
	// else
	// {
	// 	JumpFactor = FMath::FInterpTo(JumpFactor, 0.f, DeltaSeconds, 20.f);
	// }

	// 瞄准
	// if (HumanCharacter->GetCombatComponent()->IsAiming())
	// {
	// 	AimFactor = FMath::FInterpTo(AimFactor, -0.2f, DeltaSeconds, 10.f);
	// }
	// else
	// {
	// 	AimFactor = FMath::FInterpTo(AimFactor, 0.f, DeltaSeconds, 20.f);
	// }

	// 射击
	if (URecoilComponent* RecoilComponent = HumanCharacter->GetRecoilComponent())
	{
		ShootFactor = (RecoilComponent->GetCurRecoilVert() + FMath::Abs(RecoilComponent->GetCurRecoilVert()))
		/ (Weapon->RecoilTotalVertLimit + Weapon->RecoilTotalHorLimit);
	}

	float TotalFactor = 1.f + VelocityFactor + JumpFactor + AimFactor + ShootFactor * 2.f;

	float BaseWeaponSpread = (Weapon->RecoilMaxVert + Weapon->RecoilMaxHor) * 4;

	BaseController->ChangeCrosshairSpread.Broadcast(2.f + BaseWeaponSpread * TotalFactor);
}
