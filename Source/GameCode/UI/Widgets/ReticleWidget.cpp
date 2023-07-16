#include "ReticleWidget.h"

void UReticleWidget::OnEquippedItemChanged_Implementation(const AEquipableItem* EquipableItem)
{
	CurrentEquippedItem = EquipableItem;
	SetupCurrentReticle();
}

void UReticleWidget::OnAimingStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UReticleWidget::SetupCurrentReticle()
{
	 CurrentReticle = CurrentEquippedItem.IsValid() ? CurrentEquippedItem->GetReticleType() : EReticleType::None;
}
