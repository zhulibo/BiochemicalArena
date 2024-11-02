#include "SpectatorCharacter.h"

#include "CommonInputSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "Data/InputBase.h"
#include "Data/InputSpectator.h"
#include "GameFramework/GameMode.h"

void ASpectatorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASpectatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputBase == nullptr || InputSpectator == nullptr) return;
	
	if (ABaseGameState* BaseGameState = Cast<ABaseGameState>(GetWorld()->GetGameState()))
	{
		// 还未生成角色时会默认生成ASpectator，避免绑定输入。
		if (BaseGameState->GetMatchState() != MatchState::EnteringMap && BaseGameState->GetMatchState() != MatchState::WaitingToStart)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
				{
					Subsystem->AddMappingContext(InputBase->BaseMappingContext, 0);
					Subsystem->AddMappingContext(InputSpectator->SpectatorMappingContext, 1);
				}
			}

			if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
			{
				EnhancedInputComponent->BindAction(InputBase->ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::ScoreboardButtonPressed);
				EnhancedInputComponent->BindAction(InputBase->ScoreboardAction, ETriggerEvent::Completed, this, &ThisClass::ScoreboardButtonReleased);
				EnhancedInputComponent->BindAction(InputBase->PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::PauseMenuButtonPressed);
				EnhancedInputComponent->BindAction(InputBase->TextChatAction, ETriggerEvent::Triggered, this, &ThisClass::TextChat);
		
				EnhancedInputComponent->BindAction(InputSpectator->ViewNextAction, ETriggerEvent::Triggered, this, &ThisClass::ViewNextPlayer);
				EnhancedInputComponent->BindAction(InputSpectator->ViewPrevAction, ETriggerEvent::Triggered, this, &ThisClass::ViewPrevPlayer);
			}
		}
	}
}

void ASpectatorCharacter::Destroyed()
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BaseController->GetLocalPlayer()))
		{
			if (InputBase) Subsystem->RemoveMappingContext(InputBase->BaseMappingContext);
			if (InputSpectator) Subsystem->RemoveMappingContext(InputSpectator->SpectatorMappingContext);
		}
	}

	Super::Destroyed();
}

void ASpectatorCharacter::ViewPrevPlayer(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ServerViewPrevPlayer();
	}
}

void ASpectatorCharacter::ViewNextPlayer(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ServerViewNextPlayer();
	}
}

void ASpectatorCharacter::ScoreboardButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowScoreboard.Broadcast(true);
	}
}

void ASpectatorCharacter::ScoreboardButtonReleased(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowScoreboard.Broadcast(false);
	}
}

void ASpectatorCharacter::PauseMenuButtonPressed(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->ShowPauseMenu.Broadcast();
	}
}

void ASpectatorCharacter::TextChat(const FInputActionValue& Value)
{
	// TODO 手柄暂未处理
	if (UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetWorld()->GetFirstLocalPlayerFromController()))
	{
		if (CommonInputSubsystem->GetCurrentInputType() != ECommonInputType::MouseAndKeyboard)
		{
			return;
		}
	}

	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		BaseController->FocusUI();
		BaseController->ShowTextChat.Broadcast();
	}
}
