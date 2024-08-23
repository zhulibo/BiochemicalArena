#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BagMenu.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UBagMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class ABaseController* BaseController;
	UPROPERTY()
	class AHumanCharacter* HumanCharacter;
	UPROPERTY()
	class AMutationPlayerState* MutationPlayerState;

	UPROPERTY()
	mutable class UStorageSubsystem* StorageSubsystem;

	UPROPERTY(meta = (BindWidget))
	class UCommonButton* BackButton;
	UFUNCTION()
	void OnBackButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* BagMenuButtonContainer;
	UFUNCTION()
	void OnBagMenuButtonClicked(int32 CurBagIndex);

	void CloseMenu();

	void OnTeamChange(ETeam TempTeam);

};
