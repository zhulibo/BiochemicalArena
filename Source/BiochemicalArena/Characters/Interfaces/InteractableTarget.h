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
	virtual bool CanInteract() { return false; }
	// 通知交互目标
	virtual void OnInteract(class ABaseCharacter* BaseCharacter) {}
	// 在服务端通知交互目标
	virtual void OnInteractOnServer() {}
	// 通知交互者已经与突变体交互成功
	virtual void OnInteractMutantSuccess(class AMutantCharacter* MutantCharacter) {}

};
