// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Weapon/Weapon.h"
#include "Perception/PawnSensingComponent.h"
#include "Soul.h"
#include "Health.h"

AEnemy::AEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	// Default values for Mesh and Capsule
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore); // Preventing camera coliding with enemy mesh resulting in camera zooming in
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget(); // Chooses Patrol target for Enemy to go to next, sets timer, Enemy moves to patrol target after timer ends
	}


}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) // Called when ApplyDamage is called
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn(); // Set Combat Target
	if (InTargetRange(CombatTarget, AttackRadius))
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (OutsideTargetRange(CombatTarget, AttackRadius))
	{
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	if (EquippedWeapon2)
	{
		EquippedWeapon2->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	ImpactHitPoint = ImpactPoint;
	if (!IsDead())
	{
		ShowHealthBar(); // If enemy is dead, don't show health bar
	}

	// shouldn't be able to patrol or attack when geting hit
	ClearTimer(PatrolTimer);
	ClearTimer(AttackTimer);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision); // turn off weapon collision
	StopMontage(AttackMontage);
	
	if (InTargetRange(CombatTarget, AttackRadius))
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen); // Whenever PawnSees other pawn, calls PawnSeen
	}
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation()
{
	Super::Die_Implementation();
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearTimer(AttackTimer);
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(SoulSpawnTimer, this, &AEnemy::SoulSpawnTimerFinish, 1.f); // Set timer for TimerNum
	

}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, SoulOffset); // Spawn Location is ActorLocation with Offset for Z
		int32 RandNum = RandomNumberGenerator(1, 4);
		UE_LOG(LogTemp, Warning, TEXT("RandNum %d"), RandNum);
		if (RandNum != 4)
		{
			SpawnDeathSoul(World, SpawnLocation);
		}
		else
		{
			SpawnDeathHealth(World, SpawnLocation);
		}
		
	}
}

void AEnemy::SpawnDeathHealth(UWorld* World, const FVector& SpawnLocation)
{
	AHealth* SpawnedHealth = World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	if (SpawnedHealth)
	{
		SpawnedHealth->SetSouls(Attributes->GetSouls()); // SetSouls to however many Souls tha enemy with that attribute had
		SpawnedHealth->SetOwner(this);
	}
}

void AEnemy::SpawnDeathSoul(UWorld* World, const FVector& SpawnLocation)
{
	ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
	if (SpawnedSoul)
	{
		SpawnedSoul->SetSouls(Attributes->GetSouls()); // SetSouls to however many Souls tha enemy with that attribute had
		SpawnedSoul->SetOwner(this);
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return; // Makes sure enemy isn't dead

	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack = InTargetRange(CombatTarget, AttackRadius) && !IsAttacking() && !IsDead() && !IsEngaged();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthBarWidget) HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());

}

void AEnemy::PlayDeathMontage()
{
	double Theta = CalculateHitAngle(ImpactHitPoint); /**********************/
	FName Section = PlayDeathAnimation(Theta);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(Section, DeathMontage); // Plays random deaths
	}

}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget(); // Choose the Patrol Target to go to next
		const float TimerNum = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, TimerNum); // Set timer for TimerNum
	}
}

void AEnemy::CheckCombatTarget()
{
	if (OutsideTargetRange(CombatTarget, CombatRadius)) //Outside Combat Range
	{
		ClearTimer(AttackTimer);
		LoseInterest();

		if (!IsEngaged()) StartPatrolling(); // If not Engaged, start patrolling
	}
	else if (OutsideTargetRange(CombatTarget, AttackRadius) && !IsChasing()) //Outside Attack Range and is not chasing
	{
		ClearTimer(AttackTimer);
		if (!IsEngaged()) ChaseTarget(); // If not Engaged, ChaseTarget
	}
	else if (CanAttack()) // In Attack Radius and isn't Attacking
	{
		StartAttackTimer();
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolWalkSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	// Outside attack range, chase character
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChaseWalkSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float RandAttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, RandAttackTime);
}

void AEnemy::ClearTimer(FTimerHandle& Timer)
{
	GetWorldTimerManager().ClearTimer(Timer);
}


FName AEnemy::PlayDeathAnimation(double Theta)
{
	FName Section = FName("DeathForward");
	DeathPose = EDeathPose::EDP_Death1;

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("DeathBack");
		DeathPose = EDeathPose::EDP_Death2;
	}
	else if (Theta >= 45 && Theta < 135.f)
	{
		Section = FName("DeathRight");
		DeathPose = EDeathPose::EDP_Death3;
	}
	else if (Theta >= -135 && Theta < 45)
	{
		Section = FName("DeathLeft");
		DeathPose = EDeathPose::EDP_Death4;
	}

	return Section;
}

void AEnemy::SoulSpawnTimerFinish()
{
	SpawnSoul();
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size(); // Distance between Target and Enemy
	return DistanceToTarget <= Radius; // returns true if Radius is greater or equal to DistanceToTarget
}

bool AEnemy::OutsideTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size(); // Distance between Target and Enemy
	bool Answer = DistanceToTarget <= Radius;
	return !Answer; // returns true if DistanceToTarget is greator or equal to Radius
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius); // Enemy will stop 50 units short of patrol target

	EnemyController->MoveTo(MoveRequest); // Moves to the goal location and uses NavPath
}

AActor* AEnemy::ChoosePatrolTarget()
{
	// Makes sure we don't get the same target over again
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num(); // Gets Num ammount of ValidTarget
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1); // Random number between 0 and NumPatrol Targets - 1 (counting starts at 0)
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass); // Spawns an Actor based on AWeapon. WeaponClass is a TSubclassOf<AWeapon>
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this); // Calls Weapons Equip function
		EquippedWeapon = DefaultWeapon; // Current equipped weapon is DefaultWeapon

		if (HasTwoWeapons)
		{
			AWeapon* DefaultWeapon2 = World->SpawnActor<AWeapon>(WeaponClass);
			DefaultWeapon2->Equip(GetMesh(), FName("WeaponSocket2"), this, this);
			EquippedWeapon2 = DefaultWeapon2;
		}
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (IsDeadBool) return;
	// True if Not dead, not chasing, not attacking, not engaging, and SeenPawn is SlashCharacter
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking&&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearTimer(PatrolTimer);
		ChaseTarget();
	}
	
}


