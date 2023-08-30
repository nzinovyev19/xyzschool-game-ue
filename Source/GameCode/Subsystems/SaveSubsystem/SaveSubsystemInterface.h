// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveSubsystemInterface.generated.h"

UINTERFACE(MinimalAPI)
class USaveSubsystemInterface : public UInterface
{
	GENERATED_BODY()
};


class GAMECODE_API ISaveSubsystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	public:
	
	/**
	 * Use this method instead @AActor::BeginPlay() for logic of savable actors.
	 * It will be called always after @AActor::@BeginPlay().
	 */
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Subsystem")
	void OnLevelDeserialized();
	
	virtual void OnLevelDeserialized_Implementation() PURE_VIRTUAL(ISaveSubsystemInterface::OnLevelDeserialized_Implementation, );

};
