// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributesWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UCharacterAttributesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
	
	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetOxygenPercent() const;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility StaminaBarIsVisible() const;
	
	UFUNCTION(BlueprintCallable)
	ESlateVisibility OxygenBarIsVisible() const;
};
