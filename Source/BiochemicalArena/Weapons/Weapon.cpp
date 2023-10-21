#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BiochemicalArena/Weapons/Cassings/Casing.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 服务端统一检测OnSphereOverlap事件
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	int32 AmmoNum = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetAmmo(AmmoNum);
}

void AWeapon::DropWeapon()
{
	SetWeaponState(EWeaponState::EwsDropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	// 丢弃武器时，给予一个向前的冲量
	AHumanCharacter* OwnerCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UCameraComponent* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			float ImpulseNum =  1200.f;
			if (OwnerCharacter->IsElimmed())
			{
				ImpulseNum = 600.f;
			}
			WeaponMesh->AddImpulse(CameraComponent->GetForwardVector() * ImpulseNum);
		}
	}
	SetOwner(nullptr);
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (Owner == nullptr)
	{
		Character = nullptr;
		Controller = nullptr;
	}
	else
	{
		SetAmmo(Ammo);
		SetCarriedAmmo(CarriedAmmo);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EwsEquipped:
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EwsDropped:
		if (HasAuthority())
		{
			/**
			 * HACK 延迟开启AreaSphere碰撞
			 * 避免DropWeapon未执行完，发生OnSphereOverlap > EquipWeapon
			 * 同时确保武器已离开角色Overlap区域
			 */
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeapon::SetAreaSphereCollision, .4f);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeapon::SetAreaSphereCollision()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EwsEquipped:
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EwsDropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AWeapon::SetAmmo(int32 AmmoNum)
{
	Ammo = AmmoNum;
	SetHUDAmmo(); // if local is server, OnRep_Ammo will not be called, so call it here
}

void AWeapon::SetCarriedAmmo(int32 AmmoNum)
{
	CarriedAmmo = AmmoNum;
	SetHUDCarriedAmmo(); // if local is server, OnRep_CarriedAmmo will not be called, so call it here
}

void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();

	if (WeaponType == EWeaponType::EWT_Shotgun && IsFull() && Character && Character->GetCombat())
	{
		Character->GetCombat()->JumpToShotgunEnd();
	}
}

void AWeapon::OnRep_CarriedAmmo()
{
	SetHUDCarriedAmmo();

	if (WeaponType == EWeaponType::EWT_Shotgun && CarriedAmmo == 0 && Character && Character->GetCombat())
	{
		Character->GetCombat()->JumpToShotgunEnd();
	}
}

void AWeapon::SetHUDAmmo()
{
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller)
		{
			Controller->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SetHUDCarriedAmmo()
{
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller)
		{
			Controller->SetHUDCarriedAmmo(CarriedAmmo);
		}
	}
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(OtherActor);
	if (HumanCharacter)
	{
		HumanCharacter->SetOverlappingWeapon(this);
	}
}
