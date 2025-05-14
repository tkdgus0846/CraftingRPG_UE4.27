
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_OnAggro.generated.h"

UCLASS()
class MYRPG_API UBTTask_OnAggro : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_OnAggro();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChangeStateToRun = true;
};
