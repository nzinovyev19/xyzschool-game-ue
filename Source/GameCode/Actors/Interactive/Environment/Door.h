// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameCode/Actors/Interactive/Interface/Interactive.h"
#include "GameCode/Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class GAMECODE_API ADoor : public AActor, public IInteractable, public ISaveSubsystemInterface
{
	GENERATED_BODY()
	
public:	
	ADoor();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AGCBaseCharacter* Character) override;

	virtual FName GetActionEventName() const override;

	virtual bool HasOnInteractionCallback() const override;

	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;

	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override;

	virtual void OnLevelDeserialized_Implementation() override;

protected:
	virtual void BeginPlay() override;

	IInteractable::FOnInteraction OnInteractionEvent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

private:
	void InteractWithDoor();
	
	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);

	UFUNCTION()
	void OnDoorAnimationFinished();
	
	FTimeline DoorOpenAnimTimeline;

	UPROPERTY(SaveGame)
	bool bIsOpened = false;

};
