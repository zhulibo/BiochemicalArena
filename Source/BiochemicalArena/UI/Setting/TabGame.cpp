#include "TabGame.h"

#include "CommonButtonBase.h"
#include "CommonHierarchicalScrollBox.h"
#include "BiochemicalArena/System/Storage/DefaultConfig.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Components/ComboBoxString.h"
#include "GameFramework/GameUserSettings.h"
#include "Input/CommonUIInputTypes.h"
#include "Internationalization/Culture.h"

void UTabGame::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定提示信息切换菜单
	TArray<UWidget*> Tabs = SettingLeft->GetAllChildren();
	for (int i = 0; i < Tabs.Num(); i++)
	{
		if (UCommonButtonBase* TabButton = Cast<UCommonButtonBase>(Tabs[i]))
		{
			TabButton->OnHovered().AddUObject(this, &ThisClass::OnTabButtonHovered, i);
		}
	}

	LanguageComboBox->AddOption("en");
	LanguageComboBox->AddOption("zh");
	LanguageComboBox->AddOption("ja");

	SetUISavedValue();
	
	LanguageComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnLanguageChanged);

	SetDefaultHandle = RegisterUIActionBinding(FBindUIActionArgs(SetDefaultData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::SetDefault)));
}

UWidget* UTabGame::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}

void UTabGame::OnTabButtonHovered(int Index)
{
	TArray<UWidget*> Contents = SettingRight->GetAllChildren();
	for (int i = 0; i < Contents.Num(); i++)
	{
		Contents[i]->SetVisibility(i == Index ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UTabGame::SetUISavedValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		// 在游戏中修改语言为zh时，编辑器语言会变成英文，可能跟语言回退有关（编辑器支持zh-Hans/zh-Hant，无法回退到zh）
		if (GetWorld()->WorldType != EWorldType::PIE)
		{
			LanguageComboBox->SetSelectedOption(StorageSubsystem->CacheSetting->Language);
		}
	}
}

void UTabGame::OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FInternationalization::Get().SetCurrentCulture(SelectedItem);
	
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->Language = SelectedItem;
		StorageSubsystem->Save();
	}
}

void UTabGame::SetDefault()
{
	if (const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>())
	{
		if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
		if (StorageSubsystem && StorageSubsystem->CacheSetting)
		{
			LanguageComboBox->SetSelectedOption(DefaultConfig->Language);

			StorageSubsystem->CacheSetting->Language = DefaultConfig->Language;
			StorageSubsystem->Save();
		}
	}
}
