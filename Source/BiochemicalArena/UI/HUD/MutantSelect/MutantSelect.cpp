#include "MutantSelect.h"

#include "AbilitySystemComponent.h"
#include "MutantSelectButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "DataRegistrySubsystem.h"
#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/Characters/Data/MutantCommon.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "Components/ScrollBoxSlot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BiochemicalArena/Abilities/GameplayAbility_ChangeMutant.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"

void UMutantSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (MutationGameState == nullptr) MutationGameState = GetWorld()->GetGameState<AMutationGameState>();
	if (MutationGameState)
	{
		MutationGameState->OnRoundEnded.AddUObject(this, &ThisClass::OnRoundEnded);
	}

	if (UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get())
	{
		if (UDataRegistry* DataRegistry = DRSubsystem->GetRegistryForType(DR_MutantCharacterMain))
		{
			const UScriptStruct* OutStruct;
			DataRegistry->GetAllCachedItems(MutantCharacterMain, OutStruct);

			for (const TPair<FDataRegistryId, const uint8*>& Pair : MutantCharacterMain)
			{
				FMutantCharacterMain ItemValue = *reinterpret_cast<const FMutantCharacterMain*>(Pair.Value);

				UMutantSelectButton* MutantSelectButton = CreateWidget<UMutantSelectButton>(this, MutantSelectButtonClass);
				if (MutantSelectButton)
				{
					MutantSelectButton->CharacterNameText->SetText(FText::FromName(Pair.Key.ItemName));
					MutantSelectButton->CharacterDescText->SetText(FText::FromString(ItemValue.Desc));

					MutantSelectButton->OnClicked().AddUObject(this, &ThisClass::OnMutantSelectButtonClicked, ItemValue.MutantCharacterName);

					UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(MutantSelectButtonContainer->AddChild(MutantSelectButton));
					if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
				}
			}
		}
	}
}

UWidget* UMutantSelect::NativeGetDesiredFocusTarget() const
{
	if (AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn()))
	{
		FString EnumValue = UEnum::GetValueAsString(MutantCharacter->MutantCharacterName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

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
		BasePlayerState->SetMutantCharacterName(MutantCharacterName);
	}

	// 保存到文件
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->MutantCharacterName = MutantCharacterName;
		StorageSubsystem->Save();
	}

	// 切换角色
	if (AMutantCharacter* MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn()))
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(TAG_MUTANT_CHANGE_ACTIVE);

		if (AbilitySystemComponent == nullptr) AbilitySystemComponent = MutantCharacter->GetAbilitySystemComponent();
		if (AbilitySystemComponent && AbilitySystemComponent->GetTagCount(Tag) > 0)
		{
			MutantCharacter->ServerSelectCharacter(MutantCharacterName);

			// 终止切换角色技能
			if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
			if (AssetSubsystem && AssetSubsystem->MutantCommon)
			{
				if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AssetSubsystem->MutantCommon->ChangeMutantAbility))
				{
					AbilitySystemComponent->CancelAbility(Spec->Ability);
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, C_GREEN, TEXT("Mutant change will apply in the next round!"), false);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, C_GREEN, TEXT("Mutant change have saved!"), false);
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

void UMutantSelect::CloseMenu(bool bCloseAll)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController && BaseController->HUDContainer)
	{
		BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
		if (bCloseAll)
		{
			BaseController->HUDContainer->MainStack->RemoveWidget(*BaseController->HUDContainer->MainStack->GetActiveWidget());
			FInputModeGameOnly InputModeData;
			BaseController->SetInputMode(InputModeData);
			BaseController->SetShowMouseCursor(false);
		}
	}
}
