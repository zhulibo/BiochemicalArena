#pragma once

#include <eos_types.h>

#include "CoreMinimal.h"
#include "eos_logging.h"
#include "ServiceManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInitComplete, bool bWasSuccessful);

UCLASS()
class COMMONONLINESERVICES_API UServiceManager : public UObject
{
	GENERATED_BODY()

public:
	UServiceManager();

	FOnInitComplete OnInitComplete;

	UFUNCTION()
	static UServiceManager* GetServiceManager();
	static void EnsureInstanceCreated();

	static EOS_HPlatform GetPlatformHandle();

	UFUNCTION()
	bool InitEOS();
	UFUNCTION()
	void UpdateEOS();
	UFUNCTION()
	void ShutdownEOS();

	UFUNCTION()
	static class UAuth* GetAuth();
	UFUNCTION()
	static class UConnect* GetConnect();
	UFUNCTION()
	static class ULobby* GetLobby();
	UFUNCTION()
	static class UP2P* GetP2P();

protected:
	static void EOSSDKLoggingCb(const EOS_LogMessage* InMsg);
	UFUNCTION()
	bool CreatePlatform();

	static UServiceManager* Instance;
	EOS_HPlatform PlatformHandle;

	UPROPERTY()
	UAuth* Auth;
	UPROPERTY()
	UConnect* Connect;
	UPROPERTY()
	ULobby* Lobby;
	UPROPERTY()
	UP2P* P2P;

private:
	UPROPERTY()
	FString ProductName = TEXT("BiochemicalArena");
	UPROPERTY()
	FString ProductVersion = TEXT("1.0.0");
	UPROPERTY()
	FString ProductId = TEXT("9508c26078234ba69aeb152fe34128b7");
	UPROPERTY()
	FString ClientId = TEXT("xyza7891qU46M9UrjCgDmyj7h1lkbsgl");
	UPROPERTY()
	FString ClientSecret = TEXT("ecSzA1aWrVuztlCDCpf+KtRA/1NvhY7P7hKIr5ug9bg");
	const char* EncryptionKey = "1111111111111111111111111111111111111111111111111111111111111111";

	// Live 默认使用正式环境参数，用户不从Epic Games Launcher启动时（文件管理器启动exe或是主机平台），
	// 无法接收到命令行参数，不能覆盖为相应的的环境参数，所以首要确保正式环境是正常的。
	UPROPERTY()
	FString SandboxId = TEXT("89f212c83ee24545b54c54a268018166");
	UPROPERTY()
	FString DeploymentId = TEXT("b446d96287d44204bb1ecf733fe4217b");

	// Dev
	UPROPERTY()
	FString SandboxId_Dev = TEXT("p-9hdddp2tmrprnmnbt2gbur4m6dqs6d");
	UPROPERTY()
	FString DeploymentId_Dev = TEXT("e14517da5555424cad0399ef5990b23e");

	// Stage
	UPROPERTY()
	FString SandboxId_Stage = TEXT("p-juzhxm7jefpr8xgvygjlfvk6ayx7yq");
	UPROPERTY()
	FString DeploymentId_Stage = TEXT("cd0500ab780a47b3a616271cbb361648");

};
