#include "AnimInstance_Mutant.h"

#include "MutateArena/Characters/MutantCharacter.h"
#include "MutateArena/System/PlayerSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAnimInstance_Mutant::UAnimInstance_Mutant()
{
	// bUseMultiThreadedAnimationUpdate = false;
}

void UAnimInstance_Mutant::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MutantCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(TryGetPawnOwner());
	if (MutantCharacter == nullptr) return;

	FVector Velocity = MutantCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MutantCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = MutantCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bIsCrouched = MutantCharacter->bIsCrouched;

	// 计算瞄准方向与移动方向的偏移量，用于控制脚步朝向
	FRotator AimRotation = MutantCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MutantCharacter->GetVelocity());
	FRotator DiffRotation = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	AimYaw = DiffRotation.Yaw;

	/*
	* 人类被感染、开局被选为突变体、突变体切换角色等瞬间切换角色时，需要保持被销毁角色的AimPitch，但新角色生成后头几帧AimPitch为0，角色躯体闪动。
	* 因为Character中的Controller未就绪，无法获取APawn::GetViewRotation > AController::GetControlRotation。
	*/
	AimPitch = MutantCharacter->GetAimPitch();

	// HACK 本地死亡时，直接获取GetFirstPlayerController中的GetControlRotation().Pitch
	if (AimPitch == 0.f)
	{
		if (PlayerSubsystem == nullptr) PlayerSubsystem = ULocalPlayer::GetSubsystem<UPlayerSubsystem>(GetWorld()->GetFirstLocalPlayerFromController());
		if (PlayerSubsystem && PlayerSubsystem->IsDead) // IsDead状态只持续很短时间，尽可能避免将本地AimPitch应用给非本地角色/或本地角色延迟在出生点重生
		{
			if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
			{
				AimPitch = MutantCharacter->MappingAimPitch(PlayerController->GetControlRotation().Pitch);
			}
		}
	}
	// TODO 非本地角色躯体闪动暂未处理

	// 根据AimPitch计算骨骼偏移量
	Spine_01_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 20.f, -20.f);
	Spine_02_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 10.f, -10.f);
	Spine_03_Rotator.Roll = UKismetMathLibrary::MapRangeClamped(AimPitch, -90.f, 90.f, 60.f, -60.f);
}
