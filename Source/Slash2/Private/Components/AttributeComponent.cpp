// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth); // Makes sure we never set Health to a value less then 0 or greater then MaxHealth
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);
}

void UAttributeComponent::UseStamina(float StaminaUsed)
{
	Stamina = FMath::Clamp(Stamina - StaminaUsed, 0.f, MaxStamina); // Makes sure we never set Stamina to a value less then 0 or greater then MaxStamina
}

float UAttributeComponent::GetHealthPercent()
{
	return Health/MaxHealth; // Gets percentage of health
}

float UAttributeComponent::GetBeginHealthPercent(float HealthInput)
{
	return HealthInput/MaxHealth;
}

float UAttributeComponent::GetIncreaseHealthPercent(float IncreaseHealth)
{
	Health += IncreaseHealth;
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina; // Gets percentage of Stamina
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f; // If health is greater then 0, return true. Otherwise return false
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
}

void UAttributeComponent::AddGold(int32 AmmountOfGold)
{
	Gold += AmmountOfGold;
}

void UAttributeComponent::IncreaseHealth(float HealthGain)
{
	Health = FMath::Clamp(Health + HealthGain, 0.f, MaxHealth); // Makes sure we never set health to a value lower than 0.f and higher the MaxHealth
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina); // Adds a certain ammount to Stamina every frame but clamps it between 0.f and MaxStamina
}

