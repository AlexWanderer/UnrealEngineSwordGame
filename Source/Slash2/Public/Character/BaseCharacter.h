// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH2_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Combat */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual bool CanAttack();
	virtual void HandleDamage(float DamageAmount);
	bool IsAlive();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	void DisableMeshCollision();
	void CheckIfDead();
	int32 RandomNumberGenerator(int32 a, int32 b);

	/** Montage */
	void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	double CalculateHitAngle(const FVector& ImpactPoint);
	void StopMontage(UAnimMontage* Montage);

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) //UPROPERTY(BlueprintReadOnly)
	class UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	bool IsDeadBool = false;

	/**
	*Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;

	FVector ImpactHitPoint;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	virtual int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

public:
	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }
	
};
