#include "SpectatorCharacter.h"

#include "CommonInputSubsystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BiochemicalArena/GameStates/BaseGameState.h"
#include "BiochemicalArena/PlayerControllers/BaseController.h"
#include "BiochemicalArena/System/AssetSubsystem.h"
#include "Data/InputAsset.h"

void ASpectatorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASpectatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->InputAsset == nullptr || AssetSubsystem->InputAsset == nullptr) return;
	
	if (ABaseGameState* BaseGameState = Cast<ABaseGameState>(GetWorld()->GetGameState()))
	{
		// 第一次进入游戏生成角色前会默认生成ASpectator，避免绑定输入。
		if (BaseGameState->bCanSpectate == false) return;

		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(AssetSubsystem->InputAsset->BaseMappingContext, 10);
				Subsystem->AddMappingContext(AssetSubsystem->InputAsset->SpectatorMappingContext, 20);
			}
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
		{
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::ScoreboardButtonPressed);
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->ScoreboardAction, ETriggerEvent::Completed, this, &ThisClass::ScoreboardButtonReleased);
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->PauseMenuAction, ETriggerEvent::Triggered, this, &ThisClass::PauseMenuButtonPressed);
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->TextChatAction, ETriggerEvent::Triggered, this, &ThisClass::TextChat);
		
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->SwitchPerspectiveAction, ETriggerEvent::Triggered, this, &ThisClass::SwitchPerspective);
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->ViewNextAction, ETriggerEvent::Triggered, this, &ThisClass::ViewNextPlayer);
			EnhancedInputComponent->BindAction(AssetSubsystem->InputAsset->ViewPrevAction, ETriggerEvent::Triggered, this, &ThisClass::ViewPrevPlayer);
		}
	}
}

void ASpectatorCharacter::Destroyed()
{
	if (AssetSubsystem == nullptr) AssetSubsystem = GetGameInstance()->GetSubsystem<UAssetSubsystem>();
	if (AssetSubsystem == nullptr || AssetSubsystem->InputAsset == nullptr || AssetSubsystem->InputAsset == nullptr) return;
	
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(BaseController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(AssetSubsystem->InputAsset->BaseMappingContext);
			Subsystem->RemoveMappingContext(AssetSubsystem->InputAsset->SpectatorMappingContext);
		}
	}

	Super::Destroyed();
}

void ASpectatorCharacter::SwitchPerspective(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		if (PerspectiveType == EPerspectiveType::FirstPerson)
		{
			PerspectiveType = EPerspectiveType::Free;
			BaseController->ServerViewSelf();
		}
		else
		{
			PerspectiveType = EPerspectiveType::FirstPerson;
			BaseController->ServerViewPrevPlayer();
		}
	}
}

void ASpectatorCharacter::ViewPrevPlayer(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		PerspectiveType = EPerspectiveType::FirstPerson;
		BaseController->ServerViewPrevPlayer();
	}
}

void ASpectatorCharacter::ViewNextPlayer(const FInputActionValue& Value)
{
	if (BaseController == nullptr) BaseController = Cast<ABaseController>(Controller);
	if (BaseController)
	{
		PerspectiveType = EPerspectiveType::FirstPerson;
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
