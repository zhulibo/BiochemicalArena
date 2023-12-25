#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/Weapons/Weapon.h"
#include "Components/PickupComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatState.h"
#include "Components/OverheadWidget.h"

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

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// TODO 设置头部对自己不可见
	// GetMesh()->SetOwnerNoSee(true);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);

	Pickup = CreateDefaultSubobject<UPickupComponent>(TEXT("Pickup"));
	Pickup->SetIsReplicated(true);
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (OverheadWidget)
	{
		UUserWidget* Tem = OverheadWidget->GetUserWidgetObject();
		if (Tem)
		{
			UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(Tem);
			if (OverheadWidgetClass && OverheadWidgetClass->HumanCharacter == nullptr)
			{
				OverheadWidgetClass->HumanCharacter = this;
			}
		}
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AHumanCharacter::ReceiveDamage);
	}
}

void AHumanCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();

	// FVector Test = CameraComponent->GetComponentLocation();
	// UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), Test.X, Test.Y, Test.Z);

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

		UClass* PrimaryWeaponClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Weapons/AK47.AK47_C'"));
		if (PrimaryWeaponClass)
		{
			AWeapon* DefaultPrimaryWeapon = GetWorld()->SpawnActor<AWeapon>(PrimaryWeaponClass);
			Combat->EquipWeapon(DefaultPrimaryWeapon);
			Combat->SwapWeapon(EWeaponType::Primary);
			Combat->CurrentWeaponType = EWeaponType::Primary;
		}

		UClass* SecondaryWeaponClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Weapons/Glock17.Glock17_C'"));
		if (SecondaryWeaponClass)
		{
			AWeapon* DefaultSecondaryWeapon = GetWorld()->SpawnActor<AWeapon>(SecondaryWeaponClass);
			Combat->EquipWeapon(DefaultSecondaryWeapon);
			Combat->LastWeaponType = EWeaponType::Secondary;
		}

		UClass* MeleeWeaponClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Weapons/Melee.Melee_C'"));
		if (MeleeWeaponClass)
		{
			AWeapon* DefaultMeleeWeapon = GetWorld()->SpawnActor<AWeapon>(MeleeWeaponClass);
			Combat->EquipWeapon(DefaultMeleeWeapon);
		}

		// UClass* ThrowingWeaponClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/Weapons/Melee.Melee_C'"));
		// if (ThrowingWeaponClass)
		// {
		// 	AWeapon* DefaultThrowingWeapon = GetWorld()->SpawnActor<AWeapon>(ThrowingWeaponClass);
		// 	Combat->EquipWeapon(DefaultThrowingWeapon);
		// }
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
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ThisClass::DropButtonPressed);
		EnhancedInputComponent->BindAction(SwapPrimaryWeaponAction, ETriggerEvent::Triggered, this, &ThisClass::SwapPrimaryWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapSecondaryWeaponAction, ETriggerEvent::Triggered, this, &ThisClass::SwapSecondaryWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapMeleeWeaponAction, ETriggerEvent::Triggered, this, &ThisClass::SwapMeleeWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapThrowingWeaponAction, ETriggerEvent::Triggered, this, &ThisClass::SwapThrowingWeaponButtonPressed);
		EnhancedInputComponent->BindAction(SwapLastWeaponAction, ETriggerEvent::Triggered, this, &ThisClass::SwapLastWeaponButtonPressed);
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
		if (Health == 0.f) Kill();
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
	if (bIsKilled || Weapon->GetOwner()) return;
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
	SwapPrimaryWeapon();
}

void AHumanCharacter::SwapSecondaryWeaponButtonPressed(const FInputActionValue& Value)
{
	SwapSecondaryWeapon();
}

void AHumanCharacter::SwapMeleeWeaponButtonPressed(const FInputActionValue& Value)
{
	SwapMeleeWeapon();
}

void AHumanCharacter::SwapThrowingWeaponButtonPressed(const FInputActionValue& Value)
{
	SwapThrowingWeapon();
}

void AHumanCharacter::SwapPrimaryWeapon()
{
	if (Combat && Combat->PrimaryWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Primary)
	{
		Combat->SwapWeapon(EWeaponType::Primary);
	}
}

void AHumanCharacter::SwapSecondaryWeapon()
{
	if (Combat && Combat->SecondaryWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Secondary)
	{
		Combat->SwapWeapon(EWeaponType::Secondary);
	}
}

void AHumanCharacter::SwapMeleeWeapon()
{
	if (Combat && Combat->MeleeWeapon &&
		Combat->GetCurrentWeapon() && Combat->GetCurrentWeapon()->GetWeaponType() != EWeaponType::Melee)
	{
		Combat->SwapWeapon(EWeaponType::Melee);
	}
}

void AHumanCharacter::SwapThrowingWeapon()
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

void AHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (bIsKilled) return;
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();

	if (Health == 0.f)
	{
		if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
		if (TeamDeadMatchMode)
		{
			if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
			AHumanController* AttackerController = Cast<AHumanController>(InstigatorController);
			TeamDeadMatchMode->KillPlayer(this, HumanController, AttackerController, DamageCauser);
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

void AHumanCharacter::Kill()
{
	if (Combat && Combat->GetCurrentWeapon())
	{
		Combat->GetCurrentWeapon()->DropWeapon();
	}
	MulticastKill();
	GetWorldTimerManager().SetTimer(
		KillTimer,
		this,
		&AHumanCharacter::KillTimerFinished,
		KillDelay
	);
}

void AHumanCharacter::MulticastKill_Implementation()
{
	bIsKilled = true;
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (HumanController)
	{
		DisableInput(HumanController);
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHumanCharacter::KillTimerFinished()
{
	if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
	if (TeamDeadMatchMode)
	{
		TeamDeadMatchMode->Respawn(this, Controller);
	}
}

bool AHumanCharacter::IsAiming()
{
	return Combat && Combat->bIsAiming;
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
