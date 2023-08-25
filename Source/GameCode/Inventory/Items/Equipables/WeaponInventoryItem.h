// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Inventory/Items/InventoryItem.h"
#include "WeaponInventoryItem.generated.h"

class AEquipableItem;
UCLASS()
class GAMECODE_API UWeaponInventoryItem : public UInventoryItem
{
	GENERATED_BODY()

public:
	UWeaponInventoryItem();
	
	void SetEquipWeaponClass(TSubclassOf<AEquipableItem>& WeaponClass);

	TSubclassOf<AEquipableItem> GetEquipWeaponClass() const;
	
protected:
	TSubclassOf<AEquipableItem> EquipWeaponClass;
	
};
