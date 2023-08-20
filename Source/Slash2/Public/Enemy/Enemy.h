// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"

class UWidgetComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASH2_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	void SpawnDeathHealth(UWorld* World, const FVector& SpawnLocation);
	void SpawnDeathSoul(UWorld* World, const FVector& SpawnLocation);
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	void PlayDeathMontage();
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	/** <AI Behiavior> */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void StartAttackTimer();
	void ClearTimer(FTimerHandle& Timer);
	bool InTargetRange(AActor* Target, double Radius);
	bool OutsideTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();
	FName PlayDeathAnimation(double Theta);
	void SoulSpawnTimerFinish();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 50.f;

	UPROPERTY()
	class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	// Patrol targets to choose from once enemy reaches Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;
	FTimerHandle SoulSpawnTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolWalkSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseWalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 25.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float SoulOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AHealth> HealthClass;

	UPROPERTY(EditAnywhere)
	bool HasTwoWeapons = false;

};
