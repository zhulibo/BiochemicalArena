#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputBase.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UInputBase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditAnywhere)
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere)
	UInputAction* LookMouseAction;
	UPROPERTY(EditAnywhere)
	UInputAction* LookStickAction;
	UPROPERTY(EditAnywhere)
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere)
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere)
	UInputAction* CrouchControllerAction;

	UPROPERTY(EditAnywhere)
	UInputAction* ScoreboardAction;
	UPROPERTY(EditAnywhere)
	UInputAction* PauseMenuAction;

	UPROPERTY(EditAnywhere)
	UInputAction* RadialMenuAction;
	UPROPERTY(EditAnywhere)
	UInputAction* RadialMenuChangeAction;
	UPROPERTY(EditAnywhere)
	UInputAction* RadialMenuSelectAction;

};
