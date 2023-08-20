// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SLASH2_API ABird : public APawn
{
	GENERATED_BODY()

public:
	
	ABird();
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// ****** Not using anymore ******
	/*void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* BirdMappingContext; //UInputMappingContext that is going to be set in blueprints detail pannel

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction; // MoveAction that will be set in blueprints details pannel

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction; // LookAction that will be set in blueprints detail pannel

	void Move(const FInputActionValue& Value); // Responsible for Moving

	void Look(const FInputActionValue& Value); // Responsible for looking

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
};
