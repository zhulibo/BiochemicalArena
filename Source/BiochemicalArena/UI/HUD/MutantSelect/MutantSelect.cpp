#include "MutantSelect.h"

#include "AbilitySystemComponent.h"
#include "MutantSelectButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "DataRegistrySubsystem.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/Characters/Data/CharacterAsset.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/UI/HUD/Mutation/MutationContainer.h"
#include "Components/ScrollBoxSlot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/Abilities/Mutant/GameplayAbility_MutantChange.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/System/Storage/SaveGameLoadout.h"
#include "BiochemicalArena/UI/GameLayout.h"
#include "BiochemicalArena/Utils/LibraryCommon.h"
#include "BiochemicalArena/Utils/LibraryNotify.h"

#define LOCTEXT_NAMESPACE "UMutantSelect"

void UMutantSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MutationGameState == nullptr) MutationGameState = GetWorld()->GetGameState<AMutationGameState>();
	if (MutationGameState)
	{
		MutationGameState->OnRoundEnded.AddUObject(this, &ThisClass::OnRoundEnded);
	}

	if (UDataRegistry* DataRegistry = UDataRegistrySubsystem::Get()->GetRegistryForType(DR_MUTANT_CHARACTER_MAIN))
	{
		const UScriptStruct* OutStruct;
		DataRegistry->GetAllCachedItems(MutantCharacterMain, OutStruct);

		for (const TPair<FDataRegistryId, const uint8*>& Pair : MutantCharacterMain)
		{
			FMutantCharacterMain ItemValue = *reinterpret_cast<const FMutantCharacterMain*>(Pair.Value);

			if (UMutantSelectButton* MutantSelectButton = CreateWidget<UMutantSelectButton>(this, MutantSelectButtonClass))
			{
				FText CharacterNameText = FText();
				FString MutantCharacterName = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(ItemValue.MutantCharacterName));
				FText::FindTextInLiveTable_Advanced(CULTURE_MUTANT, MutantCharacterName, CharacterNameText);
				MutantSelectButton->CharacterNameText->SetText(CharacterNameText);

				MutantSelectButton->CharacterDescText->SetText(FText::FromString(ItemValue.Desc));

				MutantSelectButton->OnClicked().AddUObject(this, &ThisClass::OnMutantSelectButtonClicked, ItemValue.MutantCharacterName);

				UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(MutantSelectButtonContainer->AddChild(MutantSelectButton));
				if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
			}
		}
	}
}

UWidget* UMutantSelect::NativeGetDesiredFocusTarget() const
{
	if (AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn()))
	{
		FString EnumValue = ULibraryCommon::GetEnumValue(UEnum::GetValueAsString(MutantCharacter->MutantCharacterName));

		for (int i = 0; i < MutantSelectButtonContainer->GetChildrenCount(); ++i)
		{
			if (UMutantSelectButton* MutantSelectButton = Cast<UMutantSelectButton>(MutantSelectButtonContainer->GetChildAt(i)))
			{
				if (MutantSelectButton->CharacterNameText->GetText().ToString() == EnumValue)
				{
					return MutantSelectButtonContainer->GetChildAt(i);
				}
			}
		}
	}

	return MutantSelectButtonContainer->GetChildAt(0);
}

void UMutantSelect::OnMutantSelectButtonClicked(EMutantCharacterName MutantCharacterName)
{
	// 保存到PlayerState
	if (BasePlayerState == nullptr) BasePlayerState = Cast<ABasePlayerState>(GetOwningPlayerState());
	if (BasePlayerState)
	{
		BasePlayerState->ServerSetMutantCharacterName(MutantCharacterName);
	}

	// 保存到文件
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheLoadout)
	{
		StorageSubsystem->CacheLoadout->MutantCharacterName = MutantCharacterName;
		StorageSubsystem->SaveLoadouts();
	}

	// 切换角色
	if (AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn()))
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TAG_MUTANT_CHANGE_ACTIVE);

		if (AbilitySystemComponent == nullptr) AbilitySystemComponent = MutantCharacter->GetAbilitySystemComponent();
		if (AbilitySystemComponent && AbilitySystemComponent->GetTagCount(Tag) > 0)
		{
			MutantCharacter->ServerSelectMutant(MutantCharacterName);

			// 终止切换角色技能
			if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
			if (AssetSubsystem && AssetSubsystem->CharacterAsset)
			{
				if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AssetSubsystem->CharacterAsset->MutantChangeAbility))
				{
					AbilitySystemComponent->CancelAbility(Spec->Ability);
				}
			}
		}
		else
		{
			NOTIFY(this, C_GREEN, LOCTEXT("MutantChangeApply", "Mutant change will apply in the next round"));
		}
	}
	else
	{
		NOTIFY(this, C_GREEN, LOCTEXT("MutantChangeSave", "Mutant change have saved"));
	}

	CloseMenu(true);
}

void UMutantSelect::OnRoundEnded()
{
	if (IsActivated())
	{
		CloseMenu(true);
	}
}

void UMutantSelect::CloseMenu(bool bClosePauseMenu)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		DeactivateWidget();

		if (bClosePauseMenu && BaseController->GameLayout)
		{
			BaseController->GameLayout->MenuStack->RemoveWidget(*BaseController->GameLayout->MenuStack->GetActiveWidget());
		}
	}
}

#undef LOCTEXT_NAMESPACE
