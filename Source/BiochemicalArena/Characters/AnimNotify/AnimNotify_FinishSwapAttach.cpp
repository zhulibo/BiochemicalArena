#include "AnimNotify_FinishSwapAttach.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"
#include "BiochemicalArena/Characters/Components/CombatComponent.h"

UAnimNotify_FinishSwapAttach::UAnimNotify_FinishSwapAttach()
{
	EquipmentDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Equipments/Data/DT_EquipmentData.DT_EquipmentData'"));
	if (EquipmentDataTable)
	{
		EquipmentDataTable->GetAllRows<FEquipmentData>("", EquipmentDataRows);
	}
}

void UAnimNotify_FinishSwapAttach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	FName MontageSectionName = MeshComp->GetAnimInstance()->Montage_GetCurrentSection();
	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());

	if (!MontageSectionName.IsNone() && HumanCharacter && HumanCharacter->GetCombat())
	{
		for (int32 i = 0; i < EquipmentDataRows.Num(); ++i)
		{
			FString EquipmentName = UEnum::GetValueAsString(EquipmentDataRows[i]->EquipmentName);
			EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2);
			if (EquipmentName == MontageSectionName)
			{
				HumanCharacter->GetCombat()->FinishSwapAttach(EquipmentDataRows[i]->EquipmentType);
			}
		}
	}
}
