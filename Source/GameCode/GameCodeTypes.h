#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_Bullet ECC_GameTraceChannel3

const FName FXParamTraceEnd = FName("TraceEnd");


const FName SocketHeadBone = FName("head");
const FName SocketFPCamera = FName("CameraSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");

const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");

UENUM(BlueprintType)
enum class EEquippableItemType: uint8
{
	None,
	Pistol,
	Rifle
};

UENUM(BlueprintType)
enum class EAmunitionType: uint8
{
	None,
	Pistol,
	Rifle,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	MAX UMETA(Hidden)
};
