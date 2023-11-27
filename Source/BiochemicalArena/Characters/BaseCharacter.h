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

	void CalculateAO_Pitch();
	virtual void Landed(const FHitResult& Hit) override;
	virtual float CalcFallDamageCoefficient();
	void PlayOuchWeaponSound(float DamageCoefficient);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayOuchWeaponSound(float DamageCoefficient);

private:
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Default", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchControllerAction;

	float AO_Pitch;

public:
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

};
