
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CalcSkill.generated.h"

/* 일단 이 노드는 잠정폐기. Skill Task에서 이 역할을 처리하고있음. */
UCLASS()
class MYRPG_API UBTService_CalcSkill : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CalcSkill();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
