#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiCultureSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UMultiCultureSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};