#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubsystem.generated.h"

enum class ELoginType : uint8;

UCLASS()
class BIOCHEMICALARENA_API UPlayerSubsystem : public ULocalPlayerSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPlayerSubsystem();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UPlayerSubsystem, STATGROUP_Tickables);
	}

	void Login(ELoginType LoginType, FString ID, FString Token);

protected:
	UPROPERTY()
	class UServiceManager* ServiceManager;
	UPROPERTY()
	class UAuth* Auth;
	void OnLoginOut();

	UPROPERTY()
	class UConnect* Connect;
	void OnConnectComplete(bool bWasSuccessful);
	void OnConnectAuthExpiration();
	void OnConnectLoginOut();

};
