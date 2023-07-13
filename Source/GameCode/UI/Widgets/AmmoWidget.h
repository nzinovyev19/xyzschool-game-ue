// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 TotalAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 GrenadeAmount;

private:
	UFUNCTION()
	void UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo, int32 NewGrenadeAmount);

	UFUNCTION()
	void UpdateGrenadeCount(int32 NewGrenadeAmount);
};
