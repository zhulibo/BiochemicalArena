#include "AnimNotify_FinishSwapAttach.h"
#include "BiochemicalArena/Characters/HumanCharacter.h"

void UAnimNotify_FinishSwapAttach::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AHumanCharacter* HumanCharacter = Cast<AHumanCharacter>(MeshComp->GetOwner());
	FName MontageSectionName = MeshComp->GetAnimInstance()->Montage_GetCurrentSection();

	if (HumanCharacter && HumanCharacter->GetCombat() && !MontageSectionName.IsNone())
	{
		// 循环EWeaponName，根据MontageSectionName找到对应枚举项的WeaponType
		FName WeaponName_Meta_WeaponType;
		for (int32 i = 0; i < static_cast<int32>(EWeaponName::MAX); ++i)
		{
			UEnum* EnumObject = FindObject<UEnum>(nullptr, TEXT("/Script/BIOCHEMICALARENA.EWeaponName"));
			FName SectionName = FName(*EnumObject->GetMetaData(TEXT("MontageSectionName"), i));
			if (SectionName == MontageSectionName)
			{
				WeaponName_Meta_WeaponType = FName(*EnumObject->GetMetaData(TEXT("WeaponType"), i));
				break;
			}
		}
		EWeaponType WeaponType = EWeaponType::Melee;
		if (WeaponName_Meta_WeaponType == "Primary")
		{
			WeaponType = EWeaponType::Primary;
		}
		else if (WeaponName_Meta_WeaponType == "Secondary")
		{
			WeaponType = EWeaponType::Secondary;
		}
		else if (WeaponName_Meta_WeaponType == "Melee")
		{
			WeaponType = EWeaponType::Melee;
		}
		else if (WeaponName_Meta_WeaponType == "Throwing")
		{
			WeaponType = EWeaponType::Throwing;
		}

		HumanCharacter->GetCombat()->FinishSwapAttach(WeaponType);
	}
}
