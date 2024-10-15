#include "MutantCharacter.h"

#include "DataRegistrySubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HumanCharacter.h"
#include "MutantAnimInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "BiochemicalArena/Abilities/AttributeSetBase.h"
#include "BiochemicalArena/Abilities/BAAbilitySystemComponent.h"
#include "BiochemicalArena/Abilities/GameplayAbilityBase.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Equipments/Data/DamageTypeMutant.h"
#include "BiochemicalArena/GameModes/BaseMode.h"
#include "BiochemicalArena/GameModes/MutationMode.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/InputBase.h"
#include "Data/InputMutant.h"
#include "Data/MutantCommon.h"
#include "Kismet/GameplayStatics.h"

AMutantCharacter::AMutantCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	BloodColor = C_GREEN;

	MutantState = EMutantState::Ready;

	RightHandCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightHandCapsule"));
	RightHandCapsule->SetupAttachment(GetMesh(), "RightHandCapsule");
	RightHandCapsule->SetGenerateOverlapEvents(true);
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCapsule->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
	RightHandCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnRightHandCapsuleOverlap);

	LeftHandCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftHandCapsule"));
	LeftHandCapsule->SetupAttachment(GetMesh(), "LeftHandCapsule");
	LeftHandCapsule->SetGenerateOverlapEvents(true);
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCapsule->SetCollisionResponseToChannel(ECC_Team1SkeletalMesh, ECollisionResponse::ECR_Overlap);
	LeftHandCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnLeftHandCapsuleOverlap);
}

void AMutantCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMutantCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMutantCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		FString EnumValue = UEnum::GetValueAsString(MutantCharacterName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

		FDataRegistryId DataRegistryId(DR_MutantCharacterMain, FName(EnumValue));
		if (const FMutantCharacterMain* MutantCharacterMain = DRSubsystem->GetCachedItem<FMutantCharacterMain>(DataRegistryId))
		{
			LightAttackDamage = MutantCharacterMain->LightAttackDamage;
			HeavyAttackDamage = MutantCharacterMain->HeavyAttackDamage;
		}
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddUniqueDynamic(this, &ThisClass::MutantReceiveDamage);
	}
}

void AMutantCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputMutant == nullptr) return;

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMutant->MutantMappingContext, 1);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(InputMutant->LightAttackAction, ETriggerEvent::Started, this, &ThisClass::LightAttackButtonPressed);
		EnhancedInputComponent->BindAction(InputMutant->LightAttackAction, ETriggerEvent::Completed, this, &ThisClass::LightAttackButtonReleased);
		EnhancedInputComponent->BindAction(InputMutant->HeavyAttackAction, ETriggerEvent::Started, this, &ThisClass::HeavyAttackButtonPressed);
		EnhancedInputComponent->BindAction(InputMutant->HeavyAttackAction, ETriggerEvent::Completed, this, &ThisClass::HeavyAttackButtonReleased);
		EnhancedInputComponent->BindAction(InputMutant->SkillAction, ETriggerEvent::Triggered, this, &ThisClass::SkillButtonPressed);
	}
}

void AMutantCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 调试用 自动行走
	// if (IsLocallyControlled())
	// {
	// 	AddMovementInput(GetActorForwardVector(), 1);
	// }
}

void AMutantCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(SkillAbility));

		if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
		if (AssetSubsystem && AssetSubsystem->MutantCommon)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AssetSubsystem->MutantCommon->ChangeMutantAbility));

			if (bSpawnByInfectOrChosen)
			{
				AbilitySystemComponent->TryActivateAbilityByClass(AssetSubsystem->MutantCommon->ChangeMutantAbility);
			}
		}
	}
}

void AMutantCharacter::OnAbilitySystemComponentInit()
{
	Super::OnAbilitySystemComponentInit();

	if (AbilitySystemComponent && AttributeSetBase && IsLocallyControlled())
	{
		FGameplayTag SkillCooldownTag = FGameplayTag::RequestGameplayTag(TAG_Mutant_SKILL_CD);
		AbilitySystemComponent->RegisterGameplayTagEvent(
			SkillCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnLocalSkillCooldownTagChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &ThisClass::OnLocalCharacterLevelChanged);
	}
}

// 更新skill UI
void AMutantCharacter::OnLocalSkillCooldownTagChanged(FGameplayTag GameplayTag, int32 TagCount)
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(Controller);
	if (MutationController)
	{
		MutationController->ShowHUDSkill(TagCount == 0 && GetCharacterLevel() >= 2.f);
	}
}

// 更新skill UI
void AMutantCharacter::OnLocalCharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(Controller);
	if (MutationController && AbilitySystemComponent)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TAG_Mutant_SKILL_CD);
		MutationController->ShowHUDSkill(AbilitySystemComponent->GetTagCount(Tag) == 0 && Data.NewValue > 2.f);
	}
}

// 处理skill功能
void AMutantCharacter::SkillButtonPressed(const FInputActionValue& Value)
{
	if (AbilitySystemComponent && GetCharacterLevel() >= 2.f)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(SkillAbility);
	}
}

void AMutantCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BaseController->GetLocalPlayer());
		if (Subsystem)
		{
			if (InputBase) Subsystem->RemoveMappingContext(InputBase->BaseMappingContext);
			if (InputMutant) Subsystem->RemoveMappingContext(InputMutant->MutantMappingContext);
		}
	}
}

void AMutantCharacter::Destroyed()
{
	if (AbilitySystemComponent)
	{
		if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(SkillAbility))
		{
			AbilitySystemComponent->CancelAbility(Spec->Ability);
		}
	}

	Super::Destroyed();
}

void AMutantCharacter::LightAttackButtonPressed(const FInputActionValue& Value)
{
	if (MutantState != EMutantState::Ready) return;

	LocalLightAttack();
	ServerLightAttack();
}

void AMutantCharacter::ServerLightAttack_Implementation()
{
	MulticastLightAttack();
}

void AMutantCharacter::MulticastLightAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		LocalLightAttack();
	}
}

void AMutantCharacter::LocalLightAttack()
{
	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantState = EMutantState::LightAttacking;
		MutantAnimInstance->Montage_Play(LightAttackMontage);

		// 动画结束后，重置状态
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			MutantState = EMutantState::Ready;
			RightHandHitEnemies.Empty();
			LeftHandHitEnemies.Empty();
		});
		MutantAnimInstance->Montage_SetEndDelegate(OnMontageEnded, LightAttackMontage);
	}
}

void AMutantCharacter::LightAttackButtonReleased(const FInputActionValue& Value)
{
}

void AMutantCharacter::HeavyAttackButtonPressed(const FInputActionValue& Value)
{
	if (MutantState != EMutantState::Ready) return;

	LocalHeavyAttack();
	ServerHeavyAttack();
}

void AMutantCharacter::ServerHeavyAttack_Implementation()
{
	MulticastHeavyAttack();
}

void AMutantCharacter::MulticastHeavyAttack_Implementation()
{
	if (!IsLocallyControlled())
	{
		LocalHeavyAttack();
	}
}

void AMutantCharacter::LocalHeavyAttack()
{
	if (MutantAnimInstance == nullptr) MutantAnimInstance = Cast<UMutantAnimInstance>(GetMesh()->GetAnimInstance());
	if (MutantAnimInstance)
	{
		MutantState = EMutantState::HeavyAttacking;
		MutantAnimInstance->Montage_Play(HeavyAttackMontage);

		// 动画结束后，重置状态
		FOnMontageEnded OnMontageEnded;
		OnMontageEnded.BindWeakLambda(this, [this](UAnimMontage* AnimMontage, bool bInterrupted)
		{
			MutantState = EMutantState::Ready;
			RightHandHitEnemies.Empty();
			LeftHandHitEnemies.Empty();
		});
		MutantAnimInstance->Montage_SetEndDelegate(OnMontageEnded, HeavyAttackMontage);
	}
}

void AMutantCharacter::HeavyAttackButtonReleased(const FInputActionValue& Value)
{
}

void AMutantCharacter::RightHandAttackBegin()
{
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMutantCharacter::RightHandAttackEnd()
{
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutantCharacter::LeftHandAttackBegin()
{
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMutantCharacter::LeftHandAttackEnd()
{
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// TODO 不在服务端视线内有时没有碰撞
void AMutantCharacter::OnRightHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (HasAuthority())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OnLeftHandCapsuleOverlap 3"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OnLeftHandCapsuleOverlap 2"));
	// }

	if (!RightHandHitEnemies.Contains(OtherActor))
	{
		RightHandHitEnemies.Add(OtherActor);

		float Damage = MutantState == EMutantState::LightAttacking ? LightAttackDamage : HeavyAttackDamage;

		DropBlood(OverlappedComponent, OtherActor, OtherComp, Damage);

		if (HasAuthority())
		{
			// 造成伤害
			UGameplayStatics::ApplyDamage(OtherActor, Damage, Controller, this, UDamageTypeMutant::StaticClass());

			// 造成感染
			AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
			if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
			if (HumanCharacter && BaseController)
			{
				HumanCharacter->GetInfect(this, BaseController, MutantState);
			}
		}
	}
}

void AMutantCharacter::OnLeftHandCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!LeftHandHitEnemies.Contains(OtherActor))
	{
		LeftHandHitEnemies.Add(OtherActor);

		float Damage = MutantState == EMutantState::LightAttacking ? LightAttackDamage : HeavyAttackDamage;

		DropBlood(OverlappedComponent, OtherActor, OtherComp, Damage);

		if (HasAuthority())
		{
			// 造成伤害
			UGameplayStatics::ApplyDamage(OtherActor, Damage, Controller, this, UDamageTypeMutant::StaticClass());

			// 造成感染
			AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
			if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
			if (HumanCharacter && BaseController)
			{
				HumanCharacter->GetInfect(this, BaseController, MutantState);
			}
		}
	}
}

void AMutantCharacter::DropBlood(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, float Damage)
{
	ABaseCharacter* OverlappedCharacter = Cast<ABaseCharacter>(OtherActor);

	if (OverlappedCharacter == nullptr || OverlappedCharacter->BloodEffect_Melee == nullptr) return;

	TArray<FHitResult> TraceResults;

	auto Start = OverlappedComponent->GetComponentLocation();
	auto End = OverlappedCharacter->GetActorLocation();

	FCollisionQueryParams CollisionQueryParams;

	GetWorld()->SweepMultiByObjectType(
		TraceResults,
		Start,
		End,
		FQuat::Identity,
		FCollisionObjectQueryParams(OverlappedCharacter->GetMesh()->GetCollisionObjectType()),
		FCollisionShape::MakeSphere(10.f),
		CollisionQueryParams
	);

	// DrawDebugLine(GetWorld(), Start, End, COLOR_MAIN, true);

	for (auto TraceResult : TraceResults)
	{
		if (TraceResult.GetComponent()->GetUniqueID() == OtherComp->GetUniqueID())
		{
			auto BloodEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				OverlappedCharacter->BloodEffect_Melee,
				TraceResult.ImpactPoint,
				TraceResult.ImpactNormal.Rotation()
			);

			BloodEffectComponent->SetVariableInt("ParticleCount", ULibraryCommon::GetBloodParticleCount(Damage));
			BloodEffectComponent->SetVariableLinearColor("Color", OverlappedCharacter->BloodColor);

			break;
		}
	}
}

void AMutantCharacter::MutantReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* AttackerController, AActor* DamageCauser)
{
	if (bIsDead) return;

	if (BaseMode == nullptr) BaseMode = GetWorld()->GetAuthGameMode<ABaseMode>();
	if (BaseMode)
	{
		if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
		if (BaseController)
		{
			BaseMode->MutantReceiveDamage(this, BaseController, Damage, DamageType, AttackerController, DamageCauser);
		}
	}
}

void AMutantCharacter::MulticastDead_Implementation()
{
	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController) DisableInput(BaseController);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 重生
	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			if (MutationMode == nullptr) MutationMode = GetWorld()->GetAuthGameMode<AMutationMode>();
			if (MutationMode)
			{
				MutationMode->MutantRespawn(this, Controller);
			}
		});
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 3.f, false);
	}
}

void AMutantCharacter::ServerSelectMutant_Implementation(EMutantCharacterName TempMutantCharacterName)
{
	// 在服务器上保存角色，生成角色时会用
	if (BasePlayerState == nullptr) BasePlayerState = GetPlayerState<ABasePlayerState>();
	if (BasePlayerState)
	{
		BasePlayerState->SetMutantCharacterName(TempMutantCharacterName);
	}

	if (MutationMode == nullptr) MutationMode = GetWorld()->GetAuthGameMode<AMutationMode>();
	if (MutationMode)
	{
		MutationMode->SelectMutant(this, Controller);
	}
}

void AMutantCharacter::MulticastRepel_Implementation(FVector ImpulseVector)
{
	TSharedPtr<FRootMotionSource_ConstantForce> RootMotionSource = MakeShared<FRootMotionSource_ConstantForce>();
	RootMotionSource->Force = ImpulseVector;
	RootMotionSource->Duration = 0.1f;
	RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Additive;
	GetCharacterMovement()->ApplyRootMotionSource(RootMotionSource);
}
