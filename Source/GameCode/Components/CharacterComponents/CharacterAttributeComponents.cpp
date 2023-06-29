
#include "CharacterAttributeComponents.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "GameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameCode/Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

UCharacterAttributeComponents::UCharacterAttributeComponents()
{
	PrimaryComponentTick.bCanEverTick = true;
}


float UCharacterAttributeComponents::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void UCharacterAttributeComponents::BeginPlay()
{
	Super::BeginPlay();
	checkf(MaxHealth > 0.0f, TEXT("UCharacterAttributeComponents::BeginPlay max health cannot be less then 0"))
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterAttributeComponents::BeginPlay can be used only with AGCBaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());

	Health = MaxHealth;
	Oxygen = MaxOxygen;
	Stamina = MaxStamina;
	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponents::OnTakeAnyDamage);
}

void UCharacterAttributeComponents::UpdateStaminaValue(float DeltaTime)
{
	const float StaminaDelta = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting() ? -SprintStaminaConsumptionVelocity : SprintStaminaConsumptionVelocity;
	Stamina = FMath::Clamp(Stamina + StaminaDelta * DeltaTime, 0.0f, MaxStamina);
	if (OnOutOfStaminaEventSignature.IsBound())
	{
		if (FMath::IsNearlyEqual(Stamina, MaxStamina))
		{
			OnOutOfStaminaEventSignature.Broadcast(false);
		}
		else if (FMath::IsNearlyZero(Stamina))
		{
			OnOutOfStaminaEventSignature.Broadcast(true);
		}
	}
	
}

void UCharacterAttributeComponents::UpdateOxygenValue(float DeltaTime)
{
	const float OxygenDelta = CachedBaseCharacterOwner->IsSwimmingUnderWater() ? OxygenRestoreVelocity : -SwimOxygenConsumptionVelocity;
	Oxygen = FMath::Clamp(Oxygen + OxygenDelta * DeltaTime, 0.0f, MaxOxygen);
	if (FMath::IsNearlyZero(Oxygen))
	{
		CachedBaseCharacterOwner->TakeDamage(
			DamageFromOutOfOxygen / DamageFromOutOfOxygenVelocity * DeltaTime,
			FDamageEvent(),
			CachedBaseCharacterOwner->GetController(),
			// TODO: use at least actor of physics volume
			CachedBaseCharacterOwner->GetOwner()
		);
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponents::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector TextLocationOfHealth =
		CachedBaseCharacterOwner->GetActorLocation() +
		(CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocationOfHealth, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
	
	FVector TextLocationOfStamina =
		CachedBaseCharacterOwner->GetActorLocation() +
		(CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocationOfStamina, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Blue, 0.0f, true);

	FVector TextLocationOfOxygen =
		CachedBaseCharacterOwner->GetActorLocation() +
		(CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 15.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocationOfOxygen, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::White, 0.0f, true);
}
#endif

void UCharacterAttributeComponents::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}
	
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponents::OnTakeAnyDamage %s recevied %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	if (Health <= 0.0f)
	{
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponents::OnTakeAnyDamage character %s is killed by an actor %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}


void UCharacterAttributeComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

