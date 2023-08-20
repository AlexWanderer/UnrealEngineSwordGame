// Fill out your copyright notice in the Description page of Project Settings.


#include "Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Checks if Soul location is greator then desired location
	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		// Move Soul Down every frame by a certain ammount
		const FVector DeltaLocation = FVector(0.f, 0.f, DescendRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
	else if (LocationZ <= DesiredZ)
	{
		ItemState = EItemState::EIS_Hovering;
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	ItemState = EItemState::EIS_MAX;

	const FVector Start = GetActorLocation(); // Start line trace at soul location
	const FVector End = Start - FVector(0.f, 0.f, 2000.f); // End line trace at 2000 units below soul location
	FHitResult HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes; // Object Types to be hit by the line trace
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore; // Actors to ignore from Line Trace
	ActorsToIgnore.Add(GetOwner());

	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true);
	DesiredZ = HitResult.ImpactPoint.Z + 100.f;

	ConstantSoulSound = UGameplayStatics::SpawnSoundAtLocation(this, SoulSound, GetActorLocation());
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddSouls(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
		ConstantSoulSound->Stop();
	}
}
