// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	void Throw();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AGCProjectile> ProjectileClass;
	
};
