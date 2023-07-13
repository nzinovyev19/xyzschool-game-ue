// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"

#include "GameCode/Actors/Projectiles/GCProjectile.h"
#include "GameCode/Characters/GCBaseCharacter.h"

void AThrowableItem::Throw()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("AThrowableItem::Throw only character can be an owner of a throwable weapon"))
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);
	
	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;
	AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);

	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

EAmunitionType AThrowableItem::GetAmmoType() const
{
	return AmmoType;
}
