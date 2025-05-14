// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/ActionActor.h"
#include "SwordQSkill.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API ASwordQSkill : public AActionActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	virtual void OnOverlapBegin_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapEnd_Implementation(class AActor* OtherActor) override;
	virtual void OnOverlapping_Implementation(float DeltaTime) override;

private:
	void Action(class AEnemy* Enemy);

	const FVector EnemyAction(class AEnemy* Enemy);
	void PlayerAction(const FVector& EnemyPos);

	bool bOverlapped = false;

};
