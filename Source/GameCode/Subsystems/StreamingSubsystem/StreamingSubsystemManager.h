// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelStreaming.h"
#include "StreamingSubsystemManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStreamingSubsystem, Log, All);

class AStreamingSubsystemVolume;


UCLASS()
class GAMECODE_API UStreamingSubsystemManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ULevelStreaming* InStreamingLevel, const FString& InLevelName);
	void Deinitialize();

	void AddLoadRequest(AStreamingSubsystemVolume* SubsystemVolume);
	void RemoveLoadRequest(AStreamingSubsystemVolume* SubsystemVolume);

	void AddUnloadRequest(AStreamingSubsystemVolume* SubsystemVolume);
	void RemoveUnloadRequest(AStreamingSubsystemVolume* SubsystemVolume);

	ULevelStreaming::ECurrentState GetStreamingLevelState() const;

private:
	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void OnLevelUnloaded();

	void LoadLevel();
	void UnloadLevel();

	FString LevelName;
	ULevelStreaming::ECurrentState StreamingLevelState = ULevelStreaming::ECurrentState::Unloaded;
	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolume>> LoadRequests;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolume>> UnloadRequests;

};
