// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Soul.h"
#include "Health.generated.h"

/**
 * 
 */
UCLASS()
class SLASH2_API AHealth : public ASoul
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float HealthGain = 1.f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	USoundBase* HealthSound;

	/*UPROPERTY()
	UAudioComponent* ConstantHealthSound;*/

	double DesiredZ;
public:
	FORCEINLINE float GetHealthGain() const { return HealthGain; }
};
