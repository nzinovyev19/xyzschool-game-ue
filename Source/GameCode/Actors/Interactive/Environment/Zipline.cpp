// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "Kismet/KismetMathLibrary.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LadderRoot"));

	FirstPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPillar"));
	FirstPillarMeshComponent->SetCollisionProfileName(CollisionProfilePawn);
	FirstPillarMeshComponent->SetupAttachment(RootComponent);

	SecondPilarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPillar"));
	SecondPilarMeshComponent->SetupAttachment(RootComponent);

	CableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cabel"));
	CableMeshComponent->SetupAttachment(RootComponent);
	CableMeshComponent->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	if (UStaticMesh* FirstPillarMesh = FirstPillarMeshComponent->GetStaticMesh())
	{
		float MeshHeight = FirstPillarMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			FirstPillarMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, FirstPillarHeight / MeshHeight));
		}
	}
	FirstPillarMeshComponent->SetRelativeLocation(
		FirstPillarLocation != FVector::ZeroVector
			? FirstPillarLocation
			: FVector(0.0f, -ZiplineLength * 0.5f, FirstPillarHeight * 0.5f)
	);

	if (UStaticMesh* SecondPillarMesh = SecondPilarMeshComponent->GetStaticMesh())
	{
		float MeshHeight = SecondPillarMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			SecondPilarMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, SecondPillarHeight / MeshHeight));
		}
	}
	SecondPilarMeshComponent->SetRelativeLocation(
		SecondPillarLocation != FVector::ZeroVector
			? SecondPillarLocation
			: FVector(0.0f, ZiplineLength * 0.5f, SecondPillarHeight * 0.5f)
	);

	TopPointOfFirstPillar = FVector(FirstPillarLocation.X, FirstPillarLocation.Y, FirstPillarLocation.Z + FirstPillarHeight * 0.5f);
	TopPointOfSecondPillar = FVector(SecondPillarLocation.X, SecondPillarLocation.Y, SecondPillarLocation.Z + SecondPillarHeight * 0.5f);
	float ComputedZiplineLength = (TopPointOfFirstPillar - TopPointOfSecondPillar).Size();
	CableMeshComponent->SetRelativeLocation((TopPointOfFirstPillar + TopPointOfSecondPillar) * 0.5f);
	CableMeshComponent->SetRelativeRotation(FRotator(90.f, 0.f, 0.f) + UKismetMathLibrary::FindLookAtRotation(TopPointOfFirstPillar, TopPointOfSecondPillar));
	if (UStaticMesh* CableMesh = CableMeshComponent->GetStaticMesh())
	{
		float MeshWidth = CableMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			CableMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, ComputedZiplineLength / MeshWidth));
		}
	}

	GetZiplineInteractiveCapsule()->SetCapsuleRadius(InteractiveVolumeRadius);
	GetZiplineInteractiveCapsule()->SetCapsuleHalfHeight(ComputedZiplineLength / 2);
	GetZiplineInteractiveCapsule()->SetRelativeLocation(CableMeshComponent->GetRelativeLocation());
	GetZiplineInteractiveCapsule()->SetRelativeRotation(CableMeshComponent->GetRelativeRotation());
}

FVector AZipline::GetTopPointOfFirstPillar() const
{
	return TopPointOfFirstPillar;
}

FVector AZipline::GetTopPointOfSecondPillar() const
{
	return TopPointOfSecondPillar;
}

UCapsuleComponent* AZipline::GetZiplineInteractiveCapsule() const
{
	return Cast<UCapsuleComponent>(InteractionVolume);
}
