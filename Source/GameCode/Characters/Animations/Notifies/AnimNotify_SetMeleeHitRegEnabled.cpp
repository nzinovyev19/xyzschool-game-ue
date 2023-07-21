// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SetMeleeHitRegEnabled.h"

#include "GameCode/Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "GameCode/Characters/GCBaseCharacter.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeaponItem* MeleeWeaponItem = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeaponItem();
	if (IsValid(MeleeWeaponItem))
	{
		MeleeWeaponItem->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
