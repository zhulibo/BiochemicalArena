#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CrosshairInterface.h"
#include "BaseCharacter.generated.h"

enum class ECommonInputType : uint8;
struct FBag;

UCLASS()
class BIOCHEMICALARENA_API ABaseCharacter : public ACharacter, public ICrosshairInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void OnLocallyControllerReady() {}

	bool HasInitMeshCollision = false;

	void PlayFootstepSound();

	virtual void FellOutOfWorld(const UDamageType& DmgType) override;

	void SetHealth(float TempHealth);
	void SetHUDHealth();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* OverheadWidget;

	UPROPERTY()
	class UAssetSubsystem* AssetSubsystem;
	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;
	UPROPERTY()
	class ABaseMode* BaseMode;
	UPROPERTY()
	class ABaseGameState* BaseGameState;
	UPROPERTY()
	class ABasePlayerState* BasePlayerState;
	UPROPERTY()
	class ABaseController* BaseController;

	void PollInitMeshCollision();

	float AimPitch; // 俯仰
	void CalcAimPitch();

	void PollInit();

	UPROPERTY()
	ECommonInputType CommonInputType;
	UFUNCTION()
	void OnInputMethodChanged(ECommonInputType TempCommonInputType);

	UPROPERTY(EditAnywhere, Category = "Input | Base")
	class UInputMappingContext* BaseMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* LookMouseAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* LookStickAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* CrouchControllerAction;

	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* ScoreboardAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* PauseMenuAction;

	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* RadialMenuAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* RadialMenuChangeAction;
	UPROPERTY(EditAnywhere, Category = "Input | Base")
	UInputAction* RadialMenuSelectAction;

	void Move(const struct FInputActionValue& Value);
	void LookMouse(const FInputActionValue& Value);
	void LookStick(const FInputActionValue& Value);
	void JumpButtonPressed(const FInputActionValue& Value);
	void CrouchButtonPressed(const FInputActionValue& Value);
	void CrouchButtonReleased(const FInputActionValue& Value);
	void CrouchControllerButtonPressed(const FInputActionValue& Value);

	void ScoreboardButtonPressed(const FInputActionValue& Value);
	void ScoreboardButtonReleased(const FInputActionValue& Value);
	void PauseMenuButtonPressed(const FInputActionValue& Value);

	void RadialMenuButtonPressed(const FInputActionValue& Value);
	void RadialMenuButtonReleased(const FInputActionValue& Value);
	void RadialMenuChange(const FInputActionValue& Value);
	void RadialMenuSelect(const FInputActionValue& Value);

	UPROPERTY()
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;
	UFUNCTION()
	void OnRep_Health();

	virtual void Landed(const FHitResult& Hit) override;
	float CalcFallDamageRate();
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayOuchSound(float DamageRate);

	bool bIsDead = false;

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE UWidgetComponent* GetOverheadWidget() const { return OverheadWidget; }
	FORCEINLINE float GetAimPitch() const { return AimPitch; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

};
