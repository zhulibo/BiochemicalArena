#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

struct FBag;

UCLASS()
class BIOCHEMICALARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	UPROPERTY()
	TArray<FBag> Bags;

	void PlayFootstepSound();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;
	void GetPlayerStorage();

	bool HasInitMeshCollision = false;
	void PollInitMeshCollision();
	void CalculateAO_Pitch();

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

	virtual float CalcFallDamageFactor();
	void PlayOuchSound(float DamageFactor);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayOuchSound(float DamageFactor);

private:
	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	class ABasePlayerState* BasePlayerState;

	float AO_Pitch; // 俯仰

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

	bool bIsRadialMenuOpened; // 径向菜单是否已打开

public:
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

};
