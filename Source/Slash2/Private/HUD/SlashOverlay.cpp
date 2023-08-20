// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::IncreaseHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGoldText(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold); // Converts Gold to FString
		const FText Text = FText::FromString(String); // Converts Gold FString to FText
		GoldText->SetText(Text);
	}
}

void USlashOverlay::SetSoulsText(int32 Souls)
{
	if (SoulText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls); // Converts Souls to FString
		const FText Text = FText::FromString(String); // Converts Souls FString to FText
		SoulText->SetText(Text);
	}
}
