// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


DECLARE_LOG_CATEGORY_EXTERN(LogSaveSubsystem, Log, All);
const FName FileExtensionSave = TEXT("save");

/**
 * Used for loading and saving.
 */
struct FSaveSubsystemArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveSubsystemArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails);
};

/**
 * Used to get save file names as save id's.
 */
class FSaveDirectoryVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
	FSaveDirectoryVisitor(TArray<int32>& InSaveIds);
	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override;

private:
	TArray<int32>& SaveIds;
};

/**
 * Used to change bool value for scope.
 */
struct BoolScopeWrapper
{
public:
	BoolScopeWrapper(bool& bInValue, bool bNewValue);
	~BoolScopeWrapper();

private:
	bool& bValue;
	bool bInitialValue;
};


// class USaveSubsystem;
// class ULevelStreaming;
// UCLASS()
// class UStreamingLevelObserver : public UObject
// {
// 	GENERATED_BODY()
//
// public:
// 	UStreamingLevelObserver();
//
// 	void Initialize(USaveSubsystem* InSaveSubsystem, ULevelStreaming* InStreamingLevel);
// 	void Deinitialize();
//
// 	virtual void Serialize(FArchive& Archive) override;
//
// private:
// 	UFUNCTION()
// 	void OnLevelShown();
//
// 	UFUNCTION()
// 	void OnLevelHidden();
//
// 	FLevelSaveData LevelSaveData;
// 	TWeakObjectPtr<USaveSubsystem> SaveSubsystem;
// 	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
// };
