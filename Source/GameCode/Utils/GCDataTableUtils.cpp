#include "GCDataTableUtils.h"

#include "GameCode/Inventory/Items/InventoryItem.h"


FWeaponTableRow* GCDataTableUtils::FindWeaponData(FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/GameCode/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}
	
	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}
