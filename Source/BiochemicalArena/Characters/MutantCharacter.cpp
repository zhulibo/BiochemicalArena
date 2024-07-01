#include "MutantCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HumanCharacter.h"
#include "MutantAnimInstance.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Equipments/DamageTypes/MutantDamageType.h"
#include "BiochemicalArena/GameModes/BaseMode.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AMutantCharacter::AMutantCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MutantState = EMutantState::Ready;

	RightHandCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandCapsule"));
	RightHandCapsule->SetupAttachment(GetMesh(), "RightHandCapsule");
	RightHandCapsule->SetGenerateOverlapEvents(true);
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCapsule->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
	RightHandCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnRightHandCapsuleOverlap);

	LeftHandCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandCapsule"));
	LeftHandCapsule->SetupAttachment(GetMesh(), "LeftHandCapsule");
	LeftHandCapsule->SetGenerateOverlapEvents(true);
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCapsule->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
	LeftHandCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnLeftHandCapsuleOverlap);
}

void AMutantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMutantCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMutantCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddUniqueDynamic(this, &ThisClass::MutantReceiveDamage);
	}
}

void AMutantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(MutantMappingContext, 1);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ThisClass::LightAttackButtonPressed);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Completed, this, &ThisClass::LightAttackButtonReleased);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttackButtonPressed);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ThisClass::HeavyAttackButtonReleased);
	}
}

void AMutantCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 调试用 自动行走
	// if (IsLocallyControlled())
	// {
	// 	AddMovementInput(GetActorForwardVector(), 1);
	// }
}

void AMutantCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BaseController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->RemoveMappingContext(MutantMappingContext);
		}
	}
}

void AMutantCharacter::LightAttackButtonPressed(const FInputActionValue& Value)
{
	if (MutantState != EMutantState::Ready) return;

	LocalLightAttack();
	ServerLightAttack();
}

void AMutantCharacter::ServerLightAttack_Implementation()
{
	MulticastLightAttack();
}

void AMutantCharacter::MulticastLightAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		LocalLightAttack();
	}
}

void AMutantCharacter::LocalLightAttack()
{
	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantState = EMutantState::LightAttacking;
		MutantAnimInstance->Montage_Play(LightAttackMontage);

		// 动画结束后，重置状态
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			MutantState = EMutantState::Ready;
			RightHandHitEnemies.Empty();
			LeftHandHitEnemies.Empty();
		});
		MutantAnimInstance->Montage_SetEndDelegate(OnMontageEnded, LightAttackMontage);
	}
}

void AMutantCharacter::LightAttackButtonReleased(const FInputActionValue& Value)
{
}

void AMutantCharacter::HeavyAttackButtonPressed(const FInputActionValue& Value)
{
	if (MutantState != EMutantState::Ready) return;

	LocalHeavyAttack();
	ServerHeavyAttack();
}

void AMutantCharacter::ServerHeavyAttack_Implementation()
{
	MulticastHeavyAttack();
}

void AMutantCharacter::MulticastHeavyAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		LocalHeavyAttack();
	}
}

void AMutantCharacter::LocalHeavyAttack()
{
	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantState = EMutantState::HeavyAttacking;
		MutantAnimInstance->Montage_Play(HeavyAttackMontage);

		// 动画结束后，重置状态
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			MutantState = EMutantState::Ready;
			RightHandHitEnemies.Empty();
			LeftHandHitEnemies.Empty();
		});
		MutantAnimInstance->Montage_SetEndDelegate(OnMontageEnded, HeavyAttackMontage);
	}
}

void AMutantCharacter::HeavyAttackButtonReleased(const FInputActionValue& Value)
{
}

void AMutantCharacter::RightHandAttackBegin()
{
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMutantCharacter::RightHandAttackEnd()
{
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutantCharacter::LeftHandAttackBegin()
{
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMutantCharacter::LeftHandAttackEnd()
{
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutantCharacter::OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && !RightHandHitEnemies.Contains(OtherActor))
	{
		RightHandHitEnemies.Add(OtherActor);

		// 造成伤害
		float Damage = MutantState == EMutantState::LightAttacking ? LightAttackDamage : HeavyAttackDamage;
		UGameplayStatics::ApplyDamage(OtherActor, Damage, Controller, this, UMutantDamageType::StaticClass());

		// 造成感染
		AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
		if (HumanCharacter && BaseController)
		{
			HumanCharacter->GetInfect(this, BaseController, MutantState);
		}
	}
}

void AMutantCharacter::OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority() && !LeftHandHitEnemies.Contains(OtherActor))
	{
		LeftHandHitEnemies.Add(OtherActor);

		// 造成伤害
		float Damage = MutantState == EMutantState::LightAttacking ? LightAttackDamage : HeavyAttackDamage;
		UGameplayStatics::ApplyDamage(OtherActor, Damage, Controller, this, UMutantDamageType::StaticClass());

		// 造成感染
		AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
		if (HumanCharacter && BaseController)
		{
			HumanCharacter->GetInfect(this, BaseController, MutantState);
		}
	}
}

void AMutantCharacter::MutantReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* AttackerController, AActor* DamageCauser)
{
	if (bIsDead) return;

	if (BaseMode == nullptr) BaseMode = GetWorld()->GetAuthGameMode<ABaseMode>();
	if (BaseMode)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
		if (BaseController)
		{
			BaseMode->MutantReceiveDamage(this, BaseController, Damage, DamageType, AttackerController, DamageCauser);
		}
	}
}

void AMutantCharacter::MulticastDead_Implementation()
{
	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController) DisableInput(BaseController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 重生
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			if (MutationMode == nullptr) MutationMode = GetWorld()->GetAuthGameMode<AMutationMode>();
			if (MutationMode)
			{
				MutationMode->MutantRespawn(this, Controller);
			}
		});
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 3.f, false);
	}
}

void AMutantCharacter::ServerSelectCharacter_Implementation(const FString& CharacterName)
{
	if (BasePlayerState == nullptr) BasePlayerState = GetPlayerState<ABasePlayerState>();
	if (BasePlayerState)
	{
		BasePlayerState->SetMutantCharacterName(CharacterName);
	}

	if (MutationMode == nullptr) MutationMode = GetWorld()->GetAuthGameMode<AMutationMode>();
	if (MutationMode)
	{
		MutationMode->SelectCharacter(this, Controller);
	}
}
