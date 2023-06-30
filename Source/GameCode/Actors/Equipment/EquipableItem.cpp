#include "EquipableItem.h"

EEquippableItemType AEquipableItem::GetItemType()
{
	return ItemType;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}
