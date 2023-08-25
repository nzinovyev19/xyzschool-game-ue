#include "EquipableItem.h"
#include "GameCode/Characters/GCBaseCharacter.h"

AEquipableItem::AEquipableItem()
{
	SetReplicates(true);
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("AEquipableItem::SetOwner only character can be an owner of equipable item" ))
		CachedCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
		if (GetLocalRole() == ROLE_Authority)
		{
			SetAutonomousProxy(true);
		}
	}
	else
	{
		CachedCharacterOwner = nullptr;
	}
}

AGCBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.IsValid() ? CachedCharacterOwner.Get() : nullptr;
}

EEquippableItemType AEquipableItem::GetItemType()
{
	return ItemType;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

FName AEquipableItem::GetDataTableID() const
{
	return DataTableID;
}

bool AEquipableItem::IsSlotCompatible(EEquipmentSlots Slot)
{
	return CompatibleEquipmentSlots.Contains(Slot);
}
