#include "ServiceManager.h"

#include <string>

#include "Auth.h"
#include "Connect.h"
#include "eos_sdk.h"
#include "Lobby.h"
#include "P2P.h"

UServiceManager* UServiceManager::Instance = nullptr;

UServiceManager::UServiceManager()
{
	UE_LOG(LogTemp, Warning, TEXT("UServiceManager"));
}

// 设置EOS日志输出
void UServiceManager::EOSSDKLoggingCb(const EOS_LogMessage* InMsg)
{
	FString Category(InMsg->Category);
	FString Message(InMsg->Message);

	if (InMsg->Level == EOS_ELogLevel::EOS_LOG_Error || InMsg->Level == EOS_ELogLevel::EOS_LOG_Fatal)
	{
		UE_LOG(LogTemp, Error, TEXT("[EOSSDK---] %s %s"), *Category, *Message);
	}
	else if (InMsg->Level == EOS_ELogLevel::EOS_LOG_Warning)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EOSSDK---] %s %s"), *Category, *Message);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[EOSSDK---] %s %s"), *Category, *Message);
	}
}

// 初始化EOS
bool UServiceManager::InitEOS()
{
	EOS_InitializeOptions Options = {};

	const FTCHARToUTF8 Utf8ProductName(*ProductName);
	const FTCHARToUTF8 Utf8ProductVersion(*ProductVersion);

	Options.ApiVersion = EOS_INITIALIZE_API_LATEST;
	Options.AllocateMemoryFunction = nullptr;
	Options.ReallocateMemoryFunction = nullptr;
	Options.ReleaseMemoryFunction = nullptr;
	Options.ProductName = Utf8ProductName.Get();
	Options.ProductVersion = Utf8ProductVersion.Get();
	Options.Reserved = nullptr;
	Options.SystemInitializeOptions = nullptr;
	Options.OverrideThreadAffinity = nullptr;

	EOS_EResult Result = EOS_Initialize(&Options);

	if (Result != EOS_EResult::EOS_Success && Result != EOS_EResult::EOS_AlreadyConfigured)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Initialize: %hs"), EOS_EResult_ToString(Result));

		GetServiceManager()->OnInitComplete.Broadcast(false);

		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EOS_Initialize: %hs"), EOS_EResult_ToString(Result));
	}

	EOS_EResult Result2 = EOS_Logging_SetCallback(&EOSSDKLoggingCb);

	if (Result2 != EOS_EResult::EOS_Success)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Logging_SetCallback: %hs"), EOS_EResult_ToString(Result2));
	}
	else
	{
		EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_VeryVerbose);
	}

	return CreatePlatform();
}

// 创建平台接口
bool UServiceManager::CreatePlatform()
{
	// 如果是开发环境，使用开发参数覆盖
#if UE_BUILD_DEVELOPMENT
	SandboxId = SandboxId_Dev;
	DeploymentId = DeploymentId_Dev;
#endif
	// UE_LOG(LogTemp, Warning, TEXT("UE_BUILD_DEVELOPMENT %s %s"), *SandboxId, *DeploymentId);

	// 如果是从Epic Games Launcher启动，获取命令行中的参数并覆盖
	FParse::Value(FCommandLine::Get(), TEXT("EpicSandboxId="), SandboxId);
	FParse::Value(FCommandLine::Get(), TEXT("EpicDeploymentId="), DeploymentId);
	// UE_LOG(LogTemp, Warning, TEXT("FCommandLine %s %s"), *SandboxId, *DeploymentId);

	std::string ProductId2 = TCHAR_TO_UTF8(*ProductId);
	std::string SandboxId2 = TCHAR_TO_UTF8(*SandboxId);
	std::string ClientId2 = TCHAR_TO_UTF8(*ClientId);
	std::string ClientSecret2 = TCHAR_TO_UTF8(*ClientSecret);
	std::string DeploymentId2 = TCHAR_TO_UTF8(*DeploymentId);

	EOS_Platform_Options Options = {};
	Options.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
	Options.Reserved = nullptr;
	Options.ProductId = ProductId2.c_str();
	Options.SandboxId = SandboxId2.c_str();
	Options.ClientCredentials.ClientId = ClientId2.c_str();
	Options.ClientCredentials.ClientSecret = ClientSecret2.c_str();
	Options.bIsServer = EOS_FALSE;
	Options.EncryptionKey = EncryptionKey;
	Options.OverrideCountryCode = nullptr;
	Options.OverrideLocaleCode = nullptr;
	Options.DeploymentId = DeploymentId2.c_str();
	Options.Flags = 0;
	Options.CacheDirectory = nullptr;
	Options.TickBudgetInMilliseconds = 0;
	Options.RTCOptions = nullptr;
	Options.IntegratedPlatformOptionsContainerHandle = nullptr;
	Options.SystemSpecificOptions = nullptr;
	Options.TaskNetworkTimeoutSeconds = nullptr;

	PlatformHandle = EOS_Platform_Create(&Options);

	if (PlatformHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS_Platform_Create failed!"));

		GetServiceManager()->OnInitComplete.Broadcast(false);

		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EOS_Platform_Create success!"));

		GetServiceManager()->OnInitComplete.Broadcast(true);

		return true;
	}
}

void UServiceManager::UpdateEOS()
{
	if (PlatformHandle)
	{
		EOS_Platform_Tick(PlatformHandle);
	}
}

void UServiceManager::ShutdownEOS()
{
	if (PlatformHandle)
	{
		EOS_Platform_Release(PlatformHandle);
	}

	EOS_EResult Result = EOS_Shutdown();

	UE_LOG(LogTemp, Warning, TEXT("EOS_Shutdown: %hs"), EOS_EResult_ToString(Result));
}

UServiceManager* UServiceManager::GetServiceManager()
{
	EnsureInstanceCreated();

	return Instance;
}

void UServiceManager::EnsureInstanceCreated()
{
	if (Instance == nullptr)
	{
		Instance = NewObject<UServiceManager>();
		Instance->AddToRoot();
	}
}

EOS_HPlatform UServiceManager::GetPlatformHandle()
{
	EnsureInstanceCreated();

	return Instance->PlatformHandle;
}

UAuth* UServiceManager::GetAuth()
{
	EnsureInstanceCreated();

	if (Instance->Auth == nullptr)
	{
		Instance->Auth = NewObject<UAuth>();
	}

	return Instance->Auth;
}

UConnect* UServiceManager::GetConnect()
{
	EnsureInstanceCreated();

	if (Instance->Connect == nullptr)
	{
		Instance->Connect = NewObject<UConnect>();
	}

	return Instance->Connect;
}

ULobby* UServiceManager::GetLobby()
{
	EnsureInstanceCreated();

	if (Instance->Lobby == nullptr)
	{
		Instance->Lobby = NewObject<ULobby>();

		Instance->Lobby->AddNotifyLobbyUpdates();
	}

	return Instance->Lobby;
}

UP2P* UServiceManager::GetP2P()
{
	EnsureInstanceCreated();

	if (Instance->P2P == nullptr)
	{
		Instance->P2P = NewObject<UP2P>();
	}

	return Instance->P2P;
}
