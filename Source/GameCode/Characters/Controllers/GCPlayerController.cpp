// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameCode/Characters/GCBaseCharacter.h"
#include "GameCode/UI/Widgets/PlayerHUDWidget.h"
#include "GameCode/GameCodeTypes.h"
#include "GameCode/Subsystems/SaveSubsystem/SaveSubsystem.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

void AGCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AGCBaseCharacter>(InPawn);
	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndInitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AGCPlayerController::OnInteractableObjectFound);
	}
}

bool AGCPlayerController::GetIgnoreCameraPitch()
{
	return bIgnoreCameraPitch;
}

void AGCPlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCPlayerController::LookUpAtRate);
	InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
	InputComponent->BindAxis("TurnAtRate", this, &AGCPlayerController::TurnAtRate);
	InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Released, this, &AGCPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Released, this, &AGCPlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AGCPlayerController::ChangeProneState);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AGCPlayerController::StopSprint);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AGCPlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AGCPlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AGCPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AGCPlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AGCPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("SwitchFireMode", EInputEvent::IE_Pressed, this, &AGCPlayerController::SwitchWeaponMode);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &AGCPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &AGCPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &AGCPlayerController::Interact);
	InputComponent->BindAction("UseInventory", EInputEvent::IE_Pressed, this, &AGCPlayerController::UseInventory);
	InputComponent->BindAction("ConfirmWeaponWheelSelection", EInputEvent::IE_Pressed, this, &AGCPlayerController::ConfirmWeaponWheelSelection);
	InputComponent->BindAction("QuickSaveGame", EInputEvent::IE_Pressed, this, &AGCPlayerController::QuickSaveGame);
	InputComponent->BindAction("QuickLoadGame", EInputEvent::IE_Pressed, this, &AGCPlayerController::QuickLoadGame);
	
	FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &AGCPlayerController::ToggleMainMenu);
	ToggleMenuBinding.bExecuteWhenPaused = true;
}

void AGCPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AGCPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AGCPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AGCPlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void AGCPlayerController::LookUpAtRate(float Value)
{

	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AGCPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AGCPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AGCPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void AGCPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AGCPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AGCPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AGCPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AGCPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AGCPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AGCPlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AGCPlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AGCPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AGCPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AGCPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AGCPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AGCPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AGCPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AGCPlayerController::SwitchWeaponMode()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwitchWeaponMode();
	}
}

void AGCPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AGCPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AGCPlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget))
	{
		return;
	}
	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly { });
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent();
		SetInputMode(FInputModeGameAndUI { });
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void AGCPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void AGCPlayerController::UseInventory()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory(this);
	}
}

void AGCPlayerController::ConfirmWeaponWheelSelection()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConfirmWeaponWheelSelection();
	}
}

void AGCPlayerController::QuickSaveGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->SaveGame();
}

void AGCPlayerController::QuickLoadGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->LoadLastGame();
}

void AGCPlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);	
}

void AGCPlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}	
	}

	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	if (CachedBaseCharacter.IsValid() && IsValid(PlayerHUDWidget))
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if (IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipmentComponent->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipmentComponent->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			CharacterEquipmentComponent->OnCurrentThrowableCountChangedEvent.AddUFunction(AmmoWidget, FName("UpdateGrenadeCount"));
		}
	}

	SetInputMode(FInputModeGameOnly { });
	bShowMouseCursor = false;
}
