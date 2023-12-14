#include "AnimNotify_FinishSwapAttach.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"

UAnimNotify_FinishSwapAttach::UAnimNotify_FinishSwapAttach()
{
	WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Weapons/Data/DT_WeaponData.DT_WeaponData'"));
	if (WeaponDataTable)
	{
		WeaponDataTable->GetAllRows<FWeaponData>("", WeaponDataRows);
	}
}

void UAnimNotify_FinishSwapAttach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	FName MontageSectionName = MeshComp->GetAnimInstance()->Montage_GetCurrentSection();

	if (!MontageSectionName.IsNone() && HumanCharacter && HumanCharacter->GetCombat())
	{
		for (int32 i = 0; i < WeaponDataRows.Num(); ++i)
		{
			FString WeaponName = UEnum::GetValueAsString(WeaponDataRows[i]->WeaponName);
			WeaponName = WeaponName.Right(WeaponName.Len() - WeaponName.Find("::") - 2);
			if (WeaponName == MontageSectionName)
			{
				HumanCharacter->GetCombat()->FinishSwapAttach(WeaponDataRows[i]->WeaponType);
			}
		}
	}
}
