#include "HumanCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "..\Equipments\Throwing.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Melee.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "..\System\StorageSaveGameType.h"
#include "..\System\StorageSaveGame.h"
#include "BiochemicalArena/Equipments/Pickups/PickupEquipment.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/CrosshairComponent.h"
#include "Components/RecoilComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AHumanCharacter::AHumanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	RecoilComponent = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));

	CrosshairComponent = CreateDefaultSubobject<UCrosshairComponent>(TEXT("CrosshairComponent"));

	MaxHealth = 300.f;
	Health = MaxHealth;
}

void AHumanCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DefaultPrimaryEquipment);
	DOREPLIFETIME(ThisClass, DefaultSecondaryEquipment);
	DOREPLIFETIME(ThisClass, DefaultMeleeEquipment);
	DOREPLIFETIME(ThisClass, DefaultThrowingEquipment);
}

void AHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent && CombatComponent->HumanCharacter == nullptr)
	{
		CombatComponent->HumanCharacter = this;
	}
	if (RecoilComponent && RecoilComponent->HumanCharacter == nullptr)
	{
		RecoilComponent->HumanCharacter = this;
	}
	if (CrosshairComponent && CrosshairComponent->HumanCharacter == nullptr)
	{
		CrosshairComponent->HumanCharacter = this;
	}
}

void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddUniqueDynamic(this, &ThisClass::HumanReceiveDamage);
	}
}

void AHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(HumanMappingContext, 1);
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
}

void AHumanCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BaseController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->RemoveMappingContext(HumanMappingContext);
		}
	}
}

void AHumanCharacter::OnLocallyControllerReady()
{
	Super::OnLocallyControllerReady();

	// 获取本地背包里的装备的名字
	FString PrimaryEquipmentName = GetEquipmentName(0, EEquipmentType::Primary);
	FString SecondaryEquipmentName = GetEquipmentName(0, EEquipmentType::Secondary);
	FString MeleeEquipmentName = GetEquipmentName(0, EEquipmentType::Melee);
	FString ThrowingEquipmentName = GetEquipmentName(0, EEquipmentType::Throwing);

	// 测试用
	// PrimaryEquipmentName = "M870";

	// 本地Controller就绪后，在服务端生成武器，然后复制到所有客户端
	ServerSetDefaultEquipment(PrimaryEquipmentName, SecondaryEquipmentName, MeleeEquipmentName, ThrowingEquipmentName);
}

void AHumanCharacter::ServerSetDefaultEquipment_Implementation(const FString& PrimaryEquipmentName,
	const FString& SecondaryEquipmentName, const FString& MeleeEquipmentName, const FString& ThrowingEquipmentName)
{
	if (CombatComponent == nullptr) return;

	// 加载装备类
	UClass* PrimaryEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(PrimaryEquipmentName));
	UClass* SecondaryEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(SecondaryEquipmentName));
	UClass* MeleeEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(MeleeEquipmentName));
	UClass* ThrowingEquipmentClass = StaticLoadClass(UObject::StaticClass(), nullptr, *GetEquipmentClassPath(ThrowingEquipmentName));

	if (PrimaryEquipmentClass)
	{
		DefaultPrimaryEquipment = GetWorld()->SpawnActor<AWeapon>(PrimaryEquipmentClass);
		CombatComponent->LocalEquipEquipment(DefaultPrimaryEquipment);
		CombatComponent->LocalSwapEquipment(EEquipmentType::Primary);
	}
	if (SecondaryEquipmentClass)
	{
		DefaultSecondaryEquipment = GetWorld()->SpawnActor<AWeapon>(SecondaryEquipmentClass);
		CombatComponent->LocalEquipEquipment(DefaultSecondaryEquipment);
	}
	if (MeleeEquipmentClass)
	{
		DefaultMeleeEquipment = GetWorld()->SpawnActor<AMelee>(MeleeEquipmentClass);
		CombatComponent->LocalEquipEquipment(DefaultMeleeEquipment);
	}
	if (ThrowingEquipmentClass)
	{
		DefaultThrowingEquipment = GetWorld()->SpawnActor<AThrowing>(ThrowingEquipmentClass);
		CombatComponent->LocalEquipEquipment(DefaultThrowingEquipment);
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
	if (CombatComponent)
	{
		CombatComponent->LocalEquipEquipment(DefaultPrimaryEquipment);
		CombatComponent->LocalSwapEquipment(EEquipmentType::Primary);
	}
}

void AHumanCharacter::OnRep_DefaultSecondaryEquipment()
{
	if (CombatComponent)
	{
		CombatComponent->LocalEquipEquipment(DefaultSecondaryEquipment);
	}
}

void AHumanCharacter::OnRep_DefaultMeleeEquipment()
{
	if (CombatComponent)
	{
		CombatComponent->LocalEquipEquipment(DefaultMeleeEquipment);
	}
}

void AHumanCharacter::OnRep_DefaultThrowingEquipment()
{
	if (CombatComponent)
	{
		CombatComponent->LocalEquipEquipment(DefaultThrowingEquipment);
	}
}

void AHumanCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (CombatComponent == nullptr) return;

	if (CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->SetAiming(true);
	}
	else if (CombatComponent->CurrentEquipmentType == EEquipmentType::Melee)
	{
		CombatComponent->MeleeAttack(1); // TODO CombatState == ECombatState::Ready
	}
}

void AHumanCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (CombatComponent && CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->SetAiming(false);
	}
}

void AHumanCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	if (CombatComponent == nullptr) return;

	if (CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->FireHandle(true);
	}
	else if (CombatComponent->GetCurMeleeEquipment())
	{
		CombatComponent->MeleeAttack(0); // TODO CombatState == ECombatState::Ready
	}
	else if (CombatComponent->GetCurThrowingEquipment())
	{
		CombatComponent->Throw();
	}
}

void AHumanCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	if (CombatComponent && CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->FireHandle(false);
	}
}

void AHumanCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	if (CombatComponent && CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->Reload();
	}
}

void AHumanCharacter::DropButtonPressed(const FInputActionValue& Value)
{
	// 只有主副武器可以丢弃
	if (CombatComponent && CombatComponent->GetCurShotEquipment())
	{
		CombatComponent->DropEquipment(CombatComponent->CurrentEquipmentType);
		if (CombatComponent->GetLastEquipment())
		{
			CombatComponent->SwapEquipment(CombatComponent->LastEquipmentType);
		}
		else
		{
			CombatComponent->SwapEquipment(EEquipmentType::Melee);
		}

		ServerDetectOverlappingEquipment();
	}
}

// 检测重叠装备
void AHumanCharacter::ServerDetectOverlappingEquipment_Implementation()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, AEquipment::StaticClass());
	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (AEquipment* Equipment = Cast<AEquipment>(OverlappingActor))
		{
			EquipOverlappingEquipment(Equipment);
			break;
		}
	}
}

// 装备检测到的重叠装备
void AHumanCharacter::EquipOverlappingEquipment(AEquipment* Equipment)
{
	if (bIsDead || Equipment == nullptr || Equipment->GetOwner()) return;
	if (CombatComponent && CombatComponent->HasEquippedThisTypeEquipment(Equipment->GetEquipmentType())) return;
	if (CombatComponent)
	{
		CombatComponent->MulticastEquipEquipment2(Equipment);
	}
}

// 给予补给箱装备
void AHumanCharacter::ServerGivePickupEquipment_Implementation(APickupEquipment* PickupEquipment)
{
	if (bIsDead || CombatComponent == nullptr || PickupEquipment == nullptr) return;

	AEquipment* Equipment = PickupEquipment->Equipment;
	if (Equipment == nullptr) return;

	// 丢弃旧装备
	CombatComponent->MulticastDropEquipment2(Equipment->GetEquipmentType());

	// 装备类型使用中，替换
	if (CombatComponent->CurrentEquipmentType == Equipment->GetEquipmentType())
	{
		CombatComponent->MulticastReplaceCurEquipment(Equipment);
	}
	// 装备类型非使用中，装备
	else
	{
		CombatComponent->MulticastEquipEquipment2(Equipment);
	}

	PickupEquipment->Destroy();
}

void AHumanCharacter::SwapPrimaryEquipmentButtonPressed()
{
	if (CombatComponent) CombatComponent->SwapEquipment(EEquipmentType::Primary);
}

void AHumanCharacter::SwapSecondaryEquipmentButtonPressed()
{
	if (CombatComponent) CombatComponent->SwapEquipment(EEquipmentType::Secondary);
}

void AHumanCharacter::SwapMeleeEquipmentButtonPressed()
{
	if (CombatComponent) CombatComponent->SwapEquipment(EEquipmentType::Melee);
}

void AHumanCharacter::SwapThrowingEquipmentButtonPressed()
{
	if (CombatComponent) CombatComponent->SwapEquipment(EEquipmentType::Throwing);
}

void AHumanCharacter::SwapLastEquipmentButtonPressed(const FInputActionValue& Value)
{
	if (CombatComponent) CombatComponent->SwapEquipment(CombatComponent->LastEquipmentType);
}

void AHumanCharacter::HumanReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* AttackerController, AActor* DamageCauser)
{
	if (bIsDead) return;

	if (BaseMode == nullptr) BaseMode = GetWorld()->GetAuthGameMode<ABaseMode>();
	if (BaseMode)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
		if (BaseController)
		{
			BaseMode->HumanReceiveDamage(this, BaseController, Damage, DamageType, AttackerController, DamageCauser);
		}
	}
}

void AHumanCharacter::MulticastTeamDeadMatchDead_Implementation()
{
	bIsDead = true;

	if (CombatComponent)
	{
		CombatComponent->LocalDropEquipment(EEquipmentType::Primary);
		CombatComponent->DestroyEquipments();
	}

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController) DisableInput(BaseController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 重生
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			if (TeamDeadMatchMode == nullptr) TeamDeadMatchMode = GetWorld()->GetAuthGameMode<ATeamDeadMatchMode>();
			if (TeamDeadMatchMode)
			{
				TeamDeadMatchMode->HumanRespawn(this, Controller);
			}
		});
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 3.f, false);
	}
}

void AHumanCharacter::MulticastMutationDead_Implementation()
{
	bIsDead = true;

	if (CombatComponent)
	{
		CombatComponent->LocalDropEquipment(EEquipmentType::Primary);
		CombatComponent->DestroyEquipments();
	}

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController) DisableInput(BaseController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHumanCharacter::GetInfect(AMutantCharacter* AttackerCharacter, ABaseController* AttackerController, EMutantState MutantState)
{
	if (bIsDead || bIsImmune) return;

	if (MutationMode == nullptr) MutationMode = GetWorld()->GetAuthGameMode<AMutationMode>();
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (MutationMode && BaseController)
	{
		MutationMode->GetInfect(this, BaseController, AttackerCharacter, AttackerController, MutantState);
	}
}

AEquipment* AHumanCharacter::GetCurrentEquipment()
{
	if (CombatComponent) return CombatComponent->GetCurEquipment();
	return nullptr;
}

FVector AHumanCharacter::GetHitTarget() const
{
	if (CombatComponent) return CombatComponent->HitTarget;
	return FVector();
}
