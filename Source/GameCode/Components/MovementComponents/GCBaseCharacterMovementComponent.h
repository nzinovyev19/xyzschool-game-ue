// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCBaseCharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FMantlingMovementParameters
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPrimitiveComponent* LedgeActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCurveVector* MantlingCurve;

	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector RelativeLocationByLedgeActor = FVector::ZeroVector;
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_Max UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff,
};

class AGCBaseCharacter;

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void PhysicsRotation(float DeltaTime) override;

	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	virtual float GetMaxSpeed() const override;

	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	void StartSprint();
	void StopSprint();

	virtual bool IsProning() const;
	virtual void Prone(bool bClientSimulation = false);
	virtual void UnProne(bool bClientSimulation = false);
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual bool CanProneInCurrentState() const;
	bool bWantsToProne = false;
	bool bUnproneInFullHeight = false;

	UPROPERTY(Category = "Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	bool bProneMaintainsBaseLocation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PronedHalfHeight = 34.0f;

	void StartMantle(const FMantlingMovementParameters & MantlingParameters); 
	void EndMantle();
	bool IsMantling() const;

	void AttachToLadder(const class ALadder* Ladder);
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	float GetLadderSpeedRatio() const;
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder();

	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();
	float GetActorToCurrentZiplineProjection(const FVector& Location) const;
	bool IsOnZipline() const;

protected:
	UPROPERTY(Transient, DuplicateTransient)
	AGCBaseCharacter* BaseCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: isOutOfStaminaSpeed", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IsOutOfStaminaSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ProneSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingOnLadderBrakingDeceleration = 2048.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxLadderTopOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Zipline", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSpeedOnZipline = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ZiplineToCharacterOffset = 60.0f;

	class AGCBaseCharacter* GetBaseCharacterOwner() const;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);

	void PhysLadder(float DeltaTime, int32 Iterations);

	void PhysZipline(float DeltaTime, int32 Iterations);

private:
	bool bIsSprinting;
	bool bIsOutOfStamina;

	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Character movement: Ladder")
	const ALadder* CurrentLadder = nullptr;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Character movement: Zipline")
	const AZipline* CurrentZipline = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

};
