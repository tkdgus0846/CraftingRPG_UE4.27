// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../SkillCaster.h"
#include "IceQuakeCaster.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API AIceQuakeCaster : public ASkillCaster
{
	GENERATED_BODY()

public:
	// ASkillCaster을(를) 통해 상속됨
	void SkillPressed() override;
	void SkillPressing(float DeltaTime) override;
	void SkillReleased() override;

protected:

private:
	FVector SkillSize = { 500.f,200.f,30.f };

	FTransform SkillTransfrom;

	virtual void SpawnSkillActor() override;
	
};
