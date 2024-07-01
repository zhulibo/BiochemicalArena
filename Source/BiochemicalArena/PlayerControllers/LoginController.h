#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginController.generated.h"

UCLASS()
class BIOCHEMICALARENA_API ALoginController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class ULogin* Login;
	UPROPERTY()
	class UCommonActivatableWidgetStack* ServerStack;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULogin> LoginClass;
	void AddContainer();

};
