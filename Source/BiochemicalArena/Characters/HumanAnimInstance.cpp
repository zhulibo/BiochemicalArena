#include "HumanAnimInstance.h"
#include "HumanCharacter.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "Components/CombatComponent.h"

void UHumanAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(TryGetPawnOwner());
	if (HumanCharacter == nullptr || HumanCharacter->GetCombatComponent() == nullptr) return;

	FVector Velocity = HumanCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = HumanCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = HumanCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bIsCrouched = HumanCharacter->bIsCrouched;
	bIsAiming = HumanCharacter->GetCombatComponent()->IsAiming();
	bIsDead = HumanCharacter->IsDead();

	// 计算瞄准方向与移动方向的偏移量，用于控制脚步朝向
	FRotator AimRotation = HumanCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(HumanCharacter->GetVelocity());
	FRotator DiffRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	AimYaw = DiffRotation.Yaw;

	// 根据AimPitch计算offset骨骼偏移量
	AimPitch = HumanCharacter->GetAimPitch();
	Head_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 30.f, -30.f);
	Neck_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 20.f, -20.f);

	UpperArm_R_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 75.f, -75.f);
	UpperArm_L_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 80.f, -80.f);

	if (HumanCharacter->IsLocallyControlled() && HumanCharacter->GetCurrentEquipment())
	{
		FTransform MuzzleTipTransform = HumanCharacter->GetCurrentEquipment()->GetEquipmentMesh()->GetSocketTransform(FName("Muzzle"), ERelativeTransformSpace::RTS_World);
		FVector MuzzleZ(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::Z));
		DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleZ * 2000.f, COLOR_MAIN);
	}
}
