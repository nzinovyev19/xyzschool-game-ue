// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameCode/Components/CharacterComponents/CharacterAttributeComponents.h"
#include "GameCode/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GCBaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool);

class AInteractiveActor;
class UCharacterEquipmentComponent;
class UGCBaseCharacterMovementComponent;

UCLASS(abstract, NotBlueprintable)
class GAMECODE_API AGCBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AGCBaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void LookUpAtRate(float Value) {};
	virtual void Jump();

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void ChangeCrouchState();
	virtual void StartSprint();
	virtual void StopSprint();

	virtual void StartFire();
	virtual void StopFire();

	float GetAimingMovementSpeed() const;

	bool IsAiming() const;

	FOnAimingStateChanged OnAimingStateChanged;

	void StartAiming();
	void StopAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	void Reload();

	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();

	void SwitchWeaponMode();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UGCBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return GCBaseCharacterMovementComponent; }

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;
	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;

	const UCharacterAttributeComponents* GetCharacterAttributesComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKPelvisOffset() const { return IKPelvisOffset; }

	virtual void ChangeProneState();
	virtual void OnEndProne(float HeightAdjust, float ScaledHeightAdjust);
	virtual void OnStartProne(float HeightAdjust, float ScaledHeightAdjust);
	 
	UPROPERTY(BlueprintReadOnly, Category = "Character | Prone")
	bool bIsProned = false;

	UFUNCTION(BlueprintCallable, Category = "Character | Prone", meta = (HidePin = "bClientSimulation"))
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character | Prone", meta = (HidePin = "bClientSimulation"))
	virtual void UnProne(bool bUnproneInFullHeight, bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character | Prone")
	virtual bool CanProne() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnEndProne", ScriptName = "OnEndProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnStartProne", ScriptName = "OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);
	virtual bool CanJumpInternal_Implementation() const override;

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void ClimbLadderUp(float Value); 
	void InteractWithLadder();
	const class ALadder* GetAvailableLadder() const;

	void InteractWithZipline();
	const class AZipline* GetAvailableZipline() const;

	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;
	
	bool IsSwimmingUnderWater() const;

	/* IGenericTeamAgentInterface */
	virtual FGenericTeamId GetGenericTeamId() const override;
	/* ~IGenericTeamAgentInterface */

	UFUNCTION(Server, Reliable)
	void Server_ActivatePlatformTrigger(class APlatformTrigger* PlatformTrigger, bool bIsActivated);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
	float BaseLookUpRate = 45.0f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnSprintStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnSprintEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config | IK settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKInterpSpeed = 30.f;

	virtual bool CanSprint();

	bool CanMantle() const;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	UGCBaseCharacterMovementComponent* GCBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HighMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.f, UIMin = 0.f))
	float LowMantleMaxHeight = 125.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterAttributeComponents* CharacterAttributeComponents;

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;

	// Damage depending from fall height in meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	float MinHeightOfHardLanding = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnHardLandingAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* FPHardLandingMontage;

	virtual void OnHardLanded();

	virtual void OnOutOfStamina(bool IsOutOfStamina);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;
	
	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
	ETeams Team = ETeams::Enemy;
	
private:
	void TryChangeSprintState(float DeltaTime);
	bool bIsSprintRequested = false;

	void UpdateIKSettings(float DeltaTime);
	float GetIKOffsetForASocket(const FName& SocketName) const;
	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKPelvisOffset = 0.0f;
	float CalculateIKPelvisOffset();
	
	bool bIsOutOfStamina = false;
	
	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	TArray<AInteractiveActor*> AvailableInteractiveActors;

	void EnableRagdoll();
	
	FVector CurrentFallApex;
	
	void HardLanded();
	void EnableMovement();
	FTimerHandle OnHardLandedMontageTimer;

	bool bIsAiming;
	float CurrentAimingMovementSpeed;
};
