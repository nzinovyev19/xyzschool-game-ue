// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesWidget.h"

#include "GameCode/Characters/GCBaseCharacter.h"

float UCharacterAttributesWidget::GetHealthPercent() const
{
	float Result = 1;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UCharacterAttributeComponents* CharacterAttributes = Character->GetCharacterAttributesComponent();
		Result = CharacterAttributes->GetHealthPercent();
	}
	return Result;
}

float UCharacterAttributesWidget::GetOxygenPercent() const
{
	float Result = 1;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UCharacterAttributeComponents* CharacterAttributes = Character->GetCharacterAttributesComponent();
		Result = CharacterAttributes->GetOxygenPercent();
	}
	return Result;
}

float UCharacterAttributesWidget::GetStaminaPercent() const
{
	float Result = 1;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UCharacterAttributeComponents* CharacterAttributes = Character->GetCharacterAttributesComponent();
		Result = CharacterAttributes->GetStaminaPercent();
	}
	return Result;
}

ESlateVisibility UCharacterAttributesWidget::StaminaBarIsVisible() const
{
	return GetStaminaPercent() < 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

ESlateVisibility UCharacterAttributesWidget::OxygenBarIsVisible() const
{
	return GetOxygenPercent() < 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}


