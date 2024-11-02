#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "SpectatorCharacter.generated.h"

enum class ECommonInputType : uint8;

UCLASS()
class BIOCHEMICALARENA_API ASpectatorCharacter : public ASpectatorPawn
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Destroyed() override;

	UPROPERTY()
	class ABaseController* BaseController;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputBase> InputBase;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UInputSpectator> InputSpectator;
	void ScoreboardButtonPressed(const struct FInputActionValue& Value);
	void ScoreboardButtonReleased(const FInputActionValue& Value);
	void PauseMenuButtonPressed(const FInputActionValue& Value);

	void TextChat(const FInputActionValue& Value);

	void ViewPrevPlayer(const FInputActionValue& Value);
	void ViewNextPlayer(const FInputActionValue& Value);

};
