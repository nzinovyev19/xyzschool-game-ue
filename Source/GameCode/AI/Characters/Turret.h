#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Searching,
	Firing
};

class UWeaponBarrelComponent;
UCLASS()
class GAMECODE_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	ATurret();
	
	virtual void Tick(float DeltaTime) override;

	void SetCurrentTarget(AActor* NewTarget);

	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetViewRotation() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBarellComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponBarrelComponent* WeaponBarrel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BaseSearchingRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BaseFiringInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BarellPitchRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxBarellPitchAngle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinBarellPitchAngle = -30.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float BulletSpreadAngle = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret params | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float FireDelayTime = 3.0f;

private:
	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);
	
	void SetCurrentTurretState(ETurretState NewState);
	ETurretState CurrentTurretState = ETurretState::Searching;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	void MakeShot();

	FTimerHandle ShotTimer;
	
};
