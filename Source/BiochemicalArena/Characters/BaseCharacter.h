#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

enum class ECommonInputType : uint8;

UCLASS()
class BIOCHEMICALARENA_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	bool bIsControllerReady = false;
	virtual void OnControllerReady() {}

	bool HasInitMeshCollision = false;
	void InitAbilityActorInfo();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UAttributeSetBase* GetAttributeSetBase();
	float GetMaxHealth();
	float GetHealth();
	float GetDamageReceivedMul();
	float GetRepelReceivedMul();
	float GetCharacterLevel();
	float GetJumpZVelocity();

	void PlayFootstepSound();

	virtual void FellOutOfWorld(const UDamageType& DmgType) override;
	void SetHealth(float TempHealth);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHealth(float TempHealth, AController* AttackerController);

	UPROPERTY()
	FColor BloodColor;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* BloodEffect_Projectile;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* BloodEffect_Melee;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY()
	class UBAAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UAttributeSetBase* AttributeSetBase;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UGameplayAbilityBase>> StartupAbilities;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	virtual void OnAbilitySystemComponentInit();

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* OverheadWidget;
	UPROPERTY()
	class UOverheadWidget* OverheadWidgetClass;

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

	UFUNCTION()
	void OnInputMethodChanged(ECommonInputType TempCommonInputType);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	void PollSetMeshCollision();

	UPROPERTY(Replicated)
	float ControllerPitch;
	UPROPERTY()
	float AimPitch;
	void CalcAimPitch();

	void PollInit();

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputBase> InputBase;
	void Move(const struct FInputActionValue& Value);
	void LookMouse(const FInputActionValue& Value);
	void LookStick(const FInputActionValue& Value);
	void JumpButtonPressed(const FInputActionValue& Value);
	void CrouchButtonPressed(const FInputActionValue& Value);
	void CrouchButtonReleased(const FInputActionValue& Value);
	void CrouchControllerButtonPressed(const FInputActionValue& Value);

	void InteractStarted(const FInputActionValue& Value);
	void InteractOngoing(const FInputActionValue& Value);
	void InteractTriggered(const FInputActionValue& Value);
	void InteractCompleted(const FInputActionValue& Value);
	void InteractCanceled(const FInputActionValue& Value);

	void ScoreboardButtonPressed(const FInputActionValue& Value);
	void ScoreboardButtonReleased(const FInputActionValue& Value);
	void PauseMenuButtonPressed(const FInputActionValue& Value);

	void RadialMenuButtonPressed(const FInputActionValue& Value);
	void RadialMenuButtonReleased(const FInputActionValue& Value);
	void RadialMenuChangeButtonPressed(const FInputActionValue& Value);
	void RadialMenuSelect(const FInputActionValue& Value);

	void TextChat(const FInputActionValue& Value);
	
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnHealthChanged(const FOnAttributeChangeData& Data);

	virtual void Landed(const FHitResult& Hit) override;
	float CalcFallDamageRate();
	UPROPERTY(EditAnywhere)
	class USoundCue* OuchSound;
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayOuchSound(float DamageRate);

	bool bIsDead = false;

	UPROPERTY()
	AActor* InteractActor;
	void TraceInteractActor(FHitResult& OutHit);

public:
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE UWidgetComponent* GetOverheadWidget() const { return OverheadWidget; }
	FORCEINLINE float GetAimPitch() const { return AimPitch; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

};
