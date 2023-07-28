// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCAIController.h"
#include "GCAICharacterController.generated.h"

class AGCAICharacter;
UCLASS()
class GAMECODE_API AGCAICharacterController : public AGCAIController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

private:
	TWeakObjectPtr<AGCAICharacter> CachedAICharacter;
	
};
