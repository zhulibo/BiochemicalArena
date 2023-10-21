#include "HumanAnimation.h"
#include "HumanCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BiochemicalArena/Weapons/Weapon.h"

void UHumanAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	HumanCharacter = Cast<AHumanCharacter>(TryGetPawnOwner());
}

void UHumanAnimation::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (HumanCharacter == nullptr)
	{
		HumanCharacter = Cast<AHumanCharacter>(TryGetPawnOwner());
	}
	if (HumanCharacter == nullptr) return;

	FVector Velocity = HumanCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = HumanCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = HumanCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsCrouched = HumanCharacter->bIsCrouched;
	bAiming = HumanCharacter->IsAiming();
	bElimmed = HumanCharacter->IsElimmed();
	AO_Pitch = HumanCharacter->GetAO_Pitch();

	// 瞄准方向与移动方向的偏移量，用于控制脚步朝向
	FRotator AimRotation = HumanCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(HumanCharacter->GetVelocity());
	FRotator DiffRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	YawOffset = DiffRotation.Yaw;

	bWeaponEquipped = HumanCharacter->IsWeaponEquipped();
	EquippedWeapon = HumanCharacter->GetEquippedWeapon();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && HumanCharacter->GetMesh())
	{
		// 获取武器左手插槽的世界坐标
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		// 转换为相对于右手骨骼空间的坐标
		HumanCharacter->GetMesh()->TransformToBoneSpace(FName("RightHand"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
		// 红色为准心指向，黄色为枪管指向
		if (HumanCharacter->IsLocallyControlled())
		{
			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleSocket"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 2000.f, FColor::Yellow);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), HumanCharacter->GetHitTarget(), FColor::Red);
		}
	}

	bUseLeftHandFABRIK = HumanCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
}
