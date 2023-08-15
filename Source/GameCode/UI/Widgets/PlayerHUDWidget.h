// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "CharacterAttributesWidget.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UHighlightInteractable;
UCLASS()
class GAMECODE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UReticleWidget* GetReticleWidget();

	UAmmoWidget* GetAmmoWidget();

	UCharacterAttributesWidget* GetCharacterAttributesWidget();

	void SetHighlightInteractableVisibility(bool bIsVisible);

	void SetHighlightInteractableActionText(FName KeyName);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName CharacterAttributesWidgetName;

	UPROPERTY(meta = (BindWidget))
	UHighlightInteractable* InteractableKey;
};
