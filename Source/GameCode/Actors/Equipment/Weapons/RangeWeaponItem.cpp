#include "RangeWeaponItem.h"

#include "GameCode/GameCodeTypes.h"
#include "GameCode/Characters/GCBaseCharacter.h"


ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	
	WeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrel"));
	WeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	AlternativeWeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("AlternativeWeaponBarrel"));
	AlternativeWeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;
	CurrentWeaponBarrel = WeaponBarrel;
}

void ARangeWeaponItem::MakeShot()
{
	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	if (!CanShoot())
	{
		StopFire();
		if (WeaponBarrel->GetAmmo() == 0 && WeaponBarrel->GetAutoReload())
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

	SetAmmo(WeaponBarrel->GetAmmo() - 1);
	WeaponBarrel->Shot(PlayerViewPoint, ViewDirection, GetCurrentBulletSpreadAngle());

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}
		case EWeaponFireMode::FullAuto:
		{
			MakeShot();
		}
	}
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}
	bIsFiring = true;
	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
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
	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	bIsRealoding = true;
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if (ReloadType == EReloadType::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(
				ReloadTimer,
				[this]() { EndReload(true); },
				MontageDuration,
				false
			);
		}
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

	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();

	if (!bIsSuccess)
	{
		if (IsValid(CharacterOwner))
		{
			CharacterOwner->StopAnimMontage(CharacterReloadMontage);
			StopAnimMontage(WeaponReloadMontage);
		}
	}

	if (ReloadType == EReloadType::ByBullet)
	{
		UAnimInstance* CharacterAnimInstance = IsValid(CharacterOwner) ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	
	bIsRealoding = false;
	if (bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
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

EAmunitionType ARangeWeaponItem::GetAmmoType() const
{
	return WeaponBarrel->GetAmmoType();
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return WeaponBarrel->GetAmmo();
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return WeaponBarrel->GetMaxAmmo();
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	WeaponBarrel->SetAmmo(NewAmmo);
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(NewAmmo);
	}
}


bool ARangeWeaponItem::CanShoot() const
{
	return WeaponBarrel->GetAmmo() > 0;
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

void ARangeWeaponItem::SwitchWeaponMode()
{
	if (!HasAnAlternativeMode) return;
	if (WeaponBarrel == CurrentWeaponBarrel)
	{
		WeaponBarrel = AlternativeWeaponBarrel;
	}
	else
	{
		WeaponBarrel = CurrentWeaponBarrel;
	}
	SetAmmo(WeaponBarrel->GetAmmo()); 
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return !bIsAiming ? ReticleType : AimReticleType;
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	WeaponBarrel->SetAmmo(WeaponBarrel->GetMaxAmmo());
	AlternativeWeaponBarrel->SetAmmo(AlternativeWeaponBarrel->GetMaxAmmo());
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / WeaponBarrel->GetRateOfFire();
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

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
	}
}


