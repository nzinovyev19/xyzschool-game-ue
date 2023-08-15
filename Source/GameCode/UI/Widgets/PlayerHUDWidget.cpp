// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "HighlightInteractable.h"
#include "Blueprint/WidgetTree.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if (!IsValid(InteractableKey))
	{
		return;
	}
	
	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (IsValid(InteractableKey))
	{
		InteractableKey->SetActionText(KeyName);
	}
}
