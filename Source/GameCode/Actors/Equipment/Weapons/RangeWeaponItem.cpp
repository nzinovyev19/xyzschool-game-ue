// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"

#include "GameCode/GameCodeTypes.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "GameCode/Components/Weapon/WeaponBarellComponent.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);
}

void ARangeWeaponItem::MakeShot()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::Fire only character can be an owner of range weapon"))
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());

	if (!CanShoot())
	{
		StopFire();
		if (Ammo == 0 && bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}

	EndReload(false);
	
	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	ViewDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, GetCurrentBulletSpreadAngle()), PlayerViewRotation);

	SetAmmo(Ammo - 1);
	WeaponBarell->Shot(PlayerViewPoint, ViewDirection, Controller);
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
	return Result;
}

void ARangeWeaponItem::StartFire()
{
	MakeShot();
	if (WeaponFireMode == EWeaponFireMode::FullAuto)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
		GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::MakeShot, GetShotTimerInterval(), true);
	}
}

void ARangeWeaponItem::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
}

void ARangeWeaponItem::StartAiming()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAiming()
{
	bIsAiming = false;
}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("ARangeWeaponItem::StartReload only character can be an owner of range weapon"))
	AGCBaseCharacter* CharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());

	bIsRealoding = true;
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
	}
	else
	{
		EndReload(true);
	}
	
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsRealoding)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	
	bIsRealoding = false;
	if (bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return Ammo;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return MaxAmmo;
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return Ammo > 0;
}

EAmunitionType ARangeWeaponItem::GetAmmoType() const
{
	return AmmoType;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	return AimTurnModifier;
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegress = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegress);
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / RateOfFire;
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result  = 0.0f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}


