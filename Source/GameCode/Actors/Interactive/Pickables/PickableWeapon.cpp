#include "PickableWeapon.h"

#include "Engine/DataTable.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "GameCode/Inventory/Items/InventoryItem.h"
#include "GameCode/Utils/GCDataTableUtils.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

void APickableWeapon::Interact(AGCBaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = GCDataTableUtils::FindWeaponData(DataTableID);
	if (WeaponRow)
	{
		Character->AddEquipmentItem(WeaponRow->EquipableActor);
		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}
