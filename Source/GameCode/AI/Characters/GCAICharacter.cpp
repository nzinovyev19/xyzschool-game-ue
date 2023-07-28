// Fill out your copyright notice in the Description page of Project Settings.


#include "GCAICharacter.h"

#include "GameCode/Components/CharacterComponents/AIPatrollingComponent.h"

AGCAICharacter::AGCAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));
}

UAIPatrollingComponent* AGCAICharacter::GetPatrollingComponent() const
{
	return AIPatrollingComponent;
}
