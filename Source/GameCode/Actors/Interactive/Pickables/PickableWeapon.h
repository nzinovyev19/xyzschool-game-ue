#pragma once

#include "CoreMinimal.h"
#include "PickableItem.h"
#include "PickableWeapon.generated.h"

UCLASS(Blueprintable)
class GAMECODE_API APickableWeapon : public APickableItem
{
	GENERATED_BODY()

public:
	APickableWeapon();	
	
	virtual void Interact(AGCBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMesh;
	
};
