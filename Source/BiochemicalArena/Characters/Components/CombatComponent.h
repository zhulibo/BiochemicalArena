#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

enum class ECombatState : uint8;
enum class EEquipmentType : uint8;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BIOCHEMICALARENA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class AHumanCharacter;

	class AEquipment* GetCurrentEquipment(); // If has a equipment in use return it, or return nullptr
	class AWeapon* GetCurrentShotEquipment(); // If primary or secondary equipment is in use return it, or return nullptr
	class AMelee* GetCurrentMeleeEquipment(); // If MeleeEquipment is in use return it, or return nullptr
	class AThrowing* GetCurrentThrowingEquipment();

	void FinishSwapAttach(EEquipmentType EquipmentType);
	void FinishSwap();

	void ShellReload();
	void FinishReload();

	void EnableMeshCollision(bool bIsEnabled);

	void ThrowOut();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	AHumanCharacter* Character;
	UPROPERTY()
	class AHumanController* Controller;
	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY(Replicated)
	ECombatState CombatState;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed = 450.f;

	FVector HitTarget;
	void TraceUnderCrosshair(FHitResult& TraceHitResult); // 检测准星对应游戏世界的物体

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	void SetHUDCrosshair(float DeltaSeconds); // TODO 使用CrosshairSpread实现动态准心

	float DefaultFOV;
	float CurrentFOV;
	void InterpFOV(float DeltaSeconds);

	UPROPERTY(Replicated)
	AWeapon* PrimaryEquipment;
	UPROPERTY(Replicated)
	AWeapon* SecondaryEquipment;
	UPROPERTY(Replicated)
	AMelee* MeleeEquipment;
	UPROPERTY(Replicated)
	AThrowing* ThrowingEquipment;
	UPROPERTY(Replicated)
	EEquipmentType CurrentEquipmentType;
	UPROPERTY(Replicated)
	EEquipmentType LastEquipmentType;

	AEquipment* GetLastEquipment();
	AEquipment* GetEquipmentByType(EEquipmentType EquipmentType);
	bool HasEquippedThisTypeEquipment(EEquipmentType EquipmentType);

	// 装备
	void EquipEquipment(AEquipment* Equipment);
	UFUNCTION(Server, Reliable)
	void ServerEquipEquipment(AEquipment* Equipment);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipEquipment(AEquipment* Equipment);
	void LocalEquipEquipment(AEquipment* Equipment);
	void AttachEquipmentToBodySocket(AEquipment* Equipment);
	UPROPERTY()
	USoundCue* EquipSound;

	// 切换
	void SwapEquipment(EEquipmentType EquipmentType);
	UFUNCTION(Server, Reliable)
	void ServerSwapEquipment(EEquipmentType EquipmentType);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwapEquipment(EEquipmentType EquipmentType);
	void LocalSwapEquipment(EEquipmentType EquipmentType);
	void PlaySwapMontage(AEquipment* EquipmentType);

	// 使用
	void UseEquipment(AEquipment* EquipmentType);
	void AttachEquipmentToRightHand(AEquipment* EquipmentType);

	// 瞄准
	UPROPERTY()
	bool bIsAiming = false;
	void SetAiming(bool bNewAimingState);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bNewAimingState);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetAiming(bool bNewAimingState);
	void LocalSetAiming(bool bNewAimingState);

	// 射击
	bool bCanFire = true;
	bool bFireButtonPressed;
	FTimerHandle FireTimer;
	UPROPERTY()
	USoundCue* ClickSound; // 无子弹时扣动扳机的声音
	void FireHandle(bool bPressed);
	void Fire();
	bool CanFire();
	void LoadNewBulletFinished();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void PlayFireMontage();

	// 装弹
	void Reload();
	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastReload();
	void LocalReload();
	void PlayReloadMontage();
	void JumpToShotgunEnd();

	// 丢弃
	void DropEquipment();
	UFUNCTION(Server, Reliable)
	void ServerDropEquipment();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDropEquipment();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDropEquipment2();
	void LocalDropEquipment();

	// 销毁
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyEquipment();

	// 近战攻击
	void MeleeAttack(int32 Type);
	UFUNCTION(Server, Reliable)
	void ServerMeleeAttack(int32 Type);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMeleeAttack(int32 Type);
	void LocalMeleeAttack(int32 Type);

	// 投掷
	void Throw();
	UFUNCTION(Server, Reliable)
	void ServerThrow();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastThrow();
	void LocalThrow();

public:
	FORCEINLINE AWeapon* GetPrimaryEquipment() const { return PrimaryEquipment; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

};
