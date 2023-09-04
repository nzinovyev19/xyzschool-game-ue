// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/StreamingSubsystem/StreamingSubsystemVolume.h"

// Sets default values
AStreamingSubsystemVolume::AStreamingSubsystemVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStreamingSubsystemVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStreamingSubsystemVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

