#include "MultiCultureSubsystem.h"

void UMultiCultureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 统一进行翻译，供FText::FindTextInLiveTable_Advanced查找使用。

	// 需与 EHumanCharacterName 的值一致
#define LOCTEXT_NAMESPACE "Human"
	LOCTEXT("SAS", "SAS");
	LOCTEXT("SEAL", "SEAL");
	LOCTEXT("PLA", "PLA");
	LOCTEXT("Test", "Test");
#undef LOCTEXT_NAMESPACE

	// 需与 EMutantCharacterName 的值一致
#define LOCTEXT_NAMESPACE "Mutant"
	LOCTEXT("Tank", "Tank");
	LOCTEXT("Ghost", "Ghost");
	LOCTEXT("Smoker", "Smoker");
	LOCTEXT("Hunter", "Hunter");
#undef LOCTEXT_NAMESPACE

	// 需与 EEquipmentName 的值一致
#define LOCTEXT_NAMESPACE "Equipment"
	LOCTEXT("AK47", "AK47");
	LOCTEXT("AK47_Desert", "AK47_Desert");
	LOCTEXT("M870", "M870");
	LOCTEXT("Glock17", "Glock17");
	LOCTEXT("Kukri", "Kukri");
	LOCTEXT("Grenade", "Grenade");
#undef LOCTEXT_NAMESPACE

	// 需与 EEquipmentType 的值一致
#define LOCTEXT_NAMESPACE "EquipmentType"
	LOCTEXT("Primary", "Primary");
	LOCTEXT("Secondary", "Secondary");
	LOCTEXT("Melee", "Melee");
	LOCTEXT("Throwing", "Throwing");
#undef LOCTEXT_NAMESPACE

}
