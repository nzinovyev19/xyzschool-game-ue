// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"

#include "GameCode/Characters/FPPlayerCharacter.h"
#include "GameCode/Characters/Controllers/GCPlayerController.h"
#include "GameCode/Components/MovementComponents/GCBaseCharacterMovementComponent.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay only FPPlayerAnimInstance can use UFPPlayerAnimInstance"));
	CachedFirstPersonCharacterOwner = Cast<AFPPlayerCharacter>(TryGetPawnOwner());
}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedFirstPersonCharacterOwner.IsValid())
	{
		return;
	}
	PlayerCameraPitchAngle = CalculateCameraPitchAngle();
}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;
	AGCPlayerController* Controller = CachedFirstPersonCharacterOwner->GetController<AGCPlayerController>();
	if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		Result = Controller->GetControlRotation().Pitch;
	}
	return Result;
}
