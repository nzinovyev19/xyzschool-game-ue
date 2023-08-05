// #ifndef SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_
// #define SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameCode/GameCodeTypes.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

// #endif SOURCE_GAMECODE_ACTORS_EQUIPMENT_EQUIPABLEITEM_H_

class AGCBaseCharacter;
UCLASS()
class GAMECODE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquipableItem();
	
	virtual void SetOwner(AActor* NewOwner) override;
	
	EEquippableItemType GetItemType();

	UAnimMontage* GetCharacterEquipAnimMontage() const;

	FName GetUnEquippedSocketName() const;
	FName GetEquippedSocketName() const;

	virtual void Equip();
	virtual void UnEquip();

	virtual EReticleType GetReticleType() const;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquippableItemType ItemType = EEquippableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None;

	AGCBaseCharacter* GetCharacterOwner() const;
	
private:
	TWeakObjectPtr<AGCBaseCharacter> CachedCharacterOwner;
		
};
