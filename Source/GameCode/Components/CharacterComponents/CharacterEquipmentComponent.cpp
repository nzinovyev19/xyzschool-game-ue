#include "CharacterEquipmentComponent.h"

#include "GameCode/GameCodeTypes.h"
#include "GameCode/Actors/Equipment/Throwables/ThrowableItem.h"
#include "GameCode/Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "GameCode/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCode/Characters/GCBaseCharacter.h"

EEquippableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquippableItemType Result = EEquippableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeaponItem() const
{
	return CurrentEquippedWeapon;
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeaponItem() const
{
	return CurrentMeleeWeaponItem;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if (AvailableAmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);
	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if (!AvailableAmunition || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(
			CachedBaseCharacter->GetMesh(),
			FAttachmentTransformRules::KeepRelativeTransform,
			CurrentEquippedItem->GetUnEquippedSocketName()
		);
		CurrentEquippedItem->UnEquip();
	}
	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadHandle);
	}

	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(
			CachedBaseCharacter->GetMesh(),
			FAttachmentTransformRules::KeepRelativeTransform,
			CurrentEquippedItem->GetEquippedSocketName()
		);
	}
}

bool UCharacterEquipmentComponent::IsReadyToEquip(AEquipableItem* Item)
{
	CurrentThrowableItem = Cast<AThrowableItem>(Item);
	return !IsValid(CurrentThrowableItem) || AmmunitionArray[(uint32)CurrentThrowableItem->GetAmmoType()] != 0;
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping || !IsReadyToEquip(ItemsArray[(uint32)Slot]))
	{
		return;
	}
	UnEquipCurrentItem();
	
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeWeaponItem = Cast<AMeleeWeaponItem>(CurrentEquippedItem);
	
	if (IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage))
		{
			bIsEquipping = true;
			float EquipDuration = CachedBaseCharacter->PlayAnimMontage(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();	
		}
		CurrentEquippedItem->Equip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}

	if (OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}

	CurrentEquippedSlot = Slot;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while (
		CurrentSlotIndex != NextSlotIndex
		&& (IgnoredSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex)
		|| !IsValid(ItemsArray[NextSlotIndex]))
	)
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	while (
		CurrentSlotIndex != PreviousSlotIndex
		&& (IgnoredSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex)
		|| !IsValid(ItemsArray[PreviousSlotIndex]))
	)
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (IsValid(CurrentThrowableItem))
	{
		CurrentThrowableItem->Throw();

		AmmunitionArray[(uint32)EAmunitionType::FragGrenades] = GetAvailableAmunitionForGrenade() - 1;
		OnCurrentThrowableCountChanged();

		bIsEquipping = false;
		EquipItemInSlot(PreviousEquippedSlot);
	}
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with a BaseCharacter"));
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());
	
	CreateLoadout();
	OnCurrentThrowableCountChanged();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[(uint32)ItemPair.Key] = Item;
	}
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex -  1;
	}
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeaponItem());
	return AmmunitionArray[(uint32)GetCurrentRangeWeaponItem()->GetAmmoType()];
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForGrenade()
{
	return AmmunitionArray[(uint32)EAmunitionType::FragGrenades];
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon(0, false);
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon());
	}
}

void UCharacterEquipmentComponent::OnCurrentThrowableCountChanged()
{
	if (OnCurrentThrowableCountChangedEvent.IsBound())
	{
		OnCurrentThrowableCountChangedEvent.Broadcast(GetAvailableAmunitionForGrenade());
	}
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

