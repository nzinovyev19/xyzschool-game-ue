// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZipline();
	
	virtual void OnConstruction(const FTransform& Transform) override;

	FVector GetTopPointOfFirstPillar() const;

	FVector GetTopPointOfSecondPillar() const;

	UCapsuleComponent* GetZiplineInteractiveCapsule() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float ZiplineLength = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float FirstPillarHeight = 200.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zipline parameters", meta = (MakeEditWidget))
	FVector FirstPillarLocation = FVector(0.0f, -ZiplineLength * 0.5f, FirstPillarHeight * 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float SecondPillarHeight = 200.0f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zipline parameters", meta = (MakeEditWidget))
	FVector SecondPillarLocation = FVector(0.0f, ZiplineLength * 0.5f, SecondPillarHeight * 0.5f);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zipline parameters", meta = (MakeEditWidget))
	float InteractiveVolumeRadius = 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector TopPointOfFirstPillar = FVector(FirstPillarLocation.X, FirstPillarLocation.Y, FirstPillarLocation.Z + FirstPillarHeight * 0.5f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FVector TopPointOfSecondPillar = FVector(SecondPillarLocation.X, SecondPillarLocation.Y, SecondPillarLocation.Z + SecondPillarHeight * 0.5f);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FirstPillarMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SecondPilarMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableMeshComponent;
};
