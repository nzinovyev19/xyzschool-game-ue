// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StreamingSubsystemUtils.generated.h"

class ACharacter;
class AStreamingSubsystemVolume;

UCLASS()
class GAMECODE_API UStreamingSubsystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Streaming Subsystem Utils")
	static void CheckCharacterOverlapStreamingSubsystemVolume(ACharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Streaming Subsystem Utils")
	static void CheckStreamingSubsystemVolumeOverlapCharacter(AStreamingSubsystemVolume* SubsystemVolume);

};
