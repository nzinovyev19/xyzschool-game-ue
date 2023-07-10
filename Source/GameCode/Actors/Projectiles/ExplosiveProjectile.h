// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Actors/Projectiles/GCProjectile.h"
#include "GameCode/Components/Explosion/ExplosionComponent.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API AExplosiveProjectile : public AGCProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 2.0f;

	virtual void OnProjectileLaunched() override;

private:
	AController* GetController();
	void OnDetonationTimerElapsed();
	FTimerHandle DetonationTimer;
	
};
