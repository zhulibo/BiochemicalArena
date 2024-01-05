#include "Equipment.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "Animation/AnimationAsset.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"
#include "BiochemicalArena/PlayerControllers/HumanController.h"
#include "Components/SkeletalMeshComponent.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AEquipment::AEquipment()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(EquipmentMesh);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquipmentMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	EquipmentMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAreaSphereOverlap);
}

void AEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerTeam);
}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

void AEquipment::OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHumanCharacter* OverlapHumanCharacter = Cast<AHumanCharacter>(OtherActor);
	if (OverlapHumanCharacter && OverlapHumanCharacter->IsLocallyControlled())
	{
		OverlapHumanCharacter->EquipOverlappingEquipment(this);
	}
}

void AEquipment::SetEquipmentState(EEquipmentState State)
{
	EquipmentState = State;

	switch (EquipmentState)
	{
	case EEquipmentState::Equipped:
		OnEquipped();
		break;
	case EEquipmentState::Dropped:
		OnDropped();
		break;
	}
}

void AEquipment::OnEquipped()
{
	if (DestroyEquipmentTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DestroyEquipmentTimerHandle); // 取消销毁定时器
	}

	EquipmentMesh->SetSimulatePhysics(false);
	EquipmentMesh->SetEnableGravity(false);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetOwnerTeam();

	if (HumanCharacter) HumanController = Cast<AHumanController>(HumanCharacter->GetController()); // 缓存HumanController
}

void AEquipment::SetOwnerTeam()
{
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		ABasePlayerState* PlayerState = Cast<ABasePlayerState>(HumanCharacter->GetPlayerState());
		if (PlayerState) OwnerTeam = PlayerState->GetTeam();
	}
}

void AEquipment::OnDropped()
{
	EquipmentMesh->SetSimulatePhysics(true);
	EquipmentMesh->SetEnableGravity(true);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	/**
	 * HACK 延迟开启AreaSphere碰撞
	 * 1 避免DropEquipment未执行完，当前角色与武器发生OnSphereOverlap > EquipEquipment
	 * 2 确保武器已被丢出当前角色的Overlap区域
	 * 3 角色死亡时，等待bIsKilled已从服务器同步到本地，因为伤害只在服务端计算，Kill() > MulticastKill()
	 */
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::SetAreaSphereCollision, .4f);

	// 丢弃一定时间后销毁
	GetWorldTimerManager().SetTimer(DestroyEquipmentTimerHandle, this, &ThisClass::DestroyEquipment, 10.f);
}

void AEquipment::SetAreaSphereCollision()
{
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEquipment::DropEquipment()
{
	SetEquipmentState(EEquipmentState::Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	EquipmentMesh->DetachFromComponent(DetachRules);
	// 丢弃武器时，给予一个向前的冲量
	if (HumanCharacter == nullptr) HumanCharacter = Cast<AHumanCharacter>(GetOwner());
	if (HumanCharacter)
	{
		UCameraComponent* CameraComponent = HumanCharacter->FindComponentByClass<UCameraComponent>();
		if (CameraComponent)
		{
			float ImpulsePerKg =  300.f;
			if (HumanCharacter->IsKilled())
			{
				ImpulsePerKg = ImpulsePerKg / 2;
			}
			EquipmentMesh->AddImpulse(CameraComponent->GetForwardVector() * ImpulsePerKg * EquipmentMesh->GetMass());
			// UE_LOG(LogTemp, Warning, TEXT("EquipmentMesh->GetMass(): %f"), EquipmentMesh->GetMass());
		}
	}
	SetOwner(nullptr);
	HumanCharacter = nullptr;
	HumanController = nullptr;
	OwnerTeam = ETeam::NoTeam;
}

void AEquipment::DestroyEquipment()
{
	Destroy();
}
