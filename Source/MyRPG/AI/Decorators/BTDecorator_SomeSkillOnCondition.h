// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SomeSkillOnCondition.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UBTDecorator_SomeSkillOnCondition : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_SomeSkillOnCondition();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
