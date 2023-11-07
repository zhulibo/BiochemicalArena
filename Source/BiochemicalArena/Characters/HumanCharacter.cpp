#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "CharacterComponents/CombatComponent.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "CharacterComponents/PickupComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"

AHumanCharacter::AHumanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "HeadSocket");
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// TODO 设置头部对自己不可见
	// GetMesh()->SetOwnerNoSee(true);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	Pickup = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
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
		if (Combat->CombatState == ECombatState::MAX) Combat->CombatState = ECombatState::Ready; // 初始化ECombatState
		if (DefaultPrimaryWeaponClass)
		{
			AWeapon* DefaultPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultPrimaryWeaponClass);
			Combat->EquipWeapon(DefaultPrimaryWeapon);
			Combat->SwapWeapon(EWeaponType::Primary);
			Combat->CurrentWeaponType = EWeaponType::Primary;
		}
		if (DefaultSecondaryWeaponClass)
		{
			AWeapon* DefaultSecondaryWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultSecondaryWeaponClass);
			Combat->EquipWeapon(DefaultSecondaryWeapon);
			Combat->LastWeaponType = EWeaponType::Secondary;
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
			Subsystem->AddMappingContext(WeaponMappingContext, 0);
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
		EnhancedInputComponent->BindAction(SwapPrimaryWeaponAction, ETriggerEvent::Triggered, this, &AHumanCharacter::SwapPrimaryWeaponButtonPressed);
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

	if (Combat && Combat->Character == nullptr)
	{
		Combat->Character = this;
	}
}

void AHumanCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	float DamageCoefficient = CalcFallDamageCoefficient();
	if (DamageCoefficient == 0.f) return;

	Health = FMath::Clamp(Health - DamageCoefficient * MaxHealth, 0.f, MaxHealth);

	if (IsLocallyControlled())
	{
		UpdateHUDHealth();
		PlayOuchWeaponSound(DamageCoefficient);
	}
	if (HasAuthority())
	{
		MulticastPlayOuchWeaponSound(DamageCoefficient);
		if (Health == 0.f) Elim();
	}
}

void AHumanCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimButtonPressed"));
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
		Combat->EquipWeapon(Weapon);
	}
}

void AHumanCharacter::DropButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Melee)
	{
		Combat->DropWeapon();
		Combat->SwapWeapon(EWeaponType::Melee); // 刀不可丢弃
		DetectOverlappingWeapon(); // 检测是否有武器重叠
	}
}

void AHumanCharacter::DetectOverlappingWeapon()
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

void AHumanCharacter::SwapPrimaryWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->PrimaryWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Primary)
	{
		Combat->SwapWeapon(EWeaponType::Primary);
	}
}

void AHumanCharacter::SwapSecondaryWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->SecondaryWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Secondary)
	{
		Combat->SwapWeapon(EWeaponType::Secondary);
	}
	Combat->SwapWeapon(EWeaponType::Secondary);
}

void AHumanCharacter::SwapMeleeWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->MeleeWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Melee)
	{
		Combat->SwapWeapon(EWeaponType::Melee);
	}
}

void AHumanCharacter::SwapThrowingWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->ThrowingWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Throwing)
	{
		Combat->SwapWeapon(EWeaponType::Throwing);
	}
}

void AHumanCharacter::SwapLastWeaponButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		EWeaponType WeaponTypeToSwap = Combat->LastWeaponType;
		switch (WeaponTypeToSwap)
		{
		case EWeaponType::Primary:
			if (Combat->PrimaryWeapon) Combat->SwapWeapon(EWeaponType::Primary);
			break;
		case EWeaponType::Secondary:
			if (Combat->SecondaryWeapon) Combat->SwapWeapon(EWeaponType::Secondary);
			break;
		case EWeaponType::Melee:
			if (Combat->MeleeWeapon) Combat->SwapWeapon(EWeaponType::Melee);
			break;
		case EWeaponType::Throwing:
			if (Combat->ThrowingWeapon) Combat->SwapWeapon(EWeaponType::Throwing);
			break;
		}
	}
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (Combat && Combat->GetCurrentWeapon() && AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName MontageSectionName = GetMetaData(Combat->GetCurrentWeapon()->GetWeaponName(), "MontageSectionName");
		AnimInstance->Montage_JumpToSection(MontageSectionName);
	}
}

void AHumanCharacter::PlaySwapMontage(EWeaponName NewWeaponName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (Combat && AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
		FName MontageSectionName = GetMetaData(NewWeaponName, "MontageSectionName");
		AnimInstance->Montage_JumpToSection(MontageSectionName);
	}
}

void AHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (bElimmed) return;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();

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
}

void AHumanCharacter::UpdateHUDHealth()
{
	if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
	if (HumanController)
	{
		HumanController->SetHUDHealth(Health, MaxHealth);
	}
}

void AHumanCharacter::Elim()
{
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

bool AHumanCharacter::IsAiming()
{
	return Combat && Combat->bAiming;
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
	return Combat->CombatState;
}
