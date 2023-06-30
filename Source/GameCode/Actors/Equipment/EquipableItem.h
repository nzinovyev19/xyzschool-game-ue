// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/GameCodeTypes.h"
#include "GameFramework/Actor.h"
#include "EquipableItem.generated.h"

UCLASS()
class GAMECODE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	EEquippableItemType GetItemType();

	FName GetUnEquippedSocketName() const;
	FName GetEquippedSocketName() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquippableItemType ItemType = EEquippableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;
};
