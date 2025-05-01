#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ThrowOut.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UAN_ThrowOut : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	int LastUniqueID;
	double LastTriggerTime;

};
