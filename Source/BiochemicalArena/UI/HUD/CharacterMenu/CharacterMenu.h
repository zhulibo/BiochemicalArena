#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "CharacterMenu.generated.h"

enum class EMutantCharacterName : uint8;

UCLASS()
class BIOCHEMICALARENA_API UCharacterMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class AMutationController* MutationController;
	UPROPERTY()
	mutable class AMutantCharacter* MutantCharacter;
	UPROPERTY()
	class AMutationGameState* MutationGameState;
	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();
	void CloseMenu();

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* CharacterMenuButtonContainer;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterMenuButton> CharacterMenuButtonClass;
	UFUNCTION()
	void OnCharacterMenuButtonClicked(int32 EnumIndex);

	void OnRoundEnded();

};
