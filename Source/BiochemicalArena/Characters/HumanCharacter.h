#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CommonInputSubsystem.h"
#include "Camera/CameraComponent.h"
#include "BiochemicalArena/Weapons/Projectile.h"
#include "HumanCharacter.generated.h"

class UInputComponent;

UCLASS()
class BIOCHEMICALARENA_API AHumanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHumanCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 是否是手柄
	bool bController = false;

	// 摄像机
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
	// 手臂
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* ArmComponent;

	// 要生成的发射物类
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;
	// 枪口相对于摄像机位置的偏移
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector MuzzleOffset;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// 移动
	void Move(const FInputActionValue& Value);
	// 视角
	void Look(const FInputActionValue& Value);
	// 跳
	void JumpButtonPressed(const FInputActionValue& Value);
	// 蹲
	void CrouchButtonPressed(const FInputActionValue& Value);
	void CrouchButtonReleased(const FInputActionValue& Value);
	// 开火
	void Fire(const FInputActionValue& Value);

private:
	// MappingContext
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	// Move Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	// Look Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	// Jump Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;
	// Crouch Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;
	// MappingContext
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* WeaponsMappingContext;
	// Fire Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	// 实时检测输入设备类型
	UPROPERTY()
	UCommonInputSubsystem* CommonInputSubsystem;
	ECommonInputType CurrentInputType;
	FTimerHandle DetectTimer;
	float DetectDelay = 2.f;
	void StartDetect();
	void DetectCurrentInputDeviceType();

	// 重写CanJump()实现蹲跳，不知道有bug没
	virtual bool CanJumpInternal_Implementation() const override;

};
