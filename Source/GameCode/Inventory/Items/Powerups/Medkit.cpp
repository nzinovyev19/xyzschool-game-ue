// Fill out your copyright notice in the Description page of Project Settings.


#include "Medkit.h"

#include "GameCode/Characters/GCBaseCharacter.h"

bool UMedkit::Consume(AGCBaseCharacter* ConsumeTarget)
{
	ConsumeTarget->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}
