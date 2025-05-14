// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Mace.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AMace : public AWeapon
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
	UPROPERTY()
	class ASplineActor* SplineActor = nullptr;

	bool bQPressed = false;
	FTimerHandle RideSplineTimerHandle;

	void SpawnQSkillActor();
	void SpawnESkillActor();

	const float QSkillMaxDistance = 1000.f;
	FVector QSkillSize = { 256.f,256.f,128.f };

};
