// Fill out your copyright notice in the Description page of Project Settings.

#ifndef SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_
#define SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_

#pragma once

#include "CoreMinimal.h"
#include "GameCode/GameCodeTypes.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

UCLASS()
class GAMECODE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	EEquippableItemType GetItemType();

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	FName GetUnEquippedSocketName() const;
	FName GetEquippedSocketName() const;

	virtual void Equip();
	virtual void UnEquip();

	virtual EReticleType GetReticleType() const;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquippableItemType ItemType = EEquippableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;
#endif SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None;
};
