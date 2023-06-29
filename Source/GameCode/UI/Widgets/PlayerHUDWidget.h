// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UReticleWidget* GetReticleWidget();

	UAmmoWidget* GetAmmoWidget();

protected:
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName AmmoWidgetName;
};
