#include "SaveData.h"

FBaseSaveData::FBaseSaveData()
{

}

bool FBaseSaveData::Serialize(FArchive& Archive)
{

	Archive << Name;
	return true;
}

//
FObjectSaveData::FObjectSaveData()
	: Class(nullptr)
{
}

FObjectSaveData::FObjectSaveData(const UObject* Object)
	: Class(nullptr)
{
	if (IsValid(Object))
	{
		Name = Object->GetFName();
		Class = TStrongObjectPtr<UClass>(Object->GetClass());
	}
}

bool FObjectSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);

	UClass* ClassPtr = Class.Get();
	Archive << ClassPtr;

	if (Archive.IsLoading() && IsValid(ClassPtr))
	{
		Class = TStrongObjectPtr<UClass>(ClassPtr);
	}

	Archive << RawData;
	return true;
}

//
FActorSaveData::FActorSaveData()
	: Transform(FTransform::Identity)
{
}

FActorSaveData::FActorSaveData(const AActor* Actor)
	: Super(Actor)
	, Transform(FTransform::Identity)
{
}

bool FActorSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << Transform;
	Archive << ComponentsSaveData;
	return true;
}

//
FLevelSaveData::FLevelSaveData()
{
}

FLevelSaveData::FLevelSaveData(const FName& LevelName)
{
	Name = LevelName;
}

bool FLevelSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << ActorsSaveData;
	return true;
}

//
FGameSaveData::FGameSaveData()
	: PersistentLevel(FName(TEXT("Persistent")))
	, StartTransform(FTransform::Identity)
	, bIsSerialized(false)
{
}

bool FGameSaveData::Serialize(FArchive& Archive)
{
	Super::Serialize(Archive);
	Archive << LevelName;
	Archive << PersistentLevel;
	Archive << StreamingLevels;
	Archive << GameInstance;
	Archive << StartTransform;

	bIsSerialized = true;
	return true;
}
