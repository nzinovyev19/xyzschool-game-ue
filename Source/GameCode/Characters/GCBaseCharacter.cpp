// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacter.h"

#include "Curves/CurveVector.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameCode/Actors/Interactive/InteractiveActor.h"
#include "GameCode/Components/LedgeDetectorComponent.h"
#include "GameCode/Actors/Interactive/Environment/Ladder.h"
#include "GameCode/Actors/Interactive/Environment/Zipline.h"
#include "GameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Components/CharacterComponents/CharacterAttributeComponents.h"
#include "GameCode/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameCode/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameFramework/PhysicsVolume.h"

AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributeComponents = CreateDefaultSubobject<UCharacterAttributeComponents>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
}

void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterAttributeComponents->OnDeathEvent.AddUObject(this, &AGCBaseCharacter::OnDeath);
	CharacterAttributeComponents->OnOutOfStaminaEventSignature.AddUObject(this, &AGCBaseCharacter::OnOutOfStamina);

}

void AGCBaseCharacter::Jump()
{
	if (!GCBaseCharacterMovementComponent->IsOutOfStamina() && !GCBaseCharacterMovementComponent->IsProning()) 
	{
		Super::Jump();
	}
	if (GCBaseCharacterMovementComponent->IsProning())
	{
		UnProne(true);
	}
}

void AGCBaseCharacter::ChangeCrouchState()
{
	if (!GetCharacterMovement()->IsCrouching() && !GCBaseCharacterMovementComponent->IsProning())
	{
		Crouch(); 
	}
}

void AGCBaseCharacter::ChangeProneState()
{
	if (GetCharacterMovement()->IsCrouching() && !GCBaseCharacterMovementComponent->IsProning())
	{
		Prone();
	}
	else if (!GetCharacterMovement()->IsCrouching() && GCBaseCharacterMovementComponent->IsProning())
	{
		UnProne(false);
	}
}

void AGCBaseCharacter::OnEndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	const float HeightDifference = GCBaseCharacterMovementComponent->bUnproneInFullHeight 
		? 0.0f 
		: GCBaseCharacterMovementComponent->CrouchedHalfHeight - GCBaseCharacterMovementComponent->PronedHalfHeight;
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightDifference;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightDifference;
	}

	K2_OnEndProne(HeightAdjust, ScaledHeightAdjust);
}

void AGCBaseCharacter::OnStartProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust + GCBaseCharacterMovementComponent->CrouchedHalfHeight;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust + GCBaseCharacterMovementComponent->CrouchedHalfHeight;
	}

	K2_OnStartProne(HeightAdjust, ScaledHeightAdjust);
}

bool AGCBaseCharacter::CanProne() const
{
	return !bIsProned && GCBaseCharacterMovementComponent && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void AGCBaseCharacter::Mantle(bool bForce)
{
	if (!(CanMantle() || bForce))
	{
		return;
	}
	
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription) && !GCBaseCharacterMovementComponent->IsMantling())
	{
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.LedgeActor = LedgeDescription.LedgeActor;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.RelativeLocationByLedgeActor = LedgeDescription.RelativeLocationByLedgeActor;

		FVector CharacterBottomLocation = GetCapsuleComponent()->GetComponentLocation() - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - GetCapsuleComponent()->GetScaledCapsuleRadius());
		float MantlingHeight = (LedgeDescription.LedgeHeightLocation - CharacterBottomLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		MantlingParameters.Duration = MaxRange - MinRange;

		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		
		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		GCBaseCharacterMovementComponent->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AGCBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AGCBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

const ALadder* AGCBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AGCBaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

const AZipline* AGCBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AGCBaseCharacter::Falling()
{
	Super::Falling();
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void AGCBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;
	if (FallHeight > MinHeightOfHardLanding * 0.01)
	{
		HardLanded();
	}
	if (IsValid(FallDamageCurve))
	{
		
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

bool AGCBaseCharacter::IsSwimmingUnderWater() const
{
	if (GCBaseCharacterMovementComponent->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetSocketLocation(SocketHeadBone);
		
		APhysicsVolume* Volume = GCBaseCharacterMovementComponent->GetPhysicsVolume();
		FVector VolumeTopPlane = Volume->GetBounds().Origin + Volume->GetBounds().BoxExtent;
		
		return HeadPosition.Z > VolumeTopPlane.Z;

	}

	return true;
}

void AGCBaseCharacter::Prone(bool bClientSimulation)
{
	if (GCBaseCharacterMovementComponent)
	{
		if (CanProne())
		{
			GCBaseCharacterMovementComponent->bWantsToProne = true;
		}
	}
}

void AGCBaseCharacter::UnProne(bool bUnproneInFullHeight, bool bClientSimulation)
{
	if (GCBaseCharacterMovementComponent)
	{
		GCBaseCharacterMovementComponent->bWantsToProne = false;
		GCBaseCharacterMovementComponent->bUnproneInFullHeight = bUnproneInFullHeight;
	}
}

void AGCBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched) 
	{
		UnCrouch();
	}
	if (bIsProned)
	{
		UnProne(true);
	}
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AGCBaseCharacter::StartFire()
{
	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void AGCBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

float AGCBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

bool AGCBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

void AGCBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAiming();
	OnStartAiming();
}

void AGCBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeaponItem();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAiming();
	}
	
	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	OnStopAiming();
}

void AGCBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AGCBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

void AGCBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void AGCBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void AGCBaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeaponItem()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void AGCBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AGCBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void AGCBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void AGCBaseCharacter::SwitchWeaponMode()
{
	CharacterEquipmentComponent->GetCurrentRangeWeaponItem()->SwitchWeaponMode();
}

void AGCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// UpdateIKSettings(DeltaTime);
	TryChangeSprintState(DeltaTime);
}

UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

const UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

const UCharacterAttributeComponents* AGCBaseCharacter::GetCharacterAttributesComponent() const
{
	return CharacterAttributeComponents;
}

bool AGCBaseCharacter::CanSprint()
{
	return !GCBaseCharacterMovementComponent->IsOutOfStamina() && !GCBaseCharacterMovementComponent->IsFalling();
}

bool AGCBaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder() || !GetBaseCharacterMovementComponent()->IsOnZipline();
}

void AGCBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
}

void AGCBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f)
	{
		EnableRagdoll();
	}
}

void AGCBaseCharacter::OnHardLanded()
{
}

void AGCBaseCharacter::OnOutOfStamina(bool IsOutOfStamina)
{
	GCBaseCharacterMovementComponent->SetIsOutOfStamina(IsOutOfStamina);
}

void AGCBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (bIsSprintRequested && !GCBaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		GCBaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}
	if (!bIsSprintRequested && GCBaseCharacterMovementComponent->IsSprinting())
	{
		GCBaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
	}
}

float AGCBaseCharacter::GetIKOffsetForASocket(const FName& SocketName) const
{
	float Result = 0.0f;

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	const FVector FootSizeBox = FVector(1.f, 15.f, 7.f);
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, FootSizeBox, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = TraceStart.Z - CapsuleHalfHeight - HitResult.Location.Z;
	}

	return Result;
}

void AGCBaseCharacter::UpdateIKSettings(float DeltaTime)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaTime, IKInterpSpeed);
}

float AGCBaseCharacter::CalculateIKPelvisOffset()
{
	return  IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

void AGCBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

void AGCBaseCharacter::HardLanded()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnHardLandingAnimMontage);

	GetWorld()->GetTimerManager().SetTimer(OnHardLandedMontageTimer, this, &AGCBaseCharacter::EnableMovement, Duration, false);
	OnHardLanded();
}

void AGCBaseCharacter::EnableMovement()
{
	GetCharacterMovement()->SetDefaultMovementMode();
}
