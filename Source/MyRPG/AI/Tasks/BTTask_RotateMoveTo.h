
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_RotateMoveTo.generated.h"

UCLASS()
class MYRPG_API UBTTask_RotateMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UBTTask_RotateMoveTo();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
