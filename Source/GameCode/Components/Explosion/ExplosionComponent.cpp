#include "ExplosionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"


void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetComponentLocation(),
		InnerRadius,
		OuterRadius,
		DamageFalloff,
		DamageTypeClass,
		IgnoredActors,
		GetOwner(),
		Controller,
		ECC_Visibility
	);
	
	if (IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
	
	if (OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}
