// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamingSubsystem.h"

#include "StreamingSubsystemManager.h"
#include "StreamingSubsystemVolume.h"

void UStreamingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::Initialize(): %s"), *GetNameSafe(this));

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UStreamingSubsystem::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UStreamingSubsystem::OnPostLoadMapWithWorld);

	CreateStreamingLevelManagers(GetWorld());
}

void UStreamingSubsystem::Deinitialize()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::Deinitialize(): %s"), *GetNameSafe(this));

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	RemoveStreamingLevelManagers();

	Super::Deinitialize();
}

UWorld* UStreamingSubsystem::GetWorld() const
{
	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		return GameInstance->GetWorld();
	}

	UObject* Outer = GetOuter();
	if (IsValid(Outer))
	{
		return Outer->GetWorld();
	}

	return nullptr;
}

void UStreamingSubsystem::OnVolumeOverlapBegin(AStreamingSubsystemVolume* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::OnVolumeOverlapBegin(): %s, SubsystemVolume: %s"), *GetNameSafe(this), *GetNameSafe(SubsystemVolume));

	for (const FString& LevelToLoad : SubsystemVolume->GetLevelsToLoad())
	{
		UStreamingSubsystemManager* LevelManager = nullptr;;
		if (!FindStreamingLevelManager(LevelToLoad, LevelManager))
		{
			continue;
		}

		LevelManager->AddLoadRequest(SubsystemVolume);
	}

	for (const FString& LevelToUnload : SubsystemVolume->GetLevelsToUnLoad())
	{
		UStreamingSubsystemManager* LevelManager = nullptr;;
		if (!FindStreamingLevelManager(LevelToUnload, LevelManager))
		{
			continue;
		}

		LevelManager->AddUnloadRequest(SubsystemVolume);
	}
}

void UStreamingSubsystem::OnVolumeOverlapEnd(AStreamingSubsystemVolume* SubsystemVolume)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::OnVolumeOverlapBegin(): %s, SubsystemVolume: %s"), *GetNameSafe(this), *GetNameSafe(SubsystemVolume));

	for (const FString& LevelToLoad : SubsystemVolume->GetLevelsToLoad())
	{
		UStreamingSubsystemManager* LevelManager = nullptr;;
		if (!FindStreamingLevelManager(LevelToLoad, LevelManager))
		{
			continue;
		}

		LevelManager->RemoveLoadRequest(SubsystemVolume);
	}

	for (const FString& LevelToUnload : SubsystemVolume->GetLevelsToUnLoad())
	{
		UStreamingSubsystemManager* LevelManager = nullptr;;
		if (!FindStreamingLevelManager(LevelToUnload, LevelManager))
		{
			continue;
		}

		LevelManager->RemoveUnloadRequest(SubsystemVolume);
	}
}

bool UStreamingSubsystem::CanUseSubsystem() const
{
	return StreamingLevelManagers.Num() > 0;
}

void UStreamingSubsystem::CreateStreamingLevelManagers(UWorld* World)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::CreateStreamingLevelManagers(): %s, World: %s"), *GetNameSafe(this), *GetNameSafe(World));

	RemoveStreamingLevelManagers();

	if (!IsValid(World))
	{
		return;
	}

	const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();
	StreamingLevelManagers.Reserve(StreamingLevels.Num());
	for (ULevelStreaming* Level : StreamingLevels)
	{
		FString ShortLevelName = FPackageName::GetShortName(Level->GetWorldAssetPackageName());
		if (ShortLevelName.StartsWith(World->StreamingLevelsPrefix))
		{
			ShortLevelName = ShortLevelName.RightChop(World->StreamingLevelsPrefix.Len());
		}

		UStreamingSubsystemManager* Manager = NewObject<UStreamingSubsystemManager>(this);
		Manager->Initialize(Level, ShortLevelName);

		StreamingLevelManagers.Add(ShortLevelName, Manager);
	}
}

void UStreamingSubsystem::RemoveStreamingLevelManagers()
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::RemoveStreamingLevelManagers(): %s"), *GetNameSafe(this));

	for (TPair<FString, UStreamingSubsystemManager*>& Manager : StreamingLevelManagers)
	{
		if (!IsValid(Manager.Value))
		{
			continue;
		}
		Manager.Value->Deinitialize();
	}

	StreamingLevelManagers.Empty();
}

void UStreamingSubsystem::OnPreLoadMap(const FString& MapName)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::OnPreLoadMap(): %s, MapName: %s"), *GetNameSafe(this), *MapName);

	RemoveStreamingLevelManagers();
}

void UStreamingSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("UStreamingSubsystem::OnPostLoadMapWithWorld(): %s, LoadedWorld: %s"), *GetNameSafe(this), *GetNameSafe(LoadedWorld));

	CreateStreamingLevelManagers(LoadedWorld);
}

bool UStreamingSubsystem::FindStreamingLevelManager(const FString& LevelName, UStreamingSubsystemManager*& LevelManager)
{
	UStreamingSubsystemManager** ManagerPtr = StreamingLevelManagers.Find(LevelName);
	if (ManagerPtr == nullptr || !IsValid(*ManagerPtr))
	{
		LevelManager = nullptr;

		return false;
	}

	LevelManager = *ManagerPtr;

	return true;
}

