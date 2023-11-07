#include "CombatComponent.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);

	DOREPLIFETIME(UCombatComponent, PrimaryWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, MeleeWeapon);
	DOREPLIFETIME(UCombatComponent, ThrowingWeapon);
	DOREPLIFETIME(UCombatComponent, CurrentWeaponType);
	DOREPLIFETIME(UCombatComponent, LastWeaponType);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	ClickSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetCamera())
		{
			DefaultFOV = Character->GetCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHUDCrosshair(DeltaTime);
		InterpFOV(DeltaTime);
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

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
	if (Controller)
	{
		if (HUD == nullptr) HUD = Cast<AHumanHUD>(Controller->GetHUD());
		if (HUD)
		{
			// Calculate crosshair spread
			// [0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 10.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 20.f);
			}

			if (GetCurrentWeapon() && bFireButtonPressed)
			{
				CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, .75f, DeltaTime, 20.f);
			}
			else
			{
				CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 10.f);
			}

			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (GetCurrentWeapon() == nullptr) return;
	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, GetCurrentWeapon()->GetZoomedFOV(), DeltaTime, GetCurrentWeapon()->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetCamera())
	{
		Character->GetCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	LocalEquipWeapon(WeaponToEquip);
	ServerEquipWeapon(WeaponToEquip);
}

void UCombatComponent::ServerEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	MulticastEquipWeapon(WeaponToEquip);
}

void UCombatComponent::MulticastEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalEquipWeapon(WeaponToEquip);
	}
}

void UCombatComponent::LocalEquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || Character == nullptr) return;
	WeaponToEquip->SetOwner(Character);
	WeaponToEquip->SetWeaponState(EWeaponState::Equipped);
	EWeaponType WeaponType = WeaponToEquip->GetWeaponType();
	switch (WeaponType)
	{
	case EWeaponType::Primary:
		PrimaryWeapon = WeaponToEquip;
		break;
	case EWeaponType::Secondary:
		SecondaryWeapon = WeaponToEquip;
		break;
	case EWeaponType::Melee:
		MeleeWeapon = WeaponToEquip;
		break;
	case EWeaponType::Throwing:
		ThrowingWeapon = WeaponToEquip;
		break;
	}
	AttachActorToBodySocket(WeaponToEquip);
}

void UCombatComponent::SwapWeapon(EWeaponType NewWeaponType)
{
	if (CombatState == ECombatState::Ready)
	{
		LocalSwapWeapon(NewWeaponType);
		ServerSwapWeapon(NewWeaponType);
	}
}

void UCombatComponent::ServerSwapWeapon_Implementation(EWeaponType NewWeaponType)
{
	MulticastSwapWeapon(NewWeaponType);
}

void UCombatComponent::MulticastSwapWeapon_Implementation(EWeaponType NewWeaponType)
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalSwapWeapon(NewWeaponType);
	}
}

void UCombatComponent::LocalSwapWeapon(EWeaponType NewWeaponType)
{
	if (Character == nullptr) return;
	AWeapon* NewWeapon = GetWeaponByType(NewWeaponType);
	if (NewWeapon)
	{
		Character->PlaySwapMontage(NewWeapon->GetWeaponName());
		CombatState = ECombatState::Swapping;
	}
}

void UCombatComponent::FinishSwapAttach(EWeaponType NewWeaponType)
{
	if (Character == nullptr) return;
	AWeapon* OldWeapon = GetCurrentWeapon();
	if (OldWeapon) // if DropWeapon, OldWeapon is nullptr
	{
		AttachActorToBodySocket(OldWeapon);
		LastWeaponType = OldWeapon->GetWeaponType();
	}
	switch (NewWeaponType)
	{
	case EWeaponType::Primary:
		UseWeapon(PrimaryWeapon);
		break;
	case EWeaponType::Secondary:
		UseWeapon(SecondaryWeapon);
		break;
	case EWeaponType::Melee:
		UseWeapon(MeleeWeapon);
		break;
	case EWeaponType::Throwing:
		UseWeapon(ThrowingWeapon);
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

void UCombatComponent::UseWeapon(AWeapon* WeaponToUse)
{
	if (Character == nullptr || WeaponToUse == nullptr) return;
	AttachActorToRightHand(WeaponToUse);
	CurrentWeaponType = WeaponToUse->GetWeaponType();
	GetCurrentWeapon()->SetAmmo(WeaponToUse->GetAmmo());
	GetCurrentWeapon()->SetCarriedAmmo(WeaponToUse->GetCarriedAmmo());
	PlayUseWeaponSound();
}

void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::AttachActorToBodySocket(AWeapon* ActorToAttach)
{
	if (ActorToAttach == nullptr || Character == nullptr || Character->GetMesh() == nullptr) return;
	FName BodySocketName = Character->GetMetaData(ActorToAttach->GetWeaponType(), "BodySocketName");
	const USkeletalMeshSocket* BodyScoket = Character->GetMesh()->GetSocketByName(BodySocketName);
	if (BodyScoket)
	{
		BodyScoket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::PlayUseWeaponSound()
{
	if (Character && GetCurrentWeapon() && GetCurrentWeapon()->UseWeaponSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GetCurrentWeapon()->UseWeaponSound, Character->GetActorLocation());
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAimButtonPressed = bIsAiming;
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
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
		if (GetCurrentWeapon() && Character)
		{
			Character->GetWorldTimerManager().SetTimer(
				FireTimer,
				this,
				&UCombatComponent::LoadNewBulletFinished,
				GetCurrentWeapon()->FireDelay
			);
		}
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
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	if (CombatState == ECombatState::Reloading && GetCurrentWeapon()->GetWeaponCate() == EWeaponCate::Shotgun)
	{
		Character->PlayFireMontage(bAiming);
		GetCurrentWeapon()->Fire(TraceHitTarget);
		CombatState = ECombatState::Ready;
		return;
	}

	if (CombatState == ECombatState::Ready)
	{
		Character->PlayFireMontage(bAiming);
		GetCurrentWeapon()->Fire(TraceHitTarget);
	}
}

void UCombatComponent::LoadNewBulletFinished()
{
	bCanFire = true;
	if (bFireButtonPressed && GetCurrentWeapon() && GetCurrentWeapon()->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (GetCurrentWeapon() == nullptr) return false;
	if (Character && GetCurrentWeapon()->IsEmpty())
	{
		if (ClickSound) UGameplayStatics::PlaySoundAtLocation(this, ClickSound, Character->GetActorLocation());
	}
	bool bShotgunReloadingWithBulletIn = !GetCurrentWeapon()->IsEmpty() &&
		bCanFire &&
		CombatState == ECombatState::Reloading &&
		GetCurrentWeapon()->GetWeaponCate() == EWeaponCate::Shotgun;
	if (bShotgunReloadingWithBulletIn) return true;
	if (!GetCurrentWeapon()->IsEmpty() && bCanFire && CombatState == ECombatState::Ready) return true;
	return false;
}

void UCombatComponent::Reload()
{
	if (GetCurrentWeapon() && GetCurrentWeapon()->GetCarriedAmmo() > 0 &&
		CombatState == ECombatState::Ready && !GetCurrentWeapon()->IsFull())
	{
		CombatState = ECombatState::Reloading;
		Character->PlayReloadMontage();
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	MulticastReload();
}

void UCombatComponent::MulticastReload_Implementation()
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	if (!Character->IsLocallyControlled())
	{
		CombatState = ECombatState::Reloading;
		Character->PlayReloadMontage();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	CombatState = ECombatState::Ready;
	// 设置ammo和CarriedAmmo
	int32 Ammo = GetCurrentWeapon()->GetAmmo();
	int32 MagCapacity = GetCurrentWeapon()->GetMagCapacity();
	int32 CarriedAmmo = GetCurrentWeapon()->GetCarriedAmmo();
	int32 NeedAmmoNum = MagCapacity - Ammo;
	if (CarriedAmmo > NeedAmmoNum)
	{
		GetCurrentWeapon()->SetAmmo(MagCapacity);
		GetCurrentWeapon()->SetCarriedAmmo(CarriedAmmo - NeedAmmoNum);
	}
	else
	{
		GetCurrentWeapon()->SetAmmo(Ammo + CarriedAmmo);
		GetCurrentWeapon()->SetCarriedAmmo(0);
	}
}

void UCombatComponent::ShellReload()
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	GetCurrentWeapon()->SetAmmo(GetCurrentWeapon()->GetAmmo() + 1);
	GetCurrentWeapon()->SetCarriedAmmo(GetCurrentWeapon()->GetCarriedAmmo() - 1);
	// 装入一发可立即开火
	bCanFire = true;
	if (GetCurrentWeapon()->IsFull() || GetCurrentWeapon()->GetCarriedAmmo() == 0)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("M870ReloadingEnd"));
	}
}

void UCombatComponent::DropWeapon()
{
	LocalDropWeapon();
	ServerDropWeapon();
}

void UCombatComponent::ServerDropWeapon_Implementation()
{
	MulticastDropWeapon();
}

void UCombatComponent::MulticastDropWeapon_Implementation()
{
	if (Character && !Character->IsLocallyControlled())
	{
		LocalDropWeapon();
	}
}

void UCombatComponent::LocalDropWeapon()
{
	LastWeaponType = CurrentWeaponType;
	GetCurrentWeapon()->DropWeapon();
	EWeaponType WeaponType = GetCurrentWeapon()->GetWeaponType();
	switch (WeaponType)
	{
	case EWeaponType::Primary:
		PrimaryWeapon = nullptr;
		break;
	case EWeaponType::Secondary:
		SecondaryWeapon = nullptr;
		break;
	case EWeaponType::Throwing:
		ThrowingWeapon = nullptr;
		break;
	}
}

AWeapon* UCombatComponent::GetCurrentWeapon()
{
	return GetWeaponByType(CurrentWeaponType);
}

AWeapon* UCombatComponent::GetWeaponByType(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::Primary:
		return PrimaryWeapon;
	case EWeaponType::Secondary:
		return SecondaryWeapon;
	case EWeaponType::Melee:
		return MeleeWeapon;
	case EWeaponType::Throwing:
		return ThrowingWeapon;
	default:
		return nullptr;
	}
}

bool UCombatComponent::HasEquippedThisTypeWeapon(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::Primary:
		return PrimaryWeapon == nullptr ? false : true;
	case EWeaponType::Secondary:
		return SecondaryWeapon == nullptr ? false : true;
	case EWeaponType::Melee:
		return MeleeWeapon == nullptr ? false : true;
	case EWeaponType::Throwing:
		return ThrowingWeapon == nullptr ? false : true;
	default:
		return false;
	}
}
