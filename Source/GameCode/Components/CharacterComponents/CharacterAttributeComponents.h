// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponents.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfStaminaEventSignature, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterAttributeComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponents();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;

	FOnOutOfStaminaEventSignature OnOutOfStaminaEventSignature;

	bool IsAlive() { return Health > 0.0f; }

	float GetHealthPercent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
	float MaxHealth = 100.0f;

	void UpdateStaminaValue(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float StaminaRestoreVelocity = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina")
	float SprintStaminaConsumptionVelocity = 15.0f;

	void UpdateOxygenValue(float DeltaTime);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen")
	float MaxOxygen = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen")
	float OxygenRestoreVelocity = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen")
	float SwimOxygenConsumptionVelocity = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	float DamageFromOutOfOxygen = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	float DamageFromOutOfOxygenVelocity = 2.0f;
	
private:
	float Health = 0.0f;

	float Stamina = 0.0f;

	float Oxygen = 0.0f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacterOwner;
};
