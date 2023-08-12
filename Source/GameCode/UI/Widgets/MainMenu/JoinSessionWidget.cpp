// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinSessionWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Gamecode/GCGameInstance.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UGCGameInstance>());
	GCGameInstance = StaticCast<UGCGameInstance*>(GetGameInstance());
}

void UJoinSessionWidget::FindOnlineSession()
{
	GCGameInstance->OnMatchFound.AddUFunction(this, FName("OnMatchFound"));
	GCGameInstance->FindAMatch(bIsLAN);
	SearchingSessionState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	GCGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccessful)
{
	SearchingSessionState = bIsSuccessful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;
	GCGameInstance->OnMatchFound.RemoveAll(this);
}

void UJoinSessionWidget::CloseWidget()
{
	GCGameInstance->OnMatchFound.RemoveAll(this);
	Super::CloseWidget();
}
