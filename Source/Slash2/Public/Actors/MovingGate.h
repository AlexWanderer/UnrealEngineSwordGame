// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingGate.generated.h"

UCLASS()
class SLASH2_API AMovingGate : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingGate();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:



	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Gate;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	class UBoxComponent* OverlapBox;

	UPROPERTY(EditAnywhere)
	FVector MoveOffset;

	UPROPERTY(EditAnywhere)
	float MoveTime = 4;

	UPROPERTY(EditAnywhere)
	bool ShouldMove = false;

	// Starting location
	FVector OriginalLocation;

	

};
