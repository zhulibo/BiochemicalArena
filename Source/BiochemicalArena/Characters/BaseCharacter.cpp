#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "..\PlayerStates\TeamType.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "CommonInputSubsystem.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(50.f);
	GetCharacterMovement()->AirControl = 0.4;
	GetCharacterMovement()->AirControlBoostMultiplier = 1;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 监听输入设备变化
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetWorld()->GetFirstLocalPlayerFromController());
	if (CommonInputSubsystem)
	{
		CommonInputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::OnInputMethodChanged);
	}
}

// 增强输入
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
		EnhancedInputComponent->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::LookMouse);
		EnhancedInputComponent->BindAction(LookStickAction, ETriggerEvent::Triggered, this, &ThisClass::LookStick);
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

	CalcAimPitch();

	// if (!HasAuthority() && IsLocallyControlled())
	// {
	// 	AddMovementInput(GetActorForwardVector(), 1);
	// }
}

// 设置碰撞
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

// 计算俯仰
void ABaseCharacter::CalcAimPitch()
{
	AimPitch = GetBaseAimRotation().Pitch;

	// Remote character need map pitch from [360, 270) to [0, -90)
	if (AimPitch > 90.f && !IsLocallyControlled())
	{
		FVector2D InRange(360.f, 270.f);
		FVector2D OutRange(0.f, -90.f);
		AimPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimPitch);
	}
}

// 输入设备变化
void ABaseCharacter::OnInputMethodChanged(ECommonInputType TempCommonInputType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInputMethodChanged: %d"), TempCommonInputType);
	CommonInputType = TempCommonInputType;
}

// 根据地形播放不同脚步声
void ABaseCharacter::PlayFootstepSound()
{
	// 射线检测脚下地形
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (HitResult.bBlockingHit && AssetSubsystem)
	{
		switch (UGameplayStatics::GetSurfaceType(HitResult))
		{
		case EPhysicalSurface::SurfaceType1:
			if (AssetSubsystem->MetalSound) UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->MetalSound, HitResult.Location);
			break;
		case EPhysicalSurface::SurfaceType2:
			if (AssetSubsystem->WaterSound) UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->WaterSound, HitResult.Location);
			break;
		case EPhysicalSurface::SurfaceType3:
			if (AssetSubsystem->GrassSound) UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->GrassSound, HitResult.Location);
			break;
		case EPhysicalSurface::SurfaceType4:
			if (AssetSubsystem->MudSound) UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->MudSound, HitResult.Location);
			break;
		default:
			if (AssetSubsystem->CommonSound) UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->CommonSound, HitResult.Location);
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

// 分开处理Look输入，支持同时使用键鼠和手柄控制一个角色
void ABaseCharacter::LookMouse(const FInputActionValue& Value)
{
	if (bIsRadialMenuOpened) return;
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	AddControllerYawInput(AxisVector.X * StorageSubsystem->StorageCache->MouseSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->StorageCache->MouseSensitivity);
}

void ABaseCharacter::LookStick(const FInputActionValue& Value)
{
	if (bIsRadialMenuOpened) return;
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	AddControllerYawInput(AxisVector.X * StorageSubsystem->StorageCache->ControllerSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->StorageCache->ControllerSensitivity);
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

// 键鼠为长按蹲
void ABaseCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	Crouch();
}

void ABaseCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	UnCrouch();
}

// 手柄为切换蹲
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

// 计算跌落伤害比例
float ABaseCharacter::CalcFallDamageRate()
{
	FVector Velocity = GetCharacterMovement()->Velocity; // 用当前帧的速度即可，Landed判定的时机是即将落地时，此时速度达到最大
	float Gravity = GetCharacterMovement()->GetGravityZ();
	float DiffHighMeter = Velocity.Z / Gravity;

	// 角色降落时不遵循自由落体，大致模拟跌落伤害
	float DamageRate; // 跌落扣血比例（占MaxHealth）
	if (DiffHighMeter < 1.f) // 大约对应游戏里5m
	{
		DamageRate = 0.f;
	}
	else if (DiffHighMeter >= 1.f && DiffHighMeter < 1.2f)
	{
		DamageRate = 0.05f;
	}
	else if (DiffHighMeter >= 1.2f && DiffHighMeter < 1.5f)
	{
		DamageRate = 0.1f;
	}
	else
	{
		DamageRate = 0.15f;
	}

	return DamageRate;
}

// 播放跌落受伤声音
void ABaseCharacter::PlayOuchSound(float DamageRate)
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr) return;
	if (DamageRate == 0.05f && AssetSubsystem->OuchSound1)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->OuchSound1, GetActorLocation());
	}
	else if (DamageRate == 0.1f && AssetSubsystem->OuchSound2)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->OuchSound2, GetActorLocation());
	}
	else if (DamageRate == 0.15f && AssetSubsystem->OuchSound3)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AssetSubsystem->OuchSound3, GetActorLocation());
	}
}

void ABaseCharacter::MulticastPlayOuchSound_Implementation(float DamageRate)
{
	if (!IsLocallyControlled())
	{
		PlayOuchSound(DamageRate);
	}
}
