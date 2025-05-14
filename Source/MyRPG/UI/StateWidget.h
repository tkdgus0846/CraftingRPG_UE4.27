// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRPG/Types/StateMaterialType.h"
#include "StateWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Meta = (BindWidget))
	class UImage* BackgroundImage;
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* StateName;
	UPROPERTY(Meta = (BindWidget))
	class UProgressBar* DurationRemainBar;

	UFUNCTION()
	void ChangeState(EStateMaterialType NewState, float Duration);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void ChangePercent(float DeltaSeconds);
	void StateSetting(FText StateText, FLinearColor Color);

	FTimerHandle StateDurationTimer;

	float CurDuration;
	float OriginDuration;
	bool bChangePercent = false;
	
};
