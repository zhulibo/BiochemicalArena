#include "Lobby.h"
#include "CommonTextBlock.h"
#include "BiochemicalArena/PlayerControllers/MenuController.h"
#include "BiochemicalArena/UI/Common/CommonButton.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "PlayerLineButton.h"
#include "CommonHierarchicalScrollBox.h"

void ULobby::NativeConstruct()
{
	Super::NativeConstruct();

	SendMsgButton->ButtonText->SetText(FText::FromString("SendMsg"));
	if (!SendMsgButton->OnClicked().IsBoundToObject(this)) SendMsgButton->OnClicked().AddUObject(this, &ThisClass::OnSendMsgButtonClicked);

	JoinServerButton->ButtonText->SetText(FText::FromString("Join"));
	if (!JoinServerButton->OnClicked().IsBoundToObject(this)) JoinServerButton->OnClicked().AddUObject(this, &ThisClass::OnJoinServerButtonClicked);

	StartServerButton->ButtonText->SetText(FText::FromString("Start"));
	if (!StartServerButton->OnClicked().IsBoundToObject(this)) StartServerButton->OnClicked().AddUObject(this, &ThisClass::OnStartServerButtonClicked);

	Team1Button->ButtonText->SetText(FText::FromString("Join Team1"));
	if (!Team1Button->OnClicked().IsBoundToObject(this)) Team1Button->OnClicked().AddUObject(this, &ThisClass::OnTeam1ButtonClicked);

	Team2Button->ButtonText->SetText(FText::FromString("Join Team2"));
	if (!Team2Button->OnClicked().IsBoundToObject(this)) Team2Button->OnClicked().AddUObject(this, &ThisClass::OnTeam2ButtonClicked);

	BackButton->ButtonText->SetText(FText::FromString("Back"));
	if (!BackButton->OnClicked().IsBoundToObject(this)) BackButton->OnClicked().AddUObject(this, &ThisClass::OnBackButtonClicked);

	EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		// 绑定委托
		if (!EOS->OnLeaveLobbyComplete.IsBoundToObject(this)) EOS->OnLeaveLobbyComplete.AddUObject(this, &ThisClass::OnLeaveLobbyComplete);
		if (!EOS->OnModifyLobbyAttributesComplete.IsBoundToObject(this)) EOS->OnModifyLobbyAttributesComplete.AddUObject(this, &ThisClass::OnModifyLobbyAttributesComplete);
		if (!EOS->OnModifyLobbyMemberAttributesComplete.IsBoundToObject(this)) EOS->OnModifyLobbyMemberAttributesComplete.AddUObject(this, &ThisClass::OnModifyLobbyMemberAttributesComplete);

		if (!EOS->OnLobbyMemberJoined.IsBoundToObject(this)) EOS->OnLobbyMemberJoined.AddUObject(this, &ThisClass::OnLobbyMemberJoined);
		if (!EOS->OnLobbyMemberLeft.IsBoundToObject(this)) EOS->OnLobbyMemberLeft.AddUObject(this, &ThisClass::OnLobbyMemberLeft);
		if (!EOS->OnLobbyLeft.IsBoundToObject(this)) EOS->OnLobbyLeft.AddUObject(this, &ThisClass::OnLobbyLeft);
		if (!EOS->OnLobbyAttributesChanged.IsBoundToObject(this)) EOS->OnLobbyAttributesChanged.AddUObject(this, &ThisClass::OnLobbyAttributesChanged);
		if (!EOS->OnLobbyMemberAttributesChanged.IsBoundToObject(this)) EOS->OnLobbyMemberAttributesChanged.AddUObject(this, &ThisClass::OnLobbyMemberAttributesChanged);
		if (!EOS->OnLobbyLeaderChanged.IsBoundToObject(this)) EOS->OnLobbyLeaderChanged.AddUObject(this, &ThisClass::OnLobbyLeaderChanged);
	}
}

// 初始化玩家列表
void ULobby::InitPlayerList()
{
	EOS = GetGameInstance()->GetSubsystem<UEOS>();
	if (EOS)
	{
		for (auto& Member : EOS->CurrentLobby->Members)
		{
			if (Team1Container && PlayerLineButtonClass)
			{
				UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
				if (PlayerLineButton)
				{
					PlayerLineButton->Member = Member.Value;
					PlayerLineButton->PlayerName->SetText(FText::FromString(Member.Value->Attributes.Find(FName(TEXT("Appearance")))->GetString()));
					Team1Container->AddChild(PlayerLineButton);
				}
			}
		}
	}
}

void ULobby::OnSendMsgButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnSendMsgButtonClicked"));
}

void ULobby::OnJoinServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinServerButtonClicked"));
}

void ULobby::OnStartServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnStartServerButtonClicked"));
}

void ULobby::OnTeam1ButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTeam1ButtonClicked"));
}

void ULobby::OnTeam2ButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnTeam2ButtonClicked"));
}

void ULobby::OnBackButtonClicked()
{
	if (EOS)
	{
		EOS->LeaveLobby();
	}
}

// 离开大厅完成事件
void ULobby::OnLeaveLobbyComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
		if (MenuController)
		{
			MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("LeaveLobby Failed!"));
	}
}

// 修改大厅属性完成事件
void ULobby::OnModifyLobbyAttributesComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifyLobbyAttributes Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ModifyLobbyAttributes Failed!"));
	}
}

// 修改大厅成员属性完成事件
void ULobby::OnModifyLobbyMemberAttributesComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifyLobbyMemberAttributes Success!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ModifyLobbyMemberAttributes Failed!"));
	}
}

// 成员加入大厅事件
void ULobby::OnLobbyMemberJoined(const FLobbyMemberJoined& LobbyMemberJoined)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberJoined"));
	if (Team1Container && PlayerLineButtonClass)
	{
		UPlayerLineButton* PlayerLineButton = CreateWidget<UPlayerLineButton>(this, PlayerLineButtonClass);
		if (PlayerLineButton)
		{
			PlayerLineButton->Member = LobbyMemberJoined.Member;
			PlayerLineButton->PlayerName->SetText(FText::FromString(LobbyMemberJoined.Member->Attributes.Find(FName(TEXT("Appearance")))->GetString()));
			Team1Container->AddChild(PlayerLineButton);
		}
	}
}

// 成员离开大厅事件
void ULobby::OnLobbyMemberLeft(const FLobbyMemberLeft& LobbyMemberLeft)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberLeft"));
	if (Team1Container)
	{
		for (int i = 0; i < Team1Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyMemberLeft.Member->AccountId)
			{
				Team1Container->RemoveChild(PlayerLineButton);
				break;
			}
		}
	}
}

// 被踢出大厅事件
void ULobby::OnLobbyLeft(const FLobbyLeft& LobbyLeft)
{
	if (MenuController == nullptr) MenuController = Cast<AMenuController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->ServerStack->RemoveWidget(*MenuController->ServerStack->GetActiveWidget());
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Unfortunately you got kicked!"));
}

// 大厅属性改变事件
void ULobby::OnLobbyAttributesChanged(const FLobbyAttributesChanged& LobbyAttributesChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyAttributesChanged"));
	for (auto& ChangedAttribute : LobbyAttributesChanged.ChangedAttributes)
	{
		if (ChangedAttribute.Key == FName(TEXT("SessionId")))
		{
			// EOS->JoinSession(ChangedAttribute.Value);
			UE_LOG(LogTemp, Warning, TEXT("OnLobbyAttributesChanged GameSessionId"));
		}
	}
}

// 大厅成员属性改变事件
void ULobby::OnLobbyMemberAttributesChanged(const FLobbyMemberAttributesChanged& LobbyMemberAttributesChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyMemberAttributesChanged"));
}

// 大厅房主改变事件
void ULobby::OnLobbyLeaderChanged(const FLobbyLeaderChanged& LobbyLeaderChanged)
{
	UE_LOG(LogTemp, Warning, TEXT("OnLobbyLeaderChanged"));
	if (Team1Container)
	{
		for (int i = 0; i < Team1Container->GetChildrenCount(); ++i)
		{
			UPlayerLineButton* PlayerLineButton = Cast<UPlayerLineButton>(Team1Container->GetChildAt(i));
			if (PlayerLineButton && PlayerLineButton->Member->AccountId == LobbyLeaderChanged.Leader->AccountId)
			{
				// TODO 添加房主图标
				break;
			}
		}
	}
}
