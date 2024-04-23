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

	UPROPERTY()
	FVector InitVelocity;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	UStaticMeshComponent* ShellMesh;

	UPROPERTY()
	bool bIsFirstOnHit;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
