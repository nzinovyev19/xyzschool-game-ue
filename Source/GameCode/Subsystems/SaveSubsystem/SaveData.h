#pragma once

#include "CoreMinimal.h"

#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "SaveData.generated.h"

USTRUCT()
struct FBaseSaveData
{
	GENERATED_BODY()

public:
	FBaseSaveData();

	virtual ~FBaseSaveData() {}
	virtual bool Serialize(FArchive& Archive);

	friend FArchive& operator << (FArchive& Archive, FBaseSaveData& SaveData)
	{
		SaveData.Serialize(Archive);
		return Archive;
	}

	FName Name;
};

FORCEINLINE bool operator == (const FBaseSaveData& First, const FBaseSaveData& Second) { return First.Name == Second.Name; }


/**
 * Object data that will be saved/loaded.
 */
USTRUCT()
struct FObjectSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FObjectSaveData();
	FObjectSaveData(const UObject* Object);

	virtual bool Serialize(FArchive& Archive) override;

	TStrongObjectPtr<UClass> Class;
	TArray<uint8> RawData;
};

/**
 * Actor data that will be saved/loaded.
 */
USTRUCT()
struct FActorSaveData : public FObjectSaveData
{
	GENERATED_BODY()

	FActorSaveData();
	FActorSaveData(const AActor* Actor);

	virtual bool Serialize(FArchive& Archive) override;

	TArray<FObjectSaveData> ComponentsSaveData;
	FTransform Transform;
};

/**
 * Level data that will be saved/loaded.
 */
USTRUCT()
struct FLevelSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FLevelSaveData();
	FLevelSaveData(const FName& LevelName);

	virtual bool Serialize(FArchive& Archive) override;

	TArray<FActorSaveData> ActorsSaveData;
};

/**
 * Global game data that will be saved/loaded.
 */
USTRUCT()
struct FGameSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FGameSaveData();

	virtual bool Serialize(FArchive& Archive) override;

	FName LevelName;
	FLevelSaveData PersistentLevel;
	TArray<FLevelSaveData> StreamingLevels;
	FObjectSaveData GameInstance;
	FTransform StartTransform;

};
