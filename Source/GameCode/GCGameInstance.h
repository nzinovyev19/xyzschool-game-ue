	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GCGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

};
