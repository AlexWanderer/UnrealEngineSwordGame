// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASH2_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void SetHealthBarPercent(float Percent);


	void IncreaseHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);

	UFUNCTION(BlueprintCallable)
	void SetGoldText(int32 Gold);

	UFUNCTION(BlueprintCallable)
	void SetSoulsText(int32 Souls);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulText;
};
