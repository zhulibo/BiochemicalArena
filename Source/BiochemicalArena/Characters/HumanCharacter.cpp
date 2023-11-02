#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CharacterComponents/CombatComponent.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/GameModes/InfectMode.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "CharacterComponents/PickupComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"

AHumanCharacter::AHumanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	CameraComponent->SetRelativeLocation(FVector(10.0f, 10.0f, BaseEyeHeight - 10.0f));
	CameraComponent->bUsePawnControlRotation = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Pickup = CreateDefaultSubobject<UPickupComponent>(TEXT("BuffComponent"));
	Pickup->SetIsReplicated(true);
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AHumanCharacter::ReceiveDamage);
	}
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
}

void AHumanCharacter::PollInit()
{
	if (HumanController == nullptr)
	{
		HumanController = Cast<AHumanController>(Controller);
		if (HumanController && HasAuthority())
		{
			SetDefaultWeapon();
		}
	}
}

void AHumanCharacter::SetDefaultWeapon()
{
	if (Combat)
	{
		if (DefaultMainWeaponClass)
		{
			AWeapon* DefaultMainWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultMainWeaponClass);
			Combat->EquipWeapon(DefaultMainWeapon);
			Combat->UseWeapon(DefaultMainWeapon);
			Combat->CurrentWeaponType = EWeaponType::EWT_MainWeapon;
		}
		if (DefaultSecondaryWeaponClass)
		{
			AWeapon* DefaultSecondaryWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultSecondaryWeaponClass);
			Combat->EquipWeapon(DefaultSecondaryWeapon);
			Combat->LastWeaponType = EWeaponType::EWT_SecondaryWeapon;
		}
		if (DefaultMeleeWeaponClass)
		{
			AWeapon* DefaultMeleeWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultMeleeWeaponClass);
			Combat->EquipWeapon(DefaultMeleeWeapon);
		}
		if (DefaultThrowingWeaponClass)
		{
			AWeapon* DefaultThrowingWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultThrowingWeaponClass);
			Combat->EquipWeapon(DefaultThrowingWeapon);
		}
	}
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(WeaponsMappingContext, 0);
		}
	}
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AHumanCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AHumanCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AHumanCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AHumanCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AHumanCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &AHumanCharacter::DropButtonPressed);
		EnhancedInputComponent->BindAction(SwapMainWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapMainWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapSecondaryWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapSecondaryWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapMeleeWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapMeleeWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapThrowingWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapThrowingWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapLastWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapLastWeaponButtonPressed);
	}
}

void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AHumanCharacter, Health);
}

void AHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void AHumanCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AHumanCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AHumanCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireHandle(true);
	}
}

void AHumanCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireHandle(false);
	}
}

void AHumanCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->Reload();
	}
}

void AHumanCharacter::EquipOverlappingWeapon(AWeapon* Weapon)
{
	if (bElimmed || Weapon->GetOwner()) return;
	if (Combat && Combat->HasEquippedThisTypeWeapon(Weapon->GetWeaponType())) return;
	if (Combat)
	{
		ServerEquipWeaponHandle(Weapon);
	}
}

void AHumanCharacter::ServerEquipWeaponHandle_Implementation(AWeapon* Weapon)
{
	if (Combat)
	{
		Combat->EquipWeapon(Weapon);
	}
}

void AHumanCharacter::DropButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentWeapon())
	{
		if (Combat->GetCurrentWeapon()->GetWeaponType() == EWeaponType::EWT_MeleeWeapon) return; // 刀不可丢弃
		if (HasAuthority())
		{
			Combat->GetCurrentWeapon()->DropWeapon();
			Combat->SetCombatWeaponPointNull(Combat->GetCurrentWeapon()->GetWeaponType());
			Combat->SwapWeapon(EWeaponType::EWT_MeleeWeapon);
		}
		else
		{
			ServerDropButtonPressed();
		}
		ServerDetectOverlappingWeapon(); // 检测是否有武器重叠
	}
}

void AHumanCharacter::ServerDropButtonPressed_Implementation()
{
	if (Combat && Combat->GetCurrentWeapon())
	{
		Combat->GetCurrentWeapon()->DropWeapon();
		Combat->SetCombatWeaponPointNull(Combat->GetCurrentWeapon()->GetWeaponType());
		Combat->SwapWeapon(EWeaponType::EWT_MeleeWeapon);
	}
}

void AHumanCharacter::ServerDetectOverlappingWeapon_Implementation()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AWeapon::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		AWeapon* Weapon = Cast<AWeapon>(OverlappingActor);
		if (Weapon)
		{
			EquipOverlappingWeapon(Weapon);
			break;
		}
	}
}

void AHumanCharacter::SwapMainWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->MainWeapon && Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::EWT_MainWeapon)
	{
		ServerSwapWeaponHandle(EWeaponType::EWT_MainWeapon);
	}
}

void AHumanCharacter::SwapSecondaryWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->SecondaryWeapon && Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::EWT_SecondaryWeapon)
	{
		ServerSwapWeaponHandle(EWeaponType::EWT_SecondaryWeapon);
	}
}

void AHumanCharacter::SwapMeleeWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->MeleeWeapon && Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::EWT_MeleeWeapon)
	{
		ServerSwapWeaponHandle(EWeaponType::EWT_MeleeWeapon);
	}
}

void AHumanCharacter::SwapThrowingWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->ThrowingWeapon && Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::EWT_ThrowingWeapon)
	{
		ServerSwapWeaponHandle(EWeaponType::EWT_ThrowingWeapon);
	}
}

void AHumanCharacter::SwapLastWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		EWeaponType WeaponTypeToSwap = Combat->LastWeaponType;
		switch (WeaponTypeToSwap)
		{
		case EWeaponType::EWT_MainWeapon:
			if (Combat->MainWeapon) ServerSwapWeaponHandle(EWeaponType::EWT_MainWeapon);
			break;
		case EWeaponType::EWT_SecondaryWeapon:
			if (Combat->SecondaryWeapon) ServerSwapWeaponHandle(EWeaponType::EWT_SecondaryWeapon);
			break;
		case EWeaponType::EWT_MeleeWeapon:
			if (Combat->MeleeWeapon) ServerSwapWeaponHandle(EWeaponType::EWT_MeleeWeapon);
			break;
		case EWeaponType::EWT_ThrowingWeapon:
			if (Combat->ThrowingWeapon) ServerSwapWeaponHandle(EWeaponType::EWT_ThrowingWeapon);
			break;
		}
	}
}

void AHumanCharacter::ServerSwapWeaponHandle_Implementation(EWeaponType NewWeaponType)
{
	Combat->SwapWeapon(NewWeaponType);
}

void AHumanCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->GetCurrentWeapon() == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
	}
}

void AHumanCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->GetCurrentWeapon() == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->GetCurrentWeapon()->GetWeaponCate())
		{
		case EWeaponCate::EWT_GeneralWeapon:
			SectionName = FName("AK47");
			break;
		case EWeaponCate::EWT_Shotgun:
			SectionName = FName("M870");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
		if (TeamDeadMatchMode)
		{
			if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
			AHumanController* AttackerController = Cast<AHumanController>(InstigatorController);
			if (HumanController && AttackerController)
			{
				TeamDeadMatchMode->PlayerEliminated(this, HumanController, AttackerController);
			}
		}
	}
}

void AHumanCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void AHumanCharacter::UpdateHUDHealth()
{
	if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
	if (HumanController)
	{
		HumanController->SetHUDHealth(Health, MaxHealth);
	}
}

void AHumanCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->GetCurrentWeapon() == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AHumanCharacter::Elim()
{
	bElimmed = true; // set immediately
	if (Combat && Combat->GetCurrentWeapon())
	{
		Combat->GetCurrentWeapon()->DropWeapon();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AHumanCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void AHumanCharacter::MulticastElim_Implementation()
{
	bElimmed = true;
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (HumanController)
	{
		DisableInput(HumanController);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHumanCharacter::ElimTimerFinished()
{
	if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
	if (TeamDeadMatchMode)
	{
		TeamDeadMatchMode->RequestRespawn(this, Controller);
	}
}

bool AHumanCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->GetCurrentWeapon());
}

bool AHumanCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AHumanCharacter::GetCurrentWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->GetCurrentWeapon();
}

FVector AHumanCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AHumanCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_Max;
	return Combat->CombatState;
}
