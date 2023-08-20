// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


ABird::ABird()
{

	PrimaryActorTick.bCanEverTick = true;

	// Capsule
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	// Mesh
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());

	// Spring
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	// Camera
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) // Gets the EnhancedInputLocalPlayerSubsystem
		{
			Subsystem->AddMappingContext(BirdMappingContext, 0); // Sets BirdMappingContext as the mapping context
		}
	}
}

void ABird::Move(const FInputActionValue& Value) // Gets called when MoveAction is Triggered
{
	const float DirectionValue = Value.Get<float>(); // Gets Float Value from FInputActionValue& Value

	if ((Controller) && (DirectionValue != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue); //Adds movement input using the Direction Value and get forward vector 
	}
}

void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>(); // Gets FVector2D from FInputActionValue& Value
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X); // Gets the X Part of LookAxisValue and uses it to move left or right (yaw)
		AddControllerPitchInput(LookAxisValue.Y); // Gets the Y Part of LookAxisValue and uses it to up or down (pitch)
	}
}


void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) // Casting to EnhancedInputComponent in Player Input Component
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move); // Binds Action to the ABird::Move (calls it when triggered)
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look); // Binds Action to ABird::Look (calls it when triggered)
	}
	
}

