// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Characters/PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class UCameraComponent* FirstPersonCameraComponent;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMinPitch = -50.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMaxPitch =  50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMinYaw = -30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMaxYaw = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMinPitch = -90.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxPitch = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float ZiplineCameraMinYaw = -30.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float ZiplineCameraMaxYaw = 30.0f;

	virtual void OnHardLanded() override;
	
private:
	FTimerHandle FPMontageTimer;

	void OnFPMontageTimerElapsed();
	
	bool IsFPMontagePlaying() const;

	void OnCameraChangingStarted(float MinPitch, float MaxPitch, float MinYaw, float MaxYaw);
	void OnCameraChangingStopped();

	TWeakObjectPtr<class AGCPlayerController> GCPlayerController;
};
