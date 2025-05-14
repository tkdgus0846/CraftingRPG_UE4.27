// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include "SwordESkill.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ASwordESkill : public AAttackActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;

private:
	float ElapsedTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float HitTiming = 0.4f;
};
