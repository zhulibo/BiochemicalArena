#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableTarget.generated.h"

UINTERFACE(MinimalAPI)
class UInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

class BIOCHEMICALARENA_API IInteractableTarget
{
	GENERATED_BODY()

public:
	virtual void OnInteract(class ABaseCharacter* BaseCharacter) = 0;
	virtual bool CanInteract() = 0;

};
