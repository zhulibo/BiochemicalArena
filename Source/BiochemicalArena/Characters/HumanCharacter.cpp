#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "..\Equipments\Throwing.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/Equipments/Melee.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "..\System\StorageSaveGameType.h"
#include "..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatStateType.h"
#include "Components/OverheadWidget.h"

AHumanCharacter::AHumanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "Camera");
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);
}

void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Health);

	DOREPLIFETIME(ThisClass, DefaultPrimaryEquipment);
	DOREPLIFETIME(ThisClass, DefaultSecondaryEquipment);
	DOREPLIFETIME(ThisClass, DefaultMeleeEquipment);
	DOREPLIFETIME(ThisClass, DefaultThrowingEquipment);
}

void AHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat && Combat->Character == nullptr)
	{
		Combat->Character = this;
	}
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (OverheadWidget)
	{
		UUserWidget* TempUserWidget = OverheadWidget->GetUserWidgetObject();
		if (TempUserWidget)
		{
			UOverheadWidget* OverheadWidgetClass = Cast<UOverheadWidget>(TempUserWidget);
			if (OverheadWidgetClass && OverheadWidgetClass->HumanCharacter == nullptr)
			{
				OverheadWidgetClass->HumanCharacter = this;
			}
		}
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
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
			Subsystem->AddMappingContext(EquipmentMappingContext, 0);
		}
	}

	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimButtonReleased);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ThisClass::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ThisClass::FireButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Triggered, this, &ThisClass::DropButtonPressed);
		EnhancedInputComponent->BindAction(SwapPrimaryEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapPrimaryEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(SwapSecondaryEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapSecondaryEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(SwapMeleeEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapMeleeEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(SwapThrowingEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapThrowingEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(SwapLastEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapLastEquipmentButtonPressed);
	}
}

void AHumanCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PollInit();
}

void AHumanCharacter::PollInit()
{
	if (IsLocallyControlled() && HumanController == nullptr)
	{
		HumanController = Cast<AHumanController>(Controller);
		if (HumanController)
		{
			HumanController->SetHasInitDefaultHUD(false);

			// 获取本地背包里的装备的名字
			FString PrimaryEquipmentName = GetEquipmentName(0, EEquipmentType::Primary);
			FString SecondaryEquipmentName = GetEquipmentName(0, EEquipmentType::Secondary);
			FString MeleeEquipmentName = GetEquipmentName(0, EEquipmentType::Melee);
			FString ThrowingEquipmentName = GetEquipmentName(0, EEquipmentType::Throwing);

			// 本地Controller就绪后，在服务端生成武器，然后复制到所有客户端
			ServerSetDefaultEquipment(PrimaryEquipmentName, SecondaryEquipmentName, MeleeEquipmentName, ThrowingEquipmentName);
		}
	}
}

void AHumanCharacter::ServerSetDefaultEquipment_Implementation(const FString& PrimaryEquipmentName,
	const FString& SecondaryEquipmentName, const FString& MeleeEquipmentName, const FString& ThrowingEquipmentName)
{
	if (Combat == nullptr) return;

	// 加载装备类
	UClass* PrimaryEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(PrimaryEquipmentName));
	UClass* SecondaryEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(SecondaryEquipmentName));
	UClass* MeleeEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(MeleeEquipmentName));
	UClass* ThrowingEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(ThrowingEquipmentName));

	if (PrimaryEquipmentClass)
	{
		DefaultPrimaryEquipment = GetWorld()->SpawnActor<AWeapon>(PrimaryEquipmentClass);
		Combat->LocalEquipEquipment(DefaultPrimaryEquipment);
		Combat->LocalSwapEquipment(EEquipmentType::Primary);
	}
	if (SecondaryEquipmentClass)
	{
		DefaultSecondaryEquipment = GetWorld()->SpawnActor<AWeapon>(SecondaryEquipmentClass);
		Combat->LocalEquipEquipment(DefaultSecondaryEquipment);
	}
	if (MeleeEquipmentClass)
	{
		DefaultMeleeEquipment = GetWorld()->SpawnActor<AMelee>(MeleeEquipmentClass);
		Combat->LocalEquipEquipment(DefaultMeleeEquipment);
	}
	if (ThrowingEquipmentClass)
	{
		DefaultThrowingEquipment = GetWorld()->SpawnActor<AThrowing>(ThrowingEquipmentClass);
		Combat->LocalEquipEquipment(DefaultThrowingEquipment);
	}
}

// 获取装备的名字
FString AHumanCharacter::GetEquipmentName(int32 BagIndex, EEquipmentType EquipmentType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem == nullptr || StorageSubsystem->StorageCache->Bags.Num() == 0) return FString();
	FString EquipmentName;
	switch (EquipmentType)
	{
	case EEquipmentType::Primary:
		EquipmentName = StorageSubsystem->StorageCache->Bags[BagIndex].Primary;
		break;
	case EEquipmentType::Secondary:
		EquipmentName = StorageSubsystem->StorageCache->Bags[BagIndex].Secondary;
		break;
	case EEquipmentType::Melee:
		EquipmentName = StorageSubsystem->StorageCache->Bags[BagIndex].Melee;
		break;
	case EEquipmentType::Throwing:
		EquipmentName = StorageSubsystem->StorageCache->Bags[BagIndex].Throwing;
		break;
	}
	return EquipmentName;
}

// 获取装备类的存放路径
FString AHumanCharacter::GetEquipmentClassPath(FString EquipmentName)
{
	if (EquipmentName.IsEmpty()) return FString();
	if (EquipmentName.Contains("_")) // HACK 如果装备名字包含下划线，说明是带皮肤的装备
	{
		FString EquipmentFolderName = EquipmentName.Left(EquipmentName.Find("_"));
		return FString::Printf(TEXT("/Script/Engine.Blueprint'/Game/Equipments/Main/%s/%s/BP_%s.BP_%s_C'"), *EquipmentFolderName, *EquipmentName, *EquipmentName, *EquipmentName);
	}
	return FString::Printf(TEXT("/Script/Engine.Blueprint'/Game/Equipments/Main/%s/Default/BP_%s.BP_%s_C'"), *EquipmentName, *EquipmentName, *EquipmentName);
}

void AHumanCharacter::OnRep_DefaultPrimaryEquipment()
{
	if (Combat)
	{
		Combat->LocalEquipEquipment(DefaultPrimaryEquipment);
		Combat->LocalSwapEquipment(EEquipmentType::Primary);
	}
}

void AHumanCharacter::OnRep_DefaultSecondaryEquipment()
{
	if (Combat)
	{
		Combat->LocalEquipEquipment(DefaultSecondaryEquipment);
	}
}

void AHumanCharacter::OnRep_DefaultMeleeEquipment()
{
	if (Combat)
	{
		Combat->LocalEquipEquipment(DefaultMeleeEquipment);
	}
}

void AHumanCharacter::OnRep_DefaultThrowingEquipment()
{
	if (Combat)
	{
		Combat->LocalEquipEquipment(DefaultThrowingEquipment);
	}
}

// 落地事件（测试发现只在本地和服务端执行）
void AHumanCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	float DamageRate = CalcFallDamageRate();
	if (DamageRate == 0.f) return;

	// 扣血（扣血只发生在本地和服务端，然后服务端会通过属性复制把Health更新到模拟角色上）
	Health = FMath::Clamp(Health - DamageRate * MaxHealth, 0.f, MaxHealth);

	if (IsLocallyControlled())
	{
		UpdateHUDHealth(); // 更新本地HUD血量
		PlayOuchSound(DamageRate);
	}
	if (HasAuthority())
	{
		MulticastPlayOuchSound(DamageRate); // 需要Multicast通知模拟角色
		if (Health == 0.f) Kill();
	}
}

void AHumanCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (Combat == nullptr) return;
	if (Combat->GetCurrentShotEquipment())
	{
		Combat->SetAiming(true);
	}
	else if (Combat->CurrentEquipmentType == EEquipmentType::Melee)
	{
		Combat->MeleeAttack(1);
	}
}

void AHumanCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentShotEquipment())
	{
		Combat->SetAiming(false);
	}
}

void AHumanCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (Combat == nullptr) return;
	if (Combat->GetCurrentShotEquipment())
	{
		Combat->FireHandle(true);
	}
	else if (Combat->GetCurrentMeleeEquipment())
	{
		Combat->MeleeAttack(0);
	}
	else if (Combat->GetCurrentThrowingEquipment())
	{
		Combat->Throw();
	}
}

void AHumanCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentShotEquipment())
	{
		Combat->FireHandle(false);
	}
}

void AHumanCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentShotEquipment())
	{
		Combat->Reload();
	}
}

void AHumanCharacter::DropButtonPressed(const FInputActionValue& Value)
{
	if (Combat && Combat->GetCurrentShotEquipment())
	{
		Combat->DropEquipment();
		if (Combat->GetLastEquipment())
		{
			Combat->SwapEquipment(Combat->LastEquipmentType);
		}
		else
		{
			Combat->SwapEquipment(EEquipmentType::Melee);
		}
		DetectOverlappingEquipment(); // 检测是否有武器重叠
	}
}

void AHumanCharacter::DetectOverlappingEquipment()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AEquipment::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		AEquipment* Equipment = Cast<AEquipment>(OverlappingActor);
		if (Equipment)
		{
			EquipOverlappingEquipment(Equipment);
			break;
		}
	}
}

void AHumanCharacter::EquipOverlappingEquipment(AEquipment* Equipment)
{
	if (bIsKilled || Equipment->GetOwner()) return;
	if (Combat && Combat->HasEquippedThisTypeEquipment(Equipment->GetEquipmentType())) return;
	if (Combat)
	{
		Combat->EquipEquipment(Equipment);
	}
}

void AHumanCharacter::SwapPrimaryEquipmentButtonPressed()
{
	if (Combat && Combat->CurrentEquipmentType != EEquipmentType::Primary && Combat->PrimaryEquipment)
	{
		Combat->SwapEquipment(EEquipmentType::Primary);
	}
}

void AHumanCharacter::SwapSecondaryEquipmentButtonPressed()
{
	if (Combat && Combat->CurrentEquipmentType != EEquipmentType::Secondary && Combat->SecondaryEquipment)
	{
		Combat->SwapEquipment(EEquipmentType::Secondary);
	}
}

void AHumanCharacter::SwapMeleeEquipmentButtonPressed()
{
	if (Combat && Combat->CurrentEquipmentType != EEquipmentType::Melee && Combat->MeleeEquipment)
	{
		Combat->SwapEquipment(EEquipmentType::Melee);
	}
}

void AHumanCharacter::SwapThrowingEquipmentButtonPressed()
{
	if (Combat && Combat->CurrentEquipmentType != EEquipmentType::Throwing && Combat->ThrowingEquipment)
	{
		Combat->SwapEquipment(EEquipmentType::Throwing);
	}
}

void AHumanCharacter::SwapLastEquipmentButtonPressed(const FInputActionValue& Value)
{
	if (Combat)
	{
		switch (Combat->LastEquipmentType)
		{
		case EEquipmentType::Primary:
			if (Combat->PrimaryEquipment) Combat->SwapEquipment(EEquipmentType::Primary);
			break;
		case EEquipmentType::Secondary:
			if (Combat->SecondaryEquipment) Combat->SwapEquipment(EEquipmentType::Secondary);
			break;
		case EEquipmentType::Melee:
			if (Combat->MeleeEquipment) Combat->SwapEquipment(EEquipmentType::Melee);
			break;
		case EEquipmentType::Throwing:
			if (Combat->ThrowingEquipment) Combat->SwapEquipment(EEquipmentType::Throwing);
			break;
		}
	}
}

void AHumanCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	if (bIsKilled) return;

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	if (IsLocallyControlled()) UpdateHUDHealth();

	if (Health == 0.f)
	{
		if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
		if (TeamDeadMatchMode)
		{
			if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
			AHumanController* AttackerController = Cast<AHumanController>(InstigatorController);
			TeamDeadMatchMode->KillPlayer(this, HumanController, AttackerController, DamageCauser);
		}
	}
}

void AHumanCharacter::OnRep_Health()
{
	if (IsLocallyControlled()) UpdateHUDHealth();
}

void AHumanCharacter::UpdateHUDHealth()
{
	if (HumanController == nullptr) HumanController = Cast<AHumanController>(Controller);
	if (HumanController)
	{
		HumanController->SetHUDHealth(Health);
	}
}

void AHumanCharacter::Kill()
{
	if (Combat && Combat->GetCurrentEquipment())
	{
		Combat->MulticastDropEquipment2();
		Combat->MulticastDestroyEquipment();
	}
	MulticastKill();
}

void AHumanCharacter::MulticastKill_Implementation()
{
	bIsKilled = true;

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if (HumanController) DisableInput(HumanController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(KillTimer, this, &ThisClass::KillTimerFinished, KillDelay);
}

void AHumanCharacter::KillTimerFinished()
{
	if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
	if (TeamDeadMatchMode)
	{
		TeamDeadMatchMode->Respawn(this, Controller);
	}
}

ECombatState AHumanCharacter::GetCombatState() const
{
	return Combat->CombatState;
}

bool AHumanCharacter::IsAiming()
{
	return Combat && Combat->bIsAiming;
}

AEquipment* AHumanCharacter::GetCurrentEquipment()
{
	if (Combat == nullptr) return nullptr;
	return Combat->GetCurrentEquipment();
}

FVector AHumanCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}
