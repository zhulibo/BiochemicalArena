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

	DOREPLIFETIME(UCombatComponent, MainWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, MeleeWeapon);
	DOREPLIFETIME(UCombatComponent, ThrowingWeapon);
	DOREPLIFETIME(UCombatComponent, CurrentWeaponType);
	DOREPLIFETIME(UCombatComponent, LastWeaponType);
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
			if (GetCurrentWeapon())
			{
				HUDPackage.CrosshairCenter = GetCurrentWeapon()->CrosshairCenter;
				HUDPackage.CrosshairLeft = GetCurrentWeapon()->CrosshairLeft;
				HUDPackage.CrosshairRight = GetCurrentWeapon()->CrosshairRight;
				HUDPackage.CrosshairBottom = GetCurrentWeapon()->CrosshairBottom;
				HUDPackage.CrosshairTop = GetCurrentWeapon()->CrosshairTop;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairTop = nullptr;
			}

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
	MulticastEquipWeapon(WeaponToEquip);
}

void UCombatComponent::MulticastEquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (Character && !Character->HasAuthority())
	{
		LocalEquipWeapon(WeaponToEquip);
	}
}

void UCombatComponent::LocalEquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || Character == nullptr) return;
	WeaponToEquip->SetOwner(Character);
	WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);
	EWeaponType WeaponType = WeaponToEquip->GetWeaponType();
	switch (WeaponType)
	{
	case EWeaponType::EWT_MainWeapon:
		MainWeapon = WeaponToEquip;
		break;
	case EWeaponType::EWT_SecondaryWeapon:
		SecondaryWeapon = WeaponToEquip;
		break;
	case EWeaponType::EWT_MeleeWeapon:
		MeleeWeapon = WeaponToEquip;
		break;
	case EWeaponType::EWT_ThrowingWeapon:
		ThrowingWeapon = WeaponToEquip;
		break;
	}
	AttachActorToBodySocket(WeaponToEquip);
}

void UCombatComponent::UseWeapon(AWeapon* WeaponToUse)
{
	LocalUseWeapon(WeaponToUse);
	MulticastUseWeapon(WeaponToUse);
}

void UCombatComponent::MulticastUseWeapon_Implementation(AWeapon* WeaponToUse)
{
	if (Character && !Character->HasAuthority())
	{
		LocalUseWeapon(WeaponToUse);
	}
}

void UCombatComponent::LocalUseWeapon(AWeapon* WeaponToUse)
{
	if (Character == nullptr || WeaponToUse == nullptr || GetCurrentWeapon() == nullptr) return;
	AttachActorToRightHand(WeaponToUse);
	CurrentWeaponType = WeaponToUse->GetWeaponType();
	GetCurrentWeapon()->SetAmmo(WeaponToUse->GetAmmo());
	GetCurrentWeapon()->SetCarriedAmmo(WeaponToUse->GetCarriedAmmo());
	PlayEquipWeaponSound();
}

void UCombatComponent::SwapWeapon(EWeaponType NewWeaponType)
{
	AWeapon* OldWeapon = GetCurrentWeapon();
	EWeaponType OldWeaponType= CurrentWeaponType;
	switch (NewWeaponType)
	{
	case EWeaponType::EWT_MainWeapon:
		if (MainWeapon == nullptr) return;
		UseWeapon(MainWeapon);
		break;
	case EWeaponType::EWT_SecondaryWeapon:
		if (SecondaryWeapon == nullptr) return;
		UseWeapon(SecondaryWeapon);
		break;
	case EWeaponType::EWT_MeleeWeapon:
		if (MeleeWeapon == nullptr) return;
		UseWeapon(MeleeWeapon);
		break;
	case EWeaponType::EWT_ThrowingWeapon:
		if (ThrowingWeapon == nullptr) return;
		UseWeapon(ThrowingWeapon);
		break;
	}
	if (OldWeapon) AttachActorToBodySocket(OldWeapon);
	LastWeaponType = OldWeaponType;
	CurrentWeaponType = NewWeaponType;
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
	FName BodySocketName;
	switch (ActorToAttach->GetWeaponType())
	{
	case EWeaponType::EWT_MainWeapon:
		BodySocketName = FName("RightShoulderSocket");
		break;
	case EWeaponType::EWT_SecondaryWeapon:
		BodySocketName = FName("RightCrotchSocket");
		break;
	case EWeaponType::EWT_MeleeWeapon:
		BodySocketName = FName("LeftShoulderSocket");
		break;
	case EWeaponType::EWT_ThrowingWeapon:
		BodySocketName = FName("LeftCrotchSocket");
		break;
	}
	const USkeletalMeshSocket* BodyScoket = Character->GetMesh()->GetSocketByName(BodySocketName);
	if (BodyScoket)
	{
		BodyScoket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

void UCombatComponent::PlayEquipWeaponSound()
{
	if (Character && Character->IsLocallyControlled() && GetCurrentWeapon() && GetCurrentWeapon()->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			GetCurrentWeapon()->EquipSound,
			Character->GetActorLocation()
		);
	}
}

AWeapon* UCombatComponent::GetCurrentWeapon()
{
	switch (CurrentWeaponType)
	{
	case EWeaponType::EWT_MainWeapon:
		return MainWeapon;
	case EWeaponType::EWT_SecondaryWeapon:
		return SecondaryWeapon;
	case EWeaponType::EWT_MeleeWeapon:
		return MeleeWeapon;
	case EWeaponType::EWT_ThrowingWeapon:
		return ThrowingWeapon;
	default:
		return nullptr;
	}
}

bool UCombatComponent::HasEquippedThisTypeWeapon(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::EWT_MainWeapon:
		return MainWeapon == nullptr ? false : true;
	case EWeaponType::EWT_SecondaryWeapon:
		return SecondaryWeapon == nullptr ? false : true;
	case EWeaponType::EWT_MeleeWeapon:
		return MeleeWeapon == nullptr ? false : true;
	case EWeaponType::EWT_ThrowingWeapon:
		return ThrowingWeapon == nullptr ? false : true;
	default:
		return false;
	}
}

void UCombatComponent::SetCombatWeaponPointNull(EWeaponType WeaponType)
{
	switch (WeaponType)
	{
	case EWeaponType::EWT_MainWeapon:
		MainWeapon = nullptr;
		break;
	case EWeaponType::EWT_SecondaryWeapon:
		SecondaryWeapon = nullptr;
		break;
	case EWeaponType::EWT_ThrowingWeapon:
		ThrowingWeapon = nullptr;
		break;
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
	if (Character && Character->IsLocallyControlled()) return;
	LocalFire(TraceHitTarget);
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	if (CombatState == ECombatState::ECS_Reloading && GetCurrentWeapon()->GetWeaponCate() == EWeaponCate::EWT_Shotgun)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		GetCurrentWeapon()->Fire(TraceHitTarget);
		CombatState = ECombatState::ECS_Unoccupied;
		return;
	}

	if (CombatState == ECombatState::ECS_Unoccupied)
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
	bool bShotgunReloadingWithBulletIn = !GetCurrentWeapon()->IsEmpty() &&
		bCanFire &&
		CombatState == ECombatState::ECS_Reloading &&
		GetCurrentWeapon()->GetWeaponCate() == EWeaponCate::EWT_Shotgun;
	if (bShotgunReloadingWithBulletIn) return true;
	if (bLocallyReloading) return false;
	if (!GetCurrentWeapon()->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied) return true;
	return false;
}

void UCombatComponent::Reload()
{
	if (GetCurrentWeapon() && GetCurrentWeapon()->GetCarriedAmmo() > 0 && CombatState == ECombatState::ECS_Unoccupied && !GetCurrentWeapon()->IsFull() && !bLocallyReloading)
	{
		bLocallyReloading = true;
		if (Character->IsLocallyControlled())
		{
			CombatState = ECombatState::ECS_Reloading;
			Character->PlayReloadMontage();
		}
		ServerReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	if (!Character->IsLocallyControlled())
	{
		CombatState = ECombatState::ECS_Reloading;
		Character->PlayReloadMontage();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr || GetCurrentWeapon() == nullptr) return;
	bLocallyReloading = false;
	CombatState = ECombatState::ECS_Unoccupied;
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
	if (Character->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("server: %d"), GetCurrentWeapon()->GetAmmo() + 1);
	}
	if (Character->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("Locally: %d"), GetCurrentWeapon()->GetAmmo() + 1);
	}
	if (!Character->HasAuthority() && !Character->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("client: %d"), GetCurrentWeapon()->GetAmmo() + 1);
	}
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

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled()) Character->PlayReloadMontage();
		break;
	case ECombatState::ECS_Unoccupied:
		break;
	}
}
