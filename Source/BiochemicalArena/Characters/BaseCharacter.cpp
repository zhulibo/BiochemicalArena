#include "BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "..\PlayerStates\TeamType.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "CommonInputSubsystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BiochemicalArena/Abilities/AttributeSetBase.h"
#include "BiochemicalArena/Abilities/BAAbilitySystemComponent.h"
#include "BiochemicalArena/Abilities/GameplayAbilityBase.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeFall.h"
#include "BiochemicalArena/Equipments/Projectiles/ProjectileBullet.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/OverheadWidget.h"
#include "Components/WidgetComponent.h"
#include "Data/CharacterSound.h"
#include "GameFramework/SpringArmComponent.h"
#include "Data/InputBase.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), "Head");
	CameraBoom->TargetArmLength = 0.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh(), "Head");
	OverheadWidget->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // TODO 垂直向上
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(50.f);
	GetCharacterMovement()->AirControl = 0.4;
	GetCharacterMovement()->AirControlBoostMultiplier = 1;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ControllerPitch);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 监听输入设备类型改变
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetWorld()->GetFirstLocalPlayerFromController());
	if (CommonInputSubsystem)
	{
		if (!CommonInputSubsystem->OnInputMethodChangedNative.IsBoundToObject(this))
		{
			CommonInputSubsystem->OnInputMethodChangedNative.AddUObject(this, &ThisClass::OnInputMethodChanged);
		}
	}

	if (OverheadWidget)
	{
		if (OverheadWidgetClass == nullptr) OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
		if (OverheadWidgetClass)
		{
			OverheadWidgetClass->BaseCharacter = this;
		}
	}

	GetMesh()->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnHit);
}

// 增强输入
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputBase == nullptr) return;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputBase->BaseMappingContext, 0);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(InputBase->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(InputBase->LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::LookMouse);
		EnhancedInputComponent->BindAction(InputBase->LookStickAction, ETriggerEvent::Triggered, this, &ThisClass::LookStick);
		EnhancedInputComponent->BindAction(InputBase->JumpAction, ETriggerEvent::Triggered, this, &ThisClass::JumpButtonPressed);
		EnhancedInputComponent->BindAction(InputBase->CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(InputBase->CrouchAction, ETriggerEvent::Completed, this, &ThisClass::CrouchButtonReleased);
		EnhancedInputComponent->BindAction(InputBase->CrouchControllerAction, ETriggerEvent::Triggered, this, &ThisClass::CrouchControllerButtonPressed);

		EnhancedInputComponent->BindAction(InputBase->ScoreboardAction, ETriggerEvent::Started, this, &ThisClass::ScoreboardButtonPressed);
		EnhancedInputComponent->BindAction(InputBase->ScoreboardAction, ETriggerEvent::Completed, this, &ThisClass::ScoreboardButtonReleased);
		EnhancedInputComponent->BindAction(InputBase->PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::PauseMenuButtonPressed);

		EnhancedInputComponent->BindAction(InputBase->RadialMenuAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuButtonPressed);
		EnhancedInputComponent->BindAction(InputBase->RadialMenuAction, ETriggerEvent::Completed, this, &ThisClass::RadialMenuButtonReleased);
		EnhancedInputComponent->BindAction(InputBase->RadialMenuChangeAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuChangeButtonPressed);
		EnhancedInputComponent->BindAction(InputBase->RadialMenuSelectAction, ETriggerEvent::Triggered, this, &ThisClass::RadialMenuSelect);
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollSetMeshCollision();

	CalcAimPitch();

	PollInit();
}

// 设置碰撞
void ABaseCharacter::PollSetMeshCollision()
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
	// 把ControllerPitch复制到模拟端
	if (HasAuthority())
	{
		ControllerPitch = GetViewRotation().Pitch;
	}

	AimPitch = ControllerPitch;

	// 本地存在Controller，覆盖掉网络复制的值，避免延迟
	if (IsLocallyControlled())
	{
		AimPitch = GetViewRotation().Pitch;
	}

	if (AimPitch > 90.f)
	{
		FVector2D InRange(360.f, 270.f);
		FVector2D OutRange(0.f, -90.f);
		AimPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimPitch);
	}
}

void ABaseCharacter::PollInit()
{
	if (IsLocallyControlled() && !bIsControllerReady)
	{
		BaseController = Cast<ABaseController>(Controller);
		if (BaseController)
		{
			bIsControllerReady = true;

			OnControllerReady();

			BaseController->ManualReset();
		}
	}
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 初始化ASC
	InitAbilityActorInfo();

	OnAbilitySystemComponentInit();

	if (AbilitySystemComponent)
	{
		// 赋予初始技能
		for (TSubclassOf<UGameplayAbilityBase> StartupAbility : StartupAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility));
		}

		// 赋予初始Effect
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		for (TSubclassOf<UGameplayEffect> StartupEffect : StartupEffects)
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StartupEffect, GetCharacterLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		// 赋值默认AttributeSet Effect
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, GetCharacterLevel(), EffectContext);
		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
		}
	}
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	OnAbilitySystemComponentInit();
}

void ABaseCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (BloodEffect_Projectile)
	{
		// UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::OnHit Location: %s"), *Hit.ImpactPoint.ToString());
		// UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::OnHit Rotation: %s"), *Hit.ImpactNormal.Rotation().ToString());

		FRotator HitRotation = Hit.ImpactNormal.Rotation();
		auto BloodEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BloodEffect_Projectile,
			Hit.ImpactPoint,
			FRotator(-HitRotation.Pitch, HitRotation.Yaw + 180.f, HitRotation.Roll)
		);

		if (AProjectileBullet* ProjectileBullet = Cast<AProjectileBullet>(OtherActor))
		{
			float Damage = ProjectileBullet->GetDamage(Hit.Distance);
			BloodEffectComponent->SetVariableInt("ParticleCount", ULibraryCommon::GetBloodParticleCount(Damage));
		}
		BloodEffectComponent->SetVariableLinearColor("Color", BloodColor);
	}
}

void ABaseCharacter::InitAbilityActorInfo()
{
	if (BasePlayerState == nullptr) BasePlayerState = GetPlayerState<ABasePlayerState>();
	if (BasePlayerState)
	{
		AbilitySystemComponent = Cast<UBAAbilitySystemComponent>(BasePlayerState->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(BasePlayerState, this);
		AttributeSetBase = BasePlayerState->GetAttributeSetBase();
	}
}

void ABaseCharacter::OnAbilitySystemComponentInit()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
	}
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetBase* ABaseCharacter::GetAttributeSetBase()
{
	return AttributeSetBase;
}

float ABaseCharacter::GetMaxHealth()
{
	return AttributeSetBase ? AttributeSetBase->GetMaxHealth() : 0.f;
}

float ABaseCharacter::GetHealth()
{
	return AttributeSetBase ? AttributeSetBase->GetHealth() : 0.f;
}

float ABaseCharacter::GetDamageReceivedMul()
{
	return AttributeSetBase ? AttributeSetBase->GetDamageReceivedMul() : 0.f;
}

float ABaseCharacter::GetRepelReceivedMul()
{
	return AttributeSetBase ? AttributeSetBase->GetRepelReceivedMul() : 0.f;
}

float ABaseCharacter::GetCharacterLevel()
{
	return AttributeSetBase ? AttributeSetBase->GetCharacterLevel() : 0.f;
}

float ABaseCharacter::GetJumpZVelocity()
{
	return AttributeSetBase ? AttributeSetBase->GetJumpZVelocity() : 0.f;
}

// 输入设备类型改变
void ABaseCharacter::OnInputMethodChanged(ECommonInputType TempCommonInputType)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInputMethodChanged: %d"), TempCommonInputType);

	CommonInputType = TempCommonInputType;
}

// 根据地形播放不同脚步声
void ABaseCharacter::PlayFootstepSound()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0.f, 0.f, 100.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (HitResult.bBlockingHit && AssetSubsystem && AssetSubsystem->CharacterSound)
	{
		USoundCue* Sound;
		switch (UGameplayStatics::GetSurfaceType(HitResult))
		{
		case EPhysicalSurface::SurfaceType1:
			Sound = AssetSubsystem->CharacterSound->MetalSound;
			break;
		case EPhysicalSurface::SurfaceType2:
			Sound = AssetSubsystem->CharacterSound->WaterSound;
			break;
		case EPhysicalSurface::SurfaceType3:
			Sound = AssetSubsystem->CharacterSound->GrassSound;
			break;
		case EPhysicalSurface::SurfaceType4:
			Sound = AssetSubsystem->CharacterSound->MudSound;
			break;
		default:
			Sound = AssetSubsystem->CharacterSound->CommonSound;
			break;
		}
		UGameplayStatics::PlaySoundAtLocation(this, Sound, HitResult.Location);
	}
}

void ABaseCharacter::FellOutOfWorld(const UDamageType& DmgType)
{
	UGameplayStatics::ApplyDamage(this, GetHealth(), BaseController, this, UDamageTypeFall::StaticClass());
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
	AddControllerYawInput(AxisVector.X * StorageSubsystem->CacheSetting->MouseSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->CacheSetting->MouseSensitivity);
}

void ABaseCharacter::LookStick(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	AddControllerYawInput(AxisVector.X * StorageSubsystem->CacheSetting->ControllerSensitivity);
	AddControllerPitchInput(AxisVector.Y * StorageSubsystem->CacheSetting->ControllerSensitivity);
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
		BaseController->ShowScoreboard.Broadcast(true);
	}
}

void ABaseCharacter::ScoreboardButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowScoreboard.Broadcast(false);
	}
}

void ABaseCharacter::PauseMenuButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowPauseMenu.Broadcast();
	}
}

void ABaseCharacter::RadialMenuButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowRadialMenu.Broadcast(true);
	}
}

void ABaseCharacter::RadialMenuButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowRadialMenu.Broadcast(false);
	}
}

void ABaseCharacter::RadialMenuChangeButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ChangeRadialMenu.Broadcast();
	}
}

// TODO 弦操作使用IA_RadialMenu时，手柄RadialMenuButtonPressed不触发，使用了新建的IA_RadialMenuSelectChord
void ABaseCharacter::RadialMenuSelect(const FInputActionValue& Value)
{
	FVector2D AxisVector = Value.Get<FVector2D>();
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->SelectRadialMenu.Broadcast(AxisVector.X, AxisVector.Y);
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
		float TakenDamage = FMath::Clamp(GetMaxHealth() * DamageRate, 0.f, GetHealth());
		UGameplayStatics::ApplyDamage(this, TakenDamage, Controller, this, UDamageTypeFall::StaticClass());
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
	if (OuchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OuchSound, GetActorLocation());
	}
}

void ABaseCharacter::SetHealth(float TempHealth)
{
	if (AttributeSetBase)
	{
		AttributeSetBase->SetHealth(TempHealth);
	}
}

// 使用RPC通知攻击者立刻响应受伤者血量变化（UAttributeSetBase中的Health同步有点慢）
void ABaseCharacter::MulticastSetHealth_Implementation(float TempHealth, AController* AttackerController)
{
	if (HasAuthority()) return;

	if (AttackerController && AttackerController->IsLocalController())
	{
		SetHealth(TempHealth);
	}
}

void ABaseCharacter::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if (OverheadWidgetClass == nullptr) OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
	if (OverheadWidgetClass)
	{
		OverheadWidgetClass->OnMaxHealthChange(Data.NewValue);
	}
}

void ABaseCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (OverheadWidgetClass == nullptr) OverheadWidgetClass = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject());
	if (OverheadWidgetClass)
	{
		OverheadWidgetClass->OnHealthChange(Data.OldValue, Data.NewValue);
	}

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->SetHUDHealth(Data.NewValue);
	}
}