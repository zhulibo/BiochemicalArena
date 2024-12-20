#include "LibraryNotify.h"

#include "BiochemicalArena/BiochemicalArena.h"
#include "BiochemicalArena/System/PlayerSubsystem.h"

void ULibraryNotify::ShowNotify(const UObject* Context, const FColor DisplayColor, const FText& Msg)
{
	if (Context && Context->GetWorld())
	{
		if (UPlayerSubsystem* PlayerSubsystem = ULocalPlayer::GetSubsystem<UPlayerSubsystem>(Context->GetWorld()->GetFirstLocalPlayerFromController()))
		{
			PlayerSubsystem->ShowNotify(DisplayColor, Msg);

			return;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 20.f, C_RED, TEXT("Show notify failed!"), false);
}
