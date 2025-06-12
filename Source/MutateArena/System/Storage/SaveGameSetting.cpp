#include "SaveGameSetting.h"

#include "DefaultConfig.h"

USaveGameSetting::USaveGameSetting()
{
	const UDefaultConfig* DefaultConfig = GetDefault<UDefaultConfig>();

	if (DefaultConfig == nullptr) return;

	Language = DefaultConfig->Language;
	ObfuscatePlayerName = DefaultConfig->ObfuscatePlayerName;
	ObfuscateTextChat = DefaultConfig->ObfuscateTextChat;

	MouseSensitivity = DefaultConfig->MouseSensitivity;
	MouseAimAssistSteering = DefaultConfig->MouseAimAssistSteering;
	MouseAimAssistSlowdown = DefaultConfig->MouseAimAssistSlowdown;
	ControllerSensitivity = DefaultConfig->ControllerSensitivity;
	ControllerAimAssistSteering = DefaultConfig->ControllerAimAssistSteering;
	ControllerAimAssistSlowdown = DefaultConfig->ControllerAimAssistSlowdown;

	WindowMode = DefaultConfig->WindowMode;
	ScreenResolution = DefaultConfig->ScreenResolution;
	Brightness = DefaultConfig->Brightness;

	MasterVolume = DefaultConfig->MasterVolume;
	EffectsVolume = DefaultConfig->EffectsVolume;
	MusicVolume = DefaultConfig->MusicVolume;
	DialogueVolume = DefaultConfig->DialogueVolume;
}
