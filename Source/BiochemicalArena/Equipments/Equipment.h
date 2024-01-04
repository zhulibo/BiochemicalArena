#pragma once

#include "CoreMinimal.h"
#include "EquipmentType.h"
#include "BiochemicalArena/PlayerStates/Team.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AEquipment : public AActor
{
	GENERATED_BODY()

public:
	AEquipment();

	virtual void SetEquipmentState(EEquipmentState State);

	UPROPERTY(EditAnywhere)
	USoundCue* UseEquipmentSound;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* SwapMontage;

	void DropEquipment();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	USkeletalMeshComponent* EquipmentMesh;
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	class USphereComponent* AreaSphere;

	UFUNCTION()
	virtual void OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	EEquipmentState EquipmentState;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	EEquipmentType EquipmentType;
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	EEquipmentCate EquipmentCate;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	EEquipmentName EquipmentName;

	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AHumanController* HumanController;

	virtual void OnEquipped();
	UPROPERTY(replicated)
	ETeam OwnerTeam = ETeam::NoTeam;
	void SetOwnerTeam();

	virtual void OnDropped();
	void SetAreaSphereCollision();
	FTimerHandle DestroyEquipmentTimerHandle;
	void DestroyEquipment();

public:
	FORCEINLINE USkeletalMeshComponent* GetEquipmentMesh() const { return EquipmentMesh; }

	FORCEINLINE EEquipmentType GetEquipmentType() const { return EquipmentType; }
	FORCEINLINE EEquipmentCate GetEquipmentCate() const { return EquipmentCate; }
	FORCEINLINE EEquipmentName GetEquipmentName() const { return EquipmentName; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }

};
