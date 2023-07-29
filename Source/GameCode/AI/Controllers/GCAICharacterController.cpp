// Fill out your copyright notice in the Description page of Project Settings.


#include "GCAICharacterController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/AI/Characters/GCAICharacter.h"
#include "GameCode/Components/CharacterComponents/AIPatrollingComponent.h"

void AGCAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AGCAICharacter>(), TEXT("AGCAICharacterController::SetPawn AICharacterController can posses only GCAICharacter"));
		CachedAICharacter = StaticCast<AGCAICharacter*>(InPawn);
		RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
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
	TryMoveToNextTarget();
}

void AGCAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess())
	{
		return;
	}
	TryMoveToNextTarget();
}

void AGCAICharacterController::TryMoveToNextTarget()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor))
	{
		if (IsValid(Blackboard))
		{ 
			Blackboard->SetValueAsObject(BB_CurrentTarget, ClosestActor);
		}
		bIsPatrolling = false;
	} else if (PatrollingComponent->CanPatrol())
	{
		FVector Waypoint = bIsPatrolling ? PatrollingComponent->SelectNextWaypoint() : PatrollingComponent->SelectClosestWaypoint();
		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, Waypoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}
		bIsPatrolling = true;
	}
}

bool AGCAICharacterController::IsTargetReached(FVector TargetLocation) const
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachRadius);
}

void AGCAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWaypoint = PatrollingComponent->SelectClosestWaypoint();
		if (IsValid(Blackboard))
		{
			Blackboard->SetValueAsVector(BB_NextLocation, ClosestWaypoint);
			Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		}
		bIsPatrolling = true;
	}
}
