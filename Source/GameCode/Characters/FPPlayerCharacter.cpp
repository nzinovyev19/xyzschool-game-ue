// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Controllers/GCPlayerController.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;
	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	GCPlayerController = Cast<AGCPlayerController>(NewController);
}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && GCPlayerController.IsValid())
	{
		FRotator TargetControllRotation = GCPlayerController->GetControlRotation();
		TargetControllRotation.Pitch = 0.0f;
		float BlendSpeed = 300.0f;
		TargetControllRotation = FMath::RInterpTo(GCPlayerController->GetControlRotation(), TargetControllRotation, DeltaTime, BlendSpeed);
		GCPlayerController->SetControlRotation(TargetControllRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();
	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}
	return Result;
}

void AFPPlayerCharacter::OnCameraChangingStarted(float MinPitch, float MaxPitch, float MinYaw, float MaxYaw)
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = MinPitch;
		CameraManager->ViewPitchMax = MaxPitch;
		CameraManager->ViewYawMin = GetActorForwardVector().ToOrientationRotator().Yaw + MinYaw;
		CameraManager->ViewYawMax = GetActorForwardVector().ToOrientationRotator().Yaw + MaxYaw;
	}
}

void AFPPlayerCharacter::OnCameraChangingStopped()
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		OnCameraChangingStarted(LadderCameraMinPitch, LadderCameraMaxPitch, LadderCameraMinYaw, LadderCameraMaxYaw);
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		OnCameraChangingStopped();
	}
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		OnCameraChangingStarted(ZiplineCameraMinPitch, ZiplineCameraMaxPitch, ZiplineCameraMinYaw, ZiplineCameraMaxYaw);
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		OnCameraChangingStopped();
	}
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		if (GCPlayerController.IsValid())
		{
			GCPlayerController->SetIgnoreLookInput(true);
			GCPlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::OnHardLanded()
{
	Super::OnHardLanded();
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && FPHardLandingMontage)
	{
		if (GCPlayerController.IsValid())
		{
			GCPlayerController->SetIgnoreLookInput(true);
			GCPlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(FPHardLandingMontage, 1.0f, EMontagePlayReturnType::Duration);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreLookInput(false);
		GCPlayerController->SetIgnoreMoveInput(false);
	}
}
