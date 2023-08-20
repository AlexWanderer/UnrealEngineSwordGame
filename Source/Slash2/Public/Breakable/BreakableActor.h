// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class SLASH2_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void SpawnTreasure();

	UFUNCTION()
	virtual void ChaosBreak(const FChaosBreakEvent& BreakEvent);

	UPROPERTY(EditAnywhere)
	float ZOffset = 75.f;

protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;
private:	
	
	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	FTimerHandle TreasureTimer;

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	float TreasureTimerLength = 1.f;

	bool bBroken = false;
};
