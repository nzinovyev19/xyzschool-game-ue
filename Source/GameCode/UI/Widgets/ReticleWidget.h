#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Actors/Equipment/EquipableItem.h"
#include "ReticleWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquipableItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle = EReticleType::None;

private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};
