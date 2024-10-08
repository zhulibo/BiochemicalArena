#pragma once

#include <eos_lobby_types.h>
#include <string>

#include "CoreMinimal.h"
#include "Connect.h"
#include "ServiceManager.h"
#include "Lobby.generated.h"

USTRUCT()
struct FLobbiesAttr {
	GENERATED_BODY()

	std::string Key;

	int64_t AsInt64;
	double AsDouble;
	EOS_Bool AsBool;
	std::string AsUtf8;

	EOS_ELobbyAttributeType ValueType;

	EOS_EComparisonOp ComparisonOp;

	EOS_Lobby_AttributeData ToAttrData();

	void InitFromAttrData(EOS_Lobby_Attribute* Attr);

};

USTRUCT()
struct FLobbyAttrBase {
	GENERATED_BODY()

	TArray<FLobbiesAttr> Attrs;

	int64_t GetIntAttr(FName Key) const
	{
		int64_t Res = 0;
		for (const auto& Attr : Attrs)
		{
			if (FName(Attr.Key.c_str()) == Key)
			{
				Res = Attr.AsInt64;
			}
		}
		return Res;
	}

	double GetDoubleAttr(FName Key) const
	{
		double Res = 0.f;
		for (const auto& Attr : Attrs)
		{
			if (FName(Attr.Key.c_str()) == Key)
			{
				Res = Attr.AsDouble;
			}
		}
		return Res;
	}

	bool GetBoolAttr(FName Key) const
	{
		bool Res = false;
		for (const auto& Attr : Attrs)
		{
			if (FName(Attr.Key.c_str()) == Key)
			{
				Res = Attr.AsBool == EOS_TRUE;
			}
		}
		return Res;
	}

	FString GetStringAttr(FName Key) const
	{
		FString Res;
		for (const auto& Attr : Attrs)
		{
			if (FName(Attr.Key.c_str()) == Key)
			{
				Res = Attr.AsUtf8.c_str();
			}
		}
		return Res;
	}
};

USTRUCT()
struct FCoolLobbyMember : public FLobbyAttrBase {
	GENERATED_BODY()

	EOS_EpicAccountId EpicAccountId;

	EOS_ProductUserId ProductUserId;

};

USTRUCT()
struct FCoolLobby : public FLobbyAttrBase {
	GENERATED_BODY()

	std::string LobbyId;
	EOS_ProductUserId LobbyOwnerUserId;
	uint32_t AvailableSlots;
	uint32_t MaxMembers;

	TArray<FCoolLobbyMember> Members;

};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreateLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindLobbiesComplete, bool bWasSuccessful, TArray<TSharedPtr<FCoolLobby>> FoundLobbies);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE(FOnLobbyUpdateReceived);
DECLARE_MULTICAST_DELEGATE(FOnLobbyMemberUpdateReceived);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMemberStatusReceived, EOS_ProductUserId TargetUserId, EOS_ELobbyMemberStatus CurrentStatus);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLeaveLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestroyLobbyComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPromoteMemberComplete, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKickMemberComplete, bool bWasSuccessful);

UCLASS()
class COMMONONLINESERVICES_API ULobby : public UObject
{
	GENERATED_BODY()

public:
	ULobby();

	TSharedPtr<FCoolLobby> CurLobby = MakeShared<FCoolLobby>();
	TArray<EOS_HLobbyDetails> LobbyDetailsHandles;

	bool IsOwner(EOS_ProductUserId ProductUserId = UServiceManager::GetConnect()->ProductUserId);

	EOS_NotificationId LobbyUpdateReceivedId = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId LobbyMemberUpdateReceivedId = EOS_INVALID_NOTIFICATIONID;
	EOS_NotificationId MemberStatusReceivedId = EOS_INVALID_NOTIFICATIONID;
	void AddNotifyLobbyUpdates();
	static EOS_HLobbyDetails GetLobbyDetailsHandle(std::string LobbyId);
	static void EOS_CALL LobbyUpdateReceivedCb(const EOS_Lobby_LobbyUpdateReceivedCallbackInfo* Data);
	static void EOS_CALL LobbyMemberUpdateReceivedCb(const EOS_Lobby_LobbyMemberUpdateReceivedCallbackInfo* Data);
	static void EOS_CALL MemberStatusReceivedCb(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);
	FOnLobbyUpdateReceived OnLobbyUpdateReceived;
	FOnLobbyMemberUpdateReceived OnLobbyMemberUpdateReceived;
	FOnMemberStatusReceived OnMemberStatusReceived;

	void CreateLobby();
	static void EOS_CALL CreateLobbyCb(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	FOnCreateLobbyComplete OnCreateLobbyComplete;

	static void UpdateLobby(EOS_Lobby_AttributeData& Attr);
	static void EOS_CALL UpdateLobbyCb(const EOS_Lobby_UpdateLobbyCallbackInfo* Data);
	FOnUpdateLobbyComplete OnUpdateLobbyComplete;

	EOS_HLobbySearch LobbySearchHandle = nullptr;
	void FindLobbies(TArray<FLobbiesAttr> Attrs);
	static void EOS_CALL FindLobbiesCb(const EOS_LobbySearch_FindCallbackInfo* Data);
	FOnFindLobbiesComplete OnFindLobbiesComplete;

	void JoinLobby(TSharedPtr<FCoolLobby> Lobby, EOS_HLobbyDetails LobbyDetailsHandle);
	static void EOS_CALL JoinLobbyCb(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	FOnJoinLobbyComplete OnJoinLobbyComplete;

	void GetLobbyDetails(EOS_HLobbyDetails LobbyDetailsHandle, TSharedPtr<FCoolLobby> OutLobby);
	TArray<FLobbiesAttr> GetLobbyAttrs(EOS_HLobbyDetails LobbyDetailsHandle);
	TArray<FCoolLobbyMember> GetLobbyMembers(EOS_HLobbyDetails LobbyDetailsHandle);

	void LeaveLobby();
	static void EOS_CALL LeaveLobbyCb(const EOS_Lobby_LeaveLobbyCallbackInfo* Data);
	FOnLeaveLobbyComplete OnLeaveLobbyComplete;

	void DestroyLobby();
	static void EOS_CALL DestroyLobbyCb(const EOS_Lobby_DestroyLobbyCallbackInfo* Data);
	FOnDestroyLobbyComplete OnDestroyLobbyComplete;

	void PromoteMember(EOS_ProductUserId ProductUserId);
	static void EOS_CALL PromoteMemberCb(const EOS_Lobby_PromoteMemberCallbackInfo* Data);
	FOnPromoteMemberComplete OnPromoteMemberComplete;

	void KickMember(EOS_ProductUserId ProductUserId);
	static void EOS_CALL KickMemberCb(const EOS_Lobby_KickMemberCallbackInfo* Data);
	FOnKickMemberComplete OnKickMemberComplete;

};
