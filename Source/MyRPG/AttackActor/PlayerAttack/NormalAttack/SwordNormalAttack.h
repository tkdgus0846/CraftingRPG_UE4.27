// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/AttackActor.h"
#include "SwordNormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ASwordNormalAttack : public AAttackActor
{
	GENERATED_BODY()
	
protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;
};
