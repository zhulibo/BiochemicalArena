#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AHumanCharacter::AHumanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 创建摄像机组件
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	// 将摄像机组件附加到胶囊体组件
	CameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	// 将摄像机置于略高于眼睛上方的位置
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// 启用Pawn控制摄像机旋转
	CameraComponent->bUsePawnControlRotation = true;

	// 创建手臂组件
	ArmComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmComponent"));
	// 仅所属玩家可见
	ArmComponent->SetOnlyOwnerSee(true);
	// 添加到摄像机
	ArmComponent->SetupAttachment(CameraComponent);
	// 禁用某些环境阴影
	ArmComponent->bCastDynamicShadow = false;
	ArmComponent->CastShadow = false;
	// 对所属玩家隐藏全身网格体
	GetMesh()->SetOwnerNoSee(true);

	// 设置CharacterMovementComponent可蹲下
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

// Called every frame
void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 旋转跳测试
	// int32 Speed = FMath::RoundToInt(GetVelocity().Size2D());
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, Speed > 600 ? FColor::Red : FColor::Green, FString::Printf(TEXT("%d"), Speed));
}

// Called when the game starts or when spawned
void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 判断输入设备是键鼠还是手柄
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	CommonInputSubsystem = UCommonInputSubsystem::Get(LocalPlayer);
	StartDetect();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			Subsystem->AddMappingContext(WeaponsMappingContext, 0);
		}
	}

}

// Called to bind functionality to input
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
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AHumanCharacter::Fire);
	}
}

void AHumanCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHumanCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHumanCharacter::JumpButtonPressed(const FInputActionValue& Value)
{
	Jump();
}

void AHumanCharacter::CrouchButtonPressed(const FInputActionValue& Value)
{
	// 手柄
	if (bController)
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
	// 键鼠
	else
	{
		Crouch();
	}
}

void AHumanCharacter::CrouchButtonReleased(const FInputActionValue& Value)
{
	// 键鼠
	if (!bController)
	{
		UnCrouch();
	}
}

void AHumanCharacter::Fire(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Fire"));
	// 试图发射发射物
	if (ProjectileClass)
	{
		// 获取摄像机变换
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// 设置MuzzleOffset，在略靠近摄像机前生成发射物
		MuzzleOffset.Set(100.0f, 0.f, 0.f);

		// 将MuzzleOffset从摄像机空间变换到世界空间
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// 使目标方向略向上倾斜
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// 在枪口位置生成发射物
			AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// 设置发射物的初始轨迹。
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}

void AHumanCharacter::StartDetect()
{
	GetWorldTimerManager().SetTimer(DetectTimer, this, &AHumanCharacter::DetectCurrentInputDeviceType, DetectDelay, true);
}

void AHumanCharacter::DetectCurrentInputDeviceType()
{
	if (CommonInputSubsystem)
	{
		CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
		if (CurrentInputType == ECommonInputType::Gamepad)
		{
			bController = true;
		}
		else
		{
			bController = false;
		}
	}
}

bool AHumanCharacter::CanJumpInternal_Implementation() const
{
	return GetCharacterMovement()->IsFalling() ? false : true;
}
