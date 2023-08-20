// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SlashAnimInstance.h"
#include "Character/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner()); // Gets the PawnOwner of ASlashCharacter
	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement(); // Gets CharacterMovement component
	}

}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{	
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity); // Gets XY character velocity
		IsFalling = SlashCharacterMovement->IsFalling(); // Checks if SlashCharacter is falling
		CharacterState = SlashCharacter->GetCharacterState(); // Get the character state
		ActionState = SlashCharacter->GetActionState(); // Set ActionState
		DeathPose = SlashCharacter->GetDeathPose();
	}
}
