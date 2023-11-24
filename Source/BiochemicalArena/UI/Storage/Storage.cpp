#include "Storage.h"
#include "BiochemicalArena/Weapons/WeaponType.h"
#include "BiochemicalArena/Characters/CharacterType.h"

void UStorage::NativeConstruct()
{
	Super::NativeConstruct();

	WeaponNameEnum = FindObject<UEnum>(nullptr, TEXT("/Script/BIOCHEMICALARENA.EWeaponName"));
	AddEquipmentTypeButton();
}

// 添加顶部装备类型按钮
void UStorage::AddEquipmentTypeButton()
{
	if (EquipmentTypeButtonContainer && EquipmentTypeButtonClass)
	{
		// 全部按钮
		UCommonButton* AllTypeButton = CreateWidget<UCommonButton>(this, EquipmentTypeButtonClass);
		if (AllTypeButton)
		{
			AllTypeButton->ButtonText->SetText(FText::FromString("All"));
			AllTypeButton->SetPadding(FMargin(0, 0, 20, 0));
			AllTypeButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentTypeButtonClicked, AllTypeButton);
			EquipmentTypeButtonContainer->AddChild(AllTypeButton);
			// 默认显示全部装备，主动点击一下AllTypeButton(不知道怎么主动触发点击事件，先这样写吧！)
			OnEquipmentTypeButtonClicked(AllTypeButton);
			AllTypeButton->SetIsSelected(true);
		}
		// 武器分类按钮
		for (int32 i = 0; i < (int32)EWeaponType::MAX; ++i)
		{
			UCommonButton* WeaponTypeButton = CreateWidget<UCommonButton>(this, EquipmentTypeButtonClass);
			if (WeaponTypeButton)
			{
				FString EnumItemName = UEnum::GetValueAsString(EWeaponType(i));
				EnumItemName = EnumItemName.Right(EnumItemName.Len() - EnumItemName.Find("::") - 2);
				WeaponTypeButton->ButtonText->SetText(FText::FromString(EnumItemName));
				WeaponTypeButton->SetPadding(FMargin(0, 0, 20, 0));
				WeaponTypeButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentTypeButtonClicked, WeaponTypeButton);
				EquipmentTypeButtonContainer->AddChild(WeaponTypeButton);
			}
		}
		// 角色按钮
		UCommonButton* CharacterTypeButton = CreateWidget<UCommonButton>(this, EquipmentTypeButtonClass);
		if (CharacterTypeButton)
		{
			CharacterTypeButton->ButtonText->SetText(FText::FromString("Character"));
			CharacterTypeButton->SetPadding(FMargin(0, 0, 20, 0));
			CharacterTypeButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentTypeButtonClicked, CharacterTypeButton);
			EquipmentTypeButtonContainer->AddChild(CharacterTypeButton);
		}
	}
}

// 点击装备类型按钮
void UStorage::OnEquipmentTypeButtonClicked(UCommonButton* CommonButton)
{
	if (CommonButton->GetSelected()) return;
	// 将其他按钮置为未选中状态
	for (int32 i = 0; i < EquipmentTypeButtonContainer->GetChildrenCount(); ++i)
	{
		UCommonButton* EquipmentTypeButton = Cast<UCommonButton>(EquipmentTypeButtonContainer->GetChildAt(i));
		if (EquipmentTypeButton && EquipmentTypeButton != CommonButton)
		{
			EquipmentTypeButton->ClearSelection();
		}
	}

	EquipmentButtonContainer->ClearChildren();

	FString EquipmentType = CommonButton->ButtonText->GetText().ToString();
	if (EquipmentType == "All")
	{
		TArray<FText> WeaponNames = FilterWeapon(EquipmentType);
		AddWeaponButton(WeaponNames);
		AddCharacterButton();
	}
	else if (EquipmentType == "Character")
	{
		AddCharacterButton();
	}
	else
	{
		TArray<FText> WeaponNames = FilterWeapon(EquipmentType);
		AddWeaponButton(WeaponNames);
	}
}

// 根据武器类型筛选出武器名称
TArray<FText> UStorage::FilterWeapon(FString WeaponTypeToFilter)
{
	TArray<FText> WeaponNames;
	if (WeaponNameEnum == nullptr) WeaponNameEnum = FindObject<UEnum>(nullptr, TEXT("/Script/BIOCHEMICALARENA.EWeaponName"));
	if (WeaponNameEnum == nullptr) return WeaponNames;

	for (int32 i = 0; i < (int32)EWeaponName::MAX; ++i)
	{
		FString EnumItemName = UEnum::GetValueAsString((EWeaponName)i);
		EnumItemName = EnumItemName.Right(EnumItemName.Len() - EnumItemName.Find("::") - 2);
		// TODO if (未购买EnumItemName) continue;

		FString WeaponType;
		WeaponType = WeaponNameEnum->GetMetaData(TEXT("WeaponType"), i);

		if (WeaponTypeToFilter == "All") // 返回所有类型武器
		{
			WeaponNames.Add(FText::FromString(EnumItemName));
		}
		else if (WeaponType == WeaponTypeToFilter) // 返回对应类型武器
		{
			WeaponNames.Add(FText::FromString(EnumItemName));
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("WeaponNames: %s"), *FText::Join(FText::FromString(", "), WeaponNames).ToString());
	return WeaponNames;
}

// 添加武器
void UStorage::AddWeaponButton(TArray<FText> WeaponNames)
{
	if (WeaponButtonClass == nullptr) return;

	for (int i = 0; i < WeaponNames.Num(); ++i)
	{
		UEquipmentButton* WeaponButton = CreateWidget<UEquipmentButton>(this, WeaponButtonClass);
		if (WeaponButton)
		{
			WeaponButton->ButtonText->SetText(WeaponNames[i]);
			WeaponButton->SetPadding(FMargin(0, 0, 20, 20));
			WeaponButton->OnClicked().AddUObject(this, &ThisClass::OnWeaponButtonClicked, WeaponButton);
			EquipmentButtonContainer->AddChild(WeaponButton);
		}
	}
}

// 添加角色
void UStorage::AddCharacterButton()
{
	if (CharacterButtonClass == nullptr) return;

	for (int32 i = 0; i < (int32)EHumanCharacterName::MAX; ++i)
	{
		FString EnumItemName = UEnum::GetValueAsString(EHumanCharacterName(i));
		EnumItemName = EnumItemName.Right(EnumItemName.Len() - EnumItemName.Find("::") - 2);
		// TODO if (未购买EnumItemName) continue;

		UEquipmentButton* CharacterButton = CreateWidget<UEquipmentButton>(this, CharacterButtonClass);
		if (CharacterButton)
		{
			CharacterButton->ButtonText->SetText(FText::FromString(EnumItemName));
			CharacterButton->SetPadding(FMargin(0, 0, 20, 20));
			CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterButtonClicked, CharacterButton);
			EquipmentButtonContainer->AddChild(CharacterButton);
		}
	}
}

void UStorage::OnWeaponButtonClicked(UEquipmentButton* EquipmentButton)
{
	FString WeaponName = EquipmentButton->ButtonText->GetText().ToString();

	if (WeaponNameEnum == nullptr) WeaponNameEnum = FindObject<UEnum>(nullptr, TEXT("/Script/BIOCHEMICALARENA.EWeaponName"));
	if (WeaponNameEnum == nullptr) return;

	for (int32 i = 0; i < (int32)EWeaponName::MAX; ++i)
	{
		FString EnumItemName = UEnum::GetValueAsString((EWeaponName)i);
		EnumItemName = EnumItemName.Right(EnumItemName.Len() - EnumItemName.Find("::") - 2);

		if (EnumItemName == WeaponName)
		{
			FString WeaponType = WeaponNameEnum->GetMetaData(TEXT("WeaponType"), i);
			SetBagContent(WeaponType, WeaponName);
			break;
		}
	}
}

void UStorage::SetBagContent(FString& WeaponType, FString& WeaponName)
{
	if (BagSwitcher)
	{
		UBagContent* ActiveBag = Cast<UBagContent>(BagSwitcher->GetActiveWidget());
		if (ActiveBag)
		{
			if (WeaponType == "Primary")
			{
				ActiveBag->Primary->ButtonText->SetText(FText::FromString(WeaponName));
			}
			else if (WeaponType == "Secondary")
			{
				ActiveBag->Secondary->ButtonText->SetText(FText::FromString(WeaponName));
			}
			else if (WeaponType == "Melee")
			{
				ActiveBag->Melee->ButtonText->SetText(FText::FromString(WeaponName));
			}
			else if (WeaponType == "Throwing")
			{
				ActiveBag->Throwing->ButtonText->SetText(FText::FromString(WeaponName));
			}
		}
	}
	SaveBag();
}

void UStorage::SaveBag()
{
	TArray<FBag> Bags;
	for (int32 i = 0; i < BagSwitcher->GetChildrenCount(); ++i)
	{
		UBagContent* BagContent = Cast<UBagContent>(BagSwitcher->GetChildAt(i));
		if (BagContent)
		{
			FBag Bag;
			Bag.Primary = BagContent->Primary->ButtonText->GetText().ToString();
			Bag.Secondary = BagContent->Secondary->ButtonText->GetText().ToString();
			Bag.Melee = BagContent->Melee->ButtonText->GetText().ToString();
			Bag.Throwing = BagContent->Throwing->ButtonText->GetText().ToString();
			Bags.Add(Bag);
		}
	}
	// TODO 保存
}

void UStorage::OnCharacterButtonClicked(UEquipmentButton* EquipmentButton)
{
	FString CharacterName = EquipmentButton->ButtonText->GetText().ToString();
	Character->ButtonText->SetText(FText::FromString(CharacterName));
	// TODO 保存
}
