#include "CombatComponent.h"

#include "CombatStateType.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "..\..\Equipments\Throwing.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "BiochemicalArena/Equipments/Melee.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CombatState);

	DOREPLIFETIME(ThisClass, PrimaryEquipment);
	DOREPLIFETIME(ThisClass, SecondaryEquipment);
	DOREPLIFETIME(ThisClass, MeleeEquipment);
	DOREPLIFETIME(ThisClass, ThrowingEquipment);
	DOREPLIFETIME(ThisClass, CurrentEquipmentType);
	DOREPLIFETIME(ThisClass, LastEquipmentType);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetCamera())
		{
			DefaultFOV = Character->GetCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}

	EquipSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	ClickSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
}

void UCombatComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshair(DeltaSeconds);
		InterpFOV(DeltaSeconds);
	}
}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

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

void UCombatComponent::SetHUDCrosshair(float DeltaSeconds)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);

	if (Controller)
	{
		// Calculate crosshair spread [0, 600] -> [0, 1]
		FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
		FVector2D VelocityMultiplierRange(0.f, 1.f);
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;
		CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

		if (Character->GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaSeconds, 2.25f);
		}
		else
		{
			CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaSeconds, 30.f);
		}

		if (bIsAiming)
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaSeconds, 10.f);
		}
		else
		{
			CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaSeconds, 20.f);
		}

		if (GetCurrentEquipment() && bFireButtonPressed)
		{
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, .75f, DeltaSeconds, 20.f);
		}
		else
		{
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaSeconds, 10.f);
		}

		float CrosshairSpread = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
	}
}

void UCombatComponent::InterpFOV(float DeltaSeconds)
{
	if (GetCurrentShotEquipment() == nullptr) return;
	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, GetCurrentShotEquipment()->GetAimingFOVFactor() * DefaultFOV, DeltaSeconds, GetCurrentShotEquipment()->GetAimSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaSeconds, GetCurrentShotEquipment()->GetAimSpeed());
	}
	if (Character && Character->GetCamera())
	{
		Character->GetCamera()->SetFieldOfView(CurrentFOV);
	}
}

AEquipment* UCombatComponent::GetCurrentEquipment()
{
	return GetEquipmentByType(CurrentEquipmentType);
}

AWeapon* UCombatComponent::GetCurrentShotEquipment()
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

AMelee* UCombatComponent::GetCurrentMeleeEquipment()
{
	switch (CurrentEquipmentType)
	{
	case EEquipmentType::Melee:
		return MeleeEquipment;
	default:
		return nullptr;
	}
}

AThrowing* UCombatComponent::GetCurrentThrowingEquipment()
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
	if (Character && !Character->IsLocallyControlled())
	{
		LocalEquipEquipment(Equipment);
	}
}

void UCombatComponent::LocalEquipEquipment(AEquipment* Equipment)
{
	if (Equipment == nullptr || Character == nullptr) return;
	Equipment->SetOwner(Character);
	Equipment->SetEquipmentState(EEquipmentState::Equipped);
	switch (Equipment->GetEquipmentType())
	{
	case EEquipmentType::Primary:
		if (AWeapon* TemEquipment = Cast<AWeapon>(Equipment)) PrimaryEquipment = TemEquipment;
		break;
	case EEquipmentType::Secondary:
		if (AWeapon* TemEquipment = Cast<AWeapon>(Equipment)) SecondaryEquipment = TemEquipment;
		break;
	case EEquipmentType::Melee:
		if (AMelee* TemEquipment = Cast<AMelee>(Equipment)) MeleeEquipment = TemEquipment;
		break;
	case EEquipmentType::Throwing:
		if (AThrowing* TemEquipment = Cast<AThrowing>(Equipment)) ThrowingEquipment = TemEquipment;
		break;
	}
	AttachEquipmentToBodySocket(Equipment);
}

void UCombatComponent::AttachEquipmentToBodySocket(AEquipment* Equipment)
{
	if (Equipment == nullptr || Character == nullptr || Character->GetMesh() == nullptr) return;
	FName BodySocketName;
	switch (Equipment->GetEquipmentType())
	{
	case EEquipmentType::Primary:
		BodySocketName = FName("RightShoulderSocket");
		break;
	case EEquipmentType::Secondary:
		BodySocketName = FName("RightCrotchSocket");
		break;
	case EEquipmentType::Melee:
		BodySocketName = FName("LeftShoulderSocket");
		break;
	case EEquipmentType::Throwing:
		BodySocketName = FName("LeftCrotchSocket");
		break;
	}
	const USkeletalMeshSocket* BodyScoket = Character->GetMesh()->GetSocketByName(BodySocketName);
	if (BodyScoket)
	{
		BodyScoket->AttachActor(Equipment, Character->GetMesh());

		if (EquipSound) UGameplayStatics::PlaySoundAtLocation(this, EquipSound, Character->GetActorLocation());
	}
}

void UCombatComponent::SwapEquipment(EEquipmentType EquipmentType)
{
	if (CombatState == ECombatState::Ready)
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
	if (Character && !Character->IsLocallyControlled())
	{
		LocalSwapEquipment(EquipmentType);
	}
}

void UCombatComponent::LocalSwapEquipment(EEquipmentType EquipmentType)
{
	if (Character == nullptr) return;
	AEquipment* Equipment = GetEquipmentByType(EquipmentType);
	if (Equipment)
	{
		PlaySwapMontage(Equipment);
		CombatState = ECombatState::Swapping;
	}
}

void UCombatComponent::PlaySwapMontage(AEquipment* Equipment)
{
	if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
	UAnimMontage* SwapMontage = Equipment->SwapMontage;
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

// TODO 逻辑修改 创建变量记录切枪状态是否中断 不依赖MontageSectionName 且不需要判断CombatState == ECombatState::Ready
void UCombatComponent::FinishSwapAttach(EEquipmentType EquipmentType)
{
	if (Character == nullptr) return;
	AEquipment* CurrentEquipment = GetCurrentEquipment();
	if (CurrentEquipment) AttachEquipmentToBodySocket(CurrentEquipment); // If DropEquipment, OldEquipment is nullptr

	LastEquipmentType = CurrentEquipmentType;
	switch (EquipmentType)
	{
	case EEquipmentType::Primary:
		UseEquipment(PrimaryEquipment);
		break;
	case EEquipmentType::Secondary:
		UseEquipment(SecondaryEquipment);
		break;
	case EEquipmentType::Melee:
		UseEquipment(MeleeEquipment);
		break;
	case EEquipmentType::Throwing:
		UseEquipment(ThrowingEquipment);
		break;
	}
}

void UCombatComponent::FinishSwap()
{
	if (Character)
	{
		CombatState = ECombatState::Ready;
	}
}

void UCombatComponent::UseEquipment(AEquipment* Equipment)
{
	if (Equipment == nullptr) return;
	AttachEquipmentToRightHand(Equipment);
	CurrentEquipmentType = Equipment->GetEquipmentType();

	// 更新子弹
	if (Character && Character->IsLocallyControlled() && GetCurrentEquipment())
	{
		if (GetCurrentShotEquipment())
		{
			AWeapon* TemEquipment = Cast<AWeapon>(Equipment);
			if (TemEquipment)
			{
				GetCurrentShotEquipment()->SetAmmo(TemEquipment->GetAmmo());
				GetCurrentShotEquipment()->SetCarriedAmmo(TemEquipment->GetCarriedAmmo());
			}
		}
		else
		{
			if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
			if (Controller)
			{
				Controller->SetHUDAmmo(0);
				Controller->SetHUDCarriedAmmo(0);
			}
		}
	}
}

void UCombatComponent::AttachEquipmentToRightHand(AEquipment* Equipment)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || Equipment == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(Equipment, Character->GetMesh());
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
	if (Character && !Character->IsLocallyControlled())
	{
		LocalSetAiming(bNewAimingState);
	}
}

void UCombatComponent::LocalSetAiming(bool bNewAimingState)
{
	bIsAiming = bNewAimingState;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::FireHandle(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		CrosshairShootingFactor = .75f;
		LocalFire(HitTarget);
		ServerFire(HitTarget);
		if (GetCurrentShotEquipment() && Character)
		{
			Character->GetWorldTimerManager().SetTimer(
				FireTimer,
				this,
				&ThisClass::LoadNewBulletFinished,
				GetCurrentShotEquipment()->GetFireDelay()
			);
		}
	}
	else if (GetCurrentShotEquipment() && GetCurrentShotEquipment()->GetAmmo() <= 0 && ClickSound && Character)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ClickSound, Character->GetActorLocation());
	}
}

bool UCombatComponent::CanFire()
{
	if (GetCurrentShotEquipment() == nullptr || GetCurrentShotEquipment()->IsEmpty() || !bCanFire) return false;

	if (CombatState == ECombatState::Reloading && GetCurrentShotEquipment()->GetEquipmentCate() == EEquipmentCate::Shotgun) return true;

	return CombatState == ECombatState::Ready;
}

void UCombatComponent::LoadNewBulletFinished()
{
	bCanFire = true;
	if (bFireButtonPressed && GetCurrentShotEquipment() && GetCurrentShotEquipment()->IsAutomatic())
	{
		Fire();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalFire(TraceHitTarget);
	}
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character == nullptr || GetCurrentShotEquipment() == nullptr) return;
	if (CombatState == ECombatState::Reloading && GetCurrentShotEquipment()->GetEquipmentCate() == EEquipmentCate::Shotgun)
	{
		PlayFireMontage();
		GetCurrentShotEquipment()->Fire(TraceHitTarget);
		CombatState = ECombatState::Ready;
		return;
	}

	if (CombatState == ECombatState::Ready)
	{
		PlayFireMontage();
		GetCurrentShotEquipment()->Fire(TraceHitTarget);
	}
}

void UCombatComponent::PlayFireMontage()
{
	if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && GetCurrentShotEquipment())
	{
		UAnimMontage* FireMontage = GetCurrentShotEquipment()->FireMontage;
		if (FireMontage)
		{
			AnimInstance->Montage_Play(FireMontage);
		}
	}
}

void UCombatComponent::Reload()
{
	if (GetCurrentShotEquipment() && GetCurrentShotEquipment()->GetCarriedAmmo() > 0 && !GetCurrentShotEquipment()->IsFull() && CombatState == ECombatState::Ready)
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
	if (Character && !Character->IsLocallyControlled())
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
	if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && GetCurrentShotEquipment())
	{
		UAnimMontage* ReloadMontage = GetCurrentShotEquipment()->ReloadMontage;
		if (ReloadMontage)
		{
			AnimInstance->Montage_Play(ReloadMontage);
		}
	}
}

void UCombatComponent::FinishReload()
{
	if (Character == nullptr || GetCurrentShotEquipment() == nullptr) return;
	CombatState = ECombatState::Ready;

	int32 Ammo = GetCurrentShotEquipment()->GetAmmo();
	int32 MagCapacity = GetCurrentShotEquipment()->GetMagCapacity();
	int32 CarriedAmmo = GetCurrentShotEquipment()->GetCarriedAmmo();
	int32 NeedAmmoNum = MagCapacity - Ammo;

	if (CarriedAmmo > NeedAmmoNum)
	{
		GetCurrentShotEquipment()->SetAmmo(MagCapacity);
		GetCurrentShotEquipment()->SetCarriedAmmo(CarriedAmmo - NeedAmmoNum);
	}
	else
	{
		GetCurrentShotEquipment()->SetAmmo(Ammo + CarriedAmmo);
		GetCurrentShotEquipment()->SetCarriedAmmo(0);
	}
}

void UCombatComponent::ShellReload()
{
	if (Character == nullptr || GetCurrentShotEquipment() == nullptr) return;
	GetCurrentShotEquipment()->SetAmmo(GetCurrentShotEquipment()->GetAmmo() + 1);
	GetCurrentShotEquipment()->SetCarriedAmmo(GetCurrentShotEquipment()->GetCarriedAmmo() - 1);
	// 装入一发可立即开火
	bCanFire = true;
	if (GetCurrentShotEquipment()->IsFull() || GetCurrentShotEquipment()->GetCarriedAmmo() == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && GetCurrentShotEquipment() && GetCurrentShotEquipment()->ReloadMontage)
	{
		AnimInstance->Montage_JumpToSection(FName("ReloadEnd"));
	}
}

void UCombatComponent::DropEquipment()
{
	LocalDropEquipment();
	ServerDropEquipment();
}

void UCombatComponent::ServerDropEquipment_Implementation()
{
	MulticastDropEquipment();
}

void UCombatComponent::MulticastDropEquipment_Implementation()
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalDropEquipment();
	}
}

void UCombatComponent::MulticastDropEquipment2_Implementation()
{
	LocalDropEquipment();
}

void UCombatComponent::LocalDropEquipment()
{
	if (GetCurrentEquipment() == nullptr) return;
	GetCurrentEquipment()->DropEquipment();
	switch (GetCurrentEquipment()->GetEquipmentType())
	{
	case EEquipmentType::Primary:
		PrimaryEquipment = nullptr;
		break;
	case EEquipmentType::Secondary:
		SecondaryEquipment = nullptr;
		break;
	}
}

void UCombatComponent::MulticastDestroyEquipment_Implementation()
{
	if (SecondaryEquipment) SecondaryEquipment->Destroy();
	if (MeleeEquipment) MeleeEquipment->Destroy();
	if (ThrowingEquipment) ThrowingEquipment->ManualDestroy();
}

void UCombatComponent::MeleeAttack(int32 Type)
{
	LocalMeleeAttack(Type);
	ServerMeleeAttack(Type);
}

void UCombatComponent::ServerMeleeAttack_Implementation(int32 Type)
{
	MulticastMeleeAttack(Type);
}

void UCombatComponent::MulticastMeleeAttack_Implementation(int32 Type)
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalMeleeAttack(Type);
	}
}

void UCombatComponent::LocalMeleeAttack(int32 Type)
{
	if (CombatState == ECombatState::Ready)
	{
		if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance == nullptr && GetCurrentMeleeEquipment())
		{
			UAnimMontage* AttackMontage = GetCurrentMeleeEquipment()->AttackMontage;
			if (AttackMontage)
			{
				AnimInstance->Montage_Play(AttackMontage);
				if (Type == 0)
				{
					AnimInstance->Montage_JumpToSection(FName("LightAttack"));
					CombatState = ECombatState::LightAttacking;
				}
				else
				{
					AnimInstance->Montage_JumpToSection(FName("HeavyAttack"));
					CombatState = ECombatState::HeavyAttacking;
				}
			}
		}
	}
}

void UCombatComponent::EnableMeshCollision(bool bIsEnabled)
{
	if (MeleeEquipment)
	{
		MeleeEquipment->EnableMeshCollision(bIsEnabled);
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
	if (Character && !Character->IsLocallyControlled())
	{
		LocalThrow();
	}
}

void UCombatComponent::LocalThrow()
{
	if (CombatState == ECombatState::Ready)
	{
		if (AnimInstance == nullptr) AnimInstance = Character->GetMesh()->GetAnimInstance();
		if (AnimInstance && GetCurrentThrowingEquipment())
		{
			UAnimMontage* ThrowMontage = GetCurrentThrowingEquipment()->ThrowMontage;
			if (ThrowMontage)
			{
				AnimInstance->Montage_Play(ThrowMontage);
				CombatState = ECombatState::Throwing;
			}
		}
	}
}

void UCombatComponent::ThrowOut()
{
	if (ThrowingEquipment && ThrowingEquipment->GetEquipmentMesh() && Character)
	{
		ThrowingEquipment->GetEquipmentMesh()->SetSimulatePhysics(true);
		ThrowingEquipment->GetEquipmentMesh()->SetEnableGravity(true);
		ThrowingEquipment->GetEquipmentMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		ThrowingEquipment->GetEquipmentMesh()->DetachFromComponent(DetachRules);

		UCameraComponent* CameraComponent = Character->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			ThrowingEquipment->GetEquipmentMesh()->AddImpulse(CameraComponent->GetForwardVector() * 1000.f * ThrowingEquipment->GetEquipmentMesh()->GetMass());
		}

		ThrowingEquipment->ThrowOut();

		CombatState = ECombatState::Ready;
	}
}
