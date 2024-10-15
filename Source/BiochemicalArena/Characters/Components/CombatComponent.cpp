#include "CombatComponent.h"

#include "CombatStateType.h"
#include "RecoilComponent.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "..\..\Equipments\Throwing.h"
#include "BiochemicalArena/Characters/HumanAnimInstance.h"
#include "BiochemicalArena/Characters/Data/CharacterSound.h"
#include "BiochemicalArena/Equipments/EquipmentAnimInstance.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "BiochemicalArena/Equipments/Melee.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CombatState = ECombatState::Ready;

	// 模拟正在使用副武器，以便开局切换到主武器后，LastEquipmentType默认为副武器
	CurrentEquipmentType = EEquipmentType::Secondary;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (HumanCharacter)
	{
		HumanCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (HumanCharacter->GetCamera())
		{
			DefaultFOV = HumanCharacter->GetCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;

		InterpFOV(DeltaSeconds);
	}
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector Position;
	FVector Direction;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation, Position, Direction
	);

	if (bScreenToWorld)
	{
		FVector Start = Position + Direction * 100.f;
		FVector End = Start + Direction * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaSeconds)
{
	if (GetCurShotEquipment() == nullptr) return;

	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV * GetCurShotEquipment()->GetAimingFOVMul(), DeltaSeconds, GetCurShotEquipment()->GetAimSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaSeconds, GetCurShotEquipment()->GetAimSpeed());
	}

	if (HumanCharacter && HumanCharacter->GetCamera())
	{
		HumanCharacter->GetCamera()->SetFieldOfView(CurrentFOV);
	}
}

AEquipment* UCombatComponent::GetCurEquipment()
{
	return GetEquipmentByType(CurrentEquipmentType);
}

AWeapon* UCombatComponent::GetCurShotEquipment()
{
	switch (CurrentEquipmentType)
	{
	case EEquipmentType::Primary:
		return PrimaryEquipment;
	case EEquipmentType::Secondary:
		return SecondaryEquipment;
	default:
		return nullptr;
	}
}

AMelee* UCombatComponent::GetCurMeleeEquipment()
{
	switch (CurrentEquipmentType)
	{
	case EEquipmentType::Melee:
		return MeleeEquipment;
	default:
		return nullptr;
	}
}

AThrowing* UCombatComponent::GetCurThrowingEquipment()
{
	switch (CurrentEquipmentType)
	{
	case EEquipmentType::Throwing:
		return ThrowingEquipment;
	default:
		return nullptr;
	}
}

AEquipment* UCombatComponent::GetLastEquipment()
{
	return GetEquipmentByType(LastEquipmentType);
}

AEquipment* UCombatComponent::GetEquipmentByType(EEquipmentType EquipmentType)
{
	switch (EquipmentType)
	{
	case EEquipmentType::Primary:
		return PrimaryEquipment;
	case EEquipmentType::Secondary:
		return SecondaryEquipment;
	case EEquipmentType::Melee:
		return MeleeEquipment;
	case EEquipmentType::Throwing:
		return ThrowingEquipment;
	default:
		return nullptr;
	}
}

bool UCombatComponent::HasEquippedThisTypeEquipment(EEquipmentType EquipmentType)
{
	switch (EquipmentType)
	{
	case EEquipmentType::Primary:
		return PrimaryEquipment != nullptr;
	case EEquipmentType::Secondary:
		return SecondaryEquipment != nullptr;
	case EEquipmentType::Melee:
		return MeleeEquipment != nullptr;
	case EEquipmentType::Throwing:
		return ThrowingEquipment != nullptr;
	default:
		return false;
	}
}

void UCombatComponent::EquipEquipment(AEquipment* Equipment)
{
	LocalEquipEquipment(Equipment);
	ServerEquipEquipment(Equipment);
}

void UCombatComponent::ServerEquipEquipment_Implementation(AEquipment* Equipment)
{
	MulticastEquipEquipment(Equipment);
}

void UCombatComponent::MulticastEquipEquipment_Implementation(AEquipment* Equipment)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalEquipEquipment(Equipment);
	}
}

void UCombatComponent::MulticastEquipEquipment2_Implementation(AEquipment* Equipment)
{
	if (Equipment == nullptr) return;

	// 取消补给箱装备隐藏
	Equipment->GetEquipmentMesh()->SetVisibility(true);

	LocalEquipEquipment(Equipment);
}

void UCombatComponent::LocalEquipEquipment(AEquipment* Equipment)
{
	if (Equipment == nullptr || HumanCharacter == nullptr) return;

	Equipment->SetOwner(HumanCharacter);
	Equipment->OnEquip();

	AssignEquipment(Equipment);

	if (AssetSubsystem == nullptr) AssetSubsystem = HumanCharacter->GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem && AssetSubsystem->CharacterSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->CharacterSound->EquipSound, HumanCharacter->GetActorLocation());
	}

	AttachToBodySocket(Equipment);
}

void UCombatComponent::AssignEquipment(AEquipment* Equipment)
{
	if (Equipment == nullptr) return;

	switch (Equipment->GetEquipmentType())
	{
	case EEquipmentType::Primary:
		if (AWeapon* TempEquipment = Cast<AWeapon>(Equipment)) PrimaryEquipment = TempEquipment;
		break;
	case EEquipmentType::Secondary:
		if (AWeapon* TempEquipment = Cast<AWeapon>(Equipment)) SecondaryEquipment = TempEquipment;
		break;
	case EEquipmentType::Melee:
		if (AMelee* TempEquipment = Cast<AMelee>(Equipment)) MeleeEquipment = TempEquipment;
		break;
	case EEquipmentType::Throwing:
		if (AThrowing* TempEquipment = Cast<AThrowing>(Equipment)) ThrowingEquipment = TempEquipment;
		break;
	}
}

void UCombatComponent::AttachToBodySocket(AEquipment* Equipment)
{
	if (Equipment == nullptr || HumanCharacter == nullptr || HumanCharacter->GetMesh() == nullptr) return;

	FName BodySocketName;
	switch (Equipment->GetEquipmentType())
	{
	case EEquipmentType::Primary:
		BodySocketName = FName("Shoulder_R");
		break;
	case EEquipmentType::Secondary:
		BodySocketName = FName("Thigh_R");
		break;
	case EEquipmentType::Melee:
		BodySocketName = FName("Shoulder_L");
		break;
	case EEquipmentType::Throwing:
		BodySocketName = FName("Thigh_L");
		break;
	}

	if (const USkeletalMeshSocket* BodySocket = HumanCharacter->GetMesh()->GetSocketByName(BodySocketName))
	{
		BodySocket->AttachActor(Equipment, HumanCharacter->GetMesh());
	}
}

void UCombatComponent::SwapEquipment(EEquipmentType EquipmentType)
{
	if (CurrentEquipmentType != EquipmentType)
	{
		LocalSwapEquipment(EquipmentType);
		ServerSwapEquipment(EquipmentType);
	}
}

void UCombatComponent::ServerSwapEquipment_Implementation(EEquipmentType EquipmentType)
{
	MulticastSwapEquipment(EquipmentType);
}

void UCombatComponent::MulticastSwapEquipment_Implementation(EEquipmentType EquipmentType)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalSwapEquipment(EquipmentType);
	}
}

void UCombatComponent::LocalSwapEquipment(EEquipmentType EquipmentType)
{
	if (AEquipment* NewEquipment = GetEquipmentByType(EquipmentType))
	{
		PreLocalSwapEquipment();

		PlaySwapOutMontage(NewEquipment);
	}
}

void UCombatComponent::PreLocalSwapEquipment()
{
	GetWorld()->GetTimerManager().ClearTimer(LoadNewBulletTimerHandle);

	bCanFire = true;

	bIsAiming = false;

	CombatState = ECombatState::Swapping;
}

// 播放当前装备切出动画
void UCombatComponent::PlaySwapOutMontage(AEquipment* NewEquipment)
{
	// 记录旧装备（不等切出动画完成，适应快速切枪）
	if (NewEquipment)
	{
		LastEquipmentType = CurrentEquipmentType;
		CurrentEquipmentType = NewEquipment->GetEquipmentType();
	}

	if (GetLastEquipment())
	{
		GetLastEquipment()->OnStartSwapOut();

		if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
		if (HumanAnimInstance)
		{
			// 播放切出时的角色动画
			HumanAnimInstance->Montage_Play(GetLastEquipment()->SwapOutMontage_C);

			// 播放切出时的装备动画
			if (GetLastEquipment()->GetEquipmentAnimInstance())
			{
				GetLastEquipment()->GetEquipmentAnimInstance()->Montage_Play(GetLastEquipment()->SwapOutMontage_E);
			}

			// 切出动画播放完后播放切入动画
			// HACK 依赖混出时间
			FOnMontageBlendingOutStarted OnMontageBlendingOutStarted;
			OnMontageBlendingOutStarted.BindWeakLambda(this, [this, NewEquipment](UAnimMontage* AnimMontage, bool bInterrupted)
			{
				PlaySwapInMontage(bInterrupted, NewEquipment);
			});
			// Don't use Montage_SetEndDelegate, if SwapOutMontage ended, current frame will be the base pose.
			HumanAnimInstance->Montage_SetBlendingOutDelegate(OnMontageBlendingOutStarted, GetLastEquipment()->SwapOutMontage_C);
		}
	}
	// 投掷装备扔出后切换到上一个武器 / 开局赋予武器时，当前武器为空
	else
	{
		PlaySwapInMontage(false, NewEquipment);
	}
}

// 播放新装备切入动画
void UCombatComponent::PlaySwapInMontage(bool bInterrupted, AEquipment* NewEquipment)
{
	if (bInterrupted) return;

	// 切出旧装备
	if (AEquipment* LastEquipment = GetLastEquipment())
	{
		AttachToBodySocket(LastEquipment);
	}

	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance)
	{
		// 播放切入时的角色动画
		HumanAnimInstance->Montage_Play(NewEquipment->SwapInMontage_C);

		// 播放切入时的装备动画
		if (NewEquipment->GetEquipmentAnimInstance())
		{
			NewEquipment->GetEquipmentAnimInstance()->Montage_Play(NewEquipment->SwapInMontage_E);
		}
	}

	UseEquipment(NewEquipment);
}

void UCombatComponent::MulticastReplaceCurEquipment_Implementation(AEquipment* Equipment)
{
	if (Equipment == nullptr) return;

	// 取消补给箱装备隐藏
	Equipment->GetEquipmentMesh()->SetVisibility(true);

	Equipment->SetOwner(HumanCharacter);
	Equipment->OnEquip();

	bIsAiming = false;

	AssignEquipment(Equipment);
	UseEquipment(Equipment);
}

void UCombatComponent::UseEquipment(AEquipment* Equipment)
{
	if (Equipment == nullptr || HumanCharacter == nullptr) return;

	AttachToRightHand(Equipment);

	// 记录装备名字，以便动画蓝图应用不同的idle animation
	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance)
	{
		HumanAnimInstance->EquipmentName = Equipment->GetEquipmentName();
	}

	// 更新子弹
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(HumanCharacter->Controller);
	if (HumanCharacter->IsLocallyControlled() && GetCurEquipment() && BaseController)
	{
		if (GetCurShotEquipment())
		{
			BaseController->SetHUDAmmo(GetCurShotEquipment()->GetAmmo());
			BaseController->SetHUDCarriedAmmo(GetCurShotEquipment()->GetCarriedAmmo());
		}
		else
		{
			BaseController->SetHUDAmmo(0);
			BaseController->SetHUDCarriedAmmo(0);
		}
	}

	// 更新玩家速度
	HumanCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * Equipment->GetMoveSpeedMul();
}

void UCombatComponent::FinishSwap()
{
	if (HumanCharacter)
	{
		CombatState = ECombatState::Ready;
	}
}

void UCombatComponent::AttachToRightHand(AEquipment* Equipment)
{
	AttachToHand(Equipment, "_R");
}

void UCombatComponent::AttachToLeftHand(AEquipment* Equipment)
{
	AttachToHand(Equipment, "_L");
}

void UCombatComponent::AttachToHand(AEquipment* Equipment, FString SocketNameSuffix)
{
	if (HumanCharacter == nullptr || HumanCharacter->GetMesh() == nullptr || Equipment == nullptr || SocketNameSuffix.IsEmpty()) return;

	EEquipmentName TempEquipmentName = Equipment->GetEquipmentParentName() == EEquipmentName::NONE ?
		Equipment->GetEquipmentName() : Equipment->GetEquipmentParentName();
	FString EquipmentName = UEnum::GetValueAsString(TempEquipmentName);
	EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2) + SocketNameSuffix;

	const USkeletalMeshSocket* HandSocket = HumanCharacter->GetMesh()->GetSocketByName(*EquipmentName);
	if (HandSocket)
	{
		HandSocket->AttachActor(Equipment, HumanCharacter->GetMesh());
	}
}

void UCombatComponent::SetAiming(bool bNewAimingState)
{
	LocalSetAiming(bNewAimingState);
	ServerSetAiming(bNewAimingState);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bNewAimingState)
{
	MulticastSetAiming(bNewAimingState);
}

void UCombatComponent::MulticastSetAiming_Implementation(bool bNewAimingState)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalSetAiming(bNewAimingState);
	}
}

void UCombatComponent::LocalSetAiming(bool bNewAimingState)
{
	if (GetCurEquipment() == nullptr) return;

	bIsAiming = bNewAimingState;

	if (HumanCharacter)
	{
		float Speed;
		if (bIsAiming)
		{
			Speed = AimWalkSpeed * GetCurEquipment()->GetMoveSpeedMul();
		}
		else
		{
			Speed = BaseWalkSpeed * GetCurEquipment()->GetMoveSpeedMul();
		}

		HumanCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}

void UCombatComponent::StartFire()
{
	bFireButtonPressed = true;

	bIsFirstShot = true;

	Fire();
}

void UCombatComponent::StopFire()
{
	bFireButtonPressed = false;

	// 重置水平后坐力方向
	if (HumanCharacter->GetRecoilComponent())
	{
		HumanCharacter->GetRecoilComponent()->SetRecoilHorDirection(ERecoilHorDirection::Random);
	}
}

void UCombatComponent::Fire()
{
	if (HumanCharacter == nullptr || HumanCharacter->GetRecoilComponent() == nullptr || GetCurShotEquipment() == nullptr) return;

	if (CanFire())
	{
		bCanFire = false;

		// 获取后坐力
		float RecoilVert = HumanCharacter->GetRecoilComponent()->GetCurRecoilVert();
		float RecoilHor = HumanCharacter->GetRecoilComponent()->GetCurRecoilHor();

		LocalFire(HitTarget, RecoilVert, RecoilHor);
		ServerFire(HitTarget, RecoilVert, RecoilHor);

		// 开火后增加后坐力
		HumanCharacter->GetRecoilComponent()->IncRecoil();

		// 子弹上膛
		GetWorld()->GetTimerManager().SetTimer(LoadNewBulletTimerHandle, this, &ThisClass::LoadNewBulletFinished, GetCurShotEquipment()->GetFireDelay());
	}
}

bool UCombatComponent::CanFire()
{
	if (!bCanFire) return false;

	if (GetCurShotEquipment() == nullptr) return false;

	// 子弹耗尽
	if (GetCurShotEquipment()->IsEmpty())
	{
		// 播放击锤音效
		if (AssetSubsystem == nullptr) AssetSubsystem = HumanCharacter->GetGameInstance()->GetSubsystem<UAssetSubsystem>();
		if (AssetSubsystem && AssetSubsystem->CharacterSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->CharacterSound->ClickSound, HumanCharacter->GetActorLocation());
		}

		return false;
	}

	// Shotgun正在上膛时可以开火
	if (CombatState == ECombatState::Reloading
		&& GetCurShotEquipment()->GetEquipmentCate() == EEquipmentCate::Shotgun) return true;

	return CombatState == ECombatState::Ready;
}

// 子弹上膛完成
void UCombatComponent::LoadNewBulletFinished()
{
	bCanFire = true;

	// 自动武器持续射击
	if (bFireButtonPressed && GetCurShotEquipment() && GetCurShotEquipment()->IsAutomatic())
	{
		if (bIsFirstShot == true) bIsFirstShot = false;

		Fire();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float RecoilVert, float RecoilHor)
{
	MulticastFire(TraceHitTarget, RecoilVert, RecoilHor);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float RecoilVert, float RecoilHor)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalFire(TraceHitTarget, RecoilVert, RecoilHor);
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget, float RecoilVert, float RecoilHor)
{
	if (HumanCharacter == nullptr || GetCurShotEquipment() == nullptr) return;

	PlayFireMontage();
	GetCurShotEquipment()->Fire(TraceHitTarget, RecoilVert, RecoilHor);

	if (CombatState == ECombatState::Reloading && GetCurShotEquipment()->GetEquipmentCate() == EEquipmentCate::Shotgun)
	{
		CombatState = ECombatState::Ready;
	}
}

void UCombatComponent::PlayFireMontage()
{
	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance && GetCurShotEquipment())
	{
		HumanAnimInstance->Montage_Play(GetCurShotEquipment()->FireMontage_C);
	}
}

void UCombatComponent::Reload()
{
	if (GetCurShotEquipment() && GetCurShotEquipment()->GetCarriedAmmo() > 0 && !GetCurShotEquipment()->IsFull() && CombatState == ECombatState::Ready)
	{
		LocalReload();
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	MulticastReload();
}

void UCombatComponent::MulticastReload_Implementation()
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalReload();
	}
}

void UCombatComponent::LocalReload()
{
	CombatState = ECombatState::Reloading;

	PlayReloadMontage();
}

void UCombatComponent::PlayReloadMontage()
{
	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance && GetCurShotEquipment())
	{
		HumanAnimInstance->Montage_Play(GetCurShotEquipment()->ReloadMontage_C);

		if (GetCurShotEquipment()->GetEquipmentAnimInstance())
		{
			GetCurShotEquipment()->GetEquipmentAnimInstance()->Montage_Play(GetCurShotEquipment()->ReloadMontage_E);
		}
	}
}

void UCombatComponent::FinishReload()
{
	if (HumanCharacter == nullptr || GetCurShotEquipment() == nullptr) return;

	CombatState = ECombatState::Ready;

	int32 Ammo = GetCurShotEquipment()->GetAmmo();
	int32 MagCapacity = GetCurShotEquipment()->GetMagCapacity();
	int32 CarriedAmmo = GetCurShotEquipment()->GetCarriedAmmo();
	int32 NeedAmmoNum = MagCapacity - Ammo;

	if (CarriedAmmo > NeedAmmoNum)
	{
		GetCurShotEquipment()->SetAmmo(MagCapacity);
		GetCurShotEquipment()->SetCarriedAmmo(CarriedAmmo - NeedAmmoNum);
	}
	else
	{
		GetCurShotEquipment()->SetAmmo(Ammo + CarriedAmmo);
		GetCurShotEquipment()->SetCarriedAmmo(0);
	}
}

void UCombatComponent::ShellReload()
{
	if (HumanCharacter == nullptr || GetCurShotEquipment() == nullptr) return;

	GetCurShotEquipment()->SetAmmo(GetCurShotEquipment()->GetAmmo() + 1);
	GetCurShotEquipment()->SetCarriedAmmo(GetCurShotEquipment()->GetCarriedAmmo() - 1);

	// 装入一发可立即开火
	bCanFire = true;

	// 弹匣装满或没有备弹时结束换弹
	if (GetCurShotEquipment()->IsFull() || GetCurShotEquipment()->GetCarriedAmmo() == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance && GetCurShotEquipment())
	{
		HumanAnimInstance->Montage_JumpToSection(FName("ReloadEnd"));

		if (GetCurShotEquipment()->GetEquipmentAnimInstance())
		{
			GetCurShotEquipment()->GetEquipmentAnimInstance()->Montage_JumpToSection(FName("ReloadEnd"));
		}
	}
}

void UCombatComponent::DropEquipment(EEquipmentType EquipmentType)
{
	LocalDropEquipment(EquipmentType);
	ServerDropEquipment(EquipmentType);
}

void UCombatComponent::ServerDropEquipment_Implementation(EEquipmentType EquipmentType)
{
	MulticastDropEquipment(EquipmentType);
}

void UCombatComponent::MulticastDropEquipment_Implementation(EEquipmentType EquipmentType)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalDropEquipment(EquipmentType);
	}
}

void UCombatComponent::MulticastDropEquipment2_Implementation(EEquipmentType EquipmentType)
{
	LocalDropEquipment(EquipmentType);
}

void UCombatComponent::LocalDropEquipment(EEquipmentType EquipmentType)
{
	if (GetEquipmentByType(EquipmentType))
	{
		GetEquipmentByType(EquipmentType)->OnDrop();

		switch (EquipmentType)
		{
		case EEquipmentType::Primary:
			PrimaryEquipment = nullptr;
			break;
		case EEquipmentType::Secondary:
			SecondaryEquipment = nullptr;
			break;
		}
	}
}

void UCombatComponent::DestroyEquipments()
{
	if (PrimaryEquipment) PrimaryEquipment->Destroy();
	if (SecondaryEquipment) SecondaryEquipment->Destroy();
	if (MeleeEquipment) MeleeEquipment->Destroy();
	if (ThrowingEquipment) ThrowingEquipment->Destroy();
}

void UCombatComponent::MeleeAttack(ECombatState CombatStateToSet)
{
	if (CombatState != ECombatState::Ready) return;

	LocalMeleeAttack(CombatStateToSet);
	ServerMeleeAttack(CombatStateToSet);
}

void UCombatComponent::ServerMeleeAttack_Implementation(ECombatState CombatStateToSet)
{
	MulticastMeleeAttack(CombatStateToSet);
}

void UCombatComponent::MulticastMeleeAttack_Implementation(ECombatState CombatStateToSet)
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalMeleeAttack(CombatStateToSet);
	}
}

void UCombatComponent::LocalMeleeAttack(ECombatState CombatStateToSet)
{
	if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
	if (HumanAnimInstance && GetCurMeleeEquipment())
	{
		CombatState = CombatStateToSet;

		if (CombatState == ECombatState::LightAttacking)
		{
			HumanAnimInstance->Montage_Play(GetCurMeleeEquipment()->LightAttackMontage_C);
			if (GetCurMeleeEquipment()->GetEquipmentAnimInstance())
			{
				GetCurMeleeEquipment()->GetEquipmentAnimInstance()->Montage_Play(GetCurMeleeEquipment()->LightAttackMontage_E);
			}
		}
		else if (CombatState == ECombatState::HeavyAttacking)
		{
			HumanAnimInstance->Montage_Play(GetCurMeleeEquipment()->HeavyAttackMontage_C);
			if (GetCurMeleeEquipment()->GetEquipmentAnimInstance())
			{
				GetCurMeleeEquipment()->GetEquipmentAnimInstance()->Montage_Play(GetCurMeleeEquipment()->HeavyAttackMontage_E);
			}
		}
	}
}

void UCombatComponent::SetAttackCollisionEnabled(bool bIsEnabled)
{
	if (MeleeEquipment)
	{
		MeleeEquipment->SetAttackCollisionEnabled(bIsEnabled);
	}

	if (!bIsEnabled)
	{
		CombatState = ECombatState::Ready;
	}
}

void UCombatComponent::Throw()
{
	LocalThrow();
	ServerThrow();
}

void UCombatComponent::ServerThrow_Implementation()
{
	MulticastThrow();
}

void UCombatComponent::MulticastThrow_Implementation()
{
	if (HumanCharacter && !HumanCharacter->IsLocallyControlled())
	{
		LocalThrow();
	}
}

void UCombatComponent::LocalThrow()
{
	if (CombatState == ECombatState::Ready)
	{
		if (HumanAnimInstance == nullptr) HumanAnimInstance = Cast<UHumanAnimInstance>(HumanCharacter->GetMesh()->GetAnimInstance());
		if (HumanAnimInstance && GetCurThrowingEquipment())
		{
			HumanAnimInstance->Montage_Play(GetCurThrowingEquipment()->ThrowMontage_C);

			if (GetCurThrowingEquipment()->GetEquipmentAnimInstance())
			{
				GetCurThrowingEquipment()->GetEquipmentAnimInstance()->Montage_Play(GetCurThrowingEquipment()->ThrowMontage_E);
			}

			CombatState = ECombatState::Throwing;
		}
	}
}

void UCombatComponent::ThrowOut()
{
	if (ThrowingEquipment)
	{
		ThrowingEquipment->ThrowOut();

		ThrowingEquipment = nullptr;
		CombatState = ECombatState::Ready;

		// 切换至上一个装备
		if (HumanCharacter && HumanCharacter->IsLocallyControlled())
		{
			switch (LastEquipmentType)
			{
			case EEquipmentType::Primary:
				if (PrimaryEquipment) SwapEquipment(EEquipmentType::Primary);
				break;
			case EEquipmentType::Secondary:
				if (SecondaryEquipment) SwapEquipment(EEquipmentType::Secondary);
				break;
			case EEquipmentType::Melee:
				if (MeleeEquipment) SwapEquipment(EEquipmentType::Melee);
				break;
			}
		}
	}
}
