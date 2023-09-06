// Fill out your copyright notice in the Description page of Project Settings.


#include "StreamingSubsystemVolume.h"

#include "StreamingSubsystem.h"
#include "StreamingSubsystemManager.h"
#include "StreamingSubsystemUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

AStreamingSubsystemVolume::AStreamingSubsystemVolume()
{
 	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

}

const TSet<FString>& AStreamingSubsystemVolume::GetLevelsToLoad() const
{
	return LevelsToLoad;
}

const TSet<FString>& AStreamingSubsystemVolume::GetLevelsToUnLoad() const
{
	return LevelToUnLoad;
}

void AStreamingSubsystemVolume::HandleCharacterOverlapBegin(ACharacter* Character)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolume::HandleCharacterOverlapBegin(): %s, Character: %s"), *GetNameSafe(this), *GetNameSafe(Character));
	if (!StreamingSubsystem.IsValid() || !StreamingSubsystem->CanUseSubsystem())
	{
		return;
	}

	if (!IsValid(Character) || !Character->IsPlayerControlled())
	{
		return;
	}

	if (OverlappedCharacter.IsValid())
	{
		return;
	}

	OverlappedCharacter = Character;
	StreamingSubsystem->OnVolumeOverlapBegin(this);
}

void AStreamingSubsystemVolume::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolume::BeginPlay(): %s"), *GetNameSafe(this));

	StreamingSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStreamingSubsystem>();

	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AStreamingSubsystemVolume::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AStreamingSubsystemVolume::OnOverlapEnd);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	UStreamingSubsystemUtils::CheckStreamingSubsystemVolumeOverlapCharacter(this);
}

void AStreamingSubsystemVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolume::BeginPlay(): %s"), *GetNameSafe(this));

	CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AStreamingSubsystemVolume::OnOverlapBegin);
	CollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &AStreamingSubsystemVolume::OnOverlapEnd);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (OverlappedCharacter.IsValid())
	{
		OverlappedCharacter.Reset();

		StreamingSubsystem->OnVolumeOverlapEnd(this);
	}

	StreamingSubsystem.Reset();

	Super::EndPlay(EndPlayReason);
}

void AStreamingSubsystemVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolume::OnOverlapBegin(): %s, OtherActor: %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));
	if (!IsValid(OtherActor))
	{
		return;
	}

	HandleCharacterOverlapBegin(Cast<ACharacter>(OtherActor));
}

void AStreamingSubsystemVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	UE_LOG(LogStreamingSubsystem, Display, TEXT("AStreamingSubsystemVolume::OnOverlapEnd(): %s, OtherActor: %s"), *GetNameSafe(this), *GetNameSafe(OtherActor));
	if (!StreamingSubsystem.IsValid() || !StreamingSubsystem->CanUseSubsystem())
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!IsValid(Character) || !Character->IsPlayerControlled())
	{
		return;
	}

	if (OverlappedCharacter != OtherActor)
	{
		return;
	}

	OverlappedCharacter.Reset();
	StreamingSubsystem->OnVolumeOverlapEnd(this);
}

