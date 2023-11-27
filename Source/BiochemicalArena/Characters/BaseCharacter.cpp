#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 设置CharacterMovementComponent可蹲下
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	MetalSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	WaterSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	GrassSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	MudSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	CommonSound = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Common_Cue.Footsteps_Common_Cue"));

	OuchSound1 = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	OuchSound2 = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
	OuchSound3 = LoadObject<USoundCue>(nullptr, TEXT("/Game/Assets/Sounds/Footsteps/Footsteps_Water_Cue.Footsteps_Water_Cue"));
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CalculateAO_Pitch();
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
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABaseCharacter::JumpButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABaseCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABaseCharacter::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(CrouchControllerAction, ETriggerEvent::Triggered, this, &ABaseCharacter::CrouchControllerButtonPressed);
	}
}

void ABaseCharacter::PlayFootstepSound()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, QueryParams);

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
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ABaseCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABaseCharacter::JumpButtonPressed(const FInputActionValue& Value)
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

void ABaseCharacter::CalculateAO_Pitch()
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

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

float ABaseCharacter::CalcFallDamageCoefficient()
{
	FVector Velocity = GetCharacterMovement()->Velocity; // 用当前帧的速度即可，Landed判定的时机是即将落地时，此时速度达到最大
	float Gravity = GetCharacterMovement()->GetGravityZ();
	float DiffHighMeter = Velocity.Z / Gravity;

	// 测试发现角色降落时不遵循自由落体，下面大致模拟跌落伤害
	float DamageCoefficient; // 跌落扣血系数（占MaxHealth）
	if (DiffHighMeter < 1.f) // 大约对应游戏里5m
	{
		DamageCoefficient = 0.f;
	}
	else if (DiffHighMeter >= 1.f && DiffHighMeter < 1.2f)
	{
		DamageCoefficient = 0.05f;
	}
	else if (DiffHighMeter >= 1.2f && DiffHighMeter < 1.5f)
	{
		DamageCoefficient = 0.1f;
	}
	else
	{
		DamageCoefficient = 0.15f;
	}

	return DamageCoefficient;
}

void ABaseCharacter::PlayOuchWeaponSound(float DamageCoefficient)
{
	if (DamageCoefficient == 0.05f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound1, GetActorLocation());
	}
	else if (DamageCoefficient == 0.1f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound2, GetActorLocation());
	}
	else if (DamageCoefficient == 0.15f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound3, GetActorLocation());
	}
}

void ABaseCharacter::MulticastPlayOuchWeaponSound_Implementation(float DamageCoefficient)
{
	if (!IsLocallyControlled()) PlayOuchWeaponSound(DamageCoefficient);
}
