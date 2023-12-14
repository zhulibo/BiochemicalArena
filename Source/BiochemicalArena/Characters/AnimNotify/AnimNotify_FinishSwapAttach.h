#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BiochemicalArena/Weapons/WeaponType.h"
#include "AnimNotify_FinishSwapAttach.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UAnimNotify_FinishSwapAttach : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_FinishSwapAttach();

private:
	UPROPERTY()
	UDataTable* WeaponDataTable;
	TArray<FWeaponData*> WeaponDataRows;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

};
