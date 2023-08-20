// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Treasure.h"
#include "Components/CapsuleComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"


ABreakableActor::ABreakableActor()
{

	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::ChaosBreak);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::SpawnTreasure()
{
	UWorld* World = GetWorld();

	// Spawn treasure after breaking
	if (World && TreasureClasses.Num() > 0)
	{
		FVector Location = GetActorLocation();
		Location.Z += ZOffset;

		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1); // TreasureClasses - 1 because Array counting starts at 0

		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
		Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}


void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return; // Prevents loop from happening
	bBroken = true;

	GetWorldTimerManager().SetTimer(TreasureTimer, this, &ABreakableActor::SpawnTreasure, TreasureTimerLength);

}

void ABreakableActor::ChaosBreak(const FChaosBreakEvent& BreakEvent)
{
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetLifeSpan(3.f);
}
