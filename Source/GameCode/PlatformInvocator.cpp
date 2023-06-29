// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformInvocator.h"

void APlatformInvocator::Invoke()
{
	if (OnInvocatorActivated.IsBound()) 
	{
		OnInvocatorActivated.Broadcast();
	};
}

// Called when the game starts or when spawned
void APlatformInvocator::BeginPlay()
{
	Super::BeginPlay();

}

