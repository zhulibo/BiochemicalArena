#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "BiochemicalArena/Weapons/Cassings/Casing.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Camera/CameraComponent.h"
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
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	MaxCarriedAmmo = CarriedAmmo;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
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
	SpendRound();
}

void AWeapon::DropWeapon()
{
	SetWeaponState(EWeaponState::Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	// 丢弃武器时，给予一个向前的冲量
	AHumanCharacter* OwnerCharacter = Cast<AHumanCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		UCameraComponent* CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			float ImpulsePerKg =  300.f;
			if (OwnerCharacter->IsElimmed()) ImpulsePerKg = ImpulsePerKg / 2;
			WeaponMesh->AddImpulse(CameraComponent->GetForwardVector() * ImpulsePerKg * WeaponMesh->GetMass());
		}
	}
	SetOwner(nullptr);
	Character = nullptr;
	Controller = nullptr;
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::Equipped:
		OnEquipped();
		break;
	case EWeaponState::Dropped:
		OnDropped();
		break;
	}
}

void AWeapon::OnEquipped()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnDropped()
{
	/**
	 * HACK 延迟开启AreaSphere碰撞
	 * 1 避免DropWeapon未执行完，发生OnSphereOverlap > EquipWeapon
	 * 2 确保武器已被丢出该角色Overlap区域
	 * 2 角色死亡时，等待bElimmed属性本地已设置，因为伤害只在服务端计算，需等待Elim() > MulticastElim()
	 */
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeapon::SetAreaSphereCollision, .4f);

	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AWeapon::SetAreaSphereCollision()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled())
	{
		SetHUDAmmo();
	}
}

void AWeapon::SetAmmo(int32 AmmoNum)
{
	Ammo = AmmoNum;
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled()) SetHUDAmmo();
}

void AWeapon::SetCarriedAmmo(int32 AmmoNum)
{
	CarriedAmmo = AmmoNum;
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character && Character->IsLocallyControlled()) SetHUDCarriedAmmo();
}

void AWeapon::SetHUDAmmo()
{
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller) Controller->SetHUDAmmo(Ammo);
	}
}

void AWeapon::SetHUDCarriedAmmo()
{
	if (Character == nullptr) Character = Cast<AHumanCharacter>(GetOwner());
	if (Character)
	{
		if (Controller == nullptr) Controller = Cast<AHumanController>(Character->Controller);
		if (Controller) Controller->SetHUDCarriedAmmo(CarriedAmmo);
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
	if (HumanCharacter && HumanCharacter->IsLocallyControlled())
	{
		HumanCharacter->EquipOverlappingWeapon(this);
	}
}
