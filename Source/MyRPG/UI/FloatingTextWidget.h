// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UFloatingTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetText(const FString& Text, const FLinearColor& Color);

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* FloatingText;

private:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Animations", Meta = (AllowPrivateAccess = true, BindWidgetAnim))
	class UWidgetAnimation* FadeOutAnimation;

	//void SetAnimationTranslationRandom();

	UPROPERTY(EditDefaultsOnly)
	FVector2D RandXRange = { -120.f, 120.f };
	UPROPERTY(EditDefaultsOnly)
	FVector2D RandYRange = { -80.f, -180.f };

	FVector2D RandTargetTranslation;

	UFUNCTION()
	void LerpRenderTranslation(float ElapsedTime);
	float AnimationTime = 1.f;

	FTimerHandle LerpTranslationTimer;
};
