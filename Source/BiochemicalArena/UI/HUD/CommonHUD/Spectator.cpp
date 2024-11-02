#include "Spectator.h"

#include "CommonTextBlock.h"
#include "BiochemicalArena/Characters/BaseCharacter.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/PlayerStates/BasePlayerState.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"

#define LOCTEXT_NAMESPACE "USpectator"

void USpectator::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BaseController = Cast<ABaseController>(GetOwningPlayer());
	if (BaseController)
	{
		BaseController->OnViewTargetChange.AddUObject(this, &ThisClass::OnViewTargetChange);
	}
}

void USpectator::OnViewTargetChange(AActor* ViewTarget)
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(ViewTarget))
	{
		if (ABasePlayerState* BasePlayerState = Cast<ABasePlayerState>(BaseCharacter->GetPlayerState()))
		{
			CurPlayer->SetText(FText::FromString(BasePlayerState->GetPlayerName()));
		}
	}
}

#undef LOCTEXT_NAMESPACE
