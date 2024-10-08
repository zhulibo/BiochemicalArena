#include "TabVideo.h"
#include "AnalogSlider.h"
#include "CommonButtonBase.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/DefaultConfig.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Components/ComboBoxString.h"
#include "GameFramework/GameUserSettings.h"
#include "Input/CommonUIInputTypes.h"

void UTabVideo::NativeOnInitialized()
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

	// 设置全屏模式下拉项
	WindowModeComboBox->AddOption("Fullscreen");
	WindowModeComboBox->AddOption("WindowedFullscreen");
	WindowModeComboBox->AddOption("Windowed");

	// 设置分辨率下拉项
	FIntPoint BaseResolution = FIntPoint(1280, 720);
	if (const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>())
	{
		BaseResolution = DefaultConfig->ScreenResolution;
	}

	FScreenResolutionArray TempResolutions;
	RHIGetAvailableResolutions(TempResolutions, true);
	for (const FScreenResolutionRHI& Resolution : TempResolutions)
	{
		if (Resolution.Width >= static_cast<uint32>(BaseResolution.X) && Resolution.Height >= static_cast<uint32>(BaseResolution.Y))
		{
			Resolutions.Add(FIntPoint(Resolution.Width, Resolution.Height));
			ScreenResolutionComboBox->AddOption(FString::FromInt(Resolution.Width) + "x" + FString::FromInt(Resolution.Height));
		}
	}

	if (Resolutions.Num() == 0)
	{
		Resolutions.Add(FIntPoint(BaseResolution.X, BaseResolution.Y));
		ScreenResolutionComboBox->AddOption(FString::FromInt(BaseResolution.X) + "x" + FString::FromInt(BaseResolution.Y));
	}

	// 设置保存值
	SetUISavedValue();

	// 绑定值变化回调
	WindowModeComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnWindowModeChanged);
	ScreenResolutionComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnScreenResolutionChanged);
	BrightnessAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnBrightnessChanged);

	SetDefaultHandle = RegisterUIActionBinding(FBindUIActionArgs(SetDefaultData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::SetDefault)));
}

void UTabVideo::NativeConstruct()
{
	Super::NativeConstruct();

	// 默认隐藏所有提示信息
	TArray<UWidget*> Contents = SettingRight->GetAllChildren();
	for (int i = 0; i < Contents.Num(); i++)
	{
		Contents[i]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UWidget* UTabVideo::NativeGetDesiredFocusTarget() const
{
	return WindowModeComboBox;
}

void UTabVideo::OnTabButtonHovered(int Index)
{
	TArray<UWidget*> Contents = SettingRight->GetAllChildren();
	for (int i = 0; i < Contents.Num(); i++)
	{
		Contents[i]->SetVisibility(i == Index ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UTabVideo::SetUISavedValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		if (GameUserSettings == nullptr) GameUserSettings = GetMutableDefault<UGameUserSettings>();
		if (GameUserSettings)
		{
			switch (GameUserSettings->GetFullscreenMode())
			{
			case EWindowMode::Fullscreen:
				WindowModeComboBox->SetSelectedOption("Fullscreen");
				break;
			case EWindowMode::WindowedFullscreen:
				WindowModeComboBox->SetSelectedOption("WindowedFullscreen");
				break;
			case EWindowMode::Windowed:
				WindowModeComboBox->SetSelectedOption("Windowed");
				break;
			}

			FIntPoint CurResolution = GameUserSettings->GetScreenResolution();
			FString CurResolutionStr = FString::FromInt(CurResolution.X) + "x" + FString::FromInt(CurResolution.Y);
			if (Resolutions.Contains(CurResolution))
			{
				ScreenResolutionComboBox->SetSelectedOption(CurResolutionStr);
			}
		}

		BrightnessAnalogSlider->SetValue(StorageSubsystem->CacheSetting->Brightness);
		Brightness->SetText(FText::AsNumber(StorageSubsystem->CacheSetting->Brightness));
	}
}

void UTabVideo::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	EWindowMode::Type WindowMode = EWindowMode::WindowedFullscreen;

	if (SelectedItem == FString("Fullscreen"))
	{
		WindowMode = EWindowMode::Fullscreen;
	}
	else if (SelectedItem == FString("WindowedFullscreen"))
	{
		WindowMode = EWindowMode::WindowedFullscreen;
	}
	else if (SelectedItem == FString("Windowed"))
	{
		WindowMode = EWindowMode::Windowed;
	}

	if (GameUserSettings == nullptr) GameUserSettings = GetMutableDefault<UGameUserSettings>();
	if (GameUserSettings)
	{
		GameUserSettings->SetFullscreenMode(WindowMode);
		GameUserSettings->ApplyResolutionSettings(false);
		GameUserSettings->SaveSettings();
	}

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->WindowMode = WindowMode;
		StorageSubsystem->Save();
	}
}

void UTabVideo::OnScreenResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FIntPoint Resolution = Resolutions[ScreenResolutionComboBox->FindOptionIndex(SelectedItem)];

	if (GameUserSettings == nullptr) GameUserSettings = GetMutableDefault<UGameUserSettings>();
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(Resolution);
		GameUserSettings->ApplyResolutionSettings(false);
		GameUserSettings->SaveSettings();
	}

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->ScreenResolution = Resolution;
		StorageSubsystem->Save();
	}
}

void UTabVideo::OnBrightnessChanged(float Value)
{
	Value = FMath::RoundToFloat(Value * 10) / 10;

	Brightness->SetText(FText::AsNumber(Value));

	GEngine->DisplayGamma = Value;

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->Brightness = Value;
		StorageSubsystem->Save();
	}
}

void UTabVideo::SetDefault()
{
	if (const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>())
	{
		if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
		if (StorageSubsystem && StorageSubsystem->CacheSetting)
		{
			switch (DefaultConfig->WindowMode)
			{
			case EWindowMode::Fullscreen:
				WindowModeComboBox->SetSelectedOption("Fullscreen");
				break;
			case EWindowMode::WindowedFullscreen:
				WindowModeComboBox->SetSelectedOption("WindowedFullscreen");
				break;
			case EWindowMode::Windowed:
				WindowModeComboBox->SetSelectedOption("Windowed");
				break;
			}

			ScreenResolutionComboBox->SetSelectedIndex(ScreenResolutionComboBox->GetOptionCount() - 1);

			if (GameUserSettings == nullptr) GameUserSettings = GetMutableDefault<UGameUserSettings>();
			if (GameUserSettings)
			{
				GameUserSettings->SetFullscreenMode(DefaultConfig->WindowMode);
				GameUserSettings->SetScreenResolution(DefaultConfig->ScreenResolution);
				GameUserSettings->ApplyResolutionSettings(false);
				GameUserSettings->SaveSettings();
			}

			BrightnessAnalogSlider->SetValue(DefaultConfig->Brightness);
			Brightness->SetText(FText::AsNumber(DefaultConfig->Brightness));

			StorageSubsystem->CacheSetting->WindowMode = DefaultConfig->WindowMode;
			StorageSubsystem->CacheSetting->ScreenResolution = DefaultConfig->ScreenResolution;
			StorageSubsystem->CacheSetting->Brightness = DefaultConfig->Brightness;
			StorageSubsystem->Save();
		}
	}
}
