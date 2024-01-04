#include "Storage.h"
#include "BagContent.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "StorageButton.h"
#include "BiochemicalArena/Characters/CharacterType.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/WrapBox.h"

void UStorage::NativeConstruct()
{
	Super::NativeConstruct();

	EquipmentDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/Equipments/Data/DT_EquipmentData.DT_EquipmentData'"));
	if (EquipmentDataTable)
	{
		EquipmentDataTable->GetAllRows<FEquipmentData>("", EquipmentDataRows);
	}

	AddStorageTypeButton();
}

// 添加顶部装备类型按钮
void UStorage::AddStorageTypeButton()
{
	if (StorageTypeButtonContainer && StorageTypeButtonClass)
	{
		// 全部按钮
		UCommonButton* AllTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
		if (AllTypeButton)
		{
			AllTypeButton->ButtonText->SetText(FText::FromString("All"));
			AllTypeButton->SetPadding(FMargin(0, 0, 20, 0));
			AllTypeButton->SetIsSelectable(true);
			AllTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, AllTypeButton);
			StorageTypeButtonContainer->AddChild(AllTypeButton);
			// 默认显示全部装备，主动点击一下AllTypeButton(不知道怎么主动触发点击事件，先这样写吧！)
			OnStorageTypeButtonClicked(AllTypeButton);
			AllTypeButton->SetIsSelected(true);
		}
		// 武器分类按钮
		for (int32 i = 0; i < (int32)EEquipmentType::MAX; ++i)
		{
			UCommonButton* EquipmentTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
			if (EquipmentTypeButton)
			{
				FString EnumValue = UEnum::GetValueAsString(EEquipmentType(i));
				EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
				EquipmentTypeButton->ButtonText->SetText(FText::FromString(EnumValue));
				EquipmentTypeButton->SetPadding(FMargin(0, 0, 20, 0));
				EquipmentTypeButton->SetIsSelectable(true);
				EquipmentTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, EquipmentTypeButton);
				StorageTypeButtonContainer->AddChild(EquipmentTypeButton);
			}
		}
		// 角色按钮
		UCommonButton* CharacterTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
		if (CharacterTypeButton)
		{
			CharacterTypeButton->ButtonText->SetText(FText::FromString("Character"));
			CharacterTypeButton->SetPadding(FMargin(0, 0, 20, 0));
			CharacterTypeButton->SetIsSelectable(true);
			CharacterTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, CharacterTypeButton);
			StorageTypeButtonContainer->AddChild(CharacterTypeButton);
		}
	}
}

// 点击装备类型按钮
void UStorage::OnStorageTypeButtonClicked(UCommonButton* CommonButton)
{
	if (CommonButton->GetSelected()) return;
	// 将其他按钮置为未选中状态
	for (int32 i = 0; i < StorageTypeButtonContainer->GetChildrenCount(); ++i)
	{
		UCommonButton* EquipmentTypeButton = Cast<UCommonButton>(StorageTypeButtonContainer->GetChildAt(i));
		if (EquipmentTypeButton && EquipmentTypeButton != CommonButton)
		{
			EquipmentTypeButton->ClearSelection();
		}
	}

	StorageButtonContainer->ClearChildren();

	FString EquipmentType = CommonButton->ButtonText->GetText().ToString();
	if (EquipmentType == "All")
	{
		AddEquipmentButton(FilterEquipment(EquipmentType));
		AddCharacterButton();
	}
	else if (EquipmentType == "Character")
	{
		AddCharacterButton();
	}
	else
	{
		AddEquipmentButton(FilterEquipment(EquipmentType));
	}
}

// 根据武器类型筛选出武器名称
TArray<FText> UStorage::FilterEquipment(FString EquipmentType)
{
	TArray<FText> EquipmentNames;

	if (EquipmentType == "All")
	{
		for (int32 i = 0; i < EquipmentDataRows.Num(); ++i)
		{
			FString EquipmentName = UEnum::GetValueAsString(EquipmentDataRows[i]->EquipmentName);
			EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2);
			// TODO if (未购买EquipmentName) continue;
			EquipmentNames.Add(FText::FromString(EquipmentName));
		}
	}
	else
	{
		for (int32 i = 0; i < EquipmentDataRows.Num(); ++i)
		{
			FString EnumValue = UEnum::GetValueAsString(EquipmentDataRows[i]->EquipmentType);
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			// TODO if (未购买EquipmentName) continue;
			if (EnumValue == EquipmentType)
			{
				FString EquipmentName = UEnum::GetValueAsString(EquipmentDataRows[i]->EquipmentName);
				EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2);
				EquipmentNames.Add(FText::FromString(EquipmentName));
			}
		}
	}

	return EquipmentNames;
}

// 添加武器
void UStorage::AddEquipmentButton(TArray<FText> EquipmentNames)
{
	if (EquipmentButtonClass == nullptr) return;

	for (int32 i = 0; i < EquipmentNames.Num(); ++i)
	{
		UStorageButton* EquipmentButton = CreateWidget<UStorageButton>(this, EquipmentButtonClass);
		if (EquipmentButton)
		{
			EquipmentButton->ButtonText->SetText(EquipmentNames[i]);
			EquipmentButton->SetPadding(FMargin(0, 0, 20, 20));
			EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentButtonClicked, EquipmentButton);
			StorageButtonContainer->AddChild(EquipmentButton);
		}
	}
}

// 添加角色
void UStorage::AddCharacterButton()
{
	if (CharacterButtonClass == nullptr) return;

	for (int32 i = 0; i < (int32)EHumanCharacterName::MAX; ++i)
	{
		FString EnumValue = UEnum::GetValueAsString(EHumanCharacterName(i));
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		// TODO if (未购买EnumValue) continue;

		UStorageButton* CharacterButton = CreateWidget<UStorageButton>(this, CharacterButtonClass);
		if (CharacterButton)
		{
			CharacterButton->ButtonText->SetText(FText::FromString(EnumValue));
			CharacterButton->SetPadding(FMargin(0, 0, 20, 20));
			CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterButtonClicked, CharacterButton);
			StorageButtonContainer->AddChild(CharacterButton);
		}
	}
}

void UStorage::OnEquipmentButtonClicked(UStorageButton* EquipmentButton)
{
	FString EquipmentName = EquipmentButton->ButtonText->GetText().ToString();

	for (int32 i = 0; i < EquipmentDataRows.Num(); ++i)
	{
		FString EnumValue = UEnum::GetValueAsString(EquipmentDataRows[i]->EquipmentName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		if (EnumValue == EquipmentName)
		{
			SetBagContent(EquipmentDataRows[i]->EquipmentType, EquipmentName);
			break;
		}
	}
}

void UStorage::SetBagContent(EEquipmentType& EquipmentType, FString& EquipmentName)
{
	if (BagSwitcher)
	{
		UBagContent* ActiveBag = Cast<UBagContent>(BagSwitcher->GetActiveWidget());
		if (ActiveBag)
		{
			if (EquipmentType == EEquipmentType::Primary)
			{
				ActiveBag->PrimaryEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			}
			else if (EquipmentType == EEquipmentType::Secondary)
			{
				ActiveBag->SecondaryEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			}
			else if (EquipmentType == EEquipmentType::Melee)
			{
				ActiveBag->MeleeEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			}
			else if (EquipmentType == EEquipmentType::Throwing)
			{
				ActiveBag->ThrowingEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
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
			Bag.Primary = BagContent->PrimaryEquipment->ButtonText->GetText().ToString();
			Bag.Secondary = BagContent->SecondaryEquipment->ButtonText->GetText().ToString();
			Bag.Melee = BagContent->MeleeEquipment->ButtonText->GetText().ToString();
			Bag.Throwing = BagContent->ThrowingEquipment->ButtonText->GetText().ToString();
			Bags.Add(Bag);
		}
	}
	// TODO 保存
}

void UStorage::OnCharacterButtonClicked(UStorageButton* EquipmentButton)
{
	FString CharacterName = EquipmentButton->ButtonText->GetText().ToString();
	Character->SetText(FText::FromString(CharacterName));
	// TODO 保存
}
