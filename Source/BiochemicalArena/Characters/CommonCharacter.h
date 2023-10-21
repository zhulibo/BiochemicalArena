#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CommonCharacter.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ACommonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACommonCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void PlayFootstepSound();

protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpButtonPressed(const FInputActionValue& Value);
	void CrouchButtonPressed(const FInputActionValue& Value);
	void CrouchButtonReleased(const FInputActionValue& Value);
	void CrouchControllerButtonPressed(const FInputActionValue& Value);

	void CalculateAO_Pitch();

private:
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundCue* MetalSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* WaterSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* GrassSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* MudSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* CommonSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchControllerAction;

	float AO_Pitch;

public:
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

};
