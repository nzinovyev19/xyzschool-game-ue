// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNotify_EnableRagdoll.h"

#include "GameCode/GameCodeTypes.h"

void UAnimNotify_EnableRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	MeshComp->SetCollisionProfileName(CollisionProfileRagdoll);
	MeshComp->SetSimulatePhysics(true);
}
