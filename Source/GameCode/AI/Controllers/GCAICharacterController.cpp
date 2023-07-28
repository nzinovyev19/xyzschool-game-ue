// Fill out your copyright notice in the Description page of Project Settings.


#include "GCAICharacterController.h"

#include "Perception/AISense_Sight.h"
#include "GameCode/AI/Characters/GCAICharacter.h"

void AGCAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AGCAICharacter>(), TEXT("AGCAICharacterController::SetPawn AICharacterController can posses only GCAICharacter"));
		CachedAICharacter = StaticCast<AGCAICharacter*>(InPawn);
	}
	else
	{
		CachedAICharacter = nullptr;
	}
}

void AGCAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedAICharacter.IsValid())
	{
		return;
	}

	AActor* ClosesActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosesActor))
	{
		MoveToActor(ClosesActor);	
	}
}
