// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterSpawner.h"

#include "GameCode/AI/Characters/GCAICharacter.h"

AAICharacterSpawner::AAICharacterSpawner()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnerRoot"));
	SetRootComponent(SceneRoot);
}

void AAICharacterSpawner::SpawnAi()
{
	if (!bCanSpawn || !IsValid(CharacterClass))
	{
		return;
	}

	AGCAICharacter* AICharacter = GetWorld()->SpawnActor<AGCAICharacter>(CharacterClass, GetTransform());
	if (!IsValid(AICharacter->Controller))
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce)
	{
		bCanSpawn = false;
	}
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (bIsSpawnOnStart)
	{
		SpawnAi();
	}
}


