#pragma once

#include "CoreMinimal.h"
#include "BaseMode.h"
#include "MutationMode.generated.h"

UCLASS()
class BIOCHEMICALARENA_API AMutationMode : public ABaseMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

};
