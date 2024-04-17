#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

enum class ECommonInputType : uint8;
struct FBag;

UCLASS()
class BIOCHEMICALARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	void PlayFootstepSound();

	UPROPERTY()
	float MouseSensitivityRate;
	UPROPERTY()
	bool bMouseAimAssistSteering;
	UPROPERTY()
	bool bMouseAimAssistSlowdown;
	UPROPERTY()
	float ControllerSensitivityRate;
	UPROPERTY()
	bool bControllerAimAssistSteering;
	UPROPERTY()
	bool bControllerAimAssistSlowdown;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	bool HasInitMeshCollision = false;
	void PollInitMeshCollision();

	void CalcAimPitch();

	void OnLocalControllerReady();

	UPROPERTY()
	ECommonInputType CommonInputType;
	UFUNCTION()
	void OnInputMethodChanged(ECommonInputType TemCommonInputType);

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

	virtual float CalcFallDamageRate();
	void PlayOuchSound(float DamageRate);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayOuchSound(float DamageRate);

private:
	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	class ABasePlayerState* BasePlayerState;

	float AimPitch; // 俯仰

	UPROPERTY()
	USoundCue* MetalSound;
	UPROPERTY()
	USoundCue* WaterSound;
	UPROPERTY()
	USoundCue* GrassSound;
	UPROPERTY()
	USoundCue* MudSound;
	UPROPERTY()
	USoundCue* CommonSound;

	UPROPERTY()
	USoundCue* OuchSound1;
	UPROPERTY()
	USoundCue* OuchSound2;
	UPROPERTY()
	USoundCue* OuchSound3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* BaseMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookMouseAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookStickAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchControllerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* ScoreboardAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* RadialMenuAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* RadialMenuChangeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Base", meta = (AllowPrivateAccess = "true"))
	UInputAction* RadialMenuSelectAction;

	bool bIsRadialMenuOpened; // 径向菜单是否已打开

public:
	FORCEINLINE float GetAimPitch() const { return AimPitch; }

};
