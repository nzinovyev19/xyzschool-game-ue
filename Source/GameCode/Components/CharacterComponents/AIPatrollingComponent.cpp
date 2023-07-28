#include "AIPatrollingComponent.h"

#include "GameCode/Actors/Navigation/PatrollingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWaypoints().Num() > 0;
}

FVector UAIPatrollingComponent::SelectClosestWaypoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> Waypoints = PatrollingPath->GetWaypoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWaypoint;
	float MinSquaredDistance = FLT_MAX;
	for (int32 i = 0; i < Waypoints.Num(); ++i)
	{
		FVector WaypointWorld = PathTransform.TransformPosition(Waypoints[i]);
		float CurrentSquaredDistance = (OwnerLocation - WaypointWorld).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestWaypoint = WaypointWorld;
			CurrentWaypointIndex = i;
		}
	}
	return ClosestWaypoint;
}

FVector UAIPatrollingComponent::SelectNextWaypoint()
{
	++CurrentWaypointIndex;
	const TArray<FVector> Waypoints = PatrollingPath->GetWaypoints();
	if (CurrentWaypointIndex == PatrollingPath->GetWaypoints().Num())
	{
		CurrentWaypointIndex = 0;
	}
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector Waypoint = PathTransform.TransformPosition(Waypoints[CurrentWaypointIndex]);
	return Waypoint;
}
