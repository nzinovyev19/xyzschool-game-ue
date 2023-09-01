#include "SaveSubsystem.h"

#include "SaveSubsystemInterface.h"
#include "SaveSubsystemTypes.h"
#include "SaveSubsystemUtils.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"

const FGameSaveData& USaveSubsystem::GetGameSaveData() const
{
	return GameSaveData;
}

void USaveSubsystem::SaveGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SaveGame(): %s"), *GetNameSafe(this));

	SerializeGame();
	WriteSaveToFile();
}

void USaveSubsystem::LoadLastGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s"), *GetNameSafe(this));

	if (SaveIds.Num() == 0)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadLastGame(): %s Failed! No saves."), *GetNameSafe(this));
		return;
	}

	LoadGame(SaveIds[SaveIds.Num() - 1]);
}

void USaveSubsystem::LoadGame(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadGame()"));
	if (!SaveIds.Contains(SaveId))
	{
		UE_LOG(LogSaveSubsystem, Warning, TEXT("USaveSubsystem::LoadGame(): Failed!"));
		return;
	}
	
	// RemoveStreamingLevelObservers();

	LoadSaveFromFile(SaveId);
	UGameplayStatics::OpenLevel(this, GameSaveData.LevelName);

}

void USaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Initialize(): %s"), *GetNameSafe(this));

	GameSaveData = FGameSaveData();

	SaveDirectoryName = FString::Printf(TEXT("%sSaveGames/"), *FPaths::ProjectSavedDir());

	FSaveDirectoryVisitor DirectoryVisitor(SaveIds);
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*SaveDirectoryName, DirectoryVisitor);
	SaveIds.Sort();

	// CreateStreamingLevelObservers(GetWorld());

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USaveSubsystem::OnPostLoadMapWithWorld);

}

void USaveSubsystem::Deinitialize()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::Deinitialize(): %s"), *GetNameSafe(this));

	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	// RemoveStreamingLevelObservers();

	Super::Deinitialize();
}

UWorld* USaveSubsystem::GetWorld() const
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

void USaveSubsystem::SerializeLevel(const ULevel* Level, const ULevelStreaming* StreamingLevel /*= nullptr*/)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.PersistentLevel;
	// if (IsValid(StreamingLevel))
	// {
	// 	TArray<FLevelSaveData>& StreamingLevles = GameSaveData.StreamingLevels;
	// 	LevelSaveData = &StreamingLevles[StreamingLevles.Emplace(StreamingLevel->GetWorldAssetPackageFName())];
	//
	// }
	// else
	// {
	// 	LevelSaveData = &GameSaveData.PersistentLevel;
	//
	// }

	TArray<FActorSaveData>& ActorsSaveData = LevelSaveData->ActorsSaveData;
	ActorsSaveData.Empty();

	for (AActor* Actor : Level->Actors)
	{
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
		{
			continue;
		}

		FActorSaveData& ActorSaveData = ActorsSaveData[ActorsSaveData.AddUnique(FActorSaveData(Actor))];
		ActorSaveData.Transform = Actor->GetTransform();

		TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData.ComponentsSaveData;
		ComponentsSaveData.Empty();
		
		for (UActorComponent* ActorComponent : Actor->GetComponents())
		{
			if (ActorComponent->Implements<USaveSubsystemInterface>())
			{
				FObjectSaveData& ComponentSaveData = ComponentsSaveData[ComponentsSaveData.Emplace(ActorComponent)];
				FMemoryWriter MemoryWriter(ComponentSaveData.RawData, true);
				FSaveSubsystemArchive Archive(MemoryWriter, false);
				ActorComponent->Serialize(Archive);
			}
		}

		FMemoryWriter MemoryWriter(ActorSaveData.RawData, true);
		FSaveSubsystemArchive Archive(MemoryWriter, false);
		
		Actor->Serialize(Archive);
	}

}

void USaveSubsystem::DeserializeLevel(ULevel* Level, const ULevelStreaming* StreamingLevel /*= nullptr*/)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Level: %s, StreamingLevel: %s"), *GetNameSafe(this), *GetNameSafe(Level), *GetNameSafe(StreamingLevel));

	FLevelSaveData* LevelSaveData = nullptr;
	LevelSaveData = &GameSaveData.PersistentLevel;
	// if (IsValid(StreamingLevel))
	// {
	// 	const FName LevelName = StreamingLevel->GetWorldAssetPackageFName();
	// 	LevelSaveData = GameSaveData.StreamingLevels.FindByPredicate([=](const FLevelSaveData& Data) { return Data.Name == LevelName; });
	// }
	// else
	// {
	// 	LevelSaveData = &GameSaveData.PersistentLevel;
	// }

	if (LevelSaveData == nullptr)
	{
		// There is no save for this level yet. Call OnLevelDeserialized() in all cases!
		USaveSubsystemUtils::BroadcastOnLevelDeserialized(Level);
		return;
	}

	TArray<AActor*> ActorsToNotify;

	TArray<FActorSaveData*> ActorsSaveData;
	ActorsSaveData.Reserve(LevelSaveData->ActorsSaveData.Num());
	for (FActorSaveData& ActorSaveData : LevelSaveData->ActorsSaveData)
	{
		ActorsSaveData.Add(&ActorSaveData);
	}

	for (TArray<AActor*>::TIterator ActorIterator = Level->Actors.CreateIterator(); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (!IsValid(Actor) || !Actor->Implements<USaveSubsystemInterface>())
		{
			continue;
		}

		FActorSaveData* ActorSaveData = nullptr;
		for (TArray<FActorSaveData*>::TIterator ActorSaveDataIterator = ActorsSaveData.CreateIterator(); ActorSaveDataIterator; ++ActorSaveDataIterator)
		{
			if ((*ActorSaveDataIterator)->Name == Actor->GetFName())
			{
				ActorSaveData = *ActorSaveDataIterator;
				ActorSaveDataIterator.RemoveCurrent();
				break;
			}
		}

		if (ActorSaveData == nullptr)
		{
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, ActorSaveData not found! Destroy actor: %s"), *GetNameSafe(this), *GetNameSafe(Actor));
			Actor->Destroy();
		}
		else
		{
			DeserializeActor(Actor, ActorSaveData);

			if (Actor->Implements<USaveSubsystemInterface>())
			{
				ActorsToNotify.Add(Actor);
			}
		}
	}

	UWorld* const World = GetWorld();

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.OverrideLevel = Level;
	ActorSpawnParameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (FActorSaveData* ActorSaveData : ActorsSaveData)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());

		ActorSpawnParameters.Name = ActorSaveData->Name;

		// @bIgnoreOnActorSpawnedCallbackFast is hook to avoid double @OnLevelDeserialized invocation!
		BoolScopeWrapper OnActorSpawnedHook(bIgnoreOnActorSpawnedCallback, true);

		AActor* Actor = World->SpawnActor(ActorSaveData->Class.Get(), &ActorSaveData->Transform, ActorSpawnParameters);

		if (!IsValid(Actor))
		{
			// This is valid case. Actor can be IsPendingKill due some actor's logic started from overlaps.
			UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeLevel(): %s, Failed to spawn new actor with name: %s"), *GetNameSafe(this), *ActorSaveData->Name.ToString());
			continue;
		}

		// Actor name can change so update it.
		ActorSaveData->Name = Actor->GetFName();

		DeserializeActor(Actor, ActorSaveData);

		if (Actor->Implements<USaveSubsystemInterface>())
		{
			ActorsToNotify.Add(Actor);
		}
	}

	for (AActor* Actor : ActorsToNotify)
	{
		NotifyActorsAndComponents(Actor);
	}

}

// void USaveSubsystem::CreateStreamingLevelObservers(UWorld* World)
// {
// 	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::CreateStreamingLevelObservers(): %s, World: %s"), *GetNameSafe(this), *GetNameSafe(World));
//
// 	RemoveStreamingLevelObservers();
//
// 	if (!IsValid(World))
// 	{
// 		return;
// 	}
//
//
// 	FOnActorSpawned::FDelegate OnActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &USaveSubsystem::OnActorSpawned);
// 	OnActorSpawnedDelegateHandle = World->AddOnActorSpawnedHandler(OnActorSpawnedDelegate);
//
// 	const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();
// 	StreamingLevelObservers.Reserve(StreamingLevels.Num());
// 	for (ULevelStreaming* Level : StreamingLevels)
// 	{
// 		UStreamingLevelObserver* Observer = NewObject<UStreamingLevelObserver>(this);
// 		Observer->Initialize(this, Level);
// 		StreamingLevelObservers.Add(Observer);
// 	}
//
// }
//
// void USaveSubsystem::RemoveStreamingLevelObservers()
// {
// 	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::RemoveStreamingLevelObservers(): %s"), *GetNameSafe(this));
//
// 	UWorld* World = GetWorld();
// 	if (IsValid(World))
// 	{
// 		World->RemoveOnActorSpawnedHandler(OnActorSpawnedDelegateHandle);
// 	}
//
// 	OnActorSpawnedDelegateHandle.Reset();
//
// 	for (UStreamingLevelObserver* Observer : StreamingLevelObservers)
// 	{
// 		if (!IsValid(Observer))
// 		{
// 			continue;
// 		}
//
// 		Observer->Deinitialize();
// 	}
//
// 	StreamingLevelObservers.Empty();
//
// }

void USaveSubsystem::SerializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::SerializeGame(): %s"), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance();
	GameSaveData.GameInstance = FObjectSaveData(GetGameInstance());
	FMemoryWriter MemoryWriter(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryWriter, false);
	GameInstance->Serialize(Archive);

	const UWorld* World = GetWorld();
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (World->IsPlayInEditor())
	{
		LevelName = UWorld::RemovePIEPrefix(LevelName);
	}

	GameSaveData.LevelName = FName(LevelName);

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (IsValid(PlayerCharacter))
	{
		GameSaveData.StartTransform = PlayerCharacter->GetTransform();
	}

	SerializeLevel(World->PersistentLevel);

	// for (const ULevelStreaming* Level : World->GetStreamingLevels())
	// {
	// 	if (Level->IsLevelLoaded() && Level->GetCurrentState() == ULevelStreaming::ECurrentState::LoadedVisible)
	// 	{
	// 		SerializeLevel(Level->GetLoadedLevel(), Level);
	// 	}
	// }
}

void USaveSubsystem::DeserializeGame()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeGame(): %s"), *GetNameSafe(this));

	UGameInstance* GameInstance = GetGameInstance();
	FMemoryReader MemoryReader(GameSaveData.GameInstance.RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	GameInstance->Serialize(Archive);

	const UWorld* World = GetWorld();

	DeserializeLevel(World->PersistentLevel);

	// for (const ULevelStreaming* Level : World->GetStreamingLevels())
	// {
	// 	if (Level->IsLevelLoaded())
	// 	{
	// 		DeserializeLevel(Level->GetLoadedLevel(), Level);
	// 	}
	// }
}

void USaveSubsystem::WriteSaveToFile()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::WriteSaveToFile(): %s"), *GetNameSafe(this));

	const int32 SaveId = GetNextSaveId();
	SaveIds.AddUnique(SaveId);

	GameSaveData.Name = FName(FString::FromInt(SaveId));

	TArray<uint8> SaveBytes;
	FMemoryWriter MemoryWriter(SaveBytes);
	FObjectAndNameAsStringProxyArchive WriterArchive(MemoryWriter, false);
	GameSaveData.Serialize(WriterArchive);

	FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*GetSaveFilePath(SaveId));

	if (bUseCompressedSaves)
	{
		TArray<uint8> CompressedSaveBytes;
		FArchiveSaveCompressedProxy CompressedArchive(CompressedSaveBytes, NAME_Zlib);
		CompressedArchive << SaveBytes;
		CompressedArchive.Flush();

		*FileWriter << CompressedSaveBytes;
	}
	else
	{
		*FileWriter << SaveBytes;
	}

	FileWriter->Close();
	delete FileWriter;

}

void USaveSubsystem::LoadSaveFromFile(int32 SaveId)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::LoadSaveFromFile(): %s, SaveId %i"), *GetNameSafe(this), SaveId);

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*GetSaveFilePath(SaveId));
	TArray<uint8> SaveBytes;

	if (bUseCompressedSaves)
	{
		TArray<uint8> CompressedSaveBytes;
		*FileReader << CompressedSaveBytes;

		FArchiveLoadCompressedProxy DecompressedArchive(CompressedSaveBytes, NAME_Zlib);
		DecompressedArchive << SaveBytes;
		DecompressedArchive.Flush();
	}
	else
	{
		*FileReader << SaveBytes;
	}

	FileReader->Close();
	delete FileReader;

	FMemoryReader MemoryReader(SaveBytes, true);
	FObjectAndNameAsStringProxyArchive ReaderArchive(MemoryReader, true);
	GameSaveData = FGameSaveData();
	GameSaveData.Serialize(ReaderArchive);

}

void USaveSubsystem::OnPostLoadMapWithWorld(UWorld* LoadedWorld)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnPostLoadMapWithWorld(): %s, World: %s"), *GetNameSafe(this), *GetNameSafe(LoadedWorld));
	
	// CreateStreamingLevelObservers(LoadedWorld);
	
	DeserializeGame();

}

void USaveSubsystem::DeserializeActor(AActor* Actor, const FActorSaveData* ActorSaveData)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::DeserializeActor(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(Actor));

	Actor->SetActorTransform(ActorSaveData->Transform);

	const TArray<FObjectSaveData>& ComponentsSaveData = ActorSaveData->ComponentsSaveData;
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			const FObjectSaveData* ComponentSaveData = ComponentsSaveData.FindByPredicate([=](const FObjectSaveData& SaveData) { return SaveData.Name == ActorComponent->GetFName(); });
			FMemoryReader MemoryReader(ComponentSaveData->RawData, true);
			FSaveSubsystemArchive Archive(MemoryReader, false);
			ActorComponent->Serialize(Archive);
		}
	}

	FMemoryReader MemoryReader(ActorSaveData->RawData, true);
	FSaveSubsystemArchive Archive(MemoryReader, false);
	Actor->Serialize(Archive);
}

FString USaveSubsystem::GetSaveFilePath(int32 SaveId) const
{
	return SaveDirectoryName / FString::Printf(TEXT("%i.save"), SaveId);
}

int32 USaveSubsystem::GetNextSaveId() const
{
	if (SaveIds.Num() == 0)
	{
		return 1;
	}

	return SaveIds[SaveIds.Num() - 1] + 1;
}

void USaveSubsystem::OnActorSpawned(AActor* SpawnedActor)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Actor %s"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
	if (bIgnoreOnActorSpawnedCallback)
	{
		UE_LOG(LogSaveSubsystem, Display, TEXT("USaveSubsystem::OnActorSpawned(): %s, Skipped for actor %s. bIgnoreOnActorSpawnedCallback == true!"), *GetNameSafe(this), *GetNameSafe(SpawnedActor));
		return;
	}

	if (IsValid(SpawnedActor) && SpawnedActor->HasActorBegunPlay() && SpawnedActor->Implements<USaveSubsystemInterface>())
	{
		// We should notify a runtime spawned actors too.
		NotifyActorsAndComponents(SpawnedActor);
	}
}

void USaveSubsystem::NotifyActorsAndComponents(AActor* Actor)
{
	ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
	for (UActorComponent* ActorComponent : Actor->GetComponents())
	{
		if (ActorComponent->Implements<USaveSubsystemInterface>())
		{
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(ActorComponent);
		}
	}
}
