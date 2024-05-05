#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UPlayerSubsystem();

	void Login(int32 Type);

protected:
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	void OnLoginComplete(bool bWasSuccessful);
	void OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);

};
