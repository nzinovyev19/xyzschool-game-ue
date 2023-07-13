// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Actors/Equipment/Throwables/ThrowableItem.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowableCountChanged, int32);

class ARangeWeaponItem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquippableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeaponItem() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

	FOnCurrentThrowableCountChanged OnCurrentThrowableCountChangedEvent;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull);
	void UnEquipCurrentItem();
	void AttachCurrentItemToEquippedSocket();

	void EquipItemInSlot(EEquipmentSlots Slot);
	void EquipNextItem();
	void EquipPreviousItem();
	bool IsEquipping() const;

	void LaunchCurrentThrowableItem();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoredSlotsWhileSwitching;

private:
	TAmmunitionArray AmmunitionArray;
	TItemsArray ItemsArray;
	
	void CreateLoadout();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	int32 GetAvailableAmunitionForCurrentWeapon();
	int32 GetAvailableAmunitionForGrenade();

	bool bIsEquipping = false;

	UFUNCTION()
	void OnWeaponReloadComplete();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentThrowableCountChanged();

	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	ARangeWeaponItem* CurrentEquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	AEquipableItem* CurrentEquippedItem;

	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	AThrowableItem* CurrentThrowableItem;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadHandle;
	EEquipmentSlots PreviousEquippedSlot;
	EEquipmentSlots CurrentEquippedSlot;
	void EquipAnimationFinished();
	
	bool IsReadyToEquip(AEquipableItem* Item);

	FTimerHandle EquipTimer;
	
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
}; 
