// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MovingGate.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AMovingGate::AMovingGate()
{
	PrimaryActorTick.bCanEverTick = true;

	Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gate"));
	RootComponent = Gate;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	OverlapBox->SetupAttachment(GetRootComponent());

}

void AMovingGate::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalLocation = GetActorLocation();
	OverlapBox->OnComponentBeginOverlap.AddDynamic(this, &AMovingGate::OnBoxOverlap);
}

void AMovingGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector TargetLocation = OriginalLocation;

	if (ShouldMove)
	{
		TargetLocation = OriginalLocation + MoveOffset;
	}

	FVector CurrentLocation = GetActorLocation();
	float Speed = MoveOffset.Length() / MoveTime;

	// Interpolates Gate to TargetLocation by using speed and sets that to new location every frame
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);
	SetActorLocation(NewLocation); 

}

void AMovingGate::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
