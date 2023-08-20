// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ASoul;
class ATreasure;
class UAnimMontage;


class UInputMappingContext;
class UInputAction;

UCLASS()
class SLASH2_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void AddHealth(class AHealth* Health) override;

protected:
	virtual void BeginPlay() override;

	/**
	* Input
	*/

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	/** Callback for input */

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();

	/** Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm(); // Checks if character can Disarm
	bool CanArm(); // Checks if character can Arm
	void Disarm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	void PlayDeathMontage();
	void PlayDodgeMontage();

private:
	bool IsUnoccupied();
	void AddMappingContext();
	void InitializeSlashOverlay();
	void SetHUDHealth();
	FName PlayDeathAnimation(double Theta);

	/** Character Components */

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped; //Unequipped by default

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied; // Unoccupdied by default

	UPROPERTY(EditAnywhere)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere)
	float OverlapSphereRadius = 150.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float HealthNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 GoldNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 SoulsNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 BeginGoldNum = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 BeginSoulsNum = 0;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
