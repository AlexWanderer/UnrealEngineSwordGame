// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController(); // First player controller
		if (Controller && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass); // Set SlashOverlay to SlashOverlayClass Widget
			SlashOverlay->AddToViewport(); // Add SlashOverlay to the viewport
		}
	}
}