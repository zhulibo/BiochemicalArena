#include "Dev.h"

#include "CommonTextBlock.h"
#include "MutateArena/Utils/LibraryCommon.h"

void UDev::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDev::NativeConstruct()
{
	Super::NativeConstruct();

	ProjectVersion->SetText(FText::FromString(ULibraryCommon::GetProjectVersion()));
	
	const FEngineVersion& CurrentVersion = FEngineVersion::Current();
	int32 Major = CurrentVersion.GetMajor();
	int32 Minor = CurrentVersion.GetMinor();
	int32 Patch = CurrentVersion.GetPatch();
	EngineVersion->SetText(FText::FromString(FString::Printf(TEXT("UE %d.%d.%d"), Major, Minor, Patch)));
}

UWidget* UDev::NativeGetDesiredFocusTarget() const
{
	return Super::NativeGetDesiredFocusTarget();
}
