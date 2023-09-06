// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Camera/CameraComponent.h"
#include "GameCode/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCode/Subsystems/StreamingSubsystem/StreamingSubsystemUtils.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	Team = ETeams::Player;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimingTimeline.TickTimeline(DeltaTime);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	AddControllerPitchInput(IsAiming() ? Value * CurrentRangeWeapon->GetAimLookUpModifier() : Value);
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	Super::LookUpAtRate(Value);
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	AddControllerPitchInput(
		IsAiming()
			? Value * CurrentRangeWeapon->GetAimLookUpModifier() * BaseTurnRate * GetWorld()->GetDeltaSeconds()
			: Value * BaseTurnRate * GetWorld()->GetDeltaSeconds()
	);
}

void APlayerCharacter::Turn(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	AddControllerYawInput(IsAiming() ? Value * CurrentRangeWeapon->GetAimTurnModifier() : Value);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	Super::TurnAtRate(Value);
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
	AddControllerYawInput(
		IsAiming()
			? Value * CurrentRangeWeapon->GetAimTurnModifier() * BaseTurnRate * GetWorld()->GetDeltaSeconds()
			: Value * BaseTurnRate * GetWorld()->GetDeltaSeconds()
	);
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return (bIsCrouched && !GetBaseCharacterMovementComponent()->IsMantling()) || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched && GetBaseCharacterMovementComponent()->IsMantling())
	{
		UnCrouch();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(this);
	
	if (IsValid(AimingTimelineCurve))
	{
		FOnTimelineFloatStatic AimingFOVTimelineUpdate;
		AimingFOVTimelineUpdate.BindUObject(this, &APlayerCharacter::AimingTimelineUpdate);
		AimingTimeline.AddInterpFloat(AimingTimelineCurve, AimingFOVTimelineUpdate);
		AimingTimeline.SetLooping(false);
	}
}

void APlayerCharacter::OnStartAiming_Implementation()
{
	Super::OnStartAiming_Implementation();
	if (IsValid(AimingTimelineCurve)) 
	{
		AimingTimeline.Play();
	}
	else 
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		SetCameraFOV(CurrentRangeWeapon->GetAimFOV());
	} 
}

void APlayerCharacter::OnStopAiming_Implementation()
{
	Super::OnStopAiming_Implementation();
	if (IsValid(AimingTimelineCurve)) 
	{
		AimingTimeline.Reverse();
	}
	else 
	{
		APlayerCameraManager* CameraManager = GetCameraManager();
		if (IsValid(CameraManager))
		{
			CameraManager->UnlockFOV();
		}
	} 
}

void APlayerCharacter::AimingTimelineUpdate(float Alpha)
{
	APlayerCameraManager* CameraManager = GetCameraManager();
	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeaponItem();
		SetCameraFOV(FMath::Lerp(CameraManager->DefaultFOV, CurrentRangeWeapon->GetAimFOV(), Alpha));
	}
}

void APlayerCharacter::SetCameraFOV(float FOV)
{
	APlayerCameraManager* CameraManager = GetCameraManager();
	if (IsValid(CameraManager))
	{
		CameraManager->SetFOV(FOV);
	}
}

APlayerCameraManager* APlayerCharacter::GetCameraManager() const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	return IsValid(PlayerController) ? PlayerController->PlayerCameraManager : nullptr;
}
