// Fill out your copyright notice in the Description page of Project Settings.


#include "HighlightInteractable.h"

#include "Components/TextBlock.h"

void UHighlightInteractable::SetActionText(FName KeyName)
{
	if (IsValid(ActionText))
	{
		ActionText->SetText(FText::FromName(KeyName));
	}
}
