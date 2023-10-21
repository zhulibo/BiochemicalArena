#include "CommonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACommonCharacter::ACommonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 设置CharacterMovementComponent可蹲下
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void ACommonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateAO_Pitch();
}

void ACommonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACommonCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACommonCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACommonCharacter::JumpButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACommonCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACommonCharacter::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(CrouchControllerAction, ETriggerEvent::Triggered, this, &ACommonCharacter::CrouchControllerButtonPressed);
	}
}

void ACommonCharacter::PlayFootstepSound()
{
	// TODO 测试声音是否消失
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

void ACommonCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACommonCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACommonCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACommonCharacter::JumpButtonPressed(const FInputActionValue& Value)
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

void ACommonCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	Crouch();
}

void ACommonCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	UnCrouch();
}

void ACommonCharacter::CrouchControllerButtonPressed(const FInputActionValue& Value)
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

void ACommonCharacter::CalculateAO_Pitch()
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
