// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);

class ARangeWeaponItem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquippableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeaponItem() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

	void ReloadCurrentWeapon();

	void EquipItemInSlot(EEquipmentSlots Slot);
	void EquipNextItem();
	void EquipPreviousItem();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

private:
	TAmunitionArray AmunitionArray;
	TItemsArray ItemsArray;
	
	void CreateLoadout();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	ARangeWeaponItem* CurrentEquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	AEquipableItem* CurrentEquippedItem;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadHandle;
	EEquipmentSlots CurrentEquippedSlot;
	
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
}; 
