// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Axe.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AAxe : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual bool NormalAttackPressed() override;
	virtual bool NormalAttackPressing(float DeltaTime) override;
	virtual bool NormalAttackReleased() override;

	virtual void QSkillPressed() override;
	virtual void QSkillPressing(float DeltaTime) override;
	virtual void QSkillReleased() override;

	virtual void ESkillPressed() override;
	virtual void ESkillPressing(float DeltaTime) override;
	virtual void ESkillReleased() override;

private:
	void ESkill();
	FVector ESkillSize = { 200.f,200.f,128.f };
	FVector TargetLocation;
};
