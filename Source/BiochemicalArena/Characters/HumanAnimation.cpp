#include "HumanAnimation.h"
#include "HumanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BiochemicalArena/Equipments/Equipment.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "Components/CombatStateType.h"

void UHumanAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(TryGetPawnOwner());
	if (HumanCharacter == nullptr) return;

	FVector Velocity = HumanCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = HumanCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = HumanCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bIsCrouched = HumanCharacter->bIsCrouched;
	bIsAiming = HumanCharacter->IsAiming();
	bIsKilled = HumanCharacter->IsKilled();
	AO_Pitch = HumanCharacter->GetAO_Pitch();

	// 瞄准方向与移动方向的偏移量，用于控制脚步朝向
	FRotator AimRotation = HumanCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(HumanCharacter->GetVelocity());
	FRotator DiffRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	YawOffset = DiffRotation.Yaw;

	CurrentEquipment = HumanCharacter->GetCurrentEquipment();
	if (CurrentEquipment == nullptr)
	{
		bIsUseLeftHandFABRIK = false;
		return;
	}

	bIsUseLeftHandFABRIK = (CurrentEquipment->GetEquipmentType() == EEquipmentType::Primary || CurrentEquipment->GetEquipmentType() == EEquipmentType::Secondary) &&
		HumanCharacter->GetCombatState() == ECombatState::Ready;

	if (bIsUseLeftHandFABRIK && CurrentEquipment->GetEquipmentMesh() && HumanCharacter->GetMesh())
	{
		// 获取武器左手插槽的世界坐标
		LeftHandTransform = CurrentEquipment->GetEquipmentMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		// 转换为相对于右手骨骼空间的坐标
		HumanCharacter->GetMesh()->TransformToBoneSpace(FName("Hand_R"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
		// 红色为准心指向，黄色为枪管指向
		if (HumanCharacter->IsLocallyControlled())
		{
			FTransform MuzzleTipTransform = CurrentEquipment->GetEquipmentMesh()->GetSocketTransform(FName("MuzzleSocket"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 2000.f, FColor::Yellow);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), HumanCharacter->GetHitTarget(), FColor::Red);
		}
	}
}
