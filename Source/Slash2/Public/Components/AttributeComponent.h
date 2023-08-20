// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH2_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	void RegenStamina(float DeltaTime);
	int32 GetSoulCount() const { return Souls; }
protected:

	virtual void BeginPlay() override;

private:
	
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	// Currest Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 33.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaUsed);
	float GetHealthPercent();
	float GetBeginHealthPercent(float HealthInput);
	float GetIncreaseHealthPercent(float IncreaseHealth);
	float GetStaminaPercent();
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	void AddSouls(int32 NumberOfSouls);

	UFUNCTION(BlueprintCallable)
	void AddGold(int32 AmmountOfGold);

	UFUNCTION(BlueprintCallable)
	void IncreaseHealth(float HealthGain);

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
