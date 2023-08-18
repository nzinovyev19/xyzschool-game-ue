// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class AEquipableItem;
class ARangeWeaponItem;
class AThrowableItem;
class AMeleeWeaponItem;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowableCountChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	EEquippableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeaponItem() const;
	
	AMeleeWeaponItem* GetCurrentMeleeWeaponItem() const;

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

	FOnCurrentThrowableCountChanged OnCurrentThrowableCountChangedEvent;

	FOnEquippedItemChanged OnEquippedItemChanged;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull);
	void UnEquipCurrentItem();
	void AttachCurrentItemToEquippedSocket();

	void EquipItemInSlot(EEquipmentSlots Slot);
	void EquipNextItem();
	void EquipPreviousItem();
	bool IsEquipping() const;

	void LaunchCurrentThrowableItem();

	void AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoredSlotsWhileSwitching;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemIsSlot = EEquipmentSlots::None;

private:
	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	UPROPERTY(Replicated)
	TArray<int32> AmmunitionArray;
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray)
	TArray<AEquipableItem*> ItemsArray;

	UFUNCTION()
	void OnRep_ItemsArray();
	
	void CreateLoadout();

	void AutoEquip();

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

	UPROPERTY(EditDefaultsOnly, Transient, Category = "Character | Attributes")
	AMeleeWeaponItem* CurrentMeleeWeaponItem;

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadHandle;
	EEquipmentSlots PreviousEquippedSlot;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquippedSlot)
	EEquipmentSlots CurrentEquippedSlot;
	void EquipAnimationFinished();
	UFUNCTION()
	void OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old);
	
	bool IsReadyToEquip(EEquipmentSlots Slot);

	FTimerHandle EquipTimer;
	
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
}; 
