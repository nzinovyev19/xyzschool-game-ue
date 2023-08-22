// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Inventory/Items/InventoryItem.h"
#include "Medkit.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()

public:
	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
	float Health = 25.0f;
};
