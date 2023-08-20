// Fill out your copyright notice in the Description page of Project Settings.


#include "Health.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void AHealth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealth::BeginPlay()
{
	Super::BeginPlay();

	//ConstantHealthSound = UGameplayStatics::SpawnSoundAtLocation(this, HealthSound, GetActorLocation());
}

void AHealth::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddHealth(this);
		PickupInterface->AddSouls(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
		//ConstantHealthSound->Stop();
		ConstantSoulSound->Stop();
	}
}
