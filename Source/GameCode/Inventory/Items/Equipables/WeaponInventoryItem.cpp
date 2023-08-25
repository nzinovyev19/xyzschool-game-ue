// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInventoryItem.h"

UWeaponInventoryItem::UWeaponInventoryItem()
{
	bIsConsumable = true;
}

void UWeaponInventoryItem::SetEquipWeaponClass(TSubclassOf<AEquipableItem>& WeaponClass)
{
	EquipWeaponClass = WeaponClass;
}

TSubclassOf<AEquipableItem> UWeaponInventoryItem::GetEquipWeaponClass() const
{
	return EquipWeaponClass;
}
