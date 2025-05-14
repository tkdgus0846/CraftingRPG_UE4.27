// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyRPG/AttackActor/ActionActor.h"
#include "AxeESkill.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AAxeESkill : public AActionActor
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
	bool bLeftRotateAxe = true;

public:
	/* 1 = LeftRotate, 0 = RightRotate */
	void SetLeftRotateAxe(bool bLeftRotate);
};
