#pragma once

#include "CoreMinimal.h"
#include "BiochemicalArena/System/EOSSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubsystem.generated.h"

UCLASS()
class BIOCHEMICALARENA_API UPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UPlayerSubsystem();

	void Login(int32 Type = 0);

	void SetSpawnCharacterName(FString TemSpawnCharacterName);

protected:
	UPROPERTY()
	UEOSSubsystem* EOSSubsystem;
	void OnLoginComplete(bool bWasSuccessful);
	void OnLoginStatusChanged(const FAuthLoginStatusChanged& AuthLoginStatusChanged);

	UPROPERTY()
	FString SpawnCharacterName; // 角色名字，开局生成角色时使用

public:
	FORCEINLINE FString GetSpawnCharacterName() const { return SpawnCharacterName; }

};
