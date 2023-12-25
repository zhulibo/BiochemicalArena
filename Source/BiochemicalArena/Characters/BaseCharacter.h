#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void PlayFootstepSound();

protected:
	virtual void BeginPlay() override;

	void Move(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
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

	void CalculateAO_Pitch();
	virtual void Landed(const FHitResult& Hit) override;
	virtual float CalcFallDamageCoefficient();
	void PlayOuchWeaponSound(float DamageCoefficient);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayOuchWeaponSound(float DamageCoefficient);

private:
	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY()
	class USoundCue* MetalSound;
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
	UInputAction* LookAction;
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

	float AO_Pitch;

	bool bIsRadialMenuOpen; // 视角控制 or 径向菜单选择

public:
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

};
