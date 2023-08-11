// Fill out your copyright notice in the Description page of Project Settings.


#include "HostSessionWidget.h"

#include "GameCode/GCGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UGCGameInstance>());
	UGCGameInstance* GCGameInstance = StaticCast<UGCGameInstance*>(GetGameInstance());

	GCGameInstance->LaunchLobby(4, ServerName, bIsLAN);
}
