#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Equipment.generated.h"

enum class ETeam : uint8;
enum class EEquipmentName : uint8;
enum class EEquipmentCate : uint8;
enum class EEquipmentType : uint8;
enum class EEquipmentState : uint8;

UCLASS()
class BIOCHEMICALARENA_API AEquipment : public AActor
{
	GENERATED_BODY()

public:
	AEquipment();

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* SwapInMontage_C;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* SwapInMontage_E;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* SwapOutMontage_C;
	UPROPERTY(EditAnywhere, Category = "Equipment")
	UAnimMontage* SwapOutMontage_E;

	class UEquipmentAnimInstance* GetEquipmentAnimInstance();

	virtual void EquipEquipment();
	virtual void DropEquipment();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	USkeletalMeshComponent* EquipmentMesh;
	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	class USphereComponent* AreaSphere;

	UPROPERTY()
	UEquipmentAnimInstance* EquipmentAnimInstance;

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

	UPROPERTY(replicated)
	ETeam OwnerTeam;
	void SetOwnerTeam();

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
