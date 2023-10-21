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
}

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AHumanCharacter::ReceiveDamage);
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
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AHumanCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AHumanCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AHumanCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AHumanCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AHumanCharacter::ReloadButtonPressed);
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

void AHumanCharacter::ServerDetectOverlappingWeapon_Implementation()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AWeapon::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		AWeapon* Weapon = Cast<AWeapon>(OverlappingActor);
		if (Weapon)
		{
			SetOverlappingWeapon(Weapon);
			break;
		}
	}
}

void AHumanCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsWeaponEquipped() || bElimmed || Weapon->GetOwner()) return;
	OverlappingWeapon = Weapon;
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
	// 检测是否有武器重叠
	ServerDetectOverlappingWeapon();
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

void AHumanCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_GeneralWeapon:
			SectionName = FName("AK47");
			break;
		case EWeaponType::EWT_Shotgun:
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
		ATeamDeadMatchMode* TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
		if (TeamDeadMatchMode)
		{
			if (HumanController == nullptr)
			{
				HumanController = Cast<AHumanController>(Controller);
			}
			if (AHumanController* AttackerController = Cast<AHumanController>(InstigatorController))
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
	if (HumanController == nullptr)
	{
		HumanController = Cast<AHumanController>(Controller);
	}
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
	bElimmed = true; // set immediately
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
	ATeamDeadMatchMode* TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
	if (TeamDeadMatchMode)
	{
		TeamDeadMatchMode->RequestRespawn(this, Controller);
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

ECombatState AHumanCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_Max;
	return Combat->CombatState;
}
