#include "Storage.h"
#include "BagContent.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "StorageButton.h"
#include "BiochemicalArena/Characters/CharacterType.h"
#include "..\..\System\StorageSaveGameType.h"
#include "BiochemicalArena/Equipments/EquipmentType.h"
#include "..\..\System\StorageSaveGame.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Components/ButtonSlot.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UStorage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 添加顶部库存分类Tab按钮
	AddStorageTypeButton();

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnQueryOwnershipComplete.AddUObject(this, &ThisClass::OnQueryOwnershipComplete);
		EOSSubsystem->OnReadFileComplete.AddUObject(this, &ThisClass::OnReadFileComplete);
		EOSSubsystem->OnEnumerateFilesComplete.AddUObject(this, &ThisClass::OnEnumerateFilesComplete);
		// EOSSubsystem->QueryOwnership(); // TODO
	}
}

UWidget* UStorage::NativeGetDesiredFocusTarget() const
{
	return StorageTypeButtonContainer->GetChildAt(0);
}

// 获取已购商品完成
void UStorage::OnQueryOwnershipComplete(bool bWasSuccessful, const TArray<FString> TempOwnership)
{
	if(!bWasSuccessful) return;

	Ownership = TempOwnership;

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		// 默认显示全部库存
		if (StorageTypeButtonContainer)
		{
			UCommonButton* AllTypeButton = Cast<UCommonButton>(StorageTypeButtonContainer->GetChildAt(0));
			if (AllTypeButton) OnStorageTypeButtonClicked(AllTypeButton);
		}

		// 缓存用户文件
		EOSSubsystem->EnumerateFiles();
	}
}

// 缓存用户文件完成
void UStorage::OnEnumerateFilesComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful) return;

	if (EOSSubsystem == nullptr) EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();

	if (EOSSubsystem && StorageSubsystem)
	{
		if (EOSSubsystem->GetEnumeratedFiles().Contains(StorageSubsystem->GetSlotName())) // 云包含该存档文件
		{
			EOSSubsystem->ReadFile(StorageSubsystem->GetSlotName()); // 读取存档文件
		}
		else // 云不包含该存档文件
		{
			InitPlayerConfig(StorageSubsystem->StorageCache); // 使用本地存档
		}
	}
}

// 读取存档文件完成
void UStorage::OnReadFileComplete(bool bWasSuccessful, const FUserFileContentsRef& FileContents)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem == nullptr) return;

	if (bWasSuccessful) // 使用云存档文件
	{
		UStorageSaveGame* StorageSaveGame = NewObject<UStorageSaveGame>(this);
		FMemoryReader MemoryReader((FileContents.Get()), true);
		FObjectAndNameAsStringProxyArchive Ar(MemoryReader, false);
		Ar.ArIsSaveGame = false;
		Ar.ArNoDelta = true;
		StorageSaveGame->Serialize(Ar);

		InitPlayerConfig(StorageSaveGame);
	}
	else // 使用本地存档
	{
		InitPlayerConfig(StorageSubsystem->StorageCache);
	}
}

// 初始化玩家配置
void UStorage::InitPlayerConfig(UStorageSaveGame* StorageSaveGame)
{
	if(StorageSaveGame)
	{
		// 判断是否拥有装备，没有置空
		for (int32 i = 0; i < StorageSaveGame->Bags.Num(); ++i)
		{
			if (!HasEquipment(StorageSaveGame->Bags[i].Primary)) StorageSaveGame->Bags[i].Primary = "";
			if (!HasEquipment(StorageSaveGame->Bags[i].Secondary)) StorageSaveGame->Bags[i].Secondary = "";
			if (!HasEquipment(StorageSaveGame->Bags[i].Melee)) StorageSaveGame->Bags[i].Melee = "";
			if (!HasEquipment(StorageSaveGame->Bags[i].Throwing)) StorageSaveGame->Bags[i].Throwing = "";
		}

		// 设置装备
		for (int32 i = 0; i < BagSwitcher->GetChildrenCount(); ++i)
		{
			UBagContent* BagContent = Cast<UBagContent>(BagSwitcher->GetChildAt(i));
			if (BagContent)
			{
				BagContent->PrimaryEquipment->ButtonText->SetText(FText::FromString(StorageSaveGame->Bags[i].Primary));
				BagContent->SecondaryEquipment->ButtonText->SetText(FText::FromString(StorageSaveGame->Bags[i].Secondary));
				BagContent->MeleeEquipment->ButtonText->SetText(FText::FromString(StorageSaveGame->Bags[i].Melee));
				BagContent->ThrowingEquipment->ButtonText->SetText(FText::FromString(StorageSaveGame->Bags[i].Throwing));
			}
		}

		// 判断是否拥有角色，没有则恢复默认
		if (!HasHumanCharacter(StorageSaveGame->Character)) StorageSaveGame->Character = "SAS";

		// 设置角色
		Character->SetText(FText::FromString(StorageSaveGame->Character));
	}
}

// 是否拥有装备
bool UStorage::HasEquipment(FString EquipmentName)
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->GetEquipmentDataRows().Num() == 0) return false;

	for (int32 i = 0; i < AssetSubsystem->GetEquipmentDataRows().Num(); ++i)
	{
		FString TempEquipmentName = UEnum::GetValueAsString(AssetSubsystem->GetEquipmentDataRows()[i]->EquipmentName);
		TempEquipmentName = TempEquipmentName.Right(TempEquipmentName.Len() - TempEquipmentName.Find("::") - 2);

		// 名字不匹配
		if (EquipmentName != TempEquipmentName) continue;

		// 免费装备
		if (AssetSubsystem->GetEquipmentDataRows()[i]->AudienceItemId == "-1") return true;

		// 付费装备判断是否已拥有
		if (Ownership.Contains(AssetSubsystem->GetEquipmentDataRows()[i]->AudienceItemId)) return true;
	}

	return false;
}

// 是否拥有角色
bool UStorage::HasHumanCharacter(FString HumanCharacterName)
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->GetHumanCharacterDataRows().Num() == 0) return false;

	for (int32 i = 0; i < AssetSubsystem->GetHumanCharacterDataRows().Num(); ++i)
	{
		FString TempHumanCharacterName = UEnum::GetValueAsString(AssetSubsystem->GetHumanCharacterDataRows()[i]->HumanCharacterName);
		TempHumanCharacterName = TempHumanCharacterName.Right(TempHumanCharacterName.Len() - TempHumanCharacterName.Find("::") - 2);

		// 名字不匹配
		if (HumanCharacterName != TempHumanCharacterName) continue;

		// 免费角色
		if (AssetSubsystem->GetHumanCharacterDataRows()[i]->AudienceItemId == "-1") return true;

		// 付费角色判断是否已拥有
		if (Ownership.Contains(AssetSubsystem->GetHumanCharacterDataRows()[i]->AudienceItemId)) return true;
	}

	return false;
}

// 添加顶部库存分类Tab按钮
void UStorage::AddStorageTypeButton()
{
	if (StorageTypeButtonContainer && StorageTypeButtonClass)
	{
		// 全部按钮
		UCommonButton* AllTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
		if (AllTypeButton)
		{
			AllTypeButton->ButtonText->SetText(FText::FromString("All"));
			AllTypeButton->SetIsSelectable(true);
			AllTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, AllTypeButton);
			StorageTypeButtonContainer->AddChild(AllTypeButton);
			AllTypeButton->SetIsSelected(true);
		}
		// 装备按钮
		for (int32 i = 0; i < static_cast<int32>(EEquipmentType::MAX); ++i)
		{
			UCommonButton* EquipmentTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
			if (EquipmentTypeButton)
			{
				FString EnumValue = UEnum::GetValueAsString(static_cast<EEquipmentType>(i));
				EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
				EquipmentTypeButton->ButtonText->SetText(FText::FromString(EnumValue));
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
			CharacterTypeButton->SetIsSelectable(true);
			CharacterTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, CharacterTypeButton);
			StorageTypeButtonContainer->AddChild(CharacterTypeButton);
		}
	}
}

// 点击库存分类按钮
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

	// 清空所有分类按钮
	StorageButtonContainer->ClearChildren();

	// 点击库存分类按钮添加库存
	FString EquipmentType = CommonButton->ButtonText->GetText().ToString();
	if (EquipmentType == "All")
	{
		AddEquipmentButton(FilterEquipment(EquipmentType));
		AddCharacterButton(FilterHumanCharacter());
	}
	else if (EquipmentType == "Character")
	{
		AddCharacterButton(FilterHumanCharacter());
	}
	else
	{
		AddEquipmentButton(FilterEquipment(EquipmentType));
	}
}

// 根据装备类型筛选出拥有的装备名称
TArray<FText> UStorage::FilterEquipment(FString EquipmentType)
{
	TArray<FText> EquipmentNames;

	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->GetEquipmentDataRows().Num() == 0) return EquipmentNames;

	for (int32 i = 0; i < AssetSubsystem->GetEquipmentDataRows().Num(); ++i)
	{
		FString EnumValue = UEnum::GetValueAsString(AssetSubsystem->GetEquipmentDataRows()[i]->EquipmentType);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

		if (EquipmentType == "All" || EnumValue == EquipmentType) // 对应的装备类型
		{
			FString EquipmentName = UEnum::GetValueAsString(AssetSubsystem->GetEquipmentDataRows()[i]->EquipmentName);
			EquipmentName = EquipmentName.Right(EquipmentName.Len() - EquipmentName.Find("::") - 2);

			if (AssetSubsystem->GetEquipmentDataRows()[i]->AudienceItemId == "-1") // 免费装备直接添加
			{
				EquipmentNames.Add(FText::FromString(EquipmentName));
			}
			else // 付费装备判断是否已拥有
			{
				if (Ownership.Contains(AssetSubsystem->GetEquipmentDataRows()[i]->AudienceItemId))
				{
					EquipmentNames.Add(FText::FromString(EquipmentName));
				}
			}
		}
	}

	return EquipmentNames;
}

// 筛选出拥有的角色名称
TArray<FText> UStorage::FilterHumanCharacter()
{
	TArray<FText> HumanCharacterNames;
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->GetHumanCharacterDataRows().Num() == 0) return HumanCharacterNames;

	for (int32 i = 0; i < AssetSubsystem->GetHumanCharacterDataRows().Num(); ++i)
	{
		FString HumanCharacterName = UEnum::GetValueAsString(AssetSubsystem->GetHumanCharacterDataRows()[i]->HumanCharacterName);
		HumanCharacterName = HumanCharacterName.Right(HumanCharacterName.Len() - HumanCharacterName.Find("::") - 2);

		if (AssetSubsystem->GetHumanCharacterDataRows()[i]->AudienceItemId == "-1") // 免费角色直接添加
		{
			HumanCharacterNames.Add(FText::FromString(HumanCharacterName));
		}
		else // 付费角色判断是否已拥有
		{
			if (Ownership.Contains(AssetSubsystem->GetHumanCharacterDataRows()[i]->AudienceItemId))
			{
				HumanCharacterNames.Add(FText::FromString(HumanCharacterName));
			}
		}
	}

	return HumanCharacterNames;
}

// 添加装备
void UStorage::AddEquipmentButton(TArray<FText> EquipmentNames)
{
	if (EquipmentButtonClass == nullptr) return;

	for (int32 i = 0; i < EquipmentNames.Num(); ++i)
	{
		UStorageButton* EquipmentButton = CreateWidget<UStorageButton>(this, EquipmentButtonClass);
		if (EquipmentButton)
		{
			EquipmentButton->ButtonText->SetText(EquipmentNames[i]);
			EquipmentButton->OnClicked().AddUObject(this, &ThisClass::OnEquipmentButtonClicked, EquipmentButton);
			StorageButtonContainer->AddChild(EquipmentButton);
			UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(StorageButtonContainer->AddChild(EquipmentButton));
			if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
		}
	}
}

// 添加角色
void UStorage::AddCharacterButton(TArray<FText> CharacterNames)
{
	if (CharacterButtonClass == nullptr) return;

	for (int32 i = 0; i < CharacterNames.Num(); ++i)
	{
		UStorageButton* CharacterButton = CreateWidget<UStorageButton>(this, CharacterButtonClass);
		if (CharacterButton)
		{
			CharacterButton->ButtonText->SetText(CharacterNames[i]);
			CharacterButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterButtonClicked, CharacterButton);
			UWrapBoxSlot* NewSlot = Cast<UWrapBoxSlot>(StorageButtonContainer->AddChild(CharacterButton));
			if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 20));
		}
	}
}

// 点击装备按钮
void UStorage::OnEquipmentButtonClicked(UStorageButton* EquipmentButton)
{
	FString EquipmentName = EquipmentButton->ButtonText->GetText().ToString();

	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->GetEquipmentDataRows().Num() == 0) return;

	for (int32 i = 0; i < AssetSubsystem->GetEquipmentDataRows().Num(); ++i)
	{
		FString EnumValue = UEnum::GetValueAsString(AssetSubsystem->GetEquipmentDataRows()[i]->EquipmentName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
		if (EnumValue == EquipmentName)
		{
			SaveBag(AssetSubsystem->GetEquipmentDataRows()[i]->EquipmentType, EquipmentName);
			break;
		}
	}
}

// 保存背包
void UStorage::SaveBag(EEquipmentType& EquipmentType, FString& EquipmentName)
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

	SaveBagToStorage();
}

// 保存背包到存档
void UStorage::SaveBagToStorage()
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

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->StorageCache->Bags = Bags;
		StorageSubsystem->SaveToDisk();
	}
}

// 点击角色按钮
void UStorage::OnCharacterButtonClicked(UStorageButton* EquipmentButton)
{
	// 保存角色
	FText CharacterName = EquipmentButton->ButtonText->GetText();
	Character->SetText(CharacterName);

	// 保存角色到存档
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem)
	{
		StorageSubsystem->StorageCache->Character = CharacterName.ToString();
		StorageSubsystem->SaveToDisk();
	}
}
