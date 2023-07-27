// Fill out your copyright notice in the Description page of Project Settings.


#include "AITurretController.h"

#include "GameCode/AI/Characters/Turret.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AAITurretController can possess only turrets"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	if (!CachedTurret.IsValid())
	{
		return;	
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Damage::StaticClass());
	if (!IsValid(ClosestActor))
	{
		ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	}

	CachedTurret->SetCurrentTarget(ClosestActor);
}
