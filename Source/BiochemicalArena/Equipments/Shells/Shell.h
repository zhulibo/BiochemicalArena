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

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ShellMesh;

	UPROPERTY()
	bool bIsFirstOnHit = true;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound;

};
