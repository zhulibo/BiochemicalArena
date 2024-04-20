#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shell.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AShell : public AActor
{
	GENERATED_BODY()

public:
	AShell();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Equipment")
	UStaticMeshComponent* ShellMesh;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	float ShellEjectionImpulsePerKg = 200.f;;

	UPROPERTY()
	bool bIsFirstOnHit;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
