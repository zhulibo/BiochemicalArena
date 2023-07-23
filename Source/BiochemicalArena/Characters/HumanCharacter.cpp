#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BiochemicalArena/Components/CombatComponent.h"
#include "BiochemicalArena/ControllerS/HumanController.h"
#include "BiochemicalArena/Modes/InfectMode.h"
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
	CameraComponent->SetRelativeLocation(FVector(15.0f, 10.0f, BaseEyeHeight + 10.0f));
	CameraComponent->bUsePawnControlRotation = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	// 设置CharacterMovementComponent可蹲下
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAO_Pitch();
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(WeaponsMappingContext, 0);
		}
	}

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AHumanCharacter::ReceiveDamage);
	}

}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHumanCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHumanCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AHumanCharacter::JumpButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AHumanCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHumanCharacter::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(CrouchControllerAction, ETriggerEvent::Triggered, this, &AHumanCharacter::CrouchControllerButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AHumanCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AHumanCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AHumanCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &AHumanCharacter::DropButtonPressed);
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

void AHumanCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [360, 270) to [0, -90)
		FVector2D InRange(360.f, 270.f);
		FVector2D OutRange(0.f, -90.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AHumanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHumanCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHumanCharacter::JumpButtonPressed(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Jump();
	}
}

void AHumanCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	Crouch();
}

void AHumanCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	UnCrouch();
}

void AHumanCharacter::CrouchControllerButtonPressed(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AHumanCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimButtonPressed"));
	// if (Combat)
	// {
	// 	Combat->SetAiming(true);
	// }
}

void AHumanCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("AimButtonReleased"));
	// if (Combat)
	// {
	// 	Combat->SetAiming(false);
	// }
}

void AHumanCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AHumanCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AHumanCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	// todo 主动触发脚下的武器的SetOverlappingWeapon
	UE_LOG(LogTemp, Warning, TEXT("SetOverlappingWeapon"));
	// Weapon->GetVelocity().Z != 0判断武器是否在地上
	if (bElimmed || IsWeaponEquipped() || Weapon->GetVelocity().Z != 0) return;
	OverlappingWeapon = Weapon;
	EquipWeaponHandle();
}

void AHumanCharacter::EquipWeaponHandle()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipWeaponHandle();
		}
	}
}

void AHumanCharacter::ServerEquipWeaponHandle_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void AHumanCharacter::DropButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->EquippedWeapon)
	{
		if (HasAuthority())
		{
			Combat->EquippedWeapon->DropWeapon();
			Combat->EquippedWeapon = nullptr;
		}
		else
		{
			ServerDropButtonPressed();
		}
	}
}

void AHumanCharacter::ServerDropButtonPressed_Implementation()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->DropWeapon();
		Combat->EquippedWeapon = nullptr;
	}
}

void AHumanCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
	}
}

void AHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		AInfectMode* InfectMode = GetWorld()->GetAuthGameMode<AInfectMode>();
		if (InfectMode)
		{
			HumanController = HumanController == nullptr ? Cast<AHumanController>(Controller) : HumanController;
			AHumanController* AttackerController = Cast<AHumanController>(InstigatorController);
			InfectMode->PlayerEliminated(this, HumanController, AttackerController);
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
	HumanController = HumanController == nullptr ? Cast<AHumanController>(Controller) : HumanController;
	if (HumanController)
	{
		HumanController->SetHUDHealth(Health, MaxHealth);
	}
}

void AHumanCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

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
	bElimmed = true; // used to disable EquipWeaponHandle(Dropped may cause SetOverlappingWeapon, it execute before MulticastElim)
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->DropWeapon();
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
	AInfectMode* InfectMode = GetWorld()->GetAuthGameMode<AInfectMode>();
	if (InfectMode)
	{
		InfectMode->RequestRespawn(this, Controller);
	}
}

bool AHumanCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AHumanCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AHumanCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AHumanCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}
