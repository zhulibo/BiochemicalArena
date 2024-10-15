#include "HumanCharacter.h"

#include "DataRegistryId.h"
#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "..\Equipments\Throwing.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "Components/CombatComponent.h"
#include "BiochemicalArena/Equipments/Melee.h"
#include "BiochemicalArena/Equipments/Weapon.h"
#include "BiochemicalArena/System/Storage/ConfigType.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Equipments/Pickups/PickupEquipment.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/GameModes/TeamDeadMatchMode.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatStateType.h"
#include "Components/CrosshairComponent.h"
#include "Components/RecoilComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/InputBase.h"
#include "Data/InputHuman.h"
#include "Net/UnrealNetwork.h"

AHumanCharacter::AHumanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	RecoilComponent = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));

	CrosshairComponent = CreateDefaultSubobject<UCrosshairComponent>(TEXT("CrosshairComponent"));

	BloodColor = C_RED;
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

	if (InputHuman == nullptr) return;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputHuman->HumanMappingContext, 1);
		}
	}

	// Set up action bindings
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(InputHuman->AimAction, ETriggerEvent::Started, this, &ThisClass::AimButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->AimAction, ETriggerEvent::Completed, this, &ThisClass::AimButtonReleased);
		EnhancedInputComponent->BindAction(InputHuman->FireAction, ETriggerEvent::Started, this, &ThisClass::FireButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->FireAction, ETriggerEvent::Completed, this, &ThisClass::FireButtonReleased);
		EnhancedInputComponent->BindAction(InputHuman->ReloadAction, ETriggerEvent::Triggered, this, &ThisClass::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->DropAction, ETriggerEvent::Triggered, this, &ThisClass::DropButtonPressed);

		EnhancedInputComponent->BindAction(InputHuman->SwapPrimaryEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapPrimaryEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->SwapSecondaryEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapSecondaryEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->SwapMeleeEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapMeleeEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->SwapThrowingEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapThrowingEquipmentButtonPressed);
		EnhancedInputComponent->BindAction(InputHuman->SwapLastEquipmentAction, ETriggerEvent::Triggered, this, &ThisClass::SwapLastEquipmentButtonPressed);
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
			if (InputBase) Subsystem->RemoveMappingContext(InputBase->BaseMappingContext);
			if (InputHuman) Subsystem->RemoveMappingContext(InputHuman->HumanMappingContext);
		}
	}
}

void AHumanCharacter::OnControllerReady()
{
	Super::OnControllerReady();

	int32 CurLoadoutIndex = 0;
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		int32 TempCurLoadoutIndex = StorageSubsystem->CacheSetting->CurLoadoutIndex;
		if (TempCurLoadoutIndex > 0 && TempCurLoadoutIndex < StorageSubsystem->CacheSetting->Bags.Num())
		{
			CurLoadoutIndex = TempCurLoadoutIndex;
		}
	}

	// 获取本地背包里的装备的名字
	FString PrimaryName = GetEquipmentName(CurLoadoutIndex, EEquipmentType::Primary);
	FString SecondaryName = GetEquipmentName(CurLoadoutIndex, EEquipmentType::Secondary);
	FString MeleeName = GetEquipmentName(CurLoadoutIndex, EEquipmentType::Melee);
	FString ThrowingName = GetEquipmentName(CurLoadoutIndex, EEquipmentType::Throwing);

	// 测试用
	// PrimaryName = "M870";

	// 本地Controller就绪后，在服务端生成武器，然后复制到所有客户端
	ServerSetDefaultEquipment(PrimaryName, SecondaryName, MeleeName, ThrowingName);
}

void AHumanCharacter::ServerSetDefaultEquipment_Implementation(const FString& PrimaryName,
	const FString& SecondaryName, const FString& MeleeName, const FString& ThrowingName)
{
	if (CombatComponent == nullptr) return;

	TObjectPtr<UClass> PrimaryClass;
	TObjectPtr<UClass> SecondaryClass;
	TObjectPtr<UClass> MeleeClass;
	TObjectPtr<UClass> ThrowingClass;

	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		{
			FDataRegistryId DataRegistryId(DR_EquipmentMain, FName(PrimaryName));
			if (const FEquipmentMain* EquipmentMain = DRSubsystem->GetCachedItem<FEquipmentMain>(DataRegistryId))
			{
				PrimaryClass = EquipmentMain->EquipmentClass;
			}
		}
		{
			FDataRegistryId DataRegistryId(DR_EquipmentMain, FName(SecondaryName));
			if (const FEquipmentMain* EquipmentMain = DRSubsystem->GetCachedItem<FEquipmentMain>(DataRegistryId))
			{
				SecondaryClass = EquipmentMain->EquipmentClass;
			}
		}
		{
			FDataRegistryId DataRegistryId(DR_EquipmentMain, FName(MeleeName));
			if (const FEquipmentMain* EquipmentMain = DRSubsystem->GetCachedItem<FEquipmentMain>(DataRegistryId))
			{
				MeleeClass = EquipmentMain->EquipmentClass;
			}
		}
		{
			FDataRegistryId DataRegistryId(DR_EquipmentMain, FName(ThrowingName));
			if (const FEquipmentMain* EquipmentMain = DRSubsystem->GetCachedItem<FEquipmentMain>(DataRegistryId))
			{
				ThrowingClass = EquipmentMain->EquipmentClass;
			}
		}
	}

	if (PrimaryClass)
	{
		DefaultPrimaryEquipment = GetWorld()->SpawnActor<AWeapon>(PrimaryClass);
		CombatComponent->LocalEquipEquipment(DefaultPrimaryEquipment);
		CombatComponent->LocalSwapEquipment(EEquipmentType::Primary);
	}
	if (SecondaryClass)
	{
		DefaultSecondaryEquipment = GetWorld()->SpawnActor<AWeapon>(SecondaryClass);
		CombatComponent->LocalEquipEquipment(DefaultSecondaryEquipment);
	}
	if (MeleeClass)
	{
		DefaultMeleeEquipment = GetWorld()->SpawnActor<AMelee>(MeleeClass);
		CombatComponent->LocalEquipEquipment(DefaultMeleeEquipment);
	}
	if (ThrowingClass)
	{
		DefaultThrowingEquipment = GetWorld()->SpawnActor<AThrowing>(ThrowingClass);
		CombatComponent->LocalEquipEquipment(DefaultThrowingEquipment);
	}
}

// 获取装备的名字
FString AHumanCharacter::GetEquipmentName(int32 CurLoadoutIndex, EEquipmentType EquipmentType)
{
	FString EquipmentName = "";

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem == nullptr || StorageSubsystem->CacheSetting->Bags.IsEmpty())
	{
		return EquipmentName;
	}

	switch (EquipmentType)
	{
	case EEquipmentType::Primary:
		EquipmentName = StorageSubsystem->CacheSetting->Bags[CurLoadoutIndex].Primary;
		break;
	case EEquipmentType::Secondary:
		EquipmentName = StorageSubsystem->CacheSetting->Bags[CurLoadoutIndex].Secondary;
		break;
	case EEquipmentType::Melee:
		EquipmentName = StorageSubsystem->CacheSetting->Bags[CurLoadoutIndex].Melee;
		break;
	case EEquipmentType::Throwing:
		EquipmentName = StorageSubsystem->CacheSetting->Bags[CurLoadoutIndex].Throwing;
		break;
	}

	return EquipmentName;
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
		CombatComponent->MeleeAttack(ECombatState::HeavyAttacking);
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
		CombatComponent->StartFire();
	}
	else if (CombatComponent->GetCurMeleeEquipment())
	{
		CombatComponent->MeleeAttack(ECombatState::LightAttacking);
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
		CombatComponent->StopFire();
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
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

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
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
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
