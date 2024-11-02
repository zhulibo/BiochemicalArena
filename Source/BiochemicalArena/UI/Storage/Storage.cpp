#include "Storage.h"
#include "BagContent.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "DataRegistrySubsystem.h"
#include "StorageButton.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/System/Storage/ConfigType.h"
#include "BiochemicalArena/Equipments/Data/EquipmentType.h"
#include "BiochemicalArena/System/Storage/SaveGameLoadout.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/System/Storage/DefaultConfig.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/Utils/LibraryNotify.h"
#include "Components/ButtonSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#define LOCTEXT_NAMESPACE "UStorage"

void UStorage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 添加顶部库存分类Tab按钮
	AddStorageTypeButton();

	// 获取角色和装备数据
	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		if (UDataRegistry* DataRegistry = DRSubsystem->GetRegistryForType(DR_HUMAN_CHARACTER_MAIN))
		{
			const UScriptStruct* OutStruct;
			DataRegistry->GetAllCachedItems(HumanCharacterMains, OutStruct);
		}

		if (UDataRegistry* DataRegistry = DRSubsystem->GetRegistryForType(DR_EQUIPMENT_MAIN))
		{
			const UScriptStruct* OutStruct;
			DataRegistry->GetAllCachedItems(EquipmentMains, OutStruct);
		}
	}

	EOSSubsystem = GetGameInstance()->GetSubsystem<UEOSSubsystem>();
	if (EOSSubsystem)
	{
		EOSSubsystem->OnPurchaseCompleted.AddUObject(this, &ThisClass::OnPurchaseCompleted);
		EOSSubsystem->OnQueryEntitlementsComplete.AddUObject(this, &ThisClass::OnQueryEntitlementsComplete);

		EOSSubsystem->OnReadFileComplete.AddUObject(this, &ThisClass::OnReadFileComplete);
		EOSSubsystem->OnEnumerateFilesComplete.AddUObject(this, &ThisClass::OnEnumerateFilesComplete);

		EOSSubsystem->QueryEntitlements();
	}
}

UWidget* UStorage::NativeGetDesiredFocusTarget() const
{
	return StorageTypeButtonContainer->GetChildAt(0);
}

// 商品购买后刷新仓库
void UStorage::OnPurchaseCompleted(const FCommerceOnPurchaseComplete& CommerceOnPurchaseComplete)
{
	if (EOSSubsystem)
	{
		EOSSubsystem->QueryEntitlements();
	}
}

// 查询已购商品完成
void UStorage::OnQueryEntitlementsComplete(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if (EOSSubsystem)
		{
			EOSSubsystem->GetEntitlements();

			// 默认显示全部库存
			if (UCommonButton* AllTypeButton = Cast<UCommonButton>(StorageTypeButtonContainer->GetChildAt(0)))
			{
				OnStorageTypeButtonClicked(AllTypeButton);
			}

			// 查询用户文件
			EOSSubsystem->EnumerateFiles();
		}
	}
	else
	{
		NOTIFY(this, C_RED, LOCTEXT("QueryEntitlementsFailed", "Get purchased items failed!"));
	}
}

// 缓存用户文件完成
void UStorage::OnEnumerateFilesComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful) return;

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	
	if (EOSSubsystem && StorageSubsystem)
	{
		if (EOSSubsystem->GetEnumeratedFiles().Contains(StorageSubsystem->SlotLoadout)) // 云包含该存档文件
		{
			EOSSubsystem->ReadFile(StorageSubsystem->SlotLoadout); // 读取存档文件
		}
		else // 云不包含该存档文件
		{
			InitPlayerConfig(StorageSubsystem->CacheLoadout); // 使用本地存档
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
		USaveGameLoadout* SaveGameLoadout = NewObject<USaveGameLoadout>(this);
		FMemoryReader MemoryReader((FileContents.Get()), true);
		FObjectAndNameAsStringProxyArchive Ar(MemoryReader, false);
		Ar.ArIsSaveGame = false;
		Ar.ArNoDelta = true;
		SaveGameLoadout->Serialize(Ar);

		InitPlayerConfig(SaveGameLoadout);
	}
	else // 使用本地存档
	{
		InitPlayerConfig(StorageSubsystem->CacheLoadout);
	}
}

// 初始化玩家配置
void UStorage::InitPlayerConfig(USaveGameLoadout* SaveGameLoadout)
{
	if (SaveGameLoadout)
	{
		const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>();

		// 判断是否拥有装备，没有则恢复默认
		if (DefaultConfig)
		{
			for (int32 i = 0; i < SaveGameLoadout->Bags.Num(); ++i)
			{
				if (!HasEquipment(SaveGameLoadout->Bags[i].Primary))
				{
					SaveGameLoadout->Bags[i].Primary = DefaultConfig->DefaultPrimary;
				}
				if (!HasEquipment(SaveGameLoadout->Bags[i].Secondary))
				{
					SaveGameLoadout->Bags[i].Secondary = DefaultConfig->DefaultSecondary;
				}
				if (!HasEquipment(SaveGameLoadout->Bags[i].Melee))
				{
					SaveGameLoadout->Bags[i].Melee = DefaultConfig->DefaultMelee;
				}
				if (!HasEquipment(SaveGameLoadout->Bags[i].Throwing))
				{
					SaveGameLoadout->Bags[i].Throwing = DefaultConfig->DefaultThrowing;
				}
			}
		}
		// 设置装备
		for (int32 i = 0; i < BagSwitcher->GetChildrenCount(); ++i)
		{
			UBagContent* BagContent = Cast<UBagContent>(BagSwitcher->GetChildAt(i));
			if (BagContent)
			{
				BagContent->PrimaryEquipment->ButtonText->SetText(FText::FromString(SaveGameLoadout->Bags[i].Primary));
				BagContent->SecondaryEquipment->ButtonText->SetText(FText::FromString(SaveGameLoadout->Bags[i].Secondary));
				BagContent->MeleeEquipment->ButtonText->SetText(FText::FromString(SaveGameLoadout->Bags[i].Melee));
				BagContent->ThrowingEquipment->ButtonText->SetText(FText::FromString(SaveGameLoadout->Bags[i].Throwing));
			}
		}

		// 判断是否拥有角色，没有则恢复默认
		FString EnumValue = UEnum::GetValueAsString(SaveGameLoadout->HumanCharacterName);
		FString HumanCharacterName = EnumValue.Right(EnumValue.Len() - EnumValue.Find(TEXT("::")) - 2);
		if (DefaultConfig && !HasHumanCharacter(HumanCharacterName))
		{
			SaveGameLoadout->HumanCharacterName = DefaultConfig->HumanCharacterName;
		}
		// 设置角色
		Character->SetText(FText::FromString(HumanCharacterName));
	}
}

// 是否拥有装备
bool UStorage::HasEquipment(FString EquipmentName)
{
	for (const TPair<FDataRegistryId, const uint8*>& Pair : EquipmentMains)
	{
		FEquipmentMain ItemValue = *reinterpret_cast<const FEquipmentMain*>(Pair.Value);

		// 名字不匹配
		if (EquipmentName != Pair.Key.ItemName) continue;

		// 免费装备
		if (ItemValue.AudienceItemId.IsEmpty()) return true;

		// 付费装备判断是否已拥有
		if (EOSSubsystem && EOSSubsystem->OwnEntitlement(ItemValue.AudienceItemId)) return true;
	}

	return false;
}

// 是否拥有角色
bool UStorage::HasHumanCharacter(FString HumanCharacterName)
{
	for (const TPair<FDataRegistryId, const uint8*>& Pair : HumanCharacterMains)
	{
		FHumanCharacterMain ItemValue = *reinterpret_cast<const FHumanCharacterMain*>(Pair.Value);

		// 名字不匹配
		if (HumanCharacterName != Pair.Key.ItemName) continue;

		// 免费角色
		if (ItemValue.AudienceItemId.IsEmpty()) return true;

		// 付费角色判断是否已拥有
		if (EOSSubsystem && EOSSubsystem->OwnEntitlement(ItemValue.AudienceItemId)) return true;
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
			AllTypeButton->ButtonText->SetText(LOCTEXT("All", "All"));
			AllTypeButton->Name = STORAGE_TYPE_ALL;
			AllTypeButton->SetIsSelectable(true);
			AllTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, AllTypeButton);
			UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(StorageTypeButtonContainer->AddChild(AllTypeButton));
			if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 0));
		}
		// 装备按钮
		for (int32 i = 0; i < static_cast<int32>(EEquipmentType::NONE); ++i)
		{
			UCommonButton* EquipmentTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
			if (EquipmentTypeButton)
			{
				FString EnumValue = UEnum::GetValueAsString(static_cast<EEquipmentType>(i));
				EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
				EquipmentTypeButton->ButtonText->SetText(FText::FromString(EnumValue));
				EquipmentTypeButton->Name = EnumValue;
				
				if (EnumValue == FName(TEXT("Primary")))
				{
					EquipmentTypeButton->ButtonText->SetText(LOCTEXT("Primary", "Primary"));
				}
				if (EnumValue == FName(TEXT("Secondary")))
				{
					EquipmentTypeButton->ButtonText->SetText(LOCTEXT("Secondary", "Secondary"));
				}
				if (EnumValue == FName(TEXT("Melee")))
				{
					EquipmentTypeButton->ButtonText->SetText(LOCTEXT("Melee", "Melee"));
				}
				if (EnumValue == FName(TEXT("Throwing")))
				{
					EquipmentTypeButton->ButtonText->SetText(LOCTEXT("Throwing", "Throwing"));
				}
				
				EquipmentTypeButton->SetIsSelectable(true);
				EquipmentTypeButton->OnClicked().AddUObject(this, &ThisClass::OnStorageTypeButtonClicked, EquipmentTypeButton);
				UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(StorageTypeButtonContainer->AddChild(EquipmentTypeButton));
				if (NewSlot) NewSlot->SetPadding(FMargin(0, 0, 20, 0));
			}
		}
		// 角色按钮
		UCommonButton* CharacterTypeButton = CreateWidget<UCommonButton>(this, StorageTypeButtonClass);
		if (CharacterTypeButton)
		{
			CharacterTypeButton->ButtonText->SetText(LOCTEXT("Character", "Character"));
			CharacterTypeButton->Name = STORAGE_TYPE_CHARACTER;
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
	CommonButton->SetIsSelected(true);
	
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
	FString Name = CommonButton->Name;
	if (Name == STORAGE_TYPE_ALL)
	{
		AddEquipmentButton(FilterEquipment(Name));
		AddCharacterButton(FilterHumanCharacter());
	}
	else if (Name == STORAGE_TYPE_CHARACTER)
	{
		AddCharacterButton(FilterHumanCharacter());
	}
	else
	{
		AddEquipmentButton(FilterEquipment(Name));
	}
}

// 根据装备类型筛选出拥有的装备名称
TArray<FText> UStorage::FilterEquipment(FString EquipmentType)
{
	TArray<FText> EquipmentNames;

	for (const TPair<FDataRegistryId, const uint8*>& Pair : EquipmentMains)
	{
		FEquipmentMain ItemValue = *reinterpret_cast<const FEquipmentMain*>(Pair.Value);

		FString EnumValue = UEnum::GetValueAsString(ItemValue.EquipmentType);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

		if (EquipmentType == STORAGE_TYPE_ALL || EnumValue == EquipmentType) // 对应的装备类型
		{
			if (ItemValue.AudienceItemId.IsEmpty()) // 免费装备直接添加
			{
				EquipmentNames.Add(FText::FromName(Pair.Key.ItemName));
			}
			else // 付费装备判断是否已拥有
			{
				if (EOSSubsystem && EOSSubsystem->OwnEntitlement(ItemValue.AudienceItemId))
				{
					EquipmentNames.Add(FText::FromName(Pair.Key.ItemName));
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

	for (const TPair<FDataRegistryId, const uint8*>& Pair : HumanCharacterMains)
	{
		FHumanCharacterMain ItemValue = *reinterpret_cast<const FHumanCharacterMain*>(Pair.Value);

		if (ItemValue.AudienceItemId.IsEmpty()) // 免费角色直接添加
		{
			HumanCharacterNames.Add(FText::FText::FromName(Pair.Key.ItemName));
		}
		else // 付费角色判断是否已拥有
		{
			if (EOSSubsystem && EOSSubsystem->OwnEntitlement(ItemValue.AudienceItemId))
			{
				HumanCharacterNames.Add(FText::FText::FromName(Pair.Key.ItemName));
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

	for (const TPair<FDataRegistryId, const uint8*>& Pair : EquipmentMains)
	{
		FEquipmentMain ItemValue = *reinterpret_cast<const FEquipmentMain*>(Pair.Value);

		if (Pair.Key.ItemName == EquipmentName)
		{
			SaveBag(ItemValue.EquipmentType, EquipmentName);
			break;
		}
	}
}

// 保存背包
void UStorage::SaveBag(EEquipmentType EquipmentType, FString EquipmentName)
{
	if (UBagContent* ActiveBag = Cast<UBagContent>(BagSwitcher->GetActiveWidget()))
	{
		switch (EquipmentType)
		{
		case EEquipmentType::Primary:
			ActiveBag->PrimaryEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			break;
		case EEquipmentType::Secondary:
			ActiveBag->SecondaryEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			break;
		case EEquipmentType::Melee:
			ActiveBag->MeleeEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			break;
		case EEquipmentType::Throwing:
			ActiveBag->ThrowingEquipment->ButtonText->SetText(FText::FromString(EquipmentName));
			break;
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
	if (StorageSubsystem && StorageSubsystem->CacheLoadout)
	{
		StorageSubsystem->CacheLoadout->Bags = Bags;
		StorageSubsystem->SaveLoadout();
	}
}

// 点击角色按钮
void UStorage::OnCharacterButtonClicked(UStorageButton* EquipmentButton)
{
	// 保存角色
	FText HumanCharacterName = EquipmentButton->ButtonText->GetText();
	Character->SetText(HumanCharacterName);

	// 保存角色到存档
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheLoadout)
	{
		if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
		{
			FDataRegistryId DataRegistryId(DR_HUMAN_CHARACTER_MAIN, FName(HumanCharacterName.ToString()));
			if (const FHumanCharacterMain* HumanCharacterMain = DRSubsystem->GetCachedItem<FHumanCharacterMain>(DataRegistryId))
			{
				StorageSubsystem->CacheLoadout->HumanCharacterName = HumanCharacterMain->HumanCharacterName;
				StorageSubsystem->SaveLoadout();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
