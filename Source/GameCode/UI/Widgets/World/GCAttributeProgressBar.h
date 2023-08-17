// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GCAttributeProgressBar.generated.h"

class UProgressBar;
UCLASS()
class GAMECODE_API UGCAttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressPercentage(float Percentage);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;
	
};
