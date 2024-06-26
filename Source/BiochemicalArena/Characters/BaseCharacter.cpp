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
#include "BiochemicalArena/Equipments/DamageTypes/FallDamageType.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/OverheadWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), "Camera");
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(50.f);
	GetCharacterMovement()->AirControl = 0.4;
	GetCharacterMovement()->AirControlBoostMultiplier = 1;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 监听输入设备变化
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetWorld()->GetFirstLocalPlayerFromController());
	if (CommonInputSubsystem)
	{
		if (!CommonInputSubsystem->OnInputMethodChangedNative.IsBoundToObject(this)) CommonInputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::OnInputMethodChanged);
	}

	if (OverheadWidget)
	{
		UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
		if (OverheadWidgetClass)
		{
			OverheadWidgetClass->BaseCharacter = this;
		}
	}
}

// 增强输入
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(BaseMappingContext, 0);
		}
	}

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

		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Started, this, &ThisClass::ScoreboardButtonPressed);
		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Completed, this, &ThisClass::ScoreboardButtonReleased);
		EnhancedInputComponent->BindAction(PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::PauseMenuButtonPressed);

		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Started, this, &ThisClass::RadialMenuButtonPressed);
		EnhancedInputComponent->BindAction(RadialMenuAction, ETriggerEvent::Completed, this, &ThisClass::RadialMenuButtonReleased);
		EnhancedInputComponent->BindAction(RadialMenuChangeAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuChange);
		EnhancedInputComponent->BindAction(RadialMenuSelectAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuSelect);
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInitMeshCollision();

	CalcAimPitch();

	PollInit();

	// Don't know why Camera doesn't rotate with the CameraBoom,
	// Manually copy the CameraBone's increased RelativeRotator(camera shake) to the Camera.
	if (IsLocallyControlled())
	{
		FQuat PelvisBoneQuat = GetMesh()->GetBoneQuaternion("Pelvis", EBoneSpaces::WorldSpace); // Need EBoneSpace::LocalSpace pls!
		FQuat CameraBoneQuat = GetMesh()->GetBoneQuaternion("Camera", EBoneSpaces::WorldSpace);
		FQuat RelativeQuat = CameraBoneQuat.Inverse() * PelvisBoneQuat;

		// HACK Use UE_LOG, Print the RelativeRotator when player is in idle pose, and hardcoded it.
		// UE_LOG(LogTemp, Warning, TEXT("IdlePoseRelativeRotator: %s"), *RelativeQuat.Rotator().ToString());
		FRotator IdlePoseRelativeRotator = FRotator(0.000005, -180.000000, 90.090436);

		// IncreasedRotator = Current RelativeRotator - the RelativeRotator of idle pose
		FRotator IncreasedRotator = RelativeQuat.Rotator() - IdlePoseRelativeRotator;

		Camera->SetRelativeRotation(FRotator(IncreasedRotator.Roll, -IncreasedRotator.Yaw, -IncreasedRotator.Pitch));
	}

	// if (IsLocallyControlled())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("--------------------- %d"), GetLocalRole());
	//
	// 	FRotator CameraSocketRotation = GetMesh()->GetSocketRotation("Camera");
	// 	UE_LOG(LogTemp, Warning, TEXT("1: %s"), *CameraSocketRotation.ToString());
	//
	// 	FRotator CameraBoomRotation = CameraBoom->GetComponentRotation();
	// 	UE_LOG(LogTemp, Warning, TEXT("2: %s"), *CameraBoomRotation.ToString());
	//
	// 	FRotator CameraRotation = Camera->GetComponentRotation();
	// 	UE_LOG(LogTemp, Warning, TEXT("3: %s"), *CameraRotation.ToString());
	// }
}

// 设置碰撞
void ABaseCharacter::PollInitMeshCollision()
{
	if (!HasInitMeshCollision)
	{
		if (BasePlayerState == nullptr) BasePlayerState = GetPlayerState<ABasePlayerState>();
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

void ABaseCharacter::PollInit()
{
	if (IsLocallyControlled() && BaseController == nullptr) // WARNING 依赖BaseController为空，需要保证BaseController之前未被赋值
	{
		BaseController = Cast<ABaseController>(Controller);
		if (BaseController)
		{
			OnLocallyControllerReady();

			BaseController->ManualReset();
		}
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

void ABaseCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
	UGameplayStatics::ApplyDamage(this, Health, BaseController, this, UFallDamageType::StaticClass());
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);
}

void ABaseCharacter::Move(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), AxisVector.Y);
	AddMovementInput(GetActorRightVector(), AxisVector.X);
}

// 分开处理Look，无需根据输入设备类型区分灵敏度，支持同时使用键鼠和手柄控制一个角色
void ABaseCharacter::LookMouse(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	AddControllerYawInput(AxisVector.X * StorageSubsystem->StorageCache->MouseSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->StorageCache->MouseSensitivity);
}

void ABaseCharacter::LookStick(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	AddControllerYawInput(AxisVector.X * StorageSubsystem->StorageCache->ControllerSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->StorageCache->ControllerSensitivity);
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

// 键鼠为长按蹲
void ABaseCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFalling()) return;

	Crouch();
}

void ABaseCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFalling()) return;

	UnCrouch();
}

// 手柄为切换蹲
void ABaseCharacter::CrouchControllerButtonPressed(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFalling()) return;

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
	}
}

void ABaseCharacter::RadialMenuButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->CloseRadialMenu();
	}
}

void ABaseCharacter::RadialMenuChange(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ChangeRadialMenu();
	}
}

void ABaseCharacter::RadialMenuSelect(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->SelectRadialMenu(AxisVector.X, AxisVector.Y);
	}
}

// 落地事件（只在本地和服务端执行）
void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (HasAuthority())
	{
		// 计算扣血倍率
		float DamageRate = CalcFallDamageRate();
		if (DamageRate == 0.f) return;

		// 播放叫声
		MulticastPlayOuchSound(DamageRate);

		// 应用伤害
		float TakenDamage = FMath::Clamp(MaxHealth * DamageRate, 0.f, Health);
		UGameplayStatics::ApplyDamage(this, TakenDamage, Controller, this, UFallDamageType::StaticClass());
	}
}

// 计算跌落伤害比例
float ABaseCharacter::CalcFallDamageRate()
{
	FVector Velocity = GetCharacterMovement()->Velocity; // Landed判定的时机是即将落地时，此时速度达到最大
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

void ABaseCharacter::MulticastPlayOuchSound_Implementation(float DamageRate)
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

void ABaseCharacter::SetHealth(float TempHealth)
{
	Health = TempHealth;

	if (IsLocallyControlled()) SetHUDHealth();
}

void ABaseCharacter::OnRep_Health()
{
	if (IsLocallyControlled()) SetHUDHealth();
}

void ABaseCharacter::SetHUDHealth()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->SetHUDHealth(Health);
	}
}
