// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrollingPath.generated.h"

UCLASS()
class GAMECODE_API APatrollingPath : public AActor
{
	GENERATED_BODY()
	
public:
	const TArray<FVector>& GetWaypoints() const;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path", meta = (MakeEditWidget))
	TArray<FVector> WayPoints;
	
};
