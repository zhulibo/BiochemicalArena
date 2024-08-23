#include "CharacterMenu.h"

#include "CharacterMenuButton.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/Data/CharacterType.h"
#include "BiochemicalArena/Characters/MutantCharacter.h"
#include "BiochemicalArena/GameStates/MutationGameState.h"
#include "BiochemicalArena/PlayerControllers/MutationController.h"
#include "BiochemicalArena/System/StorageSaveGame.h"
#include "BiochemicalArena/System/StorageSubsystem.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "BiochemicalArena/UI/HUD/HUDContainer.h"
#include "Components/ScrollBoxSlot.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UCharacterMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	if (MutationGameState == nullptr) MutationGameState = GetWorld()->GetGameState<AMutationGameState>();
	if (MutationGameState)
	{
		MutationGameState->OnRoundEnded.AddUObject(this, &ThisClass::OnRoundEnded);
	}

	const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/BiochemicalArena.EMutantCharacterName"), true);
	for (int32 i = 0; i < static_cast<int32>(EMutantCharacterName::NONE); ++i)
	{
		UCharacterMenuButton* CharacterMenuButton = CreateWidget<UCharacterMenuButton>(this, CharacterMenuButtonClass);
		if (CharacterMenuButton)
		{
			FString EnumValue = UEnum::GetValueAsString(static_cast<EMutantCharacterName>(i));
			EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);
			CharacterMenuButton->CharacterNameText->SetText(FText::FromString(EnumValue));

			FString EnumDesc = "";
			if (EnumPtr) EnumDesc = EnumPtr->GetMetaData(TEXT("Desc"), i);
			CharacterMenuButton->CharacterDescText->SetText(FText::FromString(EnumDesc));

			CharacterMenuButton->OnClicked().AddUObject(this, &ThisClass::OnCharacterMenuButtonClicked, i);
			CharacterMenuButtonContainer->AddChild(CharacterMenuButton);
			UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(CharacterMenuButtonContainer->AddChild(CharacterMenuButton));
			if (NewSlot) NewSlot->SetPadding(FMargin(10, 0, 10, 0));
		}
	}
}

UWidget* UCharacterMenu::NativeGetDesiredFocusTarget() const
{
	if (MutantCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
	if (MutantCharacter)
	{
		FString EnumValue = UEnum::GetValueAsString(MutantCharacter->MutantCharacterName);
		EnumValue = EnumValue.Right(EnumValue.Len() - EnumValue.Find("::") - 2);

		for (int i = 0; i < CharacterMenuButtonContainer->GetChildrenCount(); ++i)
		{
			if (UCharacterMenuButton* CharacterMenuButton = Cast<UCharacterMenuButton>(CharacterMenuButtonContainer->GetChildAt(i)))
			{
				if (CharacterMenuButton->CharacterNameText->GetText().ToString() == EnumValue)
				{
					return CharacterMenuButtonContainer->GetChildAt(i);
				}
			}
		}
	}

	return CharacterMenuButtonContainer->GetChildAt(0);
}

void UCharacterMenu::OnBackButtonClicked()
{
	CloseMenu();
}

void UCharacterMenu::CloseMenu()
{
	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwningPlayer());
	if (MutationController && MutationController->HUDContainer)
	{
		MutationController->HUDContainer->MainStack->RemoveWidget(*MutationController->HUDContainer->MainStack->GetActiveWidget());

		FInputModeGameOnly InputModeData;
		MutationController->SetInputMode(InputModeData);
		MutationController->SetShowMouseCursor(false);
	}
}

void UCharacterMenu::OnCharacterMenuButtonClicked(int32 EnumIndex)
{
	EMutantCharacterName MutantCharacterName = static_cast<EMutantCharacterName>(EnumIndex);

	if (MutantCharacter == nullptr) MutantCharacter = Cast<AMutantCharacter>(GetOwningPlayerPawn());
	if (MutantCharacter)
	{
		MutantCharacter->ServerSelectCharacter(MutantCharacterName);
	}

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->StorageCache)
	{
		StorageSubsystem->StorageCache->MutantCharacterName = MutantCharacterName;
		StorageSubsystem->Save();
	}

	if (MutationController == nullptr) MutationController = Cast<AMutationController>(GetOwningPlayer());
	if (MutationController)
	{
		MutationController->DisableSelectCharacter();
	}

	CloseMenu();
}

void UCharacterMenu::OnRoundEnded()
{
	if (IsActivated())
	{
		CloseMenu();
	}
}
