// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PostProcessAnimator.generated.h"

class UMaterialParameterCollectionInstance;

UCLASS()
class MYRPG_API APostProcessAnimator : public AActor
{
	GENERATED_BODY()
	
public:	
	APostProcessAnimator();

	void Initialize(float Duration);

	/* Percent Input: 0~1 => 0%~100% */
	UFUNCTION(BlueprintCallable)
	void SetPlayRateByPercent(class UTimelineComponent* Timeline, float Percent);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void Play();
	UFUNCTION()
	virtual void Play_Implementation();

private:
	UPROPERTY()
	float AnimationDuration;

};
