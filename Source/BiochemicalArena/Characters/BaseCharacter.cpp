#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/PlayerStates/Team.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true; // 设置CharacterMovementComponent可蹲下
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	MetalSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	WaterSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	GrassSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	MudSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	CommonSound = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Common_Cue.Footsteps_Common_Cue'"));

	OuchSound1 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	OuchSound2 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
	OuchSound3 = LoadObject<USoundCue>(nullptr, TEXT("/Script/Engine.SoundCue'/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue'"));
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(BaseMappingContext, 0);
		}
	}
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::JumpButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(CrouchControllerAction, ETriggerEvent::Triggered, this, &ThisClass::CrouchControllerButtonPressed);

		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::ScoreboardButtonPressed);
		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Completed, this, &ThisClass::ScoreboardButtonReleased);
		EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::PauseMenuButtonPressed);

		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Started, this, &ThisClass::RadialMenuButtonPressed);
		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Completed, this, &ThisClass::RadialMenuButtonReleased);
		EnhancedInputComponent->BindAction(RadialMenuSelectAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuSelect);
		EnhancedInputComponent->BindAction(RadialMenuChangeAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuChange);
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInitMeshCollision();

	CalculateAO_Pitch();
}

void ABaseCharacter::PollInitMeshCollision()
{
	if (!HasInitMeshCollision)
	{
		if (BasePlayerState == nullptr)
		{
			BasePlayerState = GetPlayerState<ABasePlayerState>();
		}
		if (BasePlayerState && BasePlayerState->GetTeam() != ETeam::NoTeam)
		{
			switch (BasePlayerState->GetTeam())
			{
			case ETeam::Team1:
				GetMesh()->SetCollisionObjectType(ECC_Team1SkeletalMesh);
				break;
			case ETeam::Team2:
				GetMesh()->SetCollisionObjectType(ECC_Team2SkeletalMesh);
				break;
			}
			HasInitMeshCollision = true;
		}
	}
}

void ABaseCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled()) // Remote character need map pitch from [360, 270) to [0, -90)
	{
		FVector2D InRange(360.f, 270.f);
		FVector2D OutRange(0.f, -90.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABaseCharacter::PlayFootstepSound()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, Params);

	if (HitResult.bBlockingHit)
	{
		EPhysicalSurface HitSurface = UGameplayStatics::GetSurfaceType(HitResult);
		FVector_NetQuantize HitLocation = HitResult.Location;

		switch (HitSurface)
		{
		case EPhysicalSurface::SurfaceType1:
			if (MetalSound) UGameplayStatics::PlaySoundAtLocation(this, MetalSound, HitLocation);
			break;
		case EPhysicalSurface::SurfaceType2:
			if (WaterSound) UGameplayStatics::PlaySoundAtLocation(this, WaterSound, HitLocation);
			break;
		case EPhysicalSurface::SurfaceType3:
			if (GrassSound) UGameplayStatics::PlaySoundAtLocation(this, GrassSound, HitLocation);
			break;
		case EPhysicalSurface::SurfaceType4:
			if (MudSound) UGameplayStatics::PlaySoundAtLocation(this, MudSound, HitLocation);
			break;
		default:
			if (CommonSound) UGameplayStatics::PlaySoundAtLocation(this, CommonSound, HitLocation);
			break;
		}
	}
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), AxisVector.Y);
	AddMovementInput(GetActorRightVector(), AxisVector.X);
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	if (bIsRadialMenuOpened) return;
	FVector2D AxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(AxisVector.X);
	AddControllerPitchInput(AxisVector.Y);
}

void ABaseCharacter::JumpButtonPressed(const FInputActionValue& Value)
{
	if (bIsRadialMenuOpened) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Jump();
	}
}

void ABaseCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	Crouch();
}

void ABaseCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	UnCrouch();
}

void ABaseCharacter::CrouchControllerButtonPressed(const FInputActionValue& Value)
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

void ABaseCharacter::ScoreboardButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowScoreboard(true);
	}
}

void ABaseCharacter::ScoreboardButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowScoreboard(false);
	}
}

void ABaseCharacter::PauseMenuButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowPauseMenu();
	}
}

void ABaseCharacter::RadialMenuButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowRadialMenu();
		bIsRadialMenuOpened = true;
	}
}

void ABaseCharacter::RadialMenuButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->CloseRadialMenu();
		bIsRadialMenuOpened = false;
	}
}

void ABaseCharacter::RadialMenuChange(const FInputActionValue& Value)
{
	if (!bIsRadialMenuOpened) return;
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ChangeRadialMenu();
	}
}

void ABaseCharacter::RadialMenuSelect(const FInputActionValue& Value)
{
	if (!bIsRadialMenuOpened) return;
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->SelectRadialMenu(AxisVector.X, AxisVector.Y);
	}
}

float ABaseCharacter::CalcFallDamageFactor()
{
	FVector Velocity = GetCharacterMovement()->Velocity; // 用当前帧的速度即可，Landed判定的时机是即将落地时，此时速度达到最大
	float Gravity = GetCharacterMovement()->GetGravityZ();
	float DiffHighMeter = Velocity.Z / Gravity;

	// 角色降落时不遵循自由落体，大致模拟跌落伤害
	float DamageFactor; // 跌落扣血比例（占MaxHealth）
	if (DiffHighMeter < 1.f) // 大约对应游戏里5m
	{
		DamageFactor = 0.f;
	}
	else if (DiffHighMeter >= 1.f && DiffHighMeter < 1.2f)
	{
		DamageFactor = 0.05f;
	}
	else if (DiffHighMeter >= 1.2f && DiffHighMeter < 1.5f)
	{
		DamageFactor = 0.1f;
	}
	else
	{
		DamageFactor = 0.15f;
	}

	return DamageFactor;
}

void ABaseCharacter::PlayOuchSound(float DamageFactor)
{
	if (DamageFactor == 0.05f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound1, GetActorLocation());
	}
	else if (DamageFactor == 0.1f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound2, GetActorLocation());
	}
	else if (DamageFactor == 0.15f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound3, GetActorLocation());
	}
}

void ABaseCharacter::MulticastPlayOuchSound_Implementation(float DamageFactor)
{
	if (!IsLocallyControlled())
	{
		PlayOuchSound(DamageFactor);
	}
}
