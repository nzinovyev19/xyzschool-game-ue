// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighlightInteractable.generated.h"

class UTextBlock;
UCLASS()
class GAMECODE_API UHighlightInteractable : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetActionText(FName KeyName);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionText;
};
