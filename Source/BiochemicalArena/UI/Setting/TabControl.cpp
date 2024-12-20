#include "TabControl.h"
#include "AnalogSlider.h"
#include "CommonButtonBase.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/System/Storage/SaveGameSetting.h"
#include "BiochemicalArena/System/Storage/DefaultConfig.h"
#include "BiochemicalArena/System/Storage/StorageSubsystem.h"
#include "Components/ComboBoxString.h"
#include "Input/CommonUIInputTypes.h"

void UTabControl::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定提示信息切换菜单
	TArray<UWidget*> Tabs = SettingLeft->GetAllChildren();
	for (int i = 0; i < Tabs.Num(); ++i)
	{
		if (UCommonButtonBase* TabButton = Cast<UCommonButtonBase>(Tabs[i]))
		{
			TabButton->OnHovered().AddUObject(this, &ThisClass::OnTabButtonHovered, i);
		}
	}

	MouseAimAssistSteeringComboBox->AddOption("on");
	MouseAimAssistSteeringComboBox->AddOption("off");
	MouseAimAssistSlowdownComboBox->AddOption("on");
	MouseAimAssistSlowdownComboBox->AddOption("off");

	ControllerAimAssistSteeringComboBox->AddOption("on");
	ControllerAimAssistSteeringComboBox->AddOption("off");
	ControllerAimAssistSlowdownComboBox->AddOption("on");
	ControllerAimAssistSlowdownComboBox->AddOption("off");

	// TODO 辅助瞄准
	MouseAimAssistSteeringComboBox->SetIsEnabled(false);
	MouseAimAssistSlowdownComboBox->SetIsEnabled(false);
	ControllerAimAssistSteeringComboBox->SetIsEnabled(false);
	ControllerAimAssistSlowdownComboBox->SetIsEnabled(false);
	
	SetUISavedValue();

	MouseSensitivityAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnMouseSensitivityChanged);
	MouseAimAssistSteeringComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSteeringChanged);
	MouseAimAssistSlowdownComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnMouseAimAssistSlowdownChanged);
	ControllerSensitivityAnalogSlider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::OnControllerSensitivityChanged);
	ControllerAimAssistSteeringComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSteeringChanged);
	ControllerAimAssistSlowdownComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnControllerAimAssistSlowdownChanged);

	SetDefaultHandle = RegisterUIActionBinding(FBindUIActionArgs(SetDefaultData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::SetDefault)));
}

void UTabControl::NativeConstruct()
{
	Super::NativeConstruct();

	// 默认隐藏所有提示信息
	TArray<UWidget*> Contents = SettingRight->GetAllChildren();
	for (int i = 0; i < Contents.Num(); ++i)
	{
		Contents[i]->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UWidget* UTabControl::NativeGetDesiredFocusTarget() const
{
	return MouseSensitivityAnalogSlider;
}

void UTabControl::OnTabButtonHovered(int Index)
{
	TArray<UWidget*> Contents = SettingRight->GetAllChildren();
	for (int i = 0; i < Contents.Num(); ++i)
	{
		Contents[i]->SetVisibility(i == Index ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UTabControl::SetUISavedValue()
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		int32 MouseSensitivityInverse = FMath::RoundToInt(InverseMapSensitivityMouse(StorageSubsystem->CacheSetting->MouseSensitivity));
		MouseSensitivityAnalogSlider->SetValue(MouseSensitivityInverse);
		MouseSensitivity->SetText(FText::AsNumber(MouseSensitivityInverse));
		MouseAimAssistSteeringComboBox->SetSelectedOption(StorageSubsystem->CacheSetting->MouseAimAssistSteering ? "on" : "off");
		MouseAimAssistSlowdownComboBox->SetSelectedOption(StorageSubsystem->CacheSetting->MouseAimAssistSlowdown ? "on" : "off");

		int32 ControllerSensitivityInverse = FMath::RoundToInt(InverseMapSensitivityController(StorageSubsystem->CacheSetting->ControllerSensitivity));
		ControllerSensitivityAnalogSlider->SetValue(ControllerSensitivityInverse);
		ControllerSensitivity->SetText(FText::AsNumber(ControllerSensitivityInverse));
		ControllerAimAssistSteeringComboBox->SetSelectedOption(StorageSubsystem->CacheSetting->ControllerAimAssistSteering ? "on" : "off");
		ControllerAimAssistSlowdownComboBox->SetSelectedOption(StorageSubsystem->CacheSetting->ControllerAimAssistSlowdown ? "on" : "off");
	}
}

float UTabControl::MapSensitivityMouse(float Value)
{
	float Sensitivity;

	if (Value < 50.f)
	{
		FVector2D OutRange(SensitivityMouseMin, SensitivityMouseMiddle);
		Sensitivity = FMath::GetMappedRangeValueClamped(Range_1_50, OutRange, Value);
	}
	else if (Value > 50.f)
	{
		FVector2D OutRange(SensitivityMouseMiddle, SensitivityMouseMax);
		Sensitivity = FMath::GetMappedRangeValueClamped(Range_50_100, OutRange, Value);
	}
	else
	{
		Sensitivity = SensitivityMouseMiddle;
	}

	return Sensitivity;
}

float UTabControl::InverseMapSensitivityMouse(float Value)
{
	float Sensitivity;

	if (Value < SensitivityMouseMiddle)
	{
		FVector2D InRange(SensitivityMouseMin, SensitivityMouseMiddle);
		Sensitivity = FMath::GetMappedRangeValueClamped(InRange, Range_1_50, Value);
	}
	else if (Value > SensitivityMouseMiddle)
	{
		FVector2D InRange(SensitivityMouseMiddle, SensitivityMouseMax);
		Sensitivity = FMath::GetMappedRangeValueClamped(InRange, Range_50_100, Value);
	}
	else
	{
		Sensitivity = 50.f;
	}

	return Sensitivity;
}

float UTabControl::MapSensitivityController(float Value)
{
	float Sensitivity;

	if (Value < 50.f)
	{
		FVector2D OutRange(SensitivityControllerMin, SensitivityControllerMiddle);
		Sensitivity = FMath::GetMappedRangeValueClamped(Range_1_50, OutRange, Value);
	}
	else if (Value > 50.f)
	{
		FVector2D OutRange(SensitivityControllerMiddle, SensitivityControllerMax);
		Sensitivity = FMath::GetMappedRangeValueClamped(Range_50_100, OutRange, Value);
	}
	else
	{
		Sensitivity = SensitivityControllerMiddle;
	}

	return Sensitivity;
}

float UTabControl::InverseMapSensitivityController(float Value)
{
	float Sensitivity;

	if (Value < SensitivityControllerMiddle)
	{
		FVector2D InRange(SensitivityControllerMin, SensitivityControllerMiddle);
		Sensitivity = FMath::GetMappedRangeValueClamped(InRange, Range_1_50, Value);
	}
	else if (Value > SensitivityControllerMiddle)
	{
		FVector2D InRange(SensitivityControllerMiddle, SensitivityControllerMax);
		Sensitivity = FMath::GetMappedRangeValueClamped(InRange, Range_50_100, Value);
	}
	else
	{
		Sensitivity = 50.f;
	}

	return Sensitivity;
}

void UTabControl::OnMouseSensitivityChanged(float Value)
{
	MouseSensitivity->SetText(FText::AsNumber(FMath::RoundToFloat(Value)));

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->MouseSensitivity = MapSensitivityMouse(Value);
		StorageSubsystem->SaveSetting();
	}
}

void UTabControl::OnMouseAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->MouseAimAssistSteering = SelectedItem == "on";
		StorageSubsystem->SaveSetting();
	}
}

void UTabControl::OnMouseAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->MouseAimAssistSlowdown = SelectedItem == "on";
		StorageSubsystem->SaveSetting();
	}
}

void UTabControl::OnControllerSensitivityChanged(float Value)
{
	ControllerSensitivity->SetText(FText::AsNumber(FMath::RoundToFloat(Value)));

	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->ControllerSensitivity = MapSensitivityController(Value);
		StorageSubsystem->SaveSetting();
	}
}

void UTabControl::OnControllerAimAssistSteeringChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->ControllerAimAssistSteering = SelectedItem == "on";
		StorageSubsystem->SaveSetting();
	}
}
void UTabControl::OnControllerAimAssistSlowdownChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
	if (StorageSubsystem && StorageSubsystem->CacheSetting)
	{
		StorageSubsystem->CacheSetting->ControllerAimAssistSlowdown = SelectedItem == "on";
		StorageSubsystem->SaveSetting();
	}
}

void UTabControl::SetDefault()
{
	if (const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>())
	{
		if (StorageSubsystem == nullptr) StorageSubsystem = GetGameInstance()->GetSubsystem<UStorageSubsystem>();
		if (StorageSubsystem && StorageSubsystem->CacheSetting)
		{
			int32 MouseSensitivityInverse = FMath::RoundToInt(InverseMapSensitivityMouse(DefaultConfig->MouseSensitivity));
			MouseSensitivityAnalogSlider->SetValue(MouseSensitivityInverse);
			MouseSensitivity->SetText(FText::AsNumber(MouseSensitivityInverse));
			MouseAimAssistSteeringComboBox->SetSelectedOption(DefaultConfig->MouseAimAssistSteering ? "on" : "off");
			MouseAimAssistSlowdownComboBox->SetSelectedOption(DefaultConfig->MouseAimAssistSlowdown ? "on" : "off");

			int32 ControllerSensitivityInverse = FMath::RoundToInt(InverseMapSensitivityController(DefaultConfig->ControllerSensitivity));
			ControllerSensitivityAnalogSlider->SetValue(ControllerSensitivityInverse);
			ControllerSensitivity->SetText(FText::AsNumber(ControllerSensitivityInverse));
			ControllerAimAssistSteeringComboBox->SetSelectedOption(DefaultConfig->ControllerAimAssistSteering ? "on" : "off");
			ControllerAimAssistSlowdownComboBox->SetSelectedOption(DefaultConfig->ControllerAimAssistSlowdown ? "on" : "off");

			StorageSubsystem->CacheSetting->MouseSensitivity = DefaultConfig->MouseSensitivity;
			StorageSubsystem->CacheSetting->MouseAimAssistSteering = DefaultConfig->MouseAimAssistSteering;
			StorageSubsystem->CacheSetting->MouseAimAssistSlowdown = DefaultConfig->MouseAimAssistSlowdown;
			StorageSubsystem->CacheSetting->ControllerSensitivity = DefaultConfig->ControllerSensitivity;
			StorageSubsystem->CacheSetting->ControllerAimAssistSteering = DefaultConfig->ControllerAimAssistSteering;
			StorageSubsystem->CacheSetting->ControllerAimAssistSlowdown = DefaultConfig->ControllerAimAssistSlowdown;
			StorageSubsystem->SaveSetting();
		}
	}
}
