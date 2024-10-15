#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "TabGame.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UTabGame : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UPROPERTY()
	class UStorageSubsystem* StorageSubsystem;
	UPROPERTY()
	UGameUserSettings* GameUserSettings;

	UPROPERTY(meta = (BindWidget))
	class UCommonHierarchicalScrollBox* SettingLeft;
	UPROPERTY(meta = (BindWidget))
	UCommonHierarchicalScrollBox* SettingRight;
	void OnTabButtonHovered(int Index);

	void SetUISavedValue();

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* LanguageComboBox;
	UFUNCTION()
	void OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle SetDefaultData;
	FUIActionBindingHandle SetDefaultHandle;
	void SetDefault();
	
};
