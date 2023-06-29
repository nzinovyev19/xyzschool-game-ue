// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BasePlatform.generated.h"

class APlatformInvocator;

UENUM()
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class GAMECODE_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void BeginPlatformMoving();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	APlatformInvocator* PlatformInvocator;

	UFUNCTION()
	void OnPlatformInvoked();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private: 
	FTimeline PlatformTimeline;

	void PlatformTimelineUpdate(float Alpha);
};
