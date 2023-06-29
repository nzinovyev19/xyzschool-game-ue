// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "PlatformInvocator.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PlatformInvocator)) 
	{
		PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
	}
	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
		if (PlatformBehavior == EPlatformBehavior::Loop) 
		{
			ABasePlatform::BeginPlatformMoving();
		}
	}
}

void ABasePlatform::BeginPlatformMoving()
{
	PlatformTimeline.Play();
	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		PlatformTimeline.SetLooping(true);
		PlatformTimeline.Play();
	}
}

void ABasePlatform::OnPlatformInvoked()
{
	PlatformTimeline.Play();
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineUpdate(float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

