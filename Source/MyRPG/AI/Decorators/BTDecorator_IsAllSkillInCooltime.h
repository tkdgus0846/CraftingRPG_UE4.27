
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsAllSkillInCooltime.generated.h"

/**
 * 
 */
UCLASS()
class MYRPG_API UBTDecorator_IsAllSkillInCooltime : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsAllSkillInCooltime();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
