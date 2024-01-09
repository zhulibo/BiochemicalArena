#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FinishSwapAttach.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UAnimNotify_FinishSwapAttach : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_FinishSwapAttach();

private:
	UPROPERTY()
	UDataTable* EquipmentDataTable;
	TArray<struct FEquipmentData*> EquipmentDataRows;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

};
