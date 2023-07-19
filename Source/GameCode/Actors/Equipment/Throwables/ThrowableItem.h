// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

class AGCBaseCharacter;
UCLASS(Blueprintable)
class GAMECODE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	void Throw();
	EAmunitionType GetAmmoType() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AGCProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Parameters | Ammo")
	EAmunitionType AmmoType;
};
