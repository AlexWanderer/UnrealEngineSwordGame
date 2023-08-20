// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GroomComponent.h"
#include "Item.h"
#include "Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/SphereComponent.h"
#include "Enemy/Enemy.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/AttributeComponent.h"
#include "Soul.h"
#include "Treasure.h"
#include "Health.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); //Weapon uses WorldDynamic channel, so set WorldDynamic to Overlap
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); // Box trace uses visibility channel, so set visibility to block
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Creates Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	// Creates Camera and sets it up to CameraBoom
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	// Setup Hair
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	// Setup Eyebrows
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	/*OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Ignore);
	OverlapSphere->SetSphereRadius(OverlapSphereRadius);*/

}

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
	
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	AddMappingContext();

	/*OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnSphereBeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnSphereEndOverlap);*/

	Tags.Add(FName("EngageableTarget"));

	InitializeSlashOverlay();
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) // Casting to EnhancedInputComponent in Player Input Component
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}

}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth(); // Update HUD Health after taking damage
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision); // Set weapon collision to NoCollision so character can't hit enemies if attack animation gets interrupted and WeaponCollision is set to true
	if (Attributes && Attributes->GetHealthPercent() > 0.f) // makes sure character isn't dead
	{
		ActionState = EActionState::EAS_HitReaction;
	}

}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSoulsText(Attributes->GetSouls());
		SoulsNum = Attributes->GetSouls();
		UE_LOG(LogTemp, Warning, TEXT("SoulsNum: %d"), SoulsNum);
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGoldText(Attributes->GetGold());
		GoldNum = Attributes->GetGold();
	}
}

void ASlashCharacter::AddHealth(AHealth* Health)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->IncreaseHealth(Health->GetHealthGain());
		SlashOverlay->SetHealthBarPercent(Attributes->GetIncreaseHealthPercent(Health->GetHealthGain()));
		HealthNum = Attributes->GetMaxHealth();
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return; // Makes sure character can't move if attacking

	const FVector2D MovementVector = Value.Get<FVector2D>(); // Gets FVector2D from FInputActionValue& Value

	const FRotator Rotation = Controller->GetControlRotation(); // Creates rotator that = controller rotation
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); // Zeros out the Pitch and the Roll on YawRotation


	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // Gets Forward Vector corresponding to YawRotation
	AddMovementInput(ForwardDirection, MovementVector.Y); // Moves the Character in the Forward Direction when MovementVector.Y has input (pressing W or S)

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Gets Rigth Vector corresponding to YawRotation
	AddMovementInput(RightDirection, MovementVector.X); // Moves the Characetr in the Right Direction when MovementVector.X has input (pressing A or D)
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y); // Moves in Pitch Direction 
	AddControllerYawInput(LookAxisVector.X); // Moves in Yaw Direction
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem); //Casts OverlappingItem to Weapon
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking; // When attacking, Action state = EAS_Attacking
	}

}

void ASlashCharacter::Dodge()
{
	if (!IsUnoccupied() || !HasEnoughStamina()) return; // If not in Unoccupied or doesn't have stamina, return early
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost()); // Use how much stamina it costs to dodge
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this); // Calls Equip from Overlapping Item
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;

	OverlappingItem = nullptr; // After equipping the weapon, set the OverlappingItem to null so we can reach the next else statement if we press EKey again
	EquippedWeapon = Weapon; // EquippedWeapon stores the variable overlapped
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied; // Set the ActionState back to Unoccupied at the end of the attack
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped; // Only true if Action state = Unoccupied and CharacterState = Unequipped
}

bool ASlashCharacter::CanDisarm()
{
	// Checks if Character is Unoccupied and also has a weapon so that the character can unequip the weapon
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	// Checks if Character is Unoccupied and is unequipped so the character can equip a weapon (also checks if EquippedWeapon is not null)
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon; // Equipping Weapon
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon; // Equipping Weapon
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage); // Will play Equip or Unequip animation
	}
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	PlayDeathMontage();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost(); // Checks if SlashCharacter has enough stamina to dodge
}

void ASlashCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("BeginOverlap"));
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *OtherActor->GetName());
		CombatTarget = OtherActor;
	}
}

void ASlashCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("EndOverlap"));
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *OtherActor->GetName());
		CombatTarget = nullptr;
	}
}

void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayDeathMontage()
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

void ASlashCharacter::PlayDodgeMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("DodgeMontage"));
	PlayMontageSection(DodgeMontage, FName("Dodge"));
}

void ASlashCharacter::AddMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) // Gets the EnhancedInputLocalPlayerSubsystem
		{
			Subsystem->AddMappingContext(SlashContext, 0); // Sets SlashContext as the mapping context
		}
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController()); // PlayerController = APlayerController
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()); // SlashHUD = ASlashHUD
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay(); // Get the SlashOverlay and store it in function
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGoldText(BeginGoldNum);
				SlashOverlay->SetSoulsText(BeginSoulsNum);
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

FName ASlashCharacter::PlayDeathAnimation(double Theta)
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
